#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "GpioTest.h"
#include "assertMsg.h"
//虽然这个类名为GPIO，但是实现了类GPIO功能，
//并且实现了类中所规定的操作函数，这个类测试项均可代表
CGpioTest::CGpioTest():
m_uiOutPinNum(8),
m_uiInPinNum(8),
m_uiShouldTestCount(16)
{

}

CGpioTest::~CGpioTest()
{

}

BOOL CGpioTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"GPIOTest_\0\0";
    strSection[9]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"GPIO",m_strTestName,31,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_GPIO;
    //保存CTestManager对象指针
    assertMsg(pManager,L"pManager指针不能为空!");
    m_pManager=pManager;

    m_uiInPinNum=GetPrivateProfileInt(strSection,L"InPinNum",8,strIniFile);
    m_uiOutPinNum=GetPrivateProfileInt(strSection,L"OutPinNum",8,strIniFile);

    if (m_uiOutPinNum >m_uiInPinNum)
    {
        CString str;
        str.Format(L"%d,%d",m_uiInPinNum,m_uiOutPinNum);
        AfxMessageBox(str);
        pManager->MessageBox(L"这里实现的测试方法，输出引脚不能比输出引脚多",
            L"IO测试",MB_OK|MB_ICONERROR);
        return FALSE;
    }
    m_uiShouldTestCount=m_uiOutPinNum*2;

    return TRUE;
}

TEST_STATUS CGpioTest::ExcuteOneTest(CConnect *pConnect)
{
    UINT32  uiOut;
    UINT32  uiIn;

    DWORD   dwTestPin=m_uiTestCount/2;                  //当前测试的输出引脚
    BOOL    bTestHigh=m_uiTestCount%2;                  //当前测试引脚输出是否为高

    if (bTestHigh)
    {
        uiOut=1<<dwTestPin;
    }
    else
    {
        uiOut=1<<dwTestPin;
        uiOut=~uiOut;
    }
    uiOut &=m_uiOutMask;

    PACKET      pa;
    int         nRet;
    nRet=pConnect->SendAPacket(CMD_TEST_GPIO,0,sizeof(UINT32),(BYTE *)&uiOut);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }
    
    nRet=pConnect->RecvAPacket(&pa,10000);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }
    //对比输出和读入
    uiIn=*(UINT32 *)pa.pData;
    uiIn&=m_uiInMask;
    
    m_uiTestCount++;
    
    //对比输入和输出，每次对比m_uiOutPinNum;
    unsigned int     index=0;
    BOOL             bError=FALSE;
    for (index =0;index <m_uiInPinNum;index+=m_uiOutPinNum)
    {
        unsigned int uiComparePinNum=m_uiInPinNum -index;
        uiComparePinNum=uiComparePinNum>m_uiOutPinNum?m_uiOutPinNum:uiComparePinNum;

        for (unsigned int j=0;j<uiComparePinNum;j++)
        {
            UINT32 uiPinOutValue,uiPinInValue;
            uiPinOutValue=(uiOut &  (1<<j))?1:0;
            uiPinInValue=(uiIn &(1<<(j+index))  )?1:0;

            if (uiPinInValue !=uiPinOutValue)
            {
                //检测到错误引脚                
                m_uiErrMask|=1<<(index +j);
                bError=TRUE;
            }
        }
    }

    if (bError)
    {
        m_uiTestErrCount++;
        //记录错误日志
        WCHAR   strLog[512];
        swprintf(strLog,L"输出0x%02x,读入0x%02x",uiOut,uiIn);

        m_pManager->WriteLog(this,strLog);
        return TEST_ERROR;
    }

    return TEST_NORMAL;
}

TEST_STATUS CGpioTest::OnBeginTest(CConnect *pConnect)
{
    //计算出用于输入和输出的掩码
    m_uiInMask=~0;
    m_uiInMask  =m_uiInMask <<m_uiInPinNum;
    m_uiInMask  =~m_uiInMask;

    m_uiOutMask=~0;
    m_uiOutMask=m_uiOutMask <<m_uiOutPinNum;
    m_uiOutMask =~m_uiOutMask;
    //初始化错误掩码
    m_uiErrMask=0;
    m_pManager->MessageBox(L"请注意将自制GPIO测试线接到工控主板上"
        ,L"IO测试",MB_ICONINFORMATION|MB_OK);
    
    return TEST_NORMAL;
}

TEST_STATUS CGpioTest::OnEndTest(CConnect *pConnect)
{
    //如果测试过程发生了错误，则显示错误信息
    if (m_uiErrMask !=0)
    {
        CString  strErr=_T("第");
        CString  str;

        unsigned int j=0;

        //扫描直到第一个出错的位
        for (;j<m_uiInPinNum;j++)
        {
            if ( m_uiErrMask &(1<<j ) )
            {       
                break;
            }
        }

        str.Format(_T("%d"),j);
        j++;
        strErr+=str;

        for (;j<m_uiInPinNum;j++)
        {
            if ( m_uiErrMask &(1<<j ) )
            {       
                str.Format(_T(",%d"),j);
                strErr+=str;
            }
        }
        strErr+=_T("位电平错误");
        m_pManager->MessageBox(strErr,L"IO测试",MB_OK|MB_ICONERROR );  
    }

    return TEST_NORMAL;
}

