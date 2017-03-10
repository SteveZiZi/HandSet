/// @file
///
/// @brief
///     �������ݷ�����
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.3.23
///
/// �޸���ʷ��
///     Modified by:    chao
///     Modified date:  2013.6.19
///     Version:        1.0.1
///     Descriptions:   ����˷��͹���
///

#include "StdAfx.h"
#include "FrameSendNet.h"
#include "FrameSend.h"
#include "src\utils\SmvScaler\SmvScaler.h"
#include "src\service\datalayer\config\ParamConfigBasic.h"
#include "src\service\datalayer\config\ParamConfigSMVSend.h"
#include "src\service\datalayer\config\ParamConfigGOOSESend.h"

///
/// @brief
///    ���캯��
///
/// @param[in]  pFrameSend    ���ݷ��͹�����
/// @param[in]  hGPMDevice    GPMC���߾��
/// @param[in]  enPortName    Netʵ����Ӧ��ʵ�ʶ˿�
///
CFrameSendNet::CFrameSendNet( CFrameSend* pFrameSend, HANDLE hGPMDevice, DsmPortName enPortName )
: m_pDataSendManager(pFrameSend)
, m_hGPMDevice(hGPMDevice)
, m_enPortName(enPortName)
, m_bIsInited(false)
, m_hFileMaping(NULL)
, m_pMapHeader(NULL)
, m_pMapDataBuffer(NULL)
, m_pSav91Msg(NULL)
, m_pSav92Msg(NULL)
, m_pGooseMsg(NULL)
{
    m_vecSMVSendCbs.clear();
    m_vecGOOSESendCbs.clear();
}

///
/// @brief
///    ��������
///
CFrameSendNet::~CFrameSendNet(void)
{
    UnInitNet();
}

///
/// @brief
///    ��ʼ����̫�����ݷ��ͣ����ļ�ӳ��
///
bool CFrameSendNet::InitNet( const wchar_t *pNotifyEvtName, const wchar_t *pMapFileName, uint32_t nMapFileLen )
{
    // �ж��Ƿ��Ѿ���ʼ��
    if (m_bIsInited)
    {
        return true;
    }

    // ���GPM�������
    if (INVALID_HANDLE_VALUE == m_hGPMDevice)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameSendNet::InitNet(): m_hGPMDevice INVALID_HANDLE_VALUE \n")));
        return false;
    }

    // ����ͨ�������ڴ�ӳ���ļ�
    m_hFileMaping = CreateFileMapping(INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE | SEC_COMMIT,
        0,
        nMapFileLen,
        pMapFileName);
    if (NULL == m_hFileMaping)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameSendNet::InitNet(): Open map file failed On Port %d\r\n"), m_enPortName));
        goto exitCode;
    }

    // ���ӳ���ļ����ڴ�ָ��
    m_pMapHeader = (Map_File_Header*)MapViewOfFile(m_hFileMaping, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
    if (NULL == m_pMapHeader)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameSendNet::InitNet(): FileMapping for Port %d map failed"), m_enPortName));
        goto exitCode;
    }
    m_pMapDataBuffer    = (unsigned char*)m_pMapHeader;
    m_pMapDataBuffer   += sizeof(Map_File_Header);

    // ����IEC61850-9-1����ֵ��������
    m_pSav91Msg = CreateSav91MsgImpl();
    if (NULL == m_pSav91Msg)
    {
        RETAILMSG(OUTPUT_LOG_MSG,(_T("CFrameSendNet::InitNet(): Create IEC61850-9-1 frame manager falied \n")));
        goto exitCode;
    }

    // ����IEC61850-9-2����ֵ��������
    m_pSav92Msg = CreateSav92MsgImpl();
    if (NULL == m_pSav92Msg)
    {
        RETAILMSG(OUTPUT_LOG_MSG,(_T("CFrameSendNet::InitNet(): Create IEC61850-9-2 frame manager falied \n")));
        goto exitCode;
    }

    // ����GOOSE���ĵı�������
    m_pGooseMsg = CreateGooseMsgImpl();
    if (NULL == m_pGooseMsg)
    {
        RETAILMSG(OUTPUT_LOG_MSG,(_T("CFrameSendNet::InitNet(): Create GOOSE frame manager falied \n")));
        goto exitCode;
    }

    // ���Ϊ�ѳɹ���ʼ��
    m_bIsInited = true;
    return true;

exitCode:
    UnInitNet();
    return false;
}

///
/// @brief
///    ����ʼ����̫�����ͣ�����ļ�ӳ��
///
void CFrameSendNet::UnInitNet()
{
    // ֹͣ�ײ�SMV���ݷ���
    StopSMVSend();

    // ֹͣ�ײ�GOOSE���ݷ���
    StopGooseSend();

    // ȡ��ӳ���ϵ
    if (m_pMapHeader != NULL)
    {
        UnmapViewOfFile(m_pMapHeader );
        m_pMapHeader  = NULL;
    }

    // �ر��ڴ�ӳ���ļ�
    if (m_hFileMaping != NULL)
    {
        CloseHandle(m_hFileMaping);
        m_hFileMaping = NULL;
    }

    // �ͷ�IEC61850-9-1֡������
    if (NULL != m_pSav91Msg)
    {
        ReleaseSav91MsgImpl(m_pSav91Msg);
        m_pSav91Msg = NULL;
    }

    // �ͷ�IEC61850-9-2֡������
    if (NULL != m_pSav92Msg)
    {
        ReleaseSav92MsgImpl(m_pSav92Msg);
        m_pSav92Msg = NULL;
    }

    // �ͷ�GOOSE֡������
    if (NULL != m_pGooseMsg)
    {
        ReleaseGooseMsgImpl(m_pGooseMsg);
        m_pGooseMsg = NULL;
    }
}

///
/// @brief
///    Ԥ�ȼ���˿ڵ�SMV����֡
///
/// @param[in]  stDspRecvData    ����ͨ��һ���������εĲ���ֵ
/// @param[out] pStateTestResult �������������֡�Ľṹ��
///
/// @return
///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::PrepareSmvSendBuffer( CDspRecvData& stDspRecvData, CStateTestResult* pStateTestResult )
{
    uint32_t        uiAsduCount         = 0;
    uint32_t        uiFramesTotalLen    = 0;
    uint32_t        uiSmvSendCbCount    = 0;
    enumErrorCode   eRet;


    // ����������
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareSmvSendBuffer(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���û�д����͵Ŀ��ƿ飬��ôֱ�ӷ���
    if (m_vecSMVSendCbs.size() <= 0)
    {
        return ERROR_CODE_SUCCESS;
    }
    uiSmvSendCbCount = (uint32_t)m_vecSMVSendCbs.size();

    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareSmvSendBuffer(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���SMV���Ͳ������ù�����
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareSmvSendBuffer(): Get CParamConfigSMVSend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���ÿ֡���ĵ�ASDU��
    uiAsduCount = pParamConfigSMVSend->GetASDUCount();
    if ((uiAsduCount <= 0) || (uiAsduCount > MAX_ASDU_NUM))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareSmvSendBuffer(): ASDU count error \n"));
        return ERROR_CODE_FAILURE;
    }

    // ��Ӵ����ͻ���
    CStateSendBuffer*   pNewSendBuffer = new CStateSendBuffer(m_enPortName);
    if (NULL == pNewSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareSmvSendBuffer(): new CStateSendData failed \n"));
        return ERROR_CODE_FAILURE;
    }
    pNewSendBuffer->m_stIOCtrlParam.nDeviceItem = m_enPortName;
    pNewSendBuffer->m_stIOCtrlParam.nInBuf[0]   = 0;
    pNewSendBuffer->m_stIOCtrlParam.nInBuf[1]   = (uiAsduCount<<16) | uiSmvSendCbCount; ///< ���ƿ�����ASDU��
    pNewSendBuffer->m_stIOCtrlParam.nInBuf[3]   = pParamConfigSMVSend->GetFrequency();  ///< ������
    pStateTestResult->m_listSmvSendBuffers.push_back(pNewSendBuffer);

    // �������ͻ�����
    if (FRAME_TYPE_NET_9_1 == pParamConfigSMVSend->GetSMVType())
    {
        // ��IEC61850-9-1���ƿ�������֡����䵽�ڴ�ӳ�仺����
        eRet = __FillIec91SendBuffer(m_pMapDataBuffer + NET_SMV_SEND_LOOP_BUF0_POS, NET_SMV_SEND_BUF_LEN, stDspRecvData, pNewSendBuffer->m_stIOCtrlParam, uiFramesTotalLen);
        if (ERROR_CODE_SUCCESS != eRet)
        {
            return eRet;
        }
    }
    else if (FRAME_TYPE_NET_9_2 == pParamConfigSMVSend->GetSMVType())
    {
        // ��IEC61850-9-2���ƿ�������֡����䵽�ڴ�ӳ�仺����
        eRet = __FillIec92SendBuffer(m_pMapDataBuffer + NET_SMV_SEND_LOOP_BUF0_POS, NET_SMV_SEND_BUF_LEN, stDspRecvData, pNewSendBuffer->m_stIOCtrlParam, uiFramesTotalLen);
        if (ERROR_CODE_SUCCESS != eRet)
        {
            return eRet;
        }
    }
    else
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareSmvSendBuffer(): SMV type error \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���뻺��
    if (NULL == pNewSendBuffer->AllocDataBuffer(uiFramesTotalLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareSmvSendBuffer(): Alloc Data Buffer failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // �������з���֡
    if (0 != memcpy_s(pNewSendBuffer->m_pucDataBuffer, uiFramesTotalLen, m_pMapDataBuffer + NET_SMV_SEND_LOOP_BUF0_POS, uiFramesTotalLen))
    {
        return ERROR_CODE_FAILURE;
    }

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    ����SMV���ƿ�
///
/// @param[in]  stDspRecvData    ����ͨ��һ���������εĲ���ֵ
///
/// @return
///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::SendSMVCbs(CDspRecvData& stDspRecvData)
{
    uint32_t                            uiASDUCount;
    uint32_t                            uiFramesTotalLen;
    uint32_t                            uiSMVSendSbCount;
    IOCTRL_IN_PARAM_NET_BIG             stMuSendParam;
    enumErrorCode                       eRet;


    // ���û�д����͵Ŀ��ƿ飬��ôֱ�ӷ���
    if (m_vecSMVSendCbs.size() <= 0)
    {
        return ERROR_CODE_SUCCESS;
    }
    uiSMVSendSbCount = (uint32_t)m_vecSMVSendCbs.size();

    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���SMV���Ͳ������ù�����
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): Get CParamConfigSMVSend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���ÿ֡���ĵ�ASDU��
    uiASDUCount = pParamConfigSMVSend->GetASDUCount();
    if ((uiASDUCount <= 0) || (uiASDUCount > MAX_ASDU_NUM))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): ASDU count error \n"));
        return ERROR_CODE_FAILURE;
    }

    stMuSendParam.nDeviceItem   = m_enPortName;                         ///< �豸��
    stMuSendParam.nInBuf[0]     = 0;                                    ///< SMV
    stMuSendParam.nInBuf[1]     = (uiASDUCount<<16) | uiSMVSendSbCount; ///< ���ƿ�����ASDU��
    stMuSendParam.nInBuf[3]     = pParamConfigSMVSend->GetFrequency();  ///< ������

    // �������ͻ�����
    m_pMapHeader->NetHeader.nCurSmvTxBufItem = 0;                       ///< ��ǰSmv���ͻ����������
    if (FRAME_TYPE_NET_9_1 == pParamConfigSMVSend->GetSMVType())
    {
        // ��IEC61850-9-1���ƿ�������֡����䵽�ڴ�ӳ�仺����
        eRet = __FillIec91SendBuffer(m_pMapDataBuffer + NET_SMV_SEND_LOOP_BUF0_POS, NET_SMV_SEND_BUF_LEN, stDspRecvData, stMuSendParam, uiFramesTotalLen);
        if (ERROR_CODE_SUCCESS != eRet)
        {
            return eRet;
        }
    }
    else if (FRAME_TYPE_NET_9_2 == pParamConfigSMVSend->GetSMVType())
    {
        // ��IEC61850-9-2���ƿ�������֡����䵽�ڴ�ӳ�仺����
        eRet = __FillIec92SendBuffer(m_pMapDataBuffer + NET_SMV_SEND_LOOP_BUF0_POS, NET_SMV_SEND_BUF_LEN, stDspRecvData, stMuSendParam, uiFramesTotalLen);
        if (ERROR_CODE_SUCCESS != eRet)
        {
            return eRet;
        }
    }
    else
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): SMV type error \n"));
        return ERROR_CODE_FAILURE;
    }

    // ����IOCTRL SEND HEADER���ֶ�����ģʽ��
    stMuSendParam.nInBuf[TX_TEST_STYLE_INDEX] = TX_TEST_STYLE_MANUAL_TEST;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_HEADER, &stMuSendParam, sizeof(stMuSendParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): Port%d IOCTRL_SEND_HEADER failed \n", m_enPortName));
        return ERROR_CODE_FAILURE;
    }

    // ����û��Ƿ���ʱȡ���˷�������
    if (m_pDataSendManager->m_eventCancelSMV.Lock(0))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): User cancels the current operation \n"));
        return ERROR_CODE_CANCEL;
    }

    // ��ʼ��������
    IOCTRL_IN_PARAM_NET stIOCtrlInParam;
    stIOCtrlInParam.nDeviceItem = stMuSendParam.nDeviceItem;
    stIOCtrlInParam.nInBuf[0]   = stMuSendParam.nInBuf[0];
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_FRAME, &stIOCtrlInParam, sizeof(stIOCtrlInParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): Port%d IOCTRL_SEND_FRAME failed \n", m_enPortName));
        return ERROR_CODE_FAILURE;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): Port%d IOCTRL_SEND_FRAME successfully \n", m_enPortName));
    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    ���ͻ������е�����֡
///
/// @param[in]  pStateSendBuffer    ָ�򻺳�����ָ��
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSendNet::SendSMVCbs( CStateSendBuffer* pStateSendBuffer )
{
    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): uninit \n"));
        return false;
    }

    // ����������
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): Parameter error \n"));
        return false;
    }

    // �������ͻ�����
    m_pMapHeader->NetHeader.nCurSmvTxBufItem = 0;                       ///< ��ǰSmv���ͻ����������
    if (0 != memcpy_s(m_pMapDataBuffer + NET_SMV_SEND_LOOP_BUF0_POS, NET_SMV_SEND_BUF_LEN, pStateSendBuffer->m_pucDataBuffer, pStateSendBuffer->m_uiDataBufferLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): Fill send buffer failed"));
        return false;
    }

    // ����IOCTRL SEND HEADER������Ϊ״̬����ģʽ��
    pStateSendBuffer->m_stIOCtrlParam.nInBuf[TX_TEST_STYLE_INDEX] = TX_TEST_STYLE_STATUS_SEQUENCE;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_HEADER, &pStateSendBuffer->m_stIOCtrlParam, sizeof(pStateSendBuffer->m_stIOCtrlParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): Port%d IOCTRL_SEND_HEADER failed \n", m_enPortName));
        return false;
    }

    // ��ʼ��������
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_FRAME, &pStateSendBuffer->m_stIOCtrlParam, sizeof(pStateSendBuffer->m_stIOCtrlParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): Port%d IOCTRL_SEND_FRAME failed \n", m_enPortName));
        return false;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): Port%d IOCTRL_SEND_FRAME successfully \n", m_enPortName));
    return true;
}

///
/// @brief
///    ׼��GOOSE���ƿ鷢��֡
///
/// @param[in]  pStateTestResult    ָ�����������ָ�룬���ڱ���GOOSE����֡
///
/// @return
///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::PrepareGooseSendBuffer( CStateTestResult* pStateTestResult )
{
    uint32_t    uiFramesTotalLen;


    // ����������
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareGooseSendBuffer(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���û�д����͵Ŀ��ƿ飬��ôֱ�ӷ���
    if (m_vecGOOSESendCbs.size() <= 0)
    {
        return ERROR_CODE_SUCCESS;
    }

    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareGooseSendBuffer(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���GOOSE���Ͳ������ù�����
    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__PrepareGooseSendBuffer(): Get CParamConfigGOOSESend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // ��Ӵ����ͻ���
    CStateSendBuffer*   pNewSendBuffer = new CStateSendBuffer(m_enPortName);
    if (NULL == pNewSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareGooseSendBuffer(): new CStateSendData failed \n"));
        return ERROR_CODE_FAILURE;
    }
    pNewSendBuffer->m_stIOCtrlParam.nDeviceItem = m_enPortName;                                 ///< �豸��
    pNewSendBuffer->m_stIOCtrlParam.nInBuf[0]   = 1;                                            ///< GOOSE
    pNewSendBuffer->m_stIOCtrlParam.nInBuf[1]   = pParamConfigGooseSend->GetHeartbeatInterval();///< T0 ֵ
    pNewSendBuffer->m_stIOCtrlParam.nInBuf[2]   = pParamConfigGooseSend->GetMinimumInterval();  ///< T1 ֵ
    pNewSendBuffer->m_stIOCtrlParam.nInBuf[3]   = (uint32_t)m_vecGOOSESendCbs.size();           ///< ���ƿ���
    pStateTestResult->m_listGooseSendBuffers.push_back(pNewSendBuffer);

    // �������ͻ�����
    enumErrorCode   eRet = __FillGooseSendBuffer(m_pMapDataBuffer + GOOSE_SEND_LOOP_BUF0_POS, GOOSE_SEND_BUF_LEN, pNewSendBuffer, uiFramesTotalLen);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        return eRet;
    }

    // ���뻺��
    if (NULL == pNewSendBuffer->AllocDataBuffer(uiFramesTotalLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareGooseSendBuffer(): Alloc Data Buffer failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // �������з���֡
    if (0 != memcpy_s(pNewSendBuffer->m_pucDataBuffer, uiFramesTotalLen, m_pMapDataBuffer + GOOSE_SEND_LOOP_BUF0_POS, uiFramesTotalLen))
    {
        return ERROR_CODE_FAILURE;
    }

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    ׼��GOOSE���ƿ����֡
///
/// @param[in]  pStateSendBuffer    ָ��GOOSE����֡
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSendNet::PrepareGooseUpdateBuffer( CStateSendBuffer* pStateSendBuffer )
{
    // ����������
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareGooseUpdateBuffer(): Parameter error \n"));
        return false;
    }

    // ���û�д����͵Ŀ��ƿ飬��ôֱ�ӷ���
    if (m_vecGOOSESendCbs.size() <= 0)
    {
        return true;
    }

    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareGooseUpdateBuffer(): uninit \n"));
        return false;
    }

    //
    // ���ñ�λ���
    //
    int                                 iCbIndex;                       ///< ���ƿ��������
    std::vector<CGooseCb*>::iterator    iter;
    pStateSendBuffer->m_stIOCtrlParam.nDeviceItem = m_enPortName;       ///< �豸��
    pStateSendBuffer->m_stIOCtrlParam.nInBuf[0]   = 1;                  ///< GOOSE
    pStateSendBuffer->m_stIOCtrlParam.nInBuf[1]   = 0;                  ///< GOOSE��λ���
    for (iCbIndex = 0, iter = m_vecGOOSESendCbs.begin(); iter != m_vecGOOSESendCbs.end(); iCbIndex++, iter++)
    {
        CGooseCb* pGooseCb = (CGooseCb*)*iter;
        if (pGooseCb && pGooseCb->IsSendValueChanged())                 ///< GOOSEͨ��ֵ�����˸ı�
        {
            // ��һ��GOOSE���ƿ鷢����λ�ˣ���Ҫ����Ӧ����λ��1
            pStateSendBuffer->m_stIOCtrlParam.nInBuf[1] |= (1 << iCbIndex);

            // ����GOOSE���ƿ��stNum
            pGooseCb->SetStNum(pGooseCb->GetStNum() + 1);
        }
    }

    // �Է����˱�λ�Ŀ��ƿ�������±���
    if (!pStateSendBuffer->RecodeChangedGooseCb())
    {
        return false;
    }

    // �������з���֡
    return true;
}

///
/// @brief
///    ����GOOSE���ƿ�
///
/// @return
///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::SendGooseCbs()
{
    IOCTRL_IN_PARAM_NET_BIG             stMuSendParam;
    enumErrorCode                       eRet;


    // ���û�д����͵Ŀ��ƿ飬��ôֱ�ӷ���
    if (m_vecGOOSESendCbs.size() <= 0)
    {
        return ERROR_CODE_SUCCESS;
    }

    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���GOOSE���Ͳ������ù�����
    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::SendGooseCbs(): Get CParamConfigGOOSESend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    stMuSendParam.nDeviceItem   = m_enPortName;                                     ///< �豸��
    stMuSendParam.nInBuf[0]     = 1;                                                ///< GOOSE
    stMuSendParam.nInBuf[1]     = pParamConfigGooseSend->GetHeartbeatInterval();    ///< T0 ֵ
    stMuSendParam.nInBuf[2]     = pParamConfigGooseSend->GetMinimumInterval();      ///< T1 ֵ
    stMuSendParam.nInBuf[3]     = (uint32_t)m_vecGOOSESendCbs.size();               ///< ���ƿ���

    // ���UTCʱ��
    UTC_TIME_t stUtcTime;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_GET_UTC_TIME, NULL, NULL, &stUtcTime, sizeof(stUtcTime), NULL ,NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Port%d IOCTRL_GET_UTC_TIME failed \n", m_enPortName));
        return ERROR_CODE_FAILURE;
    }

    // �������ͻ�����
    m_pMapHeader->NetHeader.nCurGooseTxBufItem = 0;
    eRet = __FillGooseSendBuffer(m_pMapDataBuffer + GOOSE_SEND_LOOP_BUF0_POS, GOOSE_SEND_BUF_LEN, stUtcTime, stMuSendParam);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        return eRet;
    }

    // ����IOCTRL SEND HEADER������Ϊ�ֶ�����ģʽ��
    stMuSendParam.nInBuf[TX_TEST_STYLE_INDEX] = TX_TEST_STYLE_MANUAL_TEST;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_HEADER, &stMuSendParam, sizeof(stMuSendParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Port%d IOCTRL_SEND_HEADER failed \n", m_enPortName));
        return ERROR_CODE_FAILURE;
    }

    // ����û��Ƿ���ʱȡ���˷�������
    if (m_pDataSendManager->m_eventCancelSMV.Lock(0))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): User cancels the current operation \n"));
        return ERROR_CODE_CANCEL;
    }

    // ��ʼ��������
    IOCTRL_IN_PARAM_NET stIOCtrlInParam;
    stIOCtrlInParam.nDeviceItem = stMuSendParam.nDeviceItem;
    stIOCtrlInParam.nInBuf[0]   = stMuSendParam.nInBuf[0];
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_FRAME, &stIOCtrlInParam, sizeof(stIOCtrlInParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Port%d IOCTRL_SEND_FRAME failed \n", m_enPortName));
        return ERROR_CODE_FAILURE;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Port%d IOCTRL_SEND_FRAME successfully \n", m_enPortName));
    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    ����GOOSE���ƿ�
///
/// @param[in]  pStateSendBuffer    ָ�򻺳�����ָ��
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSendNet::SendGooseCbs( CStateSendBuffer* pStateSendBuffer )
{
    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): uninit \n"));
        return false;
    }

    // ����������
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Parameter error \n"));
        return false;
    }

    // �������ͻ�����
    m_pMapHeader->NetHeader.nCurGooseTxBufItem = 0;                     ///< ��ǰSmv���ͻ����������
    if (0 != memcpy_s(m_pMapDataBuffer + GOOSE_SEND_LOOP_BUF0_POS, GOOSE_SEND_BUF_LEN, pStateSendBuffer->m_pucDataBuffer, pStateSendBuffer->m_uiDataBufferLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Fill send buffer failed"));
        return false;
    }

    // ����IOCTRL SEND HEADER������Ϊ״̬����ģʽ��
    pStateSendBuffer->m_stIOCtrlParam.nInBuf[TX_TEST_STYLE_INDEX] = TX_TEST_STYLE_STATUS_SEQUENCE;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_HEADER, &pStateSendBuffer->m_stIOCtrlParam, sizeof(pStateSendBuffer->m_stIOCtrlParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Port%d IOCTRL_SEND_HEADER failed \n", m_enPortName));
        return false;
    }

    // ��ʼ��������
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_FRAME, &pStateSendBuffer->m_stIOCtrlParam, sizeof(pStateSendBuffer->m_stIOCtrlParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Port%d IOCTRL_SEND_FRAME failed \n", m_enPortName));
        return false;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Port%d IOCTRL_SEND_FRAME successfully \n", m_enPortName));
    return true;
}

///
/// @brief
///    ֹͣ�ײ����ݷ���
///
/// @return
/// true��ʶֹͣ�ɹ�
///
bool CFrameSendNet::StopSMVSend()
{
    IOCTRL_IN_PARAM_NET stIOCtrlInParam;


    stIOCtrlInParam.nDeviceItem     = m_enPortName;                     ///< �豸��
    stIOCtrlInParam.nInBuf[0]       = 0;                                ///< SMV
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_STOP_SEND, &stIOCtrlInParam, sizeof(stIOCtrlInParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::StopSMVSend(): Port%d IOCTRL_STOP_SEND failed \n", m_enPortName));
        return false;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::StopSMVSend(): Port%d IOCTRL_STOP_SEND successfully \n", m_enPortName));
    return true;
}

///
/// @brief
///    ֹͣGOOSE�ײ����ݷ���
///
bool CFrameSendNet::StopGooseSend()
{
    IOCTRL_IN_PARAM_NET stIOCtrlInParam;


    stIOCtrlInParam.nDeviceItem     = m_enPortName;                     ///< �豸��
    stIOCtrlInParam.nInBuf[0]       = 1;                                ///< GOOSE
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_STOP_SEND, &stIOCtrlInParam, sizeof(stIOCtrlInParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::StopGooseSend(): Port%d IOCTRL_STOP_SEND failed \n", m_enPortName));
        return false;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::StopGooseSend(): Port%d IOCTRL_STOP_SEND successfully \n", m_enPortName));
    return true;
}

///
/// @brief
///    ��ձ��������SMV���ƿ������
///
void CFrameSendNet::RemoveAllSMVSendCbs()
{
    m_vecSMVSendCbs.clear();
}

///
/// @brief
///    ��Ӵ����͵�SMV���ƿ�
///
/// @param[in]  pNewCb    SMV���ƿ�
///
/// @return
///     bool    ��ӳɹ�����true
///
bool CFrameSendNet::AddSMVSendCb( CBaseCb* pNewCb )
{
    // ����������
    if (NULL == pNewCb)
    {
        return false;
    }

    // ����Ƿ�����ظ��Ŀ��ƿ�
    std::vector<CBaseCb*>::iterator iter = m_vecSMVSendCbs.begin();
    while(m_vecSMVSendCbs.end() != iter)
    {
        CBaseCb *pBaseCb = *iter;
        if (pBaseCb && (pBaseCb == pNewCb))
        {
            return false;
        }

        iter++;
    }

    // ��ӿ��ƿ�ָ�뵽������
    m_vecSMVSendCbs.push_back(pNewCb);
    return true;
}

///
/// @brief
///    ��ձ��������GOOSE���ƿ������
///
void CFrameSendNet::RemoveAllGOOSESendCbs()
{
    m_vecGOOSESendCbs.clear();
}

///
/// @brief
///    ��Ӵ����͵�GOOSE���ƿ�
///
/// @param[in]  pNewCb    GOOSE���ƿ�
///
/// @return
///     bool    ��ӳɹ�����true
///
bool CFrameSendNet::AddGooseSendCb( CGooseCb* pNewCb )
{
    // ����������
    if (NULL == pNewCb)
    {
        return false;
    }

    // ����Ƿ�����ظ��Ŀ��ƿ�
    std::vector<CGooseCb*>::iterator iter = m_vecGOOSESendCbs.begin();
    while(m_vecGOOSESendCbs.end() != iter)
    {
        CGooseCb *pBaseCb = *iter;
        if (pBaseCb && (pBaseCb == pNewCb))
        {
            return false;
        }

        iter++;
    }

    // ��ӿ��ƿ�ָ�뵽������
    m_vecGOOSESendCbs.push_back(pNewCb);
    return true;
}

///
/// @brief
///    ��䱨�ķ��ͻ�������IEC61850-9-1��
///
/// @param[out]  pDataBuffer        �����ͻ�����
/// @param[in]   uiDataBufferLen    ���ͻ�������С���ֽڣ�
/// @param[in]   stDspRecvData      ����ͨ��һ���������εĲ���ֵ
/// @param[out]  stMuSendParam      ����ӿ�DeviceIoControl()�����ݲ����ṹ��
/// @param[out]  uiFramesTotalLen   ���д�����֡�ĳ��Ⱥͣ��ֽڣ�
///
/// @return
///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::__FillIec91SendBuffer( unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CDspRecvData& stDspRecvData, IOCTRL_IN_PARAM_NET_BIG& stMuSendParam, uint32_t& uiFramesTotalLen )
{
    unsigned char*      pDataBufferEnd          = NULL;
    uint8_t             ucMacDes[ETHER_MAC_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t             ucMacSrc[ETHER_MAC_LEN] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff};


    // ����������
    if (NULL == pDataBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): NULL parameter \n"));
        return ERROR_CODE_FAILURE;
    }

    // ��黺������С
    if (uiDataBufferLen <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): uiDataBufferLen <= 0 \n"));
        return ERROR_CODE_FAILURE;
    }
    pDataBufferEnd = pDataBuffer + uiDataBufferLen;                     ///< ָ�򻺳���ĩβ

    // ����Ƿ��д����͵Ŀ��ƿ�
    if (m_vecSMVSendCbs.size() <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): No control block to be transmitted \n"));
        return ERROR_CODE_FAILURE;
    }

    // ��û����������ù�����
    CParamConfigBasic* pParamConfigBasic = CParamConfigBasic::getInstance();
    if (NULL == pParamConfigBasic)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): Get CParamConfigBasic object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���SMV���Ͳ������ù�����
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): Get CParamConfigSMVSend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���SMV����
    enumFrameType eFrameType = pParamConfigSMVSend->GetSMVType();
    if (FRAME_TYPE_NET_9_1 != eFrameType)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): SMV type error \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���ÿ֡���ĵ�ASDU��
    int iASDUCount = pParamConfigSMVSend->GetASDUCount();
    if (iASDUCount <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): ASDU count error \n"));
        return ERROR_CODE_FAILURE;
    }

    // �����������������������ASDU��������ô���á�ÿ�����ƿ��֡����=������������/��ASDU����
    // �������á�ÿ�����ƿ��֡����=������������
    unsigned int iFrameCount = stDspRecvData.m_iSmvCount;
    if (0 == (stDspRecvData.m_iSmvCount) % iASDUCount)
    {
        iFrameCount = (stDspRecvData.m_iSmvCount) / iASDUCount;
    }
    stMuSendParam.nInBuf[2]     = iFrameCount;                          ///< һ�����ƿ����֡��

    CSmvScaler  SmvValueCaler;
    // ���ñ���ֵ����
    SmvValueCaler.SetChannelMsgValType(pParamConfigBasic->GetFrameValueType());
    // ����ͨ������Ȼֵ����
    SmvValueCaler.SetChannelNtrValType(pParamConfigSMVSend->GetSVDisplay());

    //
    // ����ÿһ�����ֵ���ϳɷ��ͱ���֡����ӵ����ͻ���
    //
    uiFramesTotalLen  = 0;
    for (unsigned int iFrameIndex = 0; iFrameIndex < iFrameCount; iFrameIndex++)
    {
        int                             iCbIndex;
        std::vector<CBaseCb*>::iterator iter;
        for (iCbIndex = 0, iter = m_vecSMVSendCbs.begin(); iter != m_vecSMVSendCbs.end(); iCbIndex++, iter++)
        {
            CIEC9_1Cb* pIEC9_1Cb = (CIEC9_1Cb*)*iter;


            // ����û��Ƿ���ʱȡ���˷�������
            if (m_pDataSendManager && m_pDataSendManager->m_eventCancelSMV.Lock(0))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): User cancels the current operation \n"));
                return ERROR_CODE_CANCEL;
            }

            ///< ���ƿ�ͨ��������Ϊ0
            if ((NULL == pIEC9_1Cb) || (0 == pIEC9_1Cb->GetChannelCount()))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): No Channel \n"));
                return ERROR_CODE_FAILURE;
            }

            // ���ͨ�����Ƿ���
            if (pIEC9_1Cb->GetChannelCount() > ETHER_IEC91_CHANNEL_MAX)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): Excessive number of channels \n"));
                return ERROR_CODE_FAILURE;
            }

            //
            // ���ÿ��ƿ������Ϣ
            //
            m_pSav91Msg->ClearSavPDU();
            pIEC9_1Cb->GetDesMAC(ucMacDes, ETHER_MAC_LEN);
            pIEC9_1Cb->GetSrcMAC(ucMacSrc, ETHER_MAC_LEN);
            m_pSav91Msg->SetMacAddr(ucMacDes, ucMacSrc, ETHER_MAC_LEN,pIEC9_1Cb->GetVlanFlag());
            m_pSav91Msg->Set8021QVlanInfo(pIEC9_1Cb->GetVLanPriority(), pIEC9_1Cb->GetVlanID(),pIEC9_1Cb->GetVlanFlag());
            m_pSav91Msg->SetAppID(pIEC9_1Cb->GetAPPID());

            //
            // ����������ӿ��ƿ�ASDU��Ϣ
            //
            for (int iAsduIndex = 0; iAsduIndex < iASDUCount; iAsduIndex++)
            {
                // ����������
                int iSmvCnt = (iFrameIndex * iASDUCount + iAsduIndex) % (stDspRecvData.m_iSmvCount);

                int     iChannelIndex       = 0;                        ///< ͨ������
                double  dSmvValue           = 0.0f;                     ///< ����ֵ
                double  dOnceRatings        = 0.0f;                     ///< һ�ζֵ
                double  dSecondaryRatings   = 0.0f;                     ///< ���ζֵ
                for (CChannel* pChannel = pIEC9_1Cb->FirstChannel(); pChannel != NULL; 
                    iChannelIndex++, pChannel = pIEC9_1Cb->NextChannel(pChannel))
                {
                    dSmvValue           = 0.0f;                             ///< ����ֵ
                    dOnceRatings        = pChannel->GetOnceRatings();       ///< һ�ζֵ
                    dSecondaryRatings   = pChannel->GetSecondaryRatings();  ///< ���ζֵ

                    //
                    // �����ͨ�����������ⷢ��ͨ������ô��������˲ʱ����ֵ
                    //
                    std::wstring strChannelName = pChannel->GetVirtualChannel();
                    if (strChannelName.size())
                    {
                        std::map<std::wstring, int>::iterator iter = stDspRecvData.m_mapChannelName2Index.find(strChannelName);
                        if (stDspRecvData.m_mapChannelName2Index.end() != iter)
                        {
                            // ����˲ʱ����ֵ
                            dSmvValue = stDspRecvData.m_stRecvData.fSmvGenData[iter->second][iSmvCnt];
                        }
                    }

                    // ����ͨ�����ͣ����÷���֡�е�ͨ��ֵ
                    switch (pChannel->GetChannelType())
                    {
                    case CHANNEL_TYPE_UNKOWN:                           ///< δ֪����
                        {
                            m_pusIEC91ValArr[iChannelIndex] = 0;
                        }
                        break;

                    case CHANNEL_TYPE_SMV_CURRENT:                      ///< ��������
                        {
                            // ����˲ʱ����ֵ�����÷���֡��ͨ��ֵ
                            m_pusIEC91ValArr[iChannelIndex] = SmvValueCaler.ChannelNtrVal2MsgVal_Sav91(
                                (float)dSmvValue,
                                (float)dOnceRatings,
                                (float)dSecondaryRatings,
                                CHANNEL_TYPE_SMV_CURRENT,
                                (pChannel->IsMeasure() ? CHANNEL_VALUE_TYPE_MEASURE : CHANNEL_VALUE_TYPE_PROTECT)
                                );
                        }
                        break;

                    case CHANNEL_TYPE_SMV_VOLTAGE:                      ///< ��ѹ����
                        {
                            // ����˲ʱ����ֵ�����÷���֡��ͨ��ֵ
                            m_pusIEC91ValArr[iChannelIndex] = SmvValueCaler.ChannelNtrVal2MsgVal_Sav91(
                                (float)dSmvValue,
                                (float)dOnceRatings,
                                (float)dSecondaryRatings,
                                CHANNEL_TYPE_SMV_VOLTAGE,
                                CHANNEL_VALUE_TYPE_UNKNOWN
                                );
                        }
                        break;

                    case CHANNEL_TYPE_SMV_TIME:                         ///< ʱ������
                        {
                            m_pusIEC91ValArr[iChannelIndex] = (uint16_t)dSmvValue;
                        }
                        break;

                    default:
                        {
                            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): Channel Type error \n"));
                            return ERROR_CODE_FAILURE;
                        }
                    }
                }

                // ���ASDU��
                SAV91_ASDU_PUBLIC stAsduPublic = {0};
                stAsduPublic.asduLen        = (uint16_t)((iChannelIndex + 10) * sizeof(m_pusIEC91ValArr[0]));
                stAsduPublic.lnName         = 0x02;
                stAsduPublic.dataSetName    = 0x01;
                stAsduPublic.ldName         = 0x00;
                stAsduPublic.phsArtg        = (uint16_t)(pIEC9_1Cb->GetRatedPhaseCurrent());;
                stAsduPublic.neutArtg       = (uint16_t)(pIEC9_1Cb->GetRatedNeutralCurrent());
                stAsduPublic.phsVrtg        = SmvValueCaler.PhsVrtgNtrVal2MsgVal_Sav91((float)pIEC9_1Cb->GetRatedPhaseVoltage());
                stAsduPublic.delayTime      = pIEC9_1Cb->GetRatedDelay();
                stAsduPublic.statusWord1    = pIEC9_1Cb->GetStatusWord1();
                stAsduPublic.statusWord2    = pIEC9_1Cb->GetStatusWord2();
                stAsduPublic.smpCnt         = iSmvCnt;
                stAsduPublic.smpRate        = (uint8_t)(stDspRecvData.m_iSmvCount);
                stAsduPublic.confRev        = (uint8_t)pIEC9_1Cb->GetConfigRevision();
                if (!m_pSav91Msg->AddAnASDU(stAsduPublic, m_pusIEC91ValArr, iChannelIndex))
                {
                    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): Add ASDU%d fail \n", iAsduIndex));
                    return ERROR_SMV_TOO_MANY_CHANNEL_ASDU;
                }
            } // end ASDU

            //
            // �������õĲ������б��룬��ñ���֡
            //
            int iByteCount = (int)(sizeof(m_pucFrameBuffer[0]) * m_pSav91Msg->Code(m_pucFrameBuffer, sizeof(m_pucFrameBuffer)));
            if (0 == iByteCount)                                        ///< ���֡��С
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): Code frame failed \n"));
                return ERROR_CODE_FAILURE;
            }

            // ��¼֡��
            if ((4 + iCbIndex) >= sizeof(stMuSendParam.nInBuf)/sizeof(stMuSendParam.nInBuf[0]))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): Record frame length failed\n"));
                return ERROR_CODE_FAILURE;
            }
            stMuSendParam.nInBuf[4 + iCbIndex] = iByteCount;

            //
            // ��������֡�����ͻ�����
            //
            if ((pDataBuffer + iByteCount) > pDataBufferEnd)            ///< �ж��Ƿ�Խ��
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): Oversize frame \n"));
                return ERROR_CODE_FAILURE;
            }
            memcpy(pDataBuffer, m_pucFrameBuffer, iByteCount);

            // ����������ָ�루4�ֽڶ��룩
            iByteCount      = ((iByteCount & 0x03) != 0)? ((iByteCount & 0xFFFC) + 4): iByteCount;
            pDataBuffer    += iByteCount;
            uiFramesTotalLen += iByteCount;
        } // end ���ƿ�
    } // end ������

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    ��䱨�ķ��ͻ�������IEC61850-9-2��
///
/// @param[out]  pDataBuffer        �����ͻ�����
/// @param[in]   uiDataBufferLen    ���ͻ�������С���ֽڣ�
/// @param[in]   stDspRecvData      ����ͨ��һ���������εĲ���ֵ
/// @param[out]  stMuSendParam      ����ӿ�DeviceIoControl()�����ݲ����ṹ��
/// @param[out]  uiFramesTotalLen   ���д�����֡�ĳ��Ⱥͣ��ֽڣ�
///
/// @return
///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::__FillIec92SendBuffer( unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CDspRecvData& stDspRecvData, IOCTRL_IN_PARAM_NET_BIG& stMuSendParam, uint32_t& uiFramesTotalLen )
{
    unsigned char*          pDataBufferEnd          = NULL;
    uint8_t                 ucMacDes[ETHER_MAC_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t                 ucMacSrc[ETHER_MAC_LEN] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff};


    // ����������
    if (NULL == pDataBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): NULL parameter \n"));
        return ERROR_CODE_FAILURE;
    }

    // ��黺������С
    if (uiDataBufferLen <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): uiDataBufferLen <= 0 \n"));
        return ERROR_CODE_FAILURE;
    }
    pDataBufferEnd = pDataBuffer + uiDataBufferLen;                     ///< ָ�򻺳���ĩβ

    // ����Ƿ��д����͵Ŀ��ƿ�
    if (m_vecSMVSendCbs.size() <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): No control block to be transmitted \n"));
        return ERROR_CODE_FAILURE;
    }

    // ��û����������ù�����
    CParamConfigBasic* pParamConfigBasic = CParamConfigBasic::getInstance();
    if (NULL == pParamConfigBasic)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): Get CParamConfigBasic object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���SMV���Ͳ������ù�����
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): Get CParamConfigSMVSend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���SMV����
    enumFrameType eFrameType = pParamConfigSMVSend->GetSMVType();
    if (FRAME_TYPE_NET_9_2 != eFrameType)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): SMV type error \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���ÿ֡���ĵ�ASDU��
    int iASDUCount = pParamConfigSMVSend->GetASDUCount();
    if (iASDUCount <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): ASDU count error \n"));
        return ERROR_CODE_FAILURE;
    }

    // �����������������������ASDU��������ô���á�ÿ�����ƿ��֡����=������������/��ASDU����
    // �������á�ÿ�����ƿ��֡����=������������
    unsigned int iFrameCount = stDspRecvData.m_iSmvCount;
    if (0 == (stDspRecvData.m_iSmvCount) % iASDUCount)
    {
        iFrameCount = (stDspRecvData.m_iSmvCount) / iASDUCount;
    }
    stMuSendParam.nInBuf[2]     = iFrameCount;                          ///< һ�����ƿ����֡��

    CSmvScaler  SmvValueCaler;
    // ���ñ���ֵ����
    SmvValueCaler.SetChannelMsgValType(pParamConfigBasic->GetFrameValueType());
    // ����ͨ������Ȼֵ����
    SmvValueCaler.SetChannelNtrValType(pParamConfigSMVSend->GetSVDisplay());

    //
    // ����ÿһ�����ֵ���ϳɷ��ͱ���֡����ӵ����ͻ���
    //
    uiFramesTotalLen  = 0;
    for (unsigned int iFrameIndex = 0; iFrameIndex < iFrameCount; iFrameIndex++)
    {
        int                             iCbIndex;
        std::vector<CBaseCb*>::iterator iter;
        for (iCbIndex = 0, iter = m_vecSMVSendCbs.begin(); iter != m_vecSMVSendCbs.end(); iter++, iCbIndex++)
        {
            CIEC9_2Cb* pIEC9_2Cb = (CIEC9_2Cb*)*iter;


            // ����û��Ƿ���ʱȡ���˷�������
            if (m_pDataSendManager && m_pDataSendManager->m_eventCancelSMV.Lock(0))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): User cancels the current operation \n"));
                return ERROR_CODE_CANCEL;
            }

            // ���ƿ�ͨ��������Ϊ0
            if ((NULL == pIEC9_2Cb) || (0 == pIEC9_2Cb->GetChannelCount()))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): No Channel \n"));
                return ERROR_CODE_FAILURE;
            }

            // ���ͨ�����Ƿ���
            if (pIEC9_2Cb->GetChannelCount() > ETHER_IEC92_CHANNEL_MAX)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): Excessive number of channels \n"));
                return ERROR_CODE_FAILURE;
            }

            //
            // ���ÿ��ƿ������Ϣ
            //
            m_pSav92Msg->ClearSavPDU();
            pIEC9_2Cb->GetDesMAC(ucMacDes, ETHER_MAC_LEN);
            pIEC9_2Cb->GetSrcMAC(ucMacSrc, ETHER_MAC_LEN);
            m_pSav92Msg->SetMacAddr(ucMacDes, ucMacSrc, ETHER_MAC_LEN,pIEC9_2Cb->GetVlanFlag());
            m_pSav92Msg->Set8021QVlanInfo(pIEC9_2Cb->GetVLanPriority(), pIEC9_2Cb->GetVlanID(),pIEC9_2Cb->GetVlanFlag());
            m_pSav92Msg->SetAppID(pIEC9_2Cb->GetAPPID());
			
            // ���ÿ�ѡ��
            SAV92_SAVPDU_OPTS   stSav92Opts;
            stSav92Opts.bDataSetOpt     = pIEC9_2Cb->GetDataSetOpt();
            stSav92Opts.bRefrTmOpt      = pIEC9_2Cb->GetRefrTmOpt();
            stSav92Opts.bSecurityOpt    = pIEC9_2Cb->GetSecurityOpt();
            stSav92Opts.bSmpRateOpt     = pIEC9_2Cb->GetSmpRateOpt();
            m_pSav92Msg->SetSavPduOpts(stSav92Opts);

            //
            // ����������ӿ��ƿ�ASDU��Ϣ
            //
            for (int iAsduIndex = 0; iAsduIndex < iASDUCount; iAsduIndex++)
            {
                // ����������
                int iSmvCnt = (iFrameIndex * iASDUCount + iAsduIndex) % (stDspRecvData.m_iSmvCount);

                int     iChannelIndex       = 0;                        ///< ͨ������
                double  dSmvValue           = 0.0f;                     ///< ����ֵ
                double  dOnceRatings        = 0.0f;                     ///< һ�ζֵ
                double  dSecondaryRatings   = 0.0f;                     ///< ���ζֵ
                std::list<CChannel *>::iterator iterChannel;
                for (CChannel* pChannel = pIEC9_2Cb->FirstChannel(); pChannel != NULL;
                    iChannelIndex++, pChannel = pIEC9_2Cb->NextChannel(pChannel))
                {
                    dSmvValue           = 0.0f;                             ///< ����ֵ
                    dOnceRatings        = pChannel->GetOnceRatings();       ///< һ�ζֵ
                    dSecondaryRatings   = pChannel->GetSecondaryRatings();  ///< ���ζֵ

                    //
                    // �����ͨ�����������ⷢ��ͨ������ô��������˲ʱ����ֵ
                    //
                    std::wstring strChannelName = pChannel->GetVirtualChannel();
                    if (strChannelName.size())
                    {
                        std::map<std::wstring, int>::iterator iter = stDspRecvData.m_mapChannelName2Index.find(strChannelName);
                        if (stDspRecvData.m_mapChannelName2Index.end() != iter)
                        {
                            // ����˲ʱ����ֵ
                            dSmvValue = stDspRecvData.m_stRecvData.fSmvGenData[iter->second][iSmvCnt];
                        }
                    }

                    // ����ͨ�����ͣ����÷���֡�е�ͨ��ֵ
                    switch (pChannel->GetChannelType())
                    {
                    case CHANNEL_TYPE_UNKOWN:                           ///< δ֪����
                        {
                            m_puiIEC92ValArr[iChannelIndex] = 0;
                        }
                        break;

                    case CHANNEL_TYPE_SMV_CURRENT:                      ///< ��������
                        {
                            // ����˲ʱ����ֵ�����÷���֡��ͨ��ֵ
                            m_puiIEC92ValArr[iChannelIndex] = SmvValueCaler.ChannelNtrVal2MsgVal_Sav92(
                                (float)dSmvValue,
                                (float)dOnceRatings,
                                (float)dSecondaryRatings,
                                CHANNEL_TYPE_SMV_CURRENT
                                );
                        }
                        break;

                    case CHANNEL_TYPE_SMV_VOLTAGE:                      ///< ��ѹ����
                        {
                            // ����˲ʱ����ֵ�����÷���֡��ͨ��ֵ
                            m_puiIEC92ValArr[iChannelIndex] = SmvValueCaler.ChannelNtrVal2MsgVal_Sav92(
                                (float)dSmvValue,
                                (float)dOnceRatings,
                                (float)dSecondaryRatings,
                                CHANNEL_TYPE_SMV_VOLTAGE
                                );
                        }
                        break;

                    case CHANNEL_TYPE_SMV_TIME:                         ///< ʱ������
                        {
                            m_puiIEC92ValArr[iChannelIndex] = (uint32_t)dSmvValue;
                        }
                        break;

                    default:
                        {
                            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): Channel Type error \n"));
                            return ERROR_CODE_FAILURE;
                        }
                    }

                    // ͨ��Ʒ��ֵ
                    m_puiIEC92QualityArr[iChannelIndex] = pChannel->GetQuality();
                }

                // ���ASDU��
                std::string         strText;
                SAV92_ASDU_PUBLIC   stAsduPublic;
                std::wstring strDataSet = pIEC9_2Cb->GetDataSetRef();
                strText.assign(strDataSet.begin(), strDataSet.end());
                if (SAV92_VISIBLE_STR_MAX_SIZE > strText.length())
                {
                    memcpy_s(stAsduPublic.datSetStr, SAV92_VISIBLE_STR_MAX_SIZE, strText.c_str(), strText.length());
                    stAsduPublic.datSetStr[strText.length()] = '\0';
                }

                std::wstring strSvID = pIEC9_2Cb->GetSvID();
                strText.assign(strSvID.begin(), strSvID.end());
                if (SAV92_VISIBLE_STR_MAX_SIZE > strText.length())
                {
                    memcpy_s(stAsduPublic.svIDStr, SAV92_VISIBLE_STR_MAX_SIZE, strText.c_str(), strText.length());
                    stAsduPublic.svIDStr[strText.length()] = '\0';
                }

                stAsduPublic.smpCnt         = iSmvCnt;
                stAsduPublic.confRev        = pIEC9_2Cb->GetConfigRevision();
                stAsduPublic.refrTm.ts_s    = 0;
                stAsduPublic.refrTm.ts_ns   = 0;
                stAsduPublic.refrTm.ts_q    = 0;
                stAsduPublic.smpSynch       = true;
                stAsduPublic.smpRate        = (uint16_t)(stDspRecvData.m_iFrequency);
                if (!m_pSav92Msg->AddAnASDU(stAsduPublic, m_puiIEC92ValArr, m_puiIEC92QualityArr, iChannelIndex))
                {
                    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): Add ASDU%d fail \n", iAsduIndex));
                    return ERROR_SMV_TOO_MANY_CHANNEL_ASDU;
                }
            } // end ASDU

            //
            // �������õĲ������б��룬��ñ���֡
            //
            int iByteCount = (int)(sizeof(m_pucFrameBuffer[0]) * m_pSav92Msg->Code(m_pucFrameBuffer, sizeof(m_pucFrameBuffer)));
            if (0 == iByteCount)                                        ///< ���֡��С
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): Code frame failed \n"));
                return ERROR_CODE_FAILURE;
            }

            // ��¼֡��
            if ((4 + iCbIndex) >= sizeof(stMuSendParam.nInBuf)/sizeof(stMuSendParam.nInBuf[0]))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): Record frame length failed\n"));
                return ERROR_CODE_FAILURE;
            }
            stMuSendParam.nInBuf[4 + iCbIndex] = iByteCount;

            //
            // ��������֡�����ͻ�����
            //
            if ((pDataBuffer + iByteCount) > pDataBufferEnd)            ///< �ж��Ƿ�Խ��
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): Oversize frame \n"));
                return ERROR_CODE_FAILURE;
            }
            memcpy(pDataBuffer, m_pucFrameBuffer, iByteCount);

            // ����������ָ�루4�ֽڶ��룩
            iByteCount      = ((iByteCount & 0x03) != 0)? ((iByteCount & 0xFFFC) + 4): iByteCount;
            pDataBuffer    += iByteCount;
            uiFramesTotalLen += iByteCount;
        } // end ���ƿ�
    } // end ������

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    ��䱨�ķ��ͻ�������GOOSE��
///
/// @param[in]  pDataBuffer         �����ͻ�����
/// @param[in]  uiDataBufferLen     ���ͻ�������С���ֽڣ�
/// @param[in]  stUtcTime           ���ĸı�ʱ��
/// @param[out] stMuSendParam       ����ӿ� DeviceIoControl���ݲ����ṹ��
///
/// @return
///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::__FillGooseSendBuffer( unsigned char* pDataBuffer, unsigned int uiDataBufferLen, UTC_TIME_t& stUtcTime, IOCTRL_IN_PARAM_NET_BIG& stMuSendParam )
{
    unsigned char*          pDataBufferEnd          = NULL;
    uint8_t                 ucMacDes[ETHER_MAC_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t                 ucMacSrc[ETHER_MAC_LEN] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff};


    // ����������
    if (NULL == pDataBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): NULL parameter \n"));
        return ERROR_CODE_FAILURE;
    }

    // ��黺������С
    if (uiDataBufferLen <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): uiDataBufferLen <= 0 \n"));
        return ERROR_CODE_FAILURE;
    }
    pDataBufferEnd = pDataBuffer + uiDataBufferLen;                     ///< ָ�򻺳���ĩβ

    // ���GOOSE���Ͳ������ù�����
    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Get CParamConfigGOOSESend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // ����Ƿ��д����͵Ŀ��ƿ�
    if (m_vecGOOSESendCbs.size() <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): No control block to be transmitted \n"));
        return ERROR_CODE_FAILURE;
    }

    //
    // ����ÿһ�����ƿ飬�ϳɷ��ͱ���֡����ӵ����ͻ���
    //
    CSmvScaler                          SmvValueCaler;
    int                                 iCbIndex;                       ///< ���ƿ��������
    std::vector<CGooseCb*>::iterator    iter;
    for (iCbIndex = 0, iter = m_vecGOOSESendCbs.begin(); iter != m_vecGOOSESendCbs.end(); iCbIndex++, iter++)
    {
        CGooseCb* pGooseCb = (CGooseCb*)*iter;


        // ����û��Ƿ���ʱȡ���˷�������
        if (m_pDataSendManager && m_pDataSendManager->m_eventCancelGoose.Lock(0))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): User cancels the current operation \n"));
            return ERROR_CODE_CANCEL;
        }

        ///< ���ƿ�ͨ��������Ϊ0
        if ((NULL == pGooseCb) || (0 == pGooseCb->GetChannelCount()))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): No Channel \n"));
            return ERROR_CODE_FAILURE;
        }

        m_pGooseMsg->ClearGoosePDU();
        pGooseCb->GetDesMAC(ucMacDes, ETHER_MAC_LEN);                   ///< ���Ŀ��MAC��ַ
        pGooseCb->GetSrcMAC(ucMacSrc, ETHER_MAC_LEN);                   ///< ���ԴMAC��ַ
        m_pGooseMsg->SetMacAddr(ucMacDes, ucMacSrc, ETHER_MAC_LEN,pGooseCb->GetVlanFlag());     ///< �����շ�MAC��ַ
        m_pGooseMsg->SetAppID(pGooseCb->GetAPPID());                    ///< APPID
        m_pGooseMsg->Set8021QVlanInfo(pGooseCb->GetVLanPriority(), pGooseCb->GetVlanID(),pGooseCb->GetVlanFlag());

        // ���ƿ�����
        GOOSE_PDU_PUBLIC stPdbPublic;
        std::wstring strValue           = pGooseCb->GetGocbRef();
        stPdbPublic.gocbRef.assign(strValue.begin(), strValue.end());

        // ���ݼ�������
        strValue = pGooseCb->GetDataSet();
        stPdbPublic.datSet.assign(strValue.begin(), strValue.end());

        // ���ĵ�Ψһ�Ա�ʶ
        strValue = pGooseCb->GetGoID();
        stPdbPublic.goID.assign(strValue.begin(), strValue.end());

        // ���ļ���
        strValue = pGooseCb->GetSecurity();
        stPdbPublic.security.assign(strValue.begin(), strValue.end());

        // ����������Ϣ
        stPdbPublic.timeAllowedtoLive   = 2 * pParamConfigGooseSend->GetHeartbeatInterval();
        stPdbPublic.eventTm.ts_s        = stUtcTime.uSecondSinceEpoch;  ///< ʱ�������������
        stPdbPublic.eventTm.ts_ns       = stUtcTime.uFractionOfSecond * 1000;    ///< ʱ���������������
        stPdbPublic.eventTm.ts_q        = 0;                            ///< ʱ�����Ʒ��
        stPdbPublic.stNum               = pGooseCb->GetStNum();         ///< ״̬��ţ���λ������
        stPdbPublic.sqNum               = 0;                            ///< ˳���
        stPdbPublic.test                = (pGooseCb->GetTestFlag()? 1: 0);
        stPdbPublic.confRev             = pGooseCb->GetConfigRevision();///< ���ư汾��
        stPdbPublic.ndsCom              = (pGooseCb->GetNeedComm()? 1: 0);
        stPdbPublic.numDatSetEntries    = pGooseCb->GetChannelCount();  ///< ͨ����
        m_pGooseMsg->SetPduPublicInfo(stPdbPublic);

        // ��������ͨ��
        for (CChannel* pChannel = pGooseCb->FirstChannel(); pChannel != NULL; 
            pChannel = pGooseCb->NextChannel(pChannel))
        {
            if ((CHANNEL_TYPE_GOOSE_STRUCT == pChannel->GetChannelType()) &&
                (pChannel->GetSubChannelCount() > 0))                   ///< �ṹ������ͨ��������ͨ��
            {
                // �����ṹ������ͨ������
                GOOSE_STRUCT stGooseStruct;
                stGooseStruct.id.type   = GSEDT_STRUCT;
                stGooseStruct.member.clear();

                // ����������ͨ��
                int         iCount      = 0;
                CChannel*   pSubChannel = pChannel->FirstSubChannel();
                while(pSubChannel)
                {
                    // ��ṹ����������ͨ��ֵ����ָ��
                    stGooseStruct.member.push_back(&(pSubChannel->GetGooseSendValue().m_stUnKnown.id));

                    // ��һ����ͨ��
                    pSubChannel = pChannel->NextSubChannel(pSubChannel);
                    iCount++;
                }

                // ��֡���������һ������ͨ��
                if (!m_pGooseMsg->AddAChannel(&(stGooseStruct.id)))
                {
                    return ERROR_GOOSE_TOO_MANY_CHANNEL;
                }
            }
            else                                                        ///< �ǽṹ������ͨ��
            {
                // ���ͨ��ֵ
                GOOSE_DATA_VARIANT stChannelValue = pChannel->GetGooseSendValue();

                // ��֡���������һ������ͨ��
                if (!m_pGooseMsg->AddAChannel(&(stChannelValue.m_stUnKnown.id)))
                {
                    return ERROR_GOOSE_TOO_MANY_CHANNEL;
                }
            }
        }

        //
        // �������õĲ������б��룬��ñ���֡
        //
        int iByteCount = (int)(sizeof(m_pucFrameBuffer[0]) * m_pGooseMsg->Code(m_pucFrameBuffer, sizeof(m_pucFrameBuffer)));
        if (0 == iByteCount)                                            ///< ���֡��С
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): Code frame failed \n"));
            return ERROR_CODE_FAILURE;
        }

        // ��¼֡��
        if ((4 + iCbIndex) >= sizeof(stMuSendParam.nInBuf)/sizeof(stMuSendParam.nInBuf[0]))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): Record frame length failed \n"));
            return ERROR_CODE_FAILURE;
        }
        stMuSendParam.nInBuf[4 + iCbIndex] = iByteCount;

        //
        // ��������֡�����ͻ�����
        //
        if ((pDataBuffer + iByteCount) > pDataBufferEnd)                ///< �ж��Ƿ�Խ��
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): Oversize frame \n"));
            return ERROR_CODE_FAILURE;
        }
        memcpy(pDataBuffer, m_pucFrameBuffer, iByteCount);

        // ����������ָ�루4�ֽڶ��룩
        iByteCount      = ((iByteCount & 0x03) != 0)? ((iByteCount & 0xFFFC) + 4): iByteCount;
        pDataBuffer    += iByteCount;
    }

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    ��䱨�ķ��ͻ�������GOOSE��
///
/// @param[in]  pDataBuffer         �ݴ淢��֡�Ļ�����
/// @param[in]  uiDataBufferLen     ����֡�������Ĵ�С���ֽڣ�
/// @param[out] pNewSendBuffer      ָ����֡�����ָ�루���ڱ��淢��֡��Ϣ��
/// @param[out] uiFramesTotalLen    ���д�����֡�ĳ��Ⱥͣ��ֽڣ�
///
/// @return
///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::__FillGooseSendBuffer( unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CStateSendBuffer* pNewSendBuffer, uint32_t& uiFramesTotalLen )
{
    unsigned char*          pDataBufferEnd          = NULL;
    uint8_t                 ucMacDes[ETHER_MAC_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t                 ucMacSrc[ETHER_MAC_LEN] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff};


    // ����������
    if (NULL == pDataBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): NULL parameter \n"));
        return ERROR_CODE_FAILURE;
    }

    // ��黺������С
    if (uiDataBufferLen <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): uiDataBufferLen <= 0 \n"));
        return ERROR_CODE_FAILURE;
    }
    pDataBufferEnd = pDataBuffer + uiDataBufferLen;                     ///< ָ�򻺳���ĩβ

    // ���GOOSE���Ͳ������ù�����
    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Get CParamConfigGOOSESend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // ����Ƿ��д����͵Ŀ��ƿ�
    if (m_vecGOOSESendCbs.size() <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): No control block to be transmitted \n"));
        return ERROR_CODE_FAILURE;
    }

    //
    // ����ÿһ�����ƿ飬�ϳɷ��ͱ���֡����ӵ����ͻ���
    //
    uiFramesTotalLen = 0;
    std::vector<CGooseCb*>::iterator    iter = m_vecGOOSESendCbs.begin();
    for (int iCbIndex = 0; iter != m_vecGOOSESendCbs.end(); iCbIndex++, iter++)
    {
        CGooseCb* pGooseCb = (CGooseCb*)*iter;


        // ����û��Ƿ���ʱȡ���˷�������
        if (m_pDataSendManager && m_pDataSendManager->m_eventCancelGoose.Lock(0))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): User cancels the current operation \n"));
            return ERROR_CODE_CANCEL;
        }

        ///< ���ƿ�ͨ��������Ϊ0
        if ((NULL == pGooseCb) || (0 == pGooseCb->GetChannelCount()))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): No Channel \n"));
            return ERROR_CODE_FAILURE;
        }

        // ��λ��λ���
        pGooseCb->SetSendValueChanged(false);

        // ��λstNum
        pGooseCb->SetStNum(1);

        m_pGooseMsg->ClearGoosePDU();
        pGooseCb->GetDesMAC(ucMacDes, ETHER_MAC_LEN);                   ///< ���Ŀ��MAC��ַ
        pGooseCb->GetSrcMAC(ucMacSrc, ETHER_MAC_LEN);                   ///< ���ԴMAC��ַ
        m_pGooseMsg->SetMacAddr(ucMacDes, ucMacSrc, ETHER_MAC_LEN,pGooseCb->GetVlanFlag());     ///< �����շ�MAC��ַ
        m_pGooseMsg->SetAppID(pGooseCb->GetAPPID());                    ///< APPID
        m_pGooseMsg->Set8021QVlanInfo(pGooseCb->GetVLanPriority(), pGooseCb->GetVlanID(),pGooseCb->GetVlanFlag());

        // ���ƿ�����
        GOOSE_PDU_PUBLIC stPdbPublic;
        std::wstring strValue           = pGooseCb->GetGocbRef();
        stPdbPublic.gocbRef.assign(strValue.begin(), strValue.end());

        // ���ݼ�������
        strValue = pGooseCb->GetDataSet();
        stPdbPublic.datSet.assign(strValue.begin(), strValue.end());

        // ���ĵ�Ψһ�Ա�ʶ
        strValue = pGooseCb->GetGoID();
        stPdbPublic.goID.assign(strValue.begin(), strValue.end());

        // ���ļ���
        strValue = pGooseCb->GetSecurity();
        stPdbPublic.security.assign(strValue.begin(), strValue.end());

        // ����������Ϣ
        stPdbPublic.timeAllowedtoLive   = 2 * pParamConfigGooseSend->GetHeartbeatInterval();
        stPdbPublic.eventTm.ts_s        = 0;                            ///< ʱ�������������
        stPdbPublic.eventTm.ts_ns       = 0;                            ///< ʱ���������������
        stPdbPublic.eventTm.ts_q        = 0;                            ///< ʱ�����Ʒ��
        stPdbPublic.stNum               = pGooseCb->GetStNum();         ///< ״̬��ţ���λ������
        stPdbPublic.sqNum               = 0;                            ///< ˳���
        stPdbPublic.test                = (pGooseCb->GetTestFlag()? 1: 0);
        stPdbPublic.confRev             = pGooseCb->GetConfigRevision();///< ���ư汾��
        stPdbPublic.ndsCom              = (pGooseCb->GetNeedComm()? 1: 0);
        stPdbPublic.numDatSetEntries    = pGooseCb->GetChannelCount();  ///< ͨ����
        m_pGooseMsg->SetPduPublicInfo(stPdbPublic);

        // ��������ͨ��
        for (CChannel* pChannel = pGooseCb->FirstChannel(); pChannel != NULL; 
            pChannel = pGooseCb->NextChannel(pChannel))
        {
            if ((CHANNEL_TYPE_GOOSE_STRUCT == pChannel->GetChannelType()) &&
                (pChannel->GetSubChannelCount() > 0))                   ///< �ṹ������ͨ��������ͨ��
            {
                // �����ṹ������ͨ������
                GOOSE_STRUCT stGooseStruct;
                stGooseStruct.id.type   = GSEDT_STRUCT;
                stGooseStruct.member.clear();

                // ����������ͨ��
                int         iCount      = 0;
                CChannel*   pSubChannel = pChannel->FirstSubChannel();
                while(pSubChannel)
                {
                    // ��ṹ����������ͨ��ֵ����ָ��
                    stGooseStruct.member.push_back(&(pSubChannel->GetGooseSendValue().m_stUnKnown.id));

                    // ��λ��λ���
                    pSubChannel->SetSendValueChanged(false);

                    // ��һ����ͨ��
                    pSubChannel = pChannel->NextSubChannel(pSubChannel);
                    iCount++;
                }

                // ��֡���������һ������ͨ��
                if (!m_pGooseMsg->AddAChannel(&(stGooseStruct.id)))
                {
                    return ERROR_GOOSE_TOO_MANY_CHANNEL;
                }
            }
            else                                                        ///< �ǽṹ������ͨ��
            {
                // ���ͨ��ֵ
                GOOSE_DATA_VARIANT stChannelValue = pChannel->GetGooseSendValue();

                // ��֡���������һ������ͨ��
                if (!m_pGooseMsg->AddAChannel(&(stChannelValue.m_stUnKnown.id)))
                {
                    return ERROR_GOOSE_TOO_MANY_CHANNEL;
                }
            }

            // ��λ��λ���
            pChannel->SetSendValueChanged(false);
        }

        //
        // �������õĲ������б��룬��ñ���֡
        //
        int iByteCount = (int)(sizeof(m_pucFrameBuffer[0]) * m_pGooseMsg->Code(m_pucFrameBuffer, sizeof(m_pucFrameBuffer)));
        if (0 == iByteCount)                                            ///< ���֡��С
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): Code frame failed \n"));
            return ERROR_CODE_FAILURE;
        }

        // ��¼֡��
        if ((4 + iCbIndex) >= sizeof(pNewSendBuffer->m_stIOCtrlParam.nInBuf)/sizeof(pNewSendBuffer->m_stIOCtrlParam.nInBuf[0]))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): Record frame length failed \n"));
            return ERROR_CODE_FAILURE;
        }
        pNewSendBuffer->m_stIOCtrlParam.nInBuf[4 + iCbIndex] = iByteCount;

        //
        // ��������֡�����ͻ�����
        //
        if ((pDataBuffer + iByteCount) > pDataBufferEnd)                ///< �ж��Ƿ�Խ��
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): Oversize frame \n"));
            return ERROR_CODE_FAILURE;
        }
        memcpy(pDataBuffer, m_pucFrameBuffer, iByteCount);

        // ��¼����֡����Ϣ����Ӧ��GOOSE���ƿ顢֡ƫ�ơ�֡���ȣ�
        CFrameInfo stFrameInfo;
        stFrameInfo.m_pGooseCb      = pGooseCb;                         ///< ����֡��Ӧ��GOOSE���ƿ�
        stFrameInfo.m_uiFrameOffset = uiFramesTotalLen;                 ///< ֡ƫ��
        stFrameInfo.m_uiFrameLen    = iByteCount;                       ///< ֡����
        pNewSendBuffer->m_listFrameInfo.push_back(stFrameInfo);

        // ����������ָ�루4�ֽڶ��룩
        iByteCount      = ((iByteCount & 0x03) != 0)? ((iByteCount & 0xFFFC) + 4): iByteCount;
        pDataBuffer    += iByteCount;
        uiFramesTotalLen += iByteCount;
    }

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///     ����SMV����֡
///
/// @param[in]  stDspRecvData   ����ͨ��һ���������εĲ���ֵ
///
/// @return
///     bool    ���ͳɹ�����true
///
bool CFrameSendNet::UpdateSMVFrames( CDspRecvData& stDspRecvData )
{
    uint32_t                            uiFramesTotalLen;
    IOCTRL_IN_PARAM_NET_BIG             stMuSendParam;


    // ���û�д����͵Ŀ��ƿ飬��ôֱ�ӷ���
    if (m_vecSMVSendCbs.size() <= 0)
    {
        return true;
    }

    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): uninit \n"));
        return false;
    }

    // ���SMV���Ͳ������ù�����
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): Get CParamConfigSMVSend object failed \n"));
        return false;
    }

    // ���㻺����ƫ��
    unsigned int uiBufferOffset = 0;
    if (0 == m_pMapHeader->NetHeader.nCurSmvTxBufItem)                  ///< ��ǰSmv���ͻ����������
    {
        uiBufferOffset  = NET_SMV_SEND_LOOP_BUF1_POS;
    }
    else
    {
        uiBufferOffset  = NET_SMV_SEND_LOOP_BUF0_POS;
    }

    // �������ͻ�����
    if (FRAME_TYPE_NET_9_1 == pParamConfigSMVSend->GetSMVType())
    {
        // ��IEC61850-9-1���ƿ�������֡����䵽�ڴ�ӳ�仺����
        if (ERROR_CODE_SUCCESS != __FillIec91SendBuffer(m_pMapDataBuffer + uiBufferOffset, NET_SMV_SEND_BUF_LEN, stDspRecvData, stMuSendParam, uiFramesTotalLen))
        {
            return false;
        }
    }
    else if (FRAME_TYPE_NET_9_2 == pParamConfigSMVSend->GetSMVType())
    {
        // ��IEC61850-9-2���ƿ�������֡����䵽�ڴ�ӳ�仺����
        if (ERROR_CODE_SUCCESS != __FillIec92SendBuffer(m_pMapDataBuffer + uiBufferOffset, NET_SMV_SEND_BUF_LEN, stDspRecvData, stMuSendParam, uiFramesTotalLen))
        {
            return false;
        }
    }
    else
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): SMV type error \n"));
        return false;
    }

    // ����û��Ƿ���ʱȡ���˷�������
    if (m_pDataSendManager->m_eventCancelSMV.Lock(0))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): User cancels the current operation \n"));
        return false;
    }

    // ��ʼ��������
    IOCTRL_IN_PARAM_NET stIOCtrlInParam;
    stIOCtrlInParam.nDeviceItem   = m_enPortName;                       ///< �豸��
    stIOCtrlInParam.nInBuf[0]     = 0;                                  ///< SMV
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_CHANGE_MU_FRAME, &stIOCtrlInParam, sizeof(stIOCtrlInParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): Port%d IOCTRL_CHANGE_MU_FRAME failed \n", m_enPortName));
        return false;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): Port%d IOCTRL_CHANGE_MU_FRAME successfully \n", m_enPortName));
    return true;
}

///
/// @brief
///    ����SMV�������е�����֡
///
/// @param[in]  pStateSendBuffer    ָ�򻺳�����ָ��
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSendNet::UpdateSMVFrames( CStateSendBuffer* pStateSendBuffer )
{
    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): uninit \n"));
        return false;
    }

    // ����������
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): Parameter error \n"));
        return false;
    }

    // ���㻺����ƫ��
    unsigned int uiBufferOffset = 0;
    if (0 == m_pMapHeader->NetHeader.nCurSmvTxBufItem)                  ///< ��ǰSmv���ͻ����������
    {
        uiBufferOffset  = NET_SMV_SEND_LOOP_BUF1_POS;
    }
    else
    {
        uiBufferOffset  = NET_SMV_SEND_LOOP_BUF0_POS;
    }

    // �������ͻ�����
    if (0 != memcpy_s(m_pMapDataBuffer + uiBufferOffset, NET_SMV_SEND_BUF_LEN, pStateSendBuffer->m_pucDataBuffer, pStateSendBuffer->m_uiDataBufferLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): Fill send buffer failed"));
        return false;
    }

    // ��ʼ��������
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_CHANGE_MU_FRAME, &pStateSendBuffer->m_stIOCtrlParam, sizeof(pStateSendBuffer->m_stIOCtrlParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): Port%d IOCTRL_CHANGE_MU_FRAME failed \n", m_enPortName));
        return false;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): Port%d IOCTRL_CHANGE_MU_FRAME successfully \n", m_enPortName));
    return true;
}

///
/// @brief
///     ����GOOSE����֡
///
/// @param[in]  stDspRecvData   ����ͨ��һ���������εĲ���ֵ
///
/// @return
///     bool    ���ͳɹ�����true
///
bool CFrameSendNet::UpdateGooseFrames()
{
    IOCTRL_IN_PARAM_NET_BIG             stMuSendParam;


    // ���û�д����͵Ŀ��ƿ飬��ôֱ�ӷ���
    if (m_vecGOOSESendCbs.size() <= 0)
    {
        return true;
    }

    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateGooseFrames(): uninit \n"));
        return false;
    }

    // ���UTCʱ��
    UTC_TIME_t stUtcTime;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_GET_UTC_TIME, NULL, NULL, &stUtcTime, sizeof(stUtcTime), NULL ,NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Port%d IOCTRL_GET_UTC_TIME failed \n", m_enPortName));
        return false;
    }

    //
    // ���ñ�λ���
    //
    int                                 iCbIndex;                       ///< ���ƿ��������
    std::vector<CGooseCb*>::iterator    iter;
    IOCTRL_IN_PARAM_NET                 stIOCtrlInParam;
    stIOCtrlInParam.nDeviceItem         = m_enPortName;                 ///< �豸��
    stIOCtrlInParam.nInBuf[0]           = 1;                            ///< GOOSE
    stIOCtrlInParam.nInBuf[1]           = 0;                            ///< GOOSE��λ���
    for (iCbIndex = 0, iter = m_vecGOOSESendCbs.begin(); iter != m_vecGOOSESendCbs.end(); iCbIndex++, iter++)
    {
        CGooseCb* pGooseCb = (CGooseCb*)*iter;
        if (pGooseCb && pGooseCb->IsSendValueChanged())                ///< GOOSEͨ��ֵ�����˸ı�
        {
            // ��һ��GOOSE���ƿ鷢����λ�ˣ���Ҫ����Ӧ����λ��1
            stIOCtrlInParam.nInBuf[1] |= (1 << iCbIndex);
        }
    }

    // ���㻺����ƫ��
    unsigned int uiBufferOffset = 0;
    if (0 == m_pMapHeader->NetHeader.nCurGooseTxBufItem)                ///< ��ǰGOOSE���ͻ����������
    {
        uiBufferOffset = GOOSE_SEND_LOOP_BUF1_POS;
    }
    else
    {
        uiBufferOffset = GOOSE_SEND_LOOP_BUF0_POS;
    }

    // �������ͻ�����
    if (ERROR_CODE_SUCCESS != __FillGooseSendBuffer(m_pMapDataBuffer + uiBufferOffset, GOOSE_SEND_BUF_LEN, stUtcTime, stMuSendParam))
    {
        return false;
    }

    // ����û��Ƿ���ʱȡ���˷�������
    if (m_pDataSendManager->m_eventCancelGoose.Lock(0))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateGooseFrames(): User cancels the current operation \n"));
        return false;
    }

    // ��ʼ��������
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_CHANGE_MU_FRAME, &stIOCtrlInParam, sizeof(stIOCtrlInParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateGooseFrames(): Port%d IOCTRL_CHANGE_MU_FRAME failed \n", m_enPortName));
        return false;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateGooseFrames(): Port%d IOCTRL_CHANGE_MU_FRAME successfully \n", m_enPortName));
    return true;
}

///
/// @brief
///     ����GOOSE����֡
///
/// @param[in]  pStateSendBuffer    ָ�򻺳�����ָ��
///
/// @return
///     bool    ���ͳɹ�����true
///
bool CFrameSendNet::UpdateGooseFrames( CStateSendBuffer* pStateSendBuffer )
{
    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateGooseFrames(): uninit \n"));
        return false;
    }

    // ����������
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateGooseFrames(): parameter error \n"));
        return false;
    }

    // ���㻺����ƫ��
    unsigned int uiBufferOffset = 0;
    if (0 == m_pMapHeader->NetHeader.nCurGooseTxBufItem)                ///< ��ǰGOOSE���ͻ����������
    {
        uiBufferOffset = GOOSE_SEND_LOOP_BUF1_POS;
    }
    else
    {
        uiBufferOffset = GOOSE_SEND_LOOP_BUF0_POS;
    }

    // �������ͻ�����
    if (0 != memcpy_s(m_pMapDataBuffer + uiBufferOffset, GOOSE_SEND_BUF_LEN, pStateSendBuffer->m_pucDataBuffer, pStateSendBuffer->m_uiDataBufferLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): Fill send buffer failed"));
        return false;
    }

    // ��ʼ��������
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_CHANGE_MU_FRAME, &pStateSendBuffer->m_stIOCtrlParam, sizeof(pStateSendBuffer->m_stIOCtrlParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateGooseFrames(): Port%d IOCTRL_CHANGE_MU_FRAME failed \n", m_enPortName));
        return false;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateGooseFrames(): Port%d IOCTRL_CHANGE_MU_FRAME successfully \n", m_enPortName));
    return true;
}

///
/// @brief
///    ������ƿ�������ͨ����ƫ����
///
/// @param[in]  pStateSendBuffer    ָ����֡��������ָ��
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSendNet::CalcChannelOffset( CStateSendBuffer* pStateSendBuffer )
{
    unsigned char* pucFrame = NULL;


    // ����������
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__CalcChannelOffset(): Parameter error \n"));
        return false;
    }

    // �������п��ƿ�
    std::list<CFrameInfo>::iterator iter = pStateSendBuffer->m_listFrameInfo.begin();
    for (; iter != pStateSendBuffer->m_listFrameInfo.end(); iter++)
    {
        CFrameInfo& stFrameInfo = *iter;
        if (stFrameInfo.m_pGooseCb)
        {
            // ����GOOSE���ƿ��Ӧ�ķ���֡
            pucFrame = pStateSendBuffer->m_pucDataBuffer + stFrameInfo.m_uiFrameOffset;
            if (0 == m_pGooseMsg->Decode(pucFrame, stFrameInfo.m_uiFrameLen))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__CalcChannelOffset(): decode frame failed \n"));
                return false;
            }

            // ������ƿ�������ͨ����ƫ����
            if (!stFrameInfo.m_pGooseCb->CalcChannelOffset(m_pGooseMsg))
            {
                return false;
            }
        }
    }

    return true;
}
