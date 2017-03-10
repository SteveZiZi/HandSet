#include "StdAfx.h"
#include <stdlib.h>
#include "testBase.h"
#include "Ft3Test.h"
#include "iniFile.h"
#include "assertMsg.h"

CFt3Test::CFt3Test(void)
{
}

CFt3Test::CFt3Test( HANDLE hDevice, UINT16 uDeviceItem)
{
    
    m_pFt3 = new _FT3_c(hDevice, uDeviceItem);

    if(DEVICE_FT3_RX1 == uDeviceItem)
    {
        if (m_pFt3->Init( L"DSM_FT3_RX1", L"ShareFT3_RX1", FT3_MAP_FILE_LEN, ft3ThreadProcRx1) == FALSE) {
            RETAILMSG(ZONE_ERROR, (_T("APP: m_Ft3Rx1.Init failed\r\n")));
        }
    }
    else if(DEVICE_FT3_RX2 == uDeviceItem)
    {

        if (m_pFt3->Init( L"DSM_FT3_RX2", L"ShareFT3_RX2", FT3_MAP_FILE_LEN, ft3ThreadProcRx2) == FALSE) {
            RETAILMSG(ZONE_ERROR, (_T("APP: m_Ft3Rx2.Init failed\r\n")));
        }
    }
  
}

CFt3Test::~CFt3Test(void)
{
    if(m_pFt3 != NULL)
    {
        m_pFt3->Ft3RxStop();
        delete m_pFt3;
    }
}

BOOL CFt3Test::Init( const WCHAR *strIniFile, int index,CTestManager *pManager )
{
    WCHAR strSection[]=L"Ft3Test_\0\0";
    strSection[8]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"Ft3收发",
        m_strTestName,31,strIniFile);
    m_pManager = pManager;
    if(index == 0)
    {
        wcscpy(m_reportNape[0].strNape,L"FT3_RX1接收报文总帧数");
        wcscpy(m_reportNape[1].strNape,L"FT3_RX1接收错误报文帧数");
        wcscpy(m_reportNape[2].strNape,L"FT3_RX1出错率");
    }
    else
    {
        wcscpy(m_reportNape[0].strNape,L"FT3_RX2接收报文总帧数");
        wcscpy(m_reportNape[1].strNape,L"FT3_RX2接收错误报文帧数");
        wcscpy(m_reportNape[2].strNape,L"FT3_RX2出错率");
    }
    
    wcscpy(m_reportNape[3].strNape,L"持续时间"); 

    m_uTotalFrame = 0;
    m_uErrorFrame = 0;
    m_iTimeOut = 0;
    return TRUE;
}

TEST_SITUATION CFt3Test::ExcuteOneTest()
{ 
    m_pFt3->Ft3Rx();
    ResetEvent(m_pFt3->m_hRxEvent);
    bool bTestRst = TRUE;
    DWORD dwRst;
   
    dwRst = WaitForSingleObject(m_pFt3->m_hRxEvent, RECV_TIME_OUT);
      

    if(dwRst == WAIT_OBJECT_0)
    {
        if(!m_pFt3->VerifySmvFrame())
        {
            bTestRst = FALSE;
        }
     
        m_pFt3->GetSmvVertifyRst(m_uTotalFrame, m_uErrorFrame);   
    }
    else 
    {
        if(m_iTimeOut++ < 5)
        {
            return SITUATION_NORMAL;
        }
        bTestRst = FALSE;
    }
    
    m_uiTestCount++;

    if(bTestRst)
    {
        return SITUATION_NORMAL;
    }
    else
    {
        m_uiErrCount++;
        return SITUATION_ERROR;
    }
 
}

const SReportNape * CFt3Test::GetReportNapes( int &count )
{
    unsigned int h,m,s;
    UINT64 t=m_uiTestTime;
    count=4;


    swprintf(m_reportNape[0].strNapeDescriptor,L"%d帧",
        m_uTotalFrame);
    m_reportNape[0].bHighLight=FALSE;  

    swprintf(m_reportNape[1].strNapeDescriptor,L"%d帧",m_uErrorFrame);


    if (m_uiTestCount >0 && m_uTotalFrame > 0)
    {
        swprintf(m_reportNape[2].strNapeDescriptor,L"%.2f%%",100 * (double)m_uErrorFrame / m_uTotalFrame); 
    }
    else
    {
        swprintf(m_reportNape[2].strNapeDescriptor,L"0.0%% ");
    }

    if (m_uErrorFrame > 0)
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

    swprintf(m_reportNape[3].strNapeDescriptor,L"%d时%d分%d秒",h,m,s);
    m_reportNape[3].bHighLight=FALSE;

    return m_reportNape;
}

