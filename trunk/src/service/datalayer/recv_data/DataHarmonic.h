/// @file
///
/// @brief
///     ����г��������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.14
///
/// �޸���ʷ��
///

#pragma once

#include "src\service\datalayer\recv_data\DataRecvBase.h"

class CDataHarmonic : public CDataRecvBase
{
public:
    CDataHarmonic(void);
    ~CDataHarmonic(void);

public:
    ///
    /// @brief
    ///    ��ȡͨ��DC��Чֵ
    ///
    float GetChannelDCValue(uint32_t nIndex);

    ///
    /// @brief
    ///    ��ȡͨ��dc��Чֵ������
    ///
    float GetChannelDcTHD(uint32_t nIndex);

    ///
    /// @brief
    ///    ��ȡĳ��ͨ��ĳ��г�������ʣ�г���ܺ�����
    ///
    /// @param[in]  nChannelIndex ͨ����������0��ʼ
    ///
    /// @param[in]  nHarmType г������������Ϊһ��г����������ֵ1��ʾһ��г��,������ֵ0��ʾͨ����г�������� 
    ///
    /// @return
    ///     float ��Ӧͨ����Ӧ����г��������
    ///
    float GetChannelHarmTHD(uint32_t nChannelIndex, uint32_t nHarmType = 0);

    ///
    /// @brief
    ///    ��ȡһ��ͨ��ĳ��г����Чֵ����λ��Ϣ
    ///
    /// @param[in]  nChannelIndex ͨ����������0��ʼ
    ///
    /// @param[in]  nHarmType г������������Ϊһ��г����������ֵ1��ʾһ��г��
    ///
    /// @param[out] fHarmContent �����Ӧ��г������Чֵ
    ///
    /// @param[out] fHarmAngle   �����Ӧ��г������λ��
    ///
    /// @return
    ///     bool ��ȡ�ɹ�����true
    ///
    bool GetChannelHarmInfo(uint32_t nChannelIndex, uint32_t nHarmType, float& fHarmContent, float& fHarmAngle);

    ///
    /// @brief
    ///    ���г����Ϣ
    ///
    void ClearHarmonicData();

private:
    friend class CFrameRecv;
    float    m_fHarmContent[MAX_SMV_CHANNELS][MAX_HARM_COUNT];        ///< ��ͨ��г����Чֵ
    float    m_fHarmAngle[MAX_SMV_CHANNELS][MAX_HARM_COUNT];          ///< ��ͨ��г����λ
    float    m_fSumHarmTHD[MAX_SMV_CHANNELS];                         ///< ��ͨ����г��������
    float    m_fHarmTHD[MAX_SMV_CHANNELS][MAX_HARM_COUNT];            ///< ��ͨ��г��������
    float    m_fDCValue[MAX_SMV_CHANNELS];                            ///< DC��Чֵ
    float    m_fDCTHD[MAX_SMV_CHANNELS];                              ///< ��ͨ��ֱ��������
};
