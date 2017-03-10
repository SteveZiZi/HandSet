/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      uintsp -- 40-bit Unsigned Long Integer -> 32-bit IEEE-754 SP        */
/*      Floating Point                                                      */
/*                                                                          */
/*  USAGE                                                                   */
/*      This function is C callable, and is called according to this        */
/*      C prototype:                                                        */
/*                                                                          */
/*      float uintsp_i                                                      */
/*      (                                                                   */
/*        unsigned long int a,   // Argument 1   //                         */
/*      );                                                                  */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This kernel returns the single precision floating point             */
/*      value of the input.                                                 */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*                                                                          */
/*  SOURCE                                                                  */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef _UINTSP_I_H_
#define _UINTSP_I_H_
extern float uintsp_i(unsigned long int x);
#endif /* _UINTSP_I_H */
/* ======================================================================== */
/*  End of file: uintsp_i.h                                                 */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2008 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
