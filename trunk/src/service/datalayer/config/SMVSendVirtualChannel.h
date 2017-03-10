/// @file
///
/// @brief
///     SMV发送虚拟通道
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.06.07
///
/// 修改历史：
///

#ifndef __SMV_SEND_VIRTUAL_CHANNEL_H__
#define __SMV_SEND_VIRTUAL_CHANNEL_H__
#include <string>
#include "Channel.h"
#include "Harmonic.h"
#include "ParamConfigBase.h"

///
/// @brief
///    功能类型
///
enum eFunctionType
{
    FUNCTION_TYPE_UNKNOWN = 0,
    FUNCTION_TYPE_MANUAL_TEST,                                          ///< 手动试验
    FUNCTION_TYPE_STATE_SEQUENCE                                        ///< 状态序列
};

///
/// @brief
///     比较方式
///
enum enumCompareType
{
    TYPE_FIND_ALL_DIFFERENT = 0,                                        ///< 找出所有的不同
    TYPE_FIND_ONE_DIFFERENT,                                            ///< 找出一处不同
};

///
/// @brief
///    虚拟通道
///
class CSMVSendVirtualChannel
{
    friend class CParamconfigManualTest;
    friend class CStatus;
    friend class CStateTestResult;

public:
    ///
    /// @brief
    ///    构造函数
    ///
    /// @param[in]  eType    功能类型
    /// @param[in]  pParamConfigBase    配置管理器对象
    ///
    CSMVSendVirtualChannel(eFunctionType eType, CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    析构函数
    ///
    virtual ~CSMVSendVirtualChannel(void);

    ///
    /// @brief
    ///    获得虚拟通道的交直流类型
    ///
    /// @return
    ///     eCurrentType    交直流类型
    ///
    eCurrentType GetACDCType();

    ///
    /// @brief
    ///    获得虚拟通道名
    ///
    /// @return
    ///     std::wstring    虚拟通道名
    ///
    std::wstring GetName();

    ///
    /// @brief
    ///    设置虚拟通道名
    ///
    /// @param[in]  strName    虚拟通道名
    ///
    /// @return
    ///     bool    设置成功返回true
    ///
    bool SetName(std::wstring strName);

    ///
    /// @brief
    ///    获得幅值（电压为V，电流为A，时间为us）
    ///
    /// @return
    ///     double    幅值
    ///
    double GetAmplitude();

    ///
    /// @brief
    ///    设置幅值（电压为V，电流为A，时间为us）
    ///
    /// @param[in]  dAmplitude    幅值
    ///
    /// @return
    ///     bool    设置成功返回true
    ///
    bool SetAmplitude(double dAmplitude);

    ///
    /// @brief
    ///     设置通道额定值
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool SetRatedAmplitude();

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
    /// @return
    ///     bool    设置成功返回true
    ///
    bool SetPhaseAngle(double dPhaseAngle);

    ///
    /// @brief
    ///    获得频率
    ///
    /// @return
    ///     double    频率
    ///
    double GetFrequency();

    ///
    /// @brief
    ///    设置频率
    ///
    /// @param[in]  dFrequency    频率
    ///
    /// @return
    ///     bool    设置成功返回true
    ///
    bool SetFrequency(double dFrequency);

    ///
    /// @brief
    ///    获得幅值步长（电压为V，电流为A，时间为us）
    ///
    /// @return
    ///     double    幅值步长
    ///
    double GetAmplitudeStep();

    ///
    /// @brief
    ///    获得相角步长
    ///
    /// @return
    ///     double    相角步长
    ///
    double GetPhaseAngleStep();

    ///
    /// @brief
    ///    获得频率步长
    ///
    /// @return
    ///     double    频率步长
    ///
    double GetFrequencyStep();

    ///
    /// @brief
    ///    设置幅值步长（电压为V，电流为A，时间为us）
    ///
    /// @param[in]  dStep    幅值步长
    ///
    /// @return
    ///     bool    设置成功返回true
    ///
    bool SetAmplitudeStep(double dStep);

    ///
    /// @brief
    ///    设置相角步长
    ///
    /// @param[in]  dStep    幅值步长
    ///
    /// @return
    ///     bool    设置成功返回true
    ///
    bool SetPhaseAngleStep(double dStep);

    ///
    /// @brief
    ///    设置频率步长
    ///
    /// @param[in]  dStep    频率步长
    ///
    /// @return
    ///     bool    设置成功返回true
    ///
    bool SetFrequencyStep(double dStep);

    ///
    /// @brief
    ///    获得谐波设置对象的指针
    ///
    /// @return
    ///     CHarmonic*    谐波设置对象的指针
    ///
    CHarmonic* GetHarmonic();

    ///
    /// @brief
    ///    获得映射到此虚拟通道的真实通道的数量
    ///
    /// @return
    ///     int    映射到此虚拟通道的真实通道的数量
    ///
    int GetChannelCount();

    ///
    /// @brief
    ///    获得SVM通道映射表中被勾选的通道的索引（从0开始计数）
    ///
    /// @return
    ///     int    被勾选的通道的索引
    ///
    int GetSelectedIndex();

    ///
    /// @brief
    ///    设置SVM通道映射表中被勾选的通道的索引（从0开始计数）
    ///
    /// @param[in]  nIndex    被勾选的通道的索引
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool SetSelectedIndex( int nIndex );

    ///
    /// @brief
    ///    枚举第一个Channel
    ///
    /// @return
    ///     CChannel*    成功时返回通道指针，失败时返回NULL
    ///
    CChannel* FirstChannel();

    ///
    /// @brief
    ///    枚举下一个Channel
    ///
    /// @param[in]  pCurChannel 当前Channel的指针
    ///
    /// @return
    ///     CChannel*    成功时返回Channel指针，失败时返回NULL
    ///
    CChannel* NextChannel(CChannel* pCurChannel);

    ///
    /// @brief
    ///    获得指定索引号的映射到此虚拟通道的真实通道
    ///
    /// @param[in]  nIndex    通道索引号
    ///
    /// @return
    ///     CChannel*    成功时返回Channel指针，失败时返回NULL
    ///
    CChannel* GetChannel(int nIndex);

    ///
    /// @brief
    ///    幅值是否发生了变化
    ///
    /// @param[in]  bIsChanged    是否发生了变化
    ///
    bool IsAmplitudeChanged();

    ///
    /// @brief
    ///    相角是否发生了变化
    ///
    /// @param[in]  bIsChanged    是否发生了变化
    ///
    bool IsPhaseAngleChanged();

    ///
    /// @brief
    ///    频率是否发生了变化
    ///
    /// @param[in]  bIsChanged    是否发生了变化
    ///
    bool IsFrequencyChanged();

    ///
    /// @brief
    ///    手动试验界面中虚拟通道是否被勾选
    ///
    /// @return
    ///     bool    是否勾选
    ///
    bool IsSelected() const
    {
        return m_bSelected;
    }

    ///
    /// @brief
    ///    设置手动试验界面中虚拟通道是否被勾选
    ///
    /// @param[in]  val    是否勾选
    ///
    void SetSelected(bool val)
    {
        m_bSelected = val;
    }

    ///
    /// @brief
    ///     比较虚拟通道的幅值、相角、频率、谐波
    ///
    /// @param[in]  eCompareType    比较方式
    /// @param[in]  pOldSMVSendVirtualChannel   与当前通道进行比较的虚拟通道
    ///
    /// @return
    ///     bool    相同返回true
    ///
    bool Compare(enumCompareType eCompareType, CSMVSendVirtualChannel* pOldSMVSendVirtualChannel);

protected:
    ///
    /// @brief
    ///    深度拷贝一个虚拟通道对象
    ///
    /// @param[in]
    ///     pOldVirtualChannel    -   已存在的虚拟通道对象
    ///
    /// @return
    ///     bool    拷贝成功返回TRUE
    ///
    bool __Copy(CSMVSendVirtualChannel* pOldVirtualChannel);

    ///
    /// @brief
    ///    加载SMV发送虚拟通道
    ///
    /// @param[in]  xml    指向XML对象的引用
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    bool __LoadConfig(CMarkup& xml);

    ///
    /// @brief
    ///    保存SMV发送虚拟通道
    ///
    /// @param[in]  xml    指向XML对象的引用
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    bool __SaveConfig(CMarkup& xml);

    ///
    /// @brief
    ///    设置通道的幅值是否发生了变化
    ///
    /// @param[in]  bIsChanged    是否发生了变化
    ///
    void __SetAmplitudeChanged(bool bIsChanged);

    ///
    /// @brief
    ///    设置通道的相角是否发生了变化
    ///
    /// @param[in]  bIsChanged    是否发生了变化
    ///
    void __SetPhaseAngleChanged(bool bIsChanged);

    ///
    /// @brief
    ///    设置通道的频率是否发生了变化
    ///
    /// @param[in]  bIsChanged    是否发生了变化
    ///
    void __SetFrequencyChanged(bool bIsChanged);

private:
    std::wstring                        m_strName;                      ///< 虚拟通道名
    eFunctionType                       m_eFunctionType;                ///< 功能类型（手动试验、状态序列）
    CParamConfigBase*                   m_pParamConfigBase;             ///< 参数配置管理器
    bool                                m_bSelected;                    ///< 手动试验界面中虚拟通道是否被勾选

    eCurrentType                        m_eACDCType;                    ///< 交直流类型
    double                              m_dAmplitude;                   ///< 幅值（一次值）
    double                              m_dPhaseAngle;                  ///< 相角
    double                              m_dFrequency;                   ///< 频率
    double                              m_dAmplitudeStep;               ///< 幅值的步长
    double                              m_dPhaseAngleStep;              ///< 相角的步长
    double                              m_dFrequencyStep;               ///< 频率的步长

    bool                                m_bIsAmplitudeChanged;          ///< 幅值是否发生了变化（不需要保存到文件中）
    bool                                m_bIsPhaseAngleChanged;         ///< 相角是否发生了变化（不需要保存到文件中）
    bool                                m_bIsFrequencyChanged;          ///< 频率是否发生了变化（不需要保存到文件中）

    CHarmonic                           m_harmonic;                     ///< 谐波设置（状态序列）

    int                                 m_iSelectedIndex;               ///< SVM通道映射表中被勾选的通道的索引（从0开始计数）
    std::vector<CChannel *>             m_vecChannelGroup;              ///< 通道容器，用于存放映射到此虚拟通道的真实通道的指针
};
#endif
