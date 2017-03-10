/// @file
///
/// @brief
///     网络数据发送类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.3.23
///
/// 修改历史：
///     Modified by:    chao
///     Modified date:  2013.6.19
///     Version:        1.0.1
///     Descriptions:   添加了发送功能
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
///    构造函数
///
/// @param[in]  pFrameSend    数据发送管理器
/// @param[in]  hGPMDevice    GPMC总线句柄
/// @param[in]  enPortName    Net实例对应的实际端口
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
///    析构函数
///
CFrameSendNet::~CFrameSendNet(void)
{
    UnInitNet();
}

///
/// @brief
///    初始化以太网数据发送，打开文件映射
///
bool CFrameSendNet::InitNet( const wchar_t *pNotifyEvtName, const wchar_t *pMapFileName, uint32_t nMapFileLen )
{
    // 判断是否已经初始化
    if (m_bIsInited)
    {
        return true;
    }

    // 检查GPM驱动句柄
    if (INVALID_HANDLE_VALUE == m_hGPMDevice)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameSendNet::InitNet(): m_hGPMDevice INVALID_HANDLE_VALUE \n")));
        return false;
    }

    // 创建通道到的内存映射文件
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

    // 获得映射文件的内存指针
    m_pMapHeader = (Map_File_Header*)MapViewOfFile(m_hFileMaping, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
    if (NULL == m_pMapHeader)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameSendNet::InitNet(): FileMapping for Port %d map failed"), m_enPortName));
        goto exitCode;
    }
    m_pMapDataBuffer    = (unsigned char*)m_pMapHeader;
    m_pMapDataBuffer   += sizeof(Map_File_Header);

    // 创建IEC61850-9-1采样值编解码对象
    m_pSav91Msg = CreateSav91MsgImpl();
    if (NULL == m_pSav91Msg)
    {
        RETAILMSG(OUTPUT_LOG_MSG,(_T("CFrameSendNet::InitNet(): Create IEC61850-9-1 frame manager falied \n")));
        goto exitCode;
    }

    // 创建IEC61850-9-2采样值编解码对象
    m_pSav92Msg = CreateSav92MsgImpl();
    if (NULL == m_pSav92Msg)
    {
        RETAILMSG(OUTPUT_LOG_MSG,(_T("CFrameSendNet::InitNet(): Create IEC61850-9-2 frame manager falied \n")));
        goto exitCode;
    }

    // 创建GOOSE报文的编解码对象
    m_pGooseMsg = CreateGooseMsgImpl();
    if (NULL == m_pGooseMsg)
    {
        RETAILMSG(OUTPUT_LOG_MSG,(_T("CFrameSendNet::InitNet(): Create GOOSE frame manager falied \n")));
        goto exitCode;
    }

    // 标记为已成功初始化
    m_bIsInited = true;
    return true;

exitCode:
    UnInitNet();
    return false;
}

///
/// @brief
///    反初始化以太网发送，解除文件映射
///
void CFrameSendNet::UnInitNet()
{
    // 停止底层SMV数据发送
    StopSMVSend();

    // 停止底层GOOSE数据发送
    StopGooseSend();

    // 取消映射关系
    if (m_pMapHeader != NULL)
    {
        UnmapViewOfFile(m_pMapHeader );
        m_pMapHeader  = NULL;
    }

    // 关闭内存映射文件
    if (m_hFileMaping != NULL)
    {
        CloseHandle(m_hFileMaping);
        m_hFileMaping = NULL;
    }

    // 释放IEC61850-9-1帧管理器
    if (NULL != m_pSav91Msg)
    {
        ReleaseSav91MsgImpl(m_pSav91Msg);
        m_pSav91Msg = NULL;
    }

    // 释放IEC61850-9-2帧管理器
    if (NULL != m_pSav92Msg)
    {
        ReleaseSav92MsgImpl(m_pSav92Msg);
        m_pSav92Msg = NULL;
    }

    // 释放GOOSE帧管理器
    if (NULL != m_pGooseMsg)
    {
        ReleaseGooseMsgImpl(m_pGooseMsg);
        m_pGooseMsg = NULL;
    }
}

///
/// @brief
///    预先计算端口的SMV发送帧
///
/// @param[in]  stDspRecvData    虚拟通道一个完整波形的采样值
/// @param[out] pStateTestResult 保存待发送数据帧的结构体
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::PrepareSmvSendBuffer( CDspRecvData& stDspRecvData, CStateTestResult* pStateTestResult )
{
    uint32_t        uiAsduCount         = 0;
    uint32_t        uiFramesTotalLen    = 0;
    uint32_t        uiSmvSendCbCount    = 0;
    enumErrorCode   eRet;


    // 检查输入参数
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareSmvSendBuffer(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }

    // 如果没有待发送的控制块，那么直接返回
    if (m_vecSMVSendCbs.size() <= 0)
    {
        return ERROR_CODE_SUCCESS;
    }
    uiSmvSendCbCount = (uint32_t)m_vecSMVSendCbs.size();

    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareSmvSendBuffer(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得SMV发送参数配置管理器
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareSmvSendBuffer(): Get CParamConfigSMVSend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得每帧报文的ASDU数
    uiAsduCount = pParamConfigSMVSend->GetASDUCount();
    if ((uiAsduCount <= 0) || (uiAsduCount > MAX_ASDU_NUM))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareSmvSendBuffer(): ASDU count error \n"));
        return ERROR_CODE_FAILURE;
    }

    // 添加待发送缓冲
    CStateSendBuffer*   pNewSendBuffer = new CStateSendBuffer(m_enPortName);
    if (NULL == pNewSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareSmvSendBuffer(): new CStateSendData failed \n"));
        return ERROR_CODE_FAILURE;
    }
    pNewSendBuffer->m_stIOCtrlParam.nDeviceItem = m_enPortName;
    pNewSendBuffer->m_stIOCtrlParam.nInBuf[0]   = 0;
    pNewSendBuffer->m_stIOCtrlParam.nInBuf[1]   = (uiAsduCount<<16) | uiSmvSendCbCount; ///< 控制块数，ASDU数
    pNewSendBuffer->m_stIOCtrlParam.nInBuf[3]   = pParamConfigSMVSend->GetFrequency();  ///< 采样率
    pStateTestResult->m_listSmvSendBuffers.push_back(pNewSendBuffer);

    // 填充待发送缓冲区
    if (FRAME_TYPE_NET_9_1 == pParamConfigSMVSend->GetSMVType())
    {
        // 将IEC61850-9-1控制块内容组帧，填充到内存映射缓冲区
        eRet = __FillIec91SendBuffer(m_pMapDataBuffer + NET_SMV_SEND_LOOP_BUF0_POS, NET_SMV_SEND_BUF_LEN, stDspRecvData, pNewSendBuffer->m_stIOCtrlParam, uiFramesTotalLen);
        if (ERROR_CODE_SUCCESS != eRet)
        {
            return eRet;
        }
    }
    else if (FRAME_TYPE_NET_9_2 == pParamConfigSMVSend->GetSMVType())
    {
        // 将IEC61850-9-2控制块内容组帧，填充到内存映射缓冲区
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

    // 申请缓冲
    if (NULL == pNewSendBuffer->AllocDataBuffer(uiFramesTotalLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareSmvSendBuffer(): Alloc Data Buffer failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // 备份所有发送帧
    if (0 != memcpy_s(pNewSendBuffer->m_pucDataBuffer, uiFramesTotalLen, m_pMapDataBuffer + NET_SMV_SEND_LOOP_BUF0_POS, uiFramesTotalLen))
    {
        return ERROR_CODE_FAILURE;
    }

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    发送SMV控制块
///
/// @param[in]  stDspRecvData    虚拟通道一个完整波形的采样值
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::SendSMVCbs(CDspRecvData& stDspRecvData)
{
    uint32_t                            uiASDUCount;
    uint32_t                            uiFramesTotalLen;
    uint32_t                            uiSMVSendSbCount;
    IOCTRL_IN_PARAM_NET_BIG             stMuSendParam;
    enumErrorCode                       eRet;


    // 如果没有待发送的控制块，那么直接返回
    if (m_vecSMVSendCbs.size() <= 0)
    {
        return ERROR_CODE_SUCCESS;
    }
    uiSMVSendSbCount = (uint32_t)m_vecSMVSendCbs.size();

    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得SMV发送参数配置管理器
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): Get CParamConfigSMVSend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得每帧报文的ASDU数
    uiASDUCount = pParamConfigSMVSend->GetASDUCount();
    if ((uiASDUCount <= 0) || (uiASDUCount > MAX_ASDU_NUM))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): ASDU count error \n"));
        return ERROR_CODE_FAILURE;
    }

    stMuSendParam.nDeviceItem   = m_enPortName;                         ///< 设备号
    stMuSendParam.nInBuf[0]     = 0;                                    ///< SMV
    stMuSendParam.nInBuf[1]     = (uiASDUCount<<16) | uiSMVSendSbCount; ///< 控制块数，ASDU数
    stMuSendParam.nInBuf[3]     = pParamConfigSMVSend->GetFrequency();  ///< 采样率

    // 填充待发送缓冲区
    m_pMapHeader->NetHeader.nCurSmvTxBufItem = 0;                       ///< 当前Smv发送缓冲区的序号
    if (FRAME_TYPE_NET_9_1 == pParamConfigSMVSend->GetSMVType())
    {
        // 将IEC61850-9-1控制块内容组帧，填充到内存映射缓冲区
        eRet = __FillIec91SendBuffer(m_pMapDataBuffer + NET_SMV_SEND_LOOP_BUF0_POS, NET_SMV_SEND_BUF_LEN, stDspRecvData, stMuSendParam, uiFramesTotalLen);
        if (ERROR_CODE_SUCCESS != eRet)
        {
            return eRet;
        }
    }
    else if (FRAME_TYPE_NET_9_2 == pParamConfigSMVSend->GetSMVType())
    {
        // 将IEC61850-9-2控制块内容组帧，填充到内存映射缓冲区
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

    // 设置IOCTRL SEND HEADER（手动试验模式）
    stMuSendParam.nInBuf[TX_TEST_STYLE_INDEX] = TX_TEST_STYLE_MANUAL_TEST;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_HEADER, &stMuSendParam, sizeof(stMuSendParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): Port%d IOCTRL_SEND_HEADER failed \n", m_enPortName));
        return ERROR_CODE_FAILURE;
    }

    // 检查用户是否临时取消了发送事务
    if (m_pDataSendManager->m_eventCancelSMV.Lock(0))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): User cancels the current operation \n"));
        return ERROR_CODE_CANCEL;
    }

    // 开始发送数据
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
///    发送缓冲区中的数据帧
///
/// @param[in]  pStateSendBuffer    指向缓冲区的指针
///
/// @return
///     bool    成功返回true
///
bool CFrameSendNet::SendSMVCbs( CStateSendBuffer* pStateSendBuffer )
{
    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): uninit \n"));
        return false;
    }

    // 检查输入参数
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): Parameter error \n"));
        return false;
    }

    // 填充待发送缓冲区
    m_pMapHeader->NetHeader.nCurSmvTxBufItem = 0;                       ///< 当前Smv发送缓冲区的序号
    if (0 != memcpy_s(m_pMapDataBuffer + NET_SMV_SEND_LOOP_BUF0_POS, NET_SMV_SEND_BUF_LEN, pStateSendBuffer->m_pucDataBuffer, pStateSendBuffer->m_uiDataBufferLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): Fill send buffer failed"));
        return false;
    }

    // 设置IOCTRL SEND HEADER（设置为状态序列模式）
    pStateSendBuffer->m_stIOCtrlParam.nInBuf[TX_TEST_STYLE_INDEX] = TX_TEST_STYLE_STATUS_SEQUENCE;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_HEADER, &pStateSendBuffer->m_stIOCtrlParam, sizeof(pStateSendBuffer->m_stIOCtrlParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendSMVCbs(): Port%d IOCTRL_SEND_HEADER failed \n", m_enPortName));
        return false;
    }

    // 开始发送数据
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
///    准备GOOSE控制块发送帧
///
/// @param[in]  pStateTestResult    指向结果集对象的指针，用于保存GOOSE发送帧
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::PrepareGooseSendBuffer( CStateTestResult* pStateTestResult )
{
    uint32_t    uiFramesTotalLen;


    // 检查输入参数
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareGooseSendBuffer(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }

    // 如果没有待发送的控制块，那么直接返回
    if (m_vecGOOSESendCbs.size() <= 0)
    {
        return ERROR_CODE_SUCCESS;
    }

    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareGooseSendBuffer(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得GOOSE发送参数配置管理器
    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::__PrepareGooseSendBuffer(): Get CParamConfigGOOSESend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // 添加待发送缓冲
    CStateSendBuffer*   pNewSendBuffer = new CStateSendBuffer(m_enPortName);
    if (NULL == pNewSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareGooseSendBuffer(): new CStateSendData failed \n"));
        return ERROR_CODE_FAILURE;
    }
    pNewSendBuffer->m_stIOCtrlParam.nDeviceItem = m_enPortName;                                 ///< 设备号
    pNewSendBuffer->m_stIOCtrlParam.nInBuf[0]   = 1;                                            ///< GOOSE
    pNewSendBuffer->m_stIOCtrlParam.nInBuf[1]   = pParamConfigGooseSend->GetHeartbeatInterval();///< T0 值
    pNewSendBuffer->m_stIOCtrlParam.nInBuf[2]   = pParamConfigGooseSend->GetMinimumInterval();  ///< T1 值
    pNewSendBuffer->m_stIOCtrlParam.nInBuf[3]   = (uint32_t)m_vecGOOSESendCbs.size();           ///< 控制块数
    pStateTestResult->m_listGooseSendBuffers.push_back(pNewSendBuffer);

    // 填充待发送缓冲区
    enumErrorCode   eRet = __FillGooseSendBuffer(m_pMapDataBuffer + GOOSE_SEND_LOOP_BUF0_POS, GOOSE_SEND_BUF_LEN, pNewSendBuffer, uiFramesTotalLen);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        return eRet;
    }

    // 申请缓冲
    if (NULL == pNewSendBuffer->AllocDataBuffer(uiFramesTotalLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareGooseSendBuffer(): Alloc Data Buffer failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // 备份所有发送帧
    if (0 != memcpy_s(pNewSendBuffer->m_pucDataBuffer, uiFramesTotalLen, m_pMapDataBuffer + GOOSE_SEND_LOOP_BUF0_POS, uiFramesTotalLen))
    {
        return ERROR_CODE_FAILURE;
    }

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    准备GOOSE控制块更新帧
///
/// @param[in]  pStateSendBuffer    指向GOOSE发送帧
///
/// @return
///     bool    成功返回true
///
bool CFrameSendNet::PrepareGooseUpdateBuffer( CStateSendBuffer* pStateSendBuffer )
{
    // 检查输入参数
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareGooseUpdateBuffer(): Parameter error \n"));
        return false;
    }

    // 如果没有待发送的控制块，那么直接返回
    if (m_vecGOOSESendCbs.size() <= 0)
    {
        return true;
    }

    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__PrepareGooseUpdateBuffer(): uninit \n"));
        return false;
    }

    //
    // 设置变位标记
    //
    int                                 iCbIndex;                       ///< 控制块计数变量
    std::vector<CGooseCb*>::iterator    iter;
    pStateSendBuffer->m_stIOCtrlParam.nDeviceItem = m_enPortName;       ///< 设备号
    pStateSendBuffer->m_stIOCtrlParam.nInBuf[0]   = 1;                  ///< GOOSE
    pStateSendBuffer->m_stIOCtrlParam.nInBuf[1]   = 0;                  ///< GOOSE变位标记
    for (iCbIndex = 0, iter = m_vecGOOSESendCbs.begin(); iter != m_vecGOOSESendCbs.end(); iCbIndex++, iter++)
    {
        CGooseCb* pGooseCb = (CGooseCb*)*iter;
        if (pGooseCb && pGooseCb->IsSendValueChanged())                 ///< GOOSE通道值发生了改变
        {
            // 哪一个GOOSE控制块发生变位了，需要给对应的那位置1
            pStateSendBuffer->m_stIOCtrlParam.nInBuf[1] |= (1 << iCbIndex);

            // 增加GOOSE控制块的stNum
            pGooseCb->SetStNum(pGooseCb->GetStNum() + 1);
        }
    }

    // 对发生了变位的控制块进行重新编码
    if (!pStateSendBuffer->RecodeChangedGooseCb())
    {
        return false;
    }

    // 备份所有发送帧
    return true;
}

///
/// @brief
///    发送GOOSE控制块
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::SendGooseCbs()
{
    IOCTRL_IN_PARAM_NET_BIG             stMuSendParam;
    enumErrorCode                       eRet;


    // 如果没有待发送的控制块，那么直接返回
    if (m_vecGOOSESendCbs.size() <= 0)
    {
        return ERROR_CODE_SUCCESS;
    }

    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得GOOSE发送参数配置管理器
    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSend::SendGooseCbs(): Get CParamConfigGOOSESend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    stMuSendParam.nDeviceItem   = m_enPortName;                                     ///< 设备号
    stMuSendParam.nInBuf[0]     = 1;                                                ///< GOOSE
    stMuSendParam.nInBuf[1]     = pParamConfigGooseSend->GetHeartbeatInterval();    ///< T0 值
    stMuSendParam.nInBuf[2]     = pParamConfigGooseSend->GetMinimumInterval();      ///< T1 值
    stMuSendParam.nInBuf[3]     = (uint32_t)m_vecGOOSESendCbs.size();               ///< 控制块数

    // 获得UTC时间
    UTC_TIME_t stUtcTime;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_GET_UTC_TIME, NULL, NULL, &stUtcTime, sizeof(stUtcTime), NULL ,NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Port%d IOCTRL_GET_UTC_TIME failed \n", m_enPortName));
        return ERROR_CODE_FAILURE;
    }

    // 填充待发送缓冲区
    m_pMapHeader->NetHeader.nCurGooseTxBufItem = 0;
    eRet = __FillGooseSendBuffer(m_pMapDataBuffer + GOOSE_SEND_LOOP_BUF0_POS, GOOSE_SEND_BUF_LEN, stUtcTime, stMuSendParam);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        return eRet;
    }

    // 设置IOCTRL SEND HEADER（设置为手动试验模式）
    stMuSendParam.nInBuf[TX_TEST_STYLE_INDEX] = TX_TEST_STYLE_MANUAL_TEST;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_HEADER, &stMuSendParam, sizeof(stMuSendParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Port%d IOCTRL_SEND_HEADER failed \n", m_enPortName));
        return ERROR_CODE_FAILURE;
    }

    // 检查用户是否临时取消了发送事务
    if (m_pDataSendManager->m_eventCancelSMV.Lock(0))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): User cancels the current operation \n"));
        return ERROR_CODE_CANCEL;
    }

    // 开始发送数据
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
///    发送GOOSE控制块
///
/// @param[in]  pStateSendBuffer    指向缓冲区的指针
///
/// @return
///     bool    成功返回true
///
bool CFrameSendNet::SendGooseCbs( CStateSendBuffer* pStateSendBuffer )
{
    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): uninit \n"));
        return false;
    }

    // 检查输入参数
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Parameter error \n"));
        return false;
    }

    // 填充待发送缓冲区
    m_pMapHeader->NetHeader.nCurGooseTxBufItem = 0;                     ///< 当前Smv发送缓冲区的序号
    if (0 != memcpy_s(m_pMapDataBuffer + GOOSE_SEND_LOOP_BUF0_POS, GOOSE_SEND_BUF_LEN, pStateSendBuffer->m_pucDataBuffer, pStateSendBuffer->m_uiDataBufferLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Fill send buffer failed"));
        return false;
    }

    // 设置IOCTRL SEND HEADER（设置为状态序列模式）
    pStateSendBuffer->m_stIOCtrlParam.nInBuf[TX_TEST_STYLE_INDEX] = TX_TEST_STYLE_STATUS_SEQUENCE;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_HEADER, &pStateSendBuffer->m_stIOCtrlParam, sizeof(pStateSendBuffer->m_stIOCtrlParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Port%d IOCTRL_SEND_HEADER failed \n", m_enPortName));
        return false;
    }

    // 开始发送数据
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
///    停止底层数据发送
///
/// @return
/// true标识停止成功
///
bool CFrameSendNet::StopSMVSend()
{
    IOCTRL_IN_PARAM_NET stIOCtrlInParam;


    stIOCtrlInParam.nDeviceItem     = m_enPortName;                     ///< 设备号
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
///    停止GOOSE底层数据发送
///
bool CFrameSendNet::StopGooseSend()
{
    IOCTRL_IN_PARAM_NET stIOCtrlInParam;


    stIOCtrlInParam.nDeviceItem     = m_enPortName;                     ///< 设备号
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
///    清空保存待发送SMV控制块的容器
///
void CFrameSendNet::RemoveAllSMVSendCbs()
{
    m_vecSMVSendCbs.clear();
}

///
/// @brief
///    添加待发送的SMV控制块
///
/// @param[in]  pNewCb    SMV控制块
///
/// @return
///     bool    添加成功返回true
///
bool CFrameSendNet::AddSMVSendCb( CBaseCb* pNewCb )
{
    // 检查输入参数
    if (NULL == pNewCb)
    {
        return false;
    }

    // 检查是否存在重复的控制块
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

    // 添加控制块指针到容器中
    m_vecSMVSendCbs.push_back(pNewCb);
    return true;
}

///
/// @brief
///    清空保存待发送GOOSE控制块的容器
///
void CFrameSendNet::RemoveAllGOOSESendCbs()
{
    m_vecGOOSESendCbs.clear();
}

///
/// @brief
///    添加待发送的GOOSE控制块
///
/// @param[in]  pNewCb    GOOSE控制块
///
/// @return
///     bool    添加成功返回true
///
bool CFrameSendNet::AddGooseSendCb( CGooseCb* pNewCb )
{
    // 检查输入参数
    if (NULL == pNewCb)
    {
        return false;
    }

    // 检查是否存在重复的控制块
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

    // 添加控制块指针到容器中
    m_vecGOOSESendCbs.push_back(pNewCb);
    return true;
}

///
/// @brief
///    填充报文发送缓冲区（IEC61850-9-1）
///
/// @param[out]  pDataBuffer        待发送缓冲区
/// @param[in]   uiDataBufferLen    发送缓冲区大小（字节）
/// @param[in]   stDspRecvData      虚拟通道一个完整波形的采样值
/// @param[out]  stMuSendParam      网络接口DeviceIoControl()的数据参数结构体
/// @param[out]  uiFramesTotalLen   所有待发送帧的长度和（字节）
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::__FillIec91SendBuffer( unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CDspRecvData& stDspRecvData, IOCTRL_IN_PARAM_NET_BIG& stMuSendParam, uint32_t& uiFramesTotalLen )
{
    unsigned char*      pDataBufferEnd          = NULL;
    uint8_t             ucMacDes[ETHER_MAC_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t             ucMacSrc[ETHER_MAC_LEN] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff};


    // 输入参数检查
    if (NULL == pDataBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): NULL parameter \n"));
        return ERROR_CODE_FAILURE;
    }

    // 检查缓冲区大小
    if (uiDataBufferLen <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): uiDataBufferLen <= 0 \n"));
        return ERROR_CODE_FAILURE;
    }
    pDataBufferEnd = pDataBuffer + uiDataBufferLen;                     ///< 指向缓冲区末尾

    // 检查是否有待发送的控制块
    if (m_vecSMVSendCbs.size() <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): No control block to be transmitted \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得基本参数配置管理器
    CParamConfigBasic* pParamConfigBasic = CParamConfigBasic::getInstance();
    if (NULL == pParamConfigBasic)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): Get CParamConfigBasic object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得SMV发送参数配置管理器
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): Get CParamConfigSMVSend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得SMV类型
    enumFrameType eFrameType = pParamConfigSMVSend->GetSMVType();
    if (FRAME_TYPE_NET_9_1 != eFrameType)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): SMV type error \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得每帧报文的ASDU数
    int iASDUCount = pParamConfigSMVSend->GetASDUCount();
    if (iASDUCount <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): ASDU count error \n"));
        return ERROR_CODE_FAILURE;
    }

    // 如果【采样点数】能整除【ASDU数】，那么设置【每个控制块的帧数】=【采样点数】/【ASDU数】
    // 否则设置【每个控制块的帧数】=【采样点数】
    unsigned int iFrameCount = stDspRecvData.m_iSmvCount;
    if (0 == (stDspRecvData.m_iSmvCount) % iASDUCount)
    {
        iFrameCount = (stDspRecvData.m_iSmvCount) / iASDUCount;
    }
    stMuSendParam.nInBuf[2]     = iFrameCount;                          ///< 一个控制块的总帧数

    CSmvScaler  SmvValueCaler;
    // 设置报文值类型
    SmvValueCaler.SetChannelMsgValType(pParamConfigBasic->GetFrameValueType());
    // 设置通道的自然值类型
    SmvValueCaler.SetChannelNtrValType(pParamConfigSMVSend->GetSVDisplay());

    //
    // 遍历每一组采样值，合成发送报文帧，添加到发送缓冲
    //
    uiFramesTotalLen  = 0;
    for (unsigned int iFrameIndex = 0; iFrameIndex < iFrameCount; iFrameIndex++)
    {
        int                             iCbIndex;
        std::vector<CBaseCb*>::iterator iter;
        for (iCbIndex = 0, iter = m_vecSMVSendCbs.begin(); iter != m_vecSMVSendCbs.end(); iCbIndex++, iter++)
        {
            CIEC9_1Cb* pIEC9_1Cb = (CIEC9_1Cb*)*iter;


            // 检查用户是否临时取消了发送事务
            if (m_pDataSendManager && m_pDataSendManager->m_eventCancelSMV.Lock(0))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): User cancels the current operation \n"));
                return ERROR_CODE_CANCEL;
            }

            ///< 控制块通道数不能为0
            if ((NULL == pIEC9_1Cb) || (0 == pIEC9_1Cb->GetChannelCount()))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): No Channel \n"));
                return ERROR_CODE_FAILURE;
            }

            // 检查通道数是否超限
            if (pIEC9_1Cb->GetChannelCount() > ETHER_IEC91_CHANNEL_MAX)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): Excessive number of channels \n"));
                return ERROR_CODE_FAILURE;
            }

            //
            // 设置控制块编码信息
            //
            m_pSav91Msg->ClearSavPDU();
            pIEC9_1Cb->GetDesMAC(ucMacDes, ETHER_MAC_LEN);
            pIEC9_1Cb->GetSrcMAC(ucMacSrc, ETHER_MAC_LEN);
            m_pSav91Msg->SetMacAddr(ucMacDes, ucMacSrc, ETHER_MAC_LEN,pIEC9_1Cb->GetVlanFlag());
            m_pSav91Msg->Set8021QVlanInfo(pIEC9_1Cb->GetVLanPriority(), pIEC9_1Cb->GetVlanID(),pIEC9_1Cb->GetVlanFlag());
            m_pSav91Msg->SetAppID(pIEC9_1Cb->GetAPPID());

            //
            // 向编码对象添加控制块ASDU信息
            //
            for (int iAsduIndex = 0; iAsduIndex < iASDUCount; iAsduIndex++)
            {
                // 样本计数器
                int iSmvCnt = (iFrameIndex * iASDUCount + iAsduIndex) % (stDspRecvData.m_iSmvCount);

                int     iChannelIndex       = 0;                        ///< 通道索引
                double  dSmvValue           = 0.0f;                     ///< 采样值
                double  dOnceRatings        = 0.0f;                     ///< 一次额定值
                double  dSecondaryRatings   = 0.0f;                     ///< 二次额定值
                for (CChannel* pChannel = pIEC9_1Cb->FirstChannel(); pChannel != NULL; 
                    iChannelIndex++, pChannel = pIEC9_1Cb->NextChannel(pChannel))
                {
                    dSmvValue           = 0.0f;                             ///< 采样值
                    dOnceRatings        = pChannel->GetOnceRatings();       ///< 一次额定值
                    dSecondaryRatings   = pChannel->GetSecondaryRatings();  ///< 二次额定值

                    //
                    // 如果此通道关联了虚拟发送通道，那么设置设置瞬时采样值
                    //
                    std::wstring strChannelName = pChannel->GetVirtualChannel();
                    if (strChannelName.size())
                    {
                        std::map<std::wstring, int>::iterator iter = stDspRecvData.m_mapChannelName2Index.find(strChannelName);
                        if (stDspRecvData.m_mapChannelName2Index.end() != iter)
                        {
                            // 设置瞬时采样值
                            dSmvValue = stDspRecvData.m_stRecvData.fSmvGenData[iter->second][iSmvCnt];
                        }
                    }

                    // 根据通道类型，设置发送帧中的通道值
                    switch (pChannel->GetChannelType())
                    {
                    case CHANNEL_TYPE_UNKOWN:                           ///< 未知类型
                        {
                            m_pusIEC91ValArr[iChannelIndex] = 0;
                        }
                        break;

                    case CHANNEL_TYPE_SMV_CURRENT:                      ///< 电流类型
                        {
                            // 根据瞬时采样值，设置发送帧中通道值
                            m_pusIEC91ValArr[iChannelIndex] = SmvValueCaler.ChannelNtrVal2MsgVal_Sav91(
                                (float)dSmvValue,
                                (float)dOnceRatings,
                                (float)dSecondaryRatings,
                                CHANNEL_TYPE_SMV_CURRENT,
                                (pChannel->IsMeasure() ? CHANNEL_VALUE_TYPE_MEASURE : CHANNEL_VALUE_TYPE_PROTECT)
                                );
                        }
                        break;

                    case CHANNEL_TYPE_SMV_VOLTAGE:                      ///< 电压类型
                        {
                            // 根据瞬时采样值，设置发送帧中通道值
                            m_pusIEC91ValArr[iChannelIndex] = SmvValueCaler.ChannelNtrVal2MsgVal_Sav91(
                                (float)dSmvValue,
                                (float)dOnceRatings,
                                (float)dSecondaryRatings,
                                CHANNEL_TYPE_SMV_VOLTAGE,
                                CHANNEL_VALUE_TYPE_UNKNOWN
                                );
                        }
                        break;

                    case CHANNEL_TYPE_SMV_TIME:                         ///< 时间类型
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

                // 添加ASDU项
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
            // 根据设置的参数进行编码，获得报文帧
            //
            int iByteCount = (int)(sizeof(m_pucFrameBuffer[0]) * m_pSav91Msg->Code(m_pucFrameBuffer, sizeof(m_pucFrameBuffer)));
            if (0 == iByteCount)                                        ///< 检查帧大小
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): Code frame failed \n"));
                return ERROR_CODE_FAILURE;
            }

            // 记录帧长
            if ((4 + iCbIndex) >= sizeof(stMuSendParam.nInBuf)/sizeof(stMuSendParam.nInBuf[0]))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): Record frame length failed\n"));
                return ERROR_CODE_FAILURE;
            }
            stMuSendParam.nInBuf[4 + iCbIndex] = iByteCount;

            //
            // 复制数据帧到发送缓冲区
            //
            if ((pDataBuffer + iByteCount) > pDataBufferEnd)            ///< 判断是否越界
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec91SendBuffer(): Oversize frame \n"));
                return ERROR_CODE_FAILURE;
            }
            memcpy(pDataBuffer, m_pucFrameBuffer, iByteCount);

            // 递增缓冲区指针（4字节对齐）
            iByteCount      = ((iByteCount & 0x03) != 0)? ((iByteCount & 0xFFFC) + 4): iByteCount;
            pDataBuffer    += iByteCount;
            uiFramesTotalLen += iByteCount;
        } // end 控制块
    } // end 采样点

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    填充报文发送缓冲区（IEC61850-9-2）
///
/// @param[out]  pDataBuffer        待发送缓冲区
/// @param[in]   uiDataBufferLen    发送缓冲区大小（字节）
/// @param[in]   stDspRecvData      虚拟通道一个完整波形的采样值
/// @param[out]  stMuSendParam      网络接口DeviceIoControl()的数据参数结构体
/// @param[out]  uiFramesTotalLen   所有待发送帧的长度和（字节）
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::__FillIec92SendBuffer( unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CDspRecvData& stDspRecvData, IOCTRL_IN_PARAM_NET_BIG& stMuSendParam, uint32_t& uiFramesTotalLen )
{
    unsigned char*          pDataBufferEnd          = NULL;
    uint8_t                 ucMacDes[ETHER_MAC_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t                 ucMacSrc[ETHER_MAC_LEN] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff};


    // 输入参数检查
    if (NULL == pDataBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): NULL parameter \n"));
        return ERROR_CODE_FAILURE;
    }

    // 检查缓冲区大小
    if (uiDataBufferLen <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): uiDataBufferLen <= 0 \n"));
        return ERROR_CODE_FAILURE;
    }
    pDataBufferEnd = pDataBuffer + uiDataBufferLen;                     ///< 指向缓冲区末尾

    // 检查是否有待发送的控制块
    if (m_vecSMVSendCbs.size() <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): No control block to be transmitted \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得基本参数配置管理器
    CParamConfigBasic* pParamConfigBasic = CParamConfigBasic::getInstance();
    if (NULL == pParamConfigBasic)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): Get CParamConfigBasic object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得SMV发送参数配置管理器
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): Get CParamConfigSMVSend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得SMV类型
    enumFrameType eFrameType = pParamConfigSMVSend->GetSMVType();
    if (FRAME_TYPE_NET_9_2 != eFrameType)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): SMV type error \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得每帧报文的ASDU数
    int iASDUCount = pParamConfigSMVSend->GetASDUCount();
    if (iASDUCount <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): ASDU count error \n"));
        return ERROR_CODE_FAILURE;
    }

    // 如果【采样点数】能整除【ASDU数】，那么设置【每个控制块的帧数】=【采样点数】/【ASDU数】
    // 否则设置【每个控制块的帧数】=【采样点数】
    unsigned int iFrameCount = stDspRecvData.m_iSmvCount;
    if (0 == (stDspRecvData.m_iSmvCount) % iASDUCount)
    {
        iFrameCount = (stDspRecvData.m_iSmvCount) / iASDUCount;
    }
    stMuSendParam.nInBuf[2]     = iFrameCount;                          ///< 一个控制块的总帧数

    CSmvScaler  SmvValueCaler;
    // 设置报文值类型
    SmvValueCaler.SetChannelMsgValType(pParamConfigBasic->GetFrameValueType());
    // 设置通道的自然值类型
    SmvValueCaler.SetChannelNtrValType(pParamConfigSMVSend->GetSVDisplay());

    //
    // 遍历每一组采样值，合成发送报文帧，添加到发送缓冲
    //
    uiFramesTotalLen  = 0;
    for (unsigned int iFrameIndex = 0; iFrameIndex < iFrameCount; iFrameIndex++)
    {
        int                             iCbIndex;
        std::vector<CBaseCb*>::iterator iter;
        for (iCbIndex = 0, iter = m_vecSMVSendCbs.begin(); iter != m_vecSMVSendCbs.end(); iter++, iCbIndex++)
        {
            CIEC9_2Cb* pIEC9_2Cb = (CIEC9_2Cb*)*iter;


            // 检查用户是否临时取消了发送事务
            if (m_pDataSendManager && m_pDataSendManager->m_eventCancelSMV.Lock(0))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): User cancels the current operation \n"));
                return ERROR_CODE_CANCEL;
            }

            // 控制块通道数不能为0
            if ((NULL == pIEC9_2Cb) || (0 == pIEC9_2Cb->GetChannelCount()))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): No Channel \n"));
                return ERROR_CODE_FAILURE;
            }

            // 检查通道数是否超限
            if (pIEC9_2Cb->GetChannelCount() > ETHER_IEC92_CHANNEL_MAX)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): Excessive number of channels \n"));
                return ERROR_CODE_FAILURE;
            }

            //
            // 设置控制块编码信息
            //
            m_pSav92Msg->ClearSavPDU();
            pIEC9_2Cb->GetDesMAC(ucMacDes, ETHER_MAC_LEN);
            pIEC9_2Cb->GetSrcMAC(ucMacSrc, ETHER_MAC_LEN);
            m_pSav92Msg->SetMacAddr(ucMacDes, ucMacSrc, ETHER_MAC_LEN,pIEC9_2Cb->GetVlanFlag());
            m_pSav92Msg->Set8021QVlanInfo(pIEC9_2Cb->GetVLanPriority(), pIEC9_2Cb->GetVlanID(),pIEC9_2Cb->GetVlanFlag());
            m_pSav92Msg->SetAppID(pIEC9_2Cb->GetAPPID());
			
            // 设置可选项
            SAV92_SAVPDU_OPTS   stSav92Opts;
            stSav92Opts.bDataSetOpt     = pIEC9_2Cb->GetDataSetOpt();
            stSav92Opts.bRefrTmOpt      = pIEC9_2Cb->GetRefrTmOpt();
            stSav92Opts.bSecurityOpt    = pIEC9_2Cb->GetSecurityOpt();
            stSav92Opts.bSmpRateOpt     = pIEC9_2Cb->GetSmpRateOpt();
            m_pSav92Msg->SetSavPduOpts(stSav92Opts);

            //
            // 向编码对象添加控制块ASDU信息
            //
            for (int iAsduIndex = 0; iAsduIndex < iASDUCount; iAsduIndex++)
            {
                // 样本计数器
                int iSmvCnt = (iFrameIndex * iASDUCount + iAsduIndex) % (stDspRecvData.m_iSmvCount);

                int     iChannelIndex       = 0;                        ///< 通道索引
                double  dSmvValue           = 0.0f;                     ///< 采样值
                double  dOnceRatings        = 0.0f;                     ///< 一次额定值
                double  dSecondaryRatings   = 0.0f;                     ///< 二次额定值
                std::list<CChannel *>::iterator iterChannel;
                for (CChannel* pChannel = pIEC9_2Cb->FirstChannel(); pChannel != NULL;
                    iChannelIndex++, pChannel = pIEC9_2Cb->NextChannel(pChannel))
                {
                    dSmvValue           = 0.0f;                             ///< 采样值
                    dOnceRatings        = pChannel->GetOnceRatings();       ///< 一次额定值
                    dSecondaryRatings   = pChannel->GetSecondaryRatings();  ///< 二次额定值

                    //
                    // 如果此通道关联了虚拟发送通道，那么设置设置瞬时采样值
                    //
                    std::wstring strChannelName = pChannel->GetVirtualChannel();
                    if (strChannelName.size())
                    {
                        std::map<std::wstring, int>::iterator iter = stDspRecvData.m_mapChannelName2Index.find(strChannelName);
                        if (stDspRecvData.m_mapChannelName2Index.end() != iter)
                        {
                            // 设置瞬时采样值
                            dSmvValue = stDspRecvData.m_stRecvData.fSmvGenData[iter->second][iSmvCnt];
                        }
                    }

                    // 根据通道类型，设置发送帧中的通道值
                    switch (pChannel->GetChannelType())
                    {
                    case CHANNEL_TYPE_UNKOWN:                           ///< 未知类型
                        {
                            m_puiIEC92ValArr[iChannelIndex] = 0;
                        }
                        break;

                    case CHANNEL_TYPE_SMV_CURRENT:                      ///< 电流类型
                        {
                            // 根据瞬时采样值，设置发送帧中通道值
                            m_puiIEC92ValArr[iChannelIndex] = SmvValueCaler.ChannelNtrVal2MsgVal_Sav92(
                                (float)dSmvValue,
                                (float)dOnceRatings,
                                (float)dSecondaryRatings,
                                CHANNEL_TYPE_SMV_CURRENT
                                );
                        }
                        break;

                    case CHANNEL_TYPE_SMV_VOLTAGE:                      ///< 电压类型
                        {
                            // 根据瞬时采样值，设置发送帧中通道值
                            m_puiIEC92ValArr[iChannelIndex] = SmvValueCaler.ChannelNtrVal2MsgVal_Sav92(
                                (float)dSmvValue,
                                (float)dOnceRatings,
                                (float)dSecondaryRatings,
                                CHANNEL_TYPE_SMV_VOLTAGE
                                );
                        }
                        break;

                    case CHANNEL_TYPE_SMV_TIME:                         ///< 时间类型
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

                    // 通道品质值
                    m_puiIEC92QualityArr[iChannelIndex] = pChannel->GetQuality();
                }

                // 添加ASDU项
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
            // 根据设置的参数进行编码，获得报文帧
            //
            int iByteCount = (int)(sizeof(m_pucFrameBuffer[0]) * m_pSav92Msg->Code(m_pucFrameBuffer, sizeof(m_pucFrameBuffer)));
            if (0 == iByteCount)                                        ///< 检查帧大小
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): Code frame failed \n"));
                return ERROR_CODE_FAILURE;
            }

            // 记录帧长
            if ((4 + iCbIndex) >= sizeof(stMuSendParam.nInBuf)/sizeof(stMuSendParam.nInBuf[0]))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): Record frame length failed\n"));
                return ERROR_CODE_FAILURE;
            }
            stMuSendParam.nInBuf[4 + iCbIndex] = iByteCount;

            //
            // 复制数据帧到发送缓冲区
            //
            if ((pDataBuffer + iByteCount) > pDataBufferEnd)            ///< 判断是否越界
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillIec92SendBuffer(): Oversize frame \n"));
                return ERROR_CODE_FAILURE;
            }
            memcpy(pDataBuffer, m_pucFrameBuffer, iByteCount);

            // 递增缓冲区指针（4字节对齐）
            iByteCount      = ((iByteCount & 0x03) != 0)? ((iByteCount & 0xFFFC) + 4): iByteCount;
            pDataBuffer    += iByteCount;
            uiFramesTotalLen += iByteCount;
        } // end 控制块
    } // end 采样点

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    填充报文发送缓冲区（GOOSE）
///
/// @param[in]  pDataBuffer         待发送缓冲区
/// @param[in]  uiDataBufferLen     发送缓冲区大小（字节）
/// @param[in]  stUtcTime           报文改变时间
/// @param[out] stMuSendParam       网络接口 DeviceIoControl数据参数结构体
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::__FillGooseSendBuffer( unsigned char* pDataBuffer, unsigned int uiDataBufferLen, UTC_TIME_t& stUtcTime, IOCTRL_IN_PARAM_NET_BIG& stMuSendParam )
{
    unsigned char*          pDataBufferEnd          = NULL;
    uint8_t                 ucMacDes[ETHER_MAC_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t                 ucMacSrc[ETHER_MAC_LEN] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff};


    // 输入参数检查
    if (NULL == pDataBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): NULL parameter \n"));
        return ERROR_CODE_FAILURE;
    }

    // 检查缓冲区大小
    if (uiDataBufferLen <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): uiDataBufferLen <= 0 \n"));
        return ERROR_CODE_FAILURE;
    }
    pDataBufferEnd = pDataBuffer + uiDataBufferLen;                     ///< 指向缓冲区末尾

    // 获得GOOSE发送参数配置管理器
    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Get CParamConfigGOOSESend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // 检查是否有待发送的控制块
    if (m_vecGOOSESendCbs.size() <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): No control block to be transmitted \n"));
        return ERROR_CODE_FAILURE;
    }

    //
    // 遍历每一个控制块，合成发送报文帧，添加到发送缓冲
    //
    CSmvScaler                          SmvValueCaler;
    int                                 iCbIndex;                       ///< 控制块计数变量
    std::vector<CGooseCb*>::iterator    iter;
    for (iCbIndex = 0, iter = m_vecGOOSESendCbs.begin(); iter != m_vecGOOSESendCbs.end(); iCbIndex++, iter++)
    {
        CGooseCb* pGooseCb = (CGooseCb*)*iter;


        // 检查用户是否临时取消了发送事务
        if (m_pDataSendManager && m_pDataSendManager->m_eventCancelGoose.Lock(0))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): User cancels the current operation \n"));
            return ERROR_CODE_CANCEL;
        }

        ///< 控制块通道数不能为0
        if ((NULL == pGooseCb) || (0 == pGooseCb->GetChannelCount()))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): No Channel \n"));
            return ERROR_CODE_FAILURE;
        }

        m_pGooseMsg->ClearGoosePDU();
        pGooseCb->GetDesMAC(ucMacDes, ETHER_MAC_LEN);                   ///< 获得目标MAC地址
        pGooseCb->GetSrcMAC(ucMacSrc, ETHER_MAC_LEN);                   ///< 获得源MAC地址
        m_pGooseMsg->SetMacAddr(ucMacDes, ucMacSrc, ETHER_MAC_LEN,pGooseCb->GetVlanFlag());     ///< 设置收发MAC地址
        m_pGooseMsg->SetAppID(pGooseCb->GetAPPID());                    ///< APPID
        m_pGooseMsg->Set8021QVlanInfo(pGooseCb->GetVLanPriority(), pGooseCb->GetVlanID(),pGooseCb->GetVlanFlag());

        // 控制块引用
        GOOSE_PDU_PUBLIC stPdbPublic;
        std::wstring strValue           = pGooseCb->GetGocbRef();
        stPdbPublic.gocbRef.assign(strValue.begin(), strValue.end());

        // 数据集的引用
        strValue = pGooseCb->GetDataSet();
        stPdbPublic.datSet.assign(strValue.begin(), strValue.end());

        // 报文的唯一性标识
        strValue = pGooseCb->GetGoID();
        stPdbPublic.goID.assign(strValue.begin(), strValue.end());

        // 报文加密
        strValue = pGooseCb->GetSecurity();
        stPdbPublic.security.assign(strValue.begin(), strValue.end());

        // 其他配置信息
        stPdbPublic.timeAllowedtoLive   = 2 * pParamConfigGooseSend->GetHeartbeatInterval();
        stPdbPublic.eventTm.ts_s        = stUtcTime.uSecondSinceEpoch;  ///< 时间戳的秒数部分
        stPdbPublic.eventTm.ts_ns       = stUtcTime.uFractionOfSecond * 1000;    ///< 时间戳的纳秒数部分
        stPdbPublic.eventTm.ts_q        = 0;                            ///< 时间戳的品质
        stPdbPublic.stNum               = pGooseCb->GetStNum();         ///< 状态序号（变位次数）
        stPdbPublic.sqNum               = 0;                            ///< 顺序号
        stPdbPublic.test                = (pGooseCb->GetTestFlag()? 1: 0);
        stPdbPublic.confRev             = pGooseCb->GetConfigRevision();///< 配制版本号
        stPdbPublic.ndsCom              = (pGooseCb->GetNeedComm()? 1: 0);
        stPdbPublic.numDatSetEntries    = pGooseCb->GetChannelCount();  ///< 通道数
        m_pGooseMsg->SetPduPublicInfo(stPdbPublic);

        // 遍历所有通道
        for (CChannel* pChannel = pGooseCb->FirstChannel(); pChannel != NULL; 
            pChannel = pGooseCb->NextChannel(pChannel))
        {
            if ((CHANNEL_TYPE_GOOSE_STRUCT == pChannel->GetChannelType()) &&
                (pChannel->GetSubChannelCount() > 0))                   ///< 结构体类型通道且有子通道
            {
                // 创建结构体类型通道对象
                GOOSE_STRUCT stGooseStruct;
                stGooseStruct.id.type   = GSEDT_STRUCT;
                stGooseStruct.member.clear();

                // 遍历所有子通道
                int         iCount      = 0;
                CChannel*   pSubChannel = pChannel->FirstSubChannel();
                while(pSubChannel)
                {
                    // 向结构体对象添加子通道值对象指针
                    stGooseStruct.member.push_back(&(pSubChannel->GetGooseSendValue().m_stUnKnown.id));

                    // 下一个子通道
                    pSubChannel = pChannel->NextSubChannel(pSubChannel);
                    iCount++;
                }

                // 向帧管理器添加一个数据通道
                if (!m_pGooseMsg->AddAChannel(&(stGooseStruct.id)))
                {
                    return ERROR_GOOSE_TOO_MANY_CHANNEL;
                }
            }
            else                                                        ///< 非结构体类型通道
            {
                // 获得通道值
                GOOSE_DATA_VARIANT stChannelValue = pChannel->GetGooseSendValue();

                // 向帧管理器添加一个数据通道
                if (!m_pGooseMsg->AddAChannel(&(stChannelValue.m_stUnKnown.id)))
                {
                    return ERROR_GOOSE_TOO_MANY_CHANNEL;
                }
            }
        }

        //
        // 根据设置的参数进行编码，获得报文帧
        //
        int iByteCount = (int)(sizeof(m_pucFrameBuffer[0]) * m_pGooseMsg->Code(m_pucFrameBuffer, sizeof(m_pucFrameBuffer)));
        if (0 == iByteCount)                                            ///< 检查帧大小
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): Code frame failed \n"));
            return ERROR_CODE_FAILURE;
        }

        // 记录帧长
        if ((4 + iCbIndex) >= sizeof(stMuSendParam.nInBuf)/sizeof(stMuSendParam.nInBuf[0]))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): Record frame length failed \n"));
            return ERROR_CODE_FAILURE;
        }
        stMuSendParam.nInBuf[4 + iCbIndex] = iByteCount;

        //
        // 复制数据帧到发送缓冲区
        //
        if ((pDataBuffer + iByteCount) > pDataBufferEnd)                ///< 判断是否越界
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): Oversize frame \n"));
            return ERROR_CODE_FAILURE;
        }
        memcpy(pDataBuffer, m_pucFrameBuffer, iByteCount);

        // 递增缓冲区指针（4字节对齐）
        iByteCount      = ((iByteCount & 0x03) != 0)? ((iByteCount & 0xFFFC) + 4): iByteCount;
        pDataBuffer    += iByteCount;
    }

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///    填充报文发送缓冲区（GOOSE）
///
/// @param[in]  pDataBuffer         暂存发送帧的缓冲区
/// @param[in]  uiDataBufferLen     发送帧缓冲区的大小（字节）
/// @param[out] pNewSendBuffer      指向发送帧对象的指针（用于保存发送帧信息）
/// @param[out] uiFramesTotalLen    所有待发送帧的长度和（字节）
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendNet::__FillGooseSendBuffer( unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CStateSendBuffer* pNewSendBuffer, uint32_t& uiFramesTotalLen )
{
    unsigned char*          pDataBufferEnd          = NULL;
    uint8_t                 ucMacDes[ETHER_MAC_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t                 ucMacSrc[ETHER_MAC_LEN] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff};


    // 输入参数检查
    if (NULL == pDataBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): NULL parameter \n"));
        return ERROR_CODE_FAILURE;
    }

    // 检查缓冲区大小
    if (uiDataBufferLen <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): uiDataBufferLen <= 0 \n"));
        return ERROR_CODE_FAILURE;
    }
    pDataBufferEnd = pDataBuffer + uiDataBufferLen;                     ///< 指向缓冲区末尾

    // 获得GOOSE发送参数配置管理器
    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Get CParamConfigGOOSESend object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // 检查是否有待发送的控制块
    if (m_vecGOOSESendCbs.size() <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): No control block to be transmitted \n"));
        return ERROR_CODE_FAILURE;
    }

    //
    // 遍历每一个控制块，合成发送报文帧，添加到发送缓冲
    //
    uiFramesTotalLen = 0;
    std::vector<CGooseCb*>::iterator    iter = m_vecGOOSESendCbs.begin();
    for (int iCbIndex = 0; iter != m_vecGOOSESendCbs.end(); iCbIndex++, iter++)
    {
        CGooseCb* pGooseCb = (CGooseCb*)*iter;


        // 检查用户是否临时取消了发送事务
        if (m_pDataSendManager && m_pDataSendManager->m_eventCancelGoose.Lock(0))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): User cancels the current operation \n"));
            return ERROR_CODE_CANCEL;
        }

        ///< 控制块通道数不能为0
        if ((NULL == pGooseCb) || (0 == pGooseCb->GetChannelCount()))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): No Channel \n"));
            return ERROR_CODE_FAILURE;
        }

        // 复位变位标记
        pGooseCb->SetSendValueChanged(false);

        // 复位stNum
        pGooseCb->SetStNum(1);

        m_pGooseMsg->ClearGoosePDU();
        pGooseCb->GetDesMAC(ucMacDes, ETHER_MAC_LEN);                   ///< 获得目标MAC地址
        pGooseCb->GetSrcMAC(ucMacSrc, ETHER_MAC_LEN);                   ///< 获得源MAC地址
        m_pGooseMsg->SetMacAddr(ucMacDes, ucMacSrc, ETHER_MAC_LEN,pGooseCb->GetVlanFlag());     ///< 设置收发MAC地址
        m_pGooseMsg->SetAppID(pGooseCb->GetAPPID());                    ///< APPID
        m_pGooseMsg->Set8021QVlanInfo(pGooseCb->GetVLanPriority(), pGooseCb->GetVlanID(),pGooseCb->GetVlanFlag());

        // 控制块引用
        GOOSE_PDU_PUBLIC stPdbPublic;
        std::wstring strValue           = pGooseCb->GetGocbRef();
        stPdbPublic.gocbRef.assign(strValue.begin(), strValue.end());

        // 数据集的引用
        strValue = pGooseCb->GetDataSet();
        stPdbPublic.datSet.assign(strValue.begin(), strValue.end());

        // 报文的唯一性标识
        strValue = pGooseCb->GetGoID();
        stPdbPublic.goID.assign(strValue.begin(), strValue.end());

        // 报文加密
        strValue = pGooseCb->GetSecurity();
        stPdbPublic.security.assign(strValue.begin(), strValue.end());

        // 其他配置信息
        stPdbPublic.timeAllowedtoLive   = 2 * pParamConfigGooseSend->GetHeartbeatInterval();
        stPdbPublic.eventTm.ts_s        = 0;                            ///< 时间戳的秒数部分
        stPdbPublic.eventTm.ts_ns       = 0;                            ///< 时间戳的纳秒数部分
        stPdbPublic.eventTm.ts_q        = 0;                            ///< 时间戳的品质
        stPdbPublic.stNum               = pGooseCb->GetStNum();         ///< 状态序号（变位次数）
        stPdbPublic.sqNum               = 0;                            ///< 顺序号
        stPdbPublic.test                = (pGooseCb->GetTestFlag()? 1: 0);
        stPdbPublic.confRev             = pGooseCb->GetConfigRevision();///< 配制版本号
        stPdbPublic.ndsCom              = (pGooseCb->GetNeedComm()? 1: 0);
        stPdbPublic.numDatSetEntries    = pGooseCb->GetChannelCount();  ///< 通道数
        m_pGooseMsg->SetPduPublicInfo(stPdbPublic);

        // 遍历所有通道
        for (CChannel* pChannel = pGooseCb->FirstChannel(); pChannel != NULL; 
            pChannel = pGooseCb->NextChannel(pChannel))
        {
            if ((CHANNEL_TYPE_GOOSE_STRUCT == pChannel->GetChannelType()) &&
                (pChannel->GetSubChannelCount() > 0))                   ///< 结构体类型通道且有子通道
            {
                // 创建结构体类型通道对象
                GOOSE_STRUCT stGooseStruct;
                stGooseStruct.id.type   = GSEDT_STRUCT;
                stGooseStruct.member.clear();

                // 遍历所有子通道
                int         iCount      = 0;
                CChannel*   pSubChannel = pChannel->FirstSubChannel();
                while(pSubChannel)
                {
                    // 向结构体对象添加子通道值对象指针
                    stGooseStruct.member.push_back(&(pSubChannel->GetGooseSendValue().m_stUnKnown.id));

                    // 复位变位标记
                    pSubChannel->SetSendValueChanged(false);

                    // 下一个子通道
                    pSubChannel = pChannel->NextSubChannel(pSubChannel);
                    iCount++;
                }

                // 向帧管理器添加一个数据通道
                if (!m_pGooseMsg->AddAChannel(&(stGooseStruct.id)))
                {
                    return ERROR_GOOSE_TOO_MANY_CHANNEL;
                }
            }
            else                                                        ///< 非结构体类型通道
            {
                // 获得通道值
                GOOSE_DATA_VARIANT stChannelValue = pChannel->GetGooseSendValue();

                // 向帧管理器添加一个数据通道
                if (!m_pGooseMsg->AddAChannel(&(stChannelValue.m_stUnKnown.id)))
                {
                    return ERROR_GOOSE_TOO_MANY_CHANNEL;
                }
            }

            // 复位变位标记
            pChannel->SetSendValueChanged(false);
        }

        //
        // 根据设置的参数进行编码，获得报文帧
        //
        int iByteCount = (int)(sizeof(m_pucFrameBuffer[0]) * m_pGooseMsg->Code(m_pucFrameBuffer, sizeof(m_pucFrameBuffer)));
        if (0 == iByteCount)                                            ///< 检查帧大小
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): Code frame failed \n"));
            return ERROR_CODE_FAILURE;
        }

        // 记录帧长
        if ((4 + iCbIndex) >= sizeof(pNewSendBuffer->m_stIOCtrlParam.nInBuf)/sizeof(pNewSendBuffer->m_stIOCtrlParam.nInBuf[0]))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): Record frame length failed \n"));
            return ERROR_CODE_FAILURE;
        }
        pNewSendBuffer->m_stIOCtrlParam.nInBuf[4 + iCbIndex] = iByteCount;

        //
        // 复制数据帧到发送缓冲区
        //
        if ((pDataBuffer + iByteCount) > pDataBufferEnd)                ///< 判断是否越界
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillGooseSendBuffer(): Oversize frame \n"));
            return ERROR_CODE_FAILURE;
        }
        memcpy(pDataBuffer, m_pucFrameBuffer, iByteCount);

        // 记录数据帧的信息（对应的GOOSE控制块、帧偏移、帧长度）
        CFrameInfo stFrameInfo;
        stFrameInfo.m_pGooseCb      = pGooseCb;                         ///< 数据帧对应的GOOSE控制块
        stFrameInfo.m_uiFrameOffset = uiFramesTotalLen;                 ///< 帧偏移
        stFrameInfo.m_uiFrameLen    = iByteCount;                       ///< 帧长度
        pNewSendBuffer->m_listFrameInfo.push_back(stFrameInfo);

        // 递增缓冲区指针（4字节对齐）
        iByteCount      = ((iByteCount & 0x03) != 0)? ((iByteCount & 0xFFFC) + 4): iByteCount;
        pDataBuffer    += iByteCount;
        uiFramesTotalLen += iByteCount;
    }

    return ERROR_CODE_SUCCESS;
}

///
/// @brief
///     更新SMV发送帧
///
/// @param[in]  stDspRecvData   虚拟通道一个完整波形的采样值
///
/// @return
///     bool    发送成功返回true
///
bool CFrameSendNet::UpdateSMVFrames( CDspRecvData& stDspRecvData )
{
    uint32_t                            uiFramesTotalLen;
    IOCTRL_IN_PARAM_NET_BIG             stMuSendParam;


    // 如果没有待发送的控制块，那么直接返回
    if (m_vecSMVSendCbs.size() <= 0)
    {
        return true;
    }

    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): uninit \n"));
        return false;
    }

    // 获得SMV发送参数配置管理器
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): Get CParamConfigSMVSend object failed \n"));
        return false;
    }

    // 计算缓冲区偏移
    unsigned int uiBufferOffset = 0;
    if (0 == m_pMapHeader->NetHeader.nCurSmvTxBufItem)                  ///< 当前Smv发送缓冲区的序号
    {
        uiBufferOffset  = NET_SMV_SEND_LOOP_BUF1_POS;
    }
    else
    {
        uiBufferOffset  = NET_SMV_SEND_LOOP_BUF0_POS;
    }

    // 填充待发送缓冲区
    if (FRAME_TYPE_NET_9_1 == pParamConfigSMVSend->GetSMVType())
    {
        // 将IEC61850-9-1控制块内容组帧，填充到内存映射缓冲区
        if (ERROR_CODE_SUCCESS != __FillIec91SendBuffer(m_pMapDataBuffer + uiBufferOffset, NET_SMV_SEND_BUF_LEN, stDspRecvData, stMuSendParam, uiFramesTotalLen))
        {
            return false;
        }
    }
    else if (FRAME_TYPE_NET_9_2 == pParamConfigSMVSend->GetSMVType())
    {
        // 将IEC61850-9-2控制块内容组帧，填充到内存映射缓冲区
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

    // 检查用户是否临时取消了发送事务
    if (m_pDataSendManager->m_eventCancelSMV.Lock(0))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): User cancels the current operation \n"));
        return false;
    }

    // 开始发送数据
    IOCTRL_IN_PARAM_NET stIOCtrlInParam;
    stIOCtrlInParam.nDeviceItem   = m_enPortName;                       ///< 设备号
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
///    更新SMV缓冲区中的数据帧
///
/// @param[in]  pStateSendBuffer    指向缓冲区的指针
///
/// @return
///     bool    成功返回true
///
bool CFrameSendNet::UpdateSMVFrames( CStateSendBuffer* pStateSendBuffer )
{
    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): uninit \n"));
        return false;
    }

    // 检查输入参数
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): Parameter error \n"));
        return false;
    }

    // 计算缓冲区偏移
    unsigned int uiBufferOffset = 0;
    if (0 == m_pMapHeader->NetHeader.nCurSmvTxBufItem)                  ///< 当前Smv发送缓冲区的序号
    {
        uiBufferOffset  = NET_SMV_SEND_LOOP_BUF1_POS;
    }
    else
    {
        uiBufferOffset  = NET_SMV_SEND_LOOP_BUF0_POS;
    }

    // 填充待发送缓冲区
    if (0 != memcpy_s(m_pMapDataBuffer + uiBufferOffset, NET_SMV_SEND_BUF_LEN, pStateSendBuffer->m_pucDataBuffer, pStateSendBuffer->m_uiDataBufferLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): Fill send buffer failed"));
        return false;
    }

    // 开始发送数据
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
///     更新GOOSE发送帧
///
/// @param[in]  stDspRecvData   虚拟通道一个完整波形的采样值
///
/// @return
///     bool    发送成功返回true
///
bool CFrameSendNet::UpdateGooseFrames()
{
    IOCTRL_IN_PARAM_NET_BIG             stMuSendParam;


    // 如果没有待发送的控制块，那么直接返回
    if (m_vecGOOSESendCbs.size() <= 0)
    {
        return true;
    }

    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateGooseFrames(): uninit \n"));
        return false;
    }

    // 获得UTC时间
    UTC_TIME_t stUtcTime;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_GET_UTC_TIME, NULL, NULL, &stUtcTime, sizeof(stUtcTime), NULL ,NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::SendGooseCbs(): Port%d IOCTRL_GET_UTC_TIME failed \n", m_enPortName));
        return false;
    }

    //
    // 设置变位标记
    //
    int                                 iCbIndex;                       ///< 控制块计数变量
    std::vector<CGooseCb*>::iterator    iter;
    IOCTRL_IN_PARAM_NET                 stIOCtrlInParam;
    stIOCtrlInParam.nDeviceItem         = m_enPortName;                 ///< 设备号
    stIOCtrlInParam.nInBuf[0]           = 1;                            ///< GOOSE
    stIOCtrlInParam.nInBuf[1]           = 0;                            ///< GOOSE变位标记
    for (iCbIndex = 0, iter = m_vecGOOSESendCbs.begin(); iter != m_vecGOOSESendCbs.end(); iCbIndex++, iter++)
    {
        CGooseCb* pGooseCb = (CGooseCb*)*iter;
        if (pGooseCb && pGooseCb->IsSendValueChanged())                ///< GOOSE通道值发生了改变
        {
            // 哪一个GOOSE控制块发生变位了，需要给对应的那位置1
            stIOCtrlInParam.nInBuf[1] |= (1 << iCbIndex);
        }
    }

    // 计算缓冲区偏移
    unsigned int uiBufferOffset = 0;
    if (0 == m_pMapHeader->NetHeader.nCurGooseTxBufItem)                ///< 当前GOOSE发送缓冲区的序号
    {
        uiBufferOffset = GOOSE_SEND_LOOP_BUF1_POS;
    }
    else
    {
        uiBufferOffset = GOOSE_SEND_LOOP_BUF0_POS;
    }

    // 填充待发送缓冲区
    if (ERROR_CODE_SUCCESS != __FillGooseSendBuffer(m_pMapDataBuffer + uiBufferOffset, GOOSE_SEND_BUF_LEN, stUtcTime, stMuSendParam))
    {
        return false;
    }

    // 检查用户是否临时取消了发送事务
    if (m_pDataSendManager->m_eventCancelGoose.Lock(0))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateGooseFrames(): User cancels the current operation \n"));
        return false;
    }

    // 开始发送数据
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
///     更新GOOSE发送帧
///
/// @param[in]  pStateSendBuffer    指向缓冲区的指针
///
/// @return
///     bool    发送成功返回true
///
bool CFrameSendNet::UpdateGooseFrames( CStateSendBuffer* pStateSendBuffer )
{
    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateGooseFrames(): uninit \n"));
        return false;
    }

    // 检查输入参数
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateGooseFrames(): parameter error \n"));
        return false;
    }

    // 计算缓冲区偏移
    unsigned int uiBufferOffset = 0;
    if (0 == m_pMapHeader->NetHeader.nCurGooseTxBufItem)                ///< 当前GOOSE发送缓冲区的序号
    {
        uiBufferOffset = GOOSE_SEND_LOOP_BUF1_POS;
    }
    else
    {
        uiBufferOffset = GOOSE_SEND_LOOP_BUF0_POS;
    }

    // 填充待发送缓冲区
    if (0 != memcpy_s(m_pMapDataBuffer + uiBufferOffset, GOOSE_SEND_BUF_LEN, pStateSendBuffer->m_pucDataBuffer, pStateSendBuffer->m_uiDataBufferLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::UpdateSMVFrames(): Fill send buffer failed"));
        return false;
    }

    // 开始发送数据
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
///    计算控制块中所有通道的偏移量
///
/// @param[in]  pStateSendBuffer    指向发送帧缓冲对象的指针
///
/// @return
///     bool    成功返回true
///
bool CFrameSendNet::CalcChannelOffset( CStateSendBuffer* pStateSendBuffer )
{
    unsigned char* pucFrame = NULL;


    // 检查输入参数
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__CalcChannelOffset(): Parameter error \n"));
        return false;
    }

    // 遍历所有控制块
    std::list<CFrameInfo>::iterator iter = pStateSendBuffer->m_listFrameInfo.begin();
    for (; iter != pStateSendBuffer->m_listFrameInfo.end(); iter++)
    {
        CFrameInfo& stFrameInfo = *iter;
        if (stFrameInfo.m_pGooseCb)
        {
            // 解码GOOSE控制块对应的发送帧
            pucFrame = pStateSendBuffer->m_pucDataBuffer + stFrameInfo.m_uiFrameOffset;
            if (0 == m_pGooseMsg->Decode(pucFrame, stFrameInfo.m_uiFrameLen))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__CalcChannelOffset(): decode frame failed \n"));
                return false;
            }

            // 计算控制块中所有通道的偏移量
            if (!stFrameInfo.m_pGooseCb->CalcChannelOffset(m_pGooseMsg))
            {
                return false;
            }
        }
    }

    return true;
}
