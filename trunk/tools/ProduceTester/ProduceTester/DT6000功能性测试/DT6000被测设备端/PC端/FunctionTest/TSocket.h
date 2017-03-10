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
** Description: 			������Ϊ����tcpIp�����׽��ֱ�̶���װ�����TSocket���Լ�һЩ��չ����
**
**
**------------------------------------------------------------------------------------------------------
** Created By: 				 chao
** Created date: 			2010.11.15
** Version: 				V1.0
** Descriptions:			The original version ��ʼ�汾
**
**
********************************************************************************************************/


/*********************************************************************************************************
** Class name:      TSocket
**
** Descriptions:    Ϊ����tcp�׽��ֱ�̵�һ����װ���
                    ���еĺ������û���������������������ģ�Ҳ����ֱ������ִ����ɻ�ʱ�Ż᷵��
**                  ������Ҫ����32λ�޷���������ʾ��IP��ַ.���ǰ�������˳���ֽ����е�4�ֽ�IP��ַ
**                  ����inet_addr�����ķ���ֵ������
**
**                  ������֧��IPv4���͵����硣
*********************************************************************************************************/

class TSocket
{
public:
    TSocket();
    ~TSocket();

    TSocket(TSocket &s);

    //����һ��tcp���͵��׽���,�ڽ����������ˣ������ӵ�һ��������֮ǰ�����Ǳ�����õĺ���
    BOOL    socketTcp();
    //����һ��tcp������������Ҫ�Ƚ���һ��tcp���͵��׽���
    BOOL    BuildTcpSever(unsigned int nPort,int ilistenNum=1);
    BOOL    BuildTcpSever(UINT32 LocalIP,unsigned int nPort,int ilistenNum=1);
    BOOL    BuildTcpSever(const WCHAR *strLocalIP,unsigned int nPort,int iListenNum=1);
    BOOL    BuildTcpSever(const char *strLocalIP,unsigned int nPort,int iListenNum=1);
    BOOL    BuildTcpSever(const struct sockaddr * name,int namelen,int iListenNum=1);


    //����һ���ͻ��˵����ӣ�����ɹ��ؾ���pSocket��ָ��Ķ���������½��������׽��֣�
    //��ִ���������֮ǰpSocket��ָ��Ķ�����ҪҲ��Ӧ�õ���socketTcp����������һ���׽���
    BOOL    Accept(TSocket *pSocket,struct sockaddr *addr=NULL,int *addrlen=NULL);    


    //���ӵ�ָ���ķ������������Ƚ���һ��tcp���͵��׽���    
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
    ** Descriptions:    �����׽ӿڽ��պ�����һ����չ������ÿһ��������Ϊ��������.
    **                  RecvNBytes:����ָ����Ŀ���ֽ���(��������)��֪�����յ�ָ����Ŀ�ֽ������߳���ŷ���
    **                  RecvTimeout:���Դ��׽ӿڽ������ݣ������ָ��ʱ���ڻ�û�����ݵ��������ʱ���򷵻�
    **                  RecvNBytesTimeOut: RecvNBytes�����ĳ�ʱ�汾
    **
    ** �������˵��:    pBuf:   �������ݵĻ�����
    **                  len:    �������ĳ���
    **                  N:      Ҫ����յ��ֽ���
    **                  uiTimeOut:��ʱ�������Ժ���Ϊ��λ
    **                  iRecvLen:ָ��һ�����α��������ڱ���ʵ���Ͻ��յ����ֽ���
    ** 
    ** Returned value:  ����ֵ����0��ʾ�ɹ�ִ���˺�����Ҫ���еĲ���
    **                  ����ֵΪ0��ʾִ�г�ʱ
    **                  ����ֵС��0��ʾ���緢���˴��󣬵��͵�����������Ѿ��Ͽ�
    **                  
    **                  ��ʹ��ִ�г�ʱ�����緢�����������£�����Ҳ���ܽ�����һЩ���ݣ���Ӧ�ü��iRecvLen��ָ
    **                  ��ĵ�Ԫ��ȷ�������˶����ֽ�
    *********************************************************************************************************/
    int     RecvNBytes(BYTE *pBuf,int N,int *iRecvLen=NULL);
    int     RecvTimeout(BYTE *pBuf,int len,unsigned int uiTimeOut,int *iRecvLen=NULL);
    int     RecvNBytesTimeOut(BYTE *pBuf,int N,unsigned int uiTimeOut,int *iRecvLen=NULL);


    /*********************************************************************************************************
    ** Function name:   sendNBytes
    **
    ** Descriptions:    ���Է���N���ֽڵ����ݣ�֪������
    **
    ** �������˵��:    pBuf:   �������ݵĻ�����
    **                  len:    �������ĳ���
    **                  N:      Ҫ����յ��ֽ���
    **                  uiTimeOut:��ʱ�������Ժ���Ϊ��λ
    **                  iRecvLen:ָ��һ�����α��������ڱ���ʵ���Ͻ��յ����ֽ���
    ** 
    ** Returned value:  ����ֵ����0��ʾ�ɹ�ִ���˺�����Ҫ���еĲ���
    **                  ����ֵΪ0��ʾִ�г�ʱ
    **                  ����ֵС��0��ʾ���緢���˴��󣬵��͵�����������Ѿ��Ͽ�
    **
    **                  ��ʹ��ִ�г�ʱ�����緢�����������£�����Ҳ���ܷ�����һЩ���ݣ���Ӧ�ü��iSendLen��ָ
    **                  ��ĵ�Ԫ��ȷ�������˶����ֽ�
    *********************************************************************************************************/
    int     sendNBytes(BYTE *pBuf,int N,int *iSendLen=NULL);

    int setsockopt(int level,int optname,const char FAR* optval,int optlen);
    //��ǰ�����Ƿ����һ����Ч���׽��ֶ���
    BOOL    IsValid()
    {
        return m_socket!=INVALID_SOCKET?TRUE:FALSE;
    }

    int getpeername(struct sockaddr* name,int* namelen);
    int getsockname(struct sockaddr* name,int* namelen);




    //�ر��׽���
    void    CloseSocket();
       

private:
    SOCKET m_socket;
};


UINT32  inet_addrW(const WCHAR *cp);
int     GetLocalIps(struct in_addr *pAddrList,int iNum);
BOOL    Ping(LPCWSTR lpDestIP, DWORD dwTimeOut, DWORD dwTimes);
#endif