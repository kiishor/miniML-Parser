#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include <stddef.h>

#include "shiporder.h"


shiporder_t shiporder;


static const xs_element_t item_descendant[] =
{
    [0].Name.String = "title",
    [0].Name.Length = 5,
    [0].MinOccur    = 1,
    [0].MaxOccur    = 1,
    [0].Target.Type    = EN_RELATIVE,
    [0].Target.Offset  = offsetof(item_t, title),
    [0].Content.Type   = EN_STRING_DYNAMIC,
    [0].Content.Facet.String.MinLength = 0,
    [0].Content.Facet.String.MaxLength = 4294967295,

    [1].Name.String = "note",
    [1].Name.Length = 4,
    [1].MinOccur    = 0,
    [1].MaxOccur    = 1,
    [1].Target.Type    = EN_RELATIVE,
    [1].Target.Offset  = offsetof(item_t, note),
    [1].Content.Type   = EN_STRING_DYNAMIC,
    [1].Content.Facet.String.MinLength = 0,
    [1].Content.Facet.String.MaxLength = 4294967295,

    [2].Name.String = "quantity",
    [2].Name.Length = 8,
    [2].MinOccur    = 1,
    [2].MaxOccur    = 1,
    [2].Target.Type    = EN_RELATIVE,
    [2].Target.Offset  = offsetof(item_t, quantity),
    [2].Content.Type   = EN_UNSIGNED,
    [2].Content.Facet.Uint.MinValue = 1,
    [2].Content.Facet.Uint.MaxValue = 4294967295,

    [3].Name.String = "price",
    [3].Name.Length = 5,
    [3].MinOccur    = 1,
    [3].MaxOccur    = 1,
    [3].Target.Type    = EN_RELATIVE,
    [3].Target.Offset  = offsetof(item_t, price),
    [3].Content.Type   = EN_DECIMAL,
    [3].Content.Facet.Decimal.MinValue = -3.40282e+038f,
    [3].Content.Facet.Decimal.MaxValue = 3.40282e+038f,
};

static const xs_element_t shipto_descendant[] =
{
    [0].Name.String = "name",
    [0].Name.Length = 4,
    [0].MinOccur    = 1,
    [0].MaxOccur    = 1,
    [0].Target.Type    = EN_RELATIVE,
    [0].Target.Offset  = offsetof(shipto_t, name),
    [0].Content.Type   = EN_STRING_DYNAMIC,
    [0].Content.Facet.String.MinLength = 0,
    [0].Content.Facet.String.MaxLength = 4294967295,

    [1].Name.String = "address",
    [1].Name.Length = 7,
    [1].MinOccur    = 1,
    [1].MaxOccur    = 1,
    [1].Target.Type    = EN_RELATIVE,
    [1].Target.Offset  = offsetof(shipto_t, address),
    [1].Content.Type   = EN_STRING_DYNAMIC,
    [1].Content.Facet.String.MinLength = 0,
    [1].Content.Facet.String.MaxLength = 4294967295,

    [2].Name.String = "city",
    [2].Name.Length = 4,
    [2].MinOccur    = 1,
    [2].MaxOccur    = 1,
    [2].Target.Type    = EN_RELATIVE,
    [2].Target.Offset  = offsetof(shipto_t, city),
    [2].Content.Type   = EN_STRING_DYNAMIC,
    [2].Content.Facet.String.MinLength = 0,
    [2].Content.Facet.String.MaxLength = 4294967295,

    [3].Name.String = "country",
    [3].Name.Length = 7,
    [3].MinOccur    = 1,
    [3].MaxOccur    = 1,
    [3].Target.Type    = EN_RELATIVE,
    [3].Target.Offset  = offsetof(shipto_t, country),
    [3].Content.Type   = EN_STRING_DYNAMIC,
    [3].Content.Facet.String.MinLength = 0,
    [3].Content.Facet.String.MaxLength = 4294967295,
};

static void* allocate_item(uint32_t occurrence);

static const xs_element_t shiporder_descendant[] =
{
    [0].Name.String = "orderperson",
    [0].Name.Length = 11,
    [0].MinOccur    = 1,
    [0].MaxOccur    = 1,
    [0].Target.Type    = EN_RELATIVE,
    [0].Target.Offset  = offsetof(shiporder_t, orderperson),
    [0].Content.Type   = EN_STRING_DYNAMIC,
    [0].Content.Facet.String.MinLength = 0,
    [0].Content.Facet.String.MaxLength = 4294967295,

    [1].Name.String = "shipto",
    [1].Name.Length = 6,
    [1].MinOccur    = 1,
    [1].MaxOccur    = 1,
    [1].Target.Type    = EN_RELATIVE,
    [1].Target.Offset  = offsetof(shiporder_t, shipto),
    [1].Content.Type   = EN_NO_XML_DATA_TYPE,
    [1].Child_Quantity = 4,
    [1].Child_Order    = EN_SEQUENCE,
    [1].Child          = shipto_descendant,

    [2].Name.String = "item",
    [2].Name.Length = 4,
    [2].MinOccur    = 1,
    [2].MaxOccur    = 4294967295,
    [2].Target.Type    = EN_DYNAMIC,
    [2].Target.Allocate = allocate_item,
    [2].Content.Type   = EN_NO_XML_DATA_TYPE,
    [2].Child_Quantity = 4,
    [2].Child_Order    = EN_SEQUENCE,
    [2].Child          = item_descendant,
};

static const xs_attribute_t shiporder_attribute[] =
{
    [0].Name.String = "xmlns:xsi",
    [0].Name.Length = 9,
    [0].Use         = EN_OPTIONAL,

    [1].Name.String = "xsi:noNamespaceSchemaLocation",
    [1].Name.Length = 29,
    [1].Use         = EN_OPTIONAL,

    [2].Name.String = "orderid",
    [2].Name.Length = 7,
    [2].Target.Type    = EN_RELATIVE,
    [2].Target.Offset  = offsetof(shiporder_t, orderid),
    [2].Content.Type   = EN_STRING_DYNAMIC,
    [2].Content.Facet.String.MinLength = 0,
    [2].Content.Facet.String.MaxLength = 4294967295,
    [2].Use         = EN_REQUIRED,
};

static const xs_element_t root_descendant[] =
{
    [0].Name.String = "shiporder",
    [0].Name.Length = 9,
    [0].MinOccur    = 1,
    [0].MaxOccur    = 1,
    [0].Target.Type    = EN_STATIC,
    [0].Target.Address = &shiporder,
    [0].Content.Type   = EN_NO_XML_DATA_TYPE,
    [0].Attribute_Quantity = 3,
    [0].Attribute          = shiporder_attribute,
    [0].Child_Quantity = 3,
    [0].Child_Order    = EN_SEQUENCE,
    [0].Child          = shiporder_descendant,
};

const xs_element_t xml_root =
{
    .Child_Quantity = 1,
    .Child_Order    = EN_CHOICE,
    .Child          = root_descendant,
};

static void* allocate_item(uint32_t occurrence)
{
    void* const target = calloc(sizeof(item_t), 1);
    if(shiporder.item == NULL)
    {
        shiporder.item = target;
        return target;
    }

    item_t* node = shiporder.item;
    while(node->Next)
    {
        node = node->Next;
    }

    node->Next = target;
    return target;
}
