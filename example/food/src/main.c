/**
 * \file
 * \brief   Simple XML parser demo

 * \author  Nandkishor Biradar
 * \date    18 April 2021

 *  Copyright (c) 2019-2022 Nandkishor Biradar
 *  https://github.com/kiishor

 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 */

/*
 *  ------------------------------ INCLUDE FILES ------------------------------
 */

#include <stdio.h>

#include "food.h"

/*
 *  ------------------------------ GLOBAL VARIABLES ------------------------------
 */

//! Input XML file
static const char xml_str[] =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<food xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
"      xsi:noNamespaceSchemaLocation=\"food.xsd\">"
"    <name>Belgian Waffles</name>"
"    <price>5.95</price>"
"    <description>Two of our famous Belgian Waffles with plenty of real maple syrup</description>"
"    <calories>650</calories>"
"</food>";

#define ADD_RESULT_CODE(code, description) [code] = #description,
const char* Result_Text[] = {XML_PARSER_RESULT};
#undef ADD_RESULT_CODE

/*
 *  ------------------------------ FUNCTION BODY ------------------------------
 */
extern void print_food(void);

int main(void)
{
  xml_parse_result_t result = parse_xml(&food_root, xml_str, NULL);
  printf("%s\n", Result_Text[result]);
  if(result == XML_PARSE_SUCCESS)
  {
    print_food();
  }

  return 0;
}
