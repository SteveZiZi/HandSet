// ChartCtrlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ChartCtrl.h"
#include "ChartCtrlDlg.h"
#include "../dlg/HistogramDlg.h"
#include "../dlg/ClockDlg.h"
#include "../dlg/HorHistoDlg.h"
#include "../dlg/OnOffStateDlg.h"
#include "../dlg/PhasorGramDlg.h"
#include "../dlg/PolarityDialDlg.h"
#include "../dlg/WaveFormDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMMER_SETTING 10

// CChartCtrlDlg 对话框

CChartCtrlDlg::CChartCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChartCtrlDlg::IDD, pParent)
	, m_TimerSetting(100)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CChartCtrlDlg::~CChartCtrlDlg()
{
}

void CChartCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_TimerSetting);
}

BEGIN_MESSAGE_MAP(CChartCtrlDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_HISTOGRAM_CREATE, &CChartCtrlDlg::OnBnClickedHistogramCreate)
	ON_BN_CLICKED(IDC_OSCILLOGRAM_CREATE, &CChartCtrlDlg::OnBnClickedOscillogram)
	ON_BN_CLICKED(IDC_PHASEGRAM_CREATE, &CChartCtrlDlg::OnBnClickedPhasegram)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SET_TIMMER, &CChartCtrlDlg::OnBnClickedSetTimmer)
	ON_BN_CLICKED(IDC_DIAL_CREATE, &CChartCtrlDlg::OnBnClickedDialCreate)
	ON_BN_CLICKED(IDC_HOR_HISTO_CREATE, &CChartCtrlDlg::OnBnClickedHorHistoCreate)
	ON_BN_CLICKED(IDC_TIME_TICK_CLOCK_CREATE, &CChartCtrlDlg::OnBnClickedTimeTickClockCreate)
	ON_BN_CLICKED(IDC_ON_OFF_CREATE, &CChartCtrlDlg::OnBnClickedOnOffState)
	ON_BN_CLICKED(IDC_RESET_GRAM, &CChartCtrlDlg::OnBnClickedKillerTime)
END_MESSAGE_MAP()


// CChartCtrlDlg 消息处理程序

BOOL CChartCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	SetTimer(2,1000,NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CChartCtrlDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_CHARTCTRL_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_CHARTCTRL_DIALOG));
	}
}
#endif


void CChartCtrlDlg::OnBnClickedHistogramCreate()
{
	// TODO: 在此添加控件通知处理程序代码
	CHistogramDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDlg::OnBnClickedOscillogram()
{
	// TODO: 在此添加控件通知处理程序代码
	CWaveFormDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDlg::OnBnClickedPhasegram()
{
	// TODO: 在此添加控件通知处理程序代码
	CPhasorGramDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDlg::OnBnClickedDialCreate()
{
	// TODO: 在此添加控件通知处理程序代码
	CPolarityDialDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDlg::OnBnClickedHorHistoCreate()
{
	// TODO: 在此添加控件通知处理程序代码
	CHorHistoDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDlg::OnBnClickedTimeTickClockCreate()
{
	// TODO: 在此添加控件通知处理程序代码
	CClockDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDlg::OnBnClickedOnOffState()
{
	// TODO: 在此添加控件通知处理程序代码
	COnOffStateDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	
//	AfxGetApp()->CWinApp::OnIdle(1);
	CDialog::OnTimer(nIDEvent);
}

#include <string>
#include <map>

typedef struct
{
	std::wstring str1;
	std::wstring str2;
// 	wchar_t  str1[1024];
// 	wchar_t  str2[1024];
}STRUCT_A;

typedef std::map<int, STRUCT_A> A_MAP;

typedef struct
{
	std::wstring str1;
	int i;
	A_MAP *aMap;
}STRUCT_B;

typedef std::map<int, STRUCT_B *> B_MAP;

typedef std::map<int, STRUCT_A *> AA_MAP;

void CChartCtrlDlg::OnBnClickedSetTimmer()
{
	// TODO: 在此添加控件通知处理程序代码
	MEMORYSTATUS  memStat = {sizeof(MEMORYSTATUS)};

	::GlobalMemoryStatus(&memStat);
	RETAILMSG(1,(_T("memStat.dwAvailPhys: %u \r\n"),memStat.dwAvailPhys));
	STRUCT_A a;
	a.str1.resize(1024,'A');
	a.str2.resize(1024,'1');
	B_MAP bMap;
	for ( int i = 0; i < 1000; i++ )
	{
		A_MAP *pAMap = new A_MAP;
		pAMap->insert(A_MAP::value_type(1,a));
		pAMap->insert(A_MAP::value_type(2,a));

		STRUCT_B *b = new STRUCT_B;
		b->str1.resize(1024,'B');
		b->i    = i;
		b->aMap = pAMap;

		bMap.insert(B_MAP::value_type(i,b));
	}

	::GlobalMemoryStatus(&memStat);
	RETAILMSG(1,(_T("memStat.dwAvailPhys: %u \r\n"),memStat.dwAvailPhys));

	for ( B_MAP::iterator it_B = bMap.begin(); it_B != bMap.end(); it_B++ )
	{
		if ( it_B->second != NULL )
		{
			if ( it_B->second->aMap != NULL )
			{
				A_MAP::iterator it_A;
				A_MAP *pAMap = it_B->second->aMap;
// 				for ( it_A = pAMap->begin(); it_A != pAMap->end(); it_A++ )
// 				{
// 					it_A->second.str1.clear();
// 					it_A->second.str2.clear();
//					RETAILMSG(1,(_T("it_A->second.str2:%d\r\n"),it_A->second.str2.capacity()));
//				}
				it_B->second->aMap->clear();
				delete it_B->second->aMap;
			}
//			it_B->second->str1.clear();
			delete it_B->second;
		}
	}
	bMap.clear();

	::GlobalMemoryStatus(&memStat);
	RETAILMSG(1,(_T("memStat.dwAvailPhys: %u \r\n"),memStat.dwAvailPhys));

	DWORD st = GetTickCount();
	HANDLE heapHdl = GetProcessHeap();
	HeapCompact(heapHdl,HEAP_NO_SERIALIZE);
	DWORD et = GetTickCount();
	RETAILMSG(1,(_T("HeapCompact Time: %u \r\n"),et-st));

	::GlobalMemoryStatus(&memStat);
	RETAILMSG(1,(_T("memStat.dwAvailPhys: %u \r\n"),memStat.dwAvailPhys));


// 	MEMORYSTATUS  memStat = {sizeof(MEMORYSTATUS)};
// 
// 	::GlobalMemoryStatus(&memStat);
// 	RETAILMSG(1,(_T("memStat.dwAvailPhys: %u \r\n"),memStat.dwAvailPhys));
// 
// 	STRUCT_A *pA = new STRUCT_A[1];
// 	pA->str1.resize(1024,'A');
// 	pA->str2.resize(1024,'1');
// 	::GlobalMemoryStatus(&memStat);
// 	RETAILMSG(1,(_T("memStat.dwAvailPhys: %u \r\n"),memStat.dwAvailPhys));
// 
// 	delete [] pA;
// 	Sleep(1000);
// 	::GlobalMemoryStatus(&memStat);
// 	RETAILMSG(1,(_T("memStat.dwAvailPhys: %u \r\n"),memStat.dwAvailPhys));



// 	::GlobalMemoryStatus(&memStat);
// 	RETAILMSG(1,(_T("memStat.dwAvailPhys: %u \r\n"),memStat.dwAvailPhys));
//
// 	AA_MAP aaMap;
// 	for ( int i = 0; i < 4; i++ )
// 	{
// 		STRUCT_A *pA = new STRUCT_A;
// 		pA->str1.resize(1024,'A');
// 		pA->str2.resize(1024,'1');
// 		aaMap.insert(AA_MAP::value_type(i,pA));
// 	}
// 
// 	::GlobalMemoryStatus(&memStat);
// 	RETAILMSG(1,(_T("memStat.dwAvailPhys: %u \r\n"),memStat.dwAvailPhys));
// 
// 	AA_MAP::iterator it_AA;
// 	std::wstring *pStr = NULL;
// 	for ( it_AA = aaMap.begin(); it_AA != aaMap.end(); it_AA++ )
// 	{
// 		if ( it_AA->second != NULL )
// 		{
// 			pStr = &(it_AA->second->str1);
// // 			it_AA->second->str1.clear();
// // 			it_AA->second->str2.clear();
// 			delete it_AA->second;
// 		}
// 	}
// 	aaMap.clear();
// 
// 	::GlobalMemoryStatus(&memStat);
// 	RETAILMSG(1,(_T("memStat.dwAvailPhys: %u \r\n"),memStat.dwAvailPhys));
// 	try
// 	{
// 		if ( pStr->size() == 0 )
// 		{
// 			pStr->assign(_T("aaaaaaaa"));
// 		}
// 		TRACE((_T("%s\r\n\r\n"),(*pStr).c_str()));
// 	}
// 	catch (...)
// 	{
// 		TRACE((_T("Error\r\n")));
// 	}
}

void CChartCtrlDlg::OnBnClickedKillerTime()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(2);
}
