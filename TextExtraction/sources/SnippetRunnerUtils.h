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

 this file contains the abstraction over the snippet runner.It is a singleton.


*********************************************************************/
#ifndef _SnippetRunnerUtils_h_
#define _SnippetRunnerUtils_h_
#include <iostream>

#include "SnippetRunner.h"
#include "PDExpT.h"

#include "CDocument.h"

#ifdef ACRO_SDK_PLUGIN_SAMPLE
#ifndef UNIX_PLATFORM
#include "DebugWindowOStream.h"
#endif
#endif

#define SDKBasePref 250

/** Priovides simple snippet utilitiy code for setting and detecting "context". Clients of this code are typically snippets that
	wish to query/obtain some frequently used state (such as the first page of the front most document). 
*/
class SnippetRunnerUtils
{
protected:
	SnippetRunnerUtils();
public:
	/** this is a singleton class, the factory method.
		@return the single instance of the class. All clients share this instance.
	*/
	static SnippetRunnerUtils * Instance();
	/** this method deletes the single instance.
		A more robust solution would use reference counting to determine
		when it is safe to delete the object.
	*/
	static void deleteInstance();

	virtual ~SnippetRunnerUtils();

	/** provides a single API that will attempt to open an example file from SnippetRunner's example file
		directory (that the developer should copy to the application's plug-in directory).
		@param filename IN the name of the file we want to open
		@return the opened file, NULL if the file does not exist.
	*/
	static ASFile openSnippetExampleASFile(const char * filename);
	
	/** turns on/off the check mark for the currently executing snippet's list entry
		@return current (pre-toggle) menu state.
	*/
	static ASBool toggleSnippetCheck();
	

	/** returns the ASPathName associated with the SnippetRunner output directory, currently
		set to 'OutputFiles' under the PDFLSnippetRunner parent directory
		@param fileName IN the intended output file name 
		@return the ASPathName associated with the SnippetRunner output directory - with empty
		param; or the ASPathName associated with the SnippetRunner output directory - with the 
		intended output file name
	*/
	static ASPathName getOutputPath(const char* fileName=NULL);
	
	/** converts in place a DI path to either a Windows or Mac (OS9 style) path.
		Note: This function fails if the character "/" appears in the file name.
		@param the DI style path to convert.
	*/
	static void convertDIPathToPlatformPath (char *fileName);

	/** for the current snippet, enables the parameter dialog
	*/
	static void turnParamDialogOn();
	/** for the current snippet, suppresses the parameter dialog
	*/
	static void turnParamDialogOff();

	/** for the current snippet, toggles whether the parameters dialog is shown
	*/
	static ASBool toggleParamDialog();

	/** returns the currently active Snippet, this may be the last snippet executed */
	static SnipRun * getCurrentSnippet();
	
	/** takes a fixed rect and dumps the co-ordinates to DebugWindow. You can pass in an optional text preamble.*/
	static void DumpRectToDebugWindow(const char * preamble,ASFixedRectP rec);
		
	/** Provides a case insensative comparison. 
		@param a IN first string to compare
		@param b IN second string to  compare
		@return true if the two strings are equal, otherwise, false.
	*/
	static int stringsMatch(string a, string b);

	/** Check if the MarkInfo dictionary is market for Tagged PDF
		@param pdDoc IN the open document
		@param bSetMarked IN flag to indicate if the caller wants to set 
		the Marked key in the MarkInfo dictionary
	*/
	static ASBool PDDocIsMarkedPDF(PDDoc pdDoc, ASBool bSetMarked);

	friend std::ostream& operator << (std::ostream& output,ASText msg);

#ifdef ACRO_SDK_PLUGIN_SAMPLE

	/** provides a single API that will attempt to open a pdf document (and return the appropriate AVDoc). 
		@return the opened file, NULL if the file does not exist.
	*/
	static AVDoc openSnippetExampleAVDoc(const char * filename);

	/** given a particular parent menu, this function will add a set of submenus as defined by the 
		colon delimitered string. 
		@param parent IN the parent menu to add the submenus to
		@param name IN the names of the sub-menus delimitered by colons. For example: "A:B:C" will result in
		two new submenus under the parent, along with a final menu item called "C". The ui names for the menu entities
		are the concatentation of the uiname of the parent menu(s) with the menu name of the child (this ensures the uinames
		are unique). If a particular menu already exists, a new submenu is not created.
		@return the final menu item at the root of the tree. If an exception is encountered building a methid, this function return NULL.
	*/
	static AVMenuItem buildNewMenu(AVMenu parent,string name);

	/** this function returns the index of a menu a submenu should be placed to ensure its title is in alphabetical order relative to its peers
	*/
	static ASInt32 getNewMenuPos(AVMenu parent,const char * title, ASBool &exists);

#else

	/** Initialises the library. This is a single shot action, the library is only initialised the intial
		time, all subsequent calls do nothing
		@return true if the library wasn't already initialised, false if it was.
	*/
	static ASBool initLibrary();
	/** Terminates the library. This is a single shot action, the library is only terminated once, all subsequent calls
		(without an init call) do nothing.
		@return true of the library wasn't previously terminated, false otherwise.
	*/
	static ASBool termLibrary();

#endif // if plugin else library


	/** Allows a snippet to force the display of the debug window. Does nothing with PDFL
	*/
	static void ShowDebugWindow();

	/** flushes the text from the debug window (clears the text) */
	static void DebugWindowFlush(void);

	/** Convert ACE string to ASCII string or ASText
		@param aceStr IN ACE string
		@param desc OUT ASCII string
		@param asTextStr OUT ASText 
		@param isUnicode OUT, if true, asTextStr is filled, otherwise desc is filled
		@return true if success, otherwise false
	*/
#if !READER_PLUGIN
	static ASBool GetStringFromACString (AC_String *aceStr, char *desc, ASText *asTextStr, ASBool *isUnicode, const ASInt32 len);
#endif
	/** Get ACE rendering intent from ASAtom 
		@param intent IN ASAtom type
		@return ACE rendering intent type
	*/
	static AC_RenderIntent GetRenderIntent(ASAtom intent);
private:
#ifdef ACRO_SDK_PLUGIN_SAMPLE
	/** ptr to the original buffer, we need to restore it. */
	static streambuf * oldClogWB;
#ifndef UNIX_PLATFORM
	static DebugWindowBuf * newClogWB;
#endif
	static streambuf* oldCoutWB;
#endif

	/** a pointer to us, so we can be a singleton */
	static SnippetRunnerUtils* gSnippetRunnerUtils;

};

#endif // _SnippetRunnerUtils_h_



