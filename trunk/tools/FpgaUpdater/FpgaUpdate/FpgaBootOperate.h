/// @file
///
/// @brief
///     对SPI Flash进行写入操作
///
/// @note
///     从相应目录中查找FPGA升级文件，经过擦除、写入、校验后完成升级
///
/// Copyright (c) 2013 IUnknownBase Inc.
///
/// 作者:
///
///     chao 2013.8.23
///
/// 修改历史:
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
    //保存从文件中读到的数据
    BYTE * m_pReadData;

    //保存从SPI Flash中读到的数据
    BYTE * m_pWriteData;

    //设备句柄
    HANDLE m_hFile;

    //文件操作
    CFile m_cFile;

    //主对话框类对象
    CFpgaUpdateDlg *m_fpgaDlg;

   
};
