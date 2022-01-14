/**
 * \file
 * \brief   Simple XML parser demo

 * \author  Nandkishor Biradar
 * \date    18 April 2021

 *  Copyright (c) 2019-2022 Nandkishor Biradar
 *  https://github.com/kiishor

 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 */


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
