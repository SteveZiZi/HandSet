#ifndef     __ISA_TEST_H__
#define     __ISA_TEST_H__

//ISA测试
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

    HANDLE      m_hIntEvent;                    //与ISA中断关联的事件对象
    BYTE        *m_pbufRead;                    //内存测试读缓冲区
    BYTE        *m_pBufWirte;                   //内存测试写缓冲区

    TEST_INFO   m_ti;

    BOOL    ExcuteOneMem16Test(UINT32 uiAddr,UINT32 uiLen);         //执行一次16位内存测试
    BOOL    ExcuteOneMem8Test(UINT32  uiAddr,UINT32 uiLen);         //执行一次8位内存测试
    BOOL    ExcuteOneIntTest(UINT32  uiIntIndex);         //执行一次中断测试


//     TEST_INFO_TRANSFER  m_tit;
};



#endif