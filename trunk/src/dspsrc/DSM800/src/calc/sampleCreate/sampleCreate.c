/// @file
///     sampleCreate.cpp
/// @brief
///     ���Ҳ�����    
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/22
/// @revise
/// 
/*******************************************************************************
���������㷨˵����
    ���㷨���Է��������һ�鲨�β���ֵ�Է����������ģ�⣬������Ӧ�Ĳ������ܵõ�
һ��ָ����С�Ĳ���ֵ���飬�������ض�Ƶ�ʶ����Ҳ����е�һ������������������=��
��Ƶ��ʱ�������һ���������ڵĲ����㣬����������������/����Ƶ�ʸ����ڵĲ����㡣
    ���Ҳ�/���Ҳ����ɺ�����������һ����׼������/���Ҳ��Ρ�
    ����г��/����г���������ɺ�����������һ��ֵ������ֱ�������͸���г���Ľ�Ƶ�ʣ�
����ͷ�����ȷ��һ���źź����������������г���������ɺ����Ķ�����Ϊ��
    ����Ƶ�ʣ�4000Hz��ֱ��������30, ������50Hz��2/3*PI��20������г����150Hz��
-1/2*PI��50���ϳɵĲ��κ���Ϊ��
    f=30+20*sin(2*PI*50*t/4000+2/3*PI)+50*sin(2*PI*150*t/4000-1/2*PI)
    Ȼ����ݸò��κ�������һ����Ŀ�Ĳ�����ֵ�������
*******************************************************************************/
#include "sampleCreate.h"
///
/// @brief
///    ���Ҳ����ɺ���
/// @param[out] 
///    pSamples             ���ɵ����Ҳ�������ֵ
/// @param[in]
///    sampleSize           ���ɵ����Ҳ����������
///    sampleFreq           ����Ƶ��
///    omiga                ��Ƶ��
///    initPhaseAngle       ������λ��
///    width                ���Ҳ�����
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
///    ���Ҳ����ɺ���
/// @param[out] 
///    pSamples             ���ɵ����Ҳ�������ֵ
/// @param[in]
///    sampleSize           ���ɵ����Ҳ����������
///    sampleFreq           ����Ƶ��
///    omiga                ��Ƶ��
///    initPhaseAngle       ������λ��
///    width                ���Ҳ�����
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
///    ����г���������ɺ���
/// @param[out] 
///    pSamples             ���ɵĲ��β�����ֵ
/// @param[in]
///    sampleSize           ���ɵĲ��β��������
///    sampleFreq           ����Ƶ��
///    dcValue              ֱ��ֵ
///    pOmigas              ��Ƶ�����飨��Ӧ����г����
///    pInitPhaseAngles     ������λ�����飨��Ӧ����г����
///    pWidths              ��ֵ���飨��Ӧ����г����
///    harmNum              г������
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
			// �����ԣ�dsp��addsp_i(0,12)=14!
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
///    ����г���������ɺ���
/// @param[out] 
///    samples              ���ɵĲ��β�����ֵ
/// @param[in]
///    sampleSize           ���ɵĲ��β��������
///    sampleFreq           ����Ƶ��
///    dcValue              ֱ��ֵ
///    pOmigas              ��Ƶ�����飨��Ӧ����г����
///    pInitPhaseAngles     ������λ�����飨��Ӧ����г����
///    pWidths              ��ֵ���飨��Ӧ����г����
///    harmNum              г������
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
