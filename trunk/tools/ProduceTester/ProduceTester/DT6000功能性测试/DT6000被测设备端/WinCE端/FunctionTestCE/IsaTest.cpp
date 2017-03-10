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

//ISA���ԵĲ���
#define     ISA_TEST_MEM_16         0           //16λ�ڴ����
#define     ISA_TEST_MEM_8          1           //8λ�ڴ����
#define     ISA_TEST_INT            2           //ISA�жϲ���

static     const int g_nIntMap[]    = {3, 4, 5, 6, 7, 9, 10, 11, 12, 14, 15};
static  const int g_nMapSize    =(int )sizeof(g_nIntMap)/sizeof(g_nIntMap[0]);

//����ISA���Ե��������ݰ�
#pragma pack(1)
struct IsaTestCmdPacket
{
    UINT32  uiTestStep;         //�ǽ������ֲ��Ե���һ��
    UINT32  uiStatrAddr;        //�ڴ���Կ�ʼ�ĵ�ַ
    UINT32  uiMemLen;           //�ڴ���ԵĲ����ڴ�ĳ���
    UINT32  uiIntIndex;         //Ҫ���Ե��ж���g_nIntMap�е�����    
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

    //��ȡ�����������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"CAN 1",
        m_strTestName,31,strIniFile);
    if (!IsaMem_LoadLib())
    {
        MessageBox(NULL,L"����epcIsa.dll����",L"����",MB_ICONERROR|MB_OK);
        return FALSE;
    }
    m_hIntEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
    if (m_hIntEvent ==NULL)
    {
        MessageBox(NULL,L"ISA���Դ����ж��¼�����ʧ��",L"����",MB_ICONERROR|MB_OK);
        return FALSE;
    }

    //�����д�õ��ڴ�
    m_pbufRead=(BYTE *)malloc(ISA_BUFFER_SIZE);
    m_pBufWirte=(BYTE *)malloc(ISA_BUFFER_SIZE);    

    if (m_pBufWirte ==NULL || m_pbufRead ==NULL)
    {
        MessageBox(NULL,L"ISA���Է����ڴ�ʧ��",L"����",MB_OK|MB_ICONERROR);
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
        m_ti.uiErrorLen=swprintf(m_ti.strErr,L"IRQ%dδ����",g_nIntMap[uiIntIndex]);
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

    //�����������
    for (i=0;i<uiLen;i++)
    {
        m_pBufWirte[i]=rand()&0xFF;
    }
    //����������д��ISA�ڴ�
    UINT16 *pData=(UINT16 *)m_pBufWirte;
    uiLen=uiLen/2;
    dwAddr=uiStartAddr;
    for(i=0;i<uiLen;i++)
    {
        epcIsaMemWordW(dwAddr,pData[i]);
        dwAddr+=2;
    }

    //��ISA�ж�ȡ����
    pData=(UINT16 *)m_pbufRead;
    dwAddr=uiStartAddr;
    for (i=0;i<uiLen;i++)
    {
        epcIsaMemWordR(dwAddr,pData);
        pData++;
        dwAddr+=2;
    }

    uiLen=uiLen*2;
    //�Ƚ�д��Ͷ���������
    if (memcmp(m_pbufRead,m_pBufWirte,uiLen) ==0 )
    {
        m_ti.bError=0;
        m_ti.uiErrorLen=0;
    }
    else
    {
        m_ti.bError=1;
        m_ti.uiErrorLen=1+swprintf(m_ti.strErr,L"16λ�ڴ�����д�0x%x��ַ��ʼ��%d"
            L"�ֽڵ����ݶ�дУ�鲻һ��",uiStartAddr,uiLen);
    }
    return TRUE;    
}

BOOL CIsaTest::ExcuteOneMem8Test(UINT32 uiStartAddr,UINT32 uiLen)
{
    DWORD           i;   
    DWORD           dwAddr;

    //�����������
    for (i=0;i<uiLen;i++)
    {
        m_pBufWirte[i]=rand()&0xFF;
    }
    //����������д��ISA�ڴ�
    BYTE *pData=m_pBufWirte;   
    dwAddr=uiStartAddr;
    for(i=0;i<uiLen;i++)
    {
        epcIsaMemByteW(dwAddr,pData[i]);
        dwAddr++;
    }

    //��ISA�ж�ȡ����
    pData=m_pbufRead;
    dwAddr=uiStartAddr;
    for (i=0;i<uiLen;i++)
    {
        epcIsaMemByteR(dwAddr,pData);
        pData++;
        dwAddr++;
    }

    //�Ƚ�д��Ͷ���������
    if (memcmp(m_pbufRead,m_pBufWirte,uiLen) ==0 )
    {
        m_ti.bError=0;
        m_ti.uiErrorLen=0;
    }
    else
    {
        m_ti.bError=1;
        m_ti.uiErrorLen=swprintf(m_ti.strErr,L"8λ�ڴ�����д�0x%x��ַ��ʼ��%d"
            L"�ֽڵ����ݶ�дУ�鲻һ��",uiStartAddr,uiLen);
    }

    return TRUE;
}