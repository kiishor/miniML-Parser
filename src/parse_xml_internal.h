#ifndef PARSE_XML_INTERNAL_H
#define PARSE_XML_INTERNAL_H

/**
 * \file
 * \brief Internal include file for XML parser

 * \author  Nandkishor Biradar
 * \date    03 March 2022

 *  Copyright (c) 2019-2022 Nandkishor Biradar
 *  https://github.com/kiishor

 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 */

 /*
 *  ------------------------------ INCLUDE FILES ------------------------------
 */

#if XML_PARSER_DEBUG
#include "stdio.h"
#endif // XML_PARSER_DEBUG

 /*
 *  ------------------------------- DEFINITION -------------------------------
 */

 //! Assert the expression.
#define ASSERT(expression, error, ...)   \
do {                                \
  if(!(expression))                 \
  {                                 \
    printf(__VA_ARGS__);            \
    return error;                   \
  }                                 \
}while(0)

#endif // PARSE_XML_INTERNAL_H

