/// @file
///
/// @brief
///     һ�����ƿ�ͨ������Чֵ/��λ��Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.22
///
/// �޸���ʷ��
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
///    ����ͨ����������
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
///    ��ȡĳһ��ͨ��������
///
/// @param[in]  nIndex Ҫ��ȡ��ͨ������
///
/// @return
///     CEffectiveChanelInfo* ��ȡ��ͨ����Ϣָ��
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
///    ���һ��ͨ��
///
/// @param[in]  rEffectiveChannelInfo ͨ����Ϣ
///
/// @return
///     bool ��ӳɹ�����true
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
///    �ж�ĳһ��ͨ���Ƿ����
///
/// @param[in]  rEffectiveChannelInfo ͨ����Ϣ
///
/// @return
///     bool �Ѿ����ڷ���true
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
///    ɾ��һ��ͨ��
///
/// @param[in]  rEffectiveChannelInfo ͨ����Ϣ
///
/// @return
///     bool �Ƴ��ɹ�����true
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

