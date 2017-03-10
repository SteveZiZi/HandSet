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

    wcscpy(m_reportNape[0].strNape,L"ÿ�δ����ֽ�");
    wcscpy(m_reportNape[1].strNape,L"���Դ���");
    wcscpy(m_reportNape[2].strNape,L"�������");
    wcscpy(m_reportNape[3].strNape,L"��ȷ��");
    wcscpy(m_reportNape[4].strNape,L"����ʱ��"); 
}

CSerialPortTest::~CSerialPortTest()
{
    //�ر����д򿪵Ĵ���
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

    //��ȡ�����������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"����1��2",
        m_strTestName,31,strIniFile);

    //��ȡ���Եķ�ʽ�����Է����գ����ǶԷ�����
    int way;
    way=GetPrivateProfileInt(strSection,L"TestWay",0,strIniFile);
    //��ȡҪ���ԵĴ��ڱ��1
    m_ucPort1=GetPrivateProfileInt(strSection,L"TestCom1",0,strIniFile);

    if (way !=0)
    {
        //����ǶԷ����գ���ȡҪ���ԵĴ��ڱ��2
        m_ucPort2=GetPrivateProfileInt(strSection,L"TestCom2",0,strIniFile);
    }
    else
    {
        //������Է����յĻ������ڱ��2��1һ��
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

    //���Դ򿪱��1����Ӧ�Ĵ���
    if (epcSerialOpen(m_ucPort1,BAUD9600) !=COM_OK)
    {
        swprintf(m_strLastError,L"�򿪴���%dʧ�ܣ�",m_ucPort1+1);
        MessageBox(NULL,m_strLastError,L"����",MB_OK|MB_ICONERROR);
        goto Init_error;
    }

    //�������1����Ӧ�Ĵ��ڵĽ���
    m_serData[0].hRecvEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
    if (!epcSerialRcvDataTread(m_ucPort1,1024,0,OnCom1Recv,this)  )
    {   
        swprintf(m_strLastError,L"��������%d����ʧ�ܣ�",m_ucPort1+1);
        MessageBox(NULL,m_strLastError,L"����",MB_OK|MB_ICONERROR);
        goto Init_error;
    }

    if (m_ucPort1==m_ucPort2)
    {        
        m_bDevInitSucess=TRUE;
        return TRUE;
    }

    //���Դ򿪱��2����Ӧ�Ĵ���
    if (epcSerialOpen(m_ucPort2,BAUD9600) !=COM_OK)
    {
        swprintf(m_strLastError,L"�򿪴���%dʧ�ܣ�",m_ucPort2+1);
        MessageBox(NULL,m_strLastError,L"����",MB_OK|MB_ICONERROR);
        goto Init_error;
    }

    //�������2����Ӧ�Ĵ��ڵĽ���
    m_serData[1].hRecvEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
    if (!epcSerialRcvDataTread(m_ucPort2,1024,0,OnCom2Recv,this)  )
    {
        swprintf(m_strLastError,L"��������%d����ʧ�ܣ�",m_ucPort2+1);
        MessageBox(NULL,m_strLastError,L"����",MB_OK|MB_ICONERROR);
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
    //���ɲ�������
    for (i=0;i<__SERIAL_DATA_LEN;i++ )
    {
        m_pDatabuf[i]=rand()&0xFF;
    }

   //��մ��ڱ��1�Ľ���
    m_serData[0].dwBufCnt=0;
    ResetEvent(m_serData[0].hRecvEvent);
    //���Ͳ�������
    epcSerialSendData(m_ucPort1,m_pDatabuf,__SERIAL_DATA_LEN);
    //�ȴ����յ�����
    dwRet=WaitForSingleObject(m_serData[0].hRecvEvent,5000);
    if (m_serData[0].dwBufCnt >=__SERIAL_DATA_LEN)
    {
        //���յ����ݣ��������ݵĶԱ�
        if ( memcmp(m_pDatabuf,m_serData[0].cBuf,__SERIAL_DATA_LEN ) ==0 )
        {
            siRet=SITUATION_NORMAL;
        }
        else
        {
            //���ͺͽ��յ������ݲ�һ��
            siRet=SITUATION_ERROR;
            swprintf(m_strLastError,L"����%d���յ������ݺ��Լ����͵����ݲ�һ��",
                m_ucPort1+1);

        }
    }
    else if (m_serData[0].dwBufCnt ==0)
    {
        //û�н��յ�����
        siRet=SITUATION_ERROR;
        swprintf(m_strLastError,L"����%dû�н��յ��Լ����͵�����",m_ucPort1+1) ;
    }
    else
    {
       
        siRet=SITUATION_ERROR;
        swprintf(m_strLastError,L"����%d���յ��Լ����͵����ݳ����˶�ʧ",
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
    //���ɲ�������
    for (i=0;i<__SERIAL_DATA_LEN;i++ )
    {
        m_pDatabuf[i]=rand()&0xFF;
    }

    m_bDir=!m_bDir;
    if (m_bDir)
    {
        //��մ��ڱ��1�Ľ���
        m_serData[0].dwBufCnt=0;
        ResetEvent(m_serData[0].hRecvEvent);
        //���2�Ĵ��ڷ��Ͳ�������
        epcSerialSendData(m_ucPort2,m_pDatabuf,__SERIAL_DATA_LEN);
        //�ȴ����յ�����
        dwRet=WaitForSingleObject(m_serData[0].hRecvEvent,5000);
        if (m_serData[0].dwBufCnt >=__SERIAL_DATA_LEN)
        {
            //���յ����ݣ��������ݵĶԱ�
            if ( memcmp(m_pDatabuf,m_serData[0].cBuf,__SERIAL_DATA_LEN ) ==0 )
            {
                siRet=SITUATION_NORMAL;
            }
            else
            {
                //���ͺͽ��յ������ݲ�һ��
                siRet=SITUATION_ERROR;
                swprintf(m_strLastError,L"����%d���յ������ݺʹ���%d���͵����ݲ�һ��",
                    m_ucPort1+1,m_ucPort2+1);

            }
        }
        else if (m_serData[0].dwBufCnt ==0)
        {
            //û�н��յ�����
            siRet=SITUATION_ERROR;
            swprintf(m_strLastError,L"����%dû�н��յ�����%d���͵�����",m_ucPort1+1,
                m_ucPort2+1) ;
        }
        else
        {
            //���յ����ݳ��ֶ�ʧ
            siRet=SITUATION_NORMAL;
            swprintf(m_strLastError,L"����%d���յ��Ĵ���%d���͵����ݳ����˶�ʧ",
                m_ucPort1+1,m_ucPort2+1);
        }
    }
    else
    {
        //��մ��ڱ��2�Ľ���
        m_serData[1].dwBufCnt=0;
        ResetEvent(m_serData[1].hRecvEvent);
        //���1�Ĵ��ڷ��Ͳ�������
        epcSerialSendData(m_ucPort1,m_pDatabuf,__SERIAL_DATA_LEN);
        //�ȴ����յ�����
        dwRet=WaitForSingleObject(m_serData[1].hRecvEvent,5000);
        if (m_serData[1].dwBufCnt >=__SERIAL_DATA_LEN)
        {
            //���յ����ݣ��������ݵĶԱ�
            if ( memcmp(m_pDatabuf,m_serData[1].cBuf,__SERIAL_DATA_LEN ) ==0 )
            {
                siRet=SITUATION_NORMAL;                
            }
            else
            {
                //���ͺͽ��յ������ݲ�һ��
                siRet=SITUATION_ERROR;
                swprintf(m_strLastError,L"����%d���յ������ݺʹ���%d���͵����ݲ�һ��",
                    m_ucPort2+1,m_ucPort1+1);

            }
        }
        else if (m_serData[1].dwBufCnt ==0)
        {
            //û�н��յ�����
            siRet=SITUATION_ERROR;
            swprintf(m_strLastError,L"����%dû�н��յ�����%d���͵�����",m_ucPort2+1,
                m_ucPort1+1);
        }
        else
        {
            //û�н��յ�����
            siRet=SITUATION_ERROR;
            swprintf(m_strLastError,L"����%d���յ��Ĵ���%d���͵����ݳ����˶�ʧ",
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

    swprintf(m_reportNape[0].strNapeDescriptor,L"%d�ֽ�",__SERIAL_DATA_LEN);
    m_reportNape[0].bHighLight=FALSE;  

    swprintf(m_reportNape[1].strNapeDescriptor,L"%d��",m_uiTestCount);
    m_reportNape[1].bHighLight=FALSE;

    swprintf(m_reportNape[2].strNapeDescriptor,L"%d��",m_uiErrCount);
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

    swprintf(m_reportNape[4].strNapeDescriptor,L"%dʱ%d��%d��",h,m,s);
    m_reportNape[4].bHighLight=FALSE;

    return m_reportNape;
}