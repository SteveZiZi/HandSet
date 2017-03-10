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

#ifndef __FAULT_CALC_H__
#define __FAULT_CALC_H__
#include "src/dspclient/sc_calc/ShortCircuitCalc.h"


///
/// @brief
///    �迹
///
class CImpedance
{
public:
    ///
    /// @brief
    ///    �迹���캯��
    ///
    CImpedance();

    ///
    /// @brief
    ///    ����迹������
    ///
    /// @return
    ///     double    �迹
    ///
    double GetImpedance() const;

    ///
    /// @brief
    ///    �����迹������
    ///
    /// @param[in]  val    �迹
    ///
    void SetImpedance(double val);

    ///
    /// @brief
    ///    ����迹�ǣ��㣩
    ///
    /// @return
    ///     double    �迹��
    ///
    double GetAngle() const;

    ///
    /// @brief
    ///    �����迹�ǣ��㣩
    ///
    /// @param[in]  val    �迹��
    ///
    void SetAngle(double val);

    ///
    /// @brief
    ///    ��õ��裨����
    ///
    /// @return
    ///     double    ����
    ///
    double GetResistance() const;

    ///
    /// @brief
    ///    ���õ��裨����
    ///
    /// @param[in]  val    ����
    ///
    void SetResistance(double val);

    ///
    /// @brief
    ///    ��õ翹������
    ///
    /// @return
    ///     double    �翹
    ///
    double GetReactance() const;

    ///
    /// @brief
    ///    ���õ翹������
    ///
    /// @param[in]  val    �翹
    ///
    void SetReactance(double val);

private:
    double                              m_dImpedance;                   ///< �迹������
    double                              m_dAngle;                       ///< �迹�ǣ��㣩
    double                              m_dResistance;                  ///< ���裨����
    double                              m_dReactance;                   ///< �翹������
};


///
/// @brief
///    ���򲹳�ϵ��
///
class CCompensate
{
public:
    ///
    /// @brief
    ///    ���򲹳�ϵ�����캯��
    ///
    CCompensate();

    ///
    /// @brief
    ///     ����迹
    ///
    /// @return
    ///     CImpedance*     ָ���迹�����ָ��
    ///
    CImpedance* GetImpedance() const;

    ///
    /// @brief
    ///     �����迹
    ///
    /// @param[in]  val     ָ���迹�����ָ��
    ///
    void SetImpedance(CImpedance* val);

    ///
    /// @brief
    ///    ���ģֵ
    ///
    /// @return
    ///     double    ģֵ
    ///
    double GetModelValue() const;

    ///
    /// @brief
    ///    ����ģֵ
    ///
    /// @param[in]  val    ģֵ
    ///
    void SetModelValue(double val);

    ///
    /// @brief
    ///    ��÷���
    ///
    /// @return
    ///     double    ����
    ///
    double GetAngle() const;

    ///
    /// @brief
    ///    ���÷���
    ///
    /// @param[in]  val    ����
    ///
    void SetAngle(double val);

    ///
    /// @brief
    ///    ���ʵ��
    ///
    /// @return
    ///     double    ʵ��
    ///
    double GetReal() const;

    ///
    /// @brief
    ///    ����ʵ��
    ///
    /// @param[in]  val    ʵ��
    ///
    void SetReal(double val);

    ///
    /// @brief
    ///    ����鲿
    ///
    /// @return
    ///     double    �鲿
    ///
    double GetVirtual() const;

    ///
    /// @brief
    ///    �����鲿
    ///
    /// @param[in]  val    �鲿
    ///
    void SetVirtual(double val);

private:
    CImpedance*                         m_pImpedance;                   ///< �迹
    double                              m_dModelValue;                  ///< ģֵ
    double                              m_dAngle;                       ///< ����
    double                              m_dReal;                        ///< ʵ��
    double                              m_dVirtual;                     ///< �鲿
};


///
/// @brief
///    ���ϼ���
///
class CFaultCalc
{
public:
    ///
    /// @brief
    ///    ���ϼ��㹹�캯��
    ///
    CFaultCalc(void);

    ///
    /// @brief
    ///    ��ù�������
    ///
    /// @return
    ///     SCCalc::SCType    ��������
    ///
    SCCalc::SCType GetFaultType() const;

    ///
    /// @brief
    ///    ���ù�������
    ///
    /// @param[in]  val    ��������
    ///
    void SetFaultType(SCCalc::SCType val);

    ///
    /// @brief
    ///    ��ü���ģ��
    ///
    /// @return
    ///     SCCalc::CalcModelType    ����ģ��
    ///
    SCCalc::CalcModelType GetCalcModelType() const;

    ///
    /// @brief
    ///    ���ü���ģ��
    ///
    /// @param[in]  val    ����ģ��
    ///
    void SetCalcModelType(SCCalc::CalcModelType val);

    ///
    /// @brief
    ///    ��ù��Ϸ���
    ///
    /// @return
    ///     DirectionType    ���Ϸ���
    ///
    SCCalc::DirectionType GetDirection() const;

    ///
    /// @brief
    ///    ���ù��Ϸ���
    ///
    /// @param[in]  val    ���Ϸ���
    ///
    void SetDirection(SCCalc::DirectionType val);

    ///
    /// @brief
    ///    ��ö��ѹ��V��
    ///
    /// @return
    ///     double    ���ѹ
    ///
    double GetRatedVoltage() const;

    ///
    /// @brief
    ///    ���ö��ѹ��V��
    ///
    /// @param[in]  val    ���ѹ
    ///
    void SetRatedVoltage(double val);

    ///
    /// @brief
    ///    ��ø��ɵ�����A��
    ///
    /// @return
    ///     double    ���ɵ���
    ///
    double GetLoadCurrent() const;

    ///
    /// @brief
    ///    ���ø��ɵ�����A��
    ///
    /// @param[in]  val    ���ɵ���
    ///
    void SetLoadCurrent(double val);

    ///
    /// @brief
    ///    ��ø��ɹ��ǣ��㣩
    ///
    /// @return
    ///     double    ���ɹ���
    ///
    double GetLoadAngle() const;

    ///
    /// @brief
    ///    ���ø��ɹ��ǣ��㣩
    ///
    /// @param[in]  val    ���ɹ���
    ///
    void SetLoadAngle(double val);

    ///
    /// @brief
    ///    �����·�迹Zl
    ///
    /// @return
    ///     CImpedance    ��·�迹Zl
    ///
    CImpedance GetLineImpedanceZl() const;

    ///
    /// @brief
    ///    ������·�迹Zl
    ///
    /// @param[in]  val    ��·�迹Zl
    ///
    void SetLineImpedanceZl(CImpedance val);

    ///
    /// @brief
    ///    �����·���򲹳�ϵ��Kl
    ///
    /// @return
    ///     CCompensate    ��·���򲹳�ϵ��
    ///
    CCompensate GetLineCompensateKl() const;

    ///
    /// @brief
    ///    ������·���򲹳�ϵ��Kl
    ///
    /// @param[in]  val    ��·���򲹳�ϵ��
    ///
    void SetLineCompensateKl(CCompensate val);

    ///
    /// @brief
    ///    ���ϵͳ�迹Zs
    ///
    /// @return
    ///     CImpedance    ϵͳ�迹
    ///
    CImpedance GetSystemImpedanceZs() const;

    ///
    /// @brief
    ///    ����ϵͳ�迹Zs
    ///
    /// @param[in]  val    ϵͳ�迹
    ///
    void SetSystemImpedanceZs(CImpedance val);

    ///
    /// @brief
    ///    ���ϵͳ���򲹳�ϵ��Ks
    ///
    /// @return
    ///     CCompensate    ϵͳ���򲹳�ϵ��
    ///
    CCompensate GetSystemCompensateKs() const;

    ///
    /// @brief
    ///    ����ϵͳ���򲹳�ϵ��Ks
    ///
    /// @param[in]  val    ϵͳ���򲹳�ϵ��
    ///
    void SetSystemCompensateKs(CCompensate val);

    ///
    /// @brief
    ///    ��ù��ϵ�ѹ��V��
    ///
    /// @return
    ///     double    ���ϵ�ѹ
    ///
    double GetFaultVoltage() const;

    ///
    /// @brief
    ///    ���ù��ϵ�ѹ��V��
    ///
    /// @param[in]  val    ���ϵ�ѹ
    ///
    void SetFaultVoltage(double val);

    ///
    /// @brief
    ///    ��ù��ϵ�����A��
    ///
    /// @return
    ///     double    ���ϵ���
    ///
    double GetFaultCurrent() const;

    ///
    /// @brief
    ///    ���ù��ϵ�����A��
    ///
    /// @param[in]  val    ���ϵ���
    ///
    void SetFaultCurrent(double val);

private:
    SCCalc::SCType                      m_eFaultType;                   ///< ��������
    SCCalc::CalcModelType               m_eCalcModelType;               ///< ����ģ��
    SCCalc::DirectionType               m_eDirection;                   ///< ���Ϸ���
    double                              m_dRatedVoltage;                ///< ���ѹ��V��
    double                              m_dLoadCurrent;                 ///< ���ɵ�����A��
    double                              m_dLoadAngle;                   ///< ���ɹ��ǣ��㣩
    CImpedance                          m_stLineImpedanceZl;            ///< ��·�迹Zl
    CCompensate                         m_stLineCompensateKl;           ///< ��·���򲹳�ϵ��Kl
    CImpedance                          m_stSystemImpedanceZs;          ///< ϵͳ�迹Zs
    CCompensate                         m_stSystemCompensateKs;         ///< ϵͳ���򲹳�ϵ��Ks
    double                              m_dFaultVoltage;                ///< ���ϵ�ѹ��V��
    double                              m_dFaultCurrent;                ///< ���ϵ�����A��
};

#endif
