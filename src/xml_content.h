#ifndef XML_CONTENT_H
#define XML_CONTENT_H

/**
 * \file
 * \brief Datatype for XML content.

 * \author  Nandkishor Biradar
 * \date    19 August 2019

 *  Copyright (c) 2019-2022 Nandkishor Biradar
 *  https://github.com/kiishor

 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 */

/*
 *  ------------------------------ INCLUDE FILES ------------------------------
 */
#include <stdbool.h>

/*
 *  ------------------------------- DEFINITION -------------------------------
 */

#define ALL_XML_CONTENT_TYPES       \
  ADD_CONTENT(EN_NO_XML_DATA_TYPE, XML element does not hold the content)    \
  ADD_CONTENT(EN_STRING,           starting address of string from the imput xml buffer and length of content is copied to string_t)  \
  ADD_CONTENT(EN_STRING_DYNAMIC,   String will be dynamically allocated using malloc and the pointer is copied to the target) \
  ADD_CONTENT(EN_CHAR_ARRAY,       String will be directly copied to the target using memcpy)  \
  ADD_CONTENT(EN_ENUM_STRING,      Enumeration)          \
  ADD_CONTENT(EN_ENUM_INT,         Enumeration of integers. e.g. enum { red = 10, green = 50};) \
  ADD_CONTENT(EN_ENUM_UINT,        Enumeration of unsigned integers) \
  ADD_CONTENT(EN_DECIMAL,          Float data type)          \
  ADD_CONTENT(EN_DOUBLE,           Double data type)         \
  ADD_CONTENT(EN_INT8,             8-bit signed integer)     \
  ADD_CONTENT(EN_UINT8,            8-bit unsigned integer)   \
  ADD_CONTENT(EN_INT16,            16-bit signed integer)    \
  ADD_CONTENT(EN_UINT16,           16-bit unsigned integer)  \
  ADD_CONTENT(EN_INT32,            32-bit signed integer)    \
  ADD_CONTENT(EN_UINT32,           32-bit unsigned integer)  \
  ADD_CONTENT(EN_INT64,            64-bit signed integer)    \
  ADD_CONTENT(EN_UINT64,           64-bit unsigned integer)  \
  ADD_CONTENT(EN_BOOL,             Boolean data type)           \
  ADD_CONTENT(EN_DATE,             Date in "YYYY-MM-DD" format) \
  ADD_CONTENT(EN_TIME,             Time in "HH:MM:SS" format)   \
  ADD_CONTENT(EN_DATE_TIME,        Date and Time in "YYYY-MM-DD HH:MM:SS") \
  ADD_CONTENT(EN_DURATION,         Duration)

/*
 *  ------------------------------- ENUMERATION -------------------------------
 */

#define ADD_CONTENT(type, ...)    type,
//! basic data type of content of an element or attribute
typedef enum
{
  ALL_XML_CONTENT_TYPES
  TOTAL_XML_CONTENT_TYPES
}xml_content_type_t;
#undef ADD_CONTENT

/*
 *  -------------------------------- STRUCTURE --------------------------------
 */

//! Structure to hold xs:date data type
typedef struct
{
  uint32_t Year;    //!< holds year
  uint32_t Month;   //!< holds month of year
  uint32_t Day;     //!< Holds day of month
}xs_date_t;

//!< Structure to hold xs:time data type
typedef struct
{
  uint32_t Hour;    //!< hours
  uint32_t Minute;  //!< Minutes
  uint32_t Second;  //!< Seconds
}xs_time_t;

//! structure to hold Date Time data type
typedef struct
{
  xs_date_t Date; //!< Holds date
  xs_time_t Time; //!< Holds time
}xs_date_time_t;

//! structure to hold xs:duration data type
typedef struct
{
  //! Sign of duration. true indicates positive duration and false indicates negative duration.
  bool Sign;
  xs_date_time_t Period;  //!< Holds the duration in date and time format
}xs_duration_t;

//! Restriction or facet for uint32_t data type.
typedef struct
{
  uint32_t MinValue;  //!< Minimum acceptable value
  uint32_t MaxValue;  //!< Maximum acceptable value
}uint_facet_t;

//! Restriction or facet for int32_t data type
typedef struct
{
  int32_t MinValue;   //!< Minimum acceptable value
  int32_t MaxValue;   //!< Maximum acceptable value
}int_facet_t;

//! Restriction or facet for uint64_t data type.
typedef struct
{
  uint64_t MinValue;  //!< Minimum acceptable value
  uint64_t MaxValue;  //!< Maximum acceptable value
}ulong_facet_t;

//! Restriction or facet for int64_t data type
typedef struct
{
  int64_t MinValue;   //!< Minimum acceptable value
  int64_t MaxValue;   //!< Maximum acceptable value
}long_facet_t;

//! Restriction or facet for string data type
typedef struct
{
  uint32_t MinLength;   //!< Minimum required length of string
  uint32_t MaxLength;   //!< Maximum allowable length of string.
}string_facet_t;

//! Restriction or facet for float data type
typedef struct
{
  float MinValue;   //!< Minimum acceptable value
  float MaxValue;   //!< Maximum acceptable value
}decimal_facet_t;

//! Restriction or facet for double data type
typedef struct
{
  double MinValue;   //!< Minimum acceptable value
  double MaxValue;   //!< Maximum acceptable value
}double_facet_t;

//!< structure for enumeration facet
typedef struct
{
	const void *const List;   //!< Pointer to array of enumerations
	uint32_t Quantity;        //!< Quantity of enumeration values
}enum_facet_t;

//! Union holding all the restriction or facets
typedef union
{
  uint_facet_t Uint;        //!< Restriction or facet for uint32_t data type
  int_facet_t Int;          //!< Restriction or facet for int32_t data type
  string_facet_t String;    //!< Restriction or facet for string data type
  decimal_facet_t Decimal;  //!< Restriction or facet for float data type
  double_facet_t Double;    //!< Restriction or facet for double data type
  long_facet_t Long;        //!< Restriction or facet for in64_t data type
  ulong_facet_t Ulong;      //!< Restriction or facet for uint64_t data type
  enum_facet_t Enum;        //!< Enumeration facet
}facet_t;

//! Structure to define the XML content.
typedef struct
{
  xml_content_type_t Type;  //!< XML content type
  facet_t Facet;            //!< Facet of XML content
}xml_content_t;

/*
 *  ---------------------------- EXPORTED FUNCTION ----------------------------
 */

/** \brief Extract the content of element or attribute
 *
 * \param content const xml_content_t*const : pointer to XML content type
 * \param target void* : Target address to store the extracted content
 * \param source const char* : Source XML content to extract
 * \param length size_t : Length of XML content
 * \return extern xml_parse_result_t : result of content extraction.
 *
 */
extern xml_parse_result_t extract_content(const xml_content_t* const content,
                                          void* target, const char* source,
                                          size_t length);

#endif // XML_CONTENT_H
