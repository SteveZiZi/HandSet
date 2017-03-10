#include "stdafx.h"
#include <windows.h>
#include <assert.h>
#include "FuncTestActiveConnectCE.h"


#define     PC_READ_SHARE_MEM_NAME          L"FuncReadSharePC"
#define     PC_READ_EVENT_NAME              L"FuncReadEventPC"
#define     CE_WRITE_EVENT_NAME             L"FuncWriteEventCE"

#define     PC_WRITE_SHARE_MEM_NAME         L"FuncWriteSharePC"
#define     PC_WRITE_EVENT_NAME             L"FuncWriteEventPC"
#define     CE_READ_EVENT_NAME              L"FuncReadEventCE"

//�����ڴ�ĳ���
#define     SHARE_MEM_LEN               2048
//����洢�ڹ����ڴ��е����ݽṹ
#pragma pack(push)
#pragma pack(1)
typedef struct __tagDataDes
{
    DWORD   dwDataLength;
    BYTE    data[1];
}DATA_DESCRIPTOR;
#pragma pack(pop)


//������ƽṹ
static CRITICAL_SECTION g_csOprate;                 //����ȫ��ͬ�����ٽ��
static BOOL             g_bInitSucess = FALSE;      //��ʼ���Ƿ�ɹ�

static HANDLE   g_hPCReadFileMap=NULL;              //��PC�˶��Ĺ����ڴ�ӳ��
static BYTE *   g_pPCReadBuf = NULL;                //��PC�˶����ڴ���ָ��
static HANDLE   g_hPCCanReadEvent = NULL;           //PC�˿ɶ����¼�����
static HANDLE     g_hCECanWriteEvent =NULL;           //CE�˿�д���¼�����

static HANDLE   g_hPCWriteFileMap = NULL;           //PC��д�Ĺ����ڴ�
static BYTE *   g_pPCWriteBuf = NULL;               //PC��д�Ļ�����
static HANDLE   g_hPCCanWriteEvent = NULL;          //PC�˿�д���¼�����
static HANDLE   g_hCECanReadEvent = NULL;           //CE�˿ɶ����¼�����



BOOL    CEConnect()
{
    if (g_bInitSucess)
    {
        return TRUE;
    }

    InitializeCriticalSection(&g_csOprate);
    EnterCriticalSection(&g_csOprate);

    //��ʼ����PC�˶�����Դ
    SetLastError(NO_ERROR);
    g_hPCReadFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,
        NULL,PAGE_READWRITE,0,SHARE_MEM_LEN,PC_READ_SHARE_MEM_NAME);
    if (g_hPCReadFileMap == NULL || GetLastError() == ERROR_ALREADY_EXISTS )
    {
        goto __Init_Error;
    }
    g_pPCReadBuf = (BYTE *)MapViewOfFile(g_hPCReadFileMap,FILE_MAP_ALL_ACCESS,
        0,0,SHARE_MEM_LEN);
    if (g_pPCReadBuf == NULL)
    {
        goto __Init_Error;
    }
    g_hPCCanReadEvent = CreateEvent(NULL,FALSE,FALSE,PC_READ_EVENT_NAME);
    if (g_hPCCanReadEvent== NULL || GetLastError() == ERROR_ALREADY_EXISTS )
    {
        goto __Init_Error;
    }
    g_hCECanWriteEvent = CreateEvent(NULL,FALSE,FALSE,CE_WRITE_EVENT_NAME);
    if (g_hCECanWriteEvent==NULL || GetLastError() == ERROR_ALREADY_EXISTS )
    {
        goto __Init_Error;
    }

    //��ʼ����PC��д����Դ
    SetLastError(NO_ERROR);
    g_hPCWriteFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,
        NULL,PAGE_READWRITE,0,SHARE_MEM_LEN,PC_WRITE_SHARE_MEM_NAME);
    if (g_hPCWriteFileMap == NULL || GetLastError() == ERROR_ALREADY_EXISTS )
    {
        goto __Init_Error;
    }
    g_pPCWriteBuf = (BYTE *)MapViewOfFile(g_hPCWriteFileMap,FILE_MAP_ALL_ACCESS,
        0,0,SHARE_MEM_LEN);
    if (g_pPCWriteBuf == NULL)
    {
        goto __Init_Error;
    }
    g_hPCCanWriteEvent = CreateEvent(NULL,FALSE,FALSE,PC_WRITE_EVENT_NAME);
    if (g_hPCCanWriteEvent== NULL || GetLastError() == ERROR_ALREADY_EXISTS )
    {
        goto __Init_Error;
    }
    g_hCECanReadEvent = CreateEvent(NULL,FALSE,FALSE,CE_READ_EVENT_NAME);
    if (g_hCECanReadEvent==NULL || GetLastError() == ERROR_ALREADY_EXISTS )
    {
        goto __Init_Error;
    }

    SetEvent(g_hPCCanWriteEvent);
    SetEvent(g_hCECanWriteEvent);

    g_bInitSucess = TRUE;
    LeaveCriticalSection(&g_csOprate);
    return TRUE;
__Init_Error:
    g_bInitSucess = FALSE;
    LeaveCriticalSection(&g_csOprate);
    CEDisConnect();
    return FALSE;
}

void CEDisConnect()
{
    EnterCriticalSection(&g_csOprate);

    g_bInitSucess = FALSE;
    //�ͷ�PC�˶�����Դ
    if (g_pPCReadBuf != NULL)
    {
        UnmapViewOfFile(g_pPCReadBuf);
        g_pPCReadBuf = NULL;
    }
    if(g_hPCReadFileMap != NULL)
    {
        CloseHandle(g_hPCReadFileMap);
        g_hPCReadFileMap = NULL;
    }
    if (g_hPCCanReadEvent != NULL)
    {
        CloseHandle(g_hPCCanReadEvent);
        g_hPCCanReadEvent = NULL;
    }
    if(g_hCECanWriteEvent != NULL)
    {
        CloseHandle(g_hCECanWriteEvent);
        g_hCECanWriteEvent = NULL;
    }

    //�ͷ�PC��д����Դ
    if (g_pPCWriteBuf != NULL)
    {
        UnmapViewOfFile(g_pPCWriteBuf);
        g_pPCWriteBuf = NULL;
    }
    if (g_hPCWriteFileMap !=NULL)
    {
        CloseHandle(g_hPCWriteFileMap);
        g_hPCWriteFileMap = NULL;
    }
    if (g_hPCCanWriteEvent != NULL )
    {
        CloseHandle(g_hPCCanWriteEvent);
        g_hPCCanWriteEvent = NULL;
    }
    if (g_hCECanReadEvent != NULL)
    {
        CloseHandle(g_hCECanReadEvent);
        g_hCECanReadEvent =NULL;
    }


    LeaveCriticalSection(&g_csOprate);
    DeleteCriticalSection(&g_csOprate);
}

BOOL CEWrite(void *pBuf,unsigned long ulLen,DWORD dwTimeOut)
{
    DWORD           dwRet;
    DATA_DESCRIPTOR *pDataDes;
    //������
    assert(ulLen <= SHARE_MEM_LEN-sizeof(DWORD));
    if (dwTimeOut ==0)
    {
        dwTimeOut =5000;
    }
    if (!g_bInitSucess )
    {
        return FALSE;
    }

    EnterCriticalSection(&g_csOprate);
    if (!g_bInitSucess )
    {
        LeaveCriticalSection(&g_csOprate);
        return FALSE;
    }

    dwRet = WaitForSingleObject(g_hCECanWriteEvent,dwTimeOut);
    if (dwRet != WAIT_OBJECT_0)
    {
        LeaveCriticalSection(&g_csOprate);
        return FALSE;
    }

    pDataDes = (DATA_DESCRIPTOR *)g_pPCReadBuf;
    pDataDes->dwDataLength = ulLen;
    memcpy(pDataDes->data,pBuf,ulLen);
    
    SetEvent(g_hPCCanReadEvent);
    LeaveCriticalSection(&g_csOprate);
    return TRUE;
}

BOOL CERead(void *pBuf,unsigned long ulLen,DWORD *pdwReaded,DWORD dwTimeOut)
{
    DWORD               dwRet,dwReaded;
    DATA_DESCRIPTOR     *pDataDes;

    assert (ulLen <= SHARE_MEM_LEN);
    assert(ulLen != 0);

    if (dwTimeOut ==0)
    {
        dwTimeOut =5000;
    }
    if (!g_bInitSucess )
    {
        return FALSE;
    }

    EnterCriticalSection(&g_csOprate);
    if (!g_bInitSucess )
    {
        LeaveCriticalSection(&g_csOprate);
        return FALSE;
    }

    dwRet = WaitForSingleObject(g_hCECanReadEvent,dwTimeOut);
    if (dwRet != WAIT_OBJECT_0)
    {
        LeaveCriticalSection(&g_csOprate);
        return FALSE;
    }

    pDataDes = (DATA_DESCRIPTOR *)g_pPCWriteBuf;
    if (pDataDes->dwDataLength >ulLen)
    {
        dwReaded = ulLen;
    }
    else
    {
        dwReaded = pDataDes->dwDataLength;
    }
    memcpy(pBuf,pDataDes->data,dwReaded);
    *pdwReaded = dwReaded;
    SetEvent(g_hPCCanWriteEvent);
    LeaveCriticalSection(&g_csOprate);
    return TRUE;


}
