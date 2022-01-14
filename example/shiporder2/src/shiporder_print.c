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

void print_shiporder(shiporder_t* const pShipOrder)
{
    printf("orderid: %s\n", pShipOrder->orderid);
    printf("orderperson: %s\n", pShipOrder->orderperson);
    printf("name: %s\n", pShipOrder->shipto.name);
    printf("address: %s\n", pShipOrder->shipto.address);
    printf("city: %s\n", pShipOrder->shipto.city);
    printf("country: %s\n", pShipOrder->shipto.country);
}
