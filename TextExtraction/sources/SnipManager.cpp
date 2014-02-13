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

 this file contains the implementation for the Snippet Manager.


*********************************************************************/

#include "SnipManager.h"

#ifdef PDFL_SDK_SAMPLE
#include "SnippetHierarchyUtils.h"
#endif

/*
	Factory methods. Single instance used.
*/
SnipManager* SnipManager::gSnipManager  = NULL;

/*
	maintain this class as a singleton
*/
SnipManager* 
SnipManager::Instance()
{
	if (gSnipManager == NULL)
	{
		gSnipManager = new SnipManager();
		gSnipManager->head = NULL;
	}
	return gSnipManager;
}


/*
*/
void 
SnipManager::DeleteInstance()
{
	while (gSnipManager->head!=NULL) {
		SnipRunNode* tmp = head->next;
		delete head;
		head = tmp;
	}
	if (gSnipManager != NULL)
		delete gSnipManager;
	gSnipManager = NULL;
}

/*
*/
SnipManager::SnipManager()
{
}

/*
*/
SnipManager::~SnipManager()
{
	if (gSnipManager != NULL){
		SnipManager::DeleteInstance();
	}
}

/*
*/
void 
SnipManager::PushSnippet(SnipRun* snippet)
{
	SnipRunNode * snr = new SnipRunNode;
	snr->snippet = snippet;
	snr->next = head;
	head = snr;

#ifdef PDFL_SDK_SAMPLE
	SnippetHierarchyUtils * theSHU = SnippetHierarchyUtils::Instance();
	theSHU->addSnippet(snippet);
#endif

}

/*
*/
SnipRun *
SnipManager::PopSnippet()
{
	if (head != NULL) {
		SnipRunNode * tmp = head;
		head = head->next;
		SnipRun * sr = tmp->snippet;
		delete tmp;
		return sr;
	}
	return NULL;
}

SnipRun *
SnipManager::getFirstSnippet(){
	current = head;
	if (current == NULL){
		return NULL;
	}
	return current->snippet;
}

SnipRun *
SnipManager::getNextSnippet(){
	if (current == NULL){
		return NULL;
	}
	current = current->next;
	if (current == NULL){
		return NULL;
	}
	return current->snippet;
}

// End, SnipRunManager.cpp.

