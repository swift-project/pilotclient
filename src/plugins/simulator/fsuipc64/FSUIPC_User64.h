#ifndef _FSUIPC_H_
#define _FSUIPC_H_

// Supported Sims
#define SIM_ANY	0
#define SIM_FS98	1
#define SIM_FS2K	2
#define SIM_CFS2	3
#define SIM_CFS1	4
#define SIM_FLY	5
#define SIM_FS2K2	6
#define SIM_FS2K4	7
#define SIM_FSX	8
#define SIM_ESP	9
#define SIM_P3D	10
#define SIM_FSX64	11
#define SIM_P3D64	12

// Error numbers
#define FSUIPC_ERR_OK		0
#define FSUIPC_ERR_OPEN		1	// Attempt to Open when already Open
#define FSUIPC_ERR_NOFS		2	// Cannot link to FSUIPC or WideClient
#define FSUIPC_ERR_REGMSG	3	// Failed to Register common message with Windows
#define FSUIPC_ERR_ATOM		4	// Failed to create Atom for mapping filename
#define FSUIPC_ERR_MAP		5	// Failed to create a file mapping object
#define FSUIPC_ERR_VIEW		6	// Failed to open a view to the file map
#define FSUIPC_ERR_VERSION	7	// Incorrect version of FSUIPC, or not FSUIPC
#define FSUIPC_ERR_WRONGFS	8	// Sim is not version requested
#define FSUIPC_ERR_NOTOPEN	9	// Call cannot execute, link not Open
#define FSUIPC_ERR_NODATA	10	// Call cannot execute: no requests accumulated
#define FSUIPC_ERR_TIMEOUT	11	// IPC timed out all retries
#define FSUIPC_ERR_SENDMSG	12	// IPC sendmessage failed all retries
#define FSUIPC_ERR_DATA		13	// IPC request contains bad data
#define FSUIPC_ERR_RUNNING	14	// Maybe running on WideClient, but FS not running on Server, or wrong FSUIPC
#define FSUIPC_ERR_SIZE		15	// Read or Write request cannot be added, memory for Process is full

#ifdef __cplusplus
extern "C" {
#endif

// Globals accessible from main code
extern DWORD FSUIPC_Version;	// HIWORD is 1000 x Version Number, minimum 1998
										// LOWORD is build letter, with a = 1 etc. For 1998 this must be at least 5 (1998e)
extern DWORD FSUIPC_FS_Version;
										// FS98=1, FS2k=2, CFS2=3. See above.
extern DWORD FSUIPC_Lib_Version;
										// HIWORD is 1000 x version, LOWORD is build letter, a = 1 etc.

// Library routines
extern BOOL FSUIPC_Open(DWORD dwFSReq, DWORD *pdwResult); // For use externally (IPCuser.lib)
extern BOOL FSUIPC_Open2(DWORD dwFSReq, DWORD *pdwResult, BYTE *pMem, DWORD dwSize); // For use internally (ModuleUser.lib)
extern void FSUIPC_Close(void);
extern BOOL FSUIPC_Read(DWORD dwOffset, DWORD dwSize, void *pDest, DWORD *pdwResult);
extern BOOL FSUIPC_ReadSpecial(DWORD dwOffset, DWORD dwSize, void *pDest, DWORD *pdwResult);
extern BOOL FSUIPC_Write(DWORD dwOffset, DWORD dwSize, void *pSrce, DWORD *pdwResult);
extern BOOL FSUIPC_Process(DWORD *pdwResult);

#ifdef __cplusplus
};
#endif

#endif // _FSUIPC_H_
