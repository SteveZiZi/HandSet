#ifndef         __SPI_FLASH_TEST_H__
#define         __SPI_FLASH_TEST_H__

#define SPI_FLASH_SIZE  0x400000 //4M
#define SPI_FLASH_TEST_SIZE 0x280000 //2.5M
class CSpiFlashTest:public CTestBase
{
public:
    CSpiFlashTest();
    ~CSpiFlashTest();


    HANDLE  m_hSpiFlashFile;

    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest();
    const SReportNape *GetReportNapes(int &count);

    UINT8   m_uBuf1[SPI_FLASH_SIZE];
    UINT8   m_uBuf2[SPI_FLASH_SIZE];

private:

};


#endif