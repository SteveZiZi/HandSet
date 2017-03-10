/// @file
///     sampleCreate.cpp
/// @brief
///     正弦波生成    
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/22
/// @revise
/// 
/*******************************************************************************
波形生成算法说明：
    本算法可以方便的生成一组波形采样值以方便进行数据模拟，输入相应的参数就能得到
一个指定大小的采样值数组，包括以特定频率对正弦波进行的一组采样，当采样点个数=采
样频率时，即输出一个完整周期的采样点，否则输出采样点个数/采样频率个周期的采样点。
    正弦波/余弦波生成函数用于生成一个标准的正弦/余弦波形。
    正弦谐波/余弦谐波波形生成函数用于输入一组值，包括直流分量和各次谐波的角频率，
初相和幅度来确定一个信号函数，例如采用正弦谐波波形生成函数的额输入为：
    采样频率：4000Hz，直流分量：30, 基波：50Hz，2/3*PI，20，三次谐波：150Hz，
-1/2*PI，50，合成的波形函数为：
    f=30+20*sin(2*PI*50*t/4000+2/3*PI)+50*sin(2*PI*150*t/4000-1/2*PI)
    然后根据该波形函数生成一定数目的采样点值并输出。
*******************************************************************************/
#include "sampleCreate.h"
///
/// @brief
///    正弦波生成函数
/// @param[out] 
///    pSamples             生成的正弦波采样点值
/// @param[in]
///    sampleSize           生成的正弦波采样点个数
///    sampleFreq           采样频率
///    omiga                角频率
///    initPhaseAngle       初相相位角
///    width                正弦波幅度
/// @return
///    void
///
void sinSampleCreate(float *pSamples, 
                     Uint16 sampleSize, 
                     Uint16 sampleFreq, 
                     float omiga, 
                     float initPhaseAngle, 
                     float width)
{
    Uint16 i=0;
    float initPhase = 0;
    for(i=0; i<sampleSize; i++)
    {
        initPhase = mpysp_i(initPhaseAngle, ANGLE2ARC_FACTOR);
        pSamples[i] = divsp_i(mpysp_i(PI2,mpysp_i(omiga,i)),sampleFreq);
        pSamples[i] = addsp_i(pSamples[i],initPhase);
        pSamples[i] = mpysp_i(sin(pSamples[i]),width);
        //samples[i]=(float)(width*sin(2*PI*omiga*i/sampleFreq+initPhase));
    }
}
///
/// @brief
///    余弦波生成函数
/// @param[out] 
///    pSamples             生成的余弦波采样点值
/// @param[in]
///    sampleSize           生成的余弦波采样点个数
///    sampleFreq           采样频率
///    omiga                角频率
///    initPhaseAngle       初相相位角
///    width                正弦波幅度
/// @return
///    void
///
void cosSampleCreate(float *pSamples, 
                     Uint16 sampleSize, 
                     Uint16 sampleFreq, 
                     float omiga, 
                     float initPhaseAngle, 
                     float width)
{
    Uint16 i=0;
    float initPhase= 0;
    for(i=0; i<sampleSize; i++)
    {
        initPhase= mpysp_i(initPhaseAngle, ANGLE2ARC_FACTOR);
        pSamples[i] = divsp_i(mpysp_i(PI2,mpysp_i(omiga,i)),sampleFreq);
        pSamples[i] = addsp_i(pSamples[i],initPhase);
        pSamples[i] = mpysp_i(cos(pSamples[i]),width);        
        //samples[i]=(float)(width*cos(2*PI*omiga*i/sampleFreq+initPhase));
    }
}
///
/// @brief
///    正弦谐波波形生成函数
/// @param[out] 
///    pSamples             生成的波形采样点值
/// @param[in]
///    sampleSize           生成的波形采样点个数
///    sampleFreq           采样频率
///    dcValue              直流值
///    pOmigas              角频率数组（对应各次谐波）
///    pInitPhaseAngles     初相相位角数组（对应各次谐波）
///    pWidths              幅值数组（对应各次谐波）
///    harmNum              谐波数量
/// @return
///    void
///
void sinSamplesCreate(float *pSamples,
                  Uint16 sampleSize,
                  Uint16 sampleFreq,
                  float dcValue,
                  const float pOmigas[],
                  const float pInitPhaseAngles[],
                  const float pWidths[],
                  Uint16 harmNum
                  )
{
    Uint16 i = 0;
    Uint16 j = 0;
    float sample = 0;
    float initPhase = 0;
    for(i = 0; i < sampleSize; i++)
    {
        pSamples[i] = dcValue;
        for(j = 0; j < harmNum; j++)
        {
            initPhase = mpysp_i(pInitPhaseAngles[j], ANGLE2ARC_FACTOR);
			// 经测试，dsp上addsp_i(0,12)=14!
			if(i==0)
			{
				sample = initPhase;
			}
			else
			{
            	sample = divsp_i(mpysp_i(PI2,mpysp_i(pOmigas[j],i)),sampleFreq);
            	sample = addsp_i(sample,initPhase);
			}
            sample = mpysp_i(sin(sample),pWidths[j]);
            pSamples[i] = addsp_i(pSamples[i],sample);
            //samples[i]+=(float)(pWidth[j]*sin(2*PI*pOmiga[j]*i/sampleFreq+
            //pInitPhase[j]));
        }
    }
}
///
/// @brief
///    余弦谐波波形生成函数
/// @param[out] 
///    samples              生成的波形采样点值
/// @param[in]
///    sampleSize           生成的波形采样点个数
///    sampleFreq           采样频率
///    dcValue              直流值
///    pOmigas              角频率数组（对应各次谐波）
///    pInitPhaseAngles     初相相位角数组（对应各次谐波）
///    pWidths              幅值数组（对应各次谐波）
///    harmNum              谐波数量
/// @return
///    void
///
void cosSamplesCreate(float *pSamples,
                      Uint16 sampleSize,
                      Uint16 sampleFreq,
                      float dcValue,
                      const float pOmigas[],
                      const float pInitPhaseAngles[],
                      const float pWidths[],
                      Uint16 harmNum
                      )
{
    Uint16 i = 0;
    Uint16 j = 0;
    float sample = 0;
    float initPhase = 0;
    for(i = 0; i < sampleSize; i++)
    {
        pSamples[i] = dcValue;
        for(j = 0; j < harmNum; j++)
        {
            initPhase = mpysp_i(pInitPhaseAngles[j], ANGLE2ARC_FACTOR);
            sample = divsp_i(mpysp_i(PI2,mpysp_i(pOmigas[j],i)),sampleFreq);
            sample = addsp_i(sample,initPhase);
            sample = mpysp_i(cos(sample),pWidths[j]);
            pSamples[i] = addsp_i(pSamples[i],sample);
            //samples[i]=addsp_i(samples[i],(float)(pWidth[j]*cos(2*PI*pOmiga[j]
            //*i/sampleFreq+pInitPhase[j])));
        }
    }
}
