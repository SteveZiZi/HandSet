/// @file
///
/// @brief
///     �ֶ�ʵ������� ͷ�ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.7.18
///
/// �޸���ʷ��
///


#pragma once

#include "src/main/resource.h"
#include "src/main/dlg/basedlg.h"
#include "src/service/datalayer/DsmDataLayer.h"

enum eMRDShowReportType
{
    MRDSmvReport = 0,
    MRDGooseReport ,
    MRDDIActionReport,
};

///
/// @brief
///      �ֶ�ʵ������� �Ի���
/// 
class CManualResultDetailDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualResultDetailDlg)

public:
	CManualResultDetailDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CManualResultDetailDlg();

// �Ի�������
	enum { IDD = IDD_MANUAL_RESULT_DETAIL_DLG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnNaviMenuResultDetail(UINT nID);
    afx_msg void OnUpdateNaviMenuResultDetail(CCmdUI* pCmdUI);
    afx_msg void OnNaviMenuDetailCbSel( UINT nID );
public:

    ///
    /// @brief
    ///     ����ҳ��
    ///
    /// @return
    ///      void
    ///
    void _SavePage();
protected:
    ///
    /// @brief
    ///      ����SMV���ñ���
    /// 
    /// @return
    ///      void    
    ///
    void _createSmvReport();
    ///
    /// @brief
    ///      ��ʾSmv����
    ///
    /// @return
    ///      void    
    ///
    void _showSmvReport();
    ///
    /// @brief
    ///      ����Smv����
    ///
    /// @return
    ///      void    
    ///
    void _updateSmvReport();

    ///
    /// @brief
    ///      ����GOOSE���ñ���
    /// 
    /// @return
    ///      void    
    ///
    void _createGooseReport();
    ///
    /// @brief
    ///      ��ʾgoose����
    ///
    /// @return
    ///      void    
    ///
    void _showGooseReport();
    ///
    /// @brief
    ///      ����Goose����
    ///
    /// @return
    ///      void    
    ///
    void _updateGooseReport();

    ///
    /// @brief
    ///      ����F2��ѡ�е�goose���ƿ�
    ///
    /// @return
    ///      void    
    ///
    void _highlightGooseF2( BOOL bHighLight = TRUE);
    ///
    /// @brief
    ///      ����DI�ı䱨��
    /// 
    /// @return
    ///      void    
    ///
    void _createDIActionReport();
    ///
    /// @brief
    ///      ��ʾDIAction����
    ///
    /// @return
    ///      void    
    ///
    void _showDIActionReport();
    ///
    /// @brief
    ///      ����DIAction����
    ///
    /// @return
    ///      void
    ///
    void _updateDIActionReport();

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
    ///      ���ؼ��ز˵�ѡ�ʹ�Զ����²˵�
    ///
    /// @return
    ///      void
    ///
    void _loadNaviMenu();
    
    ///
    /// @brief
    ///      ����tips��
    ///
    /// @param
    ///      bAssign   FALSE:Ĭ�ϻ�ȡ���㱨��Ľ���Itemֵ�� TRUE����ʾָ�����ַ��� strTips
    ///
    /// @param
    ///      strTips   ָ�����ַ��� strTips
    ///
    /// @return
    ///      void
    ///
    void _updateTips(BOOL bAssign = FALSE, CString strTips = _T(""));


public:
    typedef std::map<UINT, CGooseCb*>        CUINTGooseCbMap;

    CUINTGooseCbMap         m_gooseCbMap;                 ///< cmdID��Ӧ��goose���ƿ�ӳ���
    eMRDShowReportType      m_eShowReportType;            ///< ��ʾ�ı�������
    CReportV                m_ctlSmvReport;               ///< smv����
    CReportV                m_ctlGooseReport;             ///< goose����
    CReportV                m_ctlDIActionReport;          ///< ���ñ���

    
    BOOL                    m_bShowAllData;               ///< ��ʶ�Ƿ����û�иı������ͨ��

    CGooseCb*               m_pGooseCb;                   ///< ��ǰgoose���ƿ�
    CStateTestResult*       m_pStateTestResult;           ///< ��ǰ״̬���
    CDsmDataLayer*          m_pDataLayer;                 ///< ���ݲ�ָ��

    int                     m_nSmvGseFocusRowIndex;       ///< ��ǰѡ�е��У�������ʾ��ʾ��
    int                     m_nSmvGseFocusColIndex;       ///< ��ǰѡ�е��У�������ʾ��ʾ��
};
