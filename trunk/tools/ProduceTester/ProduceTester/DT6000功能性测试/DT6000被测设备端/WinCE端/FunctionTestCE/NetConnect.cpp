#include "stdafx.h"
#include "NetConnect.h"
#include "assertMsg.h"
#include "testCommon.h"
#include <string>
using   namespace std;


CNetConnect::CNetConnect()
{
    m_strLocalAddr[0]=0;
}

CNetConnect::~CNetConnect()
{

}

BOOL CNetConnect::Connect(const WCHAR *strRemoteIP,
                          unsigned int port,const WCHAR *strLocalIP)
{
    char  *pRemote=NULL,*pLocal=NULL;
    int   iLenRemote=(int)wcslen(strRemoteIP);
    int   iLocalLen=0;

    iLocalLen=strLocalIP==NULL?0:(int)wcslen(strLocalIP);
    if (iLenRemote <=0)
    {
        return FALSE;
    }

    int   iCharLenRemote=0,iCharLenLocal=0;
    iCharLenRemote=WideCharToMultiByte(CP_ACP,0,strRemoteIP,iLenRemote,
        NULL,0,NULL,FALSE);
    pRemote=new char [iCharLenRemote+10];
    if (pRemote ==NULL)
    {
        goto EXCUTE_FAILED;
    }
    iCharLenRemote=WideCharToMultiByte(CP_ACP,0,strRemoteIP,iLenRemote,
        pRemote,iCharLenRemote+1,NULL,FALSE);
    pRemote[iCharLenRemote]=0;

    if (iLocalLen >0)
    {
        iCharLenLocal=WideCharToMultiByte(CP_ACP,0,strLocalIP,iLocalLen,
            NULL,0,NULL,FALSE);
        pLocal=new char[iCharLenLocal+10];
        iCharLenLocal=WideCharToMultiByte(CP_ACP,0,strLocalIP,iLocalLen,
            pLocal,iCharLenLocal+1,NULL,FALSE);

        pLocal[iCharLenLocal]=0;
    }

    BOOL bRet=Connect(pRemote,port,pLocal); 

    if (pRemote !=NULL)
    {
        delete []pRemote;
    }
    if (pLocal !=NULL)
    {
        delete []pLocal;
    }
    return bRet;
EXCUTE_FAILED:
    if (pRemote !=NULL)
    {
        delete []pRemote;
    }
    if (pLocal !=NULL)
    {
        delete []pLocal;
    }
    return FALSE;
}

BOOL CNetConnect::Connect(const char *strRemoteIP,unsigned int port,const char *strLocalIP)
{
    m_comSocket.CloseSocket();
    m_comSocket.socketTcp();
    return m_comSocket.Connect(strRemoteIP,port,strLocalIP);
}



int CNetConnect::SendAPacket(const PACKET *pPacket)
{
    assertMsg(pPacket!=NULL,L"传递的pPacket参数不能为空");
    return m_comSocket.sendNBytes((BYTE *)pPacket,sizeof(PACKET));    
}

int CNetConnect::SendAPacket(UINT16 uiCmd,UINT16 param,UINT16 uiDatalen,const BYTE *pData)
{   
    PACKET pa;  

    pa.uiCmd=uiCmd;
    pa.param=param;
    pa.uiDatalen=uiDatalen;
    memcpy(pa.pData,pData,uiDatalen);
    return m_comSocket.sendNBytes((BYTE *)&pa,sizeof(pa) );
}

int CNetConnect::RecvAPacket(UINT16 &uiCmd,UINT16 &param,UINT16 &uiDatalen,BYTE *pData, unsigned int uiTimeOut/* =0 */)
{
    PACKET pa;
    int nRet;
RETRY:
    if (uiTimeOut >0)
    {
        nRet=m_comSocket.RecvNBytesTimeOut((BYTE *)&pa,sizeof(pa),uiTimeOut);
    }
    else
    {
        nRet=m_comSocket.RecvNBytes((BYTE *)&pa,sizeof(pa));
    }

    if (nRet >0)
    {
        if (pa.uiCmd==CMD_TEST_CONNECT)
        {
            goto RETRY;
        }
        uiCmd=pa.uiCmd;
        param=pa.param;
        uiDatalen=pa.uiDatalen;
        memcpy(pData,pa.pData,uiDatalen);
    }
       
    return nRet;
}


int CNetConnect::RecvAPacket(PACKET *pPacket,unsigned int uiTimeOut/* =0 */)
{
    assertMsg(pPacket!=NULL,L"指向PACKET的指针不能为空!" );
    int nRet;

RETRY:
    if (uiTimeOut >0)
    {
        nRet= m_comSocket.RecvNBytesTimeOut((BYTE *)pPacket,sizeof(PACKET),uiTimeOut);
    }
    else
    {
        nRet=m_comSocket.RecvNBytes((BYTE *)pPacket,sizeof(PACKET) );
    }

   if (nRet >0)
   {
       //检测是否收到测试连接状况的包
       if (pPacket->uiCmd ==CMD_TEST_CONNECT)
       {
           goto RETRY;
       }
   }
    return nRet;
}

int CNetConnect::TestConnect()
{
    PACKET pa;
    pa.uiCmd=CMD_TEST_CONNECT;
    return SendAPacket(&pa);
}
 
const WCHAR *CNetConnect::GetLocalID()
{
    return m_strLocalAddr;
}

void CNetConnect::Clear()
{
    m_comSocket.CloseSocket();
}



CNetServer::CNetServer()
{
}

CNetServer::~CNetServer()
{

}

BOOL CNetServer::BuildServer(const char *strLocalIP,int port,int iListenNum)
{
    m_serverSocket.CloseSocket();
    m_serverSocket.socketTcp();    
    return m_serverSocket.BuildTcpSever(strLocalIP,port,iListenNum);
}

BOOL CNetServer::BuildServer(const WCHAR *strLocalIP,int port,int iListenNum)
{
    m_serverSocket.CloseSocket();
    m_serverSocket.socketTcp();    
    return m_serverSocket.BuildTcpSever(strLocalIP,port,iListenNum);
}

BOOL CNetServer::Accept(CConnect *pConnect)
{
    CNetConnect *p=(CNetConnect *)pConnect;
    assertMsg(pConnect!=NULL,L"Accept的参数不能为空");
    assertMsg(!(p->m_comSocket.IsValid()),L"对一个已经拥有连接的connect对象使用此操作"
        L"可能会导致资源泄漏" );
    return m_serverSocket.Accept(&p->m_comSocket);
}

void CNetServer::CloseServer()
{
    m_serverSocket.CloseSocket();
}
