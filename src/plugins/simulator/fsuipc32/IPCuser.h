#include <Windows.h>

#include "FSUIPC_User.h"

#define FS6IPC_MSGNAME1 L"FsasmLib:IPC"

#define FS6IPC_MESSAGE_SUCCESS 1
#define FS6IPC_MESSAGE_FAILURE 0

// IPC message types
#define FS6IPC_READSTATEDATA_ID 1
#define FS6IPC_WRITESTATEDATA_ID 2

// read request structure
typedef struct tagFS6IPC_READSTATEDATA_HDR
{
    DWORD dwId; // FS6IPC_READSTATEDATA_ID
    DWORD dwOffset; // state table offset
    DWORD nBytes; // number of bytes of state data to read
    void *pDest; // destination buffer for data (client use only)
} FS6IPC_READSTATEDATA_HDR;

// write request structure
typedef struct tagFS6IPC_WRITESTATEDATA_HDR
{
    DWORD dwId; // FS6IPC_WRITESTATEDATA_ID
    DWORD dwOffset; // state table offset
    DWORD nBytes; // number of bytes of state data to write
} FS6IPC_WRITESTATEDATA_HDR;
