#ifndef __TSOCKET_H
#define __TSOCKET_H
#include <WinSock2.h>
#include <assert.h>

/****************************************Copyright (c)**************************************************
**					
**
**--------------File Info-------------------------------------------------------------------------------
** File Name:				TSocket
** Last modified Date: 		2010.11.15
** Last Version:			V1.0
** Description: 			声明了为方便tcpIp网络套接字编程而封装的类库TSocket，以及一些扩展函数
**
**
**------------------------------------------------------------------------------------------------------
** Created By: 				 chao
** Created date: 			2010.11.15
** Version: 				V1.0
** Descriptions:			The original version 初始版本
**
**
********************************************************************************************************/


/*********************************************************************************************************
** Class name:      TSocket
**
** Descriptions:    为方便tcp套接字编程的一个封装类库
                    所有的函数如果没有特殊声明，都是阻塞的，也就是直到函数执行完成或超时才会返回
**                  函数中要求以32位无符号整数表示的IP地址.这是按照网络顺序字节排列的4字节IP地址
**                  就像inet_addr函数的返回值那样。
**
**                  本类库仅支持IPv4类型的网络。
*********************************************************************************************************/

class TSocket
{
public:
    TSocket();
    ~TSocket();

    TSocket(TSocket &s);

    //建立一个tcp类型的套接字,在建立服务器端，或连接到一个服务器之前，这是必须调用的函数
    BOOL    socketTcp();
    //建立一个tcp服务器，必须要先建立一个tcp类型的套接字
    BOOL    BuildTcpSever(unsigned int nPort,int ilistenNum=1);
    BOOL    BuildTcpSever(UINT32 LocalIP,unsigned int nPort,int ilistenNum=1);
    BOOL    BuildTcpSever(const WCHAR *strLocalIP,unsigned int nPort,int iListenNum=1);
    BOOL    BuildTcpSever(const char *strLocalIP,unsigned int nPort,int iListenNum=1);
    BOOL    BuildTcpSever(const struct sockaddr * name,int namelen,int iListenNum=1);


    //接受一个客户端的链接，如果成功秘诀则pSocket所指向的对象代表了新建的链接套接字，
    //在执行这个方法之前pSocket所指向的对象不需要也不应该调用socketTcp方法来建立一个套接字
    BOOL    Accept(TSocket *pSocket,struct sockaddr *addr=NULL,int *addrlen=NULL);    


    //连接到指定的服务器，必须先建立一个tcp类型的套接字    
    BOOL    Connect(UINT32  FarIP,unsigned int nPort,UINT32 LocalIp=0);
    BOOL    Connect(const WCHAR   *strFarIP,unsigned int nPort,const WCHAR *strLocalIp=NULL);
    BOOL    Connect(const char    *strFarIP,unsigned int nPor, const char *strLocalIp=NULL);
    BOOL    Connect(struct sockaddr *farAddr,int iFraAddrLen, struct sockaddr* LocalAddr=NULL,int iLocalAddrLen=0);

    BOOL    Attach(SOCKET s);
    SOCKET  Detch();   
    //
    int     Send(BYTE *pBuf,int len,int flags);
    int     Recv(BYTE *pBuf,int len,int flags);
    
    /*********************************************************************************************************
    ** Function name:   RecvNBytes,RecvTimeout,RecvNBytesTimeOut
    **
    ** Descriptions:    关于套接口接收函数的一组扩展方法，每一个函数均为阻塞函数.
    **                  RecvNBytes:接收指定数目的字节数(不会多接收)，知道接收到指定数目字节数或者出错才返回
    **                  RecvTimeout:尝试从套接口接收数据，但如果指定时间内还没有数据到来或出错时，则返回
    **                  RecvNBytesTimeOut: RecvNBytes函数的超时版本
    **
    ** 总体参数说明:    pBuf:   接收数据的缓冲区
    **                  len:    缓冲区的长度
    **                  N:      要求接收的字节数
    **                  uiTimeOut:超时参数，以毫秒为单位
    **                  iRecvLen:指向一个整形变量，用于保存实际上接收到的字节数
    ** 
    ** Returned value:  返回值大于0表示成功执行了函数所要进行的操作
    **                  返回值为0表示执行超时
    **                  返回值小于0表示网络发生了错误，典型的情况是链接已经断开
    **                  
    **                  即使是执行超时和网络发生错误的情况下，函数也可能接收了一些数据，你应该检查iRecvLen所指
    **                  向的单元来确定接收了多少字节
    *********************************************************************************************************/
    int     RecvNBytes(BYTE *pBuf,int N,int *iRecvLen=NULL);
    int     RecvTimeout(BYTE *pBuf,int len,unsigned int uiTimeOut,int *iRecvLen=NULL);
    int     RecvNBytesTimeOut(BYTE *pBuf,int N,unsigned int uiTimeOut,int *iRecvLen=NULL);


    /*********************************************************************************************************
    ** Function name:   sendNBytes
    **
    ** Descriptions:    尝试发送N个字节的数据，知道出错
    **
    ** 总体参数说明:    pBuf:   接收数据的缓冲区
    **                  len:    缓冲区的长度
    **                  N:      要求接收的字节数
    **                  uiTimeOut:超时参数，以毫秒为单位
    **                  iRecvLen:指向一个整形变量，用于保存实际上接收到的字节数
    ** 
    ** Returned value:  返回值大于0表示成功执行了函数所要进行的操作
    **                  返回值为0表示执行超时
    **                  返回值小于0表示网络发生了错误，典型的情况是链接已经断开
    **
    **                  即使是执行超时和网络发生错误的情况下，函数也可能发送了一些数据，你应该检查iSendLen所指
    **                  向的单元来确定接收了多少字节
    *********************************************************************************************************/
    int     sendNBytes(BYTE *pBuf,int N,int *iSendLen=NULL);

    int setsockopt(int level,int optname,const char FAR* optval,int optlen);
    //当前对象是否包含一个有效的套接字对象
    BOOL    IsValid()
    {
        return m_socket!=INVALID_SOCKET?TRUE:FALSE;
    }

    int getpeername(struct sockaddr* name,int* namelen);
    int getsockname(struct sockaddr* name,int* namelen);




    //关闭套接字
    void    CloseSocket();
       

private:
    SOCKET m_socket;
};


UINT32  inet_addrW(const WCHAR *cp);
int     GetLocalIps(struct in_addr *pAddrList,int iNum);
BOOL    Ping(LPCWSTR lpDestIP, DWORD dwTimeOut, DWORD dwTimes);
#endif