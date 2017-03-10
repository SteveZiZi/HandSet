/// @file
///
/// @brief
///     ��ʾ������ ͷ�ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.7.18
///
/// �޸���ʷ��
///

#pragma once

#include "src/utils/ElectricControl/OnOffStateCtrl/OnOffStateCtrl.h"
#include "src/main/resource.h"
#include "src/service/datalayer/dsmdatalayer.h"
// CDIStateDlg �Ի���


enum eDIWinPosition;

///
/// @brief
///    ��ʾ������
///
class CDIStateDlg : public CDialog
{
	DECLARE_DYNAMIC(CDIStateDlg)

public:
	CDIStateDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDIStateDlg();

// �Ի�������
	enum { IDD = IDD_DI_STATE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL DestroyWindow();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    LRESULT OnDIChanged( WPARAM wParam, LPARAM lParam );

protected:
    virtual void OnCancel();
public:

    ///
    /// @brief
    ///      �ƶ���ʾ���ڵ��ײ�
    ///
    /// @return
    ///      void    
    ///
    void MoveWindowBottom();

    ///
    /// @brief
    ///      �ƶ���ʾ���ڵ�����
    ///
    /// @return
    ///      void    
    ///
    void MoveWindowTop();

    ///
    /// @brief
    ///      ����DI״̬
    ///
    /// @return
    ///      void    
    ///
    void UpdateDIStatus();

protected:

    ///
    /// @brief
    ///      ����ID����ؼ�
    ///
    /// @return
    ///      void    
    ///
    void _createDIStateCtl();

protected:

    COnOffStateCtrl   m_ctlOnOffState;           ///< ��ʾ�ؼ�
    eDIWinPosition    m_eWinPosition;            ///< ��ʾλ��
    CBitmap           m_bmpInit;                 ///< DI ״̬Init
    CBitmap           m_bmpOff;                  ///< DI ״̬OFF
    CBitmap           m_bmpOn;                   ///< DI ״̬ON
    CBitmap           m_bmpBad;                  ///< DI ״̬Bad
    CBitmap           m_bmpDisable;              ///< DI ״̬Disable
};
