#pragma once
#include ".\app_src\fiber.h"
#define  FIBER_NUM  2

//extern     int     g_iFlag;
class CSmvTest:public CTestBase
{
public:
    CSmvTest(void);
    ~CSmvTest(void);
    CSmvTest(HANDLE hDevice);
    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest();
    virtual TEST_SITUATION BeginOneTest();
    virtual TEST_SITUATION EndOneTest();
    const SReportNape *GetReportNapes(int &count);
   

private:
    _SMV_92_c * m_pFiber[FIBER_NUM];

    //接收的总报文帧数
    UINT32      m_uTotalFrame[FIBER_NUM];

    //错误的报文帧数
    UINT32      m_uErrorFrame[FIBER_NUM];

    IOCTRL_FIBER_IN_PARAM_t        m_tFiberParam;

    //当前发送的光网口
    int     m_iFlag;

    //文件路径
    WCHAR       m_strSmvFilePath[MAX_PATH*2];

    int         m_iTimeOut;
};
