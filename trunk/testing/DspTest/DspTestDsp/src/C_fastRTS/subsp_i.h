/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      subsp -- Single Precision Floating Point Subtraction                */
/*                                                                          */
/*                                                                          */   
/*  USAGE                                                                   */
/*      This function is C callable, and is called according to this        */
/*      C prototype:                                                        */
/*                                                                          */
/*      float subsp_i                                                       */
/*      (                                                                   */
/*        float left,   // Argument 1   //                                  */
/*        float right   // Argument 2   //                                  */
/*      );                                                                  */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This kernel returns the single precision subtraction of two         */
/*      32-bit single precision inputs. Underflow returns zero,             */
/*      overflow return + or - infinity, they are placed in the DEBUG macro */
/*                                                                          */    
/*    Single precision binury floating point number format is 32 bit number */
/*    BIT 31 = SIGN, BIT 30 - 23 = EXPONENT and BIT 22 - 0 = MANTISSA       */
/*                                                                          */
/*  Subtraction is performed by taking the 2's complement of the right      */
/*               operand and performing Addition                            */ 
/*                                                                          */
/*  TECHNIQUES                                                              */
/*                                                                          */
/*  SOURCE                                                                  */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef _SUBSP_I_H_
#define _SUBSP_I_H_
extern float subsp_i(float left, float right);

#endif /* _SUBSP_I_H_ */
/* ======================================================================== */
/*  End of file: subsp_i.h                                                  */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2008 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
