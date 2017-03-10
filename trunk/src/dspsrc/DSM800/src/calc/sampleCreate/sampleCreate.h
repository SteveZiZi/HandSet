/// @file
///     dispRate.h
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

#ifndef SAMPLECREATE_H_
#define SAMPLECREATE_H_
#include "../inc/inc.h"

extern void sinSampleCreate(float *pSamples, 
                     Uint16 sampleSize, 
                     Uint16 sampleFreq, 
                     float omiga, 
                     float initPhaseAngle, 
                     float width);
extern void sinSamplesCreate(float *pSamples,
                      Uint16 sampleSize,
                      Uint16 sampleFreq,
                      float dcValue,
                      const float pOmiga[],
                      const float pInitPhaseAngle[],
                      const float pWidth[],
                      Uint16 harmNum
                      );
extern void cosSampleCreate(float *pSamples, 
                     Uint16 sampleSize, 
                     Uint16 sampleFreq, 
                     float omiga, 
                     float initPhaseAngle, 
                     float width);
extern void cosSamplesCreate(float *pSamples,
                      Uint16 sampleSize,
                      Uint16 sampleFreq,
                      float dcValue,
                      const float pOmiga[],
                      const float pInitPhaseAngle[],
                      const float pWidth[],
                      Uint16 harmNum
                      );
#endif /* SAMPLECREATE_H_ */ 
