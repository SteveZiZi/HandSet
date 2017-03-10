#include "stdafx.h"
#include "connect.h"
#include "ActiveSyncConnect.h"
#include "FuncTestActiveConnectCE.h"


CActiveSyncConnect::CActiveSyncConnect()
{

}

CActiveSyncConnect::~CActiveSyncConnect()
{
    Clear();
}

BOOL CActiveSyncConnect::Connect()
{
    return CEConnect();
}

void CActiveSyncConnect::Clear()
{
    CEDisConnect();
}

int CActiveSyncConnect::TestConnect()
{
    return 1;
}

int CActiveSyncConnect::SendAPacket(const PACKET *pPacket)
{
    int nRet = CEWrite((void *)pPacket,sizeof(PACKET),0);
    return nRet;
}

int CActiveSyncConnect::SendAPacket(UINT16 uiCmd,UINT16 param,UINT16 uiDatalen,const BYTE *pData)
{
    PACKET pa;  

    pa.uiCmd=uiCmd;
    pa.param=param;
    pa.uiDatalen=uiDatalen;
    memcpy(pa.pData,pData,uiDatalen);
    return SendAPacket(&pa);
}

int CActiveSyncConnect::RecvAPacket(PACKET *pPacket,unsigned int uiTimeOut/* =0 */)
{
    DWORD   dwRead;
    return CERead(pPacket,sizeof(PACKET),&dwRead,uiTimeOut);
}

int CActiveSyncConnect::RecvAPacket(UINT16 &index, UINT16 &param, UINT16 &uiDatalen, 
                                    BYTE *pData, unsigned int uiTimeOut)
{
    PACKET pa;
    BOOL bRet=RecvAPacket(&pa);

    if (bRet)
    {
        index=pa.uiCmd;
        param=pa.param;
        uiDatalen=pa.uiDatalen;
        memcpy(pData,pa.pData,uiDatalen);
    }
    return bRet;
}