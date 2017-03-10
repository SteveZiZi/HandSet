/**
 * \file
 *      EventProc.h
 *
 * \brief
 *      页面消息处理器
 *          处理页面间跳转
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/3/21
 *
 *  \note
 *      该类负责处理按键消息，切换窗口，发送更新状态命令
 */


#pragma once

#include "common.h"
#include "src/main/dlg/BaseDlg.h"
#include "src/main/pagedata/ipagedata.h"

#define DSM_HOME_RETVAL_MAIN        0xF001          ///< 返回主画面
#define DSM_HOME_RETVAL_SNAP        0xF002          ///< 截图
#define DSM_HOME_RETVAL_NONE        0xF003          ///< 空操作


#define DSM_KEYEVT_LOCK                 0       ///< 锁键
#define DSM_KEYEVT_POWEROFF             1       ///< 关机

class CBaseDlg;

class CEventProc
{
public:

//////////////////////////////////////////////////////////////////////////
// 处理按键消息

    /**
     *      处理ESC消息
     */
    void OnKeyEsc();

    /**
     *      处理关机按键
     *
     *  \param[in]
     *      dwStatu         DSM_KEYEVT_LOCK:        锁键
     *                      DSM_KEYEVT_POWEROFF:    关机
     */
    void OnKeyPower(DWORD dwStatu);

    /**
     *      处理home键
     */
    UINT OnKeyHome(bool bModal = false);

    /**
     *      截图保存
     */
    void SnapShot();



//////////////////////////////////////////////////////////////////////////
// 
public:

    /**
     *      返回当前页面句柄
     */
    HWND GetCurrentPageHwnd();

    /**
     *      显示指定页面
     *
     *  \param[in]
     *      nPageID         页面ID，使用对话框模板ID
     *  \note
     *      页面进入退出应该有一个信息
     *      以便新来的页面可以获知
     */
    void ShowPage(UINT nPageID);

    /**
     *      显示MAIN画面
     */
    void ShowMain();

    /**
     *      显示向导画面
     */
    void ShowWizard();

    /**
     *      恢复出厂设置
     */
    void ResetFactory();

    /**
     *      退出程序前关闭窗口释放资源
     */
    void Exit();

    /**
     *      关闭程序
     */
    void Close();

    /**
     *      显示密码框
     */
    bool ShowPassword();

    /**
     *      保存配置文件
     */
    void SaveConfig();


    /**
     *      获取静态对象指针
     */
    static CEventProc* GetInstance()
    {
        static CEventProc s_Instance;
        return &s_Instance;
    }


//////////////////////////////////////////////////////////////////////////
// 处理页面数据

    /**
     *      获取当前页面数据
     */
    CPageData* GetCurrentPageData();

    /**
     *      获取前一页面数据
     */
    CPageData* GetPreviousPageData();


//////////////////////////////////////////////////////////////////////////


private:
    CEventProc(void);
    ~CEventProc(void);


private:

    /**
     *      前进打开页面
     */
    void pageForward(UINT nPageID);

    /**
     *      返回打开页面
     */
    void pageBack();

    /**
     *      打开页面
     */
    void pageShow(UINT nPageID, DsmPageAction eAction = PageEnter);

    /**
     *      更新页面栈
     */
    void updatePageStack(UINT nPageID, DsmPageAction eAction);

    /**
     *      回到主页面
     */
    void processPageMain();

    /**
     *      返回上一层页面
     */
    void processPageBack();

    /**
     *      查询指定页面的数据
     */
    CPageData* lookupPageDataByID(UINT nPageID);

    /**
     *      创建页面
     */
    CBaseDlg* pageCreate(UINT nPageID);

    /**
     *      判断页面是否是smv或者goose业务页面
     */
    bool isBusinessPage(UINT nPageID);

    /**
     *      保存当前窗口焦点
     */
    void saveFocusHwnd(HWND hWnd);

    /**
     *      恢复当前窗口焦点
     */
    void restoreFocusHwnd();

    /**
     *      清除页面栈数据
     */
    void clearPageStack();

    /**
     *      清理业务页面数据的映射表
     */
    void clearMapBussData();

    /**
     *      清理资源
     */
    void clearSource();


private:
    typedef CList<HWND, HWND>                   CStackHWND;
    typedef CList<CPageData*>                   CStackData;

    typedef CMap<UINT, UINT, CPageData*, CPageData*>    CMapBussData;

    CBaseDlg*           m_pCurPage;             ///< 当前的页面句柄
    CStackHWND          m_stackAncestor;        ///< 当前焦点窗口的祖宗
    CStackData          m_stackData;            ///< 页面关联数据
    CMapBussData        m_mapBussData;          ///< 保存业务页面数据的映射表
};
