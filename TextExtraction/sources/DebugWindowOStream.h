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

 DebugWindowOStream.h

 - Mechanisms to abstract the output streams over the ResizeDebugWindow.

*********************************************************************/
#ifndef _DebugWindowOutStream_h_
#define _DebugWindowOutStream_h_

#include "SnippetRunner.h"
#include <ostream>
	
/** this is a utility class that can be used to create a custom stream for DebugWindowBuf.
	to use <pre> 
		DebugWindowOStream &ctr = *(new DebugWindowOSStream);
		ctr << "any supported type here!" <<endl;
*/
class DebugWindowOStream: public ostream
{
public:
	/** constructor
	*/
	DebugWindowOStream(int = 0);

};


/** Implements the stream buffer required for the DebugWindow (on acrobat)
*/
class DebugWindowBuf: public streambuf
{
public:
	/** ctor. 
		@param bsize IN the size of buffer to allocate
	*/
	DebugWindowBuf(int bsize = 0);
	/** dtor. Note, the destructor does not flush the buffer before it is destroyed.
		Given this class is connected to an ADM window, this plug-in gets unloaded after
		ADM shuts down. This means there is no place for the buffer to go.
	*/
	virtual ~DebugWindowBuf();
private:
	/** outputs the buffer to debug window 
	*/
	void	put_buffer(void);
	/** outputs a single character to the window. This method is used on overflow,
		specifically for the character that has "overflown"
		@param c IN the character that has spilled out.
	*/
	void	put_char(int c);

protected:
	/** called on overflow of the buffer. Dumps the buffer to trace and resets it.
		@param c IN the character that has overflown.
		@return 0
	*/
	int	overflow(int c);
	/** syncs the contents of the buffer with the window, this is a flush operation.
		@return 0
	*/
	int	sync();
	
};

#endif //DebugWindowOutStream