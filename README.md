XML Parser in C
===============

A simple and tiny validating XML parser library in C. It is specifically developed for embedded applications in mind.

- It is extremely easy to use: You need to call only one API to parse your XML data
- It has a very small footprint: The parser uses only 1.8 kB[^1] of code memory. Hence, you can use it in very small embedded applications.
- It is a validating XML parser.
- It also extracts the content of XML data and converts it to its specified data type.
- It comes with a tool to generate the source code from XML schema file, instead of manually writing XML tree structure in C.

>[^1]: Compiled in IAR ARM 8.50.4 compiler in release mode.

This is a validating XML parser. It uses [xs_element_t](#xs_element_t) structure based on XML schema to validate the given XML string.
[xs_element_t](#xs_element_t) contains all the tree structure of XML elements and elements properties such as element name, its child elements, attributes, content type, etc..
A user can either manually create or use [XML code generator tool](#xml-code-generator-tool) to generate [xs_element_t](#xs_element_t) structure in C from XML schema file.
This is a command line tool, it takes schema file as input and generates c source code containing.

The Parser contains four files
- parse_xml.c   : XML parser source file
- parse_xml.h   : XML parser header file
- xml_content.c : XML content extractor source file (used by XML parser internally to extract the XML content).
- xml_content.h : XML content extractor header file

## parse_xml
To parse XML data you need to call only one API.

```C
xml_parse_result_t parse_xml(const xs_element_t* root, const char* source, void* const target
							, void* context);
```
Where,
- *root*: instance of [xs_element_t](#xs_element_t) structure of an XML root element.
- *source*: NULL terminated (char*) string containing XML data to parse.
- *target*: Target address to store content of an XML data.
Optional, Used only when [Target addressing method](#target-addressing-method) of root element is *relative*.
pass *NULL* if not used.
- *context*: Optional user defined context. Parser doesn't use/modify this argument for parsing purpose.
It passes this context in all the callback functions. If not required pass *NULL*.

`parse_xml` function parses the XML string, validate it against the XML schema and extract XML content.
This parser doesn't contain any static or non-local variables, hence it doesn't need any initialization function.
It's a [pure function](1) without having any [side effects](2).

The *parse_xml* function returns after completion of given XML data. It doesn't perform partial parsing.
The parser can also trigger callbacks, if you need to perform some action during the parsing such as,
- before extracting the content of an element. e.g. to allocate memory to store the XML content
- After successfully parsing of an element. e.g. to consume the XML content of an element or to deallocate the memory.

You need to set these callback in the [xs_element_t](#xs_element_t) structure of an element.

## xs_element_t

This structure represent XML schema element for the parser. It is equivalent to XML schema. It contains all the validation rules for an XML element.
- Name: It specifies name of XML element
- MinOccur, MaxOccur: minimum & maximum occurrence of an element in the XML
- Callback: Optional callback function to be called by parser during the parsing of XML.
The callback is called when parser successfully completes the parsing of an element.
- Target: Target address to store the content of an element. There are three types of target addressing methods: static, dynamic and relative.
- Content: Specifies the type of element content. It also contains the other restriction of an content such as minimum value and maximum value.
- Attribute_Quantity: Number of attributes of an element
- Attribute: Pointer to the address of an array of attributes of an element.
- Child_Quantity: Number of child elements of an element
- Child: Pointer to the address of array of child elements of an element.


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

For every XML element, you need to define an instance of *xs_element_t*. They also need to be structured similarly to tree structure of XML elements.

## XML Code generator tool
Creating the [xs_element_t](#xs_element_t) tree structure manually is cumbersome, 
hence the parser also provides an [xml_code_generator][7] tool that generates *xs_element_t* tree structure for the user.
This tools generates the *xs_element_t* tree structure from XML schema. It also generates the structure to store the extracted data.

```shell
xml_code_generator.exe food.xsd
```

The tool generates two files
- [food.c](example/food/src/food.c)    : This file contains *xs_element_t* tree structure generated from schema.
- [food.h](example/food/src/food.h)    : This file contains *food_t* structure created from schema to store the content of XML.

For most of the cases you only need XMl schema and the tool will generate all the required code to parse an XML data.
For more details refer [tool/xml_code_generator][7]

## Demo
- [food][3] : Very simple example to demonstrate parsing of an XML data.
- [shiporder1][4]: Another example to demonstrate parsing of an XML data using relative target addressing method.
- [shiporder2][5]: Parsing of an XML data using mix of relative and dynamic target addressing method
- [shiporder3][6]: Example to demonstrate use of linked-list to store content of multiple occurrence of an XML element.

## Detailed documentation

### Target addressing method
The parser extract the content to the specified target address. There are multiple ways of specifying target address to store XML content.
- Specifying target address at compile time
- Specifying target address at run time
- Specifying target address relative to (or offset from) the target address of parent element

The parser supports above three types of target addressing methods. Structure *target_address_t* defines the target address to store the XML content.

- **static**:
In this method target address to store XML content is specified at the compile time.
This is the simplest method of specifying target address to store XML content.
Parser directly stores the extracted content of XML to the specified target address.
For more details refer [food][3]

- **dynamic**:
In this method parser calls allocate callback function to get the target address to store XML content.
Use this method to specify target address to extract XML content at runtime.
e.g. using malloc()/calloc() to store the XML content on heap memory
For more details refer [shiporder2][5]

- **relative**:
In this method the target address is calculated by adding offset to target address of parents element.
When you want to store XML content on dynamic memory, allocating dynamic memory for every element in the XML is impractical.
e.g. If you have one parent element that has three child elements a, b, c of type integer.
Instead of allocating dynamic memory for each child element, declare a structure that contain three member variables.
Now allocate the dynamic memory for the structure in the parent element using dynamic memory addressing type.
And specify the offset of the member variable from the structure in the child element using `offsetof()` macro.
For more details refer [shiporder2][5]

### XML content
The parser supports most common data types of XML content such as string, integer, bool, ...
*xml_content_t* structure defines the supported XML content types and its restrictions/facets.
For more details refer the Doxygen documentation.

### Configuration of XML parser
By default parser disables context and element callback to save code space.
- **Context**:
Enable user defined context argument in parse_xml function by defining **XML_PARSER_CONTEXT** to 1.
Use compiler -D option to set these macros, e.g. `-DXML_PARSER_CONTEXT="1"`.

- **Callback for XML Element**:
Parser calls this callback when it completes the parsing of an XML element.
Enable element callback by defining **XML_PARSER_CALLBACK** to 1.
Use compiler -D option to set these macros, e.g. `-DXML_PARSER_CALLBACK="1"`

### More
- For reporting issues/bugs or requesting features use [GitHub issue tracker][8]


[1]: https://en.wikipedia.org/wiki/Pure_function
[2]: https://en.wikipedia.org/wiki/Side_effect_(computer_science)
[3]: example/food/README.md
[4]: example/shiporder1/README.md
[5]: example/shiporder2/README.md
[6]: example/shiporder3/README.md
[7]: tool/README.md
[8]: https://github.com/kiishor/xml_parser_in_c/issues
