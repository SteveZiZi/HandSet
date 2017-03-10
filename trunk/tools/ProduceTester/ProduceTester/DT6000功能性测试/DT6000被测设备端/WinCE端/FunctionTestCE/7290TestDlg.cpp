// 7290TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FunctionTestCE.h"
#include "7290TestDlg.h"




// C7290TestDlg dialog

IMPLEMENT_DYNAMIC(C7290TestDlg, CDialog)

C7290TestDlg::C7290TestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(C7290TestDlg::IDD, pParent)
    ,m_h7290Dll(NULL)
{

}

C7290TestDlg::~C7290TestDlg()
{
}

void C7290TestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(C7290TestDlg, CDialog)
    ON_WM_CHAR()
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// C7290TestDlg message handlers

BOOL C7290TestDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    if (m_h7290Dll ==NULL)
    {
        m_h7290Dll=LoadLibrary(L"\\windows\\epcZLG7290Lib.dll");
    }
    epcZlg7290ScanNum=(int (*)(DWORD ) ) GetProcAddress(m_h7290Dll,L"epcZlg7290ScanNum");
    epcZlg7290FlashTime=( int (*)(DWORD) )GetProcAddress(m_h7290Dll,L"epcZlg7290FlashTime");
    epcZlg7290ScanNumber=( int (*)(DWORD,DWORD,BOOL,BOOL) )
        GetProcAddress(m_h7290Dll,L"epcZlg7290ScanNumber");


    //epcZlg7290ScanNum(8);
    epcZlg7290FlashTime(0);


    SetFocus();
    return TRUE; 
}

void C7290TestDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if ( nChar >=L'0' &&nChar <=L'9')
    {
        for (int i=0;i<8;i++)
        {
            epcZlg7290ScanNumber(i,nChar-L'0',FALSE,TRUE);
        }
    }
    else if (nChar >=L'A' &&nChar <='F')
    {
        for (int i=0;i<8;i++)
        {
            epcZlg7290ScanNumber(i,nChar-L'A'+10,FALSE,TRUE);
        }
    }

    else if (nChar >=L'a' &&nChar <='f')
    {
        for (int i=0;i<8;i++)
        {
            epcZlg7290ScanNumber(i,nChar-L'a'+10,FALSE,TRUE);
        }
    }



    CDialog::OnChar(nChar, nRepCnt, nFlags);
}

void C7290TestDlg::OnDestroy()
{
    CDialog::OnDestroy();
    if (m_h7290Dll)
    {
        CloseHandle(m_h7290Dll);
        m_h7290Dll=NULL;
    }
}
