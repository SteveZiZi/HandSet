/// @file
///     fft.h
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

#ifndef FFT_H_
#define FFT_H_
#include "../inc/inc.h"

#if 1
extern void fft_4(float aRe[], float aIm[]);
extern void fft_5(float aRe[], float aIm[]);
extern void fft_8(float zRe[], float zIm[]);
extern void fft_10(float zRe[], float zIm[]);
//extern void fft_10_1(float zRe[], float zIm[]);
extern void fft_16(float zRe[], float zIm[]);
extern void fft_20(float zRe[], float zIm[]);
#endif
extern void compBaseSeq(Uint16 perTab[], Uint16 factor3, Uint16 factor2,
                    Uint16 factor1);
extern void permute(Uint16 perTab[], Int16 tabSize, const float xRe[], 
                    const float xIm[], float yRe[],float yIm[]);
extern void twiddleTransf(Uint16 sofarRadix, Uint16 radix, Uint16 remainRadix,
                               float yRe[], float yIm[]);

#endif /* FFT_H_*/ 

