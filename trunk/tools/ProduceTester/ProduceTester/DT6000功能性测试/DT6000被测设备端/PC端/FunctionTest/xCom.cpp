#include <windows.h>
#include "assertMsg.h"
#include <string.h>
#include <stdio.h>
#include "xCom.h"

//CxCom的实现类

CxCom::CxCom():
m_hComFile(INVALID_HANDLE_VALUE)
{
    memset(&m_overLapped,0,sizeof(m_overLapped) );
}

CxCom::~CxCom()
{
	if (m_hComFile !=INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComFile);
        m_hComFile=INVALID_HANDLE_VALUE;
	}

    if (m_overLapped.hEvent !=NULL)
    {
        CloseHandle(m_overLapped.hEvent);
        m_overLapped.hEvent=NULL;
    }
}



#ifndef NDEBUG
//有效的波特率值
static const DWORD dwValidBaudRate[]={
        CBR_110,
        CBR_300,
        CBR_600,
        CBR_1200, 
        CBR_2400,
        CBR_4800, 
        CBR_9600,
        CBR_14400, 
        CBR_19200, 
        CBR_38400, 
        CBR_56000, 
        CBR_57600, 
        CBR_115200, 
        CBR_128000, 
        CBR_256000, 
    };
static bool IsValidBaudRate(DWORD dwBaudRate)
{
    for (int i=0;i<sizeof(dwValidBaudRate)/sizeof(dwValidBaudRate[0]);i++ )
    {
        if (dwValidBaudRate[i]==dwBaudRate  )
        {
            return true;
        }
    }
    return false;
}


#endif


BOOL CxCom::Open(int nPort,int nBaudRate)
{
    HANDLE hComFile;
    //不得在已经有串口被打开的情况下，再打开另一个串口
    assert(m_hComFile ==INVALID_HANDLE_VALUE);
    assert(m_overLapped.hEvent ==NULL);

    WCHAR sz[64];
    swprintf(sz,L"\\\\.\\COM%d",nPort);

    assertMsg(IsValidBaudRate(nBaudRate),L"无效的波特率");

	hComFile=CreateFileW(sz,GENERIC_READ | GENERIC_WRITE, 0, 
		NULL, 
		OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED|FILE_ATTRIBUTE_NORMAL,
		  0);
	if(hComFile ==INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
    //构建重叠IO的环境
    memset(&m_overLapped,0,sizeof(m_overLapped) );
    m_overLapped.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
    if (m_overLapped.hEvent ==NULL)
    {
        CloseHandle(hComFile);
        return FALSE;
    }
    m_hComFile=hComFile;
    ::SetupComm(hComFile,4096,4096);

    //设置串口波特率
    DCB     dcb;
    ::GetCommState(hComFile,&dcb);
    dcb.BaudRate=nBaudRate;
    dcb.ByteSize =8;
    dcb.StopBits =ONESTOPBIT;
    dcb.Parity=NOPARITY;

    dcb.fDsrSensitivity = FALSE;
    dcb.fDtrControl  = DTR_CONTROL_DISABLE;                                        /*  硬件流量控制设置            */    
    dcb.fOutxDsrFlow = FALSE;
    dcb.fRtsControl  = RTS_CONTROL_DISABLE;
    dcb.fOutxCtsFlow = FALSE;

    dcb.fOutX = FALSE;                            /*  禁止软件流控制。fOutX: 收到Xoff后停止发送       */
    dcb.fInX  = FALSE;                            /*  禁止软件流控制。fInX: 缓冲区接收满后发送Xoff    */
    dcb.fTXContinueOnXoff = FALSE;                /*  禁止软件流控制。fInX事件之后，发送是否继续运行  */

    ::SetCommState(hComFile,&dcb);

    COMMTIMEOUTS ct={0};
    ::SetCommTimeouts(m_hComFile,&ct);

	return TRUE;

}

void CxCom::CloseCom()
{
    if (m_hComFile !=INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hComFile);
        m_hComFile=INVALID_HANDLE_VALUE;
    }
    
    if (m_overLapped.hEvent !=NULL)
    {
        CloseHandle(m_overLapped.hEvent);
        m_overLapped.hEvent=NULL;
    }
}

BOOL CxCom::Write(const void *pBuf,int iBufLen,size_t *pWriteLen)
{
    DWORD   dwReturned;
    ResetEvent(m_overLapped.hEvent);
    BOOL    bRet=WriteFile(m_hComFile,pBuf,iBufLen,&dwReturned,&m_overLapped);
    if (!bRet)
    {
        if (GetLastError()==ERROR_IO_PENDING)
        {
            WaitForSingleObject(m_overLapped.hEvent,INFINITE);
            GetOverlappedResult(m_hComFile,&m_overLapped,&dwReturned,FALSE);
            bRet=TRUE;
        }
    }
    
    if (bRet)
    {
        *pWriteLen=(size_t)dwReturned;
        return TRUE;
    }
       
    *pWriteLen=(size_t)dwReturned;
    CancelIo(m_hComFile); 
    return FALSE;
}

BOOL CxCom::WriteNByte(void *pBuf,unsigned int  N,size_t *pWriteLen/* =NULL */ )
{
    size_t       dwWriteLen=0;
    size_t      dwReturn=0;
    BOOL        bRet;
    BYTE        *p=(BYTE *)pBuf;

    assert(N>0);
    assert(pBuf!=NULL);  
    do
    {
        bRet=Write(p+dwWriteLen,N-dwWriteLen,&dwReturn);
        if (bRet)
        {
            dwWriteLen+=dwReturn;
        }
    }  while (dwWriteLen <N &&bRet);
        

    if (pWriteLen !=NULL)
    {
        *pWriteLen=dwWriteLen;
    }
    return bRet;
}

BOOL CxCom::ReadNByte(void *pBuf,unsigned int N,size_t *pReadLen/* =NULL */)
{
    register size_t  dwReadLen=0;
    size_t       dwReturn=0;
    BOOL        bRet=TRUE;;
    BYTE    *p=(BYTE *)pBuf;

    assert(N>0);
    assert(pBuf !=NULL);

    do 
    {
        bRet=Read(p+dwReadLen,N-dwReturn,&dwReturn);
        if (bRet)
        {
            dwReadLen+=dwReturn;
        }
    } while (dwReadLen <N &&bRet);
    
    if (pReadLen !=NULL)
    {
        *pReadLen=dwReadLen;
    }
    return bRet;
}

BOOL CxCom::Read(void *pBuf,int iBufLen,size_t *pReadLen)
{
    DWORD   dwReturned;

    ResetEvent(m_overLapped.hEvent);
    BOOL    bRet=ReadFile(m_hComFile,pBuf,iBufLen,&dwReturned,&m_overLapped);
    if (!bRet)
    {
        if (GetLastError()==ERROR_IO_PENDING)
        {
            WaitForSingleObject(m_overLapped.hEvent,INFINITE);
            GetOverlappedResult(m_hComFile,&m_overLapped,&dwReturned,FALSE);
            bRet=TRUE;
        }
    }

    if (bRet)
    {
        *pReadLen=(size_t)dwReturned;
    }
    CancelIo(m_hComFile);    
    return bRet;
}


BOOL  CxCom::ReadTimeOut(void *pBuf,unsigned int iBufLen, unsigned int uiTimeOut,size_t *pReadLen)
{
    DWORD   dwReturn=0;
    DWORD   dwWait;
    BOOL    bRet=FALSE;

    assert(m_hComFile!=INVALID_HANDLE_VALUE);
    assert(pReadLen!=NULL);
    assert(pBuf!=NULL);
    assert(iBufLen >0);
    assert(uiTimeOut >0);

    m_overLapped.Offset=0;
    m_overLapped.InternalHigh;
    
    ResetEvent(m_overLapped.hEvent);
    bRet=ReadFile(m_hComFile,pBuf,iBufLen,&dwReturn,&m_overLapped);    
    if ( !bRet )
    {
        dwReturn=0;
        if (GetLastError() ==ERROR_IO_PENDING)
        {
            dwWait=WaitForSingleObject(m_overLapped.hEvent,uiTimeOut);
            if (dwWait ==WAIT_OBJECT_0)
            {
                GetOverlappedResult(m_hComFile,&m_overLapped,&dwReturn,FALSE);
                bRet=TRUE;
            }                       
        }
    }

    if (bRet)
    {
        if ( dwReturn !=0)
        {
            *pReadLen=dwReturn;
            return TRUE;
        }
        bRet=FALSE;        
    }

    CancelIo(m_hComFile);
    *pReadLen=dwReturn;
    return bRet;
} 

BOOL CxCom::ReadNByteTimeOut(void *pBuf,unsigned int N, unsigned int uiTimeOut,size_t *pReadLen/* =0 */)
{
    size_t    dwRecvLen=0;
    size_t          siReturnBytes =0;      
    BYTE            *p=(BYTE *)pBuf;
    BOOL            bRet=FALSE;

    while(dwRecvLen <N)
    {
        bRet=ReadTimeOut( p+dwRecvLen,N-dwRecvLen,uiTimeOut,&siReturnBytes );
        if (!bRet)
        {
            break;
        }
        dwRecvLen+=siReturnBytes;
    }   

    *pReadLen = dwRecvLen;
    return bRet;

}