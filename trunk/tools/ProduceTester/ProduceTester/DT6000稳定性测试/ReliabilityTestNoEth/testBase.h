#ifndef __TEST_BASE_H__
#define __TEST_BASE_H__

//定义所有测试项目的基类CTestBase

//定义测试情况
typedef enum _TEST_SITUATION
{
    SITUATION_ERROR=0,                  //测试项 测试错误
    SITUATION_NORMAL,                   //测试项 测试正常
    SITUATION_DEAD,                     //测试项 测试的时候 执行测试的函数或线程死掉了，没有响应
    
}TEST_SITUATION;


//定义测试项的测试状态
typedef enum _TEST_STATUS
{
    STATUS_WAITING=0,                   //待测试
    STATUS_TESTING,                     //正在测试
    STATUS_STOP,                        //已停止，
    STATUS_BE_STOPPED,                  //因为某些原因被停止
    STATUS_DEAD,                        //测试项已死
    
}TEST_STATUS;

//定义测试报告的表格的一个行，也就是一个子记录项
struct SReportNape
{
    WCHAR strNape[32];                  //子记录项的名称
    WCHAR strNapeDescriptor[128];       //子记录项的描述
    BOOL  bHighLight;                   //子记录项是否高亮显示
};

class CTestManager;

class CTestBase
{
public:
    CTestBase():
      m_uiTestCount(0),
      m_uiErrCount(0),
      m_uiTestTime(0),
      m_testStatus(STATUS_WAITING),
      m_pManager(NULL)
    {
        m_strTestName[0]=0;
        m_strLastError[0]=0;
    }

    virtual ~CTestBase()    {};
    virtual BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager)=0;     
    virtual TEST_SITUATION ExcuteOneTest()=0; 

    virtual const WCHAR *GetTestObjName()const  {return m_strTestName;};   
    virtual unsigned int GetTestCount()const    {return m_uiTestCount;};
    virtual unsigned int GetErrCount()const     {return m_uiErrCount; };
    virtual const WCHAR *GetLastErrorMsg()const {return m_strLastError;};  


    

    //获取或更改测试状态
    const TEST_STATUS GetTestSTatus()const      {return m_testStatus;};
    void SetTestStatus(TEST_STATUS status)      {m_testStatus=status;};

    //添加测试时间，一个测试项的每次轮换的测试时间由测试管理器来统计，
    //并调用AddTestTime将时间累加到每个测试项的测试时间总和中
    void AddTestTime(unsigned int uiTestTime)   {m_uiTestTime+=uiTestTime;};
    //获取测试报告关于这个测试项的相关记录
    virtual const SReportNape *GetReportNapes(int &count)=0;

protected:
    unsigned int    m_uiTestCount;                      //已经测试的次数
    unsigned int    m_uiErrCount;                       //出错的次数
    WCHAR           m_strTestName[32];                  //本测试项的名称
    WCHAR           m_strLastError[256];                //关于最后一次出错的文本描述    
    //以下变量用于生成测试报告所需要的项
    SReportNape     m_reportNape[8];
    UINT64          m_uiTestTime;                       //本测试项一共测试的时间,ms为单位
    //m_testStatus用于显示此测试项的测试状态，测试状态仅仅是为了让外界读取状态信息
    //即使是测试管理器也不会因为他的值来决定是否进行这个测试项的测试
    TEST_STATUS     m_testStatus;    

     CTestManager    *m_pManager;
};  




#endif