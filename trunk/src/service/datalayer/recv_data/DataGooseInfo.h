/// @file
///
/// @brief
///     ����goose���ƿ���Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.6.6
///
/// �޸���ʷ��
///

#pragma once

#include <vector>
#include "src\service\datalayer\include\datatypes.h"
#include "src\utils\MessageCodec\include\goose_types.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"

///
/// @brief
///    gooseͨ����Ϣ
///
class CGooseChannel
{
public:
    CGooseChannel(): m_nID(0)
                   , m_enChannelType(CHANNEL_TYPE_UNKOWN)
                   , m_strChannelDesc(L"")
                   , m_pChannelData(NULL)
                   , m_pParentChannel(NULL)
    {
        m_vecSubChannels.clear();
    }

    ~CGooseChannel()
    {
        if ( NULL != m_pChannelData)
        {
            GooseDataFree(m_pChannelData);
            m_pChannelData = NULL;
        }
        ClearSubChannels();
    }

    ///
    /// @brief
    ///    ����ͨ��id
    ///
    void SetChannelID(uint32_t nID)
    {
        m_nID = nID;
    }

    ///
    /// @brief
    ///    ��ȡͨ��id����0��ʼ��
    ///
    uint32_t GetChannelID()
    {
        return m_nID;
    }

    ///
    /// @brief
    ///    ����ͨ������
    ///
    void SetChannelType(enumChannelType enChannelType)
    {
        m_enChannelType = enChannelType;
    }

    ///
    /// @brief
    ///    ��ȡͨ������
    ///
    enumChannelType GetChannelType()
    {
        return m_enChannelType;
    }

    ///
    /// @brief
    ///    ����ͨ������
    ///
    void SetChannelDesc(std::wstring strChannelDesc)
    {
        m_strChannelDesc = strChannelDesc;
    }

    ///
    /// @brief
    ///    ��ȡͨ������
    ///
    std::wstring GetChannelDesc()
    {
        return m_strChannelDesc;
    }

    ///
    /// @brief
    ///    ���ø�ͨ��
    ///
    void SetParentChannel(CGooseChannel* pChannel)
    {
        m_pParentChannel = pChannel;
    }

    ///
    /// @brief
    ///    ��ȡ��ͨ��
    ///
    CGooseChannel* GetParentChannel()
    {
        return m_pParentChannel;
    }

    ///
    /// @brief
    ///    ����ͨ������
    ///
    void SetChannelData(GOOSE_DATA* pData)
    {
        if (m_pChannelData)
        {
            GooseDataFree(m_pChannelData);
            m_pChannelData = NULL;
        }

        if (pData->type == GSEDT_STRUCT)
        {
            GOOSE_STRUCT* pGooseStruct = (GOOSE_STRUCT*)pData;
            std::vector<GOOSE_DATA *>::iterator it;
            CGooseChannel stSubChannel;
            uint32_t nSubChannelId = 0;
            for ( it = pGooseStruct->member.begin(); it != pGooseStruct->member.end(); it++, nSubChannelId++ )
            {
                stSubChannel.SetChannelID(nSubChannelId);
                stSubChannel.SetChannelData((*it));
                AddSubChannel(&stSubChannel);
            }
        }

        m_pChannelData = GooseDataCopy(pData);
    }

    ///
    /// @brief
    ///    ��ȡͨ������ָ��
    ///
    GOOSE_DATA* GetChannelData()
    {
        return m_pChannelData;
    }

    ///
    /// @brief
    ///    ���һ����ͨ��
    ///
    bool AddSubChannel(CGooseChannel* pSubChannel);

    ///
    /// @brief
    ///    �Ƴ�һ����ͨ��
    ///
    bool RemoveSubChannel(uint32_t nIndex);

    ///
    /// @brief
    ///    �Ƴ�һ����ͨ��
    ///
    bool RemoveSubChannel(CGooseChannel* pSubChannel);

    ///
    /// @brief
    ///    ��ȡ��ͨ����Ŀ
    ///
    uint32_t GetSubChannelCount();

    ///
    /// @brief
    ///    ��ȡһ����ͨ��
    ///
    CGooseChannel* GetSubChannelData(uint32_t nIndex);

    ///
    /// @brief
    ///    �����ͨ������
    ///
    void ClearSubChannels();

private:
    uint32_t                             m_nID;                         ///< ͨ��id
    enumChannelType                      m_enChannelType;               ///< ͨ������(��scl�ļ��л�ȡ)
    std::wstring                         m_strChannelDesc;              ///< ͨ������(��scl�ļ���ȡ)
    GOOSE_DATA*                          m_pChannelData;                ///< ͨ����������(�ӱ���֡�л�ȡ)
    CGooseChannel*                       m_pParentChannel;              ///< ��ǰͨ���ĸ�ͨ��ָ�룬ΪNULL��ʾû�и�ͨ��
    std::vector<CGooseChannel*>          m_vecSubChannels;              ///< ��ͨ����Ϣ����Ҫ����ͨ�����������ͣ���SCL�ļ���ȡ��
};

///
/// @brief
///    goose���ƿ���Ϣ
///
class CGooseInfo
{
public:
    CGooseInfo(): m_pCtrlBlk(NULL)
        , m_nStNumber(0)
        , m_nSqNumber(0)
        , m_nTTL(0)
    {
        memset(&m_stEventTime, 0, sizeof(m_stEventTime));
    }

    ~CGooseInfo()
    {
        ClearChannelData();
    }

    ///
    /// @brief
    ///    ���ÿ��ƿ�ָ��
    ///
    void SetCtrlBlk(CBaseDetectItem* pItem)
    {
        m_pCtrlBlk = pItem;
    }

    ///
    /// @brief
    ///    ��ȡ���ƿ�ָ��
    ///
    CBaseDetectItem* GetCtrlBlk()
    {
        return m_pCtrlBlk;
    }

    ///
    /// @brief
    ///    ����goose����ʱ��TTLֵ
    ///
    void SetGooseTTL(uint32_t nTTL)
    {
        m_nTTL = nTTL;
    }

    ///
    /// @brief
    ///    ��ȡgoose����ʱ��ֵ
    ///
    uint32_t GetGooseTTL()
    {
        return m_nTTL;
    }

    ///
    /// @brief
    ///    ����goose��λ����
    ///
    void SetGooseStNum(uint32_t nStNumber)
    {
        m_nStNumber = nStNumber;
    }

    ///
    /// @brief
    ///    ��ȡGoose��λ����
    ///
    uint32_t GetGooseStNum()
    {
        return m_nStNumber;
    }

    ///
    /// @brief
    ///    ����goose״̬����
    ///
    void SetGooseSqNum(uint32_t nSqNumber)
    {
        m_nSqNumber = nSqNumber;
    }

    ///
    /// @brief
    ///    ��ȡgoose״̬����
    ///
    uint32_t GetGooseSqNum()
    {
        return m_nSqNumber;
    }

    ///
    /// @brief
    ///    ���һ��ͨ��
    ///
    /// @param[in]      pChannel    Ҫ��ӵ�ͨ������ָ��
    ///
    /// @return
    ///     ��ӳɹ�����true
    ///
    /// @note
    ///     pChannelָ�벻�ᱻֱ����ӽ����ƿ�
    ///     ������������ռ䣬����pChannelָ������ݣ�Ȼ����ӽ����ƿ�
    ///
    bool AddChannel(CGooseChannel* pChannel);

    ///
    /// @brief
    ///    ����ͨ����������0��ʼ���Ƴ�һ��ͨ��
    ///
    bool RemoveChannel(uint32_t nIndex);

    ///
    /// @brief
    ///    ����ͨ����Ϣ�Ƴ�һ��ͨ�����ڲ�����ʹ��ͨ����������ͨ���Ƴ�
    ///
    bool RemoveChannel(CGooseChannel* pChannel);

    ///
    /// @brief
    ///    ���ͨ������
    ///
    void ClearChannelData();

    ///
    /// @brief
    ///    ��ȡͨ����Ŀ
    ///
    uint32_t GetChannelCounts()
    {
        return m_vecChannels.size();
    }

    ///
    /// @brief
    ///    ��ȡһ��ͨ������
    ///
    /// @param[in]      nIndex  ͨ����������0��GetChannelCounts()
    ///
    /// @return
    ///     CGooseChannel* ���ض�Ӧͨ������ָ�룬ʧ�ܷ���NULL
    ///
    CGooseChannel* GetChanneData(uint32_t nIndex);

    ///
    /// @brief
    ///    ����goose��λʱ��
    ///
    void SetEventTime(uint32_t t_s, uint32_t t_ns, uint8_t t_q)
    {
        m_stEventTime.id.type = GSEDT_UTC_TIME;
        m_stEventTime.t_s = t_s;
        m_stEventTime.t_ns = t_ns;
        m_stEventTime.t_q = t_q;
    }

    ///
    /// @brief
    ///    ��ȡgoose��λʱ��
    ///
    GOOSE_UTC_TIME GetEventTime()
    {
        return m_stEventTime;
    }

    ///
    /// @brief
    ///    ����scl�ļ������ͨ������/���͵���Ϣ
    ///
    /// @param[in]  pGooseInfo  Ҫ����gooseInfo��Ϣָ��
    ///
    /// @return
    ///     bool ��scl�ܻ�ȡ��Ϣ�ɹ�����䷵��true
    ///
    static bool FillGooseChannelDescAndType(CGooseInfo* pGooseInfo);


private:
    CBaseDetectItem*                     m_pCtrlBlk;                    ///< ��Ӧ�Ŀ��ƿ�ָ��
    uint32_t                             m_nTTL;                        ///< goose��������ʱ��
    uint32_t                             m_nStNumber;                   ///< goose��λ��������
    uint32_t                             m_nSqNumber;                   ///< gooseͬһ״̬���ݼ���
    GOOSE_UTC_TIME                       m_stEventTime;                 ///< Goose��λʱ��
    std::vector<CGooseChannel*>          m_vecChannels;                 ///< ���ƿ��Ӧ��ͨ����Ϣ
};
