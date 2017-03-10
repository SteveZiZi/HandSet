#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "GpioTest.h"
#include "assertMsg.h"
//��Ȼ�������ΪGPIO������ʵ������GPIO���ܣ�
//����ʵ�����������涨�Ĳ���������������������ɴ���
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
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"GPIOTest_\0\0";
    strSection[9]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"GPIO",m_strTestName,31,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_GPIO;
    //����CTestManager����ָ��
    assertMsg(pManager,L"pManagerָ�벻��Ϊ��!");
    m_pManager=pManager;

    m_uiInPinNum=GetPrivateProfileInt(strSection,L"InPinNum",8,strIniFile);
    m_uiOutPinNum=GetPrivateProfileInt(strSection,L"OutPinNum",8,strIniFile);

    if (m_uiOutPinNum >m_uiInPinNum)
    {
        CString str;
        str.Format(L"%d,%d",m_uiInPinNum,m_uiOutPinNum);
        AfxMessageBox(str);
        pManager->MessageBox(L"����ʵ�ֵĲ��Է�����������Ų��ܱ�������Ŷ�",
            L"IO����",MB_OK|MB_ICONERROR);
        return FALSE;
    }
    m_uiShouldTestCount=m_uiOutPinNum*2;

    return TRUE;
}

TEST_STATUS CGpioTest::ExcuteOneTest(CConnect *pConnect)
{
    UINT32  uiOut;
    UINT32  uiIn;

    DWORD   dwTestPin=m_uiTestCount/2;                  //��ǰ���Ե��������
    BOOL    bTestHigh=m_uiTestCount%2;                  //��ǰ������������Ƿ�Ϊ��

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
    //�Ա�����Ͷ���
    uiIn=*(UINT32 *)pa.pData;
    uiIn&=m_uiInMask;
    
    m_uiTestCount++;
    
    //�Ա�����������ÿ�ζԱ�m_uiOutPinNum;
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
                //��⵽��������                
                m_uiErrMask|=1<<(index +j);
                bError=TRUE;
            }
        }
    }

    if (bError)
    {
        m_uiTestErrCount++;
        //��¼������־
        WCHAR   strLog[512];
        swprintf(strLog,L"���0x%02x,����0x%02x",uiOut,uiIn);

        m_pManager->WriteLog(this,strLog);
        return TEST_ERROR;
    }

    return TEST_NORMAL;
}

TEST_STATUS CGpioTest::OnBeginTest(CConnect *pConnect)
{
    //�����������������������
    m_uiInMask=~0;
    m_uiInMask  =m_uiInMask <<m_uiInPinNum;
    m_uiInMask  =~m_uiInMask;

    m_uiOutMask=~0;
    m_uiOutMask=m_uiOutMask <<m_uiOutPinNum;
    m_uiOutMask =~m_uiOutMask;
    //��ʼ����������
    m_uiErrMask=0;
    m_pManager->MessageBox(L"��ע�⽫����GPIO�����߽ӵ�����������"
        ,L"IO����",MB_ICONINFORMATION|MB_OK);
    
    return TEST_NORMAL;
}

TEST_STATUS CGpioTest::OnEndTest(CConnect *pConnect)
{
    //������Թ��̷����˴�������ʾ������Ϣ
    if (m_uiErrMask !=0)
    {
        CString  strErr=_T("��");
        CString  str;

        unsigned int j=0;

        //ɨ��ֱ����һ�������λ
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
        strErr+=_T("λ��ƽ����");
        m_pManager->MessageBox(strErr,L"IO����",MB_OK|MB_ICONERROR );  
    }

    return TEST_NORMAL;
}

