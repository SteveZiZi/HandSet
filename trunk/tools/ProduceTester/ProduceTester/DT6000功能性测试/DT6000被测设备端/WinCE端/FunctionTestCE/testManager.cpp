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
    //初始化命令索引码与实际执行测试的CTestBase对象之间的映射关系
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
    //首先获取当前可执行文件的路径
    int         i;

    i=GetExeDirectory(m_strIniFilePath,MAX_PATH);
    m_hMainWnd=hMainWnd;

    //获取配置文件的路径
    memcpy(m_strIniFilePath+i,L"config\\",7*sizeof(WCHAR));
    i+=7;
    m_strIniFilePath[i]=0;

    wcscpy(m_strIniFilePath+i,strDeviceType);
    wcscat(m_strIniFilePath+i,L".ini");

    //查看配置文件是否存在
    if (! IsFileExist(m_strIniFilePath))
    {
        return FALSE;
    }
    //将测试设备类型保存到类的实例对象中
    wcscpy(m_strDeviceType,strDeviceType);   

    //从这里开始放入与连接不相关的测试项初始化代码，
    //包括读取配置文件，并记录初始化信息等
    int         iTestNum;
    UINT16      uiCmdIndex=0;
    UINT16      iCmd;


    //加载以太网的相关信息   
    iTestNum=GetPrivateProfileInt(L"product",L"EthTestNum",0,m_strIniFilePath); 
    for (i=0;i<iTestNum;i++)
    {
        CEthernetTest *pTestObj=new CEthernetTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this))
        {
            //如果初始化成功，则将测试项添加到TestManager中
                assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }
     
    //读取磁盘测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"DiskTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CDiskTest *pTestObj=new CDiskTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }
// 
    //读取串口测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"SERIALTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CSerialPortTest *pTestObj=new CSerialPortTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }
// 
    //读取RTC测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"RTCTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CRTCTest *pTestObj=new CRTCTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //读取屏幕测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"ScreenTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CScreenTest *pTestObj=new CScreenTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //读取ISA测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"ISATestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CIsaTest *pTestObj=new CIsaTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //读取LED测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"LEDTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CLedTest *pTestObj=new CLedTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }
// 
    //读取蜂鸣器测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"BEEPTestMum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CBeepTest *pTestObj=new CBeepTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //读取GPIO测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"GPIOTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CGpioTest *pTestObj=new CGpioTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //读取SPI Flash测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"SPIFlashTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CSpiFlashTest *pTestObj=new CSpiFlashTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //读取音频测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"AudioTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CAudioTest *pTestObj=new CAudioTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中3
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }
    
    //读取CAN测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"CANTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CCanTest *pTestObj=new CCanTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中3
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }


    
    //读取AD测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"ADTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CADTest *pTestObj=new CADTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //读取ZigBee测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"ZigBeeTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CZigBeeTest *pTestObj=new CZigBeeTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }


    //读取GPRS测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"GPRSTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CGPRSTest *pTestObj=new CGPRSTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
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
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //读取RFID测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"RFIDTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CRfidTest *pTestObj=new CRfidTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //读取继电器测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"RelayTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CRelayTest *pTestObj=new CRelayTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //读取KbdIO(扩展的IO，通常可扩展键盘)测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"KbdIOTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CKbdIoTest *pTestObj=new CKbdIoTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //读取键盘测试项
    iTestNum=GetPrivateProfileInt(L"product",L"KbdTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CKbdTest *pTestObj=new CKbdTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //读取7290测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"7290TestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        C7290Test *pTestObj=new C7290Test();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //读取使用demo程序测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"UseAppTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CUseAppTest *pTestObj=new CUseAppTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    
    CFt3Test *pRxFt3[2];

    //FT3接收测试的相关信息
    iTestNum = GetPrivateProfileInt(L"product",L"UseAppFT3RxTestNum",0,m_strIniFilePath); 
    CFunctionTestCEDlg * pDlg = (CFunctionTestCEDlg *)AfxGetApp()->m_pMainWnd;
    HANDLE hDevice  = pDlg->GetDeviceHandle();
    for (i=0;i<iTestNum;i++)
    {
        CFt3Test *pTestObj=new CFt3Test(hDevice, DEVICE_FT3_RX1 + i);
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            pRxFt3[i] = pTestObj;
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //FT3发送测试的相关信息
    iTestNum = GetPrivateProfileInt(L"product",L"UseAppFT3TxTestNum",0,m_strIniFilePath); 
    for (i=0;i<iTestNum;i++)
    {
        CFt3TxTest *pTestObj=new CFt3TxTest(hDevice, DEVICE_FT3_TX + i);
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            pTestObj->SetRxFt3(pRxFt3[0]->GetRxFt3(), pRxFt3[1]->GetRxFt3());
                m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //SMV接收测试
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
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

    //读取看门狗测试的配置并初始化相关测试项
    iTestNum=GetPrivateProfileInt(L"product",L"WdtTestNum",0,m_strIniFilePath);
    for (i=0;i<iTestNum;i++)
    {
        CWdtTest *pTestObj=new CWdtTest();
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中3
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }


    //B码对时测试
    iTestNum = GetPrivateProfileInt(L"product",L"UseAppBcodeTestNum",0,m_strIniFilePath); 
    for (i=0;i<iTestNum;i++)
    {
        CBcodeTest *pTestObj = new CBcodeTest(pSmvTest->GetSmvFiber());
        if (pTestObj->Init(m_strIniFilePath,i,&iCmd,this) )
        {
            //如果初始化成功，则将测试项添加到TestManager中
            assertMsg(m_pTestObj[iCmd]==NULL,L"命令码出现冲突");
            m_pTestObj[iCmd]=pTestObj;
        }
        else
        {
            delete pTestObj;
        }
    }

   

    
    //从配置文件中读取测试项配置并初始化测试项

    return TRUE;
}


BOOL CTestManager::GetDeviceInfo(DEVICE_INFO *pInfo)
{
    assertMsg(m_pConnect!=NULL,TEXT("CTestManager这个对象还没有设置一个可用的连接对象"));
    assertMsg(pInfo!=NULL,TEXT("无效的指针"));

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
                MessageBox(NULL,L"不存在的测试项",NULL,MB_OK|MB_ICONERROR);
            }
       
            
            break;
        }
    }    
        
    return nRet;
}

BOOL CTestManager::OnConnect(CConnect *pConnect)
{
    assertMsg(pConnect,L"CConnect对象不能为空!");
    assertMsg(m_pConnect==NULL,L"CTestManager对象的conncect对象必须先被分离"); 

    m_pConnect=pConnect;  
    return TRUE;
}

int CTestManager::TestConnect()
{
    assertMsg(m_pConnect,L"调用TestConnect接口应该在OnConnect调用成功以后");
    return m_pConnect->TestConnect();
}
