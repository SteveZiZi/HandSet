// SampleSyncTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SampleSyncTest.h"
#include "SampleSyncTestDlg.h"
#include <math.h>
#include "src\service\datalayer\recv\FrameRecvStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define INTERP_CAL_COUNT 1000

// CSampleSyncTestDlg �Ի���

CSampleSyncTestDlg::CSampleSyncTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSampleSyncTestDlg::IDD, pParent)
	, m_csTestData(_T(""))
	, m_iSmpSyncCost(0)
	, m_dSampleSpace(1)
	, m_dSampleT0(0)
	, m_nInterpType(INTERP_TYPE_LINEAR)
	, m_nCtrl2Delay(0)
	, m_bIsDelayCompensate(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSampleSyncTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEST_DATA, m_csTestData);
	DDX_Text(pDX, IDC_SMP_SYNC_COST2, m_iSmpSyncCost);
	DDX_Text(pDX, IDC_SAMPLE_SPACE, m_dSampleSpace);
	DDX_Text(pDX, IDC_SAMPLE_T0, m_dSampleT0);
	DDX_CBIndex(pDX, IDC_INTERP_TYPE, m_nInterpType);
	DDX_Text(pDX, IDC_CTRL2_DELAY, m_nCtrl2Delay);
	DDX_Check(pDX, IDC_DELAY_COMPENSATE_CHECK, m_bIsDelayCompensate);
}

BEGIN_MESSAGE_MAP(CSampleSyncTestDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_INTERP, &CSampleSyncTestDlg::OnBnClickedInterp)
	ON_BN_CLICKED(IDC_SMV_INTERP, &CSampleSyncTestDlg::OnBnClickedSmvInterp)
END_MESSAGE_MAP()


// CSampleSyncTestDlg ��Ϣ�������

BOOL CSampleSyncTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_nInterpType = INTERP_TYPE_LINEAR;
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CSampleSyncTestDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_SAMPLESYNCTEST_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_SAMPLESYNCTEST_DIALOG));
	}
}
#endif

#define PI 3.14159265

void CSampleSyncTestDlg::OnBnClickedInterp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	CString str;
	m_csTestData = _T("");

	const int smpCount = 83;                  // ��������
	int dt = 250;                             // ����������ʱ������us
	int t_error = 10;                         // ���ͼ���������ʱ����us
	double Arrayd_Y[smpCount];
	double Arrayd_X[smpCount];
	float  resultY[smpCount];
	for ( int i=0; i<smpCount; i++ )
	{
		if ( i == 0 )
			Arrayd_X[i] = rand()%t_error;
		else
			Arrayd_X[i] = Arrayd_X[i-1] + dt - 10 + (rand()%(2*t_error)+1);
		Arrayd_Y[i] = 57.735*sqrt(2.0)*sin(2.0*PI*50.0*(Arrayd_X[i]/1000000.0));
		//57.735*220000.0/100.0*sqrt(2.0)*sin(2.0*PI*50.0*(1.0/50.0/double(smpCount)*double(i))/*+2.0/3.0*PI*/)*100.0;
	}

	interp_func pInterpFunc;
	switch ( INTERP_TYPE(m_nInterpType) )
	{
	case INTERP_TYPE_LINEAR:
		pInterpFunc = Interp_Linear;
		break;
	case INTERP_TYPE_QUADRATE:
		pInterpFunc = Interp_Quadrate;
		break;
	case INTERP_TYPE_THRICE:
		pInterpFunc = Interp_Thrice;
		break;
	case INTERP_TYPE_SPLINE:
		pInterpFunc = Interp_Spline;
		break;
	default:
		pInterpFunc = Interp_Linear;
	}

	DWORD st = GetTickCount();
	m_dSampleT0 = 10;
	m_dSampleSpace = 800;
	int resmpsNum = 0;
	for ( int i=0; i<INTERP_CAL_COUNT; i++ )
	{
		resmpsNum = (*pInterpFunc)(Arrayd_X,Arrayd_Y,smpCount,m_dSampleT0,m_dSampleSpace,resultY);
	}	
	DWORD et = GetTickCount();
	m_iSmpSyncCost = et-st;

	str.Format(_T("�ز�������Ŀ��%d\r\n"),resmpsNum);
	m_csTestData += str;
	for(int i = 0; i < smpCount; i++)
	{
		str.Format(_T("%06f %06f  %06f\r\n"),Arrayd_X[i],Arrayd_Y[i],resultY[i]);
		m_csTestData += str;
	}	
	UpdateData(FALSE);
}

void CSampleSyncTestDlg::OnBnClickedSmvInterp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	CString str;
	m_csTestData = _T("");

	const int syncChannelNum = 6;
	const uint32_t nSignalFrequency = 50;                  // ԭ���ε�Ƶ��
	const uint32_t nSampleFrequency = 4000;               // ԭ����Ƶ��
	const int smpCount = nSampleFrequency/nSignalFrequency;// ÿ���ڲ�������
	uint32_t nReampleFrequency = 4000;                    // �ز���Ƶ��
	double dt = 1000000.0/(double)nSampleFrequency;        // ����������ʱ������us
	int t_error = 10;                                      // ���ͼ���������ʱ����us
	int delay = m_nCtrl2Delay;
	int channelsPerCtrl = 3;
	SMV_VALUE ctrl1SMV;
	SMV_VALUE ctrl2SMV;
	float *resultY = new float[syncChannelNum*smpCount];
	uint32_t channelIdx[3] = {0,1,2};

	// ����ͬ������
	CSampleInterpSync smpSync;
	smpSync.SetInterpType(INTERP_TYPE(m_nInterpType));
	smpSync.SetDelayCompensateEnable(m_bIsDelayCompensate);
	smpSync.SetResampleConfig(nSignalFrequency,nSampleFrequency,nReampleFrequency);
	smpSync.SetRatedDelay(0,0);
	smpSync.SetRatedDelay(1,delay);
	smpSync.SetSyncChannelIdx(0,channelIdx,3);
	smpSync.SetSyncChannelIdx(1,channelIdx,3);

	m_iSmpSyncCost = 0;
	ctrl1SMV.stFrameTime.ts_sec = 1;
	ctrl2SMV.stFrameTime.ts_sec = 1;
	for ( int i = 0; i < INTERP_CAL_COUNT; i++ )
	{
		// ����������ֵ
		if ( i == 0 )
			ctrl1SMV.stFrameTime.ts_usec = rand()%t_error;
		else
			ctrl1SMV.stFrameTime.ts_usec = ctrl1SMV.stFrameTime.ts_usec + dt - 10 + (rand()%(2*t_error)+1);
		for ( int j=0; j < channelsPerCtrl; j++ )
		{
			ctrl1SMV.fSmvValue[j] = 57.735*sqrt(2.0)*sin(2.0*PI*50.0*(ctrl1SMV.stFrameTime.ts_usec/1000000.0));
		}
		ctrl1SMV.stFrameTime.ts_sec += ctrl1SMV.stFrameTime.ts_usec / 1000000;
		ctrl1SMV.stFrameTime.ts_usec = ctrl1SMV.stFrameTime.ts_usec % 1000000;

		if ( i == 0 )
			ctrl2SMV.stFrameTime.ts_usec = rand()%t_error + delay;
		else
			ctrl2SMV.stFrameTime.ts_usec = ctrl2SMV.stFrameTime.ts_usec + dt - 10 + (rand()%(2*t_error)+1);
		for ( int j=0; j < channelsPerCtrl; j++ )
		{
			ctrl2SMV.fSmvValue[j] = 57.735*sqrt(2.0)*sin(2.0*PI*50.0*((ctrl2SMV.stFrameTime.ts_usec-delay)/1000000.0));
		}
		ctrl2SMV.stFrameTime.ts_sec += ctrl2SMV.stFrameTime.ts_usec / 1000000;
		ctrl2SMV.stFrameTime.ts_usec = ctrl2SMV.stFrameTime.ts_usec % 1000000;

		// ����ͬ��
		DWORD st = GetTickCount();

		smpSync.AddASample(0,ctrl1SMV);
		if ( smpSync.IsSampleSync() )
		{
			smpSync.GetSyncSamples(resultY,syncChannelNum,smpCount);
		}

		smpSync.AddASample(1,ctrl2SMV);
		if ( smpSync.IsSampleSync() )
		{
			smpSync.GetSyncSamples(resultY,syncChannelNum,smpCount);
		}

		DWORD et = GetTickCount();
		m_iSmpSyncCost += et-st;
	}

	// ��ʾ���
	str.Format(_T("���ƿ�1��\r\n"));
	m_csTestData += str;
	for(int i = 0; i < smpCount; i++)
	{
		str.Format(_T("%6f %6f\r\n"),resultY[i],resultY[smpCount+i]);
		m_csTestData += str;
	}

	str.Format(_T("\r\n���ƿ�2��\r\n"));
	m_csTestData += str;
	for(int i = 0; i < smpCount; i++)
	{
		str.Format(_T("%6f %6f\r\n"),resultY[3*smpCount+i],resultY[4*smpCount+i]);
		m_csTestData += str;
	}

	delete[] resultY;
	UpdateData(FALSE);
}
