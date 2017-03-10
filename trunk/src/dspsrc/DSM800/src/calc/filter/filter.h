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
#ifndef FILTER_H_
#define FILTER_H_
// 4k,5k,8k,10k,12.8k
extern float a40_70Hz[5][3][3];
extern float b40_70Hz[5][3][3];
extern float g40_70Hz[5];

typedef struct tag_IIR_State_3Order
{
     double w01;
     double w02;
     double w11;
     double w12;
     double w21;
     double w22;
}IIR_State_6order;

extern IIR_State_6order iir_state;

extern void iir_6th_filter(const float * pcmIn, float (*a)[3], float (*b)[3], 
	float gain, IIR_State_6order* filter_state, int sample_size, float *pcmOut);
#endif	/* FILTER_H_ */
