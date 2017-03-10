#include "stdafx.h"
#include <stdlib.h>

#include "testBase.h"
#include "ISATest.h"
#include "iniFile.h"
#include "IsaMemLib.h"
#include "assertMsg.h"

#define ISA_INT_ADDR			0x200
#define ISA_MEM8_BASE			0x20000
#define ISA_MEM8_SIZE			(128 * 1024)
#define ISA_MEM16_BASE			0x100000
#define	ISA_MEM16_SIZE			(256 * 2 * 1024)
#define ISA_BUFFER_SIZE			32768

CISATest::CISATest():

m_pBufWirte(NULL),
m_pbufRead(NULL),
m_testStep(ISA_TEST_MEM_16),
m_uiTestTimeStep(0),
m_hIntEvent(NULL)
{
    m_strTestName[0]=0;

    wcscpy(m_reportNape[0].strNape,L"���Դ���");
    wcscpy(m_reportNape[1].strNape,L"�������");
    wcscpy(m_reportNape[2].strNape,L"��ȷ��");
    wcscpy(m_reportNape[3].strNape,L"����ʱ��"); 
}

CISATest::~CISATest()
{
    IsaMem_UnLoadLib();
    if (m_hIntEvent!=NULL)
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


BOOL CISATest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"ISATest_\0\0";
    strSection[wcslen(strSection)]=L'1'+index;

    //��ȡ�����������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"ISA",
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

    m_testStep=ISA_TEST_MEM_16;   
    m_uiTestTimeStep=0;

    m_pManager = pManager;
    return TRUE;
}

TEST_SITUATION CISATest::ExcuteOneTest()
{
    switch(m_testStep)
    {
    case  ISA_TEST_MEM_16:
        return ExcuteOneMem16Test();
        break;
    case  ISA_TEST_MEM_8:
        return ExcuteOneMem8Test();
        break;
    case  ISA_TEST_INT:
        return ExcuteOneIntTest();
        break;
    }
    return SITUATION_NORMAL;
    
}

TEST_SITUATION CISATest::ExcuteOneMem16Test()
{
    DWORD       len=ISA_BUFFER_SIZE;
    DWORD       i;

    DWORD       dwBaseAddr=m_uiTestTimeStep*ISA_BUFFER_SIZE+ISA_MEM16_BASE;
    DWORD       dwAddr;

    TEST_SITUATION      siRet=SITUATION_NORMAL;

    if ( (m_uiTestTimeStep+1)*ISA_BUFFER_SIZE>=ISA_MEM16_SIZE )
    {
        len=ISA_MEM16_SIZE-m_uiTestTimeStep*ISA_BUFFER_SIZE;
    }

    //�����������
    for (i=0;i<len;i++)
    {
        m_pBufWirte[i]=rand()&0xFF;
    }
    //����������д��ISA�ڴ�
    UINT16 *pData=(UINT16 *)m_pBufWirte;
    len=len/2;
    dwAddr=dwBaseAddr;
    for(i=0;i<len;i++)
    {
        epcIsaMemWordW(dwAddr,pData[i]);
        dwAddr+=2;
    }

    //��ISA�ж�ȡ����
    pData=(UINT16 *)m_pbufRead;
    dwAddr=dwBaseAddr;
    for (i=0;i<len;i++)
    {
        epcIsaMemWordR(dwAddr,pData);
        pData++;
        dwAddr+=2;
    }

    len=len*2;
    //�Ƚ�д��Ͷ���������
    if (memcmp(m_pbufRead,m_pBufWirte,len) ==0 )
    {
        siRet=SITUATION_NORMAL;
    }
    else
    {
        siRet=SITUATION_ERROR;
        swprintf(m_strLastError,L"16λ�ڴ�����д�0x%x��ַ��ʼ��%d"
            L"�ֽڵ����ݶ�дУ�鲻һ��",dwBaseAddr,len);
    }
    
    if ((m_uiTestTimeStep +1)*ISA_BUFFER_SIZE >=ISA_MEM16_SIZE )
    {
        m_testStep=ISA_TEST_MEM_8;;
        m_uiTestTimeStep=0;
    }
    else
    {
        m_uiTestTimeStep++;
    }

    m_uiTestCount++;
    if (siRet !=SITUATION_NORMAL)
    {
        m_uiErrCount++;
    }

    return siRet;
}

TEST_SITUATION CISATest::ExcuteOneMem8Test()
{
    DWORD       len=ISA_BUFFER_SIZE;
    DWORD         i;

    DWORD   dwBaseAddr=m_uiTestTimeStep*ISA_BUFFER_SIZE+ISA_MEM8_BASE;
    DWORD   dwAddr;
    TEST_SITUATION      siRet=SITUATION_NORMAL;

    if ( (m_uiTestTimeStep+1)*ISA_BUFFER_SIZE>=ISA_MEM8_SIZE )
    {
        len=ISA_MEM8_SIZE-m_uiTestTimeStep*ISA_BUFFER_SIZE;
    }


    //�����������
    for (i=0;i<len;i++)
    {
        m_pBufWirte[i]=rand()&0xFF;
    }
    //����������д��ISA�ڴ�
    BYTE *pData=m_pBufWirte;   
    dwAddr=dwBaseAddr;
    for(i=0;i<len;i++)
    {
        epcIsaMemByteW(dwAddr,pData[i]);
        dwAddr++;
    }

    //��ISA�ж�ȡ����
    pData=m_pbufRead;
    dwAddr=dwBaseAddr;
    for (i=0;i<len;i++)
    {
        epcIsaMemByteR(dwAddr,pData);
        pData++;
        dwAddr++;
    }

    //�Ƚ�д��Ͷ���������
    if (memcmp(m_pbufRead,m_pBufWirte,len) ==0 )
    {
        siRet=SITUATION_NORMAL;
    }
    else
    {
        siRet=SITUATION_ERROR;
        swprintf(m_strLastError,L"8λ�ڴ�����д�0x%x��ַ��ʼ��%d"
            L"�ֽڵ����ݶ�дУ�鲻һ��",dwBaseAddr,len);
    }

    if ((m_uiTestTimeStep +1)*ISA_BUFFER_SIZE >=ISA_MEM8_SIZE )
    {
        m_testStep=ISA_TEST_INT;
        m_uiTestTimeStep=0;
    }
    else
    {
        m_uiTestTimeStep++;
    }

    m_uiTestCount++;
    if (siRet !=SITUATION_NORMAL)
    {
        m_uiErrCount++;
    }

    return siRet;
}

static 	const int g_nIntMap[]	= {3, 4, 5, 6, 7, 9, 10, 11, 12, 14, 15};
static  const int g_nMapSize    =(int )sizeof(g_nIntMap)/sizeof(g_nIntMap[0]);
TEST_SITUATION CISATest::ExcuteOneIntTest()
{
    DWORD               dwSysInt=0;    
    TEST_SITUATION      siRet=SITUATION_NORMAL;
    ResetEvent(m_hIntEvent);
    epcIsaInitIntrrupt(g_nIntMap[m_uiTestTimeStep], m_hIntEvent, &dwSysInt);
    Sleep(1);    
    epcIsaIoWordW(ISA_INT_ADDR, (WORD)~(1 << m_uiTestTimeStep));
    Sleep(1);
    epcIsaIoWordW(ISA_INT_ADDR, 0xFFFF);

    if (WaitForSingleObject(m_hIntEvent, 200) == WAIT_OBJECT_0)
    {
       siRet=SITUATION_NORMAL;
    }
    else
    {
        siRet=SITUATION_ERROR;
        swprintf(m_strLastError,L"IRQ%dδ����",g_nIntMap[m_uiTestTimeStep]);
    }

    if (m_uiTestTimeStep <g_nMapSize-1)
    {
        m_uiTestTimeStep++;
    }
    else
    {
        m_uiTestTimeStep=0;
        m_testStep=ISA_TEST_MEM_16;
    }
    epcIsaIntrruptDone(dwSysInt);
    epcIsaIntrruptDisable(dwSysInt);
    epcIsaIoWordW(ISA_INT_ADDR, 0xFFFF);

    m_uiTestCount++;
    if (siRet !=SITUATION_NORMAL)
    {
        m_uiErrCount++;
    }

    return siRet;
}

const SReportNape *CISATest::GetReportNapes(int &count)
{
    count =4;
    unsigned int h,m,s;
    UINT64 t=m_uiTestTime;

    swprintf(m_reportNape[0].strNapeDescriptor,L"%d��",m_uiTestCount);
    m_reportNape[0].bHighLight=FALSE;

    swprintf(m_reportNape[1].strNapeDescriptor,L"%d��",m_uiErrCount);    
    if (m_uiTestCount >0)
    {
        swprintf(m_reportNape[2].strNapeDescriptor,L"%.2f%% ",
            100 *(double)(m_uiTestCount-m_uiErrCount)/m_uiTestCount );
    }
    else
    {
        swprintf(m_reportNape[2].strNapeDescriptor,L"0.0%% ");
    }

    if (m_uiErrCount >0)
    {
        m_reportNape[1].bHighLight=TRUE;
        m_reportNape[2].bHighLight=TRUE;
    }
    else
    {
        m_reportNape[1].bHighLight=FALSE;
        m_reportNape[2].bHighLight=FALSE;
    }  

    h= (unsigned int)( t/(1000*3600));
    t=t%(1000*3600);
    m=(unsigned int) (t/(1000 *60));
    t=t%(1000 *60);
    s=(unsigned int)(t/1000);

    swprintf(m_reportNape[3].strNapeDescriptor,L"%dʱ%d��%d��",h,m,s);
    m_reportNape[3].bHighLight=FALSE;

    return m_reportNape;
}