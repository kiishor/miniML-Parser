#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "shiporder.h"

void print_shiporder(size_t itemQuantity)
{
    printf("orderid: %s\n", shiporder.orderid);
    printf("orderperson: %s\n", shiporder.orderperson);
    printf("name: %s\n", shiporder.shipto.name);
    printf("address: %s\n", shiporder.shipto.address);
    printf("city: %s\n", shiporder.shipto.city);
    printf("country: %s\n", shiporder.shipto.country);

    for(uint32_t i = 0; i < itemQuantity; i++)
    {
        printf("title: %s\n", shiporder.item[i].title);
        printf("note: %s\n", shiporder.item[i].note);
        printf("quantity: %u\n", shiporder.item[i].quantity);
        printf("price: %f\n", shiporder.item[i].price);
    }
}
