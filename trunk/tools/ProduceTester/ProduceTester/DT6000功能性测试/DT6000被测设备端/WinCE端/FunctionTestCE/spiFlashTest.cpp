#include "stdafx.h"
#include "testCommon.h"
#include "connect.h"
#include "testBase.h"
#include "spiFlashTest.h"
#include "iniFile.h"
#include "assertMsg.h"
#include "winioctl.h"

#define IOCTL_SET_START_POS     \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0302, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_GET_START_POS     \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0303, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_ERASE_SECTOR    \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0304, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_BULK_ERASE        \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0305, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SETCFG        \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0306, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_GET_FLASH_INFO       \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0307, METHOD_BUFFERED, FILE_ANY_ACCESS)

CSpiFlashTest::CSpiFlashTest()
{

}

CSpiFlashTest::~CSpiFlashTest()
{

}

BOOL CSpiFlashTest::Init(const WCHAR *strIniFile, int index, UINT16 *uipCmdIndex, CTestManager *pManager)
{
    WCHAR strSection[]=L"SPIFlashTest_\0\0";
    strSection[8]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"SPI Flash",
        m_strTestName,31,strIniFile);
    m_uiCmd=index+CMD_TEST_SPI_FLASH;
    *uipCmdIndex= m_uiCmd;
    return TRUE;
}

TEST_STATUS CSpiFlashTest::ProcessExcuteCmd(UINT16 param,
                                            UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect)
{
    DWORD   dwStatus=0;
    BOOL    bError=FALSE;

    HANDLE  hSpiFlashFile = INVALID_HANDLE_VALUE;

    m_ti.bError=1;
    m_ti.uiErrorLen=0;

    DWORD       dwPos =0;
    hSpiFlashFile=CreateFile(L"SPF1:", GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
    if (hSpiFlashFile != INVALID_HANDLE_VALUE) 
    {
        // 读取文件成功，写文件到SPI Flash
     
        if (DeviceIoControl(hSpiFlashFile, IOCTL_SET_START_POS,
            &dwPos, sizeof(dwPos), NULL, NULL, NULL, NULL)) 
        {
            if (DeviceIoControl(hSpiFlashFile, IOCTL_BULK_ERASE, NULL, NULL, NULL, NULL, NULL, NULL)) 
            {
                BYTE    buf1[512],buf2[512];
                DWORD   dwIo;

                for (int i=0;i<sizeof(buf1);i++)
                {
                    buf1[i]=rand();
                }

                if (WriteFile(hSpiFlashFile, buf1, sizeof(buf1),&dwIo , NULL)) 
                {
                    dwPos =0;
                    DeviceIoControl(hSpiFlashFile, IOCTL_SET_START_POS,
                        &dwPos, sizeof(dwPos), NULL, NULL, NULL, NULL);
                    if (ReadFile(hSpiFlashFile,buf2,sizeof(buf1),&dwIo,NULL))
                    {
                        if (memcmp(buf1,buf2,sizeof(buf1)) == 0)
                        {
                            m_ti.bError =0;
                        }
                        else
                        {
                            m_ti.uiErrorLen=swprintf(m_ti.strErr,L" 读写数据校验失败");
                        }
                    }
                    else
                    {
                        m_ti.uiErrorLen=swprintf(m_ti.strErr,L" Read SPI Flash failed");
                    }

                }
                else
                {
                    m_ti.uiErrorLen=swprintf(m_ti.strErr,L" Write SPI Flash failed");
                }

            }
            else
            {
                m_ti.uiErrorLen=swprintf(m_ti.strErr,L"Erase Flash failed");
            }
        }
        else
        {
             m_ti.uiErrorLen=swprintf(m_ti.strErr,L"Set Start Pos failed");
        }

    }
    else
    {
        m_ti.uiErrorLen=swprintf(m_ti.strErr,L"Open SPI Flash Driver failed");
    }


    TEST_INFO_TRANSFER tit;
    TestInfoLocalToTransfer(&tit,&m_ti);
    int nRet;
    nRet =pConnect->SendAPacket(m_uiCmd,0,sizeof(tit),(BYTE *)&tit);

    return nRet >0?TEST_NORMAL:TEST_CONNECTDOWN;
}



