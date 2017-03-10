///
/// @brief
///     FT3���ƿ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.29
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "FT3Cb.h"

///
/// @brief
///    ���캯��
///
CFT3Cb::CFT3Cb(void)
{
}

///
/// @brief
///     ���캯��
///
/// @param[in]  
///     pParamConfigBase    ָ��������ù�������ָ��
///
CFT3Cb::CFT3Cb(CParamConfigBase* pParamConfigBase)
:CBaseCb(pParamConfigBase)
,m_iLNName(2)
,m_iDSName(1)
,m_iRatedDelay(750)
,m_uiStatusWord1(0)
,m_uiStatusWord2(0)
{
    m_eFrameType    = FRAME_TYPE_FT3_NANRUI;
    m_ePortName     = FT3_PORT_LIGHT_C;

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
CFT3Cb::~CFT3Cb(void)
{
}

///
/// @brief
///    ���LNName
///
/// @return
///     int    LNName
///
int CFT3Cb::GetLNName()
{
    return m_iLNName;
}

///
/// @brief
///    ����LNName
///
/// @param[in]  int LNName
///
void CFT3Cb::SetLNName(int iLNName)
{
    // ����������
    if ((iLNName < 0) || (iLNName > 255))
    {
        return ;
    }

    if (m_iLNName != iLNName)
    {
        m_iLNName = iLNName;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���LDName
///
/// @return
///     int    LDName
///
int CFT3Cb::GetLDName()
{
    return GetAPPID();
}

///
/// @brief
///    ����LDName
///
/// @param[in]  int iLDName
///
/// @return
///     bool    true��ʾ����ɹ�
///
bool CFT3Cb::SetLDName(int iLDName)
{
    if (!SetAPPID(iLDName))
    {
        return false;
    }

    // ���Ϊ���޸�
    if (NULL != m_pParamConfigBase)
    {
        m_pParamConfigBase->SetModified(true);
    }

    return true;
}

///
/// @brief
///    ���DSName
///
/// @return
///     int    DSName
///
int CFT3Cb::GetDSName()
{
    return m_iDSName;
}

///
/// @brief
///    ����DSName
///
/// @param[in]  int iDSName
///
void CFT3Cb::SetDSName(int iDSName)
{
    // ����������
    if ((iDSName < 0) || (iDSName > 255))
    {
        return ;
    }

    if (m_iDSName != iDSName)
    {
        m_iDSName = iDSName;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ��ö�������A��
///
/// @return
///     double    ��������A��
///
double CFT3Cb::GetRatedPhaseCurrent()
{
    return m_dRatedPhaseCurrent;
}

///
/// @brief
///    ���ö�������A��
///
/// @param[in]  dRatedPhaseCurrent ��������A��
///
void CFT3Cb::SetRatedPhaseCurrent(double dRatedPhaseCurrent)
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
double CFT3Cb::GetRatedNeutralCurrent()
{
    return m_dRatedNeutralCurrent;
}

///
/// @brief
///    ���ö���ߵ�����A��
///
/// @param[in]  dRatedNeutralCurrent ����ߵ�����A��
///
void CFT3Cb::SetRatedNeutralCurrent(double dRatedNeutralCurrent)
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
double CFT3Cb::GetRatedPhaseVoltage()
{
    return m_dRatedPhaseVoltage;
}

///
/// @brief
///    ���ö���ѹ��kV��
///
/// @param[in]  dRatedPhaseVoltage ����ѹ��kV��
///
void CFT3Cb::SetRatedPhaseVoltage(double dRatedPhaseVoltage)
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
int CFT3Cb::GetRatedDelay()
{
    return m_iRatedDelay;
}

///
/// @brief
///    ���ö��ʱ��us��
///
/// @param[in]  iRatedDelay ���ʱ��us��
///
void CFT3Cb::SetRatedDelay(int iRatedDelay)
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
unsigned short CFT3Cb::GetStatusWord1()
{
    return m_uiStatusWord1;
}

///
/// @brief
///    ����״̬��#1
///
/// @param[in]  nStatusWord1 ״̬��#1
///
void CFT3Cb::SetStatusWord1(unsigned short nStatusWord1)
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
unsigned short CFT3Cb::GetStatusWord2()
{
    return m_uiStatusWord2;
}

///
/// @brief
///    ����״̬��#2
///
/// @param[in]  nStatusWord2 ״̬��#2
///
void CFT3Cb::SetStatusWord2(unsigned short nStatusWord2)
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
///    ��ȿ���һ��FT3���ƿ�
///
/// @param[in]
///     pFt3Cb    -   FT3���ƿ�
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CFT3Cb::__Copy( CFT3Cb* pFt3Cb )
{
    // ����������
    if (NULL == pFt3Cb)
    {
        return false;
    }

    // �������������
    if (!CBaseCb::__Copy(pFt3Cb))
    {
        return false;
    }

    m_iLNName                   = pFt3Cb->m_iLNName;                    ///< LNName
    m_iDSName                   = pFt3Cb->m_iDSName;                    ///< DSName

    m_dRatedPhaseCurrent        = pFt3Cb->m_dRatedPhaseCurrent;         ///< ��������A��
    m_dRatedNeutralCurrent      = pFt3Cb->m_dRatedNeutralCurrent;       ///< ����ߵ�����A��
    m_dRatedPhaseVoltage        = pFt3Cb->m_dRatedPhaseVoltage;         ///< ����ѹ��kV��
    m_iRatedDelay               = pFt3Cb->m_iRatedDelay;                ///< ���ʱ��us��
    m_uiStatusWord1             = pFt3Cb->m_uiStatusWord1;              ///< ״̬��#1
    m_uiStatusWord2             = pFt3Cb->m_uiStatusWord2;              ///< ״̬��#2

    return true;
}

///
/// @brief
///    ��ȿ���һ��ɨ���б��е�FT3���ƿ�
///
/// @param[in]
///     pFt3Msg    -   FT3���ĵı�������
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CFT3Cb::__Copy(NanRuiFT3Msg* pFt3Msg)
{
    NANRUI_FT3_ASDU_PUBLIC    stFt3PublicInfo;


    // ����������
    if (NULL == pFt3Msg)
    {
        return false;
    }

    // ��ȡ����֡�е�GoosePdu�Ĺ�����Ϣ
    if (!pFt3Msg->GetASDUPublicInfo(stFt3PublicInfo))
    {
        return false;
    }

    // ����CGooseCb������
    m_iLNName                   = stFt3PublicInfo.lnName;               ///< LNName
    m_iAPPID                    = stFt3PublicInfo.ldName;               ///< LDName
    m_iDSName                   = stFt3PublicInfo.dataSetName;          ///< DSName

    m_dRatedPhaseCurrent        = stFt3PublicInfo.phsArtg;              ///< ��������A��
    m_dRatedNeutralCurrent      = stFt3PublicInfo.neutArtg;             ///< ����ߵ�����A��
    m_dRatedPhaseVoltage        = stFt3PublicInfo.phsVrtg/10;           ///< ����ѹ��kV��
    m_iRatedDelay               = stFt3PublicInfo.delayTime;            ///< ���ʱ��us��
    m_uiStatusWord1             = stFt3PublicInfo.statusWord1;          ///< ״̬��#1
    m_uiStatusWord2             = stFt3PublicInfo.statusWord2;          ///< ״̬��#2

    // �������������
    if (!CBaseCb::__Copy(pFt3Msg))
    {
        return false;
    }

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
bool CFT3Cb::UseChannelTemplate( enumSmvRecvConfigTemplateType eType )
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
void CFT3Cb::UpdateRatedPrimaryVoltage( double dNewValue )
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
void CFT3Cb::UpdateRatedPrimaryCurrent( double dNewValue )
{
    // ���»���ĵ����ֵ
    CBaseCb::UpdateRatedPrimaryCurrent(dNewValue);

    // ���¶�����������ߵ���
    SetRatedPhaseCurrent(dNewValue);
    SetRatedNeutralCurrent(dNewValue);
}

///
/// @brief
///    ����FT3���ƿ�
///
/// @param[in]  pXml ָ��XML���������
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CFT3Cb::__LoadConfig( CMarkup& xml )
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

    // <LNName>
    if (!xml.FindElem(NODE_SmvCb_LNName))
    {
        return false;
    }
    m_iLNName = _wtoi(xml.GetData().c_str());

    // <LDName>��APPID
    if (!xml.FindElem(NODE_SmvCb_LDName))
    {
        return false;
    }
    m_iAPPID = _wtoi(xml.GetData().c_str());

    // <DSName>
    if (!xml.FindElem(NODE_SmvCb_DSName))
    {
        return false;
    }
    m_iDSName = _wtoi(xml.GetData().c_str());

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
    m_uiStatusWord1 = (unsigned short)_wtoi(xml.GetData().c_str());

    // <StatusWord2>
    if (!xml.FindElem(NODE_SmvCb_StatusWord2))
    {
        return false;
    }
    m_uiStatusWord2 = (unsigned short)_wtoi(xml.GetData().c_str());

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
///    ����FT3���ƿ�
///
/// @param[in]  pXml ָ��XML���������
///
/// @return
///     bool    true��ʾ����ɹ�
///
bool CFT3Cb::__SaveConfig( CMarkup& xml )
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

    // <LNName>
    if (!xml.AddElem(NODE_SmvCb_LNName) || !xml.SetData((int)GetLNName()))
    {
        return false;
    }

    // <LDName>
    if (!xml.AddElem(NODE_SmvCb_LDName) || !xml.SetData((int)GetLDName()))
    {
        return false;
    }

    // <DSName>
    if (!xml.AddElem(NODE_SmvCb_DSName) || !xml.SetData((int)GetDSName()))
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

///
/// @brief
///     ���캯��
///
/// @param[in]  
///     pParamConfigBase    ָ��������ù�������ָ��
///
CFT3ExpandCb::CFT3ExpandCb( CParamConfigBase* pParamConfigBase )
: CFT3Cb(pParamConfigBase)
{
    m_iDSName = 0xFE;
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
bool CFT3ExpandCb::UseChannelTemplate( enumSmvRecvConfigTemplateType eType )
{
    // ����ָ������ʹ�ò�ͬ��ģ������
    switch(eType)
    {
    case SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BODIAN:
        {
            // �������ͨ��
            ClearAllChannel();

            // ���Ĭ��ͨ��
            for (int i = 0; i < sizeof(PREDEFINED_CHANNEL_INFO_FT3_EXT_BODIAN)/ sizeof(PREDEFINED_CHANNEL_INFO_FT3_EXT_BODIAN[0]); i++)
            {
                CChannel* pChannel = AddChannel();
                if (NULL == pChannel)
                {
                    return false;
                }

                // ����ͨ��Ĭ��ֵ
                pChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_FT3_EXT_BODIAN[i].enChannelType);
                pChannel->m_eChannelPhase       = PREDEFINED_CHANNEL_INFO_FT3_EXT_BODIAN[i].enChannelPhase;
                pChannel->m_bMeasure            = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_FT3_EXT_BODIAN[i].enChannelValueType);
                pChannel->m_strDescription      = PREDEFINED_CHANNEL_INFO_FT3_EXT_BODIAN[i].strChannelDesc;
                pChannel->m_strVirtualChannel   = PREDEFINED_CHANNEL_INFO_FT3_EXT_BODIAN[i].strVirtualChannel;
            }
        }
        break;

    case SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_KAIMO:
        {
            // �������ͨ��
            ClearAllChannel();

            // ���Ĭ��ͨ��
            for (int i = 0; i < sizeof(PREDEFINED_CHANNEL_INFO_FT3_EXT_KAIMO)/ sizeof(PREDEFINED_CHANNEL_INFO_FT3_EXT_KAIMO[0]); i++)
            {
                CChannel* pChannel = AddChannel();
                if (NULL == pChannel)
                {
                    return false;
                }

                // ����ͨ��Ĭ��ֵ
                pChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_FT3_EXT_KAIMO[i].enChannelType);
                pChannel->m_eChannelPhase       = PREDEFINED_CHANNEL_INFO_FT3_EXT_KAIMO[i].enChannelPhase;
                pChannel->m_bMeasure            = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_FT3_EXT_KAIMO[i].enChannelValueType);
                pChannel->m_strDescription      = PREDEFINED_CHANNEL_INFO_FT3_EXT_KAIMO[i].strChannelDesc;
                pChannel->m_strVirtualChannel   = PREDEFINED_CHANNEL_INFO_FT3_EXT_KAIMO[i].strVirtualChannel;
            }
        }
        break;

    case SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BOOK:
        {
            // �������ͨ��
            ClearAllChannel();

            // ���Ĭ��ͨ��
            for (int i = 0; i < sizeof(PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG)/ sizeof(PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[0]); i++)
            {
                CChannel* pChannel = AddChannel();
                if (NULL == pChannel)
                {
                    return false;
                }

                // ����ͨ��Ĭ��ֵ
                pChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].enChannelType);
                pChannel->m_eChannelPhase       = PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].enChannelPhase;
                pChannel->m_bMeasure            = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].enChannelValueType);
                pChannel->m_strDescription      = PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].strChannelDesc;
                pChannel->m_strVirtualChannel   = PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].strVirtualChannel;
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
///    ��ȿ���һ��ɨ���б��е�FT3��չ���ƿ�
///
/// @param[in]
///     pFt3ExpandMsg    -   FT3��չ���ĵı�������
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CFT3ExpandCb::__Copy( GuoWangFT3Msg* pFt3ExpandMsg )
{
    GUOWANG_FT3_ASDU_PUBLIC    stFt3ExpandPublicInfo;


    // ����������
    if (NULL == pFt3ExpandMsg)
    {
        return false;
    }

    // ��ȡ����֡�е�GoosePdu�Ĺ�����Ϣ
    if (!pFt3ExpandMsg->GetASDUPublicInfo(stFt3ExpandPublicInfo))
    {
        return false;
    }

    // ����CGooseCb������
    m_iLNName                   = stFt3ExpandPublicInfo.lnName;               ///< LNName
    m_iAPPID                    = stFt3ExpandPublicInfo.ldName;               ///< LDName
    m_iDSName                   = stFt3ExpandPublicInfo.dataSetName;          ///< DSName

    m_dRatedPhaseCurrent        = stFt3ExpandPublicInfo.phsArtg;              ///< ��������A��
    m_dRatedNeutralCurrent      = stFt3ExpandPublicInfo.neutArtg;             ///< ����ߵ�����A��
    m_dRatedPhaseVoltage        = stFt3ExpandPublicInfo.phsVrtg/10;           ///< ����ѹ��kV��
    m_iRatedDelay               = stFt3ExpandPublicInfo.delayTime;            ///< ���ʱ��us��
    m_uiStatusWord1             = stFt3ExpandPublicInfo.statusWord1;          ///< ״̬��#1
    m_uiStatusWord2             = stFt3ExpandPublicInfo.statusWord2;          ///< ״̬��#2

    // �������������
    if (!CBaseCb::__Copy(pFt3ExpandMsg))
    {
        return false;
    }

    return true;
}
