/// @file
///     phaseCheck.h
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

#ifndef PHASECHECK_H_
#define PHASECHECK_H_
#include "../inc/inc.h"

extern float phaseCheckPhaseCalc(float phaseAngle1, float phaseAngle2);
extern void phaseCheckVectorCalc(float value1, float phaseAngle1,
                            float value2, float phaseAngle2,
                            float *pDifValue, float *pDifPhaseAngle);
#endif /* PHASECHECK_H_ */ 
