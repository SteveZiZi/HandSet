#pragma once

#include "src/main/resource.h"
#include "src/utils/sliderboxcontrol/sliderbox.h"

class CRNaviMenu;

// CHelpDlg �Ի���

class CHelpDlg : public CDialog
{
	DECLARE_DYNAMIC(CHelpDlg)

public:
	CHelpDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CHelpDlg();

// �Ի�������
	enum { IDD = IDD_HELP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();

private:
    CSliderBox          m_sliderbox;            ///< ͼƬ����ؼ�
    CRNaviMenu*         m_pRNaviMenu;           ///< �˵�ά������ CNaviMenu
};
