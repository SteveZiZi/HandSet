#ifndef      __ZIG_BEE_TEST__
#define     __ZIG_BEE_TEST__

class CZigBeeTest : public CTestBase
{
public:
    CZigBeeTest();
    ~CZigBeeTest();

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,
        const BYTE *pData,CConnect *pConnect);
private:
    int                m_iComIndex;                //ZigBeeģ����ʹ�õĴ��ڱ��
    TEST_INFO          m_ti;

};


#endif