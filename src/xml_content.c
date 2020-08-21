/**
 * \file
 * \brief   Helper library for extracting XML content

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

#include "parse_xml.h"

/*
 *  ------------------------------- DEFINITION -------------------------------
 */
//! Asserts the expression. if fails return the function with error_code.
#define ASSERT3(expression, error_code) if(!(expression)) return error_code

/*
 *  ------------------------------ FUNCTION BODY ------------------------------
 */

static inline bool get_duration_content(const char* source, const char* const end,
                                        const char* pToken, uint32_t* pTarget)
{
  for(uint32_t i = 0; (i < 3) && (source < end); i++)
  {
    char* endptr;
    uint32_t value = strtoul(source, &endptr, 10);

    if(endptr >= end)
    {
      return false;
    }
    source = endptr;
    while(pToken[i] != *source++)
    {
      if(++i == 3)
      {
        return false;
      }
    }
    pTarget[i] = value;

  }
  return true;
}

static inline xml_parse_result_t get_duration(const char* source, const char* const end,
                                xs_duration_t* const duration)
{
  duration->Sign = true;
  if(*source == '-')
  {
    duration->Sign = false;
  }

  if(*source++ != 'P')
  {
    return XML_DURATION_SYNTAX_ERROR;
  }

  if(*source != 'T')
  {
    if(get_duration_content(source, end, "YMD", &duration->Period.Date.Year) == false)
    {
      return XML_DURATION_SYNTAX_ERROR;
    }
  }

  if(*source++ == 'T')
  {
    if(get_duration_content(source, end, "HMS", &duration->Period.Time.Hour) == false)
    {
      return XML_DURATION_SYNTAX_ERROR;
    }
  }

  return XML_PARSE_SUCCESS;
}

xml_parse_result_t extract_content(const xml_content_t* const content,
                            void* target, const char* const source,
                            size_t length)
{
  if(target == NULL)
  {
    return XML_PARSE_SUCCESS;
  }

  switch(content->Type)
  {
  case EN_STRING:
  {
    string_t* const String = target;
    String->String = (char*)source;
    String->Length = length;
    break;
  }

  case EN_CHAR_ARRAY:
    ASSERT3((length >= content->Facet.String.MinLength), XML_MIN_LENGTH_ERROR);
    ASSERT3((length <= content->Facet.String.MaxLength), XML_MAX_LENGTH_ERROR);

    memcpy(target, source, length);
    ((char*)target)[length] = '\0';
    break;

  case EN_STRING_DYNAMIC:
  {
    ASSERT3((length >= content->Facet.String.MinLength), XML_MIN_LENGTH_ERROR);
    ASSERT3((length <= content->Facet.String.MaxLength), XML_MAX_LENGTH_ERROR);

    char* data = (char*)malloc(length);
    ASSERT3(data!= NULL, FAILED_TO_ALLOCATE_MEMORY);
    memcpy(data, source, length);
    data[length] = '\0';
    (*(char**)target) = data;
  }
  break;

  case EN_UNSIGNED:
  {
    uint32_t value = strtoul(source, NULL, 10);
    ASSERT3((value >= content->Facet.Uint.MinValue), XML_MIN_VALUE_ERROR);
    ASSERT3((value <= content->Facet.Uint.MaxValue), XML_MAX_VALUE_ERROR);
    (*(uint32_t*)target) = value;
  }
  break;

  case EN_DECIMAL:
  {
    float value = strtof(source, NULL);
    ASSERT3((value >= content->Facet.Decimal.MinValue), XML_MIN_VALUE_ERROR);
    ASSERT3((value <= content->Facet.Decimal.MaxValue), XML_MAX_VALUE_ERROR);
    (*(float*)target) = value;
  }
  break;

  case EN_ENUM_STRING:
  {
    const string_t* const list = content->Facet.Enum.List;
    for(uint32_t i = 0; i < content->Facet.Enum.Quantity; i++)
    {
      if((length == list[i].Length) && (memcmp(list[i].String, source, length) == 0))
      {
        (*(uint32_t*)target) = i;
        return XML_PARSE_SUCCESS;
      }
    }
  }
  return XML_ENUM_NOT_FOUND;

  case EN_ENUM_UINT:
  {
    uint32_t value = strtoul(source, NULL, 10);
    const uint32_t* const list = content->Facet.Enum.List;
    for(uint32_t i = 0; i < content->Facet.Enum.Quantity; i++)
    {
      if(value == list[i])
      {
        (*(uint32_t*)target) = value;
        return XML_PARSE_SUCCESS;
      }
    }
  }
  return XML_ENUM_NOT_FOUND;

  case EN_DURATION:
  {
    return get_duration(source, source + length, target);
  }
  break;

  default:
    return XML_CONTENT_UNSUPPORTED;
  }
  return XML_PARSE_SUCCESS;
}
