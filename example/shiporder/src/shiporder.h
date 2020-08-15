#ifndef shiporder_H_INCLUDED
#define shiporder_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#include "libs/parse_xml.h"

typedef struct item_t item_t;

struct item_t
{
    char* title;
    char* note;
    uint32_t quantity;
    float price;
    item_t* Next;
};

typedef struct
{
    char* name;
    char* address;
    char* city;
    char* country;
}shipto_t;

typedef struct
{
    char* orderperson;
    shipto_t shipto;
    item_t* item;
    char* orderid;
}shiporder_t;

extern shiporder_t shiporder;

extern const xs_element_t xml_root;

#endif
