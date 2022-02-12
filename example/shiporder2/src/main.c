/**
 * \file
 * \brief   Parsing of an XML data using relative addressing and dynamic addressing method.

 * \author  Nandkishor Biradar
 * \date    14 September 2019

 *  Copyright (c) 2019-2022 Nandkishor Biradar
 *  https://github.com/kiishor

 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 */

/*
 *  ------------------------------ INCLUDE FILES ------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "shiporder.h"

/*
 *  ------------------------------ GLOBAL VARIABLES ------------------------------
 */

#define ADD_RESULT_CODE(code, description) [code] = #description,
const char* Result_Text[] = {XML_PARSER_RESULT};
#undef ADD_RESULT_CODE

/*
 *  ------------------------------ FUNCTION BODY ------------------------------
 */
extern void print_shiporder(shiporder_t* const pShipOrder);

//! returns dynamically allocated memory/address to store content of "item" XML element
void* allocate_item(uint32_t occurrence)
{
  return calloc(sizeof(item_t), 1);
}

//! Deallocates dynamically allocated memory for "item" XML element
void deallocate_item(uint32_t occurrence, void* const content)
{
  printf("*****\titem: %d\t*****\n", occurrence);
  item_t* item = (item_t*)content;
  printf("title: %s\n", item->title);
  printf("note: %s\n", item->note);
  printf("quantity: %u\n", item->quantity);
  printf("price: %f\n\n", item->price);

  free(item);
}

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

  // Get the size of XML file to allocate the memory
  fseek(fXml, 0, SEEK_END);
  size_t size = ftell(fXml);
  fseek(fXml, 0, SEEK_SET);

// Allocate the memory to copy XML source from file.
  char* const xml = malloc(size);
  fread(xml, 1, size, fXml);
  fclose(fXml);

  shiporder_t book;   // Holds the extracted content of XML file.
  xml_parse_result_t result = parse_xml(&shiporder_root, xml, &book);
  printf("%s\n", Result_Text[result]);
  if(result == XML_PARSE_SUCCESS)
  {
    print_shiporder(&book);
  }

  free(xml);

  return 0;
}


