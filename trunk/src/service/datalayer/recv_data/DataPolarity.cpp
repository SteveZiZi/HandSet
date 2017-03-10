/// @file
///
/// @brief
///     ����˫ad������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.18
///
/// �޸���ʷ��
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
///    ��ȡͨ���ļ�����Ϣ
///
/// @param[in]      nIndex  ͨ����ţ���0��ʼ
///
/// @return
///     int32_t     ���ؼ�����Ϣ������0��ʾ�����ԣ�С��0��ʾ������
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
///    ���һ��ͨ��
///
/// @param[in]  rEffectiveChannelInfo ͨ����Ϣ
///
/// @return
///     bool ��ӳɹ�����true
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
///    �ж�ĳһ��ͨ���Ƿ����
///
/// @param[in]  rEffectiveChannelInfo ͨ����Ϣ
///
/// @return
///     bool �Ѿ����ڷ���true
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
///    ��ȡĳһ��ͨ��������
///
/// @param[in]  nIndex Ҫ��ȡ��ͨ������
///
/// @return
///     CEffectiveChanelInfo* ��ȡ��ͨ����Ϣָ��
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
///    �������ͨ����Ϣ
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
///    ����ͨ���ļ�����Ϣ
///
/// @param[in]      nIndex  ͨ����ţ���0��ʼ
/// @param[in]      nPorlarity  ͨ���ļ�����Ϣ
///
/// @return
///     int32_t     ���ü�����Ϣ�����óɹ��ϻ�true
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

