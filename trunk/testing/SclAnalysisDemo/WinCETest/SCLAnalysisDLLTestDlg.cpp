// SCLAnalysisDLLTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SCLAnalysisDLLTest.h"
#include "SCLAnalysisDLLTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*#pragma comment(lib,"./lib/sclanalysislib.lib")*/

// CSCLAnalysisDLLTestDlg 对话框

CSCLAnalysisDLLTestDlg::CSCLAnalysisDLLTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSCLAnalysisDLLTestDlg::IDD, pParent)
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

CSCLAnalysisDLLTestDlg::~CSCLAnalysisDLLTestDlg()
{
	RelaseSCDSMV92AnalysisImpl(m_SMV92Analysis);
	RelaseSCDGOOSEAnalysisImpl(m_GOOSEAnalysis);
	RelaseSCLCacheManagerImpl(m_pSclCacheMgr);
}

void CSCLAnalysisDLLTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SCL_DATA, m_TestData);
	DDX_Text(pDX, IDC_LOAD_TIME, m_LoadTime);
	DDX_Text(pDX, IDC_READ_TIME, m_ReadTime);
	DDX_Text(pDX, IDC_WRITE_TIME, m_WriteTime);
	DDX_Text(pDX, IDC_SAVE_TIME, m_SaveTime);
}

BEGIN_MESSAGE_MAP(CSCLAnalysisDLLTestDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOAD_FILE, &CSCLAnalysisDLLTestDlg::OnBnClickedLoadFile)
	ON_BN_CLICKED(IDC_GET_IED_INFO, &CSCLAnalysisDLLTestDlg::OnBnClickedGetIedInfo)
	ON_BN_CLICKED(IDC_GET_DATASET_INFO, &CSCLAnalysisDLLTestDlg::OnBnClickedGetDatasetInfo)
	ON_BN_CLICKED(IDC_LOAD_FILE_GSE, &CSCLAnalysisDLLTestDlg::OnBnClickedLoadFileGse)
	ON_BN_CLICKED(IDC_GET_IEDGSE_INFO, &CSCLAnalysisDLLTestDlg::OnBnClickedGetIedgseInfo)
	ON_BN_CLICKED(IDC_GET_GSEDATASET_INFO, &CSCLAnalysisDLLTestDlg::OnBnClickedGetGsedatasetInfo)
	ON_BN_CLICKED(IDC_GET_GSEINPUTS_INFO, &CSCLAnalysisDLLTestDlg::OnBnClickedGetGseinputsInfo)
	ON_BN_CLICKED(IDC_GET_SMVINPUTS_INFO, &CSCLAnalysisDLLTestDlg::OnBnClickedGetSmvinputsInfo)
	ON_BN_CLICKED(IDC_SAVE_TEST, &CSCLAnalysisDLLTestDlg::OnBnClickedSaveTest)
END_MESSAGE_MAP()


// CSCLAnalysisDLLTestDlg 消息处理程序

BOOL CSCLAnalysisDLLTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CSCLAnalysisDLLTestDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_SCLANALYSISDLLTEST_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_SCLANALYSISDLLTEST_DIALOG));
	}
}
#endif


void CSCLAnalysisDLLTestDlg::OnBnClickedLoadFile()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;

	m_TestData = _T("");

	// 获取一个开始时间值
	DWORD dwStartTime = GetTickCount();

// 	std::wstring fileName(_T("\\FlashDisk2\\test.scd"));
//	std::wstring fileName(_T("\\FlashDisk\\WC1016B.scd"));
//	std::wstring fileName(_T("\\FlashDisk\\GuoWangMUTest.SCD"));
	std::wstring fileName(_T("\\FlashDisk2\\WC1016B_2.scd"));
	m_pSclCacheMgr->EnableGseAnalysis(true);
	m_pSclCacheMgr->EnableSmvAnalysis(true);
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

void CSCLAnalysisDLLTestDlg::OnBnClickedGetIedInfo()
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

	if ( smvNum > 0 )
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

void CSCLAnalysisDLLTestDlg::OnBnClickedGetDatasetInfo()
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

void CSCLAnalysisDLLTestDlg::DisplayIEDInfo(SCDSMV92ANALYSIS_IEDSMV_INFO *iedInfo)
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
	str.Format(_T("SMVCtrl_smvOptSmpSync:%d\r\n"),iedInfo->smvCbSmpSync);
	m_TestData += str;
	str.Format(_T("SMVCtrl_smvOptSecurity:%d\r\n\r\n"),iedInfo->smvCbSecurity);
	m_TestData += str;
}

void CSCLAnalysisDLLTestDlg::DisplayDataSetInfo(SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo)
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
	str.Format(_T("lnDesc:%s\r\n"),dataSetInfo->lnDesc.c_str());
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

void CSCLAnalysisDLLTestDlg::DisplayGOOSEIEDInfo(SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedInfo)
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

void CSCLAnalysisDLLTestDlg::DisplayGOOSEDataSetInfo(SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo)
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

void CSCLAnalysisDLLTestDlg::DisplayInputsExtRefInfo(SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO *inputsInfo)
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

void CSCLAnalysisDLLTestDlg::DisplayInputsExtRefInfo(SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO *inputsInfo)
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

void CSCLAnalysisDLLTestDlg::OnBnClickedLoadFileGse()
{
	// TODO: 在此添加控件通知处理程序代码
	// 获取一个开始时间值
	CString  str;

	m_TestData = _T("");

	DWORD dwStartTime = GetTickCount();

 	std::wstring fileName(_T("\\FlashDisk2\\test.scd"));
//	std::wstring fileName(_T("\\FlashDisk2\\WC1016B_2.scd"));
//	std::wstring fileName(_T("\\FlashDisk\\WC1016B.scd"));
//	std::wstring fileName(_T("\\FlashDisk\\GuoWangMUTest.SCD"));
	m_pSclCacheMgr->EnableGseAnalysis(false);
	m_pSclCacheMgr->EnableSmvAnalysis(false);
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

void CSCLAnalysisDLLTestDlg::OnBnClickedGetIedgseInfo()
{
	// TODO: 在此添加控件通知处理程序代码
	// 获取一个开始时间值
	CString  str;
	SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedInfo = NULL;
	int gooseNum;
	unsigned char macAddr[6] = {0x01,0x0C,0xCD,0x01,0x10,0x80};
	unsigned short appID = 0x1080;
// 	unsigned char macAddr[6] = {0x01,0x0C,0xCD,0x01,0x00,0x03};
// 	unsigned short appID = 0x0003;

	m_TestData = _T("");

	DWORD dwStartTime = GetTickCount();

	gooseNum = m_GOOSEAnalysis->GetIEDGOOSENum();
	str.Format(_T("GOOSENum:%d\r\n"),gooseNum);
	m_TestData += str;

	if ( gooseNum > 0  )
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

		delete[] iedInfo;
	}

	// 获取一个执行结束时间值，得到总的执行时间值
	DWORD dwFinishTime = GetTickCount();
	str.Format(_T("%d "), (DWORD)(dwFinishTime-dwStartTime)); 
	m_ReadTime = str;
	UpdateData(FALSE);
}

void CSCLAnalysisDLLTestDlg::OnBnClickedGetGsedatasetInfo()
{
	// TODO: 在此添加控件通知处理程序代码
	// 获取数据集信息
	CString  str;
	SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo = NULL;
	int fcdaNum;
	int ideSeq;

	unsigned char macAddr[6] = {0x01,0x0C,0xCD,0x01,0x10,0x80};
	unsigned short appID = 0x1080;
// 	unsigned char macAddr[6] = {0x01,0x0C,0xCD,0x01,0x00,0x03};
// 	unsigned short appID = 0x0003;

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
				DisplayGOOSEDataSetInfo(&dataSetInfo[i]);
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
		delete[] dataSetInfo;
	}

	// 获取一个执行结束时间值，得到总的执行时间值
	DWORD dwFinishTime = GetTickCount();
	str.Format(_T("%d "), (DWORD)(dwFinishTime-dwStartTime)); 
	m_ReadTime = str;
	m_WriteTime  = _T("0");
	m_SaveTime = _T("0");
	UpdateData(FALSE);

}

void CSCLAnalysisDLLTestDlg::OnBnClickedGetGseinputsInfo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString  str;
	SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO *inputsInfo = NULL;
	SCDGOOSEANALYSIS_IEDGOOSE_INFO iedInfo;
	int inputsNum = 0;
	int goInNum = 0;

	m_TestData = _T("");

	DWORD dwStartTime = GetTickCount();

	inputsNum = m_GOOSEAnalysis->GetOneIEDInputsExtRefNum(_T("DMU833_220"));

	if ( inputsNum > 0 )
	{
		inputsInfo = new SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO[inputsNum];
		goInNum = m_GOOSEAnalysis->GetOneIEDGOINExtRef(_T("DMU833_220"),inputsInfo,inputsNum);
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

		delete[] inputsInfo;
	}

	// 获取一个执行结束时间值，得到总的执行时间值
	DWORD dwFinishTime = GetTickCount();
	str.Format(_T("%d "), (DWORD)(dwFinishTime-dwStartTime)); 
	m_ReadTime = str;
	m_WriteTime  = _T("0");
	m_SaveTime = _T("0");
	UpdateData(FALSE);
}

void CSCLAnalysisDLLTestDlg::OnBnClickedGetSmvinputsInfo()
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

		delete[] inputsInfo;
	}

	// 获取一个执行结束时间值，得到总的执行时间值
	DWORD dwFinishTime = GetTickCount();
	str.Format(_T("%d "), (DWORD)(dwFinishTime-dwStartTime)); 
	m_ReadTime = str;
	m_WriteTime  = _T("0");
	m_SaveTime = _T("0");
	UpdateData(FALSE);
}

void CSCLAnalysisDLLTestDlg::OnBnClickedSaveTest()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
// 	std::wstring fileName(_T("\\FlashDisk2\\2012102401leigu.scd"));
// 	std::wstring fileName(_T("\\FlashDisk2\\220kV桐子湾变201303081044.scd"));
// 	std::wstring fileName(_T("\\FlashDisk\\WC1016B.scd"));
//	std::wstring fileName(_T("\\FlashDisk\\GuoWangMUTest.SCD"));
	std::wstring fileName(_T("\\FlashDisk2\\WC1016B_2.scd"));
	m_TestData = _T("");

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
