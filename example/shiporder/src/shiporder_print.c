#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "shiporder.h"

void print_shiporder(void)
{
    printf("orderperson: %s\n", shiporder.orderperson);
    printf("name: %s\n", shiporder.shipto.name);
    printf("address: %s\n", shiporder.shipto.address);
    printf("city: %s\n", shiporder.shipto.city);
    printf("country: %s\n", shiporder.shipto.country);

    const item_t* item = shiporder.item;
    while(item != NULL)
    {
        printf("title: %s\n", item->title);
        printf("note: %s\n", item->note);
        printf("quantity: %u\n", item->quantity);
        printf("price: %f\n", item->price);

        item = item->Next;
    }
}
