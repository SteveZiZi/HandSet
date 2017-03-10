/// @file
///
/// @brief
///     ״̬����״̬�б� ͷ�ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.7.18
///
/// �޸���ʷ��
///

#pragma once


#include "src/main/dlg/basedlg.h"
#include "src/main/statesequence/DIDOStateDlg.h"
#include "src/main/popdlg/MsgDlg.h"
// CStateSequenceDlg �Ի���


///
///    ״̬�б���״̬�����ֵ
///
const int  g_nSSStateSequenceCountMax = 10;


class CStatus;

///
/// @brief
///    ״̬����״̬�б� ��չ����
///
class CStateSequenceDataExtra :public IPageDataExtra
{

public:
    CStateSequenceDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_nFocusRowIndex(-1)
        , m_pStatus(NULL)
    {

    }
    virtual ~CStateSequenceDataExtra()
    {

    }

public:

    ///
    /// @brief
    ///      ��ȡ����������󶨵�����ҳ������ָ��
    ///
    /// @return
    ///      CPageData*    
    ///
    virtual CPageData* GetPageData()
    {
        return m_pPageData;
    }
public:

    int                     m_nFocusRowIndex;           ///< ���ڻָ�ѡ�е���
    CStatus*                m_pStatus;                  ///< ״ֵ̬
    CPageData*              m_pPageData;                ///< ������ҳ������ָ��
    
};



///
/// @brief
///    ״̬����״̬�б� ��չ����
///
class CStateSequenceDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateSequenceDlg)

public:
	CStateSequenceDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStateSequenceDlg();

// �Ի�������
	enum { IDD = IDD_STATE_SEQUENCE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    LRESULT OnDIChanged( WPARAM wParam, LPARAM lParam );
    LRESULT OnWhetherSwitch( WPARAM wParam, LPARAM lParam );
    LRESULT OnStatusChanged( WPARAM wParam, LPARAM lParam );
    LRESULT OnTestCompleted( WPARAM wParam, LPARAM lParam );
    afx_msg void OnNaviMenuStateSequence(UINT nID);
    afx_msg void OnUpdateNaviMenuStateSequence(CCmdUI* pCmdUI);
    afx_msg void OnStateOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
protected:
    ///
    /// @brief
    ///      ����״̬�б���
    ///
    /// @return
    ///      void    
    ///
    void _createReport();
    ///
    /// @brief
    ///      ����״̬�б���
    ///
    /// @return
    ///      void    
    ///
    void _updateReport();
    ///
    /// @brief
    ///      ���״̬
    ///
    /// @return
    ///      void    
    ///
    void _addStatus();
	///
	/// @brief
	///      �༭״̬
	///
	/// @return
	///      void    
	///
	void _editStatus();
	///
	/// @brief
	///      ɾ��״̬
	///
	/// @return
	///      void    
	///
	void _deleteStatus();
    ///
    /// @brief
    ///      ���״̬
    ///
    /// @return
    ///      void    
    ///
    void _clearStatus();
    ///
    /// @brief
    ///      ����״̬����
    ///
    /// @return
    ///      void    
    ///
    void _updateTestIndex();
    ///
    /// @brief
    ///      ���ò˵�
    ///
    /// @return
    ///      void    
    ///
    void _resetMenu();

    ///
    /// @brief
    ///      �ָ�ҳ������
    ///
    /// @return
    ///      void    
    ///
    void _restorePage();

    ///
    /// @brief
    ///      �Ƿ�����ʾ������
    ///
    /// @return
    ///      BOOL    
    ///
    virtual BOOL _isDIDOShowing();

    ///
    /// @brief
    ///      ���ÿ�������ʾ
    ///
    /// @return
    ///      void    
    ///
    virtual void _showDIDO(BOOL bShow);

    ///
    /// @brief
    ///      ���¿�����
    ///
    /// @return
    ///      void    
    ///
    virtual void _updateDOStatus(int nRowIndex);


public:
    ///
    /// @brief
    ///      ����ҳ������
    ///
    /// @return
    ///      void    
    ///
    virtual void  _SavePage();

    ///
    /// @brief
    ///      ���ؼ��ز˵�ѡ�ʹ�Զ����²˵�
    ///
    /// @return
    ///      void
    ///
    void _loadNaviMenu();

public:

    CParamConfigStateSequence*  m_pConfigStateSequ;
    CReportV                    m_ctlStateSequReport;   ///< ״̬�б���
    CDIDOStateDlg*              m_pDIDOStateDlg;        ///< ������
    BOOL                        m_bTesting;             ///< �Ƿ���������
    int                         m_nTestStateIndex;      ///< ��ǰ���������״̬����
};
