/// @file
///
/// @brief
///     ������FPGA�Ľ�����������ݷ��Ͳ���
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.3.22
///
/// �޸���ʷ��
///     Modified by:    chao
///     Modified date:  2013.6.19
///     Version:        1.0.1
///     Descriptions:   ����˷��͹���
///

#include "StdAfx.h"
#include "FrameSend.h"
#include "src\service\datalayer\config\ParamConfigSMVSend.h"
#include "src\service\datalayer\config\ParamConfigGOOSESend.h"
#include "src\service\datalayer\config\ParamconfigManualTest.h"
#include "src\service\datalayer\config\ParamConfigGOOSERecv.h"
#include "src\service\datalayer\config\ParamConfigStateSequence.h"
#include "src\service\datalayer\DsmDataLayer.h"

#define MAX_TEST_RESULT_COUNT           50                              ///< ���Խ�������������

///
/// @brief
///    ���캯��
///
CFrameSend::CFrameSend()
: m_hGPMDevice(INVALID_HANDLE_VALUE)
, m_bIsInited(false)
, m_pFrameFt3C(NULL)
, m_pFrameNetA(NULL)
, m_pFrameNetB(NULL)
, m_pFrameNetC(NULL)
, m_eManualTestSMVSendStatus(SEND_STATUS_STOPPED)
, m_eManualTestGooseSendStatus(SEND_STATUS_STOPPED)
, m_eCurFunctionType(FUNCTION_TYPE_UNKNOWN)
, m_iCurStateIndex(-1)
{
    m_vecManualTestResult.clear();
    m_vecStateSequenceResult.clear();
}

///
/// @brief
///    ��������
///
CFrameSend::~CFrameSend(void)
{
    // ж�ط��͹�����
    UnInitFrameSend();

    // �ͷ����е��ֶ���������
    ClearAllManualTestResult();

    // �ͷ����е�״̬���н����
    ClearAllStateSequenceResult();
}

///
/// @brief
///    ��ȡ���ķ��͹�����ʵ��
///
CFrameSend* CFrameSend::getInstance()
{
    static CFrameSend s_frameSend;
    return &s_frameSend;
}

///
/// @brief
///    ��ʼ�����͹�����
///
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSend::InitFrameSend(HANDLE hGPMDevice)
{
    // �ж��Ƿ��Ѿ���ʼ��
    if (m_bIsInited)
    {
        return true;
    }

    // ���GPMC���߾��
    if (INVALID_HANDLE_VALUE == hGPMDevice)
    {
        // �������Ϊ��
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::InitFrameSend(): GPM Device Handle Invalide ! \n"));
        goto exitCode;
    }
    m_hGPMDevice = hGPMDevice;

    // �⴮��
    if (NULL == m_pFrameFt3C)
    {
        m_pFrameFt3C = new CFrameSendFt3(this, m_hGPMDevice, FT3_PORT_LIGHT_C);
        if (NULL == m_pFrameFt3C)
        {
            // ����Ft3���ʵ��ʧ��
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::InitFrameSend(): Create Ft3 Send on Port C Failed \n"));
            goto exitCode;
        }
        m_pFrameFt3C->InitFt3(FT3_SEND_NOTIFY_EVENT_NAME_C.c_str(), FT3_SEND_MAP_FILE_NAME_C.c_str(), FT3_SEND_MAP_FILE_LEN);
    }

    // ����̫����A
    if (NULL == m_pFrameNetA)
    {
        m_pFrameNetA = new CFrameSendNet(this, m_hGPMDevice, NET_PORT_LIGHT_A);
        if (NULL == m_pFrameNetA)
        {
            // ��������ʵ��ʧ��
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::InitFrameSend(): Create Net Send on Port A Failed \n"));
            goto exitCode;
        }
        m_pFrameNetA->InitNet(NET_SEND_NOTIFY_EVENT_NAME_A.c_str(), NET_SEND_MAP_FILE_NAME_A.c_str(), NET_SEND_MAP_FILE_LEN);
    }

    // ����̫����B
    if (NULL == m_pFrameNetB)
    {
        m_pFrameNetB = new CFrameSendNet(this, m_hGPMDevice, NET_PORT_LIGHT_B);
        if (NULL == m_pFrameNetB)
        {
            // ��������ʵ��ʧ��
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::InitFrameSend(): Create Net Send on Port B Failed \n"));
            goto exitCode;
        }
        m_pFrameNetB->InitNet(NET_SEND_NOTIFY_EVENT_NAME_B.c_str(), NET_SEND_MAP_FILE_NAME_B.c_str(), NET_SEND_MAP_FILE_LEN);
    }

    // ����̫����
    if (NULL == m_pFrameNetC)
    {
        m_pFrameNetC = new CFrameSendNet(this, m_hGPMDevice, NET_PORT_ELECTRONIC);
        if (NULL == m_pFrameNetC)
        {
            // ��������ʵ��ʧ��
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::InitFrameSend(): Create NET Send on Port C Failed \n"));
            goto exitCode;
        }
        m_pFrameNetC->InitNet(NET_SEND_NOTIFY_EVENT_NAME_C.c_str(), NET_SEND_MAP_FILE_NAME_C.c_str(), NET_SEND_MAP_FILE_LEN);
    }

    // Ӳ����ʱ��
    if (!m_objHardwareTimer.Init(_T("TMR1:")))
    {
        goto exitCode;
    }

    // ���Ϊ�ѳɹ���ʼ��
    m_bIsInited = true;
    return true;

exitCode:
    UnInitFrameSend();
    return false;
}

///
/// @brief
///    ж�ط��͹�����
///
void CFrameSend::UnInitFrameSend()
{
    // �⴮��
    if (NULL != m_pFrameFt3C )
    {
        m_pFrameFt3C->UnInitFt3();
        delete m_pFrameFt3C;
        m_pFrameFt3C = NULL;
    }

    // ����̫����A
    if (NULL != m_pFrameNetA )
    {
        m_pFrameNetA->UnInitNet();
        delete m_pFrameNetA;
        m_pFrameNetA = NULL;
    }

    // ����̫����B
    if (NULL != m_pFrameNetB )
    {
        m_pFrameNetB->UnInitNet();
        delete m_pFrameNetB;
        m_pFrameNetB = NULL;
    }

    // ����̫����
    if (NULL != m_pFrameNetC)
    {
        m_pFrameNetC->UnInitNet();
        delete m_pFrameNetC;
        m_pFrameNetC = NULL;
    }

    // Ӳ����ʱ��
    m_objHardwareTimer.UnInit();

    // ����Ϊδ��ʼ��
    m_bIsInited = false;
}

///
/// @brief
///     ���GPMC���߾��
///
/// @return
///     HANDLE    GPMC���߾��
///
HANDLE CFrameSend::GetGPMDevice()
{
    return m_hGPMDevice;
}

///
/// @brief
///    �Ƿ��ѳ�ʼ��
///
///
/// @return
///     bool    �Ƿ��ѳ�ʼ��
///
bool CFrameSend::IsInited()
{
    return m_bIsInited;
}

///
/// @brief
///    ��õ�ǰ���ڽ��е��������ͣ��ֶ����顢״̬���У�
///
/// @return
///     eFunctionType    ��ǰ���ڽ��е���������
///
eFunctionType CFrameSend::GetCurFunctionType()
{
    return m_eCurFunctionType;
}

///
/// @brief
///    ���SMV���͵�״̬
///
///
/// @return
///     eSendStatus    SMV����״̬
///
eSendStatus CFrameSend::GetManualTestSMVSendStatus()
{
    return m_eManualTestSMVSendStatus;
}

///
/// @brief
///    ����SMV���ƿ�
///
/// @param[in]  stFunctionType  �������ͣ��ֶ����顢״̬���У�
/// @param[in]  pStateTestResult    ָ�����������ָ��
///
/// @return
///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSend::__SendSMVCbs(eFunctionType stFunctionType, CStateTestResult* pStateTestResult)
{
    // ����������
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__SendSMVCbs(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }

    // �ֶ�������Ҫ������֡��Ȼ��������
    if (FUNCTION_TYPE_MANUAL_TEST == stFunctionType)
    {
        // ���ɱ�ӳ������ͨ��һ���������εĲ���ֵ
        CDspRecvData    stDspRecvData;
        CStatus*        pStatus = pStateTestResult->__GetSMVSendValues();
        if (!__GenVirtualChannelSamples(pStatus, stDspRecvData))
        {
            return ERROR_CODE_FAILURE;
        }

        // �ַ�SMV���ƿ�
        if (!__DistributeSmvCbs())
        {
            return ERROR_CODE_FAILURE;
        }

        //
        // ���Ͱ󶨵������˿ڵĿ��ƿ�
        // ��������˷���ʧ�ܣ���ôֹͣ���з���
        //
        enumErrorCode   eRet = m_pFrameNetA->SendSMVCbs(stDspRecvData);
        if (ERROR_CODE_SUCCESS != eRet)
        {
            m_pFrameNetA->StopSMVSend();
            return eRet;
        }

        eRet = m_pFrameNetB->SendSMVCbs(stDspRecvData);
        if (ERROR_CODE_SUCCESS != eRet)
        {
            m_pFrameNetB->StopSMVSend();
            return eRet;
        }

        eRet = m_pFrameNetC->SendSMVCbs(stDspRecvData);
        if (ERROR_CODE_SUCCESS != eRet)
        {
            m_pFrameNetC->StopSMVSend();
            return eRet;
        }

        eRet = m_pFrameFt3C->SendSMVCbs(stDspRecvData);
        if (ERROR_CODE_SUCCESS != eRet)
        {
            m_pFrameFt3C->StopFt3Send();
            return eRet;
        }
    }
    // ״̬����ֱ�ӷ���Ԥ�ȼ���õķ���֡
    else if (FUNCTION_TYPE_STATE_SEQUENCE == stFunctionType)
    {
        std::list<CStateSendBuffer*>::iterator  iter;
        CStateSendBuffer*                       pStateSendBuffer;


        // ����ÿһ���˿ڵķ��ͻ���
        iter = pStateTestResult->m_listSmvSendBuffers.begin();
        for (; iter != pStateTestResult->m_listSmvSendBuffers.end(); iter++)
        {
            pStateSendBuffer = *iter;
            if (pStateSendBuffer)
            {
                // �˿ڷ�������
                switch(pStateSendBuffer->m_ePortName)
                {
                case NET_PORT_LIGHT_A:
                    {
                        if (!m_pFrameNetA->SendSMVCbs(pStateSendBuffer))
                        {
                            return ERROR_CODE_FAILURE;
                        }
                    }
                    break;

                case NET_PORT_LIGHT_B:
                    {
                        if (!m_pFrameNetB->SendSMVCbs(pStateSendBuffer))
                        {
                            return ERROR_CODE_FAILURE;
                        }
                    }
                    break;

                case NET_PORT_ELECTRONIC:
                    {
                        if (!m_pFrameNetC->SendSMVCbs(pStateSendBuffer))
                        {
                            return ERROR_CODE_FAILURE;
                        }
                    }
                    
                    break;

                case FT3_PORT_LIGHT_C:
                    {
                        if (!m_pFrameFt3C->SendSMVCbs(pStateSendBuffer))
                        {
                            return ERROR_CODE_FAILURE;
                        }
                    }
                    break;
                }
            }
        }
    }

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    ׼��SMV����֡
///
/// @param[in]  pStateTestResult    ָ�����������ָ�룬���ڱ���SMV����֡
///
/// @return
///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSend::__PrepareSmvSendBuffer(CStateTestResult* pStateTestResult)
{
    bool        bFlag   = true;
    CStatus*    pStatus = NULL;


    // ����������
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__PrepareSmvSendBuffer(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }

    // ���ɱ�ӳ������ͨ��һ���������εĲ���ֵ
    CDspRecvData    stDspRecvData;
    pStatus = pStateTestResult->__GetSMVSendValues();
    if (!__GenVirtualChannelSamples(pStatus, stDspRecvData))
    {
        return ERROR_CODE_FAILURE;
    }

    // Ԥ�ȼ�����˿ڵ�SMV����֡
    enumErrorCode   eRet = m_pFrameNetA->PrepareSmvSendBuffer(stDspRecvData, pStateTestResult);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        return eRet;
    }

    eRet = m_pFrameNetB->PrepareSmvSendBuffer(stDspRecvData, pStateTestResult);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        return eRet;
    }

    eRet = m_pFrameNetC->PrepareSmvSendBuffer(stDspRecvData, pStateTestResult);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        return eRet;
    }

    eRet = m_pFrameFt3C->PrepareSmvSendBuffer(stDspRecvData, pStateTestResult);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        return eRet;
    }

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    ����SMV����֡
///
/// @param[in]  stFunctionType  ��������(�ֶ����顢״̬����)
/// @param[in]  pStateTestResult    ָ�����������ָ��
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSend::__UpdateSMVSend(eFunctionType stFunctionType, CStateTestResult* pStateTestResult)
{
    bool        bFlag   = true;
    CStatus*    pStatus = NULL;


    // ����������
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__SendSMVCbs(): Parameter error \n"));
        return false;
    }

    // �ֶ�������Ҫ������֡��Ȼ��������
    if (FUNCTION_TYPE_MANUAL_TEST == stFunctionType)
    {
        // ���ɱ�ӳ������ͨ��һ���������εĲ���ֵ
        CDspRecvData    stDspRecvData;
        pStatus = pStateTestResult->__GetSMVSendValues();
        if (!__GenVirtualChannelSamples(pStatus, stDspRecvData))
        {
            return false;
        }

        // ���Ͱ󶨵������˿ڵĿ��ƿ�
        if (!m_pFrameNetA->UpdateSMVFrames(stDspRecvData) ||
            !m_pFrameNetB->UpdateSMVFrames(stDspRecvData) ||
            !m_pFrameNetC->UpdateSMVFrames(stDspRecvData) ||
            !m_pFrameFt3C->UpdateSMVFrames(stDspRecvData)
            )
        {
            return false;
        }
    }
    // ״̬����ֱ�ӷ���Ԥ�ȼ���õķ���֡
    else if (FUNCTION_TYPE_STATE_SEQUENCE == stFunctionType)
    {
        std::list<CStateSendBuffer*>::iterator  iter;
        CStateSendBuffer*                       pStateSendBuffer;


        // ����ÿһ���˿ڵķ��ͻ���
        iter = pStateTestResult->m_listSmvSendBuffers.begin();
        for (; iter != pStateTestResult->m_listSmvSendBuffers.end(); iter++)
        {
            pStateSendBuffer = *iter;
            if (pStateSendBuffer)
            {
                // ���˿ڷ�������
                switch(pStateSendBuffer->m_ePortName)
                {
                case NET_PORT_LIGHT_A:
                    {
                        if (!m_pFrameNetA->UpdateSMVFrames(pStateSendBuffer))
                        {
                            return false;
                        }
                    }
                    break;

                case NET_PORT_LIGHT_B:
                    {
                        if (!m_pFrameNetB->UpdateSMVFrames(pStateSendBuffer))
                        {
                            return false;
                        }
                    }
                    break;

                case NET_PORT_ELECTRONIC:
                    {
                        if (!m_pFrameNetC->UpdateSMVFrames(pStateSendBuffer))
                        {
                            return false;
                        }
                    }
                    break;

                case FT3_PORT_LIGHT_C:
                    {
                        if (!m_pFrameFt3C->UpdateSMVFrames(pStateSendBuffer))
                        {
                            return false;
                        }
                    }
                    break;
                }
            }
        }
    }

    return true;
}

///
/// @brief
///    ֹͣSMV����
///
bool CFrameSend::__StopSMVSend()
{
    // ֹͣ�������Ͷ˿�
    if (!m_pFrameNetA->StopSMVSend() ||
        !m_pFrameNetB->StopSMVSend() ||
        !m_pFrameNetC->StopSMVSend() ||
        !m_pFrameFt3C->StopFt3Send()
        )
    {
        return false;
    }

    return true;
}

///
/// @brief
///    ���GOOSE�ķ���״̬
///
///
/// @return
///     eSendStatus    GOOSE�ķ���״̬
///
eSendStatus CFrameSend::GetManualTestGooseSendStatus()
{
    return m_eManualTestGooseSendStatus;
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
enumErrorCode CFrameSend::__PrepareGooseSendBuffer( CStateTestResult* pStateTestResult )
{
    enumErrorCode   eRet;


    // Ԥ�ȼ�����˿ڵ�GOOSE����֡
    eRet = m_pFrameNetA->PrepareGooseSendBuffer(pStateTestResult);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        return eRet;
    }

    eRet = m_pFrameNetB->PrepareGooseSendBuffer(pStateTestResult);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        return eRet;
    }

    eRet = m_pFrameNetC->PrepareGooseSendBuffer(pStateTestResult);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        return eRet;
    }

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    ׼��GOOSE���ƿ����֡
///
/// @param[in]  pStateTestResult    ָ�����������ָ�룬���ڱ���GOOSE����֡
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSend::__PrepareGooseUpdateBuffer( CStateTestResult* pStateTestResult )
{
    CStateSendBuffer*   pStateSendBuffer;
    std::list<CStateSendBuffer*>::iterator  iter;


    // ����ÿһ���˿ڵķ��ͻ���
    iter = pStateTestResult->m_listGooseSendBuffers.begin();
    for (; iter != pStateTestResult->m_listGooseSendBuffers.end(); iter++)
    {
        pStateSendBuffer = *iter;
        if (pStateSendBuffer)
        {
            // ���˿ڸ��·�������
            switch(pStateSendBuffer->m_ePortName)
            {
            case NET_PORT_LIGHT_A:
                {
                    if (!m_pFrameNetA->PrepareGooseUpdateBuffer(pStateSendBuffer))
                    {
                        return false;
                    }
                }
                break;

            case NET_PORT_LIGHT_B:
                {
                    if (!m_pFrameNetB->PrepareGooseUpdateBuffer(pStateSendBuffer))
                    {
                        return false;
                    }
                }
                break;

            case NET_PORT_ELECTRONIC:
                {
                    if (!m_pFrameNetC->PrepareGooseUpdateBuffer(pStateSendBuffer))
                    {
                        return false;
                    }
                }
                break;
            }
        }
    }

    return true;
}

///
/// @brief
///    ����GOOSE���ƿ�
///
/// @param[in]  stFunctionType      �������ͣ��ֶ����顢״̬���У�
/// @param[in]  pStateTestResult    ����״̬���Խ���Ķ����ڲ������д����͵�GOOSE���ƿ�
///
/// @return
///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSend::__SendGooseCbs(eFunctionType stFunctionType, CStateTestResult* pStateTestResult)
{
    // ����������
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__SendGooseCbs(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }

    // �ֶ�������Ҫ������֡��Ȼ��������
    if (FUNCTION_TYPE_MANUAL_TEST == stFunctionType)
    {
        // �ַ�GOOSE���ƿ鵽�������Ͷ˿�
        __DistributeGooseCbs(pStateTestResult);

        //
        // ���Ͱ󶨵������˿ڵĿ��ƿ�
        // ��������˷���ʧ�ܣ���ôֹͣ���з���
        //
        enumErrorCode   eRet = m_pFrameNetA->SendGooseCbs();
        if (ERROR_CODE_SUCCESS != eRet)
        {
            m_pFrameNetA->StopGooseSend();
            return eRet;
        }

        eRet = m_pFrameNetB->SendGooseCbs();
        if (ERROR_CODE_SUCCESS != eRet)
        {
            m_pFrameNetB->StopGooseSend();
            return eRet;
        }

        eRet = m_pFrameNetC->SendGooseCbs();
        if (ERROR_CODE_SUCCESS != eRet)
        {
            m_pFrameNetC->StopGooseSend();
            return eRet;
        }
    }
    // ״̬����ֱ�ӷ���Ԥ�ȼ���õķ���֡
    else if (FUNCTION_TYPE_STATE_SEQUENCE == stFunctionType)
    {
        std::list<CStateSendBuffer*>::iterator  iter;
        CStateSendBuffer*                       pStateSendBuffer;


        // ����ÿһ���˿ڵķ��ͻ���
        iter = pStateTestResult->m_listGooseSendBuffers.begin();
        for (; iter != pStateTestResult->m_listGooseSendBuffers.end(); iter++)
        {
            pStateSendBuffer = *iter;
            if (pStateSendBuffer)
            {
                // �޸ķ���֡�ı�λʱ���ֽ�
                if (!pStateSendBuffer->UpdateEventTime(m_stStateSequenceStartTime + pStateTestResult->m_stStateStartTime))
                {
                    return ERROR_CODE_FAILURE;
                }

                // ���˿ڷ���GOOSE����
                switch(pStateSendBuffer->m_ePortName)
                {
                case NET_PORT_LIGHT_A:
                    {
                        if (!m_pFrameNetA->SendGooseCbs(pStateSendBuffer))
                        {
                            return ERROR_CODE_FAILURE;
                        }
                    }
                    break;

                case NET_PORT_LIGHT_B:
                    {
                        if (!m_pFrameNetB->SendGooseCbs(pStateSendBuffer))
                        {
                            return ERROR_CODE_FAILURE;
                        }
                    }
                    break;

                case NET_PORT_ELECTRONIC:
                    {
                        if (!m_pFrameNetC->SendGooseCbs(pStateSendBuffer))
                        {
                            return ERROR_CODE_FAILURE;
                        }
                    }
                    break;
                }
            }
        }
    }

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    ���ɱ�ӳ������ͨ��һ���������εĲ���ֵ
///
/// @param[in]  pStatus         ָ��һ��״̬�����ָ��
/// @param[out] stDspRecvData   ��ż�����Ĳ���ֵ�Ľṹ��
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSend::__GenVirtualChannelSamples(CStatus* pStatus, CDspRecvData& stDspRecvData)
{
    int                                 iCount = 0;                     ///< ͨ��������
    CDspSendData                        stDspSendData;                  ///< ��DSP���͵Ľṹ��
    CSMVSendVirtualChannel*             pVirtualChannel;


    // ����������
    if (NULL == pStatus)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::GenVirtualChannelSamples(): Parameter error \n"));
        return false;
    }

    // ���DSP���Ͳ���
    stDspSendData.m_mapChannelName2Index.clear();
    memset(&(stDspSendData.m_stSendData), 0 , sizeof(stDspSendData.m_stSendData));

    //
    // �������ͨ���Ĳ������ã����ں���DSP�Ĳ�������
    //
    pVirtualChannel = pStatus->FirstSMVSendVirtualChannel();
    while(pVirtualChannel)
    {
        // �������ͨ����
        if (iCount > MAX_SMV_GEN_CHANNEL_COUNT)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::GenVirtualChannelSamples(): iCount > MAX_SMV_GEN_CHANNEL_COUNT \n"));
            return false;
        }

        // ����ͨ����
        stDspSendData.m_mapChannelName2Index.insert(std::pair<std::wstring, int>(pVirtualChannel->GetName(), iCount));

        //
        // ��������ͨ���Ľ�ֱ�����ͣ�����ֱ����������ֵ����ǡ�Ƶ��
        //
        if (CURRENT_TYPE_AC == pVirtualChannel->GetACDCType())
        {
            // ֱ������
            stDspSendData.m_stSendData.fDCValue[iCount]          = 0.0f;

            // ��ֵ
            stDspSendData.m_stSendData.fRmsValue[iCount][0]      = (float)pVirtualChannel->GetAmplitude();

            // ���
            stDspSendData.m_stSendData.fPhaseAngle[iCount][0]    = (float)pVirtualChannel->GetPhaseAngle();

            // Ƶ��
            stDspSendData.m_stSendData.fFreq[iCount][0]          = (float)pVirtualChannel->GetFrequency();
        }
        else
        {
            // ֱ������
            stDspSendData.m_stSendData.fDCValue[iCount]          = (float)pVirtualChannel->GetAmplitude();
        }

        //
        // ����г������
        //
        stDspSendData.m_stSendData.nHarmCount[iCount] = 1;
        CHarmonic*  pHarmonic = pVirtualChannel->GetHarmonic();
        if (pHarmonic)
        {
            // �����ֱ��г������ô��ֱ��г���ķ�ֵ�ۼӵ�ֱ��������
            if (pHarmonic->GetHarmonicType() == HARMONIC_DC)
            {
                stDspSendData.m_stSendData.fDCValue[iCount]     += (float)pHarmonic->GetAmplitude();
            }
            else if (pHarmonic->GetHarmonicType()  >= HARMONIC_2_TIMES)
            {
                // г������
                stDspSendData.m_stSendData.nHarmCount[iCount]    = 2;

                // г����ֵ
                stDspSendData.m_stSendData.fRmsValue[iCount][1]  = (float)pHarmonic->GetAmplitude();

                // г�����
                stDspSendData.m_stSendData.fPhaseAngle[iCount][1]= (float)pHarmonic->GetPhaseAngle();

                // г��Ƶ�ʣ�����ͨ��Ƶ�� * г������)
                stDspSendData.m_stSendData.fFreq[iCount][1]      = (float)pVirtualChannel->GetFrequency() * (pHarmonic->GetHarmonicType() - HARMONIC_2_TIMES + 2);
            }
        }
        stDspSendData.m_stSendData.fScaleFactor[iCount] = 1.0f;         ///< ��������

        //
        // �����һ����ӳ�������ͨ��
        //
        iCount++;
        pVirtualChannel = pStatus->NextSMVSendVirtualChannel(pVirtualChannel);
    }

    stDspSendData.m_stSendData.nChannelCount = iCount;                  ///< ͨ����
    stDspSendData.m_stSendData.nSampleRate   = (uint16_t)__GetSampleFrequency();///< ������
    stDspSendData.m_stSendData.nSmvCount     = (uint16_t)(stDspSendData.m_stSendData.nSampleRate / 50);

    // ����ͨ����
    stDspRecvData.m_mapChannelName2Index.clear();
    stDspRecvData.m_iFrequency  = __GetSampleFrequency();               ///< ����Ƶ��
    stDspRecvData.m_iSmvCount   = stDspSendData.m_stSendData.nSmvCount; ///< ÿ�ܲ��Ĳ�������
    stDspRecvData.m_mapChannelName2Index.insert(stDspSendData.m_mapChannelName2Index.begin(), stDspSendData.m_mapChannelName2Index.end());

    //
    // ������ڱ�ӳ�������ͨ������ô�����䲨��
    //
    if (iCount > 0)
    {
        //
        // ���´�����DSP���ͼ������󣬲��ȴ�������
        //
        CDsp    dspObject;
        if (dspObject.SmvGenCalc(1000, stDspSendData.m_stSendData, &(stDspRecvData.m_stRecvData)))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::GenVirtualChannelSamples(): Dsp SendSmvGen() failed \n"));
            return false;
        }

        // �ȴ�DSP������
        if (dspObject.WaitForObject(CDsp::T_SMV_GEN, 1000))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::GenVirtualChannelSamples(): Dsp WaitForObject() failed \n"));
            return false;
        }
    }

    return true;
}

///
/// @brief
///     ���SMV����Ƶ��
///
/// @return
///     int     ����Ƶ��
///
int CFrameSend::__GetSampleFrequency()
{
    int   iValue = 0;


    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL != pParamConfigSMVSend)
    {
        switch(pParamConfigSMVSend->GetFrequency())
        {
        case FT3_SAMP_RATE_4K:
            iValue    = 4000;
            break;

        case FT3_SAMP_RATE_5K:
            iValue    = 5000;
            break;

        case FT3_SAMP_RATE_8K:
            iValue    = 8000;
            break;

        case FT3_SAMP_RATE_10K:
            iValue    = 10000;
            break;

        case FT3_SAMP_RATE_12P8K:
            iValue    = 12800;
            break;

        default:
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::GetSampleFrequency(): Unknown frequency \n"));
            }
        }
    }

    return iValue;
}

///
/// @brief
///     ����GOOSE����֡
///
/// @param[in]  stFunctionType      �������ͣ��ֶ����顢״̬���У�
/// @param[in]  pStateTestResult    ����״̬���Խ���Ķ����ڲ������д����͵�GOOSE���ƿ�
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSend::__UpdateGooseSend(eFunctionType stFunctionType, CStateTestResult* pStateTestResult)
{
    // ����������
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__UpdateGooseSend(): Parameter error \n"));
        return false;
    }

    // �ֶ�������Ҫ������֡��Ȼ��������
    if (FUNCTION_TYPE_MANUAL_TEST == stFunctionType)
    {
        // �ַ�GOOSE���ƿ鵽�������Ͷ˿�
        __DistributeGooseCbs(pStateTestResult);

        // ���Ͱ󶨵������˿ڵĿ��ƿ�
        if (!m_pFrameNetA->UpdateGooseFrames() ||
            !m_pFrameNetB->UpdateGooseFrames() ||
            !m_pFrameNetC->UpdateGooseFrames() )
        {
            return false;
        }
    }
    // ״̬����ֱ�ӷ���Ԥ�ȼ���õķ���֡
    else if (FUNCTION_TYPE_STATE_SEQUENCE == stFunctionType)
    {
        std::list<CStateSendBuffer*>::iterator  iter;
        CStateSendBuffer*                       pStateSendBuffer;


        // ����ÿһ���˿ڵķ��ͻ���
        iter = pStateTestResult->m_listGooseSendBuffers.begin();
        for (; iter != pStateTestResult->m_listGooseSendBuffers.end(); iter++)
        {
            pStateSendBuffer = *iter;
            if (pStateSendBuffer)
            {
                // �޸����з���֡�ı�λʱ���ֽ�
                if (!pStateSendBuffer->UpdateEventTime(m_stStateSequenceStartTime + pStateTestResult->m_stStateStartTime))
                {
                    return false;
                }

                // �޸����з���֡�б�λʱ��ͨ����ֵ
                if (!pStateSendBuffer->UpdateChannelTime(m_stStateSequenceStartTime + pStateTestResult->m_stStateStartTime))
                {
                    return false;
                }

                // ���˿ڷ��ͱ���
                switch(pStateSendBuffer->m_ePortName)
                {
                case NET_PORT_LIGHT_A:
                    {
                        if (!m_pFrameNetA->UpdateGooseFrames(pStateSendBuffer))
                        {
                            return false;
                        }
                    }
                    break;

                case NET_PORT_LIGHT_B:
                    {
                        if (!m_pFrameNetB->UpdateGooseFrames(pStateSendBuffer))
                        {
                            return false;
                        }
                    }
                    break;

                case NET_PORT_ELECTRONIC:
                    {
                        if (!m_pFrameNetC->UpdateGooseFrames(pStateSendBuffer))
                        {
                            return false;
                        }
                    }
                    break;
                }
            }
        }
    }

    return true;
}

///
/// @brief
///    ֹͣGOOSE����
///
bool CFrameSend::__StopGooseSend()
{
    // ֹͣ�������Ͷ˿�
    if (!m_pFrameNetA->StopGooseSend() ||
        !m_pFrameNetB->StopGooseSend() ||
        !m_pFrameNetC->StopGooseSend()
        )
    {
        return false;
    }

    return true;
}

///
/// @brief
///    �����ֶ�����SMV�ķ���״̬
///
/// @param[in]  eNewStatus    �µ�SMV����״̬
///
void CFrameSend::__SetManualTestSMVSendingStatus( eSendStatus eNewStatus )
{
    m_eManualTestSMVSendStatus  = eNewStatus;                           ///< �µ�SMV����״̬
}

///
/// @brief
///    �����ֶ�����Goose�ķ���״̬
///
/// @param[in]  eNewStatus  �µ�GOOSE����״̬
///
void CFrameSend::__SetManualTestGooseSendStatus( eSendStatus eNewStatus )
{
    m_eManualTestGooseSendStatus = eNewStatus;                          ///< �µ��ֶ�����Goose����״̬
}

///
/// @brief
///    ���õ�ǰ�����ڽ��е���������
///
/// @param[in]  eNewType    �µ���������
///
void CFrameSend::__SetCurFunctionType( eFunctionType eNewType )
{
    m_eCurFunctionType = eNewType;
}

///
/// @brief
///    ���һ��״̬���в��Խ��������������
///
/// @return
///     CStateTestResult*    �ɹ�ʱ����״̬���Խ�������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::__AddStateSequenceResult()
{
    CStateTestResult* pStateTestResult = NULL;


    // newһ��CStateTestResult����
    pStateTestResult = new CStateTestResult(TEST_TYPE_STATE_SEQUENCE);
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__AddStateSequenceResult(): new CStateTestResult failed \n"));
        return NULL;
    }

    // ��ʼ��CStateTestResult����
    if (!pStateTestResult->Init())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__AddStateSequenceResult(): CStateTestResult object init failed \n"));
        delete pStateTestResult;
        return NULL;
    }

    // ��Ӷ��󵽽����������
    m_vecStateSequenceResult.push_back(pStateTestResult);
    return pStateTestResult;
}

///
/// @brief
///    ���״̬���н��������
///
/// @return
///     unsigned int    ״̬���н��������
///
unsigned int CFrameSend::GetStateSequenceResultCount()
{
    return m_vecStateSequenceResult.size();
}

///
/// @brief
///    ���ָ��״̬�ŵ�״̬���н��������
///
/// @param[in]  iStateIndex    ״̬��(��0��ʼ����)
///
/// @return
///     CStateTestResult*    ���ָ��״̬���Խ���Ķ���ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::GetStateSequenceResult( unsigned int iStateIndex )
{
    // ����������
    if (iStateIndex >= m_vecStateSequenceResult.size())
    {
        return NULL;
    }

    return m_vecStateSequenceResult[iStateIndex];
}

///
/// @brief
///    ������е�״̬���н����
///
void CFrameSend::ClearAllStateSequenceResult()
{
    CStateTestResult* pStateTestResult = NULL;
    std::vector<CStateTestResult*>::iterator iter;


    // �ͷŲ��Խ������
    iter = m_vecStateSequenceResult.begin();
    while(m_vecStateSequenceResult.end() != iter)
    {
        pStateTestResult = *iter;
        if (pStateTestResult)
        {
            delete pStateTestResult;
        }

        iter++;
    }

    // �������
    m_vecStateSequenceResult.clear();
}

///
/// @brief
///    ��ʼ״̬���в���
///
/// @param[in]  hWnd  ���ھ������DIֵ�����仯ʱ���ᷢ��WM_DI_CHANGED��Ϣ���˴���
///                             ����Ҫѯ���û��Ƿ��ֶ��л�ʱ���ᷢ��WM_WHETHER_SWITCH��Ϣ���˴���
///                             ��״̬�л��󣬻ᷢ��WM_STATUS_CHANGED��Ϣ���˴���
///                             ��������ɺ󣬻ᷢ��WM_TEST_COMPLETED��Ϣ���˴���
///
/// @return
///     enumErrorCode   ���������̳߳ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSend::StartStateSequenceTest( HWND hWnd )
{
    int                 iStateIndex         = 0;
    CStateTestResult*   pNewStateTestResult = NULL;
    CStateTestResult*   pOldStateTestResult = NULL;
    enumErrorCode       eRet                = ERROR_CODE_SUCCESS;
    std::map<std::wstring, bool> mapChangedDoArray;


    // ����Ƿ��ѳ�ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // �жϴ��ھ���Ƿ���һ�����ڵĴ���
    if (!IsWindow(hWnd))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }
    m_hNotifyHwnd = hWnd;

    // ������ݲ���������
    CDsmDataLayer* pDsmDataLayer = CDsmDataLayer::getInstance();
    if (NULL == pDsmDataLayer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): CDsmDataLayer::getInstance() failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // ����̻߳�����
    m_criticalFunctionType.Lock();

    // ��鵱ǰ�Ƿ����ڷ���״̬����
    if (FUNCTION_TYPE_UNKNOWN != GetCurFunctionType())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): Currently is sending state sequence \n"));
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }
    // ����Ϊ���ڷ���״̬����
    __SetCurFunctionType(FUNCTION_TYPE_STATE_SEQUENCE);

    // �ͷ��̻߳�����
    m_criticalFunctionType.Unlock();

    // �����ǰ�Ĳ��Խ��
    ClearAllStateSequenceResult();

    // ���GOOSE���Ͳ������ù�����
    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        eRet = ERROR_CODE_FAILURE;
        goto ExitHandle;
    }

    // ���GOOSE���ղ������ù�����
    CParamConfigGOOSERecv* pParamConfigGooseRecv = CParamConfigGOOSERecv::getInstance();
    if (NULL == pParamConfigGooseRecv)
    {
        eRet = ERROR_CODE_FAILURE;
        goto ExitHandle;
    }

    // ����SMV���ƿ鵽�������Ͷ˿�
    if (!__DistributeSmvCbs())
    {
        eRet = ERROR_CODE_FAILURE;
        goto ExitHandle;
    }

    // �ַ�GOOSE���ƿ鵽�������Ͷ˿�
    if (!__DistributeGooseCbs())
    {
        eRet = ERROR_CODE_FAILURE;
        goto ExitHandle;
    }

    // ���״̬�������ù�����
    CParamConfigStateSequence* pParamConfigStateSequence = CParamConfigStateSequence::getInstance();
    if (NULL == pParamConfigStateSequence)
    {
        eRet = ERROR_CODE_FAILURE;
        goto ExitHandle;
    }

    // ��ǰ�����ÿ��״̬�ķ��ͱ���
    iStateIndex             = 0;
    CStatus*    pOldStatus  = NULL;
    CStatus*    pStatus     = pParamConfigStateSequence->FirstStatus(false);
    while(pStatus)
    {
        // ����û��Ƿ���ʱȡ��������
        if (m_eventCancelStateSequence.Lock(0))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): User cancels the current operation \n"));
            eRet = ERROR_CODE_CANCEL;
            goto ExitHandle;
        }

        // ��Ӵ��״̬���Խ���Ķ���
        pNewStateTestResult = __AddStateSequenceResult();
        if (NULL == pNewStateTestResult)
        {
            eRet = ERROR_CODE_FAILURE;
            goto ExitHandle;
        }

        // ����SMV�����Ϳ��ƿ��ֵ
        if (!pNewStateTestResult->__GetSMVSendValues()->__Copy(pStatus))
        {
            eRet = ERROR_CODE_FAILURE;
            goto ExitHandle;
        }

        // ������ͬ��SMV����״̬
        CStateTestResult* pSimilarStateTestResult = __FindSimilarSmvTestResult(pNewStateTestResult, iStateIndex);

        // ���������ͬ��SMV����״̬����ô������SMV���ͻ���
        // �����������ͬ��SMV����״̬����ô���¹���SMV���ͻ���
        if (pSimilarStateTestResult)
        {
            // ����SMV���ͻ���
            if (!pNewStateTestResult->__CopySmvSendBuffers(pSimilarStateTestResult))
            {
                eRet = ERROR_CODE_FAILURE;
                goto ExitHandle;
            }
        }
        else
        {
            // ׼��SMV����֡
            eRet = __PrepareSmvSendBuffer(pNewStateTestResult);
            if (ERROR_CODE_SUCCESS != eRet)
            {
                goto ExitHandle;
            }
        }

        // ׼��״̬0��GOOSE����֡
        if (0 == iStateIndex)
        {
            // ���״̬0����DO�ļ�ֵ��
            pStatus->__FindChangedDO(NULL, mapChangedDoArray);

            // ����ӳ�䵽ָ������ͨ����������ʵͨ����ֵ
            if (!pParamConfigGooseSend->UpdateGooseChannelSendValue(mapChangedDoArray))
            {
                eRet = ERROR_CODE_FAILURE;
                goto ExitHandle;
            }

            // ׼��GOOSE����֡
            eRet = __PrepareGooseSendBuffer(pNewStateTestResult);
            if (ERROR_CODE_SUCCESS != eRet)
            {
                goto ExitHandle;
            }

            // ����GOOSE���ƿ�������ͨ����ƫ���������޸Ļ�������ʱ��ͨ����ֵʱ���õ�
            if (!__CalcChannelOffset(pNewStateTestResult))
            {
                eRet = ERROR_CODE_FAILURE;
                goto ExitHandle;
            }
        }
        // ׼������״̬��GOOSE����֡
        else
        {
            // ����GOOSE���ͻ���
            if (!pNewStateTestResult->__CopyGooseSendBuffers(pOldStateTestResult))
            {
                eRet = ERROR_CODE_FAILURE;
                goto ExitHandle;
            }

            // ���ϵ�״̬�Ƚϣ�����true��ʾDO�����˸ı�
            if (pStatus->__FindChangedDO(pOldStatus, mapChangedDoArray))
            {
                // ����ӳ�䵽ָ������ͨ����������ʵͨ����ֵ������Ǳ�λ�ĵ���/˫��ͨ��
                if (!pParamConfigGooseSend->UpdateGooseChannelSendValue(mapChangedDoArray))
                {
                    eRet = ERROR_CODE_FAILURE;
                    goto ExitHandle;
                }

                // �Զ�Ѱ�ұ�λ�ĵ���/˫��ͨ����Ȼ���¼���ڻ�����ʱ��ͨ��
                pNewStateTestResult->__SaveGooseCbTimeChannel();

                // ׼��GOOSE����֡
                if (!__PrepareGooseUpdateBuffer(pNewStateTestResult))
                {
                    eRet = ERROR_CODE_FAILURE;
                    goto ExitHandle;
                }
            }
        }

        // ����״̬�Ľ����
        pOldStateTestResult = pNewStateTestResult;
        pStatus->SetStateTestResult(pNewStateTestResult);

        // ���״̬������һ��״̬
        pOldStatus  = pStatus;
        pStatus     = pParamConfigStateSequence->NextStatus(pOldStatus);
        iStateIndex++;
    }

    // ����GOOSE�����߳�
    pDsmDataLayer->StartRecvData(NET_RECV_TYPE_GOOSE, DSM_MODE_GOOSE_RECV_DI);

    // ���η���ÿ��״̬�ı���
    iStateIndex = 0;
    pStatus     = pParamConfigStateSequence->FirstStatus(false);
    while(pStatus)
    {
        // ����û��Ƿ���ʱȡ��������
        if (m_eventCancelStateSequence.Lock(0))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): User cancels the current operation \n"));
            eRet = ERROR_CODE_CANCEL;
            goto ExitHandle;
        }

        // ���״̬��Ӧ�Ľ��������
        pNewStateTestResult = pStatus->GetStateTestResult();
        if (NULL == pNewStateTestResult)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): irregular test result \n"));
            eRet = ERROR_CODE_FAILURE;
            goto ExitHandle;
        }

        // ֪ͨ�������ڷ��͵�iStateIndex��״̬
        ::PostMessage(hWnd, WM_STATUS_CHANGED, iStateIndex, 0);

        // ��õ�ǰFPGAʱ��
        CEventTime  stCurTime;
        GetCurrentFpgaTime(stCurTime);

        // ����ǵ�һ��״̬����ô����SendSMVCbs������
        // �����������״̬����ô����UpdateSMVSend������
        if (0 == iStateIndex)
        {
            // ����״̬���п�ʼʱ��
            m_stStateSequenceStartTime = stCurTime;

            // ����״̬����ʱ�䣨���ʱ�䣩
            pNewStateTestResult->SetStartTime(stCurTime - m_stStateSequenceStartTime);

            // ��ʼ����SMV
            if (ERROR_CODE_SUCCESS != __SendSMVCbs(FUNCTION_TYPE_STATE_SEQUENCE, pNewStateTestResult))
            {
                break;
            }

            // ��ʼ����GOOSE
            if (ERROR_CODE_SUCCESS != __SendGooseCbs(FUNCTION_TYPE_STATE_SEQUENCE, pNewStateTestResult))
            {
                break;
            }
        }
        else
        {
            // ����״̬����ʱ�䣨���ʱ�䣩
            pNewStateTestResult->SetStartTime(stCurTime - m_stStateSequenceStartTime);

            // ����SMV
            if (!__UpdateSMVSend(FUNCTION_TYPE_STATE_SEQUENCE, pNewStateTestResult))
            {
                break;
            }

            // ����GOOSE
            if (!__UpdateGooseSend(FUNCTION_TYPE_STATE_SEQUENCE, pNewStateTestResult))
            {
                break;
            }
        }

        // ���ò�����ɱ�ʶ
        pNewStateTestResult->SetTested(true);

        // ���õ�ǰ���ڷ��͵�״̬���к�
        __SetCurStateIndex(iStateIndex);
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): State %d send successfully \n", iStateIndex));

        //
        // ���������ж��Ƿ��л���״̬������һ��״̬
        //
        eSwitchType eType = pStatus->GetSwitchType();
        if (SWITCH_TYPE_TIME == eType)                                  ///< ��ʱ�л�
        {
            // �ȴ�ָ��ʱ����û�ȡ���¼���λ
            if (!m_objHardwareTimer.Wait(pStatus->GetDelay(), m_eventCancelStateSequence.m_hObject))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): User cancels the current operation \n"));
                eRet = ERROR_CODE_CANCEL;
                goto ExitHandle;
            }
        }
        else if (SWITCH_TYPE_DI == eType)                               ///< �������л�
        {
            eLogicalOperator eOperator = pStatus->GetLogicalOperator();
            if (LOGICAL_AND == eOperator)                               ///< �߼���
            {
                while(1)
                {
                    bool                        bResult     = true;
                    CGOOSERecvVirtualChannel*   pDI         = NULL;
                    CChannel*                   pChannel    = NULL;


                    for (int i = 0; i < sizeof(PREDEFINED_DI_INFO)/sizeof(PREDEFINED_DI_INFO[0]); i++)
                    {
                        // ����û��Ƿ���ʱȡ��������
                        if (m_eventCancelStateSequence.Lock(0))
                        {
                            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): User cancels the current operation \n"));
                            eRet = ERROR_CODE_CANCEL;
                            goto ExitHandle;
                        }

                        // �ж�ָ����DI�Ƿ񱻹�ѡ
                        if (!pStatus->IsDIChecked(PREDEFINED_DI_INFO[i].m_strName))
                        {
                            continue;
                        }

                        // �ж�ָ��������ͨ���Ƿ����
                        pDI = pParamConfigGooseRecv->GetVirtualChannelByName(PREDEFINED_DI_INFO[i].m_strName);
                        if (NULL == pDI)
                        {
                            continue;
                        }

                        // �ж�ָ��������ͨ���Ƿ�ӳ������ʵͨ��
                        pChannel = pDI->GetChannel();
                        if (NULL == pChannel)
                        {
                            continue;
                        }

                        // ����ǵ���ͨ������ô�ж���ֵ�Ƿ�Ϊtrue
                        // �����˫��ͨ������ô�ж���ֵ�Ƿ�Ϊon
                        GOOSE_DATA_VARIANT stRealtimeValue = pChannel->GetGooseRecvValue();
                        if (CHANNEL_TYPE_GOOSE_POINT == pChannel->GetChannelType())
                        {
                            if (!stRealtimeValue.m_stBoolean.boolVal)
                            {
                                bResult = false;
                                break;
                            }
                        }
                        else if (CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pChannel->GetChannelType())
                        {
                            if (stRealtimeValue.m_stBitString.GetString() != strDPValue[DP_VALUE_ON])
                            {
                                bResult = false;
                                break;
                            }
                        }
                    }

                    // ������б�ӳ��ͨ����ֵ��Ϊon����ô�߼�����������������ѭ��
                    // ��������һ��ʱ����ٴ�ȥ�ж�DIͨ����״̬
                    if (bResult)
                    {
                        break;
                    }
                    else
                    {
                        Sleep(10);
                    }
                }
            }
            else
            {
                while(1)
                {
                    bool                        bResult     = false;
                    CGOOSERecvVirtualChannel*   pDI         = NULL;
                    CChannel*                   pChannel    = NULL;


                    for (int i = 0; i < sizeof(PREDEFINED_DI_INFO)/sizeof(PREDEFINED_DI_INFO[0]); i++)
                    {
                        // ����û��Ƿ���ʱȡ��������
                        if (m_eventCancelStateSequence.Lock(0))
                        {
                            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): User cancels the current operation \n"));
                            eRet = ERROR_CODE_CANCEL;
                            goto ExitHandle;
                        }

                        // �ж�ָ����DI�Ƿ񱻹�ѡ
                        if (!pStatus->IsDIChecked(PREDEFINED_DI_INFO[i].m_strName))
                        {
                            continue;
                        }

                        // �ж�ָ��������ͨ���Ƿ����
                        pDI = pParamConfigGooseRecv->GetVirtualChannelByName(PREDEFINED_DI_INFO[i].m_strName);
                        if (NULL == pDI)
                        {
                            continue;
                        }

                        // �ж�ָ��������ͨ���Ƿ�ӳ������ʵͨ��
                        pChannel = pDI->GetChannel();
                        if (NULL == pChannel)
                        {
                            continue;
                        }

                        // ����ǵ���ͨ������ô�ж���ֵ�Ƿ�Ϊtrue
                        // �����˫��ͨ������ô�ж���ֵ�Ƿ�Ϊon
                        GOOSE_DATA_VARIANT stRealtimeValue = pChannel->GetGooseRecvValue();
                        if (CHANNEL_TYPE_GOOSE_POINT == pChannel->GetChannelType())
                        {
                            if (stRealtimeValue.m_stBoolean.boolVal)
                            {
                                bResult = true;
                                break;
                            }
                        }
                        else if (CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pChannel->GetChannelType())
                        {
                            if (stRealtimeValue.m_stBitString.GetString() == strDPValue[DP_VALUE_ON])
                            {
                                bResult = true;
                                break;
                            }
                        }
                    }

                    // ���ĳ����ӳ��ͨ����ֵΪon����ô�߼�����������������ѭ��
                    // ��������һ��ʱ����ٴ�ȥ�ж�DIͨ����״̬
                    if (bResult)
                    {
                        break;
                    }
                    else
                    {
                        Sleep(10);
                    }
                }
            }
        }
        else                                                            ///< �ֶ��л�
        {
            // ѯ�ʴ����Ƿ��ֶ��л�����һ��״̬
            ::SendMessage(hWnd, WM_WHETHER_SWITCH, 0, 0);
        }

        //
        // ���״̬������һ��״̬
        //
        pStatus = pParamConfigStateSequence->NextStatus(pStatus);
        iStateIndex++;
    }

ExitHandle:
    // ֹͣ����GOOSE����
    pDsmDataLayer->StopRecvData();

    // ֹͣSMV��GOOSE����
    if (__StopSMVSend() && __StopGooseSend())
    {
        // ����״̬���з���״̬��ֹͣ
        __SetCurFunctionType(FUNCTION_TYPE_UNKNOWN);
    }

    return eRet;
}

///
/// @brief
///    ֹͣ״̬���в���
///
bool CFrameSend::StopStateSequenceTest()
{
    MSG     stMsg;


    // ����Ƿ��ѳ�ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StopStateSequenceTest(): uninit \n"));
        return false;
    }

    // ��鵱ǰ�Ƿ����ڽ���״̬���в��ԣ�������ڲ��ԣ�����ȡ����ʶ���ȴ�������ֹ
    if (FUNCTION_TYPE_STATE_SEQUENCE == GetCurFunctionType())
    {
        // ��λȡ����־
        m_eventCancelStateSequence.SetEvent();
        m_eventCancelSMV.SetEvent();

        // һֱ�ȴ���ֱ��״̬���з���״̬Ϊ��ֹͣ
        while(FUNCTION_TYPE_STATE_SEQUENCE == GetCurFunctionType())
        {
            // ���һ����Ϣ�ã��ڵȴ�״̬�����߳̽�����ʱ����Լ����ַ���Ϣ
            while (PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&stMsg);
                DispatchMessage(&stMsg);
            }

            Sleep(10);
        }

        // ��λ��֡��־
        m_eventCancelSMV.ResetEvent();
        m_eventCancelStateSequence.ResetEvent();
    }

    return true;
}

///
/// @brief
///    ���֪ͨ����
///
///
/// @return
///     HWND    ���ھ��
///
HWND CFrameSend::GetNotifyHwnd()
{
    return m_hNotifyHwnd;
}

///
/// @brief
///    ��õ�ǰ���ڷ��͵ڼ���״̬����0��ʼ������
///
int CFrameSend::GetCurStateIndex()
{
    return m_iCurStateIndex;
}

///
/// @brief
///    ���õ�ǰ���ڷ��͵ĵڼ���״̬(��0��ʼ����)
///
/// @param[in]  iNewStateIndex    
///
void CFrameSend::__SetCurStateIndex( int iNewStateIndex )
{
    m_iCurStateIndex = iNewStateIndex;                                  ///< �������ڷ��͵�״̬���к�
}

///
/// @brief
///    ���һ���ֶ�������Խ��������������
///
/// @param[in]  eType    ���Ե�����
///
/// @return
///     CStateTestResult*    CStateTestResult*    �ɹ�ʱ����״̬���Խ�������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::__AddManualTestResult(enumTestType eType)
{
    CStateTestResult* pNewStateTestResult = NULL;


    // newһ��CStateTestResult����
    pNewStateTestResult = new CStateTestResult(eType);
    if (NULL == pNewStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__AddManualTestResult(): new CStateTestResult failed \n"));
        return NULL;
    }

    // ��ʼ��CStateTestResult����
    if (!pNewStateTestResult->Init())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__AddManualTestResult(): CStateTestResult object init failed \n"));
        delete pNewStateTestResult;
        return NULL;
    }

    // ��õ�ǰʱ��
    CEventTime stCurTime;
    if (!GetCurrentFpgaTime(stCurTime))
    {
        return NULL;
    }

    // ��¼�ֶ����鿪ʼʱ��
    if (0 == m_vecManualTestResult.size())
    {
        m_stManualTestStartTime = stCurTime;
    }
    // ����������������������ôɾ�����ϵļ�¼
    else if (m_vecManualTestResult.size() >= MAX_TEST_RESULT_COUNT)
    {
        delete *(m_vecManualTestResult.begin());
        m_vecManualTestResult.erase(m_vecManualTestResult.begin());
    }

    // ����״̬��ʼʱ�䣨���ʱ�䣩
    pNewStateTestResult->SetStartTime(stCurTime - m_stManualTestStartTime);

    // ��Ӷ��󵽽����������
    m_vecManualTestResult.push_back(pNewStateTestResult);
    return pNewStateTestResult;
}

///
/// @brief
///    ɾ��ָ�����ֶ�������Խ������
///
/// @param[in]  pDeleteTestResult    ��ɾ�����ֶ�������Խ������
///
void CFrameSend::__DeleteManualTestResult( CStateTestResult* pDeleteTestResult )
{
    CStateTestResult* pStateTestResult = NULL;
    std::vector<CStateTestResult*>::iterator iter;


    // ����ָ���Ĵ�ɾ������
    iter = m_vecManualTestResult.begin();
    while(m_vecManualTestResult.end() != iter)
    {
        pStateTestResult = *iter;
        if ((NULL != pStateTestResult) && (pStateTestResult == pDeleteTestResult))
        {
            m_vecManualTestResult.erase(iter);
            delete pStateTestResult;
            break;
        }

        iter++;
    }
}

///
/// @brief
///    ����ֶ�������������
///
/// @return
///     unsigned int    �ֶ�������������
///
unsigned int CFrameSend::GetManualTestResultCount()
{
    return m_vecManualTestResult.size();
}

///
/// @brief
///    ���ָ�������ŵ��ֶ�������������
///
/// @param[in]  nIndex    �����������(��0��ʼ����)
///
/// @return
///     CStateTestResult*    ���ָ��״̬���Խ���Ķ���ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::GetManualTestResult( unsigned int nIndex )
{
    // ����������
    if (nIndex >= m_vecManualTestResult.size())
    {
        return NULL;
    }

    return m_vecManualTestResult[nIndex];
}

///
/// @brief
///    ���ָ���ֶ��������������������
///
/// @param[in]  pCurResult    �ֶ�������������
///
/// @return
///     int    ����������ţ���0��ʼ������ʧ��ʱ����-1
///
int CFrameSend::GetManualTestResultIndex( CStateTestResult* pCurResult )
{
    int                                         nIndex;
    std::vector<CStateTestResult*>::iterator    iter;


    // ����������
    if (NULL == pCurResult)
    {
        return (-1);
    }

    // ����ָ���Ľ��������
    for (nIndex = 0, iter = m_vecManualTestResult.begin(); m_vecManualTestResult.end() != iter; nIndex++, iter++)
    {
        CStateTestResult* pResult = *iter;
        if (NULL != pResult)
        {
            if (pResult == pCurResult)
            {
                return nIndex;
            }
        }
    }

    return (-1);
}

///
/// @brief
///    ��õ�һ���ֶ�������������
///
/// @return
///     CStateTestResult*    ָ�����������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::FirstManualTestResult()
{
    std::vector<CStateTestResult*>::iterator iter;

    iter = m_vecManualTestResult.begin();
    if (m_vecManualTestResult.end() != iter)
    {
        return (*iter);
    }

    return NULL;
}

///
/// @brief
///     �����һ���ֶ�������������
///
/// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
///
/// @return
///     CStateTestResult*   ָ����һ������������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::NextManualTestResult( CStateTestResult* pCurTestResult )
{
    std::vector<CStateTestResult*>::iterator    iter;


    iter = m_vecManualTestResult.begin();
    while (m_vecManualTestResult.end() != iter)
    {
        // ���ҵ�ǰ����������λ��
        if (*iter == pCurTestResult)
        {
            // �����һ������������λ��
            if (m_vecManualTestResult.end() != ++iter)
            {
                return (*iter);
            }

            break;
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    �����һ���ֶ�������������
///
/// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
///
/// @return
///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::PrevManualTestResult( CStateTestResult* pCurTestResult )
{
    std::vector<CStateTestResult*>::reverse_iterator  rIter;


    rIter = m_vecManualTestResult.rbegin();
    while (m_vecManualTestResult.rend() != rIter)
    {
        // ���ҵ�ǰ����������λ��
        if (*rIter == pCurTestResult)
        {
            // �����һ������������λ��
            if (m_vecManualTestResult.rend() != ++rIter)
            {
                return (*rIter);
            }

            break;
        }

        rIter++;
    }

    return NULL;
}

///
/// @brief
///     ������һ���ֶ�������������
///
/// @return
///     CStateTestResult*    ָ�����������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::LastManualTestResult()
{
    CStateTestResult* pStateTestResult = NULL;


    // ����̻߳���������Ϊ�����߳̿����ڸ������ݵ�ʱ����ͼȥ������һ�����������
    m_criticalFunctionType.Lock();

    // ������һ�����������
    std::vector<CStateTestResult*>::reverse_iterator rIter = m_vecManualTestResult.rbegin();
    if (m_vecManualTestResult.rend() != rIter)
    {
        pStateTestResult = *rIter;
    }

    // �ͷ��̻߳�����
    m_criticalFunctionType.Unlock();

    return pStateTestResult;
}

///
/// @brief
///    ������е��ֶ���������
///
void CFrameSend::ClearAllManualTestResult()
{
    CStateTestResult* pStateTestResult = NULL;
    std::vector<CStateTestResult*>::iterator iter;


    // �ͷŲ��Խ������
    iter = m_vecManualTestResult.begin();
    while(m_vecManualTestResult.end() != iter)
    {
        pStateTestResult = *iter;
        if (pStateTestResult)
        {
            delete pStateTestResult;
        }

        iter++;
    }

    // �������
    m_vecManualTestResult.clear();
}

///
/// @brief
///    ��ʼ�ֶ�����SMV����
///
/// @param[in]  hWnd    hWnd  ���ھ������DIֵ�����仯ʱ���ᷢ��WM_DI_CHANGED��Ϣ���˴���
///
/// @return
///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSend::StartManualTestSMV( HWND hWnd )
{
    // ����Ƿ��ѳ�ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestSMV(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // �жϴ��ھ���Ƿ���һ�����ڵĴ���
    if (!IsWindow(hWnd))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestSMV(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }
    m_hNotifyHwnd = hWnd;

    // ����̻߳�����
    m_criticalFunctionType.Lock();

    // �����ǰ���ڲ���״̬���У���ôֱ�ӷ���
    if (FUNCTION_TYPE_STATE_SEQUENCE == m_eCurFunctionType)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestSMV(): Currently is testing state sequence \n"));
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }

    // ��鵱ǰ�Ƿ����ڷ���SMV
    if (SEND_STATUS_STOPPED != m_eManualTestSMVSendStatus)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestSMV(): Currently is transmitting data \n"));
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }

    // �����ǰû�п����κβ���
    if (FUNCTION_TYPE_UNKNOWN == m_eCurFunctionType)
    {
        // �����ǰ�Ĳ��Խ��
        ClearAllManualTestResult();

        // ���õ�ǰ״̬��Ϊ-1
        m_iCurStateIndex = -1;
    }

    // ��Ӵ��״̬���Խ���Ķ���
    CStateTestResult* pStateTestResult = __AddManualTestResult(TEST_TYPE_SMV);
    if (NULL == pStateTestResult)
    {
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }

    // �����ֶ������SMV��������ͨ����ֵ
    if (!pStateTestResult->__SaveManualTestSMVSendValues())
    {
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }

    // ��λ״̬�ı���
    CParamConfigSMVSend::getInstance()->SetStatusWordChanged(false);
    CParamConfigSMVSend::getInstance()->SetQualityChanged(false);

    // ���õ�ǰ���ڷ��͵ڼ���״̬
    m_iCurStateIndex = m_iCurStateIndex + 1;

    // ���÷��͹�����״̬Ϊ������׼������֡
    __SetManualTestSMVSendingStatus(SEND_STATUS_PREPARE);
    // ����Ϊ���ڽ����ֶ��������
    __SetCurFunctionType(FUNCTION_TYPE_MANUAL_TEST);

    // �ͷ��̻߳�����
    m_criticalFunctionType.Unlock();

    //
    // �ϳɷ���֡��Ȼ��ʼ����
    //
    enumErrorCode eRet = __SendSMVCbs(FUNCTION_TYPE_MANUAL_TEST, pStateTestResult);
    if (ERROR_CODE_SUCCESS == eRet)
    {
        // ���ò�����ɱ�ʶ
        pStateTestResult->SetTested(true);

        // ����GOOSE�����߳�
        CDsmDataLayer* pDsmDataLayer = CDsmDataLayer::getInstance();
        if (pDsmDataLayer)
        {
            pDsmDataLayer->StartRecvData(NET_RECV_TYPE_GOOSE, DSM_MODE_GOOSE_RECV_DI);
        }

        // ���÷��͹�����״̬Ϊ�����ڷ���
        __SetManualTestSMVSendingStatus(SEND_STATUS_SENDING);
    }
    else
    {
        // ���GOOSE����Ҳ����ֹͣ״̬����ô�����ֶ��������״̬Ϊ��ֹͣ
        m_criticalFunctionType.Lock();
        if (SEND_STATUS_STOPPED == GetManualTestGooseSendStatus())
        {
            // ����Ϊ��ֹͣ�ֶ��������
            __SetCurFunctionType(FUNCTION_TYPE_UNKNOWN);
        }
        m_criticalFunctionType.Unlock();

        // ���÷��͹�����״̬Ϊ����ֹͣ
        __SetManualTestSMVSendingStatus(SEND_STATUS_STOPPED);
    }

    return eRet;
}

///
/// @brief
///    �����ֶ�����SMV���Ե�����
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSend::UpateManualTestSMV()
{
    bool                bRet                = true;
    CStateTestResult*   pOldStateTestResult = NULL;


    // ����Ƿ��ѳ�ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::UpateManualTestSMV(): uninit \n"));
        return false;
    }

    // ����̻߳�����
    m_criticalFunctionType.Lock();

    // ��鵱ǰ�Ƿ����ڷ���SMV����
    if (SEND_STATUS_SENDING != GetManualTestSMVSendStatus())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::UpateManualTestSMV(): Has not yet begun to send data \n"));
        m_criticalFunctionType.Unlock();
        return false;
    }

    // ��Ӵ��״̬���Խ���Ķ���
    CStateTestResult* pNewStateTestResult = __AddManualTestResult(TEST_TYPE_SMV);
    if (NULL == pNewStateTestResult)
    {
        m_criticalFunctionType.Unlock();
        return false;
    }

    // �����ֶ������SMV��������ͨ����ֵ
    if (!pNewStateTestResult->__SaveManualTestSMVSendValues())
    {
        m_criticalFunctionType.Unlock();
        return false;
    }

    // ����״̬�֡�Ʒ�ʱ�λ���
    pNewStateTestResult->SetStatusWordChanged(CParamConfigSMVSend::getInstance()->IsStatusWordChanged());
    pNewStateTestResult->SetQualityChanged(CParamConfigSMVSend::getInstance()->IsQualityChanged());
    CParamConfigSMVSend::getInstance()->SetStatusWordChanged(false);
    CParamConfigSMVSend::getInstance()->SetQualityChanged(false);

    // ������һ��������SMV����ͨ����Ϣ�Ľ��������
    pNewStateTestResult->SetSmvChannelChanged(true);
    pOldStateTestResult = PrevManualTestSMVResult(pNewStateTestResult);
    if (pOldStateTestResult)
    {
        // ���SMV����ֵ�Ƿ����˱仯
        if (pNewStateTestResult->__CompareSMVSendValues(TYPE_FIND_ALL_DIFFERENT, pOldStateTestResult))
        {
            pNewStateTestResult->SetSmvChannelChanged(false);
        }
    }

    // ���ͨ��ֵ��״̬�֡�Ʒ���Ƿ����˱仯
    if (!pNewStateTestResult->IsSmvChannelChanged() &&
        !pNewStateTestResult->IsStatusWordChanged() &&
        !pNewStateTestResult->IsQualityChanged())
    {
        // ���û���κα仯����ôȡ����������
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::UpateManualTestSMV(): No change, the transmission is canceled \n"));

        // ɾ��������ӵ��Ǹ��������
        __DeleteManualTestResult(pNewStateTestResult);
        m_criticalFunctionType.Unlock();
        return false;
    }

    // ���õ�ǰ���ڷ��͵ڼ���״̬
    m_iCurStateIndex = m_iCurStateIndex + 1;

    // ���÷��͹�����״̬Ϊ������׼������֡
    __SetManualTestSMVSendingStatus(SEND_STATUS_PREPARE);

    // �ͷ��̻߳�����
    m_criticalFunctionType.Unlock();

    // ����SMV����֡
    bRet = __UpdateSMVSend(FUNCTION_TYPE_MANUAL_TEST, pNewStateTestResult);

    // ���ò�����ɱ�ʶ
    pNewStateTestResult->SetTested(bRet);

    // ���÷��͹�����״̬Ϊ�����ڷ���
    __SetManualTestSMVSendingStatus(SEND_STATUS_SENDING);
    return bRet;
}

///
/// @brief
///    ֹͣ�ֶ�����SMV����
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSend::StopManualTestSMV()
{
    // ����Ƿ��ѳ�ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StopManualTestSMV(): uninit \n"));
        return false;
    }

    // ��鵱ǰ�Ƿ�������֡�����������֡������ȡ����֡��ʶ���ȴ���֡����
    m_criticalFunctionType.Lock();
    if (SEND_STATUS_PREPARE == GetManualTestSMVSendStatus())
    {
        // ��λȡ����֡��־
        m_eventCancelSMV.SetEvent();

        // һֱ�ȴ���ֱ��SMV�ķ������Ͳ�Ϊ��׼������
        while(SEND_STATUS_PREPARE == GetManualTestSMVSendStatus())
        {
            Sleep(10);
        }

        // ��λȡ����֡��־
        m_eventCancelSMV.ResetEvent();
    }
    m_criticalFunctionType.Unlock();

    //
    // ֹͣSMV����
    //
    if (__StopSMVSend())
    {
        // ���GOOSE����Ҳ����ֹͣ״̬����ôֹͣGOOSE�����߳�
        m_criticalFunctionType.Lock();
        if (SEND_STATUS_STOPPED == GetManualTestGooseSendStatus())
        {
            CDsmDataLayer* pDsmDataLayer = CDsmDataLayer::getInstance();
            if (pDsmDataLayer)
            {
                pDsmDataLayer->StopRecvData();
            }

            // �����ֶ��������״̬Ϊֹͣ
            __SetCurFunctionType(FUNCTION_TYPE_UNKNOWN);
        }
        m_criticalFunctionType.Unlock();

        // �����ֶ�����SMV����״̬Ϊ��ֹͣ����
        __SetManualTestSMVSendingStatus(SEND_STATUS_STOPPED);
        return true;
    }
    else
    {
        return false;
    }
}

///
/// @brief
///    ��ʼ�ֶ�����GOOSE����
///
/// @param[in]  hWnd    hWnd  ���ھ������DIֵ�����仯ʱ���ᷢ��WM_DI_CHANGED��Ϣ���˴���
///
/// @return
///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSend::StartManualTestGoose( HWND hWnd )
{
    // ����Ƿ��ѳ�ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestGoose(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // �жϴ��ھ���Ƿ���һ�����ڵĴ���
    if (!IsWindow(hWnd))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestGoose(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }
    m_hNotifyHwnd = hWnd;

    // ����̻߳�����
    m_criticalFunctionType.Lock();

    // �����ǰ���ڲ���״̬���У���ôֱ�ӷ���
    if (FUNCTION_TYPE_STATE_SEQUENCE == GetCurFunctionType())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestGoose(): Currently is testing state sequence \n"));
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }

    // ��鵱ǰ�Ƿ����ڷ���GOOSE
    if (SEND_STATUS_STOPPED != GetManualTestGooseSendStatus())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestGoose(): Currently is transmitting data \n"));
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }

    // �����ǰ��û�п�ʼ�ֶ�������ԣ���ô�����ǰ�����в��Խ��
    if (FUNCTION_TYPE_UNKNOWN == GetCurFunctionType())
    {
        // �����ǰ�Ĳ��Խ��
        ClearAllManualTestResult();

        // ���õ�ǰ״̬��Ϊ-1
        m_iCurStateIndex = -1;
    }

    // ��Ӵ��״̬���Խ���Ķ���
    CStateTestResult* pStateTestResult = __AddManualTestResult(TEST_TYPE_GOOSE);
    if (NULL == pStateTestResult)
    {
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }

    // �����ֶ������GOOSE���Ϳ��ƿ��ֵ
    if (!pStateTestResult->__SaveGooseSendCbs())
    {
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }
    // ���õ�ǰ���ڷ��͵ڼ���״̬
    m_iCurStateIndex = m_iCurStateIndex + 1;

    // �����ֶ�����GOOSE����״̬Ϊ������׼������֡
    __SetManualTestGooseSendStatus(SEND_STATUS_PREPARE);
    // ����Ϊ���ڽ����ֶ��������
    __SetCurFunctionType(FUNCTION_TYPE_MANUAL_TEST);

    // �ͷ��̻߳�����
    m_criticalFunctionType.Unlock();

    //
    // �ϳɷ���֡��Ȼ��ʼ����
    //
    enumErrorCode eRet = __SendGooseCbs(FUNCTION_TYPE_MANUAL_TEST, pStateTestResult);
    if (ERROR_CODE_SUCCESS == eRet)
    {
        // ���ò�����ɱ�ʶ
        pStateTestResult->SetTested(true);

        // ����GOOSE�����߳�
        CDsmDataLayer* pDsmDataLayer = CDsmDataLayer::getInstance();
        if (pDsmDataLayer)
        {
            pDsmDataLayer->StartRecvData(NET_RECV_TYPE_GOOSE, DSM_MODE_GOOSE_RECV_DI);
        }

        // �����ֶ�����GOOSE����״̬Ϊ�����ڷ���
        __SetManualTestGooseSendStatus(SEND_STATUS_SENDING);
    }
    else
    {
        // ���SMV����Ҳ����ֹͣ״̬����ô�����ֶ��������״̬Ϊ��ֹͣ
        m_criticalFunctionType.Lock();
        if (SEND_STATUS_STOPPED == GetManualTestSMVSendStatus())
        {
            // ����Ϊ��ֹͣ�ֶ��������
            __SetCurFunctionType(FUNCTION_TYPE_UNKNOWN);
        }
        m_criticalFunctionType.Unlock();

        // �����ֶ�����GOOSE����״̬Ϊ����ֹͣ
        __SetManualTestGooseSendStatus(SEND_STATUS_STOPPED);
    }

    return eRet;
}

///
/// @brief
///    �����ֶ�����GOOSE���Ե�����
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSend::UpateManualTestGoose()
{
    bool                bRet                = true;
    CStateTestResult*   pOldStateTestResult = NULL;


    // ����Ƿ��ѳ�ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::UpateManualTestGoose(): uninit \n"));
        return false;
    }

    // ����̻߳�����
    m_criticalFunctionType.Lock();

    // ��鵱ǰ�Ƿ����ڷ���GOOSE����
    if (SEND_STATUS_SENDING != GetManualTestGooseSendStatus())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::UpateManualTestGoose(): Has not yet begun to send data \n"));
        m_criticalFunctionType.Unlock();
        return false;
    }

    // ��Ӵ��״̬���Խ���Ķ���
    CStateTestResult* pNewStateTestResult = __AddManualTestResult(TEST_TYPE_GOOSE);
    if (NULL == pNewStateTestResult)
    {
        m_criticalFunctionType.Unlock();
        return false;
    }

    // �����ֶ������GOOSE���Ϳ��ƿ��ֵ
    if (!pNewStateTestResult->__SaveGooseSendCbs())
    {
        m_criticalFunctionType.Unlock();
        return false;
    }

    // ������һ��������GOOSE���ƿ���Ϣ�Ľ��������
    pNewStateTestResult->SetGooseChannelChanged(true);
    pOldStateTestResult = PrevManualTestGooseResult(pNewStateTestResult);
    if (pOldStateTestResult)
    {
        // ���û���κα仯����ôȡ����������
        if (pNewStateTestResult->__CompareGooseSendValues(pOldStateTestResult))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::UpateManualTestGoose(): No change, the transmission is canceled \n"));

            // ɾ��������ӵ��Ǹ��������
            __DeleteManualTestResult(pNewStateTestResult);
            m_criticalFunctionType.Unlock();
            return false;
        }

        // �޸�GOOSE���ƿ��е���/˫������ͨ��ʱ����Ҫ�Զ��޸�����/�����ʱ��ͨ����ֵ
        pNewStateTestResult->__UpdateGooseChannelTime(m_stManualTestStartTime);
    }
    // ���õ�ǰ���ڷ��͵ڼ���״̬
    m_iCurStateIndex = m_iCurStateIndex + 1;

    // �����ֶ�����GOOSE����״̬Ϊ������׼������֡
    __SetManualTestGooseSendStatus(SEND_STATUS_PREPARE);
    m_criticalFunctionType.Unlock();

    // ����GOOSE����֡
    bRet = __UpdateGooseSend(FUNCTION_TYPE_MANUAL_TEST, pNewStateTestResult);

    // ���ò�����ɱ�ʶ
    pNewStateTestResult->SetTested(bRet);

    // �����ֶ�����GOOSE����״̬Ϊ�����ڷ���
    __SetManualTestGooseSendStatus(SEND_STATUS_SENDING);
    return bRet;
}

///
/// @brief
///    ֹͣ�ֶ�����GOOSE����
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSend::StopManualTestGoose()
{
    // ����Ƿ��ѳ�ʼ��
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StopManualTestGoose(): uninit \n"));
        return false;
    }

    // ��鵱ǰ�Ƿ�������֡�����������֡������ȡ����֡��ʶ���ȴ���֡����
    m_criticalFunctionType.Lock();
    if (SEND_STATUS_PREPARE == GetManualTestGooseSendStatus())
    {
        // ��λȡ����֡��־
        m_eventCancelGoose.SetEvent();

        // һֱ�ȴ���ֱ��Goose�ķ������Ͳ�Ϊ��׼������
        while(SEND_STATUS_PREPARE == GetManualTestGooseSendStatus())
        {
            Sleep(10);
        }

        // ��λȡ����֡��־
        m_eventCancelGoose.ResetEvent();
    }
    m_criticalFunctionType.Unlock();

    //
    // ֹͣGOOSE����
    //
    if (__StopGooseSend())
    {
        // ���SMV����Ҳ����ֹͣ״̬����ôֹͣGOOSE�����߳�
        m_criticalFunctionType.Lock();
        if (SEND_STATUS_STOPPED == GetManualTestSMVSendStatus())
        {
            CDsmDataLayer* pDsmDataLayer = CDsmDataLayer::getInstance();
            if (pDsmDataLayer)
            {
                pDsmDataLayer->StopRecvData();
            }

            // �����ֶ��������״̬Ϊ��ֹͣ
            __SetCurFunctionType(FUNCTION_TYPE_UNKNOWN);
        }
        m_criticalFunctionType.Unlock();

        // �����ֶ�����GOOSE����״̬Ϊ��ֹͣ����
        __SetManualTestGooseSendStatus(SEND_STATUS_STOPPED);
        return true;
    }
    else
    {
        return false;
    }
}

///
/// @brief
///     ����ֶ�ʵ��Ŀ�ʼʱ��
///
/// @return
///     CEventTime    ��ʼʱ��
///
CEventTime CFrameSend::GetManualTestStartTime()
{
    return m_stManualTestStartTime;
}

///
/// @brief
///     ���״̬���в��ԵĿ�ʼʱ��
///
/// @return
///     CEventTime    ��ʼʱ��
///
CEventTime CFrameSend::GetStateSequenceStartTime()
{
    return m_stStateSequenceStartTime;
}

///
/// @brief
///    ��õ�һ������SMV����ͨ����Ϣ���ֶ�������������
///
/// @return
///     CStateTestResult*    ָ�����������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::FirstManualTestSMVResult()
{
    std::vector<CStateTestResult*>::iterator    iter;
    CStateTestResult*                           pTestResult = NULL;


    // ������������������ҵ�һ������SMV����ͨ����Ϣ�Ľ��������
    iter = m_vecManualTestResult.begin();
    while(m_vecManualTestResult.end() != iter)
    {
        pTestResult = *iter;
        if ((NULL != pTestResult) && (pTestResult->GetTestType() == TEST_TYPE_SMV))
        {
            break;
        }

        iter++;
    }

    return pTestResult;
}

///
/// @brief
///    �����һ������SMV����ͨ����Ϣ���ֶ�������������
///
/// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
///
/// @return
///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::NextManualTestSMVResult( CStateTestResult* pCurTestResult )
{
    std::vector<CStateTestResult*>::iterator    iter;
    CStateTestResult*                           pNextTestResult = NULL;


    iter = m_vecManualTestResult.begin();
    while (m_vecManualTestResult.end() != iter)
    {
        // ���ҵ�ǰ����������λ��
        if (*iter == pCurTestResult)
        {
            // �����һ������������λ��
            while(m_vecManualTestResult.end() != ++iter)
            {
                pNextTestResult = *iter;
                if ((NULL != pNextTestResult) && (pNextTestResult->GetTestType() == TEST_TYPE_SMV))
                {
                    return pNextTestResult;
                }
            }

            break;
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    �����һ������SMV����ͨ����Ϣ���ֶ�������������
///
/// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
///
/// @return
///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::PrevManualTestSMVResult( CStateTestResult* pCurTestResult )
{
    std::vector<CStateTestResult*>::reverse_iterator    rIter;
    CStateTestResult*                                   pPrevTestResult = NULL;


    rIter = m_vecManualTestResult.rbegin();
    while (m_vecManualTestResult.rend() != rIter)
    {
        // ���ҵ�ǰ����������λ��
        if (*rIter == pCurTestResult)
        {
            // �����һ������������λ��
            while(m_vecManualTestResult.rend() != ++rIter)
            {
                pPrevTestResult = *rIter;
                if ((NULL != pPrevTestResult) && (pPrevTestResult->GetTestType() == TEST_TYPE_SMV))
                {
                    return pPrevTestResult;
                }
            }

            break;
        }

        rIter++;
    }

    return NULL;
}

///
/// @brief
///    ������һ������SMV����ͨ����Ϣ���ֶ�������������
///
/// @return
///     CStateTestResult*    ָ�����������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::LastManualTestSMVResult()
{
    std::vector<CStateTestResult*>::reverse_iterator    rIter;
    CStateTestResult*                                   pTestResult = NULL;


    // ����������������������һ������SMV����ͨ����Ϣ�Ľ��������
    rIter = m_vecManualTestResult.rbegin();
    while(m_vecManualTestResult.rend() != rIter)
    {
        pTestResult = *rIter;
        if ((NULL != pTestResult) && (pTestResult->GetTestType() == TEST_TYPE_SMV))
        {
            break;
        }

        rIter++;
    }

    return pTestResult;
}

///
/// @brief
///    ��õ�һ������GOOSE���ƿ���ֶ�������������
///
/// @return
///     CStateTestResult*    ָ�����������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::FirstManualTestGooseResult()
{
    std::vector<CStateTestResult*>::iterator    iter;
    CStateTestResult*                           pTestResult = NULL;


    // ������������������ҵ�һ������SMV����ͨ����Ϣ�Ľ��������
    iter = m_vecManualTestResult.begin();
    while(m_vecManualTestResult.end() != iter)
    {
        pTestResult = *iter;
        if ((NULL != pTestResult) && (pTestResult->GetTestType() == TEST_TYPE_GOOSE))
        {
            break;
        }

        iter++;
    }

    return pTestResult;
}

///
/// @brief
///    �����һ������GOOSE���ƿ���ֶ�������������
///
/// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
///
/// @return
///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::NextManualTestGooseResult( CStateTestResult* pCurTestResult )
{
    std::vector<CStateTestResult*>::iterator    iter;
    CStateTestResult*                           pNextTestResult = NULL;


    iter = m_vecManualTestResult.begin();
    while (m_vecManualTestResult.end() != iter)
    {
        // ���ҵ�ǰ����������λ��
        if (*iter == pCurTestResult)
        {
            // �����һ������������λ��
            while(m_vecManualTestResult.end() != ++iter)
            {
                pNextTestResult = *iter;
                if ((NULL != pNextTestResult) && (pNextTestResult->GetTestType() == TEST_TYPE_GOOSE))
                {
                    return pNextTestResult;
                }
            }

            break;
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    �����һ������GOOSE���ƿ���ֶ�������������
///
/// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
///
/// @return
///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::PrevManualTestGooseResult( CStateTestResult* pCurTestResult )
{
    std::vector<CStateTestResult*>::reverse_iterator    rIter;
    CStateTestResult*                                   pPrevTestResult = NULL;


    rIter = m_vecManualTestResult.rbegin();
    while (m_vecManualTestResult.rend() != rIter)
    {
        // ���ҵ�ǰ����������λ��
        if (*rIter == pCurTestResult)
        {
            // �����һ������������λ��
            while(m_vecManualTestResult.rend() != ++rIter)
            {
                pPrevTestResult = *rIter;
                if ((NULL != pPrevTestResult) && (pPrevTestResult->GetTestType() == TEST_TYPE_GOOSE))
                {
                    return pPrevTestResult;
                }
            }

            break;
        }

        rIter++;
    }

    return NULL;
}

///
/// @brief
///    ������һ������GOOSE���ƿ���ֶ�������������
///
/// @return
///     CStateTestResult*    ָ�����������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::LastManualTestGooseResult()
{
    std::vector<CStateTestResult*>::reverse_iterator    rIter;
    CStateTestResult*                                   pTestResult = NULL;


    // ����������������������һ������SMV����ͨ����Ϣ�Ľ��������
    rIter = m_vecManualTestResult.rbegin();
    while(m_vecManualTestResult.rend() != rIter)
    {
        pTestResult = *rIter;
        if ((NULL != pTestResult) && (pTestResult->GetTestType() == TEST_TYPE_GOOSE))
        {
            break;
        }

        rIter++;
    }

    return pTestResult;
}

///
/// @brief
///    ��õ�ǰ��FPGAʱ��
///
/// @param[out]  stCurFpgaTime    ���FPGAʱ��Ķ���
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSend::GetCurrentFpgaTime(CEventTime& stCurFpgaTime)
{
    //  ��õ�ǰFPGAʱ��
    UTC_TIME_t stUtcTime;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_GET_UTC_TIME, NULL, NULL, &stUtcTime, sizeof(stUtcTime), NULL ,NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::GetCurrentFpgaTime(): IOCTRL_GET_UTC_TIME failed \n"));
        return false;
    }

    // �ϳɵ�ǰFPGAʱ��
    stCurFpgaTime.ts_s  = stUtcTime.uSecondSinceEpoch;
    stCurFpgaTime.ts_ns = stUtcTime.uFractionOfSecond * 1000;
    stCurFpgaTime.ts_q  = 0;

    return true;
}

///
/// @brief
///    ����SMV���ƿ鵽�������Ͷ˿�
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSend::__DistributeSmvCbs()
{
    // ��ո�������˿ڵĴ����Ϳ��ƿ�����
    m_pFrameNetA->RemoveAllSMVSendCbs();
    m_pFrameNetB->RemoveAllSMVSendCbs();
    m_pFrameNetC->RemoveAllSMVSendCbs();
    m_pFrameFt3C->SetSMVSendCb(NULL);

    // ���SMV���Ͳ������ù�����
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__DistributeSmvCbs(): Get CParamConfigSMVSend object failed \n"));
        return false;
    }

    //
    // ��SMV���ƿ���ӵ���������˿�
    //
    enumFrameType eFrameType = pParamConfigSMVSend->GetSMVType();
    switch (eFrameType)
    {
    case FRAME_TYPE_NET_9_1:                                            ///< IEC61850-9-1
        {
            // ����IEC61850-9-1���ƿ�
            CIEC9_1Cb* pIEC9_1Cb = pParamConfigSMVSend->FirstIEC9_1Cb();
            while(pIEC9_1Cb)
            {
                // ����˿��ƿ鱻��ѡ
                if (pIEC9_1Cb->IsSelected() && (pIEC9_1Cb->GetChannelCount() > 0))
                {
                    // ��ÿ��ƿ�ķ��Ͷ˿�
                    DsmPortName ePortName = pIEC9_1Cb->GetPortName();
                    switch(ePortName)
                    {
                    case NET_PORT_LIGHT_A:                              ///< ��̫�����A
                        {
                            m_pFrameNetA->AddSMVSendCb(pIEC9_1Cb);
                        }
                        break;

                    case NET_PORT_LIGHT_B:                              ///< ��̫�����B
                        {
                            m_pFrameNetB->AddSMVSendCb(pIEC9_1Cb);
                        }
                        break;

                    case NET_PORT_ELECTRONIC:                           ///< ����̫����
                        {
                            m_pFrameNetC->AddSMVSendCb(pIEC9_1Cb);
                        }
                        break;

                    default:
                        {
                            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__DistributeSmvCbs(): Control block %d send port error \n", pIEC9_1Cb->GetAPPID()));
                            return false;
                        }
                    }
                }

                pIEC9_1Cb = pParamConfigSMVSend->NextIEC9_1Cb(pIEC9_1Cb);
            }
        }
        break;

    case FRAME_TYPE_NET_9_2:                                            ///< IEC61850-9-2
        {
            // ����IEC61850-9-2���ƿ�
            CIEC9_2Cb* pIEC9_2Cb = pParamConfigSMVSend->FirstIEC9_2Cb();
            while(pIEC9_2Cb)
            {
                // ����˿��ƿ鱻��ѡ
                if (pIEC9_2Cb->IsSelected() && (pIEC9_2Cb->GetChannelCount() > 0))
                {
                    // ��ÿ��ƿ�ķ��Ͷ˿�
                    DsmPortName ePortName = pIEC9_2Cb->GetPortName();
                    switch(ePortName)
                    {
                    case NET_PORT_LIGHT_A:                              ///< ��̫�����A
                        {
                            m_pFrameNetA->AddSMVSendCb(pIEC9_2Cb);
                        }
                        break;

                    case NET_PORT_LIGHT_B:                              ///< ��̫�����B
                        {
                            m_pFrameNetB->AddSMVSendCb(pIEC9_2Cb);
                        }
                        break;

                    case NET_PORT_ELECTRONIC:                           ///< ����̫����
                        {
                            m_pFrameNetC->AddSMVSendCb(pIEC9_2Cb);
                        }
                        break;

                    default:
                        {
                            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__DistributeSmvCbs(): Control block %d send port error \n", pIEC9_2Cb->GetAPPID()));
                            return false;
                        }
                    }
                }

                pIEC9_2Cb = pParamConfigSMVSend->NextIEC9_2Cb(pIEC9_2Cb);
            }
        }
        break;

    case FRAME_TYPE_FT3_NANRUI:                                         ///< FT3
        {
            // ����FT3���ƿ�
            CFT3Cb* pFt3Cb = pParamConfigSMVSend->FirstFt3Cb();
            while(pFt3Cb)
            {
                // ����˿��ƿ鱻��ѡ
                if (pFt3Cb->IsSelected() && (pFt3Cb->GetChannelCount() > 0))
                {
                    // ��ÿ��ƿ�ķ��Ͷ˿�
                    DsmPortName ePortName = pFt3Cb->GetPortName();
                    switch(ePortName)
                    {
                    case FT3_PORT_LIGHT_C:                              ///< �⴮��C�����ͣ�
                        {
                            m_pFrameFt3C->SetSMVSendCb(pFt3Cb);
                        }
                        break;

                    default:
                        {
                            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__DistributeSmvCbs(): Control block %d send port error \n", pFt3Cb->GetAPPID()));
                            return false;
                        }
                    }
                }

                pFt3Cb = pParamConfigSMVSend->NextFt3Cb(pFt3Cb);
            }
        }
        break;

    case FRAME_TYPE_FT3_GUOWANG:                                        ///< FT3��չ
        {
            // ����FT3��չ���ƿ�
            CFT3ExpandCb* pFt3ExpandCb = pParamConfigSMVSend->FirstFt3ExpandCb();
            while(pFt3ExpandCb)
            {
                // ����˿��ƿ鱻��ѡ
                if (pFt3ExpandCb->IsSelected() && (pFt3ExpandCb->GetChannelCount() > 0))
                {
                    // ��ÿ��ƿ�ķ��Ͷ˿�
                    DsmPortName ePortName = pFt3ExpandCb->GetPortName();
                    switch(ePortName)
                    {
                    case FT3_PORT_LIGHT_C:                              ///< �⴮��C�����ͣ�
                        {
                            m_pFrameFt3C->SetSMVSendCb(pFt3ExpandCb);
                        }
                        break;

                    default:
                        {
                            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__DistributeSmvCbs(): Control block %d send port error \n", pFt3ExpandCb->GetAPPID()));
                            return false;
                        }
                    }
                }

                pFt3ExpandCb = pParamConfigSMVSend->NextFt3ExpandCb(pFt3ExpandCb);
            }
        }
        break;

    default:
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__DistributeSmvCbs(): SMV type error \n"));
            return false;
        }
    }

    return true;
}

///
/// @brief
///    ����GOOSE���ƿ鵽�������Ͷ˿�
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSend::__DistributeGooseCbs()
{
    // ��ո�������˿ڵĴ����Ϳ��ƿ�����
    m_pFrameNetA->RemoveAllGOOSESendCbs();
    m_pFrameNetB->RemoveAllGOOSESendCbs();
    m_pFrameNetC->RemoveAllGOOSESendCbs();

    // ���SMV���Ͳ������ù�����
    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__DistributeGooseCbs(): Get CParamConfigGOOSESend object failed \n"));
        return false;
    }

    // �������ƿ�
    CGooseCb* pGooseCb = pParamConfigGooseSend->FirstGocb();
    while(pGooseCb)
    {
        // ����˿��ƿ鱻��ѡ
        if (pGooseCb->IsSelected() && (pGooseCb->GetChannelCount() > 0))
        {
            // ��ÿ��ƿ�ķ��Ͷ˿�
            DsmPortName ePortName = pGooseCb->GetPortName();
            switch(ePortName)
            {
            case NET_PORT_LIGHT_A:                                      ///< ��̫�����A
                {
                    m_pFrameNetA->AddGooseSendCb(pGooseCb);
                }
                break;

            case NET_PORT_LIGHT_B:                                      ///< ��̫�����B
                {
                    m_pFrameNetB->AddGooseSendCb(pGooseCb);
                }
                break;

            case NET_PORT_ELECTRONIC:                                   ///< ����̫����
                {
                    m_pFrameNetC->AddGooseSendCb(pGooseCb);
                }
                break;

            default:
                {
                    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__DistributeGooseCbs(): Control block %d send port error \n", pGooseCb->GetAPPID()));
                    return false;
                }
            }
        }

        pGooseCb = pParamConfigGooseSend->NextGocb(pGooseCb);
    }

    return true;
}

///
/// @brief
///    ����GOOSE���ƿ鵽�������Ͷ˿�
///
/// @param[in]  pStateTestResult    ָ�����������ָ�룬�������˴����͵�GOOSE���ƿ�
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSend::__DistributeGooseCbs( CStateTestResult* pStateTestResult )
{
    // ����������
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__DistributeGooseCbs(): Parameter error \n"));
        return false;
    }

    // ��ո�������˿ڵĴ����Ϳ��ƿ�����
    m_pFrameNetA->RemoveAllGOOSESendCbs();
    m_pFrameNetB->RemoveAllGOOSESendCbs();
    m_pFrameNetC->RemoveAllGOOSESendCbs();

    // �������ƿ�
    CGooseCb* pGooseCb = pStateTestResult->FirstGooseSendCb();
    while(pGooseCb)
    {
        // ����˿��ƿ鱻��ѡ
        if (pGooseCb->IsSelected() && (pGooseCb->GetChannelCount() > 0))
        {
            // ��ÿ��ƿ�ķ��Ͷ˿�
            DsmPortName ePortName = pGooseCb->GetPortName();
            switch(ePortName)
            {
            case NET_PORT_LIGHT_A:                                      ///< ��̫�����A
                {
                    m_pFrameNetA->AddGooseSendCb(pGooseCb);
                }
                break;

            case NET_PORT_LIGHT_B:                                      ///< ��̫�����B
                {
                    m_pFrameNetB->AddGooseSendCb(pGooseCb);
                }
                break;

            case NET_PORT_ELECTRONIC:                                   ///< ����̫����
                {
                    m_pFrameNetC->AddGooseSendCb(pGooseCb);
                }
                break;

            default:
                {
                    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__DistributeGooseCbs(): Control block %d send port error \n", pGooseCb->GetAPPID()));
                    return false;
                }
            }
        }

        pGooseCb = pStateTestResult->NextGooseSendCb(pGooseCb);
    }

    return true;
}

///
/// @brief
///    ���һ����־��¼
///
/// @param[in]  strText    ������
/// @param[in]  ulTime    ʱ��ֵ
///
void CFrameSend::AddLogs( std::string strText, unsigned long ulTime )
{
    m_listTextTime.push_back(std::make_pair(strText, ulTime));
}

///
/// @brief
///    ˢ�»���������־��¼����־�ļ���
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSend::FlushLogs()
{
    uint32_t    uiCurTime;
    uint32_t    uiPrevTime;
    char    cBuffer[101]    = {0};
    char*   pEndString      = "**********************end**********************";
    std::vector<std::pair<std::string, unsigned long> >::iterator    iter;


    // ����¼��
    if (0 == m_listTextTime.size())
    {
        return true;
    }

    // ����־�ļ�
    FILE* pLogFile = fopen("\\FlashDisk2\\DSTester\\time_log.txt", "a+");
    if (pLogFile == NULL)
    {
        return false;
    }

    // ����־�ļ����������
    iter = m_listTextTime.begin();
    uiPrevTime = (*iter).second;
    for (; iter != m_listTextTime.end(); iter++)
    {
        memset(cBuffer, 0, sizeof(cBuffer)/sizeof(cBuffer[0]));
        uiCurTime = (*iter).second;

        sprintf_s(cBuffer, 100, "%s: time = %d ms, spend %d ms\n", (*iter).first.c_str(), uiCurTime, uiCurTime - uiPrevTime);
        fwrite(cBuffer, strlen(cBuffer), 1, pLogFile);

        uiPrevTime = uiCurTime;
    }
    m_listTextTime.clear();

    // ˢ�¡��ر���־�ļ�
    fwrite(pEndString, strlen(pEndString), 1, pLogFile);
    fflush(pLogFile);
    fclose(pLogFile);
    return true;
}

///
/// @brief
///    ������ƿ�������ͨ����ƫ����
///
/// @param[in]  pStateTestResult    ָ�����������ָ��
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameSend::__CalcChannelOffset( CStateTestResult* pStateTestResult )
{
    std::list<CStateSendBuffer*>::iterator  iter;
    CStateSendBuffer*                       pStateSendBuffer;


    // ����������
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__CalcChannelOffset(): Parameter error \n"));
        return false;
    }

    // ����ÿһ���˿ڵķ��ͻ���
    iter = pStateTestResult->m_listGooseSendBuffers.begin();
    for (; iter != pStateTestResult->m_listGooseSendBuffers.end(); iter++)
    {
        pStateSendBuffer = *iter;
        if (pStateSendBuffer)
        {
            switch(pStateSendBuffer->m_ePortName)
            {
            case NET_PORT_LIGHT_A:
                {
                    if (!m_pFrameNetA->CalcChannelOffset(pStateSendBuffer))
                    {
                        return false;
                    }
                }
                break;

            case NET_PORT_LIGHT_B:
                {
                    if (!m_pFrameNetB->CalcChannelOffset(pStateSendBuffer))
                    {
                        return false;
                    }
                }
                break;

            case NET_PORT_ELECTRONIC:
                {
                    if (!m_pFrameNetC->CalcChannelOffset(pStateSendBuffer))
                    {
                        return false;
                    }
                }
                break;
            }
        }
    }

    return true;
}

///
/// @brief
///     �������Ƶ�SMV����״̬�������ظ�״̬��Ӧ�ķ��ͻ���
///
/// @param[in]  pCurStateTestResult ��ǰSMV����״̬��Ӧ�Ĳ��Խ����
/// @param[in]  iStateIndex         ��ǰSMV����״̬��������
///
/// @return
///     CStateTestResult*   ���Ʒ���״̬�Ĳ��Խ������ʧ��ʱ����NULL
///
CStateTestResult* CFrameSend::__FindSimilarSmvTestResult( CStateTestResult* pCurStateTestResult, int iStateIndex )
{
    int                 i                       = 0;
    bool                bSimilar                = false;
    CStatus*            pOldStatus              = NULL;
    CStateTestResult*   pSimilarStateTestResult = NULL;


    // ���״̬�������ù�����
    CParamConfigStateSequence* pParamConfigStateSequence = CParamConfigStateSequence::getInstance();
    if (NULL == pParamConfigStateSequence)
    {
        return NULL;
    }

    // ������ͬ��SMV����״̬
    pOldStatus  = pParamConfigStateSequence->FirstStatus(false);
    for (; (i < iStateIndex) && (pOldStatus != NULL); i++)
    {
        if (pCurStateTestResult->__GetSMVSendValues()->__CompareSMVSendVirtualChannels(TYPE_FIND_ONE_DIFFERENT, pOldStatus))
        {
            bSimilar = true;
            break;
        }

        pOldStatus = pParamConfigStateSequence->NextStatus(pOldStatus);
    }

    // ���������ͬ��SMV����״̬
    if (bSimilar)
    {
        // ������Ƶ�SMV���ͻ���
        if (i >= (int)m_vecStateSequenceResult.size())
        {
            return NULL;
        }

        pSimilarStateTestResult = m_vecStateSequenceResult.at(i);
    }

    return pSimilarStateTestResult;
}
