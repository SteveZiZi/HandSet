#ifndef __TEST_BASE_H__
#define __TEST_BASE_H__

#include "testCommon.h"
//�������״̬
typedef  enum  _TEST_STATUS
{
    TEST_ERROR=0,
    TEST_NORMAL,    
    TEST_CONNECTDOWN,
    TEST_ABORT,                             //������ΪĳЩԭ������ò���ֹ      
}TEST_STATUS;


//�������в�����Ŀ�Ļ���CTestBase�����еĲ�����Ĵ����඼������������������
class CTestManager;
class CTestBase
{
public:
    CTestBase():
    m_uiTestCount(0)
    ,m_uiTestErrCount(0)
    ,m_pManager(NULL)
    ,m_uiCmd(0)
    {
        m_strTestName[0]=0;
    }
    virtual ~CTestBase()
    {
    }

    /*****************************************************************************************
    ** Function name:           Init   
    ** Descriptions:            ��ʼ���˲��������,����pManager������newһ���µĲ���������ʱ��
                                ������������������ʼ�����������
                                ���������������ȥ��ʼ�����豸������Ӧ��Ӳ��
    **
    ** input parameters:        strIniFile �����ļ���ȫ·���ļ���
                                index      �ǵڼ������̲����������0��ʼ��0��ʾ���̲���1
                                pManager   ��������������CTestManager����ָ��                            
    ** Returned value:          ʧ�ܷ���FALSE����������ֵ������ʼ���ɹ�
    ******************************************************************************************/
    virtual BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager)=0;

    //�ڿ�ʼ���Ժ�ֹͣ����Ҫִ�еĲ���
    //OnBeginTest�����ڼ��ͻ��˵ĳ�ʼ�����
    //�����ʼ������OnBeginTest���Ե�����ʾ������Ϣ�ĶԻ���
    virtual TEST_STATUS OnBeginTest(CConnect *pConnect)=0;
    virtual TEST_STATUS OnEndTest(CConnect *pConnect)=0;
    virtual TEST_STATUS ExcuteOneTest(CConnect *pConnect)=0;   
    
    virtual const WCHAR *GetTestObjName()const{return m_strTestName;};   
    virtual unsigned int GetTestedCount()const{return m_uiTestCount;};
    virtual unsigned int GetTestedErrCount()const{return m_uiTestErrCount;} ;
    virtual void    ResetTestCount(){m_uiTestCount=0; m_uiTestErrCount=0; };

    virtual int  GetShouldTestTimes()const=0;
protected:
    unsigned int        m_uiTestCount;
    unsigned int        m_uiTestErrCount;
    WCHAR               m_strTestName[32];                  //�������������
    CTestManager        *m_pManager;                        //���ڱ���������������ָ��
    UINT16              m_uiCmd;                            //������������Ӧ��������
};




#endif