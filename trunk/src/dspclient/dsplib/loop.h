/** ============================================================================
 *  @file   loop.h
 *
 *  @path   $(DSPLINK)/gpp/src/samples/loop/
 *
 *  @desc   Defines the configurable parameters for the loopback test which
 *          sends a buffer across the DSP processor and recieves a buffer back
 *          using DSP/BIOS LINK. It also does the data verification on the
 *          recieved buffer. In this application the transfer is done using
 *          only one buffer and no queueing of buffers is done.
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


#if !defined (LOOP_H)
#define LOOP_H

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#include <linkcfgdefs.h>


//#define DA8XXGEM       0
//#define VERIFY_DATA    0
#define ZCPY_LINK     


	
/*  ----------------------------------- DSP/BIOS LINK API               */
#include <proc.h>
#include <chnl.h>
#include <pool.h>
#if defined (DA8XXGEM)
#include <loaderdefs.h>
#endif

/*  ----------------------------------- Application Header              */

#include "loop_os.h"


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

#define MAXSTRLENGTH        256
/*  ============================================================================
 *  @name   NUM_ARGS
 *
 *  @desc   Number of arguments specified to the DSP application.
 *  ============================================================================
 */
#define NUM_ARGS 2


/*  ============================================================================
 *  @name   XFER_CHAR
 *
 *  @desc   The value used to initialize the output buffer and used for
 *          validation against the input buffer recieved.
 *  ============================================================================
 */
#define XFER_CHAR   (Char8) 0xE7

/** ============================================================================
 *  @name   NUMBUFFERPOOLS
 *
 *  @desc   Number of buffer pools in this application.
 *  ============================================================================
 */
#define NUMBUFFERPOOLS 1

/** ============================================================================
 *  @name   NUMBUFS
 *
 *  @desc   Number of buffers in pool.
 *  ============================================================================
 */
#define NUMBUFS        4

/** ============================================================================
 *  @name   POOL_ID
 *
 *  @desc   Pool id for this application.
 *  ============================================================================
 */
#define POOL_ID        0


/*  ============================================================================
 *  @name   LOOP_BufferSize
 *
 *  @desc   Size of buffer to be used for data transfer.
 *  ============================================================================
 */
STATIC Uint32  LOOP_BufferSize ;

/*  ============================================================================
 *  @name   LOOP_NumIterations
 *
 *  @desc   Number of iterations of data transfer.
 *          A value of 0 in LOOP_NumIterations implies infinite iterations.
 *  ============================================================================
 */
STATIC Uint32  LOOP_NumIterations ;

/** ============================================================================
 *  @name   LOOP_Buffers
 *
 *  @desc   Array of buffers used by the loop application.
 *          Length of array in this application is 1.
 *  ============================================================================
 */
STATIC Char8 *LOOP_Buffers[1];  //修改为16位的数据宽度

/** ============================================================================
 *  @name   LOOP_IOReq
 *
 *  @desc   It gives information for adding or reclaiming a request.
 *  ============================================================================
 */
STATIC ChannelIOInfo LOOP_IOReq  ;

#if defined (DA8XXGEM)
/** ============================================================================
 *  @name   dspAddr
 *
 *  @desc   Address of c_int00 in the DSP executable.
 *  ============================================================================
 */
Uint32 LOOP_dspAddr ;

/** ============================================================================
 *  @name   shmAddr
 *
 *  @desc   Address of symbol DSPLINK_shmBaseAddres in the DSP executable.
 *  ============================================================================
 */
Uint32 LOOP_shmAddr ;

/** ============================================================================
 *  @name   argsAddr
 *
 *  @desc   Address of .args section in the DSP executable.
 *  ============================================================================
 */
Uint32 LOOP_argsAddr ;

/** ============================================================================
 *  @name   LINKCFG_config
 *
 *  @desc   Extern declaration to the default DSP/BIOS LINK configuration
 *          structure.
 *  ============================================================================
 */
extern  LINKCFG_Object LINKCFG_config ;

#endif




/** ============================================================================
 *  @const  CHNL_ID_OUTPUT
 *
 *  @desc   ID of channel used to send data to DSP.
 *  ============================================================================
 */
#define CHNL_ID_OUTPUT     0


/** ============================================================================
 *  @const  CHNL_ID_INPUT
 *
 *  @desc   ID of channel used to receive data from DSP.
 *  ============================================================================
 */
#define CHNL_ID_INPUT      1


/** ============================================================================
 *  @func   LOOP_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @arg    dspExecutable
 *              DSP executable name.
 *  @arg    bufferSize
 *              String representation of buffer size to be used
 *              for data transfer.
 *  @arg    strNumIterations
 *              Number of iterations a data buffer is transferred between
 *              GPP and DSP in string format.
 *
 *  @arg    processorId
 *             Id of the DSP Processor. 
 *
 *  @ret    DSP_SOK
 *              Operation Successfully completed.
 *          DSP_EFAIL
 *              Resource allocation failed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    LOOP_Delete
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DSP_Create (IN Char8 * dspExecutable,    //dsp可执行文件的名称
			 IN UINT8 ** inbuf,
             IN Char8 * strBufferSize,    //字符串缓冲区大小
             IN Uint8   processorId) ;


/** ============================================================================
 *  @func   LOOP_Execute_Send
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @arg    numIterations
 *              Number of iterations a data buffer is transferred between
 *              GPP and DSP in string format.
 *
 *  @ret    DSP_SOK
 *              Operation Successfully completed.
 *          DSP_EFAIL
 *              Loop execution failed.
 *
 *  @arg    processorId
 *             Id of the DSP Processor. 
 *
 *  @ret    DSP_SOK
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    LOOP_Delete , LOOP_Create
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DSP_Execute_Send (IN Uint8 processorId);


/** ============================================================================
 *  @func   LOOP_Execute_Recevice
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @arg    numIterations
 *              Number of iterations a data buffer is transferred between
 *              GPP and DSP in string format.
 *
 *  @ret    DSP_SOK
 *              Operation Successfully completed.
 *          DSP_EFAIL
 *              Loop execution failed.
 *
 *  @arg    processorId
 *             Id of the DSP Processor. 
 *
 *  @ret    DSP_SOK
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    LOOP_Delete , LOOP_Create
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DSP_Execute_Receive(IN Uint8 processorId,UINT8 **outbuf);
/** ============================================================================
 *  @func   LOOP_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          LOOP_Create ().
 *          During cleanup, the allocated resources are being freed
 *          unconditionally. Actual applications may require stricter check
 *          against return values for robustness.
 *
 *  @arg    processorId
 *             Id of the DSP Processor. 
 *
 *  @ret    DSP_SOK
 *
 *  @ret    DSP_SOK
 *              Operation Successfully completed.
 *          DSP_EFAIL
 *              Resource deallocation failed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    LOOP_Create
 *  ============================================================================
 */
EXPORT_API
Void
DSP_Delete (Uint8 processorId) ;



#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LOOP_H) */
