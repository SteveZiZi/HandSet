///
/// @brief
///     FT3���ƿ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.29
///
/// �޸���ʷ��
///

#ifndef __FT3_CB_H__
#define __FT3_CB_H__
#include "basecb.h"


///
/// @brief
///    FT3���ƿ�
///
class CFT3Cb : public CBaseCb
{
    friend class CParamConfigGOOSEBase;
    friend class CParamConfigGOOSESend;
    friend class CParamConfigSMVSend;
    friend class CSCLManager;
    friend class CFrameSendFt3;

protected:
    CFT3Cb(void);
    virtual ~CFT3Cb(void);

public:
    ///
    /// @brief
    ///     ���캯��
    ///
    /// @param[in]  
    ///     pParamConfigBase    ָ��������ù�������ָ��
    ///
    CFT3Cb(CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    ���LNName
    ///
    /// @return
    ///     int    LNName
    ///
    int GetLNName();

    ///
    /// @brief
    ///    ����LNName
    ///
    /// @param[in]  int LNName
    ///
    void SetLNName(int iLNName);

    ///
    /// @brief
    ///    ���LDName
    ///
    /// @return
    ///     int    LDName
    ///
    int GetLDName();

    ///
    /// @brief
    ///    ����LDName
    ///
    /// @param[in]  int iLDName
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool SetLDName(int iLDName);

    ///
    /// @brief
    ///    ���DSName
    ///
    /// @return
    ///     int    DSName
    ///
    int GetDSName();

    ///
    /// @brief
    ///    ����DSName
    ///
    /// @param[in]  int iDSName
    ///
    void SetDSName(int iDSName);

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
    ///    ����FT3���ƿ�
    ///
    /// @param[in]  pXml ָ��XML���������
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool __LoadConfig(CMarkup& xml);

    ///
    /// @brief
    ///    ����FT3���ƿ�
    ///
    /// @param[in]  pXml ָ��XML���������
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool __SaveConfig(CMarkup& xml);

    ///
    /// @brief
    ///    ��ȿ���һ��FT3���ƿ�
    ///
    /// @param[in]
    ///     pFt3Cb    -   FT3���ƿ�
    ///
    /// @return
    ///     bool    �����ɹ�����TRUE
    ///
    virtual bool __Copy(CFT3Cb* pFt3Cb);

    ///
    /// @brief
    ///    ��ȿ���һ��ɨ���б��е�FT3���ƿ�
    ///
    /// @param[in]
    ///     pFt3Msg    -   FT3���ĵı�������
    ///
    /// @return
    ///     bool    �����ɹ�����TRUE
    ///
    virtual bool __Copy(NanRuiFT3Msg* pFt3Msg);

protected:
    int                                 m_iLNName;                      ///< LNName
    int                                 m_iDSName;                      ///< DSName

    double                              m_dRatedPhaseCurrent;           ///< ��������A��
    double                              m_dRatedNeutralCurrent;         ///< ����ߵ�����A��
    double                              m_dRatedPhaseVoltage;           ///< ����ѹ��kV��
    int                                 m_iRatedDelay;                  ///< ���ʱ��us��
    unsigned short                      m_uiStatusWord1;                ///< ״̬��#1
    unsigned short                      m_uiStatusWord2;                ///< ״̬��#2
};

///
/// @brief
///    FT3��չ���ƿ�
///
class CFT3ExpandCb: public CFT3Cb
{
    friend class CParamConfigSMVSend;
    friend class CFrameSendFt3;

public:
    ///
    /// @brief
    ///     ���캯��
    ///
    /// @param[in]  
    ///     pParamConfigBase    ָ��������ù�������ָ��
    ///
    CFT3ExpandCb(CParamConfigBase* pParamConfigBase);

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

protected:
    ///
    /// @brief
    ///    ��ȿ���һ��ɨ���б��е�FT3��չ���ƿ�
    ///
    /// @param[in]
    ///     pFt3ExpandMsg    -   FT3��չ���ĵı�������
    ///
    /// @return
    ///     bool    �����ɹ�����TRUE
    ///
    virtual bool __Copy(GuoWangFT3Msg* pFt3ExpandMsg);
};

#endif
