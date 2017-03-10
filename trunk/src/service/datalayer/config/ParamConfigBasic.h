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

#pragma once
#include <string>
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\include\datatypes.h"
#include "src\include\stdint.h"
#include "src\utils\CMarkup\Markup.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"

#define ATTR_SCLFILE                            L"sclfile"                  ///< SCL文件名

#if !CHUN_HUA
#define ATTR_SCLPATH                            L"sclpath"                  ///< SCL文件路径
#endif

#define ATTR_RATED_FREQ                         L"ratedfreq"                ///< 额定频率
#define ATTR_RATED_FRAME_VALUE_TYPE             L"ratedframevaluetype"      ///< 额定数据帧数据类型
#define ATTR_RATED_VOL_PRIMARY                  L"ratedvolprimary"          ///< 电压一次额定值
#define ATTR_RATED_VOL_SECONDARY                L"ratedvolsecondary"        ///< 电压二次额定值
#define ATTR_RATED_CUR_PRIMARY                  L"ratedcurprimary"          ///< 电流一次额定值
#define ATTR_RATED_CUR_SECONDARY                L"ratedcursecondary"        ///< 电流二次额定值
#define ATTR_RATED_MU_DELAY                     L"ratedmudelay"             ///< MU额定延时
#define ATTR_RATED_TIME_SYNC_MODE               L"ratedtimesyncmode"        ///< 对时方案
#define ATTR_IEEE_1588_SELF_CLOCK_ID            L"selfclockid"              ///< 设备自身时钟id
#define ATTR_IEEE_1588_SELF_CLOCK_PORT_ID       L"selfclockportid"          ///< 设备自身时钟端口id
#define ATTR_IEEE_1588_SELF_MAC                 L"selfclockmac"             ///< 设备自身1588对时MAC地址
#define ATTR_IEEE_1588_MAIN_CLOCK_ID            L"mainclockid"              ///< 主时钟id
#define ATTR_IEEE_1588_MAIN_CLOCK_PORT_ID       L"mainclockportid"          ///< 主时钟端口id
#define ATTR_IEEE_1588_CLOCK_SYNC_PORT          L"ieee1588clocksyncport"    ///< 1588时钟同步使用端口号

class CParamConfigBasic
{
private:
    CParamConfigBasic(void);
    ~CParamConfigBasic(void);

public:
    ///
    /// @brief
    ///     获取配置实例接口
    ///
    static CParamConfigBasic* getInstance();
//
//=============================================================================
// 系统参数设置接口
//=============================================================================
//
    ///
    /// @brief
    ///    设置系统使用的SCL文件
    ///
    void SetSclFile(std::wstring strSclFile);

    ///
    /// @brief
    ///    获取系统使用的SCL文件
    ///
    std::wstring GetSclFile();

    ///
    /// @brief
    ///    设置额定频率
    ///
    bool SetRatedFreq(std::wstring strFreq);

    ///
    /// @brief
    ///    获取额定频率
    ///
    std::wstring GetRatedFreq();

    ///
    /// @brief
    ///    设置报文数据类型是一次/二次值
    ///
    bool SetFrameValueType(enumSmvValueType enValueType);

    ///
    /// @brief
    ///    获取系统设置的报文数据类型
    ///
    enumSmvValueType GetFrameValueType();

#if !CHUN_HUA
	///
	/// @brief
	///    设置报文数据类型是一次/二次值
	///
	bool SetHasVlan(bool hasVlanTag);

	///
	/// @brief
	///    获取系统设置的报文数据类型
	///
	bool GetHasVlan();
	///
	/// @brief
	///    设置系统使用的SCL文件
	///
	void SetSclPath(std::wstring strSclFile);

	///
	/// @brief
	///    获取系统使用的SCL文件
	///
	std::wstring GetSclPath();


#endif

    ///
    /// @brief
    ///    设定额定一次电压
    ///
    bool SetRatedPrimaryVoltage(float fVoltage);

    ///
    /// @brief
    ///    获取额定一次电压
    ///
    float GetRatedPrimaryVoltage();

    ///
    /// @brief
    ///    设置额定二次电压
    ///
    bool SetRatedSecondaryVoltage(float fVoltage);

    ///
    /// @brief
    ///    获取额定二次电压
    ///
    float GetRatedSecondaryVoltage();

    ///
    /// @brief
    ///    设定额定一次电流
    ///
    bool SetRatedPrimaryCurrent(float fCurrent);

    ///
    /// @brief
    ///    获取额定一次电流
    ///
    float GetRatedPrimaryCurrent();

    ///
    /// @brief
    ///    设置额定二次电流
    ///
    bool SetRatedSecondaryCurrent(float fCurrent);

    ///
    /// @brief
    ///    获取额定二次电流
    ///
    float GetRatedSecondaryCurrent();

    ///
    /// @brief
    ///    设置MU额定延时(us)
    ///
    bool SetMuRatedDelay(std::wstring  struSeconds);

    ///
    /// @brief
    ///    获取MU额定延时
    ///
    std::wstring GetMuRatedDelay();

    ///
    /// @brief
    ///    设置对时模式
    ///
    void SetTimeSyncMode(FpgaTimeSyncMode enTimeSyncMode);

    ///
    /// @brief
    ///    获取设置的对时模式
    ///
    FpgaTimeSyncMode GetTimeSyncMode();

    ///
    /// @brief
    ///    设置本机1588时钟ID
    ///
    /// @param[in] strClockId 设定本机1588从时钟的时钟ID
    ///
    /// @return
    ///     bool 设置成功返回true
    ///
    bool SetIeee1588SelfClockId(const std::wstring& strClockId);

    ///
    /// @brief
    ///    获取本机1588时钟ID
    ///
    /// @return
    ///     std::wstring 获取本机1588从时钟的时钟ID
    ///
    std::wstring GetIeee1588SelfClockId()
    {
        return m_strSelfIeee1588ClockId;
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
    bool SetIeee1588SelfClockPortId(const std::wstring& strClockPortId);

    ///
    /// @brief
    ///    获取本机1588时钟端口ID
    ///
    /// @return
    ///     std::wstring 获取本机1588从时钟的时钟端口ID
    ///
    std::wstring GetIeee1588SelfClockPortId()
    {
        return m_strSelfIeee1588PortId;
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
    bool SetIeee1588SelfClockMAC(const std::wstring& strMac);

    ///
    /// @brief
    ///    获取本机1588时钟MAC地址
    ///
    /// @return
    ///     std::wstring 返回本机1588时钟MAC地址
    ///
    std::wstring GetIeee1588SelfClockMAC()
    {
        return m_strSelfIeee1588Mac;
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
    bool SetIeee1588MainClockId(const std::wstring& strClockId);

    ///
    /// @brief
    ///    获取1588主时钟ID
    ///
    /// @return
    ///     std::wstring 获取1588主时钟的时钟ID
    ///
    std::wstring GetIeee1588MainClockId()
    {
        return m_strMainIeee1588ClockId;
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
    bool SetIeee1588MainClockPortId(const std::wstring& strClockPortId);

    ///
    /// @brief
    ///    获取1588主时钟端口ID
    ///
    /// @return
    ///     std::wstring 获取1588主时钟的时钟端口ID
    ///
    std::wstring GetIeee1588MainClockPortId()
    {
        return m_strMainIeee1588PortId;
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
    bool SetIeee1588ClockSyncPort(DsmPortName enPortName);

    ///
    /// @brief
    ///    获取1588同步时使用的网络端口
    ///
    /// @return
    ///     返回本机当前使用的对时端口
    ///
    DsmPortName GetIeee1588ClockSyncPort()
    {
        return m_enIeee1588SyncPortName;
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
    bool SetIeee1588MainClockByDetectItem(CNet1588DetectItem* pItem);

    ///
    /// @brief
    ///    1588时钟配置完毕需要调用此函数完成底层数据的同步
    ///
    /// @return
    ///     bool 底层数据设置成功返回true
    ///
    bool Ieee1588ConfigFinished();

//
//=============================================================================
// Xml文件读写
//=============================================================================
//
    ///
    /// @brief
    ///    获取指定节点的属性数据
    ///
    std::wstring GetElemAttrVal(std::wstring strElement);

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
    bool SetElemAttrVal( std::wstring strElement, std::wstring& strAttrValue);

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
    bool SetElemAttrVal( std::wstring strElement, int nAttrValue);

    ///
    /// @brief
    ///    加载配置文件,需要文件的相对路径
    ///
    bool LoadConfig(std::wstring strConfigFile = PARAM_CONFIG_BASIC);

    ///
    /// @brief
    ///    保存配置文件
    ///
    bool SaveConfig();

    ///
    /// @brief
    ///    恢复默认设置
    ///
    bool Reset();

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
    bool _HexString2ByteArray(const std::wstring& strSrcStr, uint8_t* pArray, uint32_t nArraySize, bool bLE = true);

private:
    bool                                m_bModified;                    ///< 设置是否被修改过
    std::wstring                        m_strConfigFile;                ///< 当前使用的配置文件
    CMarkup                             m_markUpXml;                    ///< 解析xml使用的CMarkUp对象

    FpgaTimeSyncMode                    m_enTimeSyncMode;               ///< 时间同步方式
    enumSmvValueType                    m_enValueType;                  ///< 接收到的报文类型
    std::wstring                        m_strPriVol;                    ///< 额定一次电压
    std::wstring                        m_strPriAmp;                    ///< 额定一次电流
    std::wstring                        m_strSecVol;                    ///< 额定二次电压
    std::wstring                        m_strSecAmp;                    ///< 额定二次电流
    std::wstring                        m_strRatedFreq;                 ///< 额定频率
    std::wstring                        m_strDelay;                     ///< 额定Mu延时
    std::wstring                        m_strSclFile;                   ///< SCL文件
    std::wstring                        m_strSelfIeee1588ClockId;       ///< 本机1588时钟ID
    std::wstring                        m_strSelfIeee1588PortId;        ///< 本机1588时钟端口ID
    std::wstring                        m_strSelfIeee1588Mac;           ///< 本机1588时钟MAC
    std::wstring                        m_strMainIeee1588ClockId;       ///< 网络主时钟的ID
    std::wstring                        m_strMainIeee1588PortId;        ///< 网络主时钟的端口ID
    DsmPortName                         m_enIeee1588SyncPortName;       ///< 本机使用的对时端口

#if !CHUN_HUA
	bool								m_bHasVlanTag;
	std::wstring                        m_strSclPath;                   ///< SCL文件路径
#endif



};
