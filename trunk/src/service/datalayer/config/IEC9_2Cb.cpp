/// @file
///
/// @brief
///     IEC61850 9-2 ���ƿ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.29
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "IEC9_2Cb.h"

///
/// @brief
///    ���캯��
///
CIEC9_2Cb::CIEC9_2Cb(void)
{
}

///
/// @brief
///     ���캯��
///
/// @param[in]  
///     pParamConfigBase    ָ��������ù�������ָ��
///
CIEC9_2Cb::CIEC9_2Cb(CParamConfigBase* pParamConfigBase)
: CNetCb(pParamConfigBase)
, m_strSvID(L"svID0")
, m_strDataSetRef(L"datSet0")
, m_bDataSetOpt(false)
, m_bRefrTmOpt(false)
, m_bSmpRateOpt(false)
, m_bSecurityOpt(false)
{
    m_eFrameType    = FRAME_TYPE_NET_9_2;
    m_ePortName     = NET_PORT_LIGHT_A;
}

///
/// @brief
///    ��������
///
CIEC9_2Cb::~CIEC9_2Cb(void)
{
}

///
/// @brief
///    ���SvID
///
/// @return
///     wstring    SvID
///
std::wstring CIEC9_2Cb::GetSvID()
{
    return m_strSvID;
}

///
/// @brief
///    ����SvID
///
/// @param[in]  strSvID  SvID
///
void CIEC9_2Cb::SetSvID(std::wstring strSvID)
{
    if (m_strSvID != strSvID)
    {
        m_strSvID = strSvID;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ������ݼ�����
///
/// @return
///     wstring    ���ݼ�����
///
std::wstring CIEC9_2Cb::GetDataSetRef()
{
    return m_strDataSetRef;
}

///
/// @brief
///    �������ݼ�����
///
/// @param[in]  strDataSetRef  ���ݼ�����
///
void CIEC9_2Cb::SetDataSetRef(std::wstring strDataSetRef)
{
    if (m_strDataSetRef != strDataSetRef)
    {
        m_strDataSetRef = strDataSetRef;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ��ȿ���һ��IEC61850-9-2���ƿ�
///
/// @param[in]
///     pIEC9_2Cb    -   ���ƿ�ָ��
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CIEC9_2Cb::__Copy(CIEC9_2Cb* pIEC9_2Cb)
{
    if (NULL == pIEC9_2Cb)
    {
        return false;
    }

    // �������������
    if (!CNetCb::__Copy(pIEC9_2Cb))
    {
        return false;
    }

    // ����CIEC9_2Cb������
    m_strSvID       = pIEC9_2Cb->m_strSvID;
    m_strDataSetRef = pIEC9_2Cb->m_strDataSetRef;
    m_bDataSetOpt   = pIEC9_2Cb->m_bDataSetOpt;
    m_bRefrTmOpt    = pIEC9_2Cb->m_bRefrTmOpt;
    m_bSmpRateOpt   = pIEC9_2Cb->m_bSmpRateOpt;
    m_bSecurityOpt  = pIEC9_2Cb->m_bSecurityOpt;

    return true;
}
///
/// @brief
///    ��ȿ���һ��ɨ���б��е�IEC61850-9-2���ƿ�
///
/// @param[in]
///     pSav92Msg    -   IEC61850-9-2���ĵı�������
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CIEC9_2Cb::__Copy(Sav92Msg* pSav92Msg)
{
    SAV92_ASDU_PUBLIC    stSav92PublicInfo;


    // ����������
    if (NULL == pSav92Msg)
    {
        return false;
    }

    // �������������
    if (!CNetCb::__Copy(pSav92Msg))
    {
        return false;
    }

    // ��ȡ����֡�еĹ�����Ϣ
    if (!pSav92Msg->GetASDUPublicInfo(0, stSav92PublicInfo))
    {
        return false;
    }

    // ��������������
    m_strSvID.assign(stSav92PublicInfo.svIDStr, stSav92PublicInfo.svIDStr + strlen(stSav92PublicInfo.svIDStr));
    m_strDataSetRef.assign(stSav92PublicInfo.datSetStr, stSav92PublicInfo.datSetStr + strlen(stSav92PublicInfo.datSetStr));
    m_iConfigRevision = stSav92PublicInfo.confRev;

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
bool CIEC9_2Cb::UseChannelTemplate( enumSmvRecvConfigTemplateType eType )
{
    // ����ָ������ʹ�ò�ͬ��ģ������
    switch(eType)
    {
    case SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BODIAN:
        {
            // �������ͨ��
            ClearAllChannel();

            // ���Ĭ��ͨ��
            for (int i = 0; i < sizeof(PREDEFINED_CHANNEL_INFO_9_2LE_BODIAN)/ sizeof(PREDEFINED_CHANNEL_INFO_9_2LE_BODIAN[0]); i++)
            {
                CChannel* pChannel = AddChannel();
                if (NULL == pChannel)
                {
                    return false;
                }

                // ����ͨ��Ĭ��ֵ
                pChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_9_2LE_BODIAN[i].enChannelType);
                pChannel->m_eChannelPhase       = PREDEFINED_CHANNEL_INFO_9_2LE_BODIAN[i].enChannelPhase;
                pChannel->m_bMeasure            = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_9_2LE_BODIAN[i].enChannelValueType);
                pChannel->m_strDescription      = PREDEFINED_CHANNEL_INFO_9_2LE_BODIAN[i].strChannelDesc;
                pChannel->m_strVirtualChannel   = PREDEFINED_CHANNEL_INFO_9_2LE_BODIAN[i].strVirtualChannel;
            }
        }
        break;

    case SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_KAIMO:
        {
            // �������ͨ��
            ClearAllChannel();

            // ���Ĭ��ͨ��
            for (int i = 0; i < sizeof(PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO)/ sizeof(PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[0]); i++)
            {
                CChannel* pChannel = AddChannel();
                if (NULL == pChannel)
                {
                    return false;
                }

                // ����ͨ��Ĭ��ֵ
                pChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[i].enChannelType);
                pChannel->m_eChannelPhase       = PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[i].enChannelPhase;
                pChannel->m_bMeasure            = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[i].enChannelValueType);
                pChannel->m_strDescription      = PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[i].strChannelDesc;
                pChannel->m_strVirtualChannel   = PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[i].strVirtualChannel;
            }
        }
        break;

    case SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BOOK:
        {
            // �������ͨ��
            ClearAllChannel();

            // ���Ĭ��ͨ��
            for (int i = 0; i < sizeof(PREDEFINED_CHANNEL_INFO_9_2LE_BOOK)/ sizeof(PREDEFINED_CHANNEL_INFO_9_2LE_BOOK[0]); i++)
            {
                CChannel* pChannel = AddChannel();
                if (NULL == pChannel)
                {
                    return false;
                }

                // ����ͨ��Ĭ��ֵ
                pChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_9_2LE_BOOK[i].enChannelType);
                pChannel->m_eChannelPhase       = PREDEFINED_CHANNEL_INFO_9_2LE_BOOK[i].enChannelPhase;
                pChannel->m_bMeasure            = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_9_2LE_BOOK[i].enChannelValueType);
                pChannel->m_strDescription      = PREDEFINED_CHANNEL_INFO_9_2LE_BOOK[i].strChannelDesc;
                pChannel->m_strVirtualChannel   = PREDEFINED_CHANNEL_INFO_9_2LE_BOOK[i].strVirtualChannel;
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
///    ������ݼ���ѡ��
///
/// @return
///     bool    �Ƿ������ݼ�
///
bool CIEC9_2Cb::GetDataSetOpt() const
{
    return m_bDataSetOpt;
}

///
/// @brief
///    �������ݼ���ѡ��
///
/// @param[in]  bNewValue    �Ƿ������ݼ�
///
void CIEC9_2Cb::SetDataSetOpt( bool bNewValue )
{
    if (m_bDataSetOpt != bNewValue)
    {
        m_bDataSetOpt = bNewValue;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���ˢ��ʱ���ѡ��
///
/// @return
///     bool    �Ƿ���ˢ��ʱ��
///
bool CIEC9_2Cb::GetRefrTmOpt() const
{
    return m_bRefrTmOpt;
}

///
/// @brief
///    ����ˢ��ʱ���ѡ��
///
/// @param[in]  bNewValue    �Ƿ���ˢ��ʱ��
///
void CIEC9_2Cb::SetRefrTmOpt( bool bNewValue )
{
    if (m_bRefrTmOpt != bNewValue)
    {
        m_bRefrTmOpt = bNewValue;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ��ò����ʿ�ѡ��
///
/// @return
///     bool    �Ƿ��Ͳ�����
///
bool CIEC9_2Cb::GetSmpRateOpt() const
{
    return m_bSmpRateOpt;
}

///
/// @brief
///    ���ò����ʿ�ѡ��
///
/// @param[in]  bNewValue    �Ƿ��Ͳ�����
///
void CIEC9_2Cb::SetSmpRateOpt( bool bNewValue )
{
    if (m_bSmpRateOpt != bNewValue)
    {
        m_bSmpRateOpt = bNewValue;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ��ñ��ļ��ܿ�ѡ��
///
/// @return
///     bool    �Ƿ�Ա��ļ���
///
bool CIEC9_2Cb::GetSecurityOpt() const
{
    return m_bSecurityOpt;
}

///
/// @brief
///    ���ñ��ļ��ܿ�ѡ��
///
/// @param[in]  bNewValue    �Ƿ�Ա��ļ���
///
void CIEC9_2Cb::SetSecurityOpt( bool bNewValue )
{
    if (m_bSecurityOpt != bNewValue)
    {
        m_bSecurityOpt = bNewValue;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ����IEC61850-9-2���ƿ�
///
/// @param[in]  pXml ָ��XML���������
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CIEC9_2Cb::__LoadConfig( CMarkup& xml )
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

    // <APPID>
    if (!xml.FindElem(NODE_SmvCb_APPID))
    {
        return false;
    }
    m_iAPPID = _wtoi(xml.GetData().c_str());

    // <SrcMAC>
    unsigned char cBuffer[ETHERNET_MAC_LEN] = {0};
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

#if !CHUN_HUA
	// <VLanTag>
	if (!xml.FindElem(NODE_Gocb_VLanTag))
	{
		return false;
	}
	m_bHasVlanFlag = _wtoi(xml.GetData().c_str());
#endif

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

    // <SvID>
    if (!xml.FindElem(NODE_SmvCb_SvID))
    {
        return false;
    }
    m_strSvID = xml.GetData();

    // <DataSet>
    if (!xml.FindElem(NODE_SmvCb_DataSetRef))
    {
        return false;
    }
    m_strDataSetRef = xml.GetData();

    // <DataSetOpt>
    if (!xml.FindElem(NODE_SmvCb_DataSetOpt))
    {
        return false;
    }
    m_bDataSetOpt = (_wtoi(xml.GetData().c_str())? true: false);

    // <RefrTmOpt>
    if (!xml.FindElem(NODE_SmvCb_RefrTmOpt))
    {
        return false;
    }
    m_bRefrTmOpt = (_wtoi(xml.GetData().c_str())? true: false);

    // <SmpRateOpt>
    if (!xml.FindElem(NODE_SmvCb_SmpRateOpt))
    {
        return false;
    }
    m_bSmpRateOpt = (_wtoi(xml.GetData().c_str())? true: false);

    // <SecurityOpt>
    if (!xml.FindElem(NODE_SmvCb_SecurityOpt))
    {
        return false;
    }
    m_bSecurityOpt = (_wtoi(xml.GetData().c_str())? true: false);

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

            // <Quality>
            if (!xml.FindElem(NODE_Channel_Quality))
            {
                return false;
            }
            pChannel->m_usQuality = (unsigned short)_wtoi(xml.GetData().c_str());

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
///    ����IEC61850-9-2���ƿ�
///
/// @param[in]  pXml ָ��XML���������
///
/// @return
///     bool    true��ʾ����ɹ�
///
bool CIEC9_2Cb::__SaveConfig( CMarkup& xml )
{
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

    // <APPID>
    if (!xml.AddElem(NODE_SmvCb_APPID) || !xml.SetData(GetAPPID()))
    {
        return false;
    }

    // <SrcMAC>
    unsigned char cBuffer[ETHERNET_MAC_LEN] = {'0'};
    GetSrcMAC(cBuffer, ETHERNET_MAC_LEN);
    if (!xml.AddElem(NODE_SmvCb_SRC_MAC) || !xml.SetData(ConvMacChar2WStr(cBuffer, ETHERNET_MAC_LEN)))
    {
        return false;
    }

    // <DesMAC>
    GetDesMAC(cBuffer, ETHERNET_MAC_LEN);
    if (!xml.AddElem(NODE_SmvCb_DES_MAC) || !xml.SetData(ConvMacChar2WStr(cBuffer, ETHERNET_MAC_LEN)))
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

    // <SvID>
    if (!xml.AddElem(NODE_SmvCb_SvID) || !xml.SetData(GetSvID()))
    {
        return false;
    }

    // <DataSet>
    if (!xml.AddElem(NODE_SmvCb_DataSetRef) || !xml.SetData(GetDataSetRef()))
    {
        return false;
    }

    // <DataSetOpt>
    if (!xml.AddElem(NODE_SmvCb_DataSetOpt) || !xml.SetData(GetDataSetOpt()? 1: 0))
    {
        return false;
    }

    // <RefrTmOpt>
    if (!xml.AddElem(NODE_SmvCb_RefrTmOpt) || !xml.SetData(GetRefrTmOpt()? 1: 0))
    {
        return false;
    }

    // <SmpRateOpt>
    if (!xml.AddElem(NODE_SmvCb_SmpRateOpt) || !xml.SetData(GetSmpRateOpt()? 1: 0))
    {
        return false;
    }

    // <SecurityOpt>
    if (!xml.AddElem(NODE_SmvCb_SecurityOpt) || !xml.SetData(GetSecurityOpt()? 1: 0))
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

            // <Quality>
            if (!xml.AddElem(NODE_Channel_Quality) || !xml.SetData(pChannel->GetQuality()))
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
///    ���ÿ��ƿ�����ͨ����Ʒ��
///
/// @param[in]  eType    �������ͣ��磺���㡢�ü��ޡ�������
///
void CIEC9_2Cb::SetAllChannelsQuality( enumQualityOperateType eType )
{
    CChannel*                       pChannel    = NULL;


    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        switch (eType)
        {
        case QUALITY_OPERATE_CLEAR:                                 ///< ����
            {
                pChannel->SetQuality(0x0000);
            }
            break;

        case QUALITY_OPERATE_TEST:                                  ///< �ü���
            {
                pChannel->SetQuality(0x0800 | pChannel->GetQuality());
            }
            break;

        case QUALITY_OPERATE_RUN:                                   ///< ������
            {
                pChannel->SetQuality(0xF7FF & pChannel->GetQuality());
            }
            break;
        }
    }
}
