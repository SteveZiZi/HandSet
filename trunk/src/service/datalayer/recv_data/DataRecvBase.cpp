/// @file
///
/// @brief
///      �������ֵ���ݽ��ջ���
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.23
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "DataRecvBase.h"

CDataRecvBase::CDataRecvBase(void):m_pCtrlBlk(NULL)
{
}

CDataRecvBase::~CDataRecvBase(void)
{
}


///
/// @brief
///    ���ÿ��ƿ�ָ��
///
void CDataRecvBase::SetCtrlBlk( CBaseDetectItem* pCtrlBlk )
{
    m_pCtrlBlk = pCtrlBlk;
}

///
/// @brief
///    ��ȡ�˿��ƿ����ݶ�Ӧ�Ŀ��ƿ�
///
CBaseDetectItem* CDataRecvBase::GetCtrlBlk()
{
    return m_pCtrlBlk;
}

///
/// @brief
///    ��ȡͨ����Ŀ
///
size_t CDataRecvBase::GetChannelCount()
{
    if (m_pCtrlBlk)
    {
        return m_pCtrlBlk->nChannelCount;
    }
    return 0;
}


///
/// @brief
///    ��ȡͨ������
///
enumChannelType CDataRecvBase::GetChannelType( uint32_t nIndex )
{
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        CSmvRecvChannelInfo* pChannelInfo = m_pCtrlBlk->pRecvConfig->GetOneChannelInfo(nIndex);
        if (pChannelInfo)
        {
            return pChannelInfo->enChannelType;
        }
    }
    return CHANNEL_TYPE_UNKOWN;
}

///
/// @brief
///    ��ȡͨ�����
///
ChannelPhase CDataRecvBase::GetChannelPhase( uint32_t nIndex )
{
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        CSmvRecvChannelInfo* pChannelInfo = m_pCtrlBlk->pRecvConfig->GetOneChannelInfo(nIndex);
        if (pChannelInfo)
        {
            return pChannelInfo->enChannelPhase;
        }
    }
    return CHANNEL_PHASE_UNKOWN;
}

///
/// @brief
///    ����ĳһ��ͨ���ĵ�ѹ/��������
///
/// @param[in]  nIndex Ҫ���õ�ͨ����
/// @param[in]  enChannelType   Ҫ�趨��ͨ������
///
/// @return
/// bool    true��ʾ���óɹ�
///
bool CDataRecvBase::SetChannelType( uint32_t nIndex, enumChannelType enChannelType )
{
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        CSmvRecvChannelInfo* pChannelInfo = m_pCtrlBlk->pRecvConfig->GetOneChannelInfo(nIndex);
        if (pChannelInfo)
        {
            pChannelInfo->enChannelType = enChannelType;
            return true;
        }
    }
    return false;
}

///
/// @brief
///    ����ĳһ��ͨ�������
///
/// @param[in]  nIndex Ҫ���õ�ͨ����
/// @param[in]  enChannelPhase   Ҫ�趨��ͨ�����
///
/// @return
/// bool    true��ʾ���óɹ�
///
bool CDataRecvBase::SetChannelPhase( uint32_t nIndex, ChannelPhase enChannelPhase )
{
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        CSmvRecvChannelInfo* pChannelInfo = m_pCtrlBlk->pRecvConfig->GetOneChannelInfo(nIndex);
        if (pChannelInfo)
        {
            pChannelInfo->enChannelPhase = enChannelPhase;
            return true;
        }
    }
    return false;
}

///
/// @brief
///    ��ȡͨ������
///
std::wstring CDataRecvBase::GetChannelDesc( uint32_t nIndex )
{
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        CSmvRecvChannelInfo* pChannelInfo = m_pCtrlBlk->pRecvConfig->GetOneChannelInfo(nIndex);
        if (pChannelInfo)
        {
            return pChannelInfo->strChannelDesc;
        }
    }
    return L"";
}