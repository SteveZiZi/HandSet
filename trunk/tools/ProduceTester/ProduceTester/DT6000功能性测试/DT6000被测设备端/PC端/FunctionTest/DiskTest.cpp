#include <Windows.h>
#include <atlstr.h>
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "DiskTest.h"
#include "assertMsg.h"

BOOL CDiskTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"DiskTest_\0\0";
    strSection[9]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"磁盘1",m_strTestName,31,strIniFile);

    m_bRemoveable=GetPrivateProfileInt(strSection,L"bRemoveable",0,
        strIniFile);
    m_lSlotNum=GetPrivateProfileInt(strSection,L"SlotNum",1,strIniFile);

    if (m_bRemoveable && m_lSlotNum <1)
    {
        pManager->MessageBox(L"错误的配置",L"磁盘测试",MB_OK|MB_ICONERROR);
        return FALSE;
    }

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_DISK1;
    //保存CTestManager对象指针
    assertMsg(pManager,L"pManager指针不能为空!");
    m_pManager=pManager;
    m_lSlotNumTested=0;

    return TRUE;
}

TEST_STATUS CDiskTest::OnBeginTest(CConnect *pConnect)
{
    TEST_STATUS statusRet=TEST_NORMAL;
    int nRet;
    PACKET      pa;
    pa.param=PARAM_GET_INIT_INFO;
    pa.uiCmd=m_uiCmd;
    pa.uiDatalen=0;

    if(m_bRemoveable&&m_lSlotNumTested==0)
    {
        CString  str;
        str.Format(L"请将%s插入%s接口1",m_strTestName,m_strTestName);
        m_pManager->MessageBox(str,L"磁盘测试",MB_OK|MB_ICONINFORMATION);
    }    

    nRet =pConnect->SendAPacket(&pa);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }
    
    nRet=pConnect->RecvAPacket(&pa,10000);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    TEST_INFO *pInfo=(TEST_INFO *)pa.pData;
    if (pInfo->bError)
    {
        m_pManager->MessageBox(pInfo->strErr,L"磁盘测试",MB_OK|MB_ICONERROR);
        return TEST_ERROR;
    }

    return TEST_NORMAL;
}

TEST_STATUS CDiskTest::OnEndTest(CConnect *pConnect)
{
    TEST_STATUS statusRet=TEST_NORMAL;
    int nRet;
    PACKET      pa;
    pa.param=DISK_TEST_CMD_DELETE;
    pa.uiCmd=m_uiCmd;
    pa.uiDatalen=0;

    m_pManager->showTestStatus(L"正在删除测试文件...");
    nRet =pConnect->SendAPacket(&pa);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    nRet=pConnect->RecvAPacket(&pa,10000);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    if (m_lSlotNumTested==m_lSlotNum)
    {
        m_lSlotNumTested=0;
    }

    return TEST_NORMAL;
}

TEST_STATUS CDiskTest::ExcuteOneTest(CConnect *pConnect)
{
    UINT32 uiVerifySumWrite=0,uiVerifySumRead=0;
    int nRet;

    if (m_uiTestCount%3==0 &&m_uiTestCount !=0 )
    {       
        if (m_lSlotNumTested <m_lSlotNum)
        {
            CString str;
            str.Format(L"请将%s插入下一个接口，已经测试%d个接口，还剩%d个接口",
                m_strTestName,m_lSlotNumTested,m_lSlotNum-m_lSlotNumTested);
            m_pManager->MessageBox(str,L"磁盘测试",MB_OK|MB_ICONERROR);

            Sleep(2000);
            nRet=OnBeginTest(pConnect);
            if (nRet ==TEST_CONNECTDOWN)
            {
                return TEST_CONNECTDOWN;
            }

            if (nRet ==TEST_ERROR)
            {
                m_uiTestCount++;
                m_uiTestErrCount++;
                return TEST_ABORT;
            }
        }
    }


    m_uiTestCount++;
    m_pManager->showTestStatus(L"正在创建并写入测试文件...");
    TEST_STATUS statusRet=TEST_NORMAL;    
    PACKET      pa;
    pa.param=DISK_TEST_CMD_WRITE;
    pa.uiCmd=m_uiCmd;
    pa.uiDatalen=0;

    nRet =pConnect->SendAPacket(&pa);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    nRet=pConnect->RecvAPacket(&pa,10000);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    TEST_INFO *pInfo=(TEST_INFO *)pa.pData;
    if (pInfo->bError)
    {
        //创建写入文件出错
        m_pManager->WriteLog(this,pInfo->strErr);
        m_uiTestErrCount++;
        return TEST_ERROR;
    }
    uiVerifySumWrite=*(UINT32 *)pInfo->bAppend;

    m_pManager->showTestStatus(L"正在读取测试文件...");
    pa.param=DISK_TEST_CMD_READ;
    pa.uiCmd=m_uiCmd;
    pa.uiDatalen=0;

    nRet =pConnect->SendAPacket(&pa);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    nRet=pConnect->RecvAPacket(&pa,10000);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    pInfo=(TEST_INFO *)pa.pData;
    if (pInfo->bError)
    {
        //读取文件出错
        m_pManager->WriteLog(this,pInfo->strErr);
        m_uiTestErrCount++;
        return TEST_ERROR;
    }
    uiVerifySumRead=*(UINT32 *)pInfo->bAppend;

    if (uiVerifySumRead !=uiVerifySumWrite)
    {
        //文件校验出错
        m_pManager->WriteLog(this,L"文件读写校验和不一致");
        m_uiTestErrCount++;
        return TEST_ERROR;
    }

    if (m_uiTestCount%3==0 &&m_uiTestCount !=0 )
    {
        m_lSlotNumTested++;
        if (m_lSlotNumTested <m_lSlotNum)
        {
            nRet=OnEndTest(pConnect);
            if (nRet ==TEST_CONNECTDOWN)
            {
                return TEST_CONNECTDOWN;
            }
        }
    }

    return TEST_NORMAL;
}

int CDiskTest::GetShouldTestTimes()const
{
    if (m_bRemoveable)
    {
        return m_lSlotNum*3;
    }
    return 3;
}
