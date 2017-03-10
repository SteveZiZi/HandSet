#include "TSocket.h"
#include <string>
#include "testManager.h"
#include "assertMsg.h"
#include "UsuallyCommon.h"
#include "DiskTest.h"
#include "WriteLog.h"
#include "GpioTest.h"
#include "UsbDeviceTest.h"
#include "rtcTest.h"
#include "ScreenTest.h"
#include "ethernetTest.h"
#include "SerialPortTest.h"
#include "BeepTest.h"
#include "LedTest.h"
#include "AudioTest.h"
#include "IsaTest.h"
#include "wdtTest.h"
#include "gprsTest.h"
#include "AdTest.h"
#include "KbdIoTest.h"
#include "canTest.h"
#include "7290Test.h"
#include "UseAppTest.h"
#include "KbdTest.h"
#include "rfidTest.h"
#include "ZigBeeTest.h"
#include "GpsTest.h"
#include "RelayTest.h"
#include "spiFlashTest.h"
#include "Ft3Test.h"
#include "SmvTest.h"
#include "BcodeTest.h"
#include "Ft3TxTest.h"
using namespace std;

CTestManager::CTestManager():
m_iTestObjNum(0),
m_pConnect(NULL),
m_hMainWnd(NULL)
{
    memset(m_pTestObj,0,sizeof(m_pTestObj) );    
}

CTestManager::~CTestManager()
{
    //ɾ�����в��������
    for (int i=0;i<m_iTestObjNum;i++ )
    {
        if (m_pTestObj[i])
        {
            delete m_pTestObj[i];
            m_pTestObj[i]=NULL;
        }
    }
    m_iTestObjNum=0;

}

BOOL CTestManager::Init(CConnect *pConnect,HWND hMainWnd)
{
    //��ȡ������Ϣ
    if (GetDeviceInfo(pConnect,&m_devInfo)<=0 )
    {
        showTestStatus(L"��ȡ�豸��Ϣʧ��");
        return FALSE;
    }

    //���������ļ�·��
    WCHAR       strTmp[MAX_PATH+10];
    GetExeDirectory(strTmp,MAX_PATH);
    wstring     strIniFilePath=strTmp;
    strIniFilePath+=L"config\\";
    strIniFilePath+=m_devInfo.strDeviceType;
    strIniFilePath+=L".ini";

    //�ж��豸�����ļ��Ƿ����
    m_hMainWnd=hMainWnd;
    if(!IsFileExist(strIniFilePath.c_str()) )
    {
        wstring str=L"�豸����Ϊ";
        str+=m_devInfo.strDeviceType;
        str+=L"�������ļ�������";
        showTestStatus(str.c_str());
        return FALSE;
    }

    //���������ļ����ظ�������
    m_iTestObjNum=0;

    int     iTestObjNum,t;

    //������̫�����Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"EthTestNum",0,strIniFilePath.c_str());
    //��ʼ����Ҫ�Ĵ��̲���
    t=0;  
    while(t<iTestObjNum)
    {
        CEthernetTest *pTest=new CEthernetTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //���ش��ڲ��Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"SERIALTestNum",0,strIniFilePath.c_str());
    t=0;  
    while(t<iTestObjNum)
    {
        CSerialPortTest *pTest=new CSerialPortTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }



    //���ش��̲��Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"DiskTestNum",0,strIniFilePath.c_str());
    //��ʼ����Ҫ�Ĵ��̲���
    t=0;  
    while(t<iTestObjNum)
    {
        CDiskTest *pTest=new CDiskTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //����CAN���Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"CANTestNum",0,strIniFilePath.c_str());
    t=0;  
    while(t<iTestObjNum)
    {
        CCANTest *pTest=new CCANTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }    

    //����usb device���Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"ActiveSyncTestNum",0,strIniFilePath.c_str());
    t=0;  
    while(t<iTestObjNum)
    {
        CActiveSyncTest *pTest=new CActiveSyncTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //����ISA���Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"ISATestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CIsaTest *pTest=new CIsaTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }



    //����RTC���Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"RTCTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CRtcTest *pTest=new CRtcTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //���ط��������Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"BEEPTestMum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CBeepTest *pTest=new CBeepTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }
    
    //������Ļ���Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"ScreenTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CSreenTest *pTest=new CSreenTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //����GPIO���Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"GPIOTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CGpioTest *pTest=new CGpioTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //������չIO ���Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"KbdIOTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CKbdIoTest *pTest=new CKbdIoTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //���ؼ��̲��Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"KbdTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CKbdTest *pTest=new CKbdTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }
    
    //����LED���Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"LEDTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CLedTest *pTest=new CLedTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //������Ƶ���Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"AudioTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CAudioTest *pTest=new CAudioTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //����SPIFlash���Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"SPIFlashTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CSpiFlash *pTest=new CSpiFlash();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //����AD���Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"ADTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CADTest *pTest=new CADTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //����GPRS���Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"GPRSTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CGPRSTest *pTest=new CGPRSTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //����GPS���Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"GPSTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CGpsTest *pTest=new CGpsTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //���ؼ̵������Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"RelayTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CRelayTest *pTest=new CRelayTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    iTestObjNum=GetPrivateProfileInt(L"product",L"RfidTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CRfidTest *pTest=new CRfidTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    iTestObjNum=GetPrivateProfileInt(L"product",L"ZigBeeTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CZigBeeTest *pTest=new CZigBeeTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //��ȡ7290���Ե����ò���ʼ����ز�����
    iTestObjNum=GetPrivateProfileInt(L"product",L"7290TestNum",
        0,strIniFilePath.c_str());
    t=0;
    while(t<iTestObjNum)
    {
        C7290Test *pTestObj=new C7290Test();
        if (pTestObj->Init(strIniFilePath.c_str(),t,this) )
        {
            //�����ʼ���ɹ����򽫲�������ӵ�TestManager��
            AddTest(pTestObj);
        }
        else
        {
            delete pTestObj;
        }
        t++;
    }


    //��ȡFT3���ղ��Ե����ò���ʼ����ز�����
    iTestObjNum=GetPrivateProfileInt(L"product",L"UseAppFT3RxTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CFt3Test *pTest=new CFt3Test();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }
    
    //��ȡFT3���Ͳ��Ե����ò���ʼ����ز�����
    iTestObjNum=GetPrivateProfileInt(L"product",L"UseAppFt3TxTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CFt3TxTest *pTest=new CFt3TxTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //��ȡSMV�շ����Ե����ò���ʼ����ز�����
    iTestObjNum=GetPrivateProfileInt(L"product",L"UseAppSmvTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CSmvTest *pTest=new CSmvTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //��ȡB���ʱ���Ե����ò���ʼ����ز�����
    iTestObjNum=GetPrivateProfileInt(L"product",L"UseAppBcodeTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CBcodeTest *pTest=new CBcodeTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }

    //���ؿ��Ź����Ե������Ϣ
    iTestObjNum=GetPrivateProfileInt(L"product",L"WdtTestNum",0,strIniFilePath.c_str()); 
    t=0;  
    while(t<iTestObjNum)
    {
        CWdtTest *pTest=new CWdtTest();
        if (pTest->Init(strIniFilePath.c_str(),t,this) )
        {
            AddTest(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        t++;
    }


    m_pConnect=pConnect;
    m_hMainWnd=hMainWnd;
    return TRUE;
}

int CTestManager::GetDeviceInfo(CConnect *pConnect,DEVICE_INFO *pDevInfo)
{
    assertMsg(pConnect!=NULL,L"pConnect����Ϊ��");
    assertMsg(pDevInfo!=NULL,L"pDevInfo����Ϊ��");

    int nRet;
    PACKET pa;
    DEVICE_INFO_TRANSFER    dit;


    nRet=pConnect->SendAPacket(CMD_DEVICE_INFO,0,NULL,0);
    if (nRet >0)
    {
        nRet =pConnect->RecvAPacket(&pa,300000);
        if (nRet >0)
        {
            memcpy(&dit,pa.pData,sizeof(dit));
            DeviceInfoTransferToLocal(pDevInfo,&dit);
        }
    }
    return nRet;
}

void CTestManager::showTestStatus(LPCWSTR strTestStatus)
{
    if (m_hMainWnd !=NULL)
    {
        SendMessage(m_hMainWnd,UM_SHOW_STATUS,0,(LPARAM)strTestStatus );
    }
}

BOOL CTestManager::AddTest(CTestBase *pTest)
{
    if (m_iTestObjNum>=MAX_TEST_OBJ )
    {
        //�������Ѿ�����
        return FALSE;
    }
    m_pTestObj[m_iTestObjNum++]=pTest;
    return TRUE;
}

CConnect *CTestManager::DettachConnect()
{
    CConnect *p=m_pConnect;
    m_pConnect=NULL;

    return p;
}

CConnect    *CTestManager::ReplaceConnect(CConnect *pConnect)
{
    CConnect *p=m_pConnect;
    m_pConnect=pConnect;
    return p;
}

BOOL CTestManager::WriteLog(CTestBase *pTest,const WCHAR *strLog)
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    if (pTest ==NULL)
    {
        ::WriteLog(L"(%d-%d-%d %d:%d:%d) %s\r\n",st.wYear,st.wMonth,st.wDay,
            st.wHour,st.wMinute,st.wSecond,strLog);
    }
    else
    {
        ::WriteLog(L"(%d-%d-%d %d:%d:%d)[%s] %s\r\n",st.wYear,st.wMonth,st.wDay,
            st.wHour,st.wMinute,st.wSecond,pTest->GetTestObjName(),strLog);
    }

    return TRUE;
}

