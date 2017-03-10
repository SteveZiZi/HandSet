#pragma once

#include "src/service/datalayer/dsmdatalayer.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/utils/ElectricControl/PolarityDialCtrl/PolarityDialCtrl.h"

#include "src/main/resource.h"
#include "src/main/smv/smvbasedlg.h"

// CSmvPolarityDlg �Ի���

class CPageDataExtra;



class CSmvPolarityDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSmvPolarityDlg)

public:
	CSmvPolarityDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSmvPolarityDlg();

// �Ի�������
	enum { IDD = IDD_SMV_POLARITY_DLG };
    
protected:
    CDataPolarity      m_dataPolarity;                                          ///< ��������

    int                m_nChannelCount;                                         ///< ��ǰ���ƿ��е�ͨ����
    int                m_nCurPageIndex;                                         ///< ��ǰҳ����

    CPolarityDialCtrl  m_polarityDialCtrl0;                                     ///< ���Կؼ�1
    CPolarityDialCtrl  m_polarityDialCtrl1;                                     ///< ���Կؼ�2
    CPolarityDialCtrl  m_polarityDialCtrl2;                                     ///< ���Կؼ�3
    CPolarityDialCtrl  m_polarityDialCtrl3;                                     ///< ���Կؼ�4

    CBitmap            m_bmpDail;                                               ///< ����ͼƬ
    CBitmap            m_bmpPoint;                                              ///< ָ��
    
public:
    
    /**
     *      ����ҳ������
     */
    void _SavePage();

protected:
    /**
     *      �ָ�ҳ����ʾ
     */
    bool _restorePage();

    /**
     *      ����ͨ��ֵ
     */
    BOOL  _setChannelValue(int nChanIndex, CDataPolarityChannelInfo* pPolarityChanInfo);
    /**
     *     ����ͨ���ؼ�
     */
    void  _updateChannelCtrl();

    /**
     *     ����ͨ��ֵ
     */
    void  _updatePolarityValue();
    /**
     *      ��������
     */
    void  _createReport();

    /**
     *      �޸Ĳ˵�HMENU
     */
    virtual void _modifyMenu();

    /**
     *      ���ͨ�����Ƿ��޸�
     */
    bool _checkChannel();
    /**
     *      ͨ���л�
     */
    bool _switchCtrlBlk(UINT nCmdID);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnMenuF3();
#if SU_FEI
    afx_msg void OnMenuF5();
#else
	afx_msg void OnMenuF4();

#endif
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL DestroyWindow();
};
