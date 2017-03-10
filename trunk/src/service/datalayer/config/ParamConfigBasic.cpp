/// @file
///
/// @brief
///     ϵͳ���������������ã���Ҫ����ϵͳ����������ص�һ��/����ֵ��Ƶ�ʵ���Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.21
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "ParamConfigBasic.h"
#include "src\service\datalayer\utility\data_utility.h"
#include "src\service\datalayer\DsmDataLayer.h"

static std::wstring ELEM_NODE_ATTR_NAME       = L"value";                ///< ����ֵ��

CParamConfigBasic::CParamConfigBasic(void) : m_bModified(false)
                                           , m_strConfigFile(L"")
                                           , m_enTimeSyncMode(FPGA_TIME_SYNC_NONE)
                                           , m_enValueType(SMV_VALUE_TYPE_PRIMARY)
                                           , m_strPriVol(L"110")
                                           , m_strPriAmp(L"3000")
                                           , m_strSecVol(L"100")
                                           , m_strSecAmp(L"5")
                                           , m_strRatedFreq(L"50")
                                           , m_strDelay(L"750")
                                           , m_strSclFile(L"")
                                           , m_strSelfIeee1588ClockId(L"000000FFFE000001")
                                           , m_strSelfIeee1588PortId(L"0001")
                                           , m_strSelfIeee1588Mac(L"00005e00FACE")
                                           , m_strMainIeee1588ClockId(L"001E4CFFFE00447C")
                                           , m_strMainIeee1588PortId(L"0001")
                                           , m_enIeee1588SyncPortName(NET_PORT_LIGHT_A)
#if !CHUN_HUA
										   , m_bHasVlanTag(true)
										   , m_strSclPath(L"")
#endif
{
}

CParamConfigBasic::~CParamConfigBasic(void)
{
}

///
/// @brief
///    ����ϵͳʹ�õ�SCL�ļ�
///
void CParamConfigBasic::SetSclFile( std::wstring strSclFile )
{
    if (strSclFile.compare(m_strSclFile))
    {
        m_strSclFile = strSclFile;
        m_bModified = true;
    }
}


///
/// @brief
///    ��ȡϵͳʹ�õ�SCL�ļ�
///
std::wstring CParamConfigBasic::GetSclFile()
{
    return m_strSclFile;
}

///
/// @brief
///    ���öƵ��
///
bool CParamConfigBasic::SetRatedFreq( std::wstring strFreq )
{
    std::wstring strResult;
    if (StrUint32Convert(strFreq, &strResult))
    {
        if (strResult.compare(m_strRatedFreq))
        {
            m_strRatedFreq = strResult;
            m_bModified = true;
        }
        return true;
    }
    return false;
}

///
/// @brief
///    ��ȡ�Ƶ��
///
std::wstring CParamConfigBasic::GetRatedFreq()
{
    return m_strRatedFreq;
}

///
/// @brief
///    ���ñ�������������һ��/����ֵ
///
bool CParamConfigBasic::SetFrameValueType( enumSmvValueType enValueType )
{
    if(m_enValueType != enValueType)
    {
        m_enValueType = enValueType;
        m_bModified = true;
    }
    return true;
}

///
/// @brief
///    ��ȡϵͳ���õı�����������
///
enumSmvValueType CParamConfigBasic::GetFrameValueType()
{
    return m_enValueType;
}

///
/// @brief
///    �趨�һ�ε�ѹ
///
bool CParamConfigBasic::SetRatedPrimaryVoltage( float fVoltage )
{
    std::wstring strResult;
    if (FloatStrConvert(fVoltage, strResult))
    {
        if (strResult.compare(m_strPriVol))
        {
            m_strPriVol = strResult;
            m_bModified = true;

            // ����SMV�������������п��ƿ��е�ѹͨ����һ�ζֵ
            CParamConfigSMVSend::getInstance()->UpdateRatedPrimaryVoltage(fVoltage * 1000);

            // ����SMV���ջ���Ŀ��ƿ�ͨ��������Ϣ
            CDsmDataLayer::getInstance()->UpdateCachedRatedValues(CHANNEL_TYPE_SMV_VOLTAGE, 1, fVoltage * 1000);
        }
        return true;
     }
    return false;
}

///
/// @brief
///    ��ȡ�һ�ε�ѹ
///
float CParamConfigBasic::GetRatedPrimaryVoltage()
{
    double fResult = 0;
    if(StrFloatConvert(m_strPriVol, NULL, &fResult))
    {
        return static_cast<float>(fResult);
    }
    return 0.0f;
}

///
/// @brief
///    ���ö���ε�ѹ
///
bool CParamConfigBasic::SetRatedSecondaryVoltage( float fVoltage )
{
    std::wstring strResult;
    if (FloatStrConvert(fVoltage, strResult))
    {
        if (strResult.compare(m_strSecVol))
        {
            m_strSecVol = strResult;
            m_bModified = true;

            // ����SMV�������������п��ƿ��е�ѹͨ���Ķ��ζֵ
            CParamConfigSMVSend::getInstance()->UpdateRatedSecondaryVoltage(fVoltage);

            // ����SMV���ջ���Ŀ��ƿ�ͨ��������Ϣ
            CDsmDataLayer::getInstance()->UpdateCachedRatedValues(CHANNEL_TYPE_SMV_VOLTAGE, 2, fVoltage);
        }
        return true;
    }
    return false;
}

///
/// @brief
///    ��ȡ����ε�ѹ
///
float CParamConfigBasic::GetRatedSecondaryVoltage()
{
    double fResult = 0;
    if(StrFloatConvert(m_strSecVol, NULL, &fResult))
    {
        return static_cast<float>(fResult);
    }
    return 0.0f;
}

///
/// @brief
///    �趨�һ�ε���
///
bool CParamConfigBasic::SetRatedPrimaryCurrent( float fCurrent )
{
    std::wstring strResult;
    if (FloatStrConvert(fCurrent, strResult))
    {
        if (strResult.compare(m_strPriAmp))
        {
            m_strPriAmp = strResult;
            m_bModified = true;

            // ����SMV�������������п��ƿ��е���ͨ����һ�ζֵ
            CParamConfigSMVSend::getInstance()->UpdateRatedPrimaryCurrent(fCurrent);

            // ����SMV���ջ���Ŀ��ƿ�ͨ��������Ϣ
            CDsmDataLayer::getInstance()->UpdateCachedRatedValues(CHANNEL_TYPE_SMV_CURRENT, 1, fCurrent);
        }
        return true;
    }
    return false;
}

///
/// @brief
///    ��ȡ�һ�ε���
///
float CParamConfigBasic::GetRatedPrimaryCurrent()
{
    double fResult = 0;
    if(StrFloatConvert(m_strPriAmp, NULL, &fResult))
    {
        return static_cast<float>(fResult);
    }
    return 0.0f;
}

///
/// @brief
///    ���ö���ε���
///
bool CParamConfigBasic::SetRatedSecondaryCurrent( float fCurrent )
{
    std::wstring strResult;
    if (FloatStrConvert(fCurrent, strResult))
    {
        if (strResult.compare(m_strSecAmp))
        {
            m_strSecAmp = strResult;
            m_bModified = true;

            // ����SMV�������������п��ƿ��е���ͨ���Ķ��ζֵ
            CParamConfigSMVSend::getInstance()->UpdateRatedSecondaryCurrent(fCurrent);

            // ����SMV���ջ���Ŀ��ƿ�ͨ��������Ϣ
            CDsmDataLayer::getInstance()->UpdateCachedRatedValues(CHANNEL_TYPE_SMV_CURRENT, 2, fCurrent);
        }
        return true;
    }
    return false;
}

///
/// @brief
///    ��ȡ����ε���
///
float CParamConfigBasic::GetRatedSecondaryCurrent()
{
    double fResult = 0;
    if(StrFloatConvert(m_strSecAmp, NULL, &fResult))
    {
        return static_cast<float>(fResult);
    }
    return 0.0f;
}

///
/// @brief
///    ����MU���ʱ(us)
///
bool CParamConfigBasic::SetMuRatedDelay( std::wstring strnuSeconds )
{
    std::wstring strResult;
    uint32_t nVal = 0;
    if (StrUint32Convert(strnuSeconds, &strResult, &nVal))
    {
        if (strResult.compare(m_strDelay))
        {
            m_strDelay = strResult;
            m_bModified = true;

            // ����SMV�������������п��ƿ���ʱ��ͨ���Ķֵ
            CParamConfigSMVSend::getInstance()->UpdateMuRatedDelay(nVal);
        }
        return true;
    }
    return false;
}

///
/// @brief
///    ��ȡMU���ʱ
///
std::wstring CParamConfigBasic::GetMuRatedDelay()
{
    return m_strDelay;
}

///
/// @brief
///    ���ö�ʱģʽ
///
void CParamConfigBasic::SetTimeSyncMode( FpgaTimeSyncMode enTimeSyncMode )
{
    if (enTimeSyncMode >= FPGA_TIME_SYNC_IEEE1588_A && enTimeSyncMode <= FPGA_TIME_SYNC_IEEE1588_C)
    {
        enTimeSyncMode = FPGA_TIME_SYNC_IRIGB_POS;
    }
    if (enTimeSyncMode != m_enTimeSyncMode)
    {
        m_enTimeSyncMode = enTimeSyncMode;
        m_bModified = true;
        CDsmDataLayer::getInstance()->SetFpgaTimeSyncMode(enTimeSyncMode);
    }
}

///
/// @brief
///    ��ȡ���õĶ�ʱģʽ
///
FpgaTimeSyncMode CParamConfigBasic::GetTimeSyncMode()
{
    if (m_enTimeSyncMode >= FPGA_TIME_SYNC_IEEE1588_A && m_enTimeSyncMode <= FPGA_TIME_SYNC_IEEE1588_C)
    {
        return FPGA_TIME_SYNC_IRIGB_POS;
    }
    return m_enTimeSyncMode;
}

///
/// @brief
///    ���������ļ�
///
bool CParamConfigBasic::LoadConfig( std::wstring strConfigFile )
{
    if (strConfigFile.empty())
    {
        return false;
    }
    else
    {
        m_strConfigFile = strConfigFile;
    }

    std::wstring strCfg = GetAppRunTimeCfgDir() + m_strConfigFile;
    if (!m_markUpXml.Load(strCfg))
    {
        return Reset();
    }

    FpgaTimeSyncMode enTimeSyncMode = static_cast<FpgaTimeSyncMode>(_wtoi(GetElemAttrVal(ATTR_RATED_TIME_SYNC_MODE).c_str()));
    SetTimeSyncMode(enTimeSyncMode);
    m_enValueType = static_cast<enumSmvValueType>(_wtoi(GetElemAttrVal(ATTR_RATED_FRAME_VALUE_TYPE).c_str()));
    m_strPriVol = GetElemAttrVal(ATTR_RATED_VOL_PRIMARY);
    m_strPriAmp = GetElemAttrVal(ATTR_RATED_CUR_PRIMARY);
    m_strSecVol = GetElemAttrVal(ATTR_RATED_VOL_SECONDARY);
    m_strSecAmp = GetElemAttrVal(ATTR_RATED_CUR_SECONDARY);
    m_strRatedFreq = GetElemAttrVal(ATTR_RATED_FREQ);
    m_strDelay = GetElemAttrVal(ATTR_RATED_MU_DELAY);
    m_strSclFile = GetElemAttrVal(ATTR_SCLFILE);

#if !CHUN_HUA
	m_strSclPath = GetElemAttrVal(ATTR_SCLPATH);
#endif

    m_strSelfIeee1588ClockId = GetElemAttrVal(ATTR_IEEE_1588_SELF_CLOCK_ID);
    m_strSelfIeee1588PortId  = GetElemAttrVal(ATTR_IEEE_1588_SELF_CLOCK_PORT_ID);
    m_strSelfIeee1588Mac     = GetElemAttrVal(ATTR_IEEE_1588_SELF_MAC);
    m_strMainIeee1588ClockId = GetElemAttrVal(ATTR_IEEE_1588_MAIN_CLOCK_ID);
    m_strMainIeee1588PortId  = GetElemAttrVal(ATTR_IEEE_1588_MAIN_CLOCK_PORT_ID);
    m_enIeee1588SyncPortName = static_cast<DsmPortName>(_wtoi(GetElemAttrVal(ATTR_IEEE_1588_CLOCK_SYNC_PORT).c_str()));

    return true;
}

///
/// @brief
///    ��ȡָ���ڵ����������
///
std::wstring CParamConfigBasic::GetElemAttrVal( std::wstring strElement )
{
    m_markUpXml.ResetPos();

    if(m_markUpXml.FindChildElem(strElement) && m_markUpXml.IntoElem())
    {
        return m_markUpXml.GetAttrib(ELEM_NODE_ATTR_NAME);
    }
    return _T("");
}

///
/// @brief
///     ����ָ���ڵ����������
///
/// @param[in]  strElement Ҫ�������ݵ�Ԫ������
/// @param[in]  strAttrValue Ҫ���õ�����ֵ
///
/// @return
///     bool    true��ʾ���óɹ�
///
bool CParamConfigBasic::SetElemAttrVal( std::wstring strElement, std::wstring& strAttrValue )
{
    m_markUpXml.ResetPos();
    if(m_markUpXml.FindChildElem(strElement) && m_markUpXml.IntoElem())
    {
        return m_markUpXml.SetAttrib(ELEM_NODE_ATTR_NAME, strAttrValue);
    }
    else
    {
        if (m_markUpXml.AddChildElem(strElement))
        {
            m_markUpXml.IntoElem();
            return m_markUpXml.SetAttrib(ELEM_NODE_ATTR_NAME, strAttrValue);
        }
    }
    return false;
}

///
/// @brief
///     ����ָ���ڵ����������
///
/// @param[in]  strElement Ҫ�������ݵ�Ԫ������
/// @param[in]  nAttrValue Ҫ���õ�����ֵ
///
/// @return
///     bool    true��ʾ���óɹ�
///
bool CParamConfigBasic::SetElemAttrVal( std::wstring strElement, int nAttrValue )
{
    m_markUpXml.ResetPos();
    if(m_markUpXml.FindChildElem(strElement) && m_markUpXml.IntoElem())
    {
        return m_markUpXml.SetAttrib(ELEM_NODE_ATTR_NAME, nAttrValue);
    }
    else
    {
        if (m_markUpXml.AddChildElem(strElement))
        {
            m_markUpXml.IntoElem();
            return m_markUpXml.SetAttrib(ELEM_NODE_ATTR_NAME, nAttrValue);
        }
    }
    return false;
}

///
/// @brief
///    ���������ļ�
///
bool CParamConfigBasic::SaveConfig()
{
    if (!m_bModified)
    {
        return true;
    }
#if 0
	SetElemAttrVal(ATTR_SCLPATH,m_strSclPath);
#endif
    SetElemAttrVal(ATTR_SCLFILE, m_strSclFile);
    SetElemAttrVal(ATTR_RATED_VOL_PRIMARY, m_strPriVol);
    SetElemAttrVal(ATTR_RATED_VOL_SECONDARY, m_strSecVol);
    SetElemAttrVal(ATTR_RATED_CUR_PRIMARY, m_strPriAmp);
    SetElemAttrVal(ATTR_RATED_CUR_SECONDARY, m_strSecAmp);
    SetElemAttrVal(ATTR_RATED_MU_DELAY, m_strDelay);
    SetElemAttrVal(ATTR_RATED_FREQ, m_strRatedFreq);
    SetElemAttrVal(ATTR_RATED_FRAME_VALUE_TYPE, static_cast<int>(m_enValueType));
    SetElemAttrVal(ATTR_RATED_TIME_SYNC_MODE, static_cast<int>(m_enTimeSyncMode));

    SetElemAttrVal(ATTR_IEEE_1588_SELF_CLOCK_ID, m_strSelfIeee1588ClockId);
    SetElemAttrVal(ATTR_IEEE_1588_SELF_CLOCK_PORT_ID, m_strSelfIeee1588PortId);
    SetElemAttrVal(ATTR_IEEE_1588_SELF_MAC, m_strSelfIeee1588Mac);
    SetElemAttrVal(ATTR_IEEE_1588_MAIN_CLOCK_ID, m_strMainIeee1588ClockId);
    SetElemAttrVal(ATTR_IEEE_1588_MAIN_CLOCK_PORT_ID, m_strMainIeee1588PortId);
    SetElemAttrVal(ATTR_IEEE_1588_CLOCK_SYNC_PORT, static_cast<int>(m_enIeee1588SyncPortName));

    std::wstring strCfg = GetAppRunTimeCfgDir() + m_strConfigFile;
    if (!m_markUpXml.Save(strCfg))
    {
        return false;
    }

    m_bModified = false;

    return true;
}

///
/// @brief
///     ��ȡ����ʵ���ӿ�
///
CParamConfigBasic* CParamConfigBasic::getInstance()
{
    static CParamConfigBasic s_paramConfigBasic;
    return &s_paramConfigBasic;
}

///
/// @brief
///    �ָ�Ĭ������
///
bool CParamConfigBasic::Reset()
{
    // ��ȡ���������ļ�·��
    std::wstring strBackup, strRun;
    strBackup = GetAppDir() + DEFAULT_CFG_BACKUP + m_strConfigFile;
    strRun    = GetAppRunTimeCfgDir() + m_strConfigFile;
    // �������ݵ������ļ�
    if(!CopyFile(strBackup.c_str(),strRun.c_str(), FALSE))
    {
        return false;
    }

    if (!m_markUpXml.Load(strRun))
    {
        return false;
    }

    //���ؽ��ڴ��Ӧ�ı���������Ӧ�ÿ�������/��ȡ����
    m_enTimeSyncMode = static_cast<FpgaTimeSyncMode>(_wtoi(GetElemAttrVal(ATTR_RATED_TIME_SYNC_MODE).c_str()));
    m_enValueType = static_cast<enumSmvValueType>(_wtoi(GetElemAttrVal(ATTR_RATED_FRAME_VALUE_TYPE).c_str()));
    m_strPriVol = GetElemAttrVal(ATTR_RATED_VOL_PRIMARY);
    m_strPriAmp = GetElemAttrVal(ATTR_RATED_CUR_PRIMARY);
    m_strSecVol = GetElemAttrVal(ATTR_RATED_VOL_SECONDARY);
    m_strSecAmp = GetElemAttrVal(ATTR_RATED_CUR_SECONDARY);
    m_strRatedFreq = GetElemAttrVal(ATTR_RATED_FREQ);
    m_strDelay = GetElemAttrVal(ATTR_RATED_MU_DELAY);
    m_strSclFile = GetElemAttrVal(ATTR_SCLFILE);

#if !CHUN_HUA
	m_strSclPath = GetElemAttrVal(ATTR_SCLPATH);
#endif

    m_strSelfIeee1588ClockId = GetElemAttrVal(ATTR_IEEE_1588_SELF_CLOCK_ID);
    m_strSelfIeee1588PortId  = GetElemAttrVal(ATTR_IEEE_1588_SELF_CLOCK_PORT_ID);
    m_strSelfIeee1588Mac     = GetElemAttrVal(ATTR_IEEE_1588_SELF_MAC);
    m_strMainIeee1588ClockId = GetElemAttrVal(ATTR_IEEE_1588_MAIN_CLOCK_ID);
    m_strMainIeee1588PortId  = GetElemAttrVal(ATTR_IEEE_1588_MAIN_CLOCK_PORT_ID);
    m_enIeee1588SyncPortName = static_cast<DsmPortName>(_wtoi(GetElemAttrVal(ATTR_IEEE_1588_CLOCK_SYNC_PORT).c_str()));


    return true;
}

///
/// @brief
///    ���ñ���1588ʱ��ID
///
/// @param[in] strClockId �趨����1588��ʱ�ӵ�ʱ��ID
///
/// @return
///     bool ���óɹ�����true
///
bool CParamConfigBasic::SetIeee1588SelfClockId( const std::wstring& strClockId )
{
    if(m_strSelfIeee1588ClockId.compare(strClockId))
    {
        m_strSelfIeee1588ClockId = strClockId;
        m_bModified = true;
    }
    return true;
}

///
/// @brief
///    ���ñ���1588ʱ�Ӷ˿�ID
///
/// @param[in] strClockPortId �趨����1588��ʱ�ӵĶ˿�ID
///
/// @return
///     bool ���óɹ�����true
///
bool CParamConfigBasic::SetIeee1588SelfClockPortId( const std::wstring& strClockPortId )
{
    if(m_strSelfIeee1588PortId.compare(strClockPortId))
    {
        m_strSelfIeee1588PortId = strClockPortId;
        m_bModified = true;
    }
    return true;
}

///
/// @brief
///    ���ñ���1588ʱ��MAC��ַ
///
/// @param[in] strMac MAC��ַ�ַ�����ʹ�ó���Ϊ5���ֽڵ�ʮ�������ַ�����ʶ
///
/// @return
///     bool ���óɹ�����true
///
bool CParamConfigBasic::SetIeee1588SelfClockMAC( const std::wstring& strMac )
{
    if(m_strSelfIeee1588Mac.compare(strMac))
    {
        m_strSelfIeee1588Mac = strMac;
        m_bModified = true;
    }
    return true;
}

///
/// @brief
///    ����1588��ʱ��ID
///
/// @param[in] strClockId �趨����1588��ʱ�ӵ�ʱ��ID
///
/// @return
///     bool ���óɹ�����true
///
bool CParamConfigBasic::SetIeee1588MainClockId( const std::wstring& strClockId )
{
    if (m_strMainIeee1588ClockId.compare(strClockId))
    {
        m_strMainIeee1588ClockId = strClockId;
        m_bModified = true;
    }
    return true;
}

///
/// @brief
///    ����1588��ʱ�Ӷ˿�ID
///
/// @param[in] strClockPortId �趨1588��ʱ�ӵĶ˿�ID
///
/// @return
///     bool ���óɹ�����true
///
bool CParamConfigBasic::SetIeee1588MainClockPortId( const std::wstring& strClockPortId )
{
    if (m_strMainIeee1588PortId.compare(strClockPortId))
    {
        m_strMainIeee1588PortId = strClockPortId;
        m_bModified = true;
    }
    return true;
}

///
/// @brief
///    ����1588ͬ��ʱʹ�õ�����˿�
///
/// @param[in]
///
/// @return
///     ���óɹ�����true
///
bool CParamConfigBasic::SetIeee1588ClockSyncPort( DsmPortName enPortName )
{
    if (m_enIeee1588SyncPortName != enPortName)
    {
        m_enIeee1588SyncPortName = enPortName;
        m_bModified = true;
    }
    return true;
}

///
/// @brief
///    1588ʱ�����������Ҫ���ô˺�����ɵײ����ݵ�ͬ��
///
/// @return
///     bool �ײ��������óɹ�����true
///
bool CParamConfigBasic::Ieee1588ConfigFinished()
{
    IEEE1588_CLOCK_SYNC_INFO stIeee1588ClockSyncInfo;
    _HexString2ByteArray(m_strSelfIeee1588ClockId, (uint8_t*)stIeee1588ClockSyncInfo.nSClockId, sizeof(stIeee1588ClockSyncInfo.nSClockId));
    _HexString2ByteArray(m_strSelfIeee1588PortId, (uint8_t*)&stIeee1588ClockSyncInfo.nSPortId, sizeof(stIeee1588ClockSyncInfo.nSPortId));
    _HexString2ByteArray(m_strSelfIeee1588Mac, (uint8_t*)stIeee1588ClockSyncInfo.nSourceMac, sizeof(stIeee1588ClockSyncInfo.nSourceMac), false);
    _HexString2ByteArray(m_strMainIeee1588ClockId, (uint8_t*)stIeee1588ClockSyncInfo.nMClockId, sizeof(stIeee1588ClockSyncInfo.nMClockId));
    _HexString2ByteArray(m_strMainIeee1588PortId, (uint8_t*)&stIeee1588ClockSyncInfo.nMPortId, sizeof(stIeee1588ClockSyncInfo.nMPortId));

    return CDsmDataLayer::getInstance()->SetIeee1588MainClock(m_enIeee1588SyncPortName, stIeee1588ClockSyncInfo);
}

///
/// @brief
///    ת��16�����ַ�������Ӧ���ֽ�����
///    "000102" -- >[0x00,0x01,0x02] (���)
///    "000102" -- >[0x02,0x01,0x00] (С��)
///
/// @param[in]  strSrcStr ��ת�����ַ���
/// @param[out] pArray    ����ת������Ļ�������ַ
/// @param[in]  nArraySize ���������������С
/// @param[in]  bLE        ��С��ģʽ��Ĭ��С��
///
/// @return
///     bool ת���ɹ�����true
///
bool CParamConfigBasic::_HexString2ByteArray( const std::wstring& strSrcStr, uint8_t* pArray, uint32_t nArraySize , bool bLE/* = true*/)
{
    if (pArray == NULL || nArraySize == 0)
    {
        return false;
    }
    uint32_t nStrLen = strSrcStr.length();
    std::wstring strSrcTmp = strSrcStr;
    if (nStrLen % 2 != 0)
    {
        strSrcTmp = L"0" + strSrcTmp;
        nStrLen = strSrcTmp.length();
    }
    std::wstring strTmp = L"";
    uint8_t nTmp = 0;
    uint32_t nPos = 0;
    if (bLE)
    {
        for (int32_t i = nStrLen - 2; i >=0; i = i -2)
        {
            if (nPos >= nArraySize)
            {
                break;
            }
            strTmp = strSrcTmp.substr(i, 2);
            swscanf_s(strTmp.c_str(), L"%x", &nTmp);
            pArray[nPos] = nTmp;
            nPos++;
        }
    }
    else
    {
        for (uint32_t i = 0; i<nArraySize; ++i)
        {
            nPos = i * 2;
            if (nPos >= nStrLen)
            {
                break;
            }
            strTmp = strSrcTmp.substr(nPos, 2);
            swscanf_s(strTmp.c_str(), L"%x", &nTmp);
            pArray[i] = nTmp;
        }
    }

    return true;
}

///
/// @brief
///    ʹ��̽�⵽��1588���ƿ�������ʱ�ӺͶ˿���Ϣ
///
/// @param[in] pItem ָ��̽�⵽��1588������Ϣ
///
/// @return
///     bool ���óɹ�����true
///
bool CParamConfigBasic::SetIeee1588MainClockByDetectItem( CNet1588DetectItem* pItem )
{
    if (pItem)
    {
        bool bRet1,bRet2,bRet3;
        bRet1 = SetIeee1588MainClockId(pItem->GetClockIdStr());
        bRet2 = SetIeee1588MainClockPortId(pItem->GetClockPortIdStr());
        bRet3 = SetIeee1588ClockSyncPort(pItem->enPortName);

        if (bRet1 && bRet2 && bRet3)
        {
            return true;
        }
    }
    return false;
}

#if !CHUN_HUA
bool CParamConfigBasic::SetHasVlan( bool hasVlanTag )
{
	m_bHasVlanTag = hasVlanTag;
	return true;
}

bool CParamConfigBasic::GetHasVlan()
{
	return m_bHasVlanTag;
}


///
/// @brief
///    ����ϵͳʹ�õ�SCL�ļ�·��
///
void CParamConfigBasic::SetSclPath( std::wstring strSclPath )
{
	if (strSclPath.compare(m_strSclPath))
	{
		m_strSclPath = strSclPath;
		m_bModified = true;
	}
}
///
/// @brief
///    ��ȡϵͳʹ�õ�SCL�ļ�·��
///
std::wstring CParamConfigBasic::GetSclPath()
{
	return m_strSclPath;
}
#endif
