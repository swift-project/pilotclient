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
#include <Windows.h>
#include <Commdlg.h>
#include "PlatformUtils.h"
#include <shlobj.h>
#include <stdio.h>


void	EndianFlipShort(short * ioShort)

{
	// Not necessary on WINTEL machines.
}

void	EndianFlipLong(long * ioLong)

{
	// Not necessary on WINTEL machines.
}

const char * GetApplicationPath(void)

{
	static	char	pathBuf[1024];
	if (GetModuleFileName(NULL, pathBuf, sizeof(pathBuf)))
		return pathBuf;
	else
		return NULL;
}

int		GetFilePathFromUser(
					int					inType,
					const char * 		inPrompt, 
					const char *		inAction,
					int					inID,
					char * 				outFileName)
{	
		BROWSEINFO	bif = { 0 };
		OPENFILENAME	ofn = { 0 };

	BOOL result;

	switch(inType) {
	case getFile_Open:
	case getFile_Save:	
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFilter = "All Files\000*.*\000";
		ofn.nFilterIndex = 1;	// Start with .acf files
		ofn.lpstrFile = outFileName;
		if (inType != getFile_Save)
			outFileName[0] = 0;		// No initialization for open.
		ofn.nMaxFile = 512;		// Guess string length?
		ofn.lpstrFileTitle = NULL;	// Don't want file name w/out path
		ofn.lpstrTitle = inPrompt;
		result = (inType == getFile_Open) ? GetOpenFileName(&ofn) : GetSaveFileName(&ofn);
		return (result) ? 1 : 0;

	case getFile_PickFolder:
		bif.hwndOwner = NULL;
		bif.pidlRoot = NULL;
		bif.pszDisplayName = NULL;
		bif.lpszTitle = inPrompt;
		bif.ulFlags = 0;
		bif.lpfn = NULL;
		bif.lParam = NULL;
		LPITEMIDLIST items = SHBrowseForFolder(&bif);
		if (items == NULL) return 0;
		result = 0;
        if (SHGetPathFromIDList (items, outFileName))
        {
        	result = 1;
			strcat(outFileName, "\\");
		}
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free ( items );
            imalloc->Release ( );
        }
        return result ? 1 : 0;
	}

	return 0;
}


void	DoUserAlert(const char * inMsg)
{
	MessageBox(NULL, inMsg, "Alert", MB_OK + MB_ICONWARNING);
}


static TCHAR sWindowClass[] = "__XPlaneInstallerWindowClass";


static char	progBuf[2048] = { 0 };


LRESULT  CALLBACK PaintProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg) {
	case WM_PAINT:
		{
			RECT	r;	
		  	GetClientRect(wnd, &r); 		
			PAINTSTRUCT	paint;
			HDC dc = BeginPaint(wnd, &paint);
			r.left += 20;
			r.right -= 20;
			r.top += 15;
			r.bottom -= 40;
			DrawText(dc, progBuf, strlen(progBuf), &r, DT_LEFT | DT_WORDBREAK);				
			EndPaint(wnd, &paint);
			return 0;
		}	
		break;
	default:
		return 	DefWindowProc(wnd, msg, wparam, lparam);
	}
}



void	ShowProgressMessage(const char * inMsg, float * inProgress)
{
	strcpy(progBuf, inMsg);

	
	static HWND wind = NULL;
	static HWND prog = NULL;
	if (wind == NULL)
	{

		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX); 


		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= PaintProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= GetModuleHandle(NULL);
		wcex.hIcon			= NULL; // LoadIcon(hInstance, (LPCTSTR)IDI_TESTDND);
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= sWindowClass;
		wcex.hIconSm		= NULL; // LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);


		RegisterClassEx(&wcex);

	
		wind = CreateWindowEx(
			WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME,
			sWindowClass,
			"X-Plane Installer",
			WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_CLIPCHILDREN ,
			50, 100, 500, 250,
			NULL,
			NULL,
			GetModuleHandle(NULL),
			NULL);


		InitCommonControls(); 
		RECT	rcClient;
		ShowWindow(wind, SW_SHOWNORMAL);			
	  	GetClientRect(wind, &rcClient); 
		int cyVScroll = GetSystemMetrics(SM_CYVSCROLL); 
		prog = CreateWindowEx(0, PROGRESS_CLASS,
			(LPSTR) NULL, WS_CHILD | WS_VISIBLE,
			rcClient.left + 20, rcClient.bottom - 20 - 
			cyVScroll, rcClient.right - 40, cyVScroll, 
			wind, NULL, GetModuleHandle(NULL), NULL); 
		SendMessage(prog, PBM_SETRANGE, 0,
			MAKELPARAM(0, 1000)); 
		SendMessage(prog, PBM_SETPOS, (WPARAM) 0, 0); 
	}


	RECT	br;
	br.left = 0;
	br.right = 500;
	br.bottom = 210;
	br.top = 0;
	InvalidateRect(wind, &br, TRUE);


	if (inProgress != NULL)
	{
		float v = *inProgress;
		ShowWindow(prog,SW_SHOWNORMAL);
		if (v >= 0.0)
		{
			int n = v * 1000.0;
//			SendMessage(prog, PBM_SETMARQUEE, (WPARAM) 0, 0); 		
			SendMessage(prog, PBM_SETPOS, (WPARAM) n, 0); 		
		} else {
//			SendMessage(prog, PBM_SETMARQUEE, (WPARAM) 1, 0); 		
			SendMessage(prog, PBM_SETPOS, (WPARAM) 0, 0); 		
		}
	} else
		ShowWindow(prog,SW_HIDE);


	MSG	msg;
	if (PeekMessage(&msg, wind, 0, 0, PM_REMOVE))
	{
		TranslateMessage( &msg ); 
		DispatchMessage( &msg ); 
	}
}

 
int		ConfirmMessage(const char * inMsg, const char * proceedBtn, const char * cancelBtn)
{
	int result = MessageBox(
						NULL,				// No Parent HWND
						inMsg,
						"X-Plane 8",			// Dialog caption
//						MB_OKCANCEL +
						MB_YESNO +
//						MB_ICONWARNING + 
						MB_USERICON + 
						MB_DEFBUTTON1);
				

	return (result == IDOK || result == IDYES) ? 1 : 0;
}

void MakePartialPathNative(char * ioBegin, char * ioEnd)
{
	for (char * p = ioBegin; p != ioEnd; ++p)
	{
		if (*p == '/' || *p == ':' || *p == '\\')
			*p = DIR_CHAR;
	}
}

