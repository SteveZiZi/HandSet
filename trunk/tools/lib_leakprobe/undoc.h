#pragma once


#ifdef __cplusplus
extern "C" {
#endif


// Definitions used with GetCallstackSnapshot

#define STACKSNAP_FAIL_IF_INCOMPLETE     1
#define STACKSNAP_EXTENDED_INFO          2
#define STACKSNAP_INPROC_ONLY            4     // Capture only the base part of the stack within
                                               // the thread's owner proc, no PSL transitions
#define STACKSNAP_RETURN_FRAMES_ON_ERROR 8     // Return number of frames found, even if error occurs. SetLastError will be set.
#define STACKSNAP_NEW_VM                 16    // New VM model, return CallSnapshot3.

typedef struct _CallSnapshot {
    DWORD dwReturnAddr;
} CallSnapshot;

typedef struct _CallSnapshotEx {
    DWORD dwReturnAddr;
    DWORD dwFramePtr;
    DWORD dwCurProc;
    DWORD dwParams[4];
} CallSnapshotEx;



ULONG GetCallStackSnapshot (ULONG dwMaxFrames, CallSnapshot lpFrames[], DWORD dwFlags, DWORD dwSkip);
ULONG GetThreadCallStack (HANDLE hThrd, ULONG dwMaxFrames, LPVOID lpFrames, DWORD dwFlags, DWORD dwSkip);


#ifdef __cplusplus
}
#endif
