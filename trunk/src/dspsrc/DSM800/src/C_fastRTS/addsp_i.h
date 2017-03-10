/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      addsp -- Single Precision Floating Point Addition                   */
/*                                                                          */
/*  USAGE                                                                   */
/*      This function is C callable, and is called according to this        */
/*      C prototype:                                                        */
/*                                                                          */
/*      float addsp_i                                                       */
/*      (                                                                   */
/*        float left,   // Argument 1   //                                  */
/*        float right   // Argument 2   //                                  */
/*      );                                                                  */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This kernel returns the single precision addition of two            */
/*      32-bit single precision inputs. Underflow returns zero,             */
/*      overflow return + or - infinity and they are under the DEBUG        */
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

#ifndef _ADDSP_I_H_
#define _ADDSP_I_H_
extern float addsp_i(float left, float right);


#endif /* _ADDSP_I_H_ */
/* ======================================================================== */
/*  End of file: addsp_i.h                                                  */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2008 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */

