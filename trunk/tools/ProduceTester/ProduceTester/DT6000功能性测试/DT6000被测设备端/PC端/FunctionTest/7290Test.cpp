#include <windows.h>
#include <atlstr.h>
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "7290Test.h"
#include "assertMsg.h"

BOOL C7290Test::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"7290Test_\0\0";
    strSection[9]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"ADC",m_strTestName,31,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_7290;
    //保存CTestManager对象指针
    assertMsg(pManager,L"pManager指针不能为空!");
    m_pManager=pManager;
    return TRUE;
}

#define  PARAM_7290_START       0
#define  PARAM_7290_END         1


TEST_STATUS C7290Test::ExcuteOneTest(CConnect *pConnect)
{
    PACKET          pa;
    int             nRet;
    pa.uiCmd=m_uiCmd;
    pa.param=PARAM_7290_START;

    nRet =pConnect->SendAPacket(&pa);
    nRet =pConnect->RecvAPacket(&pa,5000);
    if (nRet >0)
    {
        m_uiTestCount++;
        m_pManager->MessageBox(L"请逐个按数字小键盘上的按键，并观察旁边数码管的显示,"
            L"等全部案件都测试完成后再单击\"确定\" 按钮",
            L"测试",MB_OK);
        pa.uiCmd=m_uiCmd;
        pa.param=PARAM_7290_END;

        nRet =pConnect->SendAPacket(&pa);
        nRet =pConnect->RecvAPacket(&pa,5000);

        if (nRet >0)
        {
            nRet=m_pManager->MessageBox(L"数码管的显示和键盘的按键是否一一对应?",
                L"测试",MB_YESNO|MB_ICONQUESTION);

            if (nRet !=IDYES)
            {
                m_uiTestErrCount++;
                return TEST_ERROR;
            }
            return TEST_NORMAL;                
        }            
    }

    return TEST_CONNECTDOWN;
}