/*********************************************************************

 ADOBE SYSTEMS INCORPORATED
 Copyright (C) 1998-2006 Adobe Systems Incorporated
 All rights reserved.

 NOTICE: Adobe permits you to use, modify, and distribute this file
 in accordance with the terms of the Adobe license agreement
 accompanying it. If you have received this file from a source other
 than Adobe, then your use, modification, or distribution of it
 requires the prior written permission of Adobe.

 -------------------------------------------------------------------*/
/** 
\file BasicPlugin.cpp

  - This file implements the functionality of the BasicPlugin.
*********************************************************************/


// Acrobat Headers.
#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

#include "SnippetRunnerUtils.h"
#include "ParamManager.h" 

#include "stdio.h"
FILE* pOutput;

ACCB1 ASBool ACCB2 WordEnumProc(PDWordFinder, PDWord, ASInt32, void*);
bool ExtractText(PDDoc, ASInt32, ASInt32, ASBool, PDWordFinderConfig, FILE*);

static ASInt32 gNumToPDFExt;
static ASInt32 gNumFromPDFExt;
static AVConversionFromPDFHandler RightHandler;

const char* RTF_FILE = "output.doc";
const char* TXT_FILE = "output.txt";
/*-------------------------------------------------------
	Constants/Declarations
-------------------------------------------------------*/
// This plug-in's name, you should specify your own unique name here.

const char* MyPluginExtensionName = "ADBE:TextExtraction";

/* A convenient function to add a menu item for your plugin.
*/
ACCB1 ASBool ACCB2 PluginMenuItem(char* MyMenuItemTitle, char* MyMenuItemName);

/*-------------------------------------------------------
	Functions
r-------------------------------------------------------*/

/* MyPluginSetmenu
** ------------------------------------------------------
**
** Function to set up menu for the plugin.
** It calls a convenient function PluginMenuItem.
** Return true if successful, false if failed.
*/
ACCB1 ASBool ACCB2 MyPluginSetmenu()
{
	// Add a new menu item under Acrobat SDK submenu.
	// The new menu item name is "ADBE:BasicPluginMenu", title is "Basic Plugin".
	// Of course, you can change it to your own.
	return PluginMenuItem("TextExtraction", "ADBE:TextExtraction"); 
}
	

/**		BasicPlugin project is an Acrobat plugin sample with the minimum code 
	to provide an environment for plugin developers to get started quickly.
	It can help Acrobat APIs' code testing, too.  
		This file implements the functionality of the BasicPlugin. It adds a 
	new menu item that will show a message of some simple information about 
	the plugin and front PDF document. Users can modify and add code in this 
	file only to make a simple plugin of their own.   
		
		  MyPluginCommand is the function to be called when executing a menu.
	This is the entry point for user's code, just add your code inside.

	@see ASExtensionGetRegisteredName
	@see AVAppGetActiveDoc
	@see PDDocGetNumPages
*/ 
/*
getOutputPath(const char* fileName){
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
*/
/**
	User defined callback: AVConversionFromPDFEnumProc
*/
static ACCB1 ASBool ACCB2 myAVConversionFromPDFEnumProc(AVConversionFromPDFHandler handler,AVConversionEnumProcData data)
{
	AVFileFilterRec filter = handler->convFilter;
	ASUns16 numFileExt = filter.numFileDescs;
	
	// go through the conversion handlers to find a handler for rtf extention files. 
	for (ASInt32 i = 0; i < numFileExt; i++)
	{
		if (strlen(handler->convFilter.fileDescs[i].extension)>0)
		{
			// found it, fill in the handler and return false to stop going on.
			if(!strcmp(handler->convFilter.fileDescs[i].extension,"doc")) {
				RightHandler = handler;
				return false;
			}
		}
	}
	return true;
}

static ACCB1 ASBool ACCB2 myAVConversionFromPDFEnumProc2(AVConversionFromPDFHandler handler,AVConversionEnumProcData data)
{
	AVFileFilterRec filter = handler->convFilter;
	ASUns16 numFileExt = filter.numFileDescs;
	
	// go through the conversion handlers to find a handler for rtf extention files. 
	for (ASInt32 i = 0; i < numFileExt; i++)
	{
		if (strlen(handler->convFilter.fileDescs[i].extension)>0)
		{
			// found it, fill in the handler and return false to stop going on.
			if(!strcmp(handler->convFilter.fileDescs[i].extension,"txt")) {
				RightHandler = handler;
				return false;
			}
		}
	}
	return true;
}

ACCB1 void ACCB2 MyPluginCommand(void *clientData)
{
	
	// try to get front PDF document 
	AVDoc avDoc = AVAppGetActiveDoc();
	PDDoc pdDoc = NULL;
	int numPages = 0;
	if(avDoc==NULL) {
		// if no doc is loaded, make a message.
		//strcat(str,"There is no PDF document loaded in Acrobat.");
		return;
	}
	else {
		// if a PDF is open, get its number of pages
		pdDoc = AVDocGetPDDoc (avDoc);
		numPages = PDDocGetNumPages (pdDoc);
	}

	if(pdDoc == NULL)	return;
 
	// get a PDF open in front.
	/*CDocument document;
	PDDoc pdDoc = (PDDoc)document;

	if(pdDoc == NULL){
		return;
	}*/

	// enumerate from PDF conversion handlers to find the "rtf" handler.
	AVConversionEnumFromPDFConverters(myAVConversionFromPDFEnumProc, NULL);
	SnippetRunnerUtils::ShowDebugWindow();

	string szPath = "C:\\";
	CDocument::SetBaseDirectory(szPath);
	// save the rtf file to the snippetRunner's output files folder. 
	ASPathName OutPath = SnippetRunnerUtils::getOutputPath(RTF_FILE);
	if(OutPath==NULL) 
		OutPath = ASFileSysCreatePathName (NULL, ASAtomFromString("Cstring"), RTF_FILE, 0);
	if(OutPath==NULL) {
		AVAlertNote("Cannot open an output file.");
		return;
	}
	
	// do conversion
	AVConversionStatus status=AVConversionConvertFromPDFWithHandler(RightHandler, 
					NULL,kAVConversionNoFlags, pdDoc, OutPath, ASGetDefaultFileSys(),NULL);

	//////////////////////////////////////////////////////////////////////
		// save the rtf file to the snippetRunner's output files folder. 
	// enumerate from PDF conversion handlers to find the "rtf" handler.
	AVConversionEnumFromPDFConverters(myAVConversionFromPDFEnumProc2, NULL);
	SnippetRunnerUtils::ShowDebugWindow();

	OutPath = SnippetRunnerUtils::getOutputPath(TXT_FILE);
	if(OutPath==NULL) 
		OutPath = ASFileSysCreatePathName (NULL, ASAtomFromString("Cstring"), TXT_FILE, 0);
	if(OutPath==NULL) {
		AVAlertNote("Cannot open an output file.");
		return;
	}
	
	// do conversion
	status=AVConversionConvertFromPDFWithHandler(RightHandler, 
					NULL,kAVConversionNoFlags, pdDoc, OutPath, ASGetDefaultFileSys(),NULL);

	/////////////////////////////////////////////////////////////////////////
	// check the returned status and show message 
/*	if(status == kAVConversionSuccess)
		AVAlertNote("The Rtf file was saved in SnippetRunner output folder.");
	else if(status == kAVConversionFailed)
		AVAlertNote("The Rtf file conversion failed.");
	else if(status == kAVConversionSuccessAsync)
		AVAlertNote("The conversion will continue asynchronously.");
	else if(status == kAVConversionCancelled)
		AVAlertNote("The conversion was cancelled.");

/*
	ASInt32 nStartPage, nEndPage, nToUnicode;
	ASBool bToUnicode;
	nStartPage = 1;
	nEndPage = numPages;
	nToUnicode = 0;
	bToUnicode = false;

	//bToUnicode = (nToUnicode==0)?(false):(true);

	// Set up WordFinder creation options record
	PDWordFinderConfigRec wfConfig;
	
	memset(&wfConfig, 0, sizeof(PDWordFinderConfigRec));
	
	wfConfig.recSize = sizeof(PDWordFinderConfigRec);
	wfConfig.ignoreCharGaps = true;
	wfConfig.ignoreLineGaps = false;
	wfConfig.noAnnots = true;
	wfConfig.noEncodingGuess = true;		// leave non-Roman single-byte font alone

	// Std Roman treatment for custom encoding; overrides the noEncodingGuess option
	wfConfig.unknownToStdEnc = false;		
		
	wfConfig.disableTaggedPDF = false;		// legacy mode WordFinder creation
	wfConfig.noXYSort = false;
	wfConfig.preserveSpaces = false;
	wfConfig.noLigatureExp = false;
	wfConfig.noHyphenDetection = false;
	wfConfig.trustNBSpace = false;
	wfConfig.noExtCharOffset = false;		// text extraction efficiency
	wfConfig.noStyleInfo = false;			// text extraction efficiency
	wfConfig.decomposeTbl = NULL;			// Unicode character replacement
	wfConfig.decomposeTblSize = 0;
	wfConfig.charTypeTbl = NULL;			// Custom char type table
	wfConfig.charTypeTblSize = 0;
		
	ASPathName pathName;
	pathName = SnippetRunnerUtils::getOutputPath("output.txt");

#ifndef MAC_PLATFORM
	//on the Mac ASFileSysDisplayStringFromPath returns an Mac OS 9 style path.
	pOutput = fopen(ASFileSysDisplayStringFromPath (NULL, pathName), "w+b");

#else
	ASPlatformPath platformPath;
	ASFileSysAcquirePlatformPath (NULL, pathName, ASAtomFromString("POSIXPath"), &platformPath);
	POSIXPath_Ptr path = ASPlatformPathGetPOSIXPathPtr (platformPath);
	pOutput = fopen(path, "w+b");
	ASFileSysReleasePlatformPath(NULL, platformPath);
#endif
	if (pOutput)
	{
		if(ExtractText(pdDoc, nStartPage - 1, nEndPage - 1, 
			bToUnicode, &wfConfig, pOutput))
				Console::displayString("Text extraction completed."); 
		return;
	}
	else
		AVAlertNote("Text extraction aborted.");

	
	if (pathName) ASFileSysReleasePath(NULL, pathName);

	if (pOutput) fclose(pOutput);

	return;
	*/
}

/* MyPluginIsEnabled
** ------------------------------------------------------
** Function to control if a menu item should be enabled.
** Return true to enable it, false not to enable it.
*/
ACCB1 ASBool ACCB2 MyPluginIsEnabled(void *clientData)
{
	// always enabled.
	return true;
	
	// this code make it is enabled only if there is a open PDF document. 
	/* return (AVAppGetActiveDoc() != NULL); */
}


/**
** Word Enumerator callback for text extraction 
**
** @param whObj IN The WordFinder object with which to enumerate words.
** @param pdWord OUT The PDWord enumerated.
** @param pgNum OUT The page number from which the PDWord is extracted.
** @param clientData IN Data passed in to the call. In this case the FILE stream pointer.
** @return true to continue word enumeration, flase otherwise.
*/
ACCB1 ASBool ACCB2 WordEnumProc(PDWordFinder wfObj, PDWord pdWord, ASInt32 pgNum, void* clientData)
{
	char str[128];

	PDWordGetString(pdWord, str, sizeof(str));

	for (int i = 0; i < PDWordGetLength(pdWord); i++)
		fputc(str[i], (FILE*) clientData);

	if (PDWordGetAttrEx(pdWord, 0) & WXE_LAST_WORD_ON_LINE)
	{	// taking care of line/page breaks
		if (PDWordGetAttrEx(pdWord, 1) & WXE_WORD_IS_UNICODE)
		{	
			for (int i=0; i<2; i++)
			{
				fputc(0x00, (FILE*) clientData); fputc(0x0d, (FILE*) clientData);
				fputc(0x00, (FILE*) clientData); fputc(0x0a, (FILE*) clientData);
			}
		}
		else
		{
			for (int i=0; i<2; i++)
			{
				fputc(0x0d, (FILE*) clientData); fputc(0x0a, (FILE*) clientData);
			}
		}
	}
	else
	{
		// taking care of word spacing
		if (PDWordGetAttrEx(pdWord, 0) & (WXE_ADJACENT_TO_SPACE|WXE_HAS_TRAILING_PUNC&!WXE_HAS_HYPHEN))
			if (PDWordGetAttrEx(pdWord, 1) & WXE_WORD_IS_UNICODE)
			{
				fputc(0x00, (FILE*) clientData);
				fputc(0x20, (FILE*) clientData);
			}
			else
				fputc(' ', (FILE*) clientData);
	}

	return true;
}

/**
** Called to perform WordFinder creation and text extraction on a PDF document
**
** @param pdDoc IN The PDDoc object on which to perform text extraction.
** @param startPg IN The page to start text extraction.
** @param endPg IN The page to end text extraction (inclusive).
** @param toUnicode IN Whether to extract text to Unicode encoding.
** @param pConfig IN Pointer to a WordFinder Configuration Record.
** @param pOutput IN/OUT Pointer to an output FILE stream to which the extracted 
**	text will be written.
** @return true to indicate text extraction operation a success, false otherwise.
*/
bool ExtractText(PDDoc pdDoc, ASInt32 startPg, ASInt32 endPg, 
				 ASBool toUnicode, PDWordFinderConfig pConfig, FILE* pOutput)
{
	if (startPg < 0 || endPg <0 || startPg > endPg || endPg > PDDocGetNumPages(pdDoc) - 1)
	{
		AVAlertNote("Exceeding starting or ending page number limit of current document."); 
		return false;
	}

	PDWordFinder pdWordFinder = NULL;

DURING	
	pdWordFinder = PDDocCreateWordFinderEx(pdDoc, WF_LATEST_VERSION, toUnicode, pConfig);
	
	if (toUnicode) fprintf(pOutput, "%c%c", 0xfe, 0xff);

	for (int i = startPg; i <= endPg; i++)
		PDWordFinderEnumWords(pdWordFinder, i, ASCallbackCreateProto(PDWordProc, &WordEnumProc), pOutput);

	PDWordFinderDestroy(pdWordFinder);
	E_RETURN(true);
HANDLER
	char buf[256], errmsg[256];
	sprintf(buf, "[ExtractText()]Error %d: %s",  ErrGetCode(ERRORCODE), ASGetErrorString(ERRORCODE, errmsg, sizeof(errmsg)));
	AVAlertNote(buf);
	if (pdWordFinder) PDWordFinderDestroy(pdWordFinder);
	if( pOutput) fclose(pOutput);
	return false;
END_HANDLER

return true;
}
