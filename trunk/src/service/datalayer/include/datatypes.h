/// @file
///
/// @brief
///     定义数据发送接收用的一些类型定义
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.3.22
///
/// 修改历史：
///

#ifndef _DATA_TYPES_H
#define _DATA_TYPES_H

#include <string>
#include "src\include\stdint.h"
#include "src\include\pub_datatypes.h"

///
/// @brief
///    定义光串口标识，标识具体数据与 IOCTRL_IN_PARAM 中nDeviceItem 设备标识数值一致
///    可以直接用于DeviceIoControl函数中
///
enum DsmPortName{
    NET_PORT_LIGHT_A = 0,                                               ///< 以太网光口A
    NET_PORT_LIGHT_B = 1,                                               ///< 以太网光口B
    NET_PORT_ELECTRONIC = 2,                                            ///< 电以太网口
    FT3_PORT_LIGHT_A = 3,                                               ///< 光口A,接收
    FT3_PORT_LIGHT_B = 4,                                               ///< 光口B,接收
    FT3_PORT_LIGHT_C = 5,                                               ///< 光口C,发送
    LIGHT_PORT_IRIG_B = 6,                                              ///< b码同步接口
    DSM_PORT_UNKOWN                                                     ///< 未知端口
};


///
/// @brief
///    定义FT3使用的采样率，采样率定义数值与FT3控制字中的数据相同
///    作为实际数值和ft3控制字之间的转换来使用，应用层实际获取到的是实际的采样值，如4000这样的数值
///
enum FT3_SAMP_RATE{
    FT3_SAMP_RATE_UNKOWN = 0,                                           ///< 未知采样率
    FT3_SAMP_RATE_4K,                                                   ///< 采样率4000
    FT3_SAMP_RATE_5K,                                                   ///< 采样率5000
    FT3_SAMP_RATE_8K,                                                   ///< 采样率8000
    FT3_SAMP_RATE_10K,                                                  ///< 采样率10000
    FT3_SAMP_RATE_12P8K                                                 ///< 采样率12800
};

///
/// @brief
///    定义Ft3使用的波特率,波特率定义数值与FT3控制字中的数据相同
///
enum FT3_BAUD_RATE{
    FT3_BAUD_RATE_UNKOWN = 0,                                           ///< 未知波特率
    FT3_BAUD_RATE_2P5M = 1,                                             ///< 波特率2.5M
    FT3_BAUD_RATE_5M = 2,                                               ///< 波特率5M
    FT3_BAUD_RATE_10M = 3                                               ///< 波特率10M
};

enum FrameSendType{
    SEND_NONE = 0,
    SEND_MU_SMV   = 0x55AA1122,
    SEND_FT3_SMV  = 0x55AA2211,
    SEND_GOOSE    = 0xAA551212
};

///
/// @brief
///    描述通道相别信息
///
enum ChannelPhase{
    CHANNEL_PHASE_UNKOWN  = 0,                                          ///< 通道相别未知
    CHANNEL_PHASE_A = 1,                                                ///< 通道相别为A相
    CHANNEL_PHASE_B = 2,                                                ///< 通道相别为B相
    CHANNEL_PHASE_C = 3,                                                ///< 通道相别为C相
    CHANNEL_PHASE_X = 4,                                                ///< 通道相别为X相，同期/母线/抽取
    CHANNEL_PHASE_N = 5                                                 ///< 通道相别为N相，中性点
};

///
/// @brief
///    scl文件中控制块类型
///
enum CBType
{
    CB_TYPE_UNKOWN = 0,                                                 ///< 控制块类型未知
    CB_TYPE_SMV,                                                        ///< 控制块类型为采样值
    CB_TYPE_GOOSE,                                                      ///< 控制块类型为goose
};

///
/// @brief
///    电流类型
///
enum eCurrentType
{
    CURRENT_TYPE_UNKNOWN = 0,
    CURRENT_TYPE_AC,                                                    ///< 交流电
    CURRENT_TYPE_DC                                                     ///< 直流电
};

///
/// @brief
///    Fpga定义的时间同步方式
///
enum FpgaTimeSyncMode{
    FPGA_TIME_SYNC_NONE       = 0,                                      ///< 不使用校时
    FPGA_TIME_SYNC_IRIGB_POS  = 1,                                      ///< 正向B码
    FPGA_TIME_SYNC_IRIGB_NEG  = 2,                                      ///< 反向B码
    FPGA_TIME_SYNC_IEEE1588_A = 3,                                      ///< IEEE1588对时
    FPGA_TIME_SYNC_IEEE1588_B = 4,                                      ///< IEEE1588对时
    FPGA_TIME_SYNC_IEEE1588_C = 5                                       ///< IEEE1588对时
};

///
/// @brief
///    定义光串口发送数据类型（FT3/B码）
///
enum enumFt3TxStyle{
    FT3_TX_STYLE_FT3 = 0,                                               ///< 发送FT3数据
    FT3_TX_STYLE_IRIG_B                                                 ///< 发送B码数据
};


///
/// @brief
///    定义当前的操作模式
///
enum DSM_OPERATE_MODE
{
    DSM_MODE_UNKOWN = 0,                                                ///< 模式未知
    DSM_MODE_MAIN_PAGE,                                                 ///< 当前处于主界面
    DSM_MODE_QUIT_CURRENT_PAGE,                                         ///< 退出某个业务界面时设置模式
    DSM_MODE_DETECT,                                                    ///< 探测
    DSM_MODE_FILTER,                                                    ///< 过滤
    DSM_MODE_SMV_RECV_OSCILLOGRAM,                                      ///< 波形图
    DSM_MODE_SMV_RECV_EFFECTIVE_VALUE,                                  ///< 有效值
    DSM_MODE_SMV_RECV_PHASOR,                                           ///< 相量
    DSM_MODE_SMV_RECV_XU_LIANG,                                         ///< 序量
    DSM_MODE_SMV_RECV_POWER,                                            ///< 功率
    DSM_MODE_SMV_RECV_HARMONIC,                                         ///< 谐波
    DSM_MODE_SMV_RECV_DOUBLE_AD,                                        ///< 双AD
    DSM_MODE_SMV_RECV_PHASE_CHECK,                                      ///< 核相
    DSM_MODE_SMV_RECV_POLARITY,                                         ///< 极性
    DSM_MODE_SMV_RECV_STATUS_WATCH,                                     ///< 状态监视
    DSM_MODE_SMV_RECV_DISPERSION,                                       ///< 报文离散度
    DSM_MODE_SMV_RECV_FRAME_STATIC,                                     ///< 报文统计
    DSM_MODE_SMV_RECV_FRAME_MONITOR,                                    ///< 报文监测
    DSM_MODE_SMV_RECV_RECORD_WAVE,                                      ///< net smv录波
    DSM_MODE_SMV_RECV_REAL_TIME_MSG,                                    ///< 实时报文

    DSM_MODE_GOOSE_RECV_REALTIME_VAL,                                   ///< 实时值
    DSM_MODE_GOOSE_RECV_CHANGE_LIST,                                    ///< 变位列表
    DSM_MODE_GOOSE_RECV_FRAME_STATIC,                                   ///< 报文统计
    DSM_MODE_GOOSE_RECV_FRAME_MONITOR,                                  ///< 报文监测
    DSM_MODE_GOOSE_RECV_REAL_TIME_MSG,                                  ///< 实时报文
    DSM_MODE_GOOSE_RECV_RECORD_WAVE,                                    ///< 录波
    DSM_MODE_GOOSE_RECV_DI,                                             ///< 手动试验、状态序列的DI

    DSM_MODE_FT3_RECV_RECORD_WAVE,                                      ///< ft3录波，使用pcap格式

    DSM_MODE_NET_RECV_1588                                              ///< 接收1588数据帧

};

///
/// @brief
///    定义不同的厂商类型
///
enum enumSmvRecvConfigTemplateType
{
    SMV_RECV_CONFIG_TEMPLATE_TYPE_UNKNOWN = 0,

    SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_BODIAN,                       ///< 博电设备定义Ft3标准帧通道默认描述,12通道
    SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_KAIMO,                        ///< 凯默设备定义Ft3标准帧通道默认描述,12通道
    SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_60044_8,                      ///< IEC60044-8定义通道默认描述,12通道

    SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BODIAN,                       ///< 博电设备定义Ft3扩展帧通道默认描述,22通道
    SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_KAIMO,                        ///< 凯默设备定义Ft3扩展帧通道默认描述,22通道
    SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BOOK,                         ///< 本设备定义通道默认描述,22通道

    SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BODIAN,                           ///< 博电9-2默认通道配置信息,8通道
    SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_KAIMO,                            ///< 凯默9-2默认通道配置信息,12通道
    SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BOOK,                             ///< 《智能变电站原理及测试技术》P62页的18通道模板

    SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2,                                  ///< 本设备定义9-2通道默认描述,30通道
    SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_LESS_CHANNEL,                     ///< 本设备定义9-2通道（通道数小于13）默认描述,12通道

    SMV_RECV_CONFIG_TEMPLATE_FROME_SCL,                                 ///< 从SCL文件加载对应的配置信息
};

///
/// @brief
///    定义预定义通道信息结构体
///
struct PREDEFINED_CHANNEL_INFO
{
    uint32_t                nIndex;                     ///< 通道索引
    enumChannelType         enChannelType;              ///< 通道类型
    ChannelPhase            enChannelPhase;             ///< 通道相别
    enumChannelValueType    enChannelValueType;         ///< 通道是保护数据还是测量数据
    std::wstring            strChannelDesc;             ///< 通道描述
    std::wstring            strVirtualChannel;          ///< 映射的虚拟通道名
};

///
/// @brief
///    标准Ft3预定通道信息，IEEE60044-8模板
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护A相电流",         L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护B相电流",         L"Ib1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护C相电流",         L"Ic1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_MEASURE,  L"中性点电流",          L"Ix1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_MEASURE,  L"测量A相电流",         L"Ia1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_MEASURE,  L"测量B相电流",         L"Ib1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_MEASURE,  L"测量C相电流",         L"Ic1"},
    { 7,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"A相电压",             L"Ua1"},
    { 8,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"B相电压",             L"Ub1"},
    { 9,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"C相电压",             L"Uc1"},
    { 10,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"中性点电压",          L"Ux1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"母线电压",            L"Ux1"}
};


///
/// @brief
///    南瑞标准Ft3预定通道信息，凯默预定义通道
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_FT3_STD_KAIMO[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护A相电流",         L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护B相电流",         L"Ib1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护C相电流",         L"Ic1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_PROTECT,  L"零序保护电流",        L"Ix1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_MEASURE,  L"测量A相电流",         L"Ia1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_MEASURE,  L"测量B相电流",         L"Ib1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_MEASURE,  L"测量C相电流",         L"Ic1"},
    { 7,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"A相电压",             L"Ua1"},
    { 8,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"B相电压",             L"Ub1"},
    { 9,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"C相电压",             L"Uc1"},
    { 10,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"零序保护电压",        L"Ux1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"母线电压",            L"Ux1"}
};

///
/// @brief
///    南瑞标准Ft3预定通道信息，博电预定义通道
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_FT3_STD_BODIAN[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护A相电流",         L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护B相电流",         L"Ib1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护C相电流",         L"Ic1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护电流",            L"Ix1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_MEASURE,  L"测量A相电流",         L"Ia1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_MEASURE,  L"测量B相电流",         L"Ib1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_MEASURE,  L"测量C相电流",         L"Ic1"},
    { 7,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"A相电压",             L"Ua1"},
    { 8,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"B相电压",             L"Ub1"},
    { 9,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"C相电压",             L"Uc1"},
    { 10,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"电压",                L"Ux1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"电压",                L"Ux1"}
};


///
/// @brief
///    国网预定义通道信息
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_PROTECT,    L"保护A相电流1",     L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_PROTECT,    L"保护A相电流2",     L"Ia1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_PROTECT,    L"保护B相电流1",     L"Ib1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_PROTECT,    L"保护B相电流2",     L"Ib1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_PROTECT,    L"保护C相电流1",     L"Ic1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_PROTECT,    L"保护C相电流2",     L"Ic1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_MEASURE,    L"测量A相电流",      L"Ia1"},
    { 7,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_MEASURE,    L"测量B相电流",      L"Ib1"},
    { 8,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_MEASURE,    L"测量C相电流",      L"Ic1"},
    { 9,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_MEASURE,    L"中性点电流1",      L"Ix1"},
    { 10,    CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_MEASURE,    L"中性点电流2",      L"Ix1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"A相电压1",         L"Ua1"},
    { 12,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"A相电压2",         L"Ua1"},
    { 13,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"B相电压1",         L"Ub1"},
    { 14,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"B相电压2",         L"Ub1"},
    { 15,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"C相电压1",         L"Uc1"},
    { 16,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"C相电压2",         L"Uc1"},
    { 17,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"中性点电压1",      L"Ux1"},
    { 18,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"中性点电压2",      L"Ux1"},
    { 19,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"母线电压1",        L"Ux2"},
    { 20,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"母线电压2",        L"Ux2"},
    { 21,    CHANNEL_TYPE_UNKOWN,          CHANNEL_PHASE_UNKOWN,   CHANNEL_VALUE_TYPE_UNKNOWN,    L"保留",             L""},
};

///
/// @brief
///    凯默ft3扩展预定义通道信息
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_FT3_EXT_KAIMO[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_PROTECT,    L"保护A相电流1",     L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_PROTECT,    L"保护A相电流2",     L"Ia1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_PROTECT,    L"保护B相电流1",     L"Ib1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_PROTECT,    L"保护B相电流2",     L"Ib1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_PROTECT,    L"保护C相电流1",     L"Ic1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_PROTECT,    L"保护C相电流2",     L"Ic1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_PROTECT,    L"零序保护电流1",    L"Ix1"},
    { 7,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_PROTECT,    L"零序保护电流2",    L"Ix1"},
    { 8,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_MEASURE,    L"测量A相电流",      L"Ia1"},
    { 9,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_MEASURE,    L"测量B相电流",      L"Ib1"},
    { 10,    CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_MEASURE,    L"测量C相电流",      L"Ic1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"A相保护电压1",     L"Ua1"},
    { 12,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"A相保护电压2",     L"Ua1"},
    { 13,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"B相保护电压1",     L"Ub1"},
    { 14,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"B相保护电压2",     L"Ub1"},
    { 15,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"C相保护电压1",     L"Uc1"},
    { 16,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"C相保护电压2",     L"Uc1"},
    { 17,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"零序保护电压1",    L"Ux1"},
    { 18,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"零序保护电压2",    L"Ux1"},
    { 19,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"母线电压1",        L"Ux2"},
    { 20,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"母线电压2",        L"Ux2"},
    { 21,    CHANNEL_TYPE_UNKOWN,          CHANNEL_PHASE_UNKOWN,   CHANNEL_VALUE_TYPE_UNKNOWN,    L"保留",             L""},
};

///
/// @brief
///    博电ft3扩展预定义通道信息
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_FT3_EXT_BODIAN[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护A相电流",         L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护B相电流",         L"Ib1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护C相电流",         L"Ic1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护电流",            L"Ix1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_MEASURE,  L"测量A相电流",         L"Ia1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_MEASURE,  L"测量B相电流",         L"Ib1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_MEASURE,  L"测量C相电流",         L"Ic1"},
    { 7,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"A相电压",             L"Ua1"},
    { 8,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"B相电压",             L"Ub1"},
    { 9,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"C相电压",             L"Uc1"},
    { 10,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"电压",                L"Ux1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"电压",                L"Ux1"},
    { 12,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"电压",                L"Ux1"},
    { 13,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"电压",                L"Ux1"},
    { 14,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"电压",                L"Ux1"},
    { 15,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"电压",                L"Ux1"},
    { 16,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"电压",                L"Ux1"},
    { 17,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"电压",                L"Ux1"},
    { 18,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"电压",                L"Ux1"},
    { 19,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"电压",                L"Ux1"},
    { 20,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"电压",                L"Ux1"},
    { 21,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"电压",                L"Ux1"},
};

///
/// @brief
///    9-2Le在没有解析文件时使用的预定义通道信息
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_9_2LE[] = {
    { 0,     CHANNEL_TYPE_SMV_TIME,        CHANNEL_PHASE_UNKOWN,   CHANNEL_VALUE_TYPE_UNKNOWN,     L"额定延时",       L""},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"A相保护电流AD1", L""},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"A相保护电流AD2", L""},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"B相保护电流AD1", L""},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"B相保护电流AD2", L""},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"C相保护电流AD1", L""},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"C相保护电流AD2", L""},
    { 7,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"A相测量电流",    L""},
    { 8,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"B相测量电流",    L""},
    { 9,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"C相测量电流",    L""},
    { 10,    CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"中性点电流1",    L""},
    { 11,    CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"中性点电流2",    L""},
    { 12,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"A相电压1",       L""},
    { 13,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"A相电压2",       L""},
    { 14,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"B相电压1",       L""},
    { 15,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"B相电压2",       L""},
    { 16,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"C相电压1",       L""},
    { 17,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"C相电压2",       L""},
    { 18,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"中性点电压1",    L""},
    { 19,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"中性点电压2",    L""},
    { 20,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"母线电压1",      L""},
    { 21,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"母线电压2",      L""},
    { 22,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"电压",           L""},
    { 23,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"电压",           L""},
    { 24,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"电压",           L""},
    { 25,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"电压",           L""},
    { 26,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"电压",           L""},
    { 27,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"电压",           L""},
    { 28,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"电压",           L""},
    { 29,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"电压",           L""}
};

///
/// @brief
///    9-2博电默认通道
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_9_2LE_BODIAN[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护电流1",         L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护电流2",         L"Ib1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护电流3",         L"Ic1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护电流4",         L"Ix1"},
    { 4,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"保护电压1",         L"Ua1"},
    { 5,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"保护电压2",         L"Ub1"},
    { 6,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"保护电压3",         L"Uc1"},
    { 7,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"保护电压4",         L"Ux1"}
};

///
/// @brief
///    9-2凯默默认通道
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护A相电流",         L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护B相电流",         L"Ib1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_PROTECT,  L"保护C相电流",         L"Ic1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_PROTECT,  L"零序保护电流",        L"Ix1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_PROTECT,  L"测量A相电流",         L"Ia1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_PROTECT,  L"测量B相电流",         L"Ib1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_PROTECT,  L"测量C相电流",         L"Ic1"},
    { 7,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"A相电压",             L"Ua1"},
    { 8,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"B相电压",             L"Ub1"},
    { 9,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"C相电压",             L"Uc1"},
    { 10,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"零序保护电压",        L"Ux1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"母线电压",            L"Ux1"}
};

///
/// @brief
///    9-2Le在没有解析文件时使用的预定义通道信息,通道小于13个时使用
///
#define PREDEFINED_CHANNEL_INFO_9_2LE_LESS PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO

///
/// @brief
///    《智能变电站原理及测试技术》P62页的18通道模板
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_9_2LE_BOOK[] = {
    { 0,     CHANNEL_TYPE_SMV_TIME,        CHANNEL_PHASE_UNKOWN,   CHANNEL_VALUE_TYPE_UNKNOWN,     L"额定延时",        L"t1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_PROTECT,     L"A相保护电流AD1",  L"Ia1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_PROTECT,     L"A相保护电流AD2",  L"Ia1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_PROTECT,     L"B相保护电流AD1",  L"Ib1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_PROTECT,     L"B相保护电流AD2",  L"Ib1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_PROTECT,     L"C相保护电流AD1",  L"Ic1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_PROTECT,     L"C相保护电流AD2",  L"Ic1"},
    { 7,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_PROTECT,     L"A相测量电流",     L"Ia1"},
    { 8,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_PROTECT,     L"B相测量电流",     L"Ib1"},
    { 9,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_PROTECT,     L"C相测量电流",     L"Ic1"},
    { 10,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"A相保护电压AD1",  L"Ua1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"A相保护电压AD2",  L"Ua1"},
    { 12,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"B相保护电压AD1",  L"Ub1"},
    { 13,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"B相保护电压AD2",  L"Ub1"},
    { 14,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"C相保护电压AD1",  L"Uc1"},
    { 15,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"C相保护电压AD2",  L"Uc1"},
    { 16,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"母线电压AD1",     L"Ux1"},
    { 17,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"母线电压AD2",     L"Ux1"}
};

///
/// @brief
///    定义通道模板
///
struct SmvChannelTemplate 
{
    enumSmvRecvConfigTemplateType   enTemplateType;                     ///< 模板标识
    const PREDEFINED_CHANNEL_INFO*  pChannelTempalte;                   ///< 指向通道模板
    uint32_t                        nTemplateSize;                      ///< 通道数目
};

#define ARRAY_SIZE(ARRAY)  (sizeof(ARRAY)/sizeof(ARRAY[0]))

#endif //end _DATA_TYPES_H 