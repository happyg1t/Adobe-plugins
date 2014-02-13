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
#include <string>
#include "ICommand.h"
#include "SnipManager.h"
#include "SnippetRunner.h"

using namespace std;

/** ctor with standard string meta-data */
ICommand::ICommand(const char * name, const char * description, char * synopsis) : fName(string(name)), fDescription(string(description)), fSynopsis(string(synopsis))
{
	//CLIController* cliController = CLIController::Instance();
	//cliController->PushCommand(this);
}

//will only find first snippet that matches exactly, not to be used by FindSnipCommand
SnipRun* ICommand::FindSnippet(string snippetUIName) 
{
	SnipManager * sm = SnipManager::Instance();
	if (sm == NULL)
		return NULL;
	
	//We are going to support two different types of matches, full path, and terminal node.
	//we search snippetUIName for a ':', and if found, search on full path, else just the 
	//terminus
	bool fullPathSearch = true;

	if(snippetUIName.find_first_of(':') == string::npos)
		fullPathSearch = false;
	
	SnipRun * tmpSnip = NULL;
	SnipRun * snippet = sm->getFirstSnippet();
	while (snippet!=NULL){
		string name = string(snippet->GetUIName());
		// found our snippet?
		if(fullPathSearch)
		{
			if (name.compare(snippetUIName)==0) //exact match
				break;
		}
		else
		{
			//find last ':', and compare terminal nodes
			int index = name.find_last_of(':');
			string terminalName = name.substr(index+1);
			if(terminalName.compare(snippetUIName) == 0)
				break;
		}

		snippet = sm->getNextSnippet();
	}
	return snippet;
}

