/// @file
///
/// @brief
///     ���幦��������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.23
///
/// �޸���ʷ��
///

#pragma once

#include "DataEffectiveVal.h"


///
/// @brief
///    �����࣬����ÿ��ͨ������Чֵ����λ����Ϣ�����Դ���Чֵ��̳�ʹ��
///
class CDataPower : public CDataEffectiveVal
{
public:
    CDataPower(void);
    ~CDataPower(void);

    ///
    /// @brief
    ///    ��ʼ����ȡ���/������Ϣ�������������ͨ������е���
    ///
    void init();

    ///
    /// @brief
    ///    ���������ѹͨ��
    ///
    /// @param[in]  nVolPhaseA A���ѹͨ����
    /// @param[in]  nVolPhaseB B���ѹͨ����
    /// @param[in]  nVolPhaseC C���ѹͨ����
    ///
    bool SetVoltageChannles(uint32_t nVolPhaseA, uint32_t nVolPhaseB, uint32_t nVolPhaseC);

    ///
    /// @brief
    ///    ��ȡ�����ѹͨ��
    ///
    /// @param[out]  nVolPhaseA ����A���ѹͨ����
    /// @param[out]  nVolPhaseB ����B���ѹͨ����
    /// @param[out]  nVolPhaseC ����C���ѹͨ����
    ///
    bool GetVoltageChannles(uint32_t& nVolPhaseA, uint32_t& nVolPhaseB, uint32_t& nVolPhaseC);

    ///
    /// @brief
    ///    �����������ͨ��
    ///
    /// @param[in]  nCurPhaseA A�����ͨ����
    /// @param[in]  nCurPhaseB B�����ͨ����
    /// @param[in]  nCurPhaseC C�����ͨ����
    ///
    bool SetCurrentChannles(uint32_t nCurPhaseA, uint32_t nCurPhaseB, uint32_t nCurPhaseC);

    ///
    /// @brief
    ///    ��ȡ�������ͨ��
    ///
    /// @param[out]  nCurPhaseA ����A�����ͨ����
    /// @param[out]  nCurPhaseB ����B�����ͨ����
    /// @param[out]  nCurPhaseC ����C�����ͨ����
    ///
    bool GetCurrentChannles(uint32_t& nCurPhaseA, uint32_t& nCurPhaseB, uint32_t& nCurPhaseC);

    ///
    /// @brief
    ///    ��ȡ�й�����
    ///
    /// @param[out]  fPhaseAData        ����A���й�����
    /// @param[out]  fPhaseBData        ����B���й�����
    /// @param[out]  fPhaseCData        ����C���й�����
    /// @param[out]  fTotalData         �������й�����
    ///
    /// @return
    ///
    void GetActivePower( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData);

    ///
    /// @brief
    ///    ��ȡ�޹�����
    ///
    /// @param[out]  fPhaseAData        ����A���޹�����
    /// @param[out]  fPhaseBData        ����B���޹�����
    /// @param[out]  fPhaseCData        ����C���޹�����
    /// @param[out]  fTotalData         �������޹�����
    ///
    /// @return
    ///
    void GetReactivePower( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData);

    ///
    /// @brief
    ///    ��ȡ���ڹ���
    ///
    /// @param[out]  fPhaseAData        ����A�����ڹ���
    /// @param[out]  fPhaseBData        ����B�����ڹ���
    /// @param[out]  fPhaseCData        ����C�����ڹ���
    /// @param[out]  fTotalData         ���������ڹ���
    ///
    /// @return
    ///
    void GetApparentPower( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData);

    ///
    /// @brief
    ///    ��ȡ��������
    ///
    /// @param[out]  fPhaseAData        ����A�๦������
    /// @param[out]  fPhaseBData        ����B�๦������
    /// @param[out]  fPhaseCData        ����C�๦������
    /// @param[out]  fTotalData         �����ܹ�������
    ///
    /// @return
    ///
    void GetPowerFactor( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData);

    ///
    /// @brief
    ///    ��ȡ��ѹ��Чֵ��Ϣ
    ///
    /// @param[out]  fPhaseAData        ����A���ѹ��Чֵ
    /// @param[out]  fPhaseBData        ����B���ѹ��Чֵ
    /// @param[out]  fPhaseCData        ����C���ѹ��Чֵ
    ///
    /// @return
    ///     ��ȡ�ɹ�����true
    ///
    bool GetVotageEffectValue( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData);

    ///
    /// @brief
    ///    ��ȡ��ѹ��λ��Ϣ
    ///
    /// @param[out]  fPhaseAData        ����A���ѹ��λ
    /// @param[out]  fPhaseBData        ����B���ѹ��λ
    /// @param[out]  fPhaseCData        ����C���ѹ��λ
    ///
    /// @return
    ///     ��ȡ�ɹ�����true
    ///
    bool GetVoltageAngle( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData);

    ///
    /// @brief
    ///    ��ȡ������Чֵ��Ϣ
    ///
    /// @param[out]  fPhaseAData        ����A�������Чֵ
    /// @param[out]  fPhaseBData        ����B�������Чֵ
    /// @param[out]  fPhaseCData        ����C�������Чֵ
    ///
    /// @return
    ///     ��ȡ�ɹ�����true
    ///
    bool GetCurrentEffectValue( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData);

    ///
    /// @brief
    ///    ��ȡ������λ��Ϣ
    ///
    /// @param[out]  fPhaseAData        ����A�������λ
    /// @param[out]  fPhaseBData        ����B�������λ
    /// @param[out]  fPhaseCData        ����C�������λ
    ///
    /// @return
    ///     ��ȡ�ɹ�����true
    ///
    bool GetCurrentAngle( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData);


    ///
    /// @brief
    ///    ��չ�����Ϣ
    ///
    void ClearPowerData();

private:
    friend class CFrameRecv;

protected:
    float    m_fActivePower[3];                                       ///< �й����ʣ��ֱ��ӦABC����
    float    m_fReactivePower[3];                                     ///< �޹����ʣ��ֱ��ӦABC����
    float    m_fApparentPower[3];                                     ///< ���ڹ��ʣ��ֱ��ӦABC����
    float    m_fPowerFactor[3];                                       ///< �����������ֱ��ӦABC����
    float    m_fTotalActivePower;                                     ///< ���й�����
    float    m_fTotalReactivePower;                                   ///< ���޹�����
    float    m_fTotalApparentPower;                                   ///< �����ڹ���
    float    m_fTotalPowerFactor;                                     ///< �ܹ�������
};
