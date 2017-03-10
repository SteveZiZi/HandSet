#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "gprsTest.h"
#include "assertMsg.h"
#include "iniFile.h"

BOOL CGPRSTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{

    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"GPRSTest_\0\0";
    strSection[9]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"GPRS",m_strTestName,31,strIniFile);
    m_iComIndex=GetPrivateProfileInt(strSection,L"SerialPortIndex",0,strIniFile)+1;
    m_iBraudRate = GetPrivateProfileInt(strSection,L"BraudRate ",9600,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_GPRS;
    *uipCmdIndex=m_uiCmd;

    return TRUE;
}

TEST_STATUS CGPRSTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect)
{
    m_ti.bError=0;
    if ( !m_gprs.GPRS_Init(m_iComIndex,m_iBraudRate,NULL))
    {
        m_ti.bError=1;
        m_ti.uiErrorLen=swprintf(m_ti.strErr,
            L"��ʼ��GPRSģ��ʧ��");
        goto END;
    }
    //�򿪴���
//     m_xCom.CloseCom();
// 
//     if (!m_xCom.Open(m_iComIndex+1,CBR_9600) )
//     {
//         m_ti.bError=1;
//         m_ti.uiErrorLen=swprintf(m_ti.strErr,
//             L"��gprs����%dʧ��",m_iComIndex+1);
//         goto END;
//     }
// 
//     //��������
//     size_t s=0;
//     m_xCom.WriteNByte("1234567",7,&s);
// 
//     //�ȴ���������
//     s=0;
//     BYTE *buf[8];
//     m_xCom.ReadNByteTimeOut(buf,7,5000,&s);
// 
//     if (s <=0)
//     {
//         m_ti.bError=1;
//         m_ti.uiErrorLen=swprintf(m_ti.strErr,
//             L"δ�յ�GPRSģ��Ļ�Ӧ����");
//     }
// 
//     m_xCom.CloseCom();

END:
    m_gprs.GPRS_Deinit();
    TEST_INFO_TRANSFER tit;

    TestInfoLocalToTransfer(&tit,&m_ti);

    int nRet=pConnect->SendAPacket(m_uiCmd,param,sizeof(tit),
        (BYTE *)&tit );
    return nRet >0? TEST_NORMAL:TEST_CONNECTDOWN;    
}

