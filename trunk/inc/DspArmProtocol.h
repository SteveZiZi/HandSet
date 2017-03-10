/// @file
///
/// @brief
///     定义DSP与ARM交互使用的数据结构等信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    zhangli    2013.3.13

/// 修改
///    chao    2013.3.20   将const常量定义改为宏以更好的兼容DSP
///    chao    2013.3.21   添加多个DSP与ARM之间的交互命令码
///    chao    2013.3.23   添加MAX_SMV_CIRCLE_COUNT
///    chao    2013.3.24   在SMV_DATA_CACL_RT添加了fPhaseSeqAngle成员用以存储
///                           序量相位角
///    chao    2013.3.27   添加诸如FrameHead等多个结构体定义
///    chao    2013.3.29   添加DSP2ARM_CONTROLCODE_SMV_DATA控制码用于标识实际
///                           采样值数据
///    chao    2013.4.2    修改MSGTIME和MsgTimeData的定义
///    chao    2013.4.3    修正SmvData中的fChnannelData为fChannelData书写错误
///    chao    2013.4.9    修改SmvGenData和SmvData定义，去除与具体的电能相关
///                           协议相关的内容，包括协议类型，一次值/二次值，FT3增加
///                           比例因子，比例因子可根据协议和所需要的数据在ARM端
///                           确定，使算法计算与协议解码逻辑上分离，简化了DSP的
///                           职责，降低算法复杂度，提高了运算速度
///    chao    2013.4.11   考虑到报文时间数据会分次传送，所以去除MsgTimeData
///                           结构体定义，并在DSP端缓存报文时间数据，并在Config
///                           结构中添加nMsgTimeCount和nDefaultTimeDif的定义用于
///                           设置报文离散度计算条件
///    chao    2013.5.10   改SMV_DATA_CACL_RT->SMV_DATA_CALC_RT
///                           波形改为多通道，因此SmvGenData结构做相应调整，并增
///                           加SMV_GEN_DATA_CALC_RT的定义用于返回多通道波形生成
///                           数据
///    chao    2013.5.18   修改Config，增加MAX_SMV_CALC_COUNT，
///                           MAX_MSG_TIME_PER_CIRCLE，MAX_MSG_TIME_CALC_COUNT等
///                           常量定义，增加MsgTimeData结构体定义，用于一次性发送
///                           多个报文时间数据
///    chao    2013.7.3    修改Error结构体，增加nCommandID用以标记是哪个命令
///                           的错误，修改Config结构体，增加nSamplingFreq采样率
///                           和nRatedFreq额定频率的参数设置，SMV_DATA_CALC_RT增
///                           加直流含有率fDCTHD的计算结果
///
#ifndef DSP_ARM_PROTOCOL_H_
#define DSP_ARM_PROTOCOL_H_

//包含统一的int类型定义文件
#ifdef _WIN32
    #include "../src/include/stdint.h"
#else
    #include "stdint.h"
#endif

///
/// @brief
///    定义ARM和DSP之间共享内存的大小
///
#define ARM_DSP_BUF_SIZE  (32*1024)

///
/// @brief
///     SV协议的最大采样值通道数
///
#define MAX_SMV_CHANNELS  30

///
/// @brief
///     一个周波内采样点的最大值，256对应采样率为256*50=12800
///
#define MAX_SMV_PER_CIRCLE  256
///
/// @brief
///     最大采样周波数的采样数，用于计算谐波所需的采样数目
///
#define MAX_SMV_CALC_COUNT  (MAX_SMV_PER_CIRCLE*10)
///
/// @brief
///    计算的最高谐波次数
///
#define MAX_HARM_COUNT  50

///
/// @brief
///    一次传输的最大报文时间数
///
#define MAX_MSG_TIME_PER_CIRCLE 468
///
/// @brief
///    计算报文时间的最大次数
///
#define MAX_MSG_TIME_CALC_COUNT (MAX_MSG_TIME_PER_CIRCLE*10)
///
/// @brief
///     波形生成的最大通道数
///
#define MAX_SMV_GEN_CHANNEL_COUNT 20
///
/// @brief
///    波形生成的最大采样点数
///
#define MAX_SMV_GEN_COUNT 256

///
/// @brief
///     ARM对DSP的控制命令（功能码），数据最高位为0
///
#define ARM2DSP_CONTROLCODE_CONFIG       0x01              ///< 对DSP进行参数配置
#define ARM2DSP_CONTROLCODE_SMV_DATA     0x02              ///< 给DSP传送采样值数据
#define ARM2DSP_CONTROLCODE_RESET_SMV    0x03              ///< 重置采样值数据缓存
#define ARM2DSP_CONTROLCODE_RESET_TIME   0x04              ///< 重置报文数据缓存
#define ARM2DSP_CONTROLCODE_SMV_GEN      0x05              ///< 通知DSP产生特定类型波形数据
#define ARM2DSP_CONTROLCODE_TIME_DATA    0x06              ///< 给DSP传送报文时间数据
#define ARM2DSP_CONTROLCODE_INFO         0x10              ///< 得到DSP信息
#define ARM2DSP_CONTROLCODE_STOP_READ    0x7E              ///< 用于DSP回应ARM停止读线程
#define ARM2DSP_CONTROLCODE_QUIT         0x7F              ///< 通知DSP退出

///
/// @brief
///     DSP返回数据给ARM的控制命令（功能码），数据最高位为1
///
#define DSP2ARM_CONTROLCODE_INFO          0x81             ///< DSP返回硬件信息
#define DSP2ARM_CONTROLCODE_SMV_DATA      0x82             ///< DSP返回的实际采样值
#define DSP2ARM_CONTROLCODE_SMV_CALC_RT   0x83             ///< DSP返回计算有效值/谐波等数据
#define DSP2ARM_CONTROLCODE_SMV_GEN_RT    0x84             ///< DSP返回生成波形数据
#define DSP2ARM_CONTROLCODE_MSG_DISP_RT   0x85             ///< DSP返回报文离散度数据
#define DSP2ARM_CONTROLCODE_STOP_READ     0xFE             ///< DSP返回使ARM停止读线程
#define DSP2ARM_CONTROLCODE_ERROR         0xFF             ///< DSP返回执行状态信息

///
/// @brief
///    采样值是电压/电流值
///     xxxx xxxx 一个字节分高4位和低四位，低4位用于标识是电压还是电流数据，
///     高4位用于标识相别，十六进制的ABC分别代表第一个AD的ABC三相，DEF分别代表另一个
///     AD的ABC三相（用于核相计算）
///     如0xA1就代表第一个AD的A相电压，0xA2代表第一个AD的A相电流，其他相别一次类推
///

#define SMV_VCTYPE_VOLTAGE     0x01                        ///< 采样值是电压值
#define SMV_VCTYPE_CURRENT     0x02                        ///< 采样值是电流值
#define SMV_VCTYPE_PHASE_A     0xA0                        ///< 代表AD0的A相
#define SMV_VCTYPE_PHASE_B     0xB0                        ///< 代表AD0的B相
#define SMV_VCTYPE_PHASE_C     0xC0                        ///< 代表AD0的C相
#define SMV_VCTYPE_PHASE_D     0xD0                        ///< 代表AD1的A相
#define SMV_VCTYPE_PHASE_E     0xE0                        ///< 代表AD1的B相
#define SMV_VCTYPE_PHASE_F     0xF0                        ///< 代表AD1的C相

///
/// @brief
///     FrameHead结构定义ARM和DSP之间进行数据交互的数据帧的帧头，数据帧为帧头+
///     数据
///
typedef struct _FrameHead
{
    uint8_t nCommandID;                                             ///< ARM对DSP的控制命令（功能码）
    uint32_t nDataLength;                                           ///< 数据长度
}FrameHead, *PFrameHead;

///
/// @brief
///    DSP配置结构体
///
typedef struct _Config
{
    uint8_t  nASDUCount;                                            ///< ASDU的个数
    uint16_t nMsgTimeCalcCount;                                     ///< 报文时间数，每达到该数量的报文时间数据则计算报文离散度
	uint32_t nSamplingFreq;                                         ///< 采样频率
	uint16_t nRatedFreq;                                            ///< 额定频率
}Config, *PConfig;

///
/// @brief
///    定义所需要产生的波形数据结构体
///
typedef struct
{
    uint8_t nChannelCount;                                                ///< 要生成波形的通道数    
    uint16_t nSmvCount;                                                   ///< 要生成的各通道的采样点数
    uint16_t nSampleRate;                                                 ///< 采样率

    uint8_t nHarmCount[MAX_SMV_GEN_CHANNEL_COUNT];                        ///< 各通道波形的谐波数量
    float   fDCValue[MAX_SMV_GEN_CHANNEL_COUNT];                          ///< 各通道波形的直流分量
    float   fScaleFactor[MAX_SMV_GEN_CHANNEL_COUNT];                      ///< 各通道所需要产生的数据的比例因子，根据所需要的采样值类型设置

    float   fRmsValue[MAX_SMV_GEN_CHANNEL_COUNT][MAX_HARM_COUNT];         ///< 各通道所需要产生波形各次谐波的有效值
    float   fPhaseAngle[MAX_SMV_GEN_CHANNEL_COUNT][MAX_HARM_COUNT];       ///< 各通道所需要产生波形各次谐波的相位
    float   fFreq[MAX_SMV_GEN_CHANNEL_COUNT][MAX_HARM_COUNT];             ///< 各通道所需要产生波形各次谐波的频率
} SmvGenData;

///
/// @brief
///     定义ARM发送到DSP的采样值数据结构
///
typedef struct
{
    uint8_t  nChannelCount;                                         ///< 实际传输的通道数目
    uint16_t nSmvPerCircle;                                         ///< 实际传输的数据一个周波的采样数
    uint8_t  nChannelVCType[MAX_SMV_CHANNELS];                      ///< 每个通道的电压/电流类型
    float    fScaleFactor[MAX_SMV_CHANNELS];                        ///< 每个通道的比例因子
    float    fChannelData[MAX_SMV_CHANNELS][MAX_SMV_PER_CIRCLE];    ///< 一个周波内各个通道的采样数据
} SmvData, *PSmvData;

///
/// @brief
///    DSP信息结构体
///
typedef struct _DspInfo
{
    uint8_t nVersion;                                               ///< DSP版本号
}DspInfo, *PDspInfo;

///
/// @brief
///    错误信息结构体
///
typedef struct _Error
{
    uint8_t nCommandID;                                             ///< 发送的命令码，用以指示发送端的命令类型
    uint16_t nErrno;                                                ///< DSP返回的错误码
}Error, *PError;

///
/// @brief
///    报文时间结构体
///
typedef struct _MSGTIME
{
    uint32_t nSecond;                                               ///< 秒
    uint32_t nMicrosecond;                                          ///< 微秒
}MSGTIME;

///
/// @brief
///    报文时间的数据帧格式
///
typedef struct
{
    uint16_t nMsgTimeCount;                                         ///< 每次传输的报文时间数
    MSGTIME  msgTimes[MAX_MSG_TIME_PER_CIRCLE];                     ///< 每次传输的报文时间数据
}MsgTimeData, *PMsgTimeData;

///
/// @brief
///    计算波形生成返回的数据帧格式
///
typedef struct{
    uint8_t nChannelCount;
    uint16_t nSmvCount;                                             ///< 要生成的各通道的采样点数    
    float fSmvGenData[MAX_SMV_GEN_CHANNEL_COUNT][MAX_SMV_GEN_COUNT];
} SMV_GEN_DATA_CALC_RT;

///
/// @brief
///    计算报文离散度返回的数据帧格式
///
typedef struct{
    float fDispRate[11];
} MSG_DISP_CALC_RT;
///
/// @brief
///     定义计算谐波返回的数据帧格式
///
typedef struct {
    // 通道数据
    uint8_t  nChannelCount;                                         ///< 实际传输的通道数目
    uint8_t  nChannelVCType[MAX_SMV_CHANNELS];                      ///< 每个通道的电压/电流类型
    
    // 频率和有效值
    float    fFreq[MAX_SMV_CHANNELS];                               ///< 每个通道的信号频率
    float    fRmsValue[MAX_SMV_CHANNELS];                           ///< 通道数据有效值
    
    // 谐波
    float    fHarmContent[MAX_SMV_CHANNELS][MAX_HARM_COUNT];        ///< 各通道谐波有效值   xxx.xxV xx.xxxA OK   200ms
    float    fHarmAngle[MAX_SMV_CHANNELS][MAX_HARM_COUNT];          ///< 各通道谐波相位    xxx.x°            OK   200ms
    float    fDCValue[MAX_SMV_CHANNELS];                            ///< DC直流有效值        xxx.xxV xx.xxxAOK      200ms 
    
    float    fDCTHD[MAX_SMV_CHANNELS];                              ///< 各通道直流含有率
    float    fHarmTHD[MAX_SMV_CHANNELS][MAX_HARM_COUNT];            ///< 各通道谐波含有率                OK   200ms
    float    fSumHarmTHD[MAX_SMV_CHANNELS];                         ///< 各通道总谐波含有率                      OK   200ms
    
    // 序量
    float    fPhaseSeqContent[2][3];                                ///< 零序、正序、负序        xxx.xxV xx.xxxA  OK    200ms
    float    fPhaseSeqAngle[2][3];                                  ///< 零序、正序、负序 相位角       xxx.xxV xx.xxxA  OK    200ms
    float    fUnbalanceRate[2];                                     ///< 三相不平衡度
    
    // 功率
    float    fActivePower[3];                                       ///< 有功功率，分别对应ABC三相
    float    fReactivePower[3];                                     ///< 无功功率，分别对应ABC三相
    float    fApparentPower[3];                                     ///< 视在功率，分别对应ABC三相
    float    fPowerFactor[3];                                       ///< 功率因数，分别对应ABC三相
    float    fTotalActivePower;                                     ///< 总功率
    float    fTotalReactivePower;                                   ///< 总无功功率
    float    fTotalApparentPower;                                   ///< 总视在功率
    float    fTotalPowerFactor;                                     ///< 总功率因数
    
    // 核相
    float    fSPAngleDif[3];                                        ///< 同相相角差
    float    fSPVectorDifContent[3];                                ///< 同相向量差大小
    float    fSPVectorDifAngle[3];                                  ///< 同相向量差角度
    float    fIPVectorDifContent[6];                                ///< 相间向量差大小，0-2：SMV1,3-5：SMV2
    float    fIPVectorDifAngle[6];                                  ///< 相间向量差角度，0-2：SMV1,3-5：SMV2
} SMV_DATA_CALC_RT;

#endif // DSP_ARM_PROTOCOL_H_
