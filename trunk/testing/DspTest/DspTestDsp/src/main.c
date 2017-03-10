/// @file
///
/// @brief
///     DSP��ִ�г������ڶ�ARM��DSP�����ٶȵĲ���
///
///
/// Copyright (c) 2012 IUnknownBase Inc
///
/// ���ߣ�
///    chao  2013.11.22
///
/// �޸���ʷ��
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
#include "Clk.h"                                         // ����ʱ�����
#include "gbl.h"
#include "DspArmProtocol.h"
#define  ARM2DSP_CONTROLCODE_SPEED_TEST     0x60
#define  DSP2ARM_CONTROLCODE_SPEED_TEST     0x61

#define PACK_ONE __attribute__((__packed__))

///
/// @brief
///     ��FILEID��SET_FAILURE_REASON��ʹ��
///
#define FILEID              FID_APP_C

///
/// @brief
///     ������־������ⲿLog��������
///
extern LOG_Obj trace;

///
/// @brief
///     ָ�򱣴���Ӧ�ó��򵽷���/���ջ����������ݴ�����̵�һЩ��
///     ������������������������������Ϣ�ȣ���ָ�롣
///     Ӧ�ó���ͬ�׶εĹ���/���������������͡����պͽ�������Ҫ��
///
TSKLOOP_TransferInfo *info;

///
/// @brief
///     TSKLOOP�Ļ�������size
///
Uint32 xferBufSize;

///
/// @brief
///     ����/���ջ�������Ӧ�ó���֮������ݴ���ĵ�����������ֵΪ0ʱΪ���޵�����
///
Uint16 numTransfers;

///
/// @brief
///     ARM��DSP�������ݽ��������ݻ�����
///
Uint8 *g_RecvBuf = NULL;         ///< ����һ��ָ�룬��ָ��ָ����ǽ��յ�ARM�˵�����
Uint8 *g_SendBuf = NULL;         ///< ����һ��ָ�룬��ָ��ָ����Ƿ��͵�ARM�˵�����

/// @brief
///     �����̣߳���ɶ�ARM�����ݵĽ��ա��㷨����ͼ������ķ���
///
static Int tskLoop () ;


///
/// @brief
///     ������
///
Void main()
{
    Int UresSize;

    // ������
    TSK_Handle tskLoopTask;

    // ��ʼ��DSP/BIOS����
    DSPLINK_init () ;

    // ���÷���/���ջ�������Ӧ�ó���֮������ݴ���ĵ�������
    numTransfers = 1;

    // �������ݻ�������С,�����С���û����壬ע����GPP�˷���Ĵ�СҪ����һ��
    UresSize = ARM_DSP_BUF_SIZE;

    xferBufSize = DSPLINK_ALIGN ((UresSize / DSP_MAUSIZE),DSPLINK_BUF_ALIGN);

    // ����TSKLOOP
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
///     �����̣߳���ɶ�ARM�����ݵĽ��ա��㷨����ͼ������ķ���
///
static Int tskLoop()
{
	FrameHead *rfh    = NULL;
	FrameHead *sfh	  = NULL;
    Int status       = SYS_OK;
	// ��ʶ�Ƿ񷵻�����
	Bool IsReturn = TRUE;
	float *pDspTime = NULL;
	Uint32 useTime1 = 0, useTime2 = 0;
	int old;

	// ����TSKLOOP�Ľ׶��Թ���/��������ʼ��info������g_SendBufָ���ͻ�����
    status = TSKDSP_create(&info, &g_SendBuf);
       
    // ѭ�����մ���
    while(1)
   {
        // ����ARM�˴��������ݣ���g_RecvBufָ����ջ�����
        if (status == SYS_OK)
		{
            status = TSKDSP_execute_Receive(info, &g_RecvBuf);
            if (status != SYS_OK)
			{
                SET_FAILURE_REASON(status);
            }
        }
		// ���յ�ʱ��
		old = HWI_disable();
		useTime1 = CLK_gethtime();   

		HAL_cacheInv ((Ptr) g_RecvBuf, ARM_DSP_BUF_SIZE);
        // ���ݹ�����ѡ��ִ����Ӧ���㷨
        if(status == SYS_OK)
		{
			rfh = (FrameHead *)g_RecvBuf;      
			
			switch (rfh->nCommandID)
    	    {
				case ARM2DSP_CONTROLCODE_SPEED_TEST:
				{
					// ֱ�Ӹ������뻺���������������					
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
		// ������ִ�н�����͸�DSP��
        if (status == SYS_OK && IsReturn == TRUE)
		{
			HWI_restore( old );

			// ��������֮���ʱ��
		    useTime2 = CLK_gethtime(); 

			pDspTime = (float *)(g_SendBuf+sizeof(FrameHead));
			// ֡ͷ�ĵ�һ����������ʾdsp�������ݵ�ʱ��
			*pDspTime = (useTime2 - useTime1) * CLK_cpuCyclesPerHtime() / GBL_getFrequency();

            status = TSKDSP_execute_Send(info);
            if (status != SYS_OK)
			{
                SET_FAILURE_REASON(status);
            }
        }
    }

EXIT:
    // �����߳�TSKLOOP�Ľ׶��Թ���/����
    status = TSKDSP_delete(info);
    if (status != SYS_OK)
    {
        SET_FAILURE_REASON(status);
    }

   return status ;
}
