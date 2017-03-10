#include "stdafx.h"
#include "gtest/gtest.h"
#include "DspControllor.h"
#include "../../../inc/DspArmProtocol.h"

#define  ARM2DSP_CONTROLCODE_SPEED_TEST     0x60
#define  DSP2ARM_CONTROLCODE_SPEED_TEST     0x61

#ifdef PT2FILE
    #define PT(param)  fprintf(fp,param)
#else
    #define PT(param)  printf(param)
#endif
TEST(DSP_TEST, SPEED_TEST)
{
    CDspController *pDspCtrl = CDspController::GetInstance();
    BOOL ret = pDspCtrl->DspInit();
    EXPECT_TRUE(ret);
    UINT8 *pSendBuf = new UINT8[32*1024];
    UINT8 *pRevBuf = new UINT8[32*1024];
    FrameHead rfh;
    float fDspTime = 0; 

    for(int i=0; i<32*1024; i++)
    {
        pSendBuf[i] = rand()%256;
    }

    LARGE_INTEGER  secfreq;  QueryPerformanceFrequency(&secfreq);       // 获得每秒可以计数tick多少次
    double msfreq; msfreq = secfreq.QuadPart / 1000.0;  // 获得每毫秒可以计数tick多少次
    LARGE_INTEGER  lastTick;  
    LARGE_INTEGER  curTick;  

    //FILE *fp = fopen('speed_test.log', 'a');
    printf("freq:%x, %x\n", secfreq.u.HighPart, secfreq.u.LowPart);
    QueryPerformanceCounter(&lastTick);       // 获得最后一次计数tick
    Sleep(1000);
    QueryPerformanceCounter(&curTick);     // 获得当前的tick
    printf("byte: %lf ms\n",(curTick.QuadPart-lastTick.QuadPart)/msfreq);

    for(int i=0; i<10; i++)
    {
#if 0
        printf("------------------ %d ---------------------\n",i);        
        // 32字节测试
        QueryPerformanceCounter(&lastTick);       // 获得最后一次计数tick    
        pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SPEED_TEST, pSendBuf, 32);
        QueryPerformanceCounter(&curTick);     // 获得当前的tick
        printf("32 byte: %lf ms\n",(curTick.QuadPart-lastTick.QuadPart)/msfreq);

        QueryPerformanceCounter(&lastTick);       // 获得最后一次计数tick    
        pDspCtrl->FrameRecv(rfh.nCommandID, (void**)&pRevBuf, rfh.nDataLength);
        QueryPerformanceCounter(&curTick);     // 获得当前的tick    
        printf("32 byte: %lf ms\n",(curTick.QuadPart-lastTick.QuadPart)/msfreq);
        fDspTime = pRevBuf[0];
        printf("DspTime: %lf ms\n\n",fDspTime);

        // 256字节测试    
        QueryPerformanceCounter(&lastTick);       // 获得最后一次计数tick    
        pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SPEED_TEST, pSendBuf, 256);
        QueryPerformanceCounter(&curTick);     // 获得当前的tick
        printf("256 byte: %lf ms\n",(curTick.QuadPart-lastTick.QuadPart)/msfreq);

        QueryPerformanceCounter(&lastTick);       // 获得最后一次计数tick    
        pDspCtrl->FrameRecv(rfh.nCommandID, (void**)&pRevBuf, rfh.nDataLength);
        QueryPerformanceCounter(&curTick);     // 获得当前的tick
        printf("256 byte: %lf ms\n",(curTick.QuadPart-lastTick.QuadPart)/msfreq);
        fDspTime = pRevBuf[0];
        printf("DspTime: %lf ms\n\n",fDspTime);

        // 1k字节测试
        QueryPerformanceCounter(&lastTick);       // 获得最后一次计数tick    
        pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SPEED_TEST, pSendBuf, 1000);
        QueryPerformanceCounter(&curTick);     // 获得当前的tick
        printf("1k byte: %lf ms\n",(curTick.QuadPart-lastTick.QuadPart)/msfreq);

        QueryPerformanceCounter(&lastTick);       // 获得最后一次计数tick    
        pDspCtrl->FrameRecv(rfh.nCommandID, (void**)&pRevBuf, rfh.nDataLength);
        QueryPerformanceCounter(&curTick);     // 获得当前的tick
        printf("1k byte: %lf ms\n",(curTick.QuadPart-lastTick.QuadPart)/msfreq);
        fDspTime = pRevBuf[0];
        printf("DspTime: %lf ms\n\n",fDspTime);

        // 10k字节测试
        QueryPerformanceCounter(&lastTick);       // 获得最后一次计数tick    
        pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SPEED_TEST, pSendBuf, 10*1000);
        QueryPerformanceCounter(&curTick);     // 获得当前的tick
        printf("10k byte: %lf ms\n",(curTick.QuadPart-lastTick.QuadPart)/msfreq);

        QueryPerformanceCounter(&lastTick);       // 获得最后一次计数tick    
        pDspCtrl->FrameRecv(rfh.nCommandID, (void**)&pRevBuf, rfh.nDataLength);
        QueryPerformanceCounter(&curTick);     // 获得当前的tick
        printf("10k byte: %lf ms\n",(curTick.QuadPart-lastTick.QuadPart)/msfreq);
        fDspTime = pRevBuf[0];
        printf("DspTime: %lf ms\n\n",fDspTime);

        // 32k字节测试
        QueryPerformanceCounter(&lastTick);       // 获得最后一次计数tick    
        pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SPEED_TEST, pSendBuf, 32*1000);
        QueryPerformanceCounter(&curTick);     // 获得当前的tick
        printf("32k byte: %lf ms\n",(curTick.QuadPart-lastTick.QuadPart)/msfreq);

        QueryPerformanceCounter(&lastTick);       // 获得最后一次计数tick    
        pDspCtrl->FrameRecv(rfh.nCommandID, (void**)&pRevBuf, rfh.nDataLength);
        QueryPerformanceCounter(&curTick);     // 获得当前的tick
        printf("32k byte: %lf ms\n",(curTick.QuadPart-lastTick.QuadPart)/msfreq);
        fDspTime = pRevBuf[0];
        printf("DspTime: %lf ms\n\n",fDspTime);
#endif
        // 32字节测试
        pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SPEED_TEST, pSendBuf, 32);
        pDspCtrl->FrameRecv(rfh.nCommandID, (void**)&pRevBuf, rfh.nDataLength);
        fDspTime = pRevBuf[0];
        printf("DspTime: %lf ms\n\n",fDspTime);
        
        // 256字节测试            
        pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SPEED_TEST, pSendBuf, 256);  
        pDspCtrl->FrameRecv(rfh.nCommandID, (void**)&pRevBuf, rfh.nDataLength);
        fDspTime = pRevBuf[0];
        printf("DspTime: %lf ms\n\n",fDspTime);

        // 1k字节测试 
        pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SPEED_TEST, pSendBuf, 1000);
        pDspCtrl->FrameRecv(rfh.nCommandID, (void**)&pRevBuf, rfh.nDataLength);
        fDspTime = pRevBuf[0];
        printf("DspTime: %lf ms\n\n",fDspTime);

        // 10k字节测试
        pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SPEED_TEST, pSendBuf, 10*1000);
        pDspCtrl->FrameRecv(rfh.nCommandID, (void**)&pRevBuf, rfh.nDataLength);
        fDspTime = pRevBuf[0];
        printf("DspTime: %lf ms\n\n",fDspTime);

        // 32k字节测试
        pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SPEED_TEST, pSendBuf, 32*1000);
        pDspCtrl->FrameRecv(rfh.nCommandID, (void**)&pRevBuf, rfh.nDataLength);
        fDspTime = pRevBuf[0];
        printf("DspTime: %lf ms\n\n",fDspTime);
    }
    
    // 释放资源
    pDspCtrl->DspDelete();
    delete[] pSendBuf;
    //fclose(fp);
}