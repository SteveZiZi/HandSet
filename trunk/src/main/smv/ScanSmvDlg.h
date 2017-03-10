/**
 * \file
 *      ScanSmvDlg.h
 *
 * \brief
 *      smv̽��
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/19
 */

#pragma once

#include "src/main/dlg/BaseDlg.h"
#include "src/service/datalayer/dsmdatalayer.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"



namespace SMVSCAN
{
    enum SMVSCAN_PROTO_TYPE
    {
        PROTO_TYPE_9X,              ///< ̽��9x����
        PROTO_TYPE_FT3,             ///< ̽��FT3����
#if !SU_FEI
		PROTO_TYPE_GOOSE,			///<̽��GOOSE����
#endif
    };
}



// CScanSmvDlg �Ի���

class CScanSmvDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CScanSmvDlg)

public:
	CScanSmvDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CScanSmvDlg();

// �Ի�������
	enum { IDD = IDD_SCAN_SMV_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNaviMenuFn(UINT nID);
#if SU_FEI
    afx_msg void OnMenuSample(UINT nID);
#endif
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnItemChecked9X(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnItemCheckedFT3(NMHDR* pNotifyStruct, LRESULT* pResult);

#if !SU_FEI
	afx_msg void OnItemCheckGoose(NMHDR* pNotifyStruct, LRESULT* pResult);
#endif

public:
    /**
     *      ����ҳ��״̬����
     */
    virtual void _SavePage();

protected:

    /**
     *      �޸Ĳ˵�HMENU
     */
    virtual void _modifyMenu();

private:
    /**
     *      ��������
     */
    void _createReport(CReportV* pReport, UINT nID);

    /**
     *      ��9x����
     */
    void _insertColumn9x();

    /**
     *      ����FT3����
     */
    void _insertColumnFT3();

#if !SU_FEI
	/**
     *      ����goose����
     */
	void _insertColumnGoose();
	/**
     *      ̽��goose
     */
    void _scanGoose();
	
#endif

    /**
     *      ̽��9X
     */
    void _scan9X();

    /**
     *      ̽��FT3
     */
    void _scanFT3();

    /**
     *      ����9X̽����������
     */
    void _insertRecordTo9X();

    /**
     *      ����FT3̽����������
     */
    void _insertRecordToFT3();

#if !SU_FEI
	 /**
     *      ����GOOSE̽����������
     */
    void _insertRecordToGoose();
#endif

    /**
     *      �˳�ʱ�ͷ���Դ
     */
    void _clearRes();

    /**
     *      ������ѡ���ƿ�����
     */
    bool _setSelected();

    /**
     *      ��ձ����ѡ��
     */
    void _clearSelect(CReportV* pReport);

    /**
     *      ���ѡ�п��ƿ��ͨ�����Ƿ��������
     *
     *  \note
     *      92Э��12.8k�Ĳ���ͨ����������30
     */
    bool _checkChannels();

    /**
     *      ���²˵�
     */
    void _updateMenu(SMVSCAN::SMVSCAN_PROTO_TYPE eType);

private:
    typedef CList<CBaseDetectItem*, CBaseDetectItem*>   CListItem;

    SMVSCAN::SMVSCAN_PROTO_TYPE     m_eDetectType;      ///< ���ֵ�ǰ̽��Э��

    UINT                    m_nScanTimerID;     ///< ̽�ⶨʱ��ID

    CString                 m_cs9XCaption;      ///< ̽��9X����
    CString                 m_csFT3Caption;     ///< ̽��FT3����
	
#if !SU_FEI
	CString					m_csGooseCaption;	///<̽��Goose����
#endif


    CReportV                m_ctl9X;            ///< ��ʾ̽��9x��Ϣ����
    CReportV                m_ctlFT3;           ///< ��ʾ̽��FT3��Ϣ����

#if !SU_FEI
	CReportV				m_ctlGOOSE;			///<��ʾ̽��GOOSE��Ϣ��
#endif

    CListItem               m_lstRecords;       ///< ̽�⵽���ƿ��б�

    CListItem               m_lstSelect;        ///< ��ѡ�Ŀ��ƿ��б�

    CDsmDataLayer*          m_pDataLayer;       ///< ���ݲ�ָ��
};
