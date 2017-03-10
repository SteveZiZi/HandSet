/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      spuint -- 32-bit IEEE-754 SP Floating Point -> 32-bit Unsigned      */
/*      Integer                                                             */
/*                                                                          */
/*  USAGE                                                                   */
/*      This function is C callable, and is called according to this        */
/*      C prototype:                                                        */
/*                                                                          */
/*      unsigned int spuint_i                                               */
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

#ifndef _SPUINT_I_H_
#define _SPUINT_I_H_
extern float spuint_i(float x);
#endif /* _SPUINT_I_H */
/* ======================================================================== */
/*  End of file: spuint_i.h                                                 */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2008 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
