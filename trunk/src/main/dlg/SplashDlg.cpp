/**
 * \file
 *      SplashDlg.cpp
 *
 * \brief
 *      Ӧ�ó���ʼǰsplash����ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/8
 */

// SplashDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"

#include "SplashDlg.h"
#include "src/main/resourcebmp.h"
#include "src/utils/utility/dsm_utility.h"

#define DEFAULT_SPLASH_TIME             1000        ///< splashʱ��

CSplashDlg* CSplashDlg::g_spSplashDlg = NULL;

// CSplashDlg �Ի���

IMPLEMENT_DYNAMIC(CSplashDlg, CDialog)

CSplashDlg::CSplashDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSplashDlg::IDD, pParent)
    , m_nWidth(0)
    , m_nHeight(0)
    , m_nTimerID(0)
{
    m_nWidth = dsm_config::GetScreenWidth();
    m_nHeight = dsm_config::GetScreenHeight();
}

CSplashDlg::~CSplashDlg()
{
}

void CSplashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSplashDlg, CDialog)
    ON_WM_TIMER()
    ON_WM_PAINT()
END_MESSAGE_MAP()


// CSplashDlg ��Ϣ�������

void CSplashDlg::_ShowSplashScreen(CWnd* pParentWnd)
{
    if(!g_spSplashDlg)
    {
        g_spSplashDlg = new CSplashDlg;
    }

    if (!g_spSplashDlg->Create(CSplashDlg::IDD, pParentWnd))
    {
        delete g_spSplashDlg;
        g_spSplashDlg = NULL;
    }
    else
    {
        CRect rcRect(0, 0, g_spSplashDlg->m_nWidth, g_spSplashDlg->m_nHeight);
        // λ����Ҫ����

        g_spSplashDlg->MoveWindow(rcRect, TRUE);
        g_spSplashDlg->ShowWindow(SW_SHOW);
        g_spSplashDlg->CenterWindow();
    }

    g_spSplashDlg->UpdateWindow();
    //g_spSplashDlg->m_nTimerID = g_spSplashDlg->SetTimer(1, DEFAULT_SPLASH_TIME, NULL);
}

void CSplashDlg::_HideSplashScreen(void)
{
    g_spSplashDlg->KillTimer(g_spSplashDlg->m_nTimerID);
    g_spSplashDlg->DestroyWindow();
    AfxGetMainWnd()->UpdateWindow();

    delete g_spSplashDlg;
    g_spSplashDlg = NULL;
}

void CSplashDlg::OnTimer(UINT_PTR nIDEvent)
{
    UNREFERENCED_PARAMETER(nIDEvent);

    _HideSplashScreen();
}


void CSplashDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    CDC dcMem;
    dcMem.CreateCompatibleDC(&dc);
    HGDIOBJ hOriginal = dcMem.SelectObject(dsm_bmpmgr::Get(DSM_BMP_SPLASHLOGO));

    dc.BitBlt(0, 0, m_nWidth, m_nHeight, &dcMem, 0, 0, SRCCOPY);

    dcMem.SelectObject(hOriginal);
    dcMem.DeleteDC();
}
