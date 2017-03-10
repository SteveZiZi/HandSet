/// @file
///     effValue.cpp
/// @brief
///     有效值计算
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/21
/// @revise
/// 
/*******************************************************************************
有效值计算说明：
    输入：n周期电压/电流采样值
    输出：电压/电流有效值
    计算：采用方均根算法计算所有点的均方根值，即为有效值。
    公式： Xrms=sqrt((x1^2+x2^2+...xn^2)/N)
*******************************************************************************/
#include "effValue.h"

///
/// @brief
///    有效值计算
/// @param[in] 
///    pSampleValues    采样点值
///    sampleNum        采样点数
/// @param[out]
///    pEffValue        有效值
/// @return
///    void
///
void effValueCalc(const float pSampleValues[], Uint16 sampleNum, float *pEffValue)
{
    Uint16 i = 0;
    float tmpEffValue = 0.0;
    
    // 计算平方和
    for (i = 0; i < sampleNum; i++)
    {
        tmpEffValue = addsp_i(tmpEffValue, mpysp_i((float)pSampleValues[i],
            (float)pSampleValues[i]));
    }

    // 计算方均根
    tmpEffValue = sqrtsp_i(divsp_i(tmpEffValue, sampleNum));

    *pEffValue = tmpEffValue;
}
