/*********************************************************************

 ADOBE SYSTEMS INCORPORATED
 Copyright (C) 2000-2006 Adobe Systems Incorporated
 All rights reserved.

 NOTICE: Adobe permits you to use, modify, and distribute this file
 in accordance with the terms of the Adobe license agreement
 accompanying it. If you have received this file from a source other
 than Adobe, then your use, modification, or distribution of it
 requires the prior written permission of Adobe.

 ---------------------------------------------------------------------

 ParamManager.cpp

  - Implements the abstraction over parameters for SnippetRunner

*********************************************************************/

#include "ParamManager.h"
#include "SnippetRunner.h"
#include "Console.h"
#include <string>
#include "ASCalls.h"

using namespace std;

ParamManager::~ParamManager(){
	SNode * tmp = head;
	while (tmp!=NULL) {
		tmp = head->next;
		delete head;
		head = tmp;
	}
}


#ifdef ACRO_SDK_PLUGIN_SAMPLE
//#ifndef UNIX_PLATFORM
//#include "SnippetParamDialog.h"
//#endif

ASInt32 
ParamManager::doDialog(const char * name, const char * desc, const char * defparams,const char * params){
	string tString;

	// we push the parameter list onto the queue structure
	head = tail = NULL;
//CC fix	if (SnippetParamDialog::DoDialog(name,desc,defparams,params,tString)==IDCANCEL){
//CC		return IDCANCEL;
//	}


	tString = params;
	bool notDone = true;
	while (notDone){
		if (tString[0] ==' '){
			// strip spaces
			tString = tString.substr(1,tString.length()-1);
		}
		else
		if (tString[0] == '\"') {
			// pull in strings
			int closingPos;
			// knock off the first '"'
			tString = tString.substr(1,tString.length()-1);
			if ((closingPos = tString.find("\"")) == string::npos){
				Console::displayString("server:Malformed string parameter!");
			}
			else {
				string tmpVal = tString.substr(0,closingPos);
				tString = tString.substr(closingPos+1,tString.length()-closingPos+1);
				this->push(tmpVal);
			}
		}
		else {
			int spacePos = tString.find_first_of(" ");
			if (spacePos == string::npos){
				// last word, push it and we are done
				this->push(tString);
				notDone = false;
			}
			else {
				string tmpVal = tString.substr(0,spacePos);
				tString = tString.substr(spacePos+1,tString.length()-spacePos);
				this->push(tmpVal);
			}

		}
	}
	return IDOK;
}
#else // PDFL
ASInt32 
ParamManager::doDialog(const char * name, const char * desc, const char * defparams,const char * params){
	string tString;
	//if we have params, ask
	if(params[0])
	{
		Console::displayString(desc);
		Console::displayString("\n");
		Console::displayString("Default parameters: ");
		Console::displayString(defparams);
		Console::displayString("\n");
			Console::displayString("enter your parameters, or '.' for default parameters\n");
		tString = Console::getString();
		if (tString.compare(".")==0) {
			tString = defparams;
		}
	}
	else
		tString = params;
	// we push the parameter list onto the queue structure
	head = tail = NULL;

	this->fParams = tString;
	ASBool notDone = true;
	while (notDone){
		if (tString[0] ==' '){
			// strip spaces
			tString = tString.substr(1,tString.length()-1);
		}
		else
		if (tString[0] == '\"') {
			// pull in strings
			int closingPos;
			// knock off the first '"'
			tString = tString.substr(1,tString.length()-1);
			if ((closingPos = tString.find("\"")) == string::npos){
				Console::displayString("server:Malformed string parameter!");
			}
			else {
				string tmpVal = tString.substr(0,closingPos);
				tString = tString.substr(closingPos+1,tString.length()-closingPos+1);
				this->push(tmpVal);
			}
		}
		else {
			int spacePos = tString.find_first_of(" ");
			if (spacePos == string::npos){
				// last word, push it and we are done
				this->push(tString);
				notDone = false;
			}
			else {
				string tmpVal = tString.substr(0,spacePos);
				tString = tString.substr(spacePos+1,tString.length()-spacePos);
				this->push(tmpVal);
			}

		}
	}
	return IDOK;
}
#endif
ASInt32 
ParamManager::getParams(const char * name, const char * desc, const char * defparams,const char * params){
	string tString;
	//if we have params, ask
	
	tString = params;
	// we push the parameter list onto the queue structure
	head = tail = NULL;

	ASBool notDone = true;
	while (notDone){
		if (tString[0] ==' '){
			// strip spaces
			tString = tString.substr(1,tString.length()-1);
		}
		else
		if (tString[0] == '\"') {
			// pull in strings
			int closingPos;
			// knock off the first '"'
			tString = tString.substr(1,tString.length()-1);
			if ((closingPos = tString.find("\"")) == string::npos){
				Console::displayString("server:Malformed string parameter!");
			}
			else {
				string tmpVal = tString.substr(0,closingPos);
				tString = tString.substr(closingPos+1,tString.length()-closingPos+1);
				this->push(tmpVal);
			}
		}
		else {
			int spacePos = tString.find_first_of(" ");
			if (spacePos == string::npos){
				// last word, push it and we are done
				this->push(tString);
				notDone = false;
			}
			else {
				string tmpVal = tString.substr(0,spacePos);
				tString = tString.substr(spacePos+1,tString.length()-spacePos);
				this->push(tmpVal);
			}

		}
	}
	return IDOK;
}


ASInt32 
ParamManager::getNumberParams(){
	int t = 0;
	SNode * tmp = head;
	while (tmp!=NULL){
		t++;
		tmp=tmp->next;
	}
	return t;
}

ASBool 
ParamManager::getNextParamAsInt(ASInt32 &param){
	param = atoi((this->pop()).c_str());
	return true;
}
	
ASBool 
ParamManager::getNextParamAsString(string &param){
	param = this->pop();
	if (param.length()>0) {
		return true;
	}
	else {
		return false;
	}
}
	
// dataentered might be aa55aa55, 6565454565654545
ASBool 
ParamManager::getNextParamAsHexData(void **dataEntered, ASInt32 &numEntries){
	string tmp = this->pop();
	ASInt32 strLen = tmp.length();
	ASInt32 numOfInts = strLen / 8;
	if(strLen % 8)
		numOfInts++;

	ASInt32 *dEntered = static_cast<ASInt32 *>(ASmalloc(numOfInts * 4));
	char * charRep = static_cast<char *>(ASmalloc(strLen + 1));
	char * charPtr = charRep;
#ifdef MAC_PLATFORM
	strlcpy(charRep, tmp.c_str(), strLen +1);
#else
	strcpy_s(charRep, strLen +1, tmp.c_str());
#endif
	numEntries = strLen / 2;
	if(strLen % 2)
		numEntries++;

	for(int lp = 0; lp < numOfInts;lp++) {
		ASInt32 tmp = 0;
#ifdef MAC_PLATFORM
		sscanf(charPtr, "%08x", &tmp);
#else
		sscanf_s(charPtr, "%08x", &tmp);
#endif
		dEntered[lp] = tmp;
		charPtr += 8;
	}

	ASfree(charRep);
	*dataEntered = static_cast<void *>(dEntered);
	return true;
}

ASBool 
ParamManager::getNextParamAsFixed(ASFixed &param){
	string tmp = this->pop();
	param = ASCStringToFixed(tmp.c_str());
	return true;
}

string 
ParamManager::pop(){
	if (head == NULL){
		return "";
	}
	string ret = tail->param;
	SNode * tmp = head;
	if (head == tail){
		delete head;tail=NULL;head=NULL;
		return ret;
	}
	else {
		while (tmp->next != tail){
			tmp=tmp->next;
		}
		tmp->next = NULL;
	}
	delete tail;
	tail = tmp;
	return ret;
}

void
ParamManager::push(string tmpVal){
	SNode * tmpSnode = new SNode;
	tmpSnode->param = tmpVal;
	tmpSnode->next = head;
	head = tmpSnode;
	if (tail == NULL) {
		tail = tmpSnode;
	}
}

