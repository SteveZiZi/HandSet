/// @file
///
/// @brief
///     �ֶ������������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.06.07
///
/// �޸���ʷ��
///

#ifndef __PARAM_CONFIG_MANUAL_TEST_H__
#define __PARAM_CONFIG_MANUAL_TEST_H__
#include <map>
#include "paramconfigbase.h"
#include "SMVSendVirtualChannel.h"

///
/// @brief
///    SMV�仯��
///
enum eSMVChangeItem
{
    CHANGE_ITEM_UNKNOWN = 0,                                            ///< δ֪
    CHANGE_ITEM_AMPLITUDE,                                              ///< ��ֵ
    CHANGE_ITEM_PHASE_ANGLE,                                            ///< ���
    CHANGE_ITEM_FREQUENCY                                               ///< Ƶ��
};

///
/// @brief
///    �޸���Ч��ʽ
///
enum eModifyEffectiveMode
{
    EFFECTIVE_MODE_UNKNOWN = 0,                                         ///< δ֪
    EFFECTIVE_MODE_IMMEDIATE,                                           ///< ��ʱ��Ч
    EFFECTIVE_MODE_MANUALLY                                             ///< �ֶ���Ч
};

///
/// @brief
///    ���鿪ʼ��ʽ
///
enum eStartMode
{
    START_MODE_UNKNOWN = 0,                                             ///< δ֪
    START_MODE_APART,                                                   ///< �ֱ�ʼ
    START_MODE_TOGETHER                                                 ///< ͬʱ��ʼ
};

///
/// @brief
///    �ֶ�����
///
class CParamconfigManualTest :
    public CParamConfigBase
{
    friend class CSMVSendVirtualChannel;
    friend class CStateTestResult;

protected:
    CParamconfigManualTest(void);
    virtual ~CParamconfigManualTest(void);

public:
    ///
    /// @brief
    ///     ��ȡ����ʵ���ӿ�
    ///
    static CParamconfigManualTest* getInstance();

    ///
    /// @brief
    ///    ���������ļ�
    ///
    /// @param[in]  strConfigFile �ļ�·��
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool LoadConfig( std::wstring strConfigFile );

    ///
    /// @brief
    ///    ���������ļ�
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool SaveConfig();

    ///
    /// @brief
    ///    ��������SMV��������ͨ������ʵͨ��ӳ���
    ///    ע�⣺����FirstSMVSendVirtualChannel()����ʱ���Զ����ô˺�����
    ///           �����Ҫ��������ӳ��������ֶ����ô˺���
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool RebuildSMVSendChannelMap();

    ///
    /// @brief
    ///    ö�ٵ�һ����ӳ���SMV��������ͨ��
    ///
    /// @param[in]  bRebuildMap     �Ƿ���Ҫ�ؽ�SMV����ӳ���GOOSE����ӳ���
    ///
    /// @return
    ///     CVirtualChannel*    ָ������ͨ�������ָ�룬ʧ��ʱ����NULL
    ///
    CSMVSendVirtualChannel* FirstSMVSendVirtualChannel(bool bRebuildMap = true);

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
    ///    ���SMV�仯��
    ///
    /// @return
    ///     eSMVChangeItem    SMV�仯��
    ///
    eSMVChangeItem GetSMVChangeItem();

    ///
    /// @brief
    ///    ����SMV�仯��
    ///
    /// @param[in]  newSMVChangeItem    �µ�SMV�仯��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool SetSMVChangeItem(eSMVChangeItem newSMVChangeItem);

    ///
    /// @brief
    ///    ����޸���Ч��ʽ
    ///
    /// @return
    ///     eModifyEffectiveMode    �޸���Ч��ʽ
    ///
    eModifyEffectiveMode GetModifyEffectiveMode();

    ///
    /// @brief
    ///    �����޸���Ч��ʽ
    ///
    /// @param[in]  newModifyEffectiveMode    �µ��޸���Ч��ʽ
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool SetModifyEffectiveMode(eModifyEffectiveMode newModifyEffectiveMode);

    ///
    /// @brief
    ///    ������鿪ʼ��ʽ
    ///
    /// @return
    ///     eStartMode    ���鿪ʼ��ʽ
    ///
    eStartMode GetStartMode();

    ///
    /// @brief
    ///    �������鿪ʼ��ʽ
    ///
    /// @param[in]  newStartMode    �µ����鿪ʼ��ʽ
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool SetStartMode(eStartMode newStartMode);

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
    ///    ����Ƿ���Ҫ��ʾ������
    ///
    bool GetIsShowDI() const
    {
        return m_bIsShowDI;
    }

    ///
    /// @brief
    ///    �����Ƿ���Ҫ��ʾ������
    ///
    /// @param[in]  val    �Ƿ���ʾ
    ///
    void SetIsShowDI(bool val)
    {
        m_bIsShowDI = val;
    }

protected:

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
    ///     �ͷ�����SMV��������ͨ������
    ///
    void __ClearSMVSendVirtualChannel();

    ///
    /// @brief
    ///    ����ӳ�䵽ָ��SMV��������ͨ������ʵͨ��������ʵͨ����ָ���������ͨ����������
    ///
    /// @param[in]  pVirtualChannel    ����ͨ��ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __FillSMVSendChannelGroup(CSMVSendVirtualChannel* pVirtualChannel);

    ///
    /// @brief
    ///    ���ָ�����ֵ�����ͨ��
    ///
    /// @param[in]  strChannelName    ����ͨ����
    ///
    /// @return
    ///     CSMVSendVirtualChannel*    ָ������ͨ�������ָ��
    ///
    CSMVSendVirtualChannel* __GetSMVSendVirtualChannel(std::wstring strChannelName);

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
    eSMVChangeItem                              m_eSMVChangeItem;           ///< SMV�仯��
    eModifyEffectiveMode                        m_eModifyEffectiveMode;     ///< �޸���Ч��ʽ
    eStartMode                                  m_eStartMode;               ///< ���鿪ʼ��ʽ
    bool                                        m_bIsShowDI;                ///< �Ƿ���ʾ�������Ի���

    std::list<CSMVSendVirtualChannel*>          m_listSMVSendVirtualChannelGroup;   ///< ����ͨ������
};
#endif
