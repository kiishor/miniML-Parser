#ifndef XML_CONTENT_H
#define XML_CONTENT_H

/**
 * \file
 * \brief   datatype for XML content.

 * \author  Nandkishor Biradar
 * \date    19 August 2019
 */

/*
 *  ------------------------------ INCLUDE FILES ------------------------------
 */
#include <stdbool.h>

/*
 *  ------------------------------- DEFINITION -------------------------------
 */

#define ALL_XSD_DATA_TYPE  \
ADD_DATA_TYPE(XS_STRING,               xs:string,             char*,    string)                   \
ADD_DATA_TYPE(XS_DECIMAL,              xs:decimal,            float,    float/double)             \
ADD_DATA_TYPE(XS_INTEGER,              xs:integer,            int32_t,  int32_t/int64_t)          \
ADD_DATA_TYPE(XS_BYTE,                 xs:byte,               int8_t,   A 8-bit signed integer)   \
ADD_DATA_TYPE(XS_INT,                  xs:int,                int32_t,  A signed 32-bit integer)  \
ADD_DATA_TYPE(XS_LONG,                 xs:long,               int64_t,  A signed 64-bit integer)  \
ADD_DATA_TYPE(XS_NEGATIVE_INTEGER,     xs:negativeInteger,    int32_t,  An integer containing only negative values (..,-2,-1))       \
ADD_DATA_TYPE(XS_NON_NEGATIVE_INTEGER, xs:nonNegativeInteger, uint32_t, An integer containing only non-negative values (0,1,2,..))   \
ADD_DATA_TYPE(XS_NON_POSITIVE_INTEGER, xs:nonPositiveInteger, int32_t,  An integer containing only non-positive values (..,-2,-1,0)) \
ADD_DATA_TYPE(XS_POSITIVE_INTEGER,     xs:positiveInteger,    uint32_t, An integer containing only positive values (1,2,..))         \
ADD_DATA_TYPE(XS_SHORT,                xs:short,              int16_t,  A signed 16-bit integer)     \
ADD_DATA_TYPE(EN_UNSIGNED_LONG,        xs:unsignedLong,       uint64_t, An unsigned 64-bit integer)  \
ADD_DATA_TYPE(XS_UNSIGNED_INT,         xs:unsignedInt,        uint32_t, An unsigned 32-bit integer)  \
ADD_DATA_TYPE(EN_UNSIGNED_SHORT,       xs:unsignedShort,      uint16_t, An unsigned 16-bit integer)  \
ADD_DATA_TYPE(EN_UNSIGNED_BYTE,        xs:unsignedByte,       uint8_t,  An unsigned 8-bit integer)   \
ADD_DATA_TYPE(XS_BOOLEAN,              xs:boolean,            bool,     boolean)  \
ADD_DATA_TYPE(XS_DOUBLE,               xs:double,             double,   double)   \
ADD_DATA_TYPE(XS_FLOAT,                xs:float,              float,    float)    \
ADD_DATA_TYPE(XS_DATE,                 xs:date,               char*,    string)   \
ADD_DATA_TYPE(XS_TIME,                 xs:time,               char*,    string)   \
ADD_DATA_TYPE(XS_DATE_TIME,            xs:dateTime,           char*,    string)   \
ADD_DATA_TYPE(XS_DURATION,             xs:duration,           uint32_t, string)


#define ASSERT1(expression, result, error)        \
{                                                 \
  if(!(expression))                               \
  {                                               \
    result = error;                               \
    return NULL;                                  \
  }                                               \
}

#define ASSERT2(expression) if(!(expression)) return NULL

#define ALL_XML_CONTENT_TYPES       \
  ADD_CONTENT(EN_NO_XML_DATA_TYPE, XML element does not hold the content)    \
  ADD_CONTENT(EN_STRING, satrting address of string from the imput xml buffer and length of content is copied to string_t)  \
  ADD_CONTENT(EN_STRING_DYNAMIC, String will be dynamically allocated using malloc and the pointer is copied to the target) \
  ADD_CONTENT(EN_CHAR_ARRAY, String will be directly copied to the target using memcpy)  \
  ADD_CONTENT(EN_ENUM_STRING, Enumeration)          \
  ADD_CONTENT(EN_ENUM_INT, Enumeration of integers. e.g. enum { red = 10, green = 50};) \
  ADD_CONTENT(EN_ENUM_UINT, Enumeration of unsigned integers) \
  ADD_CONTENT(EN_DECIMAL, Float data type)          \
  ADD_CONTENT(EN_INTEGER, 32-bit signed integer)    \
  ADD_CONTENT(EN_UNSIGNED, 32-bit unsigned integer) \
  ADD_CONTENT(EN_BOOL, Boolean data type)           \
  ADD_CONTENT(EN_DATE, Date in "YYYY-MM-DD" format) \
  ADD_CONTENT(EN_TIME, Time in "HH:MM:SS" format)   \
  ADD_CONTENT(EN_DATE_TIME, Date and Time in "YYYY-MM-DD HH:MM:SS") \
  ADD_CONTENT(EN_DURATION, Duration)

/*
 *  ------------------------------- ENUMERATION -------------------------------
 */

#define ADD_CONTENT(type, ...)    type,
//! basic data type of an element or attribute
typedef enum
{
  ALL_XML_CONTENT_TYPES
  TOTAL_XML_CONTENT_TYPES
}xml_content_type_t;
#undef ADD_CONTENT

/*
 *  -------------------------------- STRUCTURE --------------------------------
 */

typedef struct
{
  char* String;   // This pointer can not be const because this can be created dynamically.
  size_t Length;
}string_t;

typedef struct
{
  uint32_t Year;
  uint32_t Month;
  uint32_t Day;
}xs_date_t;

typedef struct
{
  uint32_t Hour;
  uint32_t Minute;
  uint32_t Second;
}xs_time_t;

typedef struct
{
  xs_date_t Date;
  xs_time_t Time;
}xs_date_time_t;

typedef struct
{
  bool Sign;
  xs_date_time_t Period;
}xs_duration_t;


typedef struct
{
  uint32_t MinValue;
  uint32_t MaxValue;
}uint_facet_t;

typedef struct
{
  int32_t MinValue;
  int32_t MaxValue;
}int_facet_t;

typedef struct
{
  uint32_t MinLength;
  uint32_t MaxLength;
}string_facet_t;

typedef struct
{
  float MinValue;
  float MaxValue;
}decimal_facet_t;

typedef struct
{
	const void *const List;
	uint32_t Quantity;
}enum_facet_t;

typedef union
{
  uint_facet_t Uint;
  int_facet_t Int;
  string_facet_t String;
  decimal_facet_t Decimal;
  enum_facet_t Enum;
}facet_t;

typedef struct
{
  xml_content_type_t Type;
  facet_t Facet;
}xml_content_t;

/*
 *  ---------------------------- EXPORTED FUNCTION ----------------------------
 */
extern const char* extract_content(const xml_content_t* const content,
                            void* target, const char* source,
                            char token);

#endif // XML_CONTENT_H
