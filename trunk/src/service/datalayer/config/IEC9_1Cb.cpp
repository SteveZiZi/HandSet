/// @file
///
/// @brief
///     IEC61850 9-1 ���ƿ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.29
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "IEC9_1Cb.h"

///
/// @brief
///    ���캯��
///
CIEC9_1Cb::CIEC9_1Cb(void)
{
}

///
/// @brief
///     ���캯��
///
/// @param[in]  
///     pParamConfigBase    ָ��������ù�������ָ��
///
CIEC9_1Cb::CIEC9_1Cb(CParamConfigBase* pParamConfigBase)
:CNetCb(pParamConfigBase)
,m_iRatedDelay(750)
,m_uiStatusWord1(0)
,m_uiStatusWord2(0)
{
    m_eFrameType    = FRAME_TYPE_NET_9_1;
    m_ePortName     = NET_PORT_LIGHT_A;

    // ��ʼ��������������ߵ�������ߵ�ѹ
    CParamConfigBasic*  pParamConfigBasic  = CParamConfigBasic::getInstance();
    if (pParamConfigBasic)
    {
        m_dRatedPhaseCurrent    = pParamConfigBasic->GetRatedPrimaryCurrent();
        m_dRatedNeutralCurrent  = pParamConfigBasic->GetRatedPrimaryCurrent();
        m_dRatedPhaseVoltage    = pParamConfigBasic->GetRatedPrimaryVoltage();
    }
}

///
/// @brief
///    ��������
///
CIEC9_1Cb::~CIEC9_1Cb(void)
{
}

///
/// @brief
///    ��ö�������A��
///
/// @return
///     double    ��������A��
///
double CIEC9_1Cb::GetRatedPhaseCurrent()
{
    return m_dRatedPhaseCurrent;
}

///
/// @brief
///    ���ö�������A��
///
/// @param[in]  dRatedPhaseCurrent ��������A��
///
void CIEC9_1Cb::SetRatedPhaseCurrent(double dRatedPhaseCurrent)
{
    if (fabs(m_dRatedPhaseCurrent - dRatedPhaseCurrent) >= FLOAT_PRECISION)
    {
        m_dRatedPhaseCurrent = dRatedPhaseCurrent;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ��ö���ߵ�����A��
///
/// @return
///     double    ����ߵ�����A��
///
double CIEC9_1Cb::GetRatedNeutralCurrent()
{
    return m_dRatedNeutralCurrent;
}

///
/// @brief
///    ���ö���ߵ�����A��
///
/// @param[in]  dRatedNeutralCurrent ����ߵ�����A��
///
void CIEC9_1Cb::SetRatedNeutralCurrent(double dRatedNeutralCurrent)
{
    if (fabs(m_dRatedNeutralCurrent - dRatedNeutralCurrent) >= FLOAT_PRECISION)
    {
        m_dRatedNeutralCurrent = dRatedNeutralCurrent;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ��ö���ѹ��kV��
///
/// @return
///     double    ����ѹ��kV��
///
double CIEC9_1Cb::GetRatedPhaseVoltage()
{
    return m_dRatedPhaseVoltage;
}

///
/// @brief
///    ���ö���ѹ��kV��
///
/// @param[in]  dRatedPhaseVoltage ����ѹ��kV��
///
void CIEC9_1Cb::SetRatedPhaseVoltage(double dRatedPhaseVoltage)
{
    if (fabs(m_dRatedPhaseVoltage - dRatedPhaseVoltage) >= FLOAT_PRECISION)
    {
        m_dRatedPhaseVoltage = dRatedPhaseVoltage;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ��ö��ʱ��us��
///
/// @return
///     int    ���ʱ��us��
///
int CIEC9_1Cb::GetRatedDelay()
{
    return m_iRatedDelay;
}

///
/// @brief
///    ���ö��ʱ��us��
///
/// @param[in]  iRatedDelay ���ʱ��us��
///
void CIEC9_1Cb::SetRatedDelay(int iRatedDelay)
{
    // ����������
    if (iRatedDelay < 0)
    {
        return ;
    }

    if (m_iRatedDelay != iRatedDelay)
    {
        m_iRatedDelay = iRatedDelay;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���״̬��#1
///
/// @return
///     unsigned short    ״̬��#1
///
unsigned short CIEC9_1Cb::GetStatusWord1()
{
    return m_uiStatusWord1;
}

///
/// @brief
///    ����״̬��#1
///
/// @param[in]  nStatusWord1 ״̬��#1
///
void CIEC9_1Cb::SetStatusWord1(unsigned short nStatusWord1)
{
    if (m_uiStatusWord1 != nStatusWord1)
    {
        m_uiStatusWord1 = nStatusWord1;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
            m_pParamConfigBase->SetStatusWordChanged(true);
        }
    }
}

///
/// @brief
///    ���״̬��#2
///
/// @return
///     unsigned short    ״̬��#2
///
unsigned short CIEC9_1Cb::GetStatusWord2()
{
    return m_uiStatusWord2;
}

///
/// @brief
///    ����״̬��#2
///
/// @param[in]  nStatusWord2 ״̬��#2
///
void CIEC9_1Cb::SetStatusWord2(unsigned short nStatusWord2)
{
    if (m_uiStatusWord2 != nStatusWord2)
    {
        m_uiStatusWord2 = nStatusWord2;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
            m_pParamConfigBase->SetStatusWordChanged(true);
        }
    }
}

///
/// @brief
///    ��ȿ���һ��ɨ���б��е�IEC61850-9-1���ƿ�
///
/// @param[in]
///     Sav91Msg    -   IEC61850-9-1���ĵı�������
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CIEC9_1Cb::__Copy(Sav91Msg* pSav91Msg)
{
    SAV91_ASDU_PUBLIC    stSav91PublicInfo;


    // ����������
    if (NULL == pSav91Msg)
    {
        return false;
    }

    // �������������
    if (!CNetCb::__Copy(pSav91Msg))
    {
        return false;
    }

    // ��ȡ����֡�е�GoosePdu�Ĺ�����Ϣ
    if (!pSav91Msg->GetASDUPublicInfo(0, stSav91PublicInfo))
    {
        return false;
    }

    // ����CGooseCb������
    m_dRatedPhaseCurrent        = stSav91PublicInfo.phsArtg;            ///< ��������A��
    m_dRatedNeutralCurrent      = stSav91PublicInfo.neutArtg;           ///< ����ߵ�����A��
    m_dRatedPhaseVoltage        = stSav91PublicInfo.phsVrtg/10;         ///< ����ѹ��kV��
    m_iRatedDelay               = stSav91PublicInfo.delayTime;          ///< ���ʱ��us��
    m_uiStatusWord1             = stSav91PublicInfo.statusWord1;        ///< ״̬��#1
    m_uiStatusWord2             = stSav91PublicInfo.statusWord2;        ///< ״̬��#2
    m_iConfigRevision           = stSav91PublicInfo.confRev;            ///< ���ð汾��

    return true;
}

///
/// @brief
///    ��ȿ���һ��IEC61850-9-1���ƿ�
///
/// @param[in]
///     pIEC9_1Cb    -   IEC61850-9-1���ƿ�
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CIEC9_1Cb::__Copy( CIEC9_1Cb* pIEC9_1Cb )
{
    // ����������
    if (NULL == pIEC9_1Cb)
    {
        return false;
    }

    // �������������
    if (!CNetCb::__Copy(pIEC9_1Cb))
    {
        return false;
    }

    m_dRatedPhaseCurrent        = pIEC9_1Cb->m_dRatedPhaseCurrent;      ///< ��������A��
    m_dRatedNeutralCurrent      = pIEC9_1Cb->m_dRatedNeutralCurrent;    ///< ����ߵ�����A��
    m_dRatedPhaseVoltage        = pIEC9_1Cb->m_dRatedPhaseVoltage;      ///< ����ѹ��kV��
    m_iRatedDelay               = pIEC9_1Cb->m_iRatedDelay;             ///< ���ʱ��us��
    m_uiStatusWord1             = pIEC9_1Cb->m_uiStatusWord1;           ///< ״̬��#1
    m_uiStatusWord2             = pIEC9_1Cb->m_uiStatusWord2;           ///< ״̬��#2

    return true;
}

///
/// @brief
///    ʹ��ָ����ͨ��ģ���������ƿ�
///
/// @param[in]  eType    ģ������
///
/// @return
///     bool    �ɹ�����true
///
bool CIEC9_1Cb::UseChannelTemplate( enumSmvRecvConfigTemplateType eType )
{
    // ����ָ������ʹ�ò�ͬ��ģ������
    switch(eType)
    {
    case SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_BODIAN:
        {
            // �������ͨ��
            ClearAllChannel();

            // ���Ĭ��ͨ��
            for (int i = 0; i < sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_BODIAN)/ sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_BODIAN[0]); i++)
            {
                CChannel* pChannel = AddChannel();
                if (NULL == pChannel)
                {
                    return false;
                }

                // ����ͨ��Ĭ��ֵ
                pChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_FT3_STD_BODIAN[i].enChannelType);
                pChannel->m_eChannelPhase       = PREDEFINED_CHANNEL_INFO_FT3_STD_BODIAN[i].enChannelPhase;
                pChannel->m_bMeasure            = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_FT3_STD_BODIAN[i].enChannelValueType);
                pChannel->m_strDescription      = PREDEFINED_CHANNEL_INFO_FT3_STD_BODIAN[i].strChannelDesc;
                pChannel->m_strVirtualChannel   = PREDEFINED_CHANNEL_INFO_FT3_STD_BODIAN[i].strVirtualChannel;
            }
        }
        break;

    case SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_KAIMO:
        {
            // �������ͨ��
            ClearAllChannel();

            // ���Ĭ��ͨ��
            for (int i = 0; i < sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_KAIMO)/ sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_KAIMO[0]); i++)
            {
                CChannel* pChannel = AddChannel();
                if (NULL == pChannel)
                {
                    return false;
                }

                // ����ͨ��Ĭ��ֵ
                pChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_FT3_STD_KAIMO[i].enChannelType);
                pChannel->m_eChannelPhase       = PREDEFINED_CHANNEL_INFO_FT3_STD_KAIMO[i].enChannelPhase;
                pChannel->m_bMeasure            = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_FT3_STD_KAIMO[i].enChannelValueType);
                pChannel->m_strDescription      = PREDEFINED_CHANNEL_INFO_FT3_STD_KAIMO[i].strChannelDesc;
                pChannel->m_strVirtualChannel   = PREDEFINED_CHANNEL_INFO_FT3_STD_KAIMO[i].strVirtualChannel;
            }
        }
        break;

    case SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_60044_8:
        {
            // �������ͨ��
            ClearAllChannel();

            // ���Ĭ��ͨ��
            for (int i = 0; i < sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8)/ sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[0]); i++)
            {
                CChannel* pChannel = AddChannel();
                if (NULL == pChannel)
                {
                    return false;
                }

                // ����ͨ��Ĭ��ֵ
                pChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelType);
                pChannel->m_eChannelPhase       = PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelPhase;
                pChannel->m_bMeasure            = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelValueType);
                pChannel->m_strDescription      = PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].strChannelDesc;
                pChannel->m_strVirtualChannel   = PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].strVirtualChannel;
            }
        }
        break;

    default:
        return false;
    }

    return true;
}

///
/// @brief
///    ���¿��ƿ������е�ѹͨ����һ�ζֵ
///
/// @param[in]  dNewValue    �µĵ�ѹһ�ζֵ��V��
///
void CIEC9_1Cb::UpdateRatedPrimaryVoltage( double dNewValue )
{
    // ���»���ĵ�ѹ�ֵ
    CBaseCb::UpdateRatedPrimaryVoltage(dNewValue);

    // ���¶�ߵ�ѹ
    SetRatedPhaseVoltage(dNewValue / 1000.0f);
}

///
/// @brief
///    ���¿��ƿ������е���ͨ����һ�ζֵ
///
/// @param[in]  dNewValue    �µĵ���һ�ζֵ��A��
///
void CIEC9_1Cb::UpdateRatedPrimaryCurrent( double dNewValue )
{
    // ���»���ĵ����ֵ
    CBaseCb::UpdateRatedPrimaryCurrent(dNewValue);

    // ���¶�����������ߵ���
    SetRatedPhaseCurrent(dNewValue);
    SetRatedNeutralCurrent(dNewValue);
}

///
/// @brief
///    ����IEC61850-9-1���ƿ�
///
/// @param[in]  pXml ָ��XML���������
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CIEC9_1Cb::__LoadConfig( CMarkup& xml )
{
    // <Description>
    if (!xml.FindElem(NODE_SmvCb_Description))
    {
        return false;
    }
    m_strDescription = xml.GetData();

    // <PortName>
    if (!xml.FindElem(NODE_SmvCb_PortName))
    {
        return false;
    }
    m_ePortName = (DsmPortName)_wtoi(xml.GetData().c_str());

#if !CHUN_HUA
	// <VLanTag>
	if (!xml.FindElem(NODE_Gocb_VLanTag))
	{
		return false;
	}
	m_bHasVlanFlag = _wtoi(xml.GetData().c_str());
#endif

    // <APPID>
    if (!xml.FindElem(NODE_SmvCb_APPID))
    {
        return false;
    }
    m_iAPPID = _wtoi(xml.GetData().c_str());

    // <SrcMAC>
    unsigned char cBuffer[ETHERNET_MAC_LEN] = {'0'};
    if (!xml.FindElem(NODE_SmvCb_SRC_MAC))
    {
        return false;
    }
    ConvMacWStr2Char(xml.GetData(), cBuffer, ETHERNET_MAC_LEN);
    memcpy_s(m_ucSrcMAC, ETHERNET_MAC_LEN, cBuffer, ETHERNET_MAC_LEN);

    // <DesMAC>
    if (!xml.FindElem(NODE_SmvCb_DES_MAC))
    {
        return false;
    }
    ConvMacWStr2Char(xml.GetData(), cBuffer, ETHERNET_MAC_LEN);
    memcpy_s(m_ucDesMAC, ETHERNET_MAC_LEN, cBuffer, ETHERNET_MAC_LEN);

    // <VLanID>
    if (!xml.FindElem(NODE_SmvCb_VLanID))
    {
        return false;
    }
    m_iVLanID = _wtoi(xml.GetData().c_str());

    // <VLanPriority>
    if (!xml.FindElem(NODE_SmvCb_VLanPriority))
    {
        return false;
    }
    m_iVLanPriority = _wtoi(xml.GetData().c_str());

    // <ConfigRevision>
    if (!xml.FindElem(NODE_SmvCb_ConfigRevision))
    {
        return false;
    }
    m_iConfigRevision = _wtoi(xml.GetData().c_str());

    // <RatedPhaseCurrent>
    if (!xml.FindElem(NODE_SmvCb_RatedPhaseCurrent))
    {
        return false;
    }
    swscanf_s(xml.GetData().c_str(), L"%lf", &(m_dRatedPhaseCurrent), sizeof(m_dRatedPhaseCurrent));

    // <RatedNeutralCurrent>
    if (!xml.FindElem(NODE_SmvCb_RatedNeutralCurrent))
    {
        return false;
    }
    swscanf_s(xml.GetData().c_str(), L"%lf", &(m_dRatedNeutralCurrent), sizeof(m_dRatedNeutralCurrent));

    // <RatedPhaseVoltage>
    if (!xml.FindElem(NODE_SmvCb_RatedPhaseVoltage))
    {
        return false;
    }
    swscanf_s(xml.GetData().c_str(), L"%lf", &(m_dRatedPhaseVoltage), sizeof(m_dRatedPhaseVoltage));

    // <RatedDelay>
    if (!xml.FindElem(NODE_SmvCb_RatedDelay))
    {
        return false;
    }
    m_iRatedDelay = _wtoi(xml.GetData().c_str());

    // <StatusWord1>
    if (!xml.FindElem(NODE_SmvCb_StatusWord1))
    {
        return false;
    }
    m_uiStatusWord1 = (short)_wtoi(xml.GetData().c_str());

    // <StatusWord2>
    if (!xml.FindElem(NODE_SmvCb_StatusWord2))
    {
        return false;
    }
    m_uiStatusWord2 = (short)_wtoi(xml.GetData().c_str());

    // <ChannelGroup>
    if (xml.FindElem(NODE_SmvCb_ChannelGroup) && xml.IntoElem())
    {
        //
        // find <Channel>
        //
        while(xml.FindElem(NODE_Channel))
        {
            CChannel*   pChannel = new CChannel(this, NULL, m_pParamConfigBase);
            if (NULL == pChannel)
            {
                return false;
            }

            // inside <Channel>
            if (!xml.IntoElem())
            {
                delete pChannel;
                return false;
            }

            // ����ͨ��
            if (!pChannel->__LoadSmvConfig(xml))
            {
                delete pChannel;
                return false;
            }

            // outside <Channel>
            if (!xml.OutOfElem())
            {
                delete pChannel;
                return false;
            }

            // ���GOOSE���ƿ����ָ�뵽����
            if (!__AddChannel(pChannel))
            {
                delete pChannel;
                return false;
            }
        }

        // quit <ChannelGroup>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    ����IEC61850-9-1���ƿ�
///
/// @param[in]  pXml ָ��XML���������
///
/// @return
///     bool    true��ʾ����ɹ�
///
bool CIEC9_1Cb::__SaveConfig( CMarkup& xml )
{
    wchar_t     cBuffer[128] = {0};


    // <Description>
    if (!xml.AddElem(NODE_SmvCb_Description) || !xml.SetData(GetDescription()))
    {
        return false;
    }

    // <PortName>
    if (!xml.AddElem(NODE_SmvCb_PortName) || !xml.SetData((int)GetPortName()))
    {
        return false;
    }

#if !CHUN_HUA
	// <VLanTag>
	if (!xml.AddElem(NODE_Gocb_VLanTag) || !xml.SetData(GetVlanFlag()))
	{
		return false;
	}
#endif

    // <APPID>
    if (!xml.AddElem(NODE_SmvCb_APPID) || !xml.SetData(GetAPPID()))
    {
        return false;
    }

    // <SrcMAC>
    unsigned char cMacBuffer[ETHERNET_MAC_LEN] = {'0'};
    GetSrcMAC(cMacBuffer, ETHERNET_MAC_LEN);
    if (!xml.AddElem(NODE_SmvCb_SRC_MAC) || !xml.SetData(ConvMacChar2WStr(cMacBuffer, ETHERNET_MAC_LEN)))
    {
        return false;
    }

    // <DesMAC>
    GetDesMAC(cMacBuffer, ETHERNET_MAC_LEN);
    if (!xml.AddElem(NODE_SmvCb_DES_MAC) || !xml.SetData(ConvMacChar2WStr(cMacBuffer, ETHERNET_MAC_LEN)))
    {
        return false;
    }

    // <VLanID>
    if (!xml.AddElem(NODE_SmvCb_VLanID) || !xml.SetData(GetVlanID()))
    {
        return false;
    }

    // <VLanPriority>
    if (!xml.AddElem(NODE_SmvCb_VLanPriority) || !xml.SetData(GetVLanPriority()))
    {
        return false;
    }

    // <ConfigRevision>
    if (!xml.AddElem(NODE_SmvCb_ConfigRevision) || !xml.SetData(GetConfigRevision()))
    {
        return false;
    }

    // <RatedPhaseCurrent>
    memset(cBuffer, 0, sizeof(cBuffer));
    swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]) - 1, L"%lf", GetRatedPhaseCurrent());
    if (!xml.AddElem(NODE_SmvCb_RatedPhaseCurrent) || !xml.SetData(cBuffer))
    {
        return false;
    }

    // <RatedNeutralCurrent>
    memset(cBuffer, 0, sizeof(cBuffer));
    swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]) - 1, L"%lf", GetRatedNeutralCurrent());
    if (!xml.AddElem(NODE_SmvCb_RatedNeutralCurrent) || !xml.SetData(cBuffer))
    {
        return false;
    }

    // <RatedPhaseVoltage>
    memset(cBuffer, 0, sizeof(cBuffer));
    swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]) - 1, L"%lf", GetRatedPhaseVoltage());
    if (!xml.AddElem(NODE_SmvCb_RatedPhaseVoltage) || !xml.SetData(cBuffer))
    {
        return false;
    }

    // <RatedDelay>
    if (!xml.AddElem(NODE_SmvCb_RatedDelay) || !xml.SetData(GetRatedDelay()))
    {
        return false;
    }

    // <StatusWord1>
    if (!xml.AddElem(NODE_SmvCb_StatusWord1) || !xml.SetData(GetStatusWord1()))
    {
        return false;
    }

    // <StatusWord2>
    if (!xml.AddElem(NODE_SmvCb_StatusWord2) || !xml.SetData(GetStatusWord2()))
    {
        return false;
    }

    // ͨ��
    if (GetChannelCount())
    {
        // <ChannelGroup>
        if (!xml.AddElem(NODE_SmvCb_ChannelGroup) || !xml.IntoElem())
        {
            return false;
        }

        // ����ͨ��
        CChannel*   pChannel    = FirstChannel();
        while(pChannel)
        {
            // <Channel>
            if (!xml.AddElem(NODE_Channel))
            {
                return false;
            }

            // inside <Channel>
            if (!xml.IntoElem())
            {
                return false;
            }

            // ����Channel
            if (!pChannel->__SaveSmvConfig(xml))
            {
                return false;
            }

            // outside <Channel>
            if (!xml.OutOfElem())
            {
                return false;
            }

            pChannel = NextChannel(pChannel);
        }

        // Quit <ChannelGroup>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    return true;
}
