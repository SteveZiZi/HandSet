/// @file
///
/// @brief
///     IEC61850 9-1 控制块
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.29
///
/// 修改历史：
///

#include "StdAfx.h"
#include "IEC9_1Cb.h"

///
/// @brief
///    构造函数
///
CIEC9_1Cb::CIEC9_1Cb(void)
{
}

///
/// @brief
///     构造函数
///
/// @param[in]  
///     pParamConfigBase    指向参数配置管理器的指针
///
CIEC9_1Cb::CIEC9_1Cb(CParamConfigBase* pParamConfigBase)
:CNetCb(pParamConfigBase)
,m_iRatedDelay(750)
,m_uiStatusWord1(0)
,m_uiStatusWord2(0)
{
    m_eFrameType    = FRAME_TYPE_NET_9_1;
    m_ePortName     = NET_PORT_LIGHT_A;

    // 初始化额定相电流、额定中线电流、额定线电压
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
///    析构函数
///
CIEC9_1Cb::~CIEC9_1Cb(void)
{
}

///
/// @brief
///    获得额定相电流（A）
///
/// @return
///     double    额定相电流（A）
///
double CIEC9_1Cb::GetRatedPhaseCurrent()
{
    return m_dRatedPhaseCurrent;
}

///
/// @brief
///    设置额定相电流（A）
///
/// @param[in]  dRatedPhaseCurrent 额定相电流（A）
///
void CIEC9_1Cb::SetRatedPhaseCurrent(double dRatedPhaseCurrent)
{
    if (fabs(m_dRatedPhaseCurrent - dRatedPhaseCurrent) >= FLOAT_PRECISION)
    {
        m_dRatedPhaseCurrent = dRatedPhaseCurrent;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得额定中线电流（A）
///
/// @return
///     double    额定中线电流（A）
///
double CIEC9_1Cb::GetRatedNeutralCurrent()
{
    return m_dRatedNeutralCurrent;
}

///
/// @brief
///    设置额定中线电流（A）
///
/// @param[in]  dRatedNeutralCurrent 额定中线电流（A）
///
void CIEC9_1Cb::SetRatedNeutralCurrent(double dRatedNeutralCurrent)
{
    if (fabs(m_dRatedNeutralCurrent - dRatedNeutralCurrent) >= FLOAT_PRECISION)
    {
        m_dRatedNeutralCurrent = dRatedNeutralCurrent;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得额定相电压（kV）
///
/// @return
///     double    额定相电压（kV）
///
double CIEC9_1Cb::GetRatedPhaseVoltage()
{
    return m_dRatedPhaseVoltage;
}

///
/// @brief
///    设置额定相电压（kV）
///
/// @param[in]  dRatedPhaseVoltage 额定相电压（kV）
///
void CIEC9_1Cb::SetRatedPhaseVoltage(double dRatedPhaseVoltage)
{
    if (fabs(m_dRatedPhaseVoltage - dRatedPhaseVoltage) >= FLOAT_PRECISION)
    {
        m_dRatedPhaseVoltage = dRatedPhaseVoltage;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得额定延时（us）
///
/// @return
///     int    额定延时（us）
///
int CIEC9_1Cb::GetRatedDelay()
{
    return m_iRatedDelay;
}

///
/// @brief
///    设置额定延时（us）
///
/// @param[in]  iRatedDelay 额定延时（us）
///
void CIEC9_1Cb::SetRatedDelay(int iRatedDelay)
{
    // 输入参数检查
    if (iRatedDelay < 0)
    {
        return ;
    }

    if (m_iRatedDelay != iRatedDelay)
    {
        m_iRatedDelay = iRatedDelay;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得状态字#1
///
/// @return
///     unsigned short    状态字#1
///
unsigned short CIEC9_1Cb::GetStatusWord1()
{
    return m_uiStatusWord1;
}

///
/// @brief
///    设置状态字#1
///
/// @param[in]  nStatusWord1 状态字#1
///
void CIEC9_1Cb::SetStatusWord1(unsigned short nStatusWord1)
{
    if (m_uiStatusWord1 != nStatusWord1)
    {
        m_uiStatusWord1 = nStatusWord1;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
            m_pParamConfigBase->SetStatusWordChanged(true);
        }
    }
}

///
/// @brief
///    获得状态字#2
///
/// @return
///     unsigned short    状态字#2
///
unsigned short CIEC9_1Cb::GetStatusWord2()
{
    return m_uiStatusWord2;
}

///
/// @brief
///    设置状态字#2
///
/// @param[in]  nStatusWord2 状态字#2
///
void CIEC9_1Cb::SetStatusWord2(unsigned short nStatusWord2)
{
    if (m_uiStatusWord2 != nStatusWord2)
    {
        m_uiStatusWord2 = nStatusWord2;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
            m_pParamConfigBase->SetStatusWordChanged(true);
        }
    }
}

///
/// @brief
///    深度拷贝一个扫描列表中的IEC61850-9-1控制块
///
/// @param[in]
///     Sav91Msg    -   IEC61850-9-1报文的编解码对象
///
/// @return
///     bool    拷贝成功返回TRUE
///
bool CIEC9_1Cb::__Copy(Sav91Msg* pSav91Msg)
{
    SAV91_ASDU_PUBLIC    stSav91PublicInfo;


    // 输入参数检查
    if (NULL == pSav91Msg)
    {
        return false;
    }

    // 拷贝基类的属性
    if (!CNetCb::__Copy(pSav91Msg))
    {
        return false;
    }

    // 获取报文帧中的GoosePdu的公共信息
    if (!pSav91Msg->GetASDUPublicInfo(0, stSav91PublicInfo))
    {
        return false;
    }

    // 拷贝CGooseCb的属性
    m_dRatedPhaseCurrent        = stSav91PublicInfo.phsArtg;            ///< 额定相电流（A）
    m_dRatedNeutralCurrent      = stSav91PublicInfo.neutArtg;           ///< 额定中线电流（A）
    m_dRatedPhaseVoltage        = stSav91PublicInfo.phsVrtg/10;         ///< 额定相电压（kV）
    m_iRatedDelay               = stSav91PublicInfo.delayTime;          ///< 额定延时（us）
    m_uiStatusWord1             = stSav91PublicInfo.statusWord1;        ///< 状态字#1
    m_uiStatusWord2             = stSav91PublicInfo.statusWord2;        ///< 状态字#2
    m_iConfigRevision           = stSav91PublicInfo.confRev;            ///< 配置版本号

    return true;
}

///
/// @brief
///    深度拷贝一个IEC61850-9-1控制块
///
/// @param[in]
///     pIEC9_1Cb    -   IEC61850-9-1控制块
///
/// @return
///     bool    拷贝成功返回TRUE
///
bool CIEC9_1Cb::__Copy( CIEC9_1Cb* pIEC9_1Cb )
{
    // 检查输入参数
    if (NULL == pIEC9_1Cb)
    {
        return false;
    }

    // 拷贝基类的属性
    if (!CNetCb::__Copy(pIEC9_1Cb))
    {
        return false;
    }

    m_dRatedPhaseCurrent        = pIEC9_1Cb->m_dRatedPhaseCurrent;      ///< 额定相电流（A）
    m_dRatedNeutralCurrent      = pIEC9_1Cb->m_dRatedNeutralCurrent;    ///< 额定中线电流（A）
    m_dRatedPhaseVoltage        = pIEC9_1Cb->m_dRatedPhaseVoltage;      ///< 额定相电压（kV）
    m_iRatedDelay               = pIEC9_1Cb->m_iRatedDelay;             ///< 额定延时（us）
    m_uiStatusWord1             = pIEC9_1Cb->m_uiStatusWord1;           ///< 状态字#1
    m_uiStatusWord2             = pIEC9_1Cb->m_uiStatusWord2;           ///< 状态字#2

    return true;
}

///
/// @brief
///    使用指定的通道模板来填充控制块
///
/// @param[in]  eType    模板类型
///
/// @return
///     bool    成功返回true
///
bool CIEC9_1Cb::UseChannelTemplate( enumSmvRecvConfigTemplateType eType )
{
    // 根据指定参数使用不同的模板类型
    switch(eType)
    {
    case SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_BODIAN:
        {
            // 清空所有通道
            ClearAllChannel();

            // 添加默认通道
            for (int i = 0; i < sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_BODIAN)/ sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_BODIAN[0]); i++)
            {
                CChannel* pChannel = AddChannel();
                if (NULL == pChannel)
                {
                    return false;
                }

                // 设置通道默认值
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
            // 清空所有通道
            ClearAllChannel();

            // 添加默认通道
            for (int i = 0; i < sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_KAIMO)/ sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_KAIMO[0]); i++)
            {
                CChannel* pChannel = AddChannel();
                if (NULL == pChannel)
                {
                    return false;
                }

                // 设置通道默认值
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
            // 清空所有通道
            ClearAllChannel();

            // 添加默认通道
            for (int i = 0; i < sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8)/ sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[0]); i++)
            {
                CChannel* pChannel = AddChannel();
                if (NULL == pChannel)
                {
                    return false;
                }

                // 设置通道默认值
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
///    更新控制块中所有电压通道的一次额定值
///
/// @param[in]  dNewValue    新的电压一次额定值（V）
///
void CIEC9_1Cb::UpdateRatedPrimaryVoltage( double dNewValue )
{
    // 更新基类的电压额定值
    CBaseCb::UpdateRatedPrimaryVoltage(dNewValue);

    // 更新额定线电压
    SetRatedPhaseVoltage(dNewValue / 1000.0f);
}

///
/// @brief
///    更新控制块中所有电流通道的一次额定值
///
/// @param[in]  dNewValue    新的电流一次额定值（A）
///
void CIEC9_1Cb::UpdateRatedPrimaryCurrent( double dNewValue )
{
    // 更新基类的电流额定值
    CBaseCb::UpdateRatedPrimaryCurrent(dNewValue);

    // 更新额定相电流、额定中线电流
    SetRatedPhaseCurrent(dNewValue);
    SetRatedNeutralCurrent(dNewValue);
}

///
/// @brief
///    加载IEC61850-9-1控制块
///
/// @param[in]  pXml 指向XML对象的引用
///
/// @return
///     bool    true表示加载成功
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

            // 加载通道
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

            // 添加GOOSE控制块对象指针到容器
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
///    保存IEC61850-9-1控制块
///
/// @param[in]  pXml 指向XML对象的引用
///
/// @return
///     bool    true表示保存成功
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

    // 通道
    if (GetChannelCount())
    {
        // <ChannelGroup>
        if (!xml.AddElem(NODE_SmvCb_ChannelGroup) || !xml.IntoElem())
        {
            return false;
        }

        // 单个通道
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

            // 保存Channel
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
