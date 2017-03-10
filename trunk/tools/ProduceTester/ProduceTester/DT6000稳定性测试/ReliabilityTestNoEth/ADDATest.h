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

    //�������AD/DA�ĺ����ӿ�
    BOOL    (*epcAdDaOpenDriver)();                 //��AD/DA����
    void    (*epcAdDaCloseDriver) ();               //�ر�AD/DA�������
    //��ȡAD����ֵ
    BOOL (*epcAdGetSampleValue) (DWORD *pValueArray,DWORD dwBeginChannel,DWORD dwEndChannel);
    //����DA���ֵ
    BOOL (*epcDASetOutputValue)(DWORD *pValueArray,DWORD dwBeginChannel,DWORD dwEndChannel);

    HINSTANCE       m_hInstAdDADll;                 //����AD/DA����dll
    DWORD           m_dwADChannelNum;               //AD��ͨ����
    DWORD           m_dwDAChannelNum;               //DA��ͨ����
    DWORD           m_dwMaxValue;                   //AD/AD������������ֵ
    DWORD           m_dwTestValue;                  //���������ADͨ������˴���ֵ����������AD
    //ͨ����������Ĳο�ֵ

    BOOL    LoadAdDaDll();                          //����AD/DA���ƶ�̬�⣻ 

};

#endif