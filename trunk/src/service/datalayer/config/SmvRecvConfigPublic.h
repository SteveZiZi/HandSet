/// @file
///
/// @brief
///     SMV接收配置公用配置部分
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.26
///
/// 修改历史：
///

#pragma once

#include "src/utils/cmarkup/markup.h"
#include <string>
#include <map>
#include "src\service\datalayer\include\datatypes.h"

///
/// @brief
///    定义9-2报文的采样率信息，报文中没有采样值，需要设置
///
enum SMV92_SAMPLE_RATE
{
    SMV92_SAMPLE_RATE_4K = 1,                                           ///< 采样频率4k
    SMV92_SAMPLE_RATE_12P8K,                                            ///< 采样频率12.8k
    SMV92_SAMPLE_RATE_AUTO_DETECT                                       ///< 9-2采样率自动探测
};

///
/// @brief
///    指定核相时数据对齐方式
///
enum SMV_PHASE_CHECK_ALIGNMENT
{
    SMV_PHASE_CHECK_ALIGN_TIME = 1,                                     ///< 采样值按照时间对齐
    SMV_PHASE_CHECK_ALIGN_INDEX                                         ///< 采样值按照序号对齐
};

///
/// @brief
///    定义谐波信息的标识
///
enum HARMONIC_TYPE
{
    HARMONIC_TYPE_BASE_WAVE = 1,                                        ///< 基波，一次谐波
    HARMONIC_TYPE_2,                                                    ///< 2次谐波
    HARMONIC_TYPE_3,                                                    ///< 3次谐波
    HARMONIC_TYPE_4,                                                    ///< 4次谐波
    HARMONIC_TYPE_5,                                                    ///< 5次谐波
    HARMONIC_TYPE_6,                                                    ///< 6次谐波
    HARMONIC_TYPE_7,                                                    ///< 7次谐波
    HARMONIC_TYPE_8,                                                    ///< 8次谐波
    HARMONIC_TYPE_9,                                                    ///< 9次谐波
    HARMONIC_TYPE_10,                                                   ///< 10次谐波
    HARMONIC_TYPE_11,                                                   ///< 11次谐波
    HARMONIC_TYPE_12,                                                   ///< 12次谐波
    HARMONIC_TYPE_13,                                                   ///< 13次谐波
    HARMONIC_TYPE_14,                                                   ///< 14次谐波
    HARMONIC_TYPE_15,                                                   ///< 15次谐波
    HARMONIC_TYPE_16,                                                   ///< 16次谐波
    HARMONIC_TYPE_17,                                                   ///< 17次谐波
    HARMONIC_TYPE_18,                                                   ///< 18次谐波
    HARMONIC_TYPE_19,                                                   ///< 19次谐波
    HARMONIC_TYPE_20,                                                   ///< 20次谐波
    HARMONIC_TYPE_21,                                                   ///< 21次谐波
    HARMONIC_TYPE_THD,                                                  ///< 谐波含有率
    HARMONIC_TYPE_DC                                                    ///< 直流分量
};

///
/// @brief
///    极性校验互感器类型
///
enum TRANSFORMER_TYPE
{
    TRANSFORMER_TYPE_LEGACY = 1,                                        ///< 旧式互感器
    TRANSFORMER_TYPE_ELECTRONIC,                                        ///< 电子互感器
    TRANSFORMER_TYPE_OTHER                                              ///< 其他类型互感器
};


class CSmvRecvConfigPublic
{
private:
    CSmvRecvConfigPublic(void);
    ~CSmvRecvConfigPublic(void);


public:
    ///
    /// @brief
    ///    获取配置实例接口
    ///
    static CSmvRecvConfigPublic* getInstance();

//
//=============================================================================
// 基本设置/获取接口
//=============================================================================
//
    ///
    /// @brief
    ///    设置界面显示数据类型
    ///
    /// @param[in]  enSmvDispType   要设置的显示数据类型
    ///
    void SetSmvDispType(enumSmvValueType enSmvDispType);

    ///
    /// @brief
    ///    获取界面显示数据类型
    ///
    /// @return
    ///     返回当前设置的界面显示数据类型
    ///
    enumSmvValueType GetSmvDispType();

    ///
    /// @brief
    ///    设置采样率信息
    ///
    /// @param[in]  enSampleRate   要设置的采样率
    ///
    void SetSmv92SampleRate(SMV92_SAMPLE_RATE enSampleRate, bool bUpdateSelectCtrlBlk = true);


    ///
    /// @brief
    ///    获取9-2采样率信息
    ///
    /// @return
    ///     返回当前设置的9-2采样率信息
    ///
    SMV92_SAMPLE_RATE GetSmv92SampleRate();

//
//=============================================================================
// 谐波信息设置/获取
//=============================================================================
//
    ///
    /// @brief
    ///    设置是否显示某次谐波信息
    ///
    /// @param[in]   enHarmonicType 要设置的谐波类型
    /// @param[in]   bEnable        是否显示此类型的谐波信息
    ///
    /// @return
    ///     true表示设置成功
    ///
    bool SetHarmonicTypeEnable(HARMONIC_TYPE enHarmonicType, bool bEnable);

    ///
    /// @brief
    ///    获取某次谐波的是否显示情况
    ///
    /// @param[in]  enHarmonicType  要获取的谐波类型
    ///
    /// @return
    ///     返回true表示此类型的谐波会显示
    ///
    bool GetHarmonicTypeEnabled(HARMONIC_TYPE enHarmonicType);

//
//=============================================================================
// 核相业务设置/获取接口
//=============================================================================
//
    ///
    /// @brief
    ///    设置核相时采样值对齐方式
    ///
    /// @param[in] enPhaseCheckAlign 指定对齐方式
    ///
    void SetPhaseCheckAlign(SMV_PHASE_CHECK_ALIGNMENT enPhaseCheckAlign);

    ///
    /// @brief
    ///    获取核相是采样值对齐方式
    ///
    /// @return
    ///     SMV_PHASE_CHECK_ALIGNMENT 返回核相时数据对齐方式
    ///
    SMV_PHASE_CHECK_ALIGNMENT GetPhaseCheckAlign();

//
//=============================================================================
// 极性业务设置/获取接口
//=============================================================================
//
    ///
    /// @brief
    ///    设置互感器类型
    ///
    void SetTransformerType(TRANSFORMER_TYPE enTransformerType)
    {
        if (enTransformerType != m_enTransFormerType)
        {
            m_enTransFormerType = enTransformerType;
            m_bModified = true;
        }
    }

    ///
    /// @brief
    ///    获取互感器类型
    ///
    TRANSFORMER_TYPE GetTransformerType()
    {
        return m_enTransFormerType;
    }

    ///
    /// @brief
    ///    设置门槛值
    ///
    void SetSillValue(float fSillValue)
    {
        m_fSillValue = fSillValue;
        m_bModified = true;
    }

    ///
    /// @brief
    ///    获取门槛值
    ///
    float GetSillValue()
    {
        return m_fSillValue;
    }

    ///
    /// @brief
    ///    设置零漂值
    ///
    void SetNullShiftValue(float fNullShiftValue)
    {
        m_fNullShiftValue = fNullShiftValue;
        m_bModified = true;
    }

    ///
    /// @brief
    ///    获取零漂值
    ///
    float GetNullShiftValue()
    {
        return m_fNullShiftValue;
    }

//
//=============================================================================
// 配置保存/加载
//=============================================================================

    ///
    /// @brief
    ///    获取指定节点的属性数据
    ///
    std::wstring GetElemAttrVal(const std::wstring& strGroup, const std::wstring& strElement);

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
    bool SetElemAttrVal(const std::wstring& strGroup, const std::wstring& strElement, const std::wstring& strAttrValue);

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
    bool SetElemAttrVal(const std::wstring& strGroup, const std::wstring& strElement, int nAttrValue);

    ///
    /// @brief
    ///    从配置文件中加载信息
    ///
    /// @param[in]   strConfigPath  配置文件名字(相对路径)
    ///
    /// @return
    ///     加载成功返回true
    ///
    bool LoadConfigFile(std::wstring strConfigFile);

    ///
    /// @brief
    ///    保存当前信息到配置文件
    ///
    /// @param[in]   strConfigPath  配置文件名字(相对路径)
    ///
    /// @return
    ///     保存成功返回true
    ///
    bool SaveToConfigFile();

    ///
    /// @brief
    ///    恢复默认设置
    ///
    bool Reset();

    private:

    ///
    /// @brief
    ///    将m_markUpXml内容加载到内存的变量中
    ///
    void _LoadToMem();

private:
    bool                                        m_bModified;            ///< 配置内容是否被修改
    std::wstring                                m_strCfgFile;           ///< 配置文件路径
    CMarkup                                     m_markUpXml;            ///< CMarkup对象

    enumSmvValueType                            m_enSmvDispType;        ///< 界目显示数据的数值类型
    SMV92_SAMPLE_RATE                           m_enSmv92SampleRate;    ///< 9-2采样率信息
    SMV_PHASE_CHECK_ALIGNMENT                   m_enPhaseCheckAlign;    ///< 指定核相时采样值对齐方式
    std::map<HARMONIC_TYPE, bool>               m_mapHarmonicType;      ///< 保存不同类型的谐波是否需要显示
    TRANSFORMER_TYPE                            m_enTransFormerType;    ///< 极性校验使用的互感器类型
    float                                       m_fSillValue;           ///< 极性校验设置的门槛值
    float                                       m_fNullShiftValue;      ///< 极性校验设置的零漂值
};
