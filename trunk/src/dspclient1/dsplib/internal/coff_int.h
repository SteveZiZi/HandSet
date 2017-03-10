/** ============================================================================
 *  @file   coff_int.h
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Defines interface for generic functions and macros of COFF loader.
 *
 *  @ver    1.64
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */


#if !defined (COFF_INT_H)
#define COFF_INT_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- OSAL Headers                */
#include <kfile.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  READ_REC_SIZE
 *
 *  @desc   Record size to be specified when reading file thru' KFILE_Read
 *  ============================================================================
 */
#define READ_REC_SIZE   1u


/** ============================================================================
 *  @func   COFF_read8
 *
 *  @desc   Reads an Int8 from file.
 *
 *  @arg    fileObj
 *              File to read from.
 *
 *  @ret    The read value.
 *
 *  @enter  fileObj must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Int8
COFF_read8 (IN KFileObject * fileObj) ;


/** ============================================================================
 *  @func   COFF_read16
 *
 *  @desc   Reads an Int16 from file.
 *
 *  @arg    fileObj
 *              File to read from.
 *  @arg    swap
 *              specifies whether the bytes need to be swapped.
 *
 *  @ret    The read value.
 *
 *  @enter  fileObj must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Int16
COFF_read16 (IN KFileObject * fileObj, IN Bool swap) ;


/** ============================================================================
 *  @func   COFF_read32
 *
 *  @desc   Reads an Int32 from file.
 *
 *  @arg    fileObj
 *              File to read from.
 *  @arg    swap
 *              specifies whether the bytes need to be swapped.
 *
 *  @ret    The read value.
 *
 *  @enter  fileObj must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Int32
COFF_read32 (IN KFileObject * fileObj, IN Bool swap) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (COFF_INT_H) */
