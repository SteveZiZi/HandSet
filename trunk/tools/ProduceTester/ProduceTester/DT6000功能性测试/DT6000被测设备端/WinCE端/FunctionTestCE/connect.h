#ifndef  __CONNECT__H
#define  __CONNECT__H
#include <windows.h>
#include "testCommon.h"

class CConnect
{
public:
    virtual ~CConnect()
    {

    }  
    virtual int SendAPacket(const PACKET *pPacket)=0;
    virtual int SendAPacket(UINT16 uiCmd,UINT16 param,UINT16 uiDatalen,const BYTE *pData)=0;
    virtual int RecvAPacket(PACKET *pPacket,unsigned int uiTimeOut=0)=0;
    virtual int RecvAPacket(UINT16 &uiCmd,UINT16 &param,UINT16 &uiDatalen,BYTE *pData,
        unsigned int uiTimeOut=0)=0;    
    virtual int TestConnect()=0;
    virtual const WCHAR *GetLocalID()=0;

    virtual void Clear()=0;     //清理连接资源，断开已经连接的连接
};

class CServer
{
public:
    virtual ~CServer()
    {

    }
    virtual BOOL Accept(CConnect *pConnect)=0;
    virtual void CloseServer()=0;
};

#endif