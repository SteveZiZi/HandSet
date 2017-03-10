/**
 * \file
 *      SMVConfigDlg.h
 *
 * \brief
 *      ����ҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/22
 */

#pragma once

#include "src/main/smv/smvbasedlg.h"
#include "src/main/control/menuex.h"

#include "src/utils/ReportControl/ListCtrlEL/ListCtrlEL.h"


class CSmvRecvChannelInfo;



// CSMVConfigDlg �Ի���

class CSMVConfigDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVConfigDlg)

public:
	CSMVConfigDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSMVConfigDlg();

// �Ի�������
	enum { IDD = IDD_SMV_CONFIG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnInitDialog();
#if SU_FEI
    afx_msg void OnMenuChanged();
#else	
	afx_msg void OnMenuF3();
	afx_msg void OnMenuF6();
#endif
    afx_msg void OnMenuF1();
    afx_msg void OnMenuF2();
    afx_msg void OnItemSelectChangedBase(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnItemSelectChangedChannel(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnItemValueChangedChannel(NMHDR* pNotifyStruct, LRESULT* pResult);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    afx_msg void OnInplaceEditAction(NMHDR * pNotifyStruct, LRESULT* pResult);
	

protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
    /**
     *      �޸Ĳ˵�HMENU
     */
#if SU_FEI
    virtual void _modifyMenu() {}
#else
	virtual void _modifyMenu();
#endif

   /**
     *      ��ֹ���ؿ��ƿ鵽F1�˵���
     */
    void _loadCtrlBlk() {}

    /**
     *      ��ʼ����ǰ���ƿ�
     */
    virtual void _initCurrentCtrlBlk() {}

private:

    /**
     *      �������˵�
     */
#if SU_FEI
	void _createLeftMenu();
#endif
    /**
     *      ��������
     *
     *  \param[in]
     *      report          ����������
     *  \param[in]
     *      nID             �����������ID
     */
    void _createReport(CReportH& report, UINT nID);

    /**
     *      ��ʼ����������
     */
    void _initBaseReport();

    /**
     *      ��ʼ��ͨ������
     */
    void _initChannelReport();

    /**
     *      �����������
     */
    void _enterBase();

    /**
     *      ����ͨ������
     */
    void _enterChannel();

    /**
     *      ���ݵ�ǰ���ƿ����smv�������ñ�������
     */
    void _updateBase();

    /**
     *      ���ݵ�ǰ���ƿ����smvͨ����������
     */
    void _updateChannel();

    /**
     *     �жϱ���ؼ��Ƿ�����������
     */
    bool _allowKillFocus();

    /**
     *      �޸�ͨ����������
     */
    void _changedChannelType(ELNM_INPLACELIST_ARGS* pNotifyStruct, LRESULT* pResult);

    /**
     *      �޸�ͨ���������
     */
    void _changedChannelPhase(ELNM_INPLACELIST_ARGS* pNotifyStruct, LRESULT* pResult);

    /**
     *      ���¿��ƿ����
     */
    void _updateCtrlBlkCaption();

    /**
     *      ���µײ��˵���ʾ
     */

    void _updateMenu();

    /**
     *      ����F2�˵�����
     */

    void _updateMenuF2();

    /**
     *      ����ͨ����ģ��
     *
     *  \param[in]
     *      nMenuID     ����ID
     *  \return
     *      bool        ���ĳɹ�����true��ʧ�ܷ���false
     */
    bool _changeTemplate(UINT nMenuID);


private:

    /**
     *      ��ǰ���ÿ��ƿ��ģʽ
     */
    enum MODE_PAGE
    {
        PAGE_NORMAL,            ///< ��ͨͨ������ģʽ
        PAGE_DUALPHASE          ///< ����ͨ������ģʽ
    };

    /**
     *      ��ǰ��ʾ���ƿ�ı�ʶ
     */
    enum MODE_CTRL
    {
        CTRL_ONE,           ///< ��һ�����ƿ�
        CTRL_TWO            ///< �ڶ������ƿ�
    };

#if !SU_FEI
	enum SMV_MODE_SETTING
	{
		SETTING_BASIC,		///< ��������
		SETTING_CHANNEL		///< ͨ������
	};
#endif

private:

#if SU_FEI
    CMenuEx             m_menuNavi;             ///< ��ർ���˵�
#endif
#if !SU_FEI
	SMV_MODE_SETTING	m_modeSetting;			///< ��ǰ��ʾ����ģʽ
#endif

    CReportH*           m_pCurrentReport;       ///< ��ǰ��ʾ�ı���

    CReportH            m_reportBase;           ///< �������ñ���
    CReportH            m_reportChannel;        ///< ͨ�����ñ���
    CReportH            m_reportChannel2;       ///< ����ҳ��ĵڶ�������ͨ�����ñ���

    MODE_PAGE           m_modePage;             ///< ��ǰ���ÿ��ƿ��ģʽ
    MODE_CTRL           m_modeCtrl;             ///< ��ǰ��ʾ�Ŀ��ƿ��ʶ

    CBaseDetectItem*    m_pCurCtrlBlk;          ///< ��ǰ���ƿ�    m_pCurrentCtrlBlk�����ƽ���m_pCurCtrlBlk
    CBaseDetectItem*    m_pCtrlBlk1;            ///< smv1���ƿ�
    CBaseDetectItem*    m_pCtrlBlk2;            ///< smv2���ƿ�

    CNaviMenu*          m_pF6SubTemplate;       ///< ģ���Ӳ˵�
    UINT                m_nBeginID;             ///< �Ӳ˵�������ʼID
    UINT                m_nEndID;               ///< �Ӳ˵��������ID

    typedef std::map<UINT, enumSmvRecvConfigTemplateType>   mapTemplate;
    typedef std::map<UINT, UINT> mapTemplateCap;

    mapTemplate         m_mapTemplate;          ///< �˵�ID��ģ��IDӳ���
    mapTemplateCap      m_mapTemplateCaption;   ///< ģ����ӳ���
};
