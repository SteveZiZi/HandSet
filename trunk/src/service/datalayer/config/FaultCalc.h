/// @file
///
/// @brief
///     故障计算
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.9.10
///
/// 修改历史：
///

#ifndef __FAULT_CALC_H__
#define __FAULT_CALC_H__
#include "src/dspclient/sc_calc/ShortCircuitCalc.h"


///
/// @brief
///    阻抗
///
class CImpedance
{
public:
    ///
    /// @brief
    ///    阻抗构造函数
    ///
    CImpedance();

    ///
    /// @brief
    ///    获得阻抗（Ω）
    ///
    /// @return
    ///     double    阻抗
    ///
    double GetImpedance() const;

    ///
    /// @brief
    ///    设置阻抗（Ω）
    ///
    /// @param[in]  val    阻抗
    ///
    void SetImpedance(double val);

    ///
    /// @brief
    ///    获得阻抗角（°）
    ///
    /// @return
    ///     double    阻抗角
    ///
    double GetAngle() const;

    ///
    /// @brief
    ///    设置阻抗角（°）
    ///
    /// @param[in]  val    阻抗角
    ///
    void SetAngle(double val);

    ///
    /// @brief
    ///    获得电阻（Ω）
    ///
    /// @return
    ///     double    电阻
    ///
    double GetResistance() const;

    ///
    /// @brief
    ///    设置电阻（Ω）
    ///
    /// @param[in]  val    电阻
    ///
    void SetResistance(double val);

    ///
    /// @brief
    ///    获得电抗（Ω）
    ///
    /// @return
    ///     double    电抗
    ///
    double GetReactance() const;

    ///
    /// @brief
    ///    设置电抗（Ω）
    ///
    /// @param[in]  val    电抗
    ///
    void SetReactance(double val);

private:
    double                              m_dImpedance;                   ///< 阻抗（Ω）
    double                              m_dAngle;                       ///< 阻抗角（°）
    double                              m_dResistance;                  ///< 电阻（Ω）
    double                              m_dReactance;                   ///< 电抗（Ω）
};


///
/// @brief
///    零序补偿系数
///
class CCompensate
{
public:
    ///
    /// @brief
    ///    零序补偿系数构造函数
    ///
    CCompensate();

    ///
    /// @brief
    ///     获得阻抗
    ///
    /// @return
    ///     CImpedance*     指向阻抗对象的指针
    ///
    CImpedance* GetImpedance() const;

    ///
    /// @brief
    ///     设置阻抗
    ///
    /// @param[in]  val     指向阻抗对象的指针
    ///
    void SetImpedance(CImpedance* val);

    ///
    /// @brief
    ///    获得模值
    ///
    /// @return
    ///     double    模值
    ///
    double GetModelValue() const;

    ///
    /// @brief
    ///    设置模值
    ///
    /// @param[in]  val    模值
    ///
    void SetModelValue(double val);

    ///
    /// @brief
    ///    获得辐角
    ///
    /// @return
    ///     double    辐角
    ///
    double GetAngle() const;

    ///
    /// @brief
    ///    设置辐角
    ///
    /// @param[in]  val    辐角
    ///
    void SetAngle(double val);

    ///
    /// @brief
    ///    获得实部
    ///
    /// @return
    ///     double    实部
    ///
    double GetReal() const;

    ///
    /// @brief
    ///    设置实部
    ///
    /// @param[in]  val    实部
    ///
    void SetReal(double val);

    ///
    /// @brief
    ///    获得虚部
    ///
    /// @return
    ///     double    虚部
    ///
    double GetVirtual() const;

    ///
    /// @brief
    ///    设置虚部
    ///
    /// @param[in]  val    虚部
    ///
    void SetVirtual(double val);

private:
    CImpedance*                         m_pImpedance;                   ///< 阻抗
    double                              m_dModelValue;                  ///< 模值
    double                              m_dAngle;                       ///< 辐角
    double                              m_dReal;                        ///< 实部
    double                              m_dVirtual;                     ///< 虚部
};


///
/// @brief
///    故障计算
///
class CFaultCalc
{
public:
    ///
    /// @brief
    ///    故障计算构造函数
    ///
    CFaultCalc(void);

    ///
    /// @brief
    ///    获得故障类型
    ///
    /// @return
    ///     SCCalc::SCType    故障类型
    ///
    SCCalc::SCType GetFaultType() const;

    ///
    /// @brief
    ///    设置故障类型
    ///
    /// @param[in]  val    故障类型
    ///
    void SetFaultType(SCCalc::SCType val);

    ///
    /// @brief
    ///    获得计算模型
    ///
    /// @return
    ///     SCCalc::CalcModelType    计算模型
    ///
    SCCalc::CalcModelType GetCalcModelType() const;

    ///
    /// @brief
    ///    设置计算模型
    ///
    /// @param[in]  val    计算模型
    ///
    void SetCalcModelType(SCCalc::CalcModelType val);

    ///
    /// @brief
    ///    获得故障方向
    ///
    /// @return
    ///     DirectionType    故障方向
    ///
    SCCalc::DirectionType GetDirection() const;

    ///
    /// @brief
    ///    设置故障方向
    ///
    /// @param[in]  val    故障方向
    ///
    void SetDirection(SCCalc::DirectionType val);

    ///
    /// @brief
    ///    获得额定电压（V）
    ///
    /// @return
    ///     double    额定电压
    ///
    double GetRatedVoltage() const;

    ///
    /// @brief
    ///    设置额定电压（V）
    ///
    /// @param[in]  val    额定电压
    ///
    void SetRatedVoltage(double val);

    ///
    /// @brief
    ///    获得负荷电流（A）
    ///
    /// @return
    ///     double    负荷电流
    ///
    double GetLoadCurrent() const;

    ///
    /// @brief
    ///    设置负荷电流（A）
    ///
    /// @param[in]  val    负荷电流
    ///
    void SetLoadCurrent(double val);

    ///
    /// @brief
    ///    获得负荷功角（°）
    ///
    /// @return
    ///     double    负荷功角
    ///
    double GetLoadAngle() const;

    ///
    /// @brief
    ///    设置负荷功角（°）
    ///
    /// @param[in]  val    负荷功角
    ///
    void SetLoadAngle(double val);

    ///
    /// @brief
    ///    获得线路阻抗Zl
    ///
    /// @return
    ///     CImpedance    线路阻抗Zl
    ///
    CImpedance GetLineImpedanceZl() const;

    ///
    /// @brief
    ///    设置线路阻抗Zl
    ///
    /// @param[in]  val    线路阻抗Zl
    ///
    void SetLineImpedanceZl(CImpedance val);

    ///
    /// @brief
    ///    获得线路零序补偿系数Kl
    ///
    /// @return
    ///     CCompensate    线路零序补偿系数
    ///
    CCompensate GetLineCompensateKl() const;

    ///
    /// @brief
    ///    设置线路零序补偿系数Kl
    ///
    /// @param[in]  val    线路零序补偿系数
    ///
    void SetLineCompensateKl(CCompensate val);

    ///
    /// @brief
    ///    获得系统阻抗Zs
    ///
    /// @return
    ///     CImpedance    系统阻抗
    ///
    CImpedance GetSystemImpedanceZs() const;

    ///
    /// @brief
    ///    设置系统阻抗Zs
    ///
    /// @param[in]  val    系统阻抗
    ///
    void SetSystemImpedanceZs(CImpedance val);

    ///
    /// @brief
    ///    获得系统零序补偿系数Ks
    ///
    /// @return
    ///     CCompensate    系统零序补偿系数
    ///
    CCompensate GetSystemCompensateKs() const;

    ///
    /// @brief
    ///    设置系统零序补偿系数Ks
    ///
    /// @param[in]  val    系统零序补偿系数
    ///
    void SetSystemCompensateKs(CCompensate val);

    ///
    /// @brief
    ///    获得故障电压（V）
    ///
    /// @return
    ///     double    故障电压
    ///
    double GetFaultVoltage() const;

    ///
    /// @brief
    ///    设置故障电压（V）
    ///
    /// @param[in]  val    故障电压
    ///
    void SetFaultVoltage(double val);

    ///
    /// @brief
    ///    获得故障电流（A）
    ///
    /// @return
    ///     double    故障电流
    ///
    double GetFaultCurrent() const;

    ///
    /// @brief
    ///    设置故障电流（A）
    ///
    /// @param[in]  val    故障电流
    ///
    void SetFaultCurrent(double val);

private:
    SCCalc::SCType                      m_eFaultType;                   ///< 故障类型
    SCCalc::CalcModelType               m_eCalcModelType;               ///< 计算模型
    SCCalc::DirectionType               m_eDirection;                   ///< 故障方向
    double                              m_dRatedVoltage;                ///< 额定电压（V）
    double                              m_dLoadCurrent;                 ///< 负荷电流（A）
    double                              m_dLoadAngle;                   ///< 负荷功角（°）
    CImpedance                          m_stLineImpedanceZl;            ///< 线路阻抗Zl
    CCompensate                         m_stLineCompensateKl;           ///< 线路零序补偿系数Kl
    CImpedance                          m_stSystemImpedanceZs;          ///< 系统阻抗Zs
    CCompensate                         m_stSystemCompensateKs;         ///< 系统零序补偿系数Ks
    double                              m_dFaultVoltage;                ///< 故障电压（V）
    double                              m_dFaultCurrent;                ///< 故障电流（A）
};

#endif
