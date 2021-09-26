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
