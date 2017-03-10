/// @file
///
/// @brief
///     ����״̬�� ͷ�ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.7.18
///
/// �޸���ʷ��
///

#pragma once


#include "src/main/dlg/BaseDlg.h"
#include "src/service/datalayer/dsmdatalayer.h"

enum MSSStatusWordType
{
    MSSWStatusWordUnknown = 0,                      ///< δ֪
    MSSWStatusWord1,                                ///< ״̬��1
    MSSWStatusWord2,                                ///< ״̬��2
};

///
/// @brief
///    ״̬����״̬�б� ��չ����
///
class CManualSmvStatusWordDataExtra :public IPageDataExtra
{

public:
    CManualSmvStatusWordDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_nFocusRowIndex(-1)
        , m_pBaseCb(NULL)
        , m_eStatusWordType(MSSWStatusWordUnknown)
    {

    }
    virtual ~CManualSmvStatusWordDataExtra()
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
    CBaseCb*                m_pBaseCb;                  ///< SMV���Ϳ��ƿ�
    MSSStatusWordType       m_eStatusWordType;          ///< ״̬������
    CPageData*              m_pPageData;                ///< ������ҳ������ָ��

};


///
/// @brief
///      SMV״̬������
///
class CManualSmvStatusWordDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualSmvStatusWordDlg)

public:
	CManualSmvStatusWordDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CManualSmvStatusWordDlg();

// �Ի�������
	enum { IDD = IDD_MANUAL_SMV_STATUS_WORD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()


public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNaviMenuSmvStatusWord(UINT nID);
    afx_msg void OnSmvStatusWordValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNaviMenuCtrlSel(UINT nID);
public:
    ///
    /// @brief
    ///      ����ҳ������
    ///
    /// @return
    ///      void    
    ///
    virtual void _SavePage();

protected:
    ///
    /// @brief
    ///      ��������
    ///
    /// @return
    ///      void    
    ///
    void _createReport();

    ///
    /// @brief
    ///      ���¼���ҳ������
    ///
    /// @return
    ///      void    
    ///
    virtual void _restorePage();
	///
	/// @brief
	///      ���¼��ز˵���
	///
	/// @return
	///      void    
	///
	void _resetMenu();

	///
	/// @brief
	///      ���¼��ز˵���
	///
	/// @return
	///      void    
	///
	void _updateReport();
protected:

    CReportH                m_ctlStatusWordReport;         ///< ״̬��
    CBaseCb*                m_pBaseCb;
    CParamConfigSMVSend*    m_pParamConfigSMVSend;
    enumFrameType           m_nSmvSendType;                ///< SMV���͵�����
	typedef std::map<UINT, CBaseCb*>        CUINTCtrlCbMap;
	CUINTCtrlCbMap          m_ctrlCbMap;           ///< cmdID��Ӧ��goose���ƿ�ӳ���

};
