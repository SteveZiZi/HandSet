#include "StdAfx.h"
#include "UsuallyCommon.h"
#include "Ft3Tx.h"


#define FT3_GUOWANG_PATH     L"pcap\\GuoWang.txt"

CFt3Tx::CFt3Tx(void)
{
}

CFt3Tx::CFt3Tx( HANDLE hDevice, UINT16 uDeviceItem )
{
    m_pFt3 = new _FT3_c(hDevice, uDeviceItem);
    if(m_pFt3->Init( L"DSM_FT3_TX1", L"ShareFT3_TX1", FT3_TX_MAP_FILE_LEN) == FALSE) {
        RETAILMSG(1, (_T("APP: m_Ft3Tx.Init failed\r\n")));
    }
    //获取可执行文件所在的路径
    int iDirLen =GetExeDirectory(m_strFt3FilePath,MAX_PATH*2);
    //获得配置文件所在的路径
    wcscpy(m_strFt3FilePath + iDirLen,FT3_GUOWANG_PATH);
}

CFt3Tx::~CFt3Tx(void)
{
    if(m_pFt3 != NULL)
    {
        delete m_pFt3;
    }
}

bool CFt3Tx::Ft3Send()
{
    m_tCtrlW.wData = 0;

    m_tCtrlW.CtrlBit.bBaudRate = 1;

    m_tCtrlW.CtrlBit.bFrameStyle = _GUO_WANG;

    m_tCtrlW.CtrlBit.bSampleRate = 1;

    m_pFt3->SetFilePath(m_strFt3FilePath);
    m_pFt3->SetFT3TxInfo(m_tCtrlW);
    m_pFt3->SetFt3TxFrame();
    return m_pFt3->Ft3Tx();
}

bool CFt3Tx::Ft3Stop()
{
    return m_pFt3->Ft3StopTx();
}
