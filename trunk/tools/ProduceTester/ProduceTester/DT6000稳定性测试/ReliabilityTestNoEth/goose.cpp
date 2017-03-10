#include "stdafx.h"
#include "winioctl.h"
#include "pkfuncs.h"
#include "config.h"
#include "goose.h"

_GooseSmv_c::_GooseSmv_c(HANDLE hDevice)
{

    if (hDevice != INVALID_HANDLE_VALUE) {
        m_hDevice = hDevice;
    } else {
        m_hDevice = INVALID_HANDLE_VALUE;
        RETAILMSG(ZONE_ERROR, (_T("_GooseSmv_c->hDevice is invalid\r\n")));
    }

    m_NetMode           = _9_1_9_2_DETECT;
#if EN_9192_DETECT
    m_hNotifyEvent      = INVALID_HANDLE_VALUE;
    m_hMapping          = INVALID_HANDLE_VALUE;
    m_DataBuf           = NULL;
    m_hThread           = INVALID_HANDLE_VALUE;
#endif
#if EN_SMV
    m_hSmvExitEvent     = INVALID_HANDLE_VALUE;
    m_hSmvNotifyEvent   = INVALID_HANDLE_VALUE;
    m_hSmvMapping       = INVALID_HANDLE_VALUE;
    m_SmvDataBuf        = NULL;
    m_hSmvThread        = INVALID_HANDLE_VALUE;
#endif
#if EN_GOOSE_DETECT
    m_hGFNotifyEvent    = INVALID_HANDLE_VALUE;
    m_hGFMapping        = INVALID_HANDLE_VALUE;
    m_GFDataBuf         = NULL;
    m_hGFThread         = INVALID_HANDLE_VALUE;
#endif
}

_GooseSmv_c::~_GooseSmv_c()
{

}


BOOL _GooseSmv_c::InitGoose(WCHAR *pNotifyEvtName, WCHAR *pMapFileName, 
                            PFN_GOOSE_CALLBACK pfnGooseCallBack,void *pCallBackParam)
{
#if EN_9192_DETECT
    /* 退出线程事件 */
    m_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (m_hExitEvent == INVALID_HANDLE_VALUE) {
        RETAILMSG(ZONE_ERROR, (_T("InitGoose:Create Exit Event failed\r\n")));
        goto exitCode;
    }
    /*  打开接收FPGA缓冲区事件  */
    m_hNotifyEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, pNotifyEvtName);
    if (m_hNotifyEvent == INVALID_HANDLE_VALUE) {
        RETAILMSG(ZONE_ERROR, (_T("InitGoose:Open NotifyEvent failed,error code is %d\r\n"), GetLastError()));
        goto exitCode;
    }

    //创建通道x用到的内存映射
    m_hMapping=CreateFileMapping(INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE|SEC_COMMIT,
        0,
        MAP_FILE_LEN_9192_DETECT,
        pMapFileName);
    if (m_hMapping == INVALID_HANDLE_VALUE) {
        RETAILMSG(ZONE_ERROR, (_T("InitGoose:Open map file failed\r\n")));
        goto exitCode;
    }
    m_DataBuf=(PBYTE)MapViewOfFile(m_hMapping,FILE_MAP_WRITE|FILE_MAP_READ, 0,0,0);
    if (m_DataBuf==NULL)
    {
        RETAILMSG(ZONE_ERROR,(TEXT("InitGoose: FileMapping for Channel%d map FAILED"), 1));
        goto exitCode;
    }

    m_pfnGooseCallBack = pfnGooseCallBack;
    m_pCallBackParam= pCallBackParam;

    /*  创建服务线程    */
    m_bExitThread = FALSE;
    if (m_hThread == INVALID_HANDLE_VALUE) {
        m_hThread = CreateThread(NULL, 0, gooseThreadProc, this, NULL, NULL);
        if (m_hThread == INVALID_HANDLE_VALUE) {
            RETAILMSG(ZONE_ERROR, (_T("Create Thread Failed\r\n")));
        } else {
            if(CeSetThreadPriority(m_hThread, APP_PRIO)){
                RETAILMSG(ZONE_INFO, (_T("InitGoose:Priority:%d\r\n"),CeGetThreadPriority(m_hThread)));
            }
        }
    }

    return TRUE;

exitCode:
    DeInitGoose();
#endif
    return FALSE;
}

void _GooseSmv_c::DeInitGoose(void)
{
#if EN_9192_DETECT
    m_bExitThread = TRUE;
    if (m_hExitEvent != INVALID_HANDLE_VALUE) {
        SetEvent(m_hExitEvent);                                        /*  发送线程推出事件            */
    }
    if (m_hThread != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hThread);
//         if (WaitForSingleObject(m_hThread, 1000) == WAIT_OBJECT_0) {
//             CloseHandle(m_hThread);
//         }
        m_hThread = INVALID_HANDLE_VALUE;
    }
    CloseHandle(m_hExitEvent);
    m_hExitEvent = INVALID_HANDLE_VALUE;

    if (m_hNotifyEvent != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hNotifyEvent);
        m_hNotifyEvent = INVALID_HANDLE_VALUE;
    }
    if (m_DataBuf != NULL) {
        UnmapViewOfFile(m_DataBuf);
        m_DataBuf = NULL;
    }
    if (m_hMapping != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hMapping);
        m_hMapping = INVALID_HANDLE_VALUE;
    }
#endif
}
/*********************************************************************************************************
**  非成员函数
*********************************************************************************************************/
//光以太网口1
DWORD _GooseSmv_c::gooseThreadProc(PVOID pArg)
{
#if EN_9192_DETECT
    _GooseSmv_c *pGooseSmv = (_GooseSmv_c *)pArg;
    DWORD dwRet;
    HANDLE hEvt[] = {pGooseSmv->m_hNotifyEvent, pGooseSmv->m_hExitEvent};

    RETAILMSG(ZONE_INFO, (_T("gooseThreadProc1\r\n")));
    while (!pGooseSmv->m_bExitThread)
    {
        dwRet = WaitForMultipleObjects(2, hEvt, FALSE, INFINITE);
        if (dwRet == WAIT_OBJECT_0) {
            if (pGooseSmv->m_NetMode == _GooseSmv_c::_9_1_9_2_DETECT)     /*  9_1/9_2 探测              */
            {
                if (pGooseSmv->m_pfnGooseCallBack!=NULL)
                {
                    pGooseSmv->m_pfnGooseCallBack(pGooseSmv->m_pCallBackParam);
                }
            }
            else if (pGooseSmv->m_NetMode == _GooseSmv_c::_9_1_9_2_FILTER)/*  9_1/9_2 过滤              */
            {
                if (pGooseSmv->m_pfnGooseCallBack!=NULL)
                {
                    pGooseSmv->m_pfnGooseCallBack(pGooseSmv->m_pCallBackParam);
                }
            }
            ResetEvent(pGooseSmv->m_hNotifyEvent);

        } else if (dwRet == WAIT_OBJECT_0+1) { //退出线程
            pGooseSmv->m_bExitThread = TRUE;
        } else {
            RETAILMSG(ZONE_ERROR, (_T("gooseThreadProc1 found error CODE(%d)\r\n"),GetLastError()));
            return 0;
        }
    }
#endif

    RETAILMSG(ZONE_INFO, (_T("gooseThreadProc1 exit\r\n")));
    return 1;
}