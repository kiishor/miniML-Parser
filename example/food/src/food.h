#ifndef food_H_INCLUDED
#define food_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#include "parse_xml.h"

typedef struct
{
    char* name;
    float price;
    char* description;
    uint32_t calories;
}food_t;

extern food_t food;

extern const xs_element_t xml_root;

#endif
