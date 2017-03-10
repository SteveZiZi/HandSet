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
#include "Clk.h"                                         // ”√”⁄ ±º‰º∆À„
#include "gbl.h"

#ifdef _DEBUG
#define FILEID FID_APP_C
#endif

#define _SMV_TIME_TEST  0								// ≤‚ ‘–≥≤®º∆À„ ±º‰ø™πÿ
#define _DSP_VERSION 	7								// DSP∞Ê±æ∫≈£¨Õ‚≤øœ‘ æŒ™≥˝“‘10∫Ûµƒ∏°µ„÷µ
#define _LOST_POINT_MODE								// DSP∂™µ„ƒ£ Ω£¨»Áπ˚≤ª∂®“Â∏√∫Í£¨‘ÚŒ™◊Ë»˚ƒ£ Ω

extern LOG_Obj trace;

Uint8 *g_recvBuf = NULL;                                 // ÷∏œÚΩ” ’µΩ ˝æ›ª∫¥Ê«¯
Uint8 *g_sendBuf = NULL;                                 // ÷∏œÚ∑¢ÀÕ ˝æ›ª∫¥Ê«¯
Uint16 numTransfers ;
Uint32 xferBufSize ;

TSKLOOP_TransferInfo *info;                              // Ω” ’∑¢ÀÕ–≈œ¢Ω·ππÃÂ

Config g_config = {0};                                   ///< »´æ÷≤Œ ˝…Ë÷√

SmvCalcData g_smvCalcData = {0};                         ///< »´æ÷≤…—˘ ˝æ›ª∫¥Ê
SmvCalcData g_smvCalcData1 = {0};                        ///< »´æ÷≤…—˘ ˝æ›ª∫¥Ê

MSGTIME g_msgTime[MAX_MSG_TIME_CALC_COUNT] = {0};        ///< »´æ÷±®Œƒ ±º‰µ„ª∫¥Ê
MSGTIME g_msgTime1[MAX_MSG_TIME_CALC_COUNT] = {0};       ///< »´æ÷±®Œƒ ±º‰µ„ª∫¥Ê

static uint8_t g_fSmvSwap = 0;                           ///< º∆À„SMVµƒ∆π≈“±Í ∂
static uint8_t g_fDispSwap = 0;                          ///< º∆À„±®Œƒ¿Î…¢∂»µƒ∆π≈“±Í ∂

#ifdef _LOST_POINT_MODE
	uint8_t g_fSmvLostPoint = 0;							///< SMV∂™µ„±Í ∂
	uint8_t g_fDispLostPoint = 0;							///< ±®Œƒ¿Î…¢∂»∂™µ„±Í ∂
#endif

// »´æ÷≤®–Œ…˙≥…ª∫≥Â«¯
Error g_error[8] = {0};
DspInfo g_dspInfo = {0};                                 ///< »´æ÷DSP–≈œ¢

SmvData g_smvRealData = {0};

SMV_GEN_DATA_CALC_RT g_smvGenRT = {0};
float g_pMsgDisp[11] = {0.0f};

SMV_DATA_CALC_RT g_smvCalcRT = {0};

//uint32_t g_smvGenNum;

uint8_t g_tskFlag[8]={0};                                 ///< »´æ÷»ŒŒÒ±Í÷æ

// ªÿ¥´ ˝æ›±Í ∂
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
uint8_t g_resFlag[RES_LAST]={0};                   ///< »´æ÷Ω·π˚±Í÷æ
uint32_t g_resLength[RES_LAST]={0};                ///< »´æ÷Ω·π˚ ˝æ›≥§∂»
// »´æ÷Ω·π˚µƒª∫≥Â«¯÷∏’Î
const void *g_resPtr[RES_LAST]={&g_error[0], &g_dspInfo, &g_smvRealData,
                &g_smvGenRT, g_pMsgDisp, &g_smvCalcRT, NULL};

static uint16_t g_nSmvCacheCount = 0;              ///< »´æ÷≤…—˘÷µª∫¥Ê¥Œ ˝
static uint16_t g_nTimeCacheCount = 0;             ///< »´æ÷±®Œƒ ±º‰ª∫¥Ê¥Œ ˝



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

// µ˜ ‘∞Ê±æÀ˘–Ë
#ifdef _DEBUG
Void SetReason (int fileId, int lineNo, int failureCode)
{

}
#endif

/// ≥ı ºªØDSP…Ë÷√
void InitConfig()
{
    memset(&g_config, 0, sizeof(Config));
    g_config.nMsgTimeCalcCount = 4680;
    g_config.nASDUCount = 8;
	g_config.nSamplingFreq = 12800;
	g_config.nRatedFreq = 50;    
    
    // ≥ı ºªØ±®Œƒ¿Î…¢∂»”≥…‰hash ˝◊È
    dispHashCreate(g_config.nASDUCount);
}

/// ≥ı ºªØª∫≥Â«¯
void InitBuffers()
{
    // »´æ÷ª∫≥Â«¯«Â0
    memset(g_resFlag, 0, 8*sizeof(uint8_t));
    g_smvCalcData.nChannelCount=0;
    g_smvCalcData.nSmvPerCircle=0;
    memset(g_smvCalcData.nChannelVCType,0,MAX_SMV_CHANNELS*sizeof(uint8_t));
    memset(g_smvCalcData.fChannelData,0, 
        MAX_SMV_CHANNELS*MAX_SMV_CALC_COUNT*sizeof(float));
    
}


/// …Ë÷√DSP≤Œ ˝
void SetConfig()
{
    Error err;
    Config tmpConfig;
	int32_t nSmvCalcCount = 0;
    
    // øΩ±¥…Ë÷√
    memcpy(&tmpConfig, g_recvBuf+sizeof(FrameHead), sizeof(Config));

	nSmvCalcCount = tmpConfig.nSamplingFreq/tmpConfig.nRatedFreq*10;

    err.nErrno = 0;                                 //success
    // ÷ª‘ –Ì’‚º∏÷÷πÃ∂®¥Û–°µƒ≤…—˘µ„º∆À„£¨∏˘æ›≤ªÕ¨µƒ≤…—˘∆µ¬ µ√¿¥
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
    
    // √ª”–¥ÌŒÛæÕøΩ±¥≤Œ ˝…Ë÷√
    if(err.nErrno == 0)
    {
        memcpy(&g_config, &tmpConfig, sizeof(Config));    
    }

    // …Ë÷√ ‰≥ˆ
    memcpy(&g_error[0], &err, sizeof(Error));

}

/// ªÒ»°DSP–≈œ¢
void GetDspInfo()
{
    // …Ë÷√DSP∞Ê±æ∫≈ ‰≥ˆ
    g_dspInfo.nVersion = _DSP_VERSION;

}

/// «Âø’SMV ˝æ›ª∫¥Ê
void ResetSmvCache()
{
    // ≤ª«Âø’ª∫≥Â«¯£¨Ωˆ“∆∂Ø÷∏’ÎµΩø™Õ∑
    g_nSmvCacheCount = 0;
    
    // «Âø’ µº ÷µª∫≥Â«¯ ˝æ›
	//  µº ÷µ≤ªªÿ¥´
    g_resFlag[RES_SMV_REAL_DATA] = 0;
    g_resLength[RES_SMV_REAL_DATA] = sizeof(g_smvRealData);
    memset(&g_smvRealData, 0, g_resLength[RES_SMV_REAL_DATA]);

    // «Âø’–≥≤®µ»º∆À„Ω·π˚ª∫≥Â«¯ ˝æ›
    g_resFlag[RES_SMV_CALC_RT] = DSP2ARM_CONTROLCODE_SMV_CALC_RT;
    g_resLength[RES_SMV_CALC_RT] = sizeof(g_smvCalcRT);
    memset(&g_smvCalcRT, 0, g_resLength[RES_SMV_CALC_RT]);
}

/// «Âø’±®Œƒ ±º‰ª∫¥Ê
void ResetTimeCache()
{
    g_nTimeCacheCount = 0;
    
	// ÷ÿ÷√±®ŒƒÕ≥º∆ ˝æ›
	resetDisp();

    // «Âø’±®Œƒ¿Î…¢∂»Ω·π˚ª∫¥Ê
    g_resFlag[RES_MSG_DISP] = DSP2ARM_CONTROLCODE_MSG_DISP_RT;
    g_resLength[RES_MSG_DISP] = sizeof(g_pMsgDisp[0])*11;
    memset(g_pMsgDisp, 0, g_resLength[RES_MSG_DISP]);
}

/// ≤®–Œ…˙≥…
void SmvGenHelper()
{
    SmvGenData smvGenData;
    
    memcpy(&smvGenData, g_recvBuf+sizeof(FrameHead), sizeof(SmvGenData));
    
    SmvGenCalc(&smvGenData, &g_smvGenRT);
	
}

/// ±®Œƒ¿Î…¢∂»º∆À„
void MsgDispHelper()
{
    MsgTimeData msgTimeData;
	uint8_t fFirstCalc = 1;
    memcpy(&msgTimeData, g_recvBuf+sizeof(FrameHead), sizeof(MsgTimeData));

	// ∆π≈“≤Ÿ◊˜£¨∏¥÷∆ ˝æ›µΩª∫¥Ê
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

    // ¥ÔµΩº∆À„±®Œƒ¿Î…¢∂»Ãıº˛
    if(g_nTimeCacheCount >= g_config.nMsgTimeCalcCount)
    {
		//  ◊¥Œ‘À––Œﬁ–Ëµ»¥˝º∆À„ÕÍ≥…
		if(fFirstCalc)
		{
			fFirstCalc = 0;
		}
		else
		{
#if 1
#ifdef _LOST_POINT_MODE
			// ø™∆Ù∂™µ„
			g_fDispLostPoint = 1;
			//  Õ∑≈Ω” ’–≈∫≈¡ø
			SEM_post(&SEM7);
#endif
#endif
			// µ»¥˝º∆À„ÕÍ≥…
			SEM_pend(&SEM6, SYS_FOREVER);
		}

		if(g_fDispSwap == 0)
		{
			g_fDispSwap = 1;
#if 1
#ifdef _LOST_POINT_MODE
			// πÿ±’∂™µ„
			g_fDispLostPoint = 0;
#endif
#endif
			//  Õ∑≈Ω” ’–≈∫≈¡ø
			SEM_post(&SEM7);


			MsgDispCalc(g_msgTime, &g_config, g_pMsgDisp);
			// ÷∏ æº∆À„ÕÍ≥…
			SEM_post(&SEM6);
		}
      	else
		{
			g_fDispSwap = 0;
#if 1
#ifdef _LOST_POINT_MODE
			// πÿ±’∂™µ„
			g_fDispLostPoint = 0;
#endif
#endif
			//  Õ∑≈Ω” ’–≈∫≈¡ø
			SEM_post(&SEM7);

			MsgDispCalc(g_msgTime1, &g_config, g_pMsgDisp);	
			// ÷∏ æº∆À„ÕÍ≥…
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
///  µº ÷µº∆À„
void RealSmvHelper()
{
    uint16_t i = 0;
    SmvData smvData;
	int32_t nSmvCalcCount = g_config.nSamplingFreq / g_config.nRatedFreq * 10;
	static uint8_t fFirstCalc = 1;

    // ªÒµ√‘≠º≤…—˘÷µ ˝æ›
    memcpy(&smvData, g_recvBuf+sizeof(FrameHead), sizeof(SmvData));
    
    // ∑¿÷πª∫≥Â«¯“Á≥ˆ
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
        // ∏˘æ›±»¿˝“Ú◊”º∆À„ µº ÷µ
        for(i=0; i<smvData.nChannelCount; i++)
        {
            // ∆π≈“≤Ÿ◊˜£¨∏¥÷∆¥”…œ¥Œª∫¥Êµ„µΩª∫≥Â«¯ƒ©Œ≤µƒ ˝æ›
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
    
    // ª∫¥Ê¥Œ ˝+=√øÕ®µ¿ ˝æ› ˝¡ø
    g_nSmvCacheCount = g_nSmvCacheCount + smvData.nSmvPerCircle;

    // ‘› ±≤ªªÿ¥´ µº ÷µº∆À„Ω·π˚
    /*g_resFlag[RES_SMV_REAL_DATA] = DSP2ARM_CONTROLCODE_SMV_DATA;
    g_resLength[RES_SMV_REAL_DATA] = sizeof(SmvData);
    SEM_post(&SEM0);*/
	
    // »Áπ˚ª∫¥Ê¥Œ ˝¥ÔµΩº∆À„–≥≤®µ»À˘–Ë¥Œ ˝‘Úº∆À„–≥≤®µ» ˝æ›
    if(g_nSmvCacheCount >= nSmvCalcCount)
    {
		// ∆π≈“≤Ÿ◊˜£¨∏¥÷∆ ˝æ›
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

		// »Áπ˚ « ◊¥Œº∆À„£¨≤ªµ»¥˝º∆À„ÕÍ≥…
		if(fFirstCalc)
		{
			fFirstCalc = 0;
		}
		else
		{

#ifdef _LOST_POINT_MODE
			// ø™∆Ù∂™µ„
			g_fSmvLostPoint = 1;
			SEM_post(&SEM7);
#endif			
			// µ»¥˝º∆À„ÕÍ≥…
			SEM_pend(&SEM5, SYS_FOREVER);						
		}

        g_nSmvCacheCount = 0;

		// Ω¯–––≥≤®º∆À„
		SEM_post(&SEM2);
    }
	else
	{
		//  Õ∑≈Ω” ’–≈∫≈¡ø
		SEM_post(&SEM7);
	}
}
   
/// –≥≤®°¢”––ß÷µ°¢π¶¬ º∆À„
void SmvHelper()
{
#if _SMV_TIME_TEST
	Uint32 useTime1 = 0, useTime2 = 0;
    int old;
	old = HWI_disable();
    useTime1 = CLK_gethtime();
#endif
	// ∆π≈“≤Ÿ◊˜£¨“‘÷∏∂®ª∫≥Â«¯◊˜Œ™ ‰»Î
	if(g_fSmvSwap == 0)
	{
		g_fSmvSwap = 1;
#ifdef _LOST_POINT_MODE
		// πÿ±’∂™µ„
		g_fSmvLostPoint = 0;
#endif	
		//  Õ∑≈Ω” ’–≈∫≈¡ø
		SEM_post(&SEM7);

    	SmvDataCalc(&g_smvCalcData, &g_config, &g_smvCalcRT);		
	}
	else
	{
		g_fSmvSwap = 0;
#ifdef _LOST_POINT_MODE
		// πÿ±’∂™µ„
		g_fSmvLostPoint = 0;
#endif	
		//  Õ∑≈Ω” ’–≈∫≈¡ø
		SEM_post(&SEM7);

    	SmvDataCalc(&g_smvCalcData1, &g_config, &g_smvCalcRT);		
	}
	//  Õ∑≈º∆À„ÕÍ≥…–≈∫≈¡ø
	SEM_post(&SEM5);

#if _SMV_TIME_TEST
    HWI_restore( old );
    useTime2 = CLK_gethtime(); 
	g_smvCalcRT.fRmsValue[0] = (useTime2 - useTime1) * CLK_cpuCyclesPerHtime() / GBL_getFrequency();
#endif
}
/*********************************************************************************************************
** Function name:           main
** Descriptions:            ÷˜∫Ø ˝£¨≥ı ºªØ±‰¡ø°¢¥¥Ω®dsplinkÕ®µ¿
** Input:                   Œﬁ
** Output:                  Œﬁ
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
** Descriptions:            Ω” ’ARM∂À∑¢ÀÕ ˝æ›»ŒŒÒ
** Input:                   Œﬁ
** Output:                  Œﬁ
** Note:                    TSKDSP_execute_Receive‘⁄√ª”–Ω” ’µΩ ˝æ› ±¥¶”⁄◊Ë»˚◊¥Ã¨
*********************************************************************************************************/
void tskRecevieFun()
{
    Int status = SYS_OK; 
    FrameHead *ch;
    while(1)
    {
        status = TSKDSP_execute_Receive(info,&g_recvBuf);   //  ’ARM∂Àµƒ
        if (status != SYS_OK){
            SET_FAILURE_REASON(status);
        }
    
		// ÷ÿ÷√CPUª∫¥Ê
		HAL_cacheInv ((Ptr) g_recvBuf, ARM_DSP_BUF_SIZE) ;

        // Ω” ’µΩµƒ ˝æ›‘⁄*g_recvBuf÷–
        ch = (FrameHead *)g_recvBuf;

        // ∏˘æ›≤ªÕ¨µƒ√¸¡Ó¬Î÷¥––≤ªÕ¨µƒ≤Ÿ◊˜
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
				// »Áπ˚∂™µ„±Í ∂ø™∆Ù£¨‘Ú÷±Ω”∂™∆˙
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
				// »Áπ˚∂™µ„ƒ£ Ωø™∆Ù£¨‘Ú÷±Ω”∂™∆˙
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
** Descriptions:            DSP∂À∑¢ÀÕ ˝æ›»ŒŒÒ£¨∑¢ÀÕ‘ÀÀ„Ω·π˚µΩARN∂À
** Input:                   Œﬁ
** Output:                  Œﬁ
** Note:                    ÃÓ≥‰“™∑¢ÀÕµƒ ˝æ›g_sendBuf[SIZE]÷–£¨µ˜”√TSKDSP_execute_Send∫Ø ˝º¥ø…ÕÍ≥…∑¢ÀÕ ˝æ›
*********************************************************************************************************/
void tskSendFun()
{
    Int status = SYS_OK;
    FrameHead *sh = (FrameHead *)g_sendBuf;
    Int i=0;
    while(1)
    {
        SEM_pend(&SEM0, SYS_FOREVER);
       
        // øΩ±¥ ˝æ›µΩg_sendBuf÷–
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

		// ≥Âœ¥CPUª∫¥Ê
		HAL_cacheWbInv ((Ptr)g_sendBuf, ARM_DSP_BUF_SIZE);

        status = TSKDSP_execute_Send(info);
        if (status != SYS_OK){
                SET_FAILURE_REASON(status);
            }

    }    
}

/*********************************************************************************************************
** Function name:           tskCalc40msFun
** Descriptions:            40msº∆À„»ŒŒÒ
** Input:                   Œﬁ
** Output:                  Œﬁ
** Note:                    º∆À„≤®–Œ…˙≥…£¨±®Œƒ¿Î…¢∂»∫ÕSMV µº ÷µ
*********************************************************************************************************/
void tskCalc40msFun()
{
    while(1)
    {

        SEM_pend(&SEM1, SYS_FOREVER);
    
        /*
         * º∆À„≥Ã–Ú
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
** Descriptions:            200msº∆À„»ŒŒÒ
** Input:                   Œﬁ
** Output:                  Œﬁ
** Note:                    º∆À„–≥≤®°¢º‰–≥≤®°¢∏ﬂ¥Œ–≥≤®°¢–Ú¡ø
*********************************************************************************************************/
void tskCalc200msFun()
{

    while(1)
    {
        SEM_pend(&SEM2, SYS_FOREVER);
        /** º∆À„≥Ã–Ú*/
        SmvHelper();
        g_resFlag[RES_SMV_CALC_RT] = DSP2ARM_CONTROLCODE_SMV_CALC_RT;
        g_resLength[RES_SMV_CALC_RT] = sizeof(SMV_DATA_CALC_RT);
        SEM_post(&SEM0);
    }
}

/*********************************************************************************************************
** Function name:           tskCalc5120msFun
** Descriptions:            5120msº∆À„»ŒŒÒ
** Input:                   Œﬁ
** Output:                  Œﬁ
** Note:                    5.12sÀ≤ ±…¡±‰÷µº∆À„
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
** Descriptions:            10minº∆À„»ŒŒÒ
** Input:                   Œﬁ
** Output:                  Œﬁ
** Note:                    10min∂Ã…¡±‰÷µº∆À„£¨¿€º∆12¥Œº∆À„≥ˆ≥§…¡±‰÷µ
*********************************************************************************************************/
void tskCalc10minFun()
{
    while(1)
    {
        SEM_pend(&SEM4, SYS_FOREVER);

    }
}
