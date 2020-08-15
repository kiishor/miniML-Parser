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

#include "xml_content.h"

/*
 *  ------------------------------ FUNCTION BODY ------------------------------
 */

static inline bool get_duration_content(char** source, const char* const end,
                                        const char* pToken, uint32_t* pTarget)
{
  for(uint32_t i = 0; (i < 3) && (*source < end); i++)
  {
    uint32_t value = strtoul(*source, source, 10);

    if(*source >= end)
    {
      return false;
    }
    while(pToken[i] != **source++)
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

static inline bool get_duration(char* source, const char* const end, xs_duration_t* const duration)
{
  duration->Sign = true;
  if(*source == '-')
  {
    duration->Sign = false;
  }

  if(*source++ != 'P')
  {
    return false;
  }

  if(*source != 'T')
  {
    if(get_duration_content(&source, end, "YMD", &duration->Period.Date.Year) == false)
    {
      return false;
    }
  }

  if(*source++ == 'T')
  {
    return get_duration_content(&source, end, "HMS", &duration->Period.Time.Hour);
  }
  return true;
}

const char* extract_content(const xml_content_t* const content,
                            void* target, const char* source,
                            char token)
{
  const char* const tag = source;
  source = strchr(source, token);
  ASSERT2(source!= NULL);
  size_t length = source++ - tag;

  if(target == NULL)
  {
    return source;
  }

  switch(content->Type)
  {
  case EN_STRING:
  {
    string_t* const String = target;
    String->String = (char*)tag;
    String->Length = length;
    break;
  }

  case EN_CHAR_ARRAY:
    ASSERT2((length >= content->Facet.String.MinLength) &&
       (length <= content->Facet.String.MaxLength));
    memcpy(target, tag, length);
    ((char*)target)[length] = '\0';
    break;

  case EN_STRING_DYNAMIC:
  {
    ASSERT2((length >= content->Facet.String.MinLength) &&
       (length <= content->Facet.String.MaxLength));
    char* data = (char*)malloc(length);
    ASSERT2(data!= NULL);
    memcpy(data, tag, length);
    data[length] = '\0';
    (*(char**)target) = data;
  }
  break;

  case EN_UNSIGNED:
  {
    uint32_t value = strtoul(tag, NULL, 10);
    ASSERT2((value >= content->Facet.Uint.MinValue) &&
       (value <= content->Facet.Uint.MaxValue));
    (*(uint32_t*)target) = value;
  }
  break;


  case EN_DECIMAL:
  {
    float value = strtof(tag, NULL);
    ASSERT2((value >= content->Facet.Decimal.MinValue) &&
       (value <= content->Facet.Decimal.MaxValue));
    (*(float*)target) = value;
  }
  break;

  case EN_ENUM_STRING:
  {
    const string_t* const list = content->Facet.Enum.List;
    for(uint32_t i = 0; i < content->Facet.Enum.Quantity; i++)
    {
      if((length == list[i].Length) && (memcmp(list[i].String, tag, length) == 0))
      {
        (*(uint32_t*)target) = i;
        return source;
      }
    }
  }
  return NULL;

  case EN_ENUM_UINT:
  {
    uint32_t value = strtoul(tag, NULL, 10);
    const uint32_t* const list = content->Facet.Enum.List;
    for(uint32_t i = 0; i < content->Facet.Enum.Quantity; i++)
    {
      if(value == list[i])
      {
        (*(uint32_t*)target) = value;
        return source;
      }
    }
  }
  return NULL;

  case EN_DURATION:
  {
    char* sourceStr = (char*)tag;
    if(get_duration(sourceStr, sourceStr + length, target) == false)
    {
      return NULL;
    }
  }
  break;

  default:
    return NULL;
  }
  return source;
}
