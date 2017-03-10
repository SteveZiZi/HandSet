/// @file
///
/// @brief
///     ���ϼ���
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.9.10
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "FaultCalc.h"


///
/// @brief
///    �迹���캯��
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
///    ����迹������
///
/// @return
///     double    �迹
///
double CImpedance::GetImpedance() const
{
    return m_dImpedance;
}

///
/// @brief
///    �����迹������
///
/// @param[in]  val    �迹
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
///    ����迹�ǣ��㣩
///
/// @return
///     double    �迹��
///
double CImpedance::GetAngle() const
{
    return m_dAngle;
}

///
/// @brief
///    �����迹�ǣ��㣩
///
/// @param[in]  val    �迹��
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
///    ��õ��裨����
///
/// @return
///     double    ����
///
double CImpedance::GetResistance() const
{
    return m_dResistance;
}

///
/// @brief
///    ���õ��裨����
///
/// @param[in]  val    ����
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
///    ��õ翹������
///
/// @return
///     double    �翹
///
double CImpedance::GetReactance() const
{
    return m_dReactance;
}

///
/// @brief
///    ���õ翹������
///
/// @param[in]  val    �翹
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
///    ���򲹳�ϵ�����캯��
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
///     ����迹
///
/// @return
///     CImpedance*     ָ���迹�����ָ��
///
CImpedance* CCompensate::GetImpedance() const
{
    return m_pImpedance;
}

///
/// @brief
///     �����迹
///
/// @param[in]  val     ָ���迹�����ָ��
///
void CCompensate::SetImpedance( CImpedance* val )
{
    m_pImpedance = val;
}

///
/// @brief
///    ���ģֵ
///
/// @return
///     double    ģֵ
///
double CCompensate::GetModelValue() const
{
    return m_dModelValue;
}

///
/// @brief
///    ����ģֵ
///
/// @param[in]  val    ģֵ
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
///    ��÷���
///
/// @return
///     double    ����
///
double CCompensate::GetAngle() const
{
    return m_dAngle;
}

///
/// @brief
///    ���÷���
///
/// @param[in]  val    ����
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
///    ���ʵ��
///
/// @return
///     double    ʵ��
///
double CCompensate::GetReal() const
{
    return m_dReal;
}

///
/// @brief
///    ����ʵ��
///
/// @param[in]  val    ʵ��
///
void CCompensate::SetReal(double val)
{
    SCCalc::Vector  stVectorZl;
    SCCalc::Vector  stVectorKl;


    if ((val >= 0) && (NULL != m_pImpedance))
    {
        m_dReal         = val;

        // ���㲹��ϵ��
        stVectorZl.md   = m_pImpedance->GetImpedance();
        stVectorZl.dg   = m_pImpedance->GetAngle();
        SCCalc::KLCalc(SCCalc::KLCT_KRKX, m_dReal, m_dVirtual, stVectorZl, stVectorKl);

        // ���������
        m_dModelValue   = stVectorKl.md;
        m_dAngle        = stVectorKl.dg;
    }
}

///
/// @brief
///    ����鲿
///
/// @return
///     double    �鲿
///
double CCompensate::GetVirtual() const
{
    return m_dVirtual;
}

///
/// @brief
///    �����鲿
///
/// @param[in]  val    �鲿
///
void CCompensate::SetVirtual(double val)
{
    SCCalc::Vector  stVectorZl;
    SCCalc::Vector  stVectorKl;


    if ((val >= 0) && (NULL != m_pImpedance))
    {
        m_dVirtual      = val;

        // ���㲹��ϵ��
        stVectorZl.md   = m_pImpedance->GetImpedance();
        stVectorZl.dg   = m_pImpedance->GetAngle();
        SCCalc::KLCalc(SCCalc::KLCT_KRKX, m_dReal, m_dVirtual, stVectorZl, stVectorKl);

        // ���������
        m_dModelValue   = stVectorKl.md;
        m_dAngle        = stVectorKl.dg;
    }
}

///
/// @brief
///    ���ϼ��㹹�캯��
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
    // �������򲹳�ϵ�����迹
    m_stLineCompensateKl.SetImpedance(&m_stLineImpedanceZl);
    m_stSystemCompensateKs.SetImpedance(&m_stSystemImpedanceZs);
}

///
/// @brief
///    ��ù�������
///
/// @return
///     SCCalc::SCType    ��������
///
SCCalc::SCType CFaultCalc::GetFaultType() const
{
    return m_eFaultType;
}

///
/// @brief
///    ���ù�������
///
/// @param[in]  val    ��������
///
void CFaultCalc::SetFaultType(SCCalc::SCType val)
{
    m_eFaultType = val;
}

///
/// @brief
///    ��ü���ģ��
///
/// @return
///     SCCalc::CalcModelType    ����ģ��
///
SCCalc::CalcModelType CFaultCalc::GetCalcModelType() const
{
    return m_eCalcModelType;
}

///
/// @brief
///    ���ü���ģ��
///
/// @param[in]  val    ����ģ��
///
void CFaultCalc::SetCalcModelType(SCCalc::CalcModelType val)
{
    m_eCalcModelType = val;
}

///
/// @brief
///    ��ù��Ϸ���
///
/// @return
///     DirectionType    ���Ϸ���
///
SCCalc::DirectionType CFaultCalc::GetDirection() const
{
    return m_eDirection;
}

///
/// @brief
///    ���ù��Ϸ���
///
/// @param[in]  val    ���Ϸ���
///
void CFaultCalc::SetDirection(SCCalc::DirectionType val)
{
    m_eDirection = val;
}

///
/// @brief
///    ��ö��ѹ��V��
///
/// @return
///     double    ���ѹ
///
double CFaultCalc::GetRatedVoltage() const
{
    return m_dRatedVoltage;
}

///
/// @brief
///    ���ö��ѹ��V��
///
/// @param[in]  val    ���ѹ
///
void CFaultCalc::SetRatedVoltage(double val)
{
    m_dRatedVoltage = val;
}

///
/// @brief
///    ��ø��ɵ�����A��
///
/// @return
///     double    ���ɵ���
///
double CFaultCalc::GetLoadCurrent() const
{
    return m_dLoadCurrent;
}

///
/// @brief
///    ���ø��ɵ�����A��
///
/// @param[in]  val    ���ɵ���
///
void CFaultCalc::SetLoadCurrent(double val)
{
    m_dLoadCurrent = val;
}

///
/// @brief
///    ��ø��ɹ��ǣ��㣩
///
/// @return
///     double    ���ɹ���
///
double CFaultCalc::GetLoadAngle() const
{
    return m_dLoadAngle;
}

///
/// @brief
///    ���ø��ɹ��ǣ��㣩
///
/// @param[in]  val    ���ɹ���
///
void CFaultCalc::SetLoadAngle(double val)
{
    m_dLoadAngle = val;
}

///
/// @brief
///    �����·�迹Zl
///
/// @return
///     CImpedance    ��·�迹Zl
///
CImpedance CFaultCalc::GetLineImpedanceZl() const
{
    return m_stLineImpedanceZl;
}

///
/// @brief
///    ������·�迹Zl
///
/// @param[in]  val    ��·�迹Zl
///
void CFaultCalc::SetLineImpedanceZl(CImpedance val)
{
    m_stLineImpedanceZl = val;
}

///
/// @brief
///    �����·���򲹳�ϵ��Kl
///
/// @return
///     CCompensate    ��·���򲹳�ϵ��
///
CCompensate CFaultCalc::GetLineCompensateKl() const
{
    return m_stLineCompensateKl;
}

///
/// @brief
///    ������·���򲹳�ϵ��Kl
///
/// @param[in]  val    ��·���򲹳�ϵ��
///
void CFaultCalc::SetLineCompensateKl(CCompensate val)
{
    m_stLineCompensateKl = val;
}

///
/// @brief
///    ���ϵͳ�迹Zs
///
/// @return
///     CImpedance    ϵͳ�迹
///
CImpedance CFaultCalc::GetSystemImpedanceZs() const
{
    return m_stSystemImpedanceZs;
}

///
/// @brief
///    ����ϵͳ�迹Zs
///
/// @param[in]  val    ϵͳ�迹
///
void CFaultCalc::SetSystemImpedanceZs(CImpedance val)
{
    m_stSystemImpedanceZs = val;
}

///
/// @brief
///    ���ϵͳ���򲹳�ϵ��Ks
///
/// @return
///     CCompensate    ϵͳ���򲹳�ϵ��
///
CCompensate CFaultCalc::GetSystemCompensateKs() const
{
    return m_stSystemCompensateKs;
}

///
/// @brief
///    ����ϵͳ���򲹳�ϵ��Ks
///
/// @param[in]  val    ϵͳ���򲹳�ϵ��
///
void CFaultCalc::SetSystemCompensateKs(CCompensate val)
{
    m_stSystemCompensateKs = val;
}

///
/// @brief
///    ��ù��ϵ�ѹ��V��
///
/// @return
///     double    ���ϵ�ѹ
///
double CFaultCalc::GetFaultVoltage() const
{
    return m_dFaultVoltage;
}

///
/// @brief
///    ���ù��ϵ�ѹ��V��
///
/// @param[in]  val    ���ϵ�ѹ
///
void CFaultCalc::SetFaultVoltage(double val)
{
    m_dFaultVoltage = val;
}

///
/// @brief
///    ��ù��ϵ�����A��
///
/// @return
///     double    ���ϵ���
///
double CFaultCalc::GetFaultCurrent() const
{
    return m_dFaultCurrent;
}

///
/// @brief
///    ���ù��ϵ�����A��
///
/// @param[in]  val    ���ϵ���
///
void CFaultCalc::SetFaultCurrent(double val)
{
    m_dFaultCurrent = val;
}
