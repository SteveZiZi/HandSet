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

#include "StdAfx.h"
#include "FaultCalc.h"


///
/// @brief
///    阻抗构造函数
///
CImpedance::CImpedance()
: m_dImpedance(0.0f)
, m_dAngle(0.0f)
, m_dResistance(0.0f)
, m_dReactance(0.0f)
{
}

///
/// @brief
///    获得阻抗（Ω）
///
/// @return
///     double    阻抗
///
double CImpedance::GetImpedance() const
{
    return m_dImpedance;
}

///
/// @brief
///    设置阻抗（Ω）
///
/// @param[in]  val    阻抗
///
void CImpedance::SetImpedance(double val)
{
    if (val >= 0)
    {
        m_dImpedance    = val;
        m_dResistance   = m_dImpedance * cos(m_dAngle * SCCalc::PI / 180);
        m_dReactance    = m_dImpedance * sin(m_dAngle * SCCalc::PI / 180);
    }
}

///
/// @brief
///    获得阻抗角（°）
///
/// @return
///     double    阻抗角
///
double CImpedance::GetAngle() const
{
    return m_dAngle;
}

///
/// @brief
///    设置阻抗角（°）
///
/// @param[in]  val    阻抗角
///
void CImpedance::SetAngle(double val)
{
    if (val >= 0)
    {
        m_dAngle        = val;
        m_dResistance   = m_dImpedance * cos(m_dAngle * SCCalc::PI / 180);
        m_dReactance    = m_dImpedance * sin(m_dAngle * SCCalc::PI / 180);
    }
}

///
/// @brief
///    获得电阻（Ω）
///
/// @return
///     double    电阻
///
double CImpedance::GetResistance() const
{
    return m_dResistance;
}

///
/// @brief
///    设置电阻（Ω）
///
/// @param[in]  val    电阻
///
void CImpedance::SetResistance(double val)
{
    SCCalc::Vector  stVector;


    if (val >= 0)
    {
        m_dResistance   = val;
        SCCalc::ZLCalc(SCCalc::ZLCT_RX, m_dResistance, m_dReactance, stVector);
        m_dImpedance    = stVector.md;
        m_dAngle        = stVector.dg;
    }
}

///
/// @brief
///    获得电抗（Ω）
///
/// @return
///     double    电抗
///
double CImpedance::GetReactance() const
{
    return m_dReactance;
}

///
/// @brief
///    设置电抗（Ω）
///
/// @param[in]  val    电抗
///
void CImpedance::SetReactance(double val)
{
    SCCalc::Vector  stVector;


    if (val >= 0)
    {
        m_dReactance    = val;
        SCCalc::ZLCalc(SCCalc::ZLCT_RX, m_dResistance, m_dReactance, stVector);
        m_dImpedance    = stVector.md;
        m_dAngle        = stVector.dg;
    }
}

///
/// @brief
///    零序补偿系数构造函数
///
CCompensate::CCompensate()
: m_pImpedance(NULL)
, m_dModelValue(0.667f)
, m_dAngle(0.0f)
, m_dReal(0.667f)
, m_dVirtual(0.0f)
{
}

///
/// @brief
///     获得阻抗
///
/// @return
///     CImpedance*     指向阻抗对象的指针
///
CImpedance* CCompensate::GetImpedance() const
{
    return m_pImpedance;
}

///
/// @brief
///     设置阻抗
///
/// @param[in]  val     指向阻抗对象的指针
///
void CCompensate::SetImpedance( CImpedance* val )
{
    m_pImpedance = val;
}

///
/// @brief
///    获得模值
///
/// @return
///     double    模值
///
double CCompensate::GetModelValue() const
{
    return m_dModelValue;
}

///
/// @brief
///    设置模值
///
/// @param[in]  val    模值
///
void CCompensate::SetModelValue(double val)
{
    double  dCosValue;
    double  dSinValue;


    if ((val >= 0) && (NULL != m_pImpedance))
    {
        dCosValue   = cos(m_pImpedance->GetAngle() * SCCalc::PI / 180);
        dSinValue   = sin(m_pImpedance->GetAngle() * SCCalc::PI / 180);
        if ((fabs(dCosValue - 0.0f) >= 0.000001f) &&
            (fabs(dSinValue - 0.0f) >= 0.000001f))
        {
            m_dModelValue   = val;
            m_dReal         = m_dModelValue * cos(m_dAngle * SCCalc::PI / 180) / dCosValue;
            m_dVirtual      = m_dModelValue * sin(m_dAngle * SCCalc::PI / 180) / dSinValue;
        }
    }
}

///
/// @brief
///    获得辐角
///
/// @return
///     double    辐角
///
double CCompensate::GetAngle() const
{
    return m_dAngle;
}

///
/// @brief
///    设置辐角
///
/// @param[in]  val    辐角
///
void CCompensate::SetAngle(double val)
{
    double  dCosValue;
    double  dSinValue;


    if ((val >= 0) && (NULL != m_pImpedance))
    {
        dCosValue   = cos(m_pImpedance->GetAngle() * SCCalc::PI / 180);
        dSinValue   = sin(m_pImpedance->GetAngle() * SCCalc::PI / 180);
        if ((fabs(dCosValue - 0.0f) >= 0.000001f) &&
            (fabs(dSinValue - 0.0f) >= 0.000001f))
        {
            m_dAngle    = val;
            m_dReal     = m_dModelValue * cos(m_dAngle * SCCalc::PI / 180) / dCosValue;
            m_dVirtual  = m_dModelValue * sin(m_dAngle * SCCalc::PI / 180) / dSinValue;
        }
    }
}

///
/// @brief
///    获得实部
///
/// @return
///     double    实部
///
double CCompensate::GetReal() const
{
    return m_dReal;
}

///
/// @brief
///    设置实部
///
/// @param[in]  val    实部
///
void CCompensate::SetReal(double val)
{
    SCCalc::Vector  stVectorZl;
    SCCalc::Vector  stVectorKl;


    if ((val >= 0) && (NULL != m_pImpedance))
    {
        m_dReal         = val;

        // 计算补偿系数
        stVectorZl.md   = m_pImpedance->GetImpedance();
        stVectorZl.dg   = m_pImpedance->GetAngle();
        SCCalc::KLCalc(SCCalc::KLCT_KRKX, m_dReal, m_dVirtual, stVectorZl, stVectorKl);

        // 保存计算结果
        m_dModelValue   = stVectorKl.md;
        m_dAngle        = stVectorKl.dg;
    }
}

///
/// @brief
///    获得虚部
///
/// @return
///     double    虚部
///
double CCompensate::GetVirtual() const
{
    return m_dVirtual;
}

///
/// @brief
///    设置虚部
///
/// @param[in]  val    虚部
///
void CCompensate::SetVirtual(double val)
{
    SCCalc::Vector  stVectorZl;
    SCCalc::Vector  stVectorKl;


    if ((val >= 0) && (NULL != m_pImpedance))
    {
        m_dVirtual      = val;

        // 计算补偿系数
        stVectorZl.md   = m_pImpedance->GetImpedance();
        stVectorZl.dg   = m_pImpedance->GetAngle();
        SCCalc::KLCalc(SCCalc::KLCT_KRKX, m_dReal, m_dVirtual, stVectorZl, stVectorKl);

        // 保存计算结果
        m_dModelValue   = stVectorKl.md;
        m_dAngle        = stVectorKl.dg;
    }
}

///
/// @brief
///    故障计算构造函数
///
CFaultCalc::CFaultCalc(void)
: m_eFaultType(SCCalc::A_GROUND)
, m_eCalcModelType(SCCalc::SYSTEM_IMPEDANCE_FIXED)
, m_eDirection(SCCalc::POSITIVE)
, m_dRatedVoltage(0.0f)
, m_dLoadCurrent(0.0f)
, m_dLoadAngle(0.0f)
, m_dFaultVoltage(0.0f)
, m_dFaultCurrent(0.0f)
{
    // 设置零序补偿系数的阻抗
    m_stLineCompensateKl.SetImpedance(&m_stLineImpedanceZl);
    m_stSystemCompensateKs.SetImpedance(&m_stSystemImpedanceZs);
}

///
/// @brief
///    获得故障类型
///
/// @return
///     SCCalc::SCType    故障类型
///
SCCalc::SCType CFaultCalc::GetFaultType() const
{
    return m_eFaultType;
}

///
/// @brief
///    设置故障类型
///
/// @param[in]  val    故障类型
///
void CFaultCalc::SetFaultType(SCCalc::SCType val)
{
    m_eFaultType = val;
}

///
/// @brief
///    获得计算模型
///
/// @return
///     SCCalc::CalcModelType    计算模型
///
SCCalc::CalcModelType CFaultCalc::GetCalcModelType() const
{
    return m_eCalcModelType;
}

///
/// @brief
///    设置计算模型
///
/// @param[in]  val    计算模型
///
void CFaultCalc::SetCalcModelType(SCCalc::CalcModelType val)
{
    m_eCalcModelType = val;
}

///
/// @brief
///    获得故障方向
///
/// @return
///     DirectionType    故障方向
///
SCCalc::DirectionType CFaultCalc::GetDirection() const
{
    return m_eDirection;
}

///
/// @brief
///    设置故障方向
///
/// @param[in]  val    故障方向
///
void CFaultCalc::SetDirection(SCCalc::DirectionType val)
{
    m_eDirection = val;
}

///
/// @brief
///    获得额定电压（V）
///
/// @return
///     double    额定电压
///
double CFaultCalc::GetRatedVoltage() const
{
    return m_dRatedVoltage;
}

///
/// @brief
///    设置额定电压（V）
///
/// @param[in]  val    额定电压
///
void CFaultCalc::SetRatedVoltage(double val)
{
    m_dRatedVoltage = val;
}

///
/// @brief
///    获得负荷电流（A）
///
/// @return
///     double    负荷电流
///
double CFaultCalc::GetLoadCurrent() const
{
    return m_dLoadCurrent;
}

///
/// @brief
///    设置负荷电流（A）
///
/// @param[in]  val    负荷电流
///
void CFaultCalc::SetLoadCurrent(double val)
{
    m_dLoadCurrent = val;
}

///
/// @brief
///    获得负荷功角（°）
///
/// @return
///     double    负荷功角
///
double CFaultCalc::GetLoadAngle() const
{
    return m_dLoadAngle;
}

///
/// @brief
///    设置负荷功角（°）
///
/// @param[in]  val    负荷功角
///
void CFaultCalc::SetLoadAngle(double val)
{
    m_dLoadAngle = val;
}

///
/// @brief
///    获得线路阻抗Zl
///
/// @return
///     CImpedance    线路阻抗Zl
///
CImpedance CFaultCalc::GetLineImpedanceZl() const
{
    return m_stLineImpedanceZl;
}

///
/// @brief
///    设置线路阻抗Zl
///
/// @param[in]  val    线路阻抗Zl
///
void CFaultCalc::SetLineImpedanceZl(CImpedance val)
{
    m_stLineImpedanceZl = val;
}

///
/// @brief
///    获得线路零序补偿系数Kl
///
/// @return
///     CCompensate    线路零序补偿系数
///
CCompensate CFaultCalc::GetLineCompensateKl() const
{
    return m_stLineCompensateKl;
}

///
/// @brief
///    设置线路零序补偿系数Kl
///
/// @param[in]  val    线路零序补偿系数
///
void CFaultCalc::SetLineCompensateKl(CCompensate val)
{
    m_stLineCompensateKl = val;
}

///
/// @brief
///    获得系统阻抗Zs
///
/// @return
///     CImpedance    系统阻抗
///
CImpedance CFaultCalc::GetSystemImpedanceZs() const
{
    return m_stSystemImpedanceZs;
}

///
/// @brief
///    设置系统阻抗Zs
///
/// @param[in]  val    系统阻抗
///
void CFaultCalc::SetSystemImpedanceZs(CImpedance val)
{
    m_stSystemImpedanceZs = val;
}

///
/// @brief
///    获得系统零序补偿系数Ks
///
/// @return
///     CCompensate    系统零序补偿系数
///
CCompensate CFaultCalc::GetSystemCompensateKs() const
{
    return m_stSystemCompensateKs;
}

///
/// @brief
///    设置系统零序补偿系数Ks
///
/// @param[in]  val    系统零序补偿系数
///
void CFaultCalc::SetSystemCompensateKs(CCompensate val)
{
    m_stSystemCompensateKs = val;
}

///
/// @brief
///    获得故障电压（V）
///
/// @return
///     double    故障电压
///
double CFaultCalc::GetFaultVoltage() const
{
    return m_dFaultVoltage;
}

///
/// @brief
///    设置故障电压（V）
///
/// @param[in]  val    故障电压
///
void CFaultCalc::SetFaultVoltage(double val)
{
    m_dFaultVoltage = val;
}

///
/// @brief
///    获得故障电流（A）
///
/// @return
///     double    故障电流
///
double CFaultCalc::GetFaultCurrent() const
{
    return m_dFaultCurrent;
}

///
/// @brief
///    设置故障电流（A）
///
/// @param[in]  val    故障电流
///
void CFaultCalc::SetFaultCurrent(double val)
{
    m_dFaultCurrent = val;
}
