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

 this file contains the abstraction over the snippet runner's
 command line interface.It is a singleton.


*********************************************************************/
#ifndef _CLI_Controller_h_
#define _CLI_Controller_h_

#include "ICommand.h"
#include <string>
#ifndef MAC_PLATFORM

#ifdef ACRO_SDK_PLUGIN_SAMPLE
#include "Environ.h"
#endif
#endif

using namespace std;
/**
	Manages all registered commands. A command is a little bit of functionality that is manifest by
	an implementation of the ICommand interface. This class maintains the list of the commands, and
	handles the invoking of the specific command. 
	
	A command is a simple unit of "work", a single interaction from the user, such as "quit" to leave the applciation.
	This class maintains a list of the commands and provides the mechanism required to invoke a particular command.
	
*/
class CLIController
{
protected:
	/**
		Constructor is protected because this is a singleton.
	*/
	CLIController();
	
public:
	/**
		Returns the global instance of this singleton, call like this CLIController::Instance().
		@return the global instance of this singleton class.
	*/
	/**
		Destroys the single global instance of this singleton.
	*/
	void deleteInstance();

	/**
	*/
	virtual ~CLIController();

#ifdef ACRO_SDK_PLUGIN_SAMPLE
	/**
		Idle proc that polls the server interface
	*/
	static ACCB1 void CLIIdleProc (void * clientData);
#endif	

	static CLIController* Instance();

	/** Signifies the main event loop. This function will wait for user text to be entered, if the command was valid,
		it is returned to the caller. If not, we wait...
		@param prompt IN the text to use as a prompt character
		@return a valid command entered by the user
	*/
	ICommand * waitForCommand(string prompt);
	
	/** Reads a string from the socket, and handles the command.
		@return true to continue, false otherwise. Currently false only if quitting
	*/
	bool processOneCommand();

	/**
		Add a command to the list of registered commands.
		@param theCommand IN the command we want to add to the list of supported commands
	*/
	void PushCommand(ICommand* theCommand);

	/**
		Remove and return the first command from the list of commands we have.
		@return the command removed from the list.
	*/
	ICommand * PopCommand();
	
	/** resets the iterator to the first command, returning this command
		@return first command on the CLIController's list of commands, NULL there are no commands present
	*/
	ICommand * getFirstCommand();
	
	/** returns next command maintained by the controller
		@return next command, null if controller has no commands or if the iterator goes
		beyond the last command
	*/
	ICommand * getNextCommand();
	
	/** find the command that matches the string input. Basically matches the string with the name of the command.
		@param input IN the string name of the command we are looking for.
		@return the command that matches the string, or NULL if the command is not implemented.
	*/
	ICommand * getCommand(string input);
	
	/** returns a formatted string of all the commands supported, with a brief synopsis of the command. 
		This will be used for help purposes.
		@return a formatted string with the set of all commands supported.
	*/	
	string getCommandList();

	/** set the command prompt for the CLI */
	void setCommandPrompt(string newValue) {fCommandPrompt=newValue;};

	/** get the command prompt for the CLI */
	string getCommandPrompt() {return fCommandPrompt;};
	
	/** get the last resultString for the CLI */
	string getLastResultString() {return lastResultString;};
	void appendToResultString(string strToAppend)
					{ resultString += strToAppend; }
	
	/** writes to currentOpenSocket */
	void WriteLine(string lineToWrite);
	void WriteCharString(char* lineToWrite, int len) ;
	
	/** reads from currentOpenSocket */
	int ReadLine(char* message);
	
	bool HasOpenSocket() { return  currentOpenSocket != 0; }

	struct CommandNode {
		ICommand * command;
		CommandNode * next;
	};
private:
	static CLIController* gCLIController;
	CommandNode * head;
	/** for the iterator, does not need to be static as this is a singleton */
	CommandNode * current;
	string fCommandPrompt;
	
	//used for string to send back to server
	static string resultString;
	//saved result string from last command
	static string lastResultString;
	
	//socket data
	bool isListening;
	int mainSocket;
	int currentOpenSocket;
	
};

#endif
