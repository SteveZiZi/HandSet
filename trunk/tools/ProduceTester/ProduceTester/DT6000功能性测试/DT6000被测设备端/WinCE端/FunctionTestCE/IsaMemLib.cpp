#include "stdafx.h"
#include "IsaMemLib.h"

static HINSTANCE hDLL = NULL;
static DWORD (* pfn_epcIsaMemByteR) (DWORD dMemAddress, PUCHAR pucData)=NULL;
static DWORD (* pfn_epcIsaMemByteW)(DWORD dMemAddress, UCHAR ucData)=NULL;
static DWORD (*pfn_epcIsaMemWordR)(DWORD dMemAddress,PUINT16 pusData)=NULL;
static DWORD (*pfn_epcIsaMemWordW)(DWORD dMemAddress, UINT16  usData)=NULL;
static DWORD (*pfn_epcIsaIoWordW)(DWORD  dwIoAddress,UINT16  usData)=NULL;
static DWORD (*pfn_epcIsaInitIntrrupt)(DWORD dwIRQNumber,
                                       HANDLE hIntrEvent,PDWORD pdwSysIntr)=NULL;
static DWORD (*pfn_epcIsaIntrruptDone)(DWORD dwSysIntrNumber);
static DWORD (*pfn_epcIsaIntrruptDisable)( DWORD dwSysIntrNumber);


BOOL IsaMem_LoadLib()
{
	hDLL = LoadLibrary(_T("\\Windows\\epcIsa.dll"));
	if (!hDLL) return FALSE;

   pfn_epcIsaMemByteR=( DWORD (*)(DWORD, PUCHAR))GetProcAddress
       (hDLL, _T("epcIsaMemByteR"));
   if (pfn_epcIsaMemByteR ==NULL)
   {
       goto Init_Failed;
   }

   pfn_epcIsaMemByteW=(DWORD (*)(DWORD, UCHAR))
       GetProcAddress(hDLL, _T("epcIsaMemByteW"));
   if (pfn_epcIsaMemByteW ==NULL)
   {
       goto Init_Failed;
   }

   pfn_epcIsaMemWordR=(DWORD (*)(DWORD, PUINT16))
       GetProcAddress(hDLL, _T("epcIsaMemWordR"));
   if (pfn_epcIsaMemWordR ==NULL)
   {
       goto Init_Failed;
   }

   pfn_epcIsaMemWordW=(DWORD (*)(DWORD, UINT16))
       GetProcAddress(hDLL, _T("epcIsaMemWordW"));
   if (pfn_epcIsaMemWordW ==NULL)
   {
       goto Init_Failed;
   }

   pfn_epcIsaIoWordW=(DWORD (*)(DWORD, UINT16))
       GetProcAddress(hDLL, _T("epcIsaIoWordW"));
   if (pfn_epcIsaIoWordW ==NULL)
   {
       goto Init_Failed;
   }

   pfn_epcIsaInitIntrrupt=(DWORD (*)(DWORD, HANDLE, PDWORD))
       GetProcAddress(hDLL, _T("epcIsaInitIntrrupt"));;
   if (pfn_epcIsaInitIntrrupt ==NULL)
   {
       goto Init_Failed;
   }

   pfn_epcIsaIntrruptDone=(DWORD (*)(DWORD))
       GetProcAddress(hDLL, _T("epcIsaIntrruptDone"));;
   if (pfn_epcIsaIntrruptDone ==NULL)
   {
       goto Init_Failed;
   }

   pfn_epcIsaIntrruptDisable=(DWORD (*)(DWORD))
       GetProcAddress(hDLL, _T("epcIsaIntrruptDisable"));;
   if (pfn_epcIsaIntrruptDisable ==NULL)
   {
       goto Init_Failed;
   }
	return TRUE;
Init_Failed:
    CloseHandle(hDLL);
    hDLL=NULL;
    return FALSE;
}

void IsaMem_UnLoadLib()
{
	if (hDLL) FreeLibrary(hDLL);
    hDLL=NULL;
}

DWORD epcIsaMemByteR(DWORD dMemAddress, PUCHAR pucData)
{
    return pfn_epcIsaMemByteR(dMemAddress,pucData);
}


/*********************************************************************************************************
** Function name:           epcIsaMemUCHARW
** Descriptions:            向Memory 空间写入一个字节
** Input parameters:        dMemAddress             要写入数据的Memory 地址        
**                          ucData                   要写入的数据       
** Output parameters:                                  
** Returned value:          ISA_NO_ERR              写入数据成功
**                          ISA_MEM_OVER_RANGE      当前要访的Memory地址超出范围
*********************************************************************************************************/
DWORD epcIsaMemByteW(DWORD dMemAddress, UCHAR ucData)
{
	return pfn_epcIsaMemByteW(dMemAddress,ucData);
}

/*********************************************************************************************************
** Function name:           MemWordR
** Descriptions:            在Memory 空间读取16 bit数据
** Input parameters:        dMemAddress             要读取数据的Memory 地址               
** Output parameters:       pusData                 读取到的数据
** Returned value:          ISA_NO_ERR              读取数据成功
**                          ISA_MEM_OVER_RANGE      当前要访的Memory地址超出范围
**                          ISA_POINTER_IS_NULL     输入的指针变量为NULL
**                          ISA_ADDR_ODD            输入的地址为奇数
*********************************************************************************************************/
DWORD epcIsaMemWordR(DWORD dMemAddress,PUINT16 pusData)
{
	return pfn_epcIsaMemWordR(dMemAddress,pusData);
}

/*********************************************************************************************************
** Function name:           MemWordW
** Descriptions:            在Memory 空间写入16  bit数据
** Input parameters:        dMemAddress             要写入数据的Memory 地址               
**                          usData                  要写入的数据
** Output parameters:                          
** Returned value:          ISA_NO_ERR              读取数据成功
**                          ISA_MEM_OVER_RANGE      当前要访的Memory地址超出范围
**                          ISA_ADDR_ODD            输入的地址为奇数
*********************************************************************************************************/
DWORD epcIsaMemWordW(DWORD dMemAddress, UINT16  usData)
{
    return pfn_epcIsaMemWordW(dMemAddress,usData);
}

/*********************************************************************************************************
** Function name:           epcIsaIoWordW
** Descriptions:            在IO空间写入16  bit数据
** Input parameters:        dwIoAddress              要读取数据的IO地址               
**                          usData                  将要写入数据
** Output parameters:       
** Returned value:          ISA_NO_ERR              读取数据成功
**                          ISA_IO_OVER_RANGE       当前要访问的I/O地址超出范围
**                          ISA_ADDR_ODD            输入的地址为奇数
*********************************************************************************************************/
DWORD epcIsaIoWordW(DWORD  dwIoAddress,UINT16  usData)
{
	return pfn_epcIsaIoWordW(dwIoAddress,usData);
}

/*********************************************************************************************************
** Function name:           IsaInitIntrrupt
** Descriptions:            初始化中断
** Input parameters:        dwIRQNumber             中断号
**                                                  IRQ3、IRQ4...
**                          hIntrEvent              中断对应的事件                          
** Output parameters:       pdwSysIntr              系统分配的逻辑中断号
** Returned value:          ISA_NO_ERR              中断初始化成功
**                          ISA_NO_IRQ_NUM          PC/104总线不能使用该中断号
**                          ISA_INIT_INTR_FAIL      中断初始化失败
*********************************************************************************************************/
DWORD epcIsaInitIntrrupt( DWORD   dwIRQNumber, 
                                     HANDLE  hIntrEvent, 
                                     PDWORD  pdwSysIntr)
{
	return pfn_epcIsaInitIntrrupt(dwIRQNumber,hIntrEvent,pdwSysIntr);
}


DWORD epcIsaIntrruptDone(DWORD dwSysIntrNumber)
{
    return pfn_epcIsaIntrruptDone(dwSysIntrNumber);
}

DWORD epcIsaIntrruptDisable( DWORD dwSysIntrNumber)
{
    return pfn_epcIsaIntrruptDisable(dwSysIntrNumber);
}