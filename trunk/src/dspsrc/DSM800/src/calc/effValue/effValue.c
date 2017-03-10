/// @file
///     effValue.cpp
/// @brief
///     ��Чֵ����
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/21
/// @revise
/// 
/*******************************************************************************
��Чֵ����˵����
    ���룺n���ڵ�ѹ/��������ֵ
    �������ѹ/������Чֵ
    ���㣺���÷������㷨�������е�ľ�����ֵ����Ϊ��Чֵ��
    ��ʽ�� Xrms=sqrt((x1^2+x2^2+...xn^2)/N)
*******************************************************************************/
#include "effValue.h"

///
/// @brief
///    ��Чֵ����
/// @param[in] 
///    pSampleValues    ������ֵ
///    sampleNum        ��������
/// @param[out]
///    pEffValue        ��Чֵ
/// @return
///    void
///
void effValueCalc(const float pSampleValues[], Uint16 sampleNum, float *pEffValue)
{
    Uint16 i = 0;
    float tmpEffValue = 0.0;
    
    // ����ƽ����
    for (i = 0; i < sampleNum; i++)
    {
        tmpEffValue = addsp_i(tmpEffValue, mpysp_i((float)pSampleValues[i],
            (float)pSampleValues[i]));
    }

    // ���㷽����
    tmpEffValue = sqrtsp_i(divsp_i(tmpEffValue, sampleNum));

    *pEffValue = tmpEffValue;
}
