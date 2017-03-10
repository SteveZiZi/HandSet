#ifndef         __SPI_FLASH_TEST_H__
#define         __SPI_FLASH_TEST_H__

class CSpiFlashTest:public CTestBase
{
public:
    CSpiFlashTest();
    ~CSpiFlashTest();

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
        const BYTE *pData,CConnect *pConnect);
private:
    TEST_INFO       m_ti;

};




#endif