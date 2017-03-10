/// @file
///
/// @brief
///     定义波形图需要的数据
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.24
///
/// 修改历史：
///

#include "StdAfx.h"
#include "DataOscillogram.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\config\SmvRecvConfig.h"

CDataOscillogram::CDataOscillogram(void) : m_nVecFloatSize(0)
{
}

CDataOscillogram::~CDataOscillogram(void)
{
    ClearChannelVec();
}

///
/// @brief
///    获取一个通道保存的采样点数目
///
uint32_t CDataOscillogram::GetOneChannelSmvCount()
{
    return m_nVecFloatSize;
}

///
/// @brief
///    获取某一个通道的数据
///
/// @param[in]  nIndex 要获取的通道索引
///
/// @return
///     COscillogramChannelInfo* 获取的通道信息指针
///
COscillogramChannelInfo* CDataOscillogram::GetOneChannelInfo( uint32_t nIndex )
{
    if(nIndex < m_vecChannels.size())
    {
        std::vector<COscillogramChannelInfo*>::iterator itr = m_vecChannels.begin();
        for (; itr != m_vecChannels.end(); ++itr)
        {
            if ((*itr)->m_nIndex == nIndex)
            {
                return (*itr);
            }
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
bool CDataOscillogram::AddOneChannelInfo( COscillogramChannelInfo& rChannelInfo )
{
    if (_IsChannelInVec(rChannelInfo) || m_vecChannels.size() >= GetChannelCount())
    {
        return false;
    }
    COscillogramChannelInfo* pChannelInfo = new COscillogramChannelInfo;
    pChannelInfo->m_nIndex          = rChannelInfo.m_nIndex;
    pChannelInfo->m_bChannelValid   = rChannelInfo.m_bChannelValid;
    pChannelInfo->m_fEffectiveValue = rChannelInfo.m_fEffectiveValue;
    pChannelInfo->m_fPhase          = rChannelInfo.m_fPhase;
    pChannelInfo->m_vecChannelData.swap(rChannelInfo.m_vecChannelData);
//    pChannelInfo->m_vecChannelData.assign(rChannelInfo.m_vecChannelData.begin(), rChannelInfo.m_vecChannelData.end());
    m_nVecFloatSize = pChannelInfo->m_vecChannelData.size();
    m_vecChannels.push_back(pChannelInfo);
    return true;
}

///
/// @brief
///    删除一个通道
///
/// @param[in]  COscillogramChannelInfo 通道信息
///
/// @return
///     bool 移除成功返回true
///
bool CDataOscillogram::RemoveOneChannelInfo( const COscillogramChannelInfo& rChannelInfo )
{
    if (!_IsChannelInVec(rChannelInfo))
    {
        return false;
    }

    std::vector<COscillogramChannelInfo*>::iterator itr = m_vecChannels.begin();
    for (; itr != m_vecChannels.end(); ++itr)
    {
        if (rChannelInfo.m_nIndex == (*itr)->m_nIndex)
        {
            delete (*itr);
            (*itr) = NULL;
            m_vecChannels.erase(itr);
            return true;
        }
    }
    return false;
}

///
/// @brief
///    清理通道容器数据
///
void CDataOscillogram::ClearChannelVec()
{
    std::vector<COscillogramChannelInfo*>::iterator itr = m_vecChannels.begin();
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
///    判断某一个通道是否存在
///
/// @param[in]  rEffectiveChannelInfo 通道信息
///
/// @return
///     bool 已经存在返回true
///
bool CDataOscillogram::_IsChannelInVec( const COscillogramChannelInfo& rChannelInfo )
{
    std::vector<COscillogramChannelInfo*>::iterator itr = m_vecChannels.begin();
    for (; itr != m_vecChannels.end(); ++itr)
    {
        if ((*itr)->m_nIndex == rChannelInfo.m_nIndex)
        {
            return true;
        }
    }
    return false;
}

///
/// @brief
///    自动生成N个空通道
///
void CDataOscillogram::GenNChannels( uint32_t nChannelCount ,uint32_t nVecReserveSize)
{
    if (m_vecChannels.size() > 0)
    {
        return;
    }
    for (uint32_t i = 0; i < nChannelCount; ++i)
    {
        COscillogramChannelInfo *pChannel = new COscillogramChannelInfo();
        pChannel->m_nIndex= i;
        pChannel->m_vecChannelData.reserve(nVecReserveSize);
        m_vecChannels.push_back(pChannel);
    }
}

