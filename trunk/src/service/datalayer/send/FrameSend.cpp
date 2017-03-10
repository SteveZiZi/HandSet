/// @file
///
/// @brief
///     管理与FPGA的交互，完成数据发送操作
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.3.22
///
/// 修改历史：
///     Modified by:    chao
///     Modified date:  2013.6.19
///     Version:        1.0.1
///     Descriptions:   添加了发送功能
///

#include "StdAfx.h"
#include "FrameSend.h"
#include "src\service\datalayer\config\ParamConfigSMVSend.h"
#include "src\service\datalayer\config\ParamConfigGOOSESend.h"
#include "src\service\datalayer\config\ParamconfigManualTest.h"
#include "src\service\datalayer\config\ParamConfigGOOSERecv.h"
#include "src\service\datalayer\config\ParamConfigStateSequence.h"
#include "src\service\datalayer\DsmDataLayer.h"

#define MAX_TEST_RESULT_COUNT           50                              ///< 测试结果集的最大数量

///
/// @brief
///    构造函数
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
///    析构函数
///
CFrameSend::~CFrameSend(void)
{
    // 卸载发送管理器
    UnInitFrameSend();

    // 释放所有的手动试验结果集
    ClearAllManualTestResult();

    // 释放所有的状态序列结果集
    ClearAllStateSequenceResult();
}

///
/// @brief
///    获取报文发送管理器实例
///
CFrameSend* CFrameSend::getInstance()
{
    static CFrameSend s_frameSend;
    return &s_frameSend;
}

///
/// @brief
///    初始化发送管理器
///
///
/// @return
///     bool    成功返回true
///
bool CFrameSend::InitFrameSend(HANDLE hGPMDevice)
{
    // 判断是否已经初始化
    if (m_bIsInited)
    {
        return true;
    }

    // 检查GPMC总线句柄
    if (INVALID_HANDLE_VALUE == hGPMDevice)
    {
        // 驱动句柄为空
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::InitFrameSend(): GPM Device Handle Invalide ! \n"));
        goto exitCode;
    }
    m_hGPMDevice = hGPMDevice;

    // 光串口
    if (NULL == m_pFrameFt3C)
    {
        m_pFrameFt3C = new CFrameSendFt3(this, m_hGPMDevice, FT3_PORT_LIGHT_C);
        if (NULL == m_pFrameFt3C)
        {
            // 创建Ft3光口实例失败
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::InitFrameSend(): Create Ft3 Send on Port C Failed \n"));
            goto exitCode;
        }
        m_pFrameFt3C->InitFt3(FT3_SEND_NOTIFY_EVENT_NAME_C.c_str(), FT3_SEND_MAP_FILE_NAME_C.c_str(), FT3_SEND_MAP_FILE_LEN);
    }

    // 光以太网口A
    if (NULL == m_pFrameNetA)
    {
        m_pFrameNetA = new CFrameSendNet(this, m_hGPMDevice, NET_PORT_LIGHT_A);
        if (NULL == m_pFrameNetA)
        {
            // 创建网口实例失败
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::InitFrameSend(): Create Net Send on Port A Failed \n"));
            goto exitCode;
        }
        m_pFrameNetA->InitNet(NET_SEND_NOTIFY_EVENT_NAME_A.c_str(), NET_SEND_MAP_FILE_NAME_A.c_str(), NET_SEND_MAP_FILE_LEN);
    }

    // 光以太网口B
    if (NULL == m_pFrameNetB)
    {
        m_pFrameNetB = new CFrameSendNet(this, m_hGPMDevice, NET_PORT_LIGHT_B);
        if (NULL == m_pFrameNetB)
        {
            // 创建网口实例失败
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::InitFrameSend(): Create Net Send on Port B Failed \n"));
            goto exitCode;
        }
        m_pFrameNetB->InitNet(NET_SEND_NOTIFY_EVENT_NAME_B.c_str(), NET_SEND_MAP_FILE_NAME_B.c_str(), NET_SEND_MAP_FILE_LEN);
    }

    // 电以太网口
    if (NULL == m_pFrameNetC)
    {
        m_pFrameNetC = new CFrameSendNet(this, m_hGPMDevice, NET_PORT_ELECTRONIC);
        if (NULL == m_pFrameNetC)
        {
            // 创建网口实例失败
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::InitFrameSend(): Create NET Send on Port C Failed \n"));
            goto exitCode;
        }
        m_pFrameNetC->InitNet(NET_SEND_NOTIFY_EVENT_NAME_C.c_str(), NET_SEND_MAP_FILE_NAME_C.c_str(), NET_SEND_MAP_FILE_LEN);
    }

    // 硬件定时器
    if (!m_objHardwareTimer.Init(_T("TMR1:")))
    {
        goto exitCode;
    }

    // 标记为已成功初始化
    m_bIsInited = true;
    return true;

exitCode:
    UnInitFrameSend();
    return false;
}

///
/// @brief
///    卸载发送管理器
///
void CFrameSend::UnInitFrameSend()
{
    // 光串口
    if (NULL != m_pFrameFt3C )
    {
        m_pFrameFt3C->UnInitFt3();
        delete m_pFrameFt3C;
        m_pFrameFt3C = NULL;
    }

    // 光以太网口A
    if (NULL != m_pFrameNetA )
    {
        m_pFrameNetA->UnInitNet();
        delete m_pFrameNetA;
        m_pFrameNetA = NULL;
    }

    // 光以太网口B
    if (NULL != m_pFrameNetB )
    {
        m_pFrameNetB->UnInitNet();
        delete m_pFrameNetB;
        m_pFrameNetB = NULL;
    }

    // 电以太网口
    if (NULL != m_pFrameNetC)
    {
        m_pFrameNetC->UnInitNet();
        delete m_pFrameNetC;
        m_pFrameNetC = NULL;
    }

    // 硬件定时器
    m_objHardwareTimer.UnInit();

    // 设置为未初始化
    m_bIsInited = false;
}

///
/// @brief
///     获得GPMC总线句柄
///
/// @return
///     HANDLE    GPMC总线句柄
///
HANDLE CFrameSend::GetGPMDevice()
{
    return m_hGPMDevice;
}

///
/// @brief
///    是否已初始化
///
///
/// @return
///     bool    是否已初始化
///
bool CFrameSend::IsInited()
{
    return m_bIsInited;
}

///
/// @brief
///    获得当前正在进行的事务类型（手动试验、状态序列）
///
/// @return
///     eFunctionType    当前正在进行的事务类型
///
eFunctionType CFrameSend::GetCurFunctionType()
{
    return m_eCurFunctionType;
}

///
/// @brief
///    获得SMV发送的状态
///
///
/// @return
///     eSendStatus    SMV发送状态
///
eSendStatus CFrameSend::GetManualTestSMVSendStatus()
{
    return m_eManualTestSMVSendStatus;
}

///
/// @brief
///    发送SMV控制块
///
/// @param[in]  stFunctionType  功能类型（手动试验、状态序列）
/// @param[in]  pStateTestResult    指向结果集对象的指针
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSend::__SendSMVCbs(eFunctionType stFunctionType, CStateTestResult* pStateTestResult)
{
    // 检查输入参数
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__SendSMVCbs(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }

    // 手动试验需要重新组帧，然后发送数据
    if (FUNCTION_TYPE_MANUAL_TEST == stFunctionType)
    {
        // 生成被映射虚拟通道一个完整波形的采样值
        CDspRecvData    stDspRecvData;
        CStatus*        pStatus = pStateTestResult->__GetSMVSendValues();
        if (!__GenVirtualChannelSamples(pStatus, stDspRecvData))
        {
            return ERROR_CODE_FAILURE;
        }

        // 分发SMV控制块
        if (!__DistributeSmvCbs())
        {
            return ERROR_CODE_FAILURE;
        }

        //
        // 发送绑定到各个端口的控制块
        // 如果出现了发送失败，那么停止所有发送
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
    // 状态序列直接发送预先计算好的发送帧
    else if (FUNCTION_TYPE_STATE_SEQUENCE == stFunctionType)
    {
        std::list<CStateSendBuffer*>::iterator  iter;
        CStateSendBuffer*                       pStateSendBuffer;


        // 遍历每一个端口的发送缓冲
        iter = pStateTestResult->m_listSmvSendBuffers.begin();
        for (; iter != pStateTestResult->m_listSmvSendBuffers.end(); iter++)
        {
            pStateSendBuffer = *iter;
            if (pStateSendBuffer)
            {
                // 端口发送数据
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
///    准备SMV发送帧
///
/// @param[in]  pStateTestResult    指向结果集对象的指针，用于保存SMV发送帧
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSend::__PrepareSmvSendBuffer(CStateTestResult* pStateTestResult)
{
    bool        bFlag   = true;
    CStatus*    pStatus = NULL;


    // 检查输入参数
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__PrepareSmvSendBuffer(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }

    // 生成被映射虚拟通道一个完整波形的采样值
    CDspRecvData    stDspRecvData;
    pStatus = pStateTestResult->__GetSMVSendValues();
    if (!__GenVirtualChannelSamples(pStatus, stDspRecvData))
    {
        return ERROR_CODE_FAILURE;
    }

    // 预先计算各端口的SMV发送帧
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
///    更新SMV发送帧
///
/// @param[in]  stFunctionType  功能类型(手动试验、状态序列)
/// @param[in]  pStateTestResult    指向结果集对象的指针
///
/// @return
///     bool    成功返回true
///
bool CFrameSend::__UpdateSMVSend(eFunctionType stFunctionType, CStateTestResult* pStateTestResult)
{
    bool        bFlag   = true;
    CStatus*    pStatus = NULL;


    // 检查输入参数
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__SendSMVCbs(): Parameter error \n"));
        return false;
    }

    // 手动试验需要重新组帧，然后发送数据
    if (FUNCTION_TYPE_MANUAL_TEST == stFunctionType)
    {
        // 生成被映射虚拟通道一个完整波形的采样值
        CDspRecvData    stDspRecvData;
        pStatus = pStateTestResult->__GetSMVSendValues();
        if (!__GenVirtualChannelSamples(pStatus, stDspRecvData))
        {
            return false;
        }

        // 发送绑定到各个端口的控制块
        if (!m_pFrameNetA->UpdateSMVFrames(stDspRecvData) ||
            !m_pFrameNetB->UpdateSMVFrames(stDspRecvData) ||
            !m_pFrameNetC->UpdateSMVFrames(stDspRecvData) ||
            !m_pFrameFt3C->UpdateSMVFrames(stDspRecvData)
            )
        {
            return false;
        }
    }
    // 状态序列直接发送预先计算好的发送帧
    else if (FUNCTION_TYPE_STATE_SEQUENCE == stFunctionType)
    {
        std::list<CStateSendBuffer*>::iterator  iter;
        CStateSendBuffer*                       pStateSendBuffer;


        // 遍历每一个端口的发送缓冲
        iter = pStateTestResult->m_listSmvSendBuffers.begin();
        for (; iter != pStateTestResult->m_listSmvSendBuffers.end(); iter++)
        {
            pStateSendBuffer = *iter;
            if (pStateSendBuffer)
            {
                // 各端口发送数据
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
///    停止SMV发送
///
bool CFrameSend::__StopSMVSend()
{
    // 停止各个发送端口
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
///    获得GOOSE的发送状态
///
///
/// @return
///     eSendStatus    GOOSE的发送状态
///
eSendStatus CFrameSend::GetManualTestGooseSendStatus()
{
    return m_eManualTestGooseSendStatus;
}

///
/// @brief
///    准备GOOSE控制块发送帧
///
/// @param[in]  pStateTestResult    指向结果集对象的指针，用于保存GOOSE发送帧
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSend::__PrepareGooseSendBuffer( CStateTestResult* pStateTestResult )
{
    enumErrorCode   eRet;


    // 预先计算各端口的GOOSE发送帧
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
///    准备GOOSE控制块更新帧
///
/// @param[in]  pStateTestResult    指向结果集对象的指针，用于保存GOOSE发送帧
///
/// @return
///     bool    成功返回true
///
bool CFrameSend::__PrepareGooseUpdateBuffer( CStateTestResult* pStateTestResult )
{
    CStateSendBuffer*   pStateSendBuffer;
    std::list<CStateSendBuffer*>::iterator  iter;


    // 遍历每一个端口的发送缓冲
    iter = pStateTestResult->m_listGooseSendBuffers.begin();
    for (; iter != pStateTestResult->m_listGooseSendBuffers.end(); iter++)
    {
        pStateSendBuffer = *iter;
        if (pStateSendBuffer)
        {
            // 各端口更新发送数据
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
///    发送GOOSE控制块
///
/// @param[in]  stFunctionType      功能类型（手动试验、状态序列）
/// @param[in]  pStateTestResult    保存状态测试结果的对象，内部保存有待发送的GOOSE控制块
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSend::__SendGooseCbs(eFunctionType stFunctionType, CStateTestResult* pStateTestResult)
{
    // 检查输入参数
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__SendGooseCbs(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }

    // 手动试验需要重新组帧，然后发送数据
    if (FUNCTION_TYPE_MANUAL_TEST == stFunctionType)
    {
        // 分发GOOSE控制块到各个发送端口
        __DistributeGooseCbs(pStateTestResult);

        //
        // 发送绑定到各个端口的控制块
        // 如果出现了发送失败，那么停止所有发送
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
    // 状态序列直接发送预先计算好的发送帧
    else if (FUNCTION_TYPE_STATE_SEQUENCE == stFunctionType)
    {
        std::list<CStateSendBuffer*>::iterator  iter;
        CStateSendBuffer*                       pStateSendBuffer;


        // 遍历每一个端口的发送缓冲
        iter = pStateTestResult->m_listGooseSendBuffers.begin();
        for (; iter != pStateTestResult->m_listGooseSendBuffers.end(); iter++)
        {
            pStateSendBuffer = *iter;
            if (pStateSendBuffer)
            {
                // 修改发送帧的变位时间字节
                if (!pStateSendBuffer->UpdateEventTime(m_stStateSequenceStartTime + pStateTestResult->m_stStateStartTime))
                {
                    return ERROR_CODE_FAILURE;
                }

                // 各端口发送GOOSE报文
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
///    生成被映射虚拟通道一个完整波形的采样值
///
/// @param[in]  pStatus         指向一个状态对象的指针
/// @param[out] stDspRecvData   存放计算出的采样值的结构体
///
/// @return
///     bool    成功返回true
///
bool CFrameSend::__GenVirtualChannelSamples(CStatus* pStatus, CDspRecvData& stDspRecvData)
{
    int                                 iCount = 0;                     ///< 通道计数器
    CDspSendData                        stDspSendData;                  ///< 向DSP发送的结构体
    CSMVSendVirtualChannel*             pVirtualChannel;


    // 检查输入参数
    if (NULL == pStatus)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::GenVirtualChannelSamples(): Parameter error \n"));
        return false;
    }

    // 清空DSP发送参数
    stDspSendData.m_mapChannelName2Index.clear();
    memset(&(stDspSendData.m_stSendData), 0 , sizeof(stDspSendData.m_stSendData));

    //
    // 获得虚拟通道的参数配置，用于后面DSP的波形生成
    //
    pVirtualChannel = pStatus->FirstSMVSendVirtualChannel();
    while(pVirtualChannel)
    {
        // 检查虚拟通道数
        if (iCount > MAX_SMV_GEN_CHANNEL_COUNT)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::GenVirtualChannelSamples(): iCount > MAX_SMV_GEN_CHANNEL_COUNT \n"));
            return false;
        }

        // 保存通道名
        stDspSendData.m_mapChannelName2Index.insert(std::pair<std::wstring, int>(pVirtualChannel->GetName(), iCount));

        //
        // 根据虚拟通道的交直流类型，设置直流分量、幅值、相角、频率
        //
        if (CURRENT_TYPE_AC == pVirtualChannel->GetACDCType())
        {
            // 直流分量
            stDspSendData.m_stSendData.fDCValue[iCount]          = 0.0f;

            // 幅值
            stDspSendData.m_stSendData.fRmsValue[iCount][0]      = (float)pVirtualChannel->GetAmplitude();

            // 相角
            stDspSendData.m_stSendData.fPhaseAngle[iCount][0]    = (float)pVirtualChannel->GetPhaseAngle();

            // 频率
            stDspSendData.m_stSendData.fFreq[iCount][0]          = (float)pVirtualChannel->GetFrequency();
        }
        else
        {
            // 直流分量
            stDspSendData.m_stSendData.fDCValue[iCount]          = (float)pVirtualChannel->GetAmplitude();
        }

        //
        // 设置谐波数量
        //
        stDspSendData.m_stSendData.nHarmCount[iCount] = 1;
        CHarmonic*  pHarmonic = pVirtualChannel->GetHarmonic();
        if (pHarmonic)
        {
            // 如果有直流谐波，那么把直流谐波的幅值累加到直流分量上
            if (pHarmonic->GetHarmonicType() == HARMONIC_DC)
            {
                stDspSendData.m_stSendData.fDCValue[iCount]     += (float)pHarmonic->GetAmplitude();
            }
            else if (pHarmonic->GetHarmonicType()  >= HARMONIC_2_TIMES)
            {
                // 谐波数量
                stDspSendData.m_stSendData.nHarmCount[iCount]    = 2;

                // 谐波幅值
                stDspSendData.m_stSendData.fRmsValue[iCount][1]  = (float)pHarmonic->GetAmplitude();

                // 谐波相角
                stDspSendData.m_stSendData.fPhaseAngle[iCount][1]= (float)pHarmonic->GetPhaseAngle();

                // 谐波频率（虚拟通道频率 * 谐波类型)
                stDspSendData.m_stSendData.fFreq[iCount][1]      = (float)pVirtualChannel->GetFrequency() * (pHarmonic->GetHarmonicType() - HARMONIC_2_TIMES + 2);
            }
        }
        stDspSendData.m_stSendData.fScaleFactor[iCount] = 1.0f;         ///< 比例因子

        //
        // 获得下一个被映射的虚拟通道
        //
        iCount++;
        pVirtualChannel = pStatus->NextSMVSendVirtualChannel(pVirtualChannel);
    }

    stDspSendData.m_stSendData.nChannelCount = iCount;                  ///< 通道数
    stDspSendData.m_stSendData.nSampleRate   = (uint16_t)__GetSampleFrequency();///< 采样率
    stDspSendData.m_stSendData.nSmvCount     = (uint16_t)(stDspSendData.m_stSendData.nSampleRate / 50);

    // 复制通道名
    stDspRecvData.m_mapChannelName2Index.clear();
    stDspRecvData.m_iFrequency  = __GetSampleFrequency();               ///< 采样频率
    stDspRecvData.m_iSmvCount   = stDspSendData.m_stSendData.nSmvCount; ///< 每周波的采样点数
    stDspRecvData.m_mapChannelName2Index.insert(stDspSendData.m_mapChannelName2Index.begin(), stDspSendData.m_mapChannelName2Index.end());

    //
    // 如果存在被映射的虚拟通道，那么计算其波形
    //
    if (iCount > 0)
    {
        //
        // 以下代码向DSP发送计算请求，并等待计算结果
        //
        CDsp    dspObject;
        if (dspObject.SmvGenCalc(1000, stDspSendData.m_stSendData, &(stDspRecvData.m_stRecvData)))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::GenVirtualChannelSamples(): Dsp SendSmvGen() failed \n"));
            return false;
        }

        // 等待DSP计算结果
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
///     获得SMV采样频率
///
/// @return
///     int     采样频率
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
///     更新GOOSE发送帧
///
/// @param[in]  stFunctionType      功能类型（手动试验、状态序列）
/// @param[in]  pStateTestResult    保存状态测试结果的对象，内部保存有待发送的GOOSE控制块
///
/// @return
///     bool    成功返回true
///
bool CFrameSend::__UpdateGooseSend(eFunctionType stFunctionType, CStateTestResult* pStateTestResult)
{
    // 检查输入参数
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__UpdateGooseSend(): Parameter error \n"));
        return false;
    }

    // 手动试验需要重新组帧，然后发送数据
    if (FUNCTION_TYPE_MANUAL_TEST == stFunctionType)
    {
        // 分发GOOSE控制块到各个发送端口
        __DistributeGooseCbs(pStateTestResult);

        // 发送绑定到各个端口的控制块
        if (!m_pFrameNetA->UpdateGooseFrames() ||
            !m_pFrameNetB->UpdateGooseFrames() ||
            !m_pFrameNetC->UpdateGooseFrames() )
        {
            return false;
        }
    }
    // 状态序列直接发送预先计算好的发送帧
    else if (FUNCTION_TYPE_STATE_SEQUENCE == stFunctionType)
    {
        std::list<CStateSendBuffer*>::iterator  iter;
        CStateSendBuffer*                       pStateSendBuffer;


        // 遍历每一个端口的发送缓冲
        iter = pStateTestResult->m_listGooseSendBuffers.begin();
        for (; iter != pStateTestResult->m_listGooseSendBuffers.end(); iter++)
        {
            pStateSendBuffer = *iter;
            if (pStateSendBuffer)
            {
                // 修改所有发送帧的变位时间字节
                if (!pStateSendBuffer->UpdateEventTime(m_stStateSequenceStartTime + pStateTestResult->m_stStateStartTime))
                {
                    return false;
                }

                // 修改所有发送帧中变位时间通道的值
                if (!pStateSendBuffer->UpdateChannelTime(m_stStateSequenceStartTime + pStateTestResult->m_stStateStartTime))
                {
                    return false;
                }

                // 各端口发送报文
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
///    停止GOOSE发送
///
bool CFrameSend::__StopGooseSend()
{
    // 停止各个发送端口
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
///    设置手动试验SMV的发送状态
///
/// @param[in]  eNewStatus    新的SMV发送状态
///
void CFrameSend::__SetManualTestSMVSendingStatus( eSendStatus eNewStatus )
{
    m_eManualTestSMVSendStatus  = eNewStatus;                           ///< 新的SMV发送状态
}

///
/// @brief
///    设置手动试验Goose的发送状态
///
/// @param[in]  eNewStatus  新的GOOSE发送状态
///
void CFrameSend::__SetManualTestGooseSendStatus( eSendStatus eNewStatus )
{
    m_eManualTestGooseSendStatus = eNewStatus;                          ///< 新的手动试验Goose发送状态
}

///
/// @brief
///    设置当前的正在进行的事务类型
///
/// @param[in]  eNewType    新的事务类型
///
void CFrameSend::__SetCurFunctionType( eFunctionType eNewType )
{
    m_eCurFunctionType = eNewType;
}

///
/// @brief
///    添加一个状态序列测试结果集对象到容器中
///
/// @return
///     CStateTestResult*    成功时返回状态测试结果对象的指针，失败时返回NULL
///
CStateTestResult* CFrameSend::__AddStateSequenceResult()
{
    CStateTestResult* pStateTestResult = NULL;


    // new一个CStateTestResult对象
    pStateTestResult = new CStateTestResult(TEST_TYPE_STATE_SEQUENCE);
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__AddStateSequenceResult(): new CStateTestResult failed \n"));
        return NULL;
    }

    // 初始化CStateTestResult对象
    if (!pStateTestResult->Init())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__AddStateSequenceResult(): CStateTestResult object init failed \n"));
        delete pStateTestResult;
        return NULL;
    }

    // 添加对象到结果集容器中
    m_vecStateSequenceResult.push_back(pStateTestResult);
    return pStateTestResult;
}

///
/// @brief
///    获得状态序列结果集数量
///
/// @return
///     unsigned int    状态序列结果集数量
///
unsigned int CFrameSend::GetStateSequenceResultCount()
{
    return m_vecStateSequenceResult.size();
}

///
/// @brief
///    获得指定状态号的状态序列结果集对象
///
/// @param[in]  iStateIndex    状态号(从0开始计数)
///
/// @return
///     CStateTestResult*    存放指定状态测试结果的对象，失败时返回NULL
///
CStateTestResult* CFrameSend::GetStateSequenceResult( unsigned int iStateIndex )
{
    // 检查输入参数
    if (iStateIndex >= m_vecStateSequenceResult.size())
    {
        return NULL;
    }

    return m_vecStateSequenceResult[iStateIndex];
}

///
/// @brief
///    清除所有的状态序列结果集
///
void CFrameSend::ClearAllStateSequenceResult()
{
    CStateTestResult* pStateTestResult = NULL;
    std::vector<CStateTestResult*>::iterator iter;


    // 释放测试结果对象
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

    // 清空容器
    m_vecStateSequenceResult.clear();
}

///
/// @brief
///    开始状态序列测试
///
/// @param[in]  hWnd  窗口句柄，当DI值发生变化时，会发送WM_DI_CHANGED消息给此窗口
///                             当需要询问用户是否手动切换时，会发送WM_WHETHER_SWITCH消息给此窗口
///                             当状态切换后，会发送WM_STATUS_CHANGED消息给此窗口
///                             当测试完成后，会发送WM_TEST_COMPLETED消息给此窗口
///
/// @return
///     enumErrorCode   创建发送线程成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSend::StartStateSequenceTest( HWND hWnd )
{
    int                 iStateIndex         = 0;
    CStateTestResult*   pNewStateTestResult = NULL;
    CStateTestResult*   pOldStateTestResult = NULL;
    enumErrorCode       eRet                = ERROR_CODE_SUCCESS;
    std::map<std::wstring, bool> mapChangedDoArray;


    // 检查是否已初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // 判断窗口句柄是否是一个存在的窗口
    if (!IsWindow(hWnd))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }
    m_hNotifyHwnd = hWnd;

    // 获得数据层服务类对象
    CDsmDataLayer* pDsmDataLayer = CDsmDataLayer::getInstance();
    if (NULL == pDsmDataLayer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): CDsmDataLayer::getInstance() failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得线程互斥锁
    m_criticalFunctionType.Lock();

    // 检查当前是否正在发送状态序列
    if (FUNCTION_TYPE_UNKNOWN != GetCurFunctionType())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): Currently is sending state sequence \n"));
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }
    // 设置为正在发送状态序列
    __SetCurFunctionType(FUNCTION_TYPE_STATE_SEQUENCE);

    // 释放线程互斥锁
    m_criticalFunctionType.Unlock();

    // 清除先前的测试结果
    ClearAllStateSequenceResult();

    // 获得GOOSE发送参数配置管理器
    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        eRet = ERROR_CODE_FAILURE;
        goto ExitHandle;
    }

    // 获得GOOSE接收参数配置管理器
    CParamConfigGOOSERecv* pParamConfigGooseRecv = CParamConfigGOOSERecv::getInstance();
    if (NULL == pParamConfigGooseRecv)
    {
        eRet = ERROR_CODE_FAILURE;
        goto ExitHandle;
    }

    // 分配SMV控制块到各个发送端口
    if (!__DistributeSmvCbs())
    {
        eRet = ERROR_CODE_FAILURE;
        goto ExitHandle;
    }

    // 分发GOOSE控制块到各个发送端口
    if (!__DistributeGooseCbs())
    {
        eRet = ERROR_CODE_FAILURE;
        goto ExitHandle;
    }

    // 获得状态序列配置管理器
    CParamConfigStateSequence* pParamConfigStateSequence = CParamConfigStateSequence::getInstance();
    if (NULL == pParamConfigStateSequence)
    {
        eRet = ERROR_CODE_FAILURE;
        goto ExitHandle;
    }

    // 提前计算好每个状态的发送报文
    iStateIndex             = 0;
    CStatus*    pOldStatus  = NULL;
    CStatus*    pStatus     = pParamConfigStateSequence->FirstStatus(false);
    while(pStatus)
    {
        // 检查用户是否临时取消了事务
        if (m_eventCancelStateSequence.Lock(0))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): User cancels the current operation \n"));
            eRet = ERROR_CODE_CANCEL;
            goto ExitHandle;
        }

        // 添加存放状态测试结果的对象
        pNewStateTestResult = __AddStateSequenceResult();
        if (NULL == pNewStateTestResult)
        {
            eRet = ERROR_CODE_FAILURE;
            goto ExitHandle;
        }

        // 保存SMV各发送控制块的值
        if (!pNewStateTestResult->__GetSMVSendValues()->__Copy(pStatus))
        {
            eRet = ERROR_CODE_FAILURE;
            goto ExitHandle;
        }

        // 查找相同的SMV发送状态
        CStateTestResult* pSimilarStateTestResult = __FindSimilarSmvTestResult(pNewStateTestResult, iStateIndex);

        // 如果存在相同的SMV发送状态，那么拷贝其SMV发送缓冲
        // 如果不存在相同的SMV发送状态，那么重新构造SMV发送缓冲
        if (pSimilarStateTestResult)
        {
            // 拷贝SMV发送缓冲
            if (!pNewStateTestResult->__CopySmvSendBuffers(pSimilarStateTestResult))
            {
                eRet = ERROR_CODE_FAILURE;
                goto ExitHandle;
            }
        }
        else
        {
            // 准备SMV发送帧
            eRet = __PrepareSmvSendBuffer(pNewStateTestResult);
            if (ERROR_CODE_SUCCESS != eRet)
            {
                goto ExitHandle;
            }
        }

        // 准备状态0的GOOSE发送帧
        if (0 == iStateIndex)
        {
            // 获得状态0所有DO的键值对
            pStatus->__FindChangedDO(NULL, mapChangedDoArray);

            // 更新映射到指定虚拟通道的所有真实通道的值
            if (!pParamConfigGooseSend->UpdateGooseChannelSendValue(mapChangedDoArray))
            {
                eRet = ERROR_CODE_FAILURE;
                goto ExitHandle;
            }

            // 准备GOOSE发送帧
            eRet = __PrepareGooseSendBuffer(pNewStateTestResult);
            if (ERROR_CODE_SUCCESS != eRet)
            {
                goto ExitHandle;
            }

            // 计算GOOSE控制块中所有通道的偏移量，在修改缓冲区中时间通道的值时会用到
            if (!__CalcChannelOffset(pNewStateTestResult))
            {
                eRet = ERROR_CODE_FAILURE;
                goto ExitHandle;
            }
        }
        // 准备其他状态的GOOSE发送帧
        else
        {
            // 拷贝GOOSE发送缓冲
            if (!pNewStateTestResult->__CopyGooseSendBuffers(pOldStateTestResult))
            {
                eRet = ERROR_CODE_FAILURE;
                goto ExitHandle;
            }

            // 与老的状态比较，返回true表示DO发生了改变
            if (pStatus->__FindChangedDO(pOldStatus, mapChangedDoArray))
            {
                // 更新映射到指定虚拟通道的所有真实通道的值，并标记变位的单点/双点通道
                if (!pParamConfigGooseSend->UpdateGooseChannelSendValue(mapChangedDoArray))
                {
                    eRet = ERROR_CODE_FAILURE;
                    goto ExitHandle;
                }

                // 自动寻找变位的单点/双点通道，然后记录相邻或间隔的时间通道
                pNewStateTestResult->__SaveGooseCbTimeChannel();

                // 准备GOOSE更新帧
                if (!__PrepareGooseUpdateBuffer(pNewStateTestResult))
                {
                    eRet = ERROR_CODE_FAILURE;
                    goto ExitHandle;
                }
            }
        }

        // 设置状态的结果集
        pOldStateTestResult = pNewStateTestResult;
        pStatus->SetStateTestResult(pNewStateTestResult);

        // 获得状态序列下一组状态
        pOldStatus  = pStatus;
        pStatus     = pParamConfigStateSequence->NextStatus(pOldStatus);
        iStateIndex++;
    }

    // 开启GOOSE接收线程
    pDsmDataLayer->StartRecvData(NET_RECV_TYPE_GOOSE, DSM_MODE_GOOSE_RECV_DI);

    // 依次发送每个状态的报文
    iStateIndex = 0;
    pStatus     = pParamConfigStateSequence->FirstStatus(false);
    while(pStatus)
    {
        // 检查用户是否临时取消了事务
        if (m_eventCancelStateSequence.Lock(0))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): User cancels the current operation \n"));
            eRet = ERROR_CODE_CANCEL;
            goto ExitHandle;
        }

        // 获得状态对应的结果集对象
        pNewStateTestResult = pStatus->GetStateTestResult();
        if (NULL == pNewStateTestResult)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): irregular test result \n"));
            eRet = ERROR_CODE_FAILURE;
            goto ExitHandle;
        }

        // 通知窗口正在发送第iStateIndex组状态
        ::PostMessage(hWnd, WM_STATUS_CHANGED, iStateIndex, 0);

        // 获得当前FPGA时间
        CEventTime  stCurTime;
        GetCurrentFpgaTime(stCurTime);

        // 如果是第一组状态，那么采用SendSMVCbs函数；
        // 如果是其他组状态，那么采用UpdateSMVSend函数；
        if (0 == iStateIndex)
        {
            // 保存状态序列开始时间
            m_stStateSequenceStartTime = stCurTime;

            // 设置状态发送时间（相对时间）
            pNewStateTestResult->SetStartTime(stCurTime - m_stStateSequenceStartTime);

            // 开始发送SMV
            if (ERROR_CODE_SUCCESS != __SendSMVCbs(FUNCTION_TYPE_STATE_SEQUENCE, pNewStateTestResult))
            {
                break;
            }

            // 开始发送GOOSE
            if (ERROR_CODE_SUCCESS != __SendGooseCbs(FUNCTION_TYPE_STATE_SEQUENCE, pNewStateTestResult))
            {
                break;
            }
        }
        else
        {
            // 设置状态发送时间（相对时间）
            pNewStateTestResult->SetStartTime(stCurTime - m_stStateSequenceStartTime);

            // 更新SMV
            if (!__UpdateSMVSend(FUNCTION_TYPE_STATE_SEQUENCE, pNewStateTestResult))
            {
                break;
            }

            // 更新GOOSE
            if (!__UpdateGooseSend(FUNCTION_TYPE_STATE_SEQUENCE, pNewStateTestResult))
            {
                break;
            }
        }

        // 设置测试完成标识
        pNewStateTestResult->SetTested(true);

        // 设置当前正在发送的状态序列号
        __SetCurStateIndex(iStateIndex);
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): State %d send successfully \n", iStateIndex));

        //
        // 根据设置判断是否切换到状态序列下一组状态
        //
        eSwitchType eType = pStatus->GetSwitchType();
        if (SWITCH_TYPE_TIME == eType)                                  ///< 限时切换
        {
            // 等待指定时间或用户取消事件置位
            if (!m_objHardwareTimer.Wait(pStatus->GetDelay(), m_eventCancelStateSequence.m_hObject))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): User cancels the current operation \n"));
                eRet = ERROR_CODE_CANCEL;
                goto ExitHandle;
            }
        }
        else if (SWITCH_TYPE_DI == eType)                               ///< 开入量切换
        {
            eLogicalOperator eOperator = pStatus->GetLogicalOperator();
            if (LOGICAL_AND == eOperator)                               ///< 逻辑与
            {
                while(1)
                {
                    bool                        bResult     = true;
                    CGOOSERecvVirtualChannel*   pDI         = NULL;
                    CChannel*                   pChannel    = NULL;


                    for (int i = 0; i < sizeof(PREDEFINED_DI_INFO)/sizeof(PREDEFINED_DI_INFO[0]); i++)
                    {
                        // 检查用户是否临时取消了事务
                        if (m_eventCancelStateSequence.Lock(0))
                        {
                            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): User cancels the current operation \n"));
                            eRet = ERROR_CODE_CANCEL;
                            goto ExitHandle;
                        }

                        // 判断指定的DI是否被勾选
                        if (!pStatus->IsDIChecked(PREDEFINED_DI_INFO[i].m_strName))
                        {
                            continue;
                        }

                        // 判断指定的虚拟通道是否存在
                        pDI = pParamConfigGooseRecv->GetVirtualChannelByName(PREDEFINED_DI_INFO[i].m_strName);
                        if (NULL == pDI)
                        {
                            continue;
                        }

                        // 判断指定的虚拟通道是否映射有真实通道
                        pChannel = pDI->GetChannel();
                        if (NULL == pChannel)
                        {
                            continue;
                        }

                        // 如果是单点通道，那么判断其值是否为true
                        // 如果是双点通道，那么判断其值是否为on
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

                    // 如果所有被映射通道的值都为on，那么逻辑与条件成立，跳出循环
                    // 否则休眠一段时间后再次去判断DI通道的状态
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
                        // 检查用户是否临时取消了事务
                        if (m_eventCancelStateSequence.Lock(0))
                        {
                            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartStateSequenceTest(): User cancels the current operation \n"));
                            eRet = ERROR_CODE_CANCEL;
                            goto ExitHandle;
                        }

                        // 判断指定的DI是否被勾选
                        if (!pStatus->IsDIChecked(PREDEFINED_DI_INFO[i].m_strName))
                        {
                            continue;
                        }

                        // 判断指定的虚拟通道是否存在
                        pDI = pParamConfigGooseRecv->GetVirtualChannelByName(PREDEFINED_DI_INFO[i].m_strName);
                        if (NULL == pDI)
                        {
                            continue;
                        }

                        // 判断指定的虚拟通道是否映射有真实通道
                        pChannel = pDI->GetChannel();
                        if (NULL == pChannel)
                        {
                            continue;
                        }

                        // 如果是单点通道，那么判断其值是否为true
                        // 如果是双点通道，那么判断其值是否为on
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

                    // 如果某个被映射通道的值为on，那么逻辑或条件成立，跳出循环
                    // 否则休眠一段时间后再次去判断DI通道的状态
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
        else                                                            ///< 手动切换
        {
            // 询问窗口是否手动切换到下一个状态
            ::SendMessage(hWnd, WM_WHETHER_SWITCH, 0, 0);
        }

        //
        // 获得状态序列下一组状态
        //
        pStatus = pParamConfigStateSequence->NextStatus(pStatus);
        iStateIndex++;
    }

ExitHandle:
    // 停止接收GOOSE数据
    pDsmDataLayer->StopRecvData();

    // 停止SMV、GOOSE发送
    if (__StopSMVSend() && __StopGooseSend())
    {
        // 设置状态序列发送状态：停止
        __SetCurFunctionType(FUNCTION_TYPE_UNKNOWN);
    }

    return eRet;
}

///
/// @brief
///    停止状态序列测试
///
bool CFrameSend::StopStateSequenceTest()
{
    MSG     stMsg;


    // 检查是否已初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StopStateSequenceTest(): uninit \n"));
        return false;
    }

    // 检查当前是否正在进行状态序列测试，如果正在测试，设置取消标识，等待试验终止
    if (FUNCTION_TYPE_STATE_SEQUENCE == GetCurFunctionType())
    {
        // 置位取消标志
        m_eventCancelStateSequence.SetEvent();
        m_eventCancelSMV.SetEvent();

        // 一直等待，直到状态序列发送状态为：停止
        while(FUNCTION_TYPE_STATE_SEQUENCE == GetCurFunctionType())
        {
            // 添加一个消息泵，在等待状态序列线程结束的时候可以继续分发消息
            while (PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&stMsg);
                DispatchMessage(&stMsg);
            }

            Sleep(10);
        }

        // 复位组帧标志
        m_eventCancelSMV.ResetEvent();
        m_eventCancelStateSequence.ResetEvent();
    }

    return true;
}

///
/// @brief
///    获得通知窗口
///
///
/// @return
///     HWND    窗口句柄
///
HWND CFrameSend::GetNotifyHwnd()
{
    return m_hNotifyHwnd;
}

///
/// @brief
///    获得当前正在发送第几个状态（从0开始计数）
///
int CFrameSend::GetCurStateIndex()
{
    return m_iCurStateIndex;
}

///
/// @brief
///    设置当前正在发送的第几个状态(从0开始计数)
///
/// @param[in]  iNewStateIndex    
///
void CFrameSend::__SetCurStateIndex( int iNewStateIndex )
{
    m_iCurStateIndex = iNewStateIndex;                                  ///< 设置正在发送的状态序列号
}

///
/// @brief
///    添加一个手动试验测试结果集对象到容器中
///
/// @param[in]  eType    测试的类型
///
/// @return
///     CStateTestResult*    CStateTestResult*    成功时返回状态测试结果对象的指针，失败时返回NULL
///
CStateTestResult* CFrameSend::__AddManualTestResult(enumTestType eType)
{
    CStateTestResult* pNewStateTestResult = NULL;


    // new一个CStateTestResult对象
    pNewStateTestResult = new CStateTestResult(eType);
    if (NULL == pNewStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__AddManualTestResult(): new CStateTestResult failed \n"));
        return NULL;
    }

    // 初始化CStateTestResult对象
    if (!pNewStateTestResult->Init())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__AddManualTestResult(): CStateTestResult object init failed \n"));
        delete pNewStateTestResult;
        return NULL;
    }

    // 获得当前时间
    CEventTime stCurTime;
    if (!GetCurrentFpgaTime(stCurTime))
    {
        return NULL;
    }

    // 记录手动试验开始时间
    if (0 == m_vecManualTestResult.size())
    {
        m_stManualTestStartTime = stCurTime;
    }
    // 如果超过了最大结果集数，那么删除最老的记录
    else if (m_vecManualTestResult.size() >= MAX_TEST_RESULT_COUNT)
    {
        delete *(m_vecManualTestResult.begin());
        m_vecManualTestResult.erase(m_vecManualTestResult.begin());
    }

    // 设置状态开始时间（相对时间）
    pNewStateTestResult->SetStartTime(stCurTime - m_stManualTestStartTime);

    // 添加对象到结果集容器中
    m_vecManualTestResult.push_back(pNewStateTestResult);
    return pNewStateTestResult;
}

///
/// @brief
///    删除指定的手动试验测试结果对象
///
/// @param[in]  pDeleteTestResult    待删除的手动试验测试结果对象
///
void CFrameSend::__DeleteManualTestResult( CStateTestResult* pDeleteTestResult )
{
    CStateTestResult* pStateTestResult = NULL;
    std::vector<CStateTestResult*>::iterator iter;


    // 查找指定的待删除对象
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
///    获得手动试验结果集数量
///
/// @return
///     unsigned int    手动试验结果集数量
///
unsigned int CFrameSend::GetManualTestResultCount()
{
    return m_vecManualTestResult.size();
}

///
/// @brief
///    获得指定索引号的手动试验结果集对象
///
/// @param[in]  nIndex    结果集索引号(从0开始计数)
///
/// @return
///     CStateTestResult*    存放指定状态测试结果的对象，失败时返回NULL
///
CStateTestResult* CFrameSend::GetManualTestResult( unsigned int nIndex )
{
    // 检查输入参数
    if (nIndex >= m_vecManualTestResult.size())
    {
        return NULL;
    }

    return m_vecManualTestResult[nIndex];
}

///
/// @brief
///    获得指定手动试验结果集对象的索引号
///
/// @param[in]  pCurResult    手动试验结果集对象
///
/// @return
///     int    结果集索引号，从0开始计数，失败时返回-1
///
int CFrameSend::GetManualTestResultIndex( CStateTestResult* pCurResult )
{
    int                                         nIndex;
    std::vector<CStateTestResult*>::iterator    iter;


    // 检查输入参数
    if (NULL == pCurResult)
    {
        return (-1);
    }

    // 查找指定的结果集对象
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
///    获得第一个手动试验结果集对象
///
/// @return
///     CStateTestResult*    指向结果集对象的指针，失败时返回NULL
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
///     获得下一个手动试验结果集对象
///
/// @param[in]  pCurTestResult    指向当前结果集对象的指针
///
/// @return
///     CStateTestResult*   指向下一个结果集对象的指针，失败时返回NULL
///
CStateTestResult* CFrameSend::NextManualTestResult( CStateTestResult* pCurTestResult )
{
    std::vector<CStateTestResult*>::iterator    iter;


    iter = m_vecManualTestResult.begin();
    while (m_vecManualTestResult.end() != iter)
    {
        // 查找当前结果集对象的位置
        if (*iter == pCurTestResult)
        {
            // 获得下一个结果集对象的位置
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
///    获得上一个手动试验结果集对象
///
/// @param[in]  pCurTestResult    指向当前结果集对象的指针
///
/// @return
///     CStateTestResult*    指向上一个结果集对象的指针，失败时返回NULL
///
CStateTestResult* CFrameSend::PrevManualTestResult( CStateTestResult* pCurTestResult )
{
    std::vector<CStateTestResult*>::reverse_iterator  rIter;


    rIter = m_vecManualTestResult.rbegin();
    while (m_vecManualTestResult.rend() != rIter)
    {
        // 查找当前结果集对象的位置
        if (*rIter == pCurTestResult)
        {
            // 获得上一个结果集对象的位置
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
///     获得最后一个手动试验结果集对象
///
/// @return
///     CStateTestResult*    指向结果集对象的指针，失败时返回NULL
///
CStateTestResult* CFrameSend::LastManualTestResult()
{
    CStateTestResult* pStateTestResult = NULL;


    // 获得线程互斥锁，因为接收线程可能在更新数据的时候试图去获得最后一个结果集对象
    m_criticalFunctionType.Lock();

    // 获得最后一个结果集对象
    std::vector<CStateTestResult*>::reverse_iterator rIter = m_vecManualTestResult.rbegin();
    if (m_vecManualTestResult.rend() != rIter)
    {
        pStateTestResult = *rIter;
    }

    // 释放线程互斥锁
    m_criticalFunctionType.Unlock();

    return pStateTestResult;
}

///
/// @brief
///    清除所有的手动试验结果集
///
void CFrameSend::ClearAllManualTestResult()
{
    CStateTestResult* pStateTestResult = NULL;
    std::vector<CStateTestResult*>::iterator iter;


    // 释放测试结果对象
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

    // 清空容器
    m_vecManualTestResult.clear();
}

///
/// @brief
///    开始手动试验SMV测试
///
/// @param[in]  hWnd    hWnd  窗口句柄，当DI值发生变化时，会发送WM_DI_CHANGED消息给此窗口
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSend::StartManualTestSMV( HWND hWnd )
{
    // 检查是否已初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestSMV(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // 判断窗口句柄是否是一个存在的窗口
    if (!IsWindow(hWnd))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestSMV(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }
    m_hNotifyHwnd = hWnd;

    // 获得线程互斥锁
    m_criticalFunctionType.Lock();

    // 如果当前正在测试状态序列，那么直接返回
    if (FUNCTION_TYPE_STATE_SEQUENCE == m_eCurFunctionType)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestSMV(): Currently is testing state sequence \n"));
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }

    // 检查当前是否正在发送SMV
    if (SEND_STATUS_STOPPED != m_eManualTestSMVSendStatus)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestSMV(): Currently is transmitting data \n"));
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }

    // 如果当前没有开启任何测试
    if (FUNCTION_TYPE_UNKNOWN == m_eCurFunctionType)
    {
        // 清除先前的测试结果
        ClearAllManualTestResult();

        // 设置当前状态号为-1
        m_iCurStateIndex = -1;
    }

    // 添加存放状态测试结果的对象
    CStateTestResult* pStateTestResult = __AddManualTestResult(TEST_TYPE_SMV);
    if (NULL == pStateTestResult)
    {
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }

    // 保存手动试验各SMV发送虚拟通道的值
    if (!pStateTestResult->__SaveManualTestSMVSendValues())
    {
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }

    // 复位状态改变标记
    CParamConfigSMVSend::getInstance()->SetStatusWordChanged(false);
    CParamConfigSMVSend::getInstance()->SetQualityChanged(false);

    // 设置当前正在发送第几个状态
    m_iCurStateIndex = m_iCurStateIndex + 1;

    // 设置发送管理器状态为：正在准备发送帧
    __SetManualTestSMVSendingStatus(SEND_STATUS_PREPARE);
    // 设置为正在进行手动试验测试
    __SetCurFunctionType(FUNCTION_TYPE_MANUAL_TEST);

    // 释放线程互斥锁
    m_criticalFunctionType.Unlock();

    //
    // 合成发送帧，然后开始发送
    //
    enumErrorCode eRet = __SendSMVCbs(FUNCTION_TYPE_MANUAL_TEST, pStateTestResult);
    if (ERROR_CODE_SUCCESS == eRet)
    {
        // 设置测试完成标识
        pStateTestResult->SetTested(true);

        // 开启GOOSE接收线程
        CDsmDataLayer* pDsmDataLayer = CDsmDataLayer::getInstance();
        if (pDsmDataLayer)
        {
            pDsmDataLayer->StartRecvData(NET_RECV_TYPE_GOOSE, DSM_MODE_GOOSE_RECV_DI);
        }

        // 设置发送管理器状态为：正在发送
        __SetManualTestSMVSendingStatus(SEND_STATUS_SENDING);
    }
    else
    {
        // 如果GOOSE发送也处于停止状态，那么设置手动试验测试状态为：停止
        m_criticalFunctionType.Lock();
        if (SEND_STATUS_STOPPED == GetManualTestGooseSendStatus())
        {
            // 设置为已停止手动试验测试
            __SetCurFunctionType(FUNCTION_TYPE_UNKNOWN);
        }
        m_criticalFunctionType.Unlock();

        // 设置发送管理器状态为：已停止
        __SetManualTestSMVSendingStatus(SEND_STATUS_STOPPED);
    }

    return eRet;
}

///
/// @brief
///    更新手动试验SMV测试的数据
///
/// @return
///     bool    成功返回true
///
bool CFrameSend::UpateManualTestSMV()
{
    bool                bRet                = true;
    CStateTestResult*   pOldStateTestResult = NULL;


    // 检查是否已初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::UpateManualTestSMV(): uninit \n"));
        return false;
    }

    // 获得线程互斥锁
    m_criticalFunctionType.Lock();

    // 检查当前是否正在发送SMV报文
    if (SEND_STATUS_SENDING != GetManualTestSMVSendStatus())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::UpateManualTestSMV(): Has not yet begun to send data \n"));
        m_criticalFunctionType.Unlock();
        return false;
    }

    // 添加存放状态测试结果的对象
    CStateTestResult* pNewStateTestResult = __AddManualTestResult(TEST_TYPE_SMV);
    if (NULL == pNewStateTestResult)
    {
        m_criticalFunctionType.Unlock();
        return false;
    }

    // 保存手动试验各SMV发送虚拟通道的值
    if (!pNewStateTestResult->__SaveManualTestSMVSendValues())
    {
        m_criticalFunctionType.Unlock();
        return false;
    }

    // 保存状态字、品质变位标记
    pNewStateTestResult->SetStatusWordChanged(CParamConfigSMVSend::getInstance()->IsStatusWordChanged());
    pNewStateTestResult->SetQualityChanged(CParamConfigSMVSend::getInstance()->IsQualityChanged());
    CParamConfigSMVSend::getInstance()->SetStatusWordChanged(false);
    CParamConfigSMVSend::getInstance()->SetQualityChanged(false);

    // 查找上一个保存有SMV虚拟通道信息的结果集对象
    pNewStateTestResult->SetSmvChannelChanged(true);
    pOldStateTestResult = PrevManualTestSMVResult(pNewStateTestResult);
    if (pOldStateTestResult)
    {
        // 检查SMV发送值是否发生了变化
        if (pNewStateTestResult->__CompareSMVSendValues(TYPE_FIND_ALL_DIFFERENT, pOldStateTestResult))
        {
            pNewStateTestResult->SetSmvChannelChanged(false);
        }
    }

    // 检查通道值、状态字、品质是否发生了变化
    if (!pNewStateTestResult->IsSmvChannelChanged() &&
        !pNewStateTestResult->IsStatusWordChanged() &&
        !pNewStateTestResult->IsQualityChanged())
    {
        // 如果没有任何变化，那么取消更新数据
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::UpateManualTestSMV(): No change, the transmission is canceled \n"));

        // 删除最新添加的那个结果对象
        __DeleteManualTestResult(pNewStateTestResult);
        m_criticalFunctionType.Unlock();
        return false;
    }

    // 设置当前正在发送第几个状态
    m_iCurStateIndex = m_iCurStateIndex + 1;

    // 设置发送管理器状态为：正在准备发送帧
    __SetManualTestSMVSendingStatus(SEND_STATUS_PREPARE);

    // 释放线程互斥锁
    m_criticalFunctionType.Unlock();

    // 更新SMV发送帧
    bRet = __UpdateSMVSend(FUNCTION_TYPE_MANUAL_TEST, pNewStateTestResult);

    // 设置测试完成标识
    pNewStateTestResult->SetTested(bRet);

    // 设置发送管理器状态为：正在发送
    __SetManualTestSMVSendingStatus(SEND_STATUS_SENDING);
    return bRet;
}

///
/// @brief
///    停止手动试验SMV测试
///
/// @return
///     bool    成功返回true
///
bool CFrameSend::StopManualTestSMV()
{
    // 检查是否已初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StopManualTestSMV(): uninit \n"));
        return false;
    }

    // 检查当前是否正在组帧，如果正在组帧，设置取消组帧标识，等待组帧结束
    m_criticalFunctionType.Lock();
    if (SEND_STATUS_PREPARE == GetManualTestSMVSendStatus())
    {
        // 置位取消组帧标志
        m_eventCancelSMV.SetEvent();

        // 一直等待，直到SMV的发送类型不为：准备发送
        while(SEND_STATUS_PREPARE == GetManualTestSMVSendStatus())
        {
            Sleep(10);
        }

        // 复位取消组帧标志
        m_eventCancelSMV.ResetEvent();
    }
    m_criticalFunctionType.Unlock();

    //
    // 停止SMV接收
    //
    if (__StopSMVSend())
    {
        // 如果GOOSE发送也处于停止状态，那么停止GOOSE接收线程
        m_criticalFunctionType.Lock();
        if (SEND_STATUS_STOPPED == GetManualTestGooseSendStatus())
        {
            CDsmDataLayer* pDsmDataLayer = CDsmDataLayer::getInstance();
            if (pDsmDataLayer)
            {
                pDsmDataLayer->StopRecvData();
            }

            // 设置手动试验测试状态为停止
            __SetCurFunctionType(FUNCTION_TYPE_UNKNOWN);
        }
        m_criticalFunctionType.Unlock();

        // 设置手动试验SMV发送状态为：停止发送
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
///    开始手动试验GOOSE测试
///
/// @param[in]  hWnd    hWnd  窗口句柄，当DI值发生变化时，会发送WM_DI_CHANGED消息给此窗口
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSend::StartManualTestGoose( HWND hWnd )
{
    // 检查是否已初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestGoose(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // 判断窗口句柄是否是一个存在的窗口
    if (!IsWindow(hWnd))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestGoose(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }
    m_hNotifyHwnd = hWnd;

    // 获得线程互斥锁
    m_criticalFunctionType.Lock();

    // 如果当前正在测试状态序列，那么直接返回
    if (FUNCTION_TYPE_STATE_SEQUENCE == GetCurFunctionType())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestGoose(): Currently is testing state sequence \n"));
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }

    // 检查当前是否正在发送GOOSE
    if (SEND_STATUS_STOPPED != GetManualTestGooseSendStatus())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StartManualTestGoose(): Currently is transmitting data \n"));
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }

    // 如果当前还没有开始手动试验测试，那么清除先前的所有测试结果
    if (FUNCTION_TYPE_UNKNOWN == GetCurFunctionType())
    {
        // 清除先前的测试结果
        ClearAllManualTestResult();

        // 设置当前状态号为-1
        m_iCurStateIndex = -1;
    }

    // 添加存放状态测试结果的对象
    CStateTestResult* pStateTestResult = __AddManualTestResult(TEST_TYPE_GOOSE);
    if (NULL == pStateTestResult)
    {
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }

    // 保存手动试验各GOOSE发送控制块的值
    if (!pStateTestResult->__SaveGooseSendCbs())
    {
        m_criticalFunctionType.Unlock();
        return ERROR_CODE_FAILURE;
    }
    // 设置当前正在发送第几个状态
    m_iCurStateIndex = m_iCurStateIndex + 1;

    // 设置手动试验GOOSE发送状态为：正在准备发送帧
    __SetManualTestGooseSendStatus(SEND_STATUS_PREPARE);
    // 设置为正在进行手动试验测试
    __SetCurFunctionType(FUNCTION_TYPE_MANUAL_TEST);

    // 释放线程互斥锁
    m_criticalFunctionType.Unlock();

    //
    // 合成发送帧，然后开始发送
    //
    enumErrorCode eRet = __SendGooseCbs(FUNCTION_TYPE_MANUAL_TEST, pStateTestResult);
    if (ERROR_CODE_SUCCESS == eRet)
    {
        // 设置测试完成标识
        pStateTestResult->SetTested(true);

        // 开启GOOSE接收线程
        CDsmDataLayer* pDsmDataLayer = CDsmDataLayer::getInstance();
        if (pDsmDataLayer)
        {
            pDsmDataLayer->StartRecvData(NET_RECV_TYPE_GOOSE, DSM_MODE_GOOSE_RECV_DI);
        }

        // 设置手动试验GOOSE发送状态为：正在发送
        __SetManualTestGooseSendStatus(SEND_STATUS_SENDING);
    }
    else
    {
        // 如果SMV发送也处于停止状态，那么设置手动试验测试状态为：停止
        m_criticalFunctionType.Lock();
        if (SEND_STATUS_STOPPED == GetManualTestSMVSendStatus())
        {
            // 设置为已停止手动试验测试
            __SetCurFunctionType(FUNCTION_TYPE_UNKNOWN);
        }
        m_criticalFunctionType.Unlock();

        // 设置手动试验GOOSE发送状态为：已停止
        __SetManualTestGooseSendStatus(SEND_STATUS_STOPPED);
    }

    return eRet;
}

///
/// @brief
///    更新手动试验GOOSE测试的数据
///
/// @return
///     bool    成功返回true
///
bool CFrameSend::UpateManualTestGoose()
{
    bool                bRet                = true;
    CStateTestResult*   pOldStateTestResult = NULL;


    // 检查是否已初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::UpateManualTestGoose(): uninit \n"));
        return false;
    }

    // 获得线程互斥锁
    m_criticalFunctionType.Lock();

    // 检查当前是否正在发送GOOSE报文
    if (SEND_STATUS_SENDING != GetManualTestGooseSendStatus())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::UpateManualTestGoose(): Has not yet begun to send data \n"));
        m_criticalFunctionType.Unlock();
        return false;
    }

    // 添加存放状态测试结果的对象
    CStateTestResult* pNewStateTestResult = __AddManualTestResult(TEST_TYPE_GOOSE);
    if (NULL == pNewStateTestResult)
    {
        m_criticalFunctionType.Unlock();
        return false;
    }

    // 保存手动试验各GOOSE发送控制块的值
    if (!pNewStateTestResult->__SaveGooseSendCbs())
    {
        m_criticalFunctionType.Unlock();
        return false;
    }

    // 查找上一个保存有GOOSE控制块信息的结果集对象
    pNewStateTestResult->SetGooseChannelChanged(true);
    pOldStateTestResult = PrevManualTestGooseResult(pNewStateTestResult);
    if (pOldStateTestResult)
    {
        // 如果没有任何变化，那么取消更新数据
        if (pNewStateTestResult->__CompareGooseSendValues(pOldStateTestResult))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::UpateManualTestGoose(): No change, the transmission is canceled \n"));

            // 删除最新添加的那个结果对象
            __DeleteManualTestResult(pNewStateTestResult);
            m_criticalFunctionType.Unlock();
            return false;
        }

        // 修改GOOSE控制块中单点/双点类型通道时，需要自动修改相邻/间隔的时间通道的值
        pNewStateTestResult->__UpdateGooseChannelTime(m_stManualTestStartTime);
    }
    // 设置当前正在发送第几个状态
    m_iCurStateIndex = m_iCurStateIndex + 1;

    // 设置手动试验GOOSE发送状态为：正在准备发送帧
    __SetManualTestGooseSendStatus(SEND_STATUS_PREPARE);
    m_criticalFunctionType.Unlock();

    // 更新GOOSE发送帧
    bRet = __UpdateGooseSend(FUNCTION_TYPE_MANUAL_TEST, pNewStateTestResult);

    // 设置测试完成标识
    pNewStateTestResult->SetTested(bRet);

    // 设置手动试验GOOSE发送状态为：正在发送
    __SetManualTestGooseSendStatus(SEND_STATUS_SENDING);
    return bRet;
}

///
/// @brief
///    停止手动试验GOOSE测试
///
/// @return
///     bool    成功返回true
///
bool CFrameSend::StopManualTestGoose()
{
    // 检查是否已初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::StopManualTestGoose(): uninit \n"));
        return false;
    }

    // 检查当前是否正在组帧，如果正在组帧，设置取消组帧标识，等待组帧结束
    m_criticalFunctionType.Lock();
    if (SEND_STATUS_PREPARE == GetManualTestGooseSendStatus())
    {
        // 置位取消组帧标志
        m_eventCancelGoose.SetEvent();

        // 一直等待，直到Goose的发送类型不为：准备发送
        while(SEND_STATUS_PREPARE == GetManualTestGooseSendStatus())
        {
            Sleep(10);
        }

        // 复位取消组帧标志
        m_eventCancelGoose.ResetEvent();
    }
    m_criticalFunctionType.Unlock();

    //
    // 停止GOOSE发送
    //
    if (__StopGooseSend())
    {
        // 如果SMV发送也处于停止状态，那么停止GOOSE接收线程
        m_criticalFunctionType.Lock();
        if (SEND_STATUS_STOPPED == GetManualTestSMVSendStatus())
        {
            CDsmDataLayer* pDsmDataLayer = CDsmDataLayer::getInstance();
            if (pDsmDataLayer)
            {
                pDsmDataLayer->StopRecvData();
            }

            // 设置手动试验测试状态为：停止
            __SetCurFunctionType(FUNCTION_TYPE_UNKNOWN);
        }
        m_criticalFunctionType.Unlock();

        // 设置手动试验GOOSE发送状态为：停止发送
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
///     获得手动实验的开始时间
///
/// @return
///     CEventTime    开始时间
///
CEventTime CFrameSend::GetManualTestStartTime()
{
    return m_stManualTestStartTime;
}

///
/// @brief
///     获得状态序列测试的开始时间
///
/// @return
///     CEventTime    开始时间
///
CEventTime CFrameSend::GetStateSequenceStartTime()
{
    return m_stStateSequenceStartTime;
}

///
/// @brief
///    获得第一个含有SMV虚拟通道信息的手动试验结果集对象
///
/// @return
///     CStateTestResult*    指向结果集对象的指针，失败时返回NULL
///
CStateTestResult* CFrameSend::FirstManualTestSMVResult()
{
    std::vector<CStateTestResult*>::iterator    iter;
    CStateTestResult*                           pTestResult = NULL;


    // 遍历结果集容器，查找第一个含有SMV虚拟通道信息的结果集对象
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
///    获得下一个含有SMV虚拟通道信息的手动试验结果集对象
///
/// @param[in]  pCurTestResult    指向当前结果集对象的指针
///
/// @return
///     CStateTestResult*    指向下一个结果集对象的指针，失败时返回NULL
///
CStateTestResult* CFrameSend::NextManualTestSMVResult( CStateTestResult* pCurTestResult )
{
    std::vector<CStateTestResult*>::iterator    iter;
    CStateTestResult*                           pNextTestResult = NULL;


    iter = m_vecManualTestResult.begin();
    while (m_vecManualTestResult.end() != iter)
    {
        // 查找当前结果集对象的位置
        if (*iter == pCurTestResult)
        {
            // 获得下一个结果集对象的位置
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
///    获得上一个含有SMV虚拟通道信息的手动试验结果集对象
///
/// @param[in]  pCurTestResult    指向当前结果集对象的指针
///
/// @return
///     CStateTestResult*    指向上一个结果集对象的指针，失败时返回NULL
///
CStateTestResult* CFrameSend::PrevManualTestSMVResult( CStateTestResult* pCurTestResult )
{
    std::vector<CStateTestResult*>::reverse_iterator    rIter;
    CStateTestResult*                                   pPrevTestResult = NULL;


    rIter = m_vecManualTestResult.rbegin();
    while (m_vecManualTestResult.rend() != rIter)
    {
        // 查找当前结果集对象的位置
        if (*rIter == pCurTestResult)
        {
            // 获得上一个结果集对象的位置
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
///    获得最后一个含有SMV虚拟通道信息的手动试验结果集对象
///
/// @return
///     CStateTestResult*    指向结果集对象的指针，失败时返回NULL
///
CStateTestResult* CFrameSend::LastManualTestSMVResult()
{
    std::vector<CStateTestResult*>::reverse_iterator    rIter;
    CStateTestResult*                                   pTestResult = NULL;


    // 遍历结果集容器，查找最后一个含有SMV虚拟通道信息的结果集对象
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
///    获得第一个含有GOOSE控制块的手动试验结果集对象
///
/// @return
///     CStateTestResult*    指向结果集对象的指针，失败时返回NULL
///
CStateTestResult* CFrameSend::FirstManualTestGooseResult()
{
    std::vector<CStateTestResult*>::iterator    iter;
    CStateTestResult*                           pTestResult = NULL;


    // 遍历结果集容器，查找第一个含有SMV虚拟通道信息的结果集对象
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
///    获得下一个含有GOOSE控制块的手动试验结果集对象
///
/// @param[in]  pCurTestResult    指向当前结果集对象的指针
///
/// @return
///     CStateTestResult*    指向下一个结果集对象的指针，失败时返回NULL
///
CStateTestResult* CFrameSend::NextManualTestGooseResult( CStateTestResult* pCurTestResult )
{
    std::vector<CStateTestResult*>::iterator    iter;
    CStateTestResult*                           pNextTestResult = NULL;


    iter = m_vecManualTestResult.begin();
    while (m_vecManualTestResult.end() != iter)
    {
        // 查找当前结果集对象的位置
        if (*iter == pCurTestResult)
        {
            // 获得下一个结果集对象的位置
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
///    获得上一个含有GOOSE控制块的手动试验结果集对象
///
/// @param[in]  pCurTestResult    指向当前结果集对象的指针
///
/// @return
///     CStateTestResult*    指向上一个结果集对象的指针，失败时返回NULL
///
CStateTestResult* CFrameSend::PrevManualTestGooseResult( CStateTestResult* pCurTestResult )
{
    std::vector<CStateTestResult*>::reverse_iterator    rIter;
    CStateTestResult*                                   pPrevTestResult = NULL;


    rIter = m_vecManualTestResult.rbegin();
    while (m_vecManualTestResult.rend() != rIter)
    {
        // 查找当前结果集对象的位置
        if (*rIter == pCurTestResult)
        {
            // 获得上一个结果集对象的位置
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
///    获得最后一个含有GOOSE控制块的手动试验结果集对象
///
/// @return
///     CStateTestResult*    指向结果集对象的指针，失败时返回NULL
///
CStateTestResult* CFrameSend::LastManualTestGooseResult()
{
    std::vector<CStateTestResult*>::reverse_iterator    rIter;
    CStateTestResult*                                   pTestResult = NULL;


    // 遍历结果集容器，查找最后一个含有SMV虚拟通道信息的结果集对象
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
///    获得当前的FPGA时间
///
/// @param[out]  stCurFpgaTime    存放FPGA时间的对象
///
/// @return
///     bool    成功返回true
///
bool CFrameSend::GetCurrentFpgaTime(CEventTime& stCurFpgaTime)
{
    //  获得当前FPGA时间
    UTC_TIME_t stUtcTime;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_GET_UTC_TIME, NULL, NULL, &stUtcTime, sizeof(stUtcTime), NULL ,NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::GetCurrentFpgaTime(): IOCTRL_GET_UTC_TIME failed \n"));
        return false;
    }

    // 合成当前FPGA时间
    stCurFpgaTime.ts_s  = stUtcTime.uSecondSinceEpoch;
    stCurFpgaTime.ts_ns = stUtcTime.uFractionOfSecond * 1000;
    stCurFpgaTime.ts_q  = 0;

    return true;
}

///
/// @brief
///    分配SMV控制块到各个发送端口
///
/// @return
///     bool    成功返回true
///
bool CFrameSend::__DistributeSmvCbs()
{
    // 清空各个物理端口的待发送控制块容器
    m_pFrameNetA->RemoveAllSMVSendCbs();
    m_pFrameNetB->RemoveAllSMVSendCbs();
    m_pFrameNetC->RemoveAllSMVSendCbs();
    m_pFrameFt3C->SetSMVSendCb(NULL);

    // 获得SMV发送参数配置管理器
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__DistributeSmvCbs(): Get CParamConfigSMVSend object failed \n"));
        return false;
    }

    //
    // 将SMV控制块添加到各个物理端口
    //
    enumFrameType eFrameType = pParamConfigSMVSend->GetSMVType();
    switch (eFrameType)
    {
    case FRAME_TYPE_NET_9_1:                                            ///< IEC61850-9-1
        {
            // 遍历IEC61850-9-1控制块
            CIEC9_1Cb* pIEC9_1Cb = pParamConfigSMVSend->FirstIEC9_1Cb();
            while(pIEC9_1Cb)
            {
                // 如果此控制块被勾选
                if (pIEC9_1Cb->IsSelected() && (pIEC9_1Cb->GetChannelCount() > 0))
                {
                    // 获得控制块的发送端口
                    DsmPortName ePortName = pIEC9_1Cb->GetPortName();
                    switch(ePortName)
                    {
                    case NET_PORT_LIGHT_A:                              ///< 以太网光口A
                        {
                            m_pFrameNetA->AddSMVSendCb(pIEC9_1Cb);
                        }
                        break;

                    case NET_PORT_LIGHT_B:                              ///< 以太网光口B
                        {
                            m_pFrameNetB->AddSMVSendCb(pIEC9_1Cb);
                        }
                        break;

                    case NET_PORT_ELECTRONIC:                           ///< 电以太网口
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
            // 遍历IEC61850-9-2控制块
            CIEC9_2Cb* pIEC9_2Cb = pParamConfigSMVSend->FirstIEC9_2Cb();
            while(pIEC9_2Cb)
            {
                // 如果此控制块被勾选
                if (pIEC9_2Cb->IsSelected() && (pIEC9_2Cb->GetChannelCount() > 0))
                {
                    // 获得控制块的发送端口
                    DsmPortName ePortName = pIEC9_2Cb->GetPortName();
                    switch(ePortName)
                    {
                    case NET_PORT_LIGHT_A:                              ///< 以太网光口A
                        {
                            m_pFrameNetA->AddSMVSendCb(pIEC9_2Cb);
                        }
                        break;

                    case NET_PORT_LIGHT_B:                              ///< 以太网光口B
                        {
                            m_pFrameNetB->AddSMVSendCb(pIEC9_2Cb);
                        }
                        break;

                    case NET_PORT_ELECTRONIC:                           ///< 电以太网口
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
            // 遍历FT3控制块
            CFT3Cb* pFt3Cb = pParamConfigSMVSend->FirstFt3Cb();
            while(pFt3Cb)
            {
                // 如果此控制块被勾选
                if (pFt3Cb->IsSelected() && (pFt3Cb->GetChannelCount() > 0))
                {
                    // 获得控制块的发送端口
                    DsmPortName ePortName = pFt3Cb->GetPortName();
                    switch(ePortName)
                    {
                    case FT3_PORT_LIGHT_C:                              ///< 光串口C（发送）
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

    case FRAME_TYPE_FT3_GUOWANG:                                        ///< FT3扩展
        {
            // 遍历FT3扩展控制块
            CFT3ExpandCb* pFt3ExpandCb = pParamConfigSMVSend->FirstFt3ExpandCb();
            while(pFt3ExpandCb)
            {
                // 如果此控制块被勾选
                if (pFt3ExpandCb->IsSelected() && (pFt3ExpandCb->GetChannelCount() > 0))
                {
                    // 获得控制块的发送端口
                    DsmPortName ePortName = pFt3ExpandCb->GetPortName();
                    switch(ePortName)
                    {
                    case FT3_PORT_LIGHT_C:                              ///< 光串口C（发送）
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
///    分配GOOSE控制块到各个发送端口
///
/// @return
///     bool    成功返回true
///
bool CFrameSend::__DistributeGooseCbs()
{
    // 清空各个物理端口的待发送控制块容器
    m_pFrameNetA->RemoveAllGOOSESendCbs();
    m_pFrameNetB->RemoveAllGOOSESendCbs();
    m_pFrameNetC->RemoveAllGOOSESendCbs();

    // 获得SMV发送参数配置管理器
    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__DistributeGooseCbs(): Get CParamConfigGOOSESend object failed \n"));
        return false;
    }

    // 遍历控制块
    CGooseCb* pGooseCb = pParamConfigGooseSend->FirstGocb();
    while(pGooseCb)
    {
        // 如果此控制块被勾选
        if (pGooseCb->IsSelected() && (pGooseCb->GetChannelCount() > 0))
        {
            // 获得控制块的发送端口
            DsmPortName ePortName = pGooseCb->GetPortName();
            switch(ePortName)
            {
            case NET_PORT_LIGHT_A:                                      ///< 以太网光口A
                {
                    m_pFrameNetA->AddGooseSendCb(pGooseCb);
                }
                break;

            case NET_PORT_LIGHT_B:                                      ///< 以太网光口B
                {
                    m_pFrameNetB->AddGooseSendCb(pGooseCb);
                }
                break;

            case NET_PORT_ELECTRONIC:                                   ///< 电以太网口
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
///    分配GOOSE控制块到各个发送端口
///
/// @param[in]  pStateTestResult    指向结果集对象的指针，它保存了待发送的GOOSE控制块
///
/// @return
///     bool    成功返回true
///
bool CFrameSend::__DistributeGooseCbs( CStateTestResult* pStateTestResult )
{
    // 检查输入参数
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__DistributeGooseCbs(): Parameter error \n"));
        return false;
    }

    // 清空各个物理端口的待发送控制块容器
    m_pFrameNetA->RemoveAllGOOSESendCbs();
    m_pFrameNetB->RemoveAllGOOSESendCbs();
    m_pFrameNetC->RemoveAllGOOSESendCbs();

    // 遍历控制块
    CGooseCb* pGooseCb = pStateTestResult->FirstGooseSendCb();
    while(pGooseCb)
    {
        // 如果此控制块被勾选
        if (pGooseCb->IsSelected() && (pGooseCb->GetChannelCount() > 0))
        {
            // 获得控制块的发送端口
            DsmPortName ePortName = pGooseCb->GetPortName();
            switch(ePortName)
            {
            case NET_PORT_LIGHT_A:                                      ///< 以太网光口A
                {
                    m_pFrameNetA->AddGooseSendCb(pGooseCb);
                }
                break;

            case NET_PORT_LIGHT_B:                                      ///< 以太网光口B
                {
                    m_pFrameNetB->AddGooseSendCb(pGooseCb);
                }
                break;

            case NET_PORT_ELECTRONIC:                                   ///< 电以太网口
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
///    添加一条日志记录
///
/// @param[in]  strText    函数名
/// @param[in]  ulTime    时间值
///
void CFrameSend::AddLogs( std::string strText, unsigned long ulTime )
{
    m_listTextTime.push_back(std::make_pair(strText, ulTime));
}

///
/// @brief
///    刷新缓冲区的日志记录到日志文件中
///
/// @return
///     bool    成功返回true
///
bool CFrameSend::FlushLogs()
{
    uint32_t    uiCurTime;
    uint32_t    uiPrevTime;
    char    cBuffer[101]    = {0};
    char*   pEndString      = "**********************end**********************";
    std::vector<std::pair<std::string, unsigned long> >::iterator    iter;


    // 检查记录数
    if (0 == m_listTextTime.size())
    {
        return true;
    }

    // 打开日志文件
    FILE* pLogFile = fopen("\\FlashDisk2\\DSTester\\time_log.txt", "a+");
    if (pLogFile == NULL)
    {
        return false;
    }

    // 向日志文件中添加数据
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

    // 刷新、关闭日志文件
    fwrite(pEndString, strlen(pEndString), 1, pLogFile);
    fflush(pLogFile);
    fclose(pLogFile);
    return true;
}

///
/// @brief
///    计算控制块中所有通道的偏移量
///
/// @param[in]  pStateTestResult    指向结果集对象的指针
///
/// @return
///     bool    成功返回true
///
bool CFrameSend::__CalcChannelOffset( CStateTestResult* pStateTestResult )
{
    std::list<CStateSendBuffer*>::iterator  iter;
    CStateSendBuffer*                       pStateSendBuffer;


    // 检查输入参数
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__CalcChannelOffset(): Parameter error \n"));
        return false;
    }

    // 遍历每一个端口的发送缓冲
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
///     查找相似的SMV发送状态，并返回该状态对应的发送缓冲
///
/// @param[in]  pCurStateTestResult 当前SMV发送状态对应的测试结果集
/// @param[in]  iStateIndex         当前SMV发送状态的索引号
///
/// @return
///     CStateTestResult*   相似发送状态的测试结果集，失败时返回NULL
///
CStateTestResult* CFrameSend::__FindSimilarSmvTestResult( CStateTestResult* pCurStateTestResult, int iStateIndex )
{
    int                 i                       = 0;
    bool                bSimilar                = false;
    CStatus*            pOldStatus              = NULL;
    CStateTestResult*   pSimilarStateTestResult = NULL;


    // 获得状态序列配置管理器
    CParamConfigStateSequence* pParamConfigStateSequence = CParamConfigStateSequence::getInstance();
    if (NULL == pParamConfigStateSequence)
    {
        return NULL;
    }

    // 查找相同的SMV发送状态
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

    // 如果存在相同的SMV发送状态
    if (bSimilar)
    {
        // 获得相似的SMV发送缓冲
        if (i >= (int)m_vecStateSequenceResult.size())
        {
            return NULL;
        }

        pSimilarStateTestResult = m_vecStateSequenceResult.at(i);
    }

    return pSimilarStateTestResult;
}
