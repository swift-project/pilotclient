/* 
 * Copyright (c) 2004, Laminar Research.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 *
 */
#include "PlatformUtils.h"
#include <stdio.h>


void	EndianFlipShort(short * ioShort)

{
	// Not necessary on WINTEL machines.
}

void	EndianFlipLong(long * ioLong)

{
	// Not necessary on WINTEL machines.
}


int		GetFilePathFromUser(
					int					inType,
					const char * 		inPrompt, 
					const char *		inAction,
					int					inID,
					char * 				outFileName)
{	
}


void	DoUserAlert(const char * inMsg)
{
}


void	ShowProgressMessage(const char * inMsg, float * inProgress)
{

}

 
int		ConfirmMessage(const char * inMsg, const char * proceedBtn, const char * cancelBtn)
{
}

void MakePartialPathNative(char * ioBegin, char * ioEnd)
{
	for (char * p = ioBegin; p != ioEnd; ++p)
	{
		if (*p == '/' || *p == ':' || *p == '\\')
			*p = DIR_CHAR;
	}
}

// getting the application path on Linux systems is impossible.
// Thus, we return a link to a directory in the users home
// (this is better anyway - this way, the password won't be stored 
// on a common system)
const char * GetApplicationPath(void)
{
        return "./dummy";
}
