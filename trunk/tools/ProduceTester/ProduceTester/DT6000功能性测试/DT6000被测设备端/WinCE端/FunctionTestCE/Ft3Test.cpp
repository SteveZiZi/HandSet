#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "iniFile.h"
#include "assertMsg.h"
#include "Ft3Test.h"


CFt3Test::CFt3Test(void)
{
}

CFt3Test::CFt3Test( HANDLE hDevice, UINT8 uDeviceItem )
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
        delete m_pFt3;
    }
}

BOOL CFt3Test::Init( const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager/*=NULL*/ )
{
    WCHAR strSection[]=L"UseAppFt3RxTest_\0\0";
    strSection[16]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"Ft3",
        m_strTestName,31,strIniFile);

    m_uiCmd=index+CMD_TEST_USE_FT3_RX1;
    *uipCmdIndex = m_uiCmd;
    return TRUE;
}

TEST_STATUS CFt3Test::ProcessExcuteCmd( UINT16 param,UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect )
{
    m_ti.bError=0;
    m_ti.uiErrorLen=0;
    int nRet;

    switch(param)
    {
    case FRAME_TEST_CMD_INI:
        nRet = BeginFt3Test();
        break;
    case FRAME_TEST_CMD_EXT:
        nRet = ExcuteFt3Test();
        break;
    case FRAME_TEST_CMD_END:
        nRet = EndFt3Test();
        break;
    }

   
    TEST_INFO_TRANSFER tit;
    TestInfoLocalToTransfer(&tit,&m_ti);
  
    nRet =pConnect->SendAPacket(m_uiCmd,param,sizeof(tit),(BYTE *)&tit);

    return nRet >0?TEST_NORMAL:TEST_CONNECTDOWN;
}

TEST_STATUS CFt3Test::EndFt3Test()
{
    //停止接收FT3报文
    m_pFt3->Ft3RxStop();   
    return TEST_NORMAL;
}

TEST_STATUS CFt3Test::BeginFt3Test()
{  

    //开始探测FT3报文
    if(m_pFt3->Ft3Rx())
    {
        return TEST_NORMAL;
    }
    else
    {
        return TEST_ERROR;
    }
   
}

TEST_STATUS CFt3Test::ExcuteFt3Test()
{
    DWORD dwRst;
    ResetEvent(m_pFt3->m_hRxEvent);
    dwRst = WaitForSingleObject(m_pFt3->m_hRxEvent, 1000);
    
    //如果接收到FT3报文
    if(dwRst == WAIT_OBJECT_0)
    {
        if(!m_pFt3->VerifySmvFrame())
        {
            m_ti.bError = FRAME_RECV_ERROR;
            m_ti.uiErrorLen=swprintf(m_ti.strErr, STR_FRAME_RECV_ERROR);
            return TEST_ERROR;

        }
        ResetEvent(m_pFt3->m_hRxEvent);
    }
    else if(dwRst == WAIT_TIMEOUT)
    {
        m_ti.bError = FRAME_RECV_TIME_OUT;
        m_ti.uiErrorLen=swprintf(m_ti.strErr, STR_FRAME_RECV_TIME_OUT);
        return TEST_ERROR;
    }    
   
    return TEST_NORMAL;

}

_FT3_c * CFt3Test::GetRxFt3()
{
    return m_pFt3;
}
