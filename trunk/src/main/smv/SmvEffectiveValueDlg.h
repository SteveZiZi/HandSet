#pragma once

#include "src/main/resource.h"
#include "src/main/smv/smvbasedlg.h"
#include "src/service/datalayer/include/datatypes.h"
// CSmvEffectiveValueDlg �Ի���

class CPageDataExtra;


class CSmvEffectiveValueDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSmvEffectiveValueDlg)

public:
	CSmvEffectiveValueDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSmvEffectiveValueDlg();

// �Ի�������
	enum { IDD = IDD_SMV_EFFECTIVE_VALUE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

public:

    /**
     *      ����ҳ������
     */
    void _SavePage();
	virtual void _modifyMenu();
protected:

    /**
     *      ��������
     */
    void _createReport();

    /**
     *      ���豨��
     */
    void _resetReport();
    /**
     *      �����¼������
     */
    void _updateEffectiveValue();
    /**
     *      ��ȡͨ������
     */
    bool _restorePage();
    /**
     *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
     *
     *  /param[in]
     *      nCmdID      ���ƿ�����Ĳ˵�����ID
     */
    bool _switchCtrlBlk( UINT nCmdID );
    /**
     *      ���ͨ���Ƿ����仯
     */
    bool _checkChannel();

private:

    CDataEffectiveVal    m_dataEffVal;
    CReportV             m_ctlEffValReport;                     ///< ��Чֵ����
};
