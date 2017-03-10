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

#ifndef __FRAME_SEND_FT3_H__
#define __FRAME_SEND_FT3_H__
#include "FrameSendStruct.h"
#include "StateTestResult.h"
#include "src\service\datalayer\config\FT3Cb.h"

class CFrameSend;
class CFrameSendFt3
{
public:
    CFrameSendFt3(CFrameSend* pFrameSend, HANDLE hGPMDevice, DsmPortName enPortName);
    ~CFrameSendFt3(void);

public:
    ///
    /// @brief
    ///    初始化Ft3发送
    ///
    bool InitFt3(const wchar_t *pNotifyEvtName, const wchar_t *pMapFileName, uint32_t nMapFileLen );

    ///
    /// @brief
    ///    反初始化FT3发送
    ///
    void UnInitFt3();

    ///
    /// @brief
    ///    设置待发送的SMV控制块
    ///
    /// @param[in]  pNewCb    SMV控制块
    ///
    void SetSMVSendCb(CFT3Cb* pNewCb);

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
    enumErrorCode PrepareSmvSendBuffer(CDspRecvData& stDspRecvData, CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    发送SMV控制块
    ///
    /// @param[in]  stDspRecvData    虚拟通道一个完整波形的采样值
    ///
    /// @return
    ///     enumErrorCode   发送成功返回ERROR_CODE_SUCCESS
    ///
    enumErrorCode SendSMVCbs(CDspRecvData& stDspRecvData);

    ///
    /// @brief
    ///    发送缓冲区中的数据帧
    ///
    /// @param[in]  pStateSendBuffer    指向缓冲区的指针
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool SendSMVCbs(CStateSendBuffer* pStateSendBuffer);

    ///
    /// @brief
    ///     更新SMV发送帧
    ///
    /// @param[in]  stDspRecvData   虚拟通道一个完整波形的采样值
    ///
    /// @return
    ///     bool    发送成功返回true
    ///
    bool UpdateSMVFrames(CDspRecvData& stDspRecvData);

    ///
    /// @brief
    ///    更新SMV缓冲区中的数据帧
    ///
    /// @param[in]  pStateSendBuffer    指向缓冲区的指针
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool UpdateSMVFrames(CStateSendBuffer* pStateSendBuffer);

    ///
    /// @brief
    ///    停止Ft3数据发送
    ///
    bool StopFt3Send();

protected:
    ///
    /// @brief
    ///    获得Ft3发送配置
    ///
    /// @param[out]  stTxCfg    保存配置信息的结构体
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __GetFt3SendCfg(FT3_TX_CFG& stTxCfg);

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
    enumErrorCode __FillFt3SendBuffer(unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CDspRecvData& stDspRecvData, uint32_t& uiFramesTotalLen);

private:
    CFrameSend*                         m_pDataSendManager;             ///< 数据发送管理器对象指针
    HANDLE                              m_hGPMDevice;                   ///< GPMC总线句柄
    DsmPortName                         m_enPortName;                   ///< Ft3实例对应的实际端口
    bool                                m_bIsInited;                    ///< Ft3接收是否已经初始化

    HANDLE                              m_hFileMaping;                  ///< 驱动创建的到内存的文件映射句柄
    Map_File_Header*                    m_pMapHeader;                   ///< 指向内存映射文件的最前端
    unsigned char*                      m_pMapDataBuffer;               ///< 指向内存映射文件的数据缓冲区

    NanRuiFT3Msg*                       m_pNanRuiFT3Msg;                ///< 南瑞FT3帧管理器
    GuoWangFT3Msg*                      m_pGuoWangFT3Msg;               ///< 国网FT3帧管理器

    uint16_t                            m_usNanRuiBuffer[NANRUI_FT3_MSG_WORD];      ///< 存放南瑞发送帧的缓冲区
    uint16_t                            m_usGuoWangBuffer[GUOWANG_FT3_MSG_WORD];    ///< 存放国网发送帧的缓冲区
    uint16_t                            m_usChannelValArr[GUOWANG_FT3_MSG_WORD];    ///< 存放通道值的数组

    CFT3Cb*                             m_pSMVSendCb;                   ///< 待发送的SMV控制块
};
#endif
