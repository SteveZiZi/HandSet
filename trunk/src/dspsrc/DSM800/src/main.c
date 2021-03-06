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
#include "Clk.h"                                         // 用于时间计算
#include "gbl.h"

#ifdef _DEBUG
#define FILEID FID_APP_C
#endif

#define _SMV_TIME_TEST  0								// 测试谐波计算时间开关
#define _DSP_VERSION 	7								// DSP版本号，外部显示为除以10后的浮点值
#define _LOST_POINT_MODE								// DSP丢点模式，如果不定义该宏，则为阻塞模式

extern LOG_Obj trace;

Uint8 *g_recvBuf = NULL;                                 // 指向接收到数据缓存区
Uint8 *g_sendBuf = NULL;                                 // 指向发送数据缓存区
Uint16 numTransfers ;
Uint32 xferBufSize ;

TSKLOOP_TransferInfo *info;                              // 接收发送信息结构体

Config g_config = {0};                                   ///< 全局参数设置

SmvCalcData g_smvCalcData = {0};                         ///< 全局采样数据缓存
SmvCalcData g_smvCalcData1 = {0};                        ///< 全局采样数据缓存

MSGTIME g_msgTime[MAX_MSG_TIME_CALC_COUNT] = {0};        ///< 全局报文时间点缓存
MSGTIME g_msgTime1[MAX_MSG_TIME_CALC_COUNT] = {0};       ///< 全局报文时间点缓存

static uint8_t g_fSmvSwap = 0;                           ///< 计算SMV的乒乓标识
static uint8_t g_fDispSwap = 0;                          ///< 计算报文离散度的乒乓标识

#ifdef _LOST_POINT_MODE
	uint8_t g_fSmvLostPoint = 0;							///< SMV丢点标识
	uint8_t g_fDispLostPoint = 0;							///< 报文离散度丢点标识
#endif

// 全局波形生成缓冲区
Error g_error[8] = {0};
DspInfo g_dspInfo = {0};                                 ///< 全局DSP信息

SmvData g_smvRealData = {0};

SMV_GEN_DATA_CALC_RT g_smvGenRT = {0};
float g_pMsgDisp[11] = {0.0f};

SMV_DATA_CALC_RT g_smvCalcRT = {0};

//uint32_t g_smvGenNum;

uint8_t g_tskFlag[8]={0};                                 ///< 全局任务标志

// 回传数据标识
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
uint8_t g_resFlag[RES_LAST]={0};                   ///< 全局结果标志
uint32_t g_resLength[RES_LAST]={0};                ///< 全局结果数据长度
// 全局结果的缓冲区指针
const void *g_resPtr[RES_LAST]={&g_error[0], &g_dspInfo, &g_smvRealData,
                &g_smvGenRT, g_pMsgDisp, &g_smvCalcRT, NULL};

static uint16_t g_nSmvCacheCount = 0;              ///< 全局采样值缓存次数
static uint16_t g_nTimeCacheCount = 0;             ///< 全局报文时间缓存次数



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

// 调试版本所需
#ifdef _DEBUG
Void SetReason (int fileId, int lineNo, int failureCode)
{

}
#endif

/// 初始化DSP设置
void InitConfig()
{
    memset(&g_config, 0, sizeof(Config));
    g_config.nMsgTimeCalcCount = 4680;
    g_config.nASDUCount = 8;
	g_config.nSamplingFreq = 12800;
	g_config.nRatedFreq = 50;    
    
    // 初始化报文离散度映射hash数组
    dispHashCreate(g_config.nASDUCount);
}

/// 初始化缓冲区
void InitBuffers()
{
    // 全局缓冲区清0
    memset(g_resFlag, 0, 8*sizeof(uint8_t));
    g_smvCalcData.nChannelCount=0;
    g_smvCalcData.nSmvPerCircle=0;
    memset(g_smvCalcData.nChannelVCType,0,MAX_SMV_CHANNELS*sizeof(uint8_t));
    memset(g_smvCalcData.fChannelData,0, 
        MAX_SMV_CHANNELS*MAX_SMV_CALC_COUNT*sizeof(float));
    
}


/// 设置DSP参数
void SetConfig()
{
    Error err;
    Config tmpConfig;
	int32_t nSmvCalcCount = 0;
    
    // 拷贝设置
    memcpy(&tmpConfig, g_recvBuf+sizeof(FrameHead), sizeof(Config));

	nSmvCalcCount = tmpConfig.nSamplingFreq/tmpConfig.nRatedFreq*10;

    err.nErrno = 0;                                 //success
    // 只允许这几种固定大小的采样点计算，根据不同的采样频率得来
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
    
    // 没有错误就拷贝参数设置
    if(err.nErrno == 0)
    {
        memcpy(&g_config, &tmpConfig, sizeof(Config));    
    }

    // 设置输出
    memcpy(&g_error[0], &err, sizeof(Error));

}

/// 获取DSP信息
void GetDspInfo()
{
    // 设置DSP版本号输出
    g_dspInfo.nVersion = _DSP_VERSION;

}

/// 清空SMV数据缓存
void ResetSmvCache()
{
    // 不清空缓冲区，仅移动指针到开头
    g_nSmvCacheCount = 0;
    
    // 清空实际值缓冲区数据
	// 实际值不回传
    g_resFlag[RES_SMV_REAL_DATA] = 0;
    g_resLength[RES_SMV_REAL_DATA] = sizeof(g_smvRealData);
    memset(&g_smvRealData, 0, g_resLength[RES_SMV_REAL_DATA]);

    // 清空谐波等计算结果缓冲区数据
    g_resFlag[RES_SMV_CALC_RT] = DSP2ARM_CONTROLCODE_SMV_CALC_RT;
    g_resLength[RES_SMV_CALC_RT] = sizeof(g_smvCalcRT);
    memset(&g_smvCalcRT, 0, g_resLength[RES_SMV_CALC_RT]);
}

/// 清空报文时间缓存
void ResetTimeCache()
{
    g_nTimeCacheCount = 0;
    
	// 重置报文统计数据
	resetDisp();

    // 清空报文离散度结果缓存
    g_resFlag[RES_MSG_DISP] = DSP2ARM_CONTROLCODE_MSG_DISP_RT;
    g_resLength[RES_MSG_DISP] = sizeof(g_pMsgDisp[0])*11;
    memset(g_pMsgDisp, 0, g_resLength[RES_MSG_DISP]);
}

/// 波形生成
void SmvGenHelper()
{
    SmvGenData smvGenData;
    
    memcpy(&smvGenData, g_recvBuf+sizeof(FrameHead), sizeof(SmvGenData));
    
    SmvGenCalc(&smvGenData, &g_smvGenRT);
	
}

/// 报文离散度计算
void MsgDispHelper()
{
    MsgTimeData msgTimeData;
	uint8_t fFirstCalc = 1;
    memcpy(&msgTimeData, g_recvBuf+sizeof(FrameHead), sizeof(MsgTimeData));

	// 乒乓操作，复制数据到缓存
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

    // 达到计算报文离散度条件
    if(g_nTimeCacheCount >= g_config.nMsgTimeCalcCount)
    {
		// 首次运行无需等待计算完成
		if(fFirstCalc)
		{
			fFirstCalc = 0;
		}
		else
		{
#if 1
#ifdef _LOST_POINT_MODE
			// 开启丢点
			g_fDispLostPoint = 1;
			// 释放接收信号量
			SEM_post(&SEM7);
#endif
#endif
			// 等待计算完成
			SEM_pend(&SEM6, SYS_FOREVER);
		}

		if(g_fDispSwap == 0)
		{
			g_fDispSwap = 1;
#if 1
#ifdef _LOST_POINT_MODE
			// 关闭丢点
			g_fDispLostPoint = 0;
#endif
#endif
			// 释放接收信号量
			SEM_post(&SEM7);


			MsgDispCalc(g_msgTime, &g_config, g_pMsgDisp);
			// 指示计算完成
			SEM_post(&SEM6);
		}
      	else
		{
			g_fDispSwap = 0;
#if 1
#ifdef _LOST_POINT_MODE
			// 关闭丢点
			g_fDispLostPoint = 0;
#endif
#endif
			// 释放接收信号量
			SEM_post(&SEM7);

			MsgDispCalc(g_msgTime1, &g_config, g_pMsgDisp);	
			// 指示计算完成
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
/// 实际值计算
void RealSmvHelper()
{
    uint16_t i = 0;
    SmvData smvData;
	int32_t nSmvCalcCount = g_config.nSamplingFreq / g_config.nRatedFreq * 10;
	static uint8_t fFirstCalc = 1;

    // 获得原疾裳凳�
    memcpy(&smvData, g_recvBuf+sizeof(FrameHead), sizeof(SmvData));
    
    // 防止缓冲区溢出
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
        // 根据比例因子计算实际值
        for(i=0; i<smvData.nChannelCount; i++)
        {
            // 乒乓操作，复制从上次缓存点到缓冲区末尾的数据
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
    
    // 缓存次数+=每通道数据数量
    g_nSmvCacheCount = g_nSmvCacheCount + smvData.nSmvPerCircle;

    // 暂时不回传实际值计算结果
    /*g_resFlag[RES_SMV_REAL_DATA] = DSP2ARM_CONTROLCODE_SMV_DATA;
    g_resLength[RES_SMV_REAL_DATA] = sizeof(SmvData);
    SEM_post(&SEM0);*/
	
    // 如果缓存次数达到计算谐波等所需次数则计算谐波等数据
    if(g_nSmvCacheCount >= nSmvCalcCount)
    {
		// 乒乓操作，复制数据
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

		// 如果是首次计算，不等待计算完成
		if(fFirstCalc)
		{
			fFirstCalc = 0;
		}
		else
		{

#ifdef _LOST_POINT_MODE
			// 开启丢点
			g_fSmvLostPoint = 1;
			SEM_post(&SEM7);
#endif			
			// 等待计算完成
			SEM_pend(&SEM5, SYS_FOREVER);						
		}

        g_nSmvCacheCount = 0;

		// 进行谐波计算
		SEM_post(&SEM2);
    }
	else
	{
		// 释放接收信号量
		SEM_post(&SEM7);
	}
}
   
/// 谐波、有效值、功率计算
void SmvHelper()
{
#if _SMV_TIME_TEST
	Uint32 useTime1 = 0, useTime2 = 0;
    int old;
	old = HWI_disable();
    useTime1 = CLK_gethtime();
#endif
	// 乒乓操作，以指定缓冲区作为输入
	if(g_fSmvSwap == 0)
	{
		g_fSmvSwap = 1;
#ifdef _LOST_POINT_MODE
		// 关闭丢点
		g_fSmvLostPoint = 0;
#endif	
		// 释放接收信号量
		SEM_post(&SEM7);

    	SmvDataCalc(&g_smvCalcData, &g_config, &g_smvCalcRT);		
	}
	else
	{
		g_fSmvSwap = 0;
#ifdef _LOST_POINT_MODE
		// 关闭丢点
		g_fSmvLostPoint = 0;
#endif	
		// 释放接收信号量
		SEM_post(&SEM7);

    	SmvDataCalc(&g_smvCalcData1, &g_config, &g_smvCalcRT);		
	}
	// 释放计算完成信号量
	SEM_post(&SEM5);

#if _SMV_TIME_TEST
    HWI_restore( old );
    useTime2 = CLK_gethtime(); 
	g_smvCalcRT.fRmsValue[0] = (useTime2 - useTime1) * CLK_cpuCyclesPerHtime() / GBL_getFrequency();
#endif
}
/*********************************************************************************************************
** Function name:           main
** Descriptions:            主函数，初始化变量、创建dsplink通道
** Input:                   无
** Output:                  无
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
** Descriptions:            接收ARM端发送数据任务
** Input:                   无
** Output:                  无
** Note:                    TSKDSP_execute_Receive在没有接收到数据时处于阻塞状态
*********************************************************************************************************/
void tskRecevieFun()
{
    Int status = SYS_OK; 
    FrameHead *ch;
    while(1)
    {
        status = TSKDSP_execute_Receive(info,&g_recvBuf);   // 收ARM端的
        if (status != SYS_OK){
            SET_FAILURE_REASON(status);
        }
    
		// 重置CPU缓存
		HAL_cacheInv ((Ptr) g_recvBuf, ARM_DSP_BUF_SIZE) ;

        // 接收到的数据在*g_recvBuf中
        ch = (FrameHead *)g_recvBuf;

        // 根据不同的命令码执行不同的操作
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
				// 如果丢点标识开启，则直接丢弃
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
				// 如果丢点模式开启，则直接丢弃
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
** Descriptions:            DSP端发送数据任务，发送运算结果到ARN端
** Input:                   无
** Output:                  无
** Note:                    填充要发送的数据g_sendBuf[SIZE]中，调用TSKDSP_execute_Send函数即可完成发送数据
*********************************************************************************************************/
void tskSendFun()
{
    Int status = SYS_OK;
    FrameHead *sh = (FrameHead *)g_sendBuf;
    Int i=0;
    while(1)
    {
        SEM_pend(&SEM0, SYS_FOREVER);
       
        // 拷贝数据到g_sendBuf中
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

		// 冲洗CPU缓存
		HAL_cacheWbInv ((Ptr)g_sendBuf, ARM_DSP_BUF_SIZE);

        status = TSKDSP_execute_Send(info);
        if (status != SYS_OK){
                SET_FAILURE_REASON(status);
            }

    }    
}

/*********************************************************************************************************
** Function name:           tskCalc40msFun
** Descriptions:            40ms计算任务
** Input:                   无
** Output:                  无
** Note:                    计算波形生成，报文离散度和SMV实际值
*********************************************************************************************************/
void tskCalc40msFun()
{
    while(1)
    {

        SEM_pend(&SEM1, SYS_FOREVER);
    
        /*
         * 计算程序
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
** Descriptions:            200ms计算任务
** Input:                   无
** Output:                  无
** Note:                    计算谐波、间谐波、高次谐波、序量
*********************************************************************************************************/
void tskCalc200msFun()
{

    while(1)
    {
        SEM_pend(&SEM2, SYS_FOREVER);
        /** 计算程序*/
        SmvHelper();
        g_resFlag[RES_SMV_CALC_RT] = DSP2ARM_CONTROLCODE_SMV_CALC_RT;
        g_resLength[RES_SMV_CALC_RT] = sizeof(SMV_DATA_CALC_RT);
        SEM_post(&SEM0);
    }
}

/*********************************************************************************************************
** Function name:           tskCalc5120msFun
** Descriptions:            5120ms计算任务
** Input:                   无
** Output:                  无
** Note:                    5.12s瞬时闪变值计算
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
** Descriptions:            10min计算任务
** Input:                   无
** Output:                  无
** Note:                    10min短闪变值计算，累计12次计算出长闪变值
*********************************************************************************************************/
void tskCalc10minFun()
{
    while(1)
    {
        SEM_pend(&SEM4, SYS_FOREVER);

    }
}
