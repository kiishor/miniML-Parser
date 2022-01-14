/**
 * \file
 * \brief   Parsing of an XML data using relative addressing method.

 * \author  Nandkishor Biradar
 * \date    14 September 2019

 *  Copyright (c) 2019-2022 Nandkishor Biradar
 *  https://github.com/kiishor

 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "shiporder.h"

void print_shiporder(shiporder_t* const pShipOrder, size_t itemQuantity)
{
    printf("orderid: %s\n", pShipOrder->orderid);
    printf("orderperson: %s\n", pShipOrder->orderperson);
    printf("name: %s\n", pShipOrder->shipto.name);
    printf("address: %s\n", pShipOrder->shipto.address);
    printf("city: %s\n", pShipOrder->shipto.city);
    printf("country: %s\n", pShipOrder->shipto.country);

    for(uint32_t i = 0; i < itemQuantity; i++)
    {
        printf("title: %s\n", pShipOrder->item[i].title);
        printf("note: %s\n", pShipOrder->item[i].note);
        printf("quantity: %u\n", pShipOrder->item[i].quantity);
        printf("price: %f\n", pShipOrder->item[i].price);
    }
}
