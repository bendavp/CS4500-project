# main.cpp

This file handles parsing CLI arguments withthe OptParser and handing them off
to the SoRParser.

# SoRParser

This class handles parsing and performing basic read operation on schema-on-read
files.

Schema-on-read files consist of cells formatted as <[content]>, with rows of
data separated by line breaks. Content can either be booleans, integers,
floats, or strings with the most restrictive datatype taking precedence.
Whitespace is not allowed in strings unless the string is enclosed in quotes.
The first 500 lines of a SoR file are used to determine the data's schema,
with the most common datatype of each column used to determine the column's
content.

The constructor of this class just initializes variables - the meat of the parsing
is done with the initialize method, whihc looks at the head of the given file for
the schema.

It provides methods for modifying the arguments, as well as getting the data out in
columns.

# OptParser

This functions works like getopt, but allows for multiple arguments per option.


# StrConverter

Utility class for converting between data types and checking for valid conversions.
