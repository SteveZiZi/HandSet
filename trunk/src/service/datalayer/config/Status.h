/// @file
///
/// @brief
///     ״̬���е�״̬��
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.6.14
///
/// �޸���ʷ��
///

#ifndef __STATUS_SEQUENCE_H__
#define __STATUS_SEQUENCE_H__
#include <map>
#include "SMVSendVirtualChannel.h"

class CParamConfigBase;

///
/// @brief
///    ״̬�л���ʽ
///
enum eSwitchType
{
    SWITCH_TYPE_UNKNOWN = 0,
    SWITCH_TYPE_TIME,                                                   ///< ��ʱ�л�
    SWITCH_TYPE_DI,                                                     ///< �������л�
    SWITCH_TYPE_MANUAL                                                  ///< �ֶ��л�
};

///
/// @brief
///    �������л��߼����㷽ʽ
///
enum eLogicalOperator
{
    LOGICAL_UNKNOWN = 0,
    LOGICAL_AND,                                                        ///< �߼���
    LOGICAL_OR                                                          ///< �߼���
};

///
/// @brief
///    DI��DO״ֵ̬�ṹ��
///
struct DI_DO_INFO
{
    std::wstring                        m_strName;                      ///< ����ͨ����
    bool                                m_bChecked;                     ///< �Ƿ�ѡ��ͨ��
};

///
/// @brief
///    Ԥ�����DI״̬
///
static const DI_DO_INFO PREDEFINED_DI_INFO[] =
{
    {L"DI1",    false},
    {L"DI2",    false},
    {L"DI3",    false},
    {L"DI4",    false},
    {L"DI5",    false},
    {L"DI6",    false},
    {L"DI7",    false},
    {L"DI8",    false}
};

///
/// @brief
///    Ԥ�����DO״̬
///
static const DI_DO_INFO PREDEFINED_DO_INFO[] =
{
    {L"DO1",    false},
    {L"DO2",    false},
    {L"DO3",    false},
    {L"DO4",    false},
    {L"DO5",    false},
    {L"DO6",    false}
};

///
/// @brief
///    SMV��������ͨ��ֵ�ṹ��
///
struct stSMVSendVirtualChannelInfo
{
    std::wstring                        m_strName;                      ///< ����ͨ����
    double                              m_dAmplitude;                   ///< ��ֵ��һ��ֵ��
    double                              m_dPhaseAngle;                  ///< ���
    double                              m_dFrequency;                   ///< Ƶ��
};

///
/// @brief
///    Ԥ�����SMV��������ͨ��ֵ
///
static const stSMVSendVirtualChannelInfo PREDEFINED_SMV_SEND_VC_INFO[] =
{
    {L"Ia1",    3000.0f,    0.0f,   50.0f},
    {L"Ib1",    3000.0f,   -120.0f, 50.0f},
    {L"Ic1",    3000.0f,    120.0f, 50.0f},
    {L"Ix1",    3000.0f,    0.0f,   50.0f},
    {L"Ia2",    3000.0f,    0.0f,   50.0f},
    {L"Ib2",    3000.0f,   -120.0f, 50.0f},
    {L"Ic2",    3000.0f,    120.0f, 50.0f},
    {L"Ix2",    3000.0f,    0.0f,   50.0f},
    {L"Ua1",    63508.0f,   0.0f,   50.0f},
    {L"Ub1",    63508.0f,  -120.0f, 50.0f},
    {L"Uc1",    63508.0f,   120.0f, 50.0f},
    {L"Ux1",    63508.0f,   0.0f,   50.0f},
    {L"Ua2",    63508.0f,   0.0f,   50.0f},
    {L"Ub2",    63508.0f,  -120.0f, 50.0f},
    {L"Uc2",    63508.0f,   120.0f, 50.0f},
    {L"Ux2",    63508.0f,   0.0f,   50.0f},
    {L"t1",     750.0f,     0.0f,   50.0f},
    {L"t2",     750.0f,     0.0f,   50.0f},
    {L"t3",     750.0f,     0.0f,   50.0f},
    {L"t4",     750.0f,     0.0f,   50.0f}
};

///
/// @brief
///    ״̬��
///
class CStatus
{
    friend class CParamConfigStateSequence;
    friend class CStateTestResult;
    friend class CFrameSend;

protected:
    CStatus(void);

public:
    ///
    /// @brief
    ///    ��������
    ///
    virtual ~CStatus(void);

    ///
    /// @brief
    ///    ���캯��
    ///
    /// @param[in]  nID    ״̬����Ψһ��ʶID
    /// @param[in]  pParamConfigBase    ָ��������ù�������ָ��
    ///
    CStatus(int nID, CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    ���״̬�л���ʽ
    ///
    /// @return
    ///     eSwitchType    ״̬�л���ʽ
    ///
    eSwitchType GetSwitchType();

    ///
    /// @brief
    ///    ����״̬�л���ʽ
    ///
    /// @param[in]  eNewType    �µ�״̬�л���ʽ
    ///
    void SetSwitchType(eSwitchType eNewType);

    ///
    /// @brief
    ///    ���״̬�л���ʱ
    ///
    /// @return
    ///     eSwitchType    ״̬�л���ʱ
    ///
    int GetDelay();

    ///
    /// @brief
    ///    ����״̬�л���ʱ
    ///
    /// @param[in]  iDelay    �µ�״̬�л���ʱ
    ///
    void SetDelay(int iDelay);

    ///
    /// @brief
    ///    ��ÿ������л��߼����㷽ʽ
    ///
    /// @return
    ///     eLogicalOperator    �߼����㷽ʽ
    ///
    eLogicalOperator GetLogicalOperator();

    ///
    /// @brief
    ///    ���ÿ������л��߼����㷽ʽ
    ///
    /// @param[in]  eNewLogical    �߼����㷽ʽ
    ///
    void SetLogicalOperator(eLogicalOperator eNewLogical);

    ///
    /// @brief
    ///    ���ָ��DIͨ���Ĺ�ѡ״̬
    ///
    /// @param[in]  strDIName    DIͨ����
    ///
    /// @return
    ///     bool    DIͨ���Ĺ�ѡ״̬
    ///
    bool IsDIChecked(std::wstring strDIName);

    ///
    /// @brief
    ///    ����ָ��DIͨ���Ĺ�ѡ״̬
    ///
    /// @param[in]  strDIName    DIͨ����
    /// @param[in]  bChecked    �Ƿ񱻹�ѡ
    ///
    void SetDIChecked(std::wstring strDIName, bool bChecked);

    ///
    /// @brief
    ///    ���ָ��DOͨ����ֵ
    ///
    /// @param[in]  strDOName    DOͨ����
    ///
    /// @return
    ///     bool    DOͨ��ֵ
    ///
    bool GetDOValue(std::wstring strDOName);

    ///
    /// @brief
    ///    ����ָ��DOͨ����ֵ
    ///
    /// @param[in]  strDOName    DOͨ����
    /// @param[in]  bNewValue    ͨ������ֵ
    ///
    void SetDOValue(std::wstring strDOName, bool bNewValue);

    ///
    /// @brief
    ///    ���SMV��������ͨ��������
    ///
    /// @return
    ///     int    ����ͨ��������
    ///
    int GetSMVSendVirtualChannelCount();

    ///
    /// @brief
    ///    ö�ٵ�һ����ӳ���SMV��������ͨ��
    ///
    /// @return
    ///     CVirtualChannel*    ָ������ͨ�������ָ�룬ʧ��ʱ����NULL
    ///
    CSMVSendVirtualChannel* FirstSMVSendVirtualChannel();

    ///
    /// @brief
    ///    �����һ����ӳ���SMV��������ͨ��
    ///
    /// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
    ///
    /// @return
    ///     CVirtualChannel*    ָ������ͨ�������ָ�룬ʧ��ʱ����NULL
    ///
    CSMVSendVirtualChannel* NextSMVSendVirtualChannel(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    �ȷ�ֵ
    ///
    /// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true��ʧ�ܷ���false
    ///
    bool EqualAmplitude(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    �ֵ
    ///
    /// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true��ʧ�ܷ���false
    ///
    bool RatedAmplitude(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    ����ƽ��
    ///
    /// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true��ʧ�ܷ���false
    ///
    bool ThreePhaseBalance(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    �����
    ///
    /// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true��ʧ�ܷ���false
    ///
    bool EqualPhaseAngle(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    �������
    ///
    /// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true��ʧ�ܷ���false
    ///
    bool PhaseAnglePositiveSequence(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    ��Ƿ���
    ///
    /// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true��ʧ�ܷ���false
    ///
    bool PhaseAngleReverseSequence(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    ��Ƶ��
    ///
    /// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true��ʧ�ܷ���false
    ///
    bool EqualFrequency(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    ������������ͨ����Ƶ��Ϊָ��ֵ
    ///
    /// @param[in]  dNewFrequency    ����ͨ��Ƶ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool SetAllVCFrequency(double dNewFrequency);

    ///
    /// @brief
    ///    ʹ��������ͨ����г��������ͬ
    ///
    /// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool EqualHarmonicType(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    ʹͬ������ͨ����г����ֵ���
    ///
    /// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool EqualHarmonicAmplitude(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    ʹͬ������ͨ����г��������
    ///
    /// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool EqualHarmonicPhaseAngle(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    ���״̬���Խ��
    ///
    /// @return
    ///     CStateTestResult*    ָ����Խ�������ָ��
    ///
    CStateTestResult* GetStateTestResult() const;

    ///
    /// @brief
    ///    ����״̬���Խ��
    ///
    /// @param[in]  pNewStateTestResult    ���ò��Խ�������ָ��
    ///
    void SetStateTestResult(CStateTestResult* pNewStateTestResult);

protected:
    ///
    /// @brief
    ///    ��ȿ���һ��״̬����
    ///
    /// @param[in]
    ///     pOldStatus    -   �Ѵ��ڵ�״̬����
    ///
    /// @return
    ///     bool    �����ɹ�����TRUE
    ///
    bool __Copy(CStatus* pOldStatus);

    ///
    /// @brief
    ///    ���״̬����Ψһ��ʶID
    ///
    /// @return
    ///     int    ��ʶID
    ///
    int __GetID();

    ///
    /// @brief
    ///    ����<Status>
    ///
    /// @param[in]  xml    xml�ļ���������
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool __LoadConfig( CMarkup& xml );

    ///
    /// @brief
    ///    ����<Status>
    ///
    /// @param[in]  xml    xml�ļ���������
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool __SaveConfig( CMarkup& xml );

    ///
    /// @brief
    ///     �ͷ�����SMV��������ͨ������
    ///
    void __ClearSMVSendVirtualChannel();

    ///
    /// @brief
    ///    ����ָ�����ֵ�SMV��������ͨ��
    ///
    /// @param[in]  strChannelName    ����ͨ����
    ///
    /// @return
    ///     CSMVSendVirtualChannel*    ָ������ͨ�������ָ��
    ///
    CSMVSendVirtualChannel* __GetSMVSendVirtualChannel(std::wstring strChannelName);

    ///
    /// @brief
    ///    ���SMV��������ͨ����������
    ///
    /// @param[in]  pVirtualChannel    SMV��������ͨ��ָ��
    ///
    /// @return
    ///     bool    true��ʾ��ӳɹ�
    ///
    bool __AddSMVSendVirtualChannel(CSMVSendVirtualChannel* pVirtualChannel);

    ///
    /// @brief
    ///    ��״̬�������Ĭ�ϵ�����ͨ����
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __AddDefaultSMVSendVirtualChannels();

    ///
    /// @brief
    ///    ��ָ����״̬����Ƚ�SMV���ⷢ��ͨ���Ƿ���ͬ
    ///
    /// @param[in]  eCompareType    �ȽϷ�ʽ
    /// @param[in]  pOldStatus      ָ��״̬�����ָ��
    ///
    /// @return
    ///     bool    ��ͬ����true
    ///
    bool __CompareSMVSendVirtualChannels(enumCompareType eCompareType, CStatus* pOldStatus);

    ///
    /// @brief
    ///     ����״̬�����б仯��DO
    ///
    /// @param[in]  pOldStatus          ���Ƚϵ�״̬
    /// @param[out] mapChangedDoArray   ����ֵ�����˱仯��DOͨ�������ֺ�ֵ
    ///
    /// @return
    ///     bool    �б仯��DOʱ����true
    ///
    bool __FindChangedDO(CStatus* pOldStatus, std::map<std::wstring, bool>& mapChangedDoArray);

    ///
    /// @brief
    ///    ����ָ�����ֵ�����ͨ���ķ�ֵ
    ///
    /// @param[in]  strChannelName    ����ͨ����
    /// @param[in]  dNewAmplitude    �µķ�ֵ
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __SetSMVSendVCAmplitude(std::wstring strChannelName, double dNewAmplitude);

    ///
    /// @brief
    ///    ����ָ�����ֵ�����ͨ���ķ�ֵ��һ��ֵ��
    ///
    /// @param[in]  strChannelName    ����ͨ����
    /// @param[in]  dNewAmplitude    �µķ�ֵ��һ��ֵ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __SetSMVSendVCPrimaryAmplitude(std::wstring strChannelName, double dNewAmplitude);

    ///
    /// @brief
    ///    ����ָ�����ֵ�����ͨ�������
    ///
    /// @param[in]  strChannelName    ����ͨ����
    /// @param[in]  dNewPhaseAngle    �µ����
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __SetSMVSendVCPhaseAngle(std::wstring strChannelName, double dNewPhaseAngle);

    ///
    /// @brief
    ///    ���ָ�����ֵ�����ͨ�������
    ///
    /// @param[in]  strChannelName    ����ͨ����
    ///
    /// @return
    ///     double    ���ֵ
    ///
    double __GetSMVSendVCPhaseAngle(std::wstring strChannelName);

protected:
    int                                 m_nID;                          ///< ���ƿ�Ψһ��ʶ
    CParamConfigBase*                   m_pParamConfigBase;             ///< �������ù�����

    eSwitchType                         m_eSwitchType;                  ///< ״̬�л���ʽ
    int                                 m_iDelay;                       ///< ״̬�л���ʱms

    eLogicalOperator                    m_eLogicalOperator;             ///< �������л��߼����㷽ʽ
    std::map<std::wstring, bool>        m_mapDIGroup;                   ///< ����������
    std::map<std::wstring, bool>        m_mapDOGroup;                   ///< ����������

    std::list<CSMVSendVirtualChannel*>  m_listSMVSendVirtualChannelGroup;///< ����ͨ������
    CStateTestResult*                   m_pStateTestResult;             ///< ״̬���Խ��
};
#endif
