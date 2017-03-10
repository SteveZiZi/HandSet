/**
 * \file
 *      dsm_config.h
 *
 * \brief
 *      dsm配置信息管理器头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 *  \note
 *      config.xml应遵循一定的格式
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


// xml属性节点名
//////////////////////////////////////////////////////////////////////////
#define ATTR_VALUE                  L"val"                  ///< 属性值名
#define ATTR_BRIGHTNESS             L"brightness"           ///< 背光亮度
#define ATTR_BRIGHTTIME             L"brighttime"           ///< 背光时间
#define ATTR_LOCALID                L"localid"              ///< 区域标识
#define ATTR_SCREENWIDTH            L"screenwidth"          ///< 屏幕宽度（像素）
#define ATTR_SCREENHEIGHT           L"screenheight"         ///< 屏幕高度（像素）
#define ATTR_SYSTEMINITED           L"systeminited"         ///< 系统初始化标志
#define ATTR_TIMESYNC               L"timesync"             ///< 时间同步方式
#define ATTR_PASSWORD               L"password"             ///< 密码


#define ATTR_PRODUCT_NAME           L"product"              ///< 获取产品名称
#define ATTR_SOFTWARE_VER           L"softwarever"          ///< 获取软件版本
#define ATTR_SOFTWARE_BUILD         L"softbuild"            ///< 获取软件版本的build号
#define ATTR_COMPANY                L"company"              ///< 获取公司信息
#define ATTR_ADDRESS                L"address"              ///< 公司地址
#define ATTR_TELEPHONE              L"telephone"            ///< 公司电话




/**
 *      dsm配置管理器类
 *
 *  \note
 *      提供配置信息的存取，以及指定配置信息的备份与导入
 */
class dsm_config
{
public:
    /**
     *      加载配置文件
     */
    static bool Load();

    /**
     *      保存当前内存的xml到文件
     */
    static bool Save();

    /**
     *      重设各个属性
     */
    static bool Reset();

public:
    /**
     *      获取背光亮度
     */
    static int GetBrightness();

    /**
     *      设置背光亮度
     */
    static void SetBrightness(int nBright);

    /**
     *      获取背光时间
     */
    static int GetBrightTime();

    /**
     *      设置背光时间
     */
    static void SetBrightTime(int nTime);

    /**
     *      获取区域标识
     */
    static int GetLCID();

    /**
     *      设置语言标识
     */
    static void SetLCID(int nLocalID);

    /**
     *      获取屏幕宽度像素值
     */
    static int GetScreenWidth()
    {
        return getInstance().m_nWidth;
    }

    /**
     *      获取屏幕高度像素值
     */
    static int GetScreenHeight()
    {
        return getInstance().m_nHeight;
    }

    /**
     *      判断系统是否已经初始化
     *
     *  \return
     *      bool            true表明已经初始化，false表明未初始化
     */
    static bool GetSystemInit();

    /**
     *      设置系统初始化状态
     */
    static void SetSystemInit(bool bInit);

    /**
     *      设置密码
     *
     *  \param[in]
     *      csPassword      新密码
     */
    static void SetPassword(CString csPassword);

    /**
     *      验证密码
     *
     *  \param[in]
     *      csPassword          待验证的密码
     *  \bool
     *      bool                密码正确返回true，不正确返回false
     */
    static bool VerifyPasswrod(CString csPassword);

    /**
     *      时间是否自动同步
     */
    static bool GetTimeAutoSync();

    /**
     *      设置时间同步方式
     *
     *  \param[in]
     *      bAuto       true    -   自动同步
     *                  false   -   手动同步
     */
    static void SetTimeSync(bool bAuto);

//////////////////////////////////////////////////////////////////////////
// 关于界面的信息

    /**
     *      获取产品名称
     */
    static CString GetProductName()
    {
        return getInstance().m_csProduct;
    }

    /**
     *      获取硬件版本
     */
    static CString GetHardwareVersion()
    {
        return getInstance().m_csHardVer;
    }

    /**
     *      获取软件版本
     */
    static CString GetSoftwareVersion()
    {
        return getInstance().m_csSoftVer;
    }

    /**
     *      获取软件版本的build号
     */
    static CString GetSoftwareBuild()
    {
        return getInstance().m_csSoftBuildVer;
    }

    /**
     *      获取系统版本号
     */
    static CString GetSystemVersion()
    {
        return getInstance().m_csSysVer;
    }

    /**
     *      获取公司名称
     */
    static CString GetCompanyName()
    {
        return getInstance().m_csCompanyName;
    }

    /**
     *      获取公司地址
     */
    static CString GetCompanyAddress()
    {
        return getInstance().m_csCompanyAddress;
    }

    /**
     *      获取技术支持电话
     */
    static CString GetTelephoneSupport()
    {
        return getInstance().m_csTelephone;
    }

private:
    dsm_config();

    ~dsm_config();

    /**
     *      获取静态实例对象
     */
    static dsm_config& getInstance();

    /**
     *      获取指定节点值（字符串）
     */
    static CString getAttribute(CMarkup* pXML, const CString& rcsAttrName, CString csDefValue = L"");

    /**
     *      设置指定节点值
     */
    static bool setAttribute(const CString& rcsAttrName, const CString csValue);

    /**
     *      设置指定节点值
     */
    static bool setAttribute(const CString& rcsAttrName, int nValue);

    /**
     *      加载系统信息
     */
    static bool loadSystemInfo();

    /**
     *      加载系统硬件信息
     */
    static bool loadHardwareInfo();


private:

    bool                m_bModified;            ///< 修改标志位
    CString             m_csXML;                ///< 配置文件路径
    CMarkup*            m_pXml;                 ///< CMarkup指针

    int                 m_nWidth;               ///< 设备宽度
    int                 m_nHeight;              ///< 设备高度
    CString             m_csProduct;            ///< 设备名称
    CString             m_csHardVer;            ///< 硬件版本号
    CString             m_csSysVer;             ///< 系统版本号
    CString             m_csSoftVer;            ///< 软件版本号
    CString             m_csSoftBuildVer;       ///< 软件Build号
    CString             m_csCompanyName;        ///< 公司名称
    CString             m_csCompanyAddress;     ///< 公司地址
    CString             m_csTelephone;          ///< 技术支持电话
};


#endif // DSM_CONFIG_H__


