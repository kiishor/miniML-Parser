#ifndef shiporder_H_INCLUDED
#define shiporder_H_INCLUDED

/**
 * \file
 * \brief   Parsing of an XML data using relative addressing and dynamic addressing method.

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

typedef struct item_t item_t;

//! Structure to store content of item element.
struct item_t
{
    char* title;        //!< Holds content of title element
    char* note;         //!< Holds content of note element
    uint32_t quantity;  //!< Holds content of quantity element
    float price;        //!< Holds content of price element
    item_t* Next;       //!< Holds the address of next node (item) in the linked list.
};

//! Structure to store content of shipto element
typedef struct
{
    char* name;         //!< Holds content of name element
    char* address;      //!< Holds content of address element
    char* city;         //!< Holds content of city element
    char* country;      //!< Holds content of country element
}shipto_t;

//! Structure to store content of shiporder root element
typedef struct
{
    char* orderperson;  //!< Holds content of orderperson element
    shipto_t shipto;    //!< Structure to hold content of shipto element
    item_t* item;       //!< Pointer to linked list structure. Holds content of item element
    char* orderid;      //!< Holds content of orderid attribute
}shiporder_t;

// Tree structure of XML elements.
extern const xs_element_t shiporder_root;

#endif
