/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      spint -- 32-bit IEEE-754 SP Floating Point -> 32-bit Signed         */
/*      Integer                                                             */
/*                                                                          */
/*  USAGE                                                                   */
/*      This function is C callable, and is called according to this        */
/*      C prototype:                                                        */
/*                                                                          */
/*      int spint_i                                                         */
/*      (                                                                   */
/*        float a,   // Argument 1   //                                     */
/*      );                                                                  */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This kernel returns the integer value of the 32-bit                 */
/*      single precision input.                                             */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*                                                                          */
/*  SOURCE                                                                  */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef _SPINT_I_H_
#define _SPINT_I_H_

extern float spint_i(float x);

#endif /* _SPINT_I_H_ */
/* ======================================================================== */
/*  End of file: spint_i.h                                                  */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2008 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
