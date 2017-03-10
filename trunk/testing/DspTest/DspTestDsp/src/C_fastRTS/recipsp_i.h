/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      recipsp -- Single Precision Floating Point Reciprocal               */
/*                                                                          */
/*  USAGE                                                                   */
/*      This function is C callable, and is called according to this        */
/*      C prototype:                                                        */
/*                                                                          */
/*      float recipsp_ci                                                    */
/*      (                                                                   */
/*        float right   // Argument 1   //                                  */
/*      );                                                                  */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This kernel returns the single precision reciprocal of the          */
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

extern float recipsp_i(float right);

/* ======================================================================== */
/*  End of file: recipsp_ci.c                                                */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2008 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
