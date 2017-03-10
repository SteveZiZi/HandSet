#ifndef __TEST_BASE_H__
#define __TEST_BASE_H__

//定义所有测试项目的基类CTestBase

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
    ** Descriptions:        对测试项进行基础的初始化，包括读取配置文件，确定命令码等，
                            但是，对硬件的初始化，请放在ExcuteTest函数对PARAM_GET_INIT_INFO
                            参数的处理上

    ** input parameters:    strIniFile  配置文件的路径
                            index       在本测试类别上本测试项属于第几项，以0开始
                            uipCmdIndex 用于返回本测试项所确定的命令码    

    ** Returned value:      返回值表面了初始化测试是否成功
    ***********************************************************************************************/
    virtual BOOL Init(const WCHAR *strIniFile, int index,UINT16  *uipCmdIndex,CTestManager *pManager=NULL)=0;
    virtual const WCHAR *GetTestObjName()const {return m_strTestName;};

    virtual TEST_STATUS OnConnectDown(){return TEST_NORMAL;};
    virtual TEST_STATUS ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,
        const BYTE *pData,CConnect *pConnect)=0;
protected:
    UINT16          m_uiCmd;                    //本测试项要求的命令码
    WCHAR           m_strTestName[32];          //本测试项的名字
    CTestManager    *m_pTestManager;
};




#endif