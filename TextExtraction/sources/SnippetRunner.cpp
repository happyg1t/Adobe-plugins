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

 SnipRun.cpp

 - Provides the base snippet behaviour

*********************************************************************/
#include <string>
#include <typeinfo>

#ifdef ACRO_SDK_PLUGIN_SAMPLE

// Acrobat headers
#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif
#include "WLHFT.h"
#endif
#include "SnippetRunner.h"
#include "SnipManager.h"
#include "SnippetRunnerUtils.h"

using namespace std;

// we need to define exactly what the static sniprun is (in exactly one source file)...
SnipRun * SnipRun::thisSnip = NULL;

SnipRun::SnipRun(const char * snippet, const char * name, const char * description, char * params) : fSnippetName(snippet), fName(name), fDescription(description), fDefaultParams(params), fCheckedState(false), fShowDialogState(false)
{
	SnipManager* snippetManager = SnipManager::Instance();
	snippetManager->PushSnippet(this);
	fParams = string ("");
}

/* Destructor
*/
SnipRun::~SnipRun()
{
}


ACCB1 void ACCB2 
SnipRun::SnipCallback(void *clientData)
{
// removed the call into th documentation for PDFL, we need ot think of a different policy here to automatically bind to the documentation.
	SnipRun *p = (SnipRun *)clientData;
	SnipRun::thisSnip=p;
	DURING
		ASInt32 errcode = IDOK;
		ParamManager * thePM = NULL;
		if ((p->fDefaultParams.size())!=0){
			if (p->fParams.compare(string("")) == 0){
				p->fParams = p->fDefaultParams;
			}
		
			thePM = new ParamManager();
			errcode = thePM->doDialog(p->fName,p->fDescription,p->fDefaultParams.c_str(),p->fParams.c_str());
		}
		if (errcode == IDOK){
			if (thePM!=NULL){
				p->fParams = thePM->getParamList();
			}
			p->Run(thePM);
		}
		//don't allow use of last params, since we want that to mark diff between command line and server
		p->fParams = "";
		if (thePM != NULL){
			delete thePM;
		}
		HANDLER
			char message[255];
			ASGetErrorString(ERRORCODE, message, sizeof(message));
			char message1[255];
#ifdef MAC_PLATFORM
			strlcpy(message1,"server:Exception in snippet ",sizeof(message1));
			strlcat(message1, p->GetUIName(),sizeof(message1));
#else
			strcpy_s(message1,sizeof(message1),"server:Exception in snippet ");
			strcat_s(message1,sizeof(message1), p->GetUIName());
#endif
			Console::displayString(message1); 
			Console::displayString(string("server:")+string(message));
		END_HANDLER
}

ACCB1 void ACCB2 
SnipRun::SnipServerCallback(void *clientData)
{
// removed the call into th documentation for PDFL, we need ot think of a different policy here to automatically bind to the documentation.
	SnipRun *p = (SnipRun *)clientData;
	SnipRun::thisSnip=p;
	DURING
		ASInt32 errcode = IDOK;
		ParamManager * thePM = NULL;
		if ((p->fDefaultParams.size())!=0){
			if (p->fParams.compare(string("")) == 0){
				p->fParams = p->fDefaultParams;
			}
		
			thePM = new ParamManager();
			errcode = thePM->getParams(p->fName,p->fDescription,p->fDefaultParams.c_str(),p->fParams.c_str());
		}
		if (errcode == IDOK){
			if (thePM!=NULL){
				p->fParams = thePM->getParamList();
			}
			p->Run(thePM);
		}
		//don't allow use of last params, since we want that to mark diff between command line and server
		p->fParams = "";
		if (thePM != NULL){
			delete thePM;
		}
		HANDLER
			char message[255];
			ASGetErrorString(ERRORCODE, message, sizeof(message));
			char message1[255];
#ifdef MAC_PLATFORM
			strlcpy(message1,"server:Exception in snippet ",sizeof(message1));
			strlcat(message1, p->GetUIName(),sizeof(message1));
#else
			strcpy_s(message1,sizeof(message1),"server:Exception in snippet ");
			strcat_s(message1,sizeof(message1), p->GetUIName());
#endif
			Console::displayString(message1); 
			Console::displayString(string("server:")+string(message));
		END_HANDLER
}
ASBool
SnipRun::toggleCheckedState(){
	ASBool previousState = fCheckedState;
	fCheckedState = !previousState;
	return previousState;
}

