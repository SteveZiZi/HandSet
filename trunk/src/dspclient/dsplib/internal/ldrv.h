/** ============================================================================
 *  @file   ldrv.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/
 *
 *  @desc   Defines constants and interfaces to initialize and finalize
 *          sub-component LDRV.
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


#if !defined (LDRV_H)
#define LDRV_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <linkcfgdefs.h>
#include <cfgmap.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   LDRV_LinkCfgPtr
 *
 *  @desc   Global pointer containing DSPLINK configuration information for the
 *          driver.
 *  ============================================================================
 */
extern LINKCFG_Object * LDRV_LinkCfgPtr ;

/** ============================================================================
 *  @name   CFGMAP_Config
 *
 *  @desc   Global structure containing DSPLINK configuration mapping
 *          information for DSP/BIOS LINK.
 *  ============================================================================
 */
extern CFGMAP_Object  * CFGMAP_Config [] ;



/** ============================================================================
 *  @func   LDRV_moduleInit
 *
 *  @desc   Initializes the LDRV sub-component for the Gpp.
 *
 *  @arg    linkCfg
 *              Pointer to the user-side system configuration object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EFAIL
 *              General error from GPP-OS.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    LDRV_exit
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_moduleInit (IN LINKCFG_Object * linkCfg) ;


/** ============================================================================
 *  @func   LDRV_moduleExit
 *
 *  @desc   Releases resources used by LDRV sub-component for the Gpp.
 *
 *  @arg    none.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_EFAIL
 *              General error from GPP-OS.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    LDRV_init
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_moduleExit (Void) ;


/** ============================================================================
 *  @func   LDRV_init
 *
 *  @desc   Initializes the LDRV sub-component for a DSP.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    dspCfg
 *              Pointer to the user-side DSP configuration object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EFAIL
 *              General error from GPP-OS.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    LDRV_exit
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_init (IN ProcessorId procId, IN LINKCFG_DspConfig * dspCfg) ;


/** ============================================================================
 *  @func   LDRV_exit
 *
 *  @desc   Releases resources used by LDRV sub-component for a DSP.
 *
 *  @arg    procId
 *              Processor Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_EFAIL
 *              General error from GPP-OS.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    LDRV_init
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_exit (IN ProcessorId procId) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LDRV_H) */
