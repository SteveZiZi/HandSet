/// @file
///
/// @brief
///     ��������������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.24
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "DataXuliang.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\DsmDataLayer.h"


CDataXuliang::CDataXuliang(void) : m_enCurrentType(CHANNEL_TYPE_SMV_VOLTAGE)
{
    memset(m_fPhaseSeqContent, 0, sizeof(m_fPhaseSeqContent));
    memset(m_fPhaseSeqAngle, 0, sizeof(m_fPhaseSeqAngle));
}

CDataXuliang::~CDataXuliang(void)
{
}

///
/// @brief
///    ��ȡ�������򡢸�����ֵ
///
/// @param[out] fZeroPhaseSeqContent   ��ȡ������ֵ
/// @param[out] fPosPhaseSeqContent    ��ȡ������ֵ
/// @param[out] fNegPhaseSeqContent    ��ȡ������ֵ
///
void CDataXuliang::GetPhaseSeqContent( float& fZeroPhaseSeqContent, float& fPosPhaseSeqContent, float& fNegPhaseSeqContent )
{
    if (CHANNEL_TYPE_SMV_VOLTAGE == m_enCurrentType)
    {
        fZeroPhaseSeqContent = m_fPhaseSeqContent[0][0];
        fPosPhaseSeqContent  = m_fPhaseSeqContent[0][1];
        fNegPhaseSeqContent  = m_fPhaseSeqContent[0][2];
    }
    else if (CHANNEL_TYPE_SMV_CURRENT == m_enCurrentType)
    {
        fZeroPhaseSeqContent = m_fPhaseSeqContent[1][0];
        fPosPhaseSeqContent  = m_fPhaseSeqContent[1][1];
        fNegPhaseSeqContent  = m_fPhaseSeqContent[1][2];
    }
}

///
/// @brief
///    ��ȡ�������򡢸���Ƕ�
///
/// @param[out] fZeroPhaseSeqAngle   ��ȡ����Ƕ�
/// @param[out] fPosPhaseSeqAngle    ��ȡ����Ƕ�
/// @param[out] fNegPhaseSeqAngle    ��ȡ����Ƕ�
///
void CDataXuliang::GetPhaseSeqAngle( float& fZeroPhaseSeqAngle, float& fPosPhaseSeqAngle, float& fNegPhaseSeqAngle )
{
    if (CHANNEL_TYPE_SMV_VOLTAGE == m_enCurrentType)
    {
        fZeroPhaseSeqAngle = m_fPhaseSeqAngle[0][0];
        fPosPhaseSeqAngle  = m_fPhaseSeqAngle[0][1];
        fNegPhaseSeqAngle  = m_fPhaseSeqAngle[0][2];
    }
    else if (CHANNEL_TYPE_SMV_CURRENT == m_enCurrentType)
    {
        fZeroPhaseSeqAngle = m_fPhaseSeqAngle[1][0];
        fPosPhaseSeqAngle  = m_fPhaseSeqAngle[1][1];
        fNegPhaseSeqAngle  = m_fPhaseSeqAngle[1][2];
    }
}

///
/// @brief
///    ��ȡ�������ͨ��
///
/// @param[out]  nPhaseA ����A��ͨ����
/// @param[out]  nPhaseB ����B��ͨ����
/// @param[out]  nPhaseC ����C��ͨ����
///
/// return
///     bool    ��ȡ�ɹ�����true
///
bool CDataXuliang::GetABCChannles( uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC)
{
    CBaseDetectItem* pCurrentItem = CDsmDataLayer::getInstance()->GetCurrentCtrlBlk();
    if (!m_pCtrlBlk || m_pCtrlBlk->nID != pCurrentItem->nID)
    {
        SetCtrlBlk(pCurrentItem);
    }
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        CSmvRecvConfig* pSmvRecvConfig = m_pCtrlBlk->pRecvConfig;
        if (CHANNEL_TYPE_SMV_VOLTAGE == m_enCurrentType)
        {
            return pSmvRecvConfig->GetDspSendVolInfo(nPhaseA, nPhaseB, nPhaseC);
        }
        else if (CHANNEL_TYPE_SMV_CURRENT == m_enCurrentType)
        {
            return pSmvRecvConfig->GetDspSendCurInfo(nPhaseA, nPhaseB, nPhaseC);
        }
    }
    return false;
}


///
/// @brief
///    ���������ѹ/����ͨ��
///
/// @param[in]  nPhaseA A��ͨ����
/// @param[in]  nPhaseB B��ͨ����
/// @param[in]  nPhaseC C��ͨ����
///
/// return
///     bool    ���óɹ�����true
///
bool CDataXuliang::SetABCChannles( uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC )
{
    CBaseDetectItem* pCurrentItem = CDsmDataLayer::getInstance()->GetCurrentCtrlBlk();
    if (!m_pCtrlBlk || m_pCtrlBlk->nID != pCurrentItem->nID)
    {
        SetCtrlBlk(pCurrentItem);
    }
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        CSmvRecvConfig* pSmvRecvConfig = m_pCtrlBlk->pRecvConfig;
        if (CHANNEL_TYPE_SMV_VOLTAGE == m_enCurrentType)
        {
            pSmvRecvConfig->SetDspSendCurInfo(0xFF, 0xFF, 0xFF);
            return pSmvRecvConfig->SetDspSendVolInfo(nPhaseA, nPhaseB, nPhaseC);
        }
        else if (CHANNEL_TYPE_SMV_CURRENT == m_enCurrentType)
        {
            pSmvRecvConfig->SetDspSendVolInfo(0xFF, 0xFF, 0xFF);
            return pSmvRecvConfig->SetDspSendCurInfo(nPhaseA, nPhaseB, nPhaseC);
        }
    }
    return false;
}


///
/// @brief
///    �����������򡢸�����ֵ
///
/// @param[in] enChannelType          ָ�����õ��ǵ�ѹ���ǵ���������ֵ
/// @param[in] fZeroPhaseSeqContent   ����������ֵ
/// @param[in] fPosPhaseSeqContent    ����������ֵ
/// @param[in] fNegPhaseSeqContent    ���ø�����ֵ
///
void CDataXuliang::SetPhaseSeqContent( enumChannelType enChannelType, float& fZeroPhaseSeqContent, float& fPosPhaseSeqContent, float& fNegPhaseSeqContent )
{
    if (CHANNEL_TYPE_SMV_VOLTAGE == enChannelType)
    {
        m_fPhaseSeqContent[0][0] = fZeroPhaseSeqContent;
        m_fPhaseSeqContent[0][1] = fPosPhaseSeqContent;
        m_fPhaseSeqContent[0][2] = fNegPhaseSeqContent;
    }
    else if (CHANNEL_TYPE_SMV_CURRENT == enChannelType)
    {
        m_fPhaseSeqContent[1][0] = fZeroPhaseSeqContent;
        m_fPhaseSeqContent[1][1] = fPosPhaseSeqContent;
        m_fPhaseSeqContent[1][2] = fNegPhaseSeqContent;
    }
}

///
/// @brief
///    �����������򡢸���Ƕ�
///
/// @param[in] enChannelType        ָ�����õ��ǵ�ѹ���ǵ����������Ƕ�
/// @param[in] fZeroPhaseSeqAngle   ��������Ƕ�
/// @param[in] fPosPhaseSeqAngle    ��������Ƕ�
/// @param[in] fNegPhaseSeqAngle    ���ø���Ƕ�
///
void CDataXuliang::SetPhaseSeqAngle( enumChannelType enChannelType, float& fZeroPhaseSeqAngle, float& fPosPhaseSeqAngle, float& fNegPhaseSeqAngle )
{
    if (CHANNEL_TYPE_SMV_VOLTAGE == enChannelType)
    {
        m_fPhaseSeqAngle[0][0] = fZeroPhaseSeqAngle;
        m_fPhaseSeqAngle[0][1] = fPosPhaseSeqAngle;
        m_fPhaseSeqAngle[0][2] = fNegPhaseSeqAngle;
    }
    else if (CHANNEL_TYPE_SMV_CURRENT == enChannelType)
    {
        m_fPhaseSeqAngle[1][0] = fZeroPhaseSeqAngle;
        m_fPhaseSeqAngle[1][1] = fPosPhaseSeqAngle;
        m_fPhaseSeqAngle[1][2] = fNegPhaseSeqAngle;
    }
}

///
/// @brief
///    ���õ�ǰ������ѹ����ģʽ
///
void CDataXuliang::SetXLMode( enumChannelType enChannelType )
{
    m_enCurrentType = enChannelType;
}

///
/// @brief
///    ��ȡ��ǰ�����ĵ�ѹ����ģʽ
///
enumChannelType CDataXuliang::GetXLMode()
{
    return m_enCurrentType;
}

///
/// @brief
///    ���������Ϣ
///
void CDataXuliang::ClearXLData()
{
    memset(m_fPhaseSeqContent, 0, sizeof(m_fPhaseSeqContent));
    memset(m_fPhaseSeqAngle, 0, sizeof(m_fPhaseSeqAngle));
}
