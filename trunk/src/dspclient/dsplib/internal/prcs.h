/** ============================================================================
 *  @file   prcs.h
 *
 *  @path   $(DSPLINK)/gpp/src/osal/
 *
 *  @desc   Defines the interfaces and data structures for the sub-component
 *          PRCS.
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


#if !defined (PRCS_H)
#define PRCS_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   PrcsObject
 *
 *  @desc   Forward declaration to a OS specific client identifier object.
 *  ============================================================================
 */
typedef struct PrcsObject_tag  PrcsObject ;


/** ============================================================================
 *  @func   PRCS_Initialize
 *
 *  @desc   Initializes the PRCS subcomponent.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General error from GPP-OS.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PRCS_Initialize (Void) ;


/** ============================================================================
 *  @func   PRCS_Finalize
 *
 *  @desc   Releases resources used by the PRCS subcomponent.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General error from GPP-OS.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PRCS_Finalize (Void) ;


/** ============================================================================
 *  @func   PRCS_Create
 *
 *  @desc   Creates a PrcsObject and populates it with information to identify
 *          the client.
 *
 *  @arg    prcsObj
 *              OUT argument to store the created object.
 *  @arg    optArgs
 *              Optional arguments
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *
 *  @enter  prcsObj must be a valid pointer.
 *
 *  @leave  Valid object is returned in case of success.
 *
 *  @see    PRCS_Delete
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PRCS_Create (OUT PrcsObject ** prcsObj, IN OPT Void * optArgs) ;


/** ============================================================================
 *  @func   PRCS_Delete
 *
 *  @desc   Frees up resources used by the specified object.
 *
 *  @arg    prcsObj
 *              Object to be deleted.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EPOINTER
 *              Invalid prcsObj
 *
 *  @enter  prcsObj must be a valid object.
 *
 *  @leave  None
 *
 *  @see    PRCS_Create
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PRCS_Delete (IN PrcsObject * prcsObj) ;


/** ============================================================================
 *  @func   PRCS_IsEqual
 *
 *  @desc   Compares two clients to check if they are "equal". Equality is
 *          defined by implementation on the specific OS port.
 *
 *  @arg    client1
 *              First client's information
 *  @arg    client2
 *              Second client's information
 *  @arg    isEqual
 *              Place holder for result of comparison
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *
 *  @enter  client1 must be a valid object.
 *          client2 must be a valid object.
 *          isEqual must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    PRCS_Create
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PRCS_IsEqual (IN  PrcsObject *  client1,
              IN  PrcsObject *  client2,
              OUT Bool *        isEqual) ;


/** ============================================================================
 *  @func   PRCS_IsSameContext
 *
 *  @desc   Checks if the two clients share same context (address space).
 *
 *  @arg    client1
 *              First client's information
 *  @arg    client2
 *              Second client's information
 *  @arg    isSame
 *              Place holder for result of comparison
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *
 *  @enter  client1 must be a valid object.
 *          client2 must be a valid object.
 *          isSame must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    PRCS_Create
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PRCS_IsSameContext (IN  PrcsObject *  client1,
                    IN  PrcsObject *  client2,
                    OUT Bool *        isSame) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (PRCS_H) */
