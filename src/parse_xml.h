#ifndef PARSE_XML_H
#define PARSE_XML_H

/**
 * \file
 * \brief   Header file for parse_xml.c.

 * \author  Nandkishor Biradar
 * \date    19 August 2019
 */

/*
 *  ------------------------------ INCLUDE FILES ------------------------------
 */

#include "xml_content.h"

/*
 *  ------------------------------- DEFINITION -------------------------------
 */

#define ALL_XSD_ATTRIBUTE_USE_VALUE   \
  ADD_USE_TYPE(EN_OPTIONAL, optional)   \
  ADD_USE_TYPE(EN_REQUIRED, required)   \
  ADD_USE_TYPE(EN_PROHIBITED, prohibited)

#define ALL_TARGET_ADDRESS_TYPE \
  ADD_TYPE(EN_STATIC)   \
  ADD_TYPE(EN_DYNAMIC)  \
  ADD_TYPE(EN_RELATIVE)


/*
 *  ------------------------------- ENUMERATION -------------------------------
 */

typedef enum
{
  XML_PARSE_SUCCESS,
  XML_SYNTAX_ERROR,
  XML_SOURCE_STRING_END_ERR,
  XML_INVALID_START_TOKEN_ERR,
  XML_ELEMENT_NOT_FOUND_ERR,
  XML_ELEMENT_MAX_OCCURRENCE_ERR,
  XML_ELEMENT_MIN_OCCURRENCE_ERR,
  XML_NULL_TARGET_ADDRESS,
  XML_DUPLICATE_ATTRIBUTE,
  XML_ATTRIBUTE_NOT_FOUND,
  XML_CONTENT_ERROR,
  XML_END_TAG_NOT_FOUND,
}xml_parse_result_t;

#define ADD_TYPE(TYPE)  TYPE,
typedef enum
{
  ALL_TARGET_ADDRESS_TYPE
  TOTAL_TARGET_ADDRESS_TYPE
}address_type_t;
#undef ADD_TYPE

typedef enum
{
  EN_CHOICE,
  EN_SEQUENCE,
}element_descendant_type_t;

#define ADD_USE_TYPE(TAG, ...) TAG,
typedef enum
{
  ALL_XSD_ATTRIBUTE_USE_VALUE
  TOTAL_XSD_ATTRIBUTE_USE_VALUES
}xs_attribute_use_t;
#undef ADD_USE_TYPE

/*
 *  -------------------------------- STRUCTURE --------------------------------
 */

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
  element_descendant_type_t Child_Type;
  const xs_element_t* Child;
};

/*
 *  ---------------------------- EXPORTED FUNCTION ----------------------------
 */

 extern xml_parse_result_t parse_xml(const xs_element_t* root, const char* source,
                                     void** context);

#endif // PARSE_XML_H

