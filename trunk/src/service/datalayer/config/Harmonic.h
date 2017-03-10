///
/// @brief
///     谐波类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.6.14
///
/// 修改历史：
///

#ifndef __HARMONIC_H__
#define __HARMONIC_H__
#include "paramconfigbase.h"

///
/// @brief
///    谐波类型
///
enum enumHarmonicType
{
    HARMONIC_UNKNOWN = 0,
    HARMONIC_NONE,                                                      ///< 无谐波
    HARMONIC_DC,                                                        ///< 直流
    HARMONIC_2_TIMES,                                                   ///< 2次谐波
    HARMONIC_3_TIMES,                                                   ///< 3次谐波
    HARMONIC_4_TIMES,                                                   ///< 4次谐波
    HARMONIC_5_TIMES,                                                   ///< 5次谐波
    HARMONIC_6_TIMES,                                                   ///< 6次谐波
    HARMONIC_7_TIMES,                                                   ///< 7次谐波
    HARMONIC_8_TIMES,                                                   ///< 8次谐波
    HARMONIC_9_TIMES,                                                   ///< 9次谐波
    HARMONIC_10_TIMES,                                                  ///< 10次谐波
    HARMONIC_11_TIMES,                                                  ///< 11次谐波
    HARMONIC_12_TIMES,                                                  ///< 12次谐波
    HARMONIC_13_TIMES,                                                  ///< 13次谐波
    HARMONIC_14_TIMES,                                                  ///< 14次谐波
    HARMONIC_15_TIMES,                                                  ///< 15次谐波
    HARMONIC_16_TIMES,                                                  ///< 16次谐波
    HARMONIC_17_TIMES,                                                  ///< 17次谐波
    HARMONIC_18_TIMES,                                                  ///< 18次谐波
    HARMONIC_19_TIMES,                                                  ///< 19次谐波
    HARMONIC_20_TIMES                                                   ///< 20次谐波
};

///
/// @brief
///    谐波
///
class CHarmonic
{
    friend class CSMVSendVirtualChannel;

protected:
    CHarmonic(void);
    virtual ~CHarmonic(void);

public:
    ///
    /// @brief
    ///    构造函数
    ///
    /// @param[in]  pParamConfigBase    参数配置管理器
    ///
    CHarmonic(CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    构造函数
    ///
    /// @param[in]  pSmvSendVirtualChannel  指向虚拟发送通道对象的指针
    /// @param[in]  pParamConfigBase    参数配置管理器
    ///
    CHarmonic(CSMVSendVirtualChannel* pSmvSendVirtualChannel, CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    获得谐波类型
    ///
    /// @return
    ///     enumHarmonicType    谐波类型
    ///
    enumHarmonicType GetHarmonicType();

    ///
    /// @brief
    ///    设置谐波类型
    ///
    /// @param[in]  eNewType    谐波类型
    ///
    void SetHarmonicType(enumHarmonicType eNewType);

    ///
    /// @brief
    ///    获得幅值
    ///
    /// @return
    ///     double    幅值
    ///
    double GetAmplitude();

    ///
    /// @brief
    ///    设置幅值
    ///
    /// @param[in]  dAmplitude    新的幅值
    ///
    void SetAmplitude(double dAmplitude);

    ///
    /// @brief
    ///    获得相角
    ///
    /// @return
    ///     double    相角
    ///
    double GetPhaseAngle();

    ///
    /// @brief
    ///    设置相角
    ///
    /// @param[in]  dPhaseAngle    相角
    ///
    void SetPhaseAngle(double dPhaseAngle);

    ///
    /// @brief
    ///     获得谐波对象所属的虚拟通道
    ///
    /// @return
    ///     CSMVSendVirtualChannel* 指向虚拟通道对象的指针
    ///
    CSMVSendVirtualChannel* GetSmvSendVirtualChannel() const;

    ///
    /// @brief
    ///     设置谐波对象所属的虚拟通道
    ///
    /// @param[in]  val 指向虚拟通道对象的指针
    ///
    void SetSmvSendVirtualChannel(CSMVSendVirtualChannel* val);

    ///
    /// @brief
    ///     比较谐波类型、幅值、相角
    ///
    /// @param[in]  oldHarmonic     待对比的谐波对象
    ///
    /// @return
    ///     bool    相同返回true
    ///
    bool Compare(CHarmonic& oldHarmonic);

private:
    CSMVSendVirtualChannel*             m_pSmvSendVirtualChannel;       ///< 谐波对象所属的虚拟通道
    CParamConfigBase*                   m_pParamConfigBase;             ///< 参数配置管理器

    enumHarmonicType                    m_eHarmonicType;                ///< 谐波类型
    double                              m_dAmplitude;                   ///< 幅值
    double                              m_dPhaseAngle;                  ///< 相角
};
#endif
