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

#include "parse_xml.h"

/*
 *  ------------------------------- DEFINITION -------------------------------
 */
//! Assert the expression.
#define ASSERT(expression, error)   \
do {                                \
  if(!(expression))                 \
  {                                 \
    return error;                   \
  }                                 \
}while(0)

//! Assert the result of function.
#define ASSERT_RESULT(function)           \
do {                                      \
  xml_parse_result_t result = function;   \
  if(result != XML_PARSE_SUCCESS)         \
  {                                       \
    return result;                        \
  }                                       \
}while(0)

/*
 *	--------------------------- FORWARD DECLARATION ---------------------------
 */

static inline xml_parse_result_t parse_parent_element(const xs_element_t* const parent,
                                                      const char** input,
                                                      void* parent_target,
                                                      void** context);

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
 * \param context void** : user defined context. used only in dynamic type of target address.
 * \return void* : Returns the target address to store XML content.
 */
static inline void* get_target_address(const target_address_t* const address,
                                       void* target, uint32_t occurrence,
                                       void** context)
{
  switch(address->Type)
  {
  case EN_STATIC:
    return (void*)((uint32_t)(address->Address) + (occurrence * address->Size));

  case EN_DYNAMIC:
    return address->Allocate(occurrence, context);

  case EN_RELATIVE:
    return (void*)((uint32_t)(target) + (occurrence * address->Size) + address->Offset);

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
    if(!occurrence[i] && element->Attribute[i].Use == EN_REQUIRED)
    {
      return XML_ATTRIBUTE_NOT_FOUND;
    }
  }
  return XML_PARSE_SUCCESS;
}

/** \brief Validate the empty element.
 *
 * \param element const xs_element_t*const : element to validate.
 * \return xml_parse_result_t result of validation.
 */
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

/** \brief Validate that in case of child element order is specified as choice then
 * only element must contain only one child element.(Though it can have multiple
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
  ASSERT((source != NULL), XML_INCOMPLETE_SOURCE);
  size_t length = source - tag;

  ASSERT((length == element->Name.Length) &&
     (strncmp(element->Name.String, tag, length) == 0), XML_END_TAG_NOT_FOUND);

  source = skip_whitespace(source);
  ASSERT((source != NULL), XML_INCOMPLETE_SOURCE);
  ASSERT((*source++ == '>'), XML_SYNTAX_ERROR);
  *input = source;
  return XML_PARSE_SUCCESS;
}

/** \brief Parses XML attribute and extracts the content of attribute
 *
 * \param attribute const xs_attribute_t*const : Structure defining XML attribute to parse.
 * \param input const char** : input XML string to parse and extract the content of attribute
 * \param target void* : Target address to store XMl content
 * \param context void** : User defined context.
 * \return xml_parse_result_t result of parsing
 */
static inline xml_parse_result_t parse_attribute(const xs_attribute_t* const attribute,
                                                 const char** input, void* target,
                                                 void** context)
{
  const char* source = skip_whitespace(*input);
  ASSERT((source != NULL), XML_INCOMPLETE_SOURCE);
  ASSERT((*source++ == '='), XML_SYNTAX_ERROR);

  source = skip_whitespace(source);
  ASSERT((source != NULL), XML_INCOMPLETE_SOURCE);
  ASSERT((*source++ == '"'), XML_SYNTAX_ERROR);

  const char* const tag = source;
  source = strchr(source, '"');
  ASSERT((source != NULL), XML_INCOMPLETE_SOURCE);
  size_t length = source++ - tag;
  *input = source;
  return extract_content(&attribute->Content,
                   get_target_address(&attribute->Target, target, 0, context),
                   tag, length);
}

/** \brief parses a XML string to extract attribute and its content as specified
 *         in the given xs_element_t
 *
 * \param element const xs_element_t*const : Contain schema definition of an element.
 * \param input const char** : Input XML string to parse
 * \param target void* : Target address to store XML content.
 * \param context void** : User specified context.
 * \return xml_parse_result_t : result of parsing.
 */
static inline xml_parse_result_t parse_element(const xs_element_t* const element,
                                               const char** input, void* target,
                                               void** context)
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
    ASSERT((source != NULL), XML_INCOMPLETE_SOURCE);

    switch(*source)
    {
    case '/':
      {
        source++;
        ASSERT((*source++ == '>'), XML_SYNTAX_ERROR);
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
          ASSERT_RESULT(parse_parent_element(element, input, target, context));
          source = *input;
        }
        else if(element->Content.Type != EN_NO_XML_DATA_TYPE)
        {
          const char* const tag = source;
          source = strchr(source, '<');
          ASSERT((source != NULL), XML_INCOMPLETE_SOURCE);
          size_t length = source++ - tag;

          ASSERT_RESULT(extract_content(&element->Content, target, tag, length));
          ASSERT(*source++ == '/', XML_SYNTAX_ERROR);
        }
        else
        {
          source = skip_whitespace(source);
          ASSERT(source != NULL, XML_INCOMPLETE_SOURCE);
          ASSERT(*source++ == '<', XML_SYNTAX_ERROR);
          ASSERT(*source++ == '/', XML_SYNTAX_ERROR);
        }
        *input = source;
        return validate_element(element, input);
      }
    }

    ASSERT(attribute_occurred != element->Attribute_Quantity, XML_SYNTAX_ERROR);

    const char* const tag = source;
    source = get_attribute_tag(source);
    ASSERT(source != NULL, XML_INCOMPLETE_SOURCE);
    size_t length = source - tag;
    uint32_t i = 0;
    while(1)
    {
      if((length == element->Attribute[i].Name.Length) &&
         (strncmp(tag, element->Attribute[i].Name.String, length) == 0))
      {
        ASSERT(!occurrence[i], XML_DUPLICATE_ATTRIBUTE);

        occurrence[i] = true;
        *input = source;
        ASSERT_RESULT(parse_attribute(&element->Attribute[i], input, target, context));
        source = *input;
        attribute_occurred++;
        break;
      }

      ASSERT(++i < element->Attribute_Quantity, XML_ATTRIBUTE_NOT_FOUND);
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
 * \param context void** : User specified context.
 * \return xml_parse_result_t : result of parsing.
 */
static inline xml_parse_result_t parse_parent_element(const xs_element_t* const parent,
                                                      const char** input,
                                                      void* parent_target,
                                                      void** context)
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
    source = skip_whitespace(source);
    ASSERT(source != NULL, XML_INCOMPLETE_SOURCE);
    ASSERT(*source++ == '<', XML_INVALID_START_TOKEN_ERR);

    switch(*source)
    {
    case '?':
    case '!':
      source = strchr(source, '>');
      ASSERT(source != NULL, XML_INCOMPLETE_SOURCE);
      source++;
      continue;

    case '/':
      for(uint32_t i = 0; i < parent->Child_Quantity; i++)
      {
        ASSERT(occurrence[i] >= parent->Child[i].MinOccur, XML_ELEMENT_MIN_OCCURRENCE_ERR);
      }
      *input = ++source;
      return XML_PARSE_SUCCESS;
    }

    const char* const tag = source;
    source = get_element_end_tag(source);
    ASSERT(source != NULL, XML_INCOMPLETE_SOURCE);
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
                XML_ELEMENT_MIN_OCCURRENCE_ERR);
      }
      ASSERT(++element_index < parent->Child_Quantity, XML_ELEMENT_NOT_FOUND_ERR);
    }

    const xs_element_t* const element = &parent->Child[element_index];
    void* target = get_target_address(&element->Target, parent_target,
                                      occurrence[element_index], context);

    ASSERT(++occurrence[element_index] <= element->MaxOccur, XML_ELEMENT_MAX_OCCURRENCE_ERR);
    if(parent->Child_Order == EN_CHOICE)
    {
      ASSERT(validate_choice_order(occurrence, parent->Child_Quantity), XML_CHOICE_ELEMENT_ERR);
    }

    *input = source;
    ASSERT_RESULT(parse_element(element, input, target, context));

    if(element->Callback)
    {
      element->Callback(occurrence[element_index], target, context);
    }
    if(parent->Name.String == NULL)
    {
      return XML_PARSE_SUCCESS;
    }
    source = *input;
  }
}

#ifndef PARTIAL_PARSING
xml_parse_result_t parse_xml(const xs_element_t* root, const char* source, void** context)
{
  return parse_parent_element(root, &source, NULL, context);
}

#else

xml_parse_result_t parse_xml(const xs_element_t* root, const xml_source_t* source, void** context)
{
  return parse_parent_element(root, (const char**)source, NULL, context);
}
#endif // PARTIAL_PARSING
