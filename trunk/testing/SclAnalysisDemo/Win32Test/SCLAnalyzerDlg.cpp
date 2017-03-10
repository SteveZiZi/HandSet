// SCLAnalyzerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SCLAnalyzer.h"
#include "SCLAnalyzerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#pragma comment(lib,"./dll/sclanalysis.lib")

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSCLAnalyzerDlg 对话框




CSCLAnalyzerDlg::CSCLAnalyzerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSCLAnalyzerDlg::IDD, pParent)
	, m_TestData(_T(""))
	, m_LoadTime(_T(""))
	, m_ReadTime(_T(""))
	, m_WriteTime(_T(""))
	, m_SaveTime(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pSclCacheMgr  = CreateSCLCacheManagerImpl();
	m_SMV92Analysis = CreateSCDSMV92AnalysisImpl(m_pSclCacheMgr);
	m_GOOSEAnalysis = CreateSCDGOOSEAnalysisImpl(m_pSclCacheMgr);
}

CSCLAnalyzerDlg::~CSCLAnalyzerDlg()
{
	RelaseSCDSMV92AnalysisImpl(m_SMV92Analysis);
	RelaseSCDGOOSEAnalysisImpl(m_GOOSEAnalysis);
	RelaseSCLCacheManagerImpl(m_pSclCacheMgr);
}

void CSCLAnalyzerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_TestData);
	DDX_Text(pDX, IDC_EDIT2, m_LoadTime);
	DDX_Text(pDX, IDC_EDIT3, m_ReadTime);
	DDX_Text(pDX, IDC_EDIT4, m_WriteTime);
	DDX_Text(pDX, IDC_EDIT5, m_SaveTime);
}

BEGIN_MESSAGE_MAP(CSCLAnalyzerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_LOAD_FILE, &CSCLAnalyzerDlg::OnBnClickedLoadFile)
	ON_BN_CLICKED(ID_GET_IED_INFO, &CSCLAnalyzerDlg::OnBnClickedGetIedInfo)
	ON_BN_CLICKED(ID_GET_DATASET_INFO, &CSCLAnalyzerDlg::OnBnClickedGetDatasetInfo)
	ON_BN_CLICKED(ID_LOAD_FILE2, &CSCLAnalyzerDlg::OnBnClickedLoadFile2)
	ON_BN_CLICKED(ID_GET_IED_INFO2, &CSCLAnalyzerDlg::OnBnClickedGetIedInfo2)
	ON_BN_CLICKED(ID_GET_DATASET_INFO2, &CSCLAnalyzerDlg::OnBnClickedGetDatasetInfo2)
	ON_BN_CLICKED(ID_GET_SMVINPUT_INFO, &CSCLAnalyzerDlg::OnBnClickedGetSmvinputInfo)
	ON_BN_CLICKED(ID_GET_GSEINPUT_INFO, &CSCLAnalyzerDlg::OnBnClickedGetGseinputInfo)
	ON_BN_CLICKED(ID_SAVE_TEST, &CSCLAnalyzerDlg::OnBnClickedSaveTest)
END_MESSAGE_MAP()


// CSCLAnalyzerDlg 消息处理程序

BOOL CSCLAnalyzerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSCLAnalyzerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSCLAnalyzerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CSCLAnalyzerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSCLAnalyzerDlg::OnBnClickedLoadFile()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;

	m_TestData = _T("");

	// 获取一个开始时间值
	DWORD dwStartTime = GetTickCount();

//	std::wstring fileName(_T("E:\\数字化变电站分析仪资料\\其他资料\\SCL相关\\220kV桐子湾变201303081044.scd"));
	std::wstring fileName(_T("E:\\数字化变电站分析仪资料\\其他资料\\SCL相关\\GuoWangMUTest.SCD"));
	if ( m_pSclCacheMgr->LoadSCL(fileName) != -1 )
		str.Format(_T("Load Success!\r\n"));
	else
		str.Format(_T("Load Fail!\r\n"));
	m_TestData += str;

	// 获取一个执行结束时间值，得到总的执行时间值
	DWORD dwFinishTime = GetTickCount();
	str.Format(_T("%d "), (DWORD)(dwFinishTime-dwStartTime)); 
	m_LoadTime = str;
	UpdateData(FALSE);
}

void CSCLAnalyzerDlg::OnBnClickedGetIedInfo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString  str;
	SCDSMV92ANALYSIS_IEDSMV_INFO *iedInfo = NULL;
	int smvNum;
	unsigned char macAddr[6] = {0x01,0x0C,0xCD,0x04,0x40,0x82};

	m_TestData = _T("");

	// 获取一个开始时间值
	DWORD dwStartTime = GetTickCount();

	smvNum = m_SMV92Analysis->GetIEDSMVNum();
	str.Format(_T("SMVNum:%d\r\n"),smvNum);
	m_TestData += str;

	if ( smvNum > 0)
	{
		// 获取IED信息
		iedInfo = new SCDSMV92ANALYSIS_IEDSMV_INFO[smvNum];
		if ( m_SMV92Analysis->GetOneIEDSMVInfo(macAddr,0x4082,&iedInfo[0]) != -1 )
		{
			DisplayIEDInfo(&iedInfo[0]);
		}
		else
		{
			str.Format(_T("GetOneIEDInfo Failed!\r\n\r\n"));
			m_TestData += str;
		}

		if ( m_SMV92Analysis->GetAllIEDSMVInfo(iedInfo,smvNum) != -1 )
		{
			for ( int i=0 ; i<smvNum; i++)
			{
				DisplayIEDInfo(&iedInfo[i]);
			}
		}
		else
		{
			str.Format(_T("GetAllIEDInfo Failed!\r\n\r\n"));
			m_TestData += str;
		}
	}

	// 获取一个执行结束时间值，得到总的执行时间值
	DWORD dwFinishTime = GetTickCount();
	str.Format(_T("%d "), (DWORD)(dwFinishTime-dwStartTime)); 
	m_ReadTime = str;
	UpdateData(FALSE);

	delete[] iedInfo;
}

void CSCLAnalyzerDlg::OnBnClickedGetDatasetInfo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString  str;
	SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo = NULL;
	int fcdaNum;
	int ideSeq;
	unsigned char macAddr[6] = {0x01,0x0C,0xCD,0x04,0x40,0x82};

	m_TestData = _T("");

	// 获取一个开始时间值
	DWORD dwStartTime = GetTickCount();

	// 获取数据集信息
	fcdaNum = m_SMV92Analysis->GetIEDSMVDataSetNum(macAddr,0x4082);

	if ( fcdaNum > 0 )
	{
		dataSetInfo = new SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO[fcdaNum];
		if ( m_SMV92Analysis->GetOneIEDSMVDataSetInfo(macAddr,0x4082,0,&dataSetInfo[0]) != -1 )
		{
			DisplayDataSetInfo(&dataSetInfo[0]);
		}
		else
		{
			str.Format(_T("GetOneDataSetInfo Failed!\r\n\r\n"));
			m_TestData += str;
		}

		if ( m_SMV92Analysis->GetAllIEDSMVDataSetInfo(macAddr,0x4082,dataSetInfo,fcdaNum) != -1 )
		{
			for (int i=0; i<fcdaNum; i++)
			{
				DisplayDataSetInfo(&dataSetInfo[i]);
			}
		}
		else
		{
			str.Format(_T("GetAllDataSetInfo Failed!\r\n\r\n"));
			m_TestData += str;
		}

		ideSeq = m_SMV92Analysis->GetOneIEDSMVDataSetIdxSeq(macAddr,0x4082,&dataSetInfo[4]);
		str.Format(_T("OneIEDSMVDataSetIdxSeq:%d\r\n\r\n"),ideSeq);
		m_TestData += str;
	}

	// 获取一个执行结束时间值，得到总的执行时间值
	DWORD dwFinishTime = GetTickCount();
	str.Format(_T("%d "), (DWORD)(dwFinishTime-dwStartTime)); 
	m_ReadTime = str;
	UpdateData(FALSE);

	delete[] dataSetInfo;
}

void CSCLAnalyzerDlg::DisplayIEDInfo(SCDSMV92ANALYSIS_IEDSMV_INFO *iedInfo)
{
	CString str;

	str.Format(_T("SMV_IEDName:%s\r\n"),iedInfo->iedName.c_str());
	m_TestData += str;
	str.Format(_T("SMV_IEDDesc:%s\r\n"),iedInfo->iedDesc.c_str());
	m_TestData += str;
	str.Format(_T("SMV_APName:%s\r\n"),iedInfo->iedAPName.c_str());
	m_TestData += str;
	str.Format(_T("SMV_APDesc:%s\r\n"),iedInfo->iedAPDesc.c_str());
	m_TestData += str;
	str.Format(_T("SMV_LDInst:%s\r\n"),iedInfo->iedLdInst.c_str());
	m_TestData += str;
	str.Format(_T("SMV_LDDesc:%s\r\n\r\n"),iedInfo->iedLdDesc.c_str());
	m_TestData += str;
	str.Format(_T("DataSetName:%s\r\n"),iedInfo->dataSetName.c_str());
	m_TestData += str;
	str.Format(_T("DataSetDesc:%s\r\n"),iedInfo->dataSetDesc.c_str());
	m_TestData += str;
	str.Format(_T("DataSetChannelNum:%d\r\n"),iedInfo->channelNum);
	m_TestData += str;
	str.Format(_T("SMV_MacAddr:%x-%x-%x-%x-%x-%x\r\n"),iedInfo->smvMAC[0],iedInfo->smvMAC[1],iedInfo->smvMAC[2],
		iedInfo->smvMAC[3],iedInfo->smvMAC[4],iedInfo->smvMAC[5]);
	m_TestData += str;
	str.Format(_T("SMV_AppID:%x\r\n"),iedInfo->smvAppID);
	m_TestData += str;
	str.Format(_T("SMV_VlanID:%x\r\n"),iedInfo->smvVlanID);
	m_TestData += str;
	str.Format(_T("SMV_VlanPrio:%x\r\n\r\n"),iedInfo->smvVlanPri);
	m_TestData += str;
	str.Format(_T("SMVCtrl_Name:%s\r\n"),iedInfo->smvCbName.c_str());
	m_TestData += str;
	str.Format(_T("SMVCtrl_Desc:%s\r\n"),iedInfo->smvDesc.c_str());
	m_TestData += str;
	str.Format(_T("SMVCtrl_ConfRev:%d\r\n"),iedInfo->smvCbConfRev);
	m_TestData += str;
	str.Format(_T("SMVCtrl_DataSet:%s\r\n"),iedInfo->smvCbDataSet.c_str());
	m_TestData += str;
	str.Format(_T("SMVCtrl_NofASDU:%d\r\n"),iedInfo->smvCbNofASDU);
	m_TestData += str;
	str.Format(_T("SMVCtrl_SmpRate:%d\r\n"),iedInfo->smvCbSmpRate);
	m_TestData += str;
	str.Format(_T("SMVCtrl_SmvID:%s\r\n"),iedInfo->smvCbSmvID.c_str());
	m_TestData += str;
	str.Format(_T("SMVCtrl_Multicast:%d\r\n"),iedInfo->smvCbMulticast);
	m_TestData += str;
	str.Format(_T("SMVCtrl_smvOptRefreshTIme:%d\r\n"),iedInfo->smvCbOptRefreshTime);
	m_TestData += str;
	str.Format(_T("SMVCtrl_smvOptSmpRate:%d\r\n"),iedInfo->smvCbOptSmpRate);
	m_TestData += str;
	str.Format(_T("SMVCtrl_smvOptDataRef:%d\r\n"),iedInfo->smvCbOptDataRef);
	m_TestData += str;
	str.Format(_T("SMVCtrl_smvOptSmpSync:%d\r\n"),iedInfo->smvCbOptSmpSync);
	m_TestData += str;
	str.Format(_T("SMVCtrl_smvOptSecurity:%d\r\n\r\n"),iedInfo->smvCbOptSecurity);
	m_TestData += str;
}

void CSCLAnalyzerDlg::DisplayDataSetInfo(SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo)
{
	CString str;

	str.Format(_T("iedSeq:%d\r\n"),dataSetInfo->idxSeq);
	m_TestData += str;
	str.Format(_T("DataSetName:%s\r\n"),dataSetInfo->dataSetName.c_str());
	m_TestData += str;
	str.Format(_T("DataSetDesc:%s\r\n"),dataSetInfo->dataSetDesc.c_str());
	m_TestData += str;
	str.Format(_T("ldInste:%s\r\n"),dataSetInfo->ldInst.c_str());
	m_TestData += str;
	str.Format(_T("lnPrefix:%s\r\n"),dataSetInfo->lnPrefix.c_str());
	m_TestData += str;
	str.Format(_T("lnClass:%s\r\n"),dataSetInfo->lnClass.c_str());
	m_TestData += str;
	str.Format(_T("lnInst:%s\r\n"),dataSetInfo->lnInst.c_str());
	m_TestData += str;
	str.Format(_T("doName:%s\r\n"),dataSetInfo->doName.c_str());
	m_TestData += str;
	str.Format(_T("doDesc:%s\r\n"),dataSetInfo->doDesc.c_str());
	m_TestData += str;
	str.Format(_T("fc:%s\r\n"),dataSetInfo->daFc.c_str());
	m_TestData += str;
	str.Format(_T("daName:%s\r\n"),dataSetInfo->daName.c_str());
	m_TestData += str;
	str.Format(_T("daSAddr:%s\r\n"),dataSetInfo->dasAddr.c_str());
	m_TestData += str;
	str.Format(_T("daVal:%s\r\n\r\n"),dataSetInfo->daVal.c_str());
	m_TestData += str;
}

void CSCLAnalyzerDlg::OnBnClickedLoadFile2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString  str;

	m_TestData = _T("");

	DWORD dwStartTime = GetTickCount();

// 	std::wstring fileName(_T("E:\\数字化变电站分析仪资料\\其他资料\\SCL相关\\2012102401leigu.scd"));
 	std::wstring fileName(_T("E:\\数字化变电站分析仪资料\\其他资料\\SCL相关\\220kV桐子湾变201303081044.scd"));
// 	std::wstring fileName(_T("E:\\数字化变电站分析仪资料\\其他资料\\SCL相关\\WC1016B.scd"));
//	std::wstring fileName(_T("E:\\数字化变电站分析仪资料\\其他资料\\SCL相关\\GuoWangMUTest.SCD"));
//	std::wstring fileName(_T("E:\\数字化变电站分析仪资料\\其他资料\\SCL相关\\WC1016B_2.scd"));
//	std::wstring fileName(_T("C:\\Documents and Settings\\chao\\桌面\\test.scdx"));
	if ( m_pSclCacheMgr->LoadSCL(fileName) != -1 )
		str.Format(_T("Load Success!\r\n"));
	else
		str.Format(_T("Load Fail!\r\n"));
	m_TestData += str;


	// 获取一个执行结束时间值，得到总的执行时间值
	DWORD dwFinishTime = GetTickCount();
	str.Format(_T("%d "), (DWORD)(dwFinishTime-dwStartTime)); 
	m_LoadTime = str;
	UpdateData(FALSE);
}

void CSCLAnalyzerDlg::OnBnClickedGetIedInfo2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString  str;
	SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedInfo = NULL;
	int gooseNum;
	// GuoWang
// 	unsigned char macAddr[6] = {0x01,0x0C,0xCD,0x01,0x10,0x80};
// 	unsigned short appID = 0x1080;
	// WC1016B
// 	unsigned char macAddr[6] = {0x01,0x0C,0xCD,0x01,0x00,0x03};
// 	unsigned short appID = 0x0003;
	// 220kV桐子湾变201303081044
	unsigned char macAddr[6] = {0x01,0x0C,0xCD,0x01,0x02,0x11};
	unsigned short appID = 0x0211;

	m_TestData = _T("");

	DWORD dwStartTime = GetTickCount();

	gooseNum = m_GOOSEAnalysis->GetIEDGOOSENum();
	str.Format(_T("GOOSENum:%d\r\n"),gooseNum);
	m_TestData += str;

	if ( gooseNum > 0 )
	{
		// 获取IED信息
		iedInfo = new SCDGOOSEANALYSIS_IEDGOOSE_INFO[gooseNum];

		if ( m_GOOSEAnalysis->GetOneIEDGOOSEInfo(macAddr,appID,&iedInfo[0]) != -1 )
		{
			DisplayGOOSEIEDInfo(&iedInfo[0]);
		}
		else
		{
			str.Format(_T("GetOneIEDInfo Failed!\r\n\r\n"));
			m_TestData += str;
		}

		if ( m_GOOSEAnalysis->GetAllIEDGOOSEInfo(iedInfo,gooseNum) != -1 )
		{
			for ( int i=0 ; i<gooseNum; i++)
			{
				DisplayGOOSEIEDInfo(&iedInfo[i]);
			}
		}
		else
		{
			str.Format(_T("GetAllIEDInfo Failed!\r\n\r\n"));
			m_TestData += str;
		}
	}

	// 获取一个执行结束时间值，得到总的执行时间值
	DWORD dwFinishTime = GetTickCount();
	str.Format(_T("%d "), (DWORD)(dwFinishTime-dwStartTime)); 
	m_ReadTime = str;
	UpdateData(FALSE);

	delete[] iedInfo;
}

void CSCLAnalyzerDlg::OnBnClickedGetDatasetInfo2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString  str;
	SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo = NULL;
	int fcdaNum;
	int ideSeq;
	// GuoWang
// 	unsigned char macAddr[6] = {0x01,0x0C,0xCD,0x01,0x10,0x80};
// 	unsigned short appID = 0x1080;
	// WC1016B
// 	unsigned char macAddr[6] = {0x01,0x0C,0xCD,0x01,0x00,0x03};
// 	unsigned short appID = 0x0003;
	// 220kV桐子湾变201303081044
	unsigned char macAddr[6] = {0x01,0x0C,0xCD,0x01,0x02,0x11};
	unsigned short appID = 0x0211;

	m_TestData = _T("");

	DWORD dwStartTime = GetTickCount();

	fcdaNum = m_GOOSEAnalysis->GetIEDGOOSEDataSetNum(macAddr,appID);

	if ( fcdaNum > 0 )
	{
		dataSetInfo = new SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO[fcdaNum];
		if ( m_GOOSEAnalysis->GetOneIEDGOOSEDataSetInfo(macAddr,appID,0,&dataSetInfo[0]) != -1 )
		{
			DisplayGOOSEDataSetInfo(&dataSetInfo[0]);
		}
		else
		{
			str.Format(_T("GetOneDataSetInfo Failed!\r\n\r\n"));
			m_TestData += str;
		}

		if ( m_GOOSEAnalysis->GetAllIEDGOOSEDataSetInfo(macAddr,appID,dataSetInfo,fcdaNum) != -1 )
		{
			for (int i=0; i<fcdaNum; i++)
			{
				str.Format(_T("%d\r\n"),i);
				m_TestData += str;
				DisplayGOOSEDataSetInfo(&dataSetInfo[i]);
				if ( dataSetInfo[i].daName.empty() )
				{
					int subChannelNum = m_GOOSEAnalysis->GetIEDGOOSESubChannelNum(macAddr,appID,dataSetInfo[i]);
					if ( subChannelNum > 0 )
					{
						SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfo = new SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO[subChannelNum];
						if ( m_GOOSEAnalysis->GetAllIEDGOOSESubChannelInfo(macAddr,appID,dataSetInfo[i],subChannelInfo,subChannelNum) != -1 )
						{
							str.Format(_T("SubChannel:%d\r\n"),subChannelNum);
							m_TestData += str;
							for (int j=0; j<subChannelNum; j++)
							{
								DisplayGOOSEDataSetInfo(&subChannelInfo[j]);
							}
						}
					}
				}
			}
		}
		else
		{
			str.Format(_T("GetAllDataSetInfo Failed!\r\n\r\n"));
			m_TestData += str;
		}

		ideSeq = m_GOOSEAnalysis->GetOneIEDGOOSEDataSetIdxSeq(macAddr,appID,&dataSetInfo[1]);
		str.Format(_T("OneIEDGOOSEDataSetIdxSeq:%d\r\n\r\n"),ideSeq);
		m_TestData += str;
	}

	// 获取一个执行结束时间值，得到总的执行时间值
	DWORD dwFinishTime = GetTickCount();
	str.Format(_T("%d "), (DWORD)(dwFinishTime-dwStartTime)); 
	m_ReadTime = str;
	m_WriteTime  = _T("0");
	m_SaveTime = _T("0");
	UpdateData(FALSE);

	delete[] dataSetInfo;
}

void CSCLAnalyzerDlg::DisplayGOOSEIEDInfo(SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedInfo)
{
	CString str;

	str.Format(_T("GOOSE_IEDName:%s\r\n"),iedInfo->iedName.c_str());
	m_TestData += str;
	str.Format(_T("GOOSE_IEDDesc:%s\r\n"),iedInfo->iedDesc.c_str());
	m_TestData += str;
	str.Format(_T("GOOSE_APName:%s\r\n"),iedInfo->iedAPName.c_str());
	m_TestData += str;
	str.Format(_T("GOOSE_APDesc:%s\r\n"),iedInfo->iedAPDesc.c_str());
	m_TestData += str;
	str.Format(_T("GOOSE_LDInst:%s\r\n"),iedInfo->iedLdInst.c_str());
	m_TestData += str;
	str.Format(_T("GOOSE_LDDesc:%s\r\n\r\n"),iedInfo->iedLdDesc.c_str());
	m_TestData += str;
	str.Format(_T("DataSetName:%s\r\n"),iedInfo->dataSetName.c_str());
	m_TestData += str;
	str.Format(_T("DataSetDesc:%s\r\n"),iedInfo->dataSetDesc.c_str());
	m_TestData += str;
	str.Format(_T("DataSetChannelNum:%d\r\n"),iedInfo->channelNum);
	m_TestData += str;
	str.Format(_T("GOOSE_MacAddr:%x-%x-%x-%x-%x-%x\r\n"),iedInfo->gooseMAC[0],iedInfo->gooseMAC[1],iedInfo->gooseMAC[2],
		iedInfo->gooseMAC[3],iedInfo->gooseMAC[4],iedInfo->gooseMAC[5]);
	m_TestData += str;
	str.Format(_T("GOOSE_AppID:%x\r\n"),iedInfo->gooseAppID);
	m_TestData += str;
	str.Format(_T("GOOSE_VlanID:%x\r\n"),iedInfo->gooseVlanID);
	m_TestData += str;
	str.Format(_T("GOOSE_VlanPrio:%x\r\n"),iedInfo->gooseVlanPri);
	m_TestData += str;
	str.Format(_T("GOOSE_MinTime:%d\r\n"),iedInfo->gooseMinTime);
	m_TestData += str;
	str.Format(_T("GOOSE_MaxTime:%d\r\n\r\n"),iedInfo->gooseMaxTime);
	m_TestData += str;
	str.Format(_T("GSECtrl_Name:%s\r\n"),iedInfo->cbName.c_str());
	m_TestData += str;
	str.Format(_T("GSECtrl_Desc:%s\r\n"),iedInfo->cbDesc.c_str());
	m_TestData += str;
	str.Format(_T("GSECtrl_ConfRev:%d\r\n"),iedInfo->confRev);
	m_TestData += str;
	str.Format(_T("GSECtrl_DataSet:%s\r\n"),iedInfo->dataSet.c_str());
	m_TestData += str;
	str.Format(_T("GSECtrl_GOOSEID:%s\r\n"),iedInfo->gooseID.c_str());
	m_TestData += str;
	str.Format(_T("GSECtrl_Type:%s\r\n\r\n"),iedInfo->type.c_str());
	m_TestData += str;
}

void CSCLAnalyzerDlg::DisplayGOOSEDataSetInfo(SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo)
{
	CString str;

	str.Format(_T("iedSeq:%d\r\n"),dataSetInfo->idxSeq);
	m_TestData += str;
	str.Format(_T("DataSetName:%s\r\n"),dataSetInfo->dataSetName.c_str());
	m_TestData += str;
	str.Format(_T("DataSetDesc:%s\r\n"),dataSetInfo->dataSetDesc.c_str());
	m_TestData += str;
	str.Format(_T("ldInste:%s\r\n"),dataSetInfo->ldInst.c_str());
	m_TestData += str;
	str.Format(_T("lnPrefix:%s\r\n"),dataSetInfo->lnPrefix.c_str());
	m_TestData += str;
	str.Format(_T("lnClass:%s\r\n"),dataSetInfo->lnClass.c_str());
	m_TestData += str;
	str.Format(_T("lnInst:%s\r\n"),dataSetInfo->lnInst.c_str());
	m_TestData += str;
	str.Format(_T("doName:%s\r\n"),dataSetInfo->doName.c_str());
	m_TestData += str;
	str.Format(_T("doDesc:%s\r\n"),dataSetInfo->doDesc.c_str());
	m_TestData += str;
	str.Format(_T("fc:%s\r\n"),dataSetInfo->daFc.c_str());
	m_TestData += str;
	str.Format(_T("daName:%s\r\n"),dataSetInfo->daName.c_str());
	m_TestData += str;
	str.Format(_T("lnDesc:%s\r\n"),dataSetInfo->lnDesc.c_str());
	m_TestData += str;
	str.Format(_T("daBType:%s\r\n"),dataSetInfo->daBType.c_str());
	m_TestData += str;
	str.Format(_T("dUSAddr:%s\r\n"),dataSetInfo->dUsAddr.c_str());
	m_TestData += str;
	str.Format(_T("dUVal:%s\r\n\r\n"),dataSetInfo->dUVal.c_str());
	m_TestData += str;
}

void CSCLAnalyzerDlg::DisplayInputsExtRefInfo(SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO *inputsInfo)
{
	CString str;

	str.Format(_T("idxSeq_In:%d\r\n"),inputsInfo->idxSeq_In);
	m_TestData += str;
	str.Format(_T("iedName_In:%s\r\n"),inputsInfo->iedName_In.c_str());
	m_TestData += str;
	str.Format(_T("ldInst_In:%s\r\n"),inputsInfo->ldInst_In.c_str());
	m_TestData += str;
	str.Format(_T("lnName_In:%s\r\n"),inputsInfo->lnName_In.c_str());
	m_TestData += str;
	str.Format(_T("doName_In:%s\r\n"),inputsInfo->doName_In.c_str());
	m_TestData += str;
	str.Format(_T("daName_In:%s\r\n"),inputsInfo->daName_In.c_str());
	m_TestData += str;
	str.Format(_T("lnDesc_In:%s\r\n"),inputsInfo->lnDesc_In.c_str());
	m_TestData += str;
	str.Format(_T("doDesc_In:%s\r\n"),inputsInfo->doDesc_In.c_str());
	m_TestData += str;
	str.Format(_T("dUDesc_In:%s\r\n"),inputsInfo->dUDesc_In.c_str());
	m_TestData += str;
	str.Format(_T("idxSeq_Out:%d\r\n"),inputsInfo->idxSeq_Out);
	m_TestData += str;
	str.Format(_T("iedName_Out:%s\r\n"),inputsInfo->iedName_Out.c_str());
	m_TestData += str;
	str.Format(_T("ldInst_Out:%s\r\n"),inputsInfo->ldInst_Out.c_str());
	m_TestData += str;
	str.Format(_T("lnName_Out:%s\r\n"),inputsInfo->lnName_Out.c_str());
	m_TestData += str;
	str.Format(_T("doName_Out:%s\r\n"),inputsInfo->doName_Out.c_str());
	m_TestData += str;
	str.Format(_T("daName_Out:%s\r\n"),inputsInfo->daName_Out.c_str());
	m_TestData += str;
	str.Format(_T("lnDesc_Out:%s\r\n"),inputsInfo->lnDesc_Out.c_str());
	m_TestData += str;
	str.Format(_T("doDesc_Out:%s\r\n"),inputsInfo->doDesc_Out.c_str());
	m_TestData += str;
	str.Format(_T("dUDesc_Out:%s\r\n\r\n"),inputsInfo->dUDesc_Out.c_str());
	m_TestData += str;
}

void CSCLAnalyzerDlg::DisplayInputsExtRefInfo(SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO *inputsInfo)
{
	CString str;

	str.Format(_T("idxSeq_In:%d\r\n"),inputsInfo->idxSeq_In);
	m_TestData += str;
	str.Format(_T("iedName_In:%s\r\n"),inputsInfo->iedName_In.c_str());
	m_TestData += str;
	str.Format(_T("ldInst_In:%s\r\n"),inputsInfo->ldInst_In.c_str());
	m_TestData += str;
	str.Format(_T("lnName_In:%s\r\n"),inputsInfo->lnName_In.c_str());
	m_TestData += str;
	str.Format(_T("doName_In:%s\r\n"),inputsInfo->doName_In.c_str());
	m_TestData += str;
	str.Format(_T("daName_In:%s\r\n"),inputsInfo->daName_In.c_str());
	m_TestData += str;
	str.Format(_T("lnDesc_In:%s\r\n"),inputsInfo->lnDesc_In.c_str());
	m_TestData += str;
	str.Format(_T("doDesc_In:%s\r\n"),inputsInfo->doDesc_In.c_str());
	m_TestData += str;
	str.Format(_T("dUDesc_In:%s\r\n"),inputsInfo->dUDesc_In.c_str());
	m_TestData += str;
	str.Format(_T("idxSeq_Out:%d\r\n"),inputsInfo->idxSeq_Out);
	m_TestData += str;
	str.Format(_T("iedName_Out:%s\r\n"),inputsInfo->iedName_Out.c_str());
	m_TestData += str;
	str.Format(_T("ldInst_Out:%s\r\n"),inputsInfo->ldInst_Out.c_str());
	m_TestData += str;
	str.Format(_T("lnName_Out:%s\r\n"),inputsInfo->lnName_Out.c_str());
	m_TestData += str;
	str.Format(_T("doName_Out:%s\r\n"),inputsInfo->doName_Out.c_str());
	m_TestData += str;
	str.Format(_T("daName_Out:%s\r\n"),inputsInfo->daName_Out.c_str());
	m_TestData += str;
	str.Format(_T("lnDesc_Out:%s\r\n"),inputsInfo->lnDesc_Out.c_str());
	m_TestData += str;
	str.Format(_T("doDesc_Out:%s\r\n"),inputsInfo->doDesc_Out.c_str());
	m_TestData += str;
	str.Format(_T("dUDesc_Out:%s\r\n\r\n"),inputsInfo->dUDesc_Out.c_str());
	m_TestData += str;
}

void CSCLAnalyzerDlg::OnBnClickedGetSmvinputInfo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString  str;
	SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO *inputsInfo = NULL;
	SCDSMV92ANALYSIS_IEDSMV_INFO iedInfo;
	int inputsNum = 0;
	int smvInNum = 0;

	m_TestData = _T("");

	DWORD dwStartTime = GetTickCount();

	inputsNum = m_SMV92Analysis->GetOneIEDInputsExtRefNum(_T("DMU833_220"));

	if ( inputsNum > 0)
	{
		inputsInfo = new SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO[inputsNum];
		smvInNum = m_SMV92Analysis->GetOneIEDSVINExtRef(_T("DMU833_220"),inputsInfo,inputsNum);
		if ( smvInNum > 0 )
		{
			for (int i=0; i<smvInNum; i++)
			{
				DisplayInputsExtRefInfo(&inputsInfo[i]);
			}
		}
		else
		{
			str.Format(_T("Get InputsInfo Failed!\r\n\r\n"));
			m_TestData += str;
		}

		if ( m_SMV92Analysis->GetOutAddrSMVInfo(inputsInfo[0],&iedInfo) != -1 )
		{
			DisplayIEDInfo(&iedInfo);
		}
		else
		{
			str.Format(_T("Get OutAddrSMVInfo Failed!\r\n\r\n"));
			m_TestData += str;
		}
	}

	// 获取一个执行结束时间值，得到总的执行时间值
	DWORD dwFinishTime = GetTickCount();
	str.Format(_T("%d "), (DWORD)(dwFinishTime-dwStartTime)); 
	m_ReadTime = str;
	m_WriteTime  = _T("0");
	m_SaveTime = _T("0");
	UpdateData(FALSE);

	delete[] inputsInfo;
}

void CSCLAnalyzerDlg::OnBnClickedGetGseinputInfo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString  str;
	SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO *inputsInfo = NULL;
	SCDGOOSEANALYSIS_IEDGOOSE_INFO iedInfo;
	int inputsNum = 0;
	int goInNum = 0;

	m_TestData = _T("");

	DWORD dwStartTime = GetTickCount();

	// LINE220_1
	// DMU833_220
	inputsNum = m_GOOSEAnalysis->GetOneIEDInputsExtRefNum(_T("LINE220_1"));

	if ( inputsNum > 0)
	{
		inputsInfo = new SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO[inputsNum];
		goInNum = m_GOOSEAnalysis->GetOneIEDGOINExtRef(_T("LINE220_1"),inputsInfo,inputsNum);
		if ( goInNum > 0 )
		{
			for (int i=0; i<goInNum; i++)
			{
				DisplayInputsExtRefInfo(&inputsInfo[i]);
			}
		}
		else
		{
			str.Format(_T("Get InputsInfo Failed!\r\n\r\n"));
			m_TestData += str;
		}

		if ( m_GOOSEAnalysis->GetOutAddrGOOSEInfo(inputsInfo[0],&iedInfo) != -1 )
			DisplayGOOSEIEDInfo(&iedInfo);
		else
		{
			str.Format(_T("Get OutAddrSMVInfo Failed!\r\n\r\n"));
			m_TestData += str;
		}
	}

	// 获取一个执行结束时间值，得到总的执行时间值
	DWORD dwFinishTime = GetTickCount();
	str.Format(_T("%d "), (DWORD)(dwFinishTime-dwStartTime)); 
	m_ReadTime = str;
	m_WriteTime  = _T("0");
	m_SaveTime = _T("0");
	UpdateData(FALSE);

	delete[] inputsInfo;
}

void CSCLAnalyzerDlg::OnBnClickedSaveTest()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
//	std::wstring fileName(_T("E:\\数字化变电站分析仪资料\\其他资料\\SCL相关\\2012102401leigu.scd"));
//	std::wstring fileName(_T("E:\\数字化变电站分析仪资料\\其他资料\\SCL相关\\220kV桐子湾变201303081044.scd"));
	std::wstring fileName(_T("E:\\数字化变电站分析仪资料\\其他资料\\SCL相关\\WC1016B.scd"));
//	std::wstring fileName(_T("E:\\数字化变电站分析仪资料\\其他资料\\SCL相关\\GuoWangMUTest.SCD"));
//	std::wstring fileName(_T("E:\\数字化变电站分析仪资料\\其他资料\\SCL相关\\WC1016B_2.scd"));
	m_TestData = _T("");

	bool bInputs = false;
	m_pSclCacheMgr->EnableGseAnalysis(bInputs);
	m_pSclCacheMgr->EnableSmvAnalysis(bInputs);
	DWORD dwStartTime = GetTickCount();
	if ( m_pSclCacheMgr->LoadSCL(fileName) == 0 )
		str.Format(_T("Load File Success\r\n"));
	else
		str.Format(_T("Load File Fail\r\n"));
		m_TestData += str;
	DWORD dwFinishTime = GetTickCount();
	str.Format(_T("%d "), (DWORD)(dwFinishTime-dwStartTime)); 
	m_LoadTime = str;

	dwStartTime = GetTickCount();
	if ( m_pSclCacheMgr->SaveSCL(_T("C:\\Documents and Settings\\chao\\桌面\\test.scd")) == 0 )
		str.Format(_T("Save File Success\r\n"));
	else
		str.Format(_T("Save File Fail\r\n"));
	m_TestData += str;
	dwFinishTime = GetTickCount();
	str.Format(_T("%d "), (DWORD)(dwFinishTime-dwStartTime)); 
	m_SaveTime = str;

	UpdateData(FALSE);
}
