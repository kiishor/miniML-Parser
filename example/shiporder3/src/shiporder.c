#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include <stddef.h>

#include "shiporder.h"

extern void* allocate_item(uint32_t occurrence, void* context);

static const xs_attribute_t shiporder_attribute[];
static const xs_element_t shiporder_descendant[];
static const xs_element_t item_descendant[];
static const xs_element_t shipto_descendant[];


//! Holds XML schema property of "shiporder" root element
const xs_element_t shiporder_root =
{
    .Name.String = "shiporder",                //!< Name of an XML root element
    .Name.Length = 9,                          //!< Length of an element name
    .MinOccur    = 1,                          //!< Minimum number of times an element shall occur in the XML
    .MaxOccur    = 1,                          //!< Maximum number of times an element can occur in the XML

    .Target.Type    = EN_RELATIVE,             //!< Target address type is relative.
    .Target.Offset = 0,                        //!< Target address Offset from the address passed in "parse_xml function

    .Content.Type   = EN_NO_XML_DATA_TYPE,     //!< This element doesn't hold any content.

    .Attribute_Quantity = 3,                   //!< Number of attributes of this element
    .Attribute          = shiporder_attribute, //!< Address of structure containing attributes

    .Child_Quantity = 3,                       //!< Number of child elements
    .Child_Order    = EN_SEQUENCE,             //!< Order of child elements
    .Child          = shiporder_descendant,    //!< Address of structure holding child elements
};

//! Holds properties of attributes of root element "shiporder"
static const xs_attribute_t shiporder_attribute[] =
{
    [0].Name.String = "xmlns:xsi",             //!< Name of XML attribute
    [0].Name.Length = 9,                       //!< Length of attribute name
    [0].Use         = EN_OPTIONAL,             //!< Usage of attribute in the XML. Required / Optional

    [1].Name.String = "xsi:noNamespaceSchemaLocation",  //!< Name of XML attribute
    [1].Name.Length = 29,                      //!< Length of attribute name.
    [1].Use         = EN_OPTIONAL,             //!< Usage of attribute in the XML. Required / Optional

    [2].Name.String = "orderid",               //!< Name of XML attribute
    [2].Name.Length = 7,                       //!< Length of attribute name
    [2].Target.Type = EN_RELATIVE,             //!< Target address type is relative.
    [2].Target.Offset  = offsetof(shiporder_t, orderid),  //! Target address offset from the parent target address
    [2].Content.Type   = EN_STRING_DYNAMIC,          //!< Content type is string. It will be dynamically allocated
    [2].Content.Facet.String.MinLength = 0,          //!< Minimum length of string
    [2].Content.Facet.String.MaxLength = 4294967295, //!< Maximum length of string.
    [2].Use         = EN_REQUIRED,                   //!< Usage of attribute in the XML. Required / Optional
};

//! Holds the properties of all the child elements of "shiporder" element
static const xs_element_t shiporder_descendant[] =
{
    [0].Name.String = "orderperson",                  //!< name of XML element
    [0].Name.Length = 11,                             //!< Length of an element name
    [0].MinOccur    = 1,                              //!< Minimum number of times an element shall occur in the XML
    [0].MaxOccur    = 1,                              //!< Maximum number of times an element can occur in the XML

    [0].Target.Type   = EN_RELATIVE,                         //!< Target address type is relative.
    [0].Target.Offset = offsetof(shiporder_t, orderperson),  //!< Target address offset from the parent target address

    [0].Content.Type  = EN_STRING_DYNAMIC,            //!< Content type is string. It will be dynamically allocated
    [0].Content.Facet.String.MinLength = 0,           //!< Minimum length of string
    [0].Content.Facet.String.MaxLength = 4294967295,  //!< Maximum length of string

    [1].Name.String = "shipto",                         //!< name of XML element
    [1].Name.Length = 6,                                //!< Length of an element name
    [1].MinOccur    = 1,                                //!< Minimum number of times an element shall occur in the XML
    [1].MaxOccur    = 1,                                //!< Maximum number of times an element can occur in the XML
    [1].Target.Type    = EN_RELATIVE,                   //!< Target address type is relative.
    [1].Target.Offset  = offsetof(shiporder_t, shipto), //!< Target address offset from the parent target address
    [1].Content.Type   = EN_NO_XML_DATA_TYPE,           //!< This element doesn't hold any content.
    [1].Child_Quantity = 4,                             //!< Number of child elements
    [1].Child_Order    = EN_SEQUENCE,                   //!< Order of child elements
    [1].Child          = shipto_descendant,             //!< Address of child elements xs_element_t structure

    [2].Name.String = "item",                           //!< name of XML element
    [2].Name.Length = 4,                                //!< Length of an element name
    [2].MinOccur    = 1,                                //!< Minimum number of times an element shall occur in the XML
    [2].MaxOccur    = 10,                               //!< Maximum number of times an element can occur in the XML
    [2].Target.Type    = EN_DYNAMIC,                    //!< Target address type is dynamic.
    [2].Target.Allocate = allocate_item,                //!< Allocate callback function to get target address dynamically.
    [2].Content.Type   = EN_NO_XML_DATA_TYPE,           //!< This element doesn't hold any content.
    [2].Child_Quantity = 4,                             //!< Number of child elements
    [2].Child_Order    = EN_SEQUENCE,                   //!< Order of child elements
    [2].Child          = item_descendant,               //!< Address of child elements xs_element_t structure
};


//! Holds XML schema properties of all the child elements of "item" element
static const xs_element_t item_descendant[] =
{
    [0].Name.String = "title",                        //!< name of XML element
    [0].Name.Length = 5,                              //!< Length of an element name
    [0].MinOccur    = 1,                              //!< Minimum number of times an element shall occur in the XML
    [0].MaxOccur    = 1,                              //!< Maximum number of times an element can occur in the XML
    [0].Target.Type = EN_RELATIVE,                    //!< Target address type is relative.
    [0].Target.Offset  = offsetof(item_t, title),     //!< Target address offset from the parent target address
    [0].Content.Type   = EN_STRING_DYNAMIC,           //!< Content type is string. It will be dynamically allocated
    [0].Content.Facet.String.MinLength = 0,           //!< Minimum length of string
    [0].Content.Facet.String.MaxLength = 4294967295,  //!< Maximum length of string

    [1].Name.String = "note",                         //!< name of XML element
    [1].Name.Length = 4,                              //!< Length of an element name
    [1].MinOccur    = 0,                              //!< Minimum number of times an element shall occur in the XML
    [1].MaxOccur    = 1,                              //!< Maximum number of times an element can occur in the XML
    [1].Target.Type = EN_RELATIVE,                    //!< Target address type is relative.
    [1].Target.Offset = offsetof(item_t, note),       //!< Target address offset from the parent target address
    [1].Content.Type  = EN_STRING_DYNAMIC,            //!< Content type is string. It will be dynamically allocated
    [1].Content.Facet.String.MinLength = 0,           //!< Minimum length of string
    [1].Content.Facet.String.MaxLength = 4294967295,  //!< Maximum length of string

    [2].Name.String = "quantity",                     //!< name of XML element
    [2].Name.Length = 8,                              //!< Length of an element name
    [2].MinOccur    = 1,                              //!< Minimum number of times an element shall occur in the XML
    [2].MaxOccur    = 1,                              //!< Maximum number of times an element can occur in the
    [2].Target.Type = EN_RELATIVE,                    //!< Target address type is relative.
    [2].Target.Offset  = offsetof(item_t, quantity),  //!< Target address offset from the parent target address
    [2].Content.Type   = EN_UNSIGNED,                 //!< Content type is unsigned int
    [2].Content.Facet.Uint.MinValue = 1,              //!< Minimum value of content
    [2].Content.Facet.Uint.MaxValue = 4294967295,     //!< Maximum possible value of content

    [3].Name.String = "price",                        //!< name of XML element
    [3].Name.Length = 5,                              //!< Length of an element name
    [3].MinOccur    = 1,                              //!< Minimum number of times an element shall occur in the XML
    [3].MaxOccur    = 1,                              //!< Maximum number of times an element can occur in the
    [3].Target.Type   = EN_RELATIVE,                  //!< Target address type is relative.
    [3].Target.Offset = offsetof(item_t, price),      //!< Target address offset from the parent target address
    [3].Content.Type  = EN_DECIMAL,                      //!< Content type is float
    [3].Content.Facet.Decimal.MinValue = -3.40282e+038f, //!< Minimum value of content
    [3].Content.Facet.Decimal.MaxValue = 3.40282e+038f,  //!< Maximum possible value of content
};

//! Holds XML schema properties of all the child elements of "shipto" element
static const xs_element_t shipto_descendant[] =
{
    [0].Name.String = "name",                         //!< name of XML element
    [0].Name.Length = 4,                              //!< Length of an element name
    [0].MinOccur    = 1,                              //!< Minimum number of times an element shall occur in the XML
    [0].MaxOccur    = 1,                              //!< Maximum number of times an element can occur in the XML
    [0].Target.Type    = EN_RELATIVE,                 //!< Target address type is relative.
    [0].Target.Offset  = offsetof(shipto_t, name),    //!< offset from the parent target address
    [0].Content.Type   = EN_STRING_DYNAMIC,           //!< Content type is string. It will be dynamically allocated
    [0].Content.Facet.String.MinLength = 0,           //!< Minimum length of string
    [0].Content.Facet.String.MaxLength = 4294967295,  //!< Maximum length of string

    [1].Name.String = "address",                      //!< name of XML element
    [1].Name.Length = 7,                              //!< Length of an element name
    [1].MinOccur    = 1,                              //!< Minimum number of times an element shall occur in the XML
    [1].MaxOccur    = 1,                              //!< Maximum number of times an element can occur in the XML
    [1].Target.Type    = EN_RELATIVE,                 //!< Target address type is relative.
    [1].Target.Offset  = offsetof(shipto_t, address), //!< Target address offset from the parent target address
    [1].Content.Type   = EN_STRING_DYNAMIC,           //!< Content type is string. It will be dynamically allocated
    [1].Content.Facet.String.MinLength = 0,           //!< Minimum length of string
    [1].Content.Facet.String.MaxLength = 4294967295,  //!< Maximum length of string

    [2].Name.String = "city",                         //!< name of XML element
    [2].Name.Length = 4,                              //!< Length of an element name
    [2].MinOccur    = 1,                              //!< Minimum number of times an element shall occur in the XML
    [2].MaxOccur    = 1,                              //!< Maximum number of times an element can occur in the XML
    [2].Target.Type   = EN_RELATIVE,                  //!< Target address type is relative.
    [2].Target.Offset = offsetof(shipto_t, city),     //!< Target address offset from the parent target address
    [2].Content.Type  = EN_STRING_DYNAMIC,            //!< Content type is string. It will be dynamically allocated
    [2].Content.Facet.String.MinLength = 0,           //!< Minimum length of string
    [2].Content.Facet.String.MaxLength = 4294967295,  //!< Maximum length of string

    [3].Name.String = "country",                      //!< name of XML element
    [3].Name.Length = 7,                              //!< Length of an element name
    [3].MinOccur    = 1,                              //!< Minimum number of times an element shall occur in the XML
    [3].MaxOccur    = 1,                              //!< Maximum number of times an element can occur in the XML
    [3].Target.Type   = EN_RELATIVE,                  //!< Target address type is relative.
    [3].Target.Offset = offsetof(shipto_t, country),  //!< Target address offset from the parent target address
    [3].Content.Type  = EN_STRING_DYNAMIC,            //!< Content type is string. It will be dynamically allocated
    [3].Content.Facet.String.MinLength = 0,           //!< Minimum length of string
    [3].Content.Facet.String.MaxLength = 4294967295,  //!< Maximum length of string
};

