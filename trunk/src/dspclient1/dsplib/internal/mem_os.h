/** ============================================================================
 *  @file   mem_os.h
 *
 *  @path   $(DSPLINK)/gpp/src/osal/WinCE/
 *
 *  @desc   Defines the OS dependent attributes & structures for the
 *          sub-component MEM.
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


#if !defined (MEM_OS_H)
#define MEM_OS_H

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */



/** ============================================================================
 *  @type   MemAttrs
 *
 *  @desc   OS dependent attributes for allocating memory.
 *
 *  @field  MEM_UNCACHED: Uncached memory
 *          MEM_CACHED: Cached memory
 *  ============================================================================
 */
typedef enum MemAttrs_tag {
    MEM_UNCACHED = 0u,
    MEM_CACHED
} MemAttrs ;

/** ============================================================================
 *  @type   MemAllocAttrs
 *
 *  @desc   OS dependent attributes for allocating memory.
 *
 *  @field  physicalAddress
 *              Physical address of the allocated memory.
 *  @field  bigArea
 *              Allocate a special area (e.g BIG PHYSICAL AREA).
 *  ============================================================================
 */
typedef struct MemAllocAttrs_tag {
    Uint32 *    physicalAddress ;
    Bool        bigArea         ;
} MemAllocAttrs ;

/** ============================================================================
 *  @type   MemFreeAttrs
 *
 *  @desc   OS dependent attributes for freeing memory.
 *
 *  @field  physicalAddress
 *              Physical address of the memory to be freed.
 *  @field  size
 *              Size of the memory to be freed.
 *  @field  bigArea
 *              Free a special area (e.g BIG PHYSICAL AREA).
 *  ============================================================================
 */
typedef struct MemFreeAttrs_tag {
    Uint32 *    physicalAddress ;
    Uint32      size ;
    Bool        bigArea         ;
} MemFreeAttrs ;


/** ============================================================================
 *  @type   MemMapInfo_tag
 *
 *  @desc   OS dependent definition of the information required for mapping a
 *          memory region.
 *
 *  @field  src
 *              Address to be mapped.
 *  @field  size
 *              Size of memory region to be mapped.
 *  @field  dst
 *              Mapped address.
 *  @field  memAttrs
 *              Mapped address attributes.
 *  ============================================================================
 */
struct MemMapInfo_tag {
    Uint32   src  ;
    Uint32   size ;
    Uint32   dst  ;
    MemAttrs memAttrs ;
} ;


/** ============================================================================
 *  @type   MemUnmapInfo_tag
 *
 *  @desc   OS dependent definition of the information required for unmapping
 *          a previously mapped memory region.
 *
 *  @field  addr
 *              Address to be unmapped. This is the address returned as 'dst'
 *              address from a previous call to MEM_Map () in the MemMapInfo
 *              structure.
 *  @field  size
 *              Size of memory region to be unmapped.
 *  @field  memAttrs
 *              Mapped address attributes.
 *  ============================================================================
 */
struct MemUnmapInfo_tag {
    Uint32  addr ;
    Uint32  size ;
    MemAttrs memAttrs ;
} ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (MEM_OS_H) */
