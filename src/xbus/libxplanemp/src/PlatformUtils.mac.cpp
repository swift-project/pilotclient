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

//#define _STDINT_H_
//#include <Endian.h>
//#include <Dialogs.h>
//#include <Navigation.h>
//#include <string.h>
//#include <Appearance.h>

#include <Carbon/Carbon.h>



static	OSErr		FSSpecToPathName(const FSSpec * inFileSpec, char * outPathname);

/* Endian routines for the Mac use Apple's Endian macros. */

void	EndianFlipShort(short * ioShort)
{
	*ioShort = Endian16_Swap(*ioShort);
}

void	EndianFlipLong(long * ioLong)
{
	*ioLong = Endian32_Swap(*ioLong);
}

/* Get FilePathFromUser puts up a nav services dialog box and converts the results
   to a C string path. */

pascal void event_proc(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, void *callBackUD)
{
}

template <typename T>
struct CFSmartPtr {
	 CFSmartPtr(T p) : p_(p) {						  }
	~CFSmartPtr()			 { if (p_) CFRelease(p_); }
	operator T ()			 { return p_; }
	T p_;
};

int HFS2PosixPath(const char *path, char *result, int resultLen)
{
	bool is_dir = (path[strlen(path)-1] == ':');

	CFSmartPtr<CFStringRef>		inStr(CFStringCreateWithCString(kCFAllocatorDefault, path ,kCFStringEncodingMacRoman));
	if (inStr == NULL) return -1;
	
	CFSmartPtr<CFURLRef>		url(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, inStr, kCFURLHFSPathStyle,0));
	if (url == NULL) return -1;
	
	CFSmartPtr<CFStringRef>		outStr(CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle));
	if (outStr == NULL) return -1;
	
	if (!CFStringGetCString(outStr, result, resultLen, kCFStringEncodingMacRoman))
		return -1;
		
	if(is_dir) strcat(result, "/");

	return 0;
}

int Posix2HFSPath(const char *path, char *result, int resultLen)
{
	CFSmartPtr<CFStringRef>		inStr(CFStringCreateWithCString(kCFAllocatorDefault, path ,kCFStringEncodingMacRoman));
	if (inStr == NULL) return -1;
	
	CFSmartPtr<CFURLRef>		url(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, inStr, kCFURLPOSIXPathStyle,0));
	if (url == NULL) return -1;
	
	CFSmartPtr<CFStringRef>		outStr(CFURLCopyFileSystemPath(url, kCFURLHFSPathStyle));
	if (outStr == NULL) return -1;
	
	if (!CFStringGetCString(outStr, result, resultLen, kCFStringEncodingMacRoman))
		return -1;

	return 0;
}

void MakePartialPathNative(char * ioBegin, char * ioEnd)
{
	for (char * p = ioBegin; p != ioEnd; ++p)
	{
		if (*p == '/' || *p == ':' || *p == '\\')
			*p = DIR_CHAR;
	}
}



const char * GetApplicationPath(void)
{
	static	char		pathBuf[1024];
	ProcessInfoRec		pir;
	FSSpec				spec;
	Str255				pStr;
	ProcessSerialNumber	psn = { 0, kCurrentProcess };
	pir.processInfoLength 	= sizeof(pir);
	pir.processAppSpec 		= &spec;
	pir.processName			= pStr;
	GetProcessInformation(&psn, &pir);
	OSErr err = FSSpecToPathName(&spec, pathBuf);
	if (err != noErr)
		return NULL;
	return pathBuf;
}

int		GetFilePathFromUser(
					int					inType,
					const char * 		inPrompt, 
					const char *		inAction,
					int					inID,
					char * 				outFileName)
{
		OSErr				err;
		NavReplyRecord		reply;
		NavDialogOptions	options;
		FSSpec				fileSpec;
		
	reply.version = kNavReplyRecordVersion;
	err = NavGetDefaultDialogOptions(&options);
	if (err != noErr)
		return 0;
		
	if (inType == getFile_Save)
	{
		options.savedFileName[0] = strlen(outFileName);
		memcpy(options.savedFileName+1, outFileName, options.savedFileName[0]);
	}

	options.message[0] = strlen(inPrompt);
	memcpy(options.message+1,inPrompt, options.message[0]);
	options.actionButtonLabel[0] = strlen(inAction);
	memcpy(options.actionButtonLabel+1,inAction, options.actionButtonLabel[0]);
	options.dialogOptionFlags &= ~kNavAllowMultipleFiles;
	options.preferenceKey = inID;
	NavEventUPP eventUPP = NewNavEventUPP(event_proc);
	
	switch(inType) {
	case getFile_Open:
		err = NavGetFile(NULL, &reply, &options, eventUPP, NULL, NULL, NULL, NULL);
		if ((err != noErr) && (err != userCanceledErr))
			return 0;
		break;
	case getFile_Save:
		err = NavPutFile(NULL, &reply, &options, eventUPP, 0, 0, NULL);
		if ((err != noErr) && (err != userCanceledErr))
			return 0;
		break;
	case getFile_PickFolder:
		err = NavChooseFolder(NULL, &reply, &options, eventUPP, NULL, NULL);
		if ((err != noErr) && (err != userCanceledErr))
			return 0;
		break;
	}	
	DisposeNavEventUPP(eventUPP);			
	if (!reply.validRecord)
		goto bail;

	/* Convert the result from an AEDesc to a Mac file spec. */
	err = AEGetNthPtr(&reply.selection, 1, typeFSS, NULL, NULL, &fileSpec, sizeof(fileSpec), NULL);
	if (err != noErr)
		goto bail;

	/* Then convert the FSSpec to a full path. */
	err = FSSpecToPathName(&fileSpec, outFileName);
	if (err != noErr)
		goto bail;

	NavDisposeReply(&reply);
	return 1;

bail:
	NavDisposeReply(&reply);
	return 0;


}

void	DoUserAlert(const char * inMsg)
{
	Str255	p1,p2;
	size_t	sl;
	
	sl = strlen(inMsg);
	if (sl > 255) 
		sl = 255;
		
	p1[0] = sl;
	memcpy(p1+1, inMsg, sl);
	
	p2[0]=0;	// Ben says: GCC doesn't understand "\p".
	
	StandardAlert(kAlertStopAlert, p1, p2, NULL, NULL);
}

void	ShowProgressMessage(const char * inMsg, float * progress)
{
	static WindowRef	wind = NULL;
	Rect		windBounds = { 0, 0, 250, 500 };
	if (wind == NULL)
	{
		if (CreateNewWindow(kMovableAlertWindowClass, kWindowStandardHandlerAttribute, &windBounds, &wind) != noErr) return;
		if (wind == NULL) return;
		RepositionWindow(wind, NULL,kWindowCenterOnMainScreen);
		ShowWindow(wind);
	}

	SetPortWindowPort(wind);
	CFStringRef ref = CFStringCreateWithCString(NULL, inMsg, smSystemScript);
	EraseRect(&windBounds);
	InsetRect(&windBounds, 20, 15);
	DrawThemeTextBox(ref, kThemeSystemFont, kThemeStateActive, true, &windBounds, teJustLeft, NULL);		
	CFRelease(ref);
	
	if (progress)
	{
		float p = *progress;
		ThemeTrackDrawInfo	info;
		info.kind = (p >= 0.0) ? kThemeMediumProgressBar : kThemeMediumIndeterminateBar;
		SetRect(&info.bounds, 20, 210, 480, 230);
		info.min = 0;
		info.max = (p >= 0.0) ? 1000.0 : 0.0;
		info.value = (p >= 0.0) ? (p * 1000.0) : 0;
		info.reserved = 0;
		info.attributes = kThemeTrackHorizontal;
		info.enableState = kThemeTrackActive;
		info.filler1 = 0;
		static UInt8 nPhase = 0;
		info.trackInfo.progress.phase = nPhase;
		nPhase++;
		DrawThemeTrack(&info, NULL, NULL, 0);
	}
	QDFlushPortBuffer(GetWindowPort(wind), NULL);
}

int		ConfirmMessage(const char * inMsg, const char * proceedBtn, const char * cancelBtn)
{
	Str255					pStr, proStr, clcStr, p2;
	AlertStdAlertParamRec	params;
	short					itemHit;
	
	pStr[0] = strlen(inMsg);
	memcpy(pStr+1,inMsg,pStr[0]);
	proStr[0] = strlen(proceedBtn);
	memcpy(proStr+1, proceedBtn, proStr[0]);
	clcStr[0] = strlen(cancelBtn);
	memcpy(clcStr+1, cancelBtn, clcStr[0]);

	params.movable = false;
	params.helpButton = false;
	params.filterProc = NULL;
	params.defaultText = proStr;
	params.cancelText = clcStr;
	params.otherText = NULL;
	params.defaultButton = 1;
	params.cancelButton = 2;
	params.position = kWindowDefaultPosition;

	p2[0]=0;

	StandardAlert(kAlertCautionAlert, pStr, p2, &params, &itemHit);

	return (itemHit == 1);		
}

/*
 * FSSpecToPathName
 *
 * This routine builds a full path from a file spec by recursing up the directory
 * tree to the route, prepending each directory name.
 *
 */

OSErr	FSSpecToPathName(const FSSpec * inFileSpec, char * outPathname)
{
	short			vRefNum = inFileSpec->vRefNum;
	long			startDirID = inFileSpec->parID;

	CInfoPBRec		paramRec;
	Str255			dirName;	/* This will contain the name of the directory we get info about. */
	OSErr 			err = noErr;
	
	paramRec.dirInfo.ioCompletion = nil;
	paramRec.dirInfo.ioNamePtr = (StringPtr)(&dirName);
	paramRec.dirInfo.ioDrParID = startDirID;

	/* Start by putting a directory separator and the file name on the path. */
	outPathname[0] = ':';
	memcpy(outPathname+1, inFileSpec->name+1, inFileSpec->name[0]);
	outPathname[inFileSpec->name[0]+1] = 0;

	do {
		paramRec.dirInfo.ioVRefNum = vRefNum;
		paramRec.dirInfo.ioFDirIndex = -1;
		paramRec.dirInfo.ioDrDirID = paramRec.dirInfo.ioDrParID;
		
		if (!(err = PBGetCatInfoSync(&paramRec))) 
		{
			/* For each directory we get info about, prepend a : and the directory name.
			   But for the root directory, do NOT prepend the colon. */
			short	newPart = dirName[0] + ((paramRec.dirInfo.ioDrDirID != fsRtDirID) ? 1 : 0);
			memmove(outPathname+newPart, outPathname, strlen(outPathname)+1);
			if (paramRec.dirInfo.ioDrDirID != fsRtDirID)
			{
				outPathname[0] = ':';
				memcpy(outPathname+1, dirName+1, dirName[0]);
			} else 
				memcpy(outPathname, dirName+1, dirName[0]);
		}
	} while ((err == noErr) && (paramRec.dirInfo.ioDrDirID != fsRtDirID));
	return err;
}

