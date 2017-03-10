/// @file
///     freq.h
/// @brief
///     
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/20
/// @revise
/// 

#ifndef FREQ_H_
#define FREQ_H_
#include "../inc/inc.h"


#define MIN_SAMPLE_NUM	10						///< 计算频率最小的采样个数
extern Int16 freqCalc(Uint16 sampleNum, Uint16 sampleFreq, float peroidCount,
                      float difTime, float *pFreq);
extern Int16 peroidNumCalc(Uint16 sampleNum, Uint16 sampleFreq, 
                           const float *pSampleValues, 
                           float minFreq, float maxFreq, 
                           float *periodNum, Uint16 *validSampleNum,
                           float *difTime);


#endif /* FREQ_H_ */ 
