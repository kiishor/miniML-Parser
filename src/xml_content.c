/**
 * \file
 * \brief Helper library for extracting XML content

 * \author  Nandkishor Biradar
 * \date    19 August 2019

 *  Copyright (c) 2019-2022 Nandkishor Biradar
 *  https://github.com/kiishor

 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 */

/*
 *  ------------------------------ INCLUDE FILES ------------------------------
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "parse_xml.h"
#include "parse_xml_internal.h"

/*
 *  ------------------------------ FUNCTION BODY ------------------------------
 */

/** \brief Extract the unsigned value separated by specified token.
 *
 * \param source const char*    XML content source
 * \param end const char*const  End of XML content source
 * \param token const char      token separating unsigned value in the string
 * \param pTarget uint32_t*     Target to store extracted value.
 * \return const char*          If extraction of unsigned values is successful returns the end of string otherwise NULL
 *
 */
static inline const char* get_tokenized_content(const char* source, const char* const end,
                                                const char token, uint32_t* pTarget)
{
  uint32_t i = 0;
  do
  {
    char* endPtr;
    uint32_t value = strtoul(source, &endPtr, 10);
    if(endPtr > end)
    {
      return NULL;
    }
    source = endPtr;
    pTarget[i] = value;

    if(++i == 3)
    {
      return source;
    }

    if(token != *source++)
    {
      return NULL;
    }
  }while(1);
}

/** \brief extract the xs:dateTime from XML content
 *
 * \param source const char* : XML content source
 * \param end const char* const : End of XML content source
 * \param pTarget uint32_t* : Target to store xs:dateTime
 * \return xml_parse_result_t: Result of operation.
 *
 */
static inline xml_parse_result_t get_date_time(const char* source, const char* const end, uint32_t* pTarget)
{
  source = get_tokenized_content(source, end, '-', pTarget);
  ASSERT(source, XML_DATE_TIME_SYNTAX_ERROR, "Syntax error in XML date time content.\n");

  if(*source++ == 'T')
  {
    ASSERT(get_tokenized_content(source, end, ':', pTarget), XML_DATE_TIME_SYNTAX_ERROR,
           "Syntax error in XML date time content.\n");
  }
  return XML_PARSE_SUCCESS;
}

/** \brief Helper function to extract content of xs:date or xs:time depending on passed tokens.
 * To extract xs:date content pass "YMD"and to extract xs:time pass "HMS" as token.
 *
 * \param source const char* : Source of XML content
 * \param end const char*const : end of XML content.
 * \param pToken const char* : Array of tokens to extract the content of .
 * \param pTarget uint32_t* : Target address to store the extracted content.
 * \return bool : return true if extraction is successful otherwise false.
 *
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

/** \brief Extracts the xs:duration from XML content.
 *
 * \param source const char* : XML content source.
 * \param end const char* const : end of XML content.
 * \param duration xs_duration_t* const : pointer to store xs:duration.
 * \return xml_parse_result_t : Result of operation.
 *
 */
static inline xml_parse_result_t get_duration(const char* source, const char* const end, xs_duration_t* const duration)
{
  duration->Sign = true;
  if(*source == '-')
  {
    duration->Sign = false;
  }

  ASSERT(*source++ == 'P', XML_DURATION_SYNTAX_ERROR,
         "Syntax error in XML duration content. Missing 'P' at the start of content.\n");

  if(*source != 'T')
  {
    ASSERT(get_duration_content(source, end, "YMD", &duration->Period.Date.Year), XML_DURATION_SYNTAX_ERROR,
           "Syntax error in XML duration content.\n");
  }

  if(*source++ == 'T')
  {
    ASSERT(get_duration_content(source, end, "HMS", &duration->Period.Time.Hour), XML_DURATION_SYNTAX_ERROR,
           "Syntax error in XML duration content.\n");
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
    ASSERT((length >= content->Facet.String.MinLength), XML_MIN_LENGTH_ERROR,
           "Length of xs:string content '%llu' is less than '%u' minLength of restriction facet.\n",
           length, content->Facet.String.MinLength);
    ASSERT((length <= content->Facet.String.MaxLength), XML_MAX_LENGTH_ERROR,
           "Length of xs:string content '%llu' is greater than '%u' maxLength of restriction facet.\n",
           length, content->Facet.String.MaxLength);

    string_t* const String = target;
    String->String = (char*)source;
    String->Length = length;
    break;
  }

  case EN_CHAR_ARRAY:
    ASSERT((length >= content->Facet.String.MinLength), XML_MIN_LENGTH_ERROR,
           "Length of xs:string content '%llu' is less than '%u' minLength of restriction facet.\n",
           length, content->Facet.String.MinLength);
    ASSERT((length <= content->Facet.String.MaxLength), XML_MAX_LENGTH_ERROR,
           "Length of xs:string content '%llu' is greater than '%u' maxLength of restriction facet.\n",
           length, content->Facet.String.MaxLength);

    memcpy(target, source, length);
    ((char*)target)[length] = '\0';
    break;

  case EN_STRING_DYNAMIC:
  {
    ASSERT((length >= content->Facet.String.MinLength), XML_MIN_LENGTH_ERROR,
           "Length of xs:string content '%llu' is less than '%u' minLength of restriction facet.\n",
           length, content->Facet.String.MinLength);
    ASSERT((length <= content->Facet.String.MaxLength), XML_MAX_LENGTH_ERROR,
           "Length of x:string content '%llu' is greater than '%u' maxLength of restriction facet.\n",
           length, content->Facet.String.MaxLength);

    char* data = (char*)malloc(length + 1);
    ASSERT(data!= NULL, FAILED_TO_ALLOCATE_MEMORY, "Failed to allocate dynamic memory for XML string content\n");
    memcpy(data, source, length);
    data[length] = '\0';
    (*(char**)target) = data;
    break;
  }

  case EN_UINT32:
  {
    uint32_t value = strtoul(source, NULL, 10);
    ASSERT((value >= content->Facet.Uint.MinValue), XML_MIN_VALUE_ERROR,
           "Value of unsigned int content '%d' is less than '%d' minValue of restriction facet.\n",
           value, content->Facet.Uint.MinValue);
    ASSERT((value <= content->Facet.Uint.MaxValue), XML_MAX_VALUE_ERROR,
           "Value of unsigned int content '%d' is greater than '%d' maxValue of restriction facet.\n",
           value, content->Facet.Uint.MaxValue);
    (*(uint32_t*)target) = value;
    break;
  }

  case EN_UINT16:
  {
    uint16_t value = (uint16_t)strtoul(source, NULL, 10);
    ASSERT((value >= content->Facet.Uint.MinValue), XML_MIN_VALUE_ERROR,
           "Value of 16-bit unsigned int content '%d' is less than '%d' minValue of restriction facet.\n",
           value, content->Facet.Uint.MinValue);
    ASSERT((value <= content->Facet.Uint.MaxValue), XML_MAX_VALUE_ERROR,
           "Value of 16-bit unsigned int content '%d' is greater than '%d' maxValue of restriction facet.\n",
           value, content->Facet.Uint.MaxValue);
    (*(uint16_t*)target) = value;
    break;
  }

  case EN_UINT8:
  {
    uint8_t value = (uint8_t)strtoul(source, NULL, 10);
    ASSERT((value >= content->Facet.Uint.MinValue), XML_MIN_VALUE_ERROR,
           "Value of 8-bit unsigned short content '%d' is less than '%d' minValue of restriction facet.\n",
           value, content->Facet.Uint.MinValue);
    ASSERT((value <= content->Facet.Uint.MaxValue), XML_MAX_VALUE_ERROR,
           "Value of 8-bit unsigned short content '%d' is greater than '%d' maxValue of restriction facet.\n",
           value, content->Facet.Uint.MaxValue);
    (*(uint8_t*)target) = value;
    break;
  }

  case EN_INT32:
  {
    int32_t value = strtol(source, NULL, 10);
    ASSERT((value >= content->Facet.Int.MinValue), XML_MIN_VALUE_ERROR,
            "Value of integers content '%d' is less than '%d' minValue of restriction facet.\n",
            value, content->Facet.Int.MinValue);
    ASSERT((value <= content->Facet.Int.MaxValue), XML_MAX_VALUE_ERROR,
           "Value of integer content '%d' is greater than '%d' maxValue of restriction facet.\n",
           value, content->Facet.Int.MaxValue);
    (*(int32_t*)target) = value;
    break;
  }

  case EN_DECIMAL:
  {
    float value = strtof(source, NULL);
    ASSERT((value >= content->Facet.Decimal.MinValue), XML_MIN_VALUE_ERROR,
            "Value of decimal content '%f' is less than '%f' minValue of restriction facet.\n",
            value, content->Facet.Decimal.MinValue);
    ASSERT((value <= content->Facet.Decimal.MaxValue), XML_MAX_VALUE_ERROR,
           "Value of decimal content '%f' is greater than '%f' maxValue of restriction facet.\n",
           value, content->Facet.Decimal.MaxValue);
    (*(float*)target) = value;
    break;
  }

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
    #if XML_PARSER_DEBUG
    printf("Content '%.*s' does not match with any of specified enumerations.\n", (int)length, source);
    #endif // XML_PARSER_DEBUG
    return XML_ENUM_NOT_FOUND;
  }

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
    #if XML_PARSER_DEBUG
    printf("Content '%d' does not match with any of specified enumerations.\n", value);
    #endif // XML_PARSER_DEBUG
    return XML_ENUM_NOT_FOUND;
  }

  case EN_DURATION:
    return get_duration(source, source + length, target);

  case EN_DATE:
    {
      const char* const result = get_tokenized_content(source, source + length, '-', target);
      ASSERT(result, XML_DATE_TIME_SYNTAX_ERROR, "Syntax error in XML date content.\n");
      return XML_PARSE_SUCCESS;
    }

  case EN_TIME:
    {
      const char* const result = get_tokenized_content(source, source + length, ':', target);
      ASSERT(result, XML_DATE_TIME_SYNTAX_ERROR, "Syntax error in XML time content.\n");
      return XML_PARSE_SUCCESS;
    }

  case EN_DATE_TIME:
    return get_date_time(source, source + length, target);

  default:
    return XML_CONTENT_UNSUPPORTED;
  }
  return XML_PARSE_SUCCESS;
}
