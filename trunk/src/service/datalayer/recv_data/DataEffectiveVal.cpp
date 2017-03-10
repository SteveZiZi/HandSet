/// @file
///
/// @brief
///     一个控制块通道的有效值/相位信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.22
///
/// 修改历史：
///

#include "StdAfx.h"
#include "DataEffectiveVal.h"
#include "src\service\datalayer\include\config.h"

CDataEffectiveVal::CDataEffectiveVal(void)
{
}

CDataEffectiveVal::~CDataEffectiveVal(void)
{
    ClearChannelVec();
}

///
/// @brief
///    清理通道容器数据
///
void CDataEffectiveVal::ClearChannelVec()
{
    std::vector<CEffectiveChanelInfo*>::iterator itr = m_vecChannels.begin();
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
///    获取某一个通道的数据
///
/// @param[in]  nIndex 要获取的通道索引
///
/// @return
///     CEffectiveChanelInfo* 获取的通道信息指针
///
CEffectiveChanelInfo* CDataEffectiveVal::GetOneChannelInfo( uint32_t nIndex )
{
    std::vector<CEffectiveChanelInfo*>::iterator itr = m_vecChannels.begin();
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
///    添加一个通道
///
/// @param[in]  rEffectiveChannelInfo 通道信息
///
/// @return
///     bool 添加成功返回true
///
bool CDataEffectiveVal::AddOneChannelInfo(const CEffectiveChanelInfo& rEffectiveChannelInfo )
{
    if (_IsChannelInVec(rEffectiveChannelInfo) || m_vecChannels.size() >= GetChannelCount())
    {
        return false;
    }
    CEffectiveChanelInfo* pEffectiveChannelInfo = new CEffectiveChanelInfo;
    *pEffectiveChannelInfo = rEffectiveChannelInfo;
    m_vecChannels.push_back(pEffectiveChannelInfo);
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
bool CDataEffectiveVal::_IsChannelInVec(const CEffectiveChanelInfo& rEffectiveChannelInfo )
{
    std::vector<CEffectiveChanelInfo*>::iterator itr = m_vecChannels.begin();
    for (; itr != m_vecChannels.end(); ++itr)
    {
        if (rEffectiveChannelInfo.m_nIndex == (*itr)->m_nIndex)
        {
            return true;
        }
    }
    return false;
}

///
/// @brief
///    删除一个通道
///
/// @param[in]  rEffectiveChannelInfo 通道信息
///
/// @return
///     bool 移除成功返回true
///
bool CDataEffectiveVal::RemoveOneChannelInfo( const CEffectiveChanelInfo& rEffectiveChannelInfo )
{
    if (!_IsChannelInVec(rEffectiveChannelInfo))
    {
        return false;
    }

    std::vector<CEffectiveChanelInfo*>::iterator itr = m_vecChannels.begin();
    for (; itr != m_vecChannels.end(); ++itr)
    {
        if (rEffectiveChannelInfo.m_nIndex == (*itr)->m_nIndex)
        {
            delete (*itr);
            (*itr) = NULL;
            m_vecChannels.erase(itr);
            return true;
        }
    }
    return false;
}

