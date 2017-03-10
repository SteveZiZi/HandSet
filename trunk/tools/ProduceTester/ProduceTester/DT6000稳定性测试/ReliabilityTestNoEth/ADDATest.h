#ifndef     __AD_DA_TEST_H__
#define     __AD_DA_TEST_H__

class CAdDaTest:public CTestBase
{
public:
    CAdDaTest();
    ~CAdDaTest();

    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest();
    const SReportNape *GetReportNapes(int &count);

private:

    //定义操作AD/DA的函数接口
    BOOL    (*epcAdDaOpenDriver)();                 //打开AD/DA驱动
    void    (*epcAdDaCloseDriver) ();               //关闭AD/DA驱动句柄
    //读取AD采样值
    BOOL (*epcAdGetSampleValue) (DWORD *pValueArray,DWORD dwBeginChannel,DWORD dwEndChannel);
    //设置DA输出值
    BOOL (*epcDASetOutputValue)(DWORD *pValueArray,DWORD dwBeginChannel,DWORD dwEndChannel);

    HINSTANCE       m_hInstAdDADll;                 //加载AD/DA控制dll
    DWORD           m_dwADChannelNum;               //AD的通道数
    DWORD           m_dwDAChannelNum;               //DA的通道数
    DWORD           m_dwMaxValue;                   //AD/AD所能允许的最大值
    DWORD           m_dwTestValue;                  //如果仅存在AD通道，则此处的值表明了所有AD
    //通道采样结果的参考值

    BOOL    LoadAdDaDll();                          //加载AD/DA控制动态库； 

};

#endif