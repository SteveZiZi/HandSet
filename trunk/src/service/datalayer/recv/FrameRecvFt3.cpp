/// @file
///
/// @brief
///     FPGA FT3������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.3.20
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "FrameRecvFt3.h"
#include "FrameRecv.h"
#include "src\service\datalayer\include\config.h"

CFrameRecvFt3::CFrameRecvFt3(CFrameRecv* pFrameRecv, HANDLE hGPMDevice, DsmPortName enPortName)
                            : m_hGPMDevice(hGPMDevice)
                            , m_pFrameRecv(pFrameRecv)
                            , m_hExitEvent(NULL)
                            , m_hNotifyEvent(NULL)
                            , m_hMapping(NULL)
                            , m_pDataBuf(NULL)
                            , m_hThread(NULL)
                            , m_bRecv(false)
                            , m_bIsInited(false)
                            , m_enPortName(enPortName)
                            , m_pGuoWangFT3Msg(NULL)
                            , m_pNanRuiFt3Msg(NULL)
{

}

CFrameRecvFt3::~CFrameRecvFt3(void)
{
}

///
/// @brief
///    ��ʼ��FT3����
///
/// @return
///     true����ɹ�
///
bool CFrameRecvFt3::InitFt3(const wchar_t *pNotifyEvtName, const wchar_t *pMapFileName, uint32_t nMapFileLen)
{
    if (m_bIsInited)
    {
        return true;
    }

    if (INVALID_HANDLE_VALUE == m_hGPMDevice)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecvFt3::InitFt3 : m_hGPMDevice INVALID_HANDLE_VALUE \n")));
        return false;
    }

    //����ͨ��x�õ����ڴ�ӳ��
    m_hMapping=CreateFileMapping(INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE|SEC_COMMIT,
        0,
        nMapFileLen,
        pMapFileName);
    if (m_hMapping == NULL)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("FrameRecv: InitFt3:Open map file failed for Port %d\r\n"), m_enPortName));
        goto exitCode;
    }
    m_pDataBuf = reinterpret_cast<uint8_t*>(MapViewOfFile(m_hMapping,FILE_MAP_WRITE|FILE_MAP_READ, 0,0,0));
    if (m_pDataBuf==NULL)
    {
        RETAILMSG(OUTPUT_LOG_MSG,(TEXT("FrameRecv: InitFt3: FileMapping for Channel %d map FAILED"), m_enPortName));
        goto exitCode;
    }

    //��֪ͨ�¼�
    m_hNotifyEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, pNotifyEvtName);
    if (m_hNotifyEvent == NULL)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("FrameRecv: InitFt3:Open NotifyEvent failed,error code is %d\r\n"), GetLastError()));
        goto exitCode;
    }

    //�����߳��˳��¼�
    m_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (m_hExitEvent == NULL)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("FrameRecv: InitFt3:Create Exit Event failed\r\n")));
        goto exitCode;
    }

    //���������߳�
    if (m_hThread == NULL)
    {
        DWORD dwThreadId = 0;
        m_hThread = CreateThread(NULL, 0, Ft3ThreadProc, this, NULL, &dwThreadId);
        if (m_hThread == NULL)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("FrameRecv: InitFt3: Create Thread Failed On Port:%d\r\n"), m_enPortName));
            return false;
        }
        else 
        {
            //�����̳߳ɹ����������ȼ�
            if(CeSetThreadPriority(m_hThread, DATA_RECV_DETECT_THREAD_PRIO))
            {
#if GEN_LOG_MESSAGE
                RETAILMSG(OUTPUT_LOG_MSG, (_T("FrameRecv: InitFt3: Port %d ThreadId(0x%08x) Current Priority %d\r\n"), m_enPortName, dwThreadId, CeGetThreadPriority(m_hThread)));
#endif
            }

        }
    }

    if (!m_pGuoWangFT3Msg)
    {
        m_pGuoWangFT3Msg = CreateGuoWangFT3MsgImpl();
    }
    if (!m_pNanRuiFt3Msg)
    {
        m_pNanRuiFt3Msg = CreateNanRuiFT3MsgImpl();
    }

    m_bIsInited = true;
    return true; 

exitCode:
    UnInitFt3();
    return false;

}

///
/// @brief
///    Ft3����ж��
///
void CFrameRecvFt3::UnInitFt3( void )
{
    StopFt3Recv();

    //�����߳��˳��¼�
    if (NULL != m_hExitEvent)
    {
        SetEvent(m_hExitEvent);
    }
    if (NULL != m_hThread)
    {
        DWORD dwRet =  WaitForSingleObject(m_hThread, 1000);   //�ȴ�(1s)�߳��˳�
        if (dwRet == WAIT_OBJECT_0 )
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("Thread Exit Success On Port : %d \n"), m_enPortName));
        }
        else if(dwRet == WAIT_TIMEOUT)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("Thread Exit Failed On Port : %d, Wait 1s time out \n"), m_enPortName));
            if (TerminateThread(m_hThread,44))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("Thread Terminated Success On Port : %d \n"), m_enPortName));
            }
            else
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("Thread Terminated Failed  On Port : %d \n"), m_enPortName));
            }
        }
        else
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("Thread Exit Failed On Port : %d, Wait Failed \n"), m_enPortName));
            if (TerminateThread(m_hThread,44))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("Thread Terminated Success On Port : %d \n"), m_enPortName));
            }
            else
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("Thread Terminated Failed  On Port : %d \n"), m_enPortName));
            }
        }
        CloseHandle(m_hThread);
        m_hThread = NULL;
    }

    if (NULL != m_hExitEvent)
    {
        CloseHandle(m_hExitEvent);
        m_hExitEvent = NULL;
    }

    if (m_hNotifyEvent != NULL)
    {
        CloseHandle(m_hNotifyEvent);
        m_hNotifyEvent = NULL;
    }

    if (m_pDataBuf != NULL)
    {
        UnmapViewOfFile(m_pDataBuf);
        m_pDataBuf = NULL;
    }
    if (m_hMapping != NULL)
    {
        CloseHandle(m_hMapping);
        m_hMapping = NULL;
    }

    if (NULL != m_pGuoWangFT3Msg)
    {
        ReleaseGuoWangFT3MsgImpl(m_pGuoWangFT3Msg);
        m_pGuoWangFT3Msg = NULL;
    }
    if (NULL != m_pNanRuiFt3Msg)
    {
        ReleaseNanRuiFT3MsgImpl(m_pNanRuiFt3Msg);
        m_pNanRuiFt3Msg = NULL;
    }

    m_bIsInited = false;
}

///
/// @brief
///    ��ʼft3����
///
/// @return
///     true��������߳������ɹ�
///
bool CFrameRecvFt3::StartFt3Recv()
{
    m_bRecv = true;

    //
    //����fpga����
    //
    return StartFpgaRecvLoop(FT3_DEFAULT_RECV_FRAMES , FT3_DEFAULT_RECV_ENABLETIMEOUT);

}

///
/// @brief
///    ֹͣFt3�����߳�
///
/// @return
///     true��ʾFt3�����߳�ֹͣ�ɹ�
///
bool CFrameRecvFt3::StopFt3Recv()
{
    m_bRecv = false;

    IOCTRL_IN_PARAM_FT3 stIoCtrlParamFt3;
    stIoCtrlParamFt3.nDeviceItem = m_enPortName;
    if (FALSE == DeviceIoControl(m_hGPMDevice, IOCTRL_STOP_RECV, &stIoCtrlParamFt3, sizeof(IOCTRL_IN_PARAM_FT3), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T(" CFrameRecvFt3::StartFt3Recv(): DeviceIoControl Stop Fpga Recv Failed \n")));
        return false;
    }

    return true;
}

///
/// @brief
///    ���ý��ճ�ʱ,��λ����
///
/// @return
///     true�������óɹ�
///
bool CFrameRecvFt3::SetRecvTimeOut( uint32_t nMilliSecond )
{
    IOCTRL_IN_PARAM_FT3 stIoCtrlParamFt3;
    stIoCtrlParamFt3.nDeviceItem = m_enPortName;
    stIoCtrlParamFt3.nInBuf[0] = nMilliSecond;
    if ( nMilliSecond > 0)
    {
        if ( FALSE == DeviceIoControl(m_hGPMDevice, IOCTRL_SET_RX_TIMEOUT, &stIoCtrlParamFt3, sizeof(stIoCtrlParamFt3), NULL, NULL, NULL, NULL) )
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecvFt3::SetRecvTimeOut : DeviceIoControl Set Recv Timeout Failed . \n")));
            return false;
        }
        return true;
    }
    return false;
}

///
/// @brief
///    ����һ�ν��յ�����֡�����Ƿ�ʹ�ܽ��ճ�ʱ
///
/// @param[in]  nCount  һ�ν��յ�����֡��,���ֵ������ FT3_MAX_RECV_FRAMES/2
/// @param[in]  bEnableTimeOut �Ƿ�ʹ�ܽ��ճ�ʱ
///
/// @return
///     true��ʾ���óɹ�
///
bool CFrameRecvFt3::StartFpgaRecvLoop( uint16_t nCount, bool bEnableTimeOut )
{
    IOCTRL_IN_PARAM_FT3 stIoCtrlParamFt3;
    stIoCtrlParamFt3.nDeviceItem = m_enPortName;
    stIoCtrlParamFt3.nInBuf[0] = nCount;
    if (bEnableTimeOut)
    {
        stIoCtrlParamFt3.nInBuf[1] = 1;
    }
    else
    {
        stIoCtrlParamFt3.nInBuf[1] = 0;
    }

    if ( FALSE == DeviceIoControl(m_hGPMDevice, IOCTRL_SET_RECV_LOOP, &stIoCtrlParamFt3, sizeof(stIoCtrlParamFt3), NULL, NULL, NULL, NULL) )
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecvFt3::SetRecvCountandEnableTimeOut : DeviceIoControl Set RecvCount and bEnableTimeout Failed . \n")));
        return false;
    }
    return true;
}

///
/// @brief
///    ���ý����߳����ȼ�
///
/// @param[in]  nPriority Ҫ���õ����ȼ�
///
/// @return
///     bool ���óɹ�����true
///
bool CFrameRecvFt3::SetRxThreadPriority( int nPriority )
{
    if (m_hThread)
    {
        return (TRUE == CeSetThreadPriority(m_hThread, nPriority));
    }
    return false;
}

///
/// @brief
///    ��ڵ��жϴ������
///
DWORD Ft3ThreadProc(PVOID pArg)
{
    CFrameRecvFt3 *pFt3 = static_cast<CFrameRecvFt3*>(pArg);
    DWORD dwRet = 0;
    HANDLE hEvt[] = {pFt3->m_hNotifyEvent, pFt3->m_hExitEvent};

    RETAILMSG(OUTPUT_LOG_MSG, (_T("Ft3ThreadProc Start On Port %d \r\n"), pFt3->m_enPortName));
    while (true)
    {
        //�ȴ������¼����߳��˳��¼�
        dwRet = WaitForMultipleObjects(sizeof(hEvt)/sizeof(hEvt[0]), hEvt, FALSE, INFINITE);
        if (dwRet == WAIT_OBJECT_0) {
            pFt3->m_pFrameRecv->_Ft3DataHandler(pArg);
        } 
        else if (dwRet == WAIT_OBJECT_0+1) 
        {
            //�˳��߳�
            break;
        }
        else 
        {
            static uint32_t nCount = 1;
            nCount++; 
            RETAILMSG(OUTPUT_LOG_MSG, (_T("Ft3ThreadProc found error On Port:%d ,Count = %d\r\n"), pFt3->m_enPortName, nCount));
            Sleep(1000);
            if (nCount > 10)
            {  
                nCount = 1;
                break;
            }
        }
    }
    RETAILMSG(OUTPUT_LOG_MSG, (_T("Ft3ThreadProc Exit on port %d \r\n"), pFt3->m_enPortName));

    return 1;
}

