shiporder using linked-list
===========================

This example demonstrate a parsing of below XML content.

```XML
<shiporder orderid="889923"
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
xsi:noNamespaceSchemaLocation="shiporder.xsd">
  <orderperson>John Smith</orderperson>
  <shipto>
    <name>Ola Nordmann</name>
    <address>Langgt 23</address>
    <city>4000 Stavanger</city>
    <country>Norway</country>
  </shipto>
  <item>
    <title>Empire Burlesque</title>
    <note>Special Edition</note>
    <quantity>1</quantity>
    <price>10.90</price>
  </item>
  <item>
    <title>Hide your heart</title>
    <quantity>1</quantity>
    <price>9.90</price>
  </item>
</shiporder>
```

The schema of above XML file is defined here [shiporder.xsd](1).
the root element of this XML is "shiporder". It contains three child elements,
 - orderperson: Name of the person
 - shipto     : Address to ship
 - item:      : Details of items in the order. This element can occur from 1 to 10 times in the XML.

 This example uses lihked-list to store the content `item` element.

```C
typedef struct item_t item_t;

//! Structure to store content of item element
struct item_t
{
    char* title;        //!< Holds content of title element
    char* note;         //!< Holds content of note element
    uint32_t quantity;  //!< Holds content of quantity element
    float price;        //!< Holds content of price element
    item_t* Next;       //!< Pointer to hold the address of next node (item) in the linked list.
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

```

item_t structure contains the pointer to hold the next node of item_t.

The example contains

```C
static void* allocate_item(uint32_t occurrence)
```

A simple and tiny XML parser library in C. It is specifically developed for embedded applications in mind.

- It is extremely easy to use: You need to call only one API to parse your XML data
- It has a very small footprint: The parser uses only 1.8 kB[^1] of code memory. Hence, you can use it in very small embedded applications.
- It is a validating XML parser.
- It also extracts the content of XML data and converts it to its specified data type.
- It comes with a tool to generate the source code from XML schema file, instead of manually writing XML tree structure in C.

>[^1]: Compiled in IAR ARM 8.50.4 compiler in release mode.

This is a validating XML parser, uses [xs_element_t](#xs_element_t) structure based on XML schema to validate the given XML string.
[xs_element_t](#xs_element_t) contains all the tree structure of XML elements and elements properties such as element name, its child elements, attributes, content type, etc..
A user can either manually create or use [XML code generator tool](#xml-code-generator-tool) to generate [xs_element_t](#xs_element_t) structure in C from XML schema file.
This is a command line tool to that takes schema file as input and generates c source code containing.

The Parser contains four files
- parse_xml.c   : XML parser source file
- parse_xml.h   : XML parser header file
- xml_content.c : XML content extractor source file (used by XML parser internally to extract the XML content).
- xml_content.h : XML content extractor header file

## Quick start guide

This is a simple XML data that user wants to parse.

```XML
<food>
    <name>Belgian Waffles</name>
    <price>5.95</price>
    <description>Two of our famous Belgian Waffles with plenty of real maple syrup</description>
    <calories>650</calories>
</food>
```

This is a XML schema for the above XML data.
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
And the user wants to extract the XML data into below C structure.

```C
struct food_t
{
    char* name;
    float price;
    char* description;
    unsigned int calories;
};
```

Call `parse_xml` API, pass XML data and instance of [xs_element_t](#xs_element_t) of root element.

```C
  xml_parse_result_t result = parse_xml(&xml_root, xml_source);
```

- [xml_root](example/food/src/food.c) is an instance of [xs_element_t](#xs_element_t) structure that represents XML root element.
  [xs_element_t](#xs_element_t) also holds the address to store the extracted XML data.
- `xml_source` NULL terminated (char*) string containing XML data to parse.

`parse_xml` function parses the XML string, validate it against the XML schema and extract XML content.
This parser doesn't contain any static or non-local variables, hence it doesn't need any initialization function.
It also doesn't use heap/dynamic memory for extracting the XML content.
It's a [pure function](1) without having any [side effects](2).

### Callbacks from parser
User can specify optional callback in the [xs_element_t](#xs_element_t).
This is useful for complex use cases,
 - Where user wants to perform some action after parsing an XML element
 - Allocate/deallocate memory to store extracted contents.

## XML Code generator tool

Creating the [xs_element_t](#xs_element_t) tree structure manually is cumbersome, hence the parser also provides an xml_code_generator tool that generates xs_element_t tree structure for the user.
This tools generates the xs_element_t tree structure from XML schema. It also generates the structure to store the extracted data.

```shell
xml_code_generator.exe food.xsd
```

The tool generates three files
- [food.c](example/food/src/food.c)             : This file contains xs_element_t tree structure generated from schema. It also declare `food_t food;` to store the XML content.
- [food.h](example/food/src/food.h)             : This file contains food_t structure created from schema to store the content of XML.
- [food_print.c](example/food/src/food_print.c) : Generates the printf functions to print the content of food_t to console.

include food.h file in you source file and call parse_xml api as shown below.

```C
  xml_parse_result_t result = parse_xml(&food_root, xml_source);
```

For most of the cases you only need XMl schema and the tool will generate all the required code to parse an XML data.

## xs_element_t

This structure represent xml schema element for the parser. It is equivalent to XML schema.
For every XML element, you need to define an instance of xs_element_t. They also need to be structured similarly to tree structure of XML elements.

```C
//! Structure to define element of XML
struct xs_element_t
{
  string_t Name;                    //!< Name of an element
  uint32_t MinOccur;                //!< Minimum number of time element must occur
  uint32_t MaxOccur;                //!< Maximum number of time element is allowed to occur
  element_callback Callback;        //!< Callback after successful parsing of an element.

  target_address_t Target;          //!< Target address to store content of an element
  xml_content_t Content;            //!< Content type of an element

  uint32_t Attribute_Quantity;      //!< Number of attributes in the element
  const xs_attribute_t* Attribute;  //!< Address to array of attributes

  uint32_t Child_Quantity;          //!< Number of child elements of an element
  child_order_type_t Child_Order;   //!< order type of child elements
  const xs_element_t* Child;        //!< Address to array of child elements
};
```

- `string_t Name;` represents name of element
- `uint32_t MinOccur` & `uint32_t MaxOccur;` represents XSD occurrence indicators minOccurs & maxOccurs.
- `element_callback Callback;` Callback after successful parsing of an element. Parser calls this callback after completion end tag of an element.

 for more details refer Doxygen documentation.

### More
For reporting issues/bugs or requesting features use GitHub issue tracker


[1]: https://en.wikipedia.org/wiki/Pure_function
[2]: https://en.wikipedia.org/wiki/Side_effect_(computer_science)
