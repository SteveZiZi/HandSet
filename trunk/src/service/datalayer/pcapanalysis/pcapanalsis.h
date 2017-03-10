/// @file
///
/// @brief
///     �Լ��ص�pcap�ļ����з���/ͳ��
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.10.31
///
/// �޸���ʷ��
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

//�����ȡpcap�ļ�ʹ���ڴ滺������С
#define PCAP_FILE_READ_BUFFER_SIZE  (1024*1024*5)

//���屾ģ���Ƿ��ӡ������Ϣ,1��ӡ��0����ӡ
#define PCAP_FILE_ANALYSIS_OUTPUT_LOG   1

///
/// @brief
///    �����ļ��������֪ͨ�������Ϣid
///
#define WM_PCAPFILE_ANALYSIS_MESSAGE        (PRIVATE_WND_USERID + 51)                   ///< ���ݲ㷢�͸��������ϢID
#define MSG_PCAPFILE_STATUS_ANALYSIS_DONE   (1)                                         ///< �����쳣״̬��Ϣͳ�����
#define MSG_PCAPFILE_WAVE_ANALYSIS_DONE     (2)                                         ///< �������ݷ������
#define MSG_PCAPFILE_OPEN_FAILED            (3)                                         ///< ��pcap�ļ�ʧ��
#define MSG_PCAPFILE_ANALYSIS_USER_CACEL    (4)                                         ///< �û�ȡ��
#define MSG_PCAPFILE_ANALYSIS_FAILED        (5)                                         ///< �����ļ�ʧ��

///
/// @brief
///    �ļ���ǰ״̬��Ϣ
///
enum enumPcapFileAnalysisStatus
{
    PCAP_FILE_NOT_ANALYSISED = 0,            ///< �ļ�δ������
    PCAP_FILE_ANALYSIS_ING,                  ///< �ļ����ڷ�����
    PCAP_FILE_ANALYSIS_USER_CANCEL,          ///< �ļ��������û�ȡ��
    PCAP_FILE_ANALYSIS_DONE,                 ///< �ļ��������
    PCAP_FILE_ANALYSIS_CALC_WAVE,            ///< ���ڴ���������
    PCAP_FILE_ANALYSIS_CALC_WAVE_DONE        ///< �������ݴ������
};

///
/// @brief
///    ����һ����������Pcap�ļ��е�λ����Ϣ
///
struct SampleIndexPosition
{
    uint32_t nAppidSampleIndex;             ///< ��������ţ���0��ʼ����
    uint32_t nAppidFrameIndex;              ///< ��ǰ���������ڵ�����֡�ǵ�ǰappid�ĵڼ�֡����0��ʼ
    uint32_t nPcapFrameIndex;               ///< ��ǰ���������ڵ�����֡������pcap�ļ����ǵڼ�֡����0��ʼ
    uint32_t nAsduCount;                    ///< ��ǰ���������ڵ�����֡�м���ASDU
    uint32_t nAsduIndex;                    ///< ��ǰ����������������֡�еļ���ASDU


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
///    ����һ�����ƿ����Ϣ
///
struct AppidInfo
{
    int32_t         nAppid;
    enumFrameType   enFrameType;                      ///< ���ƿ�����֡����
    uint32_t        nSmpRate;                         ///< ����ǲ���ֵ����ʾ�Ĳ����ʣ�goose���ݴ���������
    uint32_t        nAsdu;                            ///< ���ƿ�һ������֡��asdu��Ŀ����9-1/9-2������
    uint32_t        nChannelCount;                    ///< ���ƿ�ͨ������

    uint32_t        nAppidSmpCount;                   ///< �˿��ƿ��ܲ��������
    FrameTimeStamp  stFirstTimeStamp;                 ///< �˿��ƿ��һ֡ʱ���
    FrameTimeStamp  stLastTimeStamp;                  ///< �˿��ƿ����һ֡ʱ���

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
    uint32_t        nPcapFileFrameIndex;                                  ///< ֡��pcap�ļ��е�����
    uint32_t        nFramelen;                                            ///< ֡����
    FrameTimeStamp  stTimeStamp;                                          ///< ֡��ʱ�����Ϣ
};

typedef std::map<int32_t, CMsgDisperisonCalc*>   MapAppidMsgDispersion;   ///< ĳ�����ƿ����ɢ����Ϣ
typedef std::map<uint32_t, SMV_VALUE>            MapSampleIndexSmvData;   ///< ĳ���������Ӧ��ͨ��������Ϣ
typedef std::map<int32_t, AppidInfo>             MapAppidInfo;            ///< ĳ�����ƿ�Ĳ�������Ϣ
typedef std::vector<SampleIndexPosition>         VecAppidSampleMapping;   ///< ����һ��appid��������pcap��λ����Ϣ
typedef std::vector<FrameIndexInfo>  VecAppidFrameIndex2PcapFrameIndex;   ///< ����һ��appid��Ӧ֡������pcap��֡������


///
/// @brief
///    ��Ҫͳ�ƿ��ƿ�Ĳ�������Ϣ
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
    ///    ����ÿһ֡����Ϣ
    ///
    void ParseAppidInfo(FrameTimeStamp stFrameTime, int32_t nAppid, enumFrameType enFrameType, uint32_t nAsdu, uint32_t nChannelCount, uint8_t* pBuff, uint32_t nBufLen);

    ///
    /// @brief
    ///    ����֡������ϣ������������Ϣ
    ///
    void CalcSmpRate();

    ///
    /// @brief
    ///    �����������
    ///
    void ResetData()
    {
        m_mapAppidInfoCalcInfo.clear();
    }

    ///
    /// @brief
    ///    ��ȡĳ��appid�Ĳ�����
    ///
    uint32_t GetAppidSmpRate(int32_t nAppid);

    ///
    /// @brief
    ///    ��ȡĳ�����ƿ��asdu����
    ///
    uint32_t GetAppidNoAsdu(int32_t nAppid);

    ///
    /// @brief
    ///    ��ȡĳ�����ƿ������֡����
    ///
    enumFrameType GetAppidFrameType(int32_t nAppid);

    ///
    /// @brief
    ///    ��ȡĳ�����ƿ��ͨ����Ŀ
    ///
    uint32_t GetAppidChannelCount(int32_t nAppid);
public:
    MapAppidInfo        m_mapAppidInfoCalcInfo;
};


///
/// @brief
///    ����һ�����ƿ��в�������ʵ�ʱ�������λ����Ϣ
///
class CAppidSampleMapping
{
public:
    CAppidSampleMapping();
    ~CAppidSampleMapping();

public:
    ///
    /// @brief
    ///    ����һ����������Ϣ
    ///
    void ParseSampleInfo(SampleIndexPosition stSamplePosition, const FrameTimeStamp& stTimeStamp, uint32_t nFrameLen);

    ///
    /// @brief
    ///    ����һ����������Ϣ
    ///
    void ResetData();


public:
    VecAppidSampleMapping                    m_vecAppidSampleMap;            ///< ����һ�����ƿ������λ�õ�������Ϣ
    VecAppidFrameIndex2PcapFrameIndex        m_vecAppidFrameIndexMap;        ///< ����һ�����ƿ�����֡λ�õ���Ϣ
};

///
/// @brief
///    ����smvһ�����ƿ飨appid����Ӧ������λ����Ϣ��
///    ��ҪӦ��һ��pcap���ܳ��ֶ�����ƿ�����
///
typedef std::map<int32_t, CAppidSampleMapping>     MapAppidSampleMapping;

///
/// @brief
///    ������������Ϣ
///
struct ZeroCrossPoint
{
    int32_t  nIndex;                                                        ///< ����������
    float    fPhase;                                                        ///< ���������λ���������λΪ0����180
    float    fSmpValue;                                                     ///< �����Ĳ���ֵ
    bool     bPreciseZero;                                                  ///< �Ƿ��Ǿ�ȷ��0ֵ����Χ0.000001

    ZeroCrossPoint(): nIndex(-1)
                    , fPhase(0.0)
                    , fSmpValue(0.0)
                    , bPreciseZero(false)
    {
    }
};

///
/// @brief
///    ����һ��֡�쳣���ڵ�λ
///
#define FRAME_STATUS_SMP_LOST               (1<<0)                          ///< SMV�����㶪ʧ
#define FRAME_STATUS_SMP_MIS_ORDER          (1<<1)                          ///< SMV����
#define FRAME_STATUS_SMP_RETRY              (1<<2)                          ///< SMV�ط�
#define FRAME_STATUS_SMP_LOST_SYNC          (1<<3)                          ///< SMVʧȥͬ��
#define FRAME_STATUS_SMP_Q_INVALID          (1<<4)                          ///< SMVƷ����Ч
#define FRAME_STATUS_SMP_TEST               (1<<5)                          ///< SMV����
#define FRAME_STATUS_GOOSE_SQ_LOST          (1<<6)                          ///< GOOSE sq��ʧ
#define FRAME_STATUS_GOOSE_SQ_MIS_ORDER     (1<<7)                          ///< GOOSE sq����
#define FRAME_STATUS_GOOSE_SQ_RETRY         (1<<8)                          ///< GOOSE sq�ط�
#define FRAME_STATUS_GOOSE_ST_LOST          (1<<9)                          ///< GOOSE st��ʧ
#define FRAME_STATUS_GOOSE_ST_MIS_ORDER     (1<<10)                         ///< GOOSE st����
#define FRAME_STATUS_GOOSE_FRAME_LOST       (1<<11)                         ///< GOOSE ���Ķ�ʧ��֡���ĵ���Ƽ������ TimeAllowedToLive=T0 *2��
#define FRAME_STATUS_GOOSE_COMM_BREAK       (1<<12)                         ///< GOOSE ͨ���жϣ�֡���ĵ���Ƽ������ TimeAllowedToLive * 2 ��
#define FRAME_STATUS_GOOSE_TTL_INVALID      (1<<13)                         ///< GOOSE TTL��Ч
#define FRAME_STATUS_GOOSE_CLK_NOT_SYNC     (1<<14)                         ///< GOOSE ʱ��δͬ��
#define FRAME_STATUS_GOOSE_CLK_FAILED       (1<<15)                         ///< GOOSE ʱ�ӹ���

///
/// @brief
///    ����һ��֡���쳣��Ϣ
///
class FrameStatus
{
public:
    uint16_t                    nData;                         ///< ��¼һ��֡�쳣��Ϣ��ÿһλ1�����쳣��0��ʾ����
    std::vector<uint32_t>       vecSmpCnt;                     ///< ��¼һ��֡��������Ĳ��������Ϣ

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
    ///    ����ĳһλ����
    ///
    /// @param[in]  nFrameStatusMask    ������쳣λ��ֵ
    /// @param[in]  bSetTrue            �Ƿ��ö�ӦλΪ1
    ///
    /// @note
    ///     SetValue(FRAME_STATUS_SMP_LOST) ��ʾ����smv���������쳣
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
    ///    ��ȡ�쳣����Ϣ
    ///
    uint16_t GetValue()
    {
        return nData;
    }

    ///
    /// @brief
    ///    �����쳣����Ϣ
    ///
    void SetData(uint16_t nVal)
    {
        nData = nVal;
    }
};

///
/// @brief
///    pcap�ļ�goose����֡�쳣����
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
    ///    ����һ֡���ģ����ر��ĸ�֡״̬��Ϣ
    ///
    FrameStatus ParseGooseFrame(uint32_t nTTL, FrameTimeStamp& stFrameTimeStamp,
                                uint8_t nGooseUtcTimeQuality,
                                int32_t nStNum, int32_t nSqNum);

    friend class CPcapAnalysis;
private:
    int32_t                             m_nStNum;                       ///< ��¼goose���ƿ���һ֡��stNumber��ÿ������֡���´�����
    int32_t                             m_nSqNum;                       ///< ��¼goose���ƿ���һ֡��sqNumber��ÿ������֡���´�����
    FrameTimeStamp                      m_stLastFrameTimeStamp;         ///< ��һ����֡�Ľ���ʱ�䣬�����жϳ�ʱ/����
    GooseFrameStatistics                m_stGooseFrameStatistics;       ///< ����ͳ����Ϣ
};

///
/// @brief
///    pcap�ļ�smv����ֵ״̬����ͳ��
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
    ///    ��������һ����������Ϣ
    ///
    FrameStatus ParseSamplePoint(int32_t nPcapFileFrameIndex, CBaseDetectItem* pDetectItem,
                                 int32_t nSmpIndex, CSmvCtrlBlkQuality& smvCtrlBlkQuality);

    ///
    /// @brief
    ///    ��������һ��֡ʱ�����Ϣ,�ж�ͳ���еĶ�������
    ///
    void ParseFrameTimeStamp(FrameTimeStamp& stFrameTimeStamp, uint32_t nAsdu = 1, uint32_t nSmpRate = 4000);

    friend class CPcapAnalysis;
private:
    ///
    /// @brief
    ///    ��������ʱ���ʱ���ж�ʱ���Ƿ񶶶���ʱ������ƫ�������������10us��
    ///
    bool _IsTimeStampJitter(const FrameTimeStamp& tm1, const FrameTimeStamp& tm2, uint32_t nAsdu = 1, uint32_t nSmpRate = 4000);

private:
    int32_t             m_nLastPcapFileFrameIndex;                          ///< ����pcap�ļ�������֡��ţ��ж��������ڲ���ֵ�Ƿ�λ��ͬһ����֡
    int32_t             m_nLastSampleCountIndex;                            ///< �ϴ�����֡�Ĳ���������
    FrameTimeStamp      m_stLastFrameTimeStamp;                             ///< ��һ����֡��ʱ�����ͳ�ƶ�����Ϣ
    SmvFrameStatistics  m_stSmvFrameStatistics;                             ///< ���ƿ��Ӧ��ͳ����Ϣ
};


typedef std::map<int32_t, CPcapFileGooseFrameStatus>    MapAppidGooseStatistics; ///< ĳ��goose���ƿ��Ӧ���쳣ͳ����Ϣ
typedef std::map<int32_t, CPcapFileSmvSmpStatus>        MapAppidSmvStatistics;   ///< ĳ������ֵ���ƿ��Ӧ��ͳ����Ϣ
typedef std::map<uint32_t, FrameStatus>             MapPcapFileFrameIndexStatus; ///< pcap�ļ���û֡��Ӧ���쳣��Ϣ

///
/// @brief
///    pcap�ļ�������
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
    ///    ��ȡ��ʵ���ӿ�
    ///
    static CPcapAnalysis* getInstance()
    {
        static CPcapAnalysis s_instance;
        return &s_instance;
    }

    ///
    /// @brief
    ///    ��Чֵ����
    /// @param[in] 
    ///    pSampleValues    ������ֵ
    ///    sampleNum        ��������
    ///
    /// @return
    ///    float           ��Чֵ
    ///
    static float effValueCalc(const float pSampleValues[], uint32_t sampleNum);

    ///
    /// @brief
    ///    �˲�
    /// @param[in] 
    ///    ord                  �˲�������,�ݹ̶�Ϊ4
    ///    a                    �˲�������a_w
    ///    b                    �˲�������b_w
    ///    np                   ��������
    ///    x                    ����
    ///    leftxlen             y���Ԫ�ظ����������������㣬��ָ��Ϊ0
    /// @param[out]
    ///    y                    �˲�������
    ///
    static void filterx(int ord, const float *a, const float *b, int np,
        const float *x, float *y, int leftxlen );

public:
///
///=============================================================================
/// ͨ�����ã���ȡ������Ϣ
///=============================================================================
///
    ///
    /// @brief
    ///    ����Ҫ������pcap�ļ�����·��
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
    ///    ����֪ͨ����ʱʹ�õĴ���Cwndָ��
    ///
    bool SetPcapFileNotifyWnd(CWnd* pWnd)
    {
        m_pNotifyWnd = pWnd;
        return true;
    }

    ///
    /// @brief
    ///    ��ȡ�������õ�֪ͨ����ָ��
    ///
    CWnd* GetPcapFileNotifyWnd()
    {
        return m_pNotifyWnd;
    }

    ///
    /// @brief
    ///    ��ȡpcap�ļ��п��ƿ飨appid����Ŀ
    ///
    uint32_t GetPcapFileAppidCount();

    ///
    /// @brief
    ///    ��ȡһ�����ƿ�Appid
    ///
    /// @return
    ///     ��ȡ��Ӧ������appid��-1��ʾ��ȡʧ��
    ///
    int32_t GetPcapFileOneAppid(uint32_t nIndex);

    ///
    /// @brief
    ///    ���õ�ǰ�Ŀ��ƿ�
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
    ///    ��ȡ��ǰ�Ŀ��ƿ�appid
    ///
    int32_t GetCurrentAppid()
    {
        return m_nCurrentAppid;
    }

    ///
    /// @brief
    ///    ��ȡһ��appid����ֵ��Ӧ����ֵ/֡��pcap�е�ӳ����Ϣ
    ///
    const CAppidSampleMapping* GetPcapFileAppidSampleMapping(int32_t nAppid);

    ///
    /// @brief
    ///    ��ȡ��ǰ���ƿ������֡����
    ///
    enumFrameType GetPcapFileAppidFrameType(int32_t nAppid);

    ///
    /// @brief
    ///    ��ȡpcap�ļ�������֡��
    ///
    uint32_t GetPcapFileTotalFrameCount();

    ///
    /// @brief
    ///    ��ȡpcap��ĳ��appid��Ӧ��֡��
    ///
    uint32_t GetPcapFileAppidFrameCount(int32_t nAppid);

    ///
    /// @brief
    ///    ��ȡpcap��ĳ��appid��Ӧ�Ĳ�������
    ///
    uint32_t GetPcapFileAppidSampleCount(int32_t nAppid);

    ///
    /// @brief
    ///    ��ȡappid�Ĳ�����
    ///
    uint32_t GetPcapFileAppidSampleRate(int32_t nAppid);

    ///
    /// @brief
    ///    ��ȡAppidһ֡����asdu����
    ///
    uint32_t GetPcapFileAppidNoAsdu(int32_t nAppid);

    ///
    /// @brief
    ///    ��ȡĳ�����ƿ��ͨ����Ŀ
    ///
    uint32_t GetPcapFileAppidChannelCount(int32_t nAppid);

    ///
    /// @brief
    ///    ��ȡpcap�ļ���Ӧ��������֡���쳣��Ϣ
    ///
    FrameStatus GetPcapFileFrameIndexStatus(uint32_t nIndex);

    ///
    /// @brief
    ///     �ر�pcap�ļ�
    ///
    void ClosePcapFile()
    {
        _ResetData();
        _ClosePcapFile();

        //���ô˺���Ϊ�˽���ڲ��η�����ʹ���ڴ棬�˳����η���ʱ�����Զ��ͷŵ�����
        ::HeapCompact(GetProcessHeap(), HEAP_NO_SERIALIZE);
    }

///
///=============================================================================
/// smvͳ�ƣ���ɢ����Ϣ
///=============================================================================
///
    ///
    /// @brief
    ///    ��ʼpcap�ļ�ͳ��
    ///
    ///
    /// @return
    ///     bool    ��ʼ�ɹ�����true��ʧ��false
    ///
    bool StartPcapFileStatisticsAnalysis();

    ///
    /// @brief
    ///    ֹͣpcap�ļ�ͳ��
    ///
    /// @return
    ///     bool    ֹͣ�ɹ�����true��ʧ��false
    ///
    bool StopPcapFileStatisticsAnalysis();

    ///
    /// @brief
    ///    ��ȡpcap�ļ�goose�������
    ///
    /// @param[in]  pData �ⲿ����Ŀռ�ָ�룬���ڱ���pcapͳ�ƽ��
    ///
    /// @return
    ///     bool    ��ȡ�ɹ�����true��ʧ��false
    ///
    bool GetPcapFileGooseStatisticsResult(CGooseFrameStatistics* pData);

    ///
    /// @brief
    ///    ��ȡpcap�ļ�����ֵ���������������ɢ����Ϣ
    ///
    /// @param[in]  pData �ⲿ����Ŀռ�ָ�룬���ڱ���pcapͳ�ƽ��
    ///
    /// @return
    ///     bool    ����ֵ��ȡ�ɹ�����true��ʧ��false
    ///
    bool GetPcapFileSmvStatisticsResult(CDataFrameStatistics* pData);

///
///=============================================================================
/// ����ͼ����λ�������
///=============================================================================
///
    ///
    /// @brief
    ///    ������Ҫ�Ĳ��ζ�Ӧ�Ĳ��������ݷ�Χ
    ///
    /// @param[in]  nIndexBegin ��������ʼ��������0��ʼ
    /// @param[in]  nIndexEnd   �������������
    ///
    void SetPcapFileWaveAnalysisIndex(uint32_t nIndexBegin, uint32_t nIndexEnd)
    {
        m_nWaveIndexBegin = nIndexBegin;
        m_nWaveIndexEnd   = nIndexEnd;
    }

    ///
    /// @brief
    ///    ��ʼ���÷�Χ�ڲ�����Ĳ��δ���
    ///
    /// @return
    ///     bool ����true��ʾ���ÿ�ʼ�������ݳɹ�
    ///
    bool StartPcapFileWaveAnalysis();

    ///
    /// @brief
    ///    ֹͣpcap�ļ�����ͼ���ݷ���
    ///
    /// @return
    ///     bool    ֹͣ�ɹ�����true��ʧ��false
    ///
    bool StopPcapFileWaveAnalysis();

    ///
    /// @brief
    ///    ��ȡָ��������Χ�ڲ���ͼ/��λ/��Чֵ����
    ///
    /// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    /// @note
    ///     pData ָ����ڴ������ɵ������Լ�����
    ///
    bool GetPcapFileWaveOscillogramData(CDataOscillogram* pData);

///
///=============================================================================
/// ����/���ߺ���
///=============================================================================
///
///
    ///
    /// @brief
    ///    ����λ�Ƕ��������� -180--180��Χ֮��
    ///
    static float PhaseNormaLizing(float fPhaseSrc);

    ///
    /// @brief
    ///    ֪ͨ���������ɸ�������
    ///
    bool _NotifyMessage(uint32_t nMessageId, WPARAM wParam = 0, LPARAM lParam = 0);


private:
    ///
    /// @brief
    ///    pcap�ļ���
    ///
    bool _OpenPcapFile();

    ///
    /// @brief
    ///    �ر�pcap�ļ�
    ///
    bool _ClosePcapFile();

    ///
    /// @brief
    ///    ��պ�һ��pcap�ļ���صĸ������ݣ�Ϊ�����µ�������׼��
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
    ///    ��ղ������ݻ���
    ///
    void _ClearWaveData()
    {
        m_dataOscillogram.ClearChannelVec();
    }

    

    ///
    /// @brief
    ///    ��һ������֡���з���
    ///
    void _HandleOneFrame(uint32_t nPcapFileIndex, FrameTimeStamp stFrameTime, uint8_t *pBuffer, uint32_t nBufferSize);

    ///
    /// @brief
    ///    ���ݵ�ǰ���ƿ�appid������ʱ����ɢ����Ϣ
    ///
    void _HandleFrameTimeDispersion(int32_t nAppid, FrameTimeStamp stFrameTime );

    ///
    /// @brief
    ///    ��������Ϣ����
    ///
    void _HandleFrame4Wave(const SampleIndexPosition& stSampleIndexPos, uint8_t* pBuffer, uint32_t nBufferSize );

    ///
    /// @brief
    ///    ���Ѿ�ѡ��Ĳ����������Чֵ����λ�ǵ���Ϣ
    ///
    void _CalcChoosenSmpEffVale();

    ///
    /// @brief
    ///    ���ݲ����ʷ��ض�Ӧ���˲�������a,b����Ҫ���ݵ�����
    ///
    uint32_t GetSmpRateIndex(uint32_t nSmpRate);

    ///
    /// @brief
    ///    ��ȡ����ȥ�������Ϣ
    ///
    /// @param[in]  pData  ���������ݵ�ַ
    /// @param[in]  nCount ���������ݸ���
    /// @param[out] stZeroCrossPoint ��ȡ���Ĺ������Ϣ
    ///
    /// @return
    ///     bool    ��ȡ�ɹ�����true��ʧ�ܷ���false
    ///
    bool GetZeroCrossingPoint(float *pData, uint32_t nCount, ZeroCrossPoint& stZeroCrossPoint );

    ///
    /// @brief
    ///    ��յ�ǰ���ƿ����Ϣ���統ǰ���ƿ�Ĳ�����Ϣ��
    ///
    void _ClearCurrenAppidData();

    ///
    /// @brief
    ///    ���ݲ���ֵ������֡��ӳ����Ϣ���������appid����ɢ�ȣ�������Ϣ
    ///
    void _CalcAllAppidDispersion();

    ///
    /// @brief
    ///    ���ݲ���ֵ������֡��ӳ����Ϣ���������appid����ɢ�ȣ�������Ϣ
    ///
    void _AddSmv4CurrentOscillogram(const SMV_VALUE& stSmvValue);
private:
    std::wstring                            m_strPcapFile;                  ///< pcap�ļ�·��

    //ͳ�ƽ��
    MapAppidGooseStatistics                 m_mapGooseStatistics;           ///< pcap�ļ�goose����֡ͳ�ƽ��
    MapAppidSmvStatistics                   m_mapSmvStatistics;             ///< pcap�ļ�smv����֡ͳ�ƽ��
    MapAppidMsgDispersion                   m_mapAppidDispersion;           ///< pcap�ļ���appid��Ӧ����ɢ����Ϣ
    MapAppidSampleMapping                   m_mapAppidSampleMapping;        ///< pcap�ļ���appid��Ӧ�Ĳ������������Pcap�ļ��е�λ����Ϣ
    MapPcapFileFrameIndexStatus             m_mapPcapFileFrameIndexStatus;  ///< pcap�ļ���ÿһ֡��Ӧ��֡�쳣��Ϣ

    //pcap�ļ�����
    IPcapFile*                              m_pPcapFile;                    ///< pcap�ļ������ӿ�
    uint8_t*                                m_pFrameBuffer;                 ///< ��pcap�ж�ȡ��һ֡���ݻ���ָ��

    //��������֡���
    MsgRapidDecoder                         m_cMsgRapidDecoder;             ///< ���ٽ���
    GuoWangFT3Msg*                          m_pGuoWangFT3Msg;               ///< Ft3��������֡����
    NanRuiFT3Msg*                           m_pNanRuiFt3Msg;                ///< Ft3��������֡����
    Sav91Msg*                               m_pSav91Msg;                    ///< 9-1���ݽ���
    Sav92Msg*                               m_pSav92Msg;                    ///< 9-2���ݽ���
    GooseMsg*                               m_pGooseMsg;                    ///< Goose���ݽ���

    CEvent                                  m_eventCancelAnalysis;          ///< �û��Ƿ�ȡ�����ڷ���������
    CEvent                                  m_eventCancelWaveAnalysis;      ///< �û�ȡ�����ڲ������ݴ�������

    uint32_t                                m_nWaveIndexBegin;              ///< ���ò������ݷ�Χ��ʼֵ
    uint32_t                                m_nWaveIndexEnd;                ///< ���ò������ݷ�Χ����ֵ

    enumPcapFileAnalysisStatus              m_enAnalysisStatus;             ///< �ļ�״̬ͳ�Ƶ�״̬
    enumPcapFileAnalysisStatus              m_enAnalysisWaveStatus;         ///< �ļ����в��η�����״̬
    CWnd*                                   m_pNotifyWnd;                   ///< pcap�������֪ͨ����������Ƿ�����Ϣ�Ĵ���ָ��
    int32_t                                 m_nCurrentAppid;                ///< ��ǰ���ƿ�appid

    CDataOscillogram                        m_dataOscillogram;              ///< ��ǰ���ƿ�Ĳ�������

    CAppidInfoAnalysis                      m_cAppidInfoAnalysis;           ///< ���ƿ���������������Ϣ

};

///
/// @brief
///    �߳���ڣ�����pcap�ļ�ͳ��
///
UINT __cdecl PcapFileAnalysisProc( LPVOID pParam );

///
/// @brief
///    �߳���ڣ�����pcap�ļ�������Ϣ����
///
UINT __cdecl PcapFileWaveAnalysisProc( LPVOID pParam );

#endif //_PCAP_FILE_ANALYSIS_H
