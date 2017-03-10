// DspLinkTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
/*  ----------------------------------- OS Specific Headers           */
#include <windows.h>

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>

/*  ----------------------------------- Application Header            */
#include <loop_os.h>
#include <loop.h>
int _tmain(int argc, _TCHAR* argv[])
{
#if 0
#ifdef TEST_SEND
#pragma message("DspLinkTest.cpp:TEST SEND")
    Char8 * dspExecutable    = "\\FlashDisk\\loop_send.out" ;
#endif
#ifdef TEST_RECV
#pragma message("DspLinkTest.cpp:TEST RECV")
    Char8 * dspExecutable    = "\\FlashDisk\\loop_recv.out" ;
#endif
#endif
    Char8 * dspExecutable    = "\\FlashDisk\\loop.out" ;
    Char8 * strBufferSize    = "20480";
    Char8 * strNumIterations = "1" ;
    Char8 * strProcessorId   = "0" ;
    LOOP_Main (dspExecutable, strBufferSize, strNumIterations, strProcessorId);
    getchar();
    return 0 ;
}

