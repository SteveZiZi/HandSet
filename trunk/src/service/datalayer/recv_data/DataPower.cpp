/// @file
///
/// @brief
///     ���幦��������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.23
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "DataPower.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\DsmDataLayer.h"

CDataPower::CDataPower(void): m_fTotalActivePower(0)
                            , m_fTotalApparentPower(0)
                            , m_fTotalReactivePower(0)
                            , m_fTotalPowerFactor(0)
{
    memset(m_fActivePower, 0, sizeof(m_fActivePower));
    memset(m_fReactivePower, 0, sizeof(m_fReactivePower));
    memset(m_fApparentPower, 0, sizeof(m_fApparentPower));
    memset(m_fPowerFactor, 0, sizeof(m_fPowerFactor));
    init();
}

CDataPower::~CDataPower(void)
{
}

///
/// @brief
///    ��ȡ�й�����
///
/// @param[out]  fPhaseAData        ����A���й�����
/// @param[out]  fPhaseBData        ����B���й�����
/// @param[out]  fPhaseCData        ����C���й�����
/// @param[out]  fTotalData         �������й�����
///
/// @return
///
void CDataPower::GetActivePower( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData)
{
    fPhaseAData = m_fActivePower[0];
    fPhaseBData = m_fActivePower[1];
    fPhaseCData = m_fActivePower[2];
    fTotalData  = m_fTotalActivePower;
}

///
/// @brief
///    ��ȡ�޹�����
///
/// @param[out]  fPhaseAData        ����A���޹�����
/// @param[out]  fPhaseBData        ����B���޹�����
/// @param[out]  fPhaseCData        ����C���޹�����
/// @param[out]  fTotalData         �������޹�����
///
/// @return
///
void CDataPower::GetReactivePower( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData)
{
    fPhaseAData = m_fReactivePower[0];
    fPhaseBData = m_fReactivePower[1];
    fPhaseCData = m_fReactivePower[2];
    fTotalData  = m_fTotalReactivePower;
}

///
/// @brief
///    ��ȡ���ڹ���
///
/// @param[out]  fPhaseAData        ����A�����ڹ���
/// @param[out]  fPhaseBData        ����B�����ڹ���
/// @param[out]  fPhaseCData        ����C�����ڹ���
/// @param[out]  fTotalData         ���������ڹ���
///
/// @return
///
void CDataPower::GetApparentPower( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData)
{
    fPhaseAData = m_fApparentPower[0];
    fPhaseBData = m_fApparentPower[1];
    fPhaseCData = m_fApparentPower[2];
    fTotalData  = m_fTotalApparentPower;
}

///
/// @brief
///    ��ȡ��������
///
/// @param[out]  fPhaseAData        ����A�๦������
/// @param[out]  fPhaseBData        ����B�๦������
/// @param[out]  fPhaseCData        ����C�๦������
/// @param[out]  fTotalData         �����ܹ�������
///
/// @return
///
void CDataPower::GetPowerFactor( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData)
{
    fPhaseAData = m_fPowerFactor[0];
    fPhaseBData = m_fPowerFactor[1];
    fPhaseCData = m_fPowerFactor[2];
    fTotalData  = m_fTotalPowerFactor;
}

///
/// @brief
///    ���������ѹͨ��
///
/// @param[in]  nVolPhaseA A���ѹͨ����
/// @param[in]  nVolPhaseB B���ѹͨ����
/// @param[in]  nVolPhaseC C���ѹͨ����
///
bool CDataPower::SetVoltageChannles( uint32_t nVolPhaseA, uint32_t nVolPhaseB, uint32_t nVolPhaseC )
{
    CBaseDetectItem* pCurrentItem = CDsmDataLayer::getInstance()->GetCurrentCtrlBlk();
    if (!m_pCtrlBlk || m_pCtrlBlk->nID != pCurrentItem->nID)
    {
        SetCtrlBlk(pCurrentItem);
    }
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        return m_pCtrlBlk->pRecvConfig->SetDspSendVolInfo(nVolPhaseA, nVolPhaseB, nVolPhaseC);
    }
    return false;
}

///
/// @brief
///    ��ȡ�����ѹͨ��
///
/// @param[out]  nVolPhaseA ����A���ѹͨ����
/// @param[out]  nVolPhaseB ����B���ѹͨ����
/// @param[out]  nVolPhaseC ����C���ѹͨ����
///
bool CDataPower::GetVoltageChannles( uint32_t& nVolPhaseA, uint32_t& nVolPhaseB, uint32_t& nVolPhaseC )
{
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        return m_pCtrlBlk->pRecvConfig->GetDspSendVolInfo(nVolPhaseA, nVolPhaseB, nVolPhaseC);
    }
    return false;
}

///
/// @brief
///    �����������ͨ��
///
/// @param[in]  nCurPhaseA A�����ͨ����
/// @param[in]  nCurPhaseB B�����ͨ����
/// @param[in]  nCurPhaseC C�����ͨ����
///
bool CDataPower::SetCurrentChannles( uint32_t nCurPhaseA, uint32_t nCurPhaseB, uint32_t nCurPhaseC )
{
    CBaseDetectItem* pCurrentItem = CDsmDataLayer::getInstance()->GetCurrentCtrlBlk();
    if (!m_pCtrlBlk || m_pCtrlBlk->nID != pCurrentItem->nID)
    {
        SetCtrlBlk(pCurrentItem);
    }
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        return m_pCtrlBlk->pRecvConfig->SetDspSendCurInfo(nCurPhaseA, nCurPhaseB, nCurPhaseC);
    }
    return false;
}

///
/// @brief
///    ��ȡ�������ͨ��
///
/// @param[out]  nCurPhaseA ����A�����ͨ����
/// @param[out]  nCurPhaseB ����B�����ͨ����
/// @param[out]  nCurPhaseC ����C�����ͨ����
///
bool CDataPower::GetCurrentChannles( uint32_t& nCurPhaseA, uint32_t& nCurPhaseB, uint32_t& nCurPhaseC )
{
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        return m_pCtrlBlk->pRecvConfig->GetDspSendCurInfo(nCurPhaseA, nCurPhaseB, nCurPhaseC);
    }
    return false;
}


///
/// @brief
///    ��ʼ����ȡ���/������Ϣ�������������ͨ������е���
///
void CDataPower::init()
{
    //��ʼ��ʱ�����õ�ǰ���ƿ�
    if (NULL == m_pCtrlBlk)
    {
        SetCtrlBlk(CDsmDataLayer::getInstance()->GetCurrentCtrlBlk());
    }
}

///
/// @brief
///    ��չ�����Ϣ
///
void CDataPower::ClearPowerData()
{
    memset(m_fActivePower, 0, sizeof(m_fActivePower));
    memset(m_fReactivePower, 0, sizeof(m_fReactivePower));
    memset(m_fApparentPower, 0, sizeof(m_fApparentPower));
    memset(m_fPowerFactor, 0, sizeof(m_fPowerFactor));
    m_fTotalActivePower     = 0;
    m_fTotalReactivePower   = 0;
    m_fTotalApparentPower   = 0;
    m_fTotalPowerFactor     = 0;

}

///
/// @brief
///    ��ȡ��ѹ��Чֵ��Ϣ
///
/// @param[out]  fPhaseAData        ����A���ѹ��Чֵ
/// @param[out]  fPhaseBData        ����B���ѹ��Чֵ
/// @param[out]  fPhaseCData        ����C���ѹ��Чֵ
///
/// @return
///     ��ȡ�ɹ�����true
///
bool CDataPower::GetVotageEffectValue( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData )
{
    uint32_t nPhaseABC[3] = {0};
    float    fPhaseABCData[3] = {0.0f};
    if( GetVoltageChannles(nPhaseABC[0], nPhaseABC[1], nPhaseABC[2]) )
    {
        CEffectiveChanelInfo* pChannelInfo = NULL;
        for (int i = 0; i < 3; ++i)
        {
            pChannelInfo = GetOneChannelInfo(nPhaseABC[i]);
            if (pChannelInfo)
            {
                fPhaseABCData[i] = pChannelInfo->m_fEffectiveValue;
            }
            else
            {
                return false;
            }
        }
        fPhaseAData = fPhaseABCData[0];
        fPhaseBData = fPhaseABCData[1];
        fPhaseCData = fPhaseABCData[2];
        return true;
    }
    return false;
}

///
/// @brief
///    ��ȡ��ѹ��λ��Ϣ
///
/// @param[out]  fPhaseAData        ����A���ѹ��λ
/// @param[out]  fPhaseBData        ����B���ѹ��λ
/// @param[out]  fPhaseCData        ����C���ѹ��λ
///
/// @return
///     ��ȡ�ɹ�����true
///
bool CDataPower::GetVoltageAngle( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData )
{
    uint32_t nPhaseABC[3] = {0};
    float    fPhaseABCData[3] = {0.0f};
    if( GetVoltageChannles(nPhaseABC[0], nPhaseABC[1], nPhaseABC[2]) )
    {
        CEffectiveChanelInfo* pChannelInfo = NULL;
        for (int i = 0; i < 3; ++i)
        {
            pChannelInfo = GetOneChannelInfo(nPhaseABC[i]);
            if (pChannelInfo)
            {
                fPhaseABCData[i] = pChannelInfo->m_fPhase;
            }
            else
            {
                return false;
            }
        }
        fPhaseAData = fPhaseABCData[0];
        fPhaseBData = fPhaseABCData[1];
        fPhaseCData = fPhaseABCData[2];
        return true;
    }
    return false;
}

///
/// @brief
///    ��ȡ������Чֵ��Ϣ
///
/// @param[out]  fPhaseAData        ����A�������Чֵ
/// @param[out]  fPhaseBData        ����B�������Чֵ
/// @param[out]  fPhaseCData        ����C�������Чֵ
///
/// @return
///     ��ȡ�ɹ�����true
///
bool CDataPower::GetCurrentEffectValue( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData )
{
    uint32_t nPhaseABC[3] = {0};
    float    fPhaseABCData[3] = {0.0f};
    if( GetCurrentChannles(nPhaseABC[0], nPhaseABC[1], nPhaseABC[2]) )
    {
        CEffectiveChanelInfo* pChannelInfo = NULL;
        for (int i = 0; i < 3; ++i)
        {
            pChannelInfo = GetOneChannelInfo(nPhaseABC[i]);
            if (pChannelInfo)
            {
                fPhaseABCData[i] = pChannelInfo->m_fEffectiveValue;
            }
            else
            {
                return false;
            }
        }
        fPhaseAData = fPhaseABCData[0];
        fPhaseBData = fPhaseABCData[1];
        fPhaseCData = fPhaseABCData[2];
        return true;
    }
    return false;
}

///
/// @brief
///    ��ȡ������λ��Ϣ
///
/// @param[out]  fPhaseAData        ����A�������λ
/// @param[out]  fPhaseBData        ����B�������λ
/// @param[out]  fPhaseCData        ����C�������λ
///
/// @return
///     ��ȡ�ɹ�����true
///
bool CDataPower::GetCurrentAngle( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData )
{
    uint32_t nPhaseABC[3] = {0};
    float    fPhaseABCData[3] = {0.0f};
    if( GetCurrentChannles(nPhaseABC[0], nPhaseABC[1], nPhaseABC[2]) )
    {
        CEffectiveChanelInfo* pChannelInfo = NULL;
        for (int i = 0; i < 3; ++i)
        {
            pChannelInfo = GetOneChannelInfo(nPhaseABC[i]);
            if (pChannelInfo)
            {
                fPhaseABCData[i] = pChannelInfo->m_fPhase;
            }
            else
            {
                return false;
            }
        }
        fPhaseAData = fPhaseABCData[0];
        fPhaseBData = fPhaseABCData[1];
        fPhaseCData = fPhaseABCData[2];
        return true;
    }
    return false;
}
