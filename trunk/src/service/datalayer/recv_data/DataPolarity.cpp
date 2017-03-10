/// @file
///
/// @brief
///     定义双ad数据信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.18
///
/// 修改历史：
///

#include "StdAfx.h"
#include "DataPolarity.h"

CDataPolarity::CDataPolarity(void)
{
}

CDataPolarity::~CDataPolarity(void)
{
    ClearChannelInfo();
}

///
/// @brief
///    获取通道的极性信息
///
/// @param[in]      nIndex  通道序号，从0开始
///
/// @return
///     int32_t     返回极性信息，大于0表示正极性，小于0表示反极性
///
int32_t CDataPolarity::GetChannelPolarity( uint32_t nIndex )
{
    CDataPolarityChannelInfo* pChannelInfo = GetOneChannelInfo(nIndex);
    if (pChannelInfo)
    {
        return pChannelInfo->m_nPolarity;
    }
    return 0;
}


///
/// @brief
///    添加一个通道
///
/// @param[in]  rEffectiveChannelInfo 通道信息
///
/// @return
///     bool 添加成功返回true
///
bool CDataPolarity::AddOneChannelInfo( const CDataPolarityChannelInfo& rChannelInfo )
{
    if (_IsChannelInVec(rChannelInfo) || m_vecChannels.size() >= GetChannelCount())
    {
        return false;
    }
    CDataPolarityChannelInfo* pChannelInfo = new CDataPolarityChannelInfo;
    *pChannelInfo = rChannelInfo;
    m_vecChannels.push_back(pChannelInfo);
    return true;

}

///
/// @brief
///    判断某一个通道是否存在
///
/// @param[in]  rEffectiveChannelInfo 通道信息
///
/// @return
///     bool 已经存在返回true
///
bool CDataPolarity::_IsChannelInVec( const CDataPolarityChannelInfo& rChannelInfo )
{
    std::vector<CDataPolarityChannelInfo*>::iterator itr = m_vecChannels.begin();
    for (; itr != m_vecChannels.end(); ++itr)
    {
        if (rChannelInfo.m_nIndex == (*itr)->m_nIndex)
        {
            return true;
        }
    }
    return false;

}

///
/// @brief
///    获取某一个通道的数据
///
/// @param[in]  nIndex 要获取的通道索引
///
/// @return
///     CEffectiveChanelInfo* 获取的通道信息指针
///
CDataPolarityChannelInfo* CDataPolarity::GetOneChannelInfo( uint32_t nIndex )
{
    std::vector<CDataPolarityChannelInfo*>::iterator itr = m_vecChannels.begin();
    for (; itr != m_vecChannels.end(); ++itr)
    {
        if ((*itr)->m_nIndex == nIndex)
        {
            return (*itr);
        }
    }
    return NULL;
}

///
/// @brief
///    清空所有通道信息
///
void CDataPolarity::ClearChannelInfo()
{
    std::vector<CDataPolarityChannelInfo*>::iterator itr = m_vecChannels.begin();
    for (; itr != m_vecChannels.end(); ++itr)
    {
        if (NULL != (*itr))
        {
            delete (*itr);
            (*itr) = NULL;
        }
    }
    m_vecChannels.clear();
}

///
/// @brief
///    设置通道的极性信息
///
/// @param[in]      nIndex  通道序号，从0开始
/// @param[in]      nPorlarity  通道的极性信息
///
/// @return
///     int32_t     设置极性信息，设置成功赶回true
///
bool CDataPolarity::SetChannelPolarity( uint32_t nIndex, int32_t nPorlarity )
{
    std::vector<CDataPolarityChannelInfo*>::iterator itr = m_vecChannels.begin();
    for (; itr != m_vecChannels.end(); ++itr)
    {
        if ((*itr)->m_nIndex == nIndex)
        {
            (*itr)->m_nPolarity = nPorlarity;
            return true;
        }
    }
    return false;
}

