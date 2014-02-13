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

 SnippetRunnerUtils.cpp

 - Simple utility methods for snippets to use

*********************************************************************/
#pragma warning(disable: 4786)

#include "SnippetRunnerUtils.h"
#include <string>
#include "PDCalls.h"
#include "ASCalls.h"
#include "ASExtraCalls.h"
#include "CosCalls.h"
#ifdef UNIX_PLATFORM
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#endif
using namespace std;

#include <set>

//#ifdef ACRO_SDK_PLUGIN_SAMPLE
//#ifndef UNIX_PLATFORM
//#include "ResizeDebugWindow.h"
//#endif
//#endif

SnippetRunnerUtils* SnippetRunnerUtils::gSnippetRunnerUtils  = NULL;

#ifdef ACRO_SDK_PLUGIN_SAMPLE
// we manipulate the standard IO streams, so we need to maintain the "old" versions
//DebugWindowBuf* SnippetRunnerUtils::newClogWB  = NULL;
streambuf* SnippetRunnerUtils::oldClogWB  = NULL;
streambuf* SnippetRunnerUtils::oldCoutWB  = NULL;
#endif


#ifdef PDFL_SDK_SAMPLE
#ifdef MAC_PLATFORM
#include "macUtils.h"
#endif
#endif

static ASBool fPDFLInited = false;

static string SnipRunFolder;

// define the ostream operator that takes an ASText object here.
std::ostream&
operator << (std::ostream& output,ASText msg) { 
	output << ASTextGetScriptText(msg,kASRomanScript);
	return output; 
}

SnippetRunnerUtils::SnippetRunnerUtils(){
#ifdef ACRO_SDK_PLUGIN_SAMPLE
//	DebugWindow * dbWD = DebugWindow::Instance();  //CC fix all debug win stuff
#endif
}

SnippetRunnerUtils::~SnippetRunnerUtils(){
#ifdef ACRO_SDK_PLUGIN_SAMPLE
//	DebugWindow::DeleteInstance();
#endif
}


SnippetRunnerUtils * 
SnippetRunnerUtils::Instance(){
		if (gSnippetRunnerUtils == NULL)
		{
			gSnippetRunnerUtils = new SnippetRunnerUtils();
			#if 0  //CC
			// set up standard out and log to go to the DebugWindow
			//newClogWB = new DebugWindowBuf(0);
			oldClogWB = std::clog.rdbuf(newClogWB);
			oldCoutWB = std::cout.rdbuf(newClogWB);
			#endif
		}
		return gSnippetRunnerUtils;
	};

void
SnippetRunnerUtils::deleteInstance(){
#if 0 //CC
	if (newClogWB!=NULL){
		std::clog.rdbuf(oldClogWB);
		std::clog.rdbuf(oldCoutWB);
#ifndef MAC_ENV
// why does this crash the mac?
		delete newClogWB;
#endif
		newClogWB = NULL;
	}
#endif
	if (gSnippetRunnerUtils != NULL)
	{
		delete gSnippetRunnerUtils;
		gSnippetRunnerUtils = NULL;
	}
};

void
SnippetRunnerUtils::DebugWindowFlush(){
#ifdef ACRO_SDK_PLUGIN_SAMPLE
//	DebugWindow *dbWD = DebugWindow::Instance();
//	dbWD->DebugWindowFlush();
#endif
}

void 
SnippetRunnerUtils::ShowDebugWindow(){
#ifdef ACRO_SDK_PLUGIN_SAMPLE
//	DebugWindow *dbWD = DebugWindow::Instance();
//	dbWD->ShowDebugWindow();
#endif
}


/* 
returns an ASPathName pointing to the standard output folder, creating it if necessary.
returns NULL if pathname is invalid.
caller must call ASFileSysReleasePath on returned value.
*/
ASPathName 
SnippetRunnerUtils::getOutputPath(const char* fileName){
	ASPathName baseDirectory = CDocument::GetBaseDirectory();

	//create folder if needed

	ASPathName outputFolderPathName = ASFileSysCreatePathName (NULL, ASAtomFromString("DIPath"), "OutputFiles", baseDirectory);
	ASErrorCode err = ASFileSysCreateFolder (NULL, outputFolderPathName, false);
	ASFileSysReleasePath(NULL, outputFolderPathName);

	//create file path
	char addedPath[100];
#ifdef MAC_PLATFORM
	snprintf(addedPath, sizeof(addedPath),"OutputFiles/%s", fileName);
#else
	sprintf_s(addedPath, sizeof(addedPath),"OutputFiles/%s", fileName);
#endif

	return ASFileSysCreatePathName (NULL, ASAtomFromString("DIPath"), addedPath, baseDirectory);
}


void SnippetRunnerUtils::convertDIPathToPlatformPath (char *fileName)
{
	if (!fileName || (strlen(fileName) <= 0))
		ASRaise (GenError(genErrGeneral));

	// Strip leading slash.
	if (fileName[0] == '/') memmove (fileName, fileName+1, strlen(fileName));

#ifdef WIN_PLATFORM
	// Correct drive specifier.
	ASInt32 i;
	for (i = 0; fileName[i] != '\0' && fileName[i] != '/' ; i++) ;
	if (fileName[i] == '/') {
		fileName[i++] = ':';
		memmove (fileName+i+1, fileName+i, strlen(fileName+i));
		fileName[i] = '\\';
	}

	// Process each subdirectory.
	for ( ; fileName[i] != '\0'; i++) {
		if (fileName[i] == '/') fileName[i] = '\\';
	}
#elif MAC_PLATFORM
	while (*fileName != '\0') {
		if(*fileName == '/') {
			*fileName = ':';
		}
		fileName++;
	}
#endif
}

//relative paths use CDocument::baseDirectory
ASFile
SnippetRunnerUtils::openSnippetExampleASFile(const char * filename){
	ASPathName baseDirectory = CDocument::GetBaseDirectory();

	//create file path
	char addedPath[100];
#ifdef MAC_PLATFORM
	snprintf(addedPath, sizeof(addedPath), "ExampleFiles/%s", filename);
#else
	sprintf_s(addedPath, sizeof(addedPath), "ExampleFiles/%s", filename);
#endif

	ASPathName exampleFilePathName = ASFileSysCreatePathName (NULL, ASAtomFromString("DIPath"), addedPath, baseDirectory);

	ASFile theFile;
	ASInt32 retVal = ASFileSysOpenFile(NULL,exampleFilePathName, ASFILE_READ, &theFile);
	ASFileSysReleasePath(ASGetDefaultFileSys(), exampleFilePathName);
	if (retVal != 0){
		Console::displayString("server:Error opening example file!"); 
		return NULL;
	}

	return theFile;
}


ASBool
SnippetRunnerUtils::toggleSnippetCheck(){
	return SnipRun::getThisSnip()->toggleCheckedState();
}


// of course, this isn't about the param dialog, but about the param command line stuff!
void 
SnippetRunnerUtils::turnParamDialogOn(){
	SnipRun::getThisSnip()->suppressDialog(false);
}

void 
SnippetRunnerUtils::turnParamDialogOff(){
	SnipRun::getThisSnip()->suppressDialog(true);
}

ASBool 
SnippetRunnerUtils::toggleParamDialog(){
	ASBool currentState =  SnipRun::getThisSnip()->suppressDialog();
	SnipRun::getThisSnip()->suppressDialog(!currentState);
	return !currentState;
}

void
SnippetRunnerUtils::DumpRectToDebugWindow(const char * preamble,ASFixedRectP rec){
	char lbuff[7];
	char rbuff[7];
	char tbuff[7];
	char bbuff[7];
	ASFixed left = rec->left;
	ASFixedToCString(left,lbuff,6,2);
	ASFixed right = rec->right;
	ASFixedToCString(right,rbuff,6,2);
	ASFixed top = rec->top;
	ASFixedToCString(top,tbuff,6,2);
	ASFixed bottom = rec->bottom;
	ASFixedToCString(bottom,bbuff,6,2);
	
	char buffer[120];
#ifdef MAC_PLATFORM
	if (strlen(preamble)>50){
		snprintf(buffer,sizeof(buffer),"Preamble too long! left = %s top = %s right = %s bottom = %n",lbuff,tbuff,rbuff,bbuff);
	} else {
		snprintf(buffer,sizeof(buffer),"%s {left top right bottom} {%s %s %s %s}\n",preamble,lbuff,tbuff,rbuff,bbuff);	
	}
#else
	if (strlen(preamble)>50){
		sprintf_s(buffer,sizeof(buffer),"Preamble too long! left = %s top = %s right = %s bottom = %n",lbuff,tbuff,rbuff,bbuff);
	} else {
		sprintf_s(buffer,sizeof(buffer),"%s {left top right bottom} {%s %s %s %s}\n",preamble,lbuff,tbuff,rbuff,bbuff);	
	}
#endif

	Console::displayString(buffer);
}

/* provides a case insensative comparison */
int 
SnippetRunnerUtils::stringsMatch(string a, string b){
	ASInt32 strSize = a.length();
	if (strSize!=b.length()){
		return false;
	}
	for (int t = 0;t<strSize;t++) {
		if (toupper(a[t])!=toupper(b[t])){
			return false;
		}
	}
	return true;
}


/* Check if the the MarkInfo dictionary is marked for Tagged PDF */
ASBool 
SnippetRunnerUtils::PDDocIsMarkedPDF(PDDoc pdDoc, ASBool bSetMarked)
{
	ASBool marked=false;
	ASBool hasMarkInfoDict=false;
	CosObj catalogDict, markInfoDict, structTreeRoot;

	CosDoc cosDoc = PDDocGetCosDoc (pdDoc);
	
	DURING
		// A tagged PDF is a structured PDF so check that we 
		// have a struct tree root as well. 
		catalogDict		= CosDocGetRoot(cosDoc);
		structTreeRoot	= CosDictGetKeyString(catalogDict, "StructTreeRoot");
	
		if (CosObjGetType(structTreeRoot) == CosDict)
		{
			// Doc has a valid struct root so now test mark info entry
			markInfoDict	= CosDictGetKeyString(catalogDict, "MarkInfo");
        
			if (CosObjGetType(markInfoDict) == CosDict) 
			{
				hasMarkInfoDict = true;
				CosObj markedObj = CosDictGetKeyString(markInfoDict, "Marked");
				if (CosObjGetType(markedObj) == CosBoolean) {
					marked= CosBooleanValue(markedObj);
				} 
			} 

			if (ASBoolToBool(marked) == false && ASBoolToBool(bSetMarked) == true){
				CosObj cosBool = CosNewBoolean( cosDoc, false, true);

				// if document does not have mark info 
				if ( CosObjGetType( markInfoDict ) == CosNull ){
					// add mark info to the document
					markInfoDict = CosNewDict( cosDoc, true, 1 );
					CosDictPutKeyString( markInfoDict, "Marked", cosBool );
					CosDictPutKeyString( catalogDict, "MarkInfo", markInfoDict );
				}
				else
					// update mark info with new boolean
					CosDictPutKeyString( markInfoDict, "Marked", cosBool );
			}
		}
		
	HANDLER
		char buf[255];
		ASGetErrorString(ERRORCODE, buf, 255);
		Console::displayString(string("server:")+string(buf));
		marked=false;
	END_HANDLER
	return marked;
}


#define AbsoluteColorimetric_K ASAtomFromString("AbsoluteColorimetric")
#define RelativeColorimetric_K ASAtomFromString("RelativeColorimetric")
#define Saturation_K ASAtomFromString("Saturation")
#define Perceptal_K ASAtomFromString("Perceptal")

AC_RenderIntent 
SnippetRunnerUtils::GetRenderIntent (ASAtom intent)
{
	if (intent == AbsoluteColorimetric_K)
		return AC_AbsColorimetric;
	else if (RelativeColorimetric_K)
		return AC_RelColorimetric;
	else if (intent == Saturation_K)
		return AC_Saturation;
	else if (intent == Perceptal_K)
		return AC_Perceptual;
	else
		return AC_UseProfileIntent;
}
	
#if !READER_PLUGIN
ASBool
SnippetRunnerUtils::GetStringFromACString (AC_String *aceStr, char *desc, ASText *asTextStr, ASBool *isUnicode, const ASInt32 len)
{
	AC_Error err;
	ASUns32 stringLen;
	ASUns16 UDesc[255];

	// Try localized string
	if ((err = ACStringLocalized (*aceStr, UDesc, &stringLen, sizeof(UDesc))) == AC_Error_None)
	{
		if ((err = ACStringASCII(*aceStr, desc, &stringLen, len)) == AC_Error_None)
			return true;
	}

	// Try a Unicode string
	else if ((err = ACStringUnicode(*aceStr, UDesc, &stringLen, sizeof(UDesc) / sizeof(UDesc[0]))) == AC_Error_None)
	{
		*isUnicode = true;
		stringLen *= sizeof(ASUns16);
		*asTextStr = ASTextFromSizedUnicode((const ASUns16 *) UDesc, kUTF16HostEndian, (ASInt32) stringLen);
		if (asTextStr)
			return true;
	}
	// Fall back to ASCII string
	else if ((err = ACStringASCII(*aceStr, desc, &stringLen, len)) == AC_Error_None)
		return true;

	return false;
}
#endif

#ifdef ACRO_SDK_PLUGIN_SAMPLE

AVDoc 
SnippetRunnerUtils::openSnippetExampleAVDoc(const char * filename){
	AVDoc avDoc = NULL;
	volatile PDDoc pdDoc = NULL;
	volatile ASFile theFile = NULL;
	DURING
		theFile = SnippetRunnerUtils::openSnippetExampleASFile(filename);
		if (theFile != NULL){
			pdDoc = PDDocOpenFromASFile(theFile, NULL, true);
			if (pdDoc != NULL){
				avDoc = AVDocOpenFromPDDoc(pdDoc, NULL);
			}
		}
	HANDLER
		if (pdDoc != NULL) {
			PDDocClose(pdDoc);
		}
		if (theFile!=NULL){
			ASFileClose(theFile);
		}
	END_HANDLER
	return avDoc;
}



/* takes a colon seperated string and build a menu out of it, returning the final
	menu item of the leaf node of the menu. The uiname needs to be distinct, we derive it
	from the string name passed in. It should be called with the uiname of the parent menu...
*/
AVMenuItem 
SnippetRunnerUtils::buildNewMenu(AVMenu parent,string name) {
	// first we attempt to find the specifies menu.
 	AVMenubar menubar = AVAppGetMenubar();
	volatile AVMenu myParent = AVMenuAcquire(parent);
	ASBool finished = false;
	ASBool error = false;
	ASBool withPreSep = false;
	ASBool withPostSep = false;
	string uiname,preuiname,postuiname;
	volatile AVMenu theMenu = NULL;
	volatile AVMenuItem menuItem = NULL;
	while (!finished && !error){
		uiname = string(ASAtomGetString(AVMenuGetName(myParent)));
		int rpos = name.find_first_of(":");
		string head = name.substr(0,rpos);
		name = name.substr(rpos+1,string::npos);

		// we need to get the menu position for the string,
		// strip off the *s and pass them in.
		string purehead(head);
		ASInt32 cPos;
		while ((cPos = purehead.find("*")) != string::npos){
			purehead.replace(cPos,1,"");
		}	
		DURING

			ASBool preexists;
			ASInt32 menuIndex = getNewMenuPos(myParent,purehead.c_str(),preexists);

			// at this point, we know what position we want the menu to be in, and we know if it
			// preexists. 
			// if the name starts with a '*' we add a seperator before it, if it doesn't exist...
			if (head[0]=='*') {
				string preUIName(uiname);
				preUIName.append("*");
				preUIName.append(purehead);

				// uinames are not allowed spaces, so we replace spaces with ':'
				ASInt32 charPos = 0;
				while ((charPos = preUIName.find(" "))!= string::npos){
					preUIName.replace(charPos,1,":");
				}

				AVMenu thePreMenu = AVMenubarAcquireMenuByName(menubar,preUIName.c_str());
				if (thePreMenu == NULL){
					AVMenuItem preSepMenuItem = AVMenuItemNew("-", preUIName.c_str(), NULL, true, NO_SHORTCUT, 0, NULL, gExtensionID);
					if (ASBoolToBool(preexists) == true){
						ASInt32 t= 1;
					}
					AVMenuAddMenuItem(myParent,preSepMenuItem,menuIndex);
					AVMenuItemRelease(preSepMenuItem);
					menuIndex++;
				}
			}

			// if the name end with a '*' we add a seperator after it, if it doesn't exist...
			if (head[head.length()-1]=='*') {
				string postUIName(uiname);
				postUIName.append(purehead);
				postUIName.append("*");

				// uinames are not allowed spaces, so we replace spaces with ':'
				ASInt32 charPos = 0;
				while ((charPos = postUIName.find(" "))!= string::npos){
					postUIName.replace(charPos,1,":");
				}

				AVMenu thePostMenu = AVMenubarAcquireMenuByName(menubar,postUIName.c_str());
				if (thePostMenu == NULL){
					AVMenuItem postSepMenuItem = AVMenuItemNew("-", postUIName.c_str(), NULL, true, NO_SHORTCUT, 0, NULL, gExtensionID);
					if (ASBoolToBool(preexists) == true){
						AVMenuAddMenuItem(myParent,postSepMenuItem,menuIndex+1);
					}
					else {
						AVMenuAddMenuItem(myParent,postSepMenuItem,menuIndex);
					}
					AVMenuItemRelease(postSepMenuItem);
				}
			}
			
			uiname = uiname.append(purehead);

			// uinames are not allowed spaces, so we replace spaces with ':'
			ASInt32 charPos = 0;
			while ((charPos = uiname.find(" "))!= string::npos){
				uiname.replace(charPos,1,":");
			}

			theMenu = AVMenubarAcquireMenuByName(menubar,uiname.c_str());
			if (rpos == string::npos) {

				// string any * chars
				ASInt32 charPos = 0;

				menuItem = AVMenuItemNew(purehead.c_str(), uiname.c_str(), NULL, true, NO_SHORTCUT, 0, NULL, gExtensionID);
				// if the menu is non-null, we need to order alphabetically

				AVMenuAddMenuItem(myParent,menuItem,menuIndex);

				AVMenuRelease(myParent);
				finished = true;
			}
			else if (theMenu == NULL){
				// we don't have a menu, I need to create the menu and associated parent's menuitem
				theMenu = AVMenuNew(purehead.c_str(),uiname.c_str(),gExtensionID);	
				menuItem = AVMenuItemNew(purehead.c_str(), uiname.c_str(), theMenu, true, NO_SHORTCUT, 0, NULL, gExtensionID);

			
				AVMenuAddMenuItem(myParent,menuItem,menuIndex);
				AVMenuRelease(myParent);
				myParent = theMenu;
				AVMenuItemRelease(menuItem);
			}
			else {
				AVMenuRelease(myParent);
				myParent = theMenu; // we could do an aquire and release, this is a shortcut...
			}


			
		HANDLER
			error = true;
		END_HANDLER
	}
	// we can only get here in error (if there was an exception). Delete any live objects and return
	if (ASBoolToBool(error) == true) {
		if (theMenu != NULL){
			AVMenuRelease(theMenu);
		}
		if (menuItem != NULL){
			AVMenuItemRelease(menuItem);
		}
		AVMenuRelease(myParent);
		return NULL;
	}
	return menuItem;
}

ASInt32 
SnippetRunnerUtils::getNewMenuPos(AVMenu menuContainer,const char * title, ASBool & exists){
	ASInt32 numMenus = AVMenuGetNumMenuItems(menuContainer);
	ASInt32 retVal = 0;
	ASBool hasPreSep = false;
	ASBool hasPostSep = false;
	exists = false;
	for (ASInt32 menuIndex=0;menuIndex<numMenus;menuIndex++){
		AVMenuItem theMI = AVMenuAcquireMenuItemByIndex(menuContainer,menuIndex);
		char theMITitle[50]; // we know it won't be bigger than this.
		AVMenuItemGetTitle(theMI, theMITitle, 50);

		if(strcmp(title,theMITitle)==0){
			exists = true;
			break;
		}
		
		// if we have a pre seperator, we have to decrement retval if the actual menu is the one...		
		
		// if we have a post seperator, we have to increment the retVal and ignore the comparison.
		if (strcmp("-",theMITitle)==0){
			ASAtom uiName = AVMenuItemGetName(theMI);
			const char * mstr = ASAtomGetString(uiName);
			if (mstr[strlen(mstr)-1] != '*'){
				hasPreSep = true;
			}
			else {
				hasPostSep=true;
			}
		}
		// we have a seperator, take this into account when looking at the next value
		else if (strcmp(title,theMITitle)<0){
			if (ASBoolToBool(hasPreSep) == true){
				if (retVal == 0){
					AVAlertNote("ASSERT, seperator for return value?");
				}
				--retVal;
			}
			break;
		}
		else {
			hasPreSep = false;
		}

		AVMenuItemRelease(theMI);
		retVal++;
	}

	return retVal;
}

#else // we are in the library, not Acrobat
ASBool 
SnippetRunnerUtils::initLibrary(){
	if (fPDFLInited==true){
		return false;
	}
		int err = MyPDFLInit();	// initialize the PDFLib 
#if MAC_ENV
{
	// Set the working directory where the application was launched from - This must be done after initializing
	// the Toolkit 
	if(!err)
		err = PDFLSetWorkDirToAppDir();
}
#endif


	if (err != 0)			// check for error after initialization 
	{
		cerr << "Initialization error. See \"AcroErr.h\" for more info.\n" << endl;
		cerr << "Error system: " << ErrGetSystem(err) << endl;
		cerr << "Error Severity: " << ErrGetSeverity(err) << endl;
		cerr << "Error Code: " << ErrGetCode(err) << endl;
		exit(-1); // if we cannot init the pdfl we terminate the appliation.
	}
	fPDFLInited = true;
	return true;
}

ASBool 
SnippetRunnerUtils::termLibrary(){
	if (fPDFLInited==false){
		return false;
	}
	MyPDFLTerm();	// terminate the PDFLib
	fPDFLInited = false;
	return true;
}

#endif

