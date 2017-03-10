/// @file
///
/// @brief
///     保存一些配置信息，方便对全局信息进行修改
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.3.23
///
/// 修改历史：
///

#ifndef __CONFIG_H_
#define __CONFIG_H_

///
/// @brief
///    控制RETAILMSG是否打印调试信息，默认Debug版本打印，Release版本不打印
///     OUTPUT_LOG_MSG 为真打印错误信息,MSDN上提示说使用RETAILMSG会增加文件尺寸
///     可以定义 SHIP_BUILD 宏，阻止RETAILMSG宏打印信息
///     调试错误信息使用RETAILMSG打印，正常的信息打印可以通过GEN_LOG_MESSAGE关闭调试输出
///
#include <Dbgapi.h>
#define GEN_LOG_MESSAGE 0
#ifndef OUTPUT_LOG_MSG
#ifdef DEBUG
#define     OUTPUT_LOG_MSG      1
#else
#define     OUTPUT_LOG_MSG      0
#endif
#endif

#include <string>
#include "src\include\stdint.h"
#include "inc\thread_priority.h"

#define     DRIVER_BUFFER_USE_CPY   0                       ///< 使用驱动提供缓冲区时，缓冲区内存是否单独拷贝一份
#define     ETHERNET_MAC_LEN        6                       ///< 定义以太网mac地址长度
#define     SMV_GOOSE_APPID_LEN     2                       ///< 定义smv，goose的appid长度
#define     SMV_MAX_POINT_PER_WAVE  256                     ///< 定义一个周波最多采样点数，12.8k对应256点
#define     SMV_PORLARITY_CHECK_POSITIVE    1               ///< 极性检查结果正极性
#define     SMV_PORLARITY_CHEKC_NEGATIVE    -1              ///< 极性检查结果负极性

const uint32_t DSM_MAX_SMV_CTRL_BLK   = 6;                  ///< 定义设置一次可以选择的采样值控制块数目，应用层使用
const uint32_t DSM_MAX_GOOSE_CTRL_BLK = 10;                 ///< 定义设置一次可以选择的goose控制块数目，应用层使用
const uint32_t DSM_MAX_CONFIG_CTRL_BLK = 2;                 ///< 接收设置中设置控制块的最大数量，一般设置控制块数量为1，核相时控制块数量为2
const uint32_t DSM_MAX_OSCILLOGRAM_WAVE_COUNT = 10;         ///< 定义波形显示时，数据层缓存一个通道的采样周波个数
const uint32_t DSM_DEFAULT_POINT_PER_WARE = 80;             ///< 定义默认的一个周波的采样点数，默认为80点，对应采样率4k
//定义波形显示时，数据层缓存一次缓存的采样点个数
const uint32_t DSM_DEFAULT_OSCILLOGRAM_WAVE_POINT = DSM_MAX_OSCILLOGRAM_WAVE_COUNT * DSM_DEFAULT_POINT_PER_WARE;

//核相时缓存的两个控制块数据量
const uint32_t DSM_DEFAUTL_PHASE_CHECK_DEQUE_SIZE = 80*50;
//核相时发送给dsp的通道数目
#define PHASE_CHECK_CHANNELS    6

const uint32_t DSM_MU_RX_TIMEOUT_MIN        = 1;            ///< MU接收超时时间最小1ms
const uint32_t DSM_MU_RX_TIMEOUT_MAX        = 21474;        ///< MU接收超时时间最大21474ms

//一个goose控制块保留的变位历史记录个数
const uint32_t DSM_MAX_GOOSE_CHANGELIST_SIZE = 20;          ///< 在goose变位列表中，记录一个goose控制块的变位次数,实际记录要多一次初始数据

///
/// @brief
///    设置默认系统采集电网的频率
///
const uint32_t DSM_CONFIG_DEFAULT_ELEC_FREQUENCY = 50;

///
/// @brief
///    设置一次默认接收的采样点的周波数
///
const uint32_t DSM_CONFIG_DEFAULT_RECV_WAVE_COUNT = 10;

///
/// @brief
///    设置一次接收最少的数据帧数，设置过小可能会频繁触发通知事件，需要应用更快的处理数据
///
const uint32_t DSM_CONFIG_RECV_SAMPLE_COUNT_MIN = 200;

///
/// @brief
///    设置录波时使用缓冲区大小
///
const uint32_t DSM_DEFAULT_RECORD_WAVE_BUFFER_SIZE_IN_BYTE = 20 * 1024 * 1024;

///
/// @brief
///    是否实时录波,实时录波会限制录波文件大小，不会丢数据
///    非实时录波可以录制大文件，可能会丢点
///
#define DSM_RECORD_WARE_REALTIME    1

///
/// @brief
///    定义波形图是否显示有效值/相位信息,1为显示，0为不显示
///
#define DSM_OSCILLOGRAM_SHOW_EFFECT_PHASE   0

///
/// @brief
///    定义在12.8k的采样率下是否重新采样到4k进行有效值谐波等运算
///
#define DSM_DSP_CALC_128K_RESAMPLE_TO_4K   1

//定义配置文件运行时目录和出厂文件目录
const std::wstring DEFAULT_CFG_BACKUP = L"config_backup\\";             ///< 备份配置文件路径
const std::wstring DEFAULT_CFG_RUN    = L"dsm_usercfg\\";               ///< 运行配置文件路径,位于与可执行文件父目录的同级目录


///
/// @brief
///    网络接口获取数据的类型
///
enum NET_MODE{
    _RECV_NONE = -1,
    _9_1_9_2_DETECT = 0,
    _9_1_9_2_FILTER,
    _GOOSE_DETECT,
    _GOOSE_FILTER,
    _IEEE1588_DETECT,
    _IEEE1588_FILTER,

    _MAX_NET_NUM
};

///
/// @brief
///    FT3发送数据的控制信息，16位
///
union Ft3CtlWordSend{
    // FT3报文格式
    enum eFt3FrameType
    {
        FT3_FRAME_TYPE_UNKNOWN = 0,
        FT3_FRAME_TYPE_NANRUI,
        FT3_FRAME_TYPE_GUOWANG
    };

    struct {
        uint16_t bCntClr : 1;                                             ///<  Bit[0] 计数器清零           
        uint16_t bCRCReverse : 1;                                         ///<  CRC，0：取反 1：不取反      
        uint16_t bCRCValue : 1;                                           ///<  CRC初值，0:0x0000,1:0xFFFF  
        uint16_t bSendMode : 1;                                           ///<  FT3发送模式，0:同步         
        uint16_t bFrameType : 4;                                          ///<  报文格式，1:南瑞，2:国网    
        uint16_t bSampleRate : 4;                                         ///<  采样率                      
        uint16_t bBaudRate : 4;                                           ///<  波特率                      
    }CtrlBit;
    uint16_t nData;
};

///
/// @brief
///    FT3接收数据的控制信息，16位
///
union Ft3CtlWordRecv{
    struct {
        uint16_t bNoErr:1;                                                //通讯正常 0异常 1正常
        uint16_t bReserve:2;                                              //保留
        uint16_t bFrameType:4;                                            //规约格式，国网或南瑞
        uint16_t bSampleRate:4;                                           //采样率控制位
        uint16_t bBaudRate:4;                                             //波特率控制位
        uint16_t bCrc:1;                                                  //CRC校验控制位
    }CtrlBit;
    uint16_t nData;
};

///
/// @brief
///    Net，Ft3缓冲区开头控制头结构体，固定8*sizeof(uint16_t)长度
///
typedef union BufferHeader
{
    struct
    {
        uint16_t        nLoopItem;                      ///< 循环接收时 使用哪一个缓冲区 只能是 0 1
        uint16_t        bAppDispDone;                   ///< 标记应用还没有处理完上一次报文
        uint16_t        nFramesCount;                   ///< 内存映射文件缓冲区中接收到报文的总帧数
        uint16_t        nIsTimeOut;                     ///< 是否发生接收超时
        Ft3CtlWordRecv  stCtrlWord;                     ///< FT3报文格式控制字
        uint16_t        nCurTxBufItem;                  ///< 当前发送缓冲区的序号
        uint32_t        nActiveLength;                  ///< 缓冲区有效长度
    } Ft3Header;
    struct
    {
        uint16_t        nLoopItem;                      ///< 循环接收时 使用哪一个缓冲区 只能是 0 1
        uint16_t        bAppDispDone;                   ///< 标记应用还没有处理完上一次报文
        uint16_t        nFramesCount;                   ///< 内存映射文件缓冲区中接收到报文的总帧数
        uint16_t        nIsTimeOut;                     ///< FPGA是否接收超时
        uint16_t        nNetMode;                       ///< 当前光口的工作模式,具体见NET_MODE 枚举类型
        uint16_t        nCurSmvTxBufItem;               ///< 当前Smv发送缓冲区的序号
        uint32_t        nActiveLength;                  ///< 缓冲区有效长度，是nFramesCount帧报文占用的缓冲区的空间长度
        uint16_t        nCurGooseTxBufItem;             ///< 当前GOOSE发送缓冲区的序号
    } NetHeader;
    uint16_t nData[32];
}Map_File_Header;

//FT3控制字中用于标识数据帧时国网还是南瑞,还是其他ft3扩展帧
enum enumFt3FrameStyle
{
    _FT3_TYPE_NONE = 0,
    _FT3_TYPE_NANRUI,                                       ///< 南瑞格式的FT3报文
    _FT3_TYPE_GUOWANG,                                      ///< 国网格式的FT3报文
    _FT3_TYPE_SINGLE_PHASE,                                 ///< 单相互感器的FT3报文
    _FT3_TYPE_THREE_PHASE_CUR,                              ///< 三相电流互感器的FT3报文
    _FT3_TYPE_THREE_PHASE_VOL,                              ///< 三相电压互感器的FT3报文
    _FT3_TYPE_THREE_PHASE_CUR_VOL,                          ///< 三相电流电压互感器的FT3报文
    _FT3_TYPE_RESERVE,                                      ///< 保留
    _FT3_TYPE_MAXVALUE
};

//定义FT3不同类型数据帧长度信息
const uint16_t FT3_FRAME_LEN[_FT3_TYPE_MAXVALUE] = {0, 56, 74, 26, 34, 28, 48, 0};          //FT3报文的总长度 包括起始字符、CRC校验字符
const uint16_t FT3_FRAME_ACTIVE_LEN[_FT3_TYPE_MAXVALUE] = {0, 48, 64, 20, 28, 22, 40, 0};   //FT3数据集的长度 去除起始字符、CRC校验字符


const uint32_t GUOWANG_CHANNELS_PER_FRAME = 22;             ///< 国网一个数据帧中有22个通道
const uint32_t NANRUI_CHANNELS_PER_FRAME  = 12;             ///< 南瑞一个数据帧中有12个通道
const uint32_t NET_9_1_CHANNELS_PER_FRAME = 12;             ///< 9-1数据帧一个asdu的通道数
const uint32_t NET_9_2_LE_MAX_CHANNELS_PER_ASDU = 64;       ///< 9-2数据的最大通道数目,应用方面限制数量，与dsp交互的大小无关
const uint32_t FT3_TIME_STAMP_LEN  = 8;                     ///< FT3帧前时间戳长度
const uint32_t FT3_9_1_MAX_CHANNEL_COUNT = GUOWANG_CHANNELS_PER_FRAME;  ///< FT3/9-1数据帧通道数最大值

//
//=============================================================================
// 网络端口缓冲区定义
//=============================================================================
//
//网络端口（光口和以太网口）Smv接收缓冲区大小定义
const uint32_t NET_MAX_RECV_FRAMES            = 10000;                                                                  ///< 最多接收的报文帧数
const uint32_t NET_MAX_FRAME_LEN              = 1536;                                                                   ///< 一帧报文的最大长度
const uint32_t NET_RECV_MAP_FILE_LEN          = NET_MAX_RECV_FRAMES * NET_MAX_FRAME_LEN + sizeof(BufferHeader) ;        ///< 光以太网接收时的内存映射文件大小
const uint32_t NET_RECV_LOOP_BUF0_POS         = 0;                                                                      ///< 共享内存分为两个缓冲区，这是第一个缓冲区的位置
const uint32_t NET_RECV_LOOP_BUF1_POS         = (NET_RECV_MAP_FILE_LEN - sizeof(BufferHeader))/2;                       ///< 共享内存分为两个缓冲区，这是第二个缓冲区的位置

//网络端口Goose数据接收缓冲区定义
const uint32_t GOOSE_RECV_MAX_GROUP           = 50;                                                                     ///< 最多可以接收100组GOOSE
const uint32_t GOOSE_RECV_BUF_LEN             = NET_MAX_FRAME_LEN * GOOSE_RECV_MAX_GROUP;                               ///< 长度为1536*100
const uint32_t GOOSE_RECV_LOOP_BUF0_POS       = NET_RECV_LOOP_BUF0_POS;                                                 ///< 共享内存分为两个缓冲区，这是第1个缓冲区的位置
const uint32_t GOOSE_RECV_LOOP_BUF1_POS       = NET_RECV_LOOP_BUF1_POS;                                                 ///< 共享内存分为两个缓冲区，这是第2个缓冲区的位置

//IEEE1588接收的缓冲区位置
const uint32_t IEEE1588_RECV_MAX_GROUP        = 50;                                                                     ///< 最多可以接收50组IEEE1588
const uint32_t IEEE1588_RECV_BUF_LEN          = NET_MAX_FRAME_LEN * IEEE1588_RECV_MAX_GROUP;                            ///< 长度为1536*50
const uint32_t IEEE1588_RECV_BUF0_POS         = (GOOSE_RECV_LOOP_BUF0_POS + GOOSE_RECV_BUF_LEN);                        ///< 共享内存分为两个缓冲区，这是第1个缓冲区的位置
const uint32_t IEEE1588_RECV_BUF1_POS         = (GOOSE_RECV_LOOP_BUF1_POS + GOOSE_RECV_BUF_LEN);                        ///< 共享内存分为两个缓冲区，这是第2个缓冲区的位置

//SMV发送缓冲区定义
const uint32_t NET_SEND_MAP_FILE_LEN          = NET_RECV_MAP_FILE_LEN;                                                  ///< 以太网发送数据共享内存大小,应用程序与驱动共享的内存大小
const uint32_t NET_SMV_SEND_BUF_LEN           = 7511040;                                                                ///< 采样值发送缓冲区大小
const uint32_t NET_SMV_SEND_LOOP_BUF0_POS     = IEEE1588_RECV_BUF0_POS + IEEE1588_RECV_BUF_LEN;                         ///< 发送共享内存分为连个缓冲区，这是第一个缓冲区的位置
const uint32_t NET_SMV_SEND_LOOP_BUF1_POS     = IEEE1588_RECV_BUF1_POS + IEEE1588_RECV_BUF_LEN;                         ///< 发送共享内存分为连个缓冲区，这是第二个缓冲区的位置

//GOOSE发送缓冲区定义
const uint32_t GOOSE_SEND_MAX_GROUP           = 10;                                                                     ///< 最多发出10组GOOSE
const uint32_t GOOSE_SEND_BUF_LEN             = NET_MAX_FRAME_LEN * GOOSE_SEND_MAX_GROUP;                               ///< 长度为10 * 1536
const uint32_t GOOSE_SEND_LOOP_BUF0_POS       = NET_SMV_SEND_LOOP_BUF0_POS + NET_SMV_SEND_BUF_LEN;
const uint32_t GOOSE_SEND_LOOP_BUF1_POS       = NET_SMV_SEND_LOOP_BUF1_POS + NET_SMV_SEND_BUF_LEN;

//
//=============================================================================
// 光串口缓冲区定义
//=============================================================================
//
//光串口接收数据缓冲区定义
const uint32_t FT3_MAX_RECV_FRAMES            = 2640;                                                                   ///< 应用程序能接收的最大报文数
const uint32_t FT3_MAX_FPGA_LEN               = 8192;                                                                   ///< 一个RAM缓冲区大小4096 words
const uint32_t FT3_FPGA_RAM_COUNT             = 24;                                                                     ///< 共享内存缓冲区中保存FPGA缓冲区数目
const uint32_t FT3_RECV_MAP_FILE_LEN          = FT3_MAX_FPGA_LEN * FT3_FPGA_RAM_COUNT + sizeof(BufferHeader);           ///< 光串口接收时的内存映射文件大小
const uint32_t FT3_RECV_LOOP_BUF0_POS         = 0;                                                                      ///< 共享内存分为连个缓冲区，这是第一个缓冲区的位置
const uint32_t FT3_RECV_LOOP_BUF1_POS         = (FT3_RECV_MAP_FILE_LEN - sizeof(BufferHeader))/2;                       ///< 共享内存分为连个缓冲区，这是第二个缓冲区的位置

/// FT3_SEND_MAP_FILE_LEN FT3发送时的内存映射文件大小  256*84*2 = 42KB
/// 最大接收报文长度,映射文件最前端是头信息，便于驱动与应用层的信息传递
const uint32_t FT3_SEND_MAP_FILE_LEN          = 43008;                                                                  ///< FT3发送数据共享内存大小,应用程序与驱动共享的内存大小 256*84*2
const uint32_t FT3_SEND_LOOP_BUF0_POS         = 0;                                                                      ///< 发送共享内存分为连个缓冲区，这是第一个缓冲区的位置
const uint32_t FT3_SEND_LOOP_BUF0_LEN         = (FT3_SEND_MAP_FILE_LEN - sizeof(BufferHeader))/2;                       ///< 第一个缓冲区的长度
const uint32_t FT3_SEND_LOOP_BUF1_POS         = FT3_SEND_LOOP_BUF0_LEN;                                                 ///< 发送共享内存分为连个缓冲区，这是第二个缓冲区的位置
const uint32_t FT3_SEND_LOOP_BUF1_LEN         = FT3_SEND_LOOP_BUF0_LEN;                                                 ///< 第二个缓冲区的长度

//
//=============================================================================
// 接收默认配置参数，包括一次接收多少数据帧，是否启用超时信息的配置
//=============================================================================
//
//网络端口接收默认配置
const uint32_t NET_DETECT_RECV_FRAMES         = 400;
const uint32_t NET_DEFAULT_RECV_FRAMES        = 800;                                                                    ///< 网络接收默认一次接受的数据帧数，最大不超过NET_MAX_RECV_FRAMES/2
const uint32_t NET_MUL_CTRL_BLKS_RECV_FRAMES  = 1600;                                                                   ///< 同时接收多个控制块时网络默认一次接收的数据帧数
const bool     NET_DEFAULT_RECV_ENABLETIMEOUT = true;                                                                   ///< 网络接收默认是否启用超时
const uint32_t NET_DEFAULT_RECV_TIMEOUT       = 0;                                                                      ///< 网络接收默认超时时间，单位ms,超时时间目前由驱动设置，这里设置无用

//光串口接收默认配置
const uint32_t FT3_DEFAULT_RECV_FRAMES        = 800;                                                                    ///< FT3默认一次接受的数据帧数，最大不超过FT3_MAX_RECV_FRAMES/2
const bool     FT3_DEFAULT_RECV_ENABLETIMEOUT = true;                                                                   ///< FT3接收默认是否启用超时
const uint32_t FT3_DEFAULT_RECV_TIMEOUT       = 0;                                                                      ///< FT3接收默认超时时间，单位ms,超时时间目前由驱动设置，这里设置无用



///
/// @brief
///    定义接收数据和驱动交互时Ft3使用的事件名称和文件映射名字
///
const std::wstring      FT3_RECV_NOTIFY_EVENT_NAME_A   = L"DSM_FT3_RX1";
const std::wstring      FT3_RECV_MAP_FILE_NAME_A   = L"ShareFT3_RX1";
const std::wstring      FT3_RECV_NOTIFY_EVENT_NAME_B   = L"DSM_FT3_RX2";
const std::wstring      FT3_RECV_MAP_FILE_NAME_B   = L"ShareFT3_RX2";


///
/// @brief
///    定义接收数据时和驱动交互时以太网使用的事件名称和文件映射名字
///
const std::wstring      NET_RECV_NOTIFY_EVENT_NAME_A    = L"DSM_FIBER_A";
const std::wstring      NET_RECV_MAP_FILE_NAME_A        = L"ShareFIBER_A";
const std::wstring      NET_RECV_NOTIFY_EVENT_NAME_B    = L"DSM_FIBER_B";
const std::wstring      NET_RECV_MAP_FILE_NAME_B        = L"ShareFIBER_B";
const std::wstring      NET_RECV_NOTIFY_EVENT_NAME_C    = L"DSM_FIBER_C";
const std::wstring      NET_RECV_MAP_FILE_NAME_C        = L"ShareFIBER_C";

///
/// @brief
///    定义发送数据时和驱动交互时Ft3使用的事件名称和文件映射名字
///
const std::wstring      FT3_SEND_NOTIFY_EVENT_NAME_C    = L"DSM_FT3_TX1";
const std::wstring      FT3_SEND_MAP_FILE_NAME_C        = L"ShareFT3_TX1";

///
/// @brief
///    定义发送数据和驱动交互时以太网使用的事件名称和文件映射名字
///
const std::wstring      NET_SEND_NOTIFY_EVENT_NAME_A    = L"";
const std::wstring      NET_SEND_MAP_FILE_NAME_A        = NET_RECV_MAP_FILE_NAME_A;
const std::wstring      NET_SEND_NOTIFY_EVENT_NAME_B    = L"";
const std::wstring      NET_SEND_MAP_FILE_NAME_B        = NET_RECV_MAP_FILE_NAME_B;
const std::wstring      NET_SEND_NOTIFY_EVENT_NAME_C    = L"";
const std::wstring      NET_SEND_MAP_FILE_NAME_C        = NET_RECV_MAP_FILE_NAME_C;


///
/// @brief
///    定义配置文件名字
///
#define PARAM_CONFIG_BASIC          L"paramconfig_basic.xml"            ///< 系统配置基本配置文件名
#define PARAM_CONFIG_SMV_SEND       L"paramconfig_smv_send.xml"         ///< SMV发送配置文件名
#define PARAM_CONFIG_GOOSE_SEND     L"paramconfig_goose_send.xml"       ///< GOOSE发送配置文件名
#define PARAM_CONFIG_GOOSE_RECV     L"paramconfig_goose_recv.xml"       ///< GOOSE接收配置文件名
#define PARAM_CONFIG_MANUAL_TEST    L"paramconfig_manual_test.xml"      ///< 手动试验配置文件名
#define PARAM_CONFIG_STATE_SEQUENCE L"paramconfig_state_sequence.xml"   ///< 状态序列配置文件名

#define SMV_RECV_CONFIG_PUBLIC      L"smvrecvconfig.xml"                ///< SMV接收配置

///
/// @brief
///    定义FPGA上各个接口的标识
///    2个光以太网口，3个FT3口,这里的标识程序没有使用，程序中使用的是
///    datatypes.h文件中的 enum DsmPortName，数值和这里保持一致
///
#define DEVICE_NET_A 0                                                  ///< 光以太网口A
#define DEVICE_NET_B 1                                                  ///< 光以太网口B
#define DEVICE_NET_C 2                                                  ///< 电以太网口
#define DEVICE_FT3_A 3                                                  ///< 光串口A，接收数据
#define DEVICE_FT3_B 4                                                  ///< 光串口B，接收数据
#define DEVICE_FT3_C 5                                                  ///< 光串口C，发送数据

///
/// @brief
///    Ft3 DeviceIoControl数据参数结构体
///
struct IOCTRL_IN_PARAM_FT3{
    uint32_t nDeviceItem;                                               ///< 设备号 
    uint32_t nInBuf[4];                                                 ///< 传入DeviceIoControl函数的输入参数，最大长度4

    IOCTRL_IN_PARAM_FT3():nDeviceItem(0)
    {
        memset(nInBuf, 0, sizeof(nInBuf));
    }
};

///
/// @brief
///    网络接口 DeviceIoControl数据参数结构体，传递小数据时使用
///
struct IOCTRL_IN_PARAM_NET{
    uint32_t nDeviceItem;                                               ///< 设备号 
    uint32_t nInBuf[4];                                                 ///< 传入DeviceIoControl函数的输入参数，最大长度12

    IOCTRL_IN_PARAM_NET():nDeviceItem(0)
    {
        memset(nInBuf, 0, sizeof(nInBuf));
    }
};

///
/// @brief
///    网络接口 DeviceIoControl数据参数结构体，传递数据量大时使用，如过滤信息
///
struct IOCTRL_IN_PARAM_NET_BIG{
    uint32_t nDeviceItem;                                               ///< 设备号 
    uint32_t nInBuf[20];                                                ///< 传入DeviceIoControl函数的输入参数，最大长度12

    IOCTRL_IN_PARAM_NET_BIG(): nDeviceItem(0)
    {
        memset(nInBuf, 0, sizeof(nInBuf));
    }
};

///
/// @brief
///    设置1588时钟信息结构体
///
struct IEEE1588_CLOCK_SYNC_INFO{
    uint32_t nMClockId[2];                                              ///< 主时钟ID
    uint32_t nMPortId;                                                  ///< 主时钟端口ID
    uint8_t  nSourceMac[8];                                             ///< 本机端口的源MAC地址
    uint32_t nSClockId[2];                                              ///< 本地时钟ID
    uint32_t nSPortId;                                                  ///< 本地时钟端口ID
};


///
/// @brief
///    用于设置过滤使用的mac,appid结构信息
///
///
struct FpgaNetFilterItem
{
    uint8_t macAddr[ETHERNET_MAC_LEN];
    uint8_t appId[SMV_GOOSE_APPID_LEN];
};

///
/// @brief
///    UTC 时间结构体
///
#pragma pack(push)
#pragma pack (2)                                                        ///< 配置为2Byte对齐，默认是8Byte
typedef struct {
    uint32_t uSecondSinceEpoch;                                         ///< 秒
    uint32_t uFractionOfSecond;                                         ///< 微秒
}UTC_TIME_t;
#pragma pack(pop)                                                       ///< 恢复默认对齐方式

#endif //__CONFIG_H_