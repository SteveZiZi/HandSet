/// @file
///
/// @brief
///     对加载的pcap文件进行分析/统计
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.10.31
///
/// 修改历史：
///

#ifndef _PCAP_FILE_ANALYSIS_H
#define _PCAP_FILE_ANALYSIS_H

#include <string>
#include <map>
#include "src\include\pub_datatypes.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\recv_data\DataGooseFrameStatistics.h"
#include "src\service\datalayer\recv_data\DataFrameStatistics.h"
#include "src\service\datalayer\recv_data\DataOscillogram.h"

#include "src\utils\MessageCodec\include\guowang_ft3_msg.h"
#include "src\utils\MessageCodec\include\nanrui_ft3_msg.h"
#include "src\utils\MessageCodec\include\sav91_msg.h"
#include "src\utils\MessageCodec\include\sav92_msg.h"
#include "src\utils\MessageCodec\include\goose_msg.h"
#include "src\utils\MessageCodec\include\msg_rapid_decoder.h"

#include "src\utils\PcapFile\include\pcap_file_i.h"

#include "msgdispersioncalc.h"

#include "src\service\datalayer\recv\FrameRecvStruct.h"

#include "src\service\datalayer\recv_data\DataOscillogram.h"

#include "src\utils\MessageCodec\include\iec61850_data_struct.h"

//定义读取pcap文件使用内存缓冲区大小
#define PCAP_FILE_READ_BUFFER_SIZE  (1024*1024*5)

//定义本模块是否打印调试信息,1打印，0不打印
#define PCAP_FILE_ANALYSIS_OUTPUT_LOG   1

///
/// @brief
///    定义文件分析完毕通知界面的消息id
///
#define WM_PCAPFILE_ANALYSIS_MESSAGE        (PRIVATE_WND_USERID + 51)                   ///< 数据层发送给界面的消息ID
#define MSG_PCAPFILE_STATUS_ANALYSIS_DONE   (1)                                         ///< 报文异常状态信息统计完毕
#define MSG_PCAPFILE_WAVE_ANALYSIS_DONE     (2)                                         ///< 波形数据分析完毕
#define MSG_PCAPFILE_OPEN_FAILED            (3)                                         ///< 打开pcap文件失败
#define MSG_PCAPFILE_ANALYSIS_USER_CACEL    (4)                                         ///< 用户取消
#define MSG_PCAPFILE_ANALYSIS_FAILED        (5)                                         ///< 分析文件失败

///
/// @brief
///    文件当前状态信息
///
enum enumPcapFileAnalysisStatus
{
    PCAP_FILE_NOT_ANALYSISED = 0,            ///< 文件未被分析
    PCAP_FILE_ANALYSIS_ING,                  ///< 文件正在分析中
    PCAP_FILE_ANALYSIS_USER_CANCEL,          ///< 文件分析被用户取消
    PCAP_FILE_ANALYSIS_DONE,                 ///< 文件分析完毕
    PCAP_FILE_ANALYSIS_CALC_WAVE,            ///< 正在处理波形数据
    PCAP_FILE_ANALYSIS_CALC_WAVE_DONE        ///< 波形数据处理完毕
};

///
/// @brief
///    描述一个采样点在Pcap文件中的位置信息
///
struct SampleIndexPosition
{
    uint32_t nAppidSampleIndex;             ///< 采样点序号，从0开始递增
    uint32_t nAppidFrameIndex;              ///< 当前采样点所在的数据帧是当前appid的第几帧，从0开始
    uint32_t nPcapFrameIndex;               ///< 当前采样点所在的数据帧在整个pcap文件中是第几帧，从0开始
    uint32_t nAsduCount;                    ///< 当前采样点所在的数据帧有几个ASDU
    uint32_t nAsduIndex;                    ///< 当前采样点是所在数据帧中的几个ASDU


    SampleIndexPosition(): nAppidSampleIndex(0)
                         , nAppidFrameIndex(0)
                         , nPcapFrameIndex(0)
                         , nAsduCount(1)
                         , nAsduIndex(0)
    {
    }
};

///
/// @brief
///    定义一个控制块的信息
///
struct AppidInfo
{
    int32_t         nAppid;
    enumFrameType   enFrameType;                      ///< 控制块数据帧类型
    uint32_t        nSmpRate;                         ///< 如果是采样值，表示的采样率，goose数据此项无意义
    uint32_t        nAsdu;                            ///< 控制块一个数据帧中asdu数目，对9-1/9-2有意义
    uint32_t        nChannelCount;                    ///< 控制块通道数量

    uint32_t        nAppidSmpCount;                   ///< 此控制块总采样点个数
    FrameTimeStamp  stFirstTimeStamp;                 ///< 此控制块第一帧时间戳
    FrameTimeStamp  stLastTimeStamp;                  ///< 此控制块最后一帧时间戳

    AppidInfo(): enFrameType(FRAME_TYPE_UNKOWN)
               , nSmpRate(0)
               , nAsdu(0)
               , nAppid(0)
               , nAppidSmpCount(0)
               , nChannelCount(0)
    {
    }
};

struct FrameIndexInfo
{
    uint32_t        nPcapFileFrameIndex;                                  ///< 帧在pcap文件中的索引
    uint32_t        nFramelen;                                            ///< 帧长度
    FrameTimeStamp  stTimeStamp;                                          ///< 帧的时间戳信息
};

typedef std::map<int32_t, CMsgDisperisonCalc*>   MapAppidMsgDispersion;   ///< 某个控制块的离散度信息
typedef std::map<uint32_t, SMV_VALUE>            MapSampleIndexSmvData;   ///< 某个采样点对应的通道数据信息
typedef std::map<int32_t, AppidInfo>             MapAppidInfo;            ///< 某个控制块的采样率信息
typedef std::vector<SampleIndexPosition>         VecAppidSampleMapping;   ///< 定义一个appid采样点在pcap的位置信息
typedef std::vector<FrameIndexInfo>  VecAppidFrameIndex2PcapFrameIndex;   ///< 定义一个appid对应帧在整个pcap中帧的索引


///
/// @brief
///    主要统计控制块的采样率信息
///
class CAppidInfoAnalysis
{
public:
    CAppidInfoAnalysis(){};
    ~CAppidInfoAnalysis()
    {
        m_mapAppidInfoCalcInfo.clear();
    };

public:
    ///
    /// @brief
    ///    处理每一帧的信息
    ///
    void ParseAppidInfo(FrameTimeStamp stFrameTime, int32_t nAppid, enumFrameType enFrameType, uint32_t nAsdu, uint32_t nChannelCount, uint8_t* pBuff, uint32_t nBufLen);

    ///
    /// @brief
    ///    所有帧处理完毕，计算采样率信息
    ///
    void CalcSmpRate();

    ///
    /// @brief
    ///    清空所有数据
    ///
    void ResetData()
    {
        m_mapAppidInfoCalcInfo.clear();
    }

    ///
    /// @brief
    ///    获取某个appid的采样率
    ///
    uint32_t GetAppidSmpRate(int32_t nAppid);

    ///
    /// @brief
    ///    获取某个控制块的asdu数量
    ///
    uint32_t GetAppidNoAsdu(int32_t nAppid);

    ///
    /// @brief
    ///    获取某个控制块的数据帧类型
    ///
    enumFrameType GetAppidFrameType(int32_t nAppid);

    ///
    /// @brief
    ///    获取某个控制块的通道数目
    ///
    uint32_t GetAppidChannelCount(int32_t nAppid);
public:
    MapAppidInfo        m_mapAppidInfoCalcInfo;
};


///
/// @brief
///    处理一个控制块中采样点在实际报文数据位置信息
///
class CAppidSampleMapping
{
public:
    CAppidSampleMapping();
    ~CAppidSampleMapping();

public:
    ///
    /// @brief
    ///    处理一个采样点信息
    ///
    void ParseSampleInfo(SampleIndexPosition stSamplePosition, const FrameTimeStamp& stTimeStamp, uint32_t nFrameLen);

    ///
    /// @brief
    ///    处理一个采样点信息
    ///
    void ResetData();


public:
    VecAppidSampleMapping                    m_vecAppidSampleMap;            ///< 保存一个控制块采样点位置的所有信息
    VecAppidFrameIndex2PcapFrameIndex        m_vecAppidFrameIndexMap;        ///< 保存一个控制块数据帧位置的信息
};

///
/// @brief
///    定义smv一个控制块（appid）对应采样点位置信息，
///    主要应对一个pcap可能出现多个控制块的情况
///
typedef std::map<int32_t, CAppidSampleMapping>     MapAppidSampleMapping;

///
/// @brief
///    过零点采样点信息
///
struct ZeroCrossPoint
{
    int32_t  nIndex;                                                        ///< 采样点索引
    float    fPhase;                                                        ///< 采样点的相位，过零点相位为0或者180
    float    fSmpValue;                                                     ///< 过零点的采样值
    bool     bPreciseZero;                                                  ///< 是否是精确的0值，误差范围0.000001

    ZeroCrossPoint(): nIndex(-1)
                    , fPhase(0.0)
                    , fSmpValue(0.0)
                    , bPreciseZero(false)
    {
    }
};

///
/// @brief
///    定义一个帧异常所在的位
///
#define FRAME_STATUS_SMP_LOST               (1<<0)                          ///< SMV采样点丢失
#define FRAME_STATUS_SMP_MIS_ORDER          (1<<1)                          ///< SMV错序
#define FRAME_STATUS_SMP_RETRY              (1<<2)                          ///< SMV重发
#define FRAME_STATUS_SMP_LOST_SYNC          (1<<3)                          ///< SMV失去同步
#define FRAME_STATUS_SMP_Q_INVALID          (1<<4)                          ///< SMV品质无效
#define FRAME_STATUS_SMP_TEST               (1<<5)                          ///< SMV检修
#define FRAME_STATUS_GOOSE_SQ_LOST          (1<<6)                          ///< GOOSE sq丢失
#define FRAME_STATUS_GOOSE_SQ_MIS_ORDER     (1<<7)                          ///< GOOSE sq错序
#define FRAME_STATUS_GOOSE_SQ_RETRY         (1<<8)                          ///< GOOSE sq重发
#define FRAME_STATUS_GOOSE_ST_LOST          (1<<9)                          ///< GOOSE st丢失
#define FRAME_STATUS_GOOSE_ST_MIS_ORDER     (1<<10)                         ///< GOOSE st错序
#define FRAME_STATUS_GOOSE_FRAME_LOST       (1<<11)                         ///< GOOSE 报文丢失（帧报文的设计间隔超过 TimeAllowedToLive=T0 *2）
#define FRAME_STATUS_GOOSE_COMM_BREAK       (1<<12)                         ///< GOOSE 通信中断（帧报文的设计间隔超过 TimeAllowedToLive * 2 ）
#define FRAME_STATUS_GOOSE_TTL_INVALID      (1<<13)                         ///< GOOSE TTL无效
#define FRAME_STATUS_GOOSE_CLK_NOT_SYNC     (1<<14)                         ///< GOOSE 时钟未同步
#define FRAME_STATUS_GOOSE_CLK_FAILED       (1<<15)                         ///< GOOSE 时钟故障

///
/// @brief
///    定义一个帧的异常信息
///
class FrameStatus
{
public:
    uint16_t                    nData;                         ///< 记录一个帧异常信息，每一位1代表异常，0表示正常
    std::vector<uint32_t>       vecSmpCnt;                     ///< 记录一个帧里面包含的采样序号信息

public:
    FrameStatus(): nData(0) {}
    FrameStatus(uint16_t nVal): nData(nVal) {}

    ~FrameStatus()
    {
        std::vector<uint32_t>().swap(vecSmpCnt);
        vecSmpCnt.clear();
    }

public:
    ///
    /// @brief
    ///    设置某一位数据
    ///
    /// @param[in]  nFrameStatusMask    定义的异常位的值
    /// @param[in]  bSetTrue            是否置对应位为1
    ///
    /// @note
    ///     SetValue(FRAME_STATUS_SMP_LOST) 表示设置smv丢采样点异常
    ///
    void SetValue(uint16_t nFrameStatusMask, bool bSetTrue = true)
    {
        if (bSetTrue)
        {
            nData |= nFrameStatusMask;
        }
        else
        {
            nData &= ~nFrameStatusMask;
        }
    }

    ///
    /// @brief
    ///    获取异常字信息
    ///
    uint16_t GetValue()
    {
        return nData;
    }

    ///
    /// @brief
    ///    设置异常字信息
    ///
    void SetData(uint16_t nVal)
    {
        nData = nVal;
    }
};

///
/// @brief
///    pcap文件goose数据帧异常分析
///
class CPcapFileGooseFrameStatus
{
public:
    CPcapFileGooseFrameStatus() : m_nSqNum(-1)
                                , m_nStNum(-1)
    {
        memset(&m_stGooseFrameStatistics, 0, sizeof(m_stGooseFrameStatistics));
    }

    ///
    /// @brief
    ///    处理一帧报文，返回报文该帧状态信息
    ///
    FrameStatus ParseGooseFrame(uint32_t nTTL, FrameTimeStamp& stFrameTimeStamp,
                                uint8_t nGooseUtcTimeQuality,
                                int32_t nStNum, int32_t nSqNum);

    friend class CPcapAnalysis;
private:
    int32_t                             m_nStNum;                       ///< 记录goose控制块上一帧的stNumber，每次数据帧更新此数据
    int32_t                             m_nSqNum;                       ///< 记录goose控制块上一帧的sqNumber，每次数据帧更新此数据
    FrameTimeStamp                      m_stLastFrameTimeStamp;         ///< 上一数据帧的接收时间，用于判断超时/断线
    GooseFrameStatistics                m_stGooseFrameStatistics;       ///< 数据统计信息
};

///
/// @brief
///    pcap文件smv采样值状态分析统计
///
class CPcapFileSmvSmpStatus
{
public:
    CPcapFileSmvSmpStatus(): m_nLastSampleCountIndex(-1)
                           , m_nLastPcapFileFrameIndex(-1)
    {
        memset(&m_stSmvFrameStatistics, 0, sizeof(m_stSmvFrameStatistics));
    }

    ///
    /// @brief
    ///    分析处理一个采样点信息
    ///
    FrameStatus ParseSamplePoint(int32_t nPcapFileFrameIndex, CBaseDetectItem* pDetectItem,
                                 int32_t nSmpIndex, CSmvCtrlBlkQuality& smvCtrlBlkQuality);

    ///
    /// @brief
    ///    分析处理一个帧时间戳信息,判断统计中的抖动次数
    ///
    void ParseFrameTimeStamp(FrameTimeStamp& stFrameTimeStamp, uint32_t nAsdu = 1, uint32_t nSmpRate = 4000);

    friend class CPcapAnalysis;
private:
    ///
    /// @brief
    ///    计算两个时间戳时间差，判断时间是否抖动（时间戳间隔偏离正常间隔超过10us）
    ///
    bool _IsTimeStampJitter(const FrameTimeStamp& tm1, const FrameTimeStamp& tm2, uint32_t nAsdu = 1, uint32_t nSmpRate = 4000);

private:
    int32_t             m_nLastPcapFileFrameIndex;                          ///< 处理pcap文件的数据帧序号，判断两个相邻采样值是否位于同一数据帧
    int32_t             m_nLastSampleCountIndex;                            ///< 上次数据帧的采样计数器
    FrameTimeStamp      m_stLastFrameTimeStamp;                             ///< 上一数据帧的时间戳，统计抖动信息
    SmvFrameStatistics  m_stSmvFrameStatistics;                             ///< 控制块对应的统计信息
};


typedef std::map<int32_t, CPcapFileGooseFrameStatus>    MapAppidGooseStatistics; ///< 某个goose控制块对应的异常统计信息
typedef std::map<int32_t, CPcapFileSmvSmpStatus>        MapAppidSmvStatistics;   ///< 某个采样值控制块对应的统计信息
typedef std::map<uint32_t, FrameStatus>             MapPcapFileFrameIndexStatus; ///< pcap文件中没帧对应的异常信息

///
/// @brief
///    pcap文件分析类
///
class CPcapAnalysis
{
private:
    CPcapAnalysis() : m_strPcapFile(L"")
                    , m_pPcapFile(NULL)
                    , m_pGuoWangFT3Msg(NULL)
                    , m_pNanRuiFt3Msg(NULL)
                    , m_pSav91Msg(NULL)
                    , m_pSav92Msg(NULL)
                    , m_pGooseMsg(NULL)
                    , m_pFrameBuffer(NULL)
                    , m_enAnalysisStatus(PCAP_FILE_NOT_ANALYSISED)
                    , m_enAnalysisWaveStatus(PCAP_FILE_NOT_ANALYSISED)
                    , m_pNotifyWnd(NULL)
                    , m_nCurrentAppid(-1)
                    , m_nWaveIndexBegin(0)
                    , m_nWaveIndexEnd(0)
    {
    }

    ~CPcapAnalysis()
    {
        _ResetData();
        if (m_pPcapFile)
        {
            m_pPcapFile->Close();
            ReleasePcapFileImpl(m_pPcapFile);
            m_pPcapFile = NULL;
        }
        if (m_pGuoWangFT3Msg)
        {
            ReleaseGuoWangFT3MsgImpl(m_pGuoWangFT3Msg);
            m_pGuoWangFT3Msg = NULL;
        }

        if (m_pNanRuiFt3Msg)
        {
            ReleaseNanRuiFT3MsgImpl(m_pNanRuiFt3Msg);
            m_pNanRuiFt3Msg = NULL;
        }
        if (m_pSav91Msg)
        {
            ReleaseSav91MsgImpl(m_pSav91Msg);
            m_pSav91Msg = NULL;
        }
        if (m_pSav92Msg)
        {
            ReleaseSav92MsgImpl(m_pSav92Msg);
            m_pSav92Msg = NULL;
        }
        if (m_pGooseMsg)
        {
            ReleaseGooseMsgImpl(m_pGooseMsg);
            m_pGooseMsg = NULL;
        }
        if (m_pFrameBuffer)
        {
            delete[] m_pFrameBuffer;
            m_pFrameBuffer = NULL;
        }
    }


public:
    ///
    /// @brief
    ///    获取类实例接口
    ///
    static CPcapAnalysis* getInstance()
    {
        static CPcapAnalysis s_instance;
        return &s_instance;
    }

    ///
    /// @brief
    ///    有效值计算
    /// @param[in] 
    ///    pSampleValues    采样点值
    ///    sampleNum        采样点数
    ///
    /// @return
    ///    float           有效值
    ///
    static float effValueCalc(const float pSampleValues[], uint32_t sampleNum);

    ///
    /// @brief
    ///    滤波
    /// @param[in] 
    ///    ord                  滤波器阶数,暂固定为4
    ///    a                    滤波器参数a_w
    ///    b                    滤波器参数b_w
    ///    np                   采样点数
    ///    x                    输入
    ///    leftxlen             y左边元素个数，用于增量计算，暂指定为0
    /// @param[out]
    ///    y                    滤波后的输出
    ///
    static void filterx(int ord, const float *a, const float *b, int np,
        const float *x, float *y, int leftxlen );

public:
///
///=============================================================================
/// 通用设置，获取基本信息
///=============================================================================
///
    ///
    /// @brief
    ///    设置要分析的pcap文件完整路径
    ///
    void SetPcapFilePath(const std::wstring& strPcapFile)
    {
        if (m_strPcapFile.compare(strPcapFile) != 0)
        {
            _ClosePcapFile();
            m_strPcapFile = strPcapFile;
        }
    }

    ///
    /// @brief
    ///    设置通知界面时使用的窗口Cwnd指针
    ///
    bool SetPcapFileNotifyWnd(CWnd* pWnd)
    {
        m_pNotifyWnd = pWnd;
        return true;
    }

    ///
    /// @brief
    ///    获取界面设置的通知窗口指针
    ///
    CWnd* GetPcapFileNotifyWnd()
    {
        return m_pNotifyWnd;
    }

    ///
    /// @brief
    ///    获取pcap文件中控制块（appid）数目
    ///
    uint32_t GetPcapFileAppidCount();

    ///
    /// @brief
    ///    获取一个控制块Appid
    ///
    /// @return
    ///     获取对应索引的appid，-1表示获取失败
    ///
    int32_t GetPcapFileOneAppid(uint32_t nIndex);

    ///
    /// @brief
    ///    设置当前的控制块
    ///
    void SetCurrentAppid(int32_t nAppid)
    {
        if (nAppid != m_nCurrentAppid)
        {
            m_nCurrentAppid = nAppid;
            _ClearCurrenAppidData();
        }
    }

    ///
    /// @brief
    ///    获取当前的控制块appid
    ///
    int32_t GetCurrentAppid()
    {
        return m_nCurrentAppid;
    }

    ///
    /// @brief
    ///    获取一个appid采样值对应采样值/帧在pcap中的映射信息
    ///
    const CAppidSampleMapping* GetPcapFileAppidSampleMapping(int32_t nAppid);

    ///
    /// @brief
    ///    获取当前控制块的数据帧类型
    ///
    enumFrameType GetPcapFileAppidFrameType(int32_t nAppid);

    ///
    /// @brief
    ///    获取pcap文件中所有帧数
    ///
    uint32_t GetPcapFileTotalFrameCount();

    ///
    /// @brief
    ///    获取pcap中某个appid对应的帧数
    ///
    uint32_t GetPcapFileAppidFrameCount(int32_t nAppid);

    ///
    /// @brief
    ///    获取pcap中某个appid对应的采样点数
    ///
    uint32_t GetPcapFileAppidSampleCount(int32_t nAppid);

    ///
    /// @brief
    ///    获取appid的采样率
    ///
    uint32_t GetPcapFileAppidSampleRate(int32_t nAppid);

    ///
    /// @brief
    ///    获取Appid一帧报文asdu数量
    ///
    uint32_t GetPcapFileAppidNoAsdu(int32_t nAppid);

    ///
    /// @brief
    ///    获取某个控制块的通道数目
    ///
    uint32_t GetPcapFileAppidChannelCount(int32_t nAppid);

    ///
    /// @brief
    ///    获取pcap文件对应索引数据帧的异常信息
    ///
    FrameStatus GetPcapFileFrameIndexStatus(uint32_t nIndex);

    ///
    /// @brief
    ///     关闭pcap文件
    ///
    void ClosePcapFile()
    {
        _ResetData();
        _ClosePcapFile();

        //调用此函数为了解决在波形分析中使用内存，退出波形分析时不会自动释放的问题
        ::HeapCompact(GetProcessHeap(), HEAP_NO_SERIALIZE);
    }

///
///=============================================================================
/// smv统计，离散度信息
///=============================================================================
///
    ///
    /// @brief
    ///    开始pcap文件统计
    ///
    ///
    /// @return
    ///     bool    开始成功返回true，失败false
    ///
    bool StartPcapFileStatisticsAnalysis();

    ///
    /// @brief
    ///    停止pcap文件统计
    ///
    /// @return
    ///     bool    停止成功返回true，失败false
    ///
    bool StopPcapFileStatisticsAnalysis();

    ///
    /// @brief
    ///    获取pcap文件goose分析结果
    ///
    /// @param[in]  pData 外部申请的空间指针，用于保存pcap统计结果
    ///
    /// @return
    ///     bool    获取成功返回true，失败false
    ///
    bool GetPcapFileGooseStatisticsResult(CGooseFrameStatistics* pData);

    ///
    /// @brief
    ///    获取pcap文件采样值分析结果，包括离散度信息
    ///
    /// @param[in]  pData 外部申请的空间指针，用于保存pcap统计结果
    ///
    /// @return
    ///     bool    采样值获取成功返回true，失败false
    ///
    bool GetPcapFileSmvStatisticsResult(CDataFrameStatistics* pData);

///
///=============================================================================
/// 波形图，相位相关数据
///=============================================================================
///
    ///
    /// @brief
    ///    设置需要的波形对应的采样点数据范围
    ///
    /// @param[in]  nIndexBegin 采样点起始索引，从0开始
    /// @param[in]  nIndexEnd   采样点结束索引
    ///
    void SetPcapFileWaveAnalysisIndex(uint32_t nIndexBegin, uint32_t nIndexEnd)
    {
        m_nWaveIndexBegin = nIndexBegin;
        m_nWaveIndexEnd   = nIndexEnd;
    }

    ///
    /// @brief
    ///    开始设置范围内采样点的波形处理
    ///
    /// @return
    ///     bool 返回true表示设置开始分析数据成功
    ///
    bool StartPcapFileWaveAnalysis();

    ///
    /// @brief
    ///    停止pcap文件波形图数据分析
    ///
    /// @return
    ///     bool    停止成功返回true，失败false
    ///
    bool StopPcapFileWaveAnalysis();

    ///
    /// @brief
    ///    获取指定索引范围内波形图/相位/有效值数据
    ///
    /// @param[out]  pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool   数据获取成功返回true，pData中保存获取到的数据
    ///
    /// @note
    ///     pData 指向的内存区域由调用者自己申请
    ///
    bool GetPcapFileWaveOscillogramData(CDataOscillogram* pData);

///
///=============================================================================
/// 辅助/工具函数
///=============================================================================
///
///
    ///
    /// @brief
    ///    将相位角都正常化在 -180--180范围之间
    ///
    static float PhaseNormaLizing(float fPhaseSrc);

    ///
    /// @brief
    ///    通知界面分析完成辅助函数
    ///
    bool _NotifyMessage(uint32_t nMessageId, WPARAM wParam = 0, LPARAM lParam = 0);


private:
    ///
    /// @brief
    ///    pcap文件打开
    ///
    bool _OpenPcapFile();

    ///
    /// @brief
    ///    关闭pcap文件
    ///
    bool _ClosePcapFile();

    ///
    /// @brief
    ///    清空和一个pcap文件相关的各种数据，为保存新的数据做准备
    ///
    void _ResetData()
    {
        {
            MapAppidSampleMapping::iterator itr = m_mapAppidSampleMapping.begin();
            while(itr != m_mapAppidSampleMapping.end())
            {

                itr->second.ResetData();
                ++itr;
            }
        }

        m_mapGooseStatistics.clear();
        m_mapSmvStatistics.clear();
        m_mapAppidSampleMapping.clear();
        m_mapPcapFileFrameIndexStatus.clear();

        m_eventCancelAnalysis.ResetEvent();
        m_eventCancelWaveAnalysis.ResetEvent();

        m_nWaveIndexBegin = 0;
        m_nWaveIndexEnd   = 0;

        m_cAppidInfoAnalysis.ResetData();

        MapAppidMsgDispersion::iterator itr = m_mapAppidDispersion.begin();
        while(itr != m_mapAppidDispersion.end())
        {
            delete itr->second;
            itr++;
        }
        m_mapAppidDispersion.clear();

        m_dataOscillogram.ClearChannelVec();
    }

    ///
    /// @brief
    ///    清空波形数据缓存
    ///
    void _ClearWaveData()
    {
        m_dataOscillogram.ClearChannelVec();
    }

    

    ///
    /// @brief
    ///    对一个数据帧进行分析
    ///
    void _HandleOneFrame(uint32_t nPcapFileIndex, FrameTimeStamp stFrameTime, uint8_t *pBuffer, uint32_t nBufferSize);

    ///
    /// @brief
    ///    根据当前控制块appid，分析时间离散度信息
    ///
    void _HandleFrameTimeDispersion(int32_t nAppid, FrameTimeStamp stFrameTime );

    ///
    /// @brief
    ///    处理波形信息数据
    ///
    void _HandleFrame4Wave(const SampleIndexPosition& stSampleIndexPos, uint8_t* pBuffer, uint32_t nBufferSize );

    ///
    /// @brief
    ///    对已经选择的采样点计算有效值和相位角等信息
    ///
    void _CalcChoosenSmpEffVale();

    ///
    /// @brief
    ///    根据采样率返回对应的滤波器参数a,b所需要数据的索引
    ///
    uint32_t GetSmpRateIndex(uint32_t nSmpRate);

    ///
    /// @brief
    ///    获取缓冲去过零点信息
    ///
    /// @param[in]  pData  缓冲区数据地址
    /// @param[in]  nCount 缓冲区数据个数
    /// @param[out] stZeroCrossPoint 获取到的过零点信息
    ///
    /// @return
    ///     bool    获取成功返回true，失败返回false
    ///
    bool GetZeroCrossingPoint(float *pData, uint32_t nCount, ZeroCrossPoint& stZeroCrossPoint );

    ///
    /// @brief
    ///    清空当前控制块的信息，如当前控制块的波形信息等
    ///
    void _ClearCurrenAppidData();

    ///
    /// @brief
    ///    根据采样值，数据帧的映射信息，计算各个appid的离散度，抖动信息
    ///
    void _CalcAllAppidDispersion();

    ///
    /// @brief
    ///    根据采样值，数据帧的映射信息，计算各个appid的离散度，抖动信息
    ///
    void _AddSmv4CurrentOscillogram(const SMV_VALUE& stSmvValue);
private:
    std::wstring                            m_strPcapFile;                  ///< pcap文件路径

    //统计结果
    MapAppidGooseStatistics                 m_mapGooseStatistics;           ///< pcap文件goose数据帧统计结果
    MapAppidSmvStatistics                   m_mapSmvStatistics;             ///< pcap文件smv数据帧统计结果
    MapAppidMsgDispersion                   m_mapAppidDispersion;           ///< pcap文件中appid对应的离散度信息
    MapAppidSampleMapping                   m_mapAppidSampleMapping;        ///< pcap文件中appid对应的采样点采样点在Pcap文件中的位置信息
    MapPcapFileFrameIndexStatus             m_mapPcapFileFrameIndexStatus;  ///< pcap文件中每一帧对应的帧异常信息

    //pcap文件操作
    IPcapFile*                              m_pPcapFile;                    ///< pcap文件操作接口
    uint8_t*                                m_pFrameBuffer;                 ///< 从pcap中读取的一帧数据缓冲指针

    //解码数据帧相关
    MsgRapidDecoder                         m_cMsgRapidDecoder;             ///< 快速解码
    GuoWangFT3Msg*                          m_pGuoWangFT3Msg;               ///< Ft3国网数据帧解析
    NanRuiFT3Msg*                           m_pNanRuiFt3Msg;                ///< Ft3南瑞数据帧解析
    Sav91Msg*                               m_pSav91Msg;                    ///< 9-1数据解析
    Sav92Msg*                               m_pSav92Msg;                    ///< 9-2数据解析
    GooseMsg*                               m_pGooseMsg;                    ///< Goose数据解析

    CEvent                                  m_eventCancelAnalysis;          ///< 用户是否取消正在分析的流程
    CEvent                                  m_eventCancelWaveAnalysis;      ///< 用户取消正在波形数据处理流程

    uint32_t                                m_nWaveIndexBegin;              ///< 设置波形数据范围起始值
    uint32_t                                m_nWaveIndexEnd;                ///< 设置波形数据范围结束值

    enumPcapFileAnalysisStatus              m_enAnalysisStatus;             ///< 文件状态统计的状态
    enumPcapFileAnalysisStatus              m_enAnalysisWaveStatus;         ///< 文件进行波形分析的状态
    CWnd*                                   m_pNotifyWnd;                   ///< pcap解析完毕通知界面的事情是发送消息的窗口指针
    int32_t                                 m_nCurrentAppid;                ///< 当前控制块appid

    CDataOscillogram                        m_dataOscillogram;              ///< 当前控制块的波形数据

    CAppidInfoAnalysis                      m_cAppidInfoAnalysis;           ///< 控制块数量，采样率信息

};

///
/// @brief
///    线程入口，进行pcap文件统计
///
UINT __cdecl PcapFileAnalysisProc( LPVOID pParam );

///
/// @brief
///    线程入口，进行pcap文件波形信息生成
///
UINT __cdecl PcapFileWaveAnalysisProc( LPVOID pParam );

#endif //_PCAP_FILE_ANALYSIS_H
