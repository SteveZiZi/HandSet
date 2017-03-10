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
    //���һ�δ��̲���,������̲����޴��󷵻�TRUE
    //����ִ����ɺ�m_ti��ӳ�˲��Ե���Ϣ
    TEST_SITUATION      ExcuteOneDiskTest();

    //���һ�δ��̲��Ե�д����,��������޴��󷵻�TRUE��uiVerifySumWrite����д�����ݵ�У���
    //�����������FALSE��m_strLastError��ӳ�˴������Ϣ
    //strTestFile��Ҫд����ļ�
    BOOL    WriteTestFile(const WCHAR *strTestFile,UINT32 *uiVerifySumWrite);

    //���һ�δ��̲��ԵĶ�����,��������޴��󷵻�TRUE��uiVerifySumRead����д�����ݵ�У���
    //�����������FALSE��m_strLastError��ӳ�˴������Ϣ
    //strTestFile��Ҫд����ļ�
    BOOL    ReadTestFile(const WCHAR *strTestFile,UINT32 *uiVerifySumRead);



    //У������Ƿ����
    BOOL    CheckDiskAvaliable();
        
private:
    WCHAR   m_strTestFolder[32];                //���̲���Ҫ���ԵĴ��̶�Ӧ��ϵͳ�µ��ļ���
    int     m_iTestFolderLen;                   //m_strTestFolder����Ч�ַ��ĸ���   
    BOOL    m_devInitError;                     //�豸��ʼ���Ƿ����


    static const int __DISK_TEST_BUF_UINT32_NUM=1024*256;
    BYTE buf[__DISK_TEST_BUF_UINT32_NUM*4];
};


#endif