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
** Descriptions:            ��Memory �ռ�д��һ���ֽ�
** Input parameters:        dMemAddress             Ҫд�����ݵ�Memory ��ַ        
**                          ucData                   Ҫд�������       
** Output parameters:                                  
** Returned value:          ISA_NO_ERR              д�����ݳɹ�
**                          ISA_MEM_OVER_RANGE      ��ǰҪ�õ�Memory��ַ������Χ
*********************************************************************************************************/
DWORD epcIsaMemByteW(DWORD dMemAddress, UCHAR ucData)
{
	return pfn_epcIsaMemByteW(dMemAddress,ucData);
}

/*********************************************************************************************************
** Function name:           MemWordR
** Descriptions:            ��Memory �ռ��ȡ16 bit����
** Input parameters:        dMemAddress             Ҫ��ȡ���ݵ�Memory ��ַ               
** Output parameters:       pusData                 ��ȡ��������
** Returned value:          ISA_NO_ERR              ��ȡ���ݳɹ�
**                          ISA_MEM_OVER_RANGE      ��ǰҪ�õ�Memory��ַ������Χ
**                          ISA_POINTER_IS_NULL     �����ָ�����ΪNULL
**                          ISA_ADDR_ODD            ����ĵ�ַΪ����
*********************************************************************************************************/
DWORD epcIsaMemWordR(DWORD dMemAddress,PUINT16 pusData)
{
	return pfn_epcIsaMemWordR(dMemAddress,pusData);
}

/*********************************************************************************************************
** Function name:           MemWordW
** Descriptions:            ��Memory �ռ�д��16  bit����
** Input parameters:        dMemAddress             Ҫд�����ݵ�Memory ��ַ               
**                          usData                  Ҫд�������
** Output parameters:                          
** Returned value:          ISA_NO_ERR              ��ȡ���ݳɹ�
**                          ISA_MEM_OVER_RANGE      ��ǰҪ�õ�Memory��ַ������Χ
**                          ISA_ADDR_ODD            ����ĵ�ַΪ����
*********************************************************************************************************/
DWORD epcIsaMemWordW(DWORD dMemAddress, UINT16  usData)
{
    return pfn_epcIsaMemWordW(dMemAddress,usData);
}

/*********************************************************************************************************
** Function name:           epcIsaIoWordW
** Descriptions:            ��IO�ռ�д��16  bit����
** Input parameters:        dwIoAddress              Ҫ��ȡ���ݵ�IO��ַ               
**                          usData                  ��Ҫд������
** Output parameters:       
** Returned value:          ISA_NO_ERR              ��ȡ���ݳɹ�
**                          ISA_IO_OVER_RANGE       ��ǰҪ���ʵ�I/O��ַ������Χ
**                          ISA_ADDR_ODD            ����ĵ�ַΪ����
*********************************************************************************************************/
DWORD epcIsaIoWordW(DWORD  dwIoAddress,UINT16  usData)
{
	return pfn_epcIsaIoWordW(dwIoAddress,usData);
}

/*********************************************************************************************************
** Function name:           IsaInitIntrrupt
** Descriptions:            ��ʼ���ж�
** Input parameters:        dwIRQNumber             �жϺ�
**                                                  IRQ3��IRQ4...
**                          hIntrEvent              �ж϶�Ӧ���¼�                          
** Output parameters:       pdwSysIntr              ϵͳ������߼��жϺ�
** Returned value:          ISA_NO_ERR              �жϳ�ʼ���ɹ�
**                          ISA_NO_IRQ_NUM          PC/104���߲���ʹ�ø��жϺ�
**                          ISA_INIT_INTR_FAIL      �жϳ�ʼ��ʧ��
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