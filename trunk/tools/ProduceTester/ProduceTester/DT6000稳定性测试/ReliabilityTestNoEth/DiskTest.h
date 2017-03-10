#ifndef __DISK_TEST_H__
#define __DISK_TEST_H__

class CDiskTest:public CTestBase
{
public:
    CDiskTest();
    ~CDiskTest();

    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest(){return ExcuteOneDiskTest();};
    const SReportNape *GetReportNapes(int &count);
    

public:
    //完成一次磁盘测试,如果磁盘测试无错误返回TRUE
    //函数执行完成后m_ti反映了测试的信息
    TEST_SITUATION      ExcuteOneDiskTest();

    //完成一次磁盘测试的写操作,如果操作无错误返回TRUE，uiVerifySumWrite返回写入数据的校验和
    //如果函数返回FALSE，m_strLastError反映了错误的信息
    //strTestFile是要写入的文件
    BOOL    WriteTestFile(const WCHAR *strTestFile,UINT32 *uiVerifySumWrite);

    //完成一次磁盘测试的读操作,如果操作无错误返回TRUE，uiVerifySumRead返回写入数据的校验和
    //如果函数返回FALSE，m_strLastError反映了错误的信息
    //strTestFile是要写入的文件
    BOOL    ReadTestFile(const WCHAR *strTestFile,UINT32 *uiVerifySumRead);



    //校验磁盘是否可用
    BOOL    CheckDiskAvaliable();
        
private:
    WCHAR   m_strTestFolder[32];                //磁盘测试要测试的磁盘对应的系统下的文件夹
    int     m_iTestFolderLen;                   //m_strTestFolder中有效字符的个数   
    BOOL    m_devInitError;                     //设备初始化是否出错


    static const int __DISK_TEST_BUF_UINT32_NUM=1024*256;
    BYTE buf[__DISK_TEST_BUF_UINT32_NUM*4];
};


#endif