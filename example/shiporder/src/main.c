/**
 * \file
 * \brief   Tests the XML parser library

 * \author  Nandkishor Biradar
 * \date    14 September 2019
 */

/*
 *  ------------------------------ INCLUDE FILES ------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "shiporder.h"

/*
 *  ------------------------------ FUNCTION BODY ------------------------------
 */
extern void print_shiporder(void);

int main(int argc, char *argv[])
{

  if(argc < 2)
  {
    printf("Please enter the XML file name\n");
    return 1;
  }

  FILE* fXml = fopen(argv[1], "r");
  if(fXml == NULL )
  {
    printf("Error in opening XML file\n");
    return 2;
  }

  fseek(fXml, 0, SEEK_END);
  long size = ftell(fXml);
  assert(size > 0);
  fseek(fXml, 0, SEEK_SET);

  char* const xml = malloc((size_t)size);
  fread(xml, 1, (size_t)size, fXml);
  fclose(fXml);

  xml_parse_result_t result = parse_xml(&xml_root, xml, NULL);
  if(result == XML_PARSE_SUCCESS)
  {
    printf("Parsing completed successfully\n");
    print_shiporder();
  }
  else
  {
    printf("Failed to parse XML file\n");
  }

  free(xml);

  return 0;
}


