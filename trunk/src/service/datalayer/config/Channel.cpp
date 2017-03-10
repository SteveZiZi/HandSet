/// @file
///
/// @brief
///     ͨ��
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.28
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "Channel.h"
#include "GooseCb.h"
#include "src\service\datalayer\send\FrameSend.h"
#include "src\service\datalayer\utility\data_utility.h"

#define MAX_HISTORY_SEND_RECORD         3                               ///< �����ʷ���ͼ�¼��
#define MAX_HISTORY_RECV_RECORD         3                               ///< �����ʷ���ռ�¼��

#define NODE_Channel_Description        L"Description"                  ///< ͨ������
#define NODE_Channel_Type               L"Type"                         ///< ͨ������
#define NODE_Channel_Measure            L"measure"                      ///< ͨ���Ƿ����ڲ�������
#define NODE_Channel_Phase              L"Phase"                        ///< ��λ
#define NODE_Channel_OnceRatings        L"OnceRatings"                  ///< һ�ζֵ
#define NODE_Channel_SecondaryRatings   L"SecondaryRatings"             ///< ���ζֵ
#define NODE_Channel_VirtualChannel     L"VirtualChannel"               ///< ӳ��
#define NODE_Channel_Value              L"Value"                        ///< ͨ��GOOSEֵ
#define NODE_Channel_SubChannel         L"SubChannel"                   ///< ��ͨ��

///
/// @brief
///    ����Ԥ����ͨ����Ϣ�ṹ��
///
struct PREDEFINED_GOOSE_SUBCHANNEL_INFO
{
    uint32_t                nIndex;                                     ///< ͨ������
    std::wstring            strChannelDesc;                             ///< ͨ������
    enumChannelType         enChannelType;                              ///< ͨ������
};

///
/// @brief
///    ����Ft3Ԥ��ͨ����Ϣ��9-1ͬ����ft3
///
static const PREDEFINED_GOOSE_SUBCHANNEL_INFO PREDEFINED_SUBCHANNEL_INFO_GOOSE[] = {
    { 0,    L"stVal",   CHANNEL_TYPE_GOOSE_POINT},
    { 1,    L"q",       CHANNEL_TYPE_GOOSE_QUALITY},
    { 2,    L"t",       CHANNEL_TYPE_GOOSE_TIME}
};

///
/// @brief
///    ���캯��
///
CChannel::CChannel(void)
{
}

///
/// @brief
///     ���캯��
///
/// @param[in]  pParentBaseCb       ָ�򸸿��ƿ��ָ��
/// @param[in]  pParentChannel      ָ��ͨ����ָ��
/// @param[in]  pParamConfigBase    ָ��������ù�������ָ��
///
CChannel::CChannel(CBaseCb* pParentBaseCb, CChannel* pParentChannel, CParamConfigBase* pParamConfigBase)
:m_pParentBaseCb(pParentBaseCb)
,m_pParentChannel(pParentChannel)
,m_pParamConfigBase(pParamConfigBase)
,m_bSelected(false)
,m_strDescription(L"")
,m_eChannelType(CHANNEL_TYPE_UNKOWN)
,m_usQuality(0)
,m_bMeasure(false)
,m_eChannelPhase(CHANNEL_PHASE_UNKOWN)
,m_dOnceRatings(1.0f)
,m_dSecondaryRatings(1.0f)
,m_strVirtualChannel(L"")
,m_bIsSendValueChanged(false)
,m_pSourceChannel(NULL)
,m_pPrev(NULL)
,m_pNext(NULL)
{
}

///
/// @brief
///    ��������
///
CChannel::~CChannel(void)
{
    // ���������ͨ��
    ClearAllSubChannel();
}

///
/// @brief
///    ���ͨ������ͨ�������������ƿ飨��ͨ�����е������ţ���0��ʼ������
///
/// @return
///     int    ͨ���������ţ�-1��ʾʧ�ܡ�
///
int CChannel::GetIndex()
{
    int nIndex = -1;


    if (NULL != m_pParentChannel)
    {
        // ������ͨ����������ͨ�����õ���ǰ��ͨ���ڸ�ͨ���е�����
        CChannel* pSubChannel = m_pParentChannel->FirstSubChannel();
        while(pSubChannel)
        {
            nIndex++;
            if (pSubChannel == this)
            {
                break;
            }

            pSubChannel = m_pParentChannel->NextSubChannel(pSubChannel);
        }
    }
    else if(NULL != m_pParentBaseCb)
    {
        // �������ƿ������ͨ�����õ���ǰͨ���ڿ��ƿ��е�����
        CChannel* pChannel = m_pParentBaseCb->FirstChannel();
        while(pChannel)
        {
            nIndex++;
            if (pChannel == this)
            {
                break;
            }

            pChannel = m_pParentBaseCb->NextChannel(pChannel);
        }
    }

    return nIndex;
}

///
/// @brief
///    ��ÿ��ƿ�ѡ��״̬
///
/// @return
///     bool    ����true��ʾ���ƿ鱻ѡ��
///
bool CChannel::IsSelected()
{
    return m_bSelected;
}

///
/// @brief
///    ���ÿ��ƿ�ѡ��״̬
///
/// @param[in]  bSelected  ���ƿ�ѡ��״̬
///
void CChannel::Selected(bool bSelected)
{
    if (m_bSelected != bSelected)
    {
        m_bSelected = bSelected;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���ͨ��������Ϣ
///
/// @return
///     wstring    ͨ��������Ϣ
///
std::wstring CChannel::GetDescription()
{
    return m_strDescription;
}

///
/// @brief
///    ����ͨ��������Ϣ
///
/// @param[in]  strDescription  ͨ��������Ϣ
///
void CChannel::SetDescription(std::wstring strDescription)
{
    if (m_strDescription != strDescription)
    {
        m_strDescription = strDescription;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���ͨ������
///
/// @return
///     enumChannelType    ͨ������
///
enumChannelType CChannel::GetChannelType()
{
    return m_eChannelType;
}

///
/// @brief
///    ����ͨ������
///
/// @param[in]  eNewChannelType  ͨ������
///
/// @return
///     bool    �ɹ�����true
///
bool CChannel::SetChannelType(enumChannelType eNewChannelType)
{
    if (m_eChannelType != eNewChannelType)
    {
        // ��û����������ù�����
        CParamConfigBasic* pParamConfigBasic = CParamConfigBasic::getInstance();
        if (NULL == pParamConfigBasic)
        {
            return false;
        }

        // ����ͨ�����ͣ����޸�Ĭ�ϵ�һ��ֵ������ֵ
        switch(eNewChannelType)
        {
        case CHANNEL_TYPE_UNKOWN:                                       ///< δ֪
            {
                // �޸�һ�Ρ�����ֵ
                m_dOnceRatings          = 1.0f;
                m_dSecondaryRatings     = 1.0f;

                // �������ͨ��ӳ��
                m_strVirtualChannel     = L"";
            }
            break;

        case CHANNEL_TYPE_SMV_CURRENT:                                  ///< ����
            {
                // �޸�������Ϣ
                m_strDescription = GenerateChannelDes(m_strDescription, m_eChannelType, eNewChannelType, m_eChannelPhase, m_eChannelPhase, m_bMeasure, m_bMeasure);

                // �޸�һ�Ρ�����ֵ
                m_dOnceRatings          = pParamConfigBasic->GetRatedPrimaryCurrent();
                m_dSecondaryRatings     = pParamConfigBasic->GetRatedSecondaryCurrent();

                // �������ͨ��ӳ��
                m_strVirtualChannel     = L"";
            }
            break;

        case CHANNEL_TYPE_SMV_VOLTAGE:                                  ///< ��ѹ
            {
                // �޸�������Ϣ
                m_strDescription = GenerateChannelDes(m_strDescription, m_eChannelType, eNewChannelType, m_eChannelPhase, m_eChannelPhase, m_bMeasure, m_bMeasure);

                // �޸�һ�Ρ�����ֵ
                m_dOnceRatings          = pParamConfigBasic->GetRatedPrimaryVoltage() * 1000;
                m_dSecondaryRatings     = pParamConfigBasic->GetRatedSecondaryVoltage();

                // �������ͨ��ӳ��
                m_strVirtualChannel     = L"";
            }
            break;

        case CHANNEL_TYPE_SMV_TIME:                                     ///< ʱ��
            {
                // �޸�������Ϣ
                m_strDescription = GenerateChannelDes(m_strDescription, m_eChannelType, eNewChannelType, m_eChannelPhase, m_eChannelPhase, m_bMeasure, m_bMeasure);

                // �޸�һ�Ρ�����ֵ
                swscanf_s(pParamConfigBasic->GetMuRatedDelay().c_str(), L"%lf", &m_dOnceRatings, sizeof(m_dOnceRatings));
                swscanf_s(pParamConfigBasic->GetMuRatedDelay().c_str(), L"%lf", &m_dSecondaryRatings, sizeof(m_dSecondaryRatings));

                // �������ͨ��ӳ��
                m_strVirtualChannel     = L"";
            }
            break;

        case CHANNEL_TYPE_GOOSE_POINT:                                  ///< ����
            {
                m_stGooseSendValue.__SetType(GSEDT_BOOLEAN);
                m_stGooseRecvValue.__SetType(GSEDT_BOOLEAN);
            }
            break;

        case CHANNEL_TYPE_GOOSE_DOUBLE_POINT:                           ///< ˫��
            {
                m_stGooseSendValue.__SetType(GSEDT_BIT_STR);
                m_stGooseSendValue.m_stBitString.SetString(strDPValue[DP_VALUE_INIT]);

                m_stGooseRecvValue.__SetType(GSEDT_BIT_STR);
                m_stGooseRecvValue.m_stBitString.SetString(strDPValue[DP_VALUE_INIT]);
            }
            break;

        case CHANNEL_TYPE_GOOSE_QUALITY:                                ///< Ʒ��
            {
                m_stGooseSendValue.__SetType(GSEDT_BIT_STR);
                m_stGooseSendValue.m_stBitString.SetString(L"0000000000000");

                m_stGooseRecvValue.__SetType(GSEDT_BIT_STR);
                m_stGooseRecvValue.m_stBitString.SetString(L"0000000000000");
            }
            break;

        case CHANNEL_TYPE_GOOSE_TIME:                                   ///< GOOSEʱ��
            {
                m_stGooseSendValue.__SetType(GSEDT_UTC_TIME);
                m_stGooseRecvValue.__SetType(GSEDT_UTC_TIME);
            }
            break;

        case CHANNEL_TYPE_GOOSE_FLOAT:                                  ///< ������
            {
                m_stGooseSendValue.__SetType(GSEDT_FLOAT);
                m_stGooseRecvValue.__SetType(GSEDT_FLOAT);
            }
            break;

        case CHANNEL_TYPE_GOOSE_INT8:                                   ///< INT8
            {
                m_stGooseSendValue.__SetType(GSEDT_INT8);
                m_stGooseRecvValue.__SetType(GSEDT_INT8);
            }
            break;

        case CHANNEL_TYPE_GOOSE_INT16:                                  ///< INT16
            {
                m_stGooseSendValue.__SetType(GSEDT_INT16);
                m_stGooseRecvValue.__SetType(GSEDT_INT16);
            }
            break;

        case CHANNEL_TYPE_GOOSE_INT24:                                  ///< INT24
            {
                m_stGooseSendValue.__SetType(GSEDT_INT24);
                m_stGooseRecvValue.__SetType(GSEDT_INT24);
            }
            break;

        case CHANNEL_TYPE_GOOSE_INT32:                                  ///< INT32
            {
                m_stGooseSendValue.__SetType(GSEDT_INT32);
                m_stGooseRecvValue.__SetType(GSEDT_INT32);
            }
            break;

        case CHANNEL_TYPE_GOOSE_UINT8:                                  ///< UINT8
            {
                m_stGooseSendValue.__SetType(GSEDT_UINT8);
                m_stGooseRecvValue.__SetType(GSEDT_UINT8);
            }
            break;

        case CHANNEL_TYPE_GOOSE_UINT16:                                 ///< UINT16
            {
                m_stGooseSendValue.__SetType(GSEDT_UINT16);
                m_stGooseRecvValue.__SetType(GSEDT_UINT16);
            }
            break;

        case CHANNEL_TYPE_GOOSE_UINT24:                                 ///< UINT24
            {
                m_stGooseSendValue.__SetType(GSEDT_UINT24);
                m_stGooseRecvValue.__SetType(GSEDT_UINT24);
            }
            break;

        case CHANNEL_TYPE_GOOSE_UINT32:                                 ///< UINT32
            {
                m_stGooseSendValue.__SetType(GSEDT_UINT32);
                m_stGooseRecvValue.__SetType(GSEDT_UINT32);
            }
            break;

        case CHANNEL_TYPE_GOOSE_STRUCT:                                 ///< �ṹ
            {
                //
                // �ж�ͨ���Ƿ�����ͨ�������û�У���ô���Ĭ����ͨ��
                //
                if (m_listSubChannelGroup.size() == 0)
                {
                    // ��ͨ�����Ĭ����ͨ��
                    for (int i = 0; i < sizeof(PREDEFINED_SUBCHANNEL_INFO_GOOSE)/ sizeof(PREDEFINED_SUBCHANNEL_INFO_GOOSE[0]); i++)
                    {
                        // �����ͨ��
                        CChannel*   pChannel = __AddSubChannel();
                        if (NULL == pChannel)
                        {
                            return false;
                        }

                        // ������ͨ��������Ϣ
                        pChannel->SetChannelType(PREDEFINED_SUBCHANNEL_INFO_GOOSE[i].enChannelType);
                        pChannel->m_strDescription  = PREDEFINED_SUBCHANNEL_INFO_GOOSE[i].strChannelDesc;
                    }
                }
            }
            break;

        default:
            {
                return false;
            }
        }

        // ����ͨ������
        m_eChannelType = eNewChannelType;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    ���ͨ����������
///
/// @return
///     bool    ͨ���Ƿ����ڲ�������
///
bool CChannel::IsMeasure()
{
    return m_bMeasure;
}

///
/// @brief
///    ����ͨ����������
///
/// @param[in]  bNewMeasure  ͨ���Ƿ����ڲ�������
///
void CChannel::SetMeasure(bool bNewMeasure)
{
    if (m_bMeasure != bNewMeasure)
    {
        // �޸�ͨ��������Ϣ
        m_strDescription = GenerateChannelDes(m_strDescription, m_eChannelType, m_eChannelType, m_eChannelPhase, m_eChannelPhase, m_bMeasure, bNewMeasure);

        // �������/�������
        m_bMeasure = bNewMeasure;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���ͨ����λ
///
/// @return
///     enumChannelType    ͨ����λ
///
ChannelPhase CChannel::GetChannelPhase()
{
    return m_eChannelPhase;
}

///
/// @brief
///    ����ͨ����λ
///
/// @param[in]  eNewChannelPhase  ͨ����λ
///
void CChannel::SetChannelPhase(ChannelPhase eNewChannelPhase)
{
    if (m_eChannelPhase != eNewChannelPhase)
    {
        // �޸�ͨ��������Ϣ
        m_strDescription = GenerateChannelDes(m_strDescription, m_eChannelType, m_eChannelType, m_eChannelPhase, eNewChannelPhase, m_bMeasure, m_bMeasure);

        // �����µ��������
        m_eChannelPhase = eNewChannelPhase;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���һ�ζֵ��������λΪA����ѹ��λΪV��
///
/// @return
///     double    һ�ζֵ
///
double CChannel::GetOnceRatings()
{
    return m_dOnceRatings;
}

///
/// @brief
///    ����һ�ζֵ��������λΪA����ѹ��λΪV��
///
/// @param[in]  dOnceRatings  һ�ζֵ
///
void CChannel::SetOnceRatings(double dOnceRatings)
{
    if (fabs(m_dOnceRatings - dOnceRatings) >= FLOAT_PRECISION)
    {
        m_dOnceRatings = dOnceRatings;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ��ö��ζֵ��������λΪA����ѹ��λΪV��
///
/// @return
///     double    ���ζֵ
///
double CChannel::GetSecondaryRatings()
{
    return m_dSecondaryRatings;
}

///
/// @brief
///    ���ö��ζֵ��������λΪA����ѹ��λΪV��
///
/// @param[in]  dSecondaryRatings  ���ζֵ
///
void CChannel::SetSecondaryRatings(double dSecondaryRatings)
{
    if (fabs(m_dSecondaryRatings - dSecondaryRatings) >= FLOAT_PRECISION)
    {
        m_dSecondaryRatings = dSecondaryRatings;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���ͨ��ӳ��
///
/// @return
///     wstring    ͨ��ӳ��
///
std::wstring CChannel::GetVirtualChannel()
{
    return m_strVirtualChannel;
}

///
/// @brief
///    ����ͨ��ӳ��
///
/// @param[in]  strVirtualChannel  ͨ��ӳ��
///
void CChannel::SetVirtualChannel(std::wstring strVirtualChannel)
{
    if (m_strVirtualChannel != strVirtualChannel)
    {
        m_strVirtualChannel = strVirtualChannel;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    �����ͨ��
///
/// @param[in]  pNewChannel ָ��Channel�����ָ��
///
/// @return
///     bool    true��ʾ��ӳɹ�
///
bool CChannel::__AddSubChannel(CChannel* pNewChannel)
{
    if (NULL == pNewChannel)
    {
        return false;
    }

    // ���GOOSE���ƿ鵽������
    m_listSubChannelGroup.push_back(pNewChannel);
    pNewChannel->__SetParentChannel(this);

    return true;
}

///
/// @brief
///    �����ͨ��
///
/// @return
///     CChannel*    �ɹ�ʱ������ͨ��ָ�룬ʧ��ʱ����NULL
///
CChannel* CChannel::__AddSubChannel()
{
    // newһ����ͨ��
    CChannel*  pSubChannel = new CChannel(m_pParentBaseCb, this, m_pParamConfigBase);
    if (NULL == pSubChannel)
    {
        return NULL;
    }

    // ���GOOSE���ƿ鵽������
    m_listSubChannelGroup.push_back(pSubChannel);
    return pSubChannel;
}

///
/// @brief
///    ö�ٵ�һ����ͨ��
///
/// @return
///     CChannel*    �ɹ�ʱ����ͨ��ָ�룬ʧ��ʱ����NULL
///
CChannel* CChannel::FirstSubChannel()
{
    std::list<CChannel*>::iterator iter = m_listSubChannelGroup.begin();
    if (m_listSubChannelGroup.end() != iter)
    {
        return *iter;
    }

    return NULL;
}

///
/// @brief
///    �����һ����ͨ��
///
/// @param[in]  pCurSubChannel ��ǰ��ͨ����ָ��
///
/// @return
///     CChannel*    �ɹ�ʱ������ͨ��ָ�룬ʧ��ʱ����NULL
///
CChannel* CChannel::NextSubChannel(CChannel* pCurSubChannel)
{
    CChannel*                       pSubChannel   = NULL;
    std::list<CChannel*>::iterator  iter;


    iter = m_listSubChannelGroup.begin();
    while (m_listSubChannelGroup.end() != iter)
    {
        pSubChannel = *iter;

        // ���ҵ�ǰGOOSE���ƿ��λ��
        if (pSubChannel == pCurSubChannel)
        {
            // �����һ��GOOSE���ƿ��λ��
            iter++;
            if (m_listSubChannelGroup.end() == iter)
            {
                return NULL;
            }

            return (*iter);
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    �����һ����ͨ��
///
/// @param[in]  pCurSubChannel ��ǰ��ͨ����ָ��
///
/// @return
///     CChannel*    �ɹ�ʱ������ͨ��ָ�룬ʧ��ʱ����NULL
///
CChannel* CChannel::PrevSubChannel(CChannel* pCurSubChannel)
{
    CChannel*                               pSubChannel   = NULL;
    std::list<CChannel*>::reverse_iterator  rIter;


    rIter = m_listSubChannelGroup.rbegin();
    while (m_listSubChannelGroup.rend() != rIter)
    {
        pSubChannel = *rIter;

        // ���ҵ�ǰGOOSE���ƿ��λ��
        if (pSubChannel == pCurSubChannel)
        {
            // �����һ��GOOSE���ƿ��λ��
            rIter++;
            if (m_listSubChannelGroup.rend() == rIter)
            {
                return NULL;
            }

            return (*rIter);
        }

        rIter++;
    }

    return NULL;
}

///
/// @brief
///    ���������ͨ��
///
void CChannel::ClearAllSubChannel()
{
    // �����������ͷŶ���
    std::list<CChannel*>::iterator iter = m_listSubChannelGroup.begin();
    while (m_listSubChannelGroup.end() != iter)
    {
        CChannel* pSubChannel = *iter;
        if (pSubChannel)
        {
            delete pSubChannel;
        }

        iter++;
    }

    if (m_listSubChannelGroup.size())
    {
        // �����ͨ��
        m_listSubChannelGroup.clear();

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    �����ͨ����
///
/// @return
///     int    ��ͨ����
///
int CChannel::GetSubChannelCount()
{
    return (int)m_listSubChannelGroup.size();
}

///
/// @brief
///    ��ȿ���һ��ͨ��
///
/// @param[in]  pChannel    ָ��Channel�����ָ��
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CChannel::__Copy( CChannel* pChannel )
{
    CChannel*       pNewSubChannel  = NULL;


    // ����������
    if (NULL == pChannel)
    {
        return false;
    }
    m_pSourceChannel            = pChannel;

    // ���ƻ�������
    SetChannelType(pChannel->m_eChannelType);
    m_strDescription            = pChannel->m_strDescription;
    m_usQuality                 = pChannel->m_usQuality;
    m_bMeasure                  = pChannel->m_bMeasure;
    m_eChannelPhase             = pChannel->m_eChannelPhase;
    m_strVirtualChannel         = pChannel->m_strVirtualChannel;
    m_stGooseRecvValue          = pChannel->m_stGooseRecvValue;
    m_stGooseSendValue          = pChannel->m_stGooseSendValue;

    // ���������ͨ��
    ClearAllSubChannel();

    // �����ͨ��
    std::list<CChannel*>::iterator iter = pChannel->m_listSubChannelGroup.begin();
    while (pChannel->m_listSubChannelGroup.end() != iter)
    {
        CChannel* pSubChannel = *iter;
        if (pSubChannel)
        {
            pNewSubChannel = new CChannel(GetParentBaseCb(), this, m_pParamConfigBase);
            if (NULL == pNewSubChannel)
            {
                return false;
            }

            // ��ȿ�����ͨ��
            if (!pNewSubChannel->__Copy(pSubChannel))
            {
                delete pNewSubChannel;
                return false;
            }

            // �����ͨ����������
            if (!__AddSubChannel(pNewSubChannel))
            {
                delete pNewSubChannel;
                return false;
            }
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    ��ȿ���һ��GOOSE_DATA����
///
/// @param[in]  pGooseData ָ��GOOSE_DATA�����ָ��
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CChannel::__Copy(GOOSE_DATA* pGooseData)
{
    CChannel*       pNewSubChannel  = NULL;
    enumChannelType eChannelType    = CHANNEL_TYPE_UNKOWN;


    // ����������
    if (NULL == pGooseData)
    {
        return false;
    }

    //
    // ����ͨ������
    //
    if (GSEDT_UTC_TIME == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_TIME;
    }
    else if (GSEDT_BOOLEAN == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_POINT;
    }
    else if (GSEDT_BIT_STR == pGooseData->type)
    {
        GOOSE_BIT_STRING*   pGooseBitString = (GOOSE_BIT_STRING*)pGooseData;
        int                 iBitCount       = (pGooseBitString->strSize * 8) - pGooseBitString->padding;

        // ����λ���ж�ͨ������
        if (2 == iBitCount)
        {
            eChannelType = CHANNEL_TYPE_GOOSE_DOUBLE_POINT;
        }
        else if (13 == iBitCount)
        {
            eChannelType = CHANNEL_TYPE_GOOSE_QUALITY;
        }
    }
    else if (GSEDT_INT8 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_INT8;
    }
    else if (GSEDT_INT16 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_INT16;
    }
    else if (GSEDT_INT24 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_INT24;
    }
    else if (GSEDT_INT32 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_INT32;
    }
    else if (GSEDT_UINT8 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_UINT8;
    }
    else if (GSEDT_UINT16 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_UINT16;
    }
    else if (GSEDT_UINT24 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_UINT24;
    }
    else if (GSEDT_UINT32 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_UINT32;
    }
    else if (GSEDT_FLOAT == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_FLOAT;
    }
    else if (GSEDT_STRUCT == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_STRUCT;                       ///< ͨ������

        // ���������ͨ��
        ClearAllSubChannel();

        //
        // �����ͨ��
        //
        GOOSE_STRUCT* pGooseStruct = (GOOSE_STRUCT*)pGooseData;
        if (NULL == pGooseStruct)
        {
            return false;
        }

        std::vector<GOOSE_DATA*>::iterator iter = pGooseStruct->member.begin();
        while(pGooseStruct->member.end() != iter)
        {
            GOOSE_DATA* pSubGooseData = *iter;
            if (pSubGooseData)
            {
                pNewSubChannel = new CChannel(GetParentBaseCb(), this, m_pParamConfigBase);
                if (NULL == pNewSubChannel)
                {
                    return false;
                }

                // ��ȿ�����ͨ��
                if (!pNewSubChannel->__Copy(pSubGooseData))
                {
                    delete pNewSubChannel;
                    return false;
                }

                // �����ͨ����������
                if (!__AddSubChannel(pNewSubChannel))
                {
                    delete pNewSubChannel;
                    return false;
                }
            }

            iter++;
        }
    }

    // ����ͨ������
    SetChannelType(eChannelType);

    // ����ͨ����
    {
        CString     csText;

        if (m_pParentChannel)
        {
            csText.Format(_T("��ͨ��%d"), m_pParentChannel->GetSubChannelCount() + 1);
        }
        else if (m_pParentBaseCb)
        {
            csText.Format(_T("GOOSEͨ��%d"), m_pParentBaseCb->GetChannelCount() + 1);
        }

        m_strDescription = csText.GetString();
    }

    // ���ý���ֵ
    m_stGooseRecvValue = pGooseData;

    return true;
}

///
/// @brief
///    ����ͨ�������Ŀ��ƿ�
///
/// @param[in]  pBaseCb    ���ƿ�ָ��
///
/// @return
///     void    
///
void CChannel::__SetParentBaseCb( CBaseCb* pBaseCb )
{
    m_pParentBaseCb = pBaseCb;
}

///
/// @brief
///     ���ͨ���������ƿ�����ָ��
///
/// @return
///     CBaseCb*    ͨ���������ƿ�����ָ��
///
CBaseCb* CChannel::GetParentBaseCb()
{
    return m_pParentBaseCb;
}

///
/// @brief
///    ����ͨ�������ĸ�ͨ��ָ��
///
/// @param[in]  pChannel    ��ͨ��ָ��
///
/// @return
///     void    
///
void CChannel::__SetParentChannel( CChannel* pChannel )
{
    m_pParentChannel = pChannel;
}

///
/// @brief
///    ��ø�ͨ��ָ��
///
/// @return
///     CChannel*    ��ͨ��ָ��
///
CChannel* CChannel::GetParentChannel()
{
    return m_pParentChannel;
}

///
/// @brief
///     ���GOOSEͨ���ķ���ֵ
///
/// @return
///     GOOSE_DATA_VARIANT    GOOSEͨ���ĳ�ʼֵ
///
GOOSE_DATA_VARIANT& CChannel::GetGooseSendValue()
{
    return m_stGooseSendValue;
}

///
/// @brief
///    ����GOOSEͨ���ķ���ֵ
///
/// @param[in]  stNewGooseValue    GOOSEͨ���ķ���ֵ
///
/// @return
///     bool    �ɹ�����true
///
bool CChannel::SetGooseSendValue( GOOSE_DATA_VARIANT& stNewGooseValue )
{
    if (m_stGooseSendValue != stNewGooseValue)
    {
        m_stGooseSendValue = stNewGooseValue;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    ���GOOSEͨ����ʵʱֵ
///
/// @return
///     GOOSE_DATA_VARIANT    GOOSEͨ����ʵʱֵ
///
GOOSE_DATA_VARIANT& CChannel::GetGooseRecvValue()
{
    return m_stGooseRecvValue;
}

///
/// @brief
///    ����GOOSEͨ����ʵʱֵ
///
/// @param[in]  stGooseValue    GOOSEͨ����ʵʱֵ
///
/// @return
///     bool    �ɹ�����true
///
bool CChannel::SetGooseRecvValue( GOOSE_DATA_VARIANT& stGooseValue )
{
    if (m_stGooseRecvValue != stGooseValue)
    {
        m_stGooseRecvValue = stGooseValue;
    }

    return true;
}

///
/// @brief
///    ���ͨ��Ʒ��ֵ
///
/// @return
///     unsigned short    ͨ��Ʒ��ֵ
///
unsigned short CChannel::GetQuality()
{
    return m_usQuality;
}

///
/// @brief
///    ����ͨ��Ʒ��ֵ
///
/// @param[in]  usNewQuality    �µ�Ʒ��ֵ
///
/// @return
///     bool    �ɹ�����true
///
bool CChannel::SetQuality( unsigned short usNewQuality )
{
    if (m_usQuality != usNewQuality)
    {
        m_usQuality = usNewQuality;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
            m_pParamConfigBase->SetQualityChanged(true);
        }
    }

    return true;
}

///
/// @brief
///    �ж�ͨ������ֵ�Ƿ���ָ��ͨ�������
///
/// @param[in]  pOldChannel    ָ��ͨ�������ָ��
///
/// @return
///     bool    ͨ��ֵ��ȷ���true
///
bool CChannel::__CompareSendValue( CChannel* pOldChannel )
{
    // �ṹ�����͵�ͨ����Ҫ�ֱ�Ƚ���ͨ����ֵ
    if (CHANNEL_TYPE_GOOSE_STRUCT == m_eChannelType)
    {
        bool                            bRet = true;
        std::list<CChannel *>::iterator oldIter;
        std::list<CChannel *>::iterator newIter;


        // �ж���ͨ�����Ƿ����
        if (m_listSubChannelGroup.size() != pOldChannel->m_listSubChannelGroup.size())
        {
            return false;
        }

        // ���αȽ�ÿһ����ͨ���ķ���ֵ�Ƿ����
        oldIter = pOldChannel->m_listSubChannelGroup.begin();
        newIter = m_listSubChannelGroup.begin();
        while((oldIter != pOldChannel->m_listSubChannelGroup.end()) && (newIter != m_listSubChannelGroup.end()))
        {
            CChannel* pOldSubChannel = *oldIter;
            CChannel* pNewSubChannel = *newIter;
            if ((NULL != pOldSubChannel) && (NULL != pNewSubChannel))
            {
                // �Ƚ���ͨ������ֵ
                if (!pNewSubChannel->__CompareSendValue(pOldSubChannel))
                {
                    // ���Ϊ���޸�
                    pNewSubChannel->SetSendValueChanged(true);
                    bRet = false;
                }
            }

            oldIter++;
            newIter++;
        }

        return bRet;
    }
    // ����ʱ�����͵�GOOSEͨ��
    else if (CHANNEL_TYPE_GOOSE_TIME == m_eChannelType)
    {
        return true;
    }
    // �ǽṹ�����͵�ͨ��ֱ�ӱȽ�GOOSE����ֵ
    else
    {
        bool bRet = !(m_stGooseSendValue != pOldChannel->m_stGooseSendValue);
        return bRet;
    }
}

///
/// @brief
///     ͨ���ķ���ֵ�Ƿ����˸ı�
///
/// @return
///     bool    true��ʾͨ���ķ���ֵ�����˸ı�
///
bool CChannel::IsSendValueChanged()
{
    return m_bIsSendValueChanged;
}

///
/// @brief
///    ����ͨ���ķ���ֵ�Ƿ����˸ı�
///
/// @param[in]  bIsChanged    ͨ��ֵ�Ƿ����˸ı�
///
void CChannel::SetSendValueChanged( bool bIsChanged )
{
    if (m_bIsSendValueChanged != bIsChanged)
    {
        m_bIsSendValueChanged = bIsChanged;
    }
}

///
/// @brief
///    ���ͨ��ֵ��ͨ�ű�������ռ�ֽ���
///
/// @return
///     int    �ֽ���
///
int CChannel::GetValueLength()
{
    int     iValueLen = 4;


    switch(m_eChannelType)
    {
    case CHANNEL_TYPE_SMV_CURRENT:
    case CHANNEL_TYPE_SMV_VOLTAGE:
    case CHANNEL_TYPE_SMV_TIME:
    case CHANNEL_TYPE_GOOSE_DOUBLE_POINT:
        iValueLen = 4;
        break;

    case CHANNEL_TYPE_GOOSE_STRUCT:
        {
            // ͳ��������ͨ���ڱ�������ռ�ֽ���
            iValueLen = 2;
            std::list<CChannel*>::iterator iter = m_listSubChannelGroup.begin();
            while (m_listSubChannelGroup.end() != iter)
            {
                CChannel* pSubChannel = *iter;
                if (pSubChannel)
                {
                    iValueLen += pSubChannel->GetValueLength();
                }

                iter++;
            }
        }
        break;

    case CHANNEL_TYPE_GOOSE_POINT:
        iValueLen = 3;
        break;

    case CHANNEL_TYPE_GOOSE_QUALITY:
        iValueLen = 5;
        break;

    case CHANNEL_TYPE_GOOSE_TIME:
        iValueLen = 10;
        break;

    case CHANNEL_TYPE_GOOSE_FLOAT:
        iValueLen = 7;
        break;

    case CHANNEL_TYPE_GOOSE_INT8:
    case CHANNEL_TYPE_GOOSE_UINT8:
        iValueLen = 3;
        break;

    case CHANNEL_TYPE_GOOSE_INT16:
    case CHANNEL_TYPE_GOOSE_UINT16:
        iValueLen = 4;
        break;

    case CHANNEL_TYPE_GOOSE_INT24:
    case CHANNEL_TYPE_GOOSE_UINT24:
        iValueLen = 5;
        break;

    case CHANNEL_TYPE_GOOSE_INT32:
    case CHANNEL_TYPE_GOOSE_UINT32:
        iValueLen = 6;
        break;

    default:
        break;
    }

    return iValueLen;
}

///
/// @brief
///    ����SMV<Channel>
///
/// @param[in]  pXml ָ��XML���������
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CChannel::__LoadSmvConfig( CMarkup& xml )
{
    // <Description>
    if (!xml.FindElem(NODE_Channel_Description))
    {
        return false;
    }
    m_strDescription = xml.GetData();

    // <Type>
    if (!xml.FindElem(NODE_Channel_Type))
    {
        return false;
    }
    m_eChannelType = (enumChannelType)_wtoi(xml.GetData().c_str());

    // ͨ���Ƿ����ڲ�������
    if (_wtoi(xml.GetAttrib(NODE_Channel_Measure).c_str()))
    {
        m_bMeasure = true;
    }

    // <Phase>
    if (!xml.FindElem(NODE_Channel_Phase))
    {
        return false;
    }
    m_eChannelPhase = (ChannelPhase)_wtoi(xml.GetData().c_str());

    // <OnceRatings>
    if (!xml.FindElem(NODE_Channel_OnceRatings))
    {
        return false;
    }
    swscanf_s(xml.GetData().c_str(), L"%lf", &(m_dOnceRatings), sizeof(m_dOnceRatings));

    // <SecondaryRatings>
    if (!xml.FindElem(NODE_Channel_SecondaryRatings))
    {
        return false;
    }
    swscanf_s(xml.GetData().c_str(), L"%lf", &(m_dSecondaryRatings), sizeof(m_dSecondaryRatings));

    // <VirtualChannel>
    if (!xml.FindElem(NODE_Channel_VirtualChannel))
    {
        return false;
    }
    m_strVirtualChannel = xml.GetData();

    return true;
}

///
/// @brief
///    ����SMV<Channel>
///
/// @param[in]  pXml ָ��XML���������
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CChannel::__SaveSmvConfig( CMarkup& xml )
{
    wchar_t     cBuffer[128] = {0};


    // <Description>
    if (!xml.AddElem(NODE_Channel_Description) || !xml.SetData(GetDescription()))
    {
        return false;
    }

    // <Type>
    if (!xml.AddElem(NODE_Channel_Type) || !xml.SetData(GetChannelType()))
    {
        return false;
    }

    // <Type measure="0">
    if (!xml.SetAttrib(NODE_Channel_Measure, (IsMeasure()? 1: 0)))
    {
        return false;
    }

    // <Phase>
    if (!xml.AddElem(NODE_Channel_Phase) || !xml.SetData((int)GetChannelPhase()))
    {
        return false;
    }

    // <OnceRatings>
    memset(cBuffer, 0, sizeof(cBuffer));
    swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]) - 1, L"%lf", GetOnceRatings());
    if (!xml.AddElem(NODE_Channel_OnceRatings) || !xml.SetData(cBuffer))
    {
        return false;
    }

    // <SecondaryRatings>
    memset(cBuffer, 0, sizeof(cBuffer));
    swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]) - 1, L"%lf", GetSecondaryRatings());
    if (!xml.AddElem(NODE_Channel_SecondaryRatings) || !xml.SetData(cBuffer))
    {
        return false;
    }

    // <VirtualChannel>
    if (!xml.AddElem(NODE_Channel_VirtualChannel) || !xml.SetData(GetVirtualChannel()))
    {
        return false;
    }

    return true;
}

///
/// @brief
///    ����GOOSE<Channel>
///
/// @param[in]  pXml ָ��XML���������
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CChannel::__LoadGooseConfig( CMarkup& xml )
{
    // <Description>
    if (!xml.FindElem(NODE_Channel_Description))
    {
        return false;
    }
    m_strDescription = xml.GetData();

    // <Type>
    if (!xml.FindElem(NODE_Channel_Type))
    {
        return false;
    }
    m_eChannelType = (enumChannelType)_wtoi(xml.GetData().c_str());

    // ����ͨ�����ͣ����ý���ֵ�����ֵ����
    switch(m_eChannelType)
    {
    case CHANNEL_TYPE_GOOSE_POINT:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_BOOLEAN;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_BOOLEAN;
        }
        break;

    case CHANNEL_TYPE_GOOSE_DOUBLE_POINT:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_BIT_STR;
            m_stGooseSendValue.m_stBitString.SetString(strDPValue[DP_VALUE_INIT]);

            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_BIT_STR;
            m_stGooseRecvValue.m_stBitString.SetString(strDPValue[DP_VALUE_INIT]);
        }
        break;

    case CHANNEL_TYPE_GOOSE_QUALITY:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_BIT_STR;
            m_stGooseSendValue.m_stBitString.SetString(L"0000000000000");

            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_BIT_STR;
            m_stGooseRecvValue.m_stBitString.SetString(L"0000000000000");
        }
        break;

    case CHANNEL_TYPE_GOOSE_TIME:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_UTC_TIME;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_UTC_TIME;
        }
        break;

    case CHANNEL_TYPE_GOOSE_FLOAT:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_FLOAT;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_FLOAT;
        }
        break;

    case CHANNEL_TYPE_GOOSE_INT8:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_INT8;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_INT8;
        }
        break;

    case CHANNEL_TYPE_GOOSE_INT16:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_INT16;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_INT16;
        }
        break;

    case CHANNEL_TYPE_GOOSE_INT24:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_INT24;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_INT24;
        }
        break;

    case CHANNEL_TYPE_GOOSE_INT32:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_INT32;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_INT32;
        }
        break;

    case CHANNEL_TYPE_GOOSE_UINT8:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_UINT8;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_UINT8;
        }
        break;

    case CHANNEL_TYPE_GOOSE_UINT16:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_UINT16;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_UINT16;
        }
        break;

    case CHANNEL_TYPE_GOOSE_UINT24:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_UINT24;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_UINT24;
        }
        break;

    case CHANNEL_TYPE_GOOSE_UINT32:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_UINT32;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_UINT32;
        }
        break;
    }

    // <Value>
    if (xml.FindElem(NODE_Channel_Value))
    {
        int iValueType = _wtoi(xml.GetAttrib(L"type").c_str());
        switch(iValueType)
        {
        case GSEDT_BOOLEAN:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_BOOLEAN;
                if (1 == _wtoi(xml.GetData().c_str()))
                {
                    m_stGooseSendValue.m_stBoolean.boolVal = true;
                } 
                else
                {
                    m_stGooseSendValue.m_stBoolean.boolVal = false;
                }
            }
            break;

        case GSEDT_BIT_STR:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_BIT_STR;
                m_stGooseSendValue.m_stBitString.SetString(xml.GetData());
            }
            break;

        case GSEDT_INT8:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_INT8;
                m_stGooseSendValue.m_stInt8.datVal         = (int8_t)_wtoi(xml.GetData().c_str());
            }
            break;

        case GSEDT_INT16:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_INT16;
                m_stGooseSendValue.m_stInt16.datVal        = (int16_t)_wtoi(xml.GetData().c_str());
            }
            break;

        case GSEDT_INT24:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_INT24;
                m_stGooseSendValue.m_stInt24.datVal        = (int32_t)_wtoi(xml.GetData().c_str());
            }
            break;

        case GSEDT_INT32:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_INT32;
                m_stGooseSendValue.m_stInt32.datVal        = (int32_t)_wtoi(xml.GetData().c_str());
            }
            break;

        case GSEDT_UINT8:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_UINT8;
                m_stGooseSendValue.m_stUInt8.datVal        = (uint8_t)_wtoi(xml.GetData().c_str());
            }
            break;

        case GSEDT_UINT16:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_UINT16;
                m_stGooseSendValue.m_stUInt16.datVal       = (uint16_t)_wtoi(xml.GetData().c_str());
            }
            break;

        case GSEDT_UINT24:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_UINT24;
                m_stGooseSendValue.m_stUInt24.datVal       = _wtoi(xml.GetData().c_str());
            }
            break;

        case GSEDT_UINT32:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_UINT32;
                m_stGooseSendValue.m_stUInt32.datVal       = _wtol(xml.GetData().c_str());
            }
            break;

        case GSEDT_FLOAT:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_FLOAT;
                swscanf_s(
                    xml.GetData().c_str(), 
                    L"%f", 
                    &(m_stGooseSendValue.m_stFloat.datVal), 
                    sizeof(m_stGooseSendValue.m_stFloat.datVal)
                    );
            }
            break;

        case GSEDT_OCTET_STR:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_OCTET_STR;
            }
            break;

        case GSEDT_VISIBLE_STR:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_VISIBLE_STR;
            }
            break;

        case GSEDT_UTC_TIME:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_UTC_TIME;
                m_stGooseSendValue.m_stUtcTime.SetUctTime(xml.GetData());
            }
            break;

        default:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_UNKNOW;
            }
            break;
        }
    }

    // <VirtualChannel>
    if (xml.FindElem(NODE_Channel_VirtualChannel))
    {
        m_strVirtualChannel   = xml.GetData();
    }

    // <SubChannel>
    if (xml.FindElem(NODE_Channel_SubChannel) && xml.IntoElem())
    {
        //
        // find <Channel>
        //
        while(xml.FindElem(NODE_Channel))
        {
            CChannel*   pSubChannel = new CChannel(GetParentBaseCb(), this, m_pParamConfigBase);
            if (NULL == pSubChannel)
            {
                return false;
            }

            // ����ͨ��ѡ��״̬
            int iSelected = _wtoi(xml.GetAttrib(L"selected").c_str());
            if (iSelected != 0)
            {
                pSubChannel->m_bSelected   = true;
            }

            // inside <Channel>
            if (!xml.IntoElem())
            {
                delete pSubChannel;
                return false;
            }

            // ����ͨ��
            if (!pSubChannel->__LoadGooseConfig(xml))
            {
                delete pSubChannel;
                return false;
            }

            // outside <Channel>
            if (!xml.OutOfElem())
            {
                delete pSubChannel;
                return false;
            }

            // ���GOOSE���ƿ����ָ�뵽����
            if (!__AddSubChannel(pSubChannel))
            {
                delete pSubChannel;
                return false;
            }
        }

        // quit <SubChannel>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    ����GOOSE<Channel>
///
/// @param[in]  pXml ָ��XML���������
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CChannel::__SaveGooseConfig( CMarkup& xml )
{
    // <Description>
    if (!xml.AddElem(NODE_Channel_Description) || !xml.SetData(GetDescription()))
    {
        return false;
    }

    // <Type>
    if (!xml.AddElem(NODE_Channel_Type) || !xml.SetData(GetChannelType()))
    {
        return false;
    }

    // <Value>
    GOOSE_DATA_TYPE eValueType  = m_stGooseSendValue.GetType();
    if (GSEDT_UNKNOW != eValueType)
    {
        if (!xml.AddElem(NODE_Channel_Value))
        {
            return false;
        }

        // <Value type="">
        if (!xml.SetAttrib(L"type", eValueType))
        {
            return false;
        }

        // ֵ
        switch(eValueType)
        {
        case GSEDT_BOOLEAN:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stUnKnown.id.type ? 1: 0))
                {
                    return false;
                }
            }
            break;

        case GSEDT_BIT_STR:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stBitString.GetString().c_str()))
                {
                    return false;
                }
            }
            break;

        case GSEDT_INT8:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stInt8.datVal))
                {
                    return false;
                }
            }
            break;

        case GSEDT_INT16:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stInt16.datVal))
                {
                    return false;
                }
            }
            break;

        case GSEDT_INT24:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stInt24.datVal))
                {
                    return false;
                }
            }
            break;

        case GSEDT_INT32:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stInt32.datVal))
                {
                    return false;
                }
            }
            break;

        case GSEDT_UINT8:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stUInt8.datVal))
                {
                    return false;
                }
            }
            break;

        case GSEDT_UINT16:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stUInt16.datVal))
                {
                    return false;
                }
            }
            break;

        case GSEDT_UINT24:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stUInt24.datVal))
                {
                    return false;
                }
            }
            break;

        case GSEDT_UINT32:
            {
                wchar_t     cBuffer[128]    = {0};

                _ltow_s(
                    m_stGooseSendValue.m_stUInt32.datVal,
                    cBuffer,
                    sizeof(cBuffer)/sizeof(cBuffer[0]),
                    10
                    );
                if (!xml.SetData(cBuffer))
                {
                    return false;
                }
            }
            break;

        case GSEDT_FLOAT:
            {
                wchar_t     cBuffer[128]    = {0};


                swprintf_s(
                    cBuffer, 
                    sizeof(cBuffer)/sizeof(cBuffer[0]), 
                    L"%f", 
                    m_stGooseSendValue.m_stFloat.datVal
                    );
                if (!xml.SetData(cBuffer))
                {
                    return false;
                }
            }
            break;

        case GSEDT_OCTET_STR:
            {

            }
            break;

        case GSEDT_VISIBLE_STR:
            {

            }
            break;

        case GSEDT_UTC_TIME:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stUtcTime.GetUtcString().c_str()))
                {
                    return false;
                }
            }
            break;

        default:
            {

            }
            break;
        }
    }

    // <VirtualChannel>
    if (!xml.AddElem(NODE_Channel_VirtualChannel) || !xml.SetData(GetVirtualChannel()))
    {
        return false;
    }

    //
    // ��������������������ͨ��
    //
    if (GetSubChannelCount() > 0)
    {
        // <SubChannel>
        if (!xml.AddElem(NODE_Channel_SubChannel) || !xml.IntoElem())
        {
            return false;
        }

        // ����ͨ��
        CChannel*   pSubChannel    = FirstSubChannel();
        while(pSubChannel)
        {
            // <Channel>
            if (!xml.AddElem(NODE_Channel))
            {
                return false;
            }

            // <Channel selected="">
            int iSelected = pSubChannel->IsSelected()? 1: 0;
            if (!xml.SetAttrib(L"selected", iSelected))
            {
                return false;
            }

            // inside <Channel>
            if (!xml.IntoElem())
            {
                return false;
            }

            // ����Channel
            if (!pSubChannel->__SaveGooseConfig(xml))
            {
                return false;
            }

            // outside <Channel>
            if (!xml.OutOfElem())
            {
                return false;
            }

            pSubChannel = NextSubChannel(pSubChannel);
        }

        // Quit <SubChannel>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    �޸�GOOSE���ƿ��е���/˫������ͨ��ʱ����Ҫ�Զ��޸�����/�����ʱ��ͨ����ֵ
///    �˺����Զ�Ѱ��Ѱ�ұ�λ�ĵ���/˫��ͨ����Ȼ��ı����ڻ�����ʱ��ͨ����ֵ
///
/// @param[in]  stNewTime    ����/˫��ͨ���ı�λʱ��
///
void CChannel::__UpdateSubChannelTime( CEventTime stNewTime )
{
    CChannel*                       pSrcChannel         = NULL;
    CChannel*                       pSubChannel         = NULL;
    CChannel*                       pNextSubChannel     = NULL;
    CChannel*                       pThirdSubChannel    = NULL;
    std::list<CChannel*>::iterator  iter;


    // �������ƿ������ͨ��
    iter = m_listSubChannelGroup.begin();
    while (m_listSubChannelGroup.end() != iter)
    {
        pSubChannel = *iter;

        // ���ұ�λ��ͨ��
        if (pSubChannel && pSubChannel->IsSendValueChanged())
        {
            // �����λ���ǵ��㡢˫�����͵�ͨ��
            if (CHANNEL_TYPE_GOOSE_POINT == pSubChannel->GetChannelType() ||
                CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pSubChannel->GetChannelType())
            {
                // ������ڵ�ͨ����ʱ������
                pNextSubChannel = NextSubChannel(pSubChannel);
                if (pNextSubChannel && CHANNEL_TYPE_GOOSE_TIME == pNextSubChannel->GetChannelType())
                {
                    // �������ڵ�ʱ��ͨ����ֵ
                    GOOSE_DATA_VARIANT& stChannelValue = pNextSubChannel->GetGooseSendValue();
                    stChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                    stChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;

                    // ����Դʱ��ͨ����ֵ
                    pSrcChannel = pNextSubChannel->m_pSourceChannel;
                    if (pSrcChannel)
                    {
                        GOOSE_DATA_VARIANT& stSrcChannelValue = pSrcChannel->GetGooseSendValue();
                        stSrcChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                        stSrcChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;
                    }
                }
                // ������ڵ�ͨ����Ʒ������
                else if (pNextSubChannel && CHANNEL_TYPE_GOOSE_QUALITY == pNextSubChannel->GetChannelType())
                {
                    // �жϼ����ͨ���Ƿ���ʱ������
                    pThirdSubChannel = NextSubChannel(pNextSubChannel);
                    if (pThirdSubChannel && CHANNEL_TYPE_GOOSE_TIME == pThirdSubChannel->GetChannelType())
                    {
                        // ���¼����ʱ��ͨ����ֵ
                        GOOSE_DATA_VARIANT& stChannelValue = pThirdSubChannel->GetGooseSendValue();
                        stChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                        stChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;

                        // ����Դʱ��ͨ����ֵ
                        pSrcChannel = pThirdSubChannel->m_pSourceChannel;
                        if (pSrcChannel)
                        {
                            GOOSE_DATA_VARIANT& stSrcChannelValue = pSrcChannel->GetGooseSendValue();
                            stSrcChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                            stSrcChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;
                        }
                    }
                }
            }
            // �����λ���ǽṹ���͵�ͨ��
            else if (CHANNEL_TYPE_GOOSE_STRUCT == pSubChannel->GetChannelType())
            {
                // ������ͨ����ʱ��
                pSubChannel->__UpdateSubChannelTime(stNewTime);
            }
        }

        iter++;
    }
}

///
/// @brief
///     �Է����˱�λ����ͨ���������±���
///
/// @param[in]  pucFrameHead    ָ����֡֡ͷ��ָ��
///
/// @return
///     bool    �ɹ�����true
///
bool CChannel::__RecodeChangedSubChannels( unsigned char* pucFrameHead )
{
    CChannel*       pSubChannel = NULL;
    std::list<CChannel*>::iterator  iter;


    // �������ƿ������ͨ��
    for (iter = m_listSubChannelGroup.begin(); iter != m_listSubChannelGroup.end(); iter++)
    {
        pSubChannel = *iter;
        if (pSubChannel && pSubChannel->IsSendValueChanged())
        {
            // ����ǵ�������ͨ��
            if (CHANNEL_TYPE_GOOSE_POINT == pSubChannel->GetChannelType())
            {
                // ���ͨ������ֵ
                GOOSE_DATA_VARIANT& stSendValue = pSubChannel->GetGooseSendValue();
                if (GSEDT_BOOLEAN != stSendValue.m_stUnKnown.id.type)
                {
                    return false;
                }

                // �޸Ļ������е���ͨ���ı���ֵ
                stSendValue.CodeGooseData(pucFrameHead + stSendValue.m_stUtcTime.id.offset, 3);
            }
            // �����˫������ͨ��
            else if (CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pSubChannel->GetChannelType())
            {
                // ���ͨ������ֵ
                GOOSE_DATA_VARIANT& stSendValue = pSubChannel->GetGooseSendValue();
                if (GSEDT_BIT_STR != stSendValue.m_stUnKnown.id.type)
                {
                    return false;
                }

                // �޸Ļ�������˫��ͨ���ı���ֵ
                stSendValue.CodeGooseData(pucFrameHead + stSendValue.m_stUtcTime.id.offset, 4);
            }
            // ����ǽṹ������ͨ��
            else if (CHANNEL_TYPE_GOOSE_STRUCT == pSubChannel->GetChannelType())
            {
                if (!pSubChannel->__RecodeChangedSubChannels(pucFrameHead))
                {
                    return false;
                }
            }

            // ��λ��λ��ʶ
            pSubChannel->SetSendValueChanged(false);
        }
    }

    return true;
}

///
/// @brief
///     ����������Ҫ��λ��ʱ����ͨ��������ͨ��ָ�뱣�浽������
///
/// @param[out]  listShouldChangeTimeChannel    ������Ҫ��λʱ��ͨ����ָ��
///
void CChannel::__SaveSubTimeChannel( std::list<CChannel*>& listShouldChangeTimeChannel )
{
    CChannel*       pSubChannel     = NULL;
    CChannel*       pNextSubChannel = NULL;
    CChannel*       pThirdSubChannel= NULL;
    std::list<CChannel*>::iterator  iter;


    // �������ƿ��������ͨ��
    iter = m_listSubChannelGroup.begin();
    while (m_listSubChannelGroup.end() != iter)
    {
        pSubChannel = *iter;

        // ���ұ�λ��ͨ��
        if (pSubChannel && pSubChannel->IsSendValueChanged())
        {
            // �����λ���ǵ��㡢˫�����͵�ͨ��
            if (CHANNEL_TYPE_GOOSE_POINT == pSubChannel->GetChannelType() ||
                CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pSubChannel->GetChannelType())
            {
                // ������ڵ�ͨ����ʱ������
                pNextSubChannel = NextSubChannel(pSubChannel);
                if (pNextSubChannel && CHANNEL_TYPE_GOOSE_TIME == pNextSubChannel->GetChannelType())
                {
                    // ��¼��Ҫ��λ��ʱ��ͨ��
                    listShouldChangeTimeChannel.push_back(pNextSubChannel);
                }
                // ������ڵ�ͨ����Ʒ������
                else if (pNextSubChannel && CHANNEL_TYPE_GOOSE_QUALITY == pNextSubChannel->GetChannelType())
                {
                    // �жϼ����ͨ���Ƿ���ʱ������
                    pThirdSubChannel = NextSubChannel(pNextSubChannel);
                    if (pThirdSubChannel && CHANNEL_TYPE_GOOSE_TIME == pThirdSubChannel->GetChannelType())
                    {
                        // ��¼��Ҫ��λ��ʱ��ͨ��
                        listShouldChangeTimeChannel.push_back(pThirdSubChannel);
                    }
                }
            }
            // �����λ���ǽṹ���͵�ͨ��
            else if (CHANNEL_TYPE_GOOSE_STRUCT == pSubChannel->GetChannelType())
            {
                // ����������Ҫ��λ��ʱ����ͨ��
                pSubChannel->__SaveSubTimeChannel(listShouldChangeTimeChannel);
            }
        }

        iter++;
    }
}

///
/// @brief
///    ����ͨ����ƫ����
///
/// @param[in]  pGooseData      ����GOOSEͨ����Ϣ�Ľṹ��
///
/// @return
///     bool    �ɹ�����true
///
bool CChannel::__CalcChannelOffset( GOOSE_DATA* pGooseData )
{
    unsigned int    nIndex          = 0;
    GOOSE_STRUCT*   pGooseStruct    = NULL;


    // ����������
    if (NULL == pGooseData)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CChannel::__CalcChannelOffset(): Parameter error \n"));
        return false;
    }

    // ����ǽṹ��ͨ������ô����ÿһ����ͨ����ƫ����
    if (CHANNEL_TYPE_GOOSE_STRUCT == m_eChannelType)
    {
        // ת��Ϊ�ṹ�����͵�ָ��
        pGooseStruct = (GOOSE_STRUCT*)pGooseData;

        // ����ÿһ����ͨ��
        std::list<CChannel *>::iterator iterChannel = m_listSubChannelGroup.begin();
        for (nIndex = 0; nIndex < pGooseStruct->member.size() && iterChannel != m_listSubChannelGroup.end(); nIndex++, iterChannel++)
        {
            CChannel* pSubChannel = *iterChannel;
            if (NULL == pSubChannel)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CChannel::__CalcChannelOffset(): get sub channel failed \n"));
                return false;
            }

            // �����ͨ����ƫ����
            GOOSE_DATA* pSubGooseData = pGooseStruct->member.at(nIndex);
            if (NULL == pSubGooseData)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CChannel::__CalcChannelOffset(): get sub channel info failed \n"));
                return false;
            }

            // �����ͨ������
            if ((GSEDT_STRUCT == pSubGooseData->type) || (CHANNEL_TYPE_GOOSE_STRUCT == pSubChannel->GetChannelType()))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CChannel::__CalcChannelOffset(): sub channel type error \n"));
                return false;
            }

            // ����ͨ��ƫ������GOOSE����ֵ�ṹ����
            pSubChannel->m_stGooseSendValue.m_stUnKnown.id.offset = pSubGooseData->offset;

            // �޸�Դͨ����ƫ����
            if (pSubChannel->m_pSourceChannel)
            {
                pSubChannel->m_pSourceChannel->m_stGooseSendValue.m_stUnKnown.id.offset = pSubGooseData->offset;
            }
        }
    }
    // �������ͨͨ������ô����ͨ����ƫ����
    else
    {
        // ����ͨ��ƫ������GOOSE����ֵ�ṹ����
        m_stGooseSendValue.m_stUnKnown.id.offset = pGooseData->offset;

        // �޸�Դͨ����ƫ����
        if (m_pSourceChannel)
        {
            m_pSourceChannel->m_stGooseSendValue.m_stUnKnown.id.offset = pGooseData->offset;
        }
    }

    return true;
}
