#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "shiporder.h"

void print_shiporder(void)
{
    printf("orderid: %s\n", shiporder.orderid);
    printf("orderperson: %s\n", shiporder.orderperson);
    printf("name: %s\n", shiporder.shipto.name);
    printf("address: %s\n", shiporder.shipto.address);
    printf("city: %s\n", shiporder.shipto.city);
    printf("country: %s\n", shiporder.shipto.country);
}
