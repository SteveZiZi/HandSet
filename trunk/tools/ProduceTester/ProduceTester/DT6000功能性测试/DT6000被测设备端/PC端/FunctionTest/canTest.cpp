#include <windows.h>
#include <atlstr.h>
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "CANLib.h"
#include "canTest.h"
#include "assertMsg.h"

CCANTest::CCANTest()
{

}

CCANTest::~CCANTest()
{

}

BOOL CCANTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"CANTest_\0\0";
    strSection[8]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"CAN 1",
        m_strTestName,31,strIniFile);

    m_dwDeviceType=GetPrivateProfileInt(strSection,L"DeviceType",5,
        strIniFile);
    m_dwDeviceIndex=GetPrivateProfileInt(strSection,L"DeviceIndex",0,
        strIniFile);
    m_dwCanId=GetPrivateProfileInt(strSection,L"CanId",0,
        strIniFile);
    m_dwExternFlag=GetPrivateProfileInt(strSection,L"ExternFlag",0,
        strIniFile);
    m_dwRemoteFlag=GetPrivateProfileInt(strSection,L"RemoteFlag",0,
        strIniFile);

    m_pManager=pManager;
    m_uiCmd=index+CMD_TEST_CAN1;
    return TRUE;
}

TEST_STATUS CCANTest::OnBeginTest(CConnect *pConnect)
{
    if(!VCI_OpenDevice(m_dwDeviceType,m_dwDeviceIndex,0))
    {       
        m_pManager->WriteLog(this,L"打开设备失败,本测试终止");
        m_pManager->MessageBox(L"打开CAN失败，请检查USBCAN及其驱动是否已安装",
            L"CAN测试",MB_OK|MB_ICONERROR );
        return TEST_ERROR;
    }

    VCI_INIT_CONFIG vic={0};
    vic.AccCode = 0x00000000;
    vic.AccMask = 0xffffffff;
    vic.Timing0 = 0x00;
    vic.Timing1 = 0x14;

    if(!VCI_InitCAN(m_dwDeviceType,  m_dwDeviceIndex, 
        m_dwCanId,&vic) )
    {       
        m_pManager->WriteLog(this, L"初始化CAN失败,本测试终止");
        m_pManager->MessageBox(L"初始化CAN失败,本测试终止",
            L"CAN测试",MB_OK|MB_ICONERROR );
        goto FAILED;        
        return TEST_ERROR;
    }

    if(!VCI_StartCAN(m_dwDeviceType,
        m_dwDeviceIndex, m_dwCanId))
    {
        m_pManager->WriteLog(this, L"启动CAN失败,本测试终止");
        m_pManager->MessageBox(L"启动CAN失败,本测试终止",
            L"CAN测试",MB_OK|MB_ICONERROR );        
        goto FAILED;
        return TEST_ERROR;
    }

    //初始化CAN发送数据结构
    m_vco.RemoteFlag = (BYTE)m_dwRemoteFlag;
    m_vco.ExternFlag = (BYTE)m_dwExternFlag;
    m_vco.SendType = 0;
    m_vco.ID = 0;
    m_vco.DataLen = 0;

    //首先发送和接收一帧数据以确认CAN导线是否正确连接

    if (!VCI_Transmit(m_dwDeviceType,m_dwDeviceIndex,m_dwCanId,&m_vco,1)  ) 
    {
        m_pManager->WriteLog(this, L"初始数据发送失败, 本测试终止");
        m_pManager->MessageBox(L"初始数据发送失败，请检查CAN导线连接是否正确",
            L"CAN测试",MB_OK|MB_ICONERROR );  
        goto FAILED;
    }

    int  iTryRecvTimes=0;
    BOOL bRecvSucess=FALSE;
    
    VCI_CAN_OBJ     fi[2];
    VCI_ERR_INFO    ei;
    while (iTryRecvTimes <10)
    {
        if (VCI_Receive(m_dwDeviceType,m_dwDeviceIndex,m_dwCanId, fi,1, 500)>0  )
        {
            bRecvSucess=TRUE;
            break;
        }
        VCI_ReadErrInfo(m_dwDeviceType,m_dwDeviceIndex,m_dwCanId, &ei);
        iTryRecvTimes++;
    }

    //如果没有接收的数据，则显示错误信息
    if (!bRecvSucess)
    {
        m_pManager->WriteLog(this, L"初始数据接收失败, 本测试终止");
        m_pManager->MessageBox(L"初始数据接收失败，请检查CAN导线连接是否正确",
            L"CAN测试",MB_OK|MB_ICONERROR );  
        goto FAILED;
    }

    return  TEST_NORMAL;

FAILED:
    if (!VCI_ResetCAN(m_dwDeviceType,m_dwDeviceIndex,m_dwCanId)) 
    {
        m_pManager->WriteLog(this, L"复位CAN失败");
    }
    VCI_CloseDevice(m_dwDeviceType, m_dwDeviceIndex);
    return TEST_ERROR;
}

TEST_STATUS CCANTest::OnEndTest(CConnect *pConnect)
{
    if (!VCI_ResetCAN(m_dwDeviceType,m_dwDeviceIndex,m_dwCanId)) 
    {
        m_pManager->WriteLog(this, L"复位CAN失败");
    }
    VCI_CloseDevice(m_dwDeviceType, m_dwDeviceIndex);


    return TEST_NORMAL;

}

TEST_STATUS CCANTest::ExcuteOneTest(CConnect *pConnect)
{
    //清空CAN接收缓冲的内容
    VCI_ClearBuffer(m_dwDeviceType, m_dwDeviceIndex, m_dwCanId);
   
    //生成发送的数据     
    m_vco.ID = m_uiTestCount++;
    m_vco.DataLen = (BYTE)(rand() & 0x07) + 1;
    for (int  i = 0; i < m_vco.DataLen; i++) 
    {
        m_vco.Data[i] = rand();
    }

    //由can发送数据
    if (!VCI_Transmit(m_dwDeviceType,m_dwDeviceIndex,m_dwCanId,&m_vco,1)  ) 
    {
        m_pManager->WriteLog(this, L"数据发送失败");       
        goto ERROR_TEST;
    }

    //尝试接收数据
    int  iTryRecvTimes=0;
    BOOL bRecvSucess=FALSE;

    VCI_CAN_OBJ     fi[2];
    VCI_ERR_INFO    ei;
    while (iTryRecvTimes <10)
    {
        if (VCI_Receive(m_dwDeviceType,m_dwDeviceIndex,m_dwCanId, fi,1, 500)>0  )
        {
            bRecvSucess=TRUE;
            break;
        }
        VCI_ReadErrInfo(m_dwDeviceType,m_dwDeviceIndex,m_dwCanId, &ei);
        iTryRecvTimes++;
    }

    if (!bRecvSucess)
    {
        m_pManager->WriteLog(this, L"数据接收失败");
        goto ERROR_TEST;
    }

    //校验接收到的数据
    if (m_vco.ID  !=fi[0].ID)
    {
        CString     str;
        str.Format(TEXT("ID标识不一致, 发送的ID标识为%d, 接收的ID标识为%d"),
            m_vco.ID,fi[0].ID);
        m_pManager->WriteLog(this,str);
        goto ERROR_TEST;        
    }

    if (memcmp(m_vco.Data,fi[0].Data,m_vco.DataLen ) )
    {
        m_pManager->WriteLog(this,L"发送和接收的数据不一致");
        goto ERROR_TEST;  
    }
    

    return TEST_NORMAL;

ERROR_TEST:
    m_uiTestErrCount++;
    return TEST_ERROR;
}
