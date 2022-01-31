/**
 * \file
 * \brief   Parsing of an XML data using relative addressing method.

 * \author  Nandkishor Biradar
 * \date    30 January 2022

 *  Copyright (c) 2019-2022 Nandkishor Biradar
 *  https://github.com/kiishor

 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "PurchaseOrder.h"

void print_purchase_order(PurchaseOrder_t* const pPurchaseOrder, size_t shipTo_Quantity)
{
    printf("OrderDate: %d/%d/%d\n", pPurchaseOrder->OrderDate.Day, pPurchaseOrder->OrderDate.Month, pPurchaseOrder->OrderDate.Year);

    for(uint32_t i = 0; i < shipTo_Quantity; i++)
    {
      printf("name: %s\n", pPurchaseOrder->ShipTo[i].name);
      printf("street: %s\n", pPurchaseOrder->ShipTo[i].street);
      printf("city: %s\n", pPurchaseOrder->ShipTo[i].city);
      printf("state: %s\n", pPurchaseOrder->ShipTo[i].state);
      printf("zip: %d\n", pPurchaseOrder->ShipTo[i].zip);
    }
    printf("name: %s\n", pPurchaseOrder->BillTo.name);
    printf("street: %s\n", pPurchaseOrder->BillTo.street);
    printf("city: %s\n", pPurchaseOrder->BillTo.city);
    printf("state: %s\n", pPurchaseOrder->BillTo.state);
    printf("zip: %d\n", pPurchaseOrder->BillTo.zip);

}
