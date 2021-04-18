/**
 * \file
 * \brief   Tests XML parser library

 * \author  Nandkishor Biradar
 * \date    18 April 2021
 */

/*
 *  ------------------------------ INCLUDE FILES ------------------------------
 */

#include <stdio.h>

#include "food.h"

const char xml_str[] =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<food xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
"      xsi:noNamespaceSchemaLocation=\"food.xsd\">"
"    <name>Belgian Waffles</name>"
"    <price>5.95</price>"
"    <description>Two of our famous Belgian Waffles with plenty of real maple syrup</description>"
"    <calories>650</calories>"
"</food>";

/*
 *  ------------------------------ FUNCTION BODY ------------------------------
 */
extern void print_food(void);

int main(void)
{
  xml_parse_result_t result = parse_xml(&xml_root, xml_str);
  if(result == XML_PARSE_SUCCESS)
  {
    printf("Parsing completed successfully\n");
    print_food();
  }
  else
  {
    printf("Failed to parse XML file\n");
  }

  return 0;
}


