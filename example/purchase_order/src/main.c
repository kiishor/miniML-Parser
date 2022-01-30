/**
 * \file
 * \brief   Parsing of an XML data using relative addressing method.

 * \author  Nandkishor Biradar
 * \date    30 january 2022

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
#include <assert.h>

#include "PurchaseOrder.h"

/*
 *  ------------------------------ FUNCTION BODY ------------------------------
 */
extern void print_purchase_order(PurchaseOrder_t* const pPurchaseOrder, size_t shipTo_Quantity);


/** \brief Callback function to save number of times "ShipTo" element occurred in the XML
 *
 * \param occurrence uint32_t   Number of times element occurred in the XML
 * \param content void* const   Content of an element
 * \param context void*         User defined context
 *
 */
void shipToCallback(uint32_t occurrence, void* const content, void* context)
{
  uint32_t* shipToQuantity = context;
  *shipToQuantity = occurrence;
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
  long size = ftell(fXml);
  assert(size > 0);
  fseek(fXml, 0, SEEK_SET);

// Allocate the memory to copy XML source from file.
  char* const xml = malloc((size_t)size);
  fread(xml, 1, (size_t)size, fXml);
  fclose(fXml);

  PurchaseOrder_t purchaseOrder;   // Holds the extracted content of XML file.

  uint32_t shipTo_Quantity = 0;  // Variable that holds the occurrence of "ShipTo" element in the XML
  xml_parse_result_t result = parse_xml(&PurchaseOrder_root, xml, &purchaseOrder, &shipTo_Quantity);
  if(result == XML_PARSE_SUCCESS)
  {
    printf("Parsing completed successfully\n");
    print_purchase_order(&purchaseOrder, shipTo_Quantity);
  }
  else
  {
    printf("Failed to parse XML file\n");
  }

  free(xml);

  return 0;
}


