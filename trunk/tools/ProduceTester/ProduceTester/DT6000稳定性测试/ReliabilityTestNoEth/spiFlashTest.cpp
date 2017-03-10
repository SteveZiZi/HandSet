#include "stdafx.h"
#include "testBase.h"
#include "spiFlashTest.h"
#include "iniFile.h"
#include "testManager.h"
#include "winioctl.h"
#include "UsuallyCommon.h"

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
    m_hSpiFlashFile = INVALID_HANDLE_VALUE;
    wcscpy(m_reportNape[0].strNape,L"测试次数");
    wcscpy(m_reportNape[1].strNape,L"错误次数");
    wcscpy(m_reportNape[2].strNape,L"正确率");
    wcscpy(m_reportNape[3].strNape,L"持续时间"); 
}

CSpiFlashTest::~CSpiFlashTest()
{

}

BOOL CSpiFlashTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"SPIFlashTest_\0\0";
    strSection[13]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"SPI Flash",
        m_strTestName,31,strIniFile);
    m_pManager = pManager;


    m_hSpiFlashFile=CreateFile(L"SPF1:", GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
    if (m_hSpiFlashFile == INVALID_HANDLE_VALUE) 
    {
        RETAILMSG(1, (_T("Open SPI Flash Driver failed\r\n")));
        return FALSE;
    }

    //获取全局配置文件的路径
    CString   strBootFilePath;

    GetExeDirectory(strBootFilePath.GetBuffer(MAX_PATH*2),MAX_PATH*2);
    strBootFilePath.ReleaseBuffer();
    strBootFilePath+=_T("fpgaBoot");
    CFile file(strBootFilePath, CFile::modeRead);
    if (file.GetLength() != SPI_FLASH_SIZE) {
        RETAILMSG(1, (_T("Not Boot File or the length of Boot file is not 4M\r\n")));
        return FALSE;
    }
    file.Read(m_uBuf1, SPI_FLASH_TEST_SIZE);

    file.Close();

    return TRUE;
}

const SReportNape *CSpiFlashTest::GetReportNapes(int &count)
{
    count =4;
    unsigned int h,m,s;
    UINT64 t=m_uiTestTime;

    swprintf(m_reportNape[0].strNapeDescriptor,L"%d次",m_uiTestCount);
    m_reportNape[0].bHighLight=FALSE;

    swprintf(m_reportNape[1].strNapeDescriptor,L"%d次",m_uiErrCount);    
    if (m_uiTestCount >0)
    {
        swprintf(m_reportNape[2].strNapeDescriptor,L"%.2f%% ",
            100 *(double)(m_uiTestCount-m_uiErrCount)/m_uiTestCount );
    }
    else
    {
        swprintf(m_reportNape[2].strNapeDescriptor,L"0.0%% ");
    }

    if (m_uiErrCount >0)
    {
        m_reportNape[1].bHighLight=TRUE;
        m_reportNape[2].bHighLight=TRUE;
    }
    else
    {
        m_reportNape[1].bHighLight=FALSE;
        m_reportNape[2].bHighLight=FALSE;
    }  

    h= (unsigned int)( t/(1000*3600));
    t=t%(1000*3600);
    m=(unsigned int) (t/(1000 *60));
    t=t%(1000 *60);
    s=(unsigned int)(t/1000);

    swprintf(m_reportNape[3].strNapeDescriptor,L"%d时%d分%d秒",h,m,s);
    m_reportNape[3].bHighLight=FALSE;

    return m_reportNape;
}

TEST_SITUATION CSpiFlashTest::ExcuteOneTest()
{
    DWORD   dwStatus=0;
    BOOL    bError=FALSE;

    m_uiTestCount++;
    DWORD       dwPos =0;
    DWORD dwIo =0;
    
        // 读取文件成功，写文件到SPI Flash

    if (DeviceIoControl(m_hSpiFlashFile, IOCTL_SET_START_POS,
        &dwPos, sizeof(dwPos), NULL, NULL, NULL, NULL)) 
    {
        if (ReadFile(m_hSpiFlashFile,m_uBuf2,SPI_FLASH_SIZE,&dwIo,NULL))
        {
            if (memcmp(m_uBuf2,m_uBuf1,SPI_FLASH_SIZE) == 0)
            {
                return SITUATION_NORMAL;
            }
            else
            {
                m_pManager->WriteLog(this,L" 读写数据校验失败");
            }
        }
        else
        {
            m_pManager->WriteLog(this,L"Read SPI Flash failed");
        }
    }
    else
    {
        m_pManager->WriteLog(this,L"Set Start Pos failed");
    }

    m_uiErrCount++;
    return SITUATION_ERROR;
}