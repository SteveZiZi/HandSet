#pragma once

#include "src/main/resource.h"
#include "src/main/mgr/navibarmgr.h"
#include "src/main/common.h"
//#include "src/main/resourcestr.h"
//#include "src/main/resourcebmp.h"
//#include "src/main/mgr/statusmgr.h"
//#include "src/main/mgr/navibarmgr.h"
//#include "src/utils/utility/dsm_utility.h"

// CStateSwitchStatusConfirmDlg �Ի���

class CStateSwitchStatusConfirmDlg : public CDialog
{
	DECLARE_DYNAMIC(CStateSwitchStatusConfirmDlg)

public:
	CStateSwitchStatusConfirmDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStateSwitchStatusConfirmDlg();

// �Ի�������
	enum { IDD = IDD_STATE_SWITCH_STATUS_CONFIRM_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
    virtual void OnOK();
    virtual void OnCancel();

public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnPaint();

protected:

    UINT                    m_nMenuID;            ///< �˵�ID
    CWnd*                   m_pWndPrev;          ///< ֮ǰtarget
    CNaviMenu*              m_pMenuPrev;         ///< m_pWndPrev������menu
};
