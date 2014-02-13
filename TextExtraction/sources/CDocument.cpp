/*
//
//  ADOBE SYSTEMS INCORPORATED
//  Copyright (C) 2006 Adobe Systems Incorporated
//  All rights reserved.
//
//  NOTICE: Adobe permits you to use, modify, and distribute this file
//  in accordance with the terms of the Adobe license agreement
//  accompanying it. If you have received this file from a source other
//  than Adobe, then your use, modification, or distribution of it
//  requires the prior written permission of Adobe.
//
*/

#include "CDocument.h"

#if MAC_PLATFORM
//#ifndef PLUGIN
#ifdef PDFL_SDK_SAMPLE
#include "macutils.h"
#endif
#endif

#ifdef ACRO_SDK_PLUGIN_SAMPLE
//#ifdef PLUGIN
AVDoc CDocument::avDoc = NULL;
#endif

#define MAXPATH 1024

PDDoc CDocument::pdDoc = NULL;
ASPathName CDocument::asPathName = NULL;
ASFileSys CDocument::fileSys = NULL;
ASPathName CDocument::baseDirectory = NULL;
PDPage CDocument::pdPage = NULL;

CDocument::CDocument()
{
#ifdef ACRO_SDK_PLUGIN_SAMPLE
//#ifdef PLUGIN
	CTOR; /* Macro for creating a RaiseAwareConstructor */
	avDoc = AVAppGetActiveDoc();
	if(avDoc)
	{
		pdDoc = AVDocGetPDDoc(avDoc);
		//also get the current page
		AVPageView pageView = AVDocGetPageView(avDoc);
		PDPageNumber pageNum = AVPageViewGetPageNum (pageView);
		pdPage = PDDocAcquirePage (pdDoc, pageNum);
	}
#else
	//these are automatically initialized to null since they are 'static'
	//we can't set initialize them because of some cases of overlapping CDocument creations
	//asPathName = NULL;
	//pdPage = NULL;
#endif
}

CDocument::~CDocument()
{
//!!NOTE: - PDFL has a concept of a global open document, and the only way
//to open/close that doc is to use the opendoc and closedoc commands
//all other actions shouldn't release or destroy anything
//#ifdef PLUGIN
#ifdef ACRO_SDK_PLUGIN_SAMPLE
	DTOR; /* Macro for creating a RaiseAwareDestructor */

#endif
	if(pdPage)
	{
		PDPageRelease(pdPage);
		pdPage = NULL;		
	}
}

ASPathName CDocument::MakeFullPath(const char* path)
{
	ASPathName fullPath = NULL;
	
	//Assume these are all markers of full paths
	if(path[0] != '/' && path[0] != '\\' && path[1] != ':')
	{
		if(baseDirectory)
		{
#ifdef MAC_PLATFORM
			ASPlatformPath platformPath;
			ASFileSysAcquirePlatformPath(NULL, baseDirectory, ASAtomFromString("POSIXPath"), &platformPath);
			char* basePath = ASPlatformPathGetPOSIXPathPtr(platformPath);
			char fullPathString[MAXPATH];
			strlcpy(fullPathString, basePath, sizeof(fullPathString));
			strlcat(fullPathString, path, sizeof(fullPathString));
			fullPath = ASFileSysCreatePathFromPOSIXPath(NULL, fullPathString);
#else
			ASPlatformPath platformPath;
			ASFileSysAcquirePlatformPath(NULL, baseDirectory, ASAtomFromString("Cstring"), &platformPath);
			char* basePath = ASPlatformPathGetCstringPtr(platformPath);
			char fullPathString[MAXPATH];
			strncpy_s(fullPathString, sizeof(fullPathString), basePath, _TRUNCATE);
			strcat_s(fullPathString, sizeof(fullPathString), "/");
			strcat_s(fullPathString, sizeof(fullPathString), path);
			fullPath = ASFileSysCreatePathName(NULL, ASAtomFromString("Cstring"), fullPathString, 0);
#endif
		}
		else
			Console::displayString("Need to set the directory using the setdir command");
	}
	else
	{
#ifdef MAC_PLATFORM
	fullPath = ASFileSysCreatePathFromPOSIXPath(NULL, path);
#else
	fullPath = ASFileSysCreatePathName(NULL, ASAtomFromString("Cstring"), path, 0);
#endif	
	}
	return fullPath;
}

ASPathName CDocument::MakeFullPath(const ASUTF16Val* path)
{
	ASPathName fullPath = NULL;
	
	//Assume these are all markers of full paths
	if(path[0] != '/' && path[0] != '\\' && path[1] != ':')
	{
		if(baseDirectory)
		{			
			ASPlatformPath platformPath;
			ASFileSysAcquirePlatformPath(
				CDocument::fileSys, 
				baseDirectory, 
				ASAtomFromString("WinUnicodePath"), 
				&platformPath);

			const ASUTF16Val* basePath = (const ASUTF16Val*)ASPlatformPathGetCstringPtr(platformPath);
			ASText filePath = ASTextFromUnicode(basePath, kUTF16HostEndian);		
			ASTextCatMany(
				filePath, 
				ASTextFromUnicode(path, kUTF16HostEndian), 
				NULL
				);

			ASFileSys fileSys = ASGetDefaultFileSysForPath(ASAtomFromString("ASTextPath"), filePath);
			fullPath = ASFileSysCreatePathName(fileSys, ASAtomFromString("ASTextPath"), filePath, 0);
		}
		else
			Console::displayString("Need to set the directory using the setdir command");
	}
	else
	{
		ASText asPath = ASTextFromUnicode(path, kUTF16HostEndian);
		fullPath = ASFileSysCreatePathName(
			ASGetDefaultFileSysForPath(ASAtomFromString("ASTextPath"), asPath), 
			ASAtomFromString("ASTextPath"), 
			asPath, 
			0);
	}
	return fullPath;
}


//these are used in PDFL SnippetRunner and the SOAP interface

PDDoc CDocument::OpenDocument(const char* path)
{
	DURING
		/* Create asPathName from file.*/
		// save in a static var so we can revert it...
		
		//test, and replace asPathName, if exists
		//only revert currently passes a null, as it needs to use the old asPathName to reopen the doc
		if(path)
			asPathName = MakeFullPath(path);

		/* Open pdDoc from asPathName.*/
		pdDoc = PDDocOpen(asPathName, NULL, NULL, true);
		#ifdef ACRO_SDK_PLUGIN_SAMPLE
		//#ifdef PLUGIN
		avDoc = AVDocOpenFromPDDoc(pdDoc, NULL);
		#endif
	HANDLER
		pdDoc = NULL;
	END_HANDLER
	return pdDoc;
}

PDDoc CDocument::OpenDocument(const ASUTF16Val* path)
{
	DURING
		if(path)
			asPathName = MakeFullPath(path);

		/* Open pdDoc from asPathName.*/
		ASFileSys fileSys = ASGetDefaultFileSysForPath(ASAtomFromString("ASTextPath"), asPathName);
		pdDoc = PDDocOpen(asPathName, fileSys, NULL, true);
		//#ifdef PLUGIN
		#ifdef ACRO_SDK_PLUGIN_SAMPLE
		avDoc = AVDocOpenFromPDDoc(pdDoc, NULL);
		#endif
	HANDLER
		pdDoc = NULL;
	END_HANDLER
	return pdDoc;
}

bool CDocument::RevertDocument()
{
	bool success = false;
	DURING
		//save off path so it doesn't get released on us
		ASPathName tempPath = asPathName;
		//set to null so it doesn't get released when doc closed
		asPathName = NULL;
		CloseDocument();
		asPathName = tempPath;
		ASFileSys fileSys = ASGetDefaultFileSysForPath(ASAtomFromString("ASTextPath"), asPathName);
		pdDoc = PDDocOpen(asPathName, fileSys, NULL, true);
		if(pdDoc)
			success = true;		
	HANDLER
	END_HANDLER

	return success;
}

bool CDocument::CloseDocument()
{	
	if(asPathName)
		ASFileSysReleasePath(fileSys, asPathName);
	if(pdPage)
		PDPageRelease(pdPage);
//#ifdef PLUGIN
#ifdef ACRO_SDK_PLUGIN_SAMPLE
	if(avDoc)
		AVDocClose(avDoc, true);
	else //fall through to test to close pdDoc
#endif
	if(pdDoc)
		PDDocClose(pdDoc);
	pdDoc = NULL;
	asPathName = NULL;
	pdPage = NULL;
	return true;
}

//pass absolute paths only...
void CDocument::SetBaseDirectory(string& inPath)
{
	char str[500];
	//is path quoted?
	if(inPath.c_str()[0] == '"')
	{
#ifdef MAC_PLATFORM
		//cut off initial quote
		strlcpy(str, &inPath.c_str()[1], sizeof(str));
		//if no end quote, return
		if(str[strlen(str)-1] != '"')
			return;
		//else cut off the end one too.
		str[strlen(str)-1] = '\0';
#else
				//cut off initial quote
		strncpy_s(str, sizeof(str), &inPath.c_str()[1], _TRUNCATE);
		//if no end quote, return
		if(str[strlen(str)-1] != '"')
			return;
		//else cut off the end one too.
		str[strlen(str)-1] = '\0';
#endif
	}
	else {
#ifdef MAC_PLATFORM
		strlcpy(str, inPath.c_str(), sizeof(str));	
#else
		strncpy_s(str, sizeof(str), inPath.c_str(), _TRUNCATE);	
#endif
	}

	if(baseDirectory)
	{
		ASFileSysReleasePath(NULL, baseDirectory);
		baseDirectory = NULL;		
	}

#ifdef MAC_PLATFORM
	baseDirectory = ASFileSysCreatePathFromPOSIXPath(NULL, str);
#elif WIN_PLATFORM
	ASText filePath = ASTextFromScriptText(str, kASEUnicodeScript);
	fileSys = ASGetDefaultFileSysForPath(ASAtomFromString("ASTextPath"), filePath);
	baseDirectory = ASFileSysCreatePathName(fileSys, ASAtomFromString("ASTextPath"), filePath, 0);
#else
	baseDirectory = ASFileSysCreatePathName(NULL, ASAtomFromString("Cstring"), str, 0);
#endif
}

//	Full paths are treated as such, relative is expanded like so:
//4 cases to handle
//	PDFL command line
//	PDFL Java Interface
//		both use baseDirectory, set at startup to app dir.
//	Acrobat Java Interface
//		This uses user interaction for saving
//	Acrobat SOAP
//		This uses baseDirectory as save context, needs to be set in script using setdir command
//	
//	Full paths should always work, relative paths should be relative to context.
// 		...and of course, deal with platform differences...
	
//relative paths should be DIPath based
bool CDocument::SaveDocument(const ASUTF16Val* newPath)
{
	bool success = false;
	ASPathName fPath = NULL;
#if !READER_PLUGIN
	if(!pdDoc)
		return success;
	
	DURING
		fPath = MakeFullPath(newPath);

	PDDocSave(pdDoc, PDSaveFull | PDSaveLinearized, fPath, NULL, NULL, NULL);
	ASFileSysReleasePath(NULL, fPath);
	success = true;
	
	HANDLER
	END_HANDLER

#endif
	
	return success;
}

// An overloaded function for PDFL
bool CDocument::SaveDocument(const char* newPath)
{
	bool success = false;
	ASPathName fPath = NULL;
#if !READER_PLUGIN
	if(!pdDoc)
		return success;
	
	DURING
		fPath = MakeFullPath(newPath);

	PDDocSave(pdDoc, PDSaveFull | PDSaveLinearized, fPath, NULL, NULL, NULL);
	ASFileSysReleasePath(NULL, fPath);
	success = true;
	
	HANDLER
	END_HANDLER

#endif
	
	return success;
}

bool CDocument::SetCurrentPage(int pageNumber)
{
	bool success = false;

	if (pdDoc == NULL){
		return success;
	}
	volatile PDPage tmpPage = NULL;
	ASBool invalidPage = false;
	DURING
		tmpPage = PDDocAcquirePage(pdDoc, pageNumber);	
		success = true;
	HANDLER
		invalidPage = true;
	END_HANDLER		
	if (invalidPage == false){
		if (pdPage != NULL){
			PDPageRelease(pdPage);			
		}
		pdPage = tmpPage;
	}

	return success;
}


#ifdef ACRO_SDK_PLUGIN_SAMPLE
//#ifdef PLUGIN
CDocument::operator AVDoc()
{
	CheckForValidDocument(avDoc);
	return avDoc;
}

CDocument::operator AVPageView()
{
	if(CheckForValidDocument(avDoc))
		return AVDocGetPageView(avDoc);
	else
		return NULL;
}
#endif
CDocument::operator PDDoc()
{
	CheckForValidDocument(pdDoc);
	return pdDoc;
}

//doesn't check for validity
PDDoc CDocument::GetPDDoc()
{
	return pdDoc;
}


CDocument::operator PDPage()
{
	if(CheckForValidDocument(pdDoc))
	{
		//might have been set from setCurrentPage or a previous call to this.
		if(!pdPage)
			pdPage = PDDocAcquirePage(pdDoc, 0);
		 
		 return pdPage;
	}
	else
		return NULL;
}

PDPage CDocument::GetPDPage()
{
	if(pdDoc)
	{
		//might have been set from setCurrentPage or a previous call to this.
		if(!pdPage)
			pdPage = PDDocAcquirePage(pdDoc, 0);
		 
		 return pdPage;
	}
	else
		return NULL;
}

CDocument::operator CosDoc()
{
	if(CheckForValidDocument(pdDoc))
		return PDDocGetCosDoc(pdDoc);
	else
		return NULL;
}
bool CDocument::CheckForValidDocument(void* value)
{
	if(!value)
		AVAlertNote("This snippet expects an open document");
	return value != NULL;
}

