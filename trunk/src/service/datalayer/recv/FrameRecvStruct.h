/// @file
///
/// @brief
///     �������ݽṹ�嶨��
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.3.25
///
/// �޸���ʷ��
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
///     ʱ����Ľṹ�壬��8���ֽ�
///
struct FrameTimeStamp
{
    uint32_t ts_sec;             ///< ʱ�����������4���ֽ�
    uint32_t ts_usec;            ///< ʱ�����΢����,����ʱ�����С�����֣�4���ֽ�

    FrameTimeStamp(): ts_sec(0)
                    , ts_usec(0)
    {
    }

    ///
    /// @brief
    ///    ��ʱ�������һ��΢��ʱ��
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
    ///    ��������ʱ���ʱ���
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
///     ���������ݰ�ͷ�ṹ����16���ֽڣ�����pcap�Ľṹ���
///
struct BUFFER_PCAP_PACKET_HEADER
{
    FrameTimeStamp  timeStamp;               ///< �Ӹ�������ʱ���1970-1-1 00:00:00��ץ��ʱ������ʱ�䣬8���ֽ�
    uint32_t        capLen;                  ///< ��ʾ������PCAP�ļ��е����ݰ���ʵ�ʳ��ȣ�4���ֽ�
    uint32_t        len;                     ///< ��ʾ���ݰ�����ʵ���ȣ�ԭʼ���ĳ��ȣ���4���ֽ�
    ///< len��capLen�����ʾ��ǰ��������ݰ�������
};

///
/// @brief
///     ����֡��Ϣ�ṹ��
///
struct FrameInfo
{
    uint32_t       nIndex;              ///< ��ʾ����֡����ţ���0��ʼ
    uint32_t       nOffset;             ///< ����֡����ڴ���ʼ��ƫ����
    uint32_t       capLen;              ///< ��ʾ������PCAP�ļ��е����ݰ���ʵ�ʳ��ȣ�4���ֽ�
    FrameTimeStamp timeStamp;           ///< �Ӹ�������ʱ���1970-1-1 00:00:00��ץ��ʱ������ʱ�䣬8���ֽ�
};

///
/// @brief
///    �����FPGA�����������ͣ�Ŀǰ����Ft3/����̽��ʱָ��̽������,��ϵ����������������Ҫ����޸�ֵ�Ķ���
///
enum NetFt3RecvFrameType{
    NET_RECV_TYPE_SMV = 0,                          ///< ��������ղ���ֵ���ݣ�����9-1��9-2
    NET_RECV_TYPE_GOOSE,                            ///< ���������goose����
    NET_RECV_TYPE_IEEE_1588,                        ///< ���������1588��ʱ����
    FT3_RECV_TYPE_SMV,                              ///< �ӹ⴮�ڽ���Ft3����
    RECV_TYPE_UNKOWN                                ///< δ֪����
};

//ǰ����������������
class CSmvRecvConfig;
///
/// @brief
///    
///
class CBaseDetectItem
{
public:
    uint32_t         nID;                            ///< ���ƿ��ʶ�����������жϿ��ƿ��Ƿ���ͬ
    enumFrameType    enFrameType;                    ///< ̽�⵽����֡���ͣ�����Ϊ9-1/9-2/goose
    DsmPortName      enPortName;                     ///< ��ʲô�˿�̽�⵽������
    uint32_t         nChannelCount;                  ///< ͨ����Ŀ
    uint32_t         nSampleRate;                    ///< ��ȡ�����ݵĲ�����
    uint32_t         nSampleRateDetect;              ///< ̽��/���Ķ�ȡ�Ĳ�����
    CSmvRecvConfig*  pRecvConfig;                    ///< ���ƿ��Ӧ��������Ϣָ��
    uint32_t         nAsdu;                          ///< ASDU��Ŀ
    uint32_t         nFrameLen;                      ///< ���ƿ��Ӧ����֡������Ϣ�����ڹ���¼��ʱ�䳤��

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
///    ��������̽��������ṹ�壬һ���ṹ�����ڱ�ʾһ��̽����
///
class CNetDetectItem : public CBaseDetectItem
{
public:
    uint8_t         macAddr[ETHERNET_MAC_LEN];      ///< ̽�⵽��Mac��ַ
    uint16_t        appId;                          ///< ̽�⵽��appid
    CNetDetectItem();
};
inline CNetDetectItem::CNetDetectItem() : appId(0)
{
    memset(macAddr, 0, ETHERNET_MAC_LEN);
}


///
/// @brief
///    ����⴮��̽�����ṹ��
///
struct CFt3DetectItem : public CBaseDetectItem
{
public:
    FT3_BAUD_RATE   enBaudRate;                     ///< ͨѶ�ӿڵĲ�����
    uint16_t        nLDName;                        ///< �߼��豸����
    CFt3DetectItem();
};
inline CFt3DetectItem::CFt3DetectItem() : enBaudRate(FT3_BAUD_RATE_UNKOWN)
                                        , nLDName(0)
{
}

///
/// @brief
///    ��������̽��1588����ṹ��
///
class CNet1588DetectItem : public CNetDetectItem
{
public:
    IEEE1588_SOURCE_PORT_ID     stClockID;                          ///< ̽�⵽��ʱ��ԴID
    uint32_t                    n1588Version;                       ///< 1588�汾��
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
///    Ϊ����ͼ����Ĳ���ֵ��Ϣת�������Ϣ
///
struct SMV_VALUE
{
    CBaseDetectItem* pDetectItem;                            ///< ���浱ǰ���ƿ���Ϣ
    FrameTimeStamp   stFrameTime;                            ///< ����ֵʱ��
    uint32_t         nSmpCnt;                                ///< ��ʾ����������,��1s�ڲ��������
    uint32_t         nChannelCount;                          ///< ʵ����Ч��ͨ����Ŀ
    float    fSmvValue[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];    ///< ����ͨ������ֵ

    SMV_VALUE(): pDetectItem(NULL)
               , nSmpCnt(0)
               , nChannelCount(0)
    {
        memset(fSmvValue, 0, sizeof(fSmvValue));
    }
};

///
/// @brief
///    FT3/9-1����ֵ��Ϣ
///
struct SMV_VALUE_FT3_9_1
{
    CBaseDetectItem* pDetectItem;                            ///< ���浱ǰ���ƿ���Ϣ
    FrameTimeStamp   stFrameTime;                            ///< ����ֵʱ��
    uint32_t         nSmpCnt;                                ///< ��ʾ����������,��1s�ڲ��������
    uint32_t         nChannelCount;                          ///< ʵ����Ч��ͨ����Ŀ
    int16_t    nSmvValue[FT3_9_1_MAX_CHANNEL_COUNT];         ///< ����ͨ������ֵ
};

///
/// @brief
///    9-2����ֵ��Ϣ
///
struct SMV_VALUE_9_2
{
    CBaseDetectItem* pDetectItem;                            ///< ���浱ǰ���ƿ���Ϣ
    FrameTimeStamp   stFrameTime;                            ///< ����ֵʱ��
    uint32_t         nSmpCnt;                                ///< ��ʾ����������,��1s�ڲ��������
    uint32_t         nChannelCount;                          ///< ʵ����Ч��ͨ����Ŀ
    int32_t     nSmvValue[NET_9_2_LE_MAX_CHANNELS_PER_ASDU]; ///< ����ͨ������ֵ
};

//״̬��1
#define FT3_STATUS_WORD_NEED_REPAIR           0x0001        ///< Ҫ��ά��, 0:���ã�1������򱨾���Ҫ��ά�ޣ�
#define FT3_STATUS_WORD_LLNO_MODE             0x0002        ///< ����״̬��־��0��ʶ�������У�1��ʵ�飨���ޣ� 
#define FT3_STATUS_WORD_WAKEUP_TIME_VALIDITY  0x0004        ///< ����ʱ���ʶ��0����ͨ���������У���1������ʱ�䣬������Ч
#define FT3_STATUS_WORD_MU_SYNC_METHOD        0x0008        ///< �ϲ���Ԫͬ������ 0�������ò�ֵ �� 1�����ò�ֵ
#define FT3_STATUS_WORD_SYNCED_MU             0x0010        ///< ��ͬ���ĺϲ���Ԫ 0��ͬ���� 1�� ʱ��ͬ��miss/invalid
///< ����ͨ������Ч��־��0��ʾ��Ч��1��ʾ��Ч
#define FT3_CHANNEL_MASK_CHANNEL_1            0x0020
#define FT3_CHANNEL_MASK_CHANNEL_2            0x0040
#define FT3_CHANNEL_MASK_CHANNEL_3            0x0080
#define FT3_CHANNEL_MASK_CHANNEL_4            0x0100
#define FT3_CHANNEL_MASK_CHANNEL_5            0x0200
#define FT3_CHANNEL_MASK_CHANNEL_6            0x0400
#define FT3_CHANNEL_MASK_CHANNEL_7            0x0800

#define FT3_STATUS_WORD_CT_TYPE               0x1000         ///< �����������������
#define FT3_STATUS_WORD_RANGE_FLAG            0x2000         ///< 0:�������� scp = 01cf�� 1�� ��������00e7
#define FT3_STATUS_WORD_FOUTURE_USE1          0x4000
#define FT3_STATUS_WORD_FOUTURE_USE2          0x8000

//״̬��2
#define FT3_CHANNEL_MASK_CHANNEL_8            0x0001
#define FT3_CHANNEL_MASK_CHANNEL_9            0x0002
#define FT3_CHANNEL_MASK_CHANNEL_10           0x0004
#define FT3_CHANNEL_MASK_CHANNEL_11           0x0008
#define FT3_CHANNEL_MASK_CHANNEL_12           0x0010
//����12ͨ���ģ����±���Ϊδ��ʹ�ã����ڹ�����չ����ʾ��Ӧͨ��
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

//��Ӧͨ��������Ӧ��mask��������0��ʼ
//�ӵ�8��ͨ��������Ϊ7����ʼ��maskҪʹ��״̬��2���бȽ�
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

#define SMV_9_2_VALID_MASK      0x00000003      ///< 9-2����ֵ��Ӧ����Ч��־λ�����2λ��00��ʾvalidity:good��
#define SMV_9_2_TEST_MASK       0x00000800      ///< 9-2����ֵ��Ӧ�ļ��ޱ�־����λ��0����false��û�м��ޣ�


///
/// @brief
///    �������µĿ��ƿ�ͨ����Ч/Ʒ����Ϣ
///
class CSmvCtrlBlkQuality
{
private:
    CBaseDetectItem*    m_pDetectItem;                       ///< ��Ӧ�Ŀ��ƿ���Ϣ
    uint32_t   m_nQuality[NET_9_2_LE_MAX_CHANNELS_PER_ASDU]; ///< Ʒ����Ϣ
    uint16_t            m_nStatusWord1;                      ///< ״̬��1�����Ft3/9-1
    uint16_t            m_nStatusWord2;                      ///< ״̬��2�����Ft3/9-1
    bool                m_bIsMuSync;                         ///< MU�Ƿ�ͬ��


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
    ///    ��������״̬��Ϣ�����л����ƿ�ʱʹ��
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
    ///    ���ÿ��ƿ���Ϣ
    ///
    void SetCtrlBlk(CBaseDetectItem* pDetectItem)
    {
        _ResetStatus();
        m_pDetectItem = pDetectItem;
    }

    ///
    /// @brief
    ///    ��ȡ���ƿ���Ϣ
    ///
    CBaseDetectItem* GetCtrlBlk()
    {
        return m_pDetectItem;
    }

    ///
    /// @brief
    ///    ����Mu�Ƿ�ͬ��
    ///
    /// @return
    ///     bool ͬ������true
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
    ///    ����mu�Ƿ�ͬ��
    ///
    void SetMuSync(bool bSync)
    {
        m_bIsMuSync = bSync;
    }

    ///
    /// @brief
    ///    ��ȡͨ��������Ϣ
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
    ///     h��ȡ����֡����
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
    ///    ��ȡָ��ͨ������Ч��Ϣ
    ///
    /// @return
    ///     ��Ч����true
    ///
    bool IsChannelValid(uint32_t nChannelIndex)
    {
        if (m_pDetectItem)
        {
            switch(GetFrameType())
            {
            //ͨ����12
            case FRAME_TYPE_NET_9_1:
            case FRAME_TYPE_FT3_NANRUI:
                {
                    // ͨ������0-6��7��ͨ����Ч��־λ��״̬��1
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
                //ͨ����22
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
                //�ж϶�Ӧ��Ʒ��������λ
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
    ///    ��ȡͨ����Ʒ������,���9-2
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
    ///    ����ͨ��Ʒ������,���9-2
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
    ///    ���ÿ��ƿ�״̬�֣����ft3/9-1
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
    ///    ��ȡ���ƿ�״̬�֣����ft3/9-1
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
    ///    ��ȡ9-1/ft3��rangflag��ʶ
    ///
    /// @return
    ///     ����0��ʾrangeflagΪ0������1��ʾrangefalgΪ1����������ֵ��ʾ��ȡʧ��
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
        return 3;   //��ʾ��ȡʧ��
    }

    ///
    /// @brief
    ///    ��ȡ�Ƿ��м��ޱ�־
    ///
    bool IsChannelTest(uint32_t nChannelIndex = 0)
    {
        //Ĭ��Ϊû�м��ޱ�־
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
        //Ĭ��Ϊû�м��ޱ�־
        return bRet;
    }
};

class CDetectItemFrame
{
private:
    CBaseDetectItem* m_pDetectItem;                                     ///< ��ǰ����֡��Ӧ�Ŀ��ƿ�
    uint32_t         m_nFrameLen;                                       ///< ��ǰ����֡����
    uint8_t*         m_pFrameData;                                      ///< ����֡����

public:
    CDetectItemFrame(CBaseDetectItem* pDetectItem): m_pDetectItem(pDetectItem)
                                                  , m_nFrameLen(0)
                                                  , m_pFrameData(NULL)
    {
    }

    ///
    /// @brief
    ///    �������֡����
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
    ///    ��ȡ����֡����
    ///
    uint32_t GetFrameLen()
    {
        return m_nFrameLen;
    }

    ///
    /// @brief
    ///    ��ȡ��Ӧ�Ŀ��ƿ�ָ��
    ///
    CBaseDetectItem* GetDetectItem()
    {
        return m_pDetectItem;
    }

    ///
    /// @brief
    ///    ��ȡ�����������׵�ַ
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