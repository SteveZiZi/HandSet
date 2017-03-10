#ifndef __DISK_TEST_H__
#define __DISK_TEST_H__

//������̲��Բ���������ĺ���

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
    //�������д��̲��Թ���ĳ���

    //�������Ĵ�С����4
    static const int __DISK_TEST_BUF_UINT32_NUM=1024;
    //ÿ�δ��̲����ļ���С�൱�ڼ���������
    static const int __DISK_TEST_BUF_NUM=256;

    //���±����Ǹ������Զ�����    
    WCHAR           m_strTestFolder[32];        //���̲��������Ե��ļ���
    int             m_iTestFolderLen;           //m_strTestFolder�ļ��������ַ����ĳ���
    int             m_iDiskType;                //�����������͵�����
    WCHAR           m_strStoreName[64];         //�洢������

    UINT            m_uiTestNum;                //���������Ѿ����ԵĴ���,������������ڲ�ʹ��    
    TEST_INFO       m_ti;                       //����������ǰ�Ĳ�����Ϣ
    BYTE            buf[__DISK_TEST_BUF_UINT32_NUM*4*__DISK_TEST_BUF_NUM];  //������д�ļ�
   

    //���һ�δ��̲��Ե�д����,������̲����޴��󷵻�TRUE��uiVerifySumWrite����д�����ݵ�У���
    //�����������FALSE��m_ti��ӳ�˲��Ե���Ϣ
    //�����������ǰm_strTestFolder���ݱ����ѱ��Ҫд����ļ�ȫ·�����ļ���
    BOOL    WriteTestFile(UINT32 *uiVerifySumWrite);

    //���һ�δ��̲��ԵĶ�����,������̲����޴��󷵻�TRUE��uiVerifySumWrite�����ļ����ݵ�У���
    //�����������FALSE��m_ti��ӳ�˲��Ե���Ϣ
    //�����������ǰm_strTestFolder���ݱ����ѱ��Ҫд����ļ�ȫ·�����ļ���
    BOOL    ReadTestFile(UINT32 *uiVerifySumRead);

    //ɾ�����в����ļ�
    BOOL    DeleteTestFiles();

    //��ʼ�����̲���,ִ������Ժ�m_ti��Ӧ�˳�ʼ����Ϣ
    BOOL    InitDiskTest();

};


#endif