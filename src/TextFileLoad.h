#ifndef __TEXTFILELOAD_H
#define __TEXTFILELOAD_H
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

/////////////////////////////////////////////////////////////////////////////
//
// TextFileLoad is an ANSI-compliant class that allows a user to easily import a text file.
// Data can be loaded by column name or number. Loading by name is advantageous because it
// allows the order of the columns in the input file to change without any subsequent
// effect on the analysis.
//
// This class automatically does type conversions. A user is allowed, for example, to load
// a column of integers into a vector of strings. In cases where there is no logical
// conversion (e.g., loading a column of strings into a vector of booleans),
// the data are converted to 0's.
//
// USER OPTIONS
// There are several options available to the user when importing the data:
// 1) delimiter (default is tab-delimited)
// 2) header row (default assumes first row is the header row)
// 3) Load by column number or column name
//		--If loading by column name, user can specify case sensitivity (default is no case sensitivity)
//
//
// EXAMPLE CLASS INITIALIZATIONS
//		1. (tab file): TextFileLoad TFLobj("sample text.tab");
//		2. (csv file): TextFileLoad TFLobj("sample text.csv", ",");
//		3. (tab file, no header row): TextFileLoad TFLobj("sample text.tab", false);
//		4. (csv file, no header row): TextFileLoad TFLobj("sample text.csv", ",", false);
//
//
// EXAMPLE DATA LOADS
//		1. (load "var1" column, no case sensitivity): TFLobj.getField("var1",my_vector);
//		2. (load "var2" column, case sensitive): TFLobj.getField("var1",my_vector, true);
//		3. (load third column of data): TFLobj.getField(3,my_vector);
//
//
// KNOWN ISSUES
// 1) This code currently assumes a Unix end of line format (i.e., "\r\n").
//	  Because Windows and Unix denote end of lines differently, if running under Windows, two
//    things would need to be changed in this code:
//		A) all printf() statements with a "\r\n" may need to be changed to "\n"
//		B) the value of the variable, "offset", in the method
//		   vector<string> _splitString(string str, char delimit) MUST be changed from -1 to 0
//
//
// RECOMMENDED FUTURE IMPROVEMENTS
// 1) Implement the autodetection of the OS to address issue #1 mentioned above. I began doing
//    this but Cygwin was reporting itself as "Windows NT" so I gave up.
// 2) Implement an elegant template structure to bypass the numerous overloads of the
//    getField() method.
//
/////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>

using namespace std;

//Enumeration is used as a value label for data types
enum _VT_TYPE {_VT_INT, _VT_LONG, _VT_DOUBLE, _VT_BOOL, _VT_STRING};

/*
CREATE VARIANT STRUCTURE
This structure, a simplified version of _variant_t, allows all types of data to be
held in one single object.
*/
struct variant
{
	bool vt_bool;
	int vt_int;
	long vt_long;
	double vt_double;
	string vt_string;

	//vt_type specifies which of the above properties holds a valid value of interest
	_VT_TYPE vt_type;
};

class TextFileLoad
{

private:
	//PRIVATE MEMBERS
	char delimiter;
	bool header_row;
	string filename;
	vector<string> field_names;
	vector<_VT_TYPE> field_types;
	ifstream in_stream;
	vector < vector <variant> > data;
	long field_count;
	long row_count;
	int offset; // Determined by end-of-line formatting for text file. Used by _splitString.

	//PRIVATE METHODS
	void _openFile(void);
	void _getFieldNames(void);
	void _getFieldTypes(void);
	void _getData(void);
	int _getColNum(string column_name, bool case_sensitive);
	vector<string> _splitString(string str, char delimit); //This method needs to be modified if running under Windows
	string _trim(string str);
	string _toUpper(string str);
	int _getType(string str);
	bool _isDouble(string str);
	bool _isLong(string str);

public:
	//CONSTRUCTORS AND DESTRUCTOR
	TextFileLoad(string textfile, bool header_row=true);
	TextFileLoad(string textfile, char delimit, bool header_row=true);
	~TextFileLoad(void);

	//PUBLIC METHODS
	vector<string> getFieldNames(void);
	vector<string> getFieldTypes(void);
	bool existsFieldName(string name, bool case_sensitive=false);
	long getFieldCount(void);
	long getRowCount(void);
	//Overloaded getField methods
	//1) get by field name
	void getField(string field_name, vector <bool>& col_data, bool case_sensitive=false);
	void getField(string field_name, vector <int>& col_data, bool case_sensitive=false);
	void getField(string field_name, vector <long>& col_data, bool case_sensitive=false);
	void getField(string field_name, vector <double>& col_data, bool case_sensitive=false);
	void getField(string field_name, vector <string>& col_data, bool case_sensitive=false);
	//2) get by column number
	void getField(int col_num, vector <bool>& col_data);
	void getField(int col_num, vector <int>& col_data);
	void getField(int col_num, vector <long>& col_data);
	void getField(int col_num, vector <double>& col_data);
	void getField(int col_num, vector <string>& col_data);
};
#endif
