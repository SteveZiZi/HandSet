/// @file
///
/// @brief
///     DSP端执行程序，用于对ARM到DSP传输速度的测试
///
///
/// Copyright (c) 2012 IUnknownBase Inc
///
/// 作者：
///    chao  2013.11.22
///
/// 修改历史：
///
 
///
/// @brief
///     DSP/BIOS Headers
///
#include <std.h>
#include <log.h>
#include <swi.h>
#include <sys.h>
#include <sio.h>
#include <tsk.h>
#include <gio.h>
#include <pool.h>

///
/// @brief
///     DSP/BIOS LINK Headers
///
#include <dsplink.h>
#include <failure.h>

///
/// @brief
///     Sample Headers
///
#include <tskLoop.h>
#include <loop_config.h>
#include <math.h>
#include <hal/hal_cache.h>
#include "Clk.h"                                         // 用于时间计算
#include "gbl.h"
#include "DspArmProtocol.h"
#define  ARM2DSP_CONTROLCODE_SPEED_TEST     0x60
#define  DSP2ARM_CONTROLCODE_SPEED_TEST     0x61

#define PACK_ONE __attribute__((__packed__))

///
/// @brief
///     该FILEID被SET_FAILURE_REASON宏使用
///
#define FILEID              FID_APP_C

///
/// @brief
///     用于日志输出的外部Log对象声明
///
extern LOG_Obj trace;

///
/// @brief
///     指向保存有应用程序到发送/接收缓存区的数据传输过程的一些参
///     数（包括输入输出流句柄、缓存区信息等）的指针。
///     应用程序不同阶段的功能/操作（启动、发送、接收和结束）需要。
///
TSKLOOP_TransferInfo *info;

///
/// @brief
///     TSKLOOP的缓存区的size
///
Uint32 xferBufSize;

///
/// @brief
///     发送/接收缓存区和应用程序之间的数据传输的迭代次数，当值为0时为无限迭代。
///
Uint16 numTransfers;

///
/// @brief
///     ARM与DSP进行数据交互的数据缓存区
///
Uint8 *g_RecvBuf = NULL;         ///< 定义一个指针，该指针指向的是接收到ARM端的数据
Uint8 *g_SendBuf = NULL;         ///< 定义一个指针，该指针指向的是发送到ARM端的数据

/// @brief
///     任务线程，完成对ARM端数据的接收、算法运算和计算结果的返回
///
static Int tskLoop () ;


///
/// @brief
///     主函数
///
Void main()
{
    Int UresSize;

    // 任务句柄
    TSK_Handle tskLoopTask;

    // 初始化DSP/BIOS连接
    DSPLINK_init () ;

    // 设置发送/接收缓存区和应用程序之间的数据传输的迭代次数
    numTransfers = 1;

    // 设置数据缓存区大小,这个大小由用户定义，注意与GPP端分配的大小要保持一致
    UresSize = ARM_DSP_BUF_SIZE;

    xferBufSize = DSPLINK_ALIGN ((UresSize / DSP_MAUSIZE),DSPLINK_BUF_ALIGN);

    // 创建TSKLOOP
    tskLoopTask = TSK_create(tskLoop, NULL);
    if (tskLoopTask != NULL)
    {
        LOG_printf(&trace, "Create TSKLOOP: Success\n");
    }
    else
    {
        LOG_printf(&trace, "Create TSKLOOP: Failed.\n");
    }

    return;
}

///
/// @brief
///     任务线程，完成对ARM端数据的接收、算法运算和计算结果的返回
///
static Int tskLoop()
{
	FrameHead *rfh    = NULL;
	FrameHead *sfh	  = NULL;
    Int status       = SYS_OK;
	// 标识是否返回数据
	Bool IsReturn = TRUE;
	float *pDspTime = NULL;
	Uint32 useTime1 = 0, useTime2 = 0;
	int old;

	// 启动TSKLOOP的阶段性功能/操作，初始化info，并将g_SendBuf指向发送缓存区
    status = TSKDSP_create(&info, &g_SendBuf);
       
    // 循环接收处理
    while(1)
   {
        // 接收ARM端传来的数据，将g_RecvBuf指向接收缓存区
        if (status == SYS_OK)
		{
            status = TSKDSP_execute_Receive(info, &g_RecvBuf);
            if (status != SYS_OK)
			{
                SET_FAILURE_REASON(status);
            }
        }
		// 接收到时间
		old = HWI_disable();
		useTime1 = CLK_gethtime();   

		HAL_cacheInv ((Ptr) g_RecvBuf, ARM_DSP_BUF_SIZE);
        // 根据功能码选择执行相应的算法
        if(status == SYS_OK)
		{
			rfh = (FrameHead *)g_RecvBuf;      
			
			switch (rfh->nCommandID)
    	    {
				case ARM2DSP_CONTROLCODE_SPEED_TEST:
				{
					// 直接复制输入缓冲区到输出缓冲区					
					memcpy(g_RecvBuf+sizeof(FrameHead), g_SendBuf+sizeof(FrameHead),
						 rfh->nDataLength);
					sfh->nCommandID = DSP2ARM_CONTROLCODE_SPEED_TEST;
					sfh->nDataLength = rfh->nDataLength;
					IsReturn = TRUE;
				}
				break;
				case ARM2DSP_CONTROLCODE_QUIT:
				{
					goto EXIT;
				}
				break;
				default:
				{
					;
				}
			}
        }
		HAL_cacheWbInv ((Ptr)g_SendBuf, ARM_DSP_BUF_SIZE);
		// 将命令执行结果发送给DSP端
        if (status == SYS_OK && IsReturn == TRUE)
		{
			HWI_restore( old );

			// 处理数据之后的时间
		    useTime2 = CLK_gethtime(); 

			pDspTime = (float *)(g_SendBuf+sizeof(FrameHead));
			// 帧头的第一个浮点数表示dsp处理数据的时间
			*pDspTime = (useTime2 - useTime1) * CLK_cpuCyclesPerHtime() / GBL_getFrequency();

            status = TSKDSP_execute_Send(info);
            if (status != SYS_OK)
			{
                SET_FAILURE_REASON(status);
            }
        }
    }

EXIT:
    // 结束线程TSKLOOP的阶段性功能/操作
    status = TSKDSP_delete(info);
    if (status != SYS_OK)
    {
        SET_FAILURE_REASON(status);
    }

   return status ;
}
