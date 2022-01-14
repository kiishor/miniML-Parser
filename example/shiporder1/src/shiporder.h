#ifndef shiporder_H_INCLUDED
#define shiporder_H_INCLUDED

/**
 * \file
 * \brief   Parsing of an XML data using relative addressing method.

 * \author  Nandkishor Biradar
 * \date    14 September 2019

 *  Copyright (c) 2019-2022 Nandkishor Biradar
 *  https://github.com/kiishor

 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 */

#include <stdint.h>
#include <stdbool.h>

#include "parse_xml.h"

//! Structure that represents the data structure of "item" XML element
typedef struct
{
    char* title;        //!< Holds content of "title" XML element
    char* note;         //!< Holds content of "note" XML element
    uint32_t quantity;  //!< Holds content of "quantity" XML element
    float price;        //!< Holds content of "price" XML element
}item_t;

//! structure that represents the data structure of "shipto" XML element
typedef struct
{
    char* name;     //!< Holds content of "name" XML element
    char* address;  //!< Holds content of "address" XML element
    char* city;     //!< Holds content of "city" XML element
    char* country;  //!< Holds content of "country" XML element
}shipto_t;

//! Structure that represents the data structure of shiporder XML schema
typedef struct
{
    char* orderperson;    //!< Holds content of "orderperson" XML element
    shipto_t shipto;      //!< Holds content of "shipto" XML element
    item_t item[10];      //!< Holds content of "item" XML element
    char* orderid;        //!< Holds content of "orderid" attribute
}shiporder_t;

extern const xs_element_t shiporder_root;

#endif
