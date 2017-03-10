/// @file
///
/// @brief
///     FPGA��̫��������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.3.21
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "FrameRecvNet.h"
#include "FrameRecv.h"
#include "src\service\datalayer\include\config.h"

//������뾯��todo��Ϣ
//#pragma message(__FILE__ "(" STRING(__LINE__) "): ����Uninitʱ�Ƿ�Ҫ��ֹͣ����ӿ����ݽ���")
#define STRING2(x) #x
#define STRING(x) STRING2(x)

CFrameRecvNet::CFrameRecvNet(CFrameRecv* pFrameRecv, HANDLE hGPMDevice, DsmPortName enPortName )
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
                            , m_pSav91Msg(NULL)
                            , m_pSav92Msg(NULL)
                            , m_pGooseMsg(NULL)
                            , m_pSav91Msg2(NULL)
                            , m_pSav92Msg2(NULL)
{
}

CFrameRecvNet::~CFrameRecvNet(void)
{
}

///
/// @brief
///    ��ʼ���������
///
/// @return
///     true����ɹ�
///
bool CFrameRecvNet::InitNet(const  wchar_t *pNotifyEvtName,const wchar_t *pMapFileName,uint32_t nMapFileLen )
{
    if (m_bIsInited)
    {
        return true;
    }

    if (INVALID_HANDLE_VALUE == m_hGPMDevice)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecvFt3::InitNet : m_hGPMDevice INVALID_HANDLE_VALUE \n")));
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
        RETAILMSG(OUTPUT_LOG_MSG, (_T("FrameRecv :InitNet: Open map file failed\r\n")));
        goto exitCode;
    }
    m_pDataBuf = reinterpret_cast<uint8_t*>(MapViewOfFile(m_hMapping,FILE_MAP_WRITE|FILE_MAP_READ, 0,0,0));
    if (m_pDataBuf==NULL)
    {
        RETAILMSG(OUTPUT_LOG_MSG,(TEXT("FrameRecv :InitNet: FileMapping for Port %d map FAILED"), m_enPortName));
        goto exitCode;
    }

    //�򿪽���FPGA�������¼�
    m_hNotifyEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, pNotifyEvtName);
    if (m_hNotifyEvent == NULL)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("FrameRecv :InitNet: Open NotifyEvent failed,error code is %d\r\n"), GetLastError()));
        goto exitCode;
    }

    // �˳��߳��¼�
    m_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (m_hExitEvent == NULL) {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("FrameRecv :InitNet: Create Exit Event failed\r\n")));
        goto exitCode;
    }

    // ���������߳�
    if (m_hThread == NULL)
    {
        DWORD dwThreadId = 0;
        m_hThread = CreateThread(NULL, 0, NetThreadProc, this, NULL, &dwThreadId);
        if (m_hThread == NULL)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("FrameRecv : InitNet : Create Thread Failed  On Port:%d\r\n"), m_enPortName));
            goto exitCode;
        }
        else 
        {
            if(CeSetThreadPriority(m_hThread, DATA_RECV_DETECT_THREAD_PRIO))
            {
#if GEN_LOG_MESSAGE
                RETAILMSG(OUTPUT_LOG_MSG, (_T("FrameRecv: InitNet: Port %d ThreadId(0x%08x) Current Priority %d\r\n"), m_enPortName, dwThreadId, CeGetThreadPriority(m_hThread)));
#endif
            }

        }
    }
    if (!m_pSav91Msg)
    {
        m_pSav91Msg = CreateSav91MsgImpl();
    }
    if (!m_pSav92Msg)
    {
        m_pSav92Msg = CreateSav92MsgImpl();
    }
    if (!m_pGooseMsg)
    {
        m_pGooseMsg = CreateGooseMsgImpl();
    }
    if (!m_pSav91Msg2)
    {
        m_pSav91Msg2 = CreateSav91MsgImpl();
    }
    if (!m_pSav92Msg2)
    {
        m_pSav92Msg2 = CreateSav92MsgImpl();
    }

    m_bIsInited = true;
    return true;

exitCode:
    UnInitNet();
    return false;
}

///
/// @brief
///    �����������
///
void CFrameRecvNet::UnInitNet( void )
{
    StopNetRecv();

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

    if (NULL != m_hNotifyEvent)
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

    if (m_pSav91Msg)
    {
        ReleaseSav91MsgImpl(m_pSav91Msg);
        m_pSav91Msg = NULL;
    }
    if (m_pSav92Msg)
    {
        ReleaseSav92MsgImpl(m_pSav92Msg);
        m_pSav92Msg = NULL;
    }
    if (m_pSav91Msg2)
    {
        ReleaseSav91MsgImpl(m_pSav91Msg2);
        m_pSav91Msg2 = NULL;
    }
    if (m_pSav92Msg2)
    {
        ReleaseSav92MsgImpl(m_pSav92Msg2);
        m_pSav92Msg2 = NULL;
    }
    if (m_pGooseMsg)
    {
        ReleaseGooseMsgImpl(m_pGooseMsg);
        m_pGooseMsg = NULL;
    }

    m_bIsInited = false;
}

///
/// @brief
///    ��ʼ�������
/// 
/// @return
///     true��ʶ�����߳������ɹ�
///
bool CFrameRecvNet::StartNetRecv()
{
    m_bRecv = true;
    return true;

}

///
/// @brief
///    ֹͣ������մ���
///
bool CFrameRecvNet::StopNetRecv()
{
    m_bRecv = false;

    //
    //ֹͣ��������
    //
    IOCTRL_IN_PARAM_NET stIoCtrlParamNet;
    stIoCtrlParamNet.nDeviceItem = m_enPortName;
    if (FALSE == DeviceIoControl(m_hGPMDevice, IOCTRL_STOP_RECV, &stIoCtrlParamNet, sizeof(IOCTRL_IN_PARAM_NET), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecvNet::StopNetRecv(): DeviceIoControl IOCTRL_STOP_RECV Failed On Port : %d \n"), m_enPortName));
        return false;
    }
    return true;
}

///
/// @brief
///    ��������˿ڹ���
///
bool CFrameRecvNet::SetNetFilter( IOCTRL_IN_PARAM_NET_BIG& stIoCtrlParamNet )
{
    stIoCtrlParamNet.nDeviceItem = m_enPortName;

    // ���õײ����
    if (FALSE == DeviceIoControl(m_hGPMDevice, IOCTRL_FILTER, &stIoCtrlParamNet, sizeof(stIoCtrlParamNet), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_SetNetFilter(): DeviceIoControl set NetFilter Failed \n")));
        return false;
    }
    return true;
}

///
/// @brief
///    ��������ӿ�̽��
///
/// @param[in]  enNetRecvFrameType  ָ��̽�������֡����
/// @param[in]  nCount              ����һ�ν��յ���������
/// @param[in]  bEnableTimeout      �Ƿ����ý��ճ�ʱ
/// @param[in]  nTimeoutMilliSecond ָ����ʱʱ�䣬���ó�ʱ��ʱ����Ч
///
/// @return
///     ̽����Ϣ���óɹ�����true
///
bool CFrameRecvNet::Detect(NetFt3RecvFrameType enNetRecvFrameType, uint16_t nCount, bool bEnableTimeout, uint32_t nTimeoutMilliSecond)
{
    //
    //1.���ö˿ڿ�ʼ̽��
    //
    IOCTRL_IN_PARAM_NET stIoCtrlParam;
    //����������ӿڱ�ʶ
    stIoCtrlParam.nDeviceItem = m_enPortName;
    //��������֡����
    stIoCtrlParam.nInBuf[0] = enNetRecvFrameType;
    if (FALSE == DeviceIoControl(m_hGPMDevice, IOCTRL_DETECT, &stIoCtrlParam, sizeof(stIoCtrlParam), NULL,NULL,NULL,NULL) )
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecvNet::Detect() : DeviceIoControl IOCTRL_DETECT Failed on Port : %d \n"), m_enPortName));
        return false;
    }

    //
    //2. ���ó�ʱʱ��
    //
    if (false == SetRecvTimeOut(nTimeoutMilliSecond))
    {
        return false;
    }

    //
    //3.���������߳�
    //
    StartNetRecv();

    //
    //4.���ý�����Ŀ���Ƿ����ó�ʱ
    //
    return StartFpgaRecvLoop(nCount, bEnableTimeout);
}

///
/// @brief
///    ���ý��ճ�ʱ,��λ����
///
/// @param[in]  nTimeoutMilliSecond ָ����ʱʱ�䣬���ó�ʱ��ʱ����Ч
///
/// @return
///     ���óɹ�����true
///
bool CFrameRecvNet::SetRecvTimeOut( uint32_t nMilliSecond /*= 10*/ )
{
    IOCTRL_IN_PARAM_NET stIoCtrlParamNet;
    stIoCtrlParamNet.nDeviceItem = m_enPortName;
    stIoCtrlParamNet.nInBuf[0] = nMilliSecond;
    if ( nMilliSecond >=  DSM_MU_RX_TIMEOUT_MIN && nMilliSecond <= DSM_MU_RX_TIMEOUT_MAX)
    {
        if ( FALSE == DeviceIoControl(m_hGPMDevice, IOCTRL_SET_RX_TIMEOUT, &stIoCtrlParamNet, sizeof(stIoCtrlParamNet), NULL, NULL, NULL, NULL) )
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecvNet::SetRecvTimeOut : DeviceIoControl Set Recv Timeout Failed . \n")));
            return false;
        }
        return true;
    }
    return true;
}

///
/// @brief
///    ����Fpga�������ݽ���
///
/// @param[in]  nCount              ����һ�ν��յ���������
/// @param[in]  bEnableTimeout      �Ƿ����ý��ճ�ʱ
///
/// @return
///     ���óɹ�����true
///
bool CFrameRecvNet::StartFpgaRecvLoop( uint16_t nCount, bool bEnableTimeOut )
{
    IOCTRL_IN_PARAM_NET stIoCtrlParamNet;
    stIoCtrlParamNet.nDeviceItem = m_enPortName;
    stIoCtrlParamNet.nInBuf[0] = nCount;
    if (bEnableTimeOut)
    {
        stIoCtrlParamNet.nInBuf[1] = 1;
    }
    else
    {
        stIoCtrlParamNet.nInBuf[1] = 0;
    }

    if ( FALSE == DeviceIoControl(m_hGPMDevice, IOCTRL_SET_RECV_LOOP, &stIoCtrlParamNet, sizeof(stIoCtrlParamNet), NULL, NULL, NULL, NULL) )
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecvNet::SetRecvCountandEnableTimeOut : DeviceIoControl Set RecvCount and bEnableTimeout Failed . \n")));
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
bool CFrameRecvNet::SetRxThreadPriority( int nPriority )
{
    if (m_hThread)
    {
        return (TRUE == CeSetThreadPriority(m_hThread, nPriority));
    }
    return false;
}

///
/// @brief
///    �����жϴ������
///
/// @param[in] pArg ָ������ʵ����ָ��
///
DWORD NetThreadProc( PVOID pArg )
{
    CFrameRecvNet *pNet = static_cast<CFrameRecvNet*>(pArg);
    DWORD dwRet = 0;
    HANDLE hEvt[] = {pNet->m_hNotifyEvent, pNet->m_hExitEvent};

    RETAILMSG(OUTPUT_LOG_MSG, (_T("NetThreadProc Start On Port:%d\r\n"), pNet->m_enPortName));
    while (true)
    {
        //�ȴ������¼����߳��˳��¼�
        dwRet = WaitForMultipleObjects(sizeof(hEvt)/sizeof(hEvt[0]), hEvt, FALSE, INFINITE);
        if (dwRet == WAIT_OBJECT_0) {
            DWORD dwStart = GetTickCount();
            pNet->m_pFrameRecv->_NetDataHandler(pArg);
            RETAILMSG(0, (_T("P(%d)/T(%d)\r\n"), pNet->m_enPortName, GetTickCount() - dwStart));
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
            RETAILMSG(OUTPUT_LOG_MSG, (_T("NetThreadProc found error On Port:%d ,Count = %d\r\n"), pNet->m_enPortName, nCount));
            Sleep(1000);
            if (nCount > 10)
            {  
                nCount = 1;
                break;
            }
        }
    }
    RETAILMSG(OUTPUT_LOG_MSG, (_T("NetThreadProc Exit On Port:%d\r\n"), pNet->m_enPortName));

    return 1;

}
