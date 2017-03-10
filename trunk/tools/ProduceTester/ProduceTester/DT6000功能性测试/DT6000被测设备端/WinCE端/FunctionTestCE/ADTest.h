#ifndef     __AD_TEST_H__
#define     __AD_TEST_H__

class  CADTest:public CTestBase
{
public:
    ~CADTest();

    BOOL            Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS     ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
        const BYTE *pData,CConnect *pConnect);
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

    TEST_INFO       m_ti;

    BOOL    LoadAdDaDll();                          //����AD/DA���ƶ�̬��
};



#endif