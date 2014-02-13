/*********************************************************************

 ADOBE SYSTEMS INCORPORATED
 Copyright (C) 2006 Adobe Systems Incorporated
 All rights reserved.

 NOTICE: Adobe permits you to use, modify, and distribute this file
 in accordance with the terms of the Adobe license agreement
 accompanying it. If you have received this file from a source other
 than Adobe, then your use, modification, or distribution of it
 requires the prior written permission of Adobe.

 ---------------------------------------------------------------------

 this file contains the abstraction over the a single command that might be
 input from the command line.

*********************************************************************/
#ifndef _ICommand_h_
#define _ICommand_h_

#include <string>
#include <vector>
#ifdef UNIX_PLATFORM
#include <stdio.h>
#endif

using namespace std;

class SnipRun;


/**
	Represents a single command that the user can enter. Inherit from this class and create 
	a static instance of your class to ensure the command is registered with the command
	line interpreter controller (CLIController).
*/
class ICommand
{
public:
	/** ctor with standard string meta-data */
	ICommand(const char * name, const char * description, char * synopsis);
	
	/** return the name of the command. The name of the command is the thing the user types in to
		invoke the command.
		@return the name of the command.
	*/
	string getName() const {return fName;};
	/** sets the name of the command. The name of the command is the thing the user types in to 
		invoke the command.
		@param name IN the name of the command
	*/
	void setName(string name) {fName=name;};
	
	/** return the descriptive text associated with this command.
		The descriptive text will be displayed by the help command.
		@return the descriptive help text for the command.
	*/	
	string getDescription() const {return fDescription;};
	/** sets the descriptive text for the command.
		The descriptive text will be displayed by the help command.
		@param description IN the descriptive text for the command.
	*/
	void setDescription(string description) {fDescription=description;};
		
	/** return the command synopsis. This is a one-line description and is 
		used to briefly state the purpose of the command.
		@return the command's synopsis
	*/
	string getSynopsis() const {return fSynopsis;};
	/** sets the command's synopsis. The synopsis is used to state (one line) the command purpose.
		@param synopsis IN the command's synopsis.
	*/
	void setSynopsis(string synopsis) {fSynopsis=synopsis;};
		
			/** generalized global lookup for one snippet
			@param snippet to look for
			@return first snippet that matches name
		*/
	SnipRun* FindSnippet(string snippetUIName);

	/** executes the command. The only real entry point in the command. The parameter list is the line
		of text that invoked the command, minus the command's name. The command must parse the text to
		determine how to handle the parameters.
		@param parameterList IN the remainder of the input line, minus the command name. The command 
				must parse this text and is responsible for reasoning about its validity.
		@return returns the result, if any.
	*/
	virtual void executeCommand(string parameterList) = 0;
	
	//method called when executing a server command
	virtual string executeServerCommand(string parameterList, string& ResultString) = 0;

private:
	/** the name of the command */
	string fName;
	/** the synopsis for the command, a single line description */
	string fSynopsis;
	/** the description of the command. Consider this as help text */
	string fDescription;
};

/**
	Register a command. This is a utility acro for common commands. You do not need to use it. You could just inherit from
	the base class directly...

	@param _NAME of the command
	@param _COMMAND the text the user types in to invoke the command
	@param _DESCRIPTION help text for the command
	@param _SYNOPSIS a one line description of the class.
*/
#define COMMAND_REGISTER(_NAME,_COMMAND,_DESCRIPTION,_SYNOPSIS) \
	class _NAME##Command: public ICommand \
	{ \
	public: \
		 _NAME##Command() : ICommand (_COMMAND,_DESCRIPTION,_SYNOPSIS) {}; \
		~_NAME##Command() {}; \
		void executeCommand(string ParameterList); \
		string executeServerCommand(string ParameterList, string& resultString); \
	}; \
	_NAME##Command _NAME##CommandVar;

#endif
