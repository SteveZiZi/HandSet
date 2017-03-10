///// @file
///
/// @brief
///     定义发送数据帧的结构参数等信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.3.19
///
/// 修改历史：
///     Modified by:    chao
///     Modified date:  2013.6.19
///     Version:        1.0.1
///     Descriptions:   添加了发送、接收结构体
///

#ifndef __FRAME_SEND_STRUCT_H__
#define __FRAME_SEND_STRUCT_H__
#include <map>
#include <string>
#include <Windows.h>
#include "src\include\stdint.h"
#include "src\dspclient\include\Dsp.h"
#include "src\include\p_wndusermsg_def.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\include\ctrlcode_def.h"

#define WM_DI_CHANGED                   (PRIVATE_WND_USERID + 1)        ///< DI（输入通道）值发生了变化
#define WM_WHETHER_SWITCH               (PRIVATE_WND_USERID + 2)        ///< 在状态序列试验时，是否手动切换到下一个状态
#define WM_STATUS_CHANGED               (PRIVATE_WND_USERID + 3)        ///< 已切换到状态序列中的下一个状态
#define WM_MANUAL_TEST_RESULT           (PRIVATE_WND_USERID + 4)        ///< 手动试验测试结果
#define WM_STATUS_SEQUENCE_RESULT       (PRIVATE_WND_USERID + 5)        ///< 状态序列测试结果

#define MAX_ASDU_NUM                    (8)                             ///< 一组SMV报文，最多支持8个ASDU
#define TX_TEST_STYLE_INDEX             (19)                            ///< 测试模式位置索引（手动试验、状态序列）

///
/// @brief
///    Ft3发送数据的配置信息,传递给DeviceIOControl使用
///
struct FT3_TX_CFG{
    uint16_t        nFrameLength;                                       ///< 一共发送多少数据总长度，值为发送数目nFramesCnt 与 国网/南瑞 数据帧长度的乘积
    uint16_t        nFrameCount;                                        ///< 一次向FPGA中传送多少帧报文
    uint16_t        uFramesStyle;                                       ///< 当前FT3报文的帧格式,对应Ft3CtlWordSend结构体
    uint16_t        nReserve;
};

///
/// @brief
///    DSP数据发送
///
struct CDspSendData
{
    SmvGenData                      m_stSendData;                       ///< 发送参数
    std::map<std::wstring, int>     m_mapChannelName2Index;             ///< 通道名——>索引 的映射
};

///
/// @brief
///    DSP数据接收
///
struct CDspRecvData
{
    SMV_GEN_DATA_CALC_RT            m_stRecvData;                       ///< 接收缓冲区
    int                             m_iFrequency;                       ///< 采样频率
    int                             m_iSmvCount;                        ///< 每周波的采样点数
    std::map<std::wstring, int>     m_mapChannelName2Index;             ///< 通道名——>索引 的映射
};

///
/// @brief
///    SMV、GOOSE发送状态
///
enum eSendStatus
{
    SEND_STATUS_STOPPED = 0,                                            ///< 已停止
    SEND_STATUS_PREPARE,                                                ///< 准备发送参数
    SEND_STATUS_SENDING                                                 ///< 正在发送
};

///
/// @brief
///    测试模式（手动试验、状态序列）
///
enum enumTxTestStyle
{
    TX_TEST_STYLE_MANUAL_TEST = 0,                                      ///< 手动试验发送模式
    TX_TEST_STYLE_STATUS_SEQUENCE                                       ///< 状态序列发送模式
};

///
/// @brief
///    错误码
///
enum enumErrorCode
{
    ERROR_CODE_SUCCESS = 0,                                             ///< 成功
    ERROR_CODE_FAILURE,                                                 ///< 失败
    ERROR_CODE_CANCEL,                                                  ///< 用户取消
    ERROR_SMV_TOO_MANY_CHANNEL_ASDU,                                    ///< SMV控制块的通道数太多或ASDU数太大
    ERROR_GOOSE_TOO_MANY_CHANNEL,                                       ///< GOOSE控制块的通道数太多
};

#endif //end __FRAME_SEND_STRUCT_H__