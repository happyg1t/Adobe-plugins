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

/*
	HOW THIS CLASS WORKS
	
	Create an object of this type. It uses a PDDoc to get the pieces that are needed
	for the snippet.
	
	For PDFL, the pdDoc has been set by opening a doc.
	For Acrobat, the pdDoc is set using the current doc when the object is created.
	
	Each doc or page type supported can be obtained by casting the CDocument to that type, 
	such as:
	
	CDocument cDoc;
	AVPageView pageView = (AVPageView)cDoc;
	
	Supported types are;
	AVDoc
	AVPageView
	PDDoc
	PDPage
	CosDoc
	
	The CDocument class takes care of releasing objects as necessary. If you need to 
	hold an object through the scope of the CDocument object, you should Acquire the object,
	if there is a method to do so.
	
	This object is outfitted with Raise aware macros, which will ensure that the destructor
	is called even if a raise occurs.
*/
#ifndef CDOCUMENT
#define CDOCUMENT

#ifdef ACRO_SDK_PLUGIN_SAMPLE
#include "ASRaiseAware.h"
#endif

#include "SnippetRunner.h"

class CDocument
{
#ifdef ACRO_SDK_PLUGIN_SAMPLE
	//make Raise aware, so that Raises call destructor
	RAISEAWARE(CDocument)
#endif
	public:
	
		CDocument();
		~CDocument();	

		static PDDoc OpenDocument(const char* path);
		// An overloaded version of the OpenDocument method for handling Unicode-encoded files
		static PDDoc OpenDocument(const ASUTF16Val* path);

		static bool RevertDocument();
		static bool CloseDocument();
		
		static bool SaveDocument(const ASUTF16Val* newPath);
		// An overloaded method ofr PDFL
		static bool SaveDocument(const char* newPath);
		
		static bool SetCurrentPage(int pageNumber);
		
		static void SetBaseDirectory(string& newDir);
		static ASPathName GetBaseDirectory()
						{ return baseDirectory; }

		static ASPathName MakeFullPath(const char* path);
		// An overloaded version of the OpenDocument method for handling Unicode-encoded files
		static ASPathName MakeFullPath(const ASUTF16Val* path);

#ifdef ACRO_SDK_PLUGIN_SAMPLE			
		operator AVDoc();
		operator AVPageView();
#endif		
		operator PDDoc();
		static PDDoc GetPDDoc();
		
		operator PDPage();
		static PDPage GetPDPage();
		
		operator CosDoc();
		
		
	protected:
		bool CheckForValidDocument(void* value);

		//This is the hub. Everything else is derived on the fly.
		static PDDoc pdDoc;

#ifdef ACRO_SDK_PLUGIN_SAMPLE		
		static AVDoc avDoc;

#endif
		static ASPathName asPathName;

		static ASFileSys fileSys;

		static PDPage pdPage;
		
		static ASPathName baseDirectory;		
};

#endif


