#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "ZigBeeTest.h"
#include "assertMsg.h"
#include "iniFile.h"
#include "xComCE.h"

CZigBeeTest::CZigBeeTest()
{

}
CZigBeeTest::~CZigBeeTest()
{

}

BOOL CZigBeeTest::Init(const WCHAR *strIniFile, int index,
                       UINT16 *uipCmdIndex,CTestManager *pManager)
{
    WCHAR strSection[]=L"ZigBeeTest_\0\0";
    strSection[11]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"ZigBee",
        m_strTestName,31,strIniFile);

    m_iComIndex = GetPrivateProfileInt(strSection,L"SerialPortIndex",0,
        strIniFile)+1;

    m_uiCmd=index+CMD_TEST_ZIGBEE;
    *uipCmdIndex = m_uiCmd;
    return TRUE;
}

TEST_STATUS CZigBeeTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
                                        const BYTE *pData,CConnect *pConnect)
{
    DWORD   dwStatus=0;
    m_ti.bError=0;
    m_ti.uiErrorLen=0;

    CxComCE         comCe;
    if (comCe.Open(m_iComIndex))
    {
        BYTE SendBuf[] = {0xAB, 0xBC, 0xCD, 0xD1, 0x05};
        size_t RcvLen =0;
        BYTE RcvBuf[255];
        CString ss;

        comCe.ClearRecv();
        comCe.WriteNByte(SendBuf,5);

        if (comCe.ReadTimeOut(RcvBuf,255,1000,&RcvLen))
        {
            if(RcvLen !=0x4a)
            {
                m_ti.bError=1;
                m_ti.uiErrorLen=swprintf(m_ti.strErr,L"ZigBee响应数据不正确");
            }
        }
        else
        {
            m_ti.bError=1;
            m_ti.uiErrorLen=swprintf(m_ti.strErr,L"串口%d接收失败",m_iComIndex);
        }
    }
    else
    {
        m_ti.bError=1;
        m_ti.uiErrorLen=swprintf(m_ti.strErr,L"打开ZigBee串口%d失败",m_iComIndex);
    }
  
    TEST_INFO_TRANSFER tit;
    TestInfoLocalToTransfer(&tit,&m_ti);
    int nRet;
    nRet =pConnect->SendAPacket(m_uiCmd,0,sizeof(tit),(BYTE *)&tit);

    return nRet >0?TEST_NORMAL:TEST_CONNECTDOWN;
}