/////////////////////////////////////////////////////////////////////////////
// Author: Julian Reif, 2010
//  --Added autodetection of end-of-line formatting.
//	--Support for reading numbers in scientific notation added by Ben Lewis, 2007
//
// Terms of Agreement: By using this code, you agree to the following terms...
// 1) You may use this code in your own programs (and may compile it into a program and distribute
//    it in compiled format for languages that allow it) freely and at no charge. 
// 2) You MAY NOT redistribute this code (for example to a web site). Failure to do so is a
//    violation of copyright laws.
// 3) You use this code at your own risk.
/////////////////////////////////////////////////////////////////////////////

#include "TextFileLoad.h"

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////

/*
This constructor assumes a tab-delimited file. Default argument for headers is true.
*/
TextFileLoad::TextFileLoad(string textfile, bool headers)
{
	//Set member properties
	filename = textfile;
	delimiter = '\t';
	header_row = headers;

	//Load file data
	_openFile();
	_getFieldNames();
	_getFieldTypes();
	_getData();
}

/*
This constructor allows the user to specify the delimiter. Default argument for headers is true.
*/
TextFileLoad::TextFileLoad(string textfile, char delimit, bool headers)
{
	//Set member properties
	filename = textfile;
	delimiter = delimit;
	header_row = headers;

	//Load file data
	_openFile();
	_getFieldNames();
	_getFieldTypes();
	_getData();
}

/*
The destructor ensures that the file input stream is closed.
*/
TextFileLoad::~TextFileLoad(void)
{
	in_stream.close();
}


/////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
/////////////////////////////////////////////////////////////////////////////

/*
Opens the input file stream and issues an error if the file fails to open.
Detect what the end-of-line formatting is.
*/
void TextFileLoad::_openFile(void)
{
	string full_row;
	int next_pos;
	in_stream.open(filename.c_str());

	if(!in_stream)
	{
		printf("\n\nERROR: file failed to open!\n\n");
		exit(1);
	}
	
	// Windows end-of-line files do not have a '\r'. This affects extraction of the data
	// in _splitString. Set offset equal to 0 if there is no '\r' and one equal to 1 otherwise.
	getline(in_stream, full_row);
	next_pos = (int)full_row.find('\r'); //returns -1 if nothing found
	if (next_pos==-1) offset=0;
	else offset = 1;
	
	// Reset stream pointer to beginning of file
	in_stream.clear();
	in_stream.seekg (0, ios::beg);	
}

/*
Stores the field names in a vector of strings, if a header row was specified. Otherwise,
just calculates the number of fields.
*/
void TextFileLoad::_getFieldNames(void)
{
	string first_line;
	getline( in_stream, first_line );
	if(first_line.length() == 0)
	{
		printf("\nFirst row is empty!\n");
		exit(1);
	}
	field_names = _splitString(first_line, delimiter);
	field_count = field_names.size();
	if(!header_row)
	{
		//There are no field names, so reset the ifstream pointer to the beginning of the file
		field_names.clear();
		in_stream.clear();
		in_stream.seekg(0,ios::beg);
	}
}

/*
Determines the variable type for each field based on all the data in each field. The type is
chosen to be as restrictive as possible while making sure not to lose any information. Thus, a
column of all 1's and 0's will be stored as boolean, but a column that has one million
0's and one string will be stored as string.
*/
void TextFileLoad::_getFieldTypes(void)
{
	string tmp, datum;
	vector<string> data, one_split_row;
	_VT_TYPE type;

	//Read in the entire dataset
	while(!in_stream.eof())
	{
		getline( in_stream, tmp );
		if(tmp.length()==0) //Skip empty lines
			continue;
		else
			data.push_back(tmp);
	}

	//Methodically step through and determine the type for each column
	for(int i = 0; i < field_count; i++)
	{
		//bool is most restrictive type, so that will be the default
		type = _VT_BOOL;
		for(int j = 0; j < data.size(); j++)
		{
			one_split_row = _splitString(data[j],delimiter);
			datum = one_split_row[i];
			switch(_getType(datum))
			{
				case _VT_BOOL:
					break;

				case _VT_INT:
					if(type==_VT_BOOL)
						type = _VT_INT;
					break;

				case _VT_LONG:
					if(type==_VT_BOOL || type==_VT_INT)
						type = _VT_LONG;
					break;

				case _VT_DOUBLE:
					if(type!=_VT_STRING)
						type = _VT_DOUBLE;
					break;

				case _VT_STRING:
					type = _VT_STRING;
			}
		}
		field_types.push_back(type);
	}
}

/*
Loads all the data into the vector of variants. It determines which variant property to store
the data in according to the results of the previously called _getFieldTypes() method.
Note: nulls are automatically stored as blank if column is string and 0 otherwise.
*/
void TextFileLoad::_getData(void)
{
	string full_row;
	vector<string> split_row;
	vector<variant> row_data;

	//Reading starts at line one or two of the file, depending on whether or not there is a header row
	in_stream.clear();
	in_stream.seekg (0, ios::beg);
	if(header_row)
		getline(in_stream, full_row);

	//Read in the data, line by line.
	row_count = 0;
	while(!in_stream.eof())
	{
		getline(in_stream, full_row);
		split_row = _splitString(full_row,delimiter);

		//Skip empty lines
		if(full_row.length()==0)
			continue;

		//Loop over the columns in the row and store the contents into a variant
		for(int col_num = 0; col_num < split_row.size(); col_num++)
		{
			variant tmp;
			switch(field_types[col_num])
			{
				case _VT_BOOL:
					tmp.vt_type = _VT_BOOL;
					tmp.vt_bool = atoi(split_row[col_num].c_str());
					break;

				case _VT_INT:
					tmp.vt_type = _VT_INT;
					tmp.vt_int = atoi(split_row[col_num].c_str());
					break;

				case _VT_LONG:
					tmp.vt_type = _VT_LONG;
					tmp.vt_long = atol(split_row[col_num].c_str());
					break;

				case _VT_DOUBLE:
					tmp.vt_type = _VT_DOUBLE;
					tmp.vt_double = atof(split_row[col_num].c_str());
					break;

				case _VT_STRING:
					tmp.vt_type = _VT_STRING;
					tmp.vt_string = split_row[col_num];
			}
			row_data.push_back(tmp);
		}
		data.push_back(row_data);
		row_data.clear();
		row_count++;
	}
}

/*
Tokenizes a string according and returns the tokens in a vector.
*/
vector<string> TextFileLoad::_splitString(string str, char delimit)
{
	vector<string> results;
	string temp;
	int str_length;
	int first_pos = 0;
	int next_pos = (int)str.find(delimit); //returns -1 if nothing is found
	
	while(next_pos!=-1)
	{
		//Store the token
		str_length = next_pos - first_pos;
		temp = str.substr(first_pos,str_length);
		results.push_back(temp);

		//Find the next token, if any delimiters remain
		first_pos = next_pos+1;
		next_pos = (int)str.find(delimit,first_pos);
	}

	//Grab the final token in the string (or the only token, if there were no delimiters present)
	// If a '/r' is present, we are under windows and need our offset to be 0 instead of 1
	temp = str.substr(first_pos,str.length()-first_pos-offset);
	results.push_back(temp);

	return results;
}

/*
Determines what type a particular string could be converted to.
*/
int TextFileLoad::_getType(string str)
{
	//Nulls could be anything, so let them be the most restrictive type (i.e., boolean)
	if(str.length()==0)
		return _VT_BOOL;

	if(_trim(str)=="0" | _trim(str)=="1")
		return _VT_BOOL;

	else if(!_isDouble(_trim(str)))
		return _VT_STRING;

	else if(!_isLong(_trim(str)))
		return _VT_DOUBLE;

	else
	{
		long tmp_long = atol(str.c_str());
		//If the long takes an absolute value of less than -32768, assume it can be an int
		if(tmp_long < 32768 && tmp_long > -32768)
			return _VT_INT;
		else
			return _VT_LONG;
	}
}

/*
Trims all beginning and ending spaces of a string.
*/
string TextFileLoad::_trim(string str)
{
	string r=str.erase(str.find_last_not_of(" ")+1);
	return r.erase(0,r.find_first_not_of(" "));
}

/*
Determines whether the string is a valid double. This is needed because atof will work on invalid
numbers, e.g. atof(3.23dfs) yields 3.23. We, however, want it to remain a string should that
case arise. The only thing we want trimmed is spaces.
*/
bool TextFileLoad::_isDouble(string str)
{
	bool period_present = false; // only one period allowed per string
	bool e_present = false;		 // only one 'e' (for scientific notation) allowed per string
	int ok_negative_loc = 0;	 // negatives allowed at position 0 and position x+1, where x is position of 'e'
	int ok_positive_loc = -1;	 // positives allowed at position x+1, where x is position of 'e'
	str = _trim(str);

	//Check each individual character
	for(int i = 0; i<str.length(); i++)
	{
		char ch = str[i];

		switch(ch) {
			case '0':
				break;
			case '1':
				break;
			case '2':
				break;
			case '3':
				break;
			case '4':
				break;
			case '5':
				break;
			case '6':
				break;
			case '7':
				break;
			case '8':
				break;
			case '9':
				break;
			case '.':
				if(period_present)
					return false;
				else
					period_present = true;
				break;
			case 'e':
				if(e_present)
					return false;
				else
					e_present = true;
					ok_negative_loc = i+1;
					ok_positive_loc = i+1;
				break;
			case '-':
				if( i != ok_negative_loc )
					return false;
				else
					break;
			case '+':
				if( i != ok_positive_loc )
					return false;
				else
					break;
			default:
				return false;
		}
	}
	return true;
}

/*
Determines whether the string is a valid long. This is needed because atol will work on invalid
numbers, e.g. atol(3.23dfs) yields 3. We, however, want it to remain a string should that
case arise. The only thing we want trimmed is spaces.
*/
bool TextFileLoad::_isLong(string str)
{
	str = _trim(str);

	for(int i = 0; i<str.length(); i++)
	{
		char ch = str[i];
		switch(ch) {
			case '0':
				break;
			case '1':
				break;
			case '2':
				break;
			case '3':
				break;
			case '4':
				break;
			case '5':
				break;
			case '6':
				break;
			case '7':
				break;
			case '8':
				break;
			case '9':
				break;
			default:
				return false;
		}
	}
	return true;
}

/*
Capitalizes a string.
*/
string TextFileLoad::_toUpper(string str)
{

	for(int i = 0; i<str.length(); i++)
	{
		str[i] = toupper(str[i]);
	}
	return str;
}

/*
Returns the column number for a given column name. If column not found, program exits.
*/
int TextFileLoad::_getColNum(string column_name, bool case_sensitive)
{
	if(!case_sensitive)
		column_name = _toUpper(column_name);

	//Determine the relevant column number
	for (int col_num = 0; col_num < field_count; col_num++)
	{
		string col_name;
		if(case_sensitive)
			col_name = field_names[col_num];
		else
			col_name = _toUpper(field_names[col_num]);

		if (column_name == col_name)
		{
			return col_num;
		}
	}
	printf("\nColumn name %s does not exist!\n", column_name.c_str());
	exit(1);
}


/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////


/*
Returns a vector of strings containing the field names for the columns.
*/
vector<string> TextFileLoad::getFieldNames(void)
{
	return field_names;
}

/*
Returns a vector of strings containing the field types for the columns.
*/
vector<string> TextFileLoad::getFieldTypes(void)
{
	vector<string> types;
	string tmp;
	for(int i = 0; i < field_types.size(); i++)
	{
		switch(field_types[i]) {
			case _VT_BOOL:
				tmp = "BOOLEAN";
				break;
			case _VT_INT:
				tmp = "INT";
				break;
			case _VT_LONG:
				tmp = "LONG";
				break;
			case _VT_DOUBLE:
				tmp = "DOUBLE";
				break;
			case _VT_STRING:
				tmp = "STRING";
		}
		types.push_back(tmp);
	}
	return types;
}

/*
Returns true if the string parameter matches the name of an existing field. Default value for
case_sensitive parameter is false.
*/
bool TextFileLoad::existsFieldName(string name, bool case_sensitive)
{
	if(!case_sensitive)
		name = _toUpper(name);

	for(int i = 0; i < field_names.size(); i++)
	{
		if(case_sensitive && name==field_names[i])
			return true;
		else if(name==_toUpper(field_names[i]))
			return true;
	}
	return false;
}

/*
Returns the number of columns in the dataset.
*/
long TextFileLoad::getFieldCount(void)
{
	return field_count;
}

/*
Returns the number of rows in the dataset.
*/
long TextFileLoad::getRowCount(void)
{
	return row_count;
}

/////////////////////////////////////////////////////////////////////////////
// OVERLOADED getField() METHODS
/////////////////////////////////////////////////////////////////////////////
/*
These methods load a column of data from the dataset into a vector. The user is allowed
to load a vector of booleans, ints, longs, doubles, or strings. Which column is loaded
can be specified one of two ways:
	A) User specifies the column name
	B) User specified the column number (1, 2, etc.)
These two different ways require different function overloads, so there are two sets of
overloads.

The getField() methods have been written so that even if a user loads a column of strings
into a boolean vector, an error will not occur. What does occur is outlined below:

User wants to load the data into a vector:

1) Strings
	Since all data can be converted successfully into a string, this is not a problem.

2) Doubles
	If the data is string, a vector of 0's is returned. Otherwise, normal type conversion
	takes place.

3) Longs
	If the data is string, a vector of 0's is returned. Otherwise, normal type conversion
	takes place (e.g., 3.23 becomes 3).

4) Ints
	If the data is string, a vector of 0's is returned. Otherwise, normal type conversion
	takes place (e.g., 3.23 becomes 3 and 100000000 is truncated to its integer version).

4) Bools
	If the data is string, a vector of 0's is returned. Otherwise, normal type conversion
	takes place.
*/

/////////////////////////////////////
// GET BY COLUMN NAME (see (A) above)
/////////////////////////////////////

/*
Overloaded version for BOOLS.
*/
void TextFileLoad::getField(string field_name, vector <bool>& col_data, bool case_sensitive)
{
	//Determine the relevant column number
	int col_num = _getColNum(field_name, case_sensitive);

	getField(col_num+1, col_data);
}

/*
Overloaded version for INTS.
*/
void TextFileLoad::getField(string field_name, vector <int>& col_data, bool case_sensitive)
{
	//Determine the relevant column number
	int col_num = _getColNum(field_name, case_sensitive);

	getField(col_num+1, col_data);
}

/*
Overloaded version for LONGS.
*/
void TextFileLoad::getField(string field_name, vector <long>& col_data, bool case_sensitive)
{
	//Determine the relevant column number
	int col_num = _getColNum(field_name, case_sensitive);

	getField(col_num+1, col_data);
}

/*
Overloaded version for DOUBLES.
*/
void TextFileLoad::getField(string field_name, vector <double>& col_data, bool case_sensitive)
{
	//Determine the relevant column number
	int col_num = _getColNum(field_name, case_sensitive);

	getField(col_num+1, col_data);
}

/*
Overloaded version for STRINGS.
*/
void TextFileLoad::getField(string field_name, vector <string>& col_data, bool case_sensitive)
{
	//Determine the relevant column number
	int col_num = _getColNum(field_name, case_sensitive);

	getField(col_num+1, col_data);
}

///////////////////////////////////////
// GET BY COLUMN NUMBER (see (B) above)
///////////////////////////////////////

/*
Overloaded version for BOOLS.
*/
void TextFileLoad::getField(int col_num, vector <bool>& col_data)
{
	col_num--;
	col_data.clear();

	//Load the data into the vector, and do any necessary type conversions
	switch(field_types[col_num])
	{
		case _VT_BOOL:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back(data[i][col_num].vt_bool);
			}
			break;

		case _VT_INT:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back((bool)data[i][col_num].vt_int);
			}
			break;

		case _VT_LONG:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back((bool)data[i][col_num].vt_long);
			}
			break;

		case _VT_DOUBLE:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back((bool)data[i][col_num].vt_double);
			}
			break;

		case _VT_STRING:
			//If the data is a string, just push back 0's.
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back(0);
			}
	}
}

/*
Overloaded version for INTS.
*/
void TextFileLoad::getField(int col_num, vector <int>& col_data)
{
	col_num--;
	col_data.clear();

	//Load the data into the vector, and do any necessary type conversions
	switch(field_types[col_num])
	{
		case _VT_BOOL:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back(data[i][col_num].vt_bool);
			}
			break;

		case _VT_INT:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back(data[i][col_num].vt_int);
			}
			break;

		case _VT_LONG:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back((int)data[i][col_num].vt_long);
			}
			break;

		case _VT_DOUBLE:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back((int)data[i][col_num].vt_double);
			}
			break;

		case _VT_STRING:
			//If the data is a string, just push back 0's.
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back(0);
			}
	}
}

/*
Overloaded version for LONGS.
*/
void TextFileLoad::getField(int col_num, vector <long>& col_data)
{
	col_num--;
	col_data.clear();

	//Load the data into the vector, and do any necessary type conversions
	switch(field_types[col_num])
	{
		case _VT_BOOL:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back(data[i][col_num].vt_bool);
			}
			break;

		case _VT_INT:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back(data[i][col_num].vt_int);
			}
			break;

		case _VT_LONG:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back(data[i][col_num].vt_long);
			}
			break;

		case _VT_DOUBLE:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back((long)data[i][col_num].vt_double);
			}
			break;

		case _VT_STRING:
			//If the data is a string, just push back 0's.
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back(0);
			}
	}
}

/*
Overloaded version for DOUBLES.
*/
void TextFileLoad::getField(int col_num, vector <double>& col_data)
{
	col_num--;
	col_data.clear();

	//Load the data into the vector, and do any necessary type conversions
	switch(field_types[col_num])
	{
		case _VT_BOOL:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back(data[i][col_num].vt_bool);
			}
			break;

		case _VT_INT:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back(data[i][col_num].vt_int);
			}
			break;

		case _VT_LONG:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back(data[i][col_num].vt_long);
			}
			break;

		case _VT_DOUBLE:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back(data[i][col_num].vt_double);
			}
			break;

		case _VT_STRING:
			//If the data is a string, just push back 0's.
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back(0.0);
			}
	}
}

/*
Overloaded version for STRINGS.
*/
void TextFileLoad::getField(int col_num, vector <string>& col_data)
{
	col_num--;
	col_data.clear();

	//Load the data into the vector, and do any necessary type conversions
	switch(field_types[col_num])
	{
		char conv[14]; //For converting non-string data.

		case _VT_BOOL:
			for(int i = 0; i < row_count; i ++)
			{
				snprintf(conv, sizeof(conv), "%d",data[i][col_num].vt_bool);
				col_data.push_back(conv);
			}
			break;

		case _VT_INT:
			for(int i = 0; i < row_count; i ++)
			{
				snprintf(conv, sizeof(conv), "%d",data[i][col_num].vt_int);
				col_data.push_back(conv);
			}
			break;

		case _VT_LONG:
			for(int i = 0; i < row_count; i ++)
			{
				snprintf(conv, sizeof(conv), "%ld",data[i][col_num].vt_long);
				col_data.push_back(conv);
			}
			break;

		case _VT_DOUBLE:
			for(int i = 0; i < row_count; i ++)
			{
				snprintf(conv, sizeof(conv), "%f",data[i][col_num].vt_double);
				col_data.push_back(conv);
			}
			break;

		case _VT_STRING:
			for(int i = 0; i < row_count; i ++)
			{
				col_data.push_back(data[i][col_num].vt_string);
			}
	}
}
