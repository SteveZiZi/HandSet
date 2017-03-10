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
    DWORD	m_dwDeviceType;             //使用PCI9820,PCI5121:1,PCI9810:2,USBCAN1:3,USBCAN2:4,PCI9820:5,CAN232:6
    DWORD	m_dwDeviceIndex;            //使用电脑上连接的第一个CAN设备，这个好像有待考究，最好在电脑上只使用一个CAN设备
    DWORD	m_dwCanId;                  //使用某个CAN设备上的第几路CAN
    DWORD	m_dwExternFlag;             //是否扩展帧
    DWORD	m_dwRemoteFlag;             //是否远程帧 

    VCI_CAN_OBJ     m_vco;             //
};



#endif