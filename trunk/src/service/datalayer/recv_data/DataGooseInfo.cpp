/// @file
///
/// @brief
///     描述goose控制块信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.6.6
///
/// 修改历史：
///

#include "StdAfx.h"
#include "DataGooseInfo.h"
#include "src\service\datalayer\DsmDataLayer.h"

///
/// @brief
///    添加一个通道
///
/// @param[in]      pChannel    要添加的通道数据指针
///
/// @return
///     添加成功返回true
///
/// @note
///     pChannel指针不会被直接添加进控制块
///     函数会新申请空间，拷贝pChannel指向的数据，然后添加进控制块
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
///    根据通道索引（从0开始）移除一个通道
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
///    根据通道信息移除一个通道，内部还是使用通道索引进行通道移除
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
///    清空通道数据
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
///    获取一个通道数据
///
/// @param[in]      nIndex  通道索引，从0到GetChannelCounts()
///
/// @return
///     CGooseChannel* 返回对应通道数据指针，失败返回NULL
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
///    根据scl文件，填充通道描述/类型等信息
///
/// @param[in]  pGooseInfo  要填充的gooseInfo信息指针
///
/// @return
///     bool 从scl总获取信息成功并填充返回true
///
bool CGooseInfo::FillGooseChannelDescAndType( CGooseInfo* pGooseInfo )
{
    if (!pGooseInfo)
    {
        return false;
    }

    CSCLManager* pSclMgr = CDsmDataLayer::getInstance()->GetSclManager();
    CNetDetectItem* pNetItem = dynamic_cast<CNetDetectItem*>(pGooseInfo->GetCtrlBlk());

    //在报文处理过程中只添加了报文数据。
    //通道数据，通道描述，类型需要从scl文件获取的信息从scl中获取
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
///    添加一个子通道
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
///    移除一个子通道
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
///    移除一个子通道
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
///    获取子通道数目
///
uint32_t CGooseChannel::GetSubChannelCount()
{
    return m_vecSubChannels.size();
}

///
/// @brief
///    获取一个子通道
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
///    清空子通道数据
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
