/// @file
///
/// @brief
///     SMV��������ͨ��
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.06.07
///
/// �޸���ʷ��
///

#ifndef __SMV_SEND_VIRTUAL_CHANNEL_H__
#define __SMV_SEND_VIRTUAL_CHANNEL_H__
#include <string>
#include "Channel.h"
#include "Harmonic.h"
#include "ParamConfigBase.h"

///
/// @brief
///    ��������
///
enum eFunctionType
{
    FUNCTION_TYPE_UNKNOWN = 0,
    FUNCTION_TYPE_MANUAL_TEST,                                          ///< �ֶ�����
    FUNCTION_TYPE_STATE_SEQUENCE                                        ///< ״̬����
};

///
/// @brief
///     �ȽϷ�ʽ
///
enum enumCompareType
{
    TYPE_FIND_ALL_DIFFERENT = 0,                                        ///< �ҳ����еĲ�ͬ
    TYPE_FIND_ONE_DIFFERENT,                                            ///< �ҳ�һ����ͬ
};

///
/// @brief
///    ����ͨ��
///
class CSMVSendVirtualChannel
{
    friend class CParamconfigManualTest;
    friend class CStatus;
    friend class CStateTestResult;

public:
    ///
    /// @brief
    ///    ���캯��
    ///
    /// @param[in]  eType    ��������
    /// @param[in]  pParamConfigBase    ���ù���������
    ///
    CSMVSendVirtualChannel(eFunctionType eType, CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    ��������
    ///
    virtual ~CSMVSendVirtualChannel(void);

    ///
    /// @brief
    ///    �������ͨ���Ľ�ֱ������
    ///
    /// @return
    ///     eCurrentType    ��ֱ������
    ///
    eCurrentType GetACDCType();

    ///
    /// @brief
    ///    �������ͨ����
    ///
    /// @return
    ///     std::wstring    ����ͨ����
    ///
    std::wstring GetName();

    ///
    /// @brief
    ///    ��������ͨ����
    ///
    /// @param[in]  strName    ����ͨ����
    ///
    /// @return
    ///     bool    ���óɹ�����true
    ///
    bool SetName(std::wstring strName);

    ///
    /// @brief
    ///    ��÷�ֵ����ѹΪV������ΪA��ʱ��Ϊus��
    ///
    /// @return
    ///     double    ��ֵ
    ///
    double GetAmplitude();

    ///
    /// @brief
    ///    ���÷�ֵ����ѹΪV������ΪA��ʱ��Ϊus��
    ///
    /// @param[in]  dAmplitude    ��ֵ
    ///
    /// @return
    ///     bool    ���óɹ�����true
    ///
    bool SetAmplitude(double dAmplitude);

    ///
    /// @brief
    ///     ����ͨ���ֵ
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool SetRatedAmplitude();

    ///
    /// @brief
    ///    ������
    ///
    /// @return
    ///     double    ���
    ///
    double GetPhaseAngle();

    ///
    /// @brief
    ///    �������
    ///
    /// @param[in]  dPhaseAngle    ���
    ///
    /// @return
    ///     bool    ���óɹ�����true
    ///
    bool SetPhaseAngle(double dPhaseAngle);

    ///
    /// @brief
    ///    ���Ƶ��
    ///
    /// @return
    ///     double    Ƶ��
    ///
    double GetFrequency();

    ///
    /// @brief
    ///    ����Ƶ��
    ///
    /// @param[in]  dFrequency    Ƶ��
    ///
    /// @return
    ///     bool    ���óɹ�����true
    ///
    bool SetFrequency(double dFrequency);

    ///
    /// @brief
    ///    ��÷�ֵ��������ѹΪV������ΪA��ʱ��Ϊus��
    ///
    /// @return
    ///     double    ��ֵ����
    ///
    double GetAmplitudeStep();

    ///
    /// @brief
    ///    �����ǲ���
    ///
    /// @return
    ///     double    ��ǲ���
    ///
    double GetPhaseAngleStep();

    ///
    /// @brief
    ///    ���Ƶ�ʲ���
    ///
    /// @return
    ///     double    Ƶ�ʲ���
    ///
    double GetFrequencyStep();

    ///
    /// @brief
    ///    ���÷�ֵ��������ѹΪV������ΪA��ʱ��Ϊus��
    ///
    /// @param[in]  dStep    ��ֵ����
    ///
    /// @return
    ///     bool    ���óɹ�����true
    ///
    bool SetAmplitudeStep(double dStep);

    ///
    /// @brief
    ///    ������ǲ���
    ///
    /// @param[in]  dStep    ��ֵ����
    ///
    /// @return
    ///     bool    ���óɹ�����true
    ///
    bool SetPhaseAngleStep(double dStep);

    ///
    /// @brief
    ///    ����Ƶ�ʲ���
    ///
    /// @param[in]  dStep    Ƶ�ʲ���
    ///
    /// @return
    ///     bool    ���óɹ�����true
    ///
    bool SetFrequencyStep(double dStep);

    ///
    /// @brief
    ///    ���г�����ö����ָ��
    ///
    /// @return
    ///     CHarmonic*    г�����ö����ָ��
    ///
    CHarmonic* GetHarmonic();

    ///
    /// @brief
    ///    ���ӳ�䵽������ͨ������ʵͨ��������
    ///
    /// @return
    ///     int    ӳ�䵽������ͨ������ʵͨ��������
    ///
    int GetChannelCount();

    ///
    /// @brief
    ///    ���SVMͨ��ӳ����б���ѡ��ͨ������������0��ʼ������
    ///
    /// @return
    ///     int    ����ѡ��ͨ��������
    ///
    int GetSelectedIndex();

    ///
    /// @brief
    ///    ����SVMͨ��ӳ����б���ѡ��ͨ������������0��ʼ������
    ///
    /// @param[in]  nIndex    ����ѡ��ͨ��������
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool SetSelectedIndex( int nIndex );

    ///
    /// @brief
    ///    ö�ٵ�һ��Channel
    ///
    /// @return
    ///     CChannel*    �ɹ�ʱ����ͨ��ָ�룬ʧ��ʱ����NULL
    ///
    CChannel* FirstChannel();

    ///
    /// @brief
    ///    ö����һ��Channel
    ///
    /// @param[in]  pCurChannel ��ǰChannel��ָ��
    ///
    /// @return
    ///     CChannel*    �ɹ�ʱ����Channelָ�룬ʧ��ʱ����NULL
    ///
    CChannel* NextChannel(CChannel* pCurChannel);

    ///
    /// @brief
    ///    ���ָ�������ŵ�ӳ�䵽������ͨ������ʵͨ��
    ///
    /// @param[in]  nIndex    ͨ��������
    ///
    /// @return
    ///     CChannel*    �ɹ�ʱ����Channelָ�룬ʧ��ʱ����NULL
    ///
    CChannel* GetChannel(int nIndex);

    ///
    /// @brief
    ///    ��ֵ�Ƿ����˱仯
    ///
    /// @param[in]  bIsChanged    �Ƿ����˱仯
    ///
    bool IsAmplitudeChanged();

    ///
    /// @brief
    ///    ����Ƿ����˱仯
    ///
    /// @param[in]  bIsChanged    �Ƿ����˱仯
    ///
    bool IsPhaseAngleChanged();

    ///
    /// @brief
    ///    Ƶ���Ƿ����˱仯
    ///
    /// @param[in]  bIsChanged    �Ƿ����˱仯
    ///
    bool IsFrequencyChanged();

    ///
    /// @brief
    ///    �ֶ��������������ͨ���Ƿ񱻹�ѡ
    ///
    /// @return
    ///     bool    �Ƿ�ѡ
    ///
    bool IsSelected() const
    {
        return m_bSelected;
    }

    ///
    /// @brief
    ///    �����ֶ��������������ͨ���Ƿ񱻹�ѡ
    ///
    /// @param[in]  val    �Ƿ�ѡ
    ///
    void SetSelected(bool val)
    {
        m_bSelected = val;
    }

    ///
    /// @brief
    ///     �Ƚ�����ͨ���ķ�ֵ����ǡ�Ƶ�ʡ�г��
    ///
    /// @param[in]  eCompareType    �ȽϷ�ʽ
    /// @param[in]  pOldSMVSendVirtualChannel   �뵱ǰͨ�����бȽϵ�����ͨ��
    ///
    /// @return
    ///     bool    ��ͬ����true
    ///
    bool Compare(enumCompareType eCompareType, CSMVSendVirtualChannel* pOldSMVSendVirtualChannel);

protected:
    ///
    /// @brief
    ///    ��ȿ���һ������ͨ������
    ///
    /// @param[in]
    ///     pOldVirtualChannel    -   �Ѵ��ڵ�����ͨ������
    ///
    /// @return
    ///     bool    �����ɹ�����TRUE
    ///
    bool __Copy(CSMVSendVirtualChannel* pOldVirtualChannel);

    ///
    /// @brief
    ///    ����SMV��������ͨ��
    ///
    /// @param[in]  xml    ָ��XML���������
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool __LoadConfig(CMarkup& xml);

    ///
    /// @brief
    ///    ����SMV��������ͨ��
    ///
    /// @param[in]  xml    ָ��XML���������
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool __SaveConfig(CMarkup& xml);

    ///
    /// @brief
    ///    ����ͨ���ķ�ֵ�Ƿ����˱仯
    ///
    /// @param[in]  bIsChanged    �Ƿ����˱仯
    ///
    void __SetAmplitudeChanged(bool bIsChanged);

    ///
    /// @brief
    ///    ����ͨ��������Ƿ����˱仯
    ///
    /// @param[in]  bIsChanged    �Ƿ����˱仯
    ///
    void __SetPhaseAngleChanged(bool bIsChanged);

    ///
    /// @brief
    ///    ����ͨ����Ƶ���Ƿ����˱仯
    ///
    /// @param[in]  bIsChanged    �Ƿ����˱仯
    ///
    void __SetFrequencyChanged(bool bIsChanged);

private:
    std::wstring                        m_strName;                      ///< ����ͨ����
    eFunctionType                       m_eFunctionType;                ///< �������ͣ��ֶ����顢״̬���У�
    CParamConfigBase*                   m_pParamConfigBase;             ///< �������ù�����
    bool                                m_bSelected;                    ///< �ֶ��������������ͨ���Ƿ񱻹�ѡ

    eCurrentType                        m_eACDCType;                    ///< ��ֱ������
    double                              m_dAmplitude;                   ///< ��ֵ��һ��ֵ��
    double                              m_dPhaseAngle;                  ///< ���
    double                              m_dFrequency;                   ///< Ƶ��
    double                              m_dAmplitudeStep;               ///< ��ֵ�Ĳ���
    double                              m_dPhaseAngleStep;              ///< ��ǵĲ���
    double                              m_dFrequencyStep;               ///< Ƶ�ʵĲ���

    bool                                m_bIsAmplitudeChanged;          ///< ��ֵ�Ƿ����˱仯������Ҫ���浽�ļ��У�
    bool                                m_bIsPhaseAngleChanged;         ///< ����Ƿ����˱仯������Ҫ���浽�ļ��У�
    bool                                m_bIsFrequencyChanged;          ///< Ƶ���Ƿ����˱仯������Ҫ���浽�ļ��У�

    CHarmonic                           m_harmonic;                     ///< г�����ã�״̬���У�

    int                                 m_iSelectedIndex;               ///< SVMͨ��ӳ����б���ѡ��ͨ������������0��ʼ������
    std::vector<CChannel *>             m_vecChannelGroup;              ///< ͨ�����������ڴ��ӳ�䵽������ͨ������ʵͨ����ָ��
};
#endif
