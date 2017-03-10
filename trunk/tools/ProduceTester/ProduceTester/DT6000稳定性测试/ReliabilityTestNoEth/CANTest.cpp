#include "stdafx.h"
#include <stdlib.h>
#include "testBase.h"
#include "CANTest.h"
#include "iniFile.h"
#include "CANLib.h"
#include "assertMsg.h"

CCANTest::CCANTest():
m_bInitError(TRUE),
m_testCanID1(0),
m_testCanID2(0)
{
    m_strTestName[0]=0;

    wcscpy(m_reportNape[0].strNape,L"���Դ���");
    wcscpy(m_reportNape[1].strNape,L"�������");
    wcscpy(m_reportNape[2].strNape,L"��ȷ��");
    wcscpy(m_reportNape[3].strNape,L"����ʱ��"); 
}

CCANTest::~CCANTest()
{
    if (!m_bInitError)
    {
        VCI_CloseDevice(VCI_WINCE_ONBOARD,m_testCanID1);
        if (m_testCanID1 !=m_testCanID2)
        {
            VCI_CloseDevice(VCI_WINCE_ONBOARD,m_testCanID2);
        }
        m_testCanID1=0;
        m_testCanID2=0;
        m_bInitError=TRUE;
    }

    VCI_UnLoadLib();
}

BOOL CCANTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"CANTest_\0\0";
    strSection[wcslen(strSection)]=L'1'+index;

    //��ȡ�����������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"CAN1",
        m_strTestName,31,strIniFile);

    //��ȡ���Եķ�ʽ�����Է����գ����ǶԷ�����
    int way;
    way=GetPrivateProfileInt(strSection,L"TestWay",0,strIniFile);
    //��ȡҪ���ԵĴ��ڱ��1
    m_testCanID1=GetPrivateProfileInt(strSection,L"TestCan1",0,strIniFile);

    if (way !=0)
    {
        //����ǶԷ����գ���ȡҪ���ԵĴ��ڱ��2
        m_testCanID2=GetPrivateProfileInt(strSection,L"TestCan2",0,strIniFile);
    }
    else
    {
        //������Է����յĻ������ڱ��2��1һ��
        m_testCanID2=m_testCanID1;
    }

    m_pManager = pManager;
    m_bDirection=FALSE;
    return InitDevCan();
}

TEST_SITUATION CCANTest::ExcuteOneTest()
{
    //������ʽ��ʼ����CAN�Ĳ���
    DWORD dwSndID,dwRevID;
    int   nRet;

    TEST_SITUATION      siRet=SITUATION_NORMAL;

    if (m_bDirection)
    {
        dwSndID=m_testCanID1;
        dwRevID=m_testCanID2;
    }
    else
    {
        dwSndID=m_testCanID2;
        dwRevID=m_testCanID1;
    }
    m_bDirection=!m_bDirection;

    VCI_CAN_OBJ			SendObj;
    VCI_CAN_OBJ			RecvObj[2];
    VCI_ERR_INFO		ei;
    int                 i;
    DWORD               dwLastTickCnt;

    //��䷢������
    SendObj.RemoteFlag = 0;
    SendObj.ExternFlag = 0;
    SendObj.SendType=0;
    SendObj.ID = 0;
    SendObj.DataLen = (BYTE)8;
    for (i=0;i<8;i++)
    {
        SendObj.Data[i]=rand()&0xFF;
    }
    if (m_testCanID1 ==m_testCanID2)
    {
        SendObj.SendType=2;
    }

    nRet=VCI_Transmit(VCI_WINCE_ONBOARD, dwSndID, 0, &SendObj, 1);
    if (nRet <=0)
    {
        siRet=SITUATION_ERROR;
        swprintf( m_strLastError,L"CAN%d��������ʧ��",dwSndID);
        VCI_ReadErrInfo(VCI_WINCE_ONBOARD, dwSndID, 0, &ei);
        goto Excute_End;
        
    }

    dwLastTickCnt = ::GetTickCount();
    DWORD dwTickDif = 0;
    do 
    {
        int len = VCI_Receive(VCI_WINCE_ONBOARD,dwRevID, 0, RecvObj, 2, 50);
        if (len <= 0)
        {
            //ע�⣺���û�ж��������������ô˺�������ȡ����ǰ�Ĵ����룬
            //ǧ����ʡ����һ������ʹ����ܲ���֪����������ʲô��
            VCI_ReadErrInfo(VCI_WINCE_ONBOARD, dwRevID, 0, &ei);
        }
        else 
        {
            //���յ����ݣ�ȷ�������Ƿ�һ��
            if( memcmp(RecvObj[0].Data,SendObj.Data,8) ==0 )
            {
                siRet =SITUATION_NORMAL;                
            }
            else
            {
                siRet=SITUATION_ERROR;
                swprintf(m_strLastError,L"CAN%d�յ���������CAN%d���͵����ݲ�һ��",
                    dwRevID,dwSndID);
            }
            goto Excute_End;
        }

    } while ((dwTickDif = ::GetTickCount() - dwLastTickCnt) < 1500);
    
    siRet=SITUATION_ERROR; 
    swprintf(m_strLastError,L"CAN%dû���յ�CAN%d���͵�����",
        dwRevID,dwSndID);   

Excute_End:
    m_uiTestCount++;
    if (siRet !=SITUATION_NORMAL)
    {
        m_uiErrCount++;
    }
    return siRet;
}

BOOL CCANTest::InitDevCan()
{
    VCI_INIT_CONFIG cfg;
    BOOL        bOpened1=FALSE,bOpened2=FALSE;
    WCHAR       buf[128];


    if (!VCI_LoadLib())
    {
        MessageBox(NULL,L"����ControlCAN.dllʧ��",L"������",MB_OK|MB_ICONERROR);
        return FALSE;
    }   
    cfg.AccCode = 0;
    cfg.AccMask = 0xffffffff;
    cfg.Filter = 0;
    cfg.Mode = 0;
    cfg.Timing0 = 0x00;
    cfg.Timing1 = 0x14;    
 

    //�򿪲���CAN1
    if (!VCI_OpenDevice(VCI_WINCE_ONBOARD, m_testCanID1, 0)) 
    {
        swprintf(buf,L"��CAN%dʧ��",m_testCanID1+1);
        MessageBox(NULL,buf,L"���ش���",MB_OK|MB_ICONERROR);
        goto Init_ERR;
    }
    bOpened1=TRUE;

    if (!VCI_InitCAN(VCI_WINCE_ONBOARD, m_testCanID1, 0, &cfg)) 
    {
        swprintf(buf,L"��ʼ��CAN%dʧ��",m_testCanID1+1);
        MessageBox(NULL,buf,L"���ش���",MB_OK|MB_ICONERROR);
        goto Init_ERR;
    }

    if (!VCI_StartCAN(VCI_WINCE_ONBOARD, m_testCanID1, 0))
    {
        swprintf(buf,L"����CAN%dʧ��",m_testCanID1+1);
        MessageBox(NULL,buf,L"���ش���",MB_OK|MB_ICONERROR);
        goto Init_ERR;
    }

    //�򿪲���CAN2
    if (m_testCanID1 !=m_testCanID2)
    {
        if (!VCI_OpenDevice(VCI_WINCE_ONBOARD, m_testCanID2, 0)) 
        {
            swprintf(buf,L"��CAN%dʧ��",m_testCanID2+1);
            MessageBox(NULL,buf,L"���ش���",MB_OK|MB_ICONERROR);
            goto Init_ERR;
        }
        bOpened2=TRUE;

        if (!VCI_InitCAN(VCI_WINCE_ONBOARD, m_testCanID2, 0, &cfg)) 
        {
            swprintf(buf,L"��ʼ��CAN%dʧ��",m_testCanID2+1);
            MessageBox(NULL,buf,L"���ش���",MB_OK|MB_ICONERROR);
            goto Init_ERR;
        }

        if (!VCI_StartCAN(VCI_WINCE_ONBOARD, m_testCanID2, 0))
        {
            swprintf(buf,L"����CAN%dʧ��",m_testCanID2+1);
            MessageBox(NULL,buf,L"���ش���",MB_OK|MB_ICONERROR);
            goto Init_ERR;
        }			
    }

    return TRUE;
Init_ERR:
    if (bOpened1)
    {
        VCI_CloseDevice(VCI_WINCE_ONBOARD,m_testCanID1);
    }

    if (bOpened2)
    {
        VCI_CloseDevice(VCI_WINCE_ONBOARD,m_testCanID2);
    }
    return FALSE;
}

const SReportNape *CCANTest::GetReportNapes(int &count)
{
    count =4;
    unsigned int h,m,s;
    UINT64 t=m_uiTestTime;

    swprintf(m_reportNape[0].strNapeDescriptor,L"%d��",m_uiTestCount);
    m_reportNape[0].bHighLight=FALSE;

    swprintf(m_reportNape[1].strNapeDescriptor,L"%d��",m_uiErrCount);    
    if (m_uiTestCount >0)
    {
        swprintf(m_reportNape[2].strNapeDescriptor,L"%.2f%% ",
            100 *(double)(m_uiTestCount-m_uiErrCount)/m_uiTestCount );
    }
    else
    {
        swprintf(m_reportNape[2].strNapeDescriptor,L"0.0%% ");
    }

    if (m_uiErrCount >0)
    {
        m_reportNape[1].bHighLight=TRUE;
        m_reportNape[2].bHighLight=TRUE;
    }
    else
    {
        m_reportNape[1].bHighLight=FALSE;
        m_reportNape[2].bHighLight=FALSE;
    }  

    h= (unsigned int)( t/(1000*3600));
    t=t%(1000*3600);
    m=(unsigned int) (t/(1000 *60));
    t=t%(1000 *60);
    s=(unsigned int)(t/1000);

    swprintf(m_reportNape[3].strNapeDescriptor,L"%dʱ%d��%d��",h,m,s);
    m_reportNape[3].bHighLight=FALSE;

    return m_reportNape;
}