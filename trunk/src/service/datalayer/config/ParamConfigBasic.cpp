/// @file
///
/// @brief
///     系统参数基本参数配置，主要配置系统接收数据相关的一次/二次值，频率等信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.21
///
/// 修改历史：
///

#include "StdAfx.h"
#include "ParamConfigBasic.h"
#include "src\service\datalayer\utility\data_utility.h"
#include "src\service\datalayer\DsmDataLayer.h"

static std::wstring ELEM_NODE_ATTR_NAME       = L"value";                ///< 属性值名

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
///    设置系统使用的SCL文件
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
///    获取系统使用的SCL文件
///
std::wstring CParamConfigBasic::GetSclFile()
{
    return m_strSclFile;
}

///
/// @brief
///    设置额定频率
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
///    获取额定频率
///
std::wstring CParamConfigBasic::GetRatedFreq()
{
    return m_strRatedFreq;
}

///
/// @brief
///    设置报文数据类型是一次/二次值
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
///    获取系统设置的报文数据类型
///
enumSmvValueType CParamConfigBasic::GetFrameValueType()
{
    return m_enValueType;
}

///
/// @brief
///    设定额定一次电压
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

            // 更新SMV发送配置中所有控制块中电压通道的一次额定值
            CParamConfigSMVSend::getInstance()->UpdateRatedPrimaryVoltage(fVoltage * 1000);

            // 更新SMV接收缓存的控制块通道配置信息
            CDsmDataLayer::getInstance()->UpdateCachedRatedValues(CHANNEL_TYPE_SMV_VOLTAGE, 1, fVoltage * 1000);
        }
        return true;
     }
    return false;
}

///
/// @brief
///    获取额定一次电压
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
///    设置额定二次电压
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

            // 更新SMV发送配置中所有控制块中电压通道的二次额定值
            CParamConfigSMVSend::getInstance()->UpdateRatedSecondaryVoltage(fVoltage);

            // 更新SMV接收缓存的控制块通道配置信息
            CDsmDataLayer::getInstance()->UpdateCachedRatedValues(CHANNEL_TYPE_SMV_VOLTAGE, 2, fVoltage);
        }
        return true;
    }
    return false;
}

///
/// @brief
///    获取额定二次电压
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
///    设定额定一次电流
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

            // 更新SMV发送配置中所有控制块中电流通道的一次额定值
            CParamConfigSMVSend::getInstance()->UpdateRatedPrimaryCurrent(fCurrent);

            // 更新SMV接收缓存的控制块通道配置信息
            CDsmDataLayer::getInstance()->UpdateCachedRatedValues(CHANNEL_TYPE_SMV_CURRENT, 1, fCurrent);
        }
        return true;
    }
    return false;
}

///
/// @brief
///    获取额定一次电流
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
///    设置额定二次电流
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

            // 更新SMV发送配置中所有控制块中电流通道的二次额定值
            CParamConfigSMVSend::getInstance()->UpdateRatedSecondaryCurrent(fCurrent);

            // 更新SMV接收缓存的控制块通道配置信息
            CDsmDataLayer::getInstance()->UpdateCachedRatedValues(CHANNEL_TYPE_SMV_CURRENT, 2, fCurrent);
        }
        return true;
    }
    return false;
}

///
/// @brief
///    获取额定二次电流
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
///    设置MU额定延时(us)
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

            // 更新SMV发送配置中所有控制块中时间通道的额定值
            CParamConfigSMVSend::getInstance()->UpdateMuRatedDelay(nVal);
        }
        return true;
    }
    return false;
}

///
/// @brief
///    获取MU额定延时
///
std::wstring CParamConfigBasic::GetMuRatedDelay()
{
    return m_strDelay;
}

///
/// @brief
///    设置对时模式
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
///    获取设置的对时模式
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
///    加载配置文件
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
///    获取指定节点的属性数据
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
///     设置指定节点的属性数据
///
/// @param[in]  strElement 要设置数据的元素名字
/// @param[in]  strAttrValue 要设置的属性值
///
/// @return
///     bool    true表示设置成功
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
///     设置指定节点的属性数据
///
/// @param[in]  strElement 要设置数据的元素名字
/// @param[in]  nAttrValue 要设置的属性值
///
/// @return
///     bool    true表示设置成功
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
///    保存配置文件
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
///     获取配置实例接口
///
CParamConfigBasic* CParamConfigBasic::getInstance()
{
    static CParamConfigBasic s_paramConfigBasic;
    return &s_paramConfigBasic;
}

///
/// @brief
///    恢复默认设置
///
bool CParamConfigBasic::Reset()
{
    // 获取备份配置文件路径
    std::wstring strBackup, strRun;
    strBackup = GetAppDir() + DEFAULT_CFG_BACKUP + m_strConfigFile;
    strRun    = GetAppRunTimeCfgDir() + m_strConfigFile;
    // 拷贝备份的配置文件
    if(!CopyFile(strBackup.c_str(),strRun.c_str(), FALSE))
    {
        return false;
    }

    if (!m_markUpXml.Load(strRun))
    {
        return false;
    }

    //加载进内存对应的变量，方便应用快速设置/获取数据
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
///    设置本机1588时钟ID
///
/// @param[in] strClockId 设定本机1588从时钟的时钟ID
///
/// @return
///     bool 设置成功返回true
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
///    设置本机1588时钟端口ID
///
/// @param[in] strClockPortId 设定本机1588从时钟的端口ID
///
/// @return
///     bool 设置成功返回true
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
///    设置本机1588时钟MAC地址
///
/// @param[in] strMac MAC地址字符串，使用长度为5个字节的十六进制字符串标识
///
/// @return
///     bool 设置成功返回true
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
///    设置1588主时钟ID
///
/// @param[in] strClockId 设定本机1588从时钟的时钟ID
///
/// @return
///     bool 设置成功返回true
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
///    设置1588主时钟端口ID
///
/// @param[in] strClockPortId 设定1588主时钟的端口ID
///
/// @return
///     bool 设置成功返回true
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
///    设置1588同步时使用的网络端口
///
/// @param[in]
///
/// @return
///     设置成功返回true
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
///    1588时钟配置完毕需要调用此函数完成底层数据的同步
///
/// @return
///     bool 底层数据设置成功返回true
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
///    转换16进制字符串到对应的字节数组
///    "000102" -- >[0x00,0x01,0x02] (大端)
///    "000102" -- >[0x02,0x01,0x00] (小端)
///
/// @param[in]  strSrcStr 待转换的字符串
/// @param[out] pArray    保留转换结果的缓冲区地址
/// @param[in]  nArraySize 保留结果缓冲区大小
/// @param[in]  bLE        大小端模式，默认小端
///
/// @return
///     bool 转换成功返回true
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
///    使用探测到的1588控制块设置主时钟和端口信息
///
/// @param[in] pItem 指向探测到的1588控制信息
///
/// @return
///     bool 设置成功返回true
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
///    设置系统使用的SCL文件路径
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
///    获取系统使用的SCL文件路径
///
std::wstring CParamConfigBasic::GetSclPath()
{
	return m_strSclPath;
}
#endif
