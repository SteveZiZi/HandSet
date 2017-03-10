/// @file
///
/// @brief
///     IEC61850 9-2 ���ƿ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.29
///
/// �޸���ʷ��
///

#ifndef __IEC9_2_CB_H__
#define __IEC9_2_CB_H__
#include "netcb.h"

///
/// @brief
///     ��IEC61850-9-2ͨ��Ʒ�ʵĲ���
///
enum enumQualityOperateType
{
    QUALITY_OPERATE_CLEAR = 0,                                          ///< ����
    QUALITY_OPERATE_TEST,                                               ///< �ü���
    QUALITY_OPERATE_RUN                                                 ///< ������
};

///
/// @brief
///     IEC61850-9-2���ƿ�
///
class CIEC9_2Cb : public CNetCb
{
    friend class CParamConfigGOOSEBase;
    friend class CParamConfigGOOSESend;
    friend class CParamConfigSMVSend;
    friend class CSCLManager;
    friend class CFrameSendNet;

protected:
    CIEC9_2Cb(void);
    virtual ~CIEC9_2Cb(void);

public:
    ///
    /// @brief
    ///     ���캯��
    ///
    /// @param[in]  
    ///     pParamConfigBase    ָ��������ù�������ָ��
    ///
    CIEC9_2Cb(CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    ���SvID
    ///
    /// @return
    ///     wstring    SvID
    ///
    std::wstring GetSvID();

    ///
    /// @brief
    ///    ����SvID
    ///
    /// @param[in]  strSvID  SvID
    ///
    void SetSvID(std::wstring strSvID);

    ///
    /// @brief
    ///    ������ݼ�����
    ///
    /// @return
    ///     wstring    ���ݼ�����
    ///
    std::wstring GetDataSetRef();

    ///
    /// @brief
    ///    �������ݼ�����
    ///
    /// @param[in]  strDataSetRef  ���ݼ�����
    ///
    void SetDataSetRef(std::wstring strDataSetRef);

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
    ///    ������ݼ���ѡ��
    ///
    /// @return
    ///     bool    �Ƿ������ݼ�
    ///
    bool GetDataSetOpt() const;

    ///
    /// @brief
    ///    �������ݼ���ѡ��
    ///
    /// @param[in]  bNewValue    �Ƿ������ݼ�
    ///
    void SetDataSetOpt(bool bNewValue);

    ///
    /// @brief
    ///    ���ˢ��ʱ���ѡ��
    ///
    /// @return
    ///     bool    �Ƿ���ˢ��ʱ��
    ///
    bool GetRefrTmOpt() const;

    ///
    /// @brief
    ///    ����ˢ��ʱ���ѡ��
    ///
    /// @param[in]  bNewValue    �Ƿ���ˢ��ʱ��
    ///
    void SetRefrTmOpt(bool bNewValue);

    ///
    /// @brief
    ///    ��ò����ʿ�ѡ��
    ///
    /// @return
    ///     bool    �Ƿ��Ͳ�����
    ///
    bool GetSmpRateOpt() const;

    ///
    /// @brief
    ///    ���ò����ʿ�ѡ��
    ///
    /// @param[in]  bNewValue    �Ƿ��Ͳ�����
    ///
    void SetSmpRateOpt(bool bNewValue);

    ///
    /// @brief
    ///    ��ñ��ļ��ܿ�ѡ��
    ///
    /// @return
    ///     bool    �Ƿ�Ա��ļ���
    ///
    bool GetSecurityOpt() const;

    ///
    /// @brief
    ///    ���ñ��ļ��ܿ�ѡ��
    ///
    /// @param[in]  bNewValue    �Ƿ�Ա��ļ���
    ///
    void SetSecurityOpt(bool bNewValue);

    ///
    /// @brief
    ///    ���ÿ��ƿ�����ͨ����Ʒ��
    ///
    /// @param[in]  eType    �������ͣ��磺���㡢�ü��ޡ�������
    ///
    void SetAllChannelsQuality(enumQualityOperateType eType);

protected:
    ///
    /// @brief
    ///    ����IEC61850-9-2���ƿ�
    ///
    /// @param[in]  pXml ָ��XML���������
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool __LoadConfig(CMarkup& xml);

    ///
    /// @brief
    ///    ����IEC61850-9-2���ƿ�
    ///
    /// @param[in]  pXml ָ��XML���������
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool __SaveConfig(CMarkup& xml);

    ///
    /// @brief
    ///    ��ȿ���һ��IEC61850-9-2���ƿ�
    ///
    /// @param[in]
    ///     pIEC9_2Cb    -   ���ƿ�ָ��
    ///
    /// @return
    ///     bool    �����ɹ�����TRUE
    ///
    virtual bool __Copy(CIEC9_2Cb* pIEC9_2Cb);

    ///
    /// @brief
    ///    ��ȿ���һ��ɨ���б��е�IEC61850-9-2���ƿ�
    ///
    /// @param[in]
    ///     pSav92Msg    -   IEC61850-9-2���ĵı�������
    ///
    /// @return
    ///     bool    �����ɹ�����TRUE
    ///
    virtual bool __Copy(Sav92Msg* pSav92Msg);

private:
    std::wstring                        m_strSvID;                      ///< SvID
    std::wstring                        m_strDataSetRef;                ///< ���ݼ�����
    bool                                m_bDataSetOpt;                  ///< �Ƿ������ݼ�
    bool                                m_bRefrTmOpt;                   ///< �Ƿ���ˢ��ʱ��
    bool                                m_bSmpRateOpt;                  ///< �Ƿ��Ͳ�����
    bool                                m_bSecurityOpt;                 ///< �Ƿ�Ա��Ľ��м���
};
#endif
