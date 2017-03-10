#pragma once

#include "src/main/dlg/BaseDlg.h"
#include "src/service/datalayer/DsmDataLayer.h"
#include "DIStateDlg.h"



// CManualSmvGooseSend �Ի���

class CGooseCb;
class CParamConfigGOOSESend;
class CParamconfigManualTest;

enum MSSSmvChanValCalc;
enum MSSModifyGooseValType;
enum MSSSmvFocusColType;

enum MSSShowReportType
{
    SRTShowSmvReport = 0,
    SRTShowGooseReport,
};

class CManualSmvGooseSendDataExtra :public IPageDataExtra
{

public:
    CManualSmvGooseSendDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_uNextPageID(0)
        , m_pPageData(pPageData)
        , m_pGooseCb(NULL)
        , m_nFocusCol(-1)
        , m_nFocusRow(-1)
        , m_eShowReportType(SRTShowSmvReport)
        , m_pVirChannel(NULL)
    {
    }
    virtual ~CManualSmvGooseSendDataExtra()
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
    UINT                    m_uNextPageID;          ///< ������ҳ���ID����ֻ�ڵ���˵� ״̬Ʒ�ʻ�״̬�� ����SMV�����б�ʱ����ʹ�ã�
    DWORD                   m_dwSendStatus;         ///< ���͵�״̬
    MSSShowReportType       m_eShowReportType;      ///< ��ʾ�ĸ���report
    int                     m_nFocusRow;            ///< ѡ����
    int                     m_nFocusCol;            ///< ѡ����
    CGooseCb*               m_pGooseCb;             ///< ��ǰ���ƿ�
    CSMVSendVirtualChannel* m_pVirChannel;          ///< �༭��SMVͨ��
    CPageData*              m_pPageData;            ///< ������ҳ������ָ��
};



class CManualSmvGooseSendDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualSmvGooseSendDlg)

public:
	CManualSmvGooseSendDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CManualSmvGooseSendDlg();

// �Ի�������
	enum { IDD = IDD_MANUAL_SMV_GOOSE_SEND_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnSmvSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnStepOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGooseSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGooseSendItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGooseSendInplaceButtonClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGoosePointItemOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnNaviMenuParamSmvGooseSend(UINT nID);
    afx_msg void OnUpdateNaviMenuParamSmvGooseSend(CCmdUI* pCmdUI);
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg LRESULT OnDIChanged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnManualTestResult(WPARAM wParam, LPARAM lParam);
    afx_msg void OnNaviMenuGooseCtrlSel(UINT nID);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
protected:

    ///
    /// @brief
    ///      ����SMV�������ñ���
    /// 
    /// @return
    ///      void    
    ///
    void _createSmvReport();

    ///
    /// @brief
    ///      ����SMV�������ñ���
    /// 
    /// @return
    ///      void    
    ///
    void _updateSmvReport();

    ///
    /// @brief
    ///      ����GOOSE�������ñ���
    /// 
    /// @return
    ///      void    
    ///
    void _createGooseReport();

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
    ///      ��ʾgoose����
    ///
    /// @return
    ///      void    
    ///
    void _showGooseReport();

    ///
    /// @brief
    ///      ������ʾGooseF3�Ŀ��ƿ�
    ///
    /// @return
    ///      void    
    ///
    void _highlightGooseF3(BOOL bHighLight = TRUE);

    ///
    /// @brief
    ///      ����goose����
    ///
    /// @return
    ///      void    
    ///
    void _updateGooseReport();

    ///
    /// @brief
    ///      ��ͨ����ֵ���ӷ���������
    ///
    /// @return
    ///      void    
    ///
    void _smvAddSub(MSSSmvChanValCalc nScvc);

    ///
    /// @brief
    ///      ��һ���ƿ�
    ///
    /// @return
    ///      void    
    ///
    void _nextCtrlBlock();

    ///
    /// @brief
    ///      ��Ч�޸ĵ�SMV��������
    ///
    /// @return
    ///      void    
    ///
    void _smvModifyEffImed();

    ///
    /// @brief
    ///      ��Ч�޸ĵ�GOOSE��������
    ///
    /// @return
    ///      void    
    ///
    void _gooseModifyEffImed();

    ///
    /// @brief
    ///      �޸�goose״̬����
    ///
    /// @return
    ///      void    
    ///
    void _modifyAllGooseValue(MSSModifyGooseValType mgvModifyGooseValType);
    
    ///
    /// @brief
    ///      ����ҳ������
    ///
    /// @return
    ///      void    
    ///
    virtual void _SavePage();

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
    ///      �޸Ĳ˵�HMENU
    ///
    /// @return
    ///      void    
    ///
    virtual void _resetMenu();
    ///
    /// @brief
    ///      �޸�SMV�˵�F6
    ///
    /// @return
    ///      void    
    ///
    virtual void _updateMenuSmv(MSSSmvFocusColType smvFoucsColType);

    ///
    /// @brief
    ///      �Ƿ�����ʾ������
    ///
    /// @return
    ///      BOOL    
    ///
    virtual BOOL IsDIShowing();

    ///
    /// @brief
    ///      ���ÿ�������ʾ
    ///
    /// @return
    ///      void    
    ///
    virtual void ShowDI(BOOL bShow);
#if !SHENG_ZE
	///
	/// @brief
	///      �Ƕ�ת�������Ƕ�ת��Ϊ��-180~180��Χ��
	///
	/// @return
	///      void    
	///
	double AngleConversion(double Val);
#endif
protected:
    ///
    /// @brief
    ///      ���¼��ز˵�HMENU
    ///
    /// @return
    ///      void    
    ///
    virtual void _reloadNaviMenu(UINT nMenuID);
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
    ///      ����SMV�޸ı�ʶ
    ///
    /// @return
    ///      void
    ///
    void _setSMVModify(BOOL bModify);
    ///
    /// @brief
    ///      ����GOOSE�޸ı�ʶ
    ///
    /// @return
    ///      void
    ///
    void _setGooseModify(BOOL bModify);

    ///
    /// @brief
    ///      �ж�SMV�Ƿ�Ϊ��
    ///
    /// @return
    ///      BOOL
    ///
    BOOL _isSmvEmpty();
    ///
    /// @brief
    ///      �ж�goose�Ƿ�Ϊ��
    ///
    /// @return
    ///      BOOL
    ///
    BOOL _isGooseEmpty();
	///
	void _showContexMenu();

protected:

    typedef std::map<UINT, CGooseCb*>        CUINTGooseCbMap;

    CUINTGooseCbMap         m_gooseCbMap;           ///< cmdID��Ӧ��goose���ƿ�ӳ���
    CReportV                m_ctlSmvReport;         ///< smv����
    CReportV                m_ctlGooseReport;       ///< goose����
    CDIStateDlg*            m_pDIStateDlg;          ///< ������
    
    DWORD                   m_dwSendStatus;         ///< ���͵�״̬
    MSSShowReportType       m_eShowReportType;      ///< ��ʾ�ĸ���report
    CGooseCb*               m_pGooseCb;             ///< ��ǰ���ƿ�
    CParamConfigGOOSESend*  m_pConfigGooseSend;     ///< goose����
    CParamconfigManualTest* m_pConfigManualTest;    ///< �ֶ�����������ù�����
    CDsmDataLayer*          m_pDataLayer;           ///< ���ݲ�ָ��
    
    BOOL                    m_bSmvModify;           ///< ��ʶSMV�Ƿ��޸�
    BOOL                    m_bGooseModify;         ///< ��ʶGOOSE�Ƿ��޸�

	CNaviMenu*				m_pPopupMenu;			///<�Ҽ��˵�
	BOOL					m_bIsEditing;			///�Ƿ����ڱ༭

};
