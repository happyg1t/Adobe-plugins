
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

 defines the base behaviour of a snippet, defines the macro to
 produce the derived snippet.


*********************************************************************/

#ifndef _SnipRun_
#define _SnipRun_

#pragma warning(disable: 4786)
#define HAS_PDSEDIT_READ_PROCS 1
#define HAS_PDSEDIT_WRITE_PROCS 1

#include <string>

////////////////////// LIBRARY ONLY DEFS
#ifdef PDFL_SDK_SAMPLE
#include "CorCalls.h"
#include "ASCalls.h"
#include "CosCalls.h"
#include "PDCalls.h"
#include "PERCalls.h"
#include "ASExtraCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "PDSExpT.h"
#include "PSFCalls.h"
//#include "NSelExpT.h"
#include "AcroColorCalls.h"
#include "PDMetadataCalls.h"
#include "PDSWriteCalls.h"
#include "PDSReadCalls.h"
#include "PDSysFontExpT.h"
// PDFL only 
#include "MyPDFLibUtils.h"
#include "PDFLCalls.h"
#define AS_ISP asisp_ppc604
#define AS_OS asos_mwcx
//#define ACROBAT_LIBRARY 1

// these provide a compatibility layer between PDFL and Acrobat.
#define gExtensionID 0
#define AVAlertNote(_SOMEVAL) Console::displayString("alert:"+string(_SOMEVAL)+"\n")
#define AVAppRegisterNotification	AVExtensionMgrRegisterNotification
#define AVAppUnregisterNotification		AVExtensionMgrUnregisterNotification

#ifndef FixedToFloat
#define FixedToFloat ASFixedToFloat
#define FloatToFixed FloatToASFixed
#endif

#else 
/////////////////////// PLUGIN ONLY DEFS

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

#endif //else !PDFL

#include "Console.h"

#include "ParamManager.h"


//////////////////////// SHARED DEFS
#ifndef IDOK
#define IDOK 1
#define IDCANCEL 2
#endif



// only need to include stdio for unix:
#ifdef UNIX_PLATFORM
#include <stdio.h>
#endif

#if defined(WIN) || defined(_WIN32)  || defined(WIN_ENV) || defined(WIN_PLATFORM)
#define snprintf _snprintf
#endif

using namespace std;


/** Manifistation of the snippet in memory.
*/
class SnipRun
{
	public:
		SnipRun(const char * snippet, const char * name, const char * description, char * params);

		/** 
			Destructor removes the snippet from the list maintained by \Ref{SnipRunManager}..
		*/
		virtual ~SnipRun();

		/**
			Returns internal name of snippet.
		*/
		virtual const char * GetSnippetName() const {return fSnippetName;};
		
		/**
			Returns UI name of snippet.
		*/
		virtual const char * GetUIName() const {return fName;};
		/**
			Returns description of snippet.
		*/
		virtual const char * GetDescription() const {return fDescription;};
		/**
			Returns  parameters to be used with the snippet.
		*/
		virtual string GetParams() const {return fParams;};

		/**
			sets  parameters to be used with the snippet.
		*/
		virtual void SetParams(string newParams) {fParams=newParams;};

		/**
			Returns  default parameters to be used with the snippet.
		*/
		virtual string GetDefaultParams() const {return fDefaultParams;};

		/**
			Calls the snippet function. One for the command line, one for
			the server. Difference is handling params, with dialog, or without.
			@param the snippet to be executed
			@return kSuccess on success, other ErrorCode otherwise
		*/
		static ACCB1 void ACCB2 SnipCallback(void *clientData);
		static ACCB1 void ACCB2 SnipServerCallback(void *clientData);

		virtual void Run(ParamManager *thePM) =0;
		static SnipRun * thisSnip;
		static SnipRun * getThisSnip() {return thisSnip;};
		
		/** sets the marked state, returning the previous version 
			@return old marked state for the hierarchy list item
		*/
		ASBool toggleCheckedState();

		/** returns whether the element is checked or not
		*/
		ASBool getCheckedState(){return fCheckedState;};

		/** returns whether the parameter dialog will be suppressed
		*/
		ASBool suppressDialog(){return fShowDialogState;};

		/** returns whether the parameter dialog will be suppressed
		*/
		void suppressDialog(ASBool newValue){fShowDialogState=newValue;};

	private:
		const char * fSnippetName;
		const char * fName;
		const char * fDescription;
		string fParams;
		string fDefaultParams;
		ASBool fCheckedState;
		ASBool fShowDialogState;
};


/**
	Register a snippet that has no parameters with the SnippetRunner framework.

	@param _FUNCTION of signature ErrorCode MyFunc() to be called by the framework
	@param _NAME C string name of the snippet for display in the UI (sub menus can be created by providing a colon seperated list)
	@param _DESCRIPTION C string description of the snippet.
*/
#define SNIPRUN_REGISTER(_FUNCTION, _NAME,_DESCRIPTION) \
	class Run##_FUNCTION: public SnipRun \
	{ \
	public: \
		Run##_FUNCTION() : SnipRun ( #_FUNCTION , _NAME,_DESCRIPTION,"") {}; \
		~Run##_FUNCTION() {}; \
		void Run(ParamManager *thePM){_FUNCTION();}; \
	private: \
	}; \
	Run##_FUNCTION gRun##_FUNCTION;


/**
	Register a snippet that has parameters with the SnippetRunner framework. 

	@param _FUNCTION of signature ErrorCode MyFunc() to be called by the framework
	@param _NAME C string name of the snippet for display in the UI (sub menus can be created by providing a colon seperated list)
	@param _DESCRIPTION C string description of the snippet.
	@param _PARAMS C string default params to be used for the snippet. 
*/
#define SNIPRUN_REGISTER_WITH_DIALOG(_FUNCTION, _NAME,_DESCRIPTION,_PARAMS) \
	class Run##_FUNCTION: public SnipRun \
	{ \
	public: \
		Run##_FUNCTION() : SnipRun ( #_FUNCTION , _NAME,_DESCRIPTION,_PARAMS) {}; \
		~Run##_FUNCTION() {}; \
		void Run(ParamManager *thePM){_FUNCTION(thePM);}; \
	private: \
	}; \
	Run##_FUNCTION gRun##_FUNCTION;


#endif

// End, SnippetRunner.h



