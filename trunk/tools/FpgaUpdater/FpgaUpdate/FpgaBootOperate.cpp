#include "StdAfx.h"
#include "FpgaBootOperate.h"
#include "winioctl.h"
#include "resource.h"

///< ����SPI Flash��д���׵�ַ
#define IOCTL_SET_START_POS     \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0302, METHOD_BUFFERED, FILE_ANY_ACCESS)

///< ��ȡ��ǰSPI Flash���׵�ַ
#define IOCTL_GET_START_POS     \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0303, METHOD_BUFFERED, FILE_ANY_ACCESS)

///< ��Ƭ����SPI Flash
#define IOCTL_ERASE_SECTOR    \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0304, METHOD_BUFFERED, FILE_ANY_ACCESS)

///< ��������SPI Flash
#define IOCTL_BULK_ERASE        \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0305, METHOD_BUFFERED, FILE_ANY_ACCESS)

///< ��ȡSPI Flash�ṹ��Ϣ
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
///     �������ݵĳ�ʼ��
///
/// @return
///
///
void FpgaBootOperate::Init()
{
    //��SPI Flash�豸����
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
///     ����FPGA�̼���SPI Flash
/// @param[CString]
///     strFilePath - FPGA�̼���ŵ�·��
/// @return
///     ����TRUE��ʾ��дSPI Flash�ɹ�������У����ȷ������FALSE��ʾ��дʧ��
///
bool FpgaBootOperate::WriteFpgaBoot( CString strFilePath )
{
    DWORD dwDataLen = 0;
    DWORD dwByte = 0;
    DWORD dwPos = 0;
  
    //��FPGA�̼�
    if(!m_cFile.Open((LPCTSTR)strFilePath, CFile::modeRead, NULL))
    {
        return FALSE;
    }

    if(m_fpgaDlg == NULL)
    {
        return FALSE;
    }

    //���û���������������Ƶ��
    m_fpgaDlg->SetTimer(1, 200, NULL);

    m_fpgaDlg->SetProcessRange(0.01, L"���ڽ���FPGA�̼�����,���Ժ�.....1%", L"���ڶ�ȡ�ļ�......");

    if(INVALID_HANDLE_VALUE == m_hFile)
    {
        m_fpgaDlg->SetProcessRange(1.1, L"��SPI Flash����ʧ�ܣ�����ʧ��!");
        return FALSE;
    }

    //��ȡFPGA�̼��ļ���С
    dwDataLen = (DWORD)m_cFile.GetLength();

    if(dwDataLen <= 0)
    {
        m_fpgaDlg->SetProcessRange(1.1, L"�����ļ�����Ϊ0������ʧ�ܣ�");
        return FALSE;
    }

    //�ڶ��Ϸ����ڴ�
    m_pReadData = new BYTE[dwDataLen];
    m_pWriteData = new BYTE[dwDataLen];

    if(m_pReadData != NULL && m_pWriteData != NULL)
    {
        //��ȡ�ļ��еĹ̼���Ϣ
        if(m_cFile.Read(m_pReadData, dwDataLen) != dwDataLen)
        {
            m_fpgaDlg->SetProcessRange(1.1, L"��ȡ�ļ���������ʧ�ܣ�");
            return FALSE;
        }
    }
    else
    {
        m_fpgaDlg->SetProcessRange(1.1, L"�ڴ�����������ʧ�ܣ�");
        return FALSE;
    }

    //���ö�д����ʼλ��
    if(!DeviceIoControl(m_hFile, IOCTL_SET_START_POS, &dwPos, sizeof(dwPos), 
        NULL, NULL, NULL, NULL))
    {
        m_fpgaDlg->SetProcessRange(1.1, L"���ö�д����ʼλ��ʧ�ܣ�����ʧ�ܣ�");
        return FALSE;
    }

    m_fpgaDlg->SetProcessRange(0.06, L"���ڽ���FPGA�̼�����,���Ժ�.....3%", L"���ڲ���Flash......");
    
    //��������Flash
    if(!DeviceIoControl(m_hFile, IOCTL_BULK_ERASE, 
        NULL, NULL, NULL, NULL, NULL, NULL))
    {
        m_fpgaDlg->SetProcessRange(1.1, L"����Flashʧ�ܣ�����ʧ�ܣ�");
        return FALSE;
    }

    m_fpgaDlg->SetProcessRange(0.3, L"���ڽ���FPGA�̼�����,���Ժ�.....30%", L"����д��̼�......");

    //д��FPGA�̼���SPI Flash��
    if(!WriteFile(m_hFile, m_pReadData, dwDataLen, &dwByte, NULL))
    {
        m_fpgaDlg->SetProcessRange(1.1, L"д��̼�ʧ�ܣ�����ʧ�ܣ�");
        return FALSE;
    }

    //�ڶ������ö�д����ʼλ��
    if(!DeviceIoControl(m_hFile, IOCTL_SET_START_POS, &dwPos, sizeof(dwPos),
        NULL, NULL, NULL, NULL))
    {
        m_fpgaDlg->SetProcessRange(1.1, L"�ڶ������ö�д����ʼλ��ʧ�ܣ�����ʧ�ܣ�");
        return FALSE;
    }

    m_fpgaDlg->SetProcessRange(0.72, L"���ڽ���FPGA�̼�����,���Ժ�.....72%", L"���ڽ���У��......");

    //��SPI Flash�ж�ȡд��Ĺ̼���Ϣ
    if(!ReadFile(m_hFile, m_pWriteData, dwDataLen, &dwByte, NULL))
    {
        m_fpgaDlg->SetProcessRange(1.1, L"��ȡд��Ĺ̼���Ϣʧ�ܣ�����ʧ�ܣ�");
        return FALSE;
    }

    m_fpgaDlg->SetProcessRange(0.99, L"���ڽ���FPGA�̼�����,���Ժ�.....99%");
    
    //��д��SPI Flash֮ǰ��д��֮��Ĺ̼���Ϣ���жԱ�У��
    if(0 == memcmp(m_pReadData, m_pWriteData, dwDataLen))
    {
        m_fpgaDlg->SetProcessRange(1.0, L"���ڽ���FPGA�̼�����,���Ժ�.....100%", 
            L"���ڽ���У��......");
        Sleep(500);
        m_fpgaDlg->SetProcessRange(1.1, L"�����ɹ���", L"", TRUE);

        //���ý����ʾʱ��
        Sleep(1500);
    }
    else
    {
        m_fpgaDlg->SetProcessRange(1.1, L"У��̼�ʧ�ܣ�����ʧ�ܣ�");
        Sleep(1500);
        return FALSE;
    }

    return TRUE;
}



