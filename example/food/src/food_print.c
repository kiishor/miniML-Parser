#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "food.h"

void print_food(void)
{
    printf("name: %s\n", waffle.name);
    printf("price: %f\n", waffle.price);
    printf("description: %s\n", waffle.description);
    printf("calories: %u\n", waffle.calories);
}
