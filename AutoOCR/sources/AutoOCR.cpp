/*********************************************************************

 ADOBE SYSTEMS INCORPORATED
 Copyright (C) 1998-2006 Adobe Systems Incorporated
 All rights reserved.

 NOTICE: Adobe permits you to use, modify, and distribute this file
 in accordance with the terms of the Adobe license agreement
 accompanying it. If you have received this file from a source other
 than Adobe, then your use, modification, or distribution of it
 requires the prior written permission of Adobe.

 -------------------------------------------------------------------*/
/** 
\file AutoOCR.cpp

  - This file implements the functionality of the AutoOCR.
*********************************************************************/


// Acrobat Headers.
#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

#include "ParamManager.h" 
/*-------------------------------------------------------
	Constants/Declarations
-------------------------------------------------------*/
// This plug-in's name, you should specify your own unique name here.

const char* MyPluginExtensionName = "ADBE:AutoOCE";

/* A convenient function to add a menu item for your plugin.
*/
ACCB1 ASBool ACCB2 PluginMenuItem(char* MyMenuItemTitle, char* MyMenuItemName);

/*-------------------------------------------------------
	Functions
r-------------------------------------------------------*/

/* MyPluginSetmenu
** ------------------------------------------------------
**
** Function to set up menu for the plugin.
** It calls a convenient function PluginMenuItem.
** Return true if successful, false if failed.
*/
ACCB1 ASBool ACCB2 MyPluginSetmenu()
{
	// Add a new menu item under Acrobat SDK submenu.
	// The new menu item name is "ADBE:AutoOCR", title is "AutoOCR".
	// Of course, you can change it to your own.
	return PluginMenuItem("AutoOCR", "ADBE:AutoOCR"); 
}
	

/**		
		  MyPluginCommand is the function to be called when executing a menu.
	This is the entry point for user's code, just add your code inside.

	@see ASExtensionGetRegisteredName
	@see AVAppGetActiveDoc
	@see PDDocGetNumPages
*/ 

ACCB1 void ACCB2 MyPluginCommand(void *clientData)
{
	ASAtom cmdName;
	AVCommand cmd;
	 AVCommandStatus  cmdStatus =  kAVCommandInError; 
	cmdName=ASAtomFromString("PaperCapture");

	cmd=AVCommandNew(cmdName);
	ASCab config = ASCabNew();
	ASCabPutInt (config, "UIPolicy", kAVCommandUISilent);       //hide the interface
	// Set command configuration 
	cmdStatus = AVCommandSetConfig(cmd, config);

	// we set up the parameters here
	ASCab params = ASCabNew();
	//ASCabPutInt (config, "Downsample", -1);       //clearscan
	ASCabPutInt (params, "Format", 2);       //clearscan
	//ASCabPutInt (config, "Language", 19);       //clearscan

	cmdStatus = AVCommandSetParams  (cmd, params); 
	
	if (params != NULL){
		ASCabDestroy(params);
	}
	if (config!=NULL){
		ASCabDestroy(config);
	}
		
	// we have set up the command, make sure there isn't a latent error
	// We're in silent mode, so we won't throw an alert dialog.
	if (kAVCommandInError == cmdStatus) {
		AVCommandDestroy(cmd);
		return;
	}

	// we should be good to go at this point, execute the command.
	cmdStatus = AVCommandExecute(cmd);

	// Release the resources associated with the command.
	AVCommandDestroy(cmd);
	

	return;
}

/* MyPluginIsEnabled
** ------------------------------------------------------
** Function to control if a menu item should be enabled.
** Return true to enable it, false not to enable it.
*/
ACCB1 ASBool ACCB2 MyPluginIsEnabled(void *clientData)
{
	// always enabled.
	return true;
	
	// this code make it is enabled only if there is a open PDF document. 
	/* return (AVAppGetActiveDoc() != NULL); */
}

