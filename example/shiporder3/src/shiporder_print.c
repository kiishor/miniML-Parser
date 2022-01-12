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

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "shiporder.h"

void print_shiporder(shiporder_t* const pShiporder)
{
    printf("orderid: %s\n", pShiporder->orderid);
    printf("orderperson: %s\n", pShiporder->orderperson);
    printf("name: %s\n", pShiporder->shipto.name);
    printf("address: %s\n", pShiporder->shipto.address);
    printf("city: %s\n", pShiporder->shipto.city);
    printf("country: %s\n", pShiporder->shipto.country);

    const item_t* item = pShiporder->item;
    while(item != NULL)
    {
        printf("title: %s\n", item->title);
        printf("note: %s\n", item->note);
        printf("quantity: %u\n", item->quantity);
        printf("price: %f\n", item->price);

        item = item->Next;
    }
}
