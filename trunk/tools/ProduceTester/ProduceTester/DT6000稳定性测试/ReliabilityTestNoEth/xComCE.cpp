#include "stdafx.h"
#include "assertMsg.h"
#include <string.h>
#include <stdio.h>
#include "xComCE.h"

//CxCom的实现类

CxComCE::CxComCE():
m_hComFile(INVALID_HANDLE_VALUE),
m_bKillRecvThread(FALSE),
m_pfnOnRecv(NULL),
m_pUserParam(NULL),
m_hRecvThread(NULL)
{
  
}

CxComCE::~CxComCE()
{
	if (m_hComFile !=INVALID_HANDLE_VALUE)
	{
        if (m_hRecvThread)
        {
            StopRecvThread();
        }
		CloseHandle(m_hComFile);
        m_hComFile=INVALID_HANDLE_VALUE;
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


BOOL CxComCE::Open(int nPort, 
                   int nBaudRate/* =CBR_115200 */, 
                   int iDataBit/* =8 */, 
                   BYTE bStopBit/* =ONESTOPBIT */, 
                   BYTE parity/* =NOPARITY */, 
                   BYTE dtrCtrl/* =DTR_CONTROL_DISABLE */, 
                   BYTE rtsCtrl/* =RTS_CONTROL_DISABLE */ 
                   )
{
    HANDLE hComFile;
    //不得在已经有串口被打开的情况下，再打开另一个串口
    assert(m_hComFile ==INVALID_HANDLE_VALUE);   

    WCHAR sz[64];
    swprintf(sz,L"\\$device\\COM%d",nPort);

    assertMsg(IsValidBaudRate(nBaudRate),L"无效的波特率");

	hComFile=CreateFileW(sz,GENERIC_READ | GENERIC_WRITE, 0, 
		NULL, 
		OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
		  0);
	if(hComFile ==INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

    m_hComFile=hComFile;
    ::SetupComm(hComFile,4096,4096);

    //设置串口波特率
    DCB     dcb;
    ::GetCommState(hComFile,&dcb);
    dcb.BaudRate=nBaudRate;
    dcb.ByteSize=iDataBit;
    dcb.StopBits=bStopBit;
    dcb.Parity   = parity;                                              /*  奇偶校验设置                */

    if (parity != PARITY_NONE) 
    {
        dcb.fParity = TRUE;                                             /*  奇偶校验使能控制            */
    }  
    else  
    {
        dcb.fParity = FALSE;
    }

    dcb.fBinary = TRUE;									            /*  允许二进制模式              */

    dcb.fDtrControl  = dtrCtrl;			                            /*  硬件流量控制设置            */	
    if (dtrCtrl == DTR_CONTROL_DISABLE)	{        
        dcb.fOutxDsrFlow = FALSE;
    }  else  {       
        dcb.fOutxDsrFlow = TRUE;
    }

    dcb.fRtsControl  = rtsCtrl;
    if (rtsCtrl == RTS_CONTROL_DISABLE)  {        
        dcb.fOutxCtsFlow = FALSE;
    }  else  {
        dcb.fOutxCtsFlow = TRUE;
    }

    dcb.fOutX = FALSE;						    /*  禁止软件流控制。fOutX: 收到Xoff后停止发送       */
    dcb.fInX  = FALSE;							/*  禁止软件流控制。fInX: 缓冲区接收满后发送Xoff    */
    dcb.fTXContinueOnXoff = FALSE;				/*  禁止软件流控制。fInX事件之后，发送是否继续运行  */

    ::SetCommState(hComFile,&dcb);
	return TRUE;

}

void CxComCE::CloseCom()
{
    if (m_hRecvThread)
    {
        StopRecvThread();
    }

    if (m_hComFile !=INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hComFile);
        m_hComFile=INVALID_HANDLE_VALUE;
    }   
}

BOOL CxComCE::Write(const void *pBuf,int iBufLen,size_t *pWriteLen)
{
    DWORD   dwReturned;   
    BOOL    bRet=WriteFile(m_hComFile,pBuf,iBufLen,&dwReturned,NULL);   
       
    if (pWriteLen !=NULL)
    {
        *pWriteLen=(size_t)dwReturned;
    }    
    return bRet;
}
// 
BOOL CxComCE::WriteNByte(const void *pBuf,unsigned int  N,size_t *pWriteLen/* =NULL */ )
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
 
BOOL CxComCE::ReadNByte(void *pBuf,unsigned int N,size_t *pReadLen/* =NULL */)
{
    register DWORD  dwReadLen=0;
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

BOOL CxComCE::Read(void *pBuf,int iBufLen,size_t *pReadLen)
{
    DWORD   dwReturned;
    
    BOOL    bRet=ReadFile(m_hComFile,pBuf,iBufLen,&dwReturned,NULL);
    
    if (bRet)
    {
        *pReadLen=(size_t)dwReturned;
    }
    
    return bRet;
}

BOOL CxComCE::ReadTimeOut(void *pBuf,int iBufLen,DWORD dwTimeOut,size_t *pReadLen)
{
    COMMTIMEOUTS ct;
    GetCommTimeouts(&ct);

    COMMTIMEOUTS ctNew=ct;
    ctNew.ReadTotalTimeoutConstant=dwTimeOut;
    ctNew.ReadTotalTimeoutMultiplier=0;

    ::SetCommTimeouts(m_hComFile,&ctNew);
    DWORD dwReturned;
    BOOL bRet=ReadFile(m_hComFile,pBuf,iBufLen,&dwReturned,NULL);
    if (bRet)
    {
        if (pReadLen!=NULL)
        {
            *pReadLen=(size_t)dwTimeOut;
        }
    }

    SetCommTimeouts(&ct);
    return bRet;
}

BOOL CxComCE::ReadNByteTimeOut(void *pBuf,int N,DWORD dwTimeOut,size_t *pReadLen)
{
    assertMsg(N>0, L"参数不合法");
    COMMTIMEOUTS ct;
    GetCommTimeouts(&ct);

    COMMTIMEOUTS ctNew=ct;
    ctNew.ReadTotalTimeoutConstant=dwTimeOut;
    ctNew.ReadTotalTimeoutMultiplier=0;
    ::SetCommTimeouts(m_hComFile,&ctNew);

    DWORD       dwReturned;
    BOOL        bRet;
    size_t      uiTotal=0;

    do         
    {
        dwReturned=0;
        bRet=ReadFile(m_hComFile,(BYTE *)pBuf+uiTotal,N-uiTotal,&dwReturned,NULL);
        if (dwReturned >0)
        {
            uiTotal+=(size_t)dwReturned;
        }
        else
        {
            break;
        }

    } while (uiTotal <(size_t)N);

    bRet=TRUE;
    if (uiTotal <size_t(N))
    {
        bRet=FALSE;
    }

    if (pReadLen !=NULL)
    {
        *pReadLen=uiTotal;
    }
    
    return bRet;
}

BOOL CxComCE::BeginRecvThread(PFN_ON_SERIAL_RECEIVE pfnOnRecv,void *pUserParam)
{
    assertMsg(m_hRecvThread==NULL,L"已经有一个接收线程在运行了");

    m_pUserParam=pUserParam;
    m_pfnOnRecv=pfnOnRecv;

    m_bKillRecvThread=FALSE;
    m_hRecvThread=CreateThread(NULL,0,RecvThreadProc,this,0,NULL);
    
    return m_bKillRecvThread!=NULL;
}

void CxComCE::StopRecvThread()
{
    m_bKillRecvThread=TRUE;

    if (m_hRecvThread)
    {
        WaitForSingleObject(m_hRecvThread,INFINITE);
        CloseHandle(m_hRecvThread);
        m_hRecvThread=NULL;
    }
}

DWORD CxComCE::RecvThreadProc(void *pParam)
{
    CxComCE     *pCom=(CxComCE *)pParam;


    BYTE    *pBuf=NULL;
    pBuf=(BYTE *)malloc(1024);
    if (pBuf ==NULL)
    {
        return 0;
    }
    
   
    
    COMMTIMEOUTS ctNew;
    pCom->GetCommTimeouts(&ctNew);
    ctNew.ReadTotalTimeoutConstant=50;
    ctNew.ReadTotalTimeoutMultiplier=0;
    ::SetCommTimeouts(pCom->m_hComFile,&ctNew);

    HANDLE  hFile=pCom->m_hComFile;
    DWORD   dwReturn;
    BOOL    bRet=TRUE;
    
    

    //开始接收过程
    while (bRet && !pCom->m_bKillRecvThread)
    {
        
        dwReturn=0;
        bRet = ReadFile(hFile,pBuf,1024,&dwReturn,NULL);
        if (bRet)
        {
            //接收到数据则调用回调函数
            if (dwReturn >0)
            {
                (*pCom->m_pfnOnRecv)(pCom->m_pUserParam,pBuf,dwReturn);
            }
        }
        else
        {
            MessageBox(NULL,L"接收出错了",NULL,MB_OK);
            Sleep(5000);
        }
        Sleep(10);
    }


    
    
    return 0;
}