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

#pragma once
#include <string>
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\include\datatypes.h"
#include "src\include\stdint.h"
#include "src\utils\CMarkup\Markup.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"

#define ATTR_SCLFILE                            L"sclfile"                  ///< SCL�ļ���

#if !CHUN_HUA
#define ATTR_SCLPATH                            L"sclpath"                  ///< SCL�ļ�·��
#endif

#define ATTR_RATED_FREQ                         L"ratedfreq"                ///< �Ƶ��
#define ATTR_RATED_FRAME_VALUE_TYPE             L"ratedframevaluetype"      ///< �����֡��������
#define ATTR_RATED_VOL_PRIMARY                  L"ratedvolprimary"          ///< ��ѹһ�ζֵ
#define ATTR_RATED_VOL_SECONDARY                L"ratedvolsecondary"        ///< ��ѹ���ζֵ
#define ATTR_RATED_CUR_PRIMARY                  L"ratedcurprimary"          ///< ����һ�ζֵ
#define ATTR_RATED_CUR_SECONDARY                L"ratedcursecondary"        ///< �������ζֵ
#define ATTR_RATED_MU_DELAY                     L"ratedmudelay"             ///< MU���ʱ
#define ATTR_RATED_TIME_SYNC_MODE               L"ratedtimesyncmode"        ///< ��ʱ����
#define ATTR_IEEE_1588_SELF_CLOCK_ID            L"selfclockid"              ///< �豸����ʱ��id
#define ATTR_IEEE_1588_SELF_CLOCK_PORT_ID       L"selfclockportid"          ///< �豸����ʱ�Ӷ˿�id
#define ATTR_IEEE_1588_SELF_MAC                 L"selfclockmac"             ///< �豸����1588��ʱMAC��ַ
#define ATTR_IEEE_1588_MAIN_CLOCK_ID            L"mainclockid"              ///< ��ʱ��id
#define ATTR_IEEE_1588_MAIN_CLOCK_PORT_ID       L"mainclockportid"          ///< ��ʱ�Ӷ˿�id
#define ATTR_IEEE_1588_CLOCK_SYNC_PORT          L"ieee1588clocksyncport"    ///< 1588ʱ��ͬ��ʹ�ö˿ں�

class CParamConfigBasic
{
private:
    CParamConfigBasic(void);
    ~CParamConfigBasic(void);

public:
    ///
    /// @brief
    ///     ��ȡ����ʵ���ӿ�
    ///
    static CParamConfigBasic* getInstance();
//
//=============================================================================
// ϵͳ�������ýӿ�
//=============================================================================
//
    ///
    /// @brief
    ///    ����ϵͳʹ�õ�SCL�ļ�
    ///
    void SetSclFile(std::wstring strSclFile);

    ///
    /// @brief
    ///    ��ȡϵͳʹ�õ�SCL�ļ�
    ///
    std::wstring GetSclFile();

    ///
    /// @brief
    ///    ���öƵ��
    ///
    bool SetRatedFreq(std::wstring strFreq);

    ///
    /// @brief
    ///    ��ȡ�Ƶ��
    ///
    std::wstring GetRatedFreq();

    ///
    /// @brief
    ///    ���ñ�������������һ��/����ֵ
    ///
    bool SetFrameValueType(enumSmvValueType enValueType);

    ///
    /// @brief
    ///    ��ȡϵͳ���õı�����������
    ///
    enumSmvValueType GetFrameValueType();

#if !CHUN_HUA
	///
	/// @brief
	///    ���ñ�������������һ��/����ֵ
	///
	bool SetHasVlan(bool hasVlanTag);

	///
	/// @brief
	///    ��ȡϵͳ���õı�����������
	///
	bool GetHasVlan();
	///
	/// @brief
	///    ����ϵͳʹ�õ�SCL�ļ�
	///
	void SetSclPath(std::wstring strSclFile);

	///
	/// @brief
	///    ��ȡϵͳʹ�õ�SCL�ļ�
	///
	std::wstring GetSclPath();


#endif

    ///
    /// @brief
    ///    �趨�һ�ε�ѹ
    ///
    bool SetRatedPrimaryVoltage(float fVoltage);

    ///
    /// @brief
    ///    ��ȡ�һ�ε�ѹ
    ///
    float GetRatedPrimaryVoltage();

    ///
    /// @brief
    ///    ���ö���ε�ѹ
    ///
    bool SetRatedSecondaryVoltage(float fVoltage);

    ///
    /// @brief
    ///    ��ȡ����ε�ѹ
    ///
    float GetRatedSecondaryVoltage();

    ///
    /// @brief
    ///    �趨�һ�ε���
    ///
    bool SetRatedPrimaryCurrent(float fCurrent);

    ///
    /// @brief
    ///    ��ȡ�һ�ε���
    ///
    float GetRatedPrimaryCurrent();

    ///
    /// @brief
    ///    ���ö���ε���
    ///
    bool SetRatedSecondaryCurrent(float fCurrent);

    ///
    /// @brief
    ///    ��ȡ����ε���
    ///
    float GetRatedSecondaryCurrent();

    ///
    /// @brief
    ///    ����MU���ʱ(us)
    ///
    bool SetMuRatedDelay(std::wstring  struSeconds);

    ///
    /// @brief
    ///    ��ȡMU���ʱ
    ///
    std::wstring GetMuRatedDelay();

    ///
    /// @brief
    ///    ���ö�ʱģʽ
    ///
    void SetTimeSyncMode(FpgaTimeSyncMode enTimeSyncMode);

    ///
    /// @brief
    ///    ��ȡ���õĶ�ʱģʽ
    ///
    FpgaTimeSyncMode GetTimeSyncMode();

    ///
    /// @brief
    ///    ���ñ���1588ʱ��ID
    ///
    /// @param[in] strClockId �趨����1588��ʱ�ӵ�ʱ��ID
    ///
    /// @return
    ///     bool ���óɹ�����true
    ///
    bool SetIeee1588SelfClockId(const std::wstring& strClockId);

    ///
    /// @brief
    ///    ��ȡ����1588ʱ��ID
    ///
    /// @return
    ///     std::wstring ��ȡ����1588��ʱ�ӵ�ʱ��ID
    ///
    std::wstring GetIeee1588SelfClockId()
    {
        return m_strSelfIeee1588ClockId;
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
    bool SetIeee1588SelfClockPortId(const std::wstring& strClockPortId);

    ///
    /// @brief
    ///    ��ȡ����1588ʱ�Ӷ˿�ID
    ///
    /// @return
    ///     std::wstring ��ȡ����1588��ʱ�ӵ�ʱ�Ӷ˿�ID
    ///
    std::wstring GetIeee1588SelfClockPortId()
    {
        return m_strSelfIeee1588PortId;
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
    bool SetIeee1588SelfClockMAC(const std::wstring& strMac);

    ///
    /// @brief
    ///    ��ȡ����1588ʱ��MAC��ַ
    ///
    /// @return
    ///     std::wstring ���ر���1588ʱ��MAC��ַ
    ///
    std::wstring GetIeee1588SelfClockMAC()
    {
        return m_strSelfIeee1588Mac;
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
    bool SetIeee1588MainClockId(const std::wstring& strClockId);

    ///
    /// @brief
    ///    ��ȡ1588��ʱ��ID
    ///
    /// @return
    ///     std::wstring ��ȡ1588��ʱ�ӵ�ʱ��ID
    ///
    std::wstring GetIeee1588MainClockId()
    {
        return m_strMainIeee1588ClockId;
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
    bool SetIeee1588MainClockPortId(const std::wstring& strClockPortId);

    ///
    /// @brief
    ///    ��ȡ1588��ʱ�Ӷ˿�ID
    ///
    /// @return
    ///     std::wstring ��ȡ1588��ʱ�ӵ�ʱ�Ӷ˿�ID
    ///
    std::wstring GetIeee1588MainClockPortId()
    {
        return m_strMainIeee1588PortId;
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
    bool SetIeee1588ClockSyncPort(DsmPortName enPortName);

    ///
    /// @brief
    ///    ��ȡ1588ͬ��ʱʹ�õ�����˿�
    ///
    /// @return
    ///     ���ر�����ǰʹ�õĶ�ʱ�˿�
    ///
    DsmPortName GetIeee1588ClockSyncPort()
    {
        return m_enIeee1588SyncPortName;
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
    bool SetIeee1588MainClockByDetectItem(CNet1588DetectItem* pItem);

    ///
    /// @brief
    ///    1588ʱ�����������Ҫ���ô˺�����ɵײ����ݵ�ͬ��
    ///
    /// @return
    ///     bool �ײ��������óɹ�����true
    ///
    bool Ieee1588ConfigFinished();

//
//=============================================================================
// Xml�ļ���д
//=============================================================================
//
    ///
    /// @brief
    ///    ��ȡָ���ڵ����������
    ///
    std::wstring GetElemAttrVal(std::wstring strElement);

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
    bool SetElemAttrVal( std::wstring strElement, std::wstring& strAttrValue);

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
    bool SetElemAttrVal( std::wstring strElement, int nAttrValue);

    ///
    /// @brief
    ///    ���������ļ�,��Ҫ�ļ������·��
    ///
    bool LoadConfig(std::wstring strConfigFile = PARAM_CONFIG_BASIC);

    ///
    /// @brief
    ///    ���������ļ�
    ///
    bool SaveConfig();

    ///
    /// @brief
    ///    �ָ�Ĭ������
    ///
    bool Reset();

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
    bool _HexString2ByteArray(const std::wstring& strSrcStr, uint8_t* pArray, uint32_t nArraySize, bool bLE = true);

private:
    bool                                m_bModified;                    ///< �����Ƿ��޸Ĺ�
    std::wstring                        m_strConfigFile;                ///< ��ǰʹ�õ������ļ�
    CMarkup                             m_markUpXml;                    ///< ����xmlʹ�õ�CMarkUp����

    FpgaTimeSyncMode                    m_enTimeSyncMode;               ///< ʱ��ͬ����ʽ
    enumSmvValueType                    m_enValueType;                  ///< ���յ��ı�������
    std::wstring                        m_strPriVol;                    ///< �һ�ε�ѹ
    std::wstring                        m_strPriAmp;                    ///< �һ�ε���
    std::wstring                        m_strSecVol;                    ///< ����ε�ѹ
    std::wstring                        m_strSecAmp;                    ///< ����ε���
    std::wstring                        m_strRatedFreq;                 ///< �Ƶ��
    std::wstring                        m_strDelay;                     ///< �Mu��ʱ
    std::wstring                        m_strSclFile;                   ///< SCL�ļ�
    std::wstring                        m_strSelfIeee1588ClockId;       ///< ����1588ʱ��ID
    std::wstring                        m_strSelfIeee1588PortId;        ///< ����1588ʱ�Ӷ˿�ID
    std::wstring                        m_strSelfIeee1588Mac;           ///< ����1588ʱ��MAC
    std::wstring                        m_strMainIeee1588ClockId;       ///< ������ʱ�ӵ�ID
    std::wstring                        m_strMainIeee1588PortId;        ///< ������ʱ�ӵĶ˿�ID
    DsmPortName                         m_enIeee1588SyncPortName;       ///< ����ʹ�õĶ�ʱ�˿�

#if !CHUN_HUA
	bool								m_bHasVlanTag;
	std::wstring                        m_strSclPath;                   ///< SCL�ļ�·��
#endif



};
