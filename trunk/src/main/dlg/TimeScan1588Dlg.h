/**
 * \file
 *      TimeScan1588Dlg.h
 *
 * \brief
 *      1588ʱ��Դ̽��ҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/10/9
 */


#pragma once

#include "src/main/common.h"
#include "src/main/resource.h"

#include "src/utils/reportcontrol/listctrlel/listctrlel.h"

class CDsmDataLayer;
class CNaviMenu;
class CNaviToolBar;

// CTimeScan1588Dlg �Ի���

class CTimeScan1588Dlg : public CDialog
{
	DECLARE_DYNAMIC(CTimeScan1588Dlg)

public:
	CTimeScan1588Dlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTimeScan1588Dlg();

// �Ի�������
	enum { IDD = IDD_SCAN_1588 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()


public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnPaint();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnMenuF6();

private:
    /**
     *      ����̽��1588ʱ��Դ����
     */
    void _createReport();

    /**
     *      ����̽��
     */
    void _rescan();

    /**
     *      ���ѡ��
     */
    void _clearSelect();

    /**
     *      ��������
     */
    void _clearRes();

    /**
     *      ����̽��������
     */
    void _insertRecord();


private:
    UINT                    m_nScanTimerID;         ///< ��ʱˢ��̽�����Ķ�ʱ��ID
    HWND                    m_pHwndPrev;            ///< ֮ǰtarget
    CNaviMenu*              m_pMenuPrev;            ///< m_pWndPrev������menu
    CNaviToolBar*           m_pNaviBar;             ///< �����˵�������

    CReportV                m_report1588;           ///< ̽��1588ʱ��Դ�������

    CDsmDataLayer*          m_pDataLayer;           ///< ���ݲ�ָ��
};
