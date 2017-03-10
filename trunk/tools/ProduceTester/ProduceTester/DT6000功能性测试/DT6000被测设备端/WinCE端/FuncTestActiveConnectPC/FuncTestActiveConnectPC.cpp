#include <windows.h>
#include "FuncTestActiveConnectPC.h"


extern "C" BOOL CALLBACK DllMain(HANDLE _HDllHandle, DWORD _Reason, LPVOID _Reserved)
{
    switch(_Reason)
    {
        case DLL_PROCESS_ATTACH:
             DisableThreadLibraryCalls((HMODULE)_HDllHandle);
            RETAILMSG(1,(L" DLL_PROCESS_ATTACH Process Id=%d \r\n",GetCurrentProcess()));
            break;
        case DLL_PROCESS_DETACH:
            //RETAILMSG(1,(L" DLL_PROCESS_DETACH Process Id=%d \r\n",GetCurrentProcess()));
            break;
        default:
            break;
    }
    
    return TRUE;
}


#define     PC_READ_SHARE_MEM_NAME          L"FuncReadSharePC"
#define     PC_READ_EVENT_NAME              L"FuncReadEventPC"
#define     CE_WRITE_EVENT_NAME             L"FuncWriteEventCE"

#define     PC_WRITE_SHARE_MEM_NAME         L"FuncWriteSharePC"
#define     PC_WRITE_EVENT_NAME             L"FuncWriteEventPC"
#define     CE_READ_EVENT_NAME              L"FuncReadEventCE"

//共享内存的长度
#define     SHARE_MEM_LEN               2048
//定义存储在共享内存中的数据结构
#pragma pack(push)
#pragma pack(1)
typedef struct __tagDataDes
{
    DWORD   dwDataLength;
    BYTE    data[1];
}DATA_DESCRIPTOR;
#pragma pack(pop)

//定义控制结构
static HANDLE   g_hPCReadFileMap=NULL;              //供PC端读的共享内存映射
static BYTE *   g_pPCReadBuf = NULL;                //供PC端读的内存首指针
static HANDLE   g_hPCCanReadEvent = NULL;           //PC端可读的事件对象
static HANDLE     g_hCECanWriteEvent =NULL;         //CE端可写的事件对象

static HANDLE   g_hPCWriteFileMap = NULL;           //PC端写的共享内存
static BYTE *   g_pPCWriteBuf = NULL;               //PC端写的缓冲区
static HANDLE   g_hPCCanWriteEvent = NULL;          //PC端可写的事件对象
static HANDLE   g_hCECanReadEvent = NULL;           //CE端可读的事件对象

static void    DisconnectPCWrite()
{
    //释放PC端写的资源
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
}

static void DisconnectPCRead()
{
    //释放PC端读的资源
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
}

static BOOL    TryToConnectPCWrite()
{
    //初始化供PC端写的资源
    SetLastError(NO_ERROR);
    g_hPCWriteFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,
        NULL,PAGE_READWRITE,0,SHARE_MEM_LEN,PC_WRITE_SHARE_MEM_NAME);
    if (g_hPCWriteFileMap == NULL || GetLastError() != ERROR_ALREADY_EXISTS )
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
    if (g_hPCCanWriteEvent== NULL || GetLastError() != ERROR_ALREADY_EXISTS )
    {
        goto __Init_Error;
    }
    g_hCECanReadEvent = CreateEvent(NULL,FALSE,FALSE,CE_READ_EVENT_NAME);
    if (g_hCECanReadEvent==NULL || GetLastError() != ERROR_ALREADY_EXISTS )
    {
        goto __Init_Error;
    }

    return TRUE;
__Init_Error:

    DisconnectPCWrite();
    return FALSE;

}

static BOOL  TryToConnectPCRead()
{
    //初始化供PC端读的资源
    SetLastError(NO_ERROR);
    g_hPCReadFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,
        NULL,PAGE_READWRITE,0,SHARE_MEM_LEN,PC_READ_SHARE_MEM_NAME);
    if (g_hPCReadFileMap == NULL || GetLastError() != ERROR_ALREADY_EXISTS )
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
    if (g_hPCCanReadEvent== NULL || GetLastError() != ERROR_ALREADY_EXISTS )
    {
        goto __Init_Error;
    }
    g_hCECanWriteEvent = CreateEvent(NULL,FALSE,FALSE,CE_WRITE_EVENT_NAME);
    if (g_hCECanWriteEvent==NULL || GetLastError() != ERROR_ALREADY_EXISTS )
    {
        goto __Init_Error;
    }

    return TRUE;
__Init_Error:

    DisconnectPCRead();
    return FALSE;

}

extern "C" int PCConnect(DWORD cbInput,BYTE *Input,DWORD *pvbOutput, BYTE **ppOutput,void *pStream)
{
    BOOL    bRet1 = TryToConnectPCRead();
    BOOL    bRet2 = TryToConnectPCWrite();

    ResetEvent(g_hCECanReadEvent);
    ResetEvent(g_hPCCanReadEvent);
    SetEvent(g_hPCCanWriteEvent);
    SetEvent(g_hCECanWriteEvent);

    DisconnectPCRead();
    DisconnectPCWrite();

    if (bRet1 && bRet2)
    {
        return 0;
    }
    return -1;
}


extern "C" int PCWrite(DWORD cbInput,BYTE *Input,DWORD *pvbOutput, BYTE **ppOutput,void *pStream)
{
    DWORD   dwTimeOut,dwRet;
    RETAILMSG(1,(L"PC Write1\r\n"));

    if (!TryToConnectPCWrite())
    {
        RETAILMSG(2,(L"PC Write2\r\n"));
        return -1;
    }
    cbInput -=4;
    memcpy(&dwTimeOut,Input,sizeof(DWORD));
    Input += sizeof(DWORD);

    RETAILMSG(1,(L"PC Write3:%d\r\n"));
    if (dwTimeOut ==0)
    {
        dwTimeOut = 5000;
    }
    dwRet = WaitForSingleObject(g_hPCCanWriteEvent,dwTimeOut);
    if (dwRet == WAIT_OBJECT_0)
    {
        DATA_DESCRIPTOR     *pDataDes=(DATA_DESCRIPTOR *)g_pPCWriteBuf;
        pDataDes ->dwDataLength =cbInput;
        memcpy(pDataDes->data,Input,cbInput);
        SetEvent(g_hCECanReadEvent);
        DisconnectPCWrite();
        return 0;
    }
    
    DisconnectPCWrite();
    return 1;
}

extern "C" int PCRead(DWORD cbInput,BYTE *Input,DWORD *pvbOutput, BYTE **ppOutput,void *pStream)
{
    DWORD   dwTimeOut,dwRet;
    if (!TryToConnectPCRead())
    {
        return -1;
    }
    cbInput -=4;
    memcpy(&dwTimeOut,Input,sizeof(DWORD));
    Input += sizeof(DWORD);

    if (dwTimeOut ==0)
    {
        dwTimeOut = 5000;
    }
    dwRet = WaitForSingleObject(g_hPCCanReadEvent,dwTimeOut);
    if (dwRet == WAIT_OBJECT_0)
    {
        DATA_DESCRIPTOR     *pDataDes = (DATA_DESCRIPTOR *)g_pPCReadBuf;
        DWORD       dwLen = pDataDes ->dwDataLength;
        BYTE        *pBuf= (BYTE *)LocalAlloc(LPTR,dwLen);
        memcpy (pBuf,pDataDes->data,dwLen);
        *pvbOutput=dwLen;
        *ppOutput= pBuf;
        SetEvent(g_hCECanWriteEvent);
        DisconnectPCRead();
        return 0;
    }

   



    DisconnectPCRead();
    return 0;
}

