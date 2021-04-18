#include <stdlib.h>
#include <stdbool.h>
#include <float.h>

#include "food.h"


food_t food;


static const xs_element_t food_descendant[] =
{
    [0].Name.String = "name",
    [0].Name.Length = 4,
    [0].MinOccur    = 1,
    [0].MaxOccur    = 1,
    [0].Target.Type    = EN_RELATIVE,
    [0].Target.Offset  = offsetof(food_t, name),
    [0].Content.Type   = EN_STRING_DYNAMIC,
    [0].Content.Facet.String.MinLength = 0,
    [0].Content.Facet.String.MaxLength = 4294967295,

    [1].Name.String = "price",
    [1].Name.Length = 5,
    [1].MinOccur    = 1,
    [1].MaxOccur    = 1,
    [1].Target.Type    = EN_RELATIVE,
    [1].Target.Offset  = offsetof(food_t, price),
    [1].Content.Type   = EN_DECIMAL,
    [1].Content.Facet.Decimal.MinValue = -3.40282e+038f,
    [1].Content.Facet.Decimal.MaxValue = 3.40282e+038f,

    [2].Name.String = "description",
    [2].Name.Length = 11,
    [2].MinOccur    = 1,
    [2].MaxOccur    = 1,
    [2].Target.Type    = EN_RELATIVE,
    [2].Target.Offset  = offsetof(food_t, description),
    [2].Content.Type   = EN_STRING_DYNAMIC,
    [2].Content.Facet.String.MinLength = 0,
    [2].Content.Facet.String.MaxLength = 4294967295,

    [3].Name.String = "calories",
    [3].Name.Length = 8,
    [3].MinOccur    = 1,
    [3].MaxOccur    = 1,
    [3].Target.Type    = EN_RELATIVE,
    [3].Target.Offset  = offsetof(food_t, calories),
    [3].Content.Type   = EN_UNSIGNED,
    [3].Content.Facet.Uint.MinValue = 0,
    [3].Content.Facet.Uint.MaxValue = 4294967295,
};

static const xs_attribute_t food_attribute[] =
{
    [0].Name.String = "xmlns:xsi",
    [0].Name.Length = 9,
    [0].Use         = EN_OPTIONAL,

    [1].Name.String = "xsi:noNamespaceSchemaLocation",
    [1].Name.Length = 29,
    [1].Use         = EN_OPTIONAL,
};

static const xs_element_t root_descendant[] =
{
    [0].Name.String = "food",
    [0].Name.Length = 4,
    [0].MinOccur    = 1,
    [0].MaxOccur    = 1,
    [0].Target.Type    = EN_STATIC,
    [0].Target.Address = &food,
    [0].Content.Type   = EN_NO_XML_DATA_TYPE,
    [0].Attribute_Quantity = 2,
    [0].Attribute          = food_attribute,
    [0].Child_Quantity = 4,
    [0].Child_Order    = EN_SEQUENCE,
    [0].Child          = food_descendant,
};

const xs_element_t xml_root =
{
    .Child_Quantity = 1,
    .Child_Order    = EN_CHOICE,
    .Child          = root_descendant,
};
