#include "dspCalc.h"
#include "calc/calc.h"
#include <string.h>

/// 波形生成
void SmvGenCalc(const SmvGenData *pSmvGenData, SMV_GEN_DATA_CALC_RT *pSmvGenRT)
{
    const SmvGenData *sgd = pSmvGenData;
    int16_t i = 0;
    int16_t j = 0;
    float   fWidthValue[MAX_HARM_COUNT];
    memset(fWidthValue, 0, MAX_HARM_COUNT*sizeof(float)); 
    
    // 波形计算
    for(i=0; i<sgd->nChannelCount; i++)
    {
        for(j=0; j<sgd->nHarmCount[i]; j++)
        {
            fWidthValue[j]=mpysp_i(sgd->fRmsValue[i][j],1.4142135623730950);
        }

        sinSamplesCreate(pSmvGenRT->fSmvGenData[i], sgd->nSmvCount, 
            sgd->nSampleRate, sgd->fDCValue[i], sgd->fFreq[i], sgd->fPhaseAngle[i], 
            fWidthValue, sgd->nHarmCount[i]);   
            
        for(j=0; j<sgd->nSmvCount; j++)
        {
            pSmvGenRT->fSmvGenData[i][j]=mpysp_i(pSmvGenRT->fSmvGenData[i][j], 
            	sgd->fScaleFactor[i]);
        }
    }
    
    // 设置输出帧头并回传数据
    pSmvGenRT->nChannelCount = sgd->nChannelCount;
    pSmvGenRT->nSmvCount = sgd->nSmvCount;
}


/// 实际值计算
void RealSmvCalc(const SmvData *pSmvRawData, SmvData *pSmvRealData)
{
    //FrameHead *sh;
    uint16_t i = 0;
    uint16_t j = 0; 
    
    pSmvRealData->nChannelCount = pSmvRawData->nChannelCount;
    pSmvRealData->nSmvPerCircle = pSmvRawData->nSmvPerCircle;
    memcpy(pSmvRealData->nChannelVCType, pSmvRawData->nChannelVCType, 
            pSmvRawData->nChannelCount*sizeof(pSmvRawData->nChannelVCType[0]));
    
    for(i=0; i<pSmvRawData->nChannelCount; i++)
    {
        // 计算实际值
        for(j=0; j<pSmvRawData->nSmvPerCircle; j++)
        {       
            pSmvRealData->fChannelData[i][j] = 
                mpysp_i(pSmvRawData->fChannelData[i][j], pSmvRawData->fScaleFactor[i]);
        }
    } 
}
 
/// 谐波、有效值、功率计算
void SmvDataCalc(const SmvCalcData *pSmvCalcData, const Config *pConfig, SMV_DATA_CALC_RT *pSmvCalcRT)
{
    //FrameHead *sh;
    SMV_DATA_CALC_RT *rh = pSmvCalcRT;
    int16_t i,j;
    int8_t nPhaseIndex;
    int8_t nCVIndex;
    float pSeqHarmContents[2][3] = {0};           ///< 计算序量向量大小临时数组
    float pSeqHarmAngles[2][3] = {0};             ///< 计算序量向量角临时数组
    
    float pPowerEffValues[3][2] = {0};            ///< 计算功率有效值临时数组
    float pPowerPhaseAngles[3][2] = {0};          ///< 计算功率相位角临时数组
    
    float pPowerTmpDatas[3] = {0};                ///< 计算功率临时数组
    float pPowerTmpFactors[2] = {0};              ///< 计算功率因子临时数组

    float pNuclearPhaseContents[6] = {0};         ///< 计算核相向量大小临时数组
    float pNuclearPhaseAngles[6] = {0};           ///< 计算核相向量角临时数组

    HARM_TYPE harmType = HARM_2560;
    int nHarmCount = 50;
    int nFilterParamIndex = 0;                    ///< 滤波器参数数组二维索引    
    static int nZeroPointCalcCount = 0;           ///< 统计过零点计算次数

    // 频率计算辅助缓冲区
    static float fFilteredChannelData[MAX_SMV_CALC_COUNT] = {0};         ///< 存放滤波后的临时数据
    static float fPeriodCounts[MAX_SMV_CHANNELS] = {0};     		 ///< 各通道过零点个数    
    static uint32_t nValidSampleCounts[MAX_SMV_CHANNELS] = {0};          ///< 各通道有效的采样个数    
    static float fDifTimes[MAX_SMV_CHANNELS] = {0};                      ///< 统计过零点时得到的总时间差值    
    Uint16 nValidSampleNum = 0;                ///< 每次统计的有效样点数
    float fPeriodNum = 0;                      ///< 周期个数        
    float fDifTime = 0;                        ///< 每次统计的总时间差值

	int32_t nSmvCalcCount = pConfig->nSamplingFreq / pConfig->nRatedFreq * 10;
    
    switch(nSmvCalcCount)
    {
        case 800:
        {
            harmType = HARM_800;
            nHarmCount = 39;
            nFilterParamIndex = 0;
        }
        break;
        case 1000:
        {
            harmType = HARM_1000;
            nHarmCount = 49;
            nFilterParamIndex = 1;
        }
        break;
        case 1600:
        {
            harmType = HARM_1600;
            nHarmCount = 50;
            nFilterParamIndex = 2;
        }
        break;
        case 2000:
        {
            harmType = HARM_2000;
            nHarmCount = 50;
            nFilterParamIndex = 3;
        }
        break;
        case 2560:
        {
            harmType = HARM_2560;
            nHarmCount = 50;
            nFilterParamIndex = 4;
        }
        break;
    }

    // 过零点计算次数+1
    nZeroPointCalcCount++;

    for(i=0; i<pSmvCalcData->nChannelCount; i++)
    {
        // 优先采用2路谐波计算算法优化效率
        if(i%2 ==0)
        {
            // 如果2路都有数据
            if(i+1 < pSmvCalcData->nChannelCount)
            {
                // 计算谐波
                harmCalc2(harmType, nSmvCalcCount, 
                    pSmvCalcData->fChannelData[i],pSmvCalcData->fChannelData[i+1],
                    nHarmCount, 
                    &rh->fDCValue[i], &rh->fDCValue[i+1], 
                    rh->fHarmContent[i], rh->fHarmContent[i+1], 
                    rh->fHarmAngle[i], rh->fHarmAngle[i+1]);
            }
            else
            {
                // 计算谐波
                harmCalc(harmType, nSmvCalcCount, pSmvCalcData->fChannelData[i],
                    nHarmCount, &rh->fDCValue[i], rh->fHarmContent[i], rh->fHarmAngle[i]);
            }
        }

		// 计算直流含有率
        dcTHDCalc(rh->fDCValue[i], rh->fHarmContent[i][0],
        	pSmvCalcData->nChannelVCType[i] & 0x0F, &rh->fDCTHD[i]);

        harmTHDCalc(harmType, nHarmCount, rh->fHarmContent[i], 
            pSmvCalcData->nChannelVCType[i] & 0x0F, rh->fHarmTHD[i]);

        harmSumTHDCalc(harmType, nHarmCount, rh->fHarmContent[i], 
            pSmvCalcData->nChannelVCType[i] & 0x0F, &rh->fSumHarmTHD[i]);

        // 计算有效值
        effValueCalc(pSmvCalcData->fChannelData[i], 
            nSmvCalcCount, 
            &rh->fRmsValue[i]);

        nPhaseIndex = ((pSmvCalcData->nChannelVCType[i] & 0xF0) >> 4) - 0x0A;
        nCVIndex = (pSmvCalcData->nChannelVCType[i] & 0x0F) -1;

        // 如果设置了三相
        if((pSmvCalcData->nChannelVCType[i] & 0xF0) > 0 && nPhaseIndex >= 0)
        {
            // 防止数组越界
            if(nPhaseIndex >= 0 && nPhaseIndex < 6)
            {
                if(nPhaseIndex >= 0 && nPhaseIndex < 3 && nCVIndex >=0  && nCVIndex <2)
                {
                    // 得到序量通道数据 
                    pSeqHarmContents[nCVIndex][nPhaseIndex] = rh->fHarmContent[i][0];
                    pSeqHarmAngles[nCVIndex][nPhaseIndex] = rh->fHarmAngle[i][0];

                    // 得到功率通道数据
                    pPowerEffValues[nPhaseIndex][nCVIndex] = rh->fRmsValue[i];
                    pPowerPhaseAngles[nPhaseIndex][nCVIndex] = rh->fHarmAngle[i][0];
                }
                // 得到核相通道数据
                pNuclearPhaseContents[nPhaseIndex] = rh->fHarmContent[i][0];
                pNuclearPhaseAngles[nPhaseIndex] = rh->fHarmAngle[i][0];
            }
            // 只在核相时计算A相和a相的频率
            if(nPhaseIndex == 0 || nPhaseIndex == 3)
            {
				#if 0
    			iir_6th_filter(pSmvCalcData->fChannelData[i], a40_70Hz[nFilterParamIndex],
    				b40_70Hz[nFilterParamIndex], g40_70Hz[nFilterParamIndex], 
    				&iir_state, nSmvCalcCount, fFilteredChannelData);
                // 统计各通道过零点个数
                peroidNumCalc(nSmvCalcCount, pConfig->nSamplingFreq, 
                    fFilteredChannelData,40,70, &fPeriodNum, &nValidSampleNum,
                    &fDifTime);
				#else
                // 统计各通道过零点个数
                peroidNumCalc(nSmvCalcCount, pConfig->nSamplingFreq, 
                    pSmvCalcData->fChannelData[i],40,70, &fPeriodNum, &nValidSampleNum,
                    &fDifTime);
				#endif
                fPeriodCounts[i] += fPeriodNum;
                nValidSampleCounts[i] += nValidSampleNum;
                fDifTimes[i] += fDifTime;

                // 统计过零点
                if(nZeroPointCalcCount == FREQ_CALC_PERIOD_NUM)
                {            
                    // 计算频率                    
                    freqCalc(nValidSampleCounts[i], pConfig->nSamplingFreq,
                         fPeriodCounts[i], fDifTimes[i], &rh->fFreq[i]);
                    fPeriodCounts[i]=0;
                    nValidSampleCounts[i]=0;
                    fDifTimes[i] = 0;
                }
            }                                
        }        
    }
    
    // 计算频率之后将过零点计算次数清零
    if(nZeroPointCalcCount == FREQ_CALC_PERIOD_NUM)
    {
        nZeroPointCalcCount = 0;
    }

    // 计算序量
	// 以A相为准，设A相的角度为0，其他相相角为与A相的差值
	for(i=0; i<2; i++)
	{
		pSeqHarmAngles[i][2] = pSeqHarmAngles[i][2]-pSeqHarmAngles[i][0];
        pSeqHarmAngles[i][1] = pSeqHarmAngles[i][1]-pSeqHarmAngles[i][0];
        pSeqHarmAngles[i][0] = 0;
		phaseSeqCalc(pSeqHarmContents[i], pSeqHarmAngles[i], 
        rh->fPhaseSeqContent[i], rh->fPhaseSeqAngle[i], &rh->fUnbalanceRate[i]);
	}

    // 计算功率
    for(i=0; i<3; i++)
    {
        powerCalc(pPowerEffValues[i], pPowerPhaseAngles[i], pPowerTmpDatas,
                  pPowerTmpFactors);
        rh->fApparentPower[i]=pPowerTmpDatas[0];
        rh->fActivePower[i]=pPowerTmpDatas[1];
        rh->fReactivePower[i]=pPowerTmpDatas[2];
        rh->fPowerFactor[i]=pPowerTmpFactors[0];
    }

    // 填充输出数据
    rh->fTotalActivePower = rh->fActivePower[0] + rh->fActivePower[1] 
        + rh->fActivePower[2];
    rh->fTotalReactivePower = rh->fReactivePower[0] + rh->fReactivePower[1] 
        + rh->fReactivePower[2];
    
    // 计算总视在功率
    rh->fTotalApparentPower = rh->fTotalActivePower * rh->fTotalActivePower;
    rh->fTotalApparentPower += rh->fTotalReactivePower * rh->fTotalReactivePower;
    rh->fTotalApparentPower = sqrtf(rh->fTotalApparentPower);
    
    if(rh->fTotalApparentPower!=0)
    {
        rh->fTotalPowerFactor=rh->fTotalActivePower/rh->fTotalApparentPower;
    }
    else
    {
        rh->fTotalPowerFactor = 0;
    }

    // 计算核相
	// 以A相为准，设A相的角度为0，其他相相角为与A相的差值
	for(i=5; i>=0; i--)
	{
	    pNuclearPhaseAngles[i] = pNuclearPhaseAngles[i] - pNuclearPhaseAngles[0];
	}
    for(j=0; j<3; j++)
    {
        // 同相核相
        rh->fSPAngleDif[j] = phaseCheckPhaseCalc(pNuclearPhaseAngles[j], 
            pNuclearPhaseAngles[j+3]);
        phaseCheckVectorCalc(pNuclearPhaseContents[j], 
            pNuclearPhaseAngles[j],
            pNuclearPhaseContents[j+3],
            pNuclearPhaseAngles[j+3],
            &rh->fSPVectorDifContent[j],
            &rh->fSPVectorDifAngle[j]);
            
        // 相间核相
        phaseCheckVectorCalc(pNuclearPhaseContents[j], 
            pNuclearPhaseAngles[j],
            pNuclearPhaseContents[(j+1)%3],
            pNuclearPhaseAngles[(j+1)%3],
            &rh->fIPVectorDifContent[j],
            &rh->fIPVectorDifAngle[j]);
        phaseCheckVectorCalc(pNuclearPhaseContents[j+3], 
            pNuclearPhaseAngles[j+3],
            pNuclearPhaseContents[(j+1)%3+3],
            pNuclearPhaseAngles[(j+1)%3+3],
            &rh->fIPVectorDifContent[j+3],
            &rh->fIPVectorDifAngle[j+3]);
    }

    memcpy(rh->nChannelVCType, pSmvCalcData->nChannelVCType, 
        pSmvCalcData->nChannelCount*sizeof(uint8_t));
    rh->nChannelCount = pSmvCalcData->nChannelCount;
}


/// 报文离散度计算
void MsgDispCalc(const MSGTIME pMsgTimes[], const Config *pConfig, float *pMsgDisp)
{
    int16_t i = 0;
    int32_t nTmp;
    int32_t msgTimeDifs[MAX_MSG_TIME_CALC_COUNT] = {0};   ///< 报文时间差
    
    for(i = 0; i<pConfig->nMsgTimeCalcCount-1; i++)
    {
        // 计算时间差
        nTmp = pMsgTimes[i+1].nSecond - pMsgTimes[i].nSecond;
        
        // 防止溢出
        if(nTmp>2146)                               // (2^31-1)/1000000-1
        {
            msgTimeDifs[i] = 2146000000;            // ((2^31-1)/1000000-1)*100000
        }
        else if(nTmp<-2146)
        {
            msgTimeDifs[i] = -2146000000;           // -((2^31-1)/1000000-1)*100000
        }
        else
        {
            msgTimeDifs[i] = pMsgTimes[i+1].nMicrosecond - 
                pMsgTimes[i].nMicrosecond + 1000000*nTmp;
        }
    }

    // 报文离散度计算

    dispRateCalc(msgTimeDifs, pConfig->nMsgTimeCalcCount-1,
                pConfig->nSamplingFreq, pConfig->nASDUCount,
                pMsgDisp);
}
