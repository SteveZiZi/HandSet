#ifndef __TEST_BASE_H__
#define __TEST_BASE_H__

//�������в�����Ŀ�Ļ���CTestBase

typedef enum _TEST_STATUS
{
    TEST_ERROR=0,
    TEST_NORMAL,    
    TEST_CONNECTDOWN
}TEST_STATUS;

class CTestManager ;
class CTestBase
{
public:
    CTestBase():
      m_pTestManager(NULL)
    {
        m_strTestName[0]=0;
    }
    virtual ~CTestBase()
    {
    }

    /*****************************************************************************************
    ** Function name:       Init
    ** Descriptions:        �Բ�������л����ĳ�ʼ����������ȡ�����ļ���ȷ��������ȣ�
                            ���ǣ���Ӳ���ĳ�ʼ���������ExcuteTest������PARAM_GET_INIT_INFO
                            �����Ĵ�����

    ** input parameters:    strIniFile  �����ļ���·��
                            index       �ڱ���������ϱ����������ڵڼ����0��ʼ
                            uipCmdIndex ���ڷ��ر���������ȷ����������    

    ** Returned value:      ����ֵ�����˳�ʼ�������Ƿ�ɹ�
    ***********************************************************************************************/
    virtual BOOL Init(const WCHAR *strIniFile, int index,UINT16  *uipCmdIndex,CTestManager *pManager=NULL)=0;
    virtual const WCHAR *GetTestObjName()const {return m_strTestName;};

    virtual TEST_STATUS OnConnectDown(){return TEST_NORMAL;};
    virtual TEST_STATUS ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,
        const BYTE *pData,CConnect *pConnect)=0;
protected:
    UINT16          m_uiCmd;                    //��������Ҫ���������
    WCHAR           m_strTestName[32];          //�������������
    CTestManager    *m_pTestManager;
};




#endif