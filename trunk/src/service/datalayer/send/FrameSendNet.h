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

#ifndef __FRAME_SEND_NET_H__
#define __FRAME_SEND_NET_H__
#include "FrameSendStruct.h"
#include "StateTestResult.h"
#include "src\service\datalayer\config\GooseCb.h"
#include "src\service\datalayer\config\IEC9_1Cb.h"
#include "src\service\datalayer\config\IEC9_2Cb.h"

#define ETHER_IEC91_CHANNEL_MAX           (ETHER_FRAME_MAX_LEN/2)       ///< IEC61850-9-1最大通道数
#define ETHER_IEC92_CHANNEL_MAX           (ETHER_FRAME_MAX_LEN/4)       ///< IEC61850-9-2最大通道数

class CFrameSend;
class CFrameSendNet
{
public:
    CFrameSendNet(CFrameSend* pFrameSend, HANDLE hGPMDevice, DsmPortName enPortName);
    ~CFrameSendNet(void);

public:
    ///
    /// @brief
    ///    初始化Net发送
    ///
    bool InitNet(const wchar_t *pNotifyEvtName, const wchar_t *pMapFileName, uint32_t nMapFileLen );

    ///
    /// @brief
    ///    反初始化Net发送
    ///
    void UnInitNet();

    ///
    /// @brief
    ///    清空保存待发送SMV控制块的容器
    ///
    void RemoveAllSMVSendCbs();

    ///
    /// @brief
    ///    添加待发送的SMV控制块
    ///
    /// @param[in]  pNewCb    SMV控制块
    ///
    /// @return
    ///     bool    添加成功返回true
    ///
    bool AddSMVSendCb(CBaseCb* pNewCb);

    ///
    /// @brief
    ///    清空保存待发送GOOSE控制块的容器
    ///
    void RemoveAllGOOSESendCbs();

    ///
    /// @brief
    ///    添加待发送的GOOSE控制块
    ///
    /// @param[in]  pNewCb    GOOSE控制块
    ///
    /// @return
    ///     bool    添加成功返回true
    ///
    bool AddGooseSendCb(CGooseCb* pNewCb);

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
    enumErrorCode PrepareSmvSendBuffer(CDspRecvData& stDspRecvData, CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    发送SMV控制块
    ///
    /// @param[in]  stDspRecvData    虚拟通道一个完整波形的采样值
    ///
    /// @return
    ///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
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
    ///    停止SMV底层数据发送
    ///
    bool StopSMVSend();

    ///
    /// @brief
    ///    准备GOOSE控制块发送帧
    ///
    /// @param[in]  pStateTestResult    指向结果集对象的指针，用于保存GOOSE发送帧
    ///
    /// @return
    ///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
    ///
    enumErrorCode PrepareGooseSendBuffer(CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    准备GOOSE控制块更新帧
    ///
    /// @param[in]  pStateSendBuffer    指向GOOSE发送帧
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool PrepareGooseUpdateBuffer(CStateSendBuffer* pStateSendBuffer);

    ///
    /// @brief
    ///    发送GOOSE控制块
    ///
    /// @return
    ///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
    ///
    enumErrorCode SendGooseCbs();

    ///
    /// @brief
    ///    发送GOOSE控制块
    ///
    /// @param[in]  pStateSendBuffer    指向缓冲区的指针
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool SendGooseCbs(CStateSendBuffer* pStateSendBuffer);

    ///
    /// @brief
    ///     更新GOOSE发送帧
    ///
    /// @return
    ///     bool    发送成功返回true
    ///
    bool UpdateGooseFrames();

    ///
    /// @brief
    ///     更新GOOSE发送帧
    ///
    /// @param[in]  pStateSendBuffer    指向缓冲区的指针
    ///
    /// @return
    ///     bool    发送成功返回true
    ///
    bool UpdateGooseFrames(CStateSendBuffer* pStateSendBuffer);

    ///
    /// @brief
    ///    停止GOOSE底层数据发送
    ///
    bool StopGooseSend();

    ///
    /// @brief
    ///    计算控制块中所有通道的偏移量
    ///
    /// @param[in]  pStateSendBuffer    指向发送帧缓冲对象的指针
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool CalcChannelOffset(CStateSendBuffer* pStateSendBuffer);

protected:
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
    enumErrorCode __FillIec91SendBuffer(unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CDspRecvData& stDspRecvData, IOCTRL_IN_PARAM_NET_BIG& stMuSendParam, uint32_t& uiFramesTotalLen);

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
    enumErrorCode __FillIec92SendBuffer( unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CDspRecvData& stDspRecvData, IOCTRL_IN_PARAM_NET_BIG& stMuSendParam, uint32_t& uiFramesTotalLen);

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
    enumErrorCode __FillGooseSendBuffer(unsigned char* pDataBuffer, unsigned int uiDataBufferLen, UTC_TIME_t& stUtcTime, IOCTRL_IN_PARAM_NET_BIG& stMuSendParam);

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
    enumErrorCode __FillGooseSendBuffer(unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CStateSendBuffer* pNewSendBuffer, uint32_t& uiFramesTotalLen);

private:
    CFrameSend*                         m_pDataSendManager;             ///< 数据发送管理器对象指针
    HANDLE                              m_hGPMDevice;                   ///< GPMC总线句柄
    DsmPortName                         m_enPortName;                   ///< Ft3实例对应的实际端口
    bool                                m_bIsInited;                    ///< 网络接收是否已经初始化过

    HANDLE                              m_hFileMaping;                  ///< 驱动创建的到内存的文件映射句柄
    Map_File_Header*                    m_pMapHeader;                   ///< 指向内存映射文件的最前端
    unsigned char*                      m_pMapDataBuffer;               ///< 指向内存映射文件的数据缓冲区
    
    Sav91Msg*                           m_pSav91Msg;                    ///< IEC61850-9-1采样值报文的编解码接口
    Sav92Msg*                           m_pSav92Msg;                    ///< IEC61850-9-2采样值报文的编解码接口
    GooseMsg*                           m_pGooseMsg;                    ///< GOOSE报文的编解码接口

    uint8_t                             m_pucFrameBuffer[ETHER_FRAME_MAX_LEN];          ///< 存放网络报文帧的缓冲区
    uint16_t                            m_pusIEC91ValArr[ETHER_IEC91_CHANNEL_MAX];      ///< 存放IEC61850-9-1一帧报文中所有通道值的缓冲区
    uint32_t                            m_puiIEC92ValArr[ETHER_IEC92_CHANNEL_MAX];      ///< 存放IEC61850-9-2一帧报文中所有通道值的缓冲区
    uint32_t                            m_puiIEC92QualityArr[ETHER_IEC92_CHANNEL_MAX];  ///< 存放IEC61850-9-2一帧报文中所有通道品质值得缓冲区

    std::vector<CBaseCb*>               m_vecSMVSendCbs;                ///< 待发送的SMV控制块
    std::vector<CGooseCb*>              m_vecGOOSESendCbs;              ///< 待发送的GOOSE控制块
};
#endif
