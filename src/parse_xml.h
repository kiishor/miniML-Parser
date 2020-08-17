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

//! List of
typedef enum
{
  EN_ALL,
  EN_CHOICE,
  EN_SEQUENCE,
}child_order_type_t;

typedef enum
{
  EN_OPTIONAL,
  EN_REQUIRED,
  EN_PROHIBITED,
  TOTAL_XSD_ATTRIBUTE_USE_VALUES
}xs_attribute_use_t;

/*
 *  ------------------------------ INCLUDE FILES ------------------------------
 */

#include "xml_content.h"

/*
 *  -------------------------------- STRUCTURE --------------------------------
 */
typedef struct
{
  char* String;   // This pointer can not be const because this can be created dynamically.
  size_t Length;
}string_t;


typedef void (*element_callback)(uint32_t occurrence, void* const content, void** context);
typedef void* (*allocate)(uint32_t occurrence, void** context);

typedef struct
{
  address_type_t Type;
  union  {
    void* Address;
    allocate Allocate;
    size_t Offset;
  };
// Size of Target in case of multiple occurrence to calculate the next target address.
  uint32_t Size;
}target_address_t;

typedef struct
{
  string_t Name;
  target_address_t Target;
  xml_content_t Content;
  xs_attribute_use_t Use;
}xs_attribute_t;

typedef struct xs_element_t xs_element_t;

struct xs_element_t
{
  string_t Name;
  uint32_t MinOccur;
  uint32_t MaxOccur;
  element_callback Callback;

  target_address_t Target;
  xml_content_t Content;

  uint32_t Attribute_Quantity;
  const xs_attribute_t* Attribute;

  uint32_t Child_Quantity;
  child_order_type_t Child_Order;
  const xs_element_t* Child;
};

/*
 *  ---------------------------- EXPORTED FUNCTION ----------------------------
 */

 extern xml_parse_result_t parse_xml(const xs_element_t* root, const char* source,
                                     void** context);

#endif // PARSE_XML_H

