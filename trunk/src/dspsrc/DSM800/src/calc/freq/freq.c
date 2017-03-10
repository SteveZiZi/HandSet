/// @file
///     freq.c
/// @brief
///     Ƶ�ʼ���    
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/22
/// @revise
/// 
/*******************************************************************************
�ź�Ƶ�ʼ����㷨˵����
    ���㷨ͨ������źŹ��������������ź�Ƶ�ʣ���ʱ��˳����ԭ�źŲ����㣬����
  ����N��������Ҳ����������N/2�����ڣ���ͨ��f=1/(t/5)=5/t�����źŵ��ź�Ƶ�ʣ���
  t=1/fs*N=N/fs��NΪ5�����ڵĲ��������������õ��ź�Ƶ��Ϊf=5*fs/N
*******************************************************************************/
#include "freq.h"

static Int16 checkPeriod(Uint16 sampleNumDif, Uint16 sampleFreq, Uint16 lowFreq,
                  Uint16 highFreq);
///
/// @brief
///    �ź�Ƶ�ʼ���
/// @param[in] 
///    sampleNum            ��������
///    sampleFreq           ����Ƶ��
///    periodCount          ������������ΪС����
/// @param[out]
///    pFreq                �ź�Ƶ��
/// @return
///    Int16                0���ɹ���-����������̫�٣�С��MIN_SAMPLE_NUM
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
///    ����������
/// @param[in] 
///    sampleNum                    ��������
///    sampleFreq                   ����Ƶ��
///    pSampleValues                ������ֵ
///    minFreq                      ���Ƶ��
///    maxFreq                      ���Ƶ��
/// @param[out]
///    periodNum                    ������Ŀ��ע�����С�������ڣ�
///    validSampleNum               ��Ч�Ĳ���ʱ��
///    difTime                      �������µ�ʱ�����
/// @return
///    Uint16                       0���ɹ�
///                                -1��������Ч�����ݣ��������֮���ʱ�䲻����Ч��Χ�ڣ�
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
            // ���֮ǰ���й����
            if(nLastIndex > -1)          
            {
                // ������������֮���ʱ������Ч��Χ��
                if(checkPeriod(i-nLastIndex, sampleFreq, minFreq, maxFreq)==0)
                {
                    nValidSampleCount += i-nLastIndex;
                    fDifTime += fabs(pSampleValues[nLastIndex]
                    /(pSampleValues[nLastIndex]-pSampleValues[nLastIndex-1]))
                        /(float)sampleFreq;
                }
                else
                {
                    // ��Ч��ȥ��ͳ�Ƶ���Ч����
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
                    // ������������������ղ�ֵ����
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
///    ������������ļ��Ƶ���Ƿ�����Ч��Χ֮��
/// @param[in] 
///    sampleNumDif                 ���������֮��Ĳ�������
///    sampleFreq                   ����Ƶ��
///    lowFreq                      ���Ƶ��
///    highFreq                     ���Ƶ��
/// @return
///    Uint16                       0����Ч
///                                 -1����Ч
///
Int16 checkPeriod(Uint16 sampleNumDif, Uint16 sampleFreq, Uint16 lowFreq,
                  Uint16 highFreq)
{
    // ��������
    float fPeriod = (float)sampleNumDif/(float)sampleFreq;
    float fMaxPeroid = 1.0/(float)lowFreq/2;
    float fMinPeroid = 1.0/(float)highFreq/2;
    if(fPeriod<fMinPeroid || fPeriod>fMaxPeroid)
    {
        return -1;
    }
    return 0;
}
