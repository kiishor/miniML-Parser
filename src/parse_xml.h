#ifndef PARSE_XML_H
#define PARSE_XML_H

/**
 * \file
 * \brief XML parser in C

 * \author  Nandkishor Biradar
 * \date    19 August 2019

 *  Copyright (c) 2019-2022 Nandkishor Biradar
 *  https://github.com/kiishor

 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 */

/*
 *  ------------------------------- DEFINITION -------------------------------
 */

 //! By default context is disabled
#ifndef XML_PARSER_CONTEXT
  #define XML_PARSER_CONTEXT 0
#endif // XML_PARSER_CONTEXT

//! By default parser callbacks are disabled.
#ifndef XML_PARSER_CALLBACK
  #define XML_PARSER_CALLBACK 0
#endif // XML_PARSER_CALLBACK

//! By default printf messages on error are disabled
#ifndef XML_PARSER_DEBUG
  #define XML_PARSER_DEBUG 0
#endif // XML_PARSER_DEBUG

//! List of result code of \ref parse_xml
#define XML_PARSER_RESULT \
  ADD_RESULT_CODE(XML_PARSE_SUCCESS,                Successfully parsed XML.)  \
  ADD_RESULT_CODE(XML_INCOMPLETE_SOURCE,            Incomplete XML source.)    \
  ADD_RESULT_CODE(XML_SYNTAX_ERROR,                 XML is not well formed.)  \
  ADD_RESULT_CODE(XML_INVALID_START_TOKEN_ERR,      XML is not well formed. '<' start token not found.)  \
  ADD_RESULT_CODE(XML_ELEMENT_NOT_FOUND_ERR,        XML element not found.)  \
  ADD_RESULT_CODE(XML_ELEMENT_MAX_OCCURRENCE_ERR,   XML element occurred more than specified count in the schema.)  \
  ADD_RESULT_CODE(XML_ELEMENT_MIN_OCCURRENCE_ERR,   XML element occurred less than specified count in the schema.)  \
  ADD_RESULT_CODE(XML_CHOICE_ELEMENT_ERR,           More that one child elements in choice order type.)  \
  ADD_RESULT_CODE(XML_DUPLICATE_ATTRIBUTE,          Duplicate attribute found.)  \
  ADD_RESULT_CODE(XML_ATTRIBUTE_NOT_FOUND,          Attribute not found or not all the required attribute found.)  \
  ADD_RESULT_CODE(XML_CONTENT_ERROR,                XML element does not contains content as specified in the schema.) \
  ADD_RESULT_CODE(XML_END_TAG_NOT_FOUND,            End tag does not match with start tag of an element.)  \
  ADD_RESULT_CODE(XML_CONTENT_UNSUPPORTED,          Parser does not support this XML content type.)  \
  ADD_RESULT_CODE(XML_MIN_LENGTH_ERROR,             Length of content is less than minLength of restriction facet.)  \
  ADD_RESULT_CODE(XML_MAX_LENGTH_ERROR,             Length of content is greater than maxLength of restriction facet.) \
  ADD_RESULT_CODE(XML_MIN_VALUE_ERROR,              Value of content is less than minValue of restriction facet.)  \
  ADD_RESULT_CODE(XML_MAX_VALUE_ERROR,              Value of content is greater than maxValue of restriction facet.)  \
  ADD_RESULT_CODE(XML_ENUM_NOT_FOUND,               Content does not match with any of specified enumerations.)  \
  ADD_RESULT_CODE(XML_DURATION_SYNTAX_ERROR,        XML syntax error in duration content.)  \
  ADD_RESULT_CODE(XML_DATE_TIME_SYNTAX_ERROR,       XML syntax error in date time content.)  \
  ADD_RESULT_CODE(FAILED_TO_ALLOCATE_MEMORY,        Failed to allocate the memory of string dynamic type.)

/*
 *  ------------------------------- ENUMERATION -------------------------------
 */

//! List of result code of \ref parse_xml
#define ADD_RESULT_CODE(code, description) code,
typedef enum
{
  XML_PARSER_RESULT
}xml_parse_result_t;
#undef ADD_RESULT_CODE

//! List of target type to store the XML content.
typedef enum
{
  EN_STATIC,      //!< Target address is static. No dynamic allocation is required.
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

#if XML_PARSER_CALLBACK
/** A function pointer to trigger callback on successful parsing of element.
 *
 * \param occurrence uint32_t : The current occurrence count of the element in the XML
 * \param content void* const : Address containing content of element
 * \param context void*      : Context passed to xml parser.
 */
#if XML_PARSER_CONTEXT
  typedef void (*element_callback)(uint32_t occurrence, void* const content, void* context);
#else
  typedef void (*element_callback)(uint32_t occurrence, void* const content);
#endif // XML_PARSER_CONTEXT
#endif // XML_PARSER_CALLBACK

/** A function pointer to allocate dynamic memory to store content of element.
 *
 * \param occurrence uint32_t : The current occurrence count of the element in the XML
 * \param context void*      : Context passed to xml parser.
 * \return void*              : Target address to store xml content
 */
#if XML_PARSER_CONTEXT
  typedef void* (*allocate)(uint32_t occurrence, void* context);
#else
  typedef void* (*allocate)(uint32_t occurrence);
#endif // XML_PARSER_CONTEXT

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
  xml_content_t Content;    //!< Content type of attribute
  xs_attribute_use_t Use;   //!< Use of attribute required, optional or prohibited.
}xs_attribute_t;

typedef struct xs_element_t xs_element_t;

//! Structure to define element of XML
struct xs_element_t
{
  string_t Name;        //!< Name of an element
  uint32_t MinOccur;    //!< Minimum number of time element must occur
  uint32_t MaxOccur;    //!< Maximum number of time element is allowed to occur

#if XML_PARSER_CALLBACK
  element_callback Callback;  //!< Callback after successful parsing of an element.
#endif // XML_PARSER_CALLBACK

  target_address_t Target;  //!< Target address to store content of an element
  xml_content_t Content;    //!< Content type of an element

  uint32_t Attribute_Quantity;      //!< Number of attributes in the element
  const xs_attribute_t* Attribute;  //!< Address to array of attributes

  uint32_t Child_Quantity;          //!< Number of child elements of an element
  child_order_type_t Child_Order;   //!< order type of child elements
  const xs_element_t* Child;        //!< Address to array of child elements
};

/*
 *  ---------------------------- EXPORTED FUNCTION ----------------------------
 */

/** \brief XML parser to parse XML file.
 *
 * \param root const xs_element_t*: Pointer to root element of XML element tree.
 * \param source const char*      : Buffer containing XML source to parse.
 * \param target void* const      : Target address to store content of XML element in case of relative addressing type.
 * \param context void*           : User defined context.
 *        Parser doesn't use/modify this argument for parsing purpose.
 *        It passes this context in all the callback functions.
 *        If not required pass NULL.
 * \return xml_parse_result_t result of XML parsing.
 *
 */
extern xml_parse_result_t parse_xml(const xs_element_t* root, const char* source,
                                    void* const target
                                   #if XML_PARSER_CONTEXT
                                     , void* context
                                   #endif // XML_PARSER_CONTEXT
                                   );
#endif // PARSE_XML_H

