#include <windows.h>
#include "ActiveSyncConnect.h"
#include "assertMsg.h"
#include "testCommon.h"
#include <string>
#include "rapi.h"

using namespace std;

CActiveSyncConnect::CActiveSyncConnect()
{

}

CActiveSyncConnect ::~CActiveSyncConnect()
{
    CeRapiUninit();
}

void CActiveSyncConnect::Clear()
{
    CeRapiUninit();
}

BOOL CActiveSyncConnect::Connect()
{
    RAPIINIT ri;
    ri.cbSize = sizeof(ri);
    CeRapiInitEx(&ri);
    DWORD ret = WaitForSingleObject(ri.heRapiInit, 1000);
    //CloseHandle(ri.heRapiInit);
    if((ret != S_OK) || (ri.hrRapiInit != S_OK))
    {
        return FALSE;
    }
    BYTE *pOut =NULL;
    DWORD   dwOut;

    int nRet = CeRapiInvoke(L"\\windows\\FuncTestActiveConnectPC.dll",L"PCConnect",0,NULL,
        &dwOut,&pOut,NULL,0);
    int error = CeGetLastError();

    if (nRet == 0)
    {
        return TRUE;
    }


    return FALSE;
}

int CActiveSyncConnect::TestConnect()
{
    return 1;
}

int CActiveSyncConnect::SendAPacket(const PACKET *pPacket)
{
    DWORD   dwTimeOut = 10000;
    BYTE buf[sizeof(PACKET)+sizeof(DWORD)];
    memcpy(buf,&dwTimeOut,sizeof(DWORD));
    memcpy(buf+sizeof(DWORD),pPacket,sizeof(PACKET));
    BYTE *pOut =NULL;
    DWORD   dwOut;

    int nRet = CeRapiInvoke(L"\\windows\\FuncTestActiveConnectPC.dll",L"PCWrite",sizeof(buf),buf,
        &dwOut,&pOut,NULL,0);
    int error = CeGetLastError();
    
    if (nRet == 0)
    {
        return 1;
    }

    if (CeGetLastError()==ERROR_EXCEPTION_IN_SERVICE )
    {
        return -1;
    }

    if (nRet ==1)
    {
        return 0;
    }
    return -1;
}

int CActiveSyncConnect::SendAPacket(UINT16 uiCmd, UINT16 param, UINT16 uiDatalen, const BYTE *pData)
{
    PACKET  pa;

    pa.uiCmd=uiCmd;
    pa.param=param;
    pa.uiDatalen=uiDatalen;
    memcpy(pa.pData,pData,uiDatalen);

    return SendAPacket(&pa);
}

int CActiveSyncConnect::RecvAPacket(PACKET *pPacket,unsigned int uiTimeOut/* =0 */)
{
    DWORD   dwTimeOut = uiTimeOut;
    DWORD   dwRecv=0;
    BYTE    *pBuf=NULL;

    int nRet = CeRapiInvoke(L"\\windows\\FuncTestActiveConnectPC.dll",L"PCRead",
        sizeof(dwTimeOut),(BYTE *)&dwTimeOut, &dwRecv,&pBuf,NULL,0);
    __try
    {
        if (nRet == 0)
        {
            if (pBuf !=NULL)
            {
                memcpy(pPacket,pBuf,sizeof(PACKET));
                
                return 1;
            }
        }

        if (CeGetLastError()==ERROR_EXCEPTION_IN_SERVICE )
        {
            return -1;
        }

        if (nRet ==1)
        {
            return 0;
        }
        return -1;
    }
    __finally
    {
        if (pBuf !=NULL)
        {
            LocalFree(pBuf);
        }
    }
  
}

int CActiveSyncConnect::RecvAPacket(UINT16 &index,UINT16 &param,UINT16 &uiDatalen,BYTE *pData, unsigned int uiTimeOut/* =0 */)
{
    PACKET      pa;
    int nRet= RecvAPacket(&pa,uiTimeOut);
    if (nRet == 1)
    {
        index = pa.uiCmd;
        param=pa.param;
        uiDatalen = pa.uiDatalen;
        memcpy(pData,pa.pData,pa.uiDatalen);
    }

    return nRet;
}



