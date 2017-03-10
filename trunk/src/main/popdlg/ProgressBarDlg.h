#pragma once

#include "src/main/resource.h"
#include "src/main/control/statusdisp.h"

class CRNaviMenu;
class CNaviToolBar;

typedef DWORD (WINAPI *DSMProgressFnTask)(WPARAM wParam, LPARAM lParam, void* arg);

typedef DWORD (WINAPI *DSMProgressTask)(void* arg);

// CProgressBarDlg �Ի���

class CProgressBarDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressBarDlg)

public:
	CProgressBarDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProgressBarDlg();

// �Ի�������
	enum { IDD = IDD_POPUP_PROGRESS_BAR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg BOOL OnEraseBkgnd(CDC* /* pDC */) { return TRUE; }
    afx_msg void OnPaint();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);


protected:
    virtual void OnOK() {}
    virtual void OnCancel() {}
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
    inline void _SetTask(DSMProgressTask pTask, LPVOID arg)
    {
        m_pTask = pTask;
        m_pArgs = arg;
    }

    inline void _SetTips(CString csTips)
    {
        m_csTips = csTips;
    }

    inline void _SetTitle(CString csTitle)
    {
        m_csTitle = csTitle;
    }

    inline void _SetPicRC(CRect rc)
    {
        m_rcPic = rc;
    }

    inline CRect _GetPicRC()
    {
        return m_rcPic;
    }

    inline void _SetTextRC(CRect rc)
    {
        m_rcText = rc;
    }

    inline CRect _GetTextRC()
    {
        return m_rcText;
    }

    /**
     *      ���ò˵�
     */
    void _SetMenu(UINT menu)
    {
        m_nMenuID = menu;
    }

    /**
     *      ���ò˵�ID��Χ
     */
    void _SetCommandRange(UINT begin, UINT end, DSMProgressFnTask func, void* data = NULL)
    {
        m_nBegin    = begin;
        m_nEnd      = end;
        m_pFunc     = func;
        m_pData     = data;
    }

private:

    UINT                m_nMenuID;          ///< �˵�ID

    UINT                m_nBegin;
    UINT                m_nEnd;
    DSMProgressFnTask   m_pFunc;            ///< Fn��Ӧ����
    void*               m_pData;

    DSMProgressTask     m_pTask;            ///< �̻߳ص�����

    CRect               m_rcText;           ///< ��ʾ�ַ�������
    CString             m_csTips;           ///< ������ʾ�ַ���

    CString             m_csTitlePrev;      ///< ֮ǰ����
    CString             m_csTitle;          ///< ����������

    LPVOID              m_pArgs;            ///< �̲߳���
    HANDLE              m_hThread;          ///< �����߳�

    CRect               m_rcPic;            ///< ͼƬ����
    CStatusDisp         m_ctlPic;           ///< ͼƬ�ؼ�

    CRNaviMenu*         m_pRNaviMenu;       ///< �˵���Դ�������
    CNaviToolBar*       m_pNaviBar;         ///< �˵�����
};
