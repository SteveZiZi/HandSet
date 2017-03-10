/// @file
///
/// @brief
///     �������öԻ��� �����ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.5.28
///
/// �޸���ʷ��
///


#pragma once

#include "src/main/dlg/basedlg.h"
#include "src/main/control/menuex.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/service/datalayer/include/datatypes.h"

class CParamConfigBasic;
class CGooseCb;
class CBaseCb;

#define DSM_MSG_PARAM_BASE              0x01        ///< ��������
#define DSM_MSG_PARAM_SMV               0x02        ///< SMV����
#define DSM_MSG_PARAM_GOOSES            0x03        ///< GOOSE����
#define DSM_MSG_PARAM_GOOSER            0x04        ///< GOOSE����

#if !SU_FEI
/************************************************************************/
/* ��������ֱ������                                                   */
/************************************************************************/
#define	DSM_MSG_PARAM_IMPORT_SCL		0x05		///< ����SCL
#define	DSM_MSG_PARAM_TIME_WAY			0x06		///< ��ʱ����
#endif
///
/// @brief
///    ������������
///
class CBasicParamPageDataExtra :public IPageDataExtra
{

public:
    CBasicParamPageDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_nMsgParamType (DSM_MSG_PARAM_BASE)
        , m_nRightReportFocus(0)
        , m_nAppID(-1)
      //  , m_pSmvSendSelectedCb(NULL)
      //  , m_pGooseSendSelectedCb(NULL)
      //  , m_pGooseRecSelectedCb(NULL)

    {

    }
    virtual ~CBasicParamPageDataExtra()
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
    
    int                     m_nMsgParamType;            ///< ������������
    int                     m_nRightReportFocus;        ///< �Ҳ�report����
    int                     m_nAppID;                   ///< ѡ�еĿ��ƿ�APPID
    
  //  CBaseCb*                m_pSmvSendSelectedCb;       ///< SMV����ѡ�еĿ��ƿ�
   // CGooseCb*               m_pGooseSendSelectedCb;     ///< goose����ѡ�еĿ��ƿ�
   // CGooseCb*               m_pGooseRecSelectedCb;      ///< goose����ѡ�еĿ��ƿ�
    CPageData*              m_pPageData;                ///< ������ҳ������ָ��
};


///
/// @brief
///    �������öԻ���
///
class CBasicParamDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamDlg)

public:
	CBasicParamDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBasicParamDlg();

// �Ի�������
	enum { IDD = IDD_BASIC_PARAM_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnDestroy();
#if SU_FEI
    afx_msg void OnMenuChanged();
#endif
	/*OnInplaceButtonClick�����SCL���밴ť����Ϣӳ��*/
    afx_msg void OnInplaceButtonClick(NMHDR* pNotifyStruct, LRESULT* pResult);

    afx_msg void OnItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);
#if SU_FEI
    afx_msg void OnSmvSendOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnSmvSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnSmvSendItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnNaviMenuParamSmvSend(UINT nID);

    afx_msg void OnGooseSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnNaviMenuParamGooseSend(UINT nID);

    afx_msg void OnGooseRecItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnNaviMenuParamGooseRec(UINT nID);
#endif	
	afx_msg LRESULT OnSelSCL(WPARAM, LPARAM);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    

	afx_msg void OnMenuF6();
#if !SU_FEI
	afx_msg void OnMenuF1();
	afx_msg void OnMenuF2();
	afx_msg void OnMenuF3();
	afx_msg void OnMenuF4();
	afx_msg void OnMenuF5();
#endif

public:
    ///
    /// @brief
    ///      ����ҳ������
    ///
    /// @return
    ///      void    
    ///
    virtual void  _SavePage();

	//virtual void _modifyMenu();
private:

    ///
    /// @brief
    ///      �����������ñ���
    ///
    /// @return
    ///      void    
    ///
    void        _createReport(CReportH& report, UINT nID);
#if SU_FEI
    ///
    /// @brief
    ///      ��ʼ����౨��
    ///
    /// @return
    ///      void    
    ///
    void        _initMenuNavi();
#endif
    ///
    /// @brief
    ///      ��ʾ������������
    ///
    /// @return
    ///      void    
    ///
    void        _showBase();

    ///
    /// @brief
    ///      ����������������
    ///
    /// @return
    ///      void    
    ///
    void        _createBaseReport();

#if !SU_FEI
/************************************************************************/
/* ����Ӵ���SCL��Ͷ�ʱ������                                         */
/************************************************************************/
	///
	/// @brief
	///      ��ʾSCL���
	///
	/// @return
	///      void    
	///
	void        _showSCLReport();

	///
	/// @brief
	///      ����SCL����
	///
	/// @return
	///      void    
	///
	void        _createSCLReport();
	///
	/// @brief
	///      ��ʾ��ʱ����
	///
	/// @return
	///      void    
	///
	void        _showTimeReport();

	///
	/// @brief
	///      ������ʱ��������
	///
	/// @return
	///      void    
	///
	void        _createTimeReport();
#endif

#if SU_FEI
    ///
    /// @brief
    ///      ��ʾSMV���Ͳ�������
    ///
    /// @return
    ///      void    
    ///
    void        _showSmvSend();

    ///
    /// @brief
    ///      ����SMV���Ͳ�������
    ///
    /// @return
    ///      void    
    ///
    void        _createSmvSendReport();

    ///
    /// @brief
    ///      ���콻ֱ������ʾ
    ///
    /// @return
    ///      void    
    ///
    CString  _getACDCFormatString();
    
    ///
    /// @brief
    ///      ����SMV���Ͳ�������
    ///
    /// @return
    ///      void    
    ///
    void        _refreshSmvReportWithIEC6185091();

    ///
    /// @brief
    ///      ����SMV���Ͳ�������
    ///
    /// @return
    ///      void    
    ///
    void        _refreshSmvReportWithIEC6185092();

    ///
    /// @brief
    ///      ����SMV���Ͳ�������
    ///
    /// @return
    ///      void    
    ///
    void        _refreshSmvReportWithIEC60044FT3();

    ///
    /// @brief
    ///      ����SMV���Ͳ�������
    ///
    /// @return
    ///      void    
    ///
    void        _refreshSmvReportWithIEC60044FT3Ext();

    ///
    /// @brief
    ///      ��ʾGoose���Ͳ�������
    ///
    /// @return
    ///      void    
    ///
    void        _showGooseSend();

    ///
    /// @brief
    ///      ����Goose���Ͳ�������
    ///
    /// @return
    ///      void    
    ///
    void        _createGooseSendReport();

    ///
    /// @brief
    ///      ��ʾGoose���ղ�������
    ///
    /// @return
    ///      void    
    ///
    void        _showGooseRec();

    ///
    /// @brief
    ///      ����Goose���ղ�������
    ///
    /// @return
    ///      void    
    ///
    void        _createGooseRecReport();

    ///
    /// @brief
    ///      ��װҳ��
    ///
    /// @return
    ///      void    
    ///
    void       _restorePage();

    ///
    /// @brief
    ///      ˢ�²˵�
    ///
    /// @return
    ///      void    
    ///
    void       _refreshMenu();
#else
  ///
    /// @brief
    ///      ��װҳ��
    ///
    /// @return
    ///      void    
    ///
    void       _restorePage();

#endif

private:
#if SU_FEI
    CMenuEx                 m_menuNavi;             ///< ���˵�����
    CReportH                m_ctlSmvSendReport;     ///< Smv����
    CReportH                m_ctlGooseSendReport;   ///< goose����
    CReportH                m_ctlGooseRecReport;    ///< goose����
#endif
	CReportH                m_ctlBaseReport;        ///< �����������ñ���

    CReportH*               m_pCurReport;           ///< ��ǰ��ʾ����

    CParamConfigBasic*      m_pCfg;                 ///< �������ýӿ�


#if !SU_FEI

	/************************************************************************/
	/* ����ӵĵ���scl�Ͷ�ʱ������                                          */
	/************************************************************************/
	CReportH				m_ctlImportSCLReport;	///< ����SCL
	CReportH				m_ctlTimeReport;		///< ��ʱ��������
#endif

#if !SHENG_ZE
	enum SclModel
	{
		SclFromSd,
		SclFromU,
	};
	SclModel				m_sclModel;
#endif
};
