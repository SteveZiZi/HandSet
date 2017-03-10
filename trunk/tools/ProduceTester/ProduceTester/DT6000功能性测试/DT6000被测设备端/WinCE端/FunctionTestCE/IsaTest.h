#ifndef     __ISA_TEST_H__
#define     __ISA_TEST_H__

//ISA����
class CIsaTest:public CTestBase
{
public:

    CIsaTest();
    ~CIsaTest();

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,
        const BYTE *pData,CConnect *pConnect);
private:

    HANDLE      m_hIntEvent;                    //��ISA�жϹ������¼�����
    BYTE        *m_pbufRead;                    //�ڴ���Զ�������
    BYTE        *m_pBufWirte;                   //�ڴ����д������

    TEST_INFO   m_ti;

    BOOL    ExcuteOneMem16Test(UINT32 uiAddr,UINT32 uiLen);         //ִ��һ��16λ�ڴ����
    BOOL    ExcuteOneMem8Test(UINT32  uiAddr,UINT32 uiLen);         //ִ��һ��8λ�ڴ����
    BOOL    ExcuteOneIntTest(UINT32  uiIntIndex);         //ִ��һ���жϲ���


//     TEST_INFO_TRANSFER  m_tit;
};



#endif