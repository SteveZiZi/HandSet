#ifndef __DISK_TEST_H__
#define __DISK_TEST_H__

//定义磁盘测试参数所代表的含义

#define     DISK_TEST_CMD_WRITE             0
#define     DISK_TEST_CMD_READ              1
#define     DISK_TEST_CMD_DELETE            2

class CDiskTest:public CTestBase
{
public:
    CDiskTest();
    ~CDiskTest();

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect);

private:
    //定义所有磁盘测试共享的常量

    //缓冲区的大小除以4
    static const int __DISK_TEST_BUF_UINT32_NUM=1024;
    //每次磁盘测试文件大小相当于几个缓冲区
    static const int __DISK_TEST_BUF_NUM=256;

    //以下变量是各个测试独立的    
    WCHAR           m_strTestFolder[32];        //磁盘测试欲测试的文件夹
    int             m_iTestFolderLen;           //m_strTestFolder文件夹所含字符串的长度
    int             m_iDiskType;                //表征磁盘类型的索引
    WCHAR           m_strStoreName[64];         //存储的类名

    UINT            m_uiTestNum;                //本测试项已经测试的次数,这个次数仅作内部使用    
    TEST_INFO       m_ti;                       //用于描述当前的测试信息
    BYTE            buf[__DISK_TEST_BUF_UINT32_NUM*4*__DISK_TEST_BUF_NUM];  //用作读写文件
   

    //完成一次磁盘测试的写操作,如果磁盘测试无错误返回TRUE，uiVerifySumWrite返回写入数据的校验和
    //如果函数返回FALSE，m_ti反映了测试的信息
    //调用这个函数前m_strTestFolder内容必须已变成要写入的文件全路径的文件名
    BOOL    WriteTestFile(UINT32 *uiVerifySumWrite);

    //完成一次磁盘测试的读操作,如果磁盘测试无错误返回TRUE，uiVerifySumWrite返回文件数据的校验和
    //如果函数返回FALSE，m_ti反映了测试的信息
    //调用这个函数前m_strTestFolder内容必须已变成要写入的文件全路径的文件名
    BOOL    ReadTestFile(UINT32 *uiVerifySumRead);

    //删除所有测试文件
    BOOL    DeleteTestFiles();

    //初始化磁盘测试,执行完成以后m_ti反应了初始化信息
    BOOL    InitDiskTest();

};


#endif