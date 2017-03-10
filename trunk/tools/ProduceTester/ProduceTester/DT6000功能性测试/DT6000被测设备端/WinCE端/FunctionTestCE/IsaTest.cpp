#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "IsaTest.h"
#include "iniFile.h"
#include "IsaMemLib.h"

#define ISA_INT_ADDR                0x200
#define ISA_MEM8_BASE               0x20000
#define ISA_MEM8_SIZE               (128 * 1024)
#define ISA_MEM16_BASE              0x100000
#define    ISA_MEM16_SIZE           (256 * 2 * 1024)
#define ISA_BUFFER_SIZE             32768

//ISA测试的步骤
#define     ISA_TEST_MEM_16         0           //16位内存测试
#define     ISA_TEST_MEM_8          1           //8位内存测试
#define     ISA_TEST_INT            2           //ISA中断测试

static     const int g_nIntMap[]    = {3, 4, 5, 6, 7, 9, 10, 11, 12, 14, 15};
static  const int g_nMapSize    =(int )sizeof(g_nIntMap)/sizeof(g_nIntMap[0]);

//定义ISA测试的命令数据包
#pragma pack(1)
struct IsaTestCmdPacket
{
    UINT32  uiTestStep;         //是进行三种测试的哪一钟
    UINT32  uiStatrAddr;        //内存测试开始的地址
    UINT32  uiMemLen;           //内存测试的测试内存的长度
    UINT32  uiIntIndex;         //要测试的中断在g_nIntMap中的索引    
};
#pragma pack()

CIsaTest::CIsaTest()
:m_pBufWirte(NULL)
,m_pbufRead(NULL)
,m_hIntEvent(NULL)
{

}

CIsaTest::~CIsaTest()
{
    if (m_hIntEvent)
    {
        CloseHandle(m_hIntEvent);
        m_hIntEvent=NULL;
    }

    if (m_pbufRead)
    {
        free(m_pbufRead);
        m_pbufRead=NULL;
    }
    if (m_pBufWirte)
    {
        free(m_pBufWirte);
        m_pBufWirte=NULL;
    }
}


BOOL CIsaTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{
    WCHAR strSection[]=L"ISATest_\0\0";
    strSection[wcslen(strSection)]=L'1'+index;

    //读取测试项的名字
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"CAN 1",
        m_strTestName,31,strIniFile);
    if (!IsaMem_LoadLib())
    {
        MessageBox(NULL,L"加载epcIsa.dll出错",L"错误",MB_ICONERROR|MB_OK);
        return FALSE;
    }
    m_hIntEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
    if (m_hIntEvent ==NULL)
    {
        MessageBox(NULL,L"ISA测试创建中断事件对象失败",L"错误",MB_ICONERROR|MB_OK);
        return FALSE;
    }

    //分配读写用的内存
    m_pbufRead=(BYTE *)malloc(ISA_BUFFER_SIZE);
    m_pBufWirte=(BYTE *)malloc(ISA_BUFFER_SIZE);    

    if (m_pBufWirte ==NULL || m_pbufRead ==NULL)
    {
        MessageBox(NULL,L"ISA测试分配内存失败",L"错误",MB_OK|MB_ICONERROR);
        free(m_pbufRead);
        free(m_pBufWirte);
        m_pBufWirte=NULL;
        m_pbufRead=NULL;
        CloseHandle(m_hIntEvent);
        m_hIntEvent=NULL;
        return FALSE;
    }
    m_uiCmd=index+CMD_TEST_ISA;
    *uipCmdIndex=m_uiCmd;
    return TRUE;
}

TEST_STATUS CIsaTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
                                       const BYTE *pData,CConnect *pConnect)
{
    IsaTestCmdPacket *pCmd=(IsaTestCmdPacket *)pData;
    switch (pCmd->uiTestStep)
    {
    case ISA_TEST_MEM_16:
        ExcuteOneMem16Test(pCmd->uiStatrAddr,pCmd->uiMemLen);
        break;
    case ISA_TEST_MEM_8:
        ExcuteOneMem8Test(pCmd->uiStatrAddr,pCmd->uiMemLen);
        break;
    case  ISA_TEST_INT:
        ExcuteOneIntTest(pCmd->uiIntIndex);
        break;
    }

    TEST_INFO_TRANSFER tit;
    TestInfoLocalToTransfer(&tit,&m_ti);

    int nRet=pConnect->SendAPacket(m_uiCmd,param,sizeof(tit), 
        (BYTE *)&tit );
    
    if (nRet >0)
    {
        return tit.bError?TEST_NORMAL:TEST_ERROR;
    }

    return TEST_CONNECTDOWN;
}

BOOL CIsaTest::ExcuteOneIntTest(UINT32 uiIntIndex)
{
    DWORD   dwSysInt=0;    
    ResetEvent(m_hIntEvent);
    epcIsaInitIntrrupt(g_nIntMap[uiIntIndex], m_hIntEvent, &dwSysInt);
    Sleep(1);    
    epcIsaIoWordW(ISA_INT_ADDR, (WORD)~(1 << uiIntIndex));
    Sleep(1);
    epcIsaIoWordW(ISA_INT_ADDR, 0xFFFF);

    if (WaitForSingleObject(m_hIntEvent, 200) == WAIT_OBJECT_0)
    {
        m_ti.bError=0;
        m_ti.uiErrorLen=0;
    }
    else
    {
        m_ti.bError=1;
        m_ti.uiErrorLen=swprintf(m_ti.strErr,L"IRQ%d未发生",g_nIntMap[uiIntIndex]);
    }
   
    epcIsaIntrruptDone(dwSysInt);
    epcIsaIntrruptDisable(dwSysInt);
    epcIsaIoWordW(ISA_INT_ADDR, 0xFFFF);
    return TRUE;
}

BOOL CIsaTest::ExcuteOneMem16Test(UINT32 uiStartAddr,UINT32 uiLen)
{    
    unsigned int    i;
    DWORD           dwAddr;

    //构造测试数据
    for (i=0;i<uiLen;i++)
    {
        m_pBufWirte[i]=rand()&0xFF;
    }
    //将测试数据写入ISA内存
    UINT16 *pData=(UINT16 *)m_pBufWirte;
    uiLen=uiLen/2;
    dwAddr=uiStartAddr;
    for(i=0;i<uiLen;i++)
    {
        epcIsaMemWordW(dwAddr,pData[i]);
        dwAddr+=2;
    }

    //从ISA中读取数据
    pData=(UINT16 *)m_pbufRead;
    dwAddr=uiStartAddr;
    for (i=0;i<uiLen;i++)
    {
        epcIsaMemWordR(dwAddr,pData);
        pData++;
        dwAddr+=2;
    }

    uiLen=uiLen*2;
    //比较写入和读出的数据
    if (memcmp(m_pbufRead,m_pBufWirte,uiLen) ==0 )
    {
        m_ti.bError=0;
        m_ti.uiErrorLen=0;
    }
    else
    {
        m_ti.bError=1;
        m_ti.uiErrorLen=1+swprintf(m_ti.strErr,L"16位内存测试中从0x%x地址开始的%d"
            L"字节的数据读写校验不一致",uiStartAddr,uiLen);
    }
    return TRUE;    
}

BOOL CIsaTest::ExcuteOneMem8Test(UINT32 uiStartAddr,UINT32 uiLen)
{
    DWORD           i;   
    DWORD           dwAddr;

    //构造测试数据
    for (i=0;i<uiLen;i++)
    {
        m_pBufWirte[i]=rand()&0xFF;
    }
    //将测试数据写入ISA内存
    BYTE *pData=m_pBufWirte;   
    dwAddr=uiStartAddr;
    for(i=0;i<uiLen;i++)
    {
        epcIsaMemByteW(dwAddr,pData[i]);
        dwAddr++;
    }

    //从ISA中读取数据
    pData=m_pbufRead;
    dwAddr=uiStartAddr;
    for (i=0;i<uiLen;i++)
    {
        epcIsaMemByteR(dwAddr,pData);
        pData++;
        dwAddr++;
    }

    //比较写入和读出的数据
    if (memcmp(m_pbufRead,m_pBufWirte,uiLen) ==0 )
    {
        m_ti.bError=0;
        m_ti.uiErrorLen=0;
    }
    else
    {
        m_ti.bError=1;
        m_ti.uiErrorLen=swprintf(m_ti.strErr,L"8位内存测试中从0x%x地址开始的%d"
            L"字节的数据读写校验不一致",uiStartAddr,uiLen);
    }

    return TRUE;
}