// AudioTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "AudioTest.h"
#include "FunctionTest.h"
#include "AudioTestDlg.h"



// CAudioTestDlg dialog

IMPLEMENT_DYNAMIC(CAudioTestDlg, CDialog)

CAudioTestDlg::CAudioTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAudioTestDlg::IDD, pParent)
    , m_strAudioTime(_T(""))
{

}

CAudioTestDlg::~CAudioTestDlg()
{
}

void CAudioTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS_VOLUME, m_ctrlVolume);
    DDX_Text(pDX, IDC_AUDIO_TIME, m_strAudioTime);
}


BEGIN_MESSAGE_MAP(CAudioTestDlg, CDialog)
    ON_WM_TIMER()
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CAudioTestDlg message handlers
#define  ID_TIMER_CLOSE_WINDOW          1           //�رմ��ڵĶ�ʱ��
#define  ID_TIMER_REFLESH_AUDIO_INFO    2           //��ʱˢ����Ƶ��Ϣ

BOOL CAudioTestDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    m_bCloseNormal=TRUE;

    if (uiParamCmd ==PARAM_GET_POS_PLAY)
    {
        SetWindowText(_T("���ڲ�����Ƶ...") );
    }
    else
    {
        SetWindowText(_T("����¼��...") );
    }

    //���ùرհ�ť(Ҳ�����Ǹ���)
    CMenu *pSysMenu=GetSystemMenu(FALSE);
    pSysMenu->EnableMenuItem( SC_CLOSE, MF_BYCOMMAND|MF_DISABLED);

    SetTimer(ID_TIMER_CLOSE_WINDOW,10000,NULL);
    SetTimer(ID_TIMER_REFLESH_AUDIO_INFO,50,NULL);
    
    m_ctrlVolume.SetRange32(0,32768);
    m_ctrlVolume.SetPos(0);

    m_strAudioTime=_T("00:00:00.0");
    return TRUE;  
}

struct WAV_POS_INFO
{
    UINT32  bRun;                               //��ǰ�Ƿ����ڲ��Ż���¼��
    UINT64  uiSamples;                          //��ǰ�Ѿ����Ż���¼�Ƶĺ�����
    INT32   iVolume;                            //��ǰ���������������һ����-65536~65535
};

void CAudioTestDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent ==ID_TIMER_REFLESH_AUDIO_INFO)
    {
        WAV_POS_INFO *pWavPos;
        PACKET pa;

        int nRet=pConnect->SendAPacket(uiCmd,uiParamCmd,0,NULL);
        if (nRet >0)
        {
            nRet =pConnect->RecvAPacket(&pa,5000);
            if (nRet >0)
            {
                pWavPos=(WAV_POS_INFO *)pa.pData;
                m_ctrlVolume.SetPos( abs( pWavPos->iVolume) );

                UINT64 t=pWavPos->uiSamples;

                unsigned int h=(unsigned int )( t/(1000*3600) );
                t=t%(1000*3600);
                unsigned int m=(unsigned int)(t/60000);
                t=t%60000;
                unsigned int s=(unsigned int)( t/1000 );
                t=t%1000;
                m_strAudioTime.Format(L"%02d:%02d:%02d:%d",h,m,s,t/100);
                UpdateData(FALSE);

                return;
            }
        }
        status=TEST_CONNECTDOWN;
        m_bCloseNormal=FALSE;

        PostMessage(WM_CLOSE);

    }
    else
    {
        PostMessage(WM_CLOSE);
    }  

    CDialog::OnTimer(nIDEvent);
}

void CAudioTestDlg::OnDestroy()
{
    KillTimer(ID_TIMER_REFLESH_AUDIO_INFO);
    KillTimer(ID_TIMER_CLOSE_WINDOW);

    if (m_bCloseNormal)
    {
        int nRet;
        if (uiParamCmd ==PARAM_GET_POS_PLAY)
        {
            nRet =MessageBox(_T("�Ƿ�����������"),_T("��Ƶ����"),MB_YESNO|MB_ICONQUESTION );
            if (nRet ==IDYES)
            {
                status =TEST_NORMAL;
            }
            else
            {
                status =TEST_ERROR;
            }
        }
        else
        {
            nRet =MessageBox(_T("�ڶ�����ͷ˵����ʱ���Ƿ񿴵������������б仯?"),
                _T("��Ƶ����"),MB_YESNO|MB_ICONQUESTION );
            if (nRet ==IDYES)
            {
                status =TEST_NORMAL;
            }
            else
            {
                status =TEST_ERROR;
            }
        }
    }
    else
    {

    }



    CDialog::OnDestroy();    
}
