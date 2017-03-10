#include "StdAfx.h"
#include "FpgaBootOperate.h"
#include "winioctl.h"
#include "resource.h"

///< 设置SPI Flash读写的首地址
#define IOCTL_SET_START_POS     \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0302, METHOD_BUFFERED, FILE_ANY_ACCESS)

///< 获取当前SPI Flash的首地址
#define IOCTL_GET_START_POS     \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0303, METHOD_BUFFERED, FILE_ANY_ACCESS)

///< 按片擦除SPI Flash
#define IOCTL_ERASE_SECTOR    \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0304, METHOD_BUFFERED, FILE_ANY_ACCESS)

///< 擦除整个SPI Flash
#define IOCTL_BULK_ERASE        \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0305, METHOD_BUFFERED, FILE_ANY_ACCESS)

///< 获取SPI Flash结构信息
#define IOCTL_GET_FLASH_INFO       \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0307, METHOD_BUFFERED, FILE_ANY_ACCESS)

FpgaBootOperate::FpgaBootOperate(void)
{
    Init();
}

FpgaBootOperate::FpgaBootOperate( CFpgaUpdateDlg * fgpaUpdateDlg)
{
    Init();
    m_fpgaDlg = fgpaUpdateDlg;
}

///
/// @brief
///     进行数据的初始化
///
/// @return
///
///
void FpgaBootOperate::Init()
{
    //打开SPI Flash设备驱动
    m_hFile = CreateFile(L"SPF1:", GENERIC_READ | GENERIC_WRITE, 
        0, 0, OPEN_ALWAYS, 0, 0);

    m_fpgaDlg    = NULL;
    m_pReadData  = NULL;
    m_pWriteData = NULL;
}

FpgaBootOperate::~FpgaBootOperate(void)
{
    if(m_pReadData != NULL)
    {
        delete []m_pReadData;
    }
    if(m_pWriteData != NULL)
    {
        delete []m_pWriteData;
    }
    if(m_hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hFile);
    }
}

///
/// @brief
///     下载FPGA固件到SPI Flash
/// @param[CString]
///     strFilePath - FPGA固件存放的路径
/// @return
///     返回TRUE表示烧写SPI Flash成功，并且校验正确，返回FALSE表示烧写失败
///
bool FpgaBootOperate::WriteFpgaBoot( CString strFilePath )
{
    DWORD dwDataLen = 0;
    DWORD dwByte = 0;
    DWORD dwPos = 0;
  
    //打开FPGA固件
    if(!m_cFile.Open((LPCTSTR)strFilePath, CFile::modeRead, NULL))
    {
        return FALSE;
    }

    if(m_fpgaDlg == NULL)
    {
        return FALSE;
    }

    //设置滑动进度条滚动的频率
    m_fpgaDlg->SetTimer(1, 200, NULL);

    m_fpgaDlg->SetProcessRange(0.01, L"正在进行FPGA固件升级,请稍后.....1%", L"正在读取文件......");

    if(INVALID_HANDLE_VALUE == m_hFile)
    {
        m_fpgaDlg->SetProcessRange(1.1, L"打开SPI Flash驱动失败，升级失败!");
        return FALSE;
    }

    //获取FPGA固件文件大小
    dwDataLen = (DWORD)m_cFile.GetLength();

    if(dwDataLen <= 0)
    {
        m_fpgaDlg->SetProcessRange(1.1, L"升级文件长度为0，升级失败！");
        return FALSE;
    }

    //在堆上分配内存
    m_pReadData = new BYTE[dwDataLen];
    m_pWriteData = new BYTE[dwDataLen];

    if(m_pReadData != NULL && m_pWriteData != NULL)
    {
        //读取文件中的固件信息
        if(m_cFile.Read(m_pReadData, dwDataLen) != dwDataLen)
        {
            m_fpgaDlg->SetProcessRange(1.1, L"读取文件错误，升级失败！");
            return FALSE;
        }
    }
    else
    {
        m_fpgaDlg->SetProcessRange(1.1, L"内存分配错误，升级失败！");
        return FALSE;
    }

    //设置读写的起始位置
    if(!DeviceIoControl(m_hFile, IOCTL_SET_START_POS, &dwPos, sizeof(dwPos), 
        NULL, NULL, NULL, NULL))
    {
        m_fpgaDlg->SetProcessRange(1.1, L"设置读写的起始位置失败，升级失败！");
        return FALSE;
    }

    m_fpgaDlg->SetProcessRange(0.06, L"正在进行FPGA固件升级,请稍后.....3%", L"正在擦除Flash......");
    
    //擦除整个Flash
    if(!DeviceIoControl(m_hFile, IOCTL_BULK_ERASE, 
        NULL, NULL, NULL, NULL, NULL, NULL))
    {
        m_fpgaDlg->SetProcessRange(1.1, L"擦除Flash失败，升级失败！");
        return FALSE;
    }

    m_fpgaDlg->SetProcessRange(0.3, L"正在进行FPGA固件升级,请稍后.....30%", L"正在写入固件......");

    //写入FPGA固件到SPI Flash中
    if(!WriteFile(m_hFile, m_pReadData, dwDataLen, &dwByte, NULL))
    {
        m_fpgaDlg->SetProcessRange(1.1, L"写入固件失败，升级失败！");
        return FALSE;
    }

    //第二次设置读写的起始位置
    if(!DeviceIoControl(m_hFile, IOCTL_SET_START_POS, &dwPos, sizeof(dwPos),
        NULL, NULL, NULL, NULL))
    {
        m_fpgaDlg->SetProcessRange(1.1, L"第二次设置读写的起始位置失败，升级失败！");
        return FALSE;
    }

    m_fpgaDlg->SetProcessRange(0.72, L"正在进行FPGA固件升级,请稍后.....72%", L"正在进行校验......");

    //从SPI Flash中读取写入的固件信息
    if(!ReadFile(m_hFile, m_pWriteData, dwDataLen, &dwByte, NULL))
    {
        m_fpgaDlg->SetProcessRange(1.1, L"读取写入的固件信息失败，升级失败！");
        return FALSE;
    }

    m_fpgaDlg->SetProcessRange(0.99, L"正在进行FPGA固件升级,请稍后.....99%");
    
    //把写入SPI Flash之前和写入之后的固件信息进行对比校验
    if(0 == memcmp(m_pReadData, m_pWriteData, dwDataLen))
    {
        m_fpgaDlg->SetProcessRange(1.0, L"正在进行FPGA固件升级,请稍后.....100%", 
            L"正在进行校验......");
        Sleep(500);
        m_fpgaDlg->SetProcessRange(1.1, L"升级成功！", L"", TRUE);

        //设置结果显示时间
        Sleep(1500);
    }
    else
    {
        m_fpgaDlg->SetProcessRange(1.1, L"校验固件失败，升级失败！");
        Sleep(1500);
        return FALSE;
    }

    return TRUE;
}



