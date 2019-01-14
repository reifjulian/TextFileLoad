# TEXTFILELOAD: import data from text files in a user-friendly manner

- Current version: `1.0.1 10jul2010`
- Jump to: [`updates`](#recent-updates) [`install`](#install) [`description`](#description) [`author`](#author)

-----------

## Updates:

* **July 19, 2010**
  - Updated to correct snprintf(%ld) formatting for longs

## Install:

The source code is stored in /src

## Description: 

TextFileLoad is an ANSI-compliant class that allows a user to easily import a text file. Data can be loaded by column name or number. Loading by name is advantageous because it allows the order of the columns in the input file to change without any subsequent effect on the analysis.

This class automatically does type conversions. A user is allowed, for example, to load a column of integers into a vector of strings. In cases where there is no logical conversion (e.g., loading a column of strings into a vector of booleans), the data are converted to 0's.

**USER OPTIONS**:

There are several options available to the user when importing the data:
1. delimiter (default is tab-delimited)
2. header row (default assumes first row is the header row)
3. Load by column number or column name
  - If loading by column name, user can specify case sensitivity (default is no case sensitivity)

## Author:

[Julian Reif](http://www.julianreif.com)
<br>University of Illinois
<br>jreif@illinois.edu
