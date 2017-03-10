/**
 * \file
 *      dsm_config.cpp
 *
 * \brief
 *      dsm������Ϣ������ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/3/16
 */

#include "stdafx.h"
#include "dsm_config.h"

#include "src/utils/utility/dsm_utility.h"

#define DEFAULT_SYSTEM_INFO     L"info.xml"             ///< ϵͳ��Ϣ�ļ�
#define DEFAULT_CFG_FILE        L"config.xml"           ///< Ĭ�������ļ�
#define DEFAULT_CFG_BACKUP      L"config_backup\\"      ///< ���������ļ�·��


/**
 *      ת�����ʵĸ����ַ���
 *
 *  \param[in]
 *      csVal           �����ַ���
 *  \param[in/out]
 *      rcsResult       ����Ҫ��ĸ����ַ���
 *  \return
 *      bool            ת���ɹ�����true��ʧ�ܷ���false
 *
 */
static bool dsm_strfloatConvert(CString csVal, CString& rcsResult)
{
    csVal.Trim();

    double fVolt    = 0.0;
    bool bDot       = false;
    int nLen        = csVal.GetLength();

    for(int i = 0; i < nLen; i++)
    {
        if(0 == _istdigit(csVal.GetAt(i)))
        {
            if(L'.' == csVal.GetAt(i) && !bDot)
            {
                bDot = true;
                continue;
            }

            return false;
        }
    }

    if(EOF == swscanf(csVal, L"%lf", &fVolt) || 0 == _finite(fVolt))
    {
        return false;
    }

    rcsResult.Format(L"%.03lf", fVolt);

    return true;
}

/**
 *      �ַ���ת32λ�޷�������
 *
 *  \param[in]
 *      csVal           �����ַ���
 *  \param[in/out]
 *      rcsResult       ����Ҫ��������ַ���
 *  \return
 *      bool            ת���ɹ�����true��ʧ�ܷ���false

 */
static bool dsm_struint32Convert(CString csVal, CString& rcsResult)
{
    int nLen            = csVal.GetLength();
    unsigned int nVal   = 0;

    if(nLen > 10)
    {
        return false;
    }

    for (int i = 0; i < nLen; i++)
    {
        if(_istdigit(csVal.GetAt(i)))
        {
            continue;
        }

        return false;
    }

    if(1 != swscanf(csVal, L"%u", &nVal))
    {
        return false;
    }

    rcsResult.Format(L"%u", nVal);

    return true;
}



dsm_config::dsm_config()
: m_pXml(NULL)
, m_bModified(false)
, m_csXML(L"")
, m_nHeight(480)
, m_nWidth(640)
, m_csProduct(L"")
, m_csSoftVer(L"1.0")
, m_csHardVer(L"1.0")
, m_csSysVer(L"1.0")
, m_csSoftBuildVer(L"0")
{
}

dsm_config::~dsm_config()
{
    dsm_config::Save();

    if(m_pXml)
    {
        delete m_pXml;
        m_pXml = NULL;
    }
}




/**
 *      ���������ļ�
 *
 *  \return
 *      bool        ��������xml�ɹ�����true��ʧ�ܷ���false
 */
bool dsm_config::Load()
{
    CMarkup* pXML = dsm_config::getInstance().m_pXml;

    if(pXML)
    {
        // ��������load�����
        delete pXML;
    }

    pXML = new CMarkup;
    dsm_config::getInstance().m_pXml = pXML;
    dsm_config::getInstance().m_csXML.Format(L"%s%s", DSM_GetRuntimeDir(), DEFAULT_CFG_FILE);

    // ����ϵͳ��Ϣ
    dsm_config::getInstance().loadSystemInfo();

    // ���������ļ�
    if(!pXML->Load((LPCTSTR)dsm_config::getInstance().m_csXML))
    {
        // ����ʧ����ӱ��������ļ���ȡ
        return dsm_config::Reset();
    }

    return true;
}

/**
 *      ���浱ǰ�ڴ��xml���ļ�
 *
 *  \return
 *      bool        ����xml�����ļ��ɹ�����true��ʧ�ܷ���false
 */
bool dsm_config::Save()
{
    dsm_config& rDsmConfig = dsm_config::getInstance();

    if(!rDsmConfig.m_bModified)
    {
        return true;
    }

    if(!rDsmConfig.m_pXml->Save((LPCTSTR)rDsmConfig.m_csXML))
    {
        ::OutputDebugString(L"failed to save config xml\n");
        return false;
    }

    rDsmConfig.m_bModified = false;

    return true;
}

/**
 *      ��ȡ��̬ʵ������
 *
 *  \return
 *      dsm_config&         ���ؾ�̬ʵ������
 */
dsm_config& dsm_config::getInstance()
{
    static dsm_config s_rdsmConfig;

    return s_rdsmConfig;
}

/**
 *      ��ȡָ���ڵ�ֵ
 *
 *  \param[in]
 *      rcsAttrName         ָ���Ľڵ���
 *  \return
 *      CString             ����ָ���ڵ�����ֵ�������ؿմ�
 */
CString dsm_config::getAttribute( CMarkup* pXML, const CString& rcsAttrName, CString csDefValue /* = L"" */ )
{
    if(pXML)
    {
        pXML->ResetPos();

        if(pXML->FindChildElem((LPCTSTR)rcsAttrName) && pXML->IntoElem())
        {
            CString csRet(pXML->GetAttrib(ATTR_VALUE).c_str());

            if(!csRet.IsEmpty())
            {
                return csRet;
            }
        }
    }

    ::OutputDebugString(L"dsm: config: failed to get attribute value.\n");

    return csDefValue;
}

/**
 *      ����ָ���ڵ�ֵ
 *
 *  \param[in]
 *      rcsAttrName     �ڵ���
 *  \param[in]
 *      csValue        �ڵ�ֵ
 *  \return
 *      bool            ���óɹ�����true��ʧ�ܷ���false
 *
 *  \note
 *      <�ڵ��� val="�ڵ�ֵ" />
 */
bool dsm_config::setAttribute( const CString& rcsAttrName, const CString csValue )
{
    CMarkup* pXML = dsm_config::getInstance().m_pXml;
    if(pXML)
    {
        pXML->ResetPos();

        if(pXML->FindChildElem((LPCTSTR)rcsAttrName) && pXML->IntoElem())
        {
            if(pXML->SetAttrib(ATTR_VALUE, (LPCTSTR)csValue))
            {
                dsm_config::getInstance().m_bModified = true;
                return true;
            }
        }
    }

    ::OutputDebugString(L"failed to set attribute\n");
    return false;
}

/**
 *      ����ָ���ڵ�ֵ
 *
 *  \param[in]
 *      rcsAttrName     �ڵ���
 *  \param[in]
 *      nValue          �ڵ�ֵ
 *  \return
 *      bool            ���óɹ�����true��ʧ�ܷ���false
 *
 *  \note
 *      <�ڵ��� val="�ڵ�ֵ" />
 */
bool dsm_config::setAttribute( const CString& rcsAttrName, int nValue )
{

    CString csValue;

    csValue.Format(L"%d", nValue);

    return dsm_config::setAttribute(rcsAttrName, csValue);
}


/**
 *      ��ȡ��������
 *
 *  \return
 *      int             ���ر������ȵ�ֵ1~100
 */
int dsm_config::GetBrightness()
{
    return _wtoi(dsm_config::getAttribute(getInstance().m_pXml, ATTR_BRIGHTNESS));
}

/**
 *      ���ñ�������
 *
 *  \param[in]
 *      nBright         �趨�������ȵ�ֵ��1~100
 */
void dsm_config::SetBrightness( int nBright )
{
    dsm_config::setAttribute(ATTR_BRIGHTNESS, nBright);
}

/**
 *      ��ȡ�����ʶ
 *
 * \return
 *      int             ���������ʶ�������������2052
 */
int dsm_config::GetLCID()
{
    return _wtoi(dsm_config::getAttribute(getInstance().m_pXml, ATTR_LOCALID));
}

/**
 *      �������Ա�ʶ
 *
 *  \param[in]
 *      nLocalID        �����ʶ��32λ����
 */
void dsm_config::SetLCID( int nLocalID )
{
    dsm_config::setAttribute(ATTR_LOCALID, nLocalID);
}


/**
 *      ��ȡ����ʱ��
 */
int dsm_config::GetBrightTime()
{
    return _wtoi(dsm_config::getAttribute(getInstance().m_pXml, ATTR_BRIGHTTIME));
}

/**
 *      ���ñ���ʱ��
 */
void dsm_config::SetBrightTime(int nTime)
{
    dsm_config::setAttribute(ATTR_BRIGHTTIME, nTime);
}

/**
 *      �ж�ϵͳ�Ƿ��Ѿ���ʼ��
 *
 *  \return
 *      bool            true�����Ѿ���ʼ����false����δ��ʼ��
 */
bool dsm_config::GetSystemInit()
{
#if SHENG_ZE
    return _wtoi(dsm_config::getAttribute(getInstance().m_pXml, ATTR_SYSTEMINITED)) ? true : false;
#else
	return true;
#endif
}

/**
 *      ����ϵͳ��ʼ��״̬
 */
void dsm_config::SetSystemInit(bool bInit)
{
    dsm_config::setAttribute(ATTR_SYSTEMINITED, bInit ? 1: 0);
}

/**
 *      ʱ���Ƿ��Զ�ͬ��
 */
bool dsm_config::GetTimeAutoSync()
{
    return _wtoi(dsm_config::getAttribute(getInstance().m_pXml, ATTR_TIMESYNC)) ? true : false;
}

/**
 *      ����ʱ��ͬ����ʽ
 *
 *  \param[in]
 *      bAuto       true    -   �Զ�ͬ��
 *                  false   -   �ֶ�ͬ��
 */
void dsm_config::SetTimeSync(bool bAuto)
{
    dsm_config::setAttribute(ATTR_TIMESYNC, bAuto ? 1: 0);
}


/**
 *      ��������
 */
void dsm_config::SetPassword( CString csPassword )
{
    dsm_config::setAttribute(ATTR_PASSWORD, csPassword);
}

/**
 *      ��֤����
 */
bool dsm_config::VerifyPasswrod( CString csPassword )
{
    CString csPW = dsm_config::getAttribute(getInstance().m_pXml, ATTR_PASSWORD);

    if(csPW.Compare(csPassword) == 0)
    {
        return true;
    }

    return false;
}

bool dsm_config::Reset()
{
    // ��ȡ���������ļ�·��
    CString csBackup;
    csBackup.Format(L"%s%s%s", DSM_GetAppDir(), DEFAULT_CFG_BACKUP, DEFAULT_CFG_FILE);

    // �������ݵ������ļ�
    if(!CopyFile(csBackup, dsm_config::getInstance().m_csXML, FALSE))
    {
        return false;
    }

    if(!dsm_config::getInstance().m_pXml)
    {
        dsm_config::getInstance().m_pXml = new CMarkup;
    }

    // ���¼��������ļ�
    if(!dsm_config::getInstance().m_pXml->Load((LPCTSTR)dsm_config::getInstance().m_csXML))
    {
        ::OutputDebugString(L"dsm: config: load config.xml failed.\n");

        return false;
    }

    return true;
}

bool dsm_config::loadSystemInfo()
{
    CMarkup doc;
    CString csInfo;
    csInfo.Format(L"%s%s", DSM_GetAppDir(), DEFAULT_SYSTEM_INFO);

    if(!doc.Load((LPCTSTR)csInfo))
    {
        RETAILMSG(true, (L"dsm: failed to load system info.xml\n"));

        return false;
    }

    dsm_config& rInst       = getInstance();

    rInst.m_nWidth          = _wtoi(dsm_config::getAttribute(&doc, ATTR_SCREENWIDTH, L"640"));
    rInst.m_nHeight         = _wtoi(dsm_config::getAttribute(&doc, ATTR_SCREENHEIGHT, L"480"));

    rInst.m_csProduct       = dsm_config::getAttribute(&doc, ATTR_PRODUCT_NAME, L"");

    rInst.m_csSoftVer       = dsm_config::getAttribute(&doc, ATTR_SOFTWARE_VER, L"1.0.0");
    rInst.m_csSoftBuildVer  = dsm_config::getAttribute(&doc, ATTR_SOFTWARE_BUILD, L"0");

    rInst.m_csCompanyName   = dsm_config::getAttribute(&doc, ATTR_COMPANY, L"");
    rInst.m_csCompanyAddress= dsm_config::getAttribute(&doc, ATTR_ADDRESS, L"");
    rInst.m_csTelephone     = dsm_config::getAttribute(&doc, ATTR_TELEPHONE, L"");

    loadHardwareInfo();

    return true;
}


/* 
 *  ����DLL��Ҫ�������ĺ���ָ��
 */
typedef BOOL (*PepcGetSoftWareVersion)(DWORD *pdwSoftWareVer);

typedef BOOL (*PepcGetHardWareVersion)(DWORD *pdwHardWareVer);


/**
 *      ����ϵͳӲ����Ϣ
 */
bool dsm_config::loadHardwareInfo()
{
    // ���ػ�ȡӲ���汾��Ϣ��dll
    HINSTANCE hDll = LoadLibrary(L"epcSysInfo.dll");
    if(!hDll)
    {
        RETAILMSG(true, (L"dsm: failed to load epcSysInfo.dll\n"));
        return false;
    }

    DWORD dwSys = 0, dwHard = 0;

    // ������ȡϵͳ�汾�ĺ���
    PepcGetSoftWareVersion epcGetSoftWareVersion = (PepcGetSoftWareVersion)GetProcAddress(hDll, L"epcGetSoftWareVersion");
    if(!epcGetSoftWareVersion)
    {
        RETAILMSG(true, (L"dsm: failed to export system verison function\n"));
    }
    else
    {
        if(!epcGetSoftWareVersion(&dwSys))
        {
            dwSys = 0;
            RETAILMSG(true, (L"dsm: failed to get system verison\n"));
        }
    }

    // ������ȡӲ���汾�ĺ���
    PepcGetHardWareVersion   epcGetHardWareVersion = (PepcGetHardWareVersion)GetProcAddress(hDll, L"epcGetHardWareVersion");
    if(!epcGetHardWareVersion)
    {
        RETAILMSG(true, (L"dsm: failed to export hardware verison function\n"));
    }
    else
    {
        if(!epcGetHardWareVersion(&dwHard))
        {
            dwHard = 0;
            RETAILMSG(true, (L"dsm: failed to get hardware verison\n"));
        }
    }

    if(hDll)
    {
        FreeLibrary(hDll);
    }

    getInstance().m_csSysVer.Format(L"%x.%02x", HIBYTE(dwSys), LOBYTE(dwSys));
    getInstance().m_csHardVer.Format(L"%x.%02x", HIBYTE(dwHard), LOBYTE(dwHard));

    return true;
}

