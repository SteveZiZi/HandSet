/// @file
///
/// @brief
///     Ft3数据发送类
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
#include "FrameSendFt3.h"
#include "FrameSend.h"
#include "src\utils\SmvScaler\SmvScaler.h"
#include "src\service\datalayer\config\ParamConfigBasic.h"
#include "src\service\datalayer\config\ParamConfigSMVSend.h"
#include "src\service\datalayer\config\ParamconfigManualTest.h"

///
/// @brief
///    构造函数
///
/// @param[in]  pFrameSend    数据发送管理器
/// @param[in]  hGPMDevice    GPMC总线句柄
/// @param[in]  enPortName    Ft3实例对应的实际端口
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
///    析构函数
///
CFrameSendFt3::~CFrameSendFt3(void)
{
    UnInitFt3();
}

///
/// @brief
///    初始化Ft3发送,打开共享内存
///
/// @return
///     true代表初始化成功
///
bool CFrameSendFt3::InitFt3( const wchar_t *pNotifyEvtName, const wchar_t *pMapFileName, uint32_t nMapFileLen )
{
    // 判断是否已经初始化
    if (m_bIsInited)
    {
        return true;
    }

    // 检查GPM驱动句柄
    if (INVALID_HANDLE_VALUE == m_hGPMDevice)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecvFt3::InitFt3(): m_hGPMDevice INVALID_HANDLE_VALUE \n")));
        return false;
    }

    // 创建通道到的内存映射文件
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

    // 获得映射文件的内存指针
    m_pMapHeader = (Map_File_Header*)MapViewOfFile(m_hFileMaping, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
    if (NULL == m_pMapHeader)
    {
        RETAILMSG(OUTPUT_LOG_MSG,(_T("CFrameRecvFt3::InitFt3(): FileMapping for Port%d map failed"), m_enPortName));
        goto exitCode;
    }
    m_pMapDataBuffer    = (unsigned char*)m_pMapHeader;
    m_pMapDataBuffer   += sizeof(Map_File_Header);

    // 创建南瑞FT3帧管理器
    m_pNanRuiFT3Msg = CreateNanRuiFT3MsgImpl();
    if (NULL == m_pNanRuiFT3Msg)
    {
        RETAILMSG(OUTPUT_LOG_MSG,(_T("CFrameRecvFt3::InitFt3(): Create NanRui FT3 frame manager falied \n")));
        goto exitCode;
    }

    // 创建国网FT3帧管理器
    m_pGuoWangFT3Msg = CreateGuoWangFT3MsgImpl();
    if (NULL == m_pGuoWangFT3Msg)
    {
        RETAILMSG(OUTPUT_LOG_MSG,(_T("CFrameRecvFt3::InitFt3(): Create GuoWang FT3 frame manager falied \n")));
        goto exitCode;
    }

    // 标记为已成功初始化
    m_bIsInited = true;
    return true;

exitCode:
    UnInitFt3();
    return false;
}

///
/// @brief
///    反初始化FT3发送,关闭内存映射
///
void CFrameSendFt3::UnInitFt3()
{
    // 停止底层数据发送
    StopFt3Send();

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

    // 释放南瑞FT3帧管理器
    if (NULL != m_pNanRuiFT3Msg)
    {
        ReleaseNanRuiFT3MsgImpl(m_pNanRuiFT3Msg);
        m_pNanRuiFT3Msg = NULL;
    }

    // 释放国网FT3帧管理器
    if (NULL != m_pGuoWangFT3Msg)
    {
        ReleaseGuoWangFT3MsgImpl(m_pGuoWangFT3Msg);
        m_pGuoWangFT3Msg = NULL;
    }
}

///
/// @brief
///    预先计算端口的SMV发送帧
///
/// @param[in]  stDspRecvData       虚拟通道一个完整波形的采样值
/// @param[out] pStateTestResult    保存待发送数据帧的结构体
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendFt3::PrepareSmvSendBuffer( CDspRecvData& stDspRecvData, CStateTestResult* pStateTestResult )
{
    FT3_TX_CFG                          stTxCfg;
    uint32_t                            uiFramesTotalLen;


    // 检查输入参数
    if (NULL == pStateTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__PrepareSmvSendBuffer(): Parameter error \n"));
        return ERROR_CODE_FAILURE;
    }

    // 如果没有待发送的控制块，那么直接返回
    if (NULL == m_pSMVSendCb)
    {
        return ERROR_CODE_SUCCESS;
    }

    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__PrepareSmvSendBuffer(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得Ft3发送配置
    memset(&stTxCfg, 0, sizeof(FT3_TX_CFG));
    if (!__GetFt3SendCfg(stTxCfg))
    {
        return ERROR_CODE_FAILURE;
    }

    // 添加待发送缓冲
    CStateSendBuffer* pNewSendBuffer = new CStateSendBuffer(m_enPortName);
    if (NULL == pNewSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__PrepareSmvSendBuffer(): new CStateSendData failed \n"));
        return ERROR_CODE_FAILURE;
    }
    pNewSendBuffer->m_stIOCtrlParam.nDeviceItem  = m_enPortName;        ///< Ft3实例对应的实际端口
    memcpy_s(pNewSendBuffer->m_stIOCtrlParam.nInBuf, sizeof(pNewSendBuffer->m_stIOCtrlParam.nInBuf), &stTxCfg, sizeof(stTxCfg));
    pStateTestResult->m_listSmvSendBuffers.push_back(pNewSendBuffer);

    // 填充待发送缓冲区
    enumErrorCode eRet = __FillFt3SendBuffer(m_pMapDataBuffer + FT3_SEND_LOOP_BUF0_POS, FT3_SEND_LOOP_BUF0_LEN, stDspRecvData, uiFramesTotalLen);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        return eRet;
    }

    // 申请缓冲
    if (NULL == pNewSendBuffer->AllocDataBuffer(uiFramesTotalLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__PrepareSmvSendBuffer(): Alloc Data Buffer failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // 备份所有发送帧
    if (0 != memcpy_s(pNewSendBuffer->m_pucDataBuffer, uiFramesTotalLen, m_pMapDataBuffer + FT3_SEND_LOOP_BUF0_POS, uiFramesTotalLen))
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
///     enumErrorCode   发送成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendFt3::SendSMVCbs(CDspRecvData& stDspRecvData)
{
    FT3_TX_CFG                          stTxCfg;
    IOCTRL_IN_PARAM_NET_BIG             stFt3InParam;
    uint32_t                            uiFramesTotalLen;
    enumErrorCode                       eRet;


    // 如果没有待发送的控制块，那么直接返回
    if (NULL == m_pSMVSendCb)
    {
        return ERROR_CODE_SUCCESS;
    }

    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): uninit \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得Ft3发送配置
    memset(&stTxCfg, 0, sizeof(FT3_TX_CFG));
    if (!__GetFt3SendCfg(stTxCfg))
    {
        return ERROR_CODE_FAILURE;
    }

    // 填充IOCTRL SEND HEADER
    memset(&stFt3InParam, 0, sizeof(stFt3InParam));
    stFt3InParam.nDeviceItem = m_enPortName;                            ///< Ft3实例对应的实际端口
    memcpy_s(stFt3InParam.nInBuf, sizeof(stFt3InParam.nInBuf), &stTxCfg, sizeof(FT3_TX_CFG));

    // 设置IOCTRL SEND HEADER（设置为手动试验模式）
    stFt3InParam.nInBuf[TX_TEST_STYLE_INDEX] = TX_TEST_STYLE_MANUAL_TEST;
    if (FALSE == DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_HEADER, &stFt3InParam, sizeof(stFt3InParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): Port%d IOCTRL_SEND_HEADER failed \n", m_enPortName));
        return ERROR_CODE_FAILURE;
    }

    // 填充待发送缓冲区
    m_pMapHeader->Ft3Header.nCurTxBufItem = 0;
    eRet = __FillFt3SendBuffer(m_pMapDataBuffer + FT3_SEND_LOOP_BUF0_POS, FT3_SEND_LOOP_BUF0_LEN, stDspRecvData, uiFramesTotalLen);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        return eRet;
    }

    // 检查用户是否临时取消了发送事务
    if (m_pDataSendManager->m_eventCancelSMV.Lock(0))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): User cancels the current operation \n"));
        return ERROR_CODE_CANCEL;
    }

    // 开始发送数据
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
///    发送缓冲区中的数据帧
///
/// @param[in]  pStateSendBuffer    指向缓冲区的指针
///
/// @return
///     bool    成功返回true
///
bool CFrameSendFt3::SendSMVCbs( CStateSendBuffer* pStateSendBuffer )
{
    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): uninit \n"));
        return false;
    }

    // 检查输入参数
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): Parameter error \n"));
        return false;
    }

    // 填充待发送缓冲区
    m_pMapHeader->Ft3Header.nCurTxBufItem = 0;
    if (0 != memcpy_s(m_pMapDataBuffer + FT3_SEND_LOOP_BUF0_POS, FT3_SEND_LOOP_BUF0_LEN, pStateSendBuffer->m_pucDataBuffer, pStateSendBuffer->m_uiDataBufferLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): fill send buffer failed \n"));
        return false;
    }

    // 设置IOCTRL SEND HEADER（设置为状态序列模式）
    pStateSendBuffer->m_stIOCtrlParam.nInBuf[TX_TEST_STYLE_INDEX] = TX_TEST_STYLE_STATUS_SEQUENCE;
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_SEND_HEADER, &pStateSendBuffer->m_stIOCtrlParam, sizeof(pStateSendBuffer->m_stIOCtrlParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::SendSMVCbs(): Port%d IOCTRL_SEND_HEADER failed \n", m_enPortName));
        return false;
    }

    // 开始发送数据
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
///    停止FT3数据发送
///
bool CFrameSendFt3::StopFt3Send()
{
    IOCTRL_IN_PARAM_FT3 stIoCtrolFt3TxCfg;


    // 停止FT3数据发送
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
///    设置待发送的SMV控制块
///
/// @param[in]  pNewCb    SMV控制块
///
void CFrameSendFt3::SetSMVSendCb( CFT3Cb* pNewCb )
{
    m_pSMVSendCb = pNewCb;
}

///
/// @brief
///    获得Ft3发送配置
///
/// @param[out]  stTxCfg    保存配置信息的结构体
///
/// @return
///     bool    成功返回true
///
bool CFrameSendFt3::__GetFt3SendCfg( FT3_TX_CFG& stTxCfg )
{
    Ft3CtlWordSend                      stCtrlWordSend;


    // 获得SMV发送参数配置管理器
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__GetFt3TxCfg(): Get CParamConfigSMVSend object failed \n"));
        return false;
    }

    // 波特率
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

    // 采样频率
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

    // 报文格式
    switch(pParamConfigSMVSend->GetSMVType())
    {
    case FRAME_TYPE_FT3_NANRUI:                                         ///< 标准FT3
        {
            stCtrlWordSend.CtrlBit.bFrameType   = Ft3CtlWordSend::FT3_FRAME_TYPE_NANRUI;
        }
        break;

    case FRAME_TYPE_FT3_GUOWANG:                                        ///< 扩展FT3
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
    // 设置FT3_TX_CFG结构体
    //
    stTxCfg.uFramesStyle  = (uint16_t)stCtrlWordSend.nData;             ///< 帧格式
    stTxCfg.nFrameCount   = 20;                                         ///< 一次向FPGA中传送20帧报文
    if (Ft3CtlWordSend::FT3_FRAME_TYPE_NANRUI == stCtrlWordSend.CtrlBit.bFrameType)
    {
        stTxCfg.nFrameLength    = stTxCfg.nFrameCount * (16 * 3);       ///< 数据总长度
    }
    else if (Ft3CtlWordSend::FT3_FRAME_TYPE_GUOWANG == stCtrlWordSend.CtrlBit.bFrameType)
    {
        stTxCfg.nFrameLength    = stTxCfg.nFrameCount * (16 * 4);       ///< 数据总长度
    }

    return true;
}

///
/// @brief
///    填充FT3报文发送缓冲区
///
/// @param[out]  pDataBuffer        待发送缓冲区
/// @param[in]   uiDataBufferLen    发送缓冲区大小（字节）
/// @param[in]   stDspRecvData      虚拟通道一个完整波形的采样值
/// @param[out]  uiFramesTotalLen   所有待发送帧的长度和
///
/// @return
///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
///
enumErrorCode CFrameSendFt3::__FillFt3SendBuffer( unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CDspRecvData& stDspRecvData, uint32_t& uiFramesTotalLen )
{
    unsigned char*          pDataBufferEnd  = NULL;


    // 输入参数检查
    if (NULL == pDataBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): NULL parameter \n"));
        return ERROR_CODE_FAILURE;
    }

    // 检查缓冲区大小
    if (uiDataBufferLen <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): uiDataBufferLen <= 0 \n"));
        return ERROR_CODE_FAILURE;
    }
    pDataBufferEnd = pDataBuffer + uiDataBufferLen;                     ///< 指向缓冲区末尾

    // 检查是否有待发送的控制块
    if (NULL == m_pSMVSendCb)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): SMV send control block is NULL \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得基本参数配置管理器
    CParamConfigBasic* pParamConfigBasic = CParamConfigBasic::getInstance();
    if (NULL == pParamConfigBasic)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendNet::__FillFt3SendBuffer(): Get CParamConfigBasic object failed \n"));
        return ERROR_CODE_FAILURE;
    }

    // 获得SMV发送参数配置管理器
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): Get CParamConfigSMVSend object failed \n"));
        return ERROR_CODE_FAILURE;
    }
    enumFrameType eFrameType = pParamConfigSMVSend->GetSMVType();

    // 检查控制块通道数
    if (m_pSMVSendCb->GetChannelCount() <= 0)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): Control block has no Channel \n"));
        return ERROR_CODE_FAILURE;
    }

    // 检查通道数是否超限
    if (m_pSMVSendCb->GetChannelCount() > 22)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): Excessive number of channels \n"));
        return ERROR_CODE_FAILURE;
    }

    CSmvScaler  SmvValueCaler;
    // 设置报文值类型
    SmvValueCaler.SetChannelMsgValType(pParamConfigBasic->GetFrameValueType());
    // 设置通道的自然值类型
    SmvValueCaler.SetChannelNtrValType(pParamConfigSMVSend->GetSVDisplay());

    //
    // 遍历每一组采样值，合成发送报文帧，添加到发送缓冲
    //
    uiFramesTotalLen = 0;
    for (int iFrameIndex = 0; iFrameIndex < stDspRecvData.m_iSmvCount; iFrameIndex++)
    {
        // 检查用户是否临时取消了发送事务
        if (m_pDataSendManager && m_pDataSendManager->m_eventCancelSMV.Lock(0))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): User cancels the current operation \n"));
            return ERROR_CODE_CANCEL;
        }

        // 设置发送报文帧中每个通道的值
        int     iChannelCount       = 0;                        ///< 通道索引
        double  dSmvValue           = 0.0f;                     ///< 采样值
        double  dOnceRatings        = 0.0f;                     ///< 一次额定值
        double  dSecondaryRatings   = 0.0f;                     ///< 二次额定值
        for (CChannel* pChannel = m_pSMVSendCb->FirstChannel(); pChannel != NULL; 
            iChannelCount++, pChannel = m_pSMVSendCb->NextChannel(pChannel))
        {
            dSmvValue           = 0.0f;                             ///< 采样值
            dOnceRatings        = pChannel->GetOnceRatings();       ///< 一次额定值
            dSecondaryRatings   = pChannel->GetSecondaryRatings();  ///< 二次额定值

            //
            // 如果此通道关联了虚拟发送通道，那么设置瞬时采样值
            //
            std::wstring strChannelName = pChannel->GetVirtualChannel();
            if (strChannelName.size())
            {
                std::map<std::wstring, int>::iterator iter = stDspRecvData.m_mapChannelName2Index.find(strChannelName);
                if (stDspRecvData.m_mapChannelName2Index.end() != iter)
                {
                    // 设置瞬时采样值
                    dSmvValue = stDspRecvData.m_stRecvData.fSmvGenData[iter->second][iFrameIndex];
                }
            }

            // 根据通道类型，设置发送帧中的通道值
            switch (pChannel->GetChannelType())
            {
            case CHANNEL_TYPE_UNKOWN:                                   ///< 未知类型
                {
                    m_usChannelValArr[iChannelCount] = 0;
                }
                break;

            case CHANNEL_TYPE_SMV_CURRENT:                              ///< 电流类型
                {
                    // 根据瞬时采样值，设置发送帧中通道值
                    m_usChannelValArr[iChannelCount] = SmvValueCaler.ChannelNtrVal2MsgVal_FT3(
                        (float)dSmvValue,
                        (float)dOnceRatings,
                        (float)dSecondaryRatings,
                        CHANNEL_TYPE_SMV_CURRENT,
                        (pChannel->IsMeasure() ? CHANNEL_VALUE_TYPE_MEASURE : CHANNEL_VALUE_TYPE_PROTECT)
                        );
                }
                break;

            case CHANNEL_TYPE_SMV_VOLTAGE:                              ///< 电压类型
                {
                    // 根据瞬时采样值，设置发送帧中通道值
                    m_usChannelValArr[iChannelCount] = SmvValueCaler.ChannelNtrVal2MsgVal_FT3(
                        (float)dSmvValue,
                        (float)dOnceRatings,
                        (float)dSecondaryRatings,
                        CHANNEL_TYPE_SMV_VOLTAGE,
                        CHANNEL_VALUE_TYPE_UNKNOWN
                        );
                }
                break;

            case CHANNEL_TYPE_SMV_TIME:                                 ///< 时间类型
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
        // 根据协议类型合成报文帧
        //
        uint16_t*   pusFrameBuffer  = NULL;
        int         iFrameBufferSize;
        int         iByteCount      = 0;
        if (FRAME_TYPE_FT3_NANRUI   == eFrameType)                        ///< 南瑞
        {
            pusFrameBuffer          = m_usNanRuiBuffer;                 ///< 缓冲数组
            iFrameBufferSize        = NANRUI_FT3_MSG_WORD;              ///< 缓冲数组大小

            // 设置帧头各字段的值
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

            // 设置帧头、通道值数组
            m_pNanRuiFT3Msg->SetASDU(stAsduPublic, m_usChannelValArr, iChannelCount);

            // 合成发送帧
            iByteCount = (int)(sizeof(pusFrameBuffer[0]) * m_pNanRuiFT3Msg->Code(pusFrameBuffer, iFrameBufferSize));
        }
        else if(FRAME_TYPE_FT3_GUOWANG == eFrameType)                   ///< 国网
        {
            pusFrameBuffer          = m_usGuoWangBuffer;                ///< 缓冲数组
            iFrameBufferSize        = GUOWANG_FT3_MSG_WORD;             ///< 缓冲数组大小

            // 设置帧头各字段的值
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

            // 设置帧头、通道值数组
            m_pGuoWangFT3Msg->SetASDU(stAsduPublic, m_usChannelValArr, iChannelCount);

            // 合成发送帧
            iByteCount = (int)(sizeof(pusFrameBuffer[0]) * m_pGuoWangFT3Msg->Code(pusFrameBuffer, iFrameBufferSize));
        }

        // 检查帧大小
        if (0 == iByteCount)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): Code frame failed \n"));
            return ERROR_CODE_FAILURE;
        }

        // 复制数据帧到发送缓冲区
        if ((pDataBuffer + iByteCount) > pDataBufferEnd)                ///< 判断是否越界
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::__FillFt3SendBuffer(): Oversize frame \n"));
            return ERROR_CODE_FAILURE;
        }
        memcpy(pDataBuffer, pusFrameBuffer, iByteCount);

        // 递增缓冲区指针
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
bool CFrameSendFt3::UpdateSMVFrames( CDspRecvData& stDspRecvData )
{
    uint32_t                            uiFramesTotalLen;


    // 如果没有待发送的控制块，那么直接返回
    if (NULL == m_pSMVSendCb)
    {
        return true;
    }

    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): uninit \n"));
        return false;
    }

    // 计算缓冲区偏移
    unsigned int uiBufferOffset = 0;
    if (0 == m_pMapHeader->Ft3Header.nCurTxBufItem)
    {
        uiBufferOffset  = FT3_SEND_LOOP_BUF1_POS;
    }
    else
    {
        uiBufferOffset  = FT3_SEND_LOOP_BUF0_POS;
    }

    // 填充待发送缓冲区
    if (ERROR_CODE_SUCCESS != __FillFt3SendBuffer(m_pMapDataBuffer + uiBufferOffset, FT3_SEND_LOOP_BUF0_LEN, stDspRecvData, uiFramesTotalLen))
    {
        return false;
    }

    // 检查用户是否临时取消了发送事务
    if (m_pDataSendManager->m_eventCancelSMV.Lock(0))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): User cancels the current operation \n"));
        return false;
    }

    // 开始发送数据
    IOCTRL_IN_PARAM_FT3 stFt3InParam;
    memset(&stFt3InParam, 0, sizeof(stFt3InParam));
    stFt3InParam.nDeviceItem = m_enPortName;                            ///< Ft3实例对应的实际端口
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
///    更新SMV缓冲区中的数据帧
///
/// @param[in]  pStateSendBuffer    指向缓冲区的指针
///
/// @return
///     bool    成功返回true
///
bool CFrameSendFt3::UpdateSMVFrames( CStateSendBuffer* pStateSendBuffer )
{
    // 判断是否已经初始化
    if (!m_bIsInited)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): uninit \n"));
        return false;
    }

    // 检查输入参数
    if (NULL == pStateSendBuffer)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): Parameter error \n"));
        return false;
    }

    // 计算缓冲区偏移
    unsigned int uiBufferOffset = 0;
    if (0 == m_pMapHeader->Ft3Header.nCurTxBufItem)
    {
        uiBufferOffset  = FT3_SEND_LOOP_BUF1_POS;
    }
    else
    {
        uiBufferOffset  = FT3_SEND_LOOP_BUF0_POS;
    }

    // 填充待发送缓冲区
    if (0 != memcpy_s(m_pMapDataBuffer + uiBufferOffset, FT3_SEND_LOOP_BUF0_LEN, pStateSendBuffer->m_pucDataBuffer, pStateSendBuffer->m_uiDataBufferLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): Fill send buffer failed"));
        return false;
    }

    // 开始发送数据
    IOCTRL_IN_PARAM_FT3 stFt3InParam;
    memset(&stFt3InParam, 0, sizeof(stFt3InParam));
    stFt3InParam.nDeviceItem = m_enPortName;                            ///< Ft3实例对应的实际端口
    if (!DeviceIoControl(m_hGPMDevice, IOCTRL_CHANGE_MU_FRAME, &stFt3InParam, sizeof(stFt3InParam), NULL, NULL, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): Port%d IOCTRL_CHANGE_MU_FRAME failed \n", m_enPortName));
        return false;
    }

    RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameSendFt3::UpdateSMVFrames(): Port%d IOCTRL_CHANGE_MU_FRAME successfully \n", m_enPortName));
    return true;
}
