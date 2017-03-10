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

#include "StdAfx.h"
#include "DataGooseInfo.h"
#include "src\service\datalayer\DsmDataLayer.h"

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
bool CGooseInfo::AddChannel( CGooseChannel* pChannel )
{
    if (pChannel)
    {
        CGooseChannel* pNewChannel = new CGooseChannel();
        pNewChannel->SetChannelID(pChannel->GetChannelID());
        pNewChannel->SetChannelDesc(pChannel->GetChannelDesc());
        pNewChannel->SetChannelType(pChannel->GetChannelType());
        pNewChannel->SetParentChannel(NULL);
        pNewChannel->SetChannelData(pChannel->GetChannelData());
        m_vecChannels.push_back(pNewChannel);
        return true;
    }
    return false;
}

///
/// @brief
///    ����ͨ����������0��ʼ���Ƴ�һ��ͨ��
///
bool CGooseInfo::RemoveChannel( uint32_t nIndex )
{
    if (nIndex < m_vecChannels.size())
    {
        std::vector<CGooseChannel*>::iterator itr = m_vecChannels.begin();
        while(itr != m_vecChannels.end())
        {
            if (nIndex == (*itr)->GetChannelID())
            {
                delete (*itr);
                (*itr) = NULL;
                m_vecChannels.erase(itr);
                return true;
            }
            ++itr;
        }
    }
    return false;
}

///
/// @brief
///    ����ͨ����Ϣ�Ƴ�һ��ͨ�����ڲ�����ʹ��ͨ����������ͨ���Ƴ�
///
bool CGooseInfo::RemoveChannel( CGooseChannel* pChannel )
{
    if (pChannel)
    {
        return RemoveChannel(pChannel->GetChannelID());
    }
    return false;
}

///
/// @brief
///    ���ͨ������
///
void CGooseInfo::ClearChannelData()
{
    std::vector<CGooseChannel*>::iterator itr = m_vecChannels.begin();
    while(itr != m_vecChannels.end())
    {
        if (NULL != (*itr))
        {
            delete (*itr);
            (*itr) = NULL;
        }
        ++itr;
    }
    m_vecChannels.clear();
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
CGooseChannel* CGooseInfo::GetChanneData( uint32_t nIndex )
{
    if (nIndex < m_vecChannels.size())
    {
        std::vector<CGooseChannel*>::iterator itr = m_vecChannels.begin();
        if (itr != m_vecChannels.end())
        {
            advance(itr, nIndex);
            if (itr != m_vecChannels.end())
            {
                return (*itr);
            }
        }
    }
    return NULL;
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
bool CGooseInfo::FillGooseChannelDescAndType( CGooseInfo* pGooseInfo )
{
    if (!pGooseInfo)
    {
        return false;
    }

    CSCLManager* pSclMgr = CDsmDataLayer::getInstance()->GetSclManager();
    CNetDetectItem* pNetItem = dynamic_cast<CNetDetectItem*>(pGooseInfo->GetCtrlBlk());

    //�ڱ��Ĵ��������ֻ����˱������ݡ�
    //ͨ�����ݣ�ͨ��������������Ҫ��scl�ļ���ȡ����Ϣ��scl�л�ȡ
    if (pSclMgr && pNetItem)
    {
        CGooseCb* pGooseCb = pSclMgr->GetGooseCb(pNetItem->appId);
        uint32_t nChannelCounts = pGooseInfo->GetChannelCounts();
        if (pGooseCb)
        {
            if (pGooseCb->GetChannelCount() == nChannelCounts)
            {
                for (uint32_t i = 0; i < nChannelCounts; ++i)
                {
                    CGooseChannel* pChannel = pGooseInfo->GetChanneData(i);
                    CChannel* pSclChannel = pGooseCb->GetChannelByIndex(i);

                    if (pChannel && pSclChannel)
                    {
                        pChannel->SetChannelType(pSclChannel->GetChannelType());
                        pChannel->SetChannelDesc(pSclChannel->GetDescription());
                        int nSubChannelCount = pSclChannel->GetSubChannelCount();
                        if (nSubChannelCount > 0 && nSubChannelCount == pChannel->GetSubChannelCount())
                        {
                            CChannel* pSclSubChannel = pSclChannel->FirstSubChannel();
                            uint32_t nSubChannelIndex = 0;
                            CGooseChannel* pSubChannel = pChannel->GetSubChannelData(nSubChannelIndex);
                            while (NULL != pSclSubChannel && NULL != pSubChannel)
                            {
                                pSubChannel->SetChannelDesc(pSclSubChannel->GetDescription());
                                pSubChannel->SetChannelType(pSclSubChannel->GetChannelType());
                                pSclSubChannel = pSclChannel->NextSubChannel(pSclSubChannel);
                                pSubChannel = pChannel->GetSubChannelData(++nSubChannelIndex);
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}


///
/// @brief
///    ���һ����ͨ��
///
bool CGooseChannel::AddSubChannel( CGooseChannel* pSubChannel )
{
    if (pSubChannel)
    {
        CGooseChannel* pNewChannel = new CGooseChannel();
        pNewChannel->SetChannelID(pSubChannel->GetChannelID());
        pNewChannel->SetChannelDesc(pSubChannel->GetChannelDesc());
        pNewChannel->SetChannelType(pSubChannel->GetChannelType());
        pNewChannel->SetParentChannel(this);
        pNewChannel->SetChannelData(pSubChannel->GetChannelData());
        m_vecSubChannels.push_back(pNewChannel);
        return true;
    }
    return false;
}

///
/// @brief
///    �Ƴ�һ����ͨ��
///
bool CGooseChannel::RemoveSubChannel( uint32_t nIndex )
{
    if (nIndex < m_vecSubChannels.size())
    {
        std::vector<CGooseChannel*>::iterator itr = m_vecSubChannels.begin();
        while(itr != m_vecSubChannels.end())
        {
            if (nIndex == (*itr)->GetChannelID())
            {
                delete (*itr);
                (*itr) = NULL;
                m_vecSubChannels.erase(itr);
                return true;
            }
            ++itr;
        }
    }
    return false;
}

///
/// @brief
///    �Ƴ�һ����ͨ��
///
bool CGooseChannel::RemoveSubChannel( CGooseChannel* pSubChannel )
{
    if (pSubChannel)
    {
        return RemoveSubChannel(pSubChannel->GetChannelID());
    }
    return false;
}

///
/// @brief
///    ��ȡ��ͨ����Ŀ
///
uint32_t CGooseChannel::GetSubChannelCount()
{
    return m_vecSubChannels.size();
}

///
/// @brief
///    ��ȡһ����ͨ��
///
CGooseChannel* CGooseChannel::GetSubChannelData( uint32_t nIndex )
{
    if (nIndex < m_vecSubChannels.size())
    {
        std::vector<CGooseChannel*>::iterator itr = m_vecSubChannels.begin();
        if (itr != m_vecSubChannels.end())
        {
            advance(itr, nIndex);
            if (itr != m_vecSubChannels.end())
            {
                return (*itr);
            }
        }
    }
    return NULL;
}

///
/// @brief
///    �����ͨ������
///
void CGooseChannel::ClearSubChannels()
{
    std::vector<CGooseChannel*>::iterator itr = m_vecSubChannels.begin();
    while(itr != m_vecSubChannels.end())
    {
        if (NULL != (*itr))
        {
            delete (*itr);
            (*itr) = NULL;
        }
        ++itr;
    }
    m_vecSubChannels.clear();
}
