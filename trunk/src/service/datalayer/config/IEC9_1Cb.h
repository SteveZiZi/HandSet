/// @file
///
/// @brief
///     IEC61850 9-1 ���ƿ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.29
///
/// �޸���ʷ��
///

#ifndef __IEC9_1_CB_H__
#define __IEC9_1_CB_H__
#include "netcb.h"


///
/// @brief
///     IEC61850-9-1���ƿ�
///
class CIEC9_1Cb : public CNetCb
{
    friend class CParamConfigGOOSEBase;
    friend class CParamConfigGOOSESend;
    friend class CParamConfigSMVSend;
    friend class CSCLManager;
    friend class CFrameSendNet;

protected:
    CIEC9_1Cb(void);
    virtual ~CIEC9_1Cb(void);

public:
    ///
    /// @brief
    ///     ���캯��
    ///
    /// @param[in]  
    ///     pParamConfigBase    ָ��������ù�������ָ��
    ///
    CIEC9_1Cb(CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    ��ö�������A��
    ///
    /// @return
    ///     double    ��������A��
    ///
    double GetRatedPhaseCurrent();

    ///
    /// @brief
    ///    ���ö�������A��
    ///
    /// @param[in]  dRatedPhaseCurrent ��������A��
    ///
    void SetRatedPhaseCurrent(double dRatedPhaseCurrent);

    ///
    /// @brief
    ///    ��ö���ߵ�����A��
    ///
    /// @return
    ///     double    ����ߵ�����A��
    ///
    double GetRatedNeutralCurrent();

    ///
    /// @brief
    ///    ���ö���ߵ�����A��
    ///
    /// @param[in]  dRatedNeutralCurrent ����ߵ�����A��
    ///
    void SetRatedNeutralCurrent(double dRatedNeutralCurrent);

    ///
    /// @brief
    ///    ��ö���ѹ��kV��
    ///
    /// @return
    ///     double    ����ѹ��kV��
    ///
    double GetRatedPhaseVoltage();

    ///
    /// @brief
    ///    ���ö���ѹ��kV��
    ///
    /// @param[in]  dRatedPhaseVoltage ����ѹ��kV��
    ///
    void SetRatedPhaseVoltage(double dRatedPhaseVoltage);

    ///
    /// @brief
    ///    ��ö��ʱ��us��
    ///
    /// @return
    ///     int    ���ʱ��us��
    ///
    int GetRatedDelay();

    ///
    /// @brief
    ///    ���ö��ʱ��us��
    ///
    /// @param[in]  iRatedDelay ���ʱ��us��
    ///
    void SetRatedDelay(int iRatedDelay);

    ///
    /// @brief
    ///    ���״̬��#1
    ///
    /// @return
    ///     unsigned short    ״̬��#1
    ///
    unsigned short GetStatusWord1();

    ///
    /// @brief
    ///    ����״̬��#1
    ///
    /// @param[in]  nStatusWord1 ״̬��#1
    ///
    void SetStatusWord1(unsigned short nStatusWord1);

    ///
    /// @brief
    ///    ���״̬��#2
    ///
    /// @return
    ///     unsigned short    ״̬��#2
    ///
    unsigned short GetStatusWord2();

    ///
    /// @brief
    ///    ����״̬��#2
    ///
    /// @param[in]  nStatusWord2 ״̬��#2
    ///
    void SetStatusWord2(unsigned short nStatusWord2);

    ///
    /// @brief
    ///    ʹ��ָ����ͨ��ģ���������ƿ�
    ///
    /// @param[in]  eType    ģ������
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool UseChannelTemplate(enumSmvRecvConfigTemplateType eType);

    ///
    /// @brief
    ///    ���¿��ƿ������е�ѹͨ����һ�ζֵ
    ///
    /// @param[in]  dNewValue    �µĵ�ѹһ�ζֵ��V��
    ///
    void UpdateRatedPrimaryVoltage(double dNewValue);

    ///
    /// @brief
    ///    ���¿��ƿ������е���ͨ����һ�ζֵ
    ///
    /// @param[in]  dNewValue    �µĵ���һ�ζֵ��A��
    ///
    void UpdateRatedPrimaryCurrent(double dNewValue);

protected:
    ///
    /// @brief
    ///    ����IEC61850-9-1���ƿ�
    ///
    /// @param[in]  pXml ָ��XML���������
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool __LoadConfig(CMarkup& xml);

    ///
    /// @brief
    ///    ����IEC61850-9-1���ƿ�
    ///
    /// @param[in]  pXml ָ��XML���������
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool __SaveConfig(CMarkup& xml);

    ///
    /// @brief
    ///    ��ȿ���һ��ɨ���б��е�IEC61850-9-1���ƿ�
    ///
    /// @param[in]
    ///     Sav91Msg    -   IEC61850-9-1���ĵı�������
    ///
    /// @return
    ///     bool    �����ɹ�����TRUE
    ///
    virtual bool __Copy(Sav91Msg* pSav91Msg);

    ///
    /// @brief
    ///    ��ȿ���һ��IEC61850-9-1���ƿ�
    ///
    /// @param[in]
    ///     pIEC9_1Cb    -   IEC61850-9-1���ƿ�
    ///
    /// @return
    ///     bool    �����ɹ�����TRUE
    ///
    virtual bool __Copy(CIEC9_1Cb* pIEC9_1Cb);

private:
    double                              m_dRatedPhaseCurrent;           ///< ��������A��
    double                              m_dRatedNeutralCurrent;         ///< ����ߵ�����A��
    double                              m_dRatedPhaseVoltage;           ///< ����ѹ��kV��
    int                                 m_iRatedDelay;                  ///< ���ʱ��us��
    unsigned short                      m_uiStatusWord1;                ///< ״̬��#1
    unsigned short                      m_uiStatusWord2;                ///< ״̬��#2
};
#endif
