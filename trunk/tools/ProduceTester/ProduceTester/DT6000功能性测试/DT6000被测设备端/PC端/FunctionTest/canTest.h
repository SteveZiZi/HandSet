#ifndef      __CAN_TEST_H__
#define      __CAN_TEST_H__
#include "CANLib.h"

class CCANTest:public CTestBase
{
public:
    CCANTest();
    ~CCANTest();

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return 10;};

    TEST_STATUS OnBeginTest(CConnect *pConnect);
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);

private:
    DWORD	m_dwDeviceType;             //ʹ��PCI9820,PCI5121:1,PCI9810:2,USBCAN1:3,USBCAN2:4,PCI9820:5,CAN232:6
    DWORD	m_dwDeviceIndex;            //ʹ�õ��������ӵĵ�һ��CAN�豸����������д�����������ڵ�����ֻʹ��һ��CAN�豸
    DWORD	m_dwCanId;                  //ʹ��ĳ��CAN�豸�ϵĵڼ�·CAN
    DWORD	m_dwExternFlag;             //�Ƿ���չ֡
    DWORD	m_dwRemoteFlag;             //�Ƿ�Զ��֡ 

    VCI_CAN_OBJ     m_vco;             //
};



#endif