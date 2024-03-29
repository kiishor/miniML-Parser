/**
 * \file
 * \brief   XML parser in C

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
 *  ------------------------------- DEFINITION -------------------------------
 */

//! Assert the result of function.
#define ASSERT_RESULT(function)           \
do {                                      \
  xml_parse_result_t result = function;   \
  ASSERT(result == XML_PARSE_SUCCESS, result, "");   \
}while(0)

//! Skips the white space till it finds the token
#define ASSERT_TOKEN(source, token)                 \
do {                                                \
  source = skip_whitespace(source);                 \
  ASSERT((source != NULL), XML_INCOMPLETE_SOURCE, "Incomplete XML source.\n");  \
  ASSERT((*source++ == token), XML_SYNTAX_ERROR, "XML is not well formed.\n");     \
}while(0)

#if XML_PARSER_CONTEXT
  #define CONTEXT_PTR , void* context
  #define CONTEXT_ARG , context
#else
  #define CONTEXT_PTR
  #define CONTEXT_ARG
#endif // XML_PARSER_CONTEXT

/*
 *	--------------------------- FORWARD DECLARATION ---------------------------
 */

static inline xml_parse_result_t parse_parent_element(const xs_element_t* const parent,
                                                      const char** input,
                                                      void* parent_target
                                                      CONTEXT_PTR
                                                      );
/*
 *  ------------------------------ FUNCTION BODY ------------------------------
 */

/** \brief This function skips/ignore all the whitespace chars
 *         till it reaches to non-whitespace char or end of string.
 *
 * \param source const char* Source : string to skip whitespace
 * \return const char*  Address of first non-whitespace char in the string or
 *                      NULL on end of string
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

/** \brief gets the elements end tag ('>' or '/>').
 *
 * \param source const char* : Source XML string to find element end tag
 * \return const char* : returns the address of end tag or NULL on end of string.
 */
static inline const char* get_element_end_tag(const char* source)
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

/** \brief returns the end of attribute tag.
 *
 * \param source const char* : Source of XMl string to find end of attribute tag
 * \return const char* : returns end of attribute tag or NULL on end of string.
 */
static inline const char* get_attribute_tag(const char* source)
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

/** \brief Get the target address to store XML content based on address type.
 *
 * \param address const target_address_t*const : target address type.
 * \param target void* : parent element target address. Used only in relative type of target address
 * \param occurrence uint32_t : occurrence of element in the XMl.
 * \param context void* : user defined context. used only in dynamic type of target address.
 * \return void* : Returns the target address to store XML content.
 */
static inline void* get_target_address(const target_address_t* const address,
                                       void* target, uint32_t occurrence
                                       CONTEXT_PTR)
{
  switch(address->Type)
  {
  case EN_STATIC:
    return (void*)((size_t)(address->Address) + (occurrence * address->Size));

  case EN_DYNAMIC:
    return address->Allocate(occurrence CONTEXT_ARG);

  case EN_RELATIVE:
    return (void*)((size_t)(target) + (occurrence * address->Size) + address->Offset);

  default:
    return NULL;
  }
}

/** \brief Verify that all the required attributes occurred in the XML element.
 *
 * \param element const xs_element_t* const : XML element to verify
 * \param occurrence const bool*const : Occurrence table of attribute
 * \return xml_parse_result_t result of validation.
 */
static inline xml_parse_result_t validate_attributes(const xs_element_t* const element,
                                                     const bool* const occurrence)
{
  for(uint32_t i = 0; i < element->Attribute_Quantity; i++)
  {
    ASSERT((occurrence[i] || element->Attribute[i].Use == EN_OPTIONAL),
           XML_ATTRIBUTE_NOT_FOUND,
           "required attribute '%s' not found\n", element->Attribute[i].Name.String);
  }
  return XML_PARSE_SUCCESS;
}

/** \brief Validate an empty element.
 *
 * \param element const xs_element_t*const : element to validate.
 * \return xml_parse_result_t result of validation.
 */
static inline xml_parse_result_t validate_empty_element(const xs_element_t* const element)
{
  for(uint32_t i = 0; i < element->Child_Quantity; i++)
  {
    ASSERT((!element->Child[i].MinOccur), XML_ELEMENT_MIN_OCCURRENCE_ERR,
           "XML element '%s' occurred less than specified count %d in the schema\n",
           element->Child[i].Name.String, element->Child[i].MinOccur);
  }

  ASSERT((element->Content.Type == EN_NO_XML_DATA_TYPE), XML_CONTENT_ERROR,
         "XML element '%s' does not contains content as specified in the schema.\n", element->Name.String);

  return XML_PARSE_SUCCESS;
}

/** \brief Validate that in case of child element order is specified as choice then
 * only one child element can be present.(Though it can have multiple
 * occurrence of that element as specified in the maxOccur)
 *
 * \param occurrence uint32_t* : pointer to array of occurrence table
 * \param quantity size_t : Number of child elements
 * \return bool : return true if only on child element occurred otherwise false.
 */
static inline bool validate_choice_order(uint32_t* occurrence, size_t quantity)
{
  uint32_t count = 0;
  for(uint32_t i = 0; i < quantity; i++)
  {
    if(occurrence[i])
    {
      if(++count > 1)
      {
        return false;   // More than one child elements found
      }
    }
  }
  return true;
}

/** \brief validate the XML element. Check for matching end element tag.
 *
 * \param element const xs_element_t*const : element to validate.
 * \param input const char** : Input XML string.
 * \return xml_parse_result_t : result of element validation
 */
static inline xml_parse_result_t validate_element(const xs_element_t* const element,
                                                  const char** input)
{
  const char* const tag = *input;
  const char* source = get_element_end_tag(*input);
  ASSERT((source != NULL), XML_INCOMPLETE_SOURCE, "Incomplete XML source. Missing end tag of element.\n");
  size_t length = source - tag;

  ASSERT((length == element->Name.Length) && (strncmp(element->Name.String, tag, length) == 0),
         XML_END_TAG_NOT_FOUND,
         "End tag '%.*s' does not match with start tag '%s' of an element.\n", (int)length, tag, element->Name.String);

  ASSERT_TOKEN(source, '>');
  *input = source;
  return XML_PARSE_SUCCESS;
}

/** \brief Parses XML attribute and extracts the content of attribute
 *
 * \param attribute const xs_attribute_t*const : Structure defining XML attribute to parse.
 * \param input const char** : input XML string to parse and extract the content of attribute
 * \param target void* : Target address to store XMl content
 * \param context void* : User defined context.
 * \return xml_parse_result_t result of parsing
 */
static inline xml_parse_result_t parse_attribute(const xs_attribute_t* const attribute,
                                                 const char** input, void* target
                                                 CONTEXT_PTR)
{
  const char* source = *input;

  ASSERT_TOKEN(source, '=');
  ASSERT_TOKEN(source, '"');

  const char* const tag = source;
  source = strchr(source, '"');
  ASSERT((source != NULL), XML_INCOMPLETE_SOURCE, "Incomplete XML source.\n");
  size_t length = source++ - tag;
  *input = source;
  return extract_content(&attribute->Content,
                   get_target_address(&attribute->Target, target, 0 CONTEXT_ARG),
                   tag, length);
}

/** \brief parses a XML string to extract attribute and its content as specified
 *         in the given xs_element_t
 *
 * \param element const xs_element_t*const : Contain schema definition of an element.
 * \param input const char** : Input XML string to parse
 * \param target void* : Target address to store XML content.
 * \param context void* : User specified context.
 * \return xml_parse_result_t : result of parsing.
 */
static inline xml_parse_result_t parse_element(const xs_element_t* const element,
                                               const char** input, void* target
                                               CONTEXT_PTR)
{
  bool occurrence[element->Attribute_Quantity];
  uint32_t attribute_occurred = 0;
  const char* source = *input;

  for(uint32_t i = 0; i < element->Attribute_Quantity; i++)
  {
    occurrence[i] = false;
  }

  while(1)
  {
    source = skip_whitespace(source);
    ASSERT((source != NULL), XML_INCOMPLETE_SOURCE, "Incomplete XML source.\n");

    switch(*source)
    {
    case '/':
      {
        source++;
        ASSERT((*source++ == '>'), XML_SYNTAX_ERROR, "XML is not well formed. Missing '>' after '/'.\n");
        ASSERT_RESULT(validate_attributes(element, occurrence));
        *input = source;
        return validate_empty_element(element);
      }

    case '>':
      {
        ASSERT_RESULT(validate_attributes(element, occurrence));
        source++;
        if(element->Child_Quantity > 0)
        {
          *input = source;
          ASSERT_RESULT(parse_parent_element(element, input, target CONTEXT_ARG));
          source = *input;
        }
        else if(element->Content.Type != EN_NO_XML_DATA_TYPE)
        {
          const char* const tag = source;
          source = strchr(source, '<');
          ASSERT((source != NULL), XML_INCOMPLETE_SOURCE, "Incomplete XML source. Missing end tag of element '%s'\n",
                 element->Name.String);
          size_t length = source++ - tag;

          ASSERT_RESULT(extract_content(&element->Content, target, tag, length));
          ASSERT(*source++ == '/', XML_SYNTAX_ERROR, "XML is not well formed. Missing '/' after '<'.\n");
        }
        else
        {
          ASSERT_TOKEN(source, '<');
          ASSERT(*source++ == '/', XML_SYNTAX_ERROR, "XML is not well formed.Missing '/' after '<'.\n");
        }
        *input = source;
        return validate_element(element, input);
      }
    }

    ASSERT(attribute_occurred != element->Attribute_Quantity, XML_SYNTAX_ERROR,
           "XML is not well formed. Missing end tag of element '%s'.\n", element->Name.String);

    const char* const tag = source;
    source = get_attribute_tag(source);
    ASSERT(source != NULL, XML_INCOMPLETE_SOURCE, "Incomplete XML source. Missing element '%s' attribute\n",
           element->Name.String);
    size_t length = source - tag;
    uint32_t i = 0;
    while(1)
    {
      if((length == element->Attribute[i].Name.Length) &&
         (strncmp(tag, element->Attribute[i].Name.String, length) == 0))
      {
        ASSERT(!occurrence[i], XML_DUPLICATE_ATTRIBUTE,
               "Duplicate attribute '%s' found in the element '%s'.\n",
               element->Attribute[i].Name.String, element->Name.String);

        occurrence[i] = true;
        *input = source;
        ASSERT_RESULT(parse_attribute(&element->Attribute[i], input, target CONTEXT_ARG));
        source = *input;
        attribute_occurred++;
        break;
      }

      ASSERT(++i < element->Attribute_Quantity, XML_ATTRIBUTE_NOT_FOUND,
             "Undefined attribute tag '%.*s' in the element '%s'\n", (int)length, tag, element->Name.String);
    }
  }
}

/** \brief Parses a XML source to extract child elements from parent element
 * as specified in the xs_element_t schema.
 *
 * \param parent const xs_element_t*const : : Schema of parent element
 * \param input const char** : input XML string to parse
 * \param parent_target void* : Target address of parent element. It is used for
 * for calculating the target address of child element if it's address type is relative.
 * \param context void* : User specified context.
 * \return xml_parse_result_t : result of parsing.
 */
static inline xml_parse_result_t parse_parent_element(const xs_element_t* const parent,
                                                      const char** input,
                                                      void* parent_target
                                                      CONTEXT_PTR)
{
  uint32_t occurrence[parent->Child_Quantity];
  uint32_t element_index = 0;
  const char* source = *input;

  for(uint32_t i = 0; i < parent->Child_Quantity; i++)
  {
    occurrence[i] = 0;
  }

  while(1)
  {
    ASSERT_TOKEN(source, '<');

    switch(*source)
    {
    case '?':
      source = strstr(source, "?>");
      ASSERT(source != NULL, XML_INCOMPLETE_SOURCE, "Incomplete XML source. Missing \"?>\".\n");
      source += 2;
      continue;

    case '!':
      source = strstr(source, "-->");
      ASSERT(source != NULL, XML_INCOMPLETE_SOURCE, "Incomplete XML source. Missing end of comment \"-->\".\n");
      source += (sizeof("-->") - 1);
      continue;

    case '/':
      for(uint32_t i = 0; i < parent->Child_Quantity; i++)
      {
        ASSERT(occurrence[i] >= parent->Child[i].MinOccur, XML_ELEMENT_MIN_OCCURRENCE_ERR,
               "XML element '%s' occurred less than specified count %d in the schema\n",
               parent->Child[i].Name.String, parent->Child[i].MinOccur);
      }
      *input = ++source;
      return XML_PARSE_SUCCESS;
    }

    const char* const tag = source;
    source = get_element_end_tag(source);
    ASSERT(source != NULL, XML_INCOMPLETE_SOURCE, "Incomplete XML source. Missing end tag (/>) of element '%s'.\n",
           parent->Name.String);
    size_t length = source - tag;

    if(parent->Child_Order != EN_SEQUENCE)
    {
      element_index = 0;
    }
    while(1)
    {
      if((length == parent->Child[element_index].Name.Length) &&
        (strncmp(tag, parent->Child[element_index].Name.String, length) == 0))
      {
        break;
      }

      if(parent->Child_Order == EN_SEQUENCE)
      {
        ASSERT(occurrence[element_index] >= parent->Child[element_index].MinOccur,
                XML_ELEMENT_MIN_OCCURRENCE_ERR,
                "XML element '%s' occurred less than specified count %d in the schema\n",
                parent->Child[element_index].Name.String, parent->Child[element_index].MinOccur);
      }
      ASSERT(++element_index < parent->Child_Quantity, XML_ELEMENT_NOT_FOUND_ERR,
             "Undefined XML element '%.*s'\n", (int)length, tag);
    }

    const xs_element_t* const element = &parent->Child[element_index];
    void* target = get_target_address(&element->Target, parent_target,
                                      occurrence[element_index] CONTEXT_ARG);

    ASSERT(++occurrence[element_index] <= element->MaxOccur, XML_ELEMENT_MAX_OCCURRENCE_ERR,
           "XML element '%s' occurred more than specified count %d in the schema.\n",
           element->Name.String, element->MaxOccur);

    if(parent->Child_Order == EN_CHOICE)
    {
      ASSERT(validate_choice_order(occurrence, parent->Child_Quantity), XML_CHOICE_ELEMENT_ERR,
             "XML element '%s' of type choice contains more than one child element\n", parent->Name.String);
    }

    *input = source;
    ASSERT_RESULT(parse_element(element, input, target CONTEXT_ARG));

  #if XML_PARSER_CALLBACK
    if(element->Callback)
    {
      element->Callback(occurrence[element_index], target CONTEXT_ARG);
    }
  #endif // XML_PARSER_CALLBACK

    if(parent->Name.String == NULL)
    {
      return XML_PARSE_SUCCESS;
    }
    source = *input;
  }
}

xml_parse_result_t parse_xml(const xs_element_t* root, const char* source, void* const target
                             CONTEXT_PTR)
{
  const xs_element_t parent =
  {
      .Child_Quantity = 1,
      .Child_Order    = EN_CHOICE,
      .Child          = root,
  };
  return parse_parent_element(&parent, &source, target CONTEXT_ARG);
}
