#ifndef __TEST_BASE_H__
#define __TEST_BASE_H__

#include "testCommon.h"
//定义测试状态
typedef  enum  _TEST_STATUS
{
    TEST_ERROR=0,
    TEST_NORMAL,    
    TEST_CONNECTDOWN,
    TEST_ABORT,                             //测试因为某些原因而不得不中止      
}TEST_STATUS;


//定义所有测试项目的基类CTestBase，所有的测试项的代表类都必须从这个类中派生。
class CTestManager;
class CTestBase
{
public:
    CTestBase():
    m_uiTestCount(0)
    ,m_uiTestErrCount(0)
    ,m_pManager(NULL)
    ,m_uiCmd(0)
    {
        m_strTestName[0]=0;
    }
    virtual ~CTestBase()
    {
    }

    /*****************************************************************************************
    ** Function name:           Init   
    ** Descriptions:            初始化此测试项对象,这是pManager对象在new一个新的测试项对象的时候
                                必须调用这个函数来初始化测试项对象
                                这个函数并不尝试去初始化在设备上所对应的硬件
    **
    ** input parameters:        strIniFile 配置文件的全路径文件名
                                index      是第几个磁盘测试项，这里以0开始，0表示磁盘测试1
                                pManager   控制这个测试项的CTestManager对象指针                            
    ** Returned value:          失败返回FALSE，其它返回值表明初始化成功
    ******************************************************************************************/
    virtual BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager)=0;

    //在开始测试和停止测试要执行的操作
    //OnBeginTest可用于检查客户端的初始化情况
    //如果初始化出错，OnBeginTest可以弹出显示错误信息的对话框
    virtual TEST_STATUS OnBeginTest(CConnect *pConnect)=0;
    virtual TEST_STATUS OnEndTest(CConnect *pConnect)=0;
    virtual TEST_STATUS ExcuteOneTest(CConnect *pConnect)=0;   
    
    virtual const WCHAR *GetTestObjName()const{return m_strTestName;};   
    virtual unsigned int GetTestedCount()const{return m_uiTestCount;};
    virtual unsigned int GetTestedErrCount()const{return m_uiTestErrCount;} ;
    virtual void    ResetTestCount(){m_uiTestCount=0; m_uiTestErrCount=0; };

    virtual int  GetShouldTestTimes()const=0;
protected:
    unsigned int        m_uiTestCount;
    unsigned int        m_uiTestErrCount;
    WCHAR               m_strTestName[32];                  //本测试项的名称
    CTestManager        *m_pManager;                        //用于保存测试项管理器的指针
    UINT16              m_uiCmd;                            //本测试项所对应的命令码
};




#endif