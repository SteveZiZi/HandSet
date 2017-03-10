/// @file
///     harm800.h
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

#ifndef HARM_H_
#define HARM_H_
#include "../inc/inc.h"
typedef enum _HARM_TYPE{
    HARM_800,
    HARM_1000,
    HARM_1600,
    HARM_2000,
    HARM_2560,
    HARM_LAST
}HARM_TYPE;

#define    MAX_HARM_SAMP_COUNT   2560       ///< 支持的最大的谐波采样点数

extern int harmCalc(HARM_TYPE harmType, Uint16 nSampleSize, const float pSampleValues[], 
                    Uint16 nHarmSize, float *pDCValue, float pHarmContents[], 
                    float pHarmPhaseAngles[]);
extern int harmCalc2(HARM_TYPE harmType, Uint16 nSampleSize, const float pSampleValues[], 
              const float pSampleValues1[], Uint16 nHarmSize, float *pDCValue, 
              float *pDCValue1, float pHarmContents[], float pHarmContents1[],
              float pHarmPhaseAngles[], float pHarmPhaseAngles1[]);
extern int harmTHDCalc(HARM_TYPE harmType, Uint16 nHarmSize, const float pHarmContents[],
                Int16 nType, float pTHDValues[]);
extern int harmSumTHDCalc(HARM_TYPE harmType, Uint16 nHarmSize, 
               const float pHarmContents[], Int16 nType, float *pTHDValue);
extern int dcTHDCalc(float fDCValue, float fBaseHarmContent, Int16 nType, 
			float *pTHDValue);
#endif /* HARM_H_ */ 
