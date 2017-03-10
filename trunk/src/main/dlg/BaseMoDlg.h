/**
 * \file
 *      BaseMoDlg.h
 *
 * \brief
 *      模态对话框的基类头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2014/1/2
 */


#pragma once

class CRNaviMenu;

#define DSM_BASEMODLG   CBaseMoDlg

class CBaseMoDlg : public CDialog
{
    DECLARE_DYNAMIC(CBaseMoDlg)

public:
    CBaseMoDlg(UINT nIDTemplate, CString csCaption, CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CBaseMoDlg();


protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

public:

    /**
     *      清理窗口关联的资源
     */
    virtual void _ClearResource();

    /**
     *      关闭窗口函数
     */
    virtual void _EndDialog(int nResult);

protected:

    UINT                m_nIDMenu;              ///< 菜单ID
    CString             m_csCaption;            ///< 标题字符串ID
    CRNaviMenu*         m_pRNaviMenu;           ///< 菜单资源管理对象
};
