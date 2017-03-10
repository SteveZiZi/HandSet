/**
 * \file
 *      dsm_config.cpp
 *
 * \brief
 *      dsm配置信息管理器实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/3/16
 */

#include "stdafx.h"
#include "dsm_config.h"

#include "src/utils/utility/dsm_utility.h"

#define DEFAULT_SYSTEM_INFO     L"info.xml"             ///< 系统信息文件
#define DEFAULT_CFG_FILE        L"config.xml"           ///< 默认配置文件
#define DEFAULT_CFG_BACKUP      L"config_backup\\"      ///< 备份配置文件路径


/**
 *      转换合适的浮点字符串
 *
 *  \param[in]
 *      csVal           输入字符串
 *  \param[in/out]
 *      rcsResult       符合要求的浮点字符串
 *  \return
 *      bool            转换成功返回true，失败返回false
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
 *      字符串转32位无符号整型
 *
 *  \param[in]
 *      csVal           输入字符串
 *  \param[in/out]
 *      rcsResult       符合要求的整型字符串
 *  \return
 *      bool            转换成功返回true，失败返回false

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
 *      加载配置文件
 *
 *  \return
 *      bool        加载配置xml成功返回true，失败返回false
 */
bool dsm_config::Load()
{
    CMarkup* pXML = dsm_config::getInstance().m_pXml;

    if(pXML)
    {
        // 不处理多次load的情况
        delete pXML;
    }

    pXML = new CMarkup;
    dsm_config::getInstance().m_pXml = pXML;
    dsm_config::getInstance().m_csXML.Format(L"%s%s", DSM_GetRuntimeDir(), DEFAULT_CFG_FILE);

    // 加载系统信息
    dsm_config::getInstance().loadSystemInfo();

    // 加载配置文件
    if(!pXML->Load((LPCTSTR)dsm_config::getInstance().m_csXML))
    {
        // 加载失败则从备份配置文件读取
        return dsm_config::Reset();
    }

    return true;
}

/**
 *      保存当前内存的xml到文件
 *
 *  \return
 *      bool        保存xml配置文件成功返回true，失败返回false
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
 *      获取静态实例对象
 *
 *  \return
 *      dsm_config&         返回静态实例对象
 */
dsm_config& dsm_config::getInstance()
{
    static dsm_config s_rdsmConfig;

    return s_rdsmConfig;
}

/**
 *      获取指定节点值
 *
 *  \param[in]
 *      rcsAttrName         指定的节点名
 *  \return
 *      CString             返回指定节点名的值，出错返回空串
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
 *      设置指定节点值
 *
 *  \param[in]
 *      rcsAttrName     节点名
 *  \param[in]
 *      csValue        节点值
 *  \return
 *      bool            设置成功返回true，失败返回false
 *
 *  \note
 *      <节点名 val="节点值" />
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
 *      设置指定节点值
 *
 *  \param[in]
 *      rcsAttrName     节点名
 *  \param[in]
 *      nValue          节点值
 *  \return
 *      bool            设置成功返回true，失败返回false
 *
 *  \note
 *      <节点名 val="节点值" />
 */
bool dsm_config::setAttribute( const CString& rcsAttrName, int nValue )
{

    CString csValue;

    csValue.Format(L"%d", nValue);

    return dsm_config::setAttribute(rcsAttrName, csValue);
}


/**
 *      获取背光亮度
 *
 *  \return
 *      int             返回背光亮度的值1~100
 */
int dsm_config::GetBrightness()
{
    return _wtoi(dsm_config::getAttribute(getInstance().m_pXml, ATTR_BRIGHTNESS));
}

/**
 *      设置背光亮度
 *
 *  \param[in]
 *      nBright         设定背光亮度的值，1~100
 */
void dsm_config::SetBrightness( int nBright )
{
    dsm_config::setAttribute(ATTR_BRIGHTNESS, nBright);
}

/**
 *      获取区域标识
 *
 * \return
 *      int             返回区域标识，如简体中文是2052
 */
int dsm_config::GetLCID()
{
    return _wtoi(dsm_config::getAttribute(getInstance().m_pXml, ATTR_LOCALID));
}

/**
 *      设置语言标识
 *
 *  \param[in]
 *      nLocalID        区域标识，32位整数
 */
void dsm_config::SetLCID( int nLocalID )
{
    dsm_config::setAttribute(ATTR_LOCALID, nLocalID);
}


/**
 *      获取背光时间
 */
int dsm_config::GetBrightTime()
{
    return _wtoi(dsm_config::getAttribute(getInstance().m_pXml, ATTR_BRIGHTTIME));
}

/**
 *      设置背光时间
 */
void dsm_config::SetBrightTime(int nTime)
{
    dsm_config::setAttribute(ATTR_BRIGHTTIME, nTime);
}

/**
 *      判断系统是否已经初始化
 *
 *  \return
 *      bool            true表明已经初始化，false表明未初始化
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
 *      设置系统初始化状态
 */
void dsm_config::SetSystemInit(bool bInit)
{
    dsm_config::setAttribute(ATTR_SYSTEMINITED, bInit ? 1: 0);
}

/**
 *      时间是否自动同步
 */
bool dsm_config::GetTimeAutoSync()
{
    return _wtoi(dsm_config::getAttribute(getInstance().m_pXml, ATTR_TIMESYNC)) ? true : false;
}

/**
 *      设置时间同步方式
 *
 *  \param[in]
 *      bAuto       true    -   自动同步
 *                  false   -   手动同步
 */
void dsm_config::SetTimeSync(bool bAuto)
{
    dsm_config::setAttribute(ATTR_TIMESYNC, bAuto ? 1: 0);
}


/**
 *      设置密码
 */
void dsm_config::SetPassword( CString csPassword )
{
    dsm_config::setAttribute(ATTR_PASSWORD, csPassword);
}

/**
 *      验证密码
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
    // 获取备份配置文件路径
    CString csBackup;
    csBackup.Format(L"%s%s%s", DSM_GetAppDir(), DEFAULT_CFG_BACKUP, DEFAULT_CFG_FILE);

    // 拷贝备份的配置文件
    if(!CopyFile(csBackup, dsm_config::getInstance().m_csXML, FALSE))
    {
        return false;
    }

    if(!dsm_config::getInstance().m_pXml)
    {
        dsm_config::getInstance().m_pXml = new CMarkup;
    }

    // 重新加载配置文件
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
 *  定义DLL需要到导出的函数指针
 */
typedef BOOL (*PepcGetSoftWareVersion)(DWORD *pdwSoftWareVer);

typedef BOOL (*PepcGetHardWareVersion)(DWORD *pdwHardWareVer);


/**
 *      加载系统硬件信息
 */
bool dsm_config::loadHardwareInfo()
{
    // 加载获取硬件版本信息的dll
    HINSTANCE hDll = LoadLibrary(L"epcSysInfo.dll");
    if(!hDll)
    {
        RETAILMSG(true, (L"dsm: failed to load epcSysInfo.dll\n"));
        return false;
    }

    DWORD dwSys = 0, dwHard = 0;

    // 导出获取系统版本的函数
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

    // 导出获取硬件版本的函数
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

