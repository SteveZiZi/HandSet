#ifndef     __KBD_IO_TEST_H__
#define     __KBD_IO_TEST_H__

//扩展键盘IO测试
class CKbdIoTest:public CTestBase
{
public:
    CKbdIoTest();
    ~CKbdIoTest();

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return m_uiShouldTestCount;};

    TEST_STATUS OnBeginTest(CConnect *pConnect);
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);

private:

    unsigned int        m_uiOutPinNum;              //输出引脚的个数
    unsigned int        m_uiInPinNum;               //输入引脚的个数
    unsigned int        m_uiShouldTestCount;        //应该测试的次数

    unsigned int        m_uiOutMask;                //用于输出的掩码
    unsigned int        m_uiInMask;                 //用于输入的掩码
    unsigned int        m_uiErrMask;                //用于表征第几个引脚出错的掩码(以输入为标准)
};


#endif