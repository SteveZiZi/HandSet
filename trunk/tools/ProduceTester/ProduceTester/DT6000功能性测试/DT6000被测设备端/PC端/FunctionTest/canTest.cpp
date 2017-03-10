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
        m_pManager->WriteLog(this,L"���豸ʧ��,��������ֹ");
        m_pManager->MessageBox(L"��CANʧ�ܣ�����USBCAN���������Ƿ��Ѱ�װ",
            L"CAN����",MB_OK|MB_ICONERROR );
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
        m_pManager->WriteLog(this, L"��ʼ��CANʧ��,��������ֹ");
        m_pManager->MessageBox(L"��ʼ��CANʧ��,��������ֹ",
            L"CAN����",MB_OK|MB_ICONERROR );
        goto FAILED;        
        return TEST_ERROR;
    }

    if(!VCI_StartCAN(m_dwDeviceType,
        m_dwDeviceIndex, m_dwCanId))
    {
        m_pManager->WriteLog(this, L"����CANʧ��,��������ֹ");
        m_pManager->MessageBox(L"����CANʧ��,��������ֹ",
            L"CAN����",MB_OK|MB_ICONERROR );        
        goto FAILED;
        return TEST_ERROR;
    }

    //��ʼ��CAN�������ݽṹ
    m_vco.RemoteFlag = (BYTE)m_dwRemoteFlag;
    m_vco.ExternFlag = (BYTE)m_dwExternFlag;
    m_vco.SendType = 0;
    m_vco.ID = 0;
    m_vco.DataLen = 0;

    //���ȷ��ͺͽ���һ֡������ȷ��CAN�����Ƿ���ȷ����

    if (!VCI_Transmit(m_dwDeviceType,m_dwDeviceIndex,m_dwCanId,&m_vco,1)  ) 
    {
        m_pManager->WriteLog(this, L"��ʼ���ݷ���ʧ��, ��������ֹ");
        m_pManager->MessageBox(L"��ʼ���ݷ���ʧ�ܣ�����CAN���������Ƿ���ȷ",
            L"CAN����",MB_OK|MB_ICONERROR );  
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

    //���û�н��յ����ݣ�����ʾ������Ϣ
    if (!bRecvSucess)
    {
        m_pManager->WriteLog(this, L"��ʼ���ݽ���ʧ��, ��������ֹ");
        m_pManager->MessageBox(L"��ʼ���ݽ���ʧ�ܣ�����CAN���������Ƿ���ȷ",
            L"CAN����",MB_OK|MB_ICONERROR );  
        goto FAILED;
    }

    return  TEST_NORMAL;

FAILED:
    if (!VCI_ResetCAN(m_dwDeviceType,m_dwDeviceIndex,m_dwCanId)) 
    {
        m_pManager->WriteLog(this, L"��λCANʧ��");
    }
    VCI_CloseDevice(m_dwDeviceType, m_dwDeviceIndex);
    return TEST_ERROR;
}

TEST_STATUS CCANTest::OnEndTest(CConnect *pConnect)
{
    if (!VCI_ResetCAN(m_dwDeviceType,m_dwDeviceIndex,m_dwCanId)) 
    {
        m_pManager->WriteLog(this, L"��λCANʧ��");
    }
    VCI_CloseDevice(m_dwDeviceType, m_dwDeviceIndex);


    return TEST_NORMAL;

}

TEST_STATUS CCANTest::ExcuteOneTest(CConnect *pConnect)
{
    //���CAN���ջ��������
    VCI_ClearBuffer(m_dwDeviceType, m_dwDeviceIndex, m_dwCanId);
   
    //���ɷ��͵�����     
    m_vco.ID = m_uiTestCount++;
    m_vco.DataLen = (BYTE)(rand() & 0x07) + 1;
    for (int  i = 0; i < m_vco.DataLen; i++) 
    {
        m_vco.Data[i] = rand();
    }

    //��can��������
    if (!VCI_Transmit(m_dwDeviceType,m_dwDeviceIndex,m_dwCanId,&m_vco,1)  ) 
    {
        m_pManager->WriteLog(this, L"���ݷ���ʧ��");       
        goto ERROR_TEST;
    }

    //���Խ�������
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
        m_pManager->WriteLog(this, L"���ݽ���ʧ��");
        goto ERROR_TEST;
    }

    //У����յ�������
    if (m_vco.ID  !=fi[0].ID)
    {
        CString     str;
        str.Format(TEXT("ID��ʶ��һ��, ���͵�ID��ʶΪ%d, ���յ�ID��ʶΪ%d"),
            m_vco.ID,fi[0].ID);
        m_pManager->WriteLog(this,str);
        goto ERROR_TEST;        
    }

    if (memcmp(m_vco.Data,fi[0].Data,m_vco.DataLen ) )
    {
        m_pManager->WriteLog(this,L"���ͺͽ��յ����ݲ�һ��");
        goto ERROR_TEST;  
    }
    

    return TEST_NORMAL;

ERROR_TEST:
    m_uiTestErrCount++;
    return TEST_ERROR;
}
