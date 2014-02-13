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

 Console.cpp

 - Simple IO console utility methods for snippets to use

*********************************************************************/
#include "Console.h"
#include "SnippetRunnerUtils.h"
#ifdef UNIX_PLATFORM
#include <stdio.h>
#endif

#include <set>

static ACCB1 ASBool ACCB2 myCosObjEnumProc(CosObj obj, CosObj value, void* clientData);	
// we need to keep track of the depth of cos objects we go to, there are structures that are recursive...
static ASInt32 cosLevelDepth = 0;

static set<ASInt32> cosObjSet;
static ASBool detailedRequired = false;
static ASBool objDumpTerminate = false;


const void 
Console::displayString(string output) 
{


}

const void 
Console::displayStringNow(string output) 
{
}


void  
Console::displayString(void *msg, ASInt32 numOfInts)
{
	ASText outPutVal = ASTextNew();
	ASInt32 *Msgptr = static_cast<ASInt32 *>(msg);
	char data[ 10 ];
	memset(static_cast<void *>(data),0,sizeof(data));
	ASText retchar=ASTextFromScriptText("\n",kASRomanScript);
	for (int lp=0;lp<numOfInts;lp++){
#ifdef MAC_PLATFORM
		snprintf(data, sizeof(data), "%08x",*Msgptr);
#else
		sprintf_s(data, sizeof(data), "%08x",*Msgptr);
#endif
		Msgptr++;
		ASText newASCIIInt = ASTextFromScriptText(data,kASRomanScript);
		ASTextCat(outPutVal,newASCIIInt);
		if ((lp+1)%8==0){
			ASTextCat(outPutVal,retchar);
		}
		ASTextDestroy(newASCIIInt);
	}
	Console::displayString(ASTextGetScriptText(outPutVal, kASRomanScript));
	ASTextDestroy(outPutVal);
	ASTextDestroy(retchar);
}

void  
Console::displayString(const char *msg)
{
	string strMsg(msg);
	strMsg += "\n";
	Console::displayString(string(msg));
}

void  
Console::displayStringNow(const char *msg)
{
	string strMsg(msg);
	strMsg += "\n";
	Console::displayStringNow(strMsg);
}
void  
Console::displayString(ASText msg)
{
	Console::displayString(ASTextGetScriptText(msg, kASRomanScript));
}

void 
Console::displayStringNow(const ASUTF16Val* msg)
{
}

void  
Console::displayStringNow(ASText msg)
{
	// Support Unicode-encoded string display, calling displayStringNow(ASUTF16Val*)
	const ASUTF16Val* uMsg = ASTextGetUnicodeCopy(msg, kUTF8);
	Console::displayStringNow(uMsg);
}

void
Console::displayString(CosObj thisObj, ASBool brief){
	cosObjSet.clear();
	objDumpTerminate = false;
	if (ASBoolToBool(brief) == true){
		detailedRequired = false;
	}
	else {
		cosLevelDepth = 0;
		detailedRequired = true;
	}
	Console::CosObjDump(thisObj,"");
}


void
Console::CosObjDump(CosObj thisObj, char * preamble){
	cosLevelDepth++;
	char buffer[500];
	char * indirect = "Dir";
	ASInt32 cosObjID = 0;
	ASUns16 cosObjGen = 0;
	ASBool isIndirect;
	CosType theType = CosObjGetType(thisObj);
	if (ASBoolToBool(isIndirect=CosObjIsIndirect(thisObj)) == true){
		cosObjID = CosObjGetID(thisObj);
		cosObjGen = CosObjGetGeneration(thisObj);
#ifdef MAC_PLATFORM
		snprintf(buffer,sizeof(buffer),"%s(Indir %d %d R)",preamble,cosObjID,cosObjGen);
#else
		sprintf_s(buffer,sizeof(buffer),"%s(Indir %d %d R)",preamble,cosObjID,cosObjGen);
#endif
		Console::displayString(buffer);
		indirect = "InDir";
	}	
	switch (theType){
		case CosNull:
			{
#ifdef MAC_PLATFORM
				snprintf(buffer,sizeof(buffer),"%s(%s CosNull)",preamble,indirect);
#else
				sprintf_s(buffer,sizeof(buffer),"%s(%s CosNull)",preamble,indirect);
#endif
				Console::displayString(buffer);
			}
		break;
		case CosInteger:
			{
				ASInt32 IntVal = CosIntegerValue(thisObj);
#ifdef MAC_PLATFORM
				snprintf(buffer,sizeof(buffer),"%s(%s integer - val == %d)",preamble,indirect,IntVal);
#else
				sprintf_s(buffer,sizeof(buffer),"%s(%s integer - val == %d)",preamble,indirect,IntVal);
#endif
				Console::displayString(buffer);
			}
		break;
		case CosFixed:
			{
				ASFixed FixedVal =CosFixedValue(thisObj);
				char fixedBuff[10];
				ASFixedToCString(FixedVal, fixedBuff, 9, 3);
#ifdef MAC_PLATFORM
				snprintf(buffer,sizeof(buffer),"%s(%s fixed - value == %s)",preamble,indirect,fixedBuff);
#else
				sprintf_s(buffer,sizeof(buffer),"%s(%s fixed - value == %s)",preamble,indirect,fixedBuff);
#endif
				Console::displayString(buffer);
			}
		break;
		case CosBoolean:
			{
				ASBool BoolVal = CosBooleanValue(thisObj);
#ifdef MAC_PLATFORM
				snprintf(buffer,sizeof(buffer),"%s(%s boolean - value == %d)",preamble,indirect,BoolVal);
#else
				sprintf_s(buffer,sizeof(buffer),"%s(%s boolean - value == %d)",preamble,indirect,BoolVal);
#endif
				Console::displayString(buffer);
			}
		break;
		case CosName:
			{
				ASAtom NameVal = CosNameValue(thisObj);
#ifdef MAC_PLATFORM
				snprintf(buffer,sizeof(buffer),"%s(%s name - value == %s)",preamble,indirect,ASAtomGetString(NameVal));
#else
				sprintf_s(buffer,sizeof(buffer),"%s(%s name - value == %s)",preamble,indirect,ASAtomGetString(NameVal));
#endif
				Console::displayString(buffer);
			}
 		break;
		case CosString:
			{
				ASTCount numOfChars;
				char* strVal = CosStringValue(thisObj, &numOfChars);
#ifdef MAC_PLATFORM
				snprintf(buffer,sizeof(buffer),"%s(%s string - number of chars %d - value == %s)",preamble,indirect,numOfChars,strVal);
#else
				sprintf_s(buffer,sizeof(buffer),"%s(%s string - number of chars %d - value == %s)",preamble,indirect,numOfChars,strVal);
#endif
				Console::displayString(buffer);
			}
		break;
		case CosDict:
			{
#ifdef MAC_PLATFORM
				snprintf(buffer,sizeof(buffer),"%s(%s dict::)**",preamble,indirect);
#else
				sprintf_s(buffer,sizeof(buffer),"%s(%s dict::)**",preamble,indirect);
#endif
				Console::displayString(buffer);
				char newbuff[50];
#ifdef MAC_PLATFORM
				snprintf(newbuff,sizeof(newbuff),"\t%s",preamble);
#else
				sprintf_s(newbuff,sizeof(newbuff),"\t%s",preamble);
#endif
				if (cosObjID!=0){
					if (cosObjSet.find(cosObjID)!=cosObjSet.end()){
#ifdef MAC_PLATFORM
						snprintf(buffer,sizeof(buffer),"%s\tObject repetition",preamble);
#else
						sprintf_s(buffer,sizeof(buffer),"%s\tObject repetition",preamble);
#endif
						Console::displayString(buffer);
					}else {
						cosObjSet.insert(cosObjID);
						// we track cos level depth for really complicated documents.
						if (cosLevelDepth<5){
							if (objDumpTerminate == false){
								CosObjEnum(thisObj, myCosObjEnumProc,static_cast<void *>(newbuff));
							}
						}
						else {
#ifdef MAC_PLATFORM
							snprintf(buffer,sizeof(buffer),"%sObject too complex - continue navigation using CosObjectExplorerSnip",preamble);
#else
							sprintf_s(buffer,sizeof(buffer),"%sObject too complex - continue navigation using CosObjectExplorerSnip",preamble);
#endif
							Console::displayString(buffer);
						}
					}
#ifdef MAC_PLATFORM
					snprintf(buffer,sizeof(buffer),"%s**",preamble);
#else
					sprintf_s(buffer,sizeof(buffer),"%s**",preamble);
#endif
					Console::displayString(buffer);
				}
				else {
					CosObjEnum(thisObj, myCosObjEnumProc,static_cast<void *>(newbuff));				
				}
			}
		break;
		case CosArray:
			{
				ASTArraySize arrLen = CosArrayLength(thisObj);
#ifdef MAC_PLATFORM
				snprintf(buffer,sizeof(buffer),"%s(%s array - number of elements == %d::)[",preamble,indirect,arrLen);
#else
				sprintf_s(buffer,sizeof(buffer),"%s(%s array - number of elements == %d::)[",preamble,indirect,arrLen);
#endif
				Console::displayString(buffer);
				char newbuff[50];
#ifdef MAC_PLATFORM
				snprintf(newbuff,sizeof(newbuff),"\t%s",preamble);
#else
				sprintf_s(newbuff,sizeof(newbuff),"\t%s",preamble);
#endif
				for (int loopctr=0;loopctr<arrLen;loopctr++){
					Console::CosObjDump(CosArrayGet(thisObj, loopctr),newbuff);
				}
#ifdef MAC_PLATFORM
				snprintf(buffer,sizeof(buffer),"%s]",preamble);
#else
				sprintf_s(buffer,sizeof(buffer),"%s]",preamble);
#endif
				Console::displayString(buffer);
			}
		break;
		case CosStream:
			{
				ASTCount offset = CosStreamPos(thisObj);
				ASTArraySize length = CosStreamLength(thisObj);
#ifdef MAC_PLATFORM
				snprintf(buffer,sizeof(buffer),"%s(%s Stream::)**",preamble,indirect);
#else
				sprintf_s(buffer,sizeof(buffer),"%s(%s Stream::)**",preamble,indirect);
#endif
				Console::displayString(buffer);
				char buf[128];
#ifdef MAC_PLATFORM
				snprintf(buf,sizeof(buf),"Stream offset: %d; Stream length: %d", offset, length);
#else
				sprintf_s(buf,sizeof(buf),"Stream offset: %d; Stream length: %d", offset, length);
#endif
				Console::displayString(buf);
				CosObj attrDict = CosStreamDict(thisObj);
				CosObjDump(attrDict, "");
			}
			break;
		default:
			{
#ifdef MAC_PLATFORM
				snprintf(buffer,sizeof(buffer),"%s(Object type is unknown)",preamble);
#else
				sprintf_s(buffer,sizeof(buffer),"%s(Object type is unknown)",preamble);
#endif
				Console::displayString(buffer);			

			}
	}
	 cosLevelDepth--;
}

/* callback to enable us to dump CosObj information to DebugWindow */
static ACCB1 ASBool ACCB2 myCosObjEnumProc(CosObj obj, CosObj value, void* clientData){
	char * input = static_cast<char *>(clientData);
	if (detailedRequired == false){
		objDumpTerminate = true;
	}
	char buff[100];
#ifdef MAC_PLATFORM
	snprintf(buff,sizeof(buff),"%sTAG: ",input);
	Console::CosObjDump(obj,buff);
	snprintf(buff,sizeof(buff),"  %s",input);
	Console::CosObjDump(value,buff);
#else
	sprintf_s(buff,sizeof(buff),"%sTAG: ",input);
	Console::CosObjDump(obj,buff);
	sprintf_s(buff,sizeof(buff),"  %s",input);
	Console::CosObjDump(value,buff);
#endif
	return true;
}


string 
Console::getString() 
{
	char buff[250];
	fgets(buff, 249, stdin);
	int buffLen = strlen(buff);
	buff[buffLen-1] = '\0';
	string tmp = string(buff);
	return tmp;
}

void 
Console::flush() 
{
	cout.flush();
}

/** Dump in memory string to a file */
ASBool 
Console::saveAsFile(ASPathName pathName, char *data)
{
	ASFile asFile = NULL;
	Int32 nWrite = 0;

	if (pathName == NULL)
		return false;

	if (!ASFileSysOpenFile (NULL, pathName, ASFILE_WRITE | ASFILE_CREATE, (ASFile *) &asFile))
		nWrite = ASFileWrite (asFile, data,strlen (data));
	else{
		displayString ("Error in opening the output file");
		return false;
	}

	if (nWrite == 0)
		return false;

	if (asFile)
		ASFileClose(asFile);
	if (pathName)
		ASFileSysReleasePath (NULL, pathName);

	return true;
}

