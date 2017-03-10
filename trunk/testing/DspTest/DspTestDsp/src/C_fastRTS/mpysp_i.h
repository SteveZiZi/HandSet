/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      mpysp -- Single Precision Floating Point Multiply                   */
/*                                                                          */
/*  USAGE                                                                   */
/*      This function is C callable, and is called according to this        */
/*      C prototype:                                                        */
/*                                                                          */
/*      float mpysp_i                                                       */
/*      (                                                                   */
/*        float left,   // Argument 1   //                                  */
/*        float right   // Argument 2   //                                  */
/*      );                                                                  */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This kernel returns the single precision multiplication of two      */
/*      32-bit single precision inputs.                                     */
/*                                                                          */
/*    Single precision binury floating point number format is 32 bit number */
/*    BIT 31 = SIGN, BIT 30 - 23 = EXPONENT and BIT 22 - 0 = MANTISSA       */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*                                                                          */
/*  SOURCE                                                                  */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef _MPYSP_I_H_
#define _MPYSP_I_H_
extern  float mpysp_i(float left, float right);

#endif /* _MPYSP_I_H_ */
/* ======================================================================== */
/*  End of file: mpysp_i.h                                                  */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2008 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
