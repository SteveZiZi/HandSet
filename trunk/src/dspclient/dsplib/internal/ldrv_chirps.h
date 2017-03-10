/** ============================================================================
 *  @file   ldrv_chirps.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/
 *
 *  @desc   Functions to manage list of LDRV_CHNL_Irp objects.
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


#if !defined (LDRV_CHIRPS_H)
#define LDRV_CHIRPS_H


/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>

/*  ----------------------------------- Generic Function              */
#include <list.h>

/*  ----------------------------------- Link Driver                   */
#include <ldrv_chnl.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   LDRV_CHIRPS_CreateList
 *
 *  @desc   Allocates a chirp list.
 *
 *  @arg    numIRPs
 *              Number of chirps to be allocated in the list.
 *
 *  @ret    NULL
 *              Error allocating list.
 *          non=NULL
 *              Pointer to allocated list.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
List *
LDRV_CHIRPS_CreateList (IN Uint32 numIRPs) ;


/** ============================================================================
 *  @func   LDRV_CHIRPS_FreeList
 *
 *  @desc   Deallocates a chirp list.
 *
 *  @arg    chirpList
 *              List to be freed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Error occurred while freeing the memory.
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
LDRV_CHIRPS_FreeList (IN List * chirpList) ;


/** ============================================================================
 *  @func   LDRV_CHIRPS_Alloc
 *
 *  @desc   Allocates a single chirp.
 *
 *  @arg    None
 *
 *  @ret    NULL
 *              Error allocating chirp.
 *          Non=NULL
 *              Pointer to allocated chirp.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
LDRV_CHNL_Irp *
LDRV_CHIRPS_Alloc (Void) ;


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_CHIRPS_Print
 *
 *  @desc   Prints information about a single CHIRP (Channel IO Request Packet).
 *
 *  @arg    chirp
 *              CHIRP to be printed.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_CHIRPS_Print (IN LDRV_CHNL_Irp * chirp) ;


/** ============================================================================
 *  @func   LDRV_CHIRPS_Debug
 *
 *  @desc   Prints debug information about a CHIRP (Channel IO Request Packet)
 *          List.
 *
 *  @arg    chirpList
 *              CHIRP List to be printed.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_CHIRPS_Debug (IN List * chirpList) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LDRV_CHIRPS_H) */
