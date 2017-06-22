#include <windows.h>

#include "FSUIPC_User64.h"

#define FS6IPC_MESSAGE_SUCCESS 1
#define FS6IPC_MESSAGE_FAILURE 0

// IPC message types
#define F64IPC_READSTATEDATA_ID    4
#define FS6IPC_WRITESTATEDATA_ID   2

#pragma pack (push, r1, 1)

// read request structure
typedef struct tagF64IPC_READSTATEDATA_HDR
{
  DWORD dwId;       // F64IPC_READSTATEDATA_ID
  DWORD dwOffset;   // state table offset
  DWORD nBytes;     // number of bytes of state data to read
  void* pDest;      // destination buffer for data (client use only)
} F64IPC_READSTATEDATA_HDR;

// write request structure
typedef struct tagFS6IPC_WRITESTATEDATA_HDR
{
  DWORD dwId;       // FS6IPC_WRITESTATEDATA_ID
  DWORD dwOffset;   // state table offset
  DWORD nBytes;     // number of bytes of state data to write
} FS6IPC_WRITESTATEDATA_HDR;

#pragma pack (pop, r1)

