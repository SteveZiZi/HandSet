/// @file
///
/// @brief
///     ���岨��ͼ��Ҫ������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.24
///
/// �޸���ʷ��
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
///    ��ȡһ��ͨ������Ĳ�������Ŀ
///
uint32_t CDataOscillogram::GetOneChannelSmvCount()
{
    return m_nVecFloatSize;
}

///
/// @brief
///    ��ȡĳһ��ͨ��������
///
/// @param[in]  nIndex Ҫ��ȡ��ͨ������
///
/// @return
///     COscillogramChannelInfo* ��ȡ��ͨ����Ϣָ��
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
///    ���һ��ͨ��
///
/// @param[in]  rEffectiveChannelInfo ͨ����Ϣ
///
/// @return
///     bool ��ӳɹ�����true
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
///    ɾ��һ��ͨ��
///
/// @param[in]  COscillogramChannelInfo ͨ����Ϣ
///
/// @return
///     bool �Ƴ��ɹ�����true
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
///    ����ͨ����������
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
///    �ж�ĳһ��ͨ���Ƿ����
///
/// @param[in]  rEffectiveChannelInfo ͨ����Ϣ
///
/// @return
///     bool �Ѿ����ڷ���true
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
///    �Զ�����N����ͨ��
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

