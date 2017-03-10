#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "iniFile.h"
#include "assertMsg.h"
#include "BcodeTest.h"
#define SMV_9_2_PATH     L".\\pcap\\sv92_test.pcap"

CBcodeTest::CBcodeTest(void)
{
}

CBcodeTest::CBcodeTest( _SMV_92_c * pFiber )
{
    m_pFiber = pFiber;
}

CBcodeTest::~CBcodeTest(void)
{
}

BOOL CBcodeTest::Init( const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager/*=NULL*/ )
{
    WCHAR strSection[]=L"UseAppBcodeTest_\0\0";
    strSection[16]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"Bcode",
        m_strTestName,31,strIniFile);

    m_uiCmd=index+CMD_TEST_USE_B_CODE;
    *uipCmdIndex = m_uiCmd;
    memset(m_tFiberParam.uInBuf, 0, sizeof(m_tFiberParam.uInBuf));

    UINT32 uMuTxFrameGroup = 1;
    UINT32 uMuTxAsduNum = 1;

    m_tFiberParam.uDeviceItem = 0;
    //0:Smv 1:goose
    m_tFiberParam.uInBuf[0] = 0;
    m_tFiberParam.uInBuf[1] = uMuTxFrameGroup|(uMuTxAsduNum << 16);
    m_tFiberParam.uInBuf[2] = 800; 
    //采样率
    m_tFiberParam.uInBuf[3] = 1;   
    m_tFiberParam.uDeviceItem = m_pFiber->m_nDeviceItem;
    
    //设置UTC时间
    m_pFiber->SetUtcTime();

    /*m_pFiber->SetCurSyncMode(_IRIG_B_P);

    Sleep(2000);
    m_pFiber->VertifyBcodeTime();*/

    return TRUE;
}

TEST_STATUS CBcodeTest::ProcessExcuteCmd( UINT16 param,UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect )
{
    m_ti.bError=0;
    m_ti.uiErrorLen=0;
    int nRet;

    switch(param)
    {
    case FRAME_TEST_CMD_INI:
        nRet = BeginSmvTest();
        break;
    case FRAME_TEST_CMD_EXT:
        nRet = ExcuteSmvTest();
        break;
    case FRAME_TEST_CMD_END:
        nRet = EndSmvTest();
        break;
    }


    TEST_INFO_TRANSFER tit;
    TestInfoLocalToTransfer(&tit,&m_ti);

    nRet =pConnect->SendAPacket(m_uiCmd,param,sizeof(tit),(BYTE *)&tit);

    return nRet >0?TEST_NORMAL:TEST_CONNECTDOWN;
}

TEST_STATUS CBcodeTest::BeginSmvTest()
{
    m_pFiber->SetFilePath(SMV_9_2_PATH);
    m_pFiber->SetTxParam(m_tFiberParam);

    //设置当前的Sync模式为B码
    m_pFiber->SetCurSyncMode(_IRIG_B_P);

    m_pFiber->SetFrameFlag(1);
    m_tFiberParam.uInBuf[0] = 0;
    m_pFiber->SetTxParam(m_tFiberParam);
   
    if( m_pFiber->MuTx())
    {
        return TEST_NORMAL;
    }
    else
    {
        return TEST_ERROR;
        RETAILMSG(1, (_T("初始化SMV发送失败")));
    }
}

TEST_STATUS CBcodeTest::ExcuteSmvTest()
{
    
    m_tFiberParam.uInBuf[0] = 1;
    m_pFiber->SetTxParam(m_tFiberParam);
    m_pFiber->StartMuDetect();

    ResetEvent(m_pFiber->m_hRxEvent);
    DWORD dwRet = WaitForSingleObject(m_pFiber->m_hRxEvent, RECV_TIME_OUT + 2000);
    if(WAIT_OBJECT_0 == dwRet)
    {
        //校验B码对时误差和校验GOOSE报文
        if(!m_pFiber->VertifyBcodeTime()  || !m_pFiber->VerifyGooseFrame())
        {
            m_ti.bError = FRAME_RECV_ERROR;
            m_ti.uiErrorLen=swprintf(m_ti.strErr, STR_FRAME_RECV_ERROR);
            ResetEvent(m_pFiber->m_hRxEvent);
            return TEST_ERROR;
        }
        
    }
    else if(WAIT_TIMEOUT == dwRet)
    {
        m_ti.bError = FRAME_RECV_TIME_OUT;
        m_ti.uiErrorLen=swprintf(m_ti.strErr, STR_FRAME_RECV_TIME_OUT);
        return TEST_ERROR;
    }

    return TEST_NORMAL;
}

TEST_STATUS CBcodeTest::EndSmvTest()
{
    //停止发送报文
    m_tFiberParam.uInBuf[0] = 0;
    m_pFiber->SetTxParam(m_tFiberParam);
    m_pFiber->MuStop();

    //停止探测报文
    m_tFiberParam.uInBuf[0] = 1;
    m_pFiber->SetTxParam(m_tFiberParam);
    m_pFiber->StopMuDetect();

    m_pFiber->SetCurSyncMode(_NO_SYNC);
    return TEST_NORMAL;
}
