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
#ifndef _Snip_Manager_h_
#define _Snip_Manager_h_

#include "SnippetRunner.h"
/**
	Manages all registered snippets. 
*/
class SnipManager
{
protected:
	/**
		Constructor is protected because this is a singleton.
	*/
	SnipManager();
	
public:
	/**
		Returns the global instance of this singleton, call like this SnipRunManager::Instance().
		@return the global instance of this singleton class.
	*/
	static SnipManager* Instance();

	/**
		Destroys the single global instance of this singleton.
	*/
	void DeleteInstance();

	/**
	*/
	virtual ~SnipManager();

	/**
		Add snippet to list of registered snippets.
	*/
	void PushSnippet(SnipRun* snippet);

	/**
		Remove snippet from list of registered snippets.
	*/
	SnipRun * PopSnippet();
	
	/** resets the iterator to the first snippet, returning this snippet
		@return first snippet maintained by tha manager, NULL if manager has no snippets
	*/
	SnipRun * getFirstSnippet();
	
	/** returns next snippet maintained by the manager
		@return next snippet, null if manager has no snippets or if the iterator goes
		beyond the last snippet
	*/
	SnipRun * getNextSnippet();

	struct SnipRunNode {
		SnipRun * snippet;
		SnipRunNode * next;
	};

private:
	static SnipManager* gSnipManager;
	SnipRunNode * head;
	/** for the iterator, does not need to be static as this is a singleton */
	SnipRunNode * current;
	/** Tree based snippet structure */
	SnipRunNode * root;
};

#endif



