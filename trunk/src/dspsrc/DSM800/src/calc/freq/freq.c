/// @file
///     freq.c
/// @brief
///     频率计算    
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/22
/// @revise
/// 
/*******************************************************************************
信号频率计算算法说明：
    本算法通过检测信号过零点个数来计算信号频率，以时间顺序检测原信号采样点，当发
  现有N个过零点后（也就是完整的N/2个周期），通过f=1/(t/5)=5/t计算信号的信号频率，而
  t=1/fs*N=N/fs（N为5个周期的采样点个数），则得到信号频率为f=5*fs/N
*******************************************************************************/
#include "freq.h"

static Int16 checkPeriod(Uint16 sampleNumDif, Uint16 sampleFreq, Uint16 lowFreq,
                  Uint16 highFreq);
///
/// @brief
///    信号频率计算
/// @param[in] 
///    sampleNum            采样点数
///    sampleFreq           采样频率
///    periodCount          周期数（可以为小数）
/// @param[out]
///    pFreq                信号频率
/// @return
///    Int16                0：成功，-样点数据量太少，小于MIN_SAMPLE_NUM
///
Int16 freqCalc(Uint16 sampleNum, Uint16 sampleFreq, float periodCount, float difTime,
               float *pFreq)
{
    if(sampleNum < MIN_SAMPLE_NUM)
    {
        *pFreq = 0;
        return -1;
    }    
    *pFreq = periodCount/((float)sampleNum/(float)sampleFreq+difTime);
    //*pFreq = periodCount*sampleFreq/sampleNum;
    return 0;
}

///
/// @brief
///    周期数计算
/// @param[in] 
///    sampleNum                    采样点数
///    sampleFreq                   采样频率
///    pSampleValues                采样点值
///    minFreq                      最低频率
///    maxFreq                      最高频率
/// @param[out]
///    periodNum                    周期数目（注意可能小数个周期）
///    validSampleNum               有效的采样时间
///    difTime                      采样导致的时间误差
/// @return
///    Uint16                       0：成功
///                                -1：存在无效的数据（两过零点之间的时间不在有效范围内）
///
Int16 peroidNumCalc(Uint16 sampleNum, Uint16 sampleFreq, const float *pSampleValues, 
                    float minFreq, float maxFreq, 
                    float *periodNum, Uint16 *validSampleNum, float *difTime)
{
    Uint16 i = 0;
    Int16 nLastIndex = -1;
    Uint16 nZeroPointCount = 0;
    Uint16 nValidSampleCount = 0;
    float fDifTime = 0;
    Int16 ret = 0;    
    if(pSampleValues[0]==0)
    {
        nZeroPointCount++;
        nLastIndex = 0;
    }
    for(i = 1; i<sampleNum; i++)
    {
        if(pSampleValues[i]==0 && pSampleValues[i-1]!=0)
        {
            nZeroPointCount++;
            // 如果之前已有过零点
            if(nLastIndex > -1)          
            {
                // 如果两个过零点之间的时间在有效范围内
                if(checkPeriod(i-nLastIndex, sampleFreq, minFreq, maxFreq)==0)
                {
                    nValidSampleCount += i-nLastIndex;
                    fDifTime += fabs(pSampleValues[nLastIndex]
                    /(pSampleValues[nLastIndex]-pSampleValues[nLastIndex-1]))
                        /(float)sampleFreq;
                }
                else
                {
                    // 无效就去除统计的无效数据
                    nZeroPointCount--;
                    ret = -1;
                }                
            }
            nLastIndex = i;
        }
        else if(mpysp_i(pSampleValues[i],pSampleValues[i-1])<0)
        {
            nZeroPointCount++;
            if(nLastIndex > -1)
            {
                if(checkPeriod(i-nLastIndex, sampleFreq, minFreq, maxFreq)==0)
                {
                    nValidSampleCount+= i-nLastIndex;
                    // 计算采样误差（用拉格朗日差值法）
                    fDifTime -= fabs(pSampleValues[i]/(pSampleValues[i]-pSampleValues[i-1]))
                        /(float)sampleFreq;
                    fDifTime += fabs(pSampleValues[nLastIndex]
                    /(pSampleValues[nLastIndex]-pSampleValues[nLastIndex-1]))
                        /(float)sampleFreq;
                }
                else
                {
                    nZeroPointCount--;
                    ret = -1;
                }                
            }
            nLastIndex = i;
        }
    }     
    if(nZeroPointCount == 0)
    {
        *periodNum = 0;
    }
    else
    {
        *periodNum = (float)(nZeroPointCount-1)/2.0;
    }    
    *validSampleNum = nValidSampleCount;
    *difTime = fDifTime;
    return ret;   
}
///
/// @brief
///    检查两个过零点的间隔频率是否在有效范围之类
/// @param[in] 
///    sampleNumDif                 两个过零点之间的采样点数
///    sampleFreq                   采样频率
///    lowFreq                      最低频率
///    highFreq                     最高频率
/// @return
///    Uint16                       0：有效
///                                 -1：无效
///
Int16 checkPeriod(Uint16 sampleNumDif, Uint16 sampleFreq, Uint16 lowFreq,
                  Uint16 highFreq)
{
    // 计算周期
    float fPeriod = (float)sampleNumDif/(float)sampleFreq;
    float fMaxPeroid = 1.0/(float)lowFreq/2;
    float fMinPeroid = 1.0/(float)highFreq/2;
    if(fPeriod<fMinPeroid || fPeriod>fMaxPeroid)
    {
        return -1;
    }
    return 0;
}
