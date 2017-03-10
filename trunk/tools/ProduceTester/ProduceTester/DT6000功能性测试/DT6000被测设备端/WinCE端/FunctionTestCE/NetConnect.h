#ifndef __NET_CONNECT_H
#define __NET_CONNECT_H
#include "TSocket.h"
#include "connect.h"


class CNetConnect:public CConnect
{
public:
    CNetConnect();
    ~CNetConnect();

    BOOL Connect(const WCHAR *strRemoteIP,unsigned int port,const WCHAR *strLocalIP);
    BOOL Connect(const char  *strRemoteIP,unsigned int port,const char  *strLocalIP);

    int SendAPacket(const PACKET *pPacket);
    int SendAPacket(UINT16 uiCmd,UINT16 param,UINT16 uiDatalen,const BYTE *pData);
    int RecvAPacket(PACKET *pPacket,unsigned int uiTimeOut=0);
    int RecvAPacket(UINT16 &index,UINT16 &param,UINT16 &uiDatalen,BYTE *pData,
        unsigned int uiTimeOut=0);
    const WCHAR *GetLocalID();
    void Clear();

    int TestConnect();
    friend class CNetServer;

private:
    TSocket     m_comSocket;                    //用于通信的套接字
    WCHAR       m_strLocalAddr[32];             //保存了连接对应的IP地址的字符串
};

class CNetServer:public CServer
{
public:
    CNetServer();
    virtual ~CNetServer();

    BOOL BuildServer(const WCHAR *strLocalIP,int port,int iListenNum);
    BOOL BuildServer(const char  *strLocalIP,int port,int iListenNum);
    BOOL Accept(CConnect *pConnect);
    void CloseServer();

private:
    TSocket m_serverSocket;
};




#endif