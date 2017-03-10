//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
/*++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Module Name:

ppkfuncs.h

Abstract:

Private version of kfuncs.h

Notes:


--*/
#ifndef _PRIV_KFUNCS_H__
#define _PRIV_KFUNCS_H__

#include <excpt.h>
//#include <toolhelp.h>
#include <winioctl.h>
#include <wdogapi.h>

//#include "vmlayout.h"

#ifdef __cplusplus
extern "C" {
#endif

// @doc EXTERNAL KERNEL
// @topic Kernel Entrypoints | Kernel Entrypoints

#define ARG_TYPE_MASK       0x0f
#define ARG_TYPE_BITS       4

#define ARG_O_BIT           0x8     // all output pointer parameters have this bit set
#define ARG_I_BIT           0x4     // all input pointer parameters have this bit set

/* function signature generation macros */

#define ARG_DW              0   // scaler, dword

// I64 argument is *NOT* support for it makes argument passing hard to optimize (can't simple take the # of arguments
// and allocate space for it). Use ARG_IO_PI64 instead to pass pointer, or split it into 2 arguments.
//#define ARG_I64             1   // scaler, 64-bit value

#define ARG_I_PTR           (ARG_I_BIT)             // input only pointer, size in the next argument
#define ARG_I_WSTR          (ARG_I_BIT|1)           // input only, unicode string
#define ARG_I_ASTR          (ARG_I_BIT|2)           // input only, ascii string
#define ARG_I_PDW           (ARG_I_BIT|3)           // input only, ptr to DWORD (can be used for constant sized structure for kernel PSL)

#define ARG_O_PTR           (ARG_O_BIT)             // output only pointer, size in the next argument
#define ARG_O_PDW           (ARG_O_BIT|1)           // output only, pointer to DWORD
#define ARG_O_PI64          (ARG_O_BIT|2)           // output only, pointer to 64 bit value

#define ARG_IO_PTR          (ARG_I_BIT|ARG_O_BIT)   // I/O pointer, size in the next argument
#define ARG_IO_PDW          (ARG_IO_PTR|1)          // I/O pointer to DWORD
#define ARG_IO_PI64         (ARG_IO_PTR|2)          // I/O pointer to 64 bit value

// !! KERNEL PSL ONLY !! ptr to const sized (< 64K) structure
#define ARG_CPTR            ARG_IO_PDW
#define ARG_PTR             ARG_IO_PDW

#define _ARG(arg, inx)  ((ULONGLONG) ARG_ ## arg << ARG_TYPE_BITS*(inx+1))
#define FNSIG0()    0
#define FNSIG1(a0)  (_ARG(a0,0)|1)
#define FNSIG2(a0, a1)  (_ARG(a0,0)|_ARG(a1,1)|2)
#define FNSIG3(a0, a1, a2)  (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|3)
#define FNSIG4(a0, a1, a2, a3)  (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|4)
#define FNSIG5(a0, a1, a2, a3, a4) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|5)
#define FNSIG6(a0, a1, a2, a3, a4, a5) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5)|6)
#define FNSIG7(a0, a1, a2, a3, a4, a5, a6) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5)|_ARG(a6,6)|7)
#define FNSIG8(a0, a1, a2, a3, a4, a5, a6, a7) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5)|_ARG(a6,6) \
    |_ARG(a7,7)|8)
#define FNSIG9(a0, a1, a2, a3, a4, a5, a6, a7, a8) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5)|_ARG(a6,6) \
    |_ARG(a7,7)|_ARG(a8,8)|9)
#define FNSIG10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5)|_ARG(a6,6) \
    |_ARG(a7,7)|_ARG(a8,8)|_ARG(a9,9)|10)
#define FNSIG11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5)|_ARG(a6,6) \
    |_ARG(a7,7)|_ARG(a8,8)|_ARG(a9,9)|_ARG(a10,10)|11)
#define FNSIG12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5)|_ARG(a6,6) \
    |_ARG(a7,7)|_ARG(a8,8)|_ARG(a9,9)|_ARG(a10,10)|_ARG(a11,11)|12)
#define FNSIG13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5)|_ARG(a6,6) \
    |_ARG(a7,7)|_ARG(a8,8)|_ARG(a9,9)|_ARG(a10,10)|_ARG(a11,11)|_ARG(a12,12)|13)

// KPage definitions
#define TH_KMODE 1
#define TH_UMODE 0

#define CURTLSPTR_OFFSET 0x000
#define PWR_FLAG_OFFSET  0x086
#define KINFO_OFFSET     0x300

// locale information
#define SYSLOCALE_OFFSET        0xc0
#define USRLOCALE_OFFSET        0x04

// misc
#define RAWTIMEOFFSET_OFFSET    0x190

#ifdef ARM
#define ARMArchitectureV6       0x7
#define ARCHID_OFFSET           0x02a0
#define IsVirtualTaggedCache()  (__GetUserKData (ARCHID_OFFSET) < ARMArchitectureV6)
#endif

#define UCurThread()            ((HANDLE) __GetUserKData (SYSHANDLE_OFFSET + (SH_CURTHREAD * sizeof(HANDLE))))
#define UTlsPtr()               ((LPDWORD) __GetUserKData (CURTLSPTR_OFFSET))

#define TLSSLOT_MSGQUEUE    0
#define TLSSLOT_RUNTIME     1
#define TLSSLOT_KERNEL      2

#define TLSSLOT_ICECAP      3

#define TLSKERN_TRYINGTODIE     0x00000001
#define TLSKERN_NOFAULT         0x00000002  // Don't break into the debugger if set
#define TLSKERN_CECAP_PROFILING 0x00000004  // CECAP is currently profiling
#define TLSKERN_DIRECTCALL      0x00000008  // set when making a direct call to device driver.
#define TLSKERN_NOFAULTMSG      0x00000010  // used with TLSKERN_NOFAULT, skip the display of exception message

#define TLSSLOT_NUMRES      4    // number of reserved slots (must be in order from 0)
#define TLSSLOT_RESERVE     0x0F // bitfield - reserve low 3 slots

// We move the stack base/bound as well as Current running Fiber directly
// preceeding the TLS data so that coredll can access them and implement
// fast Fiber switching
#define PRETLS_STACKBASE        -1   // stack base now moved to PRE-TLS block
#define PRETLS_STACKBOUND       -2   // stack bound now moved to PRE-TLS block
#define PRETLS_CURFIBER         -3   // current running fiber in PRE-TLS block

// NOTE: we only allow threads with default stack size to be converted to fiber,
//       and all fibers have the stack size of the default process stack size
#define PRETLS_STACKSIZE        -4   // process default stack size

#define PRETLS_THRDINFO         -5   // current thread's information (bit fields, only bit 0 used for now)

#define UTLS_INKMODE            0x00000001  // bit 1 set if in kmode
#define UTLS_NKCALLOUT          0x00000002  // bit 2 set if NK callout to FILESYS

// this is for optimization of GetProcessVersion(GetOwnerThread()), used heavily for NLS APIs
#define PRETLS_OWNER_VERSION    -6   // Owner process's version number

// the follow 1 are onlid for User Mode Driver.
#define PRETLS_DRIVER_DIRECT_CALLER      -7  // Driver IO Direct Caller for User Mode Driver.

// how many field preceeding TLS is reserved 
#define PRETLS_RESERVED          8

// macros for accessing PRETLS data
#define UStkBase()               (UTlsPtr()[PRETLS_STACKBASE])
#define UStkBound()              (UTlsPtr()[PRETLS_STACKBOUND])
#define UCurFiber()              (UTlsPtr()[PRETLS_CURFIBER])
#define UStkSize()               (UTlsPtr()[PRETLS_STACKSIZE])

#define PSLIsNKCallOut()         (UTlsPtr()[PRETLS_THRDINFO] & UTLS_NKCALLOUT)
#define PSLGetCallerTrust        CeGetCallerTrust

#define IsInPwrHdlr()            (0 != PUserKData[PWR_FLAG_OFFSET])

#define IsSecureVa(va)      (FALSE)

#define SECTION_MASK        0x03F
#define VA_SECTION          25
#define UnMapPtr(P)         (P)

/* indices for for UserKInfo array in kernel data page */
#define UserKInfo           ((long *)(PUserKData+KINFO_OFFSET))

#define KINX_PROCARRAY  0   /* address of process array */
#define KINX_PAGESIZE   1   /* system page size */
#define KINX_PFN_SHIFT  2   /* shift for page # in PTE */
#define KINX_PFN_MASK   3   /* mask for page # in PTE */
#define KINX_PAGEFREE   4   /* # of free physical pages */
#define KINX_SYSPAGES   5   /* # of pages used by kernel */
#define KINX_KHEAP      6   /* ptr to kernel heap array */
#define KINX_SECTIONS   7   /* ptr to SectionTable array */
#define KINX_MEMINFO    8   /* ptr to system MemoryInfo struct */
#define KINX_MODULES    9   /* ptr to module list */
#define KINX_DLL_LOW   10   /* lower bound of DLL shared space */
#define KINX_NUMPAGES  11   /* total # of RAM pages */
#define KINX_PTOC      12   /* ptr to ROM table of contents */
#define KINX_KDATA_ADDR 13  /* kernel mode version of KData */
#define KINX_GWESHEAPINFO 14 /* Current amount of gwes heap in use */
#define KINX_TIMEZONEBIAS 15 /* Fast timezone bias info */
#define KINX_PENDEVENTS 16  /* bit mask for pending interrupt events */
#define KINX_KERNRESERVE 17 /* number of kernel reserved pages */
#define KINX_API_MASK 18    /* bit mask for registered api sets */
#define KINX_NLS_CP 19      /* hiword OEM code page, loword ANSI code page */
#define KINX_NLS_SYSLOC 20  /* Default System locale */
#define KINX_NLS_USERLOC 21 /* Default User locale */
#define KINX_HEAP_WASTE 22  /* Kernel heap wasted space */
#define KINX_DEBUGGER 23    /* For use by debugger for protocol communication */
#define KINX_APISETS 24     /* APIset pointers */
#define KINX_MINPAGEFREE 25 /* water mark of the minimum number of free pages */
#define KINX_CELOGSTATUS 26 /* CeLog status flags */
#define KINX_NKSECTION  27  /* Address of NKSection */
#define KINX_PTR_CURTOKEN    28  /* Events to be set after power on */

/* For RegisterApiSet */
#define REGISTER_APISET_TYPE 0x80000000

typedef struct _CALLBACKINFO {
    HANDLE  hProc;      /* destination process */
    FARPROC pfn;        /* function to call in dest. process */
    PVOID   pvArg0;     /* arg0 data */
    DWORD   dwErrRtn;   /* error return value on excpetion */
} CALLBACKINFO;
typedef CALLBACKINFO *PCALLBACKINFO;

#define ZeroPtrABS(P) (P)

#define ZeroPtr(P)  (P)

#define MapPtrWithBits(Ptr, Bits) (Ptr)

#define NKD                 NKDbgPrintfW

#define NKLOG(x)            NKD (L"%s == 0x%x\r\n", TEXT(#x), x)
#define NKLOGI64(x)         NKD (L"%s == 0x%016I64x\r\n", TEXT(#x), x)
#define NKLOGS(x)           NKD (L"%s == '%s'\r\n", TEXT(#x), x)

#define DEBUGLOG(z,x)       DEBUGMSG(z, (L"%s == 0x%x\r\n", TEXT(#x), x))
#define DEBUGLOGS(z,x)      DEBUGMSG(z, (L"%s == '%s'\r\n", TEXT(#x), x))
#define DEBUGLOGI64(z,x)    DEBUGMSG(z, (L"%s == 0x%I64x\r\n", TEXT(#x), x))


// valid flags for ReadPEHeader
#define RP_READ_PE_BY_MODULE    1                   // dwAddr field is a pointer to an HMODULE
#define RP_MUI_IF_EXIST         2                   // try MUI if exist

// return value for ReadPEHeader
#define RP_RESULT_FAILED        0                   // *MUST* be 0
#define RP_RESULT_FROM_MODULE   1                   // The PE header is from original module
#define RP_RESULT_FROM_MUI      2                   // The PE header is from MUI, when RP_MUI_IF_EXIST is specified


#define CECOMPRESS_ALLZEROS 0
#define CECOMPRESS_FAILED   0xffffffffUL
#define CEDECOMPRESS_FAILED 0xffffffffUL

/* DLLMAININFO structure */
typedef struct _DLLMAININFO {
    HMODULE     hLib;           // the module
    FARPROC     pDllMain;       // the function pointer to DllMain
    LPVOID      pBasePtr;       // base of the module
    DWORD       dwSect14rva;    // section14 rva
    DWORD       dwSect14size;   // section14 size
} DLLMAININFO, *PDLLMAININFO;

//
// internal loader flags
//
#define LLIB_NO_PAGING      0x0001
#define LLIB_NO_MUI         0x0002


/* Flags for SetHardwareWatch */

#define HARDWARE_WATCH_WRITE 0x1
#define HARDWARE_WATCH_READ  0x2

/* Flags for CacheSync/CacheRangeFlush */
#define CACHE_SYNC_DISCARD      0x001   /* write back & discard all cached data */
#define CACHE_SYNC_INSTRUCTIONS 0x002   /* discard all cached instructions */
#define CACHE_SYNC_WRITEBACK    0x004   /* write back but don't discard data cache*/
#define CACHE_SYNC_FLUSH_I_TLB  0x008   /* flush I-TLB */
#define CACHE_SYNC_FLUSH_D_TLB  0x010   /* flush D-TLB */
#define CACHE_SYNC_FLUSH_TLB    (CACHE_SYNC_FLUSH_I_TLB|CACHE_SYNC_FLUSH_D_TLB)    /* flush all TLB */
#define CACHE_SYNC_L2_WRITEBACK 0x020   /* write-back L2 Cache */
#define CACHE_SYNC_L2_DISCARD   0x040   /* discard L2 Cache */

#define CACHE_SYNC_ALL          0x07F   /* sync and discard everything in Cache/TLB */

// Print flags for resource tracking
#define PRINT_ALL           (DWORD)0x00000100
#define PRINT_RECENT        (DWORD)0x00000200
#define PRINT_DETAILS       (DWORD)0x00000400
#define PRINT_TRACE         (DWORD)0x00000800
#define PRINT_FILTERTYPE    (DWORD)0x00001000
#define PRINT_FILTERPROCID  (DWORD)0x00002000
#define PRINT_FILTERHANDLE  (DWORD)0x00004000
#define PRINT_SETCHECKPOINT (DWORD)0x00008000

// Filter flags for resource tracking
#define FILTER_TYPEDEFAULTON   0x00000001
#define FILTER_TYPEDEFAULTOFF  0x00000002
#define FILTER_TYPEON          0x00000004
#define FILTER_TYPEOFF         0x00000008
#define FILTER_PROCDEFAULTOFF  0x00000020   // not yet supported
#define FILTER_PROCIDON        0x00000040
#define FILTER_PROCIDOFF       0x00000080   // not yet supported

// Internal-only flags for CreateFileMapping
// Cannot conflict with other PAGE_* flags or SEC_COMMIT from winnt.h
#define PAGE_INTERNALDBMAPPING   0x20000000
#define PAGE_INTERNALNOFSPROTECT 0x40000000

// Flags for physical memory types. Used by IOCTL_QUERY_PHYSICALMEM
#define PHYSICAL_ROM        (DWORD)0x00000001
#define PHYSICAL_RAM        (DWORD)0x00000002
#define PHYSICAL_IO         (DWORD)0x00000004
#define PHYSICAL_UNKNOWN    (DWORD)0x00000008

// Flags for processor info used by IOCTL_PROCESSOR_INFORMATION
#define PROCESSOR_FLOATINGPOINT     (DWORD)0x00000001
#define PROCESSOR_DSP               (DWORD)0x00000002
#define PROCESSOR_16BITINSTRUCTION  (DWORD)0x00000004

// Kernel IOCTLs based on FILE_DEVICE_HAL -- last ID used is 82 (IOCTL_HAL_GET_POOL_PARAMETERS)
// Note that function codes 0-2047 are reserved for Microsoft Corporation, and
// 2048-4095 are reserved for customers..
// Internal product teams are currently using values in the range of 1000-1100
#define IOCTL_HAL_GET_DEVICE_INFO               CTL_CODE(FILE_DEVICE_HAL, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_GET_IP_ADDR                   CTL_CODE(FILE_DEVICE_HAL, 3, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_RADIO_CNTRL                   CTL_CODE(FILE_DEVICE_HAL, 4, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_DDK_CALL                      CTL_CODE(FILE_DEVICE_HAL, 5, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_EDBG_REGISTER_CLIENT              CTL_CODE(FILE_DEVICE_HAL, 6, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EDBG_DEREGISTER_CLIENT            CTL_CODE(FILE_DEVICE_HAL, 7, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EDBG_REGISTER_DFLT_CLIENT         CTL_CODE(FILE_DEVICE_HAL, 8, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EDBG_SEND                         CTL_CODE(FILE_DEVICE_HAL, 9, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EDBG_RECV                         CTL_CODE(FILE_DEVICE_HAL, 10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EDBG_SET_DEBUG                    CTL_CODE(FILE_DEVICE_HAL, 11, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_KITL_STARTUP                      CTL_CODE(FILE_DEVICE_HAL, 78, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_KITL_INTRINIT                     CTL_CODE(FILE_DEVICE_HAL, 79, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_KITL_IS_KDBG_REGISTERED           CTL_CODE(FILE_DEVICE_HAL, 80, METHOD_BUFFERED, FILE_ANY_ACCESS)

// KITL PowerOn/Off ioctl, *lpInBuf = 0 for off, 1 for on.
#define IOCTL_KITL_POWER_CALL                   CTL_CODE(FILE_DEVICE_HAL, 81, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SET_KERNEL_COMM_DEV               CTL_CODE(FILE_DEVICE_HAL, 12, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_GET_UUID                      CTL_CODE(FILE_DEVICE_HAL, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_INIT_RTC                      CTL_CODE(FILE_DEVICE_HAL, 14, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_REBOOT                        CTL_CODE(FILE_DEVICE_HAL, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_HALT                          CTL_CODE(FILE_DEVICE_HAL, 16, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SET_KERNEL_DEV_PORT               CTL_CODE(FILE_DEVICE_HAL, 17, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_TRANSLATE_IRQ                 CTL_CODE(FILE_DEVICE_HAL, 18, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_GET_RIO_INFO                  CTL_CODE(FILE_DEVICE_HAL, 19, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_ILTIMING                      CTL_CODE(FILE_DEVICE_HAL, 20, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_GET_DEVICEID                  CTL_CODE(FILE_DEVICE_HAL, 21, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_PROFILE                       CTL_CODE(FILE_DEVICE_HAL, 22, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_SET_DEVICE_INFO               CTL_CODE(FILE_DEVICE_HAL, 23, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_QUERY_PHYSICALMEM                 CTL_CODE(FILE_DEVICE_HAL, 24, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PROCESSOR_INFORMATION             CTL_CODE(FILE_DEVICE_HAL, 25, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_GETREGSECUREKEYS              CTL_CODE(FILE_DEVICE_HAL, 35, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_REQUEST_IRQ                   CTL_CODE(FILE_DEVICE_HAL, 36, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_REQUEST_SYSINTR               CTL_CODE(FILE_DEVICE_HAL, 38, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_RELEASE_SYSINTR               CTL_CODE(FILE_DEVICE_HAL, 54, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to get KITL transport info
#define IOCTL_KITL_GET_INFO                     CTL_CODE(FILE_DEVICE_HAL, 37, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_ENABLE_WAKE                   CTL_CODE(FILE_DEVICE_HAL, 40, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_DISABLE_WAKE                  CTL_CODE(FILE_DEVICE_HAL, 41, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_GET_WAKE_SOURCE               CTL_CODE(FILE_DEVICE_HAL, 42, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_PRESUSPEND                    CTL_CODE(FILE_DEVICE_HAL, 61, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to tell OEM to initialize registry on boot
#define IOCTL_HAL_INITREGISTRY                  CTL_CODE(FILE_DEVICE_HAL, 44, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to give OEM a more full-featured init when the kernel is ready to go
// and before any apps are started
#define IOCTL_HAL_POSTINIT                      CTL_CODE(FILE_DEVICE_HAL, 45, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to ask OEM whether to force clean registry hive
#define IOCTL_HAL_GET_HIVE_CLEAN_FLAG           CTL_CODE(FILE_DEVICE_HAL, 49, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to ask OEM the reason for power on
#define IOCTL_HAL_GET_POWERONREASON             CTL_CODE(FILE_DEVICE_HAL, 50, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to get random hardware seed
#define IOCTL_HAL_GET_RANDOM_SEED               CTL_CODE(FILE_DEVICE_HAL, 52, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to ask OEM for a RAM region to put the registry hive into
#define IOCTL_HAL_GET_HIVE_RAM_REGION           CTL_CODE(FILE_DEVICE_HAL, 53, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to tell OEM to save the RAM region where the registry hive is stored
#define IOCTL_HAL_SAVE_HIVE_RAM_REGION          CTL_CODE(FILE_DEVICE_HAL, 55, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to write XIP section.
#define IOCTL_HAL_WRITE_XIP                     CTL_CODE(FILE_DEVICE_HAL, 46, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_XIPCHAIN                      CTL_CODE(FILE_DEVICE_HAL, 47, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL for authenticate device
#define IOCTL_HAL_AUTHENTICATE_DEVICE           CTL_CODE(FILE_DEVICE_HAL, 60, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL for WHQL testing (used to check whether hardware is in a jumpered test mode)
#define IOCTL_HAL_WHQL_TEST_MODE                CTL_CODE(FILE_DEVICE_HAL, 56, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL for retrieving location and size of bootloader in flash
#define IOCTL_HAL_GET_BOOTLOADER_PARAMS         CTL_CODE(FILE_DEVICE_HAL, 57, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL for retrieving the signed state of an OS image
#define IOCTL_HAL_GET_IMAGE_SIGNED_STATE        CTL_CODE(FILE_DEVICE_HAL, 58, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to query for a default display resolution.
#define IOCTL_HAL_QUERY_DISPLAYSETTINGS         CTL_CODE(FILE_DEVICE_HAL, 63, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to query device for device-specific information
#define IOCTL_HAL_QUERY_DEVICE_SETTINGS         CTL_CODE(FILE_DEVICE_HAL, 64, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to allow the HAL to override the extension region
#define IOCTL_HAL_GET_BIN_CHAIN                 CTL_CODE(FILE_DEVICE_HAL, 65, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to retrieve cache information
#define IOCTL_HAL_GET_CACHE_INFO                CTL_CODE(FILE_DEVICE_HAL, 66, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to control OEM-specified profiler
#define IOCTL_HAL_OEM_PROFILER                  CTL_CODE(FILE_DEVICE_HAL, 67, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to allow the networking component interrupts to use one SYSINTR_NETWORK_SHARED
#define IOCTL_HAL_SYSINTR_NETWORK_SHARED        CTL_CODE(FILE_DEVICE_HAL, 68, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to allow OEM specify MAC address for RNDIS
#define IOCTL_HAL_GET_RNDIS_MACADDR             CTL_CODE(FILE_DEVICE_HAL, 69, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to allow OEM's to specify if a partition is to be formated before being mounted.
#define IOCTL_HAL_QUERY_FORMAT_PARTITION        CTL_CODE(FILE_DEVICE_HAL, 70, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL used by halflat display driver
#define IOCTL_HAL_DDI                           CTL_CODE(FILE_DEVICE_HAL, 71, METHOD_BUFFERED, FILE_ANY_ACCESS)

// This IOCTL enables/disables update mode.  It's used when updating system images and it sets/clears a flag in
// an OEM-defined location that's interpreted by the initial program loader (IPL) at boot time.  Based on the
// flag, the IPL will either boot the OS image or it'll load the update loader which starts the update process.
//
// The IOCTL takes a single boolean input parameter: TRUE == update mode and FALSE == normal OS
//
#define IOCTL_HAL_UPDATE_MODE                   CTL_CODE(FILE_DEVICE_HAL, 77, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL used by kernel to query page pool parameters from the OAL
// The IOCTL takes no input parameters but returns an NKPoolParameters struct as output.
#define IOCTL_HAL_GET_POOL_PARAMETERS           CTL_CODE(FILE_DEVICE_HAL, 82, METHOD_BUFFERED, FILE_ANY_ACCESS)

////////////////////////////////////////////////////////////////////////////////
//  VBridge support in kernel, used by VMini (Null miniport driver).
//
#define VMINI_RX_EVENT                          TEXT("VMiniRxEvent")    //  VMini miniport waits for this.
#define EDBG_INTERRUPT_EVENT                    TEXT("EdbgIntEvent")    //  Named event EDGB wait for.
#define IOCTL_VBRIDGE_GET_TX_PACKET             CTL_CODE(FILE_DEVICE_HAL, 26, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_GET_TX_PACKET_COMPLETE    CTL_CODE(FILE_DEVICE_HAL, 27, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_GET_RX_PACKET             CTL_CODE(FILE_DEVICE_HAL, 28, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_GET_RX_PACKET_COMPLETE    CTL_CODE(FILE_DEVICE_HAL, 29, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_GET_ETHERNET_MAC          CTL_CODE(FILE_DEVICE_HAL, 30, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_ADD_MAC                   CTL_CODE(FILE_DEVICE_HAL, 31, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_SHARED_ETHERNET           CTL_CODE(FILE_DEVICE_HAL, 32, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_CURRENT_PACKET_FILTER     CTL_CODE(FILE_DEVICE_HAL, 33, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_802_3_MULTICAST_LIST      CTL_CODE(FILE_DEVICE_HAL, 34, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_WILD_CARD                 CTL_CODE(FILE_DEVICE_HAL, 39, METHOD_BUFFERED, FILE_ANY_ACCESS)


//
//  These are only handled in vbridge.lib
//

#define IOCTL_VBRIDGE_WILD_CARD_RESET_BUFFER    CTL_CODE(FILE_DEVICE_HAL, 42, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_WILD_CARD_VB_INITIALIZED  CTL_CODE(FILE_DEVICE_HAL, 43, METHOD_BUFFERED, FILE_ANY_ACCESS)

//  Ioctl to retrieve OutputDebugString
#define IOCTL_EDBG_GET_OUTPUT_DEBUG_FN          CTL_CODE(FILE_DEVICE_HAL, 48, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_EDBG_IS_STARTED                   CTL_CODE(FILE_DEVICE_HAL, 51, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to retrieve hardware-generated entropy
#define IOCTL_HAL_GET_HWENTROPY                 CTL_CODE(FILE_DEVICE_HAL, 62, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Use as input parameter to IOCTL_EDBG_REGISTER_CLIENT
typedef struct _EDBG_REGISTER_CLIENT_IN_
{
    LPSTR szServiceName;
    UCHAR Flags;
    UCHAR WindowSize;
} EDBG_REGISTER_CLIENT_IN, *PEDBG_REGISTER_CLIENT_IN;

// Used as input parameter to IOCTL_EDBG_REGISTER_DFLT_CLIENT
typedef struct _EDBG_REGISTER_DFLT_CLIENT_IN_
{
    UCHAR Service;
    UCHAR Flags;
} EDBG_REGISTER_DFLT_CLIENT_IN, *PEDBG_REGISTER_DFLT_CLIENT_IN;

// Used as input parameters to IOCTL_EDBG_SEND
typedef struct _EDBG_SEND_IN_
{
    UCHAR Id;    
    UCHAR *pUserData;
    DWORD dwUserDataLen;
} EDBG_SEND_IN, *PEDBG_SEND_IN;

// Used as input parameters to IOCTL_EDBG_RECV
typedef struct _EDBG_RECV_IN_
{
    UCHAR Id;
    DWORD dwTimeout;
} EDBG_RECV_IN, *PEDBG_RECV_IN;

// Used as output parameters to IOCTL_EDBG_RECV
typedef struct _EDBG_RECV_OUT_
{
    UCHAR *pRecvBuf;
    DWORD *pdwLen;
} EDBG_RECV_OUT, *PEDBG_RECV_OUT;

// Used by IOCTL_HAL_GET_IMAGE_SIGNED_STATE
typedef enum
{
    IMAGE_NOT_SIGNED = 0,
    IMAGE_TEST_SIGNED,
    IMAGE_FINAL_SIGNED
} IMAGE_SIGNED_STATE;

// Used by IOCTL_HAL_GET_BOOTLOADER_PARAMS
typedef struct _EBOOT_PARAMS_
{
    DWORD dwEbootStartAddress;
    DWORD dwEbootSize;
    DWORD dwEbootROMOffset;
} EBOOT_PARAMS, *PEBOOT_PARAMS;

// Used by IOCTL_QUERY_PHYSICALMEM
typedef struct _PHYSICAL_BASIC_INFORMATION {
    PVOID   lpBaseAddress;
    DWORD   dwRegionSize;
    DWORD   dwType;
} PHYSICAL_BASIC_INFORMATION;

typedef PHYSICAL_BASIC_INFORMATION *PPHYSICAL_BASIC_INFORMATION;

// Used by IOCTL_PROCESSOR_INFORMATION
typedef struct _PROCESSOR_INFO {
    WORD    wVersion;
    WCHAR   szProcessCore[40];
    WORD    wCoreRevision;
    WCHAR   szProcessorName[40];
    WORD    wProcessorRevision;
    WCHAR   szCatalogNumber[100];
    WCHAR   szVendor[100];
    DWORD   dwInstructionSet;
    DWORD   dwClockSpeed;
} PROCESSOR_INFO;

typedef PROCESSOR_INFO *PPROCESSOR_INFO;

// Used by IOCT_HAL_GET_IRQ to assign an IRQ resource to a device
//
// This structure describes the hardware location of a device in terms of it's interface,
// bus number and logical location, and is typically used for describing PCI devices.  The
// Pin member describes which pin (1-4) of a PCI device is being used for the interrupt.
// The PhysicalLoc member is reserved for future implementations.

// The BusNumber parameter is used to identify the bus that the device is on.  For PCIbus,
// the bits of the DWORD are defined as:
// 31-16: 0
// 15-8 : host-to-pci-bridge number
// 7-0  : subordinate bus number (this is duplicated in LogicalLoc for BC reasons)

// The LogicalLoc parameter is used to identify the location of the device on the
// interface/bus.  For PCIbus, the bits of the DWORD are defined as:
// 31-24: 0
// 23-16: subordinate bus number
// 15-8 : device number
// 7-0  : function number

typedef struct _DEVICE_LOCATION {
    DWORD IfcType;                  // One of the INTERFACE_TYPE enum, typically PCIBus
    DWORD BusNumber;                // Bus number, typically the PCI bus number the device resides on
    DWORD LogicalLoc;               // Defines logical location of device.  See above for PCIbus example.
    PVOID PhysicalLoc;              // Reserved for future use
    DWORD Pin;                      // PCIbus IRQ pin
} DEVICE_LOCATION, *PDEVICE_LOCATION;

typedef struct _DEVICE_PSL_NOTIFY {
    DWORD   dwSize;
    DWORD   dwFlags;
    HANDLE  hProc;
    HANDLE  hThread;
} DEVICE_PSL_NOTIFY, *PDEVICE_PSL_NOTIFY;

#define IOCTL_PSL_NOTIFY CTL_CODE(FILE_DEVICE_PSL, 255, METHOD_NEITHER, FILE_ANY_ACCESS)

typedef struct _DEVICE_ID {
    DWORD   dwSize;
    DWORD   dwPresetIDOffset;
    DWORD   dwPresetIDBytes;
    DWORD   dwPlatformIDOffset;
    DWORD   dwPlatformIDBytes;
} DEVICE_ID, *PDEVICE_ID;


// Used by IOCTL_HAL_GET_POOL_PARAMETERS

typedef struct {
    DWORD Target;
    DWORD Maximum;
    DWORD ReleaseIncrement;
    DWORD CriticalIncrement;
    WORD  NormalPriority256;
    WORD  CriticalPriority256;
} PagePoolParameters;

typedef struct {
    WORD NKVersion;  // Kernel sets this to 1
    WORD OEMVersion; // OEM must set this to 1
    PagePoolParameters Loader;
    PagePoolParameters File;
} NKPagePoolParameters;


// Used by IOCTL_KLIB_GET_POOL_STATE

typedef struct {
    DWORD CurrentSize;      // Amount of memory (bytes) currently in use by the pool
    DWORD TrimCount;        // # times the Target was exceeded and trim thread ran
    DWORD TrimTime;         // Amount of time (ms) the trim thread has run
    DWORD CriticalCount;    // # times the thread went into critical mode
    DWORD CriticalTime;     // Amount of time (ms) trim thread has run at critical prio
    DWORD FailCount;        // # allocs that failed for lack of memory
} PagePoolState;

typedef struct {
    WORD Version;   // Caller must set this to 1
    WORD Reserved;  // unused
    PagePoolParameters LoaderParams;
    PagePoolState      LoaderState;
    PagePoolParameters FileParams;
    PagePoolState      FileState;
} NKPagePoolState;


////////////////////////////////////////////////////////////////////////////////
// Types used by IOCTL_KLIB_CACHE_REGISTER
//

#define KERNEL_CACHE_VERSION 1

typedef struct {
    ULARGE_INTEGER liMapOffset; // Map offset where the view started
    DWORD   cbSize;             // size of the view
    LPBYTE  pBaseAddr;          // page-aligned base address of the view inside the process
    DWORD   dwDirty;            // # of dirty pages in this view
} CommonMapView_t;

// Flush flags can be combined together
#define MAP_FLUSH_WRITEBACK         (0x00000001)    // Flush R/W data to storage
#define MAP_FLUSH_DECOMMIT_RO       (0x00000002)    // Decommit R/O pages
#define MAP_FLUSH_DECOMMIT_RW       (0x00000004)    // Decommit R/W pages
#define MAP_FLUSH_FAIL_IF_COMMITTED (0x00000008)    // Return failure if any committed pages remain

// FS exports for NK to use
typedef BOOL   (*pfnFSAsyncFlushCacheMapping) (DWORD FSSharedFileMapId, DWORD FlushFlags);
typedef BOOL   (*pfnFSGetCacheView)           (DWORD dwAddr, DWORD* pNKSharedFileMapId, CommonMapView_t** ppview);
typedef BOOL   (*pfnFSReadWriteCacheData)     (DWORD FSSharedFileMapId, PBYTE pBuffer, DWORD cbToAccess, DWORD* pcbAccessed, const ULARGE_INTEGER* pliFileOffset, BOOL IsWrite);
typedef void   (*pfnFSLockUnlockFile)         (DWORD FSSharedFileMapId, BOOL Lock);
typedef BOOL   (*pfnFSFlushFile)              (DWORD FSSharedFileMapId);

// NK exports for FS to use
typedef LPVOID (*pfnNKAllocCacheVM)           (DWORD cbAlloc);
typedef void   (*pfnNKNotifyFSReady)          ();
typedef DWORD  (*pfnNKCreateCacheMapping)     (DWORD FSSharedFileMapId, const ULARGE_INTEGER* pcbMapSize, DWORD flProtect);
typedef BOOL   (*pfnNKPreCloseCacheMapping)   (DWORD NKSharedFileMapId);
typedef BOOL   (*pfnNKCloseCacheMapping)      (DWORD NKSharedFileMapId);
typedef BOOL   (*pfnNKGetCacheMappingSize)    (DWORD NKSharedFileMapId, ULARGE_INTEGER* pcbFileSize);
typedef BOOL   (*pfnNKSetCacheMappingSize)    (DWORD NKSharedFileMapId, const ULARGE_INTEGER* pcbNewFileSize);
typedef BOOL   (*pfnNKFlushCacheView)         (DWORD NKSharedFileMapId, CommonMapView_t* pView, PVOID* pPageList, DWORD NumPageListEntries, DWORD FlushFlags);
typedef BOOL   (*pfnNKDebugCheckCSUsage)      ();

typedef struct {
    DWORD                       Version;   // Should be set to KERNEL_CACHE_VERSION
    pfnFSAsyncFlushCacheMapping pAsyncFlushCacheMapping;
    pfnFSGetCacheView           pGetCacheView;
    pfnFSReadWriteCacheData     pReadWriteCacheData;
    pfnFSLockUnlockFile         pLockUnlockFile;
    pfnFSFlushFile              pFlushFile;
} FSCacheExports;

typedef struct {
    DWORD                       Version;   // Should be set to KERNEL_CACHE_VERSION
    pfnNKAllocCacheVM           pAllocCacheVM;
    pfnNKNotifyFSReady          pNotifyFSReady;
    pfnNKCreateCacheMapping     pCreateCacheMapping;
    pfnNKPreCloseCacheMapping   pPreCloseCacheMapping;
    pfnNKCloseCacheMapping      pCloseCacheMapping;
    pfnNKGetCacheMappingSize    pGetCacheMappingSize;
    pfnNKSetCacheMappingSize    pSetCacheMappingSize;
    pfnNKFlushCacheView         pFlushCacheView;
    pfnNKDebugCheckCSUsage      pDebugCheckCSUsage;     // DEBUG PURPOSES ONLY
} NKCacheExports;

////////////////////////////////////////////////////////////////////////////////

typedef void (*TRACKER_CALLBACK)(DWORD dwFlags, DWORD dwType, HANDLE hItem,
    DWORD dwProc, BOOL fDeleted, DWORD dwSize, DWORD dw1, DWORD dw2);

#define PerformCallBack PRIV_WIN32_CALL(DWORD, PerformCallBack, (PCALLBACKINFO pcbi, ...))
#define PerformCallForward PRIV_WIN32_CALL(DWORD, PerformCallBack, (PCALLBACKINFO pcbi, ...))
DWORD PerformCallBack4(PCALLBACKINFO pcbi, ...);
#ifdef COREDLL
DWORD
xxx_PerformCallBack4(
    PCALLBACKINFO   pcbi,
    LPVOID          p2,
    LPVOID          p3,
    LPVOID          p4
    );
#define PerformCallBack4 xxx_PerformCallBack4
#endif

BOOL CeSetExtendedPdata(LPVOID pData);

void ForcePageout(void);
HANDLE CreateAPISet(char acName[4], USHORT cFunctions, const PFNVOID *ppfnMethods, const ULONGLONG *pu64Sig);
BOOL RegisterDirectMethods (HANDLE hApiSet, const PFNVOID *ppfnDirectMethod);
BOOL VirtualCopy(LPVOID lpvDest, LPVOID lpvSrc, DWORD cbSize, DWORD fdwProtect);
BOOL VirtualCopyEx(HANDLE hDstProc, LPVOID lpvDest, HANDLE hSrcProc, LPVOID lpvSrc, DWORD cbSize, DWORD fdwProtect);
LPVOID VirtualAllocCopyEx (HANDLE hSrcProc, HANDLE hDstProc, LPVOID pAddr, DWORD cbSize, DWORD dwProtect);

/** Flags for LockPages: */
#define LOCKFLAG_WRITE      0x001   // write access required
#define LOCKFLAG_QUERY_ONLY 0x002   // query only, page in but don't lock
#define LOCKFLAG_READ       0x004   // read access required (as opposed to page present but PAGE_NOACCESS)

BOOL LockPages(LPVOID lpvAddress, DWORD cbSize, PDWORD pPFNs, int fOptions);
BOOL UnlockPages(LPVOID lpvAddress, DWORD cbSize);

LPVOID AllocPhysMem(DWORD cbSize, DWORD fdwProtect, DWORD dwAlignmentMask, DWORD dwFlags, PULONG pPhysicalAddress);
BOOL   FreePhysMem(LPVOID lpvAddress);

void SleepTillTick (void);

HRESULT
CeOpenCallerBuffer(
    PVOID* ppDestMarshalled,
    PVOID  pSrcUnmarshalled,
    DWORD  cbSrc,
    DWORD  ArgumentDescriptor,
    BOOL   ForceDuplicate
    );

HRESULT
CeCloseCallerBuffer(
    PVOID  pDestMarshalled,
    PVOID  pSrcUnmarshalled,
    DWORD  cbSrc,
    DWORD  ArgumentDescriptor
    );

//
// In Kernel mode, this following flag can be or'd with ARG_XXX for ArgumentDescriptor in calls to
//      CeAllocAsynchronousBuffer
// to force the source and destination to be aliased instead of copy-in/copy-out.
//
// NOTE: the flag must be included in the calls to
//      CeFreeAsynchronousBuffer
//      CeFlushAsynchronousBuffer
// on the buffer you allocated with the flag.
//
// And the flag has no effect (ignored) if used in any other marshal helper APIs.
// 
#define MARSHAL_FORCE_ALIAS         0x80000000

HRESULT
CeAllocAsynchronousBuffer(
    PVOID* ppDestAsyncMarshalled,
    PVOID  pSrcSyncMarshalled,
    DWORD  cbSrc,
    DWORD  ArgumentDescriptor
    );

HRESULT
CeFreeAsynchronousBuffer(
    PVOID  pDestAsyncMarshalled,
    PVOID  pSrcSyncMarshalled,
    DWORD  cbSrc,
    DWORD  ArgumentDescriptor
    );

HRESULT
CeFlushAsynchronousBuffer(
    PVOID  pDestAsyncMarshalled,
    PVOID  pSrcSyncMarshalled,
    PVOID  pSrcUnmarshalled,
    DWORD  cbSrc,
    DWORD  ArgumentDescriptor
    );

HRESULT
CeAllocDuplicateBuffer(
    PVOID* ppDestDuplicate,
    PVOID  pSrcMarshalled,
    DWORD  cbSrc,
    DWORD  ArgumentDescriptor
    );

HRESULT
CeFreeDuplicateBuffer(
    PVOID  pDestDuplicate,
    PVOID  pSrcMarshalled,
    DWORD  cbSrc,
    DWORD  ArgumentDescriptor
    );


#ifdef SH4
#define VSPF_NONE       0x00000000
#define VSPF_VARIABLE   0x00000001
#define VSPF_8BIT       0x00000000  // default
#define VSPF_16BIT      0x00000002
#define VSPF_IOSPACE    0x00000004
#define VSPF_COMMON     0x00000008
#define VSPF_ATTRIBUTE  0x0000000c
#define VSPF_TC         0x00000010
BOOL VirtualSetPageFlags(LPVOID lpvAddress, DWORD cbSize, DWORD dwFlags, LPDWORD lpdwOldFlags);
BOOL SetRAMMode(BOOL bEnable, LPVOID *lplpvAddress, LPDWORD lpLength);
LPVOID SetStoreQueueBase(DWORD dwPhysPage);
#endif
BOOL VirtualSetAttributes (LPVOID lpvAddress, DWORD cbSize, DWORD dwNewFlags, DWORD dwMask, LPDWORD lpdwOldFlags);
BOOL NKVirtualSetAttributes(LPVOID lpvAddress, DWORD cbSize, DWORD dwNewFlags, DWORD dwMask, LPDWORD lpdwOldFlags);
LPVOID CeVirtualSharedAlloc (LPVOID lpvAddr, DWORD cbSize, DWORD fdwAction);

DWORD CeModuleJit(LPCWSTR, LPWSTR, HANDLE*);
int U_ropen(const WCHAR *, UINT);
int U_rread(int, BYTE *, int);
int U_rwrite(int, BYTE *, int);
int U_rlseek(int, int, int);
int U_rclose(int);
VOID NKvDbgPrintfW(LPCWSTR lpszFmt, va_list lpParms);
BOOL GetRealTime(LPSYSTEMTIME);
BOOL SetRealTime(LPSYSTEMTIME);
LPVOID ExtractResource(LPCWSTR lpszFile, LPCWSTR lpszName, LPCWSTR lpszType);
UINT KernExtractIcons(LPCWSTR lpszFile, int nIconIndex, LPBYTE *pIconLarge, LPBYTE *pIconSmall, CALLBACKINFO *pcbi);
BOOL GetRomFileInfo(DWORD type, LPWIN32_FIND_DATA  lpfd, DWORD count);
DWORD GetRomFileBytes(DWORD type, DWORD count, DWORD pos, LPVOID buffer, DWORD nBytesToRead);
#ifdef _WIN32_WCE_EMULATION
    #define CacheSync(x)
    #define CacheRangeFlush(a, b, c)
#else
void CacheSync(int flags);
void CacheRangeFlush (LPVOID pAddr, DWORD dwLength, DWORD dwFlags);
#endif
BOOL AddTrackedItem(DWORD dwType, HANDLE handle, TRACKER_CALLBACK cb, DWORD dwProc, DWORD dwSize, DWORD dw1, DWORD dw2);
BOOL DeleteTrackedItem(DWORD dwType, HANDLE handle);
BOOL PrintTrackedItem(DWORD dwFlag, DWORD dwType, DWORD dwProcID, HANDLE handle);
BOOL GetKPhys(void *ptr, ULONG length);
BOOL GiveKPhys(void *ptr, ULONG length);
#ifndef _WIN32_WCE_EMULATION
void SetExceptionHandler(PEXCEPTION_ROUTINE per);
#endif
DWORD RegisterTrackedItem(LPWSTR szName);
VOID FilterTrackedItem(DWORD dwFlags, DWORD dwType, DWORD dwProcID);
void SetKernelAlarm(HANDLE hEvent, LPSYSTEMTIME lpst);
void RefreshKernelAlarm(void);
void CloseProcOE(DWORD);

// New API is SetOOMEvent(), SetGwesOOMEvent will go away.
VOID SetGwesOOMEvent(HANDLE hEvt, DWORD cpLow, DWORD cpCritical, DWORD cpLowBlockSize, DWORD cpCriticalBlockSize);
VOID SetOOMEvent(HANDLE hEvt, DWORD cpLow, DWORD cpCritical, DWORD cpLowBlockSize, DWORD cpCriticalBlockSize);
DWORD StringCompress(LPBYTE bufin, DWORD lenin, LPBYTE bufout, DWORD lenout);
DWORD StringDecompress(LPBYTE bufin, DWORD lenin, LPBYTE bufout, DWORD lenout);
DWORD BinaryCompress(LPBYTE bufin, DWORD lenin, LPBYTE bufout, DWORD lenout);
DWORD BinaryDecompress(LPBYTE bufin, DWORD lenin, LPBYTE bufout, DWORD lenout, DWORD skip);
DWORD DecompressBinaryBlock(LPBYTE bufin, DWORD lenin, LPBYTE bufout, DWORD lenout);
int InputDebugCharW(VOID);
void TakeCritSec(LPCRITICAL_SECTION lpcs);
void LeaveCritSec(LPCRITICAL_SECTION lpcs);
LPVOID MapPtrToProcess(LPVOID lpv, HANDLE hProc);
LPVOID MapPtrUnsecure(LPVOID lpv, HANDLE hProc);
HANDLE GetProcFromPtr(LPVOID lpv);
BOOL IsBadPtr(DWORD flags, LPBYTE ptr, UINT length);
DWORD GetProcAddrBits(HANDLE hProc);
DWORD GetFSHeapInfo(void);
BOOL OtherThreadsRunning(void);
VOID KillAllOtherThreads(void);
HANDLE GetOwnerProcess(void);

//
// GetCallerProcess is ambiguous for it can mean "direct caller" or "caller's VM". For
// now GetCallerProcess is now mapped to "GetDirectCallerProcessId". It's strongly
// recommended that you replace GetCalelrProcess with GetDirectCallerProcessId or
// GetCallerVMProcessId
//
HANDLE GetCallerProcess(void);

DWORD GetCallerVMProcessId (void);
DWORD GetDirectCallerProcessId (void);

//
// The following APIs are no longer supported - we don't have a 32 process limit
// and there is no usch thing as "process index" any more.
//
DWORD GetCallerProcessIndex(void);
DWORD GetProcessIndexFromID(HANDLE hProc);
HANDLE GetProcessIDFromIndex(DWORD dwIdx);
#define GetCurrentProcessIndex() GetProcessIndexFromID(GetCurrentProcess())

DWORD SetLowestScheduledPriority(DWORD maxprio);
BOOL IsPrimaryThread(void);
LPBYTE CreateLocaleView(LPDWORD pdwSize);
DWORD SetProcPermissions(DWORD);
DWORD GetCurrentPermissions(void);
void SetTimeZoneBias(DWORD dwBias, DWORD dwDaylightBias);
void SetCleanRebootFlag(void);
HANDLE CreateCrit(LPCRITICAL_SECTION lpcs);
void PowerOffSystem(void);
BOOL SetDbgZone(DWORD dwProcid, LPVOID lpvMod, LPVOID baseptr, DWORD zone, LPDBGPARAM lpdbgTgt);
VOID TurnOnProfiling(HANDLE hThread);
VOID WINAPI TurnOffProfiling(HANDLE hThread);
LPCWSTR GetProcName(void);
void CloseAllHandles(void);
BOOL SetHandleOwnerWorkaround (HANDLE *ph, HANDLE hProc);
HINSTANCE LoadDriver(LPCTSTR lpszFile);
HANDLE LoadIntChainHandler(LPCTSTR lpszFileName, LPCTSTR lpszFunctionName, BYTE bIRQ);
BOOL FreeIntChainHandler(HANDLE hInstance);
BOOL IntChainHandlerIoControl(HANDLE hLib,DWORD dwIoControlCode,LPVOID lpInBuf,DWORD nInBufSize,LPVOID lpOutBuf,DWORD nOutBufSize,LPDWORD lpBytesReturned);
HANDLE LoadKernelLibrary(LPCWSTR lpszFileName);
BOOL InterruptInitialize(DWORD idInt, HANDLE hEvent, LPVOID pvData, DWORD cbData);
void InterruptMask (DWORD idInt, BOOL fDisable);
void InterruptDone(DWORD idInt);
void InterruptDisable(DWORD idInt);
DWORD SetKMode(DWORD);
BOOL SetPowerOffHandler(FARPROC pfn);
BOOL SetGwesPowerHandler(FARPROC pfn);
BOOL ConnectDebugger(LPVOID pInit);
BOOL ConnectHdstub(LPVOID pInit);
BOOL ConnectOsAxsT0(LPVOID pInit);
BOOL ConnectOsAxsT1(LPVOID pInit);
__int64 CeGetRandomSeed(void);
BOOL SetHardwareWatch(LPVOID vAddr, DWORD flags);
BOOL RegisterAPISet(HANDLE hASet, DWORD dwSetID);
HANDLE CreateAPIHandle(HANDLE hASet, LPVOID pvData);

BOOL CeGetRawTime (ULONGLONG *pui64Millsec);
BOOL CeGetRawTimeOffset (LONGLONG *pi64Offset);

LPVOID VerifyAPIHandle(HANDLE hASet, HANDLE h);
FARPROC GetProcAddressInProcess (HANDLE hProcess, LPCWSTR  pszModuleName, LPCWSTR  pszFuncName);
BOOL CeCallUserProc(LPCWSTR pszDllName, LPCWSTR pszFuncName,
    LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned);

//
// LockAPIHandle and UnlockAPIHandle are for handle servers to decode+lock a handle when they can't use
// handle based API directly. (e.g. an API that takes 2 handles arguement)
//
// The return value of LockAPIHandle is a "hLock", where you should use it to call UnlockAPIHandle.
//
// NOTE: BE VERY CAREFUL USING THESE APIS. If you lock a handle without unlocking it, the handle
//       stays forever until process exit (and will stay forever for kernel mode server).
//
// Code sample:
//
//  void MyAPI (HANDLE h1, HANDLE h2) {
//
//      LPVOID pMyObject1, pMyObject2;
//      HANDLE hLock1, hLock2;
//
//      hLock1 = LockAPIHandle (hMyAPISet, GetCallerProcess (), h1, &pMyObject1);
//      hLock2 = LockAPIHandle (hMyAPISet, GetCallerProcess (), h2, &pMyObject2);
//
//      __try {
//          if (hLock1 && hLock2) {
//              DoWork (pMyObject1, pMyObject2);
//          } else {
//              HandleError ();
//          }
//      } __except (EXCEPTION_EXECUTE_HANDLE) {
//          HandleError ();
//      }
//
//      if (hLock1) {
//          UnlockAPIHandle (hMyAPISet, hLock1);
//      }
//      if (hLock2) {
//          UnlockAPIHandle (hMyAPISet, hLock2);
//      }
//  }
//
//
//
HANDLE LockAPIHandle (HANDLE hApiSet, HANDLE hSrcProc, HANDLE h, LPVOID *phObj);
BOOL UnlockAPIHandle (HANDLE hApiSet, HANDLE hLock);

// function to register an error handler for a PSL; error handler will be called when
// caller tries to call an API within this API set with invalid arguments.
// Prototype of the error handler is: DWORD pfnHandler(DWORD iMethod)
// Handler should return the error code to be passed back to the caller
// Handler input is the the method index into the PSL function table
typedef DWORD (*PFNAPIERRHANDLER)(DWORD);
BOOL SetAPIErrorHandler(HANDLE hApiSet, PFNAPIERRHANDLER pfnHandler);

void PPSHRestart(void);
BOOL AddEventAccess(HANDLE hEvent);

DWORD WaitForAPIReady (DWORD dwAPISet, DWORD dwTimeout);

DWORD GetHandleServerId (HANDLE h);

// !! IsAPIReady is being depreciated, provided here for Backward compatibility.
// Use WaitForAPIReady for new code
BOOL IsAPIReady(DWORD hAPI);


//
// kernel mode direct call entries
// NOTE: Do not call this function directly. User _DIRECT_HANDLE_CALL macro
//       below to make handle calls directly.
//
DWORD DirectHandleCall (
    REG_TYPE dwHtype,       // the expected handle type
    ...                     // variable # of arguments
);

// used both in coredll and kernel
typedef struct resdata_t {
    DWORD rva;
    DWORD size;
    DWORD codepage;
    DWORD unused;
} resdata_t;


//
// useful macro to declare typesafe direct handle calls
//
#define _HARG0(h)                                           h
#define _HARG1(h, a1)                                       h, a1
#define _HARG2(h, a1, a2)                                   h, a1, a2
#define _HARG3(h, a1, a2, a3)                               h, a1, a2, a3
#define _HARG4(h, a1, a2, a3, a4)                           h, a1, a2, a3, a4
#define _HARG5(h, a1, a2, a3, a4, a5)                       h, a1, a2, a3, a4, a5
#define _HARG6(h, a1, a2, a3, a4, a5, a6)                   h, a1, a2, a3, a4, a5, a6
#define _HARG7(h, a1, a2, a3, a4, a5, a6, a7)               h, a1, a2, a3, a4, a5, a6, a7
#define _HARG8(h, a1, a2, a3, a4, a5, a6, a7, a8)           h, a1, a2, a3, a4, a5, a6, a7, a8
#define _HARG9(h, a1, a2, a3, a4, a5, a6, a7, a8, a9)       h, a1, a2, a3, a4, a5, a6, a7, a8, a9
#define _HARG10(h, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) h, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10

// DirectHandleCall can take up to 14 total args (HANDLE + 13 args)
#define _DIRECT_HANDLE_CALL(rt, ht, id, err, argt, nargs, arglist) \
    ((rt) (*(DWORD (*) (DWORD, DWORD, DWORD, DWORD, _HARG##nargs argt)) DirectHandleCall) (ht, id, err, nargs, _HARG##nargs arglist))

#define _FORWARD_HANDLE_CALL(rt, ht, id, err, argt, nargs, arglist) \
    ((rt) (*(DWORD (*) (DWORD, DWORD, DWORD, DWORD, _HARG##nargs argt)) DirectHandleCall) (-(ht), id, err, nargs, _HARG##nargs arglist))

// internal flags
#define PHNDLR_DEVMGR           2
#define PHNDLR_GWES             1

// this MUST be a "define" instead of inline for we need to take the address of h
// to be able to workaround this
#define SetHandleOwner(h, hProc)    SetHandleOwnerWorkaround(&h, hProc)

// valid flags for PageOutModule
#define PAGE_OUT_PROCESS_ONLY                   0   // page out process/module only
#define PAGE_OUT_DLL_USED_ONLY_BY_THISPROC      1   // (ignored if hModule is a DLL) page out process
                                                    // together with dependent modules used only by the process
#define PAGE_OUT_ALL_DEPENDENT_DLL              2   // (ignored if hModule is a DLL) page out process
                                                    // together with all dependent modules. USE WITH CARE.
BOOL PageOutModule (HANDLE hModule, DWORD dwFlags);

// set/get event data
DWORD GetEventData(HANDLE hEvent);
BOOL SetEventData(HANDLE hEvent, DWORD dwData);

// check if a named event is signaled.
BOOL IsNamedEventSignaled (LPCWSTR pszName, DWORD dwFlags);

void DebugNotify(DWORD dwFlags, DWORD data);
//THSNAP *THCreateSnapshot(DWORD dwFlags, DWORD dwProcID);
BOOL KernelIoControl(DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned);

// reading information of a process/module with base address (used mostly by debugger)
#define MINFO_FUNCTION_TABLE        0x1
#define MINFO_FILE_ATTRIBUTES       0x2
#define MINFO_FULL_PATH             0x4
#define MINFO_MODULE_INFO           0x8     // internal only, implement GetModuleInformation
#define MINFO_FILE_HANDLE           0x10    // internal only, implement CeOpenFileHandle
DWORD CeGetModuleInfo (HANDLE hProcess, LPVOID lpBaseAddr, DWORD infoType, LPVOID pBuf, DWORD cbBufSize);
HANDLE CeOpenFileHandle (HMODULE hModule);

//
// Remote Heap stuffs
//
// flags for CeRemoteHeapCreate (cannot conflict with any HEAP_XXX flags in winnt.h)
#define HEAP_CLIENT_READWRITE   0x00008000  // give client r/w access to the heap
HANDLE CeRemoteHeapCreate (DWORD dwProcessId, DWORD dwFlags);

// flags for CeRemoteHeapTranslatePointer
#define CE_HEAP_REVERSE_TRANSLATE   0x00000001  // translate remote pointer to local
LPCVOID CeRemoteHeapTranslatePointer (HANDLE hHeap, DWORD dwFlags, LPCVOID pMem);


void CompactAllHeaps (void);

// Change driect caller (kernel only)
//
BOOL CeSetDirectCall (BOOL fKernel);


//
// ACL related functions
//

// the super token - access to everything
#define TOKEN_SYSTEM    ((HANDLE) 0x2)

HANDLE CeCreateToken (LPVOID pTok, DWORD dwFlags);
BOOL   CeRevertToSelf (void);
BOOL   CeAccessCheck (LPVOID pSecDesc, HANDLE hTok, DWORD dwAccess);
BOOL   CePrivilegeCheck (HANDLE hTok, LPDWORD pPrivs, int nPrivs);
BOOL   CeImpersonateToken (HANDLE hTok);
HANDLE CeCreateTokenFromAccount (LPCWSTR pszAccountName);
BOOL   CeImpersonateCurrentProcess (void);
LPVOID CeGetOwnerSID (HANDLE hTok);
LPVOID CeGetGroupSID (HANDLE hTok, DWORD idx);

//
// Standard privileges
//
#define CEPRI_IMPERSONATE                   0x00000001      // Impersonate
#define CEPRI_NETWORK                       0x00000002      // use network APIs
#define CEPRI_REALTIME                      0x00000004      // real-time APIs
#define CEPRI_READ_SYSTEM_FILE              0x00000008      // read system file
#define CEPRI_WRITE_SYSTEM_FILE             0x00000010      // update system file
#define CEPRI_RADIO_STACK                   0x00000020      // access to radio stack
#define CEPRI_OPENPROCESS                   0x00000040      // access to other processes
#define CEPRI_LOAD_KERNEL_LIBRARY           0x00000080      // load library into kernel

#define CEPRI_STANDARD_ALL                  0x3fffffff      // all standard privileges

//
// Extended privileges
//
#define CEPRI_EX_WRITE_ADB                  0x80000001      // update account DB APIs
#define CEPRI_EX_NET_ADMIN                  0x80000002      // network admin APIs
#define CEPRI_EX_READ_CREDENTIAL            0x80000003      // read credentials
#define CEPRI_EX_CREDENTIAL_ADMIN           0x80000004      // credential manager
#define CEPRI_EX_DRM_CONTENT                0x80000005      // access DRM content


// yet to determine, but might be needed
#define CEPRI_EX_DRM_ADMIN                  0x80000006      // access to DRM core/engine
#define CEPRI_EX_PROTECTED_WINDOW           0x80000007      // access to protected windows



//////////////////////////////////////////////////////////////////////////////////
// cache information structure, used with IOCTL_KLIB_GETCACHE_INFO
typedef struct _CacheInfo {
    DWORD   dwL1Flags;              // flags for L1 cache, see below
    DWORD   dwL1ICacheSize;         // total size in bytes
    DWORD   dwL1ICacheLineSize;     // line size in bytes
    DWORD   dwL1ICacheNumWays;      // number of ways, 1 for direct mapped
    DWORD   dwL1DCacheSize;         // total size in bytes
    DWORD   dwL1DCacheLineSize;     // line size in bytes
    DWORD   dwL1DCacheNumWays;      // number of ways, 1 for direct mapped
    DWORD   dwL2Flags;              // flags for L2 cache, see below
    DWORD   dwL2ICacheSize;         // total size in bytes, 0 means no L2 ICache
    DWORD   dwL2ICacheLineSize;     // line size in bytes
    DWORD   dwL2ICacheNumWays;      // number of ways, 1 for direct mapped
    DWORD   dwL2DCacheSize;         // total size in bytes, 0 means no L2 DCache
    DWORD   dwL2DCacheLineSize;     // line size in bytes
    DWORD   dwL2DCacheNumWays;      // number of ways, 1 for direct mapped

} CacheInfo, *PCacheInfo;
// cache info flags (applied to both L1 and L2
#define  CF_UNIFIED             0x01     // I and D caches unified, use ICache info fields
#define  CF_WRITETHROUGH        0x02     // writeback if not set
#define  CF_COHERENT            0x04     // no cache flush operations necessary

//
// CeGetCacheInfo: Get cache information
// Parameters:
//      cbSize: must be sizeof(CacheInfo)
//      pCacheInfo: pointer to CachInfo struct to receive the cache infomation
// Return Value:
//      TRUE if success, FALSE if failed (if OEM didn't implement the function
//
_inline BOOL CeGetCacheInfo (DWORD cbSize, PCacheInfo pCacheInfo)
{
    return KernelIoControl (IOCTL_HAL_GET_CACHE_INFO, 0, 0, pCacheInfo, cbSize, 0);
}


// CeLog status flags
// Since the CeLog zones are stored in the same UserKData DWORD as these status
// flags, they must be chosen so as not to collide with CeLog zones.
#define CELOGSTATUS_ENABLED_GENERAL CELZONE_RESERVED1
#define CELOGSTATUS_ENABLED_PROFILE CELZONE_RESERVED2
#define CELOGSTATUS_ENABLED_ANY     (CELOGSTATUS_ENABLED_GENERAL | CELOGSTATUS_ENABLED_PROFILE)

// Is CeLog loaded with this status.
#define IsCeLogStatus(Status)           ((UserKInfo[KINX_CELOGSTATUS] & (Status)) != 0)
// Is CeLog loaded with this status and one of these zones enabled.
#define IsCeLogEnabled(Status, ZoneCE)  (IsCeLogStatus(Status) && ((UserKInfo[KINX_CELOGSTATUS] & (ZoneCE)) != 0))
// Is CeLog loaded with general status and one of these zones enabled.
#define IsCeLogZoneEnabled(ZoneCE)      (IsCeLogEnabled(CELOGSTATUS_ENABLED_GENERAL, (ZoneCE)))


// Function pointers and data provided by the kernel for CeLog to use.
// Queried with IOCTL_CELOG_IMPORT.
typedef struct _CeLogImportTable {
    DWORD    dwVersion;               // Version of this structure, set to 4
    FARPROC  pCreateEventW;
    FARPROC  pEventModify;
    FARPROC  pCreateFileMappingW;
    FARPROC  pMapViewOfFile;
    FARPROC  pUnmapViewOfFile;
    FARPROC  pCloseHandle;
    FARPROC  pLockPages;
    FARPROC  pUnlockPages;
    FARPROC  pVirtualAlloc;
    FARPROC  pVirtualFree;
    FARPROC  pMapPtrToProcess;
    FARPROC  pQueryPerformanceCounter;
    FARPROC  pQueryPerformanceFrequency;
    FARPROC  pNKDbgPrintfW;
    FARPROC  pCeLogReSync;
    FARPROC  pGetLastError;
    FARPROC  pSetLastError;
    FARPROC  pGetThreadCallStack;
    FARPROC  pInSysCall;              // Pointer to InSysCall wrapper function
    DWORD   *pdwCeLogTLBMiss;         // Pointer to TLB miss count
    DWORD    dwCeLogLargeBuf;         // OEM-changeable large buffer size
    DWORD    dwCeLogSmallBuf;         // OEM-changeable small buffer size
    DWORD    dwDefaultThreadQuantum;  // The scheduler's default thread quantum
} CeLogImportTable;


// Function pointers and data provided by CeLog for the kernel to use.
// Registered with IOCTL_CELOG_REGISTER.
typedef struct _CeLogExportTable {
    DWORD   dwVersion;                // Version of this structure, set to 2
    PFNVOID pfnCeLogData;             // Receives CeLogData calls      (REQUIRED)
    PFNVOID pfnCeLogInterrupt;        // Receives CeLogInterrupt calls (OPTIONAL)
    PFNVOID pfnCeLogSetZones;         // Receives CeLogSetZones calls  (OPTIONAL)
    FARPROC pfnCeLogQueryZones;       // Receives CeLogGetZones calls, minus last param  (OPTIONAL)
    DWORD   dwCeLogTimerFrequency;    // Less than or equal to QueryPerformanceFrequency(), 0 if not used
} CeLogExportTable;

//
// KernelLibIoControl definitions
//
typedef BOOL (* PFN_KLIBIOCTL) (
    HANDLE  hLib,
    DWORD   dwIoControlCode,
    LPVOID  lpInBuf,
    DWORD   nInBufSize,
    LPVOID  lpOutBuf,
    DWORD   nOutBufSize,
    LPDWORD lpBytesReturned
    );

// Predefined Kernel Module numbers
#define KMOD_INVALID    0
#define KMOD_CORE       1
#define KMOD_DBG        2               // Debugger specific extensions
#define KMOD_CELOG      3
#define KMOD_APPVERIF   4
#define KMOD_OAL        5               // Pass through to OEMIoControl
#define KMOD_KITL       6
#define KMOD_MAX        6

// Kernel reserves the control code from 0-255
#define IOCTL_KLIB_ALLOCSHAREMEM      0
#define IOCTL_KLIB_FREESHAREMEM       1
#define IOCTL_KLIB_GETROMCHAIN        2
#define IOCTL_KLIB_GETCOMPRESSIONINFO 3
#define IOCTL_KLIB_GETALARMRESOLUTION 5         // Retrieves NKAlarmResolutionMSec from the kernel
#define IOCTL_KLIB_ISKDPRESENT        6         // Returns TRUE if KD is pressent FALSE otherwise
#define IOCTL_KLIB_SETROMDLLBASE      7         // called from filesys only, for loading externel MODULE to slot1
#define IOCTL_KLIB_GETPROCMEMINFO     8         // get per-process id and r/w memory usage
#define IOCTL_KLIB_GETCALLSTACK       9         // get callstack of of current thread with context information
#define IOCTL_KLIB_JITGETCALLSTACK    10        // get the last call stack of the thread that have an un-handled exception
#define IOCTL_KLIB_SETIGNORENOFAULT   11        // enable/disable no-fault so that one can choose to break into debugger on threads with nofault set
#define IOCTL_KLIB_SETMEMORYATTR      12        // CeSetMemoryAttributes
#define IOCTL_KLIB_GETWATSONSIZE      13        // get the size of Dr. Watson dump area
#define IOCTL_KLIB_WRITEWATSON        14        // Write data to Dr. Watson dump area
#define IOCTL_KLIB_READWATSON         15        // read data from Dr. Watson dump area
#define IOCTL_KLIB_FLUSHWATSON        16        // Flush data to Dr. Watson dump area, if using persistent storage
#define IOCTL_KLIB_CLEARWATSON        17        // Clear the Dr. Watson dump area
#define IOCTL_KLIB_SETJITDBGRPATH     18        // Change the path of JIT debugger
#define IOCTL_KLIB_WDOGAPI            19        // watchdog timer APIs
#define IOCTL_KLIB_SETDBGLIST         20        // set the debug module list to force ROM module to be loaded form FRD
#define IOCTL_KLIB_INTERRUPTDONE      21        // same as calling InterruptDone. Accessible from installable ISR from direct function all.
#define IOCTL_KLIB_LOADERINIT         22        // kcoredll notifying kernel of loading
#define IOCTL_KLIB_GETUSRCOREDLL      23        // get the handle of user mode coredll
#define IOCTL_KLIB_MAPPD              24        // Map the page directory of a process
#define IOCTL_KLIB_UNMAPPD            25        // unmap page directory
#define IOCTL_KLIB_GETKHEAPINFO       26        // get kernel heap information
#define IOCTL_KLIB_GETPHYSMEMINFO     27        // get physical adrress RAM/ROM start/end
#define IOCTL_KLIB_GET_POOL_STATE     28        // get information about page pool
#define IOCTL_KLIB_TIMEINIT           29        // called by filesys, as soon as registry is initialized.
#define IOCTL_KLIB_FORCECLEANBOOT     30        // force the next reset to be a clean boot (kmode only)
#define IOCTL_KLIB_NOTIFYCLEANBOOT    31        // call by filesys, notifying filesys corruption and system is booted clean
#define IOCTL_KLIB_UNALIGNENABLE      32        // enable/disable unaligned data access if supported
#define IOCTL_KLIB_CACHE_REGISTER     33        // filesys cache filter calls this to pass data between it and the kernel
#define IOCTL_KLIB_FLUSH_MAPFILES     34        // called during suspend/shutdown to flush memory-mapped files
#define IOCTL_KLIB_USER               256       // where OEM defined IOCTL starts.

// Debugger IOCTLs are separate from the rest of the kernel
#define IOCTL_DBG_INIT                0
#define IOCTL_DBG_HDSTUB_INIT         1
#define IOCTL_DBG_OSAXST0_INIT        2
#define IOCTL_DBG_OSAXST1_INIT        3
#define IOCTL_DBG_SANITIZE            4

// CeLog IOCTLs are separate from the rest of the kernel
#define IOCTL_CELOG_IMPORT            0
#define IOCTL_CELOG_REGISTER          1
#define IOCTL_CELOG_GETDESKTOPZONE    2
#define IOCTL_CELOG_DEREGISTER        3
#define IOCTL_CELOG_REPORTZONECHANGE  4
#define IOCTL_CELOG_IMPORT_PRIVATE    5

// App verifier IOCTLs are separate from the rest of the kernel
#define IOCTL_APPVERIF_ATTACH         0        // attach to a loaded kernel module

// Kitl IOCTLs
#define IOCTL_KLIB_KITL_INIT            0           // get the init information

BOOL KernelLibIoControl(HANDLE hLib, DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned);

// struct used by IOCTL_KLIB_MAPPD (in lpOutBuf)
typedef struct _PROCMEMINFO {
    BOOL  fIsKernel;        // if this process is kernel
    DWORD dwExeEnd;         // end of EXE code
    BYTE  pdbits[512];      // max of 512 page tables
} PROCMEMINFO, *PPROCMEMINFO;

// struct used by IOCTL_KLIB_GETPHYSMEMINFO (in lpOutBuf)
typedef struct _PHYSMEMINFO {
    DWORD dwPfnROMStart;    // pfn of ROM start
    DWORD dwPfnROMEnd;      // pfn of ROM end
    DWORD dwPfnRAMStart;    // pfn of RAM start
    DWORD dwPfnRAMEnd;      // pfn of RAM end
} PHYSMEMINFO, *PPHYSMEMINFO;


// Interface from OAL (inside kernel)
LPVOID NKCreateStaticMapping(DWORD dwPhysBase, DWORD dwSize);
BOOL NKDeleteStaticMapping (LPVOID pVirtAddr, DWORD dwSize);
// Interface from user-mode applications (kernel trap)
LPVOID CreateStaticMapping(DWORD dwPhysBase, DWORD dwSize);
BOOL DeleteStaticMapping (LPVOID pVirtAddr, DWORD dwSize);

#ifdef ARM
PFNVOID NKSetDataAbortHandler(PFNVOID pfnDataAbortHandler);
#endif


LPVOID MapCallerPtr (LPVOID ptr, DWORD dwLen);
LPVOID MapPtrToProcWithSize (LPVOID ptr, DWORD dwLen, HANDLE hProc);

// allocating shared memory for ISR/IST memory sharing
_inline BOOL KLibAllocShareMem (DWORD nPages, BOOL fNoCache, LPVOID *pVa, LPVOID *pPa)
{
    return KernelLibIoControl ((HANDLE) KMOD_CORE, IOCTL_KLIB_ALLOCSHAREMEM,
        pVa, nPages, pPa, fNoCache, NULL);
}
_inline BOOL KLibFreeShareMem (DWORD nPages, LPVOID pVa, LPVOID pPa)
{
    return KernelLibIoControl ((HANDLE) KMOD_CORE, IOCTL_KLIB_FREESHAREMEM,
        pVa, nPages, pPa, 0, NULL);
}

_inline LPVOID KLibGetROMChain (void)
{
    return (LPVOID) KernelLibIoControl ((HANDLE) KMOD_CORE, IOCTL_KLIB_GETROMCHAIN, 0, 0, 0, 0, 0);
}

_inline void KLibSetIgnoreNoFault (BOOL fIgnore)
{
    KernelLibIoControl ((HANDLE) KMOD_CORE, IOCTL_KLIB_SETIGNORENOFAULT, 0, fIgnore, 0, 0, 0);
}

_inline BOOL KLibUnalignedAccessEnable (BOOL fEnable)
{
    return KernelLibIoControl ((HANDLE) KMOD_CORE, IOCTL_KLIB_UNALIGNENABLE, &fEnable, sizeof(BOOL), 0, 0, 0);
}

//
// Use KLibIoControl to implement CeSetMemoryAttributes
//
#define PHYSICAL_ADDRESS_UNKNOWN    ((LPVOID) -1)
_inline BOOL CeSetMemoryAttributes  (LPVOID pVirtualAddr, LPVOID pShiftedPhysAddr, DWORD cbSize, DWORD dwAttributes)
{
    return KernelLibIoControl ((HANDLE) KMOD_CORE, IOCTL_KLIB_SETMEMORYATTR,
        pVirtualAddr, cbSize, pShiftedPhysAddr, dwAttributes, NULL);
}

// Dr. Watson kernel support

_inline DWORD DrWatsonGetSize (void)
{
    return KernelLibIoControl((HANDLE) KMOD_CORE, IOCTL_KLIB_GETWATSONSIZE, NULL, 0, NULL, 0, NULL);
}

_inline BOOL DrWatsonFlush (void)
{
    return KernelLibIoControl((HANDLE) KMOD_CORE, IOCTL_KLIB_FLUSHWATSON, NULL, 0, NULL, 0, NULL);
}

_inline BOOL DrWatsonClear (void)
{
    return KernelLibIoControl((HANDLE) KMOD_CORE, IOCTL_KLIB_CLEARWATSON, NULL, 0, NULL, 0, NULL);
}

_inline DWORD DrWatsonWriteData (DWORD dwOffset, LPVOID pData, DWORD cbSize)
{
    return KernelLibIoControl((HANDLE) KMOD_CORE, IOCTL_KLIB_WRITEWATSON, pData, cbSize, NULL, dwOffset, NULL);
}

_inline DWORD DrWatsonReadData (DWORD dwOffset, LPVOID pData, DWORD cbSize)
{
    return KernelLibIoControl((HANDLE) KMOD_CORE, IOCTL_KLIB_READWATSON, NULL, dwOffset, pData, cbSize, NULL);
}

// changing JIT Debugger path
_inline BOOL SetJITDebuggerPath (LPCWSTR pszDbgrPath)
{
    return KernelLibIoControl((HANDLE) KMOD_CORE, IOCTL_KLIB_SETJITDBGRPATH, (LPVOID) pszDbgrPath, 0, NULL, 0, NULL);
}


void NotifyForceCleanboot(void);
void DumpKCallProfile(DWORD bReset);
int QueryAPISetID(char *pName);
FARPROC GetAPIAddress(int setId, int iMethod);
// @CESYSGEN IF !REMOVE_DRAGON
BOOL SetStdioPathW(DWORD id, LPCWSTR pwszPath);
BOOL GetStdioPathW(DWORD id, PWSTR pwszBuf, LPDWORD lpdwLen);
// @CESYSGEN ENDIF
DWORD ReadRegistryFromOEM(DWORD dwFlags, LPBYTE pBuf, DWORD len);
BOOL WriteRegistryToOEM(DWORD dwFlags, LPBYTE pBuf, DWORD len);

BOOL FlushViewOfFileMaybe(LPCVOID lpBaseAddress, DWORD dwNumberOfBytesToFlush);

BOOL IsProcessDying(void);
void PSLNotify(DWORD flags, DWORD proc, DWORD thread);


HANDLE CreateKernelThread(LPTHREAD_START_ROUTINE lpStartAddr,
                          LPVOID lpvThreadParm, WORD prio, DWORD dwFlags);
// OEM entry point
#define NKCreateKernelThread  CreateKernelThread

// To force a clean boot (OEM entry point)
void NKForceCleanBoot (void);

//------------------------------------------------------------------------------
// Limited registry access from OAL during IOCTL_HAL_INITREGISTRY
//------------------------------------------------------------------------------

// These definitions are equivalent to the Win32 APIs

#ifndef REGSAM
typedef DWORD REGSAM;
#endif

//------------------------------------------------------------------------------

#pragma warning(disable:4200) // nonstandard extensions warning

// Definitions used with GetCallstackSnapshot

#define STACKSNAP_FAIL_IF_INCOMPLETE     1
#define STACKSNAP_EXTENDED_INFO          2
#define STACKSNAP_INPROC_ONLY            4     // Capture only the base part of the stack within
                                               // the thread's owner proc, no PSL transitions
#define STACKSNAP_RETURN_FRAMES_ON_ERROR 8     // Return number of frames found, even if error occurs. SetLastError will be set.
#define STACKSNAP_NEW_VM                 16    // New VM model, return CallSnapshot3.

typedef struct _CallSnapshot {
    DWORD dwReturnAddr;
} CallSnapshot;

typedef struct _CallSnapshotEx {
    DWORD dwReturnAddr;
    DWORD dwFramePtr;
    DWORD dwCurProc;
    DWORD dwParams[4];
} CallSnapshotEx;

typedef struct _CallSnapshot3 {
    DWORD dwReturnAddr;
    DWORD dwFramePtr;
    DWORD dwActvProc;
    DWORD dwVMProc;
    DWORD dwParams[4];
} CallSnapshot3;

typedef struct _StackSnapshot {
    WORD  wVersion;              // Version of this structure: Set to 1
    WORD  wNumCalls;             // Number of calls in this stack snapshot
    CallSnapshot rgCalls[0];     // Followed by an array of function calls
} StackSnapshot;

#pragma warning(default:4200) // nonstandard extensions warning

ULONG GetCallStackSnapshot (ULONG dwMaxFrames, CallSnapshot lpFrames[], DWORD dwFlags, DWORD dwSkip);
ULONG GetThreadCallStack (HANDLE hThrd, ULONG dwMaxFrames, LPVOID lpFrames, DWORD dwFlags, DWORD dwSkip);

_inline DWORD KLibGetLastExcpCallStack (DWORD dwMaxFrame, CallSnapshot lpFrames [])
{
    return KernelLibIoControl ((HANDLE) KMOD_CORE, IOCTL_KLIB_JITGETCALLSTACK, 0, 0, lpFrames, dwMaxFrame, 0);
}


// The following are used to support KDHAL
//++++++++++++++++++++++++++++++++++
BOOL OEMKDIoControl( DWORD dwIoControlCode, LPVOID lpBuf, DWORD nBufSize);
typedef BOOL (*PFN_OEMKDIoControl)( DWORD dwIoControlCode, LPVOID lpBuf, DWORD nBufSize);

#define KD_IOCTL_INIT                 0x00000000
#define KD_IOCTL_SET_CBP              0x00000001
#define KD_IOCTL_CLEAR_CBP            0x00000002
#define KD_IOCTL_QUERY_CBP            0x00000003
#define KD_IOCTL_ENUM_CBP             0x00000004
#define KD_IOCTL_SET_DBP              0x00000010
#define KD_IOCTL_CLEAR_DBP            0x00000011
#define KD_IOCTL_QUERY_DBP            0x00000012
#define KD_IOCTL_ENUM_DBP             0x00000013
#define KD_IOCTL_SET_BP               0x00000018 // Set any type of HW BP (including Read access data BP - flag based)
#define KD_IOCTL_QUERY_BP             0x00000019 // Query any type of HW BP (flag filters)
#define KD_IOCTL_CLEAR_BP             0x0000001A // Clear any type of HW BP (ID based)
#define KD_IOCTL_ENUM_BP              0x0000001B // Enum any type of HW BP (flag filters)
#define KD_IOCTL_MAP_EXCEPTION        0x00000020
#define KD_IOCTL_RESET                0x00000030
#define KD_IOCTL_JIT_NOTIF            0x00000040
#define KD_IOCTL_DMPGEN_START         0x00000050  // Start of Error Report dump generation, lpBuf = NULL, nBufSize=Size of reserved memory
#define KD_IOCTL_DMPGEN_END           0x00000051  // End   of Error Report dump generation, lpBuf = NULL, nBufSize=HRESULT of dump gen (S_OK for success)
#define KD_IOCTL_MEMORY_INACCESSIBLE  0x00000060  // Indicate memory inaccessible, lpBuf=Address, return TRUE if debugger should not read or write address

#define KD_HBP_FLAG_READ              0x00000001
#define KD_HBP_FLAG_WRITE             0x00000002
#define KD_HBP_FLAG_EXEC              0x00000004
#define KD_HBP_FLAG_PHYS              0x00000008 // Unused for now

//max size for save/restore CoProc registers
#define MAX_COPROCREGSIZE       512

typedef struct _KD_BPINFO{
    int     nVersion;
    ULONG   ulFlags;
    ULONG   ulAddress;
    ULONG   ulHandle;
    ULONG   ulCount;
} KD_BPINFO, *PKD_BPINFO;

typedef struct _KD_EXCEPTION_INFO {
    int     nVersion;
    ULONG   ulFlags;
    ULONG   ulAddress;
    ULONG   ulExceptionCode;
} KD_EXCEPTION_INFO, *PKD_EXCEPTION_INFO;
//++++++++++++++++++++++++++++++++++


#ifdef ARM
#define PAGE_ARM_MINICACHE 0x800
#endif

#ifdef x86
#define PAGE_x86_WRITETHRU 0x40000000
#endif

#ifdef x86
// Processor feature flags returned by x86 CPUID instruction
#define CPUID_PBE                   (1 << 31)
#define CPUID_IA64                  (1 << 30)
#define CPUID_TM                    (1 << 29)
#define CPUID_HTT                   (1 << 28)
#define CPUID_SS                    (1 << 27)
#define CPUID_SSE2                  (1 << 26)
#define CPUID_SSE                   (1 << 25)
#define CPUID_FXSR                  (1 << 24)
#define CPUID_MMX                   (1 << 23)
#define CPUID_ACPI                  (1 << 22)
#define CPUID_DTES                  (1 << 21)
#define CPUID_BIT20RESERVED         (1 << 20)
#define CPUID_CLFL                  (1 << 19)
#define CPUID_PSN                   (1 << 18)
#define CPUID_PSE36                 (1 << 17)
#define CPUID_PAT                   (1 << 16)
#define CPUID_CMOV                  (1 << 15)
#define CPUID_MCA                   (1 << 14)
#define CPUID_PGE                   (1 << 13)
#define CPUID_MTRR                  (1 << 12)
#define CPUID_SEP                   (1 << 11)
#define CPUID_BIT10RESERVED         (1 << 10)
#define CPUID_APIC                  (1 << 9)
#define CPUID_CX8                   (1 << 8)
#define CPUID_MCE                   (1 << 7)
#define CPUID_PAE                   (1 << 6)
#define CPUID_MSR                   (1 << 5)
#define CPUID_TSC                   (1 << 4)
#define CPUID_PSE                   (1 << 3)
#define CPUID_DE                    (1 << 2)
#define CPUID_VME                   (1 << 1)
#define CPUID_FPU                   (1 << 0)
#endif // x86

// Interrupt re-director function
#define _REFLECTORACCESS_CTL_CODE(_Function)  \
            CTL_CODE(FILE_DEVICE_REFLECTOR, _Function, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_REF_INTERRUPT_INITIALIZE _REFLECTORACCESS_CTL_CODE(1)
typedef struct _REF_INTERRUPT_INTIALIZE_PARAM {
    DWORD   idInt;
    HANDLE  hEvent;
    LPVOID  pvData;
    DWORD   cbData;
} REF_INTERRUPTINITIALIZE_PARAM, *PREF_INTERRUPTINITIALIZE_PARAM ;

#define IOCTL_REF_INTERRUPT_DONE _REFLECTORACCESS_CTL_CODE(2)
// dwSysIntr only.

#define IOCTL_REF_INTERRUPT_DISABLE _REFLECTORACCESS_CTL_CODE(3)
// dwSysIntr only

#define IOCTL_REF_INTERRUPT_MASK _REFLECTORACCESS_CTL_CODE(4)
typedef struct _REF_INTERRUPT_MASK_PARAM {
    DWORD idInt;
    BOOL fDisable;
} REF_INTERRUPT_MASK_PARAM, *PREF_INTERRUPT_MASK_PARAM ;

#define IOCTL_REF_VIRTUAL_COPY _REFLECTORACCESS_CTL_CODE(5)
typedef struct _REF_VIRTUALCOPY_PARAM {
    LPVOID  lpvDest;
    LPVOID  lpvSrc;
    DWORD   cbSize;
    DWORD   fdwProtect;
} REF_VIRTUALCOPY_PARAM,*PREF_VIRTUALCOPY_PARAM;

#define IOCTL_REF_LOAD_INT_CHAIN_HANDLER _REFLECTORACCESS_CTL_CODE(6)
#define REF_DEVDLL_LEN 0x40
#define REF_DEVENTRY_LEN 0x40
typedef struct _REF_LOAD_INT_CHAIN_HANDLER {
    TCHAR   szIISRDll[REF_DEVDLL_LEN];
    TCHAR   szIISREntry[REF_DEVENTRY_LEN];
    BYTE    bIRQ;
} REF_LOAD_INT_CHAIN_HANDLER, *PREF_LOAD_INT_CHAIN_HANDLER;
// Output is Handle.

#define IOCTL_REF_FREE_INT_CHAIN_HANDLER _REFLECTORACCESS_CTL_CODE(7)
// Input is Handle

#define IOCTL_REF_CREATE_STATIC_MAPPING _REFLECTORACCESS_CTL_CODE(8)
typedef struct _REF_CREATE_STATIC_MAPPING {
    DWORD   dwPhysBase;
    DWORD   dwSize;
} REF_CREATE_STATIC_MAPPING, *PREF_CREATE_STATIC_MAPPING;
// Output PVOID static addresss.
#define IOCTL_REF_INT_CHAIN_HANDLER_IOCONTROL _REFLECTORACCESS_CTL_CODE(9)
typedef struct _REF_INT_CHAIN_HANDLER_IOCONTROL {
    HANDLE  hLib;
    DWORD   dwIoControlCode;
    LPVOID  lpInBuf; // yes we use embedded pointer here.
    DWORD   nInBufSize;
} REF_INT_CHAIN_HANDLER_IOCONTROL, *PREF_INT_CHAIN_HANDLER_IOCONTROL;
// OUTPUT is IOCONTROL output buffer.

BOOL REL_UDriverProcIoControl(DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned);

// k-mode driver only - forward a ioctl request to driver.
BOOL ForwardDeviceIoControl (
    HANDLE          hDevice,
    DWORD           dwIoControlCode,
    LPVOID          lpInBuf,
    DWORD           nInBufSize,
    LPVOID          lpOutBuf,
    DWORD           nOutBufSize,
    LPDWORD         lpBytesReturned,
    LPOVERLAPPED    lpOverlapped
    );


#ifdef __cplusplus
}
#endif

#endif

