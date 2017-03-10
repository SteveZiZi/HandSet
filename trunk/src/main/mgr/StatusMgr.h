/**
 * \file
 *      StatusMgr.h
 *
 * \brief
 *      状态管理器
 *          获取DSTester的软硬件状态信息
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/3/25
 */
#pragma once

#if !SHENG_ZE
#include "devload.h"
#endif
#include "src/utils/ipc/filemapping.h"
#include "src/include/p_filemapping_def.h"


#define DSM_STATU_CONNECT_USB               1           ///< 接入USB
#define DSM_STATU_DISCONNECT_USB            0           ///< 拔出USB
#define DSM_STATU_CONNECT_SD                1           ///< 接入SD卡
#define DSM_STATU_DISCONNECT_SD             0           ///< 拔出SD卡
#if !SHENG_ZE
#define DSM_STATU_CONNECT_UCARD             1           ///< 接入U盘
#define DSM_STATU_DISCONNECT_UCARD          0           ///< 拔出U盘
#endif

namespace statusmgr
{
    /**
     *      同步模式
     */
    enum SyncMode
    {
        Mode_Err,               ///< 错误
        Mode_UNSYNC,            ///< 未同步
        Mode_1588,              ///< 1588
        Mode_IRIGB,             ///< 正向B码
        Mode_IRIGBN             ///< 反向B码
    };
}


/**
 *      软硬件信息的状态管理器
 */
class CStatusMgr
{
public:

    /**
     *      获取电量值
     *
     *  \return
     *      int             电量值是0~100
     */
    static int GetBattery();

    /**
     *      获取SD卡状态
     *
     *  \return
     *      int         DSM_STATU_CONNECT_SD:       接入SD卡
     *                  DSM_STATU_DISCONNECT_SD:    拔出SD卡
     */
    static int GetSD();

    /**
     *      获取USB状态
     *
     *  \return
     *      int         DSM_STATU_CONNECT_USB:      接入USB
     *                  DSM_STATU_DISCONNECT_USB:   拔出USB
     */
    static int GetUSB();
/**
     *      获取U盘状态
     *
     *  \return
     *      int         DSM_STATU_CONNECT_USB:      接入USB
     *                  DSM_STATU_DISCONNECT_USB:   拔出USB
     */
#if !SHENG_ZE
    /**
     *      设置SD卡状态
     *
     *  \param
     *      int         DSM_STATU_CONNECT_SD:       接入SD卡
     *                  DSM_STATU_DISCONNECT_SD:    拔出SD卡
     */
    static void SetSD(int);
    /**
     *      获取U盘状态
     *
     *  \return
     *      int         DSM_STATU_CONNECT_USB:      接入USB
     *                  DSM_STATU_DISCONNECT_USB:   拔出USB
     */
    static int GetUSBDisk();

    /**
     *      设置U盘状态
     *
     *  \param
     *      int         DSM_STATU_CONNECT_USB:      接入USB
     *                  DSM_STATU_DISCONNECT_USB:   拔出USB
     */
    static void SetUSBDisk(int);
#endif
    /**
     *      获取锁屏状态
     *  \return
     *      bool        true标识锁屏状态，false标识非锁屏状态
     */
    static bool GetScreenLock();

    /**
     *      设置锁屏状态
     */
    static void SetScreenLock(bool bLock);


    /**
     *      获取外置电源接入状态
     *
     *  /return
     *      int         AC_LINE_OFFLINE Offline 
     *                  AC_LINE_ONLINE Online 
     *                  AC_LINE_BACKUP_POWER Backup Power 
     *                  AC_LINE_UNKNOWN Unknown status 
     */
    //static int GetExPower();

    /**
     *      判断是否在充电状态
     */
    static bool GetCharge();

    /**
     *      获取业务窗口标题
     */
    static CString GetTitle();

    /**
     *      设置业务窗口标题
     */
    static void SetTitle(const CString& csTitle);

    /**
     *      显示提示信息
     */
    static void SetTips(const CString& csTips, UINT uTimeOut = 3000, COLORREF color = RGB(0xFF, 0xFF, 0xFF));

    /**
     *      获取提示信息字符串
     */
    static CString GetTips();

    /**
     *      获取提示信息颜色
     */
    static COLORREF GetTipsColor();

    /**
     *      是否显示提示信息
     *
     *  \return
     *      DWORD       0x01                显示提示信息
     *                  0x11                信息闪烁
     */
    static DWORD ShowTipsStyle();

    /**
     *      获取SD卡状态图
     */
    static HBITMAP GetSDBitmap();

    /**
     *      获取USB状态图
     */
    static HBITMAP GetUSBBitmap();
#if !SHENG_ZE
	/**
     *      获取U盘状态图
     */
	static HBITMAP GetUSBDiskBitmap();
#endif

    /**
     *      获取电池状态图
     */
    static HBITMAP GetBatteryBitmap();

    /**
     *      截取当前程序界面
     */
    static void Snapshot();

    /**
     *      清除上一次截图信息
     */
    static void ClearSnapshot();

    /**
     *      保存截图内容到文件
     *
     *  \param[in]
     *      csFile          指定保存的文件名，为空则使用默认文件名
     */
    static bool SaveSnapshot(CString csFile = L"");


    /**
     *      判断是否拥有权限
     *
     *  \return
     *      bool            若已经拥有权限返回true，否则返回false
     */
    static bool IsAuthorized();

    /**
     *      获取访问权限
     *
     *  \param[in]
     *      csPassword      密码
     *  \param[in]
     *      nTime           权限有效时间，默认2分钟
     *  \return
     *      bool            获取权限成功返回true，否则返回false
     */
    static bool GetAuthority(CString csPassword, unsigned int nTimeCycle = 120000);


    /**
     *      设置权限过期
     */
    static void ExpireAuthority();

//////////////////////////////////////////////////////////////////////////
public:

    /**
     *      设置是否允许显示输入模式
     */
    static void SetShowInputMode(bool bShow = true);

    /**
     *      获取是否允许显示输入模式
     */
    static bool GetShowInputMode();


    /**
     *      获得应用程序当前的状态
     */
    static bool GetDSM800APPInfo(DSM800AppInfo* retInfo);

    /**
     *      获取输入状态
     */
    static APP_KBInputMode GetDSM800InputMode();

    /**
     *      获取背光亮度
     */
    static BYTE GetBrightness();

    /**
     *      设置背光亮度
     */
    static void SetBrightness(BYTE byteBrightness);

    /**
     *      获取背光时间
     */
    static DWORD GetBrightTime();

    /**
     *      设置背光时间
     */
    static void SetBrightTime(DWORD dwTime);

    /**
     *      获取同步模式
     */
    static statusmgr::SyncMode GetSyncMode();

    /**
     *      修改系统注册表默认值
     */
    static void FixDefault();




private:

    /**
     *      创建BITMAP信息
     */
    static PBITMAPINFO _createBitmapInfoStruct();

    /**
     *      创建BMP文件
     */
    static bool _createBMPFile(CString csFile, PBITMAPINFO pbi);

    /**
     *      设置默认降低背光
     */
    //static void _setDefaultBKLight(DWORD dwTime = 60);

    /**
     *      修改背光时间的注册表键值
     */
    static void _fixBkLight();


private:
    /**
     *      获取状态管理器的静态实例对象
     */
    static CStatusMgr& getInstance()
    {
        static CStatusMgr s_Instance;
        return s_Instance;
    }

private:
    CStatusMgr(void);
    ~CStatusMgr(void);

private:
    typedef CMap<int, int, int, int>    CResidMap;

    CString                 m_csTitle;              ///< 业务窗口标题
    CResidMap               m_mapBatteryResID;      ///< 电池资源ID映射表

    CString                 m_csTips;               ///< 提示信息
    bool                    m_bTips;                ///< 是否显示提示信息
    UINT                    m_uTimeOut;             ///< 结束显示tips时间
    COLORREF                m_crTips;               ///< 提示信息字体颜色
    DWORD                   m_dwStyle;              ///< 提示信息属性

    static HBITMAP          m_hSnapPanel;           ///< 截图画布

    bool                    m_bLock;                ///< 锁屏状态

    bool                    m_bAuthority;           ///< 权限状态

    unsigned int            m_nTimerAuth;           ///< 权限定时器ID

    HANDLE                  m_hBrightFile;          ///< 亮度设备

    bool                    m_bShowInputMode;       ///< 显示输入模式状态
    static CShareMemory     m_sfileMapping;         ///< 内存映射文件操作辅助对象

    static SYSTEM_POWER_STATUS_EX2  m_sStatus;      ///< 设备系统电源状态
#if !SHENG_ZE
	static int              m_nSDStatu;             ///< SD接入状态
	static int              m_nUCardStatu;          ///< U盘接入状态
#endif
};
