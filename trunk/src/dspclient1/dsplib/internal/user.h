/** ============================================================================
 *  @file   user.h
 *
 *  @path   $(DSPLINK)/gpp/src/osal/WinCE/
 *
 *  @desc   Defines the interfaces and data structures for the USER access.
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


#if !defined (USER_H)
#define USER_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  KMEM_STARTADDRESS
 *
 *  @desc   Start address of Kernel Virtual Memory. On I386 machines, upper 1GB
 *          is used by kernel.
 *  ============================================================================
 */
#define KMEM_STARTADDRESS       ((Pvoid) 0xC0000000u)


/** ============================================================================
 *  @func   USER_copyFromUser
 *
 *  @desc   Copy user data to kernel buffer.
 *
 *  @arg    dst
 *              Destination bufefr.
 *  @arg    src
 *              Source buffer.
 *  @arg    numBytes
 *              number of bytes to copy.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Failed to copy.
 *
 *  @enter  src, dst must be valid.
 *
 *  @leave  None
 *
 *  @see    USER_copyToUser
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
USER_copyFromUser (IN Pvoid dst, OUT Pvoid src, IN Uint32 numBytes) ;


/** ============================================================================
 *  @func   USER_copyFromUser
 *
 *  @desc   Copy kernel data to user buffer.
 *
 *  @arg    src
 *              Source buffer.
 *  @arg    dst
 *              Destination bufefr.
 *  @arg    numBytes
 *              number of bytes to copy.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Failed to copy.
 *
 *  @enter  src, dst must be valid.
 *
 *  @leave  None
 *
 *  @see    USER_copyFromUser
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
USER_copyToUser (IN Pvoid src, OUT Pvoid dst, IN Uint32 numBytes) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !define (USER_H) */
