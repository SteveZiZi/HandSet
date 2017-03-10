#include "stdafx.h"
#include "testBase.h"
#include "testManager.h"
#include "GPSTest.h"
#include "iniFile.h"

CGpsTest::CGpsTest()
{
    wcscpy(m_reportNape[0].strNape,L"���Դ���");
    wcscpy(m_reportNape[1].strNape,L"�������");
    wcscpy(m_reportNape[2].strNape,L"��ȷ��");
    wcscpy(m_reportNape[3].strNape,L"����ʱ��");

}

BOOL CGpsTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"GPSTest_\0\0";
    strSection[8]=L'1'+index;

    //��ȡ�����������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"GPS",
        m_strTestName,31,strIniFile); 

    m_nPort=GetPrivateProfileInt(strSection,L"SerialPort",0,strIniFile);

    m_nPort++;
    char sendbuf[] = {'a','t','+','g','p','s','s','t','o','p','\r','\n'};
    char revbuf[20];


    if (!m_xCom.Open(m_nPort,CBR_9600) )
    {
        MessageBox(NULL,L"��GPS����ʧ��",L"GPS",MB_OK|MB_ICONERROR);
        return FALSE;
    }

    size_t iWriteLen=0;
    m_xCom.WriteNByte(sendbuf,sizeof(sendbuf),&iWriteLen);
  
    m_xCom.Read(revbuf,sizeof(sendbuf),&iWriteLen);
    m_pManager = pManager;
    return TRUE;
}

const SReportNape*CGpsTest::GetReportNapes(int &count)
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

TEST_SITUATION CGpsTest::ExcuteOneTest()
{
    m_uiTestCount++;
    

    m_xCom.ClearRecv();
    m_xCom.ClearCommError();
    size_t ReadLen;

    if (!m_xCom.ReadTimeOut(buf,2048,3000,&ReadLen) )
    {
        m_uiErrCount++;
        swprintf(m_strLastError,
            L"��ȡGPRS�������ݳ�ʱ" );
        return SITUATION_ERROR;
    }

    


    
    return SITUATION_NORMAL;
}