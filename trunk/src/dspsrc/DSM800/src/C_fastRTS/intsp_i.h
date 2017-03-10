/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      intsp -- 32-bit Signed Integer -> 32-bit IEEE-754 SP Floating       */
/*      Point                                                               */
/*                                                                          */
/*  USAGE                                                                   */
/*      This function is C callable, and is called according to this        */
/*      C prototype:                                                        */
/*                                                                          */
/*      float intsp_i                                                       */
/*      (                                                                   */
/*        int a,   // Argument 1   //                                       */
/*      );                                                                  */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This kernel returns the single precision floating point             */
/*      representation of the inpt integer.                                 */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*                                                                          */
/*  SOURCE                                                                  */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef _INTSP_I_H_
#define _INTSP_I_H_
extern float intsp_i(int x);
#endif /* _INTSP_I_H_ */
/* ======================================================================== */
/*  End of file: intsp_i.h                                                  */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2008 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
