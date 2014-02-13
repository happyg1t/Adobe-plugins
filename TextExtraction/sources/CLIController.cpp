/*
//
//  ADOBE SYSTEMS INCORPORATED
//  Copyright (C) 2000-2006 Adobe Systems Incorporated
//  All rights reserved.
//
//  NOTICE: Adobe permits you to use, modify, and distribute this file
//  in accordance with the terms of the Adobe license agreement
//  accompanying it. If you have received this file from a source other
//  than Adobe, then your use, modification, or distribution of it
//  requires the prior written permission of Adobe.
//
*/

#include <stdlib.h>
#include <string>
#include "CLIController.h"
#include "Console.h"
#include "SnippetRunnerUtils.h"
#include "ASCalls.h"
#include "CDocument.h"

#ifndef WIN_PLATFORM
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#ifdef ACRO_SDK_PLUGIN_SAMPLE
#define PORT 5000
#else
#define PORT 5001
#endif

//define this if you want to make soap implementation available
//#define USE_SOAP

#define NO_ERR 0L
#define ERR -1L
#define MAX_LENGTH 10000
//#define EXTREMELY_VERBOSE 1 //not good for acrobat, as accept loop will print every time
//#define VERBOSE 1
//#define SLEEP 1


#define SOAP_MESSAGE_START \
"<?xml version='1.0' ?>\
<env:Envelope xmlns:env=\"http://www.w3.org/2006/05/soap-envelope\"> \
<env:Body>\
<p:SnippetRunner xmlns:p=\"http://sr.adobe.com/commands\">\
<p:response>"

#define SOAP_MESSAGE_END \
"</p:response>\
</p:SnippetRunner>\
 </env:Body>\
</env:Envelope>"

/*
	Factory methods. Single instance used.
*/
CLIController* CLIController::gCLIController  = NULL;
string CLIController::resultString  = "";
string CLIController::lastResultString  = "";

/*
	maintain this class as a singleton
*/
CLIController* 
CLIController::Instance()
{
	if (gCLIController == NULL)
	{
		gCLIController = new CLIController();
	}
	return gCLIController;
}

#ifdef ACRO_SDK_PLUGIN_SAMPLE
ACCB1 void CLIController::CLIIdleProc (void * clientData)
{
	if(!gCLIController)
	{
		gCLIController = CLIController::Instance();
	}
	
	gCLIController->processOneCommand();
}
#endif

/*
	We delete the controller, making sure we delete all the commands it maintains.
*/
void 
CLIController::deleteInstance()
{
	while (gCLIController->head!=NULL) {
		CommandNode* tmp = head->next;
		delete head;
		head = tmp;
	}
	if (gCLIController != NULL)
		delete gCLIController;
	gCLIController = NULL;
}

/*
	ctor
*/
CLIController::CLIController() : isListening(false), currentOpenSocket(0)
{
	head = NULL;
#ifdef WIN_PLATFORM
	WSADATA wsaData;
	int wsaret = WSAStartup(0x101, &wsaData);
#endif
	struct sockaddr_in sktaddr;
	memset(&sktaddr, 0L, sizeof(sktaddr));
	sktaddr.sin_family = AF_INET;

#ifdef ACRO_SDK_PLUGIN_SAMPLE
	sktaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
#else
	sktaddr.sin_addr.s_addr = htonl(INADDR_ANY);
#endif

	sktaddr.sin_port = htons(PORT);

	mainSocket=socket(AF_INET, SOCK_STREAM, 0);
	if(mainSocket < 0)
	{
		printf("Could not open stream socket\n");
		return;
	}
 	int slen = sizeof(sktaddr);
 	
 	//allow reuse of socket addresses, as needed
 	int on = 1;
 	 setsockopt( mainSocket, SOL_SOCKET, SO_REUSEADDR, (char*) &on, sizeof(on));
 	 
	int res = bind(mainSocket,(const sockaddr*)&sktaddr,slen);
	if(res >= 0)
	{
#ifdef WIN_PLATFORM
		res = getsockname(mainSocket,(sockaddr*) &sktaddr, &slen);
#else
		res = getsockname(mainSocket,(sockaddr*) &sktaddr, (socklen_t*)(&slen));
#endif
		if(res == 0)
		{

//dont block for acrobat, pdfl should
#ifdef ACRO_SDK_PLUGIN_SAMPLE
#ifndef WIN_PLATFORM
			int flags = fcntl(mainSocket, F_GETFL, 0);
			res = fcntl(mainSocket, F_SETFL, flags | O_NONBLOCK);
			if(res < 0)
			{
				perror("fcntl() failed ");
				return;
			}
#else
			u_long mode = 1;
			ioctlsocket(mainSocket, FIONBIO, &mode);
#endif
#endif
			res = listen(mainSocket, 5);
			if(res == 0)
				isListening = true;
			else
				perror("listen() failed ");
		}		
			
	}
}

/*
	dtor just deletes the instance, if it exists (though, it should exist)
*/
CLIController::~CLIController()
{
	if (gCLIController != NULL){
		CLIController::deleteInstance();
	}
}

/*
*/
void 
CLIController::PushCommand(ICommand* theCommand)
{
	CommandNode * snr = new CommandNode;
	snr->command = theCommand;
	snr->next = head;
	head = snr;
}

/*
*/
ICommand *
CLIController::PopCommand()
{
	if (head != NULL) {
		CommandNode * tmp = head;
		head = head->next;
		ICommand * sr = tmp->command;
		delete tmp;
		return sr;
	}
	return NULL;
}

ICommand *
CLIController::getFirstCommand(){
	current = head;
	if (current == NULL){
		return NULL;
	}
	return current->command;
}

ICommand *
CLIController::getNextCommand(){
	if (current == NULL){
		return NULL;
	}
	current = current->next;
	if (current == NULL){
		return NULL;
	}
	return current->command;
}


ICommand * 
CLIController::getCommand(string input){
	ICommand * theCommand = this->getFirstCommand();
	while (theCommand != NULL){
		string tmpName = theCommand->getName();
		// test for the command
		if (ASBoolToBool(SnippetRunnerUtils::stringsMatch(tmpName,input)) == true){
			break;
		}
		// move to the next command in the list.
		theCommand = this->getNextCommand();
	}
	// returns the command if found, NULL otherwise
	return theCommand;
}

string 
CLIController::getCommandList(){
	string tmpString = "";
	ICommand * theCommand = this->getFirstCommand();
	while (theCommand != NULL){
		string tmpName = theCommand->getName();
		tmpString += tmpName + "\t-\t";
		string tmpSyn = theCommand->getSynopsis();
		tmpString += tmpSyn + '\n';
		theCommand = this->getNextCommand();
	}
	// returns the command if found, NULL otherwise
	return tmpString;
}

/* represents the main event loop */
ICommand * 
CLIController::waitForCommand(string prompt){
	this->setCommandPrompt(prompt);
	int inLoop = 1;
	ICommand * theCommand = NULL;
	string response = "";
	string name = "";
	string params = "";
	while (inLoop == 1){
		while(theCommand == NULL){
			string dispString = this->getCommandPrompt();
			PDDoc theDoc = CDocument::GetPDDoc();
			if (theDoc != NULL){
				ASInt32 docFlags = PDDocGetFlags(theDoc);
				if ((docFlags&PDDocNeedsSave)!=0) {
					dispString.replace(dispString.length()-2,string::npos,"*>");
				}
			}
			Console::displayString(dispString);
			response = Console::getString();
			// split of the space character:
			int charPos = 0;
			if ((charPos = response.find(' ',0))== string::npos){
				name = response;
				params = "";
			}
			else {
				name = response.substr(0,charPos);
				params = response.substr(charPos+1,string::npos);
			}
			theCommand = this->getCommand(name);
			if (theCommand == NULL){
				Console::displayString("Unknown command, try \"Help\" for commands\n");
			}
		}
		DURING
			theCommand->executeCommand(params);
		HANDLER
			char buf[256];
			char tbuff[280];
			ASGetErrorString(ERRORCODE, buf, sizeof(buf));
#ifdef MAC_PLATFORM
			snprintf(tbuff, sizeof(tbuff), "error code: %d Error message %s\n",ERRORCODE,buf);
#else
			sprintf_s(tbuff, sizeof(tbuff), "error code: %d Error message %s\n",ERRORCODE,buf);
#endif
			Console::displayString(tbuff);

		END_HANDLER

		if ((theCommand->getName().compare("Quit"))==0){
			Console::displayString("\nExiting application");
			Console::flush();
			break;
		}
		else {
			theCommand = NULL;
		}
	}
	return theCommand;
}
//This will read one line of info, stopping at '\0' 
// '\r' and '\n' will be stripped from the returned line
//its expected that message length will be less than MAX_LENGTH
int CLIController::ReadLine(char *message) {
  
	static int ptr=0;
	static int count;
	int offset;

	offset=0;

	/* read data from socket */
	memset(message,0x0,1); /* init buffer */
#ifdef SLEEP
	Console::displayString("sleeping.\n");
	sleep(2);
#endif
	count = recv(currentOpenSocket, message, MAX_LENGTH, 0); /* wait */
#ifdef VERBOSE
	char temp[30];
#ifdef MAC_PLATFORM
	snprintf(temp, sizeof(temp), "Read %d chars\n", count);
#else
	sprintf_s(temp, sizeof(temp), "Read %d chars\n", count);
#endif
	Console::displayString(temp);
#endif
	//error? maybe client socket closed, so clean up our end
	if (count<0) 
	{
		return ERR;
	}
	
	//check content length and wait for whole message
	if(strncmp(message, "POST", 4) == 0)
	{
		int cLength = 0;
		char* messagePtr = message;
		char* compareStr =  "Content-Length:";
		int compareLength = strlen(compareStr);
		for(int i = 0; i< count; i++, messagePtr++)
		{
			if(strncmp(messagePtr, compareStr, compareLength)==0)
			{
				cLength = atoi(&message[i+compareLength]);
				break;
			}
		}
		int returnVal = 0;
		//if count < content length do several more reads until read count is = 0
		while(count < cLength)
		{
			returnVal = recv(currentOpenSocket, &message[count], MAX_LENGTH-count, 0);
			
			if(returnVal > 0)
				count += returnVal;
		}
		return count;
	}
		
	//null terminate regular message (these are short, and so one read should take care of it
	//breaking point seems to be about 200 characters.
	while((message[count-1] == '\n') || (message[count-1] == '\r'))
	{
		message[count-1] =  '\0';
		count--;
	}
	return count;
}

void CLIController::WriteLine(string lineToWrite) 
{
#ifndef WIN_PLATFORM
  send(currentOpenSocket, (const void*)lineToWrite.c_str(), lineToWrite.length(), 0);
#else
  send(currentOpenSocket, lineToWrite.c_str(), lineToWrite.length(), 0);
#endif
}

void CLIController::WriteCharString(char* lineToWrite, int len) 
{
#ifndef WIN_PLATFORM
	send(currentOpenSocket, (const void*)lineToWrite, len, 0);
#else
  send(currentOpenSocket, lineToWrite, len, 0);
#endif
}

                                       // Base64 Index into encoding
int  pEncodingIndex[64] = {                   // and decoding table.
                        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
                        0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
                        0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
                        0x59, 0x5a, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
                        0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e,
                        0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
                        0x77, 0x78, 0x79, 0x7a, 0x30, 0x31, 0x32, 0x33,
                        0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x2b, 0x2f
                        };

//mostly from http://www.marcsweb.com/mnweb_ccppbase64.shtml
int b64encode(const char *str, char* output)
{
    int     l   = strlen(str);                // Get the length of the string.
    int     x   = 0;                        // General purpose integers.
    char   *b, *p;                          // Encoded buffer pointers.

	b = output;

	int expandedLength = ((l+3) / 3) * 4;
    memset(b, 0x3d, expandedLength);      // Initialize it to "=". 

    p = b;                                  // Save the buffer pointer.
    x = 0;                                  // Initialize string index.

    while   (x < (l - (l % 3)))             // encode each 3 byte octet.
    {
        *b++   = pEncodingIndex[  (str[x] >> 2)];
        *b++   = pEncodingIndex[((str[x]     & 0x03) << 4) + (str[x + 1] >> 4)];
        *b++   = pEncodingIndex[((str[x + 1] & 0x0f) << 2) + (str[x + 2] >> 6)];
        *b++   = pEncodingIndex[  str[x + 2] & 0x3f];
         x    += 3;                         // Next octet.
    }

    if (l - x)                              // Partial octet remaining?
    {
        *b++        = pEncodingIndex[str[x] >> 2];    // Yes, encode it.

        if  (l - x == 1)                    // End of octet?
            *b      = pEncodingIndex[ (str[x] & 0x03) << 4];
        else                            
        {                                   // No, one more part.
            *b++    = pEncodingIndex[((str[x]     & 0x03) << 4) + (str[x + 1] >> 4)];
            *b      = pEncodingIndex[ (str[x + 1] & 0x0f) << 2];
        }
    }

    return expandedLength; 
}


bool 
CLIController::processOneCommand()
{
	ICommand * theCommand = NULL;
	string name = "";
	string params = "";
	
	struct sockaddr_in cliAddr;
	static int cliLen = sizeof(cliAddr);
	char line[MAX_LENGTH];
	
	if(currentOpenSocket <= 0)
	{
#ifdef EXTREMELY_VERBOSE
		Console::displayString("Getting new socket.\n");
#endif
#ifndef WIN_PLATFORM
		currentOpenSocket = accept(mainSocket, (struct sockaddr *) &cliAddr, (socklen_t*)(&cliLen));
		if(currentOpenSocket > 0)
		{
			int flags = fcntl(currentOpenSocket, F_GETFL, 0);
			int res = fcntl(currentOpenSocket, F_SETFL, flags | O_NONBLOCK);
		}
#endif
#ifdef WIN_PLATFORM
		currentOpenSocket = accept(mainSocket, (struct sockaddr *) &cliAddr, &cliLen);
#endif
	}

    if(currentOpenSocket>0)
    { 
#ifdef VERBOSE
    	Console::displayString("Connection made.\n");
#endif
	    /* init line */
	   line[0] = '\0';
	   bool isSoapMessage = false;
	    /* get segments */
	   int result = 0;
	    if((result = ReadLine(line))> 0) 
	    {
#ifdef USE_SOAP
			// only place isSoapMessage could be set to true, 
			// so this bottleneck effectively controls the soap interface
	    	if(strncmp(line, "POST", 4) == 0)
	    		isSoapMessage = true;
#endif		    
		    bool haveParams = false;
		    
		    if(!isSoapMessage)
		    {
			   string lineStr(line);
		    	
		    	int index = lineStr.find(" ");
		   
			    if(index > 0)
			    {
			    	name = lineStr.substr(0, index);
			    	params = lineStr.substr(index+1);
			    	haveParams = true;
			    	
			    }
			    else
			    	name = lineStr;
		    }
		    else
		    {
		    	char* findCommandStr = "Body><ns0:";
		    	int startIndex = -1;
		    	int endIndex;
		    	for(int i = 0; i< result;i++)
		    	{
		    		if(line[i] == 'B')
		    			if(strncmp(&line[i], findCommandStr, 10)==0)
		    			{
		    				startIndex = i;
		    				break;
		    			}
		    	}
		    	if(startIndex != -1)
		    	{
			    	startIndex += strlen(findCommandStr);
			    	endIndex = startIndex;
			    	//need to check for space, the other is just for error checking
			    	while(line[endIndex]!=' ' && line[endIndex]!='<')
			    		endIndex++;
			    	line[endIndex] = '\0';
			    	name = &line[startIndex]; 		
		    	}
		    	
		    	char* findParamsStr = "xsd:string\">";
		    	startIndex = endIndex;
		    	for(int i = endIndex; i< result;i++)
		    	{
		    		if(line[i] == 'x')
		    			if(strncmp(&line[i], findParamsStr, 12)==0)
		    			{
		    				startIndex = i;
		    				break;
		    			}
		    	}
		    	if(startIndex != -1)
		    	{
			    	startIndex += strlen(findParamsStr);
			    	int endIndex = startIndex;
			    	char* strPtr = &line[endIndex];
			    	while(*strPtr!='<')
			    		strPtr++;
			    	*strPtr = '\0';
			    	params = &line[startIndex]; 		
		    	}
		    	else
		    		haveParams = false;
		    }
		   
	#ifdef VERBOSE	    
		    if(haveParams)
		    	printf("command: %s, params: %s\n", name.c_str(), params.c_str());
		   	else
		   		printf("command: %s, no params\n", name.c_str());
	#endif
		      theCommand = this->getCommand(name);

			if(isSoapMessage)
				WriteLine(SOAP_MESSAGE_START);
		      
		    resultString = "";
			if (theCommand != NULL)
			{			
				theCommand->executeServerCommand(params, resultString);
				
				resultString += ".\n";
				
				//if soap, encode and send, else just send
				if(isSoapMessage)
				{
					char* encodedOutput = (char*)ASmalloc(((resultString.length()+3)/3)*4);
					int len = ::b64encode(resultString.c_str(), (char*)encodedOutput);
					WriteCharString(encodedOutput, len);
					ASfree(encodedOutput);
				}
				else
					WriteLine(resultString);
				
				lastResultString = resultString;
				
	#ifdef VERBOSE
				printf("wrote data - %s\n", resultString.c_str());
	#endif
			}
			else
		    	WriteLine("error: unknown command.\n.\n");
			
			if(isSoapMessage)
			{
				WriteLine(SOAP_MESSAGE_END);
				//force single shot...
				name = "quit";
			}	
			
		    if((name.compare("quit") == 0) || (name.compare("bye") == 0))
		    {
#ifndef WIN_PLATFORM
			close(currentOpenSocket);
#else
			//a rather rude and abrupt shutdown, but...
			::shutdown (currentOpenSocket, 2);
			::closesocket (currentOpenSocket);
#endif
		    	currentOpenSocket = 0;
		    }
	    } /* if(read_line) */
	    else
	    {

//looks like this is old code, not active for plugins to begin with, and PDFL doesn't work with it.
#if 0
#ifndef WIN_PLATFORM
			close(currentOpenSocket);
#else
			//a rather rude and abrupt shutdown, but...
			::shutdown (currentOpenSocket, 2);
			::closesocket (currentOpenSocket);
#endif
#ifdef VERBOSE
			printf("Socket closed\n");
#endif
			currentOpenSocket = NULL;
#endif
	    }

#ifdef VERBOSE 
	    if(result <= 0)
		{
			Console::displayString("Connection done.\n");
			
			if(result == 0)
				Console::displayString("result == 0.\n");
		}
#endif
    }
    
    return true;
}

