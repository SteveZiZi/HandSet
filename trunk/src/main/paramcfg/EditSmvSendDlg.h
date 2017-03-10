/// @file
///
/// @brief
///     �༭SMV���Ϳ��ƿ�Ի��� �����ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.5.28
///
/// �޸���ʷ��
///

#pragma once
#include "src/main/dlg/BaseDlg.h"
#include "src/service/datalayer/include/datatypes.h"

// CEditSmvSendDlg �Ի���


class CBaseCb;

///
/// @brief
///     ��ʾ�ı�������
///
enum ESSShowReportType
{
    ESSShowCtrlBlockReport = 0,
    ESSShowChannelReport,
};


enum ESSStatusWordType
{
    ESSStatusWordUnknown = 0,                      ///< δ֪
    ESSStatusWord1,                                ///< ״̬��1
    ESSStatusWord2,                                ///< ״̬��2
};

///
/// @brief
///    ״̬����״̬�б� ��չ����
///
class CEditSmvSendDataExtra :public IPageDataExtra
{

public:
    CEditSmvSendDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_nFocusRowIndex(-1)
        , m_nFocusColIndex(-1)
        , m_pBaseCb(NULL)
        , m_eStatusWordType(ESSStatusWordUnknown)
        , m_eShowReportType(ESSShowCtrlBlockReport)
    {

    }
    virtual ~CEditSmvSendDataExtra()
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
    ESSShowReportType       m_eShowReportType;          ///< ��ʾ�ı�������
    int                     m_nFocusRowIndex;           ///< ���ڻָ�ѡ�е���
    int                     m_nFocusColIndex;           ///< ���ڻָ�ѡ�е���
    CBaseCb*                m_pBaseCb;                  ///< SMV���Ϳ��ƿ�
    ESSStatusWordType       m_eStatusWordType;          ///< ״̬������
    CPageData*              m_pPageData;                ///< ������ҳ������ָ��

};




///
/// @brief
///     �༭SMV���Ϳ��ƿ�Ի���
///
class CEditSmvSendDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CEditSmvSendDlg)

public:
	CEditSmvSendDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CEditSmvSendDlg();

// �Ի�������
    enum { IDD = IDD_EDIT_SMV_SEND_DLG };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual BOOL DestroyWindow();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    afx_msg void OnMenuEditSmvCtrlF1();
#if !SU_FEI
	afx_msg void OnMenuEditSmvCtrlF2();
#endif
    afx_msg void OnMenuEditSmvCtrlF6();
    afx_msg void OnSmvCtrlItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnSmvCtrlItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnSmvCtrlInplaceButtonClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);

    afx_msg void OnNaviMenuEditSmvChan(UINT uID);
    afx_msg void OnSmvChanItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnSmvChanItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

#if !CHUN_HUA
	afx_msg void OnPopUpMenu(UINT uID);	

#endif

public:

    ///
    /// @brief
    ///      ����ҳ�����
    ///
    /// @return
    ///      void    
    ///
    void _SavePage();
protected:
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
    ///      �������ƿ鱨��
    ///
    /// @return
    ///      void    
    ///
    void _createCtrlBlockReport();

    ///
    /// @brief
    ///      ��ʼ�����ƿ鱨�� 9-1
    ///
    /// @return
    ///      void    
    ///
    void _initCtrlReportWithIEC6185091();

    ///
    /// @brief
    ///      ��ʼ�����ƿ鱨�� 9-2
    ///
    /// @return
    ///      void    
    ///
    void _initCtrlReportWithIEC6185092();

    ///
    /// @brief
    ///      ��ʼ�����ƿ鱨�� ft3
    ///
    /// @return
    ///      void    
    ///
    void _initCtrlReportWithIEC60044FT3();

    ///
    /// @brief
    ///      ��ʼ�����ƿ鱨�� ft3��չ
    ///
    /// @return
    ///      void    
    ///
    void _initCtrlReportWithIEC60044FT3Ext();

    ///
    /// @brief
    ///      ����SMVͨ����
    ///
    /// @return
    ///      void    
    ///
    void _updateCtrlBlockChanCount();

    ///
    /// @brief
    ///      ��ʾ���ƿ鱨��
    ///
    /// @return
    ///      void    
    ///
    void _showCtrlBlockReport();

    ///
    /// @brief
    ///      ����F6
    ///
    /// @return
    ///      void    
    ///
    void _updateSmvCtrlMenuF6();

    ///
    /// @brief
    ///      ����ͨ������
    ///
    /// @return
    ///      void    
    ///
    void _createChannelReport();
    
    ///
    /// @brief
    ///      ����ͨ����Ϣ��ֻ��9-2 ͨ���������޸ģ�
    ///
    /// @return
    ///      void    
    ///
    void _updateChanReport();

    ///
    /// @brief
    ///      ��ʾͨ������
    ///
    /// @return
    ///      void    
    ///
    void _showChannelReport();

    ///
    /// @brief
    ///      ����smvͨ�� ��������<- -> ��������
    ///
    /// @return
    ///      void    
    ///
    void _updateSmvChanMenuF4();

    ///
    /// @brief
    ///      �����ò˵�
    ///
    /// @return
    ///      void    
    ///
    void _resetMenu();

#if !CHUN_HUA
	///
	/// @brief
	///      �����Ҽ��˵�
	///
	/// @return
	///      void    
	///
	void _showContexMenu();
	///
	/// @brief
	///      �����Ҽ��˵�
	///
	/// @return
	///      void    
	///
	void _updateContexMenu(int col);
	///
	/// @brief
	///      ������λ��ʾ
	///
	/// @return
	///      void    
	///
	void _setPhase( CChannel* &pChannel, int nRowIndex, int nIndex ) ;
	///
	/// @brief
	///      �Զ�ӳ��
	///
	/// @return
	///      void    
	///
	void _setAutoMap();
	///
	/// @brief
	///      ӳ�䵽��һ��
	///
	/// @return
	///      void    
	///
	void _setMapGroup1();
	///
	/// @brief
	///      ӳ�䵽�ڶ���
	///
	/// @return
	///      void    
	///
	void _setMapGroup2();
	///
	/// @brief
	///      ȫ����
	///
	/// @return
	///      void    
	///
	void _setTypeAllCurrent();
	///
	/// @brief
	///      ȫ��ѹ 
	///
	/// @return
	///      void    
	///
	void _setTypeAllVolate();
	///
	/// @brief
	///      ȫ��ѹ 
	///
	/// @return
	///      void    
	///
	void setType(CChannel* pChannel, enumChannelType nChanType, int rowIndex);

	CNaviMenu*				m_pPopupMenu;			///<�Ҽ��˵�
#endif

public:

    CReportV                m_ctlChannelReport;                                 ///< SMV����ͨ������
    CReportH                m_ctlCtrlBlockReport;                               ///< SMV���Ϳ��ƿ鱨��
    ESSShowReportType       m_eShowReportType;                                  ///< ��ʾ��������
    CBaseCb*                m_pBaseCb;                                          ///< SMV���Ϳ��ƿ�
    enumFrameType           m_nSmvSendType;                                     ///< SMV��������
};
