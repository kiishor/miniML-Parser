XML code generator
==================

This is a source code generator tool for [xml parser in c][1]. This application parses XML schema file
and generates C source code that can be used for parsing XML file.

It generates *xs_element_t* data structure in *C* based on the XML schema file. This structure contains all the
schema properties of XML elements such as element name, its child elements, attributes, content type, etc..
It also contains the tree structure of XML elements in the schema.
The parser uses this *xs_element_t* structure for validating the input XML file against XML schema constraints (rules).

## How to use

This is a command line tool, it takes XML schema file as input and generates c source code.
The simplest way to use is,
```shell
xml_code_generator.exe food.xsd
```

The tool generates two files
- food.h  : This file contains *food_t* structure created from schema to store the content of XML.
- food.c  : This file contains *xs_element_t* tree structure generated from schema.

### food.xsd

Input XML schema.

```XML
<?xml version="1.0"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
 <xs:element name="food">
    <xs:complexType>
      <xs:sequence>
        <xs:element type="xs:string" name="name"/>
        <xs:element type="xs:float" name="price"/>
        <xs:element type="xs:string" name="description"/>
        <xs:element type="xs:unsignedInt" name="calories"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>
</xs:schema>
```

### food.h

Header file contains the C structure *food_t* based on the data structure of input XML schema.
This structure is useful to store the extracted content from the XML file by [XML parser][1].

```C
typedef struct
{
    char* name;
    float price;
    char* description;
    uint32_t calories;
}food_t;
```

### food.c

Source file contains the *xs_element_t* for each element in the schema.

*food_root* is the *xs_element_t* variable for root element "food".
It represents all the XML element properties of "food" element in schema.
Also, it holds the address of its attributes (*food_attribute*) and child elements (*food_descendant*).

```C
const xs_element_t food_root =
{
    .Name.String = "food",
    .Name.Length = 4,
    .MinOccur    = 1,
    .MaxOccur    = 1,
    .Target.Type    = EN_RELATIVE,
    .Target.Offset  = 0,
    .Content.Type   = EN_NO_XML_DATA_TYPE,
    .Attribute_Quantity = 2,
    .Attribute          = food_attribute,
    .Child_Quantity = 4,
    .Child_Order    = EN_SEQUENCE,
    .Child          = food_descendant,
};
```

*food_descendant* is an array of *xs_element_t*. It holds all the child elements of *food* element.
```C
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

    ...

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
```

*food_attribute* is an array of *xs_attribute_t*. It holds all the attributes of *food* element.

```C
static const xs_attribute_t food_attribute[] =
{
    [0].Name.String = "xmlns:xsi",
    [0].Name.Length = 9,
    [0].Use         = EN_OPTIONAL,

    [1].Name.String = "xsi:noNamespaceSchemaLocation",
    [1].Name.Length = 29,
    [1].Use         = EN_OPTIONAL,
};
```

Address of *food_root* is passed to [xml parser][1] along with XML string to parse.
For further information on how to use these food.c and food.h for parsing XML file refer [food][2] example

## Command line options

### Callback
`-b` or `--callback`

Code generator includes *Callback* member in each *xs_element_t* variables of generated code.

By default *Callback* member variable is disabled in the *xs_element_t* structure to save code space.
Hence, to avoid compilation error, code generator also by default omits the *Callback* member in the generated code.
Use this option in the command line when you have enabled *Callback* member in the XML parser.

### Dynamic memory allocation
`-d` or `--dynamic`
Code generator uses dynamic addressing method (`EN_DYNAMIC`) for XML element that can occur multiple times (maxOccurs > 1) in an XML file.

By default code generator uses relative addressing method (`EN_RELATIVE`) as a target addressing method for all the XML elements.
The dynamic addressing method enables user to allocate memory at runtime to store the XML data.


### Enable context

`-c` or `--context`

Code generator includes *context* argument in the signature of *allocate* callback function.

By default user defined context in the XML parser is disabled to save code space.
Hence, to avoid compilation error, code generator also by default omits the *context* argument
in the *allocate* callback function.
Use this option in the command line when you have enabled the *context* argument in the XML parser.

### More
- For reporting issues/bugs or requesting features use [GitHub issue tracker][3]


[1]: https://github.com/kiishor/miniML-Parser
[2]: https://github.com/kiishor/miniML-Parser/tree/master/example/food
[3]: https://github.com/kiishor/xml_code_generator/issues
