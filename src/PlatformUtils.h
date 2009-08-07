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
#ifndef _PlatformUtils_h_
#define _PlatformUtils_h_

/*
 * PlatformUtils
 *
 * This file declares all of the platform specific code for the converter.
 *
 * PlatformUtils.mac.c contains the mac implementation; this code must be rewritten
 * for the PC.
 *
 */
 
/* The directory separator is a macro and should be cased by some kind of compiler
   #define or something. */

#if	IBM
	#define DIR_CHAR	'\\'
	#define DIR_STR		"\\"
#elif (APL && __MACH__) || LIN
		#define	DIR_CHAR	'/'
		#define DIR_STR		"/"
#elif APL
		#define	DIR_CHAR	':'
		#define DIR_STR		":"
#else
	#error PLATFORM NOT DEFINED
#endif	

/*
 * These routines convert between little endian and native endian.  This means they
 * swap byte order on the Mac and do nothing on the PC.  These are good for reading
 * PC File structures, but EndianUtils.h contains more powerful stuff.
 *
 */
 
void	EndianFlipShort(short * ioShort);
void	EndianFlipLong(long * ioLong);

/*
 * This routine returns a fully qualified path to the application.
 *
 */
const char * GetApplicationPath(void);

#if APL
/*
 * Convert an HFS path to a POSIX Path.  Returns 0 for success, -1 for failure.
 * WARNING: this only works for EXISTING FILES!!!!!!!!!!!!!!!!!
 *
 */
int HFS2PosixPath(const char *path, char *result, int resultLen);
int Posix2HFSPath(const char *path, char *result, int resultLen);
#endif

/*
 * Takes a path and replaces the dir chars with the 'native' ones.
 * Note that this is intended for PARTIAL paths.
 *
 */
void MakePartialPathNative(char * ioBegin, char * ioEnd);	// Takes a char range


/*
 * GetFilePathFromUser takes a prompting C-string and fills in the buffer with a path
 * to a picked file.  It returns 1 if a file was picked, 0 if the user canceled.
 *
 */
enum {
	getFile_Open,
	getFile_Save,
	getFile_PickFolder
}; 
int		GetFilePathFromUser(
					int					inType,
					const char * 		inPrompt, 
					const char *		inAction,
					int					inID,
					char * 				outFileName);

/*
 * DoUserAlert puts up an alert dialog box with the message and an OK button.
 *
 */	
void	DoUserAlert(const char * inMsg);

/*
 * ShowProgressMessage puts up a dialog box with a progress message.  Calling it
 * repeatedly changes the progress message.
 *
 */
void	ShowProgressMessage(const char * inMsg, float * progress);

/*
 * ConfirmMessage puts up a dialog box with a message and two buttons.  The proceed
 * button is the default one.  Pass in the message and the text of the two buttons.
 * Returns 1 if the user clicks the proceed button, 0 if the user cancels.
 * 
 */
int		ConfirmMessage(const char * inMsg, const char * proceedBtn, const char * cancelBtn);

#endif
