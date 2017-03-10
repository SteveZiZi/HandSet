/** ============================================================================
 *  @file   ldrv_os.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/
 *
 *  @desc   Defines constants and interfaces for the OS-specific functionality
 *          for the LDRV component.
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


#if !defined (LDRV_OS_H)
#define LDRV_OS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   LDRV_getLinkCfg
 *
 *  @desc   Gets the pointer to kernel configuration structure after creating
 *          it (if required). Copies only GPP object.
 *
 *  @arg    linkCfg
 *              Pointer to the user-side configuration object.
 *  @arg    knlLinkCfg
 *              Location to receive the pointer to the kernel-side configuration
 *              object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  linkCfg must be a valid pointer.
 *          knlLinkCfg must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_getLinkGppCfg (IN  LINKCFG_Object *  linkCfg,
                 OUT LINKCFG_Object ** knlLinkCfg) ;


/** ============================================================================
 *  @func   LDRV_getLinkDspCfg
 *
 *  @desc   Gets the pointer to kernel configuration structure after creating
 *          it (if required). Copies a DSP object.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    dspCfg
 *              Pointer to the user-side configuration object.
 *  @arg    knlLinkCfg
 *              Location to receive the pointer to the kernel-side configuration
 *              object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  linkCfg must be a valid pointer.
 *          knlLinkCfg must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_getLinkDspCfg (IN  ProcessorId         procId,
                    IN  LINKCFG_DspConfig * dspCfg,
                    OUT LINKCFG_Object *    knlLinkCfg) ;


/** ============================================================================
 *  @func   LDRV_freeLinkGppCfg
 *
 *  @desc   Frees any memory allocated for the kernel-side DSPLINK configuration
 *          structure. Frees only GPP object
 *
 *  @arg    knlLinkCfg
 *              Pointer to the kernel-side configuration object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_freeLinkGppCfg (IN  LINKCFG_Object *  knlLinkCfg) ;


/** ============================================================================
 *  @func   LDRV_freeLinkGppCfg
 *
 *  @desc   Frees any memory allocated for the kernel-side DSPLINK configuration
 *          structure. Frees only GPP object
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    knlLinkCfg
 *              Pointer to the kernel-side configuration object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_freeLinkDspCfg (IN  ProcessorId       procId,
                     IN  LINKCFG_Object *  knlLinkCfg) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LDRV_OS_H) */
