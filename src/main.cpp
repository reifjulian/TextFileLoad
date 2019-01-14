/////////////////////////////////////////////////////////////////////////////
// Author: Julian Reif, 2007
//   -Updated to correct snprintf(%ld) formatting for longs, July 19, 2010
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
// This file provides an example of how to implement the TextFileLoad class.
// Full documentation is provided in TextFileLoad.h
//
// To compile this example under Cygwin:
// 		g++ TextFileLoad.h TextFileLoad.cpp main.cpp -o main.exe
//
// To run this example under Cygwin:
//		./main
/////////////////////////////////////////////////////////////////////////////

#include "TextFileLoad.h"
#include <stdio.h>
#include <vector>

int main()
{
	string tmp;
	
	//Load the text file
	TextFileLoad TFLobj("sample_text.tab");

	//Display field names
	vector<string> fn = TFLobj.getFieldNames();
	for(int i = 0; i<fn.size();i++)
	{
		tmp = fn[i];
		printf("Field name %d is %s\r\n",i+1,tmp.c_str());
	}

	printf("\r\nThe number of fields is %ld\r\n\n", TFLobj.getFieldCount());

	//Display field types
	vector<string> ft = TFLobj.getFieldTypes();
	for(int i = 0; i<ft.size();i++)
		printf("Field type of column %d is %s\r\n",i+1,ft[i].c_str());

	printf("\r\nThe number of data rows is %ld\r\n", TFLobj.getRowCount());

	//Load the "string data" column into a vector.
	//Note that the last row has weird characters embedded in the file
	vector<string> string_data;
	TFLobj.getField("string data",string_data);
	for(int i = 0; i<string_data.size();i++)
		printf("\r\nString data for row %d is %s",i+1,string_data[i].c_str());

	//Load column 1
	//Note that although column 1 is integer, it can be loaded into the string vector
	TFLobj.getField(1,string_data);
	printf("\r\n");
	for(int i = 0; i<string_data.size();i++)
		printf("\r\nYear data for row %d is %s",i+1,string_data[i].c_str());

	//TextFileLoad understands scientic notation
	vector<double> double_data;
	TFLobj.getField("double data", double_data);
	printf("\r\n\r\n%e\r\n",double_data[1]);

	return 0;
}
