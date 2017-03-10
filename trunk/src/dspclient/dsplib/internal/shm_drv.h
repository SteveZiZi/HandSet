/** ============================================================================
 *  @file   shm_drv.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/DRV/
 *
 *  @desc   Defines the Shared Memory driver component for initialization and
 *          synchronization of the GPP and DSP-side drivers.
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


#if !defined (SHMDRV_H)
#define SHMDRV_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  SHMDRV_CTRL_PADLENGTH
 *
 *  @desc   Padding required for DSP L2 cache line alignment within SHMDRV
 *          control structure.
 *  ============================================================================
 */
#define SHMDRV_CTRL_PADLENGTH  (((CACHE_L2_LINESIZE) - (sizeof (Uint32) * 2))/2)


/** ============================================================================
 *  @name   SHMDRV_Ctrl
 *
 *  @desc   Defines the control structure used by GPP and DSP for SHM Link
 *          driver
 *
 *  @field  procId
 *              Processor Identifier.
 *  @field  handshakeGpp
 *              Handshake field to be updated by GPP.
 *  @field  handshakeDsp
 *              Handshake field to be updated by DSP.
 *  @field  padding
 *              Padding for alignment
 *  ============================================================================
 */
typedef struct SHMDRV_Ctrl_tag {
    volatile Uint32  handshakeGpp   ;
    volatile Uint32  handshakeDsp   ;
             Uint16  padding [SHMDRV_CTRL_PADLENGTH] ;
} SHMDRV_Ctrl ;


/** ============================================================================
 *  @func   SHMDRV_init
 *
 *  @desc   This function initializes the SHMDRV component.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    linkDrvId
 *              Link driver Identifier.
 *  @arg    dspAddr
 *              Location to receive the DSP address of the shared memory control
 *              region used by the link driver.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EMEMORY
 *              Out of memory.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          shDspAddr must be a valid pointer.
 *
 *  @leave  None.
 *
 *  @see    SHMDRV_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
SHMDRV_init (IN  ProcessorId dspId,
             IN  Uint32      linkDrvId,
             OUT Uint32 *    shDspAddr) ;


/** ============================================================================
 *  @func   SHMDRV_exit
 *
 *  @desc   This function finalizes the SHMDRV component.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    linkDrvId
 *              Link driver Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EMEMORY
 *              Out of memory.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None.
 *
 *  @see    SHMDRV_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
SHMDRV_exit (IN ProcessorId dspId, IN Uint32 linkDrvId) ;


/** ============================================================================
 *  @func   SHMDRV_handshake
 *
 *  @desc   This function performs the necessary handshake between the drivers
 *          on the GPP & DSP.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    linkDrvId
 *              Link driver Identifier.
 *  @arg    hshkCtrl
 *              Handshake control action to be executed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Operation failed.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None
 *
 *  @see    SHMDRV_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
SHMDRV_handshake (IN  ProcessorId   dspId,
                  IN  Uint32        linkDrvId,
                  IN  DRV_Handshake hshkCtrl) ;


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   SHMDRV_debug
 *
 *  @desc   This function prints the current status of the SHMDRV subcomponent.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    linkDrvId
 *              Link driver Identifier.
 *
 *  @ret    None
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Void
SHMDRV_debug (IN ProcessorId dspId, IN  Uint32 linkDrvId) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (SHMDRV_H) */
