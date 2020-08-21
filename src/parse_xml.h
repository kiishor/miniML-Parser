#ifndef PARSE_XML_H
#define PARSE_XML_H

/**
 * \file
 * \brief   Header file for parse_xml.c.

 * \author  Nandkishor Biradar
 * \date    19 August 2019
 */

/*
 *  ------------------------------- ENUMERATION -------------------------------
 */

//! List of result code of \ref parse_xml
typedef enum
{
  XML_PARSE_SUCCESS,                //!< XML is successfully parsed
  XML_INCOMPLETE_SOURCE,            //!< incomplete XML source.
  XML_SYNTAX_ERROR,                 //!< XML is not well formed.
  XML_INVALID_START_TOKEN_ERR,      //!< XML is not well formed. '<' start token not found
  XML_ELEMENT_NOT_FOUND_ERR,        //!< XML element not found.
  XML_ELEMENT_MAX_OCCURRENCE_ERR,   //!< XML element occurred more that specified count.
  XML_ELEMENT_MIN_OCCURRENCE_ERR,   //!< XML element occurred lest than specified count.
  XML_CHOICE_ELEMENT_ERR,           //!< More that one child elements in choice order type
  XML_DUPLICATE_ATTRIBUTE,          //!< Duplicate attribute found.
  XML_ATTRIBUTE_NOT_FOUND,          //!< Attribute not found or not all the required attribute found.
  XML_CONTENT_ERROR,                //!< Element does not contains content as specified in the schema
  XML_END_TAG_NOT_FOUND,            //!< End tag doesn't match with start tag of an element

  XML_CONTENT_UNSUPPORTED,    //!< Parser doesn't support this XML content type.
  XML_MIN_LENGTH_ERROR,       //!< Length of content is less than minLength of restriction facet.
  XML_MAX_LENGTH_ERROR,       //!< Length of content is greater than maxLength of restriction facet.
  XML_MIN_VALUE_ERROR,        //!< Value of content is less than minValue of restriction facet.
  XML_MAX_VALUE_ERROR,        //!< Value of content is greater than maxValue of restriction facet.
  XML_ENUM_NOT_FOUND,         //!< Content doesn't match with any of specified enumerations
  XML_DURATION_SYNTAX_ERROR,  //!< XML syntax error in duration content.

  FAILED_TO_ALLOCATE_MEMORY   //!< Failed to allocate the memory of string dynamic type.
}xml_parse_result_t;

//! List of target type to store the XML content.
typedef enum
{
  EN_STATIC,      //!< Target address is static
  EN_DYNAMIC,     //!< call Allocate() callback to get target address.
  EN_RELATIVE,    //!< Target address is relative to its immediate parent target address
  TOTAL_TARGET_ADDRESS_TYPE
}address_type_t;

//! List of child element order type
typedef enum
{
  //! Child elements can occur at any order.
  //! The number of occurrence can be specified in MinOccur and MaxOccur.
  EN_RANDOM,

  //! Only one child element can occur out of all the specified child elements
  //! The number of occurrence of the child element can be specified in MinOccur and MaxOccur
  EN_CHOICE,

  //! Child element must occur in sequence.
  //! The number of occurrence can be specified in MinOccur and MaxOccur
  EN_SEQUENCE,
}child_order_type_t;

//! List of enumeration for attribute **use**
typedef enum
{
  EN_OPTIONAL,      //!< Attribute can be optional in the XML.
  EN_REQUIRED,      //!< Attribute is required and must present in the XML.
  EN_PROHIBITED,    //!< Attribute is prohibited and must not present in the XML.
  TOTAL_XSD_ATTRIBUTE_USE_VALUES
}xs_attribute_use_t;

/*
 *  ------------------------------ INCLUDE FILES ------------------------------
 */

#include "xml_content.h"

/*
 *  -------------------------------- STRUCTURE --------------------------------
 */
//! A simple structure for string data type.
typedef struct
{
  char* String;   //!< Pointer to string.
  size_t Length;  //!< Length of string.
}string_t;

/** A function pointer to trigger callback on successful parsing of element.
 *
 * \param occurrence uint32_t : The current occurrence count of the element in the XML
 * \param content void* const : Address containing content of element
 * \param context void**      : Context passed to xml parser.
 */
typedef void (*element_callback)(uint32_t occurrence, void* const content, void** context);


/** A function pointer to allocate dynamic memory to store content of element.
 *
 * \param occurrence uint32_t : The current occurrence count of the element in the XML
 * \param context void**      : Context passed to xml parser.
 * \return void*              : Target address to store xml content
 */
typedef void* (*allocate)(uint32_t occurrence, void** context);

//! Structure to holds/allocate the target address to store XML content
typedef struct
{
  address_type_t Type;    //!< Target address type can be static, dynamic or relative

  //! Use of fields of below union depends on selected address type
  union  {

    //! Address of target to store XML content. Applicable if address type is static.
    void* Address;

    //! callback function to allocate memory to store XML content.
    //! Applicable if address type is dynamic
    allocate Allocate;

    //! Offset from parent target address.
    //! e.g. target address = parent target address + offset.
    //! Applicable if address type is relative
    size_t Offset;
  };

  //! Size of Target in case of multiple occurrences to calculate the next target address.
  uint32_t Size;
}target_address_t;

//! structure to define the attribute to an element
typedef struct
{
  string_t Name;            //!< Name of attribute
  target_address_t Target;  //!< Target address to store the content of attribute
  xml_content_t Content;    //!< Content type of attribtue
  xs_attribute_use_t Use;   //!< Use of attribute required, optional or prohibited.
}xs_attribute_t;

typedef struct xs_element_t xs_element_t;

//! Structure to define element of XML
struct xs_element_t
{
  string_t Name;        //!< Name of an element
  uint32_t MinOccur;    //!< Minimum number of time element must occur
  uint32_t MaxOccur;    //!< Maximum number of time element is allowed to occur
  element_callback Callback;  //!< Callback after successful parsing of an element.

  target_address_t Target;  //!< Target address to store content of an element
  xml_content_t Content;    //!< Content type of an element

  uint32_t Attribute_Quantity;      //!< Number of attributes in the element
  const xs_attribute_t* Attribute;  //!< Address to array of attribtues

  uint32_t Child_Quantity;          //!< Number of child elements of an element
  child_order_type_t Child_Order;   //!< order type of child elements
  const xs_element_t* Child;        //!< Address to array of child elements
};

/*
 *  ---------------------------- EXPORTED FUNCTION ----------------------------
 */

/** \brief XML parser to parse XML file.
 *
 * \param root const xs_element_t*: pointer to root element of XML element tree.
 * \param source const char*      : buffer containing XML source to parse.
 * \param context void**          : User defined context.
 *        Parser doesn't use/modify this argument for parsing purpose.
 *        It passes this context in all the callback functions.
 *        If not required pass NULL.
 * \return extern xml_parse_result_t result of XML parsing.
 *
 */
extern xml_parse_result_t parse_xml(const xs_element_t* root, const char* source,
                                     void** context);

#endif // PARSE_XML_H

