#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "food.h"

void print_food(void)
{
    printf("name: %s\n", food.name);
    printf("price: %f\n", food.price);
    printf("description: %s\n", food.description);
    printf("calories: %u\n", food.calories);
}
