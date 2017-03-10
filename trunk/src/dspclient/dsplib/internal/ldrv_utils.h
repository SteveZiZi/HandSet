/** ============================================================================
 *  @file   ldrv_utils.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/
 *
 *  @desc   Defines generic utilities and macros for LDRV component.
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


#if !defined (LDRV_UTILS_H)
#define LDRV_UTILS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <_dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  BYTE_TO_MADU
 *
 *  @desc   Converts a size or address value from byte to DSP MADU.
 *  ============================================================================
 */
#define BYTE_TO_MADU(value, dspMaduSize) ((Uint32) (value) / (dspMaduSize))

/** ============================================================================
 *  @macro  MADU_TO_BYTE
 *
 *  @desc   Converts a size or address value from DSP MADU to byte.
 *  ============================================================================
 */
#define MADU_TO_BYTE(value, dspMaduSize) ((Uint32) (value) * (dspMaduSize))

/** ============================================================================
 *  @macro  ADDR_GPP_TO_DSP
 *
 *  @desc   Translates an address from GPP address space to DSP address space.
 *  ============================================================================
 */
#define ADDR_GPP_TO_DSP(gppBaseAddr, dspBaseAddr, addr, dspMaduSize)           \
            (  BYTE_TO_MADU (((Uint32) (addr) - (gppBaseAddr)), (dspMaduSize)) \
             + (dspBaseAddr))

/** ============================================================================
 *  @macro  ADDR_DSP_TO_GPP
 *
 *  @desc   Translates an address from DSP address space to GPP address space.
 *  ============================================================================
 */
#define ADDR_DSP_TO_GPP(gppBaseAddr, dspBaseAddr, addr, dspMaduSize)           \
            (  MADU_TO_BYTE (((Uint32) (addr) - (dspBaseAddr)), (dspMaduSize)) \
             + (gppBaseAddr))


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (LDRV_UTILS_H) */
