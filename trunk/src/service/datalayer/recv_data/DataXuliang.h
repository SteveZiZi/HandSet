/// @file
///
/// @brief
///     ��������������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.24
///
/// �޸���ʷ��
///

#pragma once
#include "DataEffectiveVal.h"

class CDataXuliang : public CDataEffectiveVal
{
public:
    CDataXuliang(void);
    ~CDataXuliang(void);

public:
    ///
    /// @brief
    ///    ��ȡ�������򡢸�����ֵ
    ///
    /// @param[out] fZeroPhaseSeqContent   ��ȡ������ֵ
    /// @param[out] fPosPhaseSeqContent    ��ȡ������ֵ
    /// @param[out] fNegPhaseSeqContent    ��ȡ������ֵ
    ///
    void GetPhaseSeqContent(float& fZeroPhaseSeqContent, float& fPosPhaseSeqContent, float& fNegPhaseSeqContent);

    ///
    /// @brief
    ///    ��ȡ�������򡢸���Ƕ�
    ///
    /// @param[out] fZeroPhaseSeqAngle   ��ȡ����Ƕ�
    /// @param[out] fPosPhaseSeqAngle    ��ȡ����Ƕ�
    /// @param[out] fNegPhaseSeqAngle    ��ȡ����Ƕ�
    ///
    void GetPhaseSeqAngle(float& fZeroPhaseSeqAngle, float& fPosPhaseSeqAngle, float& fNegPhaseSeqAngle);

    ///
    /// @brief
    ///    ��ȡ�������ͨ��
    ///
    /// @param[out]  nPhaseA ����A��ͨ����
    /// @param[out]  nPhaseB ����B��ͨ����
    /// @param[out]  nPhaseC ����C��ͨ����
    ///
    /// return
    ///     bool    ��ȡ�ɹ�����true
    ///
    bool GetABCChannles(uint32_t& nCurPhaseA, uint32_t& nCurPhaseB, uint32_t& nCurPhaseC);

    ///
    /// @brief
    ///    ���������ѹ/����ͨ��
    ///
    /// @param[in]  nPhaseA A��ͨ����
    /// @param[in]  nPhaseB B��ͨ����
    /// @param[in]  nPhaseC C��ͨ����
    ///
    /// return
    ///     bool    ���óɹ�����true
    ///
    bool SetABCChannles(uint32_t nVolPhaseA, uint32_t nVolPhaseB, uint32_t nVolPhaseC);

    ///
    /// @brief
    ///    ���õ�ǰ������ѹ����ģʽ
    ///
    void SetXLMode(enumChannelType enChannelType);

    ///
    /// @brief
    ///    ��ȡ��ǰ�����ĵ�ѹ����ģʽ
    ///
    enumChannelType GetXLMode();

    ///
    /// @brief
    ///    ���������Ϣ
    ///
    void ClearXLData();

private:
    friend class CFrameRecv;
    ///
    /// @brief
    ///    �����������򡢸�����ֵ
    ///
    /// @param[in] enChannelType          ָ�����õ��ǵ�ѹ���ǵ���������ֵ
    /// @param[in] fZeroPhaseSeqContent   ����������ֵ
    /// @param[in] fPosPhaseSeqContent    ����������ֵ
    /// @param[in] fNegPhaseSeqContent    ���ø�����ֵ
    ///
    void SetPhaseSeqContent(enumChannelType enChannelType, float& fZeroPhaseSeqContent, float& fPosPhaseSeqContent, float& fNegPhaseSeqContent);

    ///
    /// @brief
    ///    �����������򡢸���Ƕ�
    ///
    /// @param[in] enChannelType        ָ�����õ��ǵ�ѹ���ǵ����������Ƕ�
    /// @param[in] fZeroPhaseSeqAngle   ��������Ƕ�
    /// @param[in] fPosPhaseSeqAngle    ��������Ƕ�
    /// @param[in] fNegPhaseSeqAngle    ���ø���Ƕ�
    ///
    void SetPhaseSeqAngle(enumChannelType enChannelType, float& fZeroPhaseSeqAngle, float& fPosPhaseSeqAngle, float& fNegPhaseSeqAngle);

private:
    float           m_fPhaseSeqContent[2][3];                         ///< �������򡢸������ֵ���ֱ��ʾ��ѹ������������
    float           m_fPhaseSeqAngle[2][3];                           ///< �������򡢸������λ�ǣ��ֱ��ʾ��ѹ������������
    enumChannelType m_enCurrentType;                                  ///< ��ǰ����������ģʽ���ǵ�ѹ���ǵ���
};
