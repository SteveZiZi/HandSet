/** ============================================================================
*  @file   main.c
*  @path   $(DSPLINK)/dsp/src/samples/loop/
*  @desc   Main function that calls SWI or TSK loopback applications based
*          on the compilation flag SWI_MODE or TSK_MODE.
*  @ver    1.64
*  ============================================================================*/
/*  ----------------------------------- DSP/BIOS Headers            */

#include <main.h>
#include <hal/hal_cache.h>
#include "DspArmProtocol.h"
#include "dspCalc.h"
#include "Clk.h"                                         // ����ʱ�����
#include "gbl.h"

#ifdef _DEBUG
#define FILEID FID_APP_C
#endif

#define _SMV_TIME_TEST  0								// ����г������ʱ�俪��
#define _DSP_VERSION 	7								// DSP�汾�ţ��ⲿ��ʾΪ����10��ĸ���ֵ
#define _LOST_POINT_MODE								// DSP����ģʽ�����������ú꣬��Ϊ����ģʽ

extern LOG_Obj trace;

Uint8 *g_recvBuf = NULL;                                 // ָ����յ����ݻ�����
Uint8 *g_sendBuf = NULL;                                 // ָ�������ݻ�����
Uint16 numTransfers ;
Uint32 xferBufSize ;

TSKLOOP_TransferInfo *info;                              // ���շ�����Ϣ�ṹ��

Config g_config = {0};                                   ///< ȫ�ֲ�������

SmvCalcData g_smvCalcData = {0};                         ///< ȫ�ֲ������ݻ���
SmvCalcData g_smvCalcData1 = {0};                        ///< ȫ�ֲ������ݻ���

MSGTIME g_msgTime[MAX_MSG_TIME_CALC_COUNT] = {0};        ///< ȫ�ֱ���ʱ��㻺��
MSGTIME g_msgTime1[MAX_MSG_TIME_CALC_COUNT] = {0};       ///< ȫ�ֱ���ʱ��㻺��

static uint8_t g_fSmvSwap = 0;                           ///< ����SMV��ƹ�ұ�ʶ
static uint8_t g_fDispSwap = 0;                          ///< ���㱨����ɢ�ȵ�ƹ�ұ�ʶ

#ifdef _LOST_POINT_MODE
	uint8_t g_fSmvLostPoint = 0;							///< SMV�����ʶ
	uint8_t g_fDispLostPoint = 0;							///< ������ɢ�ȶ����ʶ
#endif

// ȫ�ֲ������ɻ�����
Error g_error[8] = {0};
DspInfo g_dspInfo = {0};                                 ///< ȫ��DSP��Ϣ

SmvData g_smvRealData = {0};

SMV_GEN_DATA_CALC_RT g_smvGenRT = {0};
float g_pMsgDisp[11] = {0.0f};

SMV_DATA_CALC_RT g_smvCalcRT = {0};

//uint32_t g_smvGenNum;

uint8_t g_tskFlag[8]={0};                                 ///< ȫ�������־

// �ش����ݱ�ʶ
enum
{
    RES_CONFIG = 0,
    RES_DSP_INFO,
    RES_SMV_REAL_DATA,
    RES_SMV_GEN_RT,
    RES_MSG_DISP,
    RES_SMV_CALC_RT,
    RES_STOP_READ,
    RES_LAST
};
uint8_t g_resFlag[RES_LAST]={0};                   ///< ȫ�ֽ����־
uint32_t g_resLength[RES_LAST]={0};                ///< ȫ�ֽ�����ݳ���
// ȫ�ֽ���Ļ�����ָ��
const void *g_resPtr[RES_LAST]={&g_error[0], &g_dspInfo, &g_smvRealData,
                &g_smvGenRT, g_pMsgDisp, &g_smvCalcRT, NULL};

static uint16_t g_nSmvCacheCount = 0;              ///< ȫ�ֲ���ֵ�������
static uint16_t g_nTimeCacheCount = 0;             ///< ȫ�ֱ���ʱ�仺�����



static void InitConfig();
static void InitBuffers();
static void SetConfig();
static void GetDspInfo();
static void ResetSmvCache();
static void ResetTimeCache();

static void SmvHelper();
static void SmvGenHelper();
static void MsgDispHelper();
static void RealSmvHelper();

// ���԰汾����
#ifdef _DEBUG
Void SetReason (int fileId, int lineNo, int failureCode)
{

}
#endif

/// ��ʼ��DSP����
void InitConfig()
{
    memset(&g_config, 0, sizeof(Config));
    g_config.nMsgTimeCalcCount = 4680;
    g_config.nASDUCount = 8;
	g_config.nSamplingFreq = 12800;
	g_config.nRatedFreq = 50;    
    
    // ��ʼ��������ɢ��ӳ��hash����
    dispHashCreate(g_config.nASDUCount);
}

/// ��ʼ��������
void InitBuffers()
{
    // ȫ�ֻ�������0
    memset(g_resFlag, 0, 8*sizeof(uint8_t));
    g_smvCalcData.nChannelCount=0;
    g_smvCalcData.nSmvPerCircle=0;
    memset(g_smvCalcData.nChannelVCType,0,MAX_SMV_CHANNELS*sizeof(uint8_t));
    memset(g_smvCalcData.fChannelData,0, 
        MAX_SMV_CHANNELS*MAX_SMV_CALC_COUNT*sizeof(float));
    
}


/// ����DSP����
void SetConfig()
{
    Error err;
    Config tmpConfig;
	int32_t nSmvCalcCount = 0;
    
    // ��������
    memcpy(&tmpConfig, g_recvBuf+sizeof(FrameHead), sizeof(Config));

	nSmvCalcCount = tmpConfig.nSamplingFreq/tmpConfig.nRatedFreq*10;

    err.nErrno = 0;                                 //success
    // ֻ�����⼸�̶ֹ���С�Ĳ�������㣬���ݲ�ͬ�Ĳ���Ƶ�ʵ���
    switch(nSmvCalcCount)
    {
        case 800:
        case 1000:
        case 1600:
        case 2000:
        case 2560: err.nErrno = 0; break;
        default: err.nErrno = 1; break;
    }

    if(tmpConfig.nMsgTimeCalcCount > MAX_MSG_TIME_CALC_COUNT)
    {
        err.nErrno = 2;
    }

    if(dispHashCreate(tmpConfig.nASDUCount) != 0 )
    {
        err.nErrno = 3;
    }
    
    // û�д���Ϳ�����������
    if(err.nErrno == 0)
    {
        memcpy(&g_config, &tmpConfig, sizeof(Config));    
    }

    // �������
    memcpy(&g_error[0], &err, sizeof(Error));

}

/// ��ȡDSP��Ϣ
void GetDspInfo()
{
    // ����DSP�汾�����
    g_dspInfo.nVersion = _DSP_VERSION;

}

/// ���SMV���ݻ���
void ResetSmvCache()
{
    // ����ջ����������ƶ�ָ�뵽��ͷ
    g_nSmvCacheCount = 0;
    
    // ���ʵ��ֵ����������
	// ʵ��ֵ���ش�
    g_resFlag[RES_SMV_REAL_DATA] = 0;
    g_resLength[RES_SMV_REAL_DATA] = sizeof(g_smvRealData);
    memset(&g_smvRealData, 0, g_resLength[RES_SMV_REAL_DATA]);

    // ���г���ȼ���������������
    g_resFlag[RES_SMV_CALC_RT] = DSP2ARM_CONTROLCODE_SMV_CALC_RT;
    g_resLength[RES_SMV_CALC_RT] = sizeof(g_smvCalcRT);
    memset(&g_smvCalcRT, 0, g_resLength[RES_SMV_CALC_RT]);
}

/// ��ձ���ʱ�仺��
void ResetTimeCache()
{
    g_nTimeCacheCount = 0;
    
	// ���ñ���ͳ������
	resetDisp();

    // ��ձ�����ɢ�Ƚ������
    g_resFlag[RES_MSG_DISP] = DSP2ARM_CONTROLCODE_MSG_DISP_RT;
    g_resLength[RES_MSG_DISP] = sizeof(g_pMsgDisp[0])*11;
    memset(g_pMsgDisp, 0, g_resLength[RES_MSG_DISP]);
}

/// ��������
void SmvGenHelper()
{
    SmvGenData smvGenData;
    
    memcpy(&smvGenData, g_recvBuf+sizeof(FrameHead), sizeof(SmvGenData));
    
    SmvGenCalc(&smvGenData, &g_smvGenRT);
	
}

/// ������ɢ�ȼ���
void MsgDispHelper()
{
    MsgTimeData msgTimeData;
	uint8_t fFirstCalc = 1;
    memcpy(&msgTimeData, g_recvBuf+sizeof(FrameHead), sizeof(MsgTimeData));

	// ƹ�Ҳ������������ݵ�����
	if(g_fDispSwap == 0)
	{
	    memcpy(&g_msgTime[g_nTimeCacheCount], msgTimeData.msgTimes, 
	        msgTimeData.nMsgTimeCount*sizeof(MSGTIME));
	}
	else
	{
	    memcpy(&g_msgTime1[g_nTimeCacheCount], msgTimeData.msgTimes, 
	        msgTimeData.nMsgTimeCount*sizeof(MSGTIME));
	}
    g_nTimeCacheCount = g_nTimeCacheCount + msgTimeData.nMsgTimeCount;

    // �ﵽ���㱨����ɢ������
    if(g_nTimeCacheCount >= g_config.nMsgTimeCalcCount)
    {
		// �״���������ȴ��������
		if(fFirstCalc)
		{
			fFirstCalc = 0;
		}
		else
		{
#if 1
#ifdef _LOST_POINT_MODE
			// ��������
			g_fDispLostPoint = 1;
			// �ͷŽ����ź���
			SEM_post(&SEM7);
#endif
#endif
			// �ȴ��������
			SEM_pend(&SEM6, SYS_FOREVER);
		}

		if(g_fDispSwap == 0)
		{
			g_fDispSwap = 1;
#if 1
#ifdef _LOST_POINT_MODE
			// �رն���
			g_fDispLostPoint = 0;
#endif
#endif
			// �ͷŽ����ź���
			SEM_post(&SEM7);


			MsgDispCalc(g_msgTime, &g_config, g_pMsgDisp);
			// ָʾ�������
			SEM_post(&SEM6);
		}
      	else
		{
			g_fDispSwap = 0;
#if 1
#ifdef _LOST_POINT_MODE
			// �رն���
			g_fDispLostPoint = 0;
#endif
#endif
			// �ͷŽ����ź���
			SEM_post(&SEM7);

			MsgDispCalc(g_msgTime1, &g_config, g_pMsgDisp);	
			// ָʾ�������
			SEM_post(&SEM6);
		}

        g_nTimeCacheCount = 0;
		g_resFlag[RES_MSG_DISP] = DSP2ARM_CONTROLCODE_MSG_DISP_RT;
        g_resLength[RES_MSG_DISP] = 11*sizeof(float);
        SEM_post(&SEM0);
    }
	else
	{
		SEM_post(&SEM7);
	}
}
/// ʵ��ֵ����
void RealSmvHelper()
{
    uint16_t i = 0;
    SmvData smvData;
	int32_t nSmvCalcCount = g_config.nSamplingFreq / g_config.nRatedFreq * 10;
	static uint8_t fFirstCalc = 1;

    // ���ԭ�����ֵ����
    memcpy(&smvData, g_recvBuf+sizeof(FrameHead), sizeof(SmvData));
    
    // ��ֹ���������
    if(smvData.nChannelCount > MAX_SMV_CHANNELS)
    {
        smvData.nChannelCount = MAX_SMV_CHANNELS;
    }
    if(smvData.nSmvPerCircle > MAX_SMV_PER_CIRCLE)
    {
        smvData.nSmvPerCircle = MAX_SMV_PER_CIRCLE;
    }
    
    RealSmvCalc(&smvData, &g_smvRealData);
    	

    if(g_nSmvCacheCount + smvData.nSmvPerCircle <= MAX_SMV_CALC_COUNT)
    { 
        for(i=0; i<smvData.nChannelCount; i++)
        {
			if(g_fSmvSwap == 0)
			{
	            memcpy(&g_smvCalcData.fChannelData[i][g_nSmvCacheCount], 
	                    g_smvRealData.fChannelData[i], 
	                    smvData.nSmvPerCircle*sizeof(float));
			}
			else
			{
	            memcpy(&g_smvCalcData1.fChannelData[i][g_nSmvCacheCount], 
	                    g_smvRealData.fChannelData[i], 
	                    smvData.nSmvPerCircle*sizeof(float));			   
			}
        }
    }
    else
    {
        // ���ݱ������Ӽ���ʵ��ֵ
        for(i=0; i<smvData.nChannelCount; i++)
        {
            // ƹ�Ҳ��������ƴ��ϴλ���㵽������ĩβ������
			if(g_fSmvSwap == 0)
			{
	            memcpy(&g_smvCalcData.fChannelData[i][g_nSmvCacheCount],
	                    g_smvRealData.fChannelData[i], 
	                    (MAX_SMV_CALC_COUNT-g_nSmvCacheCount)*sizeof(float));
	        }
	        else
	        {
	            memcpy(&g_smvCalcData1.fChannelData[i][g_nSmvCacheCount],
	                    g_smvRealData.fChannelData[i], 
	                    (MAX_SMV_CALC_COUNT-g_nSmvCacheCount)*sizeof(float));
	        }   
        }
    }
    
    // �������+=ÿͨ����������
    g_nSmvCacheCount = g_nSmvCacheCount + smvData.nSmvPerCircle;

    // ��ʱ���ش�ʵ��ֵ������
    /*g_resFlag[RES_SMV_REAL_DATA] = DSP2ARM_CONTROLCODE_SMV_DATA;
    g_resLength[RES_SMV_REAL_DATA] = sizeof(SmvData);
    SEM_post(&SEM0);*/
	
    // �����������ﵽ����г����������������г��������
    if(g_nSmvCacheCount >= nSmvCalcCount)
    {
		// ƹ�Ҳ�������������
		if(g_fSmvSwap == 0)
		{
	        g_smvCalcData.nChannelCount = smvData.nChannelCount;
	        g_smvCalcData.nSmvPerCircle = smvData.nSmvPerCircle;
	        memcpy(g_smvCalcData.nChannelVCType, smvData.nChannelVCType, 
	            smvData.nChannelCount*sizeof(smvData.nChannelVCType[0]));
		}
		else
		{
	        g_smvCalcData1.nChannelCount = smvData.nChannelCount;
	        g_smvCalcData1.nSmvPerCircle = smvData.nSmvPerCircle;
	        memcpy(g_smvCalcData1.nChannelVCType, smvData.nChannelVCType, 
	            smvData.nChannelCount*sizeof(smvData.nChannelVCType[0]));
		}

		// ������״μ��㣬���ȴ��������
		if(fFirstCalc)
		{
			fFirstCalc = 0;
		}
		else
		{

#ifdef _LOST_POINT_MODE
			// ��������
			g_fSmvLostPoint = 1;
			SEM_post(&SEM7);
#endif			
			// �ȴ��������
			SEM_pend(&SEM5, SYS_FOREVER);						
		}

        g_nSmvCacheCount = 0;

		// ����г������
		SEM_post(&SEM2);
    }
	else
	{
		// �ͷŽ����ź���
		SEM_post(&SEM7);
	}
}
   
/// г������Чֵ�����ʼ���
void SmvHelper()
{
#if _SMV_TIME_TEST
	Uint32 useTime1 = 0, useTime2 = 0;
    int old;
	old = HWI_disable();
    useTime1 = CLK_gethtime();
#endif
	// ƹ�Ҳ�������ָ����������Ϊ����
	if(g_fSmvSwap == 0)
	{
		g_fSmvSwap = 1;
#ifdef _LOST_POINT_MODE
		// �رն���
		g_fSmvLostPoint = 0;
#endif	
		// �ͷŽ����ź���
		SEM_post(&SEM7);

    	SmvDataCalc(&g_smvCalcData, &g_config, &g_smvCalcRT);		
	}
	else
	{
		g_fSmvSwap = 0;
#ifdef _LOST_POINT_MODE
		// �رն���
		g_fSmvLostPoint = 0;
#endif	
		// �ͷŽ����ź���
		SEM_post(&SEM7);

    	SmvDataCalc(&g_smvCalcData1, &g_config, &g_smvCalcRT);		
	}
	// �ͷż�������ź���
	SEM_post(&SEM5);

#if _SMV_TIME_TEST
    HWI_restore( old );
    useTime2 = CLK_gethtime(); 
	g_smvCalcRT.fRmsValue[0] = (useTime2 - useTime1) * CLK_cpuCyclesPerHtime() / GBL_getFrequency();
#endif
}
/*********************************************************************************************************
** Function name:           main
** Descriptions:            ����������ʼ������������dsplinkͨ��
** Input:                   ��
** Output:                  ��
** Note:
*********************************************************************************************************/
Void main()
{
    Int UresSize;

    //TSK_Handle tskLoopTask;
    Int status = SYS_OK;
    // Initialize DSP/BIOS LINK.
    DSPLINK_init () ;
    numTransfers = 1;

    // Initialize variable
    UresSize = ARM_DSP_BUF_SIZE; // user define, same to arm
    xferBufSize = DSPLINK_ALIGN ((UresSize / DSP_MAUSIZE),DSPLINK_BUF_ALIGN);
    
    InitConfig();
    InitBuffers();
    
    status = TSKDSP_create (&info,&g_sendBuf);
    if (status != SYS_OK)
    {
        SET_FAILURE_REASON(status);
    }

    return;
}
/*********************************************************************************************************
** Function name:           tskRecevieFun
** Descriptions:            ����ARM�˷�����������
** Input:                   ��
** Output:                  ��
** Note:                    TSKDSP_execute_Receive��û�н��յ�����ʱ��������״̬
*********************************************************************************************************/
void tskRecevieFun()
{
    Int status = SYS_OK; 
    FrameHead *ch;
    while(1)
    {
        status = TSKDSP_execute_Receive(info,&g_recvBuf);   // ��ARM�˵�
        if (status != SYS_OK){
            SET_FAILURE_REASON(status);
        }
    
		// ����CPU����
		HAL_cacheInv ((Ptr) g_recvBuf, ARM_DSP_BUF_SIZE) ;

        // ���յ���������*g_recvBuf��
        ch = (FrameHead *)g_recvBuf;

        // ���ݲ�ͬ��������ִ�в�ͬ�Ĳ���
        switch (ch->nCommandID)
        {
            case ARM2DSP_CONTROLCODE_CONFIG:
            {
                SetConfig();
                g_resFlag[RES_CONFIG] = DSP2ARM_CONTROLCODE_ERROR;
                g_resLength[RES_CONFIG] = sizeof(Error);
				SEM_post(&SEM7);
                SEM_post(&SEM0);
            }
            break;
            case ARM2DSP_CONTROLCODE_SMV_DATA:
            {
#ifdef _LOST_POINT_MODE
				// ��������ʶ��������ֱ�Ӷ���
				if(g_fSmvLostPoint)
				{
					continue;
				}
#endif
                g_tskFlag[1] = ARM2DSP_CONTROLCODE_SMV_DATA;
                SEM_post(&SEM1);
            }
            break;
            case ARM2DSP_CONTROLCODE_RESET_SMV:
            {
                ResetSmvCache();
				SEM_post(&SEM7);
                SEM_post(&SEM0);
            }
			break;
            case ARM2DSP_CONTROLCODE_RESET_TIME:
            {
                ResetTimeCache();
				SEM_post(&SEM7);
                SEM_post(&SEM0);
            }
            break;
            case ARM2DSP_CONTROLCODE_SMV_GEN:
            {
                g_tskFlag[1] = ARM2DSP_CONTROLCODE_SMV_GEN;
                SEM_post(&SEM1);
            }
            break;
            case ARM2DSP_CONTROLCODE_TIME_DATA:
            {
#ifdef _LOST_POINT_MODE
				// �������ģʽ��������ֱ�Ӷ���
				if(g_fDispLostPoint)
				{
					continue;
				}
#endif
                g_tskFlag[1] = ARM2DSP_CONTROLCODE_TIME_DATA;
                SEM_post(&SEM1);
            }
            break;
            case ARM2DSP_CONTROLCODE_INFO:
            {
                GetDspInfo();
                g_resFlag[RES_DSP_INFO] = DSP2ARM_CONTROLCODE_INFO;
                g_resLength[RES_DSP_INFO] = sizeof(DspInfo);
				SEM_post(&SEM7);
                SEM_post(&SEM0);
            }
            break;
            case ARM2DSP_CONTROLCODE_QUIT:
            {
                TSKDSP_delete(info);
            }
            break;
            case ARM2DSP_CONTROLCODE_STOP_READ:
            {
                g_resFlag[RES_STOP_READ] = DSP2ARM_CONTROLCODE_STOP_READ;
                g_resLength[RES_STOP_READ] = 0;
				SEM_post(&SEM7);
                SEM_post(&SEM0);
            }
            break;
        }
		SEM_pend(&SEM7, SYS_FOREVER);
        //SEM_post(&SEM1);
    }
}

/*********************************************************************************************************
** Function name:           tskSendFun
** Descriptions:            DSP�˷����������񣬷�����������ARN��
** Input:                   ��
** Output:                  ��
** Note:                    ���Ҫ���͵�����g_sendBuf[SIZE]�У�����TSKDSP_execute_Send����������ɷ�������
*********************************************************************************************************/
void tskSendFun()
{
    Int status = SYS_OK;
    FrameHead *sh = (FrameHead *)g_sendBuf;
    Int i=0;
    while(1)
    {
        SEM_pend(&SEM0, SYS_FOREVER);
       
        // �������ݵ�g_sendBuf��
        for(i=0; i<RES_LAST; i++)
        {
            if(g_resFlag[i] != 0 && g_resLength[i] != 0)
            {
                sh->nCommandID = g_resFlag[i];
                sh->nDataLength = g_resLength[i];   
                memcpy(g_sendBuf+sizeof(FrameHead), g_resPtr[i], g_resLength[i]);           
                g_resFlag[i] = 0;
                g_resLength[i] = 0;
                break;
            }
            else if(g_resFlag[i] != 0 && g_resLength[i] == 0)
            {
                sh->nCommandID = g_resFlag[i];
                sh->nDataLength = g_resLength[i];
                g_resFlag[i] = 0;
                break;
            }
        }

		// ��ϴCPU����
		HAL_cacheWbInv ((Ptr)g_sendBuf, ARM_DSP_BUF_SIZE);

        status = TSKDSP_execute_Send(info);
        if (status != SYS_OK){
                SET_FAILURE_REASON(status);
            }

    }    
}

/*********************************************************************************************************
** Function name:           tskCalc40msFun
** Descriptions:            40ms��������
** Input:                   ��
** Output:                  ��
** Note:                    ���㲨�����ɣ�������ɢ�Ⱥ�SMVʵ��ֵ
*********************************************************************************************************/
void tskCalc40msFun()
{
    while(1)
    {

        SEM_pend(&SEM1, SYS_FOREVER);
    
        /*
         * �������
         */
         switch(g_tskFlag[1])
         {
            case ARM2DSP_CONTROLCODE_SMV_GEN:
            {
                SmvGenHelper();
                g_resFlag[RES_SMV_GEN_RT] = DSP2ARM_CONTROLCODE_SMV_GEN_RT;
                g_resLength[RES_SMV_GEN_RT] = sizeof(SMV_GEN_DATA_CALC_RT);
				SEM_post(&SEM7);
                SEM_post(&SEM0);
            }
            break;
            case ARM2DSP_CONTROLCODE_TIME_DATA:
            {
                MsgDispHelper();
            }
            break;
            case ARM2DSP_CONTROLCODE_SMV_DATA:
            {
                RealSmvHelper();
                /*g_resFlag[RES_SMV_REAL_DATA] = DSP2ARM_CONTROLCODE_SMV_DATA;
                g_resLength[RES_SMV_REAL_DATA] = sizeof(SmvData);
                SEM_post(&SEM0);*/
            }
            break;
         }
         //SEM_post(&SEM0);          //  tskSendFun

    }           
}

/*********************************************************************************************************
** Function name:           tskCalc200msFun
** Descriptions:            200ms��������
** Input:                   ��
** Output:                  ��
** Note:                    ����г������г�����ߴ�г��������
*********************************************************************************************************/
void tskCalc200msFun()
{

    while(1)
    {
        SEM_pend(&SEM2, SYS_FOREVER);
        /** �������*/
        SmvHelper();
        g_resFlag[RES_SMV_CALC_RT] = DSP2ARM_CONTROLCODE_SMV_CALC_RT;
        g_resLength[RES_SMV_CALC_RT] = sizeof(SMV_DATA_CALC_RT);
        SEM_post(&SEM0);
    }
}

/*********************************************************************************************************
** Function name:           tskCalc5120msFun
** Descriptions:            5120ms��������
** Input:                   ��
** Output:                  ��
** Note:                    5.12s˲ʱ����ֵ����
*********************************************************************************************************/
void tskCalc5120msFun()
{
    while(1)
    {
        SEM_pend(&SEM3, SYS_FOREVER);
    }
}  
/*********************************************************************************************************
** Function name:           tskCalc10minFun
** Descriptions:            10min��������
** Input:                   ��
** Output:                  ��
** Note:                    10min������ֵ���㣬�ۼ�12�μ����������ֵ
*********************************************************************************************************/
void tskCalc10minFun()
{
    while(1)
    {
        SEM_pend(&SEM4, SYS_FOREVER);

    }
}
