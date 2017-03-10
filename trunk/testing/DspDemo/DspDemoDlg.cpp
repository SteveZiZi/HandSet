/// @file
///
/// @brief
///     Demo窗口实现文件
///
/// @note
///     用于对DSP端的有效值算法的演示
///
/// Copyright (c) 2012 IUnknownBase Inc
///
/// 作者：
///    chao  2012.12.19
///
/// 修改历史：
///
// DspDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DspDemo.h"
#include "DspDemoDlg.h"
#include "pkfuncs.h"
#include "winioctl.h"
#include <assert.h>

#include "../../src/dspclient/include/Dsp.h"
#include "DspTest.h"
#include "DataManager.h"

///
/// @brief
///     CDspDemoDlg 对话框
///
IMPLEMENT_DYNAMIC(CDspDemoDlg, CDialog)

CDspDemoDlg::CDspDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDspDemoDlg::IDD, pParent)
	, cpu_info(_T(""))
	, dsp_speed(0)
    , str_msgdif(_T(""))
    , str_timedif(_T(""))
    , str_msgtime(_T(""))
    , str_time_smv10calc(_T(""))
    , str_time_smvgen(_T(""))
    , str_time_msgdisp(_T(""))
    , str_time_totaltime(_T(""))
	, data(800)
	, str_dsp(_T(""))
    , str_dsp2(_T(""))
    , m_fRefresh(false)
    , m_dwStartTime(0)
    , m_dwStopTime(0)
    , m_dwStartMsg(0)
    , m_dwStopMsg(0)
    , m_nMsgCount(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    bool isOK = DspInit();
    m_pDspTest = new CDspTest();

    pErr = new Error;
    pDspInfo = new DspInfo;
    pSmvData = new SmvData;
    pSmvCalcData = new SMV_DATA_CALC_RT;
    pSmvGenData = new SMV_GEN_DATA_CALC_RT;
    pMsgDisp = new float[11];
}

CDspDemoDlg::~CDspDemoDlg()
{
    delete m_pDspTest;
    DspRelease();
    delete pErr;
    delete pDspInfo;
    delete pSmvData;
    delete pSmvCalcData;
    //delete pSmvGenData;
    delete[] pMsgDisp;    
}

void CDspDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, cpu_info);
	DDX_Text(pDX, IDC_EDIT2, dsp_speed);
    DDX_Text(pDX, IDC_EDIT_MSGDIF, str_msgdif);
    DDX_Text(pDX, IDC_EDIT_TIMEDIF, str_timedif);
    DDX_Text(pDX, IDC_EDIT_SMV10CALC, str_time_smv10calc);
    DDX_Text(pDX, IDC_EDIT_MSGTIME, str_msgtime);
    DDX_Text(pDX, IDC_EDIT_SMVGEN, str_time_smvgen);
    DDX_Text(pDX, IDC_EDIT_MSGDISP, str_time_msgdisp);
    DDX_Text(pDX, IDC_EDIT_TOTALTIME, str_time_totaltime);
	DDX_Text(pDX, IDC_EDIT4, data);
	DDX_Text(pDX, IDC_EDIT5, str_dsp);
    DDX_Text(pDX, IDC_EDIT6, str_dsp2);
}

BEGIN_MESSAGE_MAP(CDspDemoDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDspDemoDlg::DspTest)
	ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTON2, &CDspDemoDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, &CDspDemoDlg::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_BUTTON4, &CDspDemoDlg::OnBnClickedButton4)
END_MESSAGE_MAP()

///
/// @brief
///     CDspDemoDlg 初始化
///
BOOL CDspDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	DWORD Speed = 0;
	PROCESSOR_INFO proInfo;

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
    
	// 获取DSP芯片信息
	KernelIoControl(IOCTL_HAL_GET_DSP_INFO, NULL, NULL, &Speed, sizeof(Speed), NULL);
	KernelIoControl(IOCTL_PROCESSOR_INFORMATION, NULL, NULL, &proInfo, sizeof(proInfo), NULL);
	dsp_speed = Speed;
	cpu_info = cpu_info +proInfo.szProcessorName;
	cpu_info = cpu_info +proInfo.szProcessCore;
	UpdateData(FALSE);
 
    SetTimer(IDTIMER1, 300, 0);
    return TRUE;
	//创建用户算法程序，下载用户可执行程序(.out文件到DSP端)并分配GPP和DSP通信共享数据区域
}


///
/// @brief
///     DSP Test开始
///
void CDspDemoDlg::DspTest()
{
	// TODO: Add your control notification handler code here

    // SmvCalc
    m_pDspTest->RunTest();
    m_fRefresh = true;
}

///
/// @brief
///     结束DSP Test
///
void CDspDemoDlg::OnDestroy()
{
	CDialog::OnDestroy();
	// TODO: Add your message handler code here
    KillTimer(IDTIMER1);
    m_pDspTest->ExitTest();    
}

void CDspDemoDlg::UpdateDspData()
{
    if(!m_fRefresh)
    {
        return;
    }
#if 1
    CString str;

    CDspDataSet &dspDataSet = CDataManager::GetInstance().GetDspDataSet();
    dspDataSet.GetError(*pErr);
    dspDataSet.GetDspInfo(*pDspInfo);
    //pSmvData = &dspDataSet.smvData;
    dspDataSet.GetCalcData(*pSmvCalcData);
    dspDataSet.GetGenCalcData(*pSmvGenData);
    dspDataSet.GetMsgDisp(pMsgDisp, 11*sizeof(float));

    static int lastDataCount = 0;
    
    //打印DSP端执行的结果
    str_dsp = "";
    str_dsp += _T("---------------接收数据计数---------------\r\n");
    m_nMsgCount = dspDataSet.GetDataCount();
    str.Format(_T("data count:%d\r\n"),m_nMsgCount); 
    str_dsp += str;

    str_dsp += _T("---------------接收数据数量差---------------\r\n");
    str.Format(_T("data count:%d\r\n"),m_nMsgCount-lastDataCount); 
    str_dsp += str;
    lastDataCount = m_nMsgCount;
#if 1
    str_dsp += _T("-----------------谐波（通道1）------------------\r\n");
    str.Format(_T("%f\t%f\r\n"), pSmvCalcData->fHarmContent[0][0],
        pSmvCalcData->fHarmAngle[0][0]);
    str_dsp +=str;
 
    str_dsp += _T("-----------------谐波（通道2）------------------\r\n");
    str.Format(_T("%f\t%f\r\n"), pSmvCalcData->fHarmContent[1][0],
        pSmvCalcData->fHarmAngle[1][0]);
    str_dsp +=str;

    str_dsp += _T("-----------------谐波（通道3）------------------\r\n");
    str.Format(_T("%f\t%f\r\n"), pSmvCalcData->fHarmContent[2][0],
        pSmvCalcData->fHarmAngle[2][0]);
    str_dsp +=str;
    
    str_dsp += _T("-----------------谐波（通道5）------------------\r\n");
    str.Format(_T("%f\t%f\r\n"), pSmvCalcData->fHarmContent[4][0],
        pSmvCalcData->fHarmAngle[4][0]);
    str_dsp +=str;

    str_dsp += _T("---------------Config Dsp---------------\r\n");
    str.Format(_T("configDsp[errno]:%d\r\n"),pErr->nErrno); 
    str_dsp += str;

    str_dsp += _T("----------------Dsp Info----------------\r\n");
    str.Format(_T("dspInfo[version]:%d\r\n"),pDspInfo->nVersion); 
    str_dsp += str;
#endif
    UpdateData(FALSE);

    str_dsp += _T("----------------报文离散度-------------\r\n");
    for(int i=0; i<11; i++)
    {
        str.Format(_T("msgDisp[%d]:%f\r\n"),i,pMsgDisp[i]); 
        str_dsp += str;
    }

    str_dsp += _T("-----------------序量------------------\r\n");
    for(int i=0; i<3; i++)
    {
        str.Format(_T("seq[%d]:\t%f,\t%f\r\n"),i,pSmvCalcData->fPhaseSeqContent[0][i],
            pSmvCalcData->fPhaseSeqAngle[0][i]); 
        str_dsp += str;
    }
    for(int i=0; i<3; i++)
    {
        str.Format(_T("seq1[%d]:\t%f,\t%f\r\n"),i,pSmvCalcData->fPhaseSeqContent[1][i],
            pSmvCalcData->fPhaseSeqAngle[1][i]); 
        str_dsp += str;
    }

    str_dsp += _T("-----------------功率-----------------\r\n");
    for(int i=0; i<3; i++)
    {
        str.Format(_T("Power[%d]->视在:\t%f\r\n"),i,
            pSmvCalcData->fApparentPower[i]);
        str_dsp += str;

        str.Format(_T("Power[%d]->P:\t%f\r\n"),i,
            pSmvCalcData->fActivePower[i]);
        str_dsp += str;

        str.Format(_T("Power[%d]->Q:\t%f\r\n"),i,
            pSmvCalcData->fReactivePower[i]);
        str_dsp += str;

        str.Format(_T("PowerFactor[%d]:\t%f\r\n"),i,
            pSmvCalcData->fPowerFactor[i]);
        str_dsp += str;
    }

    str_dsp2 = "";
    str_dsp2 += _T("-----------------有效值-----------------\r\n");
    for(int i=0; i<1; i++)
    {        
        str.Format(_T("effValue[%d]:\t%f\r\n"),i,pSmvCalcData->fRmsValue[i]); 
        str_dsp2 += str;
    }

    str_dsp2 += _T("-----------------频率-----------------\r\n");
    for(int i=0; i<2; i++)
    {        
        str.Format(_T("Frequency[%d]:\t%f\r\n"),i,pSmvCalcData->fFreq[i]); 
        str_dsp2 += str;
    }

    str_dsp2 += _T("-----------------核相-----------------\r\n");
    str.Format(_T("SPAngleDif[0]:\t%f\r\n"),pSmvCalcData->fSPAngleDif[0]); 
    str_dsp2 += str;

    str.Format(_T("SPVectorDif[0]:\t%f\t%f\r\n"),
        pSmvCalcData->fSPVectorDifContent[0], 
        pSmvCalcData->fSPVectorDifAngle[0]);
    str_dsp2 += str;
    
    str.Format(_T("IPVectorDif[0]:\t%f\t\t%f\r\n"),
        pSmvCalcData->fIPVectorDifContent[0],
        pSmvCalcData->fIPVectorDifAngle[0]);
    str_dsp2 += str;

    str_dsp2 += _T("-----------------谐波（通道1）------------------\r\n");
    str.Format(_T("dcValue[0]:\t%f\r\n"), pSmvCalcData->fDCValue[0]);
    str_dsp2 +=str;

    for (int i=0; i<10;i++)
    {
        str.Format(_T("harm[0][%d]:\t%f,\t%f\r\n"),i,
            pSmvCalcData->fHarmContent[0][i], pSmvCalcData->fHarmAngle[0][i]); 
        str_dsp2 += str;
    }
#if 1
    str_dsp2 += _T("-----------------谐波总THD------------------\r\n");
    str.Format(_T("harmSumTHD[%d]:\t%f\r\n"),0,
        pSmvCalcData->fSumHarmTHD[0]); 
    str_dsp2 += str;

    str_dsp2 += _T("-----------------直流THD------------------\r\n");
    str.Format(_T("DCTHD[%d]:\t%f\r\n"),0,
        pSmvCalcData->fDCTHD[0]); 

    str_dsp2 += str;
    str_dsp2 += _T("-----------------谐波THD------------------\r\n");
    for(int i=0; i<10; i++)
    {
        str.Format(_T("harmTHD[%d]:\t%f\r\n"),i,
            pSmvCalcData->fHarmTHD[0][i]); 
        str_dsp2 += str;
    }
#endif
#if 0
    str_dsp2 += _T("------------------波形生成|实际值----------------\r\n");
    for(UINT32 i=0; i<10; i++)
    {
        str.Format(_T("[%d]:%f\t\t%f\r\n"),i,pSmvGenData[i],
            smvDatafChannelData[0][i]); 
        str_dsp2 += str;
    }
#endif
#if 1
    str_dsp2 += _T("------------------波形生成（通道1）----------------\r\n");
    str.Format(_T("channel count:%d\r\n"),pSmvGenData->nChannelCount); 
    str_dsp2 += str;
    str.Format(_T("smv count:%d\r\n"),pSmvGenData->nSmvCount); 
    str_dsp2 += str;
    for(UINT32 i=0; i<10; i++)
    {
        str.Format(_T("[%d]:%f\r\n"),i,pSmvGenData->fSmvGenData[0][i]); 
        str_dsp2 += str;
    }
#endif
    /*
    for(UINT32 i=80; i<smvGenSize; i++)
    {
        str.Format(_T("[%d]:%f\r\n"),i,pSmvGenData[i]); 
        str_dsp2 += str;
    }*/
    UpdateData(FALSE);
#endif
}

void CDspDemoDlg::OnBnClickedButton2()
{
    // TODO: 在此添加控件通知处理程序代码
    m_pDspTest->ExitTest();
}

void CDspDemoDlg::OnBnClickedButton3()
{
    // TODO: 在此添加控件通知处理程序代码
    m_fRefresh = false;
    m_dwStopTime = GetTickCount();
    m_dwStopMsg = m_nMsgCount;
    str_timedif.Format(_T("%d"),m_dwStopTime-m_dwStartTime);
    str_msgdif.Format(_T("%d"), m_dwStopMsg - m_dwStartMsg);
    DWORD msgTime = (m_dwStopTime-m_dwStartTime)/(m_dwStopMsg - m_dwStartMsg);
    str_msgtime.Format(_T("%d"), msgTime);
    UpdateData(FALSE);
}

void CDspDemoDlg::OnBnClickedButton4()
{
    // TODO: 在此添加控件通知处理程序代码
    m_fRefresh = true;
    m_dwStartTime = GetTickCount();
    m_dwStartMsg = m_nMsgCount;
}

void CDspDemoDlg::OnTimer( UINT nIDEvent )
{
    switch(nIDEvent) 
    {
    case IDTIMER1:
        {   
            UpdateDspData();         
        }
        break;
    }

    CDialog::OnTimer(nIDEvent);
}
