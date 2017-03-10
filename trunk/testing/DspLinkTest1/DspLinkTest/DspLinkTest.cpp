// DspLinkTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
/*  ----------------------------------- OS Specific Headers           */
#include <windows.h>

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>

/*  ----------------------------------- Application Header            */
#include <loop_os.h>

// 在loop.h中定义TEST_SEND或者TEST_RECV来分别测试发送和接收
#include <loop.h>
int _tmain(int argc, _TCHAR* argv[])
{
#ifdef TEST_SEND
#pragma message("DspLinkTest.cpp:TEST SEND")
    Char8 * dspExecutable    = "\\FlashDisk\\loop.out" ;
#endif
#ifdef TEST_RECV
#pragma message("DspLinkTest.cpp:TEST RECV")
    Char8 * dspExecutable    = "\\FlashDisk\\loop_recv.out" ;
#endif
    Char8 * strBufferSize    = "10240";
    Char8 * strNumIterations = "10" ;
    Char8 * strProcessorId   = "0" ;
    LOOP_Main (dspExecutable, strBufferSize, strNumIterations, strProcessorId);
    getchar();
    return 0 ;
}

