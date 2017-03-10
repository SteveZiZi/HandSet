#include <windows.h>
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "IsaTest.h"
#include "assertMsg.h"

#define ISA_INT_ADDR                0x200
#define ISA_MEM8_BASE               0x20000
#define ISA_MEM8_SIZE               (128 * 1024)
#define ISA_MEM16_BASE              0x100000
#define    ISA_MEM16_SIZE           (256 * 2 * 1024)
#define ISA_BUFFER_SIZE             32768

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





CIsaTest::CIsaTest():
m_isaTestStep(ISA_TEST_MEM_16),
m_iTestStepCount(0)
{

}

CIsaTest::~CIsaTest()
{

}

BOOL CIsaTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"ISATest_\0\0";
    strSection[8]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"ISA",m_strTestName,31,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_ISA;
    //����CTestManager����ָ��
    assertMsg(pManager,L"pManagerָ�벻��Ϊ��!");
    m_pManager=pManager;

    return TRUE;
}

TEST_STATUS CIsaTest::OnBeginTest(CConnect *pConnect)
{
    m_isaTestStep=ISA_TEST_MEM_16;
    m_iTestStepCount=0;
    return TEST_NORMAL;
}

TEST_STATUS CIsaTest::OnEndTest(CConnect *pConnect)
{
    return TEST_NORMAL;
}

int CIsaTest::GetShouldTestTimes()const
{
    int TestConut=ISA_MEM16_SIZE/ISA_BUFFER_SIZE+ISA_MEM8_SIZE/ISA_BUFFER_SIZE+
                    g_nMapSize;

    if (ISA_MEM16_SIZE %ISA_BUFFER_SIZE !=0)
    {
        TestConut++;
    }

    if (ISA_MEM8_SIZE%ISA_BUFFER_SIZE !=0 )
    {
        TestConut++;
    }

    return TestConut;
}

TEST_STATUS CIsaTest::ExcuteOneTest(CConnect *pConnect)
{
    UINT32  uiAddr=0;
    UINT32  uiLen=0;

    IsaTestCmdPacket itcp={-1};
    

    switch(m_isaTestStep)
    {
    case ISA_TEST_MEM_16:
        //����16λ�ڴ���ԵĿ�ʼ��ַ�ͳ���
        uiAddr=ISA_MEM16_BASE+ISA_BUFFER_SIZE*m_iTestStepCount;
        uiLen=ISA_MEM16_SIZE-ISA_BUFFER_SIZE*m_iTestStepCount;
        uiLen=uiLen>=ISA_BUFFER_SIZE?ISA_BUFFER_SIZE:uiLen;
        //���������
        itcp.uiTestStep=ISA_TEST_MEM_16;
        itcp.uiStatrAddr=uiAddr;
        itcp.uiMemLen=uiLen;

        //�ж��Ƿ��ֵ���һ���Ĳ���
        if (uiAddr +uiLen >=ISA_MEM16_BASE+ISA_MEM16_SIZE )
        {
            m_isaTestStep=ISA_TEST_MEM_8;
            m_iTestStepCount=0;
        }
        else
        {
            m_iTestStepCount++;
        }
        break;

    case ISA_TEST_MEM_8:
        uiAddr=ISA_MEM8_BASE+ISA_BUFFER_SIZE*m_iTestStepCount;
        uiLen=ISA_MEM8_SIZE-ISA_BUFFER_SIZE*m_iTestStepCount;
        uiLen=uiLen>=ISA_BUFFER_SIZE?ISA_BUFFER_SIZE:uiLen;
        //���������
        itcp.uiTestStep=ISA_TEST_MEM_8;
        itcp.uiStatrAddr=uiAddr;
        itcp.uiMemLen=uiLen;

        //�ж��Ƿ��ֵ���һ���Ĳ���
        if (uiAddr +uiLen >=ISA_MEM8_BASE+ISA_MEM8_SIZE )
        {
            m_isaTestStep=ISA_TEST_INT;
            m_iTestStepCount=0;
        }
        else
        {
            m_iTestStepCount++;
        }
        break;
    
    case ISA_TEST_INT:
        //���������
        itcp.uiTestStep=ISA_TEST_INT;
        itcp.uiIntIndex=m_iTestStepCount;
        //�ж��Ƿ��ֵ���һ���Ĳ��� 
        m_iTestStepCount++;
        if (m_iTestStepCount>=g_nMapSize)
        {
            m_iTestStepCount=0;
            m_isaTestStep=ISA_TEST_MEM_16;
        }
        break;       

    default:
        break;
    }
    
    //�����������ݰ��������ղ��Խ��
    int nRet;
    nRet=pConnect->SendAPacket(m_uiCmd,0,sizeof(itcp),(BYTE *)&itcp);
    if (nRet >0)
    {
        PACKET pa;
        nRet =pConnect->RecvAPacket(&pa,15000);
        if (nRet >0)
        {
            m_uiTestCount++;
            TEST_INFO_TRANSFER *pInfo=(TEST_INFO_TRANSFER *)pa.pData;
            if (pInfo->bError)
            {
                TEST_INFO   ti;
                TestInfoTransferToLocal(&ti,pInfo);
                m_uiTestErrCount++;

                m_pManager->WriteLog(this,ti.strErr);
                return TEST_ERROR;
            }
            return TEST_NORMAL;
        }
    }
    
    return TEST_CONNECTDOWN;
}