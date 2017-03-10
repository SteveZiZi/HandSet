#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "iniFile.h"
#include "assertMsg.h"
#include "SmvTest.h"

#define SMV_9_2_PATH     L".\\pcap\\sv92_test.pcap"
#define GOOSE_PATH     L".\\pcap\\goose.pcap"

CSmvTest::CSmvTest(void)
{
}

CSmvTest::CSmvTest( HANDLE hDevice, UINT8 uDeviceItem )
{
    if(uDeviceItem == DEVICE_FIBER_A || DEVICE_FIBER_B == uDeviceItem)
    {
        uDeviceItem ^= 1;
    }
    m_pFiber = new _SMV_92_c(hDevice, uDeviceItem);

    //�����豸�ų�ʼ������
    if(DEVICE_FIBER_B == uDeviceItem)
    {
        
        if (m_pFiber->Init(L"DSM_FIBER_B", L"ShareFIBER_B", FIBER_MAP_FILE_LEN, FiberThreadProcRx2) == FALSE) {
            RETAILMSG(ZONE_ERROR, (_T("APP: m_pFiberB.Init failed\r\n")));
        }
    }
    else if(DEVICE_FIBER_A == uDeviceItem)
    {
        if (m_pFiber->Init(L"DSM_FIBER_A", L"ShareFIBER_A", FIBER_MAP_FILE_LEN, FiberThreadProcRx1) == FALSE) {
            RETAILMSG(ZONE_ERROR, (_T("APP: m_pFiberA.Init failed\r\n")));
        }
    }
    else if(DEVICE_FIBER_C == uDeviceItem)
    {
        if (m_pFiber->Init(L"DSM_FIBER_C", L"ShareFIBER_C", FIBER_MAP_FILE_LEN, FiberThreadProcRx3) == FALSE) {
            RETAILMSG(ZONE_ERROR, (_T("APP: m_pFiberC.Init failed\r\n")));
        }
    }

}

CSmvTest::~CSmvTest(void)
{
    if(m_pFiber != NULL)
    {
        delete m_pFiber;
    }
}

BOOL CSmvTest::Init( const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager/*=NULL*/ )
{
    WCHAR strSection[]=L"UseAppSmvTest_\0\0";
    strSection[14]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"Smv",
        m_strTestName,31,strIniFile);

    m_uiCmd=index+CMD_TEST_USE_MU1;
    *uipCmdIndex = m_uiCmd;

    memset(m_tFiberParam.uInBuf, 0, sizeof(m_tFiberParam.uInBuf));

    UINT32 uMuTxFrameGroup = 1;
    UINT32 uMuTxAsduNum = 1;

    m_tFiberParam.uDeviceItem = 0;
    //0:Smv 1:goose
    m_tFiberParam.uInBuf[0] = 0;

    m_tFiberParam.uInBuf[1] = uMuTxFrameGroup|(uMuTxAsduNum << 16);
    m_tFiberParam.uInBuf[2] = 800; 

    //������
    m_tFiberParam.uInBuf[3] = 1;    
    m_tFiberParam.uDeviceItem = m_pFiber->m_nDeviceItem;

    //���÷���SMV���ļ�·��
    m_pFiber->SetFilePath(SMV_9_2_PATH);
    m_pFiber->SetTxParam(m_tFiberParam);

    
//    m_pFiber->StartMuDetect();
    return TRUE;
}

TEST_STATUS CSmvTest::ProcessExcuteCmd( UINT16 param,UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect )
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

TEST_STATUS CSmvTest::BeginSmvTest()
{
    
    m_tFiberParam.uInBuf[0] = 0;
    m_pFiber->SetFrameFlag(0);

    m_pFiber->SetTxParam(m_tFiberParam);

    //��ʼ̽��SMV����
    if(m_pFiber->StartMuDetect())
    {
        return TEST_NORMAL;
    }
    else
    {
        return TEST_ERROR;
        RETAILMSG(1, (_T("��ʼ��SMV����ʧ��")));
    }
}

TEST_STATUS CSmvTest::ExcuteSmvTest()
{
    int nVertifyTimes = 0;
    ResetEvent(m_pFiber->m_hRxEvent);
    DWORD dwRet = WaitForSingleObject(m_pFiber->m_hRxEvent, 3000);
    if(WAIT_OBJECT_0 == dwRet)
    {
        //У����յ���SMV����
        if(!m_pFiber->VerifySmvFrame())
        {
            m_ti.bError = FRAME_RECV_ERROR;
            m_ti.uiErrorLen=swprintf(m_ti.strErr, STR_FRAME_RECV_ERROR);
            return TEST_ERROR;
        }
        ResetEvent(m_pFiber->m_hRxEvent);         
            
    }
    else if(WAIT_TIMEOUT == dwRet)
    {
        m_ti.bError = FRAME_RECV_TIME_OUT;
        m_ti.uiErrorLen=swprintf(m_ti.strErr, L"���ձ���ʧ��");
        return TEST_ERROR;
            
    }    
        
    //ֹͣ̽��SMV����
    m_pFiber->StopMuDetect();

    m_tFiberParam.uInBuf[0] = 0;
    m_pFiber->SetTxParam(m_tFiberParam);
    //����SMV����
    m_pFiber->MuTx();
    
    Sleep(500);

    ResetEvent(m_pFiber->m_hRxEvent);

    m_tFiberParam.uInBuf[0] = 1;
    m_pFiber->SetTxParam(m_tFiberParam);

    //̽��GOOSE����
    m_pFiber->StartMuDetect();

    
    dwRet = WaitForSingleObject(m_pFiber->m_hRxEvent, 1500);
    if(WAIT_OBJECT_0 == dwRet)
    {
        //У����յ���GOOSE����
        if(!m_pFiber->VerifyGooseFrame())
        {
            m_ti.bError = FRAME_RECV_ERROR;
            m_ti.uiErrorLen=swprintf(m_ti.strErr, STR_FRAME_RECV_ERROR);
            return TEST_ERROR;
        }
        ResetEvent(m_pFiber->m_hRxEvent);
    }
    else if(WAIT_TIMEOUT == dwRet)
    {
        m_ti.bError = FRAME_RECV_TIME_OUT;
        m_ti.uiErrorLen=swprintf(m_ti.strErr, L"���ͱ���ʧ��");
        return TEST_ERROR;
    }
        
    

    return TEST_NORMAL;
}

TEST_STATUS CSmvTest::EndSmvTest()
{
    //ֹͣ����SMV����
    m_tFiberParam.uInBuf[0] = 0;
    m_pFiber->SetTxParam(m_tFiberParam);
    m_pFiber->MuStop();

    //ֹͣ̽��GOOSE����
    m_tFiberParam.uInBuf[0] = 1;
    m_pFiber->SetTxParam(m_tFiberParam);
    m_pFiber->StopMuDetect();

    Sleep(2000);

    return TEST_NORMAL;
}

_SMV_92_c * CSmvTest::GetSmvFiber()
{
    return m_pFiber;
}
