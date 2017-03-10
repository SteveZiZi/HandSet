#include <Windows.h>
#include <atlstr.h>
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "SerialPortTest.h"
#include "assertMsg.h"


//将全功能串口的各功能线编号
enum SIGNAL_LINE_INDEX
{
    LINE_DCD=0,
    LINE_DTR,
    LINE_DSR,
    LINE_RTS,
    LINE_CTS,
    LINE_RI,
    LINE_RXD,
    LINE_TXD,
};

//按SIGNAL_LINE_INDEX编号排列的信号线的名称
const WCHAR *szPinName[8]   = {L"(1)DCD", L"(4)DTR", L"(6)DSR", L"(7)RTS", 
                    L"(8)CTS",L"(9)RI", L"RXD", L"(3)TXD"};

//定义串口测试在通信线路上传递的数据包
#pragma  pack(push)
#pragma  pack(1)
typedef struct __SERIAL_CMD_PACK
{
    UINT32      uiSingalMask;               //表征测试信号线的掩码
    BYTE        pData[256];                 //如果是发送功能表征的是需要测试串口发送的数据
}SerialCmdPacket;



#pragma  pack(pop)

CSerialPortTest::CSerialPortTest():
m_iTestStepCount(0),
m_iTestStep(SERIAL_PORT_TEST_CTL_LINE)
{

}

CSerialPortTest::~CSerialPortTest()
{

}

BOOL CSerialPortTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"SERIALTest_\0\0";
    strSection[11]=L'1'+index;

    //读取测试项的名字
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"串口1",
        m_strTestName,31,strIniFile); 
    m_pManager=pManager;
    m_uiCmd=index+CMD_TEST_SERIAL1;

    //读取本地串口号
    m_iLocalPort=GetPrivateProfileInt(strSection,L"LocalPort",0,strIniFile);
    //读取CE端要测试的串口类型
    m_iSerialType=GetPrivateProfileInt(strSection,L"SerialType",0,strIniFile);
    //读取串口传输的数据包长度
    m_iDataLen=GetPrivateProfileInt(strSection,L"DataLen",100,strIniFile);
    m_iBraudRate =GetPrivateProfileInt(strSection,L"BraudRate",CBR_115200,strIniFile);

    return TRUE;
}

TEST_STATUS CSerialPortTest::OnBeginTest(CConnect *pConnect)
{
    BOOL    bClientSucess=FALSE;                //客户端串口打开是否成功

    if(m_iSerialType ==0)
    {
        m_iTestStep=SERIAL_PORT_TEST_RECV;
        m_iTestStepCount=0;
    }
    else
    {
        m_iTestStep=SERIAL_PORT_TEST_CTL_LINE;
        m_iTestStepCount=0;
    }
    m_uiErrMask=0;

    __try
    {
        if (!m_xCom.Open(m_iLocalPort+1,m_iBraudRate) )
        {
            WCHAR str[256];
            swprintf(str,L"打开本机串口%d失败",m_iLocalPort+1);
            m_pManager->MessageBox(str,L"串口测试",MB_OK|MB_ICONERROR);               
            
            return TEST_ERROR;
        }

        PACKET pa={m_uiCmd,SERIAL_PORT_TEST_OPEN};

        int nRet=pConnect->SendAPacket(&pa);

        if (nRet >0)
        {
            nRet=pConnect->RecvAPacket(&pa,10000);
            if (nRet >0)
            {
                TEST_INFO_TRANSFER  *pInfoT=(TEST_INFO_TRANSFER *)pa.pData;
                if (pInfoT ->bError)
                {
                    TEST_INFO ti;
                    TestInfoTransferToLocal(&ti,pInfoT);
                    m_pManager->MessageBox(ti.strErr,L"串口测试",MB_OK|MB_ICONERROR);
                    return TEST_ERROR;
                }
                bClientSucess=TRUE;
                return TEST_NORMAL;
            }
        }

        return TEST_CONNECTDOWN;
    }
    __finally
    {
        if (!bClientSucess)
        {
            m_xCom.CloseCom();
        }
    }
}

TEST_STATUS CSerialPortTest::OnEndTest(CConnect *pConnect)
{
    
    PACKET pa={m_uiCmd,SERIAL_PORT_TEST_CLOSE};

    m_xCom.CloseCom();
    int nRet=pConnect->SendAPacket(&pa);

    if (m_uiErrMask !=0)
    {
        //输出错误信息
        int index=0;

        CString strError=_T("检测到不正常的线有:");
        for (;index <=(int)LINE_TXD;index++  )
        {
            if (m_uiErrMask & (1<<(int)index)  )
            {
                strError+=L" ";
                strError+=szPinName[(int)index ];
            }
        }

        m_pManager->WriteLog(this,strError);
        m_pManager->MessageBox(strError,L"串口测试",MB_OK);
    }

    if (nRet >0)
    {
        nRet=pConnect->RecvAPacket(&pa,5000);
        if (nRet >0)
        {
            TEST_INFO_TRANSFER  *pInfoT=(TEST_INFO_TRANSFER *)pa.pData;
            if (pInfoT ->bError)
            {
                TEST_INFO ti;
                TestInfoTransferToLocal(&ti,pInfoT);
                m_pManager->MessageBox(ti.strErr,L"串口测试",MB_OK|MB_ICONERROR);
                return TEST_ERROR;
            }
            return TEST_NORMAL;
        }
    }

    return TEST_CONNECTDOWN;
   
}

TEST_STATUS CSerialPortTest::ExcuteOneTest(CConnect *pConnect)
{
    switch (m_iTestStep)
    {
    case SERIAL_PORT_TEST_CTL_LINE:
        return ExcuteOneCTLLinTest(pConnect);
    case SERIAL_PORT_TEST_RECV:
        return ExcuteOneRecvTest(pConnect);
    case  SERIAL_PORT_TEST_SEND:
        return ExcuteOneSendTest(pConnect);
    default:
        return TEST_NORMAL;
    }

    return TEST_NORMAL;
}

int CSerialPortTest::GetShouldTestTimes()const
{
    assertMsg(m_iSerialType ==SERIAL_PORT_TYPE_SIMPLE ||
        m_iSerialType ==SERIAL_PORT_TYPE_FULL_FUNC,
        L"串口类型不支持")

    if (m_iSerialType ==SERIAL_PORT_TYPE_FULL_FUNC)
    {
        return 12;
    }
    else
    {
        return 6;
    }
}

TEST_STATUS CSerialPortTest::ExcuteOneRecvTest(CConnect *pConnect)
{
    SerialCmdPacket     scp;
    PACKET              pa;
    scp.uiSingalMask=0;

    for (int i=0;i<m_iDataLen;i++)
    {
        scp.pData[i]=rand();
    }

    int nRet=pConnect->SendAPacket(m_uiCmd,SERIAL_PORT_TEST_RECV,sizeof(scp),
        (BYTE *)&scp);
    if (nRet >0)
    {
        nRet=pConnect->RecvAPacket(&pa,5000);
        if (nRet >0)
        {
            m_xCom.WriteNByte(scp.pData,m_iDataLen);
            nRet=pConnect->RecvAPacket(&pa,15000);
            if (nRet >0)
            {
                m_uiTestCount++;
                m_iTestStepCount++;

                if (m_iTestStepCount >=3)
                {
                    m_iTestStepCount=0;
                    m_iTestStep=SERIAL_PORT_TEST_SEND;
                }
                TEST_INFO_TRANSFER *pInfo=(TEST_INFO_TRANSFER *)pa.pData;
                if (pInfo ->bError)
                {
                    //出现了错误
                    TEST_INFO ti;
                    TestInfoTransferToLocal(&ti,pInfo);
                    m_uiTestErrCount++;
                    m_pManager->WriteLog(this,ti.strErr);
                    m_uiErrMask |=1<<LINE_RXD;
                    return TEST_ERROR;
                }
                else
                {
                    //校验数据
                    if (memcmp(scp.pData,pInfo->strErr,m_iDataLen) !=0)
                    {
                        m_uiTestErrCount++;
                        m_pManager->WriteLog(this,L"数据校验错误");
                        m_uiErrMask |=1<<LINE_RXD;
                        return TEST_ERROR;
                    }
                    return TEST_NORMAL;

                }
            }
        }        
    }

    return TEST_CONNECTDOWN;
}

TEST_STATUS CSerialPortTest::ExcuteOneSendTest(CConnect *pConnect)
{

    SerialCmdPacket     scp;
    PACKET              pa;
    scp.uiSingalMask=0;

    for (int i=0;i<m_iDataLen;i++)
    {
        scp.pData[i]=rand();
    }

    m_xCom.ClearRecv();
    DWORD       Err;
    COMSTAT     stat;
    m_xCom.ClearCommError(&Err,&stat);
    int nRet=pConnect->SendAPacket(m_uiCmd,SERIAL_PORT_TEST_SEND,sizeof(scp),
        (BYTE *)&scp);
    if (nRet >0)
    {       
        nRet=pConnect->RecvAPacket(&pa,1500);        
    }

    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    BYTE        buf[256];
    size_t      siReturn=0;
    m_uiTestCount++;
    m_iTestStepCount++;

    if (m_iTestStepCount >=3)
    {
        m_iTestStepCount=0;
        m_iTestStep=SERIAL_PORT_TEST_CTL_LINE;
    }

    if (!m_xCom.ReadNByteTimeOut(buf,m_iDataLen,5000, &siReturn))
    {
        if (siReturn>0)
        {
            m_pManager->WriteLog(this,L"串口发送的数据出现丢失");
        }
        else
        {
            m_pManager->WriteLog(this,L"未收到串口发送的数据");
        }
        m_uiTestErrCount++;
        m_uiErrMask |=1<<LINE_TXD;
        return TEST_ERROR;
    }

    //校验数据
    if (memcmp(scp.pData,buf,m_iDataLen) !=0)
    {
        m_uiTestErrCount++;
        m_pManager->WriteLog(this,L"数据校验错误");
        m_uiErrMask |=1<<LINE_TXD;
        return TEST_ERROR;
    }
    
    return TEST_NORMAL;
}

TEST_STATUS CSerialPortTest::ExcuteOneCTLLinTest(CConnect *pConnect)
{
    TEST_INFO_TRANSFER  *pTit;
    PACKET               pa;

    UINT32  t=m_iTestStepCount;
    m_iTestStepCount++;
    if (m_iTestStepCount >6)
    {
        m_iTestStep=SERIAL_PORT_TEST_RECV;
        m_iTestStepCount=0;
    }

    int nRet=pConnect->SendAPacket(m_uiCmd,SERIAL_PORT_TEST_CTL_LINE,
        sizeof(t),(BYTE *)&t);
    if (nRet >0)
    {
        nRet =pConnect->RecvAPacket(&pa,3000);
        if (nRet >0)
        {
            m_uiTestCount++;
            pTit=(TEST_INFO_TRANSFER *)pa.pData;
            if (pTit->bError)
            {
                m_uiTestErrCount++;
                m_uiErrMask |=1<<t;
                return TEST_ERROR;
            }
            return TEST_NORMAL;
        }
    }

    return TEST_CONNECTDOWN;
}