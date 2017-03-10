#include "stdafx.h"
#include "TSocket.h"
#include "assertMsg.h"
#include "connect.h"
#include "testCommon.h"
#include "testBase.h"
#include "DiskTest.h"
#include "rtcTest.h"
#include "gpioTest.h"
#include "UsuallyCommon.h"
#include "testManager.h"
#include "iniFile.h"
#include "ledTest.h"
#include "SerialPortTest.h"
#include "ScreenTest.h"
#include "ethernetTest.h"
#include "BeepTest.h"
#include "IsaTest.h"
#include "AudioTest.h"
#include "wdtTest.h"
#include "canTest.h"
#include "gprsTest.h"
#include "ADTest.h"
#include "KbdIoTest.h"
#include "KbdTest.h"
#include "7290Test.h"
#include "rfidTest.h"
#include "UseAppTest.h"
#include "ZigBeeTest.h"
#include "gpsTest.h"
#include "RelayTest.h"
#include "spiFlashTest.h"
#include "Ft3Test.h"
#include "SmvTest.h"
#include "BcodeTest.h"
#include "FunctionTestCEDlg.h"
#include "Ft3TxTest.h"
#include ".\app_src\gpmcfg.h"
CTestManager::CTestManager():
m_pConnect(NULL),
m_hMainWnd(NULL)
{
    //��ʼ��������������ʵ��ִ�в��Ե�CTestBase����֮���ӳ���ϵ
    for(int i=0;i<MAX_TEST_OBJ;i++)
    {
        m_pTestObj[i]=NULL;
    }


}

CTestManager::~CTestManager()
{
    for(int i=0;i<MAX_TEST_OBJ;i++)
    {
        if (m_pTestObj[i]!=NULL)
        {
            delete m_pTestObj[i];
            m_pTestObj[i]=NULL;
        }
    }
}

BOOL CTestManager::OnConnectDown()
{
    for(int i=0;i<MAX_TEST_OBJ;i++)
    {
        if (m_pTestObj[i]!=NULL)
        {
            m_pTestObj[i]->OnConnectDown();            
        }
    }
    return TRUE;
}

BOOL CTestManager::Init(const WCHAR *strDeviceType,HWND hMainWnd)
{
    //���Ȼ�ȡ��ǰ��ִ���ļ���·��
    int         i;

    i=GetExeDirectory(m_strIniFilePath,MAX_PATH);
    m_hMainWnd=hMainWnd;

    //��ȡ�����ļ���·��
    memcpy(m_strIniFilePath+i,L"config\\",7*sizeof(WCHAR));
    i+=7;
    m_strIniFilePath[i]=0;

    wcscpy(m_strIniFilePath+i,strDeviceType);
    wcscat(m_strIniFilePath+i,L".ini");

    //�鿴�����ļ��Ƿ����
    if (! IsFileExist(m_strIniFilePath))
    {
        return FALSE;
    }
    //�������豸���ͱ��浽���ʵ��������
    wcscpy(m_strDeviceType,strDeviceType);   

    //�����￪ʼ���������Ӳ���صĲ������ʼ�����룬
    //������ȡ�����ļ�������¼��ʼ����Ϣ��
    int         iTestNum;
    UINT16      uiCmdIndex=0;
    UINT16      iCmd;


    //������̫���������Ϣ   
    iTestNum=GetPrivateProfileInt(L"product",L"EthTestNum",0,m_strIniFilePath); 
    for (i=0;i<iTestNum;i++)
    {
        CEthernetTest *pTestObj=new CEthernetTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this))
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
                assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }
     
    //��ȡ���̲��Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"DiskTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CDiskTest *pTestObj=new CDiskTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }
// 
    //��ȡ���ڲ��Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"SERIALTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CSerialPortTest *pTestObj=new CSerialPortTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }
// 
    //��ȡRTC���Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"RTCTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CRTCTest *pTestObj=new CRTCTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //��ȡ��Ļ���Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"ScreenTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CScreenTest *pTestObj=new CScreenTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //��ȡISA���Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"ISATestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CIsaTest *pTestObj=new CIsaTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //��ȡLED���Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"LEDTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CLedTest *pTestObj=new CLedTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }
// 
    //��ȡ���������Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"BEEPTestMum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CBeepTest *pTestObj=new CBeepTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //��ȡGPIO���Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"GPIOTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CGpioTest *pTestObj=new CGpioTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //��ȡSPI Flash���Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"SPIFlashTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CSpiFlashTest *pTestObj=new CSpiFlashTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //��ȡ��Ƶ���Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"AudioTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CAudioTest *pTestObj=new CAudioTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��3
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }
    
    //��ȡCAN���Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"CANTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CCanTest *pTestObj=new CCanTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��3
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }


    
    //��ȡAD���Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"ADTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CADTest *pTestObj=new CADTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //��ȡZigBee���Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"ZigBeeTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CZigBeeTest *pTestObj=new CZigBeeTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }


    //��ȡGPRS���Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"GPRSTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CGPRSTest *pTestObj=new CGPRSTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    iTestNum=GetPrivateProfileInt(L"product",L"GPSTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CGpsTest *pTestObj=new CGpsTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //��ȡRFID���Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"RFIDTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CRfidTest *pTestObj=new CRfidTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //��ȡ�̵������Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"RelayTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CRelayTest *pTestObj=new CRelayTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //��ȡKbdIO(��չ��IO��ͨ������չ����)���Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"KbdIOTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CKbdIoTest *pTestObj=new CKbdIoTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //��ȡ���̲�����
    iTestNum=GetPrivateProfileInt(L"product",L"KbdTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CKbdTest *pTestObj=new CKbdTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //��ȡ7290���Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"7290TestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        C7290Test *pTestObj=new C7290Test();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //��ȡʹ��demo������Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"UseAppTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CUseAppTest *pTestObj=new CUseAppTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    
    CFt3Test *pRxFt3[2];

    //FT3���ղ��Ե������Ϣ
    iTestNum = GetPrivateProfileInt(L"product",L"UseAppFT3RxTestNum",0,m_strIniFilePath); 
    CFunctionTestCEDlg * pDlg = (CFunctionTestCEDlg *)AfxGetApp()->m_pMainWnd;
    HANDLE hDevice  = pDlg->GetDeviceHandle();
    for (i=0;i<iTestNum;i++)
    {
        CFt3Test *pTestObj=new CFt3Test(hDevice, DEVICE_FT3_RX1 + i);
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            pRxFt3[i] = pTestObj;
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //FT3���Ͳ��Ե������Ϣ
    iTestNum = GetPrivateProfileInt(L"product",L"UseAppFT3TxTestNum",0,m_strIniFilePath); 
    for (i=0;i<iTestNum;i++)
    {
        CFt3TxTest *pTestObj=new CFt3TxTest(hDevice, DEVICE_FT3_TX + i);
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            pTestObj->SetRxFt3(pRxFt3[0]->GetRxFt3(), pRxFt3[1]->GetRxFt3());
                m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //SMV���ղ���
    CSmvTest * pSmvTest;
    iTestNum = GetPrivateProfileInt(L"product",L"UseAppSmvTestNum",0,m_strIniFilePath); 
    for (i=0;i<iTestNum;i++)
    {
        CSmvTest *pTestObj=new CSmvTest(hDevice, DEVICE_FIBER_A + i);
       
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        { 
            if(0 == i)
            {
                pSmvTest = pTestObj;
            }
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //��ȡ���Ź����Ե����ò���ʼ����ز�����
    iTestNum=GetPrivateProfileInt(L"product",L"WdtTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CWdtTest *pTestObj=new CWdtTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��3
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }


    //B���ʱ����
    iTestNum = GetPrivateProfileInt(L"product",L"UseAppBcodeTestNum",0,m_strIniFilePath); 
    for (i=0;i<iTestNum;i++)
    {
        CBcodeTest *pTestObj = new CBcodeTest(pSmvTest->GetSmvFiber());
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            assertMsg(m_pTestObj[iCmd]==NULL,L"��������ֳ�ͻ");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

   

    
    //�������ļ��ж�ȡ���������ò���ʼ��������

    return TRUE;
}


BOOL CTestManager::GetDeviceInfo(DEVICE_INFO *pInfo)
{
    assertMsg(m_pConnect!=NULL,TEXT("CTestManager�������û������һ�����õ����Ӷ���"));
    assertMsg(pInfo!=NULL,TEXT("��Ч��ָ��"));

    return TRUE;
}


int CTestManager::ProcessOnePacket()
{
    PACKET          pa;   
    int             nRet;

    CFunctionTestCEDlg *pDlg = (CFunctionTestCEDlg *)AfxGetApp()->GetMainWnd();
    nRet=m_pConnect->RecvAPacket(&pa,5000);

    if (nRet>0)
    {
        UINT16 uiCmd=pa.uiCmd;
        switch(uiCmd)
        {
        case  CMD_DEVICE_INFO:     

            DEVICE_INFO_TRANSFER dit;
            DEVICE_INFO     di;
            
            wcsncpy(di.strDeviceID,m_pConnect->GetLocalID(),31);
            wcsncpy(di.strDeviceType,m_strDeviceType,31);
            wcsncpy(di.strDeviceVer, pDlg->m_strDeviceVer, 31);
            DeviceInfoLocalToTransfer(&dit,&di);
            nRet =m_pConnect->SendAPacket(CMD_DEVICE_INFO,0,sizeof(dit),(BYTE *)&dit);                
            break;
        default:
            if (m_pTestObj[uiCmd]!=NULL)
            {
                int tmpRet;               
                tmpRet=m_pTestObj[uiCmd]->ProcessExcuteCmd(pa.param,
                    pa.uiDatalen,pa.pData,m_pConnect);
                if (tmpRet ==TEST_CONNECTDOWN)
                {
                    m_pTestObj[uiCmd]->OnConnectDown();
                    nRet=-1;
                }
            }
            else
            {
                CString strTest;
                strTest.Format(L"uicmd: %d address: %d", uiCmd, &m_pTestObj[uiCmd]);
                AfxMessageBox(strTest);
                MessageBox(NULL,L"�����ڵĲ�����",NULL,MB_OK|MB_ICONERROR);
            }
       
            
            break;
        }
    }    
        
    return nRet;
}

BOOL CTestManager::OnConnect(CConnect *pConnect)
{
    assertMsg(pConnect,L"CConnect������Ϊ��!");
    assertMsg(m_pConnect==NULL,L"CTestManager�����conncect��������ȱ�����"); 

    m_pConnect=pConnect;  
    return TRUE;
}

int CTestManager::TestConnect()
{
    assertMsg(m_pConnect,L"����TestConnect�ӿ�Ӧ����OnConnect���óɹ��Ժ�");
    return m_pConnect->TestConnect();
}
