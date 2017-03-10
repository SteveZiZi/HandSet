/// @file
///
/// @brief
///     ��SPI Flash����д�����
///
/// @note
///     ����ӦĿ¼�в���FPGA�����ļ�������������д�롢У����������
///
/// Copyright (c) 2013 IUnknownBase Inc.
///
/// ����:
///
///     chao 2013.8.23
///
/// �޸���ʷ:
///
#pragma once

#include "FpgaUpdateDlg.h"

class FpgaBootOperate
{
public:
    FpgaBootOperate(void);
    FpgaBootOperate(CFpgaUpdateDlg *);
    void Init(); 
    bool WriteFpgaBoot(CString strFilePath);
    ~FpgaBootOperate(void);
private:
    //������ļ��ж���������
    BYTE * m_pReadData;

    //�����SPI Flash�ж���������
    BYTE * m_pWriteData;

    //�豸���
    HANDLE m_hFile;

    //�ļ�����
    CFile m_cFile;

    //���Ի��������
    CFpgaUpdateDlg *m_fpgaDlg;

   
};
