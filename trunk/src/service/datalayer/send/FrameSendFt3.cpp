/// @file
///
/// @brief
///     Ft3���ݷ�����
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
#include "FrameSendFt3.h"
#include "FrameSend.h"
#include "src\utils\SmvScaler\SmvScaler.h"
#include "src\service\datalayer\config\ParamConfigBasic.h"
#include "src\service\datalayer\config\ParamConfigSMVSend.h"
#include "src\service\datalayer\config\ParamconfigManualTest.h"

///
/// @brief
///    ���캯��
///
/// @param[in]  pFrameSend    ���ݷ��͹�����
/// @param[in]  hGPMDevice    GPMC���߾��
/// @param[in]  enPortName    Ft3ʵ����Ӧ��ʵ�ʶ˿�
///
CFrameSendFt3::CFrameSendFt3(CFrameSend* pFrameSend, HANDLE hGPMDevice, DsmPortName enPortName)
: m_pDataSendManager(pFrameSend)
, m_hGPMDevice(hGPMDevice)
, m_enPortName(enPortName)
, m_bIsInited(false)
, m_hFileMaping(NULL)
, m_pMapHeader(NULL)
, m_pMapDataBuffer(NULL)
, m_pNanRuiFT3Msg(NULL)
, m_pGuoWangFT3Msg(NULL)
, m_pSMVSendCb(NULL)
{

}

///
/// @brief
///    ��������
///
CFrameSendFt3::~CFrameSendFt3(void)
{
    UnInitFt3();
}

///
/// @brief
///    ��ʼ��Ft3����,�򿪹����ڴ�
///
/// @return
///     true�����ʼ���ɹ�
///
bool CFrameSendFt3::InitFt3( const wchar_t *pNotifyEvtName, const wchar_t *pMapFileName, uint32_t nMapFileLen )
{
    // �ж��Ƿ��Ѿ���ʼ��
    if (m_bIsInited)
    {
        return true;
    }

    // ���GPM�������
    if (INVALID_HANDLE_VALUE == m_hGPMDevice)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecvFt3::InitFt3(): m_hGPMDevice INVALID_HANDLE_VALUE \n")));
        return false;
    }

    // ����ͨ�������ڴ�ӳ���ļ�
    m_hFileMaping = CreateFileMapping(INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE|SEC_COMMIT,
        0,
        nMapFileLen,
        pMapFileName);
    if (NULL == m_hFileMaping)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecvFt3::InitFt3(): Open map file failed On Port %d\r\n"), m_enPortName));
        goto exitCode;
    }

    // ���ӳ���ļ����ڴ�ָ��
    m_pMapHeader = (Map_File_Header*)MapViewOfFile(m_hFileMaping, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
    if (NULL == m_pMapHeader)
    {
        RETAILMSG(OUTPUT_LOG_MSG,(_T("CFrameRecvFt3::InitFt3(): FileMapping for Port%d map failed"), m_enPortName));
        goto exitCode;
    }
    m_pMapDataBuffer    = (unsigned char*)m_pMapHeader;
    m_pMapDataBuffer   += sizeof(Map_File_Header);

    // ��������FT3֡������
    m_pNanRuiFT3Msg = CreateNanRuiFT3MsgImpl();
    if (NULL == m_pNanRuiFT3Msg)
    {
        RETAILMSG(OUTPUT_LOG_MSG,(_T("CFrameRecvFt3::InitFt3(): Create NanRui FT3 frame manager falied \n")));
        goto exitCode;
    }

    // ��������FT3֡������
    m_pGuoWangFT3Msg = CreateGuoWangFT3MsgImpl();
    if (NULL == m_pGuoWangFT3Msg)
    {
        RETAILMSG(OUTPUT_LOG_MSG,(_T("CFrameRecvFt3::InitFt3(): Create GuoWang FT3 frame manager falied \n")));
        goto exitCode;
    }

    // ���Ϊ�ѳɹ���ʼ��
    m_bIsInited = true;
    return true;

exitCode:
    UnInitFt3();
    return false;
}

///
/// @brief
///    ����ʼ��FT3����,�ر��ڴ�ӳ��
///
void CFrameSendFt3::UnInitFt3()
{
    // ֹͣ�ײ����ݷ���
    StopFt3Send();

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

    // �ͷ�����FT3֡������
    if (NULL != m_pNanRuiFT3Msg)
    {
        ReleaseNanRuiFT3MsgImpl(m_pNanRuiFT3Msg);
        m_pNanRuiFT3Msg = NULL;
    }

    // �ͷŹ���FT3֡������
    if (NULL != m_pGuoWangFT3Msg)
    {
        ReleaseGuoWangFT3MsgImpl(m_pGuoWangFT3Msg);
        m_pGuoWangFT3Msg = NULL;
    }
}

///
/// @brief
///    Ԥ�ȼ���˿ڵ�SMV����֡
///
/// @param[in]  stDspRecvData       ����ͨ��һ���������εĲ���ֵ
/// @param[out] pStateTestResult    �������������֡�Ľṹ��
///
/// @return
///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendFt3::PrepareSmvSendBuffer( CDspRecvData& stDspRecvData, CStateTestResult* pStateTestResult )
{
    FT3_TX_CFG                          stTxCfg;
    uint32_t                            uiFramesTotalLen;


    // ����������
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__PrepareSmvSendBuffer(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���û�д����͵Ŀ��ƿ飬��ôֱ�ӷ���
    if (NULL == m_pSMVSendCb)
    {
        return ERROR_CODE_SUCCESS;
    }

    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__PrepareSmvSendBuffer(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���Ft3��������
    memset(&stTxCfg, 0, sizeof(FT3_TX_CFG));
    if (!__GetFt3SendCfg(stTxCfg))
    {
        return ERROR_CODE_FAILURE;
    }

    // ��Ӵ����ͻ���
    CStateSendBuffer* pNewSendBuffer = new CStateSendBuffer(m_enPortName);
    if (NULL == pNewSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__PrepareSmvSendBuffer(): new CStateSendData failed \n"));
        return ERROR_CODE_FAILURE;
    }
    pNewSendBuffer->m_stIOCtrlParam.nDeviceItem  = m_enPortName;        ///< Ft3ʵ����Ӧ��ʵ�ʶ˿�
    memcpy_s(pNewSendBuffer->m_stIOCtrlParam.nInBuf, sizeof(pNewSendBuffer->m_stIOCtrlParam.nInBuf), &stTxCfg, sizeof(stTxCfg));
    pStateTestResult->m_listSmvSendBuffers.push_back(pNewSendBuffer);

    // �������ͻ�����
    enumErrorCode eRet = __FillFt3SendBuffer(m_pMapDataBuffer + FT3_SEND_LOOP_BUF0_POS, FT3_SEND_LOOP_BUF0_LEN, stDspRecvData, uiFramesTotalLen);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        return eRet;
    }

    // ���뻺��
    if (NULL == pNewSendBuffer->AllocDataBuffer(uiFramesTotalLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__PrepareSmvSendBuffer(): Alloc Data Buffer failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // �������з���֡
    if (0 != memcpy_s(pNewSendBuffer->m_pucDataBuffer, uiFramesTotalLen, m_pMapDataBuffer + FT3_SEND_LOOP_BUF0_POS, uiFramesTotalLen))
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
///     enumErrorCode   ���ͳɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendFt3::SendSMVCbs(CDspRecvData& stDspRecvData)
{
    FT3_TX_CFG                          stTxCfg;
    IOCTRL_IN_PARAM_NET_BIG             stFt3InParam;
    uint32_t                            uiFramesTotalLen;
    enumErrorCode                       eRet;


    // ���û�д����͵Ŀ��ƿ飬��ôֱ�ӷ���
    if (NULL == m_pSMVSendCb)
    {
        return ERROR_CODE_SUCCESS;
    }

    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���Ft3��������
    memset(&stTxCfg, 0, sizeof(FT3_TX_CFG));
    if (!__GetFt3SendCfg(stTxCfg))
    {
        return ERROR_CODE_FAILURE;
    }

    // ���IOCTRL SEND HEADER
    memset(&stFt3InParam, 0, sizeof(stFt3InParam));
    stFt3InParam.nDeviceItem = m_enPortName;                            ///< Ft3ʵ����Ӧ��ʵ�ʶ˿�
    memcpy_s(stFt3InParam.nInBuf, sizeof(stFt3InParam.nInBuf), &stTxCfg, sizeof(FT3_TX_CFG));

    // ����IOCTRL SEND HEADER������Ϊ�ֶ�����ģʽ��
    stFt3InParam.nInBuf[TX_TEST_STYLE_INDEX] = TX_TEST_STYLE_MANUAL_TEST;
    if (FALSE == DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_HEADER, &stFt3InParam, sizeof(stFt3InParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): Port%d IOCTRL_SEND_HEADER failed \n", m_enPortName));
        return ERROR_CODE_FAILURE;
    }

    // �������ͻ�����
    m_pMapHeader->Ft3Header.nCurTxBufItem = 0;
    eRet = __FillFt3SendBuffer(m_pMapDataBuffer + FT3_SEND_LOOP_BUF0_POS, FT3_SEND_LOOP_BUF0_LEN, stDspRecvData, uiFramesTotalLen);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        return eRet;
    }

    // ����û��Ƿ���ʱȡ���˷�������
    if (m_pDataSendManager->m_eventCancelSMV.Lock(0))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): User cancels the current operation \n"));
        return ERROR_CODE_CANCEL;
    }

    // ��ʼ��������
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_FRAME, &stFt3InParam, sizeof(stFt3InParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): Port%d IOCTRL_SEND_FRAME failed \n", m_enPortName));
        return ERROR_CODE_FAILURE;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): Port%d IOCTRL_SEND_FRAME successfully \n", m_enPortName));
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
bool CFrameSendFt3::SendSMVCbs( CStateSendBuffer* pStateSendBuffer )
{
    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): uninit \n"));
        return false;
    }

    // ����������
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): Parameter error \n"));
        return false;
    }

    // �������ͻ�����
    m_pMapHeader->Ft3Header.nCurTxBufItem = 0;
    if (0 != memcpy_s(m_pMapDataBuffer + FT3_SEND_LOOP_BUF0_POS, FT3_SEND_LOOP_BUF0_LEN, pStateSendBuffer->m_pucDataBuffer, pStateSendBuffer->m_uiDataBufferLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): fill send buffer failed \n"));
        return false;
    }

    // ����IOCTRL SEND HEADER������Ϊ״̬����ģʽ��
    pStateSendBuffer->m_stIOCtrlParam.nInBuf[TX_TEST_STYLE_INDEX] = TX_TEST_STYLE_STATUS_SEQUENCE;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_HEADER, &pStateSendBuffer->m_stIOCtrlParam, sizeof(pStateSendBuffer->m_stIOCtrlParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): Port%d IOCTRL_SEND_HEADER failed \n", m_enPortName));
        return false;
    }

    // ��ʼ��������
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_FRAME, &pStateSendBuffer->m_stIOCtrlParam, sizeof(pStateSendBuffer->m_stIOCtrlParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): Port%d IOCTRL_SEND_FRAME failed \n", m_enPortName));
        return false;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): Port%d IOCTRL_SEND_FRAME successfully \n", m_enPortName));
    return true;
}

///
/// @brief
///    ֹͣFT3���ݷ���
///
bool CFrameSendFt3::StopFt3Send()
{
    IOCTRL_IN_PARAM_FT3 stIoCtrolFt3TxCfg;


    // ֹͣFT3���ݷ���
    stIoCtrolFt3TxCfg.nDeviceItem = m_enPortName ;
    if (FALSE == DeviceIoControl(m_hGPMDevice, IOCTRL_STOP_SEND, &stIoCtrolFt3TxCfg, sizeof(IOCTRL_IN_PARAM_FT3), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::StopFt3Send(): Port%d IOCTRL_STOP_SEND failed \n", m_enPortName));
        return false;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::StopFt3Send(): Port%d IOCTRL_STOP_SEND successfully \n", m_enPortName));
    return true;
}

///
/// @brief
///    ���ô����͵�SMV���ƿ�
///
/// @param[in]  pNewCb    SMV���ƿ�
///
void CFrameSendFt3::SetSMVSendCb( CFT3Cb* pNewCb )
{
    m_pSMVSendCb = pNewCb;
}

///
/// @brief
///    ���Ft3��������
///
/// @param[out]  stTxCfg    ����������Ϣ�Ľṹ��
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSendFt3::__GetFt3SendCfg( FT3_TX_CFG& stTxCfg )
{
    Ft3CtlWordSend                      stCtrlWordSend;


    // ���SMV���Ͳ������ù�����
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__GetFt3TxCfg(): Get CParamConfigSMVSend object failed \n"));
        return false;
    }

    // ������
    memset(&stCtrlWordSend, 0, sizeof(Ft3CtlWordSend));
    switch(pParamConfigSMVSend->GetBaudRate())
    {
    case FT3_BAUD_RATE_2P5M:
    case FT3_BAUD_RATE_5M:
    case FT3_BAUD_RATE_10M:
        {
            stCtrlWordSend.CtrlBit.bBaudRate    = pParamConfigSMVSend->GetBaudRate();
        }
        break;

    default:
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__GetFt3TxCfg(): Unknown baudrate \n"));
            return false;
        }
    }

    // ����Ƶ��
    switch(pParamConfigSMVSend->GetFrequency())
    {
    case FT3_SAMP_RATE_4K:
    case FT3_SAMP_RATE_5K:
    case FT3_SAMP_RATE_8K:
    case FT3_SAMP_RATE_10K:
    case FT3_SAMP_RATE_12P8K:
        {
            stCtrlWordSend.CtrlBit.bSampleRate  = pParamConfigSMVSend->GetFrequency();
        }
        break;

    default:
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__GetFt3TxCfg(): Unknown frequency \n"));
            return false;
        }
    }

    // ���ĸ�ʽ
    switch(pParamConfigSMVSend->GetSMVType())
    {
    case FRAME_TYPE_FT3_NANRUI:                                         ///< ��׼FT3
        {
            stCtrlWordSend.CtrlBit.bFrameType   = Ft3CtlWordSend::FT3_FRAME_TYPE_NANRUI;
        }
        break;

    case FRAME_TYPE_FT3_GUOWANG:                                        ///< ��չFT3
        {
            stCtrlWordSend.CtrlBit.bFrameType   = Ft3CtlWordSend::FT3_FRAME_TYPE_GUOWANG;
        }
        break;

    default:
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__GetFt3TxCfg(): Unknown SMV type \n"));
            return false;
        }
    }

    //
    // ����FT3_TX_CFG�ṹ��
    //
    stTxCfg.uFramesStyle  = (uint16_t)stCtrlWordSend.nData;             ///< ֡��ʽ
    stTxCfg.nFrameCount   = 20;                                         ///< һ����FPGA�д���20֡����
    if (Ft3CtlWordSend::FT3_FRAME_TYPE_NANRUI == stCtrlWordSend.CtrlBit.bFrameType)
    {
        stTxCfg.nFrameLength    = stTxCfg.nFrameCount * (16 * 3);       ///< �����ܳ���
    }
    else if (Ft3CtlWordSend::FT3_FRAME_TYPE_GUOWANG == stCtrlWordSend.CtrlBit.bFrameType)
    {
        stTxCfg.nFrameLength    = stTxCfg.nFrameCount * (16 * 4);       ///< �����ܳ���
    }

    return true;
}

///
/// @brief
///    ���FT3���ķ��ͻ�����
///
/// @param[out]  pDataBuffer        �����ͻ�����
/// @param[in]   uiDataBufferLen    ���ͻ�������С���ֽڣ�
/// @param[in]   stDspRecvData      ����ͨ��һ���������εĲ���ֵ
/// @param[out]  uiFramesTotalLen   ���д�����֡�ĳ��Ⱥ�
///
/// @return
///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendFt3::__FillFt3SendBuffer( unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CDspRecvData& stDspRecvData, uint32_t& uiFramesTotalLen )
{
    unsigned char*          pDataBufferEnd  = NULL;


    // ����������
    if (NULL == pDataBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): NULL parameter \n"));
        return ERROR_CODE_FAILURE;
    }

    // ��黺������С
    if (uiDataBufferLen <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): uiDataBufferLen <= 0 \n"));
        return ERROR_CODE_FAILURE;
    }
    pDataBufferEnd = pDataBuffer + uiDataBufferLen;                     ///< ָ�򻺳���ĩβ

    // ����Ƿ��д����͵Ŀ��ƿ�
    if (NULL == m_pSMVSendCb)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): SMV send control block is NULL \n"));
        return ERROR_CODE_FAILURE;
    }

    // ��û����������ù�����
    CParamConfigBasic* pParamConfigBasic = CParamConfigBasic::getInstance();
    if (NULL == pParamConfigBasic)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillFt3SendBuffer(): Get CParamConfigBasic object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���SMV���Ͳ������ù�����
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): Get CParamConfigSMVSend object failed \n"));
        return ERROR_CODE_FAILURE;
    }
    enumFrameType eFrameType = pParamConfigSMVSend->GetSMVType();

    // �����ƿ�ͨ����
    if (m_pSMVSendCb->GetChannelCount() <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): Control block has no Channel \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���ͨ�����Ƿ���
    if (m_pSMVSendCb->GetChannelCount() > 22)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): Excessive number of channels \n"));
        return ERROR_CODE_FAILURE;
    }

    CSmvScaler  SmvValueCaler;
    // ���ñ���ֵ����
    SmvValueCaler.SetChannelMsgValType(pParamConfigBasic->GetFrameValueType());
    // ����ͨ������Ȼֵ����
    SmvValueCaler.SetChannelNtrValType(pParamConfigSMVSend->GetSVDisplay());

    //
    // ����ÿһ�����ֵ���ϳɷ��ͱ���֡����ӵ����ͻ���
    //
    uiFramesTotalLen = 0;
    for (int iFrameIndex = 0; iFrameIndex < stDspRecvData.m_iSmvCount; iFrameIndex++)
    {
        // ����û��Ƿ���ʱȡ���˷�������
        if (m_pDataSendManager && m_pDataSendManager->m_eventCancelSMV.Lock(0))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): User cancels the current operation \n"));
            return ERROR_CODE_CANCEL;
        }

        // ���÷��ͱ���֡��ÿ��ͨ����ֵ
        int     iChannelCount       = 0;                        ///< ͨ������
        double  dSmvValue           = 0.0f;                     ///< ����ֵ
        double  dOnceRatings        = 0.0f;                     ///< һ�ζֵ
        double  dSecondaryRatings   = 0.0f;                     ///< ���ζֵ
        for (CChannel* pChannel = m_pSMVSendCb->FirstChannel(); pChannel != NULL; 
            iChannelCount++, pChannel = m_pSMVSendCb->NextChannel(pChannel))
        {
            dSmvValue           = 0.0f;                             ///< ����ֵ
            dOnceRatings        = pChannel->GetOnceRatings();       ///< һ�ζֵ
            dSecondaryRatings   = pChannel->GetSecondaryRatings();  ///< ���ζֵ

            //
            // �����ͨ�����������ⷢ��ͨ������ô����˲ʱ����ֵ
            //
            std::wstring strChannelName = pChannel->GetVirtualChannel();
            if (strChannelName.size())
            {
                std::map<std::wstring, int>::iterator iter = stDspRecvData.m_mapChannelName2Index.find(strChannelName);
                if (stDspRecvData.m_mapChannelName2Index.end() != iter)
                {
                    // ����˲ʱ����ֵ
                    dSmvValue = stDspRecvData.m_stRecvData.fSmvGenData[iter->second][iFrameIndex];
                }
            }

            // ����ͨ�����ͣ����÷���֡�е�ͨ��ֵ
            switch (pChannel->GetChannelType())
            {
            case CHANNEL_TYPE_UNKOWN:                                   ///< δ֪����
                {
                    m_usChannelValArr[iChannelCount] = 0;
                }
                break;

            case CHANNEL_TYPE_SMV_CURRENT:                              ///< ��������
                {
                    // ����˲ʱ����ֵ�����÷���֡��ͨ��ֵ
                    m_usChannelValArr[iChannelCount] = SmvValueCaler.ChannelNtrVal2MsgVal_FT3(
                        (float)dSmvValue,
                        (float)dOnceRatings,
                        (float)dSecondaryRatings,
                        CHANNEL_TYPE_SMV_CURRENT,
                        (pChannel->IsMeasure() ? CHANNEL_VALUE_TYPE_MEASURE : CHANNEL_VALUE_TYPE_PROTECT)
                        );
                }
                break;

            case CHANNEL_TYPE_SMV_VOLTAGE:                              ///< ��ѹ����
                {
                    // ����˲ʱ����ֵ�����÷���֡��ͨ��ֵ
                    m_usChannelValArr[iChannelCount] = SmvValueCaler.ChannelNtrVal2MsgVal_FT3(
                        (float)dSmvValue,
                        (float)dOnceRatings,
                        (float)dSecondaryRatings,
                        CHANNEL_TYPE_SMV_VOLTAGE,
                        CHANNEL_VALUE_TYPE_UNKNOWN
                        );
                }
                break;

            case CHANNEL_TYPE_SMV_TIME:                                 ///< ʱ������
                {
                    m_usChannelValArr[iChannelCount] = (uint16_t)dSmvValue;
                }
                break;

            default:
                {
                    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): Channel Type error \n"));
                    return ERROR_CODE_FAILURE;
                }
            }
        }

        //
        // ����Э�����ͺϳɱ���֡
        //
        uint16_t*   pusFrameBuffer  = NULL;
        int         iFrameBufferSize;
        int         iByteCount      = 0;
        if (FRAME_TYPE_FT3_NANRUI   == eFrameType)                        ///< ����
        {
            pusFrameBuffer          = m_usNanRuiBuffer;                 ///< ��������
            iFrameBufferSize        = NANRUI_FT3_MSG_WORD;              ///< ���������С

            // ����֡ͷ���ֶε�ֵ
            NANRUI_FT3_ASDU_PUBLIC stAsduPublic = {0};
            stAsduPublic.asduLen    = (uint16_t)((iChannelCount + 10) * sizeof(m_usChannelValArr[0]));
            stAsduPublic.lnName     = m_pSMVSendCb->GetLNName();
            stAsduPublic.dataSetName= m_pSMVSendCb->GetDSName();
            stAsduPublic.ldName     = m_pSMVSendCb->GetLDName();
            stAsduPublic.phsArtg    = (uint16_t)(m_pSMVSendCb->GetRatedPhaseCurrent());
            stAsduPublic.neutArtg   = (uint16_t)(m_pSMVSendCb->GetRatedNeutralCurrent());
            stAsduPublic.phsVrtg    = SmvValueCaler.PhsVrtgNtrVal2MsgVal_FT3((float)m_pSMVSendCb->GetRatedPhaseVoltage());
            stAsduPublic.delayTime  = m_pSMVSendCb->GetRatedDelay();
            stAsduPublic.reserved   = 0;
            stAsduPublic.statusWord1= m_pSMVSendCb->GetStatusWord1();
            stAsduPublic.statusWord2= m_pSMVSendCb->GetStatusWord2();
            stAsduPublic.smpCnt     = iFrameIndex;
            stAsduPublic.smpRate    = (uint8_t)(stDspRecvData.m_iSmvCount);
            stAsduPublic.confRev    = 1;

            // ����֡ͷ��ͨ��ֵ����
            m_pNanRuiFT3Msg->SetASDU(stAsduPublic, m_usChannelValArr, iChannelCount);

            // �ϳɷ���֡
            iByteCount = (int)(sizeof(pusFrameBuffer[0]) * m_pNanRuiFT3Msg->Code(pusFrameBuffer, iFrameBufferSize));
        }
        else if(FRAME_TYPE_FT3_GUOWANG == eFrameType)                   ///< ����
        {
            pusFrameBuffer          = m_usGuoWangBuffer;                ///< ��������
            iFrameBufferSize        = GUOWANG_FT3_MSG_WORD;             ///< ���������С

            // ����֡ͷ���ֶε�ֵ
            GUOWANG_FT3_ASDU_PUBLIC stAsduPublic = {0};
            stAsduPublic.asduLen    = (uint16_t)((iChannelCount + 10) * sizeof(m_usChannelValArr[0]));
            stAsduPublic.lnName     = m_pSMVSendCb->GetLNName();
            stAsduPublic.dataSetName= m_pSMVSendCb->GetDSName();
            stAsduPublic.ldName     = m_pSMVSendCb->GetLDName();
            stAsduPublic.phsArtg    = (uint16_t)(m_pSMVSendCb->GetRatedPhaseCurrent());
            stAsduPublic.neutArtg   = (uint16_t)(m_pSMVSendCb->GetRatedNeutralCurrent());
            stAsduPublic.phsVrtg    = SmvValueCaler.PhsVrtgNtrVal2MsgVal_FT3((float)m_pSMVSendCb->GetRatedPhaseVoltage());
            stAsduPublic.delayTime  = m_pSMVSendCb->GetRatedDelay();
            stAsduPublic.statusWord1= m_pSMVSendCb->GetStatusWord1();
            stAsduPublic.statusWord2= m_pSMVSendCb->GetStatusWord2();
            stAsduPublic.smpCnt     = iFrameIndex;

            // ����֡ͷ��ͨ��ֵ����
            m_pGuoWangFT3Msg->SetASDU(stAsduPublic, m_usChannelValArr, iChannelCount);

            // �ϳɷ���֡
            iByteCount = (int)(sizeof(pusFrameBuffer[0]) * m_pGuoWangFT3Msg->Code(pusFrameBuffer, iFrameBufferSize));
        }

        // ���֡��С
        if (0 == iByteCount)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): Code frame failed \n"));
            return ERROR_CODE_FAILURE;
        }

        // ��������֡�����ͻ�����
        if ((pDataBuffer + iByteCount) > pDataBufferEnd)                ///< �ж��Ƿ�Խ��
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): Oversize frame \n"));
            return ERROR_CODE_FAILURE;
        }
        memcpy(pDataBuffer, pusFrameBuffer, iByteCount);

        // ����������ָ��
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
bool CFrameSendFt3::UpdateSMVFrames( CDspRecvData& stDspRecvData )
{
    uint32_t                            uiFramesTotalLen;


    // ���û�д����͵Ŀ��ƿ飬��ôֱ�ӷ���
    if (NULL == m_pSMVSendCb)
    {
        return true;
    }

    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): uninit \n"));
        return false;
    }

    // ���㻺����ƫ��
    unsigned int uiBufferOffset = 0;
    if (0 == m_pMapHeader->Ft3Header.nCurTxBufItem)
    {
        uiBufferOffset  = FT3_SEND_LOOP_BUF1_POS;
    }
    else
    {
        uiBufferOffset  = FT3_SEND_LOOP_BUF0_POS;
    }

    // �������ͻ�����
    if (ERROR_CODE_SUCCESS != __FillFt3SendBuffer(m_pMapDataBuffer + uiBufferOffset, FT3_SEND_LOOP_BUF0_LEN, stDspRecvData, uiFramesTotalLen))
    {
        return false;
    }

    // ����û��Ƿ���ʱȡ���˷�������
    if (m_pDataSendManager->m_eventCancelSMV.Lock(0))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): User cancels the current operation \n"));
        return false;
    }

    // ��ʼ��������
    IOCTRL_IN_PARAM_FT3 stFt3InParam;
    memset(&stFt3InParam, 0, sizeof(stFt3InParam));
    stFt3InParam.nDeviceItem = m_enPortName;                            ///< Ft3ʵ����Ӧ��ʵ�ʶ˿�
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_CHANGE_MU_FRAME, &stFt3InParam, sizeof(stFt3InParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): Port%d IOCTRL_CHANGE_MU_FRAME failed \n", m_enPortName));
        return false;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): Port%d IOCTRL_CHANGE_MU_FRAME successfully \n", m_enPortName));
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
bool CFrameSendFt3::UpdateSMVFrames( CStateSendBuffer* pStateSendBuffer )
{
    // �ж��Ƿ��Ѿ���ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): uninit \n"));
        return false;
    }

    // ����������
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): Parameter error \n"));
        return false;
    }

    // ���㻺����ƫ��
    unsigned int uiBufferOffset = 0;
    if (0 == m_pMapHeader->Ft3Header.nCurTxBufItem)
    {
        uiBufferOffset  = FT3_SEND_LOOP_BUF1_POS;
    }
    else
    {
        uiBufferOffset  = FT3_SEND_LOOP_BUF0_POS;
    }

    // �������ͻ�����
    if (0 != memcpy_s(m_pMapDataBuffer + uiBufferOffset, FT3_SEND_LOOP_BUF0_LEN, pStateSendBuffer->m_pucDataBuffer, pStateSendBuffer->m_uiDataBufferLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): Fill send buffer failed"));
        return false;
    }

    // ��ʼ��������
    IOCTRL_IN_PARAM_FT3 stFt3InParam;
    memset(&stFt3InParam, 0, sizeof(stFt3InParam));
    stFt3InParam.nDeviceItem = m_enPortName;                            ///< Ft3ʵ����Ӧ��ʵ�ʶ˿�
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_CHANGE_MU_FRAME, &stFt3InParam, sizeof(stFt3InParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): Port%d IOCTRL_CHANGE_MU_FRAME failed \n", m_enPortName));
        return false;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): Port%d IOCTRL_CHANGE_MU_FRAME successfully \n", m_enPortName));
    return true;
}
