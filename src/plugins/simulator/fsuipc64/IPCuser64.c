/* IPCUSER64.C	User interface library for FSUIPC
*******************************************************************************

Started:          28th November 2000

With acknowledgements to Adam Szofran (author of original FS6IPC).

******************************************************************************/

#define LIB_VERSION 2002 // 2.002
#define MAX_SIZE 0x7F00 // Largest data (kept below 32k to avoid
								// any possible 16-bit sign problems)

#include "IPCuser64.h"
#include "FSUIPC_User64.h"

#define FS6IPC_MSGNAME1      "FsasmLib:IPC" 

/******************************************************************************
			IPC client stuff
******************************************************************************/

DWORD FSUIPC_Version = 0;
DWORD FSUIPC_FS_Version = 0;
DWORD FSUIPC_Lib_Version = LIB_VERSION;

static HWND    m_hWnd = 0;       // FS6 window handle
static UINT    m_msg = 0;        // id of registered window message
static ATOM    m_atom = 0;       // global atom containing name of file-mapping object
static HANDLE  m_hMap = 0;       // handle of file-mapping object
static BYTE*   m_pView = 0;      // pointer to view of file-mapping object
static BYTE*   m_pNext = 0;

/******************************************************************************
			FSUIPC_Close
******************************************************************************/

// Stop the client
void FSUIPC_Close(void)
{	m_hWnd = 0;
	m_msg = 0;
  
	if (m_atom)
	{	GlobalDeleteAtom(m_atom);
		m_atom = 0;
	}

	if (m_pView)
	{	UnmapViewOfFile((LPVOID)m_pView);
		m_pView = 0;
	}

	if (m_hMap)
	{	CloseHandle(m_hMap);
		m_hMap = 0;
	}
}

/******************************************************************************
			FSUIPC_Open
******************************************************************************/

// Start the client
// return: TRUE if successful, FALSE otherwise
BOOL FSUIPC_Open(DWORD dwFSReq, DWORD *pdwResult)
{  char szName[MAX_PATH];
	static int nTry = 0;
	BOOL fWideFS = FALSE;
	int i = 0;
	
	// abort if already started
	if (m_pView)
	{	*pdwResult = FSUIPC_ERR_OPEN;
		return FALSE;
	}

	// Clear version information, so know when connected
	FSUIPC_Version = FSUIPC_FS_Version = 0;
	
	m_hWnd = FindWindowEx(NULL, NULL, "UIPCMAIN", NULL);
	if (!m_hWnd)
	{	// If there's no UIPCMAIN, we may be using WideClient
		// which only simulates FS98
		m_hWnd = FindWindowEx(NULL, NULL, "FS98MAIN", NULL);
		fWideFS = TRUE;
		if (!m_hWnd)
		{	*pdwResult = FSUIPC_ERR_NOFS;
			return FALSE;
		}
	}
	
	if (!m_hWnd)
	{	*pdwResult = FSUIPC_ERR_NOFS;
		return FALSE;
	}
	
	// register the window message
	m_msg = RegisterWindowMessage(FS6IPC_MSGNAME1);
	if (m_msg == 0)
	{	*pdwResult = FSUIPC_ERR_REGMSG;
		return FALSE;
	}

	// create the name of our file-mapping object
	nTry++; // Ensures a unique string is used in case user closes and reopens
	wsprintf(szName, FS6IPC_MSGNAME1 ":%X:%X", GetCurrentProcessId(), nTry);

	// stuff the name into a global atom
	m_atom = GlobalAddAtom(szName);
	if (m_atom == 0)
   {	*pdwResult = FSUIPC_ERR_ATOM;
		FSUIPC_Close();
		return FALSE;
	}

	// create the file-mapping object
	m_hMap = CreateFileMapping(
					INVALID_HANDLE_VALUE, // use system paging file
					NULL,               // security
					PAGE_READWRITE,     // protection
					0, MAX_SIZE+256,       // size
					szName);            // name 

	if ((m_hMap == 0) || (GetLastError() == ERROR_ALREADY_EXISTS))
	{	*pdwResult = FSUIPC_ERR_MAP;
		FSUIPC_Close();
		return FALSE;    
	}

	// get a view of the file-mapping object
	m_pView = (BYTE*)MapViewOfFile(m_hMap, FILE_MAP_WRITE, 0, 0, 0);
	if (m_pView == NULL)
	{	*pdwResult = FSUIPC_ERR_VIEW;
		FSUIPC_Close();
		return FALSE;
	}

	// Okay, now determine FSUIPC version AND FS type
	m_pNext = m_pView;

	// Try up to 5 times with a 100mSec rest between each
	while ((i++ < 5) && ((FSUIPC_Version == 0) || (FSUIPC_FS_Version == 0)))
	{	// Read FSUIPC version
		if (!FSUIPC_Read(0x3304, 4, &FSUIPC_Version, pdwResult))
		{	FSUIPC_Close();
			return FALSE;
		}

		// and FS version and validity check pattern
		if (!FSUIPC_Read(0x3308, 4, &FSUIPC_FS_Version, pdwResult))
		{	FSUIPC_Close();
			return FALSE;
		}

		// Write our Library version number to a special read-only offset
		// This is to assist diagnosis from FSUIPC logging
		// But only do this on first try
		if ((i < 2) && !FSUIPC_Write(0x330a, 2, &FSUIPC_Lib_Version, pdwResult))
		{	FSUIPC_Close();
			return FALSE;
		}

		// Actually send the requests and get the responses ("process")
		if (!FSUIPC_Process(pdwResult))
		{	FSUIPC_Close();
			return FALSE;
		}

		// Maybe running on WideClient, and need another try
		Sleep(100); // Give it a chance
	}

	// Only allow running on FSUIPC 1.998e or later
	// with correct check pattern 0xFADE
	if ((FSUIPC_Version < 0x19980005) || ((FSUIPC_FS_Version & 0xFFFF0000L) != 0xFADE0000))
	{	*pdwResult = fWideFS ? FSUIPC_ERR_RUNNING : FSUIPC_ERR_VERSION;
		FSUIPC_Close();
		return FALSE;
	}

	FSUIPC_FS_Version &= 0xffff; // Isolates the FS version number
	if (dwFSReq && (dwFSReq != FSUIPC_FS_Version)) // Optional user specific FS request
	{	*pdwResult = FSUIPC_ERR_WRONGFS;
		FSUIPC_Close();
		return FALSE;
	}

	*pdwResult = FSUIPC_ERR_OK;
	return TRUE;
}

/******************************************************************************
			FSUIPC_Process
******************************************************************************/

BOOL FSUIPC_Process(DWORD *pdwResult)
{	DWORD_PTR dwError;
	DWORD *pdw;
	F64IPC_READSTATEDATA_HDR *pHdrR;
	FS6IPC_WRITESTATEDATA_HDR *pHdrW;
	int i = 0;
	
	if (!m_pView)
	{	*pdwResult = FSUIPC_ERR_NOTOPEN;
		return FALSE;
	}

	if (m_pView == m_pNext)
	{	*pdwResult = FSUIPC_ERR_NODATA;
		return FALSE;
	}

	ZeroMemory(m_pNext, 4); // Terminator
	m_pNext = m_pView;
	
	// send the request (allow up to 9 tries)
	while ((++i < 10) && !SendMessageTimeout(
			m_hWnd,       // FS6 window handle
			m_msg,        // our registered message id
			m_atom,       // wParam: name of file-mapping object
			0,            // lParam: offset of request into file-mapping obj
			SMTO_BLOCK,   // halt this thread until we get a response
			2000,			  // time out interval
			&dwError))    // return value
	{	Sleep(100); // Allow for things to happen
	}

	if (i >= 10) // Failed all tries?
	{	*pdwResult = GetLastError() == 0 ? FSUIPC_ERR_TIMEOUT : FSUIPC_ERR_SENDMSG;
		return FALSE;
	}

	if (dwError != FS6IPC_MESSAGE_SUCCESS)
	{	*pdwResult = FSUIPC_ERR_DATA; // FSUIPC didn't like something in the data!
		return FALSE;
	}

	// Decode and store results of Read requests
	pdw = (DWORD *) m_pView;

	while (*pdw)
	{	switch (*pdw)
		{	case F64IPC_READSTATEDATA_ID:
				pHdrR = (F64IPC_READSTATEDATA_HDR *) pdw;
				m_pNext += sizeof(F64IPC_READSTATEDATA_HDR);
				if (pHdrR->pDest && pHdrR->nBytes)
					CopyMemory(pHdrR->pDest, m_pNext, pHdrR->nBytes);
				m_pNext += pHdrR->nBytes;
				break;

			case FS6IPC_WRITESTATEDATA_ID:
				// This is a write, so there's no returned data to store
				pHdrW = (FS6IPC_WRITESTATEDATA_HDR *) pdw;
				m_pNext += sizeof(FS6IPC_WRITESTATEDATA_HDR) + pHdrW->nBytes;
				break;

			default:
				// Error! So terminate the scan
				*pdw = 0;
				break;
		}

		pdw = (DWORD *) m_pNext;
	}

	m_pNext = m_pView;
	*pdwResult = FSUIPC_ERR_OK;
	return TRUE;
}

/******************************************************************************
			FSUIPC_ReadCommon
******************************************************************************/

BOOL FSUIPC_ReadCommon(BOOL fSpecial, DWORD dwOffset, DWORD dwSize, void *pDest, DWORD *pResult)
{	F64IPC_READSTATEDATA_HDR *pHdr = (F64IPC_READSTATEDATA_HDR *) m_pNext;

	if (!m_pView)
	{	*pResult = FSUIPC_ERR_NOTOPEN;
		return FALSE;
	}

	if (((m_pNext - m_pView) + (dwSize + sizeof(F64IPC_READSTATEDATA_HDR))) > MAX_SIZE)
	{	*pResult = FSUIPC_ERR_SIZE;
		return FALSE;
	}

	pHdr->dwId = F64IPC_READSTATEDATA_ID;
	pHdr->dwOffset = dwOffset;
	pHdr->nBytes = dwSize;
	pHdr->pDest = (BYTE *) pDest;

	// Initialise the reception area, so rubbish won't be returned
	if (dwSize)
	{	if (fSpecial) CopyMemory(&m_pNext[sizeof(F64IPC_READSTATEDATA_HDR)], pDest, dwSize);
		else ZeroMemory(&m_pNext[sizeof(F64IPC_READSTATEDATA_HDR)], dwSize);
	}

	m_pNext += sizeof(F64IPC_READSTATEDATA_HDR) + dwSize;

	*pResult = FSUIPC_ERR_OK;
	return TRUE;
}

/******************************************************************************
			FSUIPC_Read
******************************************************************************/

BOOL FSUIPC_Read(DWORD dwOffset, DWORD dwSize, void *pDest, DWORD *pResult)
{	return FSUIPC_ReadCommon(FALSE, dwOffset, dwSize, pDest, pResult);
}

/******************************************************************************
			FSUIPC_ReadSpecial
******************************************************************************/

BOOL FSUIPC_ReadSpecial(DWORD dwOffset, DWORD dwSize, void *pDest, DWORD *pResult)
{	return FSUIPC_ReadCommon(TRUE, dwOffset, dwSize, pDest, pResult);
}

/******************************************************************************
			FSUIPC_Write
******************************************************************************/

BOOL FSUIPC_Write(DWORD dwOffset, DWORD dwSize, void *pSrce, DWORD *pdwResult)
{	FS6IPC_WRITESTATEDATA_HDR *pHdr = (FS6IPC_WRITESTATEDATA_HDR *) m_pNext;

	// check link is open
	if (!m_pView)
	{	*pdwResult = FSUIPC_ERR_NOTOPEN;
		return FALSE;
	}
	
	// Check have spce for this request (including terminator)
	if (((m_pNext - m_pView) + 4 + (dwSize + sizeof(FS6IPC_WRITESTATEDATA_HDR))) > MAX_SIZE)
	{	*pdwResult = FSUIPC_ERR_SIZE;
		return FALSE;
	}

	// Initialise header for write request
	pHdr->dwId = FS6IPC_WRITESTATEDATA_ID;
	pHdr->dwOffset = dwOffset;
	pHdr->nBytes = dwSize;

	// Copy in the data to be written
	if (dwSize) CopyMemory(&m_pNext[sizeof(FS6IPC_WRITESTATEDATA_HDR)], pSrce, dwSize);

	// Update the pointer ready for more data
	m_pNext += sizeof(FS6IPC_WRITESTATEDATA_HDR) + dwSize;

	*pdwResult = FSUIPC_ERR_OK;
	return TRUE;
}

/******************************************************************************
 End of IPCuser64 module
******************************************************************************/
