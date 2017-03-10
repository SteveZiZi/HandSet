#include "stdafx.h"
#include "testBase.h"
#include "iniFile.h"
#include "assertMsg.h"
#include "epcSerial.h"
#include "SerialPortTest.h"

CSerialPortTest::CSerialPortTest():
m_ucPort1(0),
m_ucPort2(0),
m_bDir(FALSE),
m_bDevInitSucess(FALSE)
{
    for(int i=0;i<2;i++)
    {
        m_serData[i].dwBufCnt=0;    
        m_serData[i].hRecvEvent=NULL;
    }

    wcscpy(m_reportNape[0].strNape,L"每次传输字节");
    wcscpy(m_reportNape[1].strNape,L"测试次数");
    wcscpy(m_reportNape[2].strNape,L"错误次数");
    wcscpy(m_reportNape[3].strNape,L"正确率");
    wcscpy(m_reportNape[4].strNape,L"持续时间"); 
}

CSerialPortTest::~CSerialPortTest()
{
    //关闭所有打开的串口
    epcSerialClose(m_ucPort1);
    if (m_ucPort1 !=m_ucPort2)
    {
        epcSerialClose(m_ucPort2);
    }   
}

BOOL CSerialPortTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"SERIALTest_\0\0";
    strSection[wcslen(strSection)]=L'1'+index;

    //读取测试项的名字
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"串口1和2",
        m_strTestName,31,strIniFile);

    //读取测试的方式，是自发自收，还是对发对收
    int way;
    way=GetPrivateProfileInt(strSection,L"TestWay",0,strIniFile);
    //读取要测试的串口编号1
    m_ucPort1=GetPrivateProfileInt(strSection,L"TestCom1",0,strIniFile);

    if (way !=0)
    {
        //如果是对发对收，读取要测试的串口编号2
        m_ucPort2=GetPrivateProfileInt(strSection,L"TestCom2",0,strIniFile);
    }
    else
    {
        //如果是自发自收的话，串口编号2和1一样
        m_ucPort2=m_ucPort1;
    }    
   
    m_bDevInitSucess=FALSE;
    m_pManager = pManager;
    return InitSerialDev();
}

TEST_SITUATION CSerialPortTest::ExcuteOneTest()
{
    if (m_ucPort1 ==m_ucPort2)
    {
        return ExcuteOneTestLoopBack();
    }
    else
    {
        return ExcuteOneTestEachOther();
    }        
}

BOOL CSerialPortTest::InitSerialDev()
{
    if (m_bDevInitSucess )
    {       
        return TRUE;
    }

    //尝试打开编号1所对应的串口
    if (epcSerialOpen(m_ucPort1,BAUD9600) !=COM_OK)
    {
        swprintf(m_strLastError,L"打开串口%d失败！",m_ucPort1+1);
        MessageBox(NULL,m_strLastError,L"错误",MB_OK|MB_ICONERROR);
        goto Init_error;
    }

    //启动编号1所对应的串口的接收
    m_serData[0].hRecvEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
    if (!epcSerialRcvDataTread(m_ucPort1,1024,0,OnCom1Recv,this)  )
    {   
        swprintf(m_strLastError,L"启动串口%d接收失败！",m_ucPort1+1);
        MessageBox(NULL,m_strLastError,L"错误",MB_OK|MB_ICONERROR);
        goto Init_error;
    }

    if (m_ucPort1==m_ucPort2)
    {        
        m_bDevInitSucess=TRUE;
        return TRUE;
    }

    //尝试打开编号2所对应的串口
    if (epcSerialOpen(m_ucPort2,BAUD9600) !=COM_OK)
    {
        swprintf(m_strLastError,L"打开串口%d失败！",m_ucPort2+1);
        MessageBox(NULL,m_strLastError,L"错误",MB_OK|MB_ICONERROR);
        goto Init_error;
    }

    //启动编号2所对应的串口的接收
    m_serData[1].hRecvEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
    if (!epcSerialRcvDataTread(m_ucPort2,1024,0,OnCom2Recv,this)  )
    {
        swprintf(m_strLastError,L"启动串口%d接收失败！",m_ucPort2+1);
        MessageBox(NULL,m_strLastError,L"错误",MB_OK|MB_ICONERROR);
        goto Init_error;
    }
   
    m_bDevInitSucess=TRUE;
    return TRUE;
Init_error:
    epcSerialClose(m_ucPort1);
    epcSerialClose(m_ucPort2);    
    for (int i=0; i<2;i++)
    {
        if (m_serData[i].hRecvEvent !=NULL)
        {
            CloseHandle(m_serData[i].hRecvEvent );
            m_serData[i].hRecvEvent=NULL;
        }
    }

    return FALSE;
}

void CSerialPortTest::OnCom1Recv(LPVOID pvUserParam, BYTE *pucBuf, DWORD dwRcvLen)
{
    CSerialPortTest *pTest=(CSerialPortTest *)pvUserParam;
    SERIALDATA &s=pTest->m_serData[0];

    memcpy(s.cBuf+s.dwBufCnt,pucBuf,dwRcvLen);
    s.dwBufCnt+=dwRcvLen;

    if (s.dwBufCnt >=__SERIAL_DATA_LEN)
    {
        SetEvent(s.hRecvEvent);
    }
}

void CSerialPortTest::OnCom2Recv(LPVOID pvUserParam, BYTE *pucBuf, DWORD dwRcvLen)
{
    CSerialPortTest *pTest=(CSerialPortTest *)pvUserParam;
    SERIALDATA &s=pTest->m_serData[1];

    memcpy(s.cBuf+s.dwBufCnt,pucBuf,dwRcvLen);
    s.dwBufCnt+=dwRcvLen;

    if (s.dwBufCnt >=__SERIAL_DATA_LEN)
    {
        SetEvent(s.hRecvEvent);
    }
}


TEST_SITUATION CSerialPortTest::ExcuteOneTestLoopBack()
{
    int     i;
    DWORD   dwRet;

    TEST_SITUATION      siRet=SITUATION_NORMAL;
    //生成测试数据
    for (i=0;i<__SERIAL_DATA_LEN;i++ )
    {
        m_pDatabuf[i]=rand()&0xFF;
    }

   //清空串口编号1的接收
    m_serData[0].dwBufCnt=0;
    ResetEvent(m_serData[0].hRecvEvent);
    //发送测试数据
    epcSerialSendData(m_ucPort1,m_pDatabuf,__SERIAL_DATA_LEN);
    //等待接收到数据
    dwRet=WaitForSingleObject(m_serData[0].hRecvEvent,5000);
    if (m_serData[0].dwBufCnt >=__SERIAL_DATA_LEN)
    {
        //接收到数据，进行数据的对比
        if ( memcmp(m_pDatabuf,m_serData[0].cBuf,__SERIAL_DATA_LEN ) ==0 )
        {
            siRet=SITUATION_NORMAL;
        }
        else
        {
            //发送和接收到的数据不一致
            siRet=SITUATION_ERROR;
            swprintf(m_strLastError,L"串口%d接收到的数据和自己发送的数据不一致",
                m_ucPort1+1);

        }
    }
    else if (m_serData[0].dwBufCnt ==0)
    {
        //没有接收到数据
        siRet=SITUATION_ERROR;
        swprintf(m_strLastError,L"串口%d没有接收到自己发送的数据",m_ucPort1+1) ;
    }
    else
    {
       
        siRet=SITUATION_ERROR;
        swprintf(m_strLastError,L"串口%d接收到自己发送的数据出现了丢失",
            m_ucPort1+1) ;
    }

    m_uiTestCount++;
    if (siRet !=SITUATION_NORMAL)
    {
        m_uiErrCount++;
    }
    return siRet;
}

TEST_SITUATION CSerialPortTest::ExcuteOneTestEachOther()
{
    
    int     i;
    DWORD   dwRet;

    TEST_SITUATION      siRet=SITUATION_NORMAL;
    //生成测试数据
    for (i=0;i<__SERIAL_DATA_LEN;i++ )
    {
        m_pDatabuf[i]=rand()&0xFF;
    }

    m_bDir=!m_bDir;
    if (m_bDir)
    {
        //清空串口编号1的接收
        m_serData[0].dwBufCnt=0;
        ResetEvent(m_serData[0].hRecvEvent);
        //编号2的串口发送测试数据
        epcSerialSendData(m_ucPort2,m_pDatabuf,__SERIAL_DATA_LEN);
        //等待接收到数据
        dwRet=WaitForSingleObject(m_serData[0].hRecvEvent,5000);
        if (m_serData[0].dwBufCnt >=__SERIAL_DATA_LEN)
        {
            //接收到数据，进行数据的对比
            if ( memcmp(m_pDatabuf,m_serData[0].cBuf,__SERIAL_DATA_LEN ) ==0 )
            {
                siRet=SITUATION_NORMAL;
            }
            else
            {
                //发送和接收到的数据不一致
                siRet=SITUATION_ERROR;
                swprintf(m_strLastError,L"串口%d接收到的数据和串口%d发送的数据不一致",
                    m_ucPort1+1,m_ucPort2+1);

            }
        }
        else if (m_serData[0].dwBufCnt ==0)
        {
            //没有接收到数据
            siRet=SITUATION_ERROR;
            swprintf(m_strLastError,L"串口%d没有接收到串口%d发送的数据",m_ucPort1+1,
                m_ucPort2+1) ;
        }
        else
        {
            //接收到数据出现丢失
            siRet=SITUATION_NORMAL;
            swprintf(m_strLastError,L"串口%d接收到的串口%d发送的数据出现了丢失",
                m_ucPort1+1,m_ucPort2+1);
        }
    }
    else
    {
        //清空串口编号2的接收
        m_serData[1].dwBufCnt=0;
        ResetEvent(m_serData[1].hRecvEvent);
        //编号1的串口发送测试数据
        epcSerialSendData(m_ucPort1,m_pDatabuf,__SERIAL_DATA_LEN);
        //等待接收到数据
        dwRet=WaitForSingleObject(m_serData[1].hRecvEvent,5000);
        if (m_serData[1].dwBufCnt >=__SERIAL_DATA_LEN)
        {
            //接收到数据，进行数据的对比
            if ( memcmp(m_pDatabuf,m_serData[1].cBuf,__SERIAL_DATA_LEN ) ==0 )
            {
                siRet=SITUATION_NORMAL;                
            }
            else
            {
                //发送和接收到的数据不一致
                siRet=SITUATION_ERROR;
                swprintf(m_strLastError,L"串口%d接收到的数据和串口%d发送的数据不一致",
                    m_ucPort2+1,m_ucPort1+1);

            }
        }
        else if (m_serData[1].dwBufCnt ==0)
        {
            //没有接收到数据
            siRet=SITUATION_ERROR;
            swprintf(m_strLastError,L"串口%d没有接收到串口%d发送的数据",m_ucPort2+1,
                m_ucPort1+1);
        }
        else
        {
            //没有接收到数据
            siRet=SITUATION_ERROR;
            swprintf(m_strLastError,L"串口%d接收到的串口%d发送的数据出现了丢失",
                m_ucPort2+1,m_ucPort1+1);
        }
    }

    m_uiTestCount++;
    if (siRet !=SITUATION_NORMAL)
    {
        m_uiErrCount++;
    }    

    return siRet;   
}

const SReportNape *CSerialPortTest::GetReportNapes(int &count)
{
    unsigned int h,m,s;
    UINT64 t=m_uiTestTime;
    count=5;

    swprintf(m_reportNape[0].strNapeDescriptor,L"%d字节",__SERIAL_DATA_LEN);
    m_reportNape[0].bHighLight=FALSE;  

    swprintf(m_reportNape[1].strNapeDescriptor,L"%d次",m_uiTestCount);
    m_reportNape[1].bHighLight=FALSE;

    swprintf(m_reportNape[2].strNapeDescriptor,L"%d次",m_uiErrCount);
    if (m_uiTestCount >0)
    {
        swprintf(m_reportNape[3].strNapeDescriptor,L"%.2f%% ",
            100 *(double)(m_uiTestCount-m_uiErrCount)/m_uiTestCount );
    }
    else
    {
        swprintf(m_reportNape[3].strNapeDescriptor,L"0.00%% ");
    }    

    if (m_uiErrCount >0)
    {
        m_reportNape[2].bHighLight=TRUE;
        m_reportNape[3].bHighLight=TRUE;
    }
    else
    {
        m_reportNape[2].bHighLight=FALSE;
        m_reportNape[3].bHighLight=FALSE;
    }  

    h= (unsigned int)( t/(1000*3600));
    t=t%(1000*3600);
    m=(unsigned int) (t/(1000 *60));
    t=t%(1000 *60);
    s=(unsigned int)(t/1000);

    swprintf(m_reportNape[4].strNapeDescriptor,L"%d时%d分%d秒",h,m,s);
    m_reportNape[4].bHighLight=FALSE;

    return m_reportNape;
}