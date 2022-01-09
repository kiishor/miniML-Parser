#ifndef food_H_INCLUDED
#define food_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#include "parse_xml.h"

//! Structure to hold XML content
typedef struct
{
    char* name;           //!< Holds content of name XML element
    float price;          //!< Holds content of price XML element
    char* description;    //!< Holds content of description XML element
    uint32_t calories;    //!< Holds content of calories XML element
}food_t;

extern food_t waffle;

extern const xs_element_t food_root;

#endif
