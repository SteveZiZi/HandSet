#include "StdAfx.h"
#include <stdlib.h>
#include "testBase.h"
#include "SmvTest.h"
#include "UsuallyCommon.h"
#include "iniFile.h"
#include "assertMsg.h"

#define SMV_9_2_PATH     L"pcap\\sv92_test.pcap"
//#define SMV_9_2_PATH     L".\\FlashDisk2\\1.pcap"
static int m_iFlag;

CSmvTest::CSmvTest(void)
{
    
}

CSmvTest::CSmvTest( HANDLE hDevice)
{
    m_pFiber[0] = new _SMV_92_c(hDevice, DEVICE_FIBER_B);
    m_pFiber[1] = new _SMV_92_c(hDevice, DEVICE_FIBER_A);

    if (m_pFiber[0]->Init(L"DSM_FIBER_B", L"ShareFIBER_B", FIBER_MAP_FILE_LEN, FiberThreadProcRx2) == FALSE) 
    {
        RETAILMSG(ZONE_ERROR, (_T("APP: m_pFiberB.Init failed\r\n")));
    }
    if (m_pFiber[1]->Init(L"DSM_FIBER_A", L"ShareFIBER_A", FIBER_MAP_FILE_LEN, FiberThreadProcRx1) == FALSE) 
    {
        RETAILMSG(ZONE_ERROR, (_T("APP: m_pFiberA.Init failed\r\n")));
    }  
   
}

CSmvTest::~CSmvTest(void)
{
    m_pFiber[0]->StopMuDetect();
    m_pFiber[1]->StopMuDetect();

    if(m_pFiber[0] != NULL)
    {
        delete m_pFiber[0];
    }
    if(m_pFiber[1] != NULL)
    {
        delete m_pFiber[1];
    }
}

BOOL CSmvTest::Init( const WCHAR *strIniFile, int index,CTestManager *pManager )
{
    WCHAR strSection[]=L"SmvTest_\0\0";
    strSection[8]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"光以太网收发",
        m_strTestName,31,strIniFile);
    m_pManager = pManager;

    wcscpy(m_reportNape[0].strNape,L"光口1接收报文总帧数");
    wcscpy(m_reportNape[1].strNape,L"光口1接收错误报文帧数");
    wcscpy(m_reportNape[2].strNape,L"光口1出错率");
    wcscpy(m_reportNape[3].strNape,L"光口2接收报文总帧数");
    wcscpy(m_reportNape[4].strNape,L"光口2接收错误报文帧数");
    wcscpy(m_reportNape[5].strNape,L"光口2出错率");
    wcscpy(m_reportNape[6].strNape,L"持续时间"); 

    memset(m_uTotalFrame, 0, sizeof(m_uTotalFrame));
    memset(m_uErrorFrame, 0, sizeof(m_uErrorFrame));

    //获取可执行文件所在的路径
    int iDirLen =GetExeDirectory(m_strSmvFilePath,MAX_PATH*2);
    //获得配置文件所在的路径
    wcscpy(m_strSmvFilePath + iDirLen,SMV_9_2_PATH);
    m_iTimeOut = 0;
    m_iFlag = 0;
    m_tFiberParam.uInBuf[0] = 0;
    m_pFiber[0]->SetTxParam(m_tFiberParam);
    m_pFiber[1]->SetTxParam(m_tFiberParam);
    m_pFiber[0]->StartMuDetect();
    m_pFiber[1]->StartMuDetect();
    return TRUE;
}

TEST_SITUATION CSmvTest::ExcuteOneTest()
{
    UINT32 uTotalFrame, uErrorFrame;
    bool bVertifyRst = FALSE;

    //if(iTimes % 25 == 0)
    //{
    //    if(iTimes != 0)
    //    {
    //        m_pFiber[m_iFlag ^ 1]->StopMuDetect();
    //        m_pFiber[m_iFlag]->MuStop();
    //    }

    //    UINT32 uMuTxFrameGroup = 1;
    //    UINT32 uMuTxAsduNum = 1;
    //    //    m_tFiberParam.uDeviceItem = m_iFlag;

    //    //0:Smv 1:goose
    //    m_tFiberParam.uInBuf[0] = 0;

    //    m_tFiberParam.uInBuf[1] = uMuTxFrameGroup|(uMuTxAsduNum << 16);
    //    m_tFiberParam.uInBuf[2] = 800;

    //    //采样率
    //    m_tFiberParam.uInBuf[3] = 1;   
    //    m_tFiberParam.uDeviceItem = m_pFiber[m_iFlag]->m_nDeviceItem;

    //    m_pFiber[m_iFlag]->SetFilePath(m_strSmvFilePath);
    //    m_pFiber[m_iFlag]->SetTxParam(m_tFiberParam); 
    //    m_pFiber[m_iFlag]->SetFrameFlag(0);

    //    m_pFiber[m_iFlag]->MuTx();
    //    Sleep(100);
    //    m_iFlag ^= 1;

    //    m_tFiberParam.uInBuf[0] = 0;
    //    m_pFiber[m_iFlag ^ 1]->SetTxParam(m_tFiberParam);

    //    m_pFiber[m_iFlag ^ 1]->StartMuDetect();
    //}
    

    ResetEvent(m_pFiber[m_iFlag ^ 1]->m_hRxEvent);
   
    DWORD dwRet = WaitForSingleObject(m_pFiber[m_iFlag ^ 1]->m_hRxEvent, RECV_TIME_OUT);
    if(WAIT_OBJECT_0 == dwRet)
    {
        m_pFiber[m_iFlag ^ 1]->LockInter();
        bVertifyRst = m_pFiber[m_iFlag ^ 1]->VerifySmvFrame();
        m_pFiber[m_iFlag ^ 1]->GetSmvVertifyRst(uTotalFrame, uErrorFrame);
        m_uTotalFrame[m_iFlag ^ 1] += uTotalFrame;
        m_uErrorFrame[m_iFlag ^ 1] += uErrorFrame;
        ResetEvent(m_pFiber[m_iFlag ^ 1]->m_hRxEvent);
        m_pFiber[m_iFlag ^ 1]->UnLockInter();
    }
    else if(WAIT_TIMEOUT == dwRet)
    {
        if(m_iTimeOut > 5)
        {
            bVertifyRst = FALSE;
        }
        else
        {
            EndOneTest();
            m_iFlag ^= 1;
            BeginOneTest();
            m_iTimeOut++;
            return SITUATION_NORMAL;
        }
       
    }    

    m_uiTestCount++;
    

    if(bVertifyRst)
    {
        return  SITUATION_NORMAL;
    }
    else
    {
        m_uiErrCount++;
        return SITUATION_ERROR;
    }
}

const SReportNape * CSmvTest::GetReportNapes( int &count )
{
    unsigned int h,m,s;
    UINT64 t=m_uiTestTime;
    count=7;


    swprintf(m_reportNape[0].strNapeDescriptor,L"%d帧",
        m_uTotalFrame[0]);
    m_reportNape[0].bHighLight=FALSE;  

    swprintf(m_reportNape[1].strNapeDescriptor,L"%d帧",m_uErrorFrame[0]);

    swprintf(m_reportNape[3].strNapeDescriptor,L"%d帧",
        m_uTotalFrame[1]);
    m_reportNape[3].bHighLight=FALSE;

    swprintf(m_reportNape[4].strNapeDescriptor,L"%d帧",m_uErrorFrame[1]);

    
    if (m_uiTestCount > 0)
    {
        if(m_uTotalFrame[0] > 0)
        {
            swprintf(m_reportNape[2].strNapeDescriptor,L"%.2f%%",100 * (double)m_uErrorFrame[0] / m_uTotalFrame[0]); 
        }
        else
        {
            swprintf(m_reportNape[2].strNapeDescriptor,L"0.0%% ");
        }
        
        if(m_uTotalFrame[1] > 0)
        {
            swprintf(m_reportNape[5].strNapeDescriptor,L"%.2f%%",100 * (double)m_uErrorFrame[1] / m_uTotalFrame[1]);  
        }
        else
        {
            swprintf(m_reportNape[5].strNapeDescriptor,L"0.0%% ");
        }
        
    }
    else
    {
        swprintf(m_reportNape[2].strNapeDescriptor,L"0.0%% ");
        swprintf(m_reportNape[5].strNapeDescriptor,L"0.0%% ");
    }
    
    if (m_uErrorFrame[0] > 0)
    {
        m_reportNape[1].bHighLight=TRUE;
        m_reportNape[2].bHighLight=TRUE;
    }
    else
    {
        m_reportNape[1].bHighLight=FALSE;
        m_reportNape[2].bHighLight=FALSE;
    }  

    if (m_uErrorFrame[1] > 0)
    {
        m_reportNape[4].bHighLight=TRUE;
        m_reportNape[5].bHighLight=TRUE;
    }
    else
    {
        m_reportNape[4].bHighLight=FALSE;
        m_reportNape[5].bHighLight=FALSE;
    }  

    h= (unsigned int)( t/(1000*3600));
    t=t%(1000*3600);
    m=(unsigned int) (t/(1000 *60));
    t=t%(1000 *60);
    s=(unsigned int)(t/1000);

    swprintf(m_reportNape[6].strNapeDescriptor,L"%d时%d分%d秒",h,m,s);
    m_reportNape[6].bHighLight=FALSE;

    return m_reportNape;
}

TEST_SITUATION CSmvTest::BeginOneTest()
{
    memset(m_tFiberParam.uInBuf, 0, sizeof(m_tFiberParam.uInBuf));
    UINT32 uMuTxFrameGroup = 1;
    UINT32 uMuTxAsduNum = 1;
    //    m_tFiberParam.uDeviceItem = m_iFlag;

    //0:Smv 1:goose
    m_tFiberParam.uInBuf[0] = 0;

    m_tFiberParam.uInBuf[1] = uMuTxFrameGroup|(uMuTxAsduNum << 16);
    m_tFiberParam.uInBuf[2] = 800;

    //采样率
    m_tFiberParam.uInBuf[3] = 1;   
    m_tFiberParam.uDeviceItem = m_pFiber[m_iFlag]->m_nDeviceItem;

    m_tFiberParam.uInBuf[TX_TEST_STYLE_ITEM] = _MANUAL_TEST;

    m_pFiber[m_iFlag]->SetFilePath(m_strSmvFilePath);
    m_pFiber[m_iFlag]->SetTxParam(m_tFiberParam); 
    m_pFiber[m_iFlag]->SetFrameFlag(0);

    m_pFiber[m_iFlag]->MuTx();
    Sleep(100);

    m_tFiberParam.uInBuf[0] = 0;
    m_pFiber[m_iFlag ^ 1]->SetTxParam(m_tFiberParam);

    return SITUATION_NORMAL;
}

TEST_SITUATION CSmvTest::EndOneTest()
{
    m_pFiber[m_iFlag]->MuStop();
    m_iFlag ^= 1;
    return SITUATION_NORMAL;
}

