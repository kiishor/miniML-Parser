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
//! Assert the expression. if fails write error to the result and returns NULL.
#define ASSERT1(expression, result, error)        \
{                                                 \
  if(!(expression))                               \
  {                                               \
    result = error;                               \
    return NULL;                                  \
  }                                               \
}

//! Assert the expression. If fails returns NULL.
#define ASSERT2(expression) if(!(expression)) return NULL

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

/** \brief This function skips/ignore all the whitespace chars
 *         till it reaches to non-whitespace char or end of string.
 *
 * \param source const char* Source : string to skip whitespace
 * \return const char*  Address of first non-whitespace char in the string or
 *                      NULL on end of string
 *
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
 *
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
 *
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
 *
 */
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

/** \brief Parses XML attribute and extracts the content of attribute
 *
 * \param attribute const xs_attribute_t*const : Structure defining XML attribute to parse.
 * \param source const char* : Source XML to parse and extract the content of attribute
 * \param target void* : Target address to store XMl content
 * \param result xml_parse_result_t* const : pointer to store the result of parsing.
 * \param context void** : User defined context.
 * \return const char* : XML source string.
 *         If parsing is successful it next character after the parsed attribute.
 *         Otherwise NULL.
 *
 */
static inline const char* parse_attribute(const xs_attribute_t* const attribute,
                                          const char* source, void* target,
                                          xml_parse_result_t* const result,
                                          void** context)
{
  source = skip_whitespace(source);
  ASSERT1((source != NULL), *result, XML_INCOMPLETE_SOURCE);
  ASSERT1((*source++ == '='), *result, XML_SYNTAX_ERROR);

  source = skip_whitespace(source);
  ASSERT1((source != NULL), *result, XML_INCOMPLETE_SOURCE);
  ASSERT1((*source++ == '"'), *result, XML_SYNTAX_ERROR);

  const char* const tag = source;
  source = strchr(source, '"');
  ASSERT1((source != NULL), *result, XML_INCOMPLETE_SOURCE);
  size_t length = source++ - tag;

  *result = extract_content(&attribute->Content,
                   get_target_address(&attribute->Target, target, 0, context),
                   tag, length);

  ASSERT2(*result == XML_PARSE_SUCCESS);
  return source;
}

/** \brief Verify that all the required attributes occurred in the XML element.
 *
 * \param element const xs_element_t* const : XML element to verify
 * \param occurrence const bool*const : Occurrence table of attribute
 * \return xml_parse_result_t result of validation.
 *
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
 *
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

/** \brief validate the XML element. Check for matching end element tag.
 *
 * \param element const xs_element_t* const : element to validate.
 * \param source const char* : XML source string.
 * \param result xml_parse_result_t* const : stores the result of element validation
 * \return const char* : if validation is successful return XML source string otherwise NULL.
 *
 */
static inline const char* validate_element(const xs_element_t* const element,
                                           const char* source,
                                           xml_parse_result_t* const result)
{
  const char* const tag = source;
  source = get_element_end_tag(source);
  ASSERT1((source != NULL), *result, XML_INCOMPLETE_SOURCE);
  size_t length = source - tag;

  ASSERT1((length == element->Name.Length) &&
     (strncmp(element->Name.String, tag, length) == 0), *result, XML_END_TAG_NOT_FOUND)

  source = skip_whitespace(source);
  ASSERT1((source != NULL), *result, XML_INCOMPLETE_SOURCE);
  ASSERT1((*source++ == '>'), *result, XML_SYNTAX_ERROR);
  return source;
}

/** \brief parses a XML string to extract attribute and its content as specified
 *         in the given xs_element_t
 *
 * \param element const xs_element_t*const : Contain schema definition of an element.
 * \param source const char* : XML source to parse
 * \param target void* : Target address to store content.
 * \param result xml_parse_result_t* const : pointer to write the result of parsing.
 * \param context void** : User specified context.
 * \return const char* : returns XML source if parsing is successful otherwise NULL.
 *
 */
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
    ASSERT1((source != NULL), *result, XML_INCOMPLETE_SOURCE);

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
        const char* const tag = source;
        source = strchr(source, '<');
        ASSERT1((source != NULL), *result, XML_INCOMPLETE_SOURCE);
        size_t length = source++ - tag;

        *result = extract_content(&element->Content, target, tag, length);
        ASSERT2(*result == XML_PARSE_SUCCESS);
        ASSERT1(*source++ == '/', *result, XML_SYNTAX_ERROR);
      }
      else
      {
        source = skip_whitespace(source);
        ASSERT1(source != NULL, *result, XML_INCOMPLETE_SOURCE);
        ASSERT1(*source++ == '<', *result, XML_SYNTAX_ERROR);
        ASSERT1(*source++ == '/', *result, XML_SYNTAX_ERROR);
      }
      return validate_element(element, source, result);
    }

    ASSERT1(attribute_occurred != element->Attribute_Quantity, *result, XML_SYNTAX_ERROR);

    const char* const tag = source;
    source = get_attribute_tag(source);
    ASSERT1(source != NULL, *result, XML_INCOMPLETE_SOURCE);
    size_t length = source - tag;
    uint32_t i = 0;
    while(1)
    {
      if((length == element->Attribute[i].Name.Length) &&
         (strncmp(tag, element->Attribute[i].Name.String, length) == 0))
      {
        ASSERT1(!occurrence[i], *result, XML_DUPLICATE_ATTRIBUTE);

        occurrence[i] = true;
        source = parse_attribute(&element->Attribute[i], source, target, result, context);
        ASSERT2(source != NULL);
        attribute_occurred++;
        break;
      }

      ASSERT1(++i < element->Attribute_Quantity, *result, XML_ATTRIBUTE_NOT_FOUND);
    }
  }
}

/** \brief Validate that in case of child element order is specified as choice then
 * only element must contain only one child element.(Though it can have multiple
 * occurrence of that element as specified in the maxOccur)
 *
 * \param occurrence uint32_t* : pointer to array of occurrence table
 * \param quantity size_t : Number of child elements
 * \return bool : return true if only on child element occurred otherwise false.
 *
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

/** \brief Parses a XML source to extract child elements from parent element
 * as specified in the xs_element_t schema.
 *
 * \param parent const xs_element_t*const : Schema of parent element
 * \param source const char* : XML source to parse
 * \param parent_target void* : Target address of parent element. It is used for
 * for calculating the target address of child element if it's address type is relative.
 * \param result xml_parse_result_t* const : Stores the result of parsing.
 * \param context void** : User specified context.
 * \return const char* : returns pointer XML source if parsing is successful otherwise NULL.
 *
 */
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
    ASSERT1(source != NULL, *result, XML_INCOMPLETE_SOURCE);
    ASSERT1(*source++ == '<', *result, XML_INVALID_START_TOKEN_ERR);

    switch(*source)
    {
    case '?':
    case '!':
      source = strchr(source, '>');
      ASSERT1(source != NULL, *result, XML_INCOMPLETE_SOURCE);
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
    source = get_element_end_tag(source);
    ASSERT1(source != NULL, *result, XML_INCOMPLETE_SOURCE);
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
        ASSERT1(occurrence[element_index] >= parent->Child[element_index].MinOccur,
                *result, XML_ELEMENT_MIN_OCCURRENCE_ERR);
      }

      ASSERT1(++element_index < parent->Child_Quantity, *result, XML_ELEMENT_NOT_FOUND_ERR);
    }

    const xs_element_t* const element = &parent->Child[element_index];
    void* target = get_target_address(&element->Target, parent_target,
                                      occurrence[element_index], context);

    ASSERT1(++occurrence[element_index] <= element->MaxOccur, *result, XML_ELEMENT_MAX_OCCURRENCE_ERR)
    if(parent->Child_Order == EN_CHOICE)
    {
      ASSERT1(validate_choice_order(occurrence, parent->Child_Quantity), *result, XML_CHOICE_ELEMENT_ERR);
    }

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
