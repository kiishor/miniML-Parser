/**
 * \file
 * \brief   Library for parsing XML.

 * \author  Nandkishor Biradar
 * \date    19 August 2019
 */

/*
 *  ------------------------------ INCLUDE FILES ------------------------------
 */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "libs/parse_xml.h"

/*
 *	--------------------------- FORWARD DECLARATION ---------------------------
 */

static inline const char* parse_parent_element(const xs_element_t* const parent,
                                               const char* source,
                                               void* target,
                                               xml_parse_result_t* const result,
                                               void** context);
/*
 *  ------------------------------ FUNCTION BODY ------------------------------
 */

static inline const char* skip_whitespace(const char* source)
{
  do
  {
    switch(*source)
    {
    case '\0':
      return NULL;

    case '\r':
    case '\n':
    case '\t':
    case ' ':
      source++;
      break;

    default:
      return source;
    }
  }while(1);
}

static inline const char* extract_element_tag(const char* source)
{
  while(1)
  {
    switch(*source)
    {
    case '\0':
      return NULL;

    case '\r':
    case '\n':
    case '\t':
    case ' ':

    case '>':
    case '/':
      return source;

    default:
      source++;
      break;
    }
  }
}

static inline const char* extract_attribute_tag(const char* source)
{
  while(1)
  {
    switch(*source)
    {
    case '\0':
      return NULL;

    case '\r':
    case '\n':
    case '\t':
    case ' ':

    case '=':
      return source;

    default:
      source++;
      break;
    }
  }
}

static inline void* get_target_address(const target_address_t* const address,
                                        void* target, uint32_t occurrence,
                                        void** context)
{
  switch(address->Type)
  {
  case EN_STATIC:
    return address->Address + (occurrence * address->Size);

  case EN_DYNAMIC:
    return address->Allocate(occurrence, context);

  case EN_RELATIVE:
    return target + (occurrence * address->Size) + address->Offset;

  default:
    return NULL;
  }
}

static inline const char* parse_attribute(const xs_attribute_t* const attribute,
                                          const char* source, void* target,
                                          void** context)
{
  source = skip_whitespace(source);
  ASSERT2(source != NULL);
  ASSERT2(*source++ == '=');

  source = skip_whitespace(source);
  ASSERT2(source != NULL);
  ASSERT2(*source++ == '"');

  return extract_content(&attribute->Content,
                         get_target_address(&attribute->Target, target, 0, context),
                         source, '"');
}

static inline xml_parse_result_t validate_attributes(const xs_element_t* const element,
                                                     const bool* const occurrence)
{
  for(uint32_t i = 0; i < element->Attribute_Quantity; i++)
  {
    if(!occurrence[i] && element->Attribute[i].Use == EN_REQUIRED)
    {
      return XML_ATTRIBUTE_NOT_FOUND;
    }
  }
  return XML_PARSE_SUCCESS;
}


static inline xml_parse_result_t validate_empty_element(const xs_element_t* const element)
{
  for(uint32_t i = 0; i < element->Child_Quantity; i++)
  {
    if(element->Child[i].MinOccur > 0)
    {
      return XML_ELEMENT_MAX_OCCURRENCE_ERR;
    }
  }

  if(element->Content.Type != EN_NO_XML_DATA_TYPE)
  {
    return XML_CONTENT_ERROR;
  }

  return XML_PARSE_SUCCESS;
}

static inline const char* validate_element(const xs_element_t* const element,
                                           const char* source,
                                           xml_parse_result_t* const result)
{
  const char* const tag = source;
  source = extract_element_tag(source);
  ASSERT1((source != NULL), *result, XML_SYNTAX_ERROR);
  size_t length = source - tag;

  ASSERT1((length == element->Name.Length) &&
     (strncmp(element->Name.String, tag, length) == 0), *result, XML_END_TAG_NOT_FOUND)

  source = skip_whitespace(source);
  ASSERT1((source != NULL), *result, XML_SYNTAX_ERROR);
  ASSERT1((*source++ == '>'), *result, XML_SYNTAX_ERROR);
  return source;
}


static inline const char* parse_element(const xs_element_t* const element,
                                               const char* source, void* target,
                                               xml_parse_result_t* const result,
                                               void** context)
{
  bool occurrence[element->Attribute_Quantity];
  uint32_t attribute_occurred = 0;

  for(uint32_t i = 0; i < element->Attribute_Quantity; i++)
  {
    occurrence[i] = false;
  }

  while(1)
  {
    source = skip_whitespace(source);
    ASSERT1((source != NULL), *result, XML_SOURCE_STRING_END_ERR);

    switch(*source)
    {
    case '/':
      source++;
      ASSERT1((*source++ == '>'), *result, XML_SYNTAX_ERROR);
      *result = validate_attributes(element, occurrence);
      ASSERT2(*result == XML_PARSE_SUCCESS);
      *result = validate_empty_element(element);
      ASSERT2(*result == XML_PARSE_SUCCESS);
      return source;

    case '>':
      *result = validate_attributes(element, occurrence);
      ASSERT2(*result == XML_PARSE_SUCCESS);
      source++;
      if(element->Child_Quantity > 0)
      {
        source = parse_parent_element(element, source, target, result, context);
        ASSERT2(source != NULL);
      }
      else if(element->Content.Type != EN_NO_XML_DATA_TYPE)
      {
        source = extract_content(&element->Content, target, source, '<');
        ASSERT1(source != NULL, *result, XML_CONTENT_ERROR);
        ASSERT1(*source++ == '/', *result, XML_SYNTAX_ERROR);
      }
      else
      {
        source = skip_whitespace(source);
        ASSERT1(source != NULL, *result, XML_SYNTAX_ERROR);
        ASSERT1(*source++ == '<', *result, XML_SYNTAX_ERROR);
        ASSERT1(*source++ == '/', *result, XML_SYNTAX_ERROR);
      }
      return validate_element(element, source, result);
    }

    ASSERT1(attribute_occurred != element->Attribute_Quantity, *result, XML_SYNTAX_ERROR);

    const char* const tag = source;
    source = extract_attribute_tag(source);
    ASSERT1(source != NULL, *result, XML_SOURCE_STRING_END_ERR);
    size_t length = source - tag;
    uint32_t i = 0;
    while(1)
    {
      if((length == element->Attribute[i].Name.Length) &&
         (strncmp(tag, element->Attribute[i].Name.String, length) == 0))
      {
        ASSERT1(!occurrence[i], *result, XML_DUPLICATE_ATTRIBUTE);

        occurrence[i] = true;
        source = parse_attribute(&element->Attribute[i], source, target, context);
        ASSERT1(source != NULL, *result, XML_SYNTAX_ERROR);
        attribute_occurred++;
        break;
      }

      ASSERT1(++i < element->Attribute_Quantity, *result, XML_ATTRIBUTE_NOT_FOUND);
    }
  }
}

static inline const char* parse_parent_element(const xs_element_t* const parent,
                                               const char* source,
                                               void* parent_target,
                                               xml_parse_result_t* const result,
                                               void** context)
{

  uint32_t occurrence[parent->Child_Quantity];
  uint32_t element_index = 0;

  for(uint32_t i = 0; i < parent->Child_Quantity; i++)
  {
    occurrence[i] = 0;
  }

  while(1)
  {
    source = skip_whitespace(source);
    ASSERT1(source != NULL, *result, XML_SOURCE_STRING_END_ERR);
    ASSERT1(*source++ == '<', *result, XML_INVALID_START_TOKEN_ERR);

    switch(*source)
    {
    case '?':
    case '!':
      source = strchr(source, '>');
      ASSERT1(source != NULL, *result, XML_SOURCE_STRING_END_ERR);
      source++;
      continue;

    case '/':
      for(uint32_t i = 0; i < parent->Child_Quantity; i++)
      {
        ASSERT1(occurrence[i] >= parent->Child[i].MinOccur, *result, XML_ELEMENT_MIN_OCCURRENCE_ERR)
      }
      *result = XML_PARSE_SUCCESS;
      return ++source;
    }

    const char* const tag = source;
    source = extract_element_tag(source);
    ASSERT1(source != NULL, *result, XML_SOURCE_STRING_END_ERR);
    size_t length = source - tag;

    switch(parent->Child_Type)
    {
    case EN_CHOICE:
      element_index = 0;
      while(1)
      {
        if((length == parent->Child[element_index].Name.Length) &&
          (strncmp(tag, parent->Child[element_index].Name.String, length) == 0))
        {
          break;
        }

        ASSERT1(++element_index < parent->Child_Quantity, *result, XML_ELEMENT_NOT_FOUND_ERR);
      }
      break;

    case EN_SEQUENCE:
      while(1)
      {
        if((length == parent->Child[element_index].Name.Length) &&
          (strncmp(tag, parent->Child[element_index].Name.String, length) == 0))
        {
          break;
        }

        ASSERT1(occurrence[element_index] >= parent->Child[element_index].MinOccur,
                *result, XML_ELEMENT_MIN_OCCURRENCE_ERR);
        ASSERT1(++element_index < parent->Child_Quantity, *result, XML_ELEMENT_NOT_FOUND_ERR);
      }
      break;
    }

    const xs_element_t* const element = &parent->Child[element_index];
    void* target = get_target_address(&element->Target, parent_target,
                                      occurrence[element_index], context);

    ASSERT1(++occurrence[element_index] <= element->MaxOccur, *result, XML_ELEMENT_MAX_OCCURRENCE_ERR)

    source = parse_element(element, source, target, result, context);
    ASSERT2(source != NULL);

    if(element->Callback)
    {
      element->Callback(occurrence[element_index], target, context);
    }

    if(parent->Name.String == NULL)
    {
      *result = XML_PARSE_SUCCESS;
      return NULL;
    }
  }
}

xml_parse_result_t parse_xml(const xs_element_t* root, const char* source, void** context)
{
  xml_parse_result_t result;
  parse_parent_element(root, source, NULL, &result, context);
  return result;
}
