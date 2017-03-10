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

#ifndef DISPRATE_H_
#define DISPRATE_H_
#include "../inc/inc.h"
#include "stdint.h"
#define MAX_ASDU_COUNT 8

extern int dispHashCreate(uint8_t nASDUCount);
extern void dispRateCalc(int32_t dataTimes[], uint16_t dataSize, 
		 uint16_t nSamplingRate, uint8_t nASDUCount, float dispRates[]);
extern void resetDisp();

#endif /* DISPRATE_H_ */ 
