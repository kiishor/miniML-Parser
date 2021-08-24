#include <stdlib.h>
#include <stdbool.h>
#include <float.h>

#include "food.h"

//! Global variable to store content of food XML
food_t waffle;

static const xs_attribute_t food_attribute[];
static const xs_element_t food_descendant[];

//! Structure containing XML schema property of food (root) element.
const xs_element_t food_element =
{
    .Name.String = "food",                     //!< name of XML element
    .Name.Length = 4,                          //!< Length of name
    .MinOccur    = 1,                          //!< Minimum number of times an element shall occur in the XML
    .MaxOccur    = 1,                          //!< Maximum number of times an element can occur in the XML

    .Content.Type   = EN_NO_XML_DATA_TYPE,     //!< This element doesn't hold any content

    .Attribute_Quantity = 2,                   //!< Number of attributes of this element
    .Attribute          = food_attribute,      //!< Address of structure containing attributes

    .Child_Quantity = 4,                       //!< Number of child elements
    .Child_Order    = EN_SEQUENCE,             //!< Order of child elements
    .Child          = food_descendant,         //!< Address of structure holding child elements
};

//! Structure containing all the attributes of food (root) element
static const xs_attribute_t food_attribute[] =
{
    [0].Name.String = "xmlns:xsi",                      //!< Name of XML attribute
    [0].Name.Length = 9,                                //!< Length of name
    [0].Use         = EN_OPTIONAL,                      //!< Usage of attribute in the XML. Required / Optional

    [1].Name.String = "xsi:noNamespaceSchemaLocation",  //!< Name of XML attribute
    [1].Name.Length = 29,                               //!< Length of attribute
    [1].Use         = EN_OPTIONAL,                      //!< Usage of attribute in the XML. Required / Optional
};

//! Structure containing all the child element of food (root) element.
static const xs_element_t food_descendant[] =
{
    [0].Name.String = "name",                           //!< Name of XML element
    [0].Name.Length = 4,                                //!< Length of name
    [0].MinOccur    = 1,                                //!< Minimum number of times an element shall occur in the XML
    [0].MaxOccur    = 1,                                //!< Maximum number of times an element can occur in the XML
    [0].Target.Type    = EN_STATIC,                     //!< Target address type is static. No dynamic memory allocation is required.
    [0].Target.Address  = &waffle.name,                   //!< Target address to store content of an element
    [0].Content.Type   = EN_STRING_DYNAMIC,             //!< Content type is string that is allocated dynamically using malloc() by parser
    [0].Content.Facet.String.MinLength = 0,             //!< Minimum length of string
    [0].Content.Facet.String.MaxLength = 4294967295,    //!< Maximum possible length of a string

    [1].Name.String = "price",                          //!< Name of XML element
    [1].Name.Length = 5,                                //!< Length of element name
    [1].MinOccur    = 1,                                //!< Minimum number of times an element shall occur in the XML
    [1].MaxOccur    = 1,                                //!< Maximum number of times an element can occur in the XML
    [1].Target.Type    = EN_STATIC,                     //!< Target address type is static. No dynamic memory allocation is required
    [1].Target.Address  = &waffle.price,                  //!< Target address to store content of an element
    [1].Content.Type   = EN_DECIMAL,                      //!< Content type is decimal i.e. float
    [1].Content.Facet.Decimal.MinValue = -3.40282e+038f,  //!< Minimum possible value of an element
    [1].Content.Facet.Decimal.MaxValue = 3.40282e+038f,   //!< Maximum possible value of an element

    [2].Name.String = "description",                    //!< Name of XML element
    [2].Name.Length = 11,                               //!< length of name
    [2].MinOccur    = 1,                                //!< Minimum number of times an element shall occur in the XML
    [2].MaxOccur    = 1,                                //!< Maximum number of times an element can occur in the XML
    [2].Target.Type    = EN_STATIC,                     //!< Target address type is static. No dynamic memory allocation is required.
    [2].Target.Address  = &waffle.description,            //!< Target address to store content of an element
    [2].Content.Type   = EN_STRING_DYNAMIC,             //!< Content type is string that is allocated dynamically using malloc() by parser
    [2].Content.Facet.String.MinLength = 0,             //!< Minimum length of string
    [2].Content.Facet.String.MaxLength = 4294967295,    //!< Maximum possible length of a string

    [3].Name.String = "calories",                       //!< Name of XML element
    [3].Name.Length = 8,                                //!< length of name
    [3].MinOccur    = 1,                                //!< Minimum number of times an element shall occur in the XML
    [3].MaxOccur    = 1,                                //!< Maximum number of times an element can occur in the XML
    [3].Target.Type    = EN_STATIC,                     //!< Target address type is static. No dynamic memory allocation is required.
    [3].Target.Address  = &waffle.calories,               //!< Target address to store content of an element
    [3].Content.Type   = EN_UNSIGNED,                   //!< Content type is unsigned int
    [3].Content.Facet.Uint.MinValue = 0,                //!< Minimum possible value of an element
    [3].Content.Facet.Uint.MaxValue = 4294967295,       //!< Maximum possible value of an element
};

