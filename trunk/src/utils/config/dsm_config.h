/**
 * \file
 *      dsm_config.h
 *
 * \brief
 *      dsm������Ϣ������ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 *  \note
 *      config.xmlӦ��ѭһ���ĸ�ʽ
 *      <attribute val="xxx" />
 *
 * \author
 *      chao      2013/3/16
 */

#ifndef DSM_CONFIG_H__
#define DSM_CONFIG_H__

#include <errno.h>

#include <map>

    
#include "src/utils/cmarkup/markup.h"


// xml���Խڵ���
//////////////////////////////////////////////////////////////////////////
#define ATTR_VALUE                  L"val"                  ///< ����ֵ��
#define ATTR_BRIGHTNESS             L"brightness"           ///< ��������
#define ATTR_BRIGHTTIME             L"brighttime"           ///< ����ʱ��
#define ATTR_LOCALID                L"localid"              ///< �����ʶ
#define ATTR_SCREENWIDTH            L"screenwidth"          ///< ��Ļ��ȣ����أ�
#define ATTR_SCREENHEIGHT           L"screenheight"         ///< ��Ļ�߶ȣ����أ�
#define ATTR_SYSTEMINITED           L"systeminited"         ///< ϵͳ��ʼ����־
#define ATTR_TIMESYNC               L"timesync"             ///< ʱ��ͬ����ʽ
#define ATTR_PASSWORD               L"password"             ///< ����


#define ATTR_PRODUCT_NAME           L"product"              ///< ��ȡ��Ʒ����
#define ATTR_SOFTWARE_VER           L"softwarever"          ///< ��ȡ����汾
#define ATTR_SOFTWARE_BUILD         L"softbuild"            ///< ��ȡ����汾��build��
#define ATTR_COMPANY                L"company"              ///< ��ȡ��˾��Ϣ
#define ATTR_ADDRESS                L"address"              ///< ��˾��ַ
#define ATTR_TELEPHONE              L"telephone"            ///< ��˾�绰




/**
 *      dsm���ù�������
 *
 *  \note
 *      �ṩ������Ϣ�Ĵ�ȡ���Լ�ָ��������Ϣ�ı����뵼��
 */
class dsm_config
{
public:
    /**
     *      ���������ļ�
     */
    static bool Load();

    /**
     *      ���浱ǰ�ڴ��xml���ļ�
     */
    static bool Save();

    /**
     *      �����������
     */
    static bool Reset();

public:
    /**
     *      ��ȡ��������
     */
    static int GetBrightness();

    /**
     *      ���ñ�������
     */
    static void SetBrightness(int nBright);

    /**
     *      ��ȡ����ʱ��
     */
    static int GetBrightTime();

    /**
     *      ���ñ���ʱ��
     */
    static void SetBrightTime(int nTime);

    /**
     *      ��ȡ�����ʶ
     */
    static int GetLCID();

    /**
     *      �������Ա�ʶ
     */
    static void SetLCID(int nLocalID);

    /**
     *      ��ȡ��Ļ�������ֵ
     */
    static int GetScreenWidth()
    {
        return getInstance().m_nWidth;
    }

    /**
     *      ��ȡ��Ļ�߶�����ֵ
     */
    static int GetScreenHeight()
    {
        return getInstance().m_nHeight;
    }

    /**
     *      �ж�ϵͳ�Ƿ��Ѿ���ʼ��
     *
     *  \return
     *      bool            true�����Ѿ���ʼ����false����δ��ʼ��
     */
    static bool GetSystemInit();

    /**
     *      ����ϵͳ��ʼ��״̬
     */
    static void SetSystemInit(bool bInit);

    /**
     *      ��������
     *
     *  \param[in]
     *      csPassword      ������
     */
    static void SetPassword(CString csPassword);

    /**
     *      ��֤����
     *
     *  \param[in]
     *      csPassword          ����֤������
     *  \bool
     *      bool                ������ȷ����true������ȷ����false
     */
    static bool VerifyPasswrod(CString csPassword);

    /**
     *      ʱ���Ƿ��Զ�ͬ��
     */
    static bool GetTimeAutoSync();

    /**
     *      ����ʱ��ͬ����ʽ
     *
     *  \param[in]
     *      bAuto       true    -   �Զ�ͬ��
     *                  false   -   �ֶ�ͬ��
     */
    static void SetTimeSync(bool bAuto);

//////////////////////////////////////////////////////////////////////////
// ���ڽ������Ϣ

    /**
     *      ��ȡ��Ʒ����
     */
    static CString GetProductName()
    {
        return getInstance().m_csProduct;
    }

    /**
     *      ��ȡӲ���汾
     */
    static CString GetHardwareVersion()
    {
        return getInstance().m_csHardVer;
    }

    /**
     *      ��ȡ����汾
     */
    static CString GetSoftwareVersion()
    {
        return getInstance().m_csSoftVer;
    }

    /**
     *      ��ȡ����汾��build��
     */
    static CString GetSoftwareBuild()
    {
        return getInstance().m_csSoftBuildVer;
    }

    /**
     *      ��ȡϵͳ�汾��
     */
    static CString GetSystemVersion()
    {
        return getInstance().m_csSysVer;
    }

    /**
     *      ��ȡ��˾����
     */
    static CString GetCompanyName()
    {
        return getInstance().m_csCompanyName;
    }

    /**
     *      ��ȡ��˾��ַ
     */
    static CString GetCompanyAddress()
    {
        return getInstance().m_csCompanyAddress;
    }

    /**
     *      ��ȡ����֧�ֵ绰
     */
    static CString GetTelephoneSupport()
    {
        return getInstance().m_csTelephone;
    }

private:
    dsm_config();

    ~dsm_config();

    /**
     *      ��ȡ��̬ʵ������
     */
    static dsm_config& getInstance();

    /**
     *      ��ȡָ���ڵ�ֵ���ַ�����
     */
    static CString getAttribute(CMarkup* pXML, const CString& rcsAttrName, CString csDefValue = L"");

    /**
     *      ����ָ���ڵ�ֵ
     */
    static bool setAttribute(const CString& rcsAttrName, const CString csValue);

    /**
     *      ����ָ���ڵ�ֵ
     */
    static bool setAttribute(const CString& rcsAttrName, int nValue);

    /**
     *      ����ϵͳ��Ϣ
     */
    static bool loadSystemInfo();

    /**
     *      ����ϵͳӲ����Ϣ
     */
    static bool loadHardwareInfo();


private:

    bool                m_bModified;            ///< �޸ı�־λ
    CString             m_csXML;                ///< �����ļ�·��
    CMarkup*            m_pXml;                 ///< CMarkupָ��

    int                 m_nWidth;               ///< �豸���
    int                 m_nHeight;              ///< �豸�߶�
    CString             m_csProduct;            ///< �豸����
    CString             m_csHardVer;            ///< Ӳ���汾��
    CString             m_csSysVer;             ///< ϵͳ�汾��
    CString             m_csSoftVer;            ///< ����汾��
    CString             m_csSoftBuildVer;       ///< ���Build��
    CString             m_csCompanyName;        ///< ��˾����
    CString             m_csCompanyAddress;     ///< ��˾��ַ
    CString             m_csTelephone;          ///< ����֧�ֵ绰
};


#endif // DSM_CONFIG_H__


