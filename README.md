XML Parser in C
===============

A simple and small XML parser library in C. It is specifically developed for embedded applications in mind.

- It is extremely easy to use: You need to call only one API to parse your XML data
- It is very small: The parser uses only 1.8 kB[1] of code memory. Hence you can use it in very small embedded applications.
- It is a validating XML parser.
- It also extracts the content of XML data and converts it to its specified data type.

>[1]: Compiled in IAR ARM 8.50.4 compiler in release mode. 

## How it works

```XML
<food>
    <name>Belgian Waffles</name>
    <price>5.95</price>
    <description>Two of our famous Belgian Waffles with plenty of real maple syrup</description>
    <calories>650</calories>
</food>
```

You have simple XML data as shown above that you want to parse and extract the content to below structure.

```C
struct food_t
{
    char* name;
    float price;
    char* description;
    unsigned int calories;
};
```

You can call \ref parse_xml API and pass XML data and instance of xs_element_t of root element.

```C 
  xml_parse_result_t result = parse_xml(&food_root, xml_source);
```

`food_root` is an instance of `xs_element_t` structure that represents XML root element food. `xs_element_t` represent the XML element like element name, its child elements, attributes, content type, etc..
It is equivalent to XML schema. For every XML element, you need to define an instance of xs_element_t. They also need to be structured similarly to tree structure of XML elements.

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

Why does parser uses xs_element_t for validation instead of directly using XML schema?
 - Using XML schema directly costs more code space compare to xs_element_t
 - Parser also needs to parse XML schema that also adds the cost to performance.
 
## XML code generator tool

Creating the xs_element_t tree structure manually is cumbersome, hence the parser also provides an xml_code_generator tool that generates this for the user.
This tools generates the xs_element_t tree structure from XML schema. It also generates the structure of food_t to store the extracted data.

```shell
xml_code_generator.exe food.xsd
```

The tool generates three files
- **food.c** :    This file contains xs_element_t tree structure generated from schema. It also contains `food_t food;` to store the XML content.
    
- **food.h** : This file contains food_t structure created from schema to store the content of XML.
    
- **food_print.c** : Generates the printf functions to print the content of food_t to console.

include food.h file in you source file and call parse_xml api as shown below.
```C 
  xml_parse_result_t result = parse_xml(&food_root, xml_source);
```

- food_root: It is an instance of xs_element_t of root element defined in the food.c 
- xml_source: NULL terminated string containing XML data to parse.

For most of the cases you only need XMl schema and the tool will generate all the required code to parse an XML data.

## parse_xml

### xs_element_t

This structure represent xml schema element for the parser. 

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
   

