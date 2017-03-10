#ifndef __ETHERNET_TEST_H__
#define __ETHERNET_TEST_H__
#include "testBase.h"
#include "TSocket.h"
#include "testCommon.h"


class CEthernetTest:public CTestBase
{
public:
    CEthernetTest();
    ~CEthernetTest();
    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest();
    const SReportNape *GetReportNapes(int &count);

    const WCHAR *GetTestObjName()const      {return m_strTestName;}; 
    unsigned int GetTestedCount()const      {return m_uiTestCount;};
    unsigned int GetTestedErrCount()const   {return m_uiErrCount; };

private:    
    TSocket     m_clientSocket;    
    BOOL        m_bInitError; 
    static DWORD ConnectProc(void *pVoid);    
    //开辟用作缓冲区
    BYTE        m_pBuf[ETH_TEST_PACKET_LEN];    
    WCHAR       m_wszEthName[48];                   //网卡的名称
    BOOL    FindNetCard(WCHAR *wszIP);              //找到指定网卡的IP地址

};





#endif