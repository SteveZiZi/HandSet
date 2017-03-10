#pragma once

#include "src/main/resource.h"
#include "src/main/common.h"
#include "src/main/resourcestr.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/utils/ElectricControl/HistogramCtrl/HistogramCtrl.h"

#include "src/main/smv/smvbasedlg.h"

// CSmvHarmonicDlg �Ի���


class CPageDataExtra;


enum SHShowReportType;

class CSmvHarmonicDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSmvHarmonicDlg)

public:
	CSmvHarmonicDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSmvHarmonicDlg();

// �Ի�������
	enum { IDD = IDD_SMV_HARMONIC_DLG };
private:

    SHShowReportType        m_eShowType;                                       ///< ��ʾ��ʽ
    int                     m_nChannelCount;                                    ///< ��ͨ����

    CReportV                m_ctlHarmonicReport;                                ///< ��Чֵ����
    int                     m_nHarmRepLeftColIndex;                             ///< ��Чֵ����������е�����

    CReportV                m_ctlRateReport;                                    ///< �����ʱ���
    int                     m_nRateRepLeftColIndex;                             ///< �����ʱ���������е�����

    CHistogramCtrl          m_ctlHarmonicHistogram;                             ///< ��״ͼ�ؼ�
    int                     m_nHisCurPageIndex;                                 ///< ��״ͼ��ǰҳ
#if !SU_FEI
	bool					m_bHisFlag;
#endif
    CDataHarmonic           m_dataHarmonic;

protected:

    
    /**
     *      ���������ʿؼ�
     */
    void _createRateReport();
    /**
     *      ��ʾ�����ʱ���
     */
    void _showRateReport();
    /**
     *      ���躬���ʱ��������
     */
    void _resetRateReport();
    /**
     *      ���º����ʱ�������
     */
    void _updateRateReport();
    /**
     *      ���º����ʱ����ͷ
     */
    void _updateRateReportColumnTitle();

    /**
     *      ������Чֵ�ؼ�
     */
    void _createHarmReport();
    /**
     *      ���躬���ʱ��������
     */
    void _resetHarmReport();
    
    /**
     *      ��ʾ��Чֵ����
     */
    void _showHarmReport();
    /**
     *      ������Чֵ��������
     */
    void _updateHarmReport();
    /**
     *      ������Чֵ�����ͷ
     */
    void _updateHarmReportColumnTitle();


    /**
     *      ����Histogram
     */
    void _createHistogram();
    /**
     *      ��ʾ��״ͼ
     */
    void _showHistogram();
    /**
     *      ������״ͼ
     */
    void _updateHistorgram();
    /**
     *      ������״ͼͨ������
     */
    void _updateHistorgramTitle();

    /**
     *      �л���ʾ��ʽ
     */
    void _changeShowType();

    /**
     *      �޸Ĳ˵�HMENU
     */
    virtual void _modifyMenu();


    /**
     *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
     *
     *  \note
     *      ����ѡ����ƿ���Ҫ���ظ÷�����һЩ�������
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);

    /**
     *      ���ͨ�����Ƿ��޸�
     */
    bool _checkChannel();

    /**
     *      �ָ�ҳ��״̬
     */
    bool _restorePage();

public:
    /**
     *      ����ҳ������
     */
    void CSmvHarmonicDlg::_SavePage();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnMenuF4();
#if !SU_FEI
	afx_msg void OnMenuF5();
#endif
};
