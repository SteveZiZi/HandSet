/**
 * \file
 *      SplashDlg.h
 *
 * \brief
 *      Ӧ�ó���ʼǰsplash����ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/8
 */


#pragma once

#include "src/main/resource.h"


// CSplashDlg �Ի���

class CSplashDlg : public CDialog
{
	DECLARE_DYNAMIC(CSplashDlg)

public:
	CSplashDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSplashDlg();

// �Ի�������
	enum { IDD = IDD_SPLASH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    /**
     *      ��ʾsplashͼƬ�Ի���
     */
    static void _ShowSplashScreen(CWnd* pParentWnd);

    /**
     *      �˳�splashͼƬ�Ի����������������Դ
     */
    static void _HideSplashScreen(void);

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnPaint();

private:
    int                     m_nWidth;           ///< ��Ļ�������ֵ
    int                     m_nHeight;          ///< ��Ļ�߶�����ֵ
    int                     m_nTimerID;         ///< ��ʱ��ID
    static CSplashDlg*      g_spSplashDlg;      ///< ��̬����ָ��
};


