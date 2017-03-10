#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "rfidTest.h"
#include "iniFile.h"
#include "IsaMemLib.h"
#include "HPC_RFID_DLL.h"

CRfidTest::CRfidTest()
    :m_iComIndex(0)
{
    
}

CRfidTest::~CRfidTest()
{

}

BOOL CRfidTest::Init(const WCHAR *strIniFile, int index, 
                UINT16 *uipCmdIndex, CTestManager *pManager)
{
    WCHAR strSection[]=L"RFIDTest_\0\0";
    strSection[9]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"RFID",
        m_strTestName,31,strIniFile);
    m_iComIndex = GetPrivateProfileInt(strSection,L"SerialPortIndex",0,
        strIniFile);


    m_uiCmd=index+CMD_TEST_RFID;
    *uipCmdIndex= m_uiCmd;
    BOOL    bRet =LoadRfidDll();

    if (!bRet)
    {
        MessageBox(pManager->GetMainWnd(),L"¼ÓÔØRFID²Ù×÷¿âÊ§°Ü",NULL,MB_OK|MB_ICONERROR);
    }

    return bRet;
}

TEST_STATUS CRfidTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
                                        const BYTE *pData,CConnect *pConnect)
{
    DWORD   dwStatus=0;
    m_ti.bError=0;
    m_ti.uiErrorLen=0;

    if(RFID_STATUS_OK == RfidModuleOpenPort(m_iComIndex) )
    {
        RfidModulePowerOn();
        BYTE ucCardSn[16];
        if (RFID_STATUS_OK != ISO14443A_ReadCardSn(ucCardSn))
        {
            m_ti.bError=1;
            m_ti.uiErrorLen=swprintf(m_ti.strErr,L"¶ÁÈ¡ISO14443A¿¨ºÅÊ§°Ü");
        }

        RfidModulePowerOff();
        RfidModuleClosePort();
    }
    else
    {
        m_ti.bError=1;
        m_ti.uiErrorLen=swprintf(m_ti.strErr,L"´ò¿ªRFID´®¿Ú%dÊ§°Ü",m_iComIndex);
    }

    TEST_INFO_TRANSFER tit;
    TestInfoLocalToTransfer(&tit,&m_ti);
    int nRet;
    nRet =pConnect->SendAPacket(m_uiCmd,0,sizeof(tit),(BYTE *)&tit);

    return nRet >0?TEST_NORMAL:TEST_CONNECTDOWN;
}