#include "stdafx.h"

#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "canTest.h"
#include "assertMsg.h"
#include "canTest.h"
#include "iniFile.h"
#include "CANLib.h"

CCanTest::CCanTest()
    :m_hTestThread(NULL)
    ,m_bContinueTest(FALSE)
{

}

CCanTest ::~CCanTest()
{
    if (m_hTestThread !=NULL)
    {
        m_bContinueTest=FALSE;
        WaitForSingleObject(m_hTestThread,INFINITE);
        CloseHandle(m_hTestThread);
        m_hTestThread=NULL;
    }
    
    VCI_CloseDevice(VCI_WINCE_ONBOARD,m_dwCanDevId);
    
}

BOOL CCanTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"CANTest_\0\0";
    strSection[8]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"CAN1",m_strTestName,31,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_CAN1;
    *uipCmdIndex=m_uiCmd;
    m_dwCanDevId=index; 

    if ( !VCI_LoadLib())
    {
        MessageBox(NULL,_T("����controlCan.dllʧ�ܣ�������������ά����Ա��ϵ"),
            _T("���ش���"),MB_OK|MB_ICONERROR);
        return FALSE;
    }
    VCI_INIT_CONFIG vic={0};
    vic.AccCode = 0x00000000;
    vic.AccMask = 0xffffffff;
    vic.Timing0 = 0x00;
    vic.Timing1 = 0x14;


    if (!VCI_OpenDevice(VCI_WINCE_ONBOARD, m_dwCanDevId, 0)) 
    {
        WCHAR str[256];
        swprintf(str,L"��CAN%dʧ��",m_dwCanDevId+1);
        MessageBox(NULL,str,
            _T("���ش���"),MB_OK|MB_ICONERROR);
        return FALSE;
    }

    if (!VCI_InitCAN(VCI_WINCE_ONBOARD,  m_dwCanDevId, 0,&vic)) 
    {
        WCHAR str[256];
        swprintf(str,L"��ʼ��CAN%dʧ��",m_dwCanDevId+1);
        MessageBox(NULL,str,
            _T("���ش���"),MB_OK|MB_ICONERROR);      
        VCI_CloseDevice(VCI_WINCE_ONBOARD, m_dwCanDevId);
    }

    if (!VCI_StartCAN(VCI_WINCE_ONBOARD,  m_dwCanDevId, 0) ) 
    {
        WCHAR str[256];
        swprintf(str,L"����CAN%dʧ��",m_dwCanDevId+1);
        MessageBox(NULL,str,
            _T("���ش���"),MB_OK|MB_ICONERROR);

        VCI_CloseDevice(VCI_WINCE_ONBOARD, m_dwCanDevId);
    }

    m_bContinueTest=TRUE;
    m_hTestThread=CreateThread(NULL,0,TestProc,this,0,NULL );
    return m_hTestThread !=NULL;
}

DWORD CCanTest::TestProc(void *pVoid)
{
    CCanTest *pTest=(CCanTest *)pVoid;

    VCI_CAN_OBJ         canobj[50];
    VCI_ERR_INFO        ei;  
    DWORD               dwLastRcvErrs = 0;
    DWORD               dwLastSndErrs = 0;

    CString strErr;
    while(pTest ->m_bContinueTest )
    {
        int len = VCI_Receive(VCI_WINCE_ONBOARD, pTest->m_dwCanDevId,0,  canobj, 50, 20); //�������ݣ��ȴ�20ms
        if (len <= 0) 
        {
            //ע�⣺���û�ж��������������ô˺�������ȡ����ǰ�Ĵ����룬
            //ǧ����ʡ����һ������ʹ����ܲ���֪����������ʲô��
            VCI_ReadErrInfo(VCI_WINCE_ONBOARD, pTest->m_dwCanDevId,0,  &ei);

            if (ei.Passive_ErrData[2] != (BYTE)dwLastSndErrs) 
            {

                strErr.Format(_T("CAN1 SendErrCnt:%d\r\n"), ei.Passive_ErrData[2]);
                dwLastSndErrs = ei.Passive_ErrData[2];
                RETAILMSG(1, (strErr));
            }
            if (ei.Passive_ErrData[1] != (BYTE)dwLastRcvErrs) {
                strErr.Format(_T("CAN1 RecvErrCnt:%d\r\n"), ei.Passive_ErrData[1]);
                dwLastRcvErrs = ei.Passive_ErrData[1];

            }        
        } 
        else 
        {
            VCI_Transmit(VCI_WINCE_ONBOARD, pTest->m_dwCanDevId,0,  canobj, len);    //�ط��յ�������
        }
        Sleep(300);
    }

    return 0;
}