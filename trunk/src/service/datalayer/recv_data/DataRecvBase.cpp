/// @file
///
/// @brief
///      定义采样值数据接收基类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.23
///
/// 修改历史：
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
///    设置控制块指针
///
void CDataRecvBase::SetCtrlBlk( CBaseDetectItem* pCtrlBlk )
{
    m_pCtrlBlk = pCtrlBlk;
}

///
/// @brief
///    获取此控制块数据对应的控制块
///
CBaseDetectItem* CDataRecvBase::GetCtrlBlk()
{
    return m_pCtrlBlk;
}

///
/// @brief
///    获取通道数目
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
///    获取通道类型
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
///    获取通道相别
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
///    设置某一个通道的电压/电流类型
///
/// @param[in]  nIndex 要设置的通道号
/// @param[in]  enChannelType   要设定的通道类型
///
/// @return
/// bool    true表示设置成功
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
///    设置某一个通道的相别
///
/// @param[in]  nIndex 要设置的通道号
/// @param[in]  enChannelPhase   要设定的通道相别
///
/// @return
/// bool    true表示设置成功
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
///    获取通道描述
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