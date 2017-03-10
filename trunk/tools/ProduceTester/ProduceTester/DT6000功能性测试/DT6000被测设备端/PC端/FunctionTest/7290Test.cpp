#include <windows.h>
#include <atlstr.h>
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "7290Test.h"
#include "assertMsg.h"

BOOL C7290Test::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"7290Test_\0\0";
    strSection[9]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"ADC",m_strTestName,31,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_7290;
    //����CTestManager����ָ��
    assertMsg(pManager,L"pManagerָ�벻��Ϊ��!");
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
        m_pManager->MessageBox(L"�����������С�����ϵİ��������۲��Ա�����ܵ���ʾ,"
            L"��ȫ��������������ɺ��ٵ���\"ȷ��\" ��ť",
            L"����",MB_OK);
        pa.uiCmd=m_uiCmd;
        pa.param=PARAM_7290_END;

        nRet =pConnect->SendAPacket(&pa);
        nRet =pConnect->RecvAPacket(&pa,5000);

        if (nRet >0)
        {
            nRet=m_pManager->MessageBox(L"����ܵ���ʾ�ͼ��̵İ����Ƿ�һһ��Ӧ?",
                L"����",MB_YESNO|MB_ICONQUESTION);

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