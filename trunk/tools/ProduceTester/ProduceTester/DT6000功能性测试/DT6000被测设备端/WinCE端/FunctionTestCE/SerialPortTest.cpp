#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "SerialPortTest.h"
#include "assertMsg.h"
#include "iniFile.h"
#include "epcIoLib.h"

//���ڲ��ԵĲ���
#define     SERIAL_PORT_TEST_CTL_LINE       0       //���Կ����ź���
#define     SERIAL_PORT_TEST_RECV           1       //����
#define     SERIAL_PORT_TEST_SEND           2       //���Է���
#define     SERIAL_PORT_TEST_OPEN           3       //�򿪴���
#define     SERIAL_PORT_TEST_CLOSE          4       //�رմ���
//��ȫ���ܴ��ڵĸ������߱��
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

//��SIGNAL_LINE_INDEX������е��ź��ߵ�����
const WCHAR *szPinName[8]   = {L"(1)DCD", L"(4)DTR", L"(6)DSR", L"(7)RTS", 
L"(8)CTS",L"(9)RI", L"RXD", L"(3)TXD"};

//���崮�ڲ�����ͨ����·�ϴ��ݵ����ݰ�
#pragma  pack(push)
#pragma  pack(1)
typedef struct __SERIAL_CMD_PACK
{
    UINT32      uiSingalMask;               //���������ź��ߵ�����
    BYTE        pData[256];                 //����Ƿ��͹��ܱ���������Ҫ���Դ��ڷ��͵�����
}SerialCmdPacket;



#pragma  pack(pop)

CSerialPortTest::CSerialPortTest():
m_iSerialPort(0)
{

}

CSerialPortTest::~CSerialPortTest()
{

}

BOOL CSerialPortTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"SERIALTest_\0\0";
    strSection[11]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"����1",m_strTestName,31,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_SERIAL1;
    *uipCmdIndex=m_uiCmd;

    m_iSerialPort=GetPrivateProfileInt(strSection,L"SerialPort",0,strIniFile);
    //��ȡ���ڴ�������ݰ�����
    m_iDataLen=GetPrivateProfileInt(strSection,L"DataLen",100,strIniFile);

    //��ȡ������
    m_iBraudRate =GetPrivateProfileInt(strSection,L"BraudRate",CBR_115200,strIniFile);
    //��ȡ��������
    int type=GetPrivateProfileInt(strSection,L"SerialType",0,strIniFile);
    if (type )
    {
        if (!LoadEpcIoLibrary() )
        {
            MessageBox(NULL,L"����epcIoLib.dllʧ��",L"����",MB_OK);
            return FALSE;
        }
    }

    return TRUE;
}

TEST_STATUS CSerialPortTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
                                       const BYTE *pData,CConnect *pConnect)
{
    
    switch (param)
    {
    case SERIAL_PORT_TEST_OPEN:
        {
            int                 nRet;
            TEST_INFO_TRANSFER  tit;
            m_ti.bError=0;
            if (  !m_serialCom.Open(m_iSerialPort+1,m_iBraudRate) )
            {
                m_ti.bError=1;
                m_ti.uiErrorLen=swprintf(m_ti.strErr,
                    L"��%sʧ��",m_strTestName);
            }
            TestInfoLocalToTransfer(&tit,&m_ti);
            nRet=pConnect->SendAPacket(m_uiCmd,param,sizeof(tit),(BYTE *)&tit);
            return nRet >0?TEST_NORMAL:TEST_CONNECTDOWN;
        }
        break;
    case SERIAL_PORT_TEST_CLOSE:
        {
            int                 nRet;
            TEST_INFO_TRANSFER  tit;
            m_serialCom.CloseCom();
            tit.bError=0;
            nRet=pConnect->SendAPacket(m_uiCmd,param,sizeof(tit),(BYTE *)&tit);
            return nRet >0?TEST_NORMAL:TEST_CONNECTDOWN;
        }
        break;
    case SERIAL_PORT_TEST_RECV:
        return ExcuteOneRecvTest(uiDatalen,pData,pConnect);
        break;
    case  SERIAL_PORT_TEST_SEND:
        return ExcuteOneSendTest(uiDatalen,pData,pConnect);
        break;
    case SERIAL_PORT_TEST_CTL_LINE:
        return ExcuteOneCTLLinTest(uiDatalen,pData,pConnect);
        break;
    }

    return TEST_NORMAL;
}

TEST_STATUS CSerialPortTest::ExcuteOneRecvTest(UINT16 uiDatalen,
                                               const BYTE *pData,CConnect *pConnect)
{
    int             nRet;
    DWORD           dwErr;
    COMSTAT         stat;
    size_t          siReturn;

    m_serialCom.ClearCommError(&dwErr,&stat);
    m_serialCom.ClearRecv();
    //�ظ�һ���������������Ѿ�׼����
    nRet =pConnect->SendAPacket(m_uiCmd,SERIAL_PORT_TEST_RECV,0,NULL);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    TEST_INFO_TRANSFER tit;
    BYTE buf[256];
    
    if (!m_serialCom.ReadNByteTimeOut(buf,m_iDataLen,6000,&siReturn) )
    {
        m_ti.bError=1;
        if (siReturn >0)
        {
            m_ti.uiErrorLen=swprintf(m_ti.strErr,L"PC�������Ĵ������ݳ��ֶ�ʧ");
        }
        else
        {
            m_ti.uiErrorLen=swprintf(m_ti.strErr,L"δ�յ�PC�������Ĵ�������");
        }
        TestInfoLocalToTransfer(&tit,&m_ti);
    }
    else
    {
        tit.bError=0;
        tit.uiErrorLen=m_iDataLen;
        memcpy(tit.strErr,buf,m_iDataLen);
    }
    
    nRet=pConnect->SendAPacket(m_uiCmd,SERIAL_PORT_TEST_RECV,sizeof(tit),
       ( BYTE *)&tit);
        

    return nRet >0?TEST_NORMAL:TEST_CONNECTDOWN;
}

TEST_STATUS CSerialPortTest::ExcuteOneSendTest(UINT16 uiDatalen,
                                               const BYTE *pData,CConnect *pConnect)
{
    const   SerialCmdPacket *pCmdPacket=(const SerialCmdPacket*)pData;
    int     nRet;
    size_t  siRet=0;

    m_serialCom.WriteNByte(pCmdPacket->pData,m_iDataLen,&siRet);
    nRet=pConnect->SendAPacket(m_uiCmd,SERIAL_PORT_TEST_SEND,
        0,NULL);  

    return nRet >0?TEST_NORMAL:TEST_CONNECTDOWN;
}

#define GPO_DCD         0                   //��DCD��������GPO0
#define GPO_DSR         1                   //��DSR��������GPO1
#define GPO_CTS         2                   //��CTS��������GPO5
#define GPO_RI          3                   //��RING��������GPO3

#define GPI_DTR         1                   //��DTR��������GPI1
#define GPI_RTS         0                   //��RTS��������GPI0



TEST_STATUS CSerialPortTest::ExcuteOneCTLLinTest(UINT16 uiDatalen,
                                                 const BYTE *pData,CConnect *pConnect)
{
    UINT32              t;
    memcpy(&t,pData,sizeof(t) );
    SIGNAL_LINE_INDEX   LineIndex =(SIGNAL_LINE_INDEX)(int)t;
    TEST_INFO_TRANSFER  tit;
    tit.uiErrorLen=0;
    tit.bError=0;

    switch (LineIndex)
    {
    case LINE_DCD:
        {
            //��DCD��Ϊ��
            DWORD   dwState1=0,dwState2=0;
            epcIoWrite( 1<<GPO_DCD);
            Sleep(50);
            m_serialCom.GetCommModemStatus(&dwState1);
            epcIoWrite( ~(1<< GPO_DCD) );
            Sleep(50);
            m_serialCom.GetCommModemStatus(&dwState2);

            //У��modem��״̬
            if ( (dwState2 &MS_RLSD_ON) && !(dwState1 &MS_RLSD_ON)  )
            {
                //�������
                tit.bError=0;
            }
            else
            {
                tit.bError=1;
            }
        }
        break;
    case LINE_DTR:
        {
            DWORD dwState1=0,dwState2=0;
            //ͨ��GPI�����DTR��״̬
            m_serialCom.EscapeCommFunction(SETDTR);
            Sleep(50);
            epcIoRead(&dwState1);
            m_serialCom.EscapeCommFunction(CLRDTR);
            Sleep(50);
            epcIoRead(&dwState2);

            if ( (dwState2&1<<GPI_DTR) && !(dwState1 & 1<<GPI_DTR)   )
            {
                tit.bError=0;
            }
            else
            {
                tit.bError=1;
            }
        }

        break;
    case LINE_DSR:
        {            
            DWORD   dwState1=0,dwState2=0;
            epcIoWrite( 1<<GPO_DSR);
            Sleep(50);
            m_serialCom.GetCommModemStatus(&dwState1);
            epcIoWrite( ~(1<< GPO_DSR) );
            Sleep(50);
            m_serialCom.GetCommModemStatus(&dwState2);

            //У��modem��״̬
            if ( (dwState2 &MS_DSR_ON) && !(dwState1 &MS_DSR_ON)  )
            {
                //�������
                tit.bError=0;
            }
            else
            {
                tit.bError=1;
            }
        }
        break;
    case LINE_RTS:
        {
            DWORD dwState1=0,dwState2=0;
            //ͨ��GPI�����RTS��״̬
            m_serialCom.EscapeCommFunction(SETRTS);
            Sleep(50);
            epcIoRead(&dwState1);
            m_serialCom.EscapeCommFunction(CLRRTS);
            Sleep(50);
            epcIoRead(&dwState2);

            if ( (dwState2&1<<GPI_RTS) && !(dwState1 & 1<<GPI_RTS)   )
            {
                tit.bError=0;
            }
            else
            {
                tit.bError=1;
            }
        }
        break;
    case LINE_CTS:
        {            
            DWORD   dwState1=0,dwState2=0;
            epcIoWrite( 1<<GPO_CTS);
            Sleep(50);
            m_serialCom.GetCommModemStatus(&dwState1);
            epcIoWrite( ~(1<< GPO_CTS) );
            Sleep(50);
            m_serialCom.GetCommModemStatus(&dwState2);

            //У��modem��״̬
            if ( (dwState2 &MS_CTS_ON) && !(dwState1 &MS_CTS_ON)  )
            {
                //�������
                tit.bError=0;
            }
            else
            {
                tit.bError=1;
            }
        }
        break;
    case  LINE_RI:
        {            
            DWORD   dwState1=0,dwState2=0;
            epcIoWrite( 1<<GPO_RI);
            Sleep(50);
            m_serialCom.GetCommModemStatus(&dwState1);
            epcIoWrite( ~(1<< GPO_RI) );
            Sleep(50);
            m_serialCom.GetCommModemStatus(&dwState2);

            //У��modem��״̬
            if ( (dwState2 &MS_RING_ON) && !(dwState1 &MS_RING_ON)  )
            {
                //�������
                tit.bError=0;
            }
            else
            {
                tit.bError=1;
            }
        }
        break;
    }


    int nRet=pConnect->SendAPacket(m_uiCmd,0,sizeof(tit),(BYTE *)&tit );
    return nRet >0?TEST_NORMAL:TEST_CONNECTDOWN;
}