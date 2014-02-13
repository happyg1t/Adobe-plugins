/*********************************************************************

 ADOBE SYSTEMS INCORPORATED
 Copyright (C) 1998-2006 Adobe Systems Incorporated
 All rights reserved.

 NOTICE: Adobe permits you to use, modify, and distribute this file
 in accordance with the terms of the Adobe license agreement
 accompanying it. If you have received this file from a source other
 than Adobe, then your use, modification, or distribution of it
 requires the prior written permission of Adobe.

 ---------------------------------------------------------------------

The basic console interaction

*********************************************************************/
#ifndef _SRConsole_h_
#define _SRConsole_h_
#include <iostream>

#include <string>
#include "ASCalls.h"
#include "CosCalls.h"

using namespace std;


/** this is the main Console abstraction. I have factorised this into an independent class because I want a  
	single place that all IO will occur. This will allow me to add common patterns (tokenisation etc.) as a
	service to all interested parties.
*/

class Console {
	/** ctor */
	Console(){};
	/** dtor */
	virtual ~Console();
public:	
	/** simple text out */
	static const void displayString(string output);
	static const void displayStringNow(string output);

	/**  Displays a (char *) string. Caller is responsible for msg memory. */
	static void displayString(const char *msg);
	static void displayStringNow(const char *msg);

	/** displays the data that is in memory to the DebugWindow. numberOfInts are displayed */	
	static void  displayString(void *msg, ASInt32 numOfInts);

	/** display notification strings with Unicode encoding */
	static void displayStringNow(const ASUTF16Val* msg);

	/**  Displays a (ASText) string.  Caller is responsible for msg memory*/
	static void displayString(ASText msg);
	static void displayStringNow(ASText msg);

	/** Displays diagnostic information for the CosObj passed in. This is designed only for simple objects */
	static void displayString(CosObj thisObj, ASBool brief = true);
	
	/** Dumps the CosObj (prepended with the preamble) to trace */
	static void CosObjDump(CosObj thisObj, char * preamble = "");


	/** simple text in */
	static string getString();
	
	/** memory flush */
	static void flush();
	/** dump data to a file*/
	static ASBool saveAsFile(ASPathName pathName, char *data);
};


#endif


