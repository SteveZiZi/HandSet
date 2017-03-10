#include "stdafx.h"
#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "TSocket.h"

#pragma comment(lib,"ws2.lib")

UINT32 inet_addrW(const WCHAR *cp)
{
    int i=0;
    UINT32 t[4]={0};
    int tPos=0;

    int     iNumCharNum=0; 

    if(cp==NULL)
        return 0;

    //尝试跳过字符串开头可能的空格
    while(cp[i] ==L' ')
    {
        i++;
    }
    while(cp[i])
    {
        switch(cp[i])
        {
        case L'0':
        case L'1':
        case L'2':
        case L'3':
        case L'4':
        case L'5':
        case L'6':
        case L'7':
        case L'8':
        case L'9': 
            t[tPos]=t[tPos]*10+(cp[i]-L'0');
            if(t[tPos] >255)
            {
                return INADDR_NONE;
            }
            iNumCharNum++;
            break;
        case L'.':
            if (iNumCharNum ==0)
            {
                return INADDR_NONE;
            }
            iNumCharNum=0;
           
            tPos++;
            break;        
        default:
            if(tPos<3 || iNumCharNum ==0)
            {
                return INADDR_NONE;
            }
            goto sucessed;
            break;
        }
        i++;
    }

    
sucessed:
    UINT32 uiReturned;
    
    BYTE *p=(BYTE *)&uiReturned;
    p[0]=t[0];
    p[1]=t[1];
    p[2]=t[2];
    p[3]=t[3];

    return uiReturned;
}

TSocket::TSocket():m_socket(INVALID_SOCKET)
{

}

TSocket::~TSocket()
{
    if (m_socket !=INVALID_SOCKET)
    {
        closesocket(m_socket);
        m_socket=INVALID_SOCKET;
    }
}

BOOL TSocket::socketTcp()
{    
    m_socket=socket(AF_INET,SOCK_STREAM,0);
    if(m_socket ==INVALID_SOCKET)
        return FALSE;
   
    return TRUE;
}

BOOL TSocket::BuildTcpSever(const struct sockaddr * name,int namelen,int iListenNum/* =1 */)
{
    assert(m_socket !=INVALID_SOCKET);
    assert(iListenNum >=1);
    assert(name !=NULL);
    assert(namelen >0);

   
    //绑定到指定地址
    if(bind(m_socket,name,namelen)==SOCKET_ERROR )
    {        
        int p=GetLastError();
        closesocket(m_socket);
        return FALSE;
    }
    
    if (listen(m_socket,iListenNum) ==SOCKET_ERROR)
    {
        closesocket(m_socket);
        return FALSE;
    }    
 
    return TRUE;
} 

BOOL TSocket::BuildTcpSever(const WCHAR *strLocalIP,unsigned int nPort,int iListenNum/* =1 */)
{
    assert(strLocalIP==NULL|| inet_addrW(strLocalIP) !=INADDR_NONE );

    struct  sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(nPort);
    addr.sin_addr.s_addr=strLocalIP==NULL?0:inet_addrW(strLocalIP);

    ZeroMemory(addr.sin_zero,8);

    return BuildTcpSever((struct sockaddr *)&addr,sizeof(struct sockaddr_in),iListenNum);
    
}

BOOL TSocket::BuildTcpSever(const char *strLocalIP,unsigned int nPort,int iListenNum/* =1 */)
{
    assert(strLocalIP==NULL|| inet_addr(strLocalIP) !=INADDR_NONE );
    
    struct  sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(nPort);
    addr.sin_addr.s_addr=strLocalIP==NULL?0:inet_addr(strLocalIP);
    
    ZeroMemory(addr.sin_zero,8);
    
    return BuildTcpSever((struct sockaddr *)&addr,sizeof(struct sockaddr_in),iListenNum);    
}

BOOL TSocket::BuildTcpSever(unsigned int nPort,int ilistenNum/* =1 */)
{
    struct  sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(nPort);
    addr.sin_addr.s_addr=INADDR_ANY;    
    ZeroMemory(addr.sin_zero,8);
    
    return BuildTcpSever((struct sockaddr *)&addr,sizeof(struct sockaddr_in),ilistenNum);
}

BOOL TSocket::BuildTcpSever(UINT32 LocalIP,unsigned int nPort,int iListenNum/* =1 */)
{
    assert(LocalIP !=INADDR_NONE);

    struct  sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(nPort);
    addr.sin_addr.s_addr=LocalIP;
    
    ZeroMemory(addr.sin_zero,8);
    
    return BuildTcpSever((struct sockaddr *)&addr,sizeof(struct sockaddr_in),iListenNum); 
}

BOOL TSocket::Accept(TSocket *pSocket,struct sockaddr *addr/* =NULL */,int *addrlen/* =NULL */)
{
    assert(pSocket->m_socket ==INVALID_SOCKET);   

    struct sockaddr_in ClientAddr;
    int len;
    if (addr ==NULL)
    {
        addr=(struct sockaddr *)&ClientAddr;
        addr->sa_family=AF_INET;  
    }
    
    if (addrlen ==NULL)
    {
        len=sizeof(struct sockaddr_in);
    }  
    else
    {
        len =*addrlen;
    }
  
    
    SOCKET s=accept(m_socket,addr,&len);     
    if(s==INVALID_SOCKET)
    {
        return FALSE;
    }
    if (addrlen !=NULL)
    {
        *addrlen=len;
    }

    pSocket->m_socket=s;
    return TRUE;
}


BOOL TSocket::Connect(struct sockaddr *farAddr,int iFarAddrLen, struct sockaddr* LocalAddr/* =NULL */,int iLocalAddrLen/* =0 */)
{
    assert(m_socket !=INVALID_SOCKET);
    assert( !(LocalAddr!=NULL && iLocalAddrLen<=0) );

//    socket()
    if (LocalAddr !=NULL)
    {        
        if(bind(m_socket,LocalAddr,iLocalAddrLen) ==SOCKET_ERROR)
        {
            return FALSE;
        }

    }

    if (SOCKET_ERROR==connect(m_socket,farAddr,iFarAddrLen ))
    {
        return FALSE;
    }
    
    return TRUE;
}

BOOL TSocket::Connect(const char *strFarIP,unsigned int nPort,const char *strLocalIp/* =NULL */)
{
    assert(m_socket !=INVALID_SOCKET); 

    struct sockaddr_in RemoteAddr;
    if (strLocalIp !=NULL)
    {

        struct sockaddr_in LocalAddr;

        assert(inet_addr(strLocalIp) !=INADDR_NONE);
        LocalAddr.sin_family=AF_INET;
        LocalAddr.sin_port=0;
        LocalAddr.sin_addr.s_addr=inet_addr(strLocalIp);
        ZeroMemory(LocalAddr.sin_zero,8);
        if(SOCKET_ERROR ==bind(m_socket,(struct sockaddr *)&LocalAddr,sizeof(struct sockaddr_in)) )
        {
            return FALSE;
        }
    }

    RemoteAddr.sin_family=AF_INET;
    RemoteAddr.sin_port=htons(nPort);
    RemoteAddr.sin_addr.s_addr=inet_addr(strFarIP);
    ZeroMemory(RemoteAddr.sin_zero,8);
    
    if ( SOCKET_ERROR ==connect(m_socket,(struct sockaddr *)&RemoteAddr,sizeof(struct sockaddr_in) ) )
    {
        return FALSE;
    }
    
    return TRUE;
}

BOOL TSocket::Connect(const WCHAR *strFarIP,unsigned int nPort,const WCHAR *strLocalIp/* =NULL */)
{
    assert(m_socket !=INVALID_SOCKET);  

    struct sockaddr_in RemoteAddr;
    if (strLocalIp !=NULL)
    {
        struct sockaddr_in LocalAddr;

        assert(inet_addrW(strLocalIp) !=INADDR_NONE);
        LocalAddr.sin_family=AF_INET;
        LocalAddr.sin_port=0;
        LocalAddr.sin_addr.s_addr=inet_addrW(strLocalIp);
        ZeroMemory(LocalAddr.sin_zero,8);
        
        if(SOCKET_ERROR ==bind(m_socket,(struct sockaddr *)&LocalAddr,sizeof(struct sockaddr_in)) )
        {
            return FALSE;
        }
    }
    
    RemoteAddr.sin_family=AF_INET;
    RemoteAddr.sin_port=htons(nPort);
    RemoteAddr.sin_addr.s_addr=inet_addrW(strFarIP);
    ZeroMemory(RemoteAddr.sin_zero,8);
    
    if ( SOCKET_ERROR ==connect(m_socket,(struct sockaddr *)&RemoteAddr,sizeof(struct sockaddr_in) ) )
    {
        return FALSE;
    }
    
    return TRUE;
}

BOOL TSocket::Connect(UINT32 FarIP,unsigned int nPort,UINT32 LocalIp/* =0 */)
{
    assert(FarIP !=INADDR_NONE);
    assert(LocalIp !=INADDR_NONE);
    assert(m_socket !=INVALID_SOCKET);

    struct sockaddr_in RemoteAddr,LocalAddr;
   
    LocalAddr.sin_family=AF_INET;
    LocalAddr.sin_port=0;
    LocalAddr.sin_addr.s_addr=LocalIp;
    ZeroMemory(LocalAddr.sin_zero,8);

    if(SOCKET_ERROR ==bind(m_socket,(struct sockaddr *)&LocalAddr,sizeof(struct sockaddr_in)) )
    {
        return FALSE;
    }
    
    RemoteAddr.sin_family=AF_INET;
    RemoteAddr.sin_port=htons(nPort);
    RemoteAddr.sin_addr.s_addr=FarIP;
    ZeroMemory(RemoteAddr.sin_zero,8);

    if ( SOCKET_ERROR ==connect(m_socket,(struct sockaddr *)&RemoteAddr,sizeof(struct sockaddr_in) ) )
    {        
        return FALSE;
    }

    return TRUE;
}


int TSocket::Send(BYTE *pBuf,int len,int flags)
{
    return ::send(m_socket,(char *)pBuf,len,flags);
}

int TSocket::Recv(BYTE *pBuf,int len,int flags)
{
    return recv(m_socket,(char *)pBuf,len,flags);
}

int TSocket::RecvNBytes(BYTE *pBuf,int N,int *iRecvLen/* =NULL */)
{
    assert(pBuf !=0);
    assert(N >0);

    int len=0;
    int t;

    do 
    {
        t=recv(m_socket,(char *)(pBuf+len),N-len,0);
        if (t<=0)
        {
            break;
        }
        len+=t;
    } while (len<N);

    if (iRecvLen !=NULL)
    {
        *iRecvLen=len;
    }

    if (len<N)
    {
        return -1;
    }

    return 1;
}

int TSocket::RecvTimeout(BYTE *pBuf,int len,unsigned int uiTimeOut,int *iRecvLen/* =NULL */)
{
    assert(uiTimeOut >0);
    assert(pBuf !=0);
    assert(len >0);


    FD_SET fd;
    struct timeval timeout;

    timeout.tv_sec=uiTimeOut/1000;
    timeout.tv_usec=uiTimeOut-timeout.tv_sec*1000;

    FD_ZERO(&fd);
    FD_SET(m_socket,&fd);
    
    int ret=select(1,&fd,NULL,NULL,&timeout);
    if(ret ==0)
    {
        return 0;
    }
    if (ret<0)
    {
        return -1;
    }


    int lenRecv=recv(m_socket,(char *)pBuf,len,0);
    if(lenRecv <=0)
    {
        return -1;
    }

    if(iRecvLen )
    {
        *iRecvLen=lenRecv;
    }

    return 0;
}

int TSocket::RecvNBytesTimeOut(BYTE *pBuf,int N,unsigned int uiTimeOut,int *iRecvLen/* =NULL */)
{
    assert(uiTimeOut >0);
    assert(pBuf !=0);
    assert(N >0);

    int totalLen=0;
    int t;

    FD_SET fd;
    struct timeval timeout;
    timeout.tv_sec=uiTimeOut/1000;
    timeout.tv_usec=uiTimeOut-timeout.tv_sec*1000;

    do 
    {
        FD_ZERO(&fd);
        FD_SET(m_socket,&fd);
        
        int nRet=select(1,&fd,NULL,NULL,&timeout);
        if(nRet <0)
        {
            goto recv_error;
        }
        if (nRet==0)
        {
            goto recv_TimeOut;
        }

        t=recv(m_socket,(char *)(pBuf+totalLen),N-totalLen,0);        
        if (t<=0)
        {
            goto recv_error;
        }
        totalLen+=t;
    } while (totalLen <N);

    if(iRecvLen !=NULL)
    {
        *iRecvLen=totalLen;
    }
    
    return 1;

recv_error:
    if(iRecvLen !=NULL)
    {
        *iRecvLen=totalLen;
    }
    int error =WSAGetLastError();
    return -1;
recv_TimeOut:
    if (iRecvLen !=NULL)
    {
        *iRecvLen =totalLen;
    }
    return 0;
}

int TSocket::sendNBytes(BYTE *pBuf,int N,int *iSendLen)
{   
    assert(N>0);
    assert(pBuf !=NULL);


    int len=0;
    int t;

    do 
    {
        t=send(m_socket,(char *)(pBuf+len),N-len,0);
        if (t<=0)
        {
            if(iSendLen !=NULL)
            {
                *iSendLen=len;
            }
            return -1;
        }

        len+=t;
    } while (len <N);

    if(iSendLen !=NULL)
    {
        *iSendLen=len;
    }
    return 1;
}


void TSocket::CloseSocket()
{
    if (m_socket !=INVALID_SOCKET)
    {
        shutdown(m_socket,SD_BOTH);
        closesocket(m_socket);
        m_socket=INVALID_SOCKET;
    }

}
BOOL    TSocket::Attach(SOCKET s)
{
    CloseSocket();
    m_socket=s;
    if (s==INVALID_SOCKET)
    {
        return FALSE;
    }
    return TRUE;
}
SOCKET  TSocket::Detch()
{
    SOCKET s=m_socket;
    m_socket=INVALID_SOCKET;

    return s;
}

int TSocket::getpeername(struct sockaddr* name,int* namelen)
{
    return ::getpeername(m_socket,name,namelen);
}

int TSocket::setsockopt(int level,int optname,const char FAR* optval,int optlen)
{
    return ::setsockopt(m_socket,level,optname,optval,optlen);
}

int TSocket::getsockname(struct sockaddr* name,int* namelen)
{
    return ::getsockname(m_socket,name,namelen);
}



int GetLocalIps(struct in_addr *pAddrList,int iNum)
{
    char localHostName[MAX_PATH];
    gethostname(localHostName,MAX_PATH);
    struct hostent *pHostent=gethostbyname(localHostName);
    if (pHostent==NULL){
        return 0;
    }
    //校验所获取的地址是否为IPV4地址
    if (pHostent->h_addrtype!=AF_INET)
    {
        return 0;
    }
    int nActNum=0;           //本机实际的IP地址个数
    while(pHostent->h_addr_list[nActNum]!=NULL)
    {
        nActNum++;
    }

    //检验pAddrList指针的有效性
    if (pAddrList ==NULL  || iNum<=0)
    {
        return nActNum;
    }

    int t=iNum>=nActNum? nActNum:iNum;

    for (int i=0;i<t;i++)
    {
        memcpy(pAddrList+i,pHostent->h_addr_list[i],sizeof(struct in_addr) );
        char *s=inet_ntoa(*pAddrList);
            
    }

    return t;
}

BOOL Ping(LPCWSTR lpDestIP, DWORD dwTimeOut, DWORD dwTimes)
{
    TCHAR                szCmd[50];
    PROCESS_INFORMATION    pi;
    STARTUPINFO            si = {sizeof(STARTUPINFO)};
    BOOL                bRet;
    DWORD                dwExitCode = (DWORD)-1;


    swprintf(szCmd, L"ping %s -n %d -w %d", lpDestIP, dwTimes, dwTimeOut);
    bRet = CreateProcess(NULL, szCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
    if (!bRet) return FALSE;
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &dwExitCode);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return (dwExitCode == 0);
}