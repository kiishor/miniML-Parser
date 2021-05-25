food.xml
========

This example demonstrate a parsing of below sample XML.

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

The schema of above XML file is defined here [food.xsd](1).
the root element of this XML is "food". It contains four child elements,
- name        : Name of food item
- price       : Price of food item
- description : Description of food item
- calories    : Calories of food item

The content of XML data shall be extracted to below structure defined in [food.h](2).

```C
typedef struct
{
    char* name;           //!< Holds content of name XML element
    float price;          //!< Holds content of price XML element
    char* description;    //!< Holds content of description XML element
    uint32_t calories;    //!< Holds content of calories XML element
}food_t;

```

[food.c](3) file contains xs_element_t structure for all the elements of XML schema.
The xs_element_t structure contains all the validation rule of an XML element specified in the schema.

- `xs_element_t food_element;`    : holds properties of root element "food".
- `xs_attribute_t food_attribute` : holds properties of attributes of root element "food".
- `xs_element_t food_descendant`  : holds properties of all the child elements (name, price, description & calories) of root element "food".
  It also holds the target address to store the extracted content of XML data.


[1]: xml/food.xsd
[2]: src/food.h
[3]: src/food.c
