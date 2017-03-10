/// @file
///
/// @brief
///     接收数据结构体定义
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.3.25
///
/// 修改历史：
///

#ifndef __FRAME_RECV_STRUCT_H
#define __FRAME_RECV_STRUCT_H

#include "src\include\stdint.h"
#include "src\service\datalayer\include\datatypes.h"
#include "src\service\datalayer\include\config.h"
#include <WinBase.h>
#include "inc\DspArmProtocol.h"
#include "src\utils\MessageCodec\include\ieee1588_types.h"


#pragma pack(push, 1)

/// @brief
///     时间戳的结构体，共8个字节
///
struct FrameTimeStamp
{
    uint32_t ts_sec;             ///< 时间戳的秒数，4个字节
    uint32_t ts_usec;            ///< 时间戳的微秒数,即该时间戳的小数部分，4个字节

    FrameTimeStamp(): ts_sec(0)
                    , ts_usec(0)
    {
    }

    ///
    /// @brief
    ///    给时间戳增加一个微秒时间
    ///
    FrameTimeStamp operator +(uint32_t uSecond)
    {
        uint32_t uSecTotal = this->ts_usec + uSecond;
        FrameTimeStamp stTimeStamp;
        stTimeStamp.ts_sec  = this->ts_sec + uSecTotal/1000000;
        stTimeStamp.ts_usec = uSecTotal%1000000;

        return stTimeStamp;
    }

    ///
    /// @brief
    ///    计算两个时间戳时间差
    ///
    int64_t operator - (const FrameTimeStamp& stTimeStamp) const
    {
        int64_t t1 = int64_t(this->ts_sec) * 1000000 + ts_usec;
        int64_t t2 = int64_t(stTimeStamp.ts_sec) * 1000000 + stTimeStamp.ts_usec;
        return t1 - t2;
    }
};

///
/// @brief
///     缓冲区数据包头结构，共16个字节，按照pcap的结构存放
///
struct BUFFER_PCAP_PACKET_HEADER
{
    FrameTimeStamp  timeStamp;               ///< 从格林尼治时间的1970-1-1 00:00:00到抓包时经过的时间，8个字节
    uint32_t        capLen;                  ///< 表示保存在PCAP文件中的数据包的实际长度，4个字节
    uint32_t        len;                     ///< 表示数据包的真实长度（原始报文长度），4个字节
    ///< len比capLen大则表示当前保存的数据包不完整
};

///
/// @brief
///     数据帧信息结构体
///
struct FrameInfo
{
    uint32_t       nIndex;              ///< 表示数据帧的序号，从0开始
    uint32_t       nOffset;             ///< 数据帧相对内存起始处偏移量
    uint32_t       capLen;              ///< 表示保存在PCAP文件中的数据包的实际长度，4个字节
    FrameTimeStamp timeStamp;           ///< 从格林尼治时间的1970-1-1 00:00:00到抓包时经过的时间，8个字节
};

///
/// @brief
///    定义从FPGA接收数据类型，目前用于Ft3/网络探测时指定探测类型,关系到和驱动交互，不要随便修改值的定义
///
enum NetFt3RecvFrameType{
    NET_RECV_TYPE_SMV = 0,                          ///< 从网络接收采样值数据，包括9-1，9-2
    NET_RECV_TYPE_GOOSE,                            ///< 从网络接收goose数据
    NET_RECV_TYPE_IEEE_1588,                        ///< 从网络接收1588对时数据
    FT3_RECV_TYPE_SMV,                              ///< 从光串口接收Ft3数据
    RECV_TYPE_UNKOWN                                ///< 未知类型
};

//前向声明接收配置类
class CSmvRecvConfig;
///
/// @brief
///    
///
class CBaseDetectItem
{
public:
    uint32_t         nID;                            ///< 控制块标识，后续用于判断控制块是否相同
    enumFrameType    enFrameType;                    ///< 探测到数据帧类型，可能为9-1/9-2/goose
    DsmPortName      enPortName;                     ///< 从什么端口探测到的数据
    uint32_t         nChannelCount;                  ///< 通道数目
    uint32_t         nSampleRate;                    ///< 获取的数据的采样率
    uint32_t         nSampleRateDetect;              ///< 探测/报文读取的采样率
    CSmvRecvConfig*  pRecvConfig;                    ///< 控制块对应的配置信息指针
    uint32_t         nAsdu;                          ///< ASDU数目
    uint32_t         nFrameLen;                      ///< 控制块对应数据帧长度信息，用于估算录波时间长短

    virtual ~CBaseDetectItem()= 0 ;
    CBaseDetectItem();
};
inline CBaseDetectItem::~CBaseDetectItem() {}
inline CBaseDetectItem::CBaseDetectItem() : enFrameType(FRAME_TYPE_UNKOWN)
                                          , enPortName(DSM_PORT_UNKOWN)
                                          , nChannelCount(0)
                                          , pRecvConfig(NULL)
                                          , nSampleRate(0)
                                          , nID(0)
                                          , nAsdu(1)
                                          , nFrameLen(0)
                                          , nSampleRateDetect(0)
{
}


///
/// @brief
///    定义网络探测结果保存结构体，一个结构体用于表示一条探测结果
///
class CNetDetectItem : public CBaseDetectItem
{
public:
    uint8_t         macAddr[ETHERNET_MAC_LEN];      ///< 探测到的Mac地址
    uint16_t        appId;                          ///< 探测到的appid
    CNetDetectItem();
};
inline CNetDetectItem::CNetDetectItem() : appId(0)
{
    memset(macAddr, 0, ETHERNET_MAC_LEN);
}


///
/// @brief
///    定义光串口探测结果结构体
///
struct CFt3DetectItem : public CBaseDetectItem
{
public:
    FT3_BAUD_RATE   enBaudRate;                     ///< 通讯接口的波特率
    uint16_t        nLDName;                        ///< 逻辑设备名字
    CFt3DetectItem();
};
inline CFt3DetectItem::CFt3DetectItem() : enBaudRate(FT3_BAUD_RATE_UNKOWN)
                                        , nLDName(0)
{
}

///
/// @brief
///    定义网络探测1588结果结构体
///
class CNet1588DetectItem : public CNetDetectItem
{
public:
    IEEE1588_SOURCE_PORT_ID     stClockID;                          ///< 探测到的时钟源ID
    uint32_t                    n1588Version;                       ///< 1588版本号
    CNet1588DetectItem();
    std::wstring GetClockIdStr();
    std::wstring GetClockPortIdStr();
};
inline CNet1588DetectItem::CNet1588DetectItem() : n1588Version(0)
{
    memset(&stClockID, 0, sizeof(stClockID));
}

inline std::wstring CNet1588DetectItem::GetClockIdStr()
{
    wchar_t wchTmp[17]  = {0};
    swprintf_s(wchTmp, 17, L"%016I64x", stClockID.clockIdentity);
    return wchTmp;
}

inline std::wstring CNet1588DetectItem::GetClockPortIdStr()
{
    wchar_t wchTmp[5] = {0};
    swprintf_s(wchTmp, 5, L"%04x", stClockID.sourcePortID);
    return wchTmp;
}


///
/// @brief
///    为波形图保存的采样值信息转换后的信息
///
struct SMV_VALUE
{
    CBaseDetectItem* pDetectItem;                            ///< 保存当前控制块信息
    FrameTimeStamp   stFrameTime;                            ///< 采样值时间
    uint32_t         nSmpCnt;                                ///< 表示采样计数器,即1s内采样点序号
    uint32_t         nChannelCount;                          ///< 实际有效的通道数目
    float    fSmvValue[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];    ///< 各个通道采样值

    SMV_VALUE(): pDetectItem(NULL)
               , nSmpCnt(0)
               , nChannelCount(0)
    {
        memset(fSmvValue, 0, sizeof(fSmvValue));
    }
};

///
/// @brief
///    FT3/9-1采样值信息
///
struct SMV_VALUE_FT3_9_1
{
    CBaseDetectItem* pDetectItem;                            ///< 保存当前控制块信息
    FrameTimeStamp   stFrameTime;                            ///< 采样值时间
    uint32_t         nSmpCnt;                                ///< 表示采样计数器,即1s内采样点序号
    uint32_t         nChannelCount;                          ///< 实际有效的通道数目
    int16_t    nSmvValue[FT3_9_1_MAX_CHANNEL_COUNT];         ///< 各个通道采样值
};

///
/// @brief
///    9-2采样值信息
///
struct SMV_VALUE_9_2
{
    CBaseDetectItem* pDetectItem;                            ///< 保存当前控制块信息
    FrameTimeStamp   stFrameTime;                            ///< 采样值时间
    uint32_t         nSmpCnt;                                ///< 表示采样计数器,即1s内采样点序号
    uint32_t         nChannelCount;                          ///< 实际有效的通道数目
    int32_t     nSmvValue[NET_9_2_LE_MAX_CHANNELS_PER_ASDU]; ///< 各个通道采样值
};

//状态字1
#define FT3_STATUS_WORD_NEED_REPAIR           0x0001        ///< 要求维修, 0:良好，1：警告或报警（要求维修）
#define FT3_STATUS_WORD_LLNO_MODE             0x0002        ///< 运行状态标志，0标识正常运行，1：实验（检修） 
#define FT3_STATUS_WORD_WAKEUP_TIME_VALIDITY  0x0004        ///< 唤醒时间标识，0：接通（正常运行），1：唤醒时间，数据无效
#define FT3_STATUS_WORD_MU_SYNC_METHOD        0x0008        ///< 合并单元同步方法 0：不采用插值 ， 1：采用插值
#define FT3_STATUS_WORD_SYNCED_MU             0x0010        ///< 对同步的合并单元 0：同步， 1： 时间同步miss/invalid
///< 对于通道的有效标志，0表示有效，1表示无效
#define FT3_CHANNEL_MASK_CHANNEL_1            0x0020
#define FT3_CHANNEL_MASK_CHANNEL_2            0x0040
#define FT3_CHANNEL_MASK_CHANNEL_3            0x0080
#define FT3_CHANNEL_MASK_CHANNEL_4            0x0100
#define FT3_CHANNEL_MASK_CHANNEL_5            0x0200
#define FT3_CHANNEL_MASK_CHANNEL_6            0x0400
#define FT3_CHANNEL_MASK_CHANNEL_7            0x0800

#define FT3_STATUS_WORD_CT_TYPE               0x1000         ///< 电流互感器输出类型
#define FT3_STATUS_WORD_RANGE_FLAG            0x2000         ///< 0:比例因子 scp = 01cf； 1： 比例因子00e7
#define FT3_STATUS_WORD_FOUTURE_USE1          0x4000
#define FT3_STATUS_WORD_FOUTURE_USE2          0x8000

//状态字2
#define FT3_CHANNEL_MASK_CHANNEL_8            0x0001
#define FT3_CHANNEL_MASK_CHANNEL_9            0x0002
#define FT3_CHANNEL_MASK_CHANNEL_10           0x0004
#define FT3_CHANNEL_MASK_CHANNEL_11           0x0008
#define FT3_CHANNEL_MASK_CHANNEL_12           0x0010
//对于12通道的，以下保留为未来使用，对于国网扩展，表示对应通道
#define FT3_CHANNEL_MASK_CHANNEL_13           0x0020
#define FT3_CHANNEL_MASK_CHANNEL_14           0x0040
#define FT3_CHANNEL_MASK_CHANNEL_15           0x0080
#define FT3_CHANNEL_MASK_CHANNEL_16           0x0100
#define FT3_CHANNEL_MASK_CHANNEL_17           0x0200
#define FT3_CHANNEL_MASK_CHANNEL_18           0x0400
#define FT3_CHANNEL_MASK_CHANNEL_19           0x0800
#define FT3_CHANNEL_MASK_CHANNEL_20           0x1000
#define FT3_CHANNEL_MASK_CHANNEL_21           0x2000
#define FT3_CHANNEL_MASK_CHANNEL_22           0x4000
#define FT3_STATUS_WORD_FOUTURE_USE3          0x8000

//对应通道索引对应的mask，索引从0开始
//从第8个通道（索引为7）开始，mask要使用状态字2进行比较
static uint16_t FT3_STATUS_WORD_MASK[]={
                FT3_CHANNEL_MASK_CHANNEL_1,
                FT3_CHANNEL_MASK_CHANNEL_2,
                FT3_CHANNEL_MASK_CHANNEL_3,
                FT3_CHANNEL_MASK_CHANNEL_4,
                FT3_CHANNEL_MASK_CHANNEL_5,
                FT3_CHANNEL_MASK_CHANNEL_6,
                FT3_CHANNEL_MASK_CHANNEL_7,
                FT3_CHANNEL_MASK_CHANNEL_8, 
                FT3_CHANNEL_MASK_CHANNEL_9, 
                FT3_CHANNEL_MASK_CHANNEL_10,
                FT3_CHANNEL_MASK_CHANNEL_11,
                FT3_CHANNEL_MASK_CHANNEL_12,
                FT3_CHANNEL_MASK_CHANNEL_13,
                FT3_CHANNEL_MASK_CHANNEL_14,
                FT3_CHANNEL_MASK_CHANNEL_15,
                FT3_CHANNEL_MASK_CHANNEL_16,
                FT3_CHANNEL_MASK_CHANNEL_17,
                FT3_CHANNEL_MASK_CHANNEL_18,
                FT3_CHANNEL_MASK_CHANNEL_19,
                FT3_CHANNEL_MASK_CHANNEL_20,
                FT3_CHANNEL_MASK_CHANNEL_21,
                FT3_CHANNEL_MASK_CHANNEL_22 };

#define SMV_9_2_VALID_MASK      0x00000003      ///< 9-2采样值对应的有效标志位，最低2位（00表示validity:good）
#define SMV_9_2_TEST_MASK       0x00000800      ///< 9-2采样值对应的检修标志，此位上0代表false（没有检修）


///
/// @brief
///    保存最新的控制块通道有效/品质信息
///
class CSmvCtrlBlkQuality
{
private:
    CBaseDetectItem*    m_pDetectItem;                       ///< 对应的控制块信息
    uint32_t   m_nQuality[NET_9_2_LE_MAX_CHANNELS_PER_ASDU]; ///< 品质信息
    uint16_t            m_nStatusWord1;                      ///< 状态字1，针对Ft3/9-1
    uint16_t            m_nStatusWord2;                      ///< 状态字2，针对Ft3/9-1
    bool                m_bIsMuSync;                         ///< MU是否同步


public:
    CSmvCtrlBlkQuality(): m_pDetectItem(NULL)
                        , m_nStatusWord1(0)
                        , m_nStatusWord2(0)
                        , m_bIsMuSync(false)
    {
        memset(m_nQuality, 0, sizeof(m_nQuality));
    }

private:
    ///
    /// @brief
    ///    重置所有状态信息，在切换控制块时使用
    ///
    void _ResetStatus()
    {
        memset(m_nQuality, 0, sizeof(m_nQuality));
        m_nStatusWord1 = 0;
        m_nStatusWord2 = 0;
        m_bIsMuSync = false;
    }

public:
    ///
    /// @brief
    ///    设置控制块信息
    ///
    void SetCtrlBlk(CBaseDetectItem* pDetectItem)
    {
        _ResetStatus();
        m_pDetectItem = pDetectItem;
    }

    ///
    /// @brief
    ///    获取控制块信息
    ///
    CBaseDetectItem* GetCtrlBlk()
    {
        return m_pDetectItem;
    }

    ///
    /// @brief
    ///    设置Mu是否同步
    ///
    /// @return
    ///     bool 同步返回true
    bool IsMuSync()
    {
        if (m_pDetectItem)
        {
            switch(GetFrameType())
            {
            case FRAME_TYPE_NET_9_1:
            case FRAME_TYPE_FT3_NANRUI:
            case FRAME_TYPE_FT3_GUOWANG:
                return !(m_nStatusWord1 & FT3_STATUS_WORD_SYNCED_MU);
            case FRAME_TYPE_NET_9_2:
                return m_bIsMuSync;
            default:
                return false;
            }
        }
        return false;
    }

    ///
    /// @brief
    ///    设置mu是否同步
    ///
    void SetMuSync(bool bSync)
    {
        m_bIsMuSync = bSync;
    }

    ///
    /// @brief
    ///    获取通道数量信息
    ///
    size_t GetChannelCount()
    {
        if (m_pDetectItem)
        {
            return m_pDetectItem->nChannelCount;
        }
        return 0;
    }

    ///
    /// @brief
    ///     h获取数据帧类型
    ///
    enumFrameType GetFrameType()
    {
        if (m_pDetectItem)
        {
            return m_pDetectItem->enFrameType;
        }
        return FRAME_TYPE_UNKOWN;
    }

    ///
    /// @brief
    ///    获取指定通道的有效信息
    ///
    /// @return
    ///     有效返回true
    ///
    bool IsChannelValid(uint32_t nChannelIndex)
    {
        if (m_pDetectItem)
        {
            switch(GetFrameType())
            {
            //通道数12
            case FRAME_TYPE_NET_9_1:
            case FRAME_TYPE_FT3_NANRUI:
                {
                    // 通道索引0-6，7个通道有效标志位于状态字1
                    if (nChannelIndex < 7 )
                    {
                        return !(m_nStatusWord1 & FT3_STATUS_WORD_MASK[nChannelIndex]);
                    }
                    else if (nChannelIndex < 12)
                    {
                        return !(m_nStatusWord2 & FT3_STATUS_WORD_MASK[nChannelIndex]);
                    }
                    else
                    {
                        return false;
                    }
                }
                //通道数22
            case FRAME_TYPE_FT3_GUOWANG:
                {
                    if (nChannelIndex < 7 )
                    {
                        return !(m_nStatusWord1 & FT3_STATUS_WORD_MASK[nChannelIndex]);
                    }
                    else if (nChannelIndex < 22)
                    {
                        return !(m_nStatusWord2 & FT3_STATUS_WORD_MASK[nChannelIndex]);
                    }
                    else
                    {
                        return false;
                    }
                    break;
                }
                //判断对应的品质因数的位
            case FRAME_TYPE_NET_9_2:
                if (nChannelIndex < min(m_pDetectItem->nChannelCount, NET_9_2_LE_MAX_CHANNELS_PER_ASDU))
                {
                    return !(m_nQuality[nChannelIndex] & SMV_9_2_VALID_MASK);
                }
                break;
            default:
                break;
            }
        }
        return false;
    }

    ///
    /// @brief
    ///    获取通道的品质因数,针对9-2
    ///
    bool GetChannelQuality(uint32_t nChannelIndex, uint32_t& nQuality)
    {
        if (m_pDetectItem && nChannelIndex < min(NET_9_2_LE_MAX_CHANNELS_PER_ASDU, m_pDetectItem->nChannelCount) && GetFrameType() == FRAME_TYPE_NET_9_2)
        {
            nQuality = m_nQuality[nChannelIndex];
            return true;
        }
        return false;
    }

    ///
    /// @brief
    ///    设置通道品质因数,针对9-2
    ///
    bool SetChannelQuality(uint32_t nChannelIndex, uint32_t nQuality)
    {
        if (m_pDetectItem && nChannelIndex < min(NET_9_2_LE_MAX_CHANNELS_PER_ASDU, m_pDetectItem->nChannelCount) && GetFrameType() == FRAME_TYPE_NET_9_2)
        {
            m_nQuality[nChannelIndex] = nQuality;
            return true;
        }
        return false;
    }

    ///
    /// @brief
    ///    设置控制块状态字，针对ft3/9-1
    ///
    bool SetCtrlBlkStatusWords(uint16_t nStatusWord1, uint16_t nStatusWord2)
    {
        if (m_pDetectItem)
        {
            switch(GetFrameType())
            {
            case FRAME_TYPE_NET_9_1:
            case FRAME_TYPE_FT3_GUOWANG:
            case FRAME_TYPE_FT3_NANRUI:
                m_nStatusWord1 = nStatusWord1;
                m_nStatusWord2 = nStatusWord2;
                return true;
                break;
            case FRAME_TYPE_NET_9_2:
                break;
            default:
                break;
            }
        }
        return false;
    }

    ///
    /// @brief
    ///    获取控制块状态字，针对ft3/9-1
    ///
    bool GetCtrlBlkStatusWords(uint16_t& nStatusWord1, uint16_t& nStatusWord2)
    {
        if (m_pDetectItem)
        {
            switch(GetFrameType())
            {
            case FRAME_TYPE_NET_9_1:
            case FRAME_TYPE_FT3_GUOWANG:
            case FRAME_TYPE_FT3_NANRUI:
                nStatusWord1 = m_nStatusWord1 ;
                nStatusWord2 = m_nStatusWord2 ;
                return true;
                break;
            case FRAME_TYPE_NET_9_2:
                break;
            default:
                break;
            }
        }
        return false;
    }

    ///
    /// @brief
    ///    获取9-1/ft3的rangflag标识
    ///
    /// @return
    ///     返回0表示rangeflag为0，返回1表示rangefalg为1，返回其他值表示获取失败
    ///
    uint32_t GetRangeFlag()
    {
        enumFrameType enFrameType = GetFrameType();
        if (enFrameType == FRAME_TYPE_NET_9_1
            || enFrameType == FRAME_TYPE_FT3_GUOWANG
            || enFrameType == FRAME_TYPE_FT3_NANRUI)
        {
            if (m_nStatusWord1 & FT3_STATUS_WORD_RANGE_FLAG)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        return 3;   //表示获取失败
    }

    ///
    /// @brief
    ///    获取是否有检修标志
    ///
    bool IsChannelTest(uint32_t nChannelIndex = 0)
    {
        //默认为没有检修标志
        bool bRet = false;
        if (m_pDetectItem)
        {
            switch(m_pDetectItem->enFrameType)
            {
            case FRAME_TYPE_FT3_NANRUI:
            case FRAME_TYPE_FT3_GUOWANG:
            case FRAME_TYPE_NET_9_1:
                bRet = ((m_nStatusWord1 & FT3_STATUS_WORD_LLNO_MODE) == FT3_STATUS_WORD_LLNO_MODE);
                break;
            case FRAME_TYPE_NET_9_2:
                {
                    if (nChannelIndex < m_pDetectItem->nChannelCount && nChannelIndex < NET_9_2_LE_MAX_CHANNELS_PER_ASDU)
                    {
                        bRet = ((m_nQuality[nChannelIndex] & SMV_9_2_TEST_MASK) == SMV_9_2_TEST_MASK);
                    }
                }
                break;
            }
        }
        //默认为没有检修标志
        return bRet;
    }
};

class CDetectItemFrame
{
private:
    CBaseDetectItem* m_pDetectItem;                                     ///< 当前数据帧对应的控制块
    uint32_t         m_nFrameLen;                                       ///< 当前数据帧长度
    uint8_t*         m_pFrameData;                                      ///< 数据帧内容

public:
    CDetectItemFrame(CBaseDetectItem* pDetectItem): m_pDetectItem(pDetectItem)
                                                  , m_nFrameLen(0)
                                                  , m_pFrameData(NULL)
    {
    }

    ///
    /// @brief
    ///    填充数据帧数据
    ///
    bool copyFrameData(uint8_t* pData, uint32_t nDataLen)
    {
        if (m_pFrameData)
        {
            delete[] m_pFrameData;
            m_pFrameData = NULL;
        }

        m_pFrameData = new uint8_t[nDataLen];
        m_nFrameLen = nDataLen;
        memcpy_s(m_pFrameData, m_nFrameLen, pData, nDataLen);
        return true;
    }

    ///
    /// @brief
    ///    获取数据帧长度
    ///
    uint32_t GetFrameLen()
    {
        return m_nFrameLen;
    }

    ///
    /// @brief
    ///    获取对应的控制块指针
    ///
    CBaseDetectItem* GetDetectItem()
    {
        return m_pDetectItem;
    }

    ///
    /// @brief
    ///    获取缓冲区数据首地址
    ///
    uint8_t* GetFrameData()
    {
        return m_pFrameData;
    }

    ~CDetectItemFrame()
    {
        if (m_pFrameData)
        {
            delete[] m_pFrameData;
            m_pFrameData = NULL;
        }
    }
};

#pragma pack(pop)

#endif //__FRAME_RECV_STRUCT_H