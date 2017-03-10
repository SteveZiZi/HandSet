#include "StdAfx.h"
#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "iniFile.h"
#include "assertMsg.h"
#include "Ft3TxTest.h"

#define FT3_GUOWANG_PATH     L".\\pcap\\GuoWang.txt"

CFt3TxTest::CFt3TxTest(void)
{
}

CFt3TxTest::CFt3TxTest( HANDLE hDevice, UINT8 uDeviceItem )
{
    m_pFt3 = new _FT3_c(hDevice, uDeviceItem);
    
    if(m_pFt3->Init( L"DSM_FT3_TX1", L"ShareFT3_TX1", FT3_TX_MAP_FILE_LEN) == FALSE) {
            RETAILMSG(1, (_T("APP: m_Ft3Tx.Init failed\r\n")));
        
    }
}

CFt3TxTest::~CFt3TxTest(void)
{
    if(m_pFt3 != NULL)
    {
        delete m_pFt3;
    }
    if(m_pRx1Ft3 != NULL)
    {
        delete m_pRx1Ft3;
    }
    if(m_pRx2Ft3 != NULL)
    {
        delete m_pRx2Ft3;
    }
}

BOOL CFt3TxTest::Init( const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager/*=NULL*/ )
{
    WCHAR strSection[]=L"UseAppFt3TxTest_\0\0";
    strSection[16]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"Ft3Tx",
        m_strTestName,31,strIniFile);

    m_uiCmd=index+CMD_TEST_USE_FT3_TX1;
    *uipCmdIndex = m_uiCmd;
    return TRUE;
}

TEST_STATUS CFt3TxTest::ProcessExcuteCmd( UINT16 param,UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect )
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

TEST_STATUS CFt3TxTest::BeginFt3Test()
{
    //初始化数据
    m_tCtrlW.wData = 0;
    m_tCtrlW.CtrlBit.bBaudRate = 1;
    m_tCtrlW.CtrlBit.bFrameStyle = _GUO_WANG;
    m_tCtrlW.CtrlBit.bSampleRate = 1;

    m_pFt3->SetFilePath(FT3_GUOWANG_PATH);
    m_pFt3->SetFT3TxInfo(m_tCtrlW);
    m_pFt3->SetFt3TxFrame();

    //发送FT3报文
    if(!m_pFt3->Ft3Tx())
    {
        m_ti.bError = FRAME_INIT_ERROR;
        m_ti.uiErrorLen=swprintf(m_ti.strErr, STR_FRAME_INIT_ERROR);
        return TEST_ERROR;
    };   
    
    Sleep(500);
    
    return TEST_NORMAL;
    
}

TEST_STATUS CFt3TxTest::ExcuteFt3Test()
{
    DWORD dwRst;
    HANDLE hEvent[2];
    hEvent[0] = m_pRx1Ft3->m_hRxEvent;
    hEvent[1] = m_pRx2Ft3->m_hRxEvent;

    bool bRst1 = m_pRx1Ft3->Ft3Rx();
    bool bRst2 = m_pRx2Ft3->Ft3Rx();

    //两个光串口都不能接收
    if(!bRst1 && !bRst2)
    {
        return TEST_ERROR;
    }
   
    _FT3_c * pRxFt3;

    ResetEvent(hEvent[0]);
    ResetEvent(hEvent[1]);

    dwRst = WaitForMultipleObjects(2, hEvent, FALSE,  3000);


    if(dwRst == WAIT_OBJECT_0)
    {
        pRxFt3 = m_pRx1Ft3;
    }
    else if(dwRst == WAIT_OBJECT_0 + 1)
    {
        pRxFt3 = m_pRx2Ft3;
    }
    else if(dwRst == WAIT_TIMEOUT)
    {
        m_ti.bError = FRAME_RECV_TIME_OUT;
        m_ti.uiErrorLen=swprintf(m_ti.strErr, L"发送报文失败");
        return TEST_ERROR;
    }
    else
    {
        return TEST_ERROR;
    }

    //校验接收到的报文
    if(!pRxFt3->VerifySmvFrame())
    {
        m_ti.bError = FRAME_RECV_ERROR;
        m_ti.uiErrorLen=swprintf(m_ti.strErr, STR_FRAME_RECV_ERROR);
        return TEST_ERROR;

    }
    ResetEvent(pRxFt3->m_hRxEvent);
        
    

    return TEST_NORMAL;
}

TEST_STATUS CFt3TxTest::EndFt3Test()
{
    //停止发送和接收FT3报文
    m_pFt3->Ft3StopTx();
    m_pRx1Ft3->Ft3RxStop();
    m_pRx2Ft3->Ft3RxStop();
    
    Sleep(2500);
    return TEST_NORMAL;
}

void CFt3TxTest::SetRxFt3( _FT3_c *pRx1Ft3, _FT3_c *pRx2Ft3 )
{
    m_pRx1Ft3 = pRx1Ft3;
    m_pRx2Ft3 = pRx2Ft3;
}
