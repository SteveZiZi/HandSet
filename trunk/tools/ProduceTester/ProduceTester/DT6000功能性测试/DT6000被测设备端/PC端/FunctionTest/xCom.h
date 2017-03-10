#ifndef __X_COM_H_
#define __X_COM_H_

class CxCom
{
public:
    CxCom();
    ~CxCom();

    //CxCom并不是线程安全的，多线程共享一个CxCom对象,调用其方法时，都必须同步

    BOOL Open(int nPort,int nBaudRate);
    void CloseCom();

    //一组读写的同步API
    BOOL Read(void *pBuf,int iBufLen,size_t *pReadLen);
    BOOL Write(const void *pBuf,int iBufLen,size_t *pWriteLen);
    BOOL    ReadNByte(void *pBuf,unsigned int N,size_t *pReadLen=NULL);
    BOOL    WriteNByte(void *pBuf,unsigned int N,size_t *pWriteLen=NULL );
    BOOL    ReadTimeOut(void  *pBuf,unsigned int iBufLen, unsigned int uiTimeOut,size_t *pReadLen);
    BOOL    ReadNByteTimeOut(void  *pBuf,unsigned int N, unsigned int uiTimeOut,size_t *pReadLen);



    //设置串口功能函数，各函数功能与同名windowAPI相同可查阅相关API
    BOOL    ClearCommError(){DWORD Err; COMSTAT sta;return ::ClearCommError(m_hComFile,&Err,&sta) ;}
    BOOL    ClearRecv(){return ::PurgeComm(m_hComFile,PURGE_RXABORT|PURGE_TXABORT); }
    BOOL SetupComm(DWORD dwInQueue, DWORD dwOutQueue)   {return ::SetupComm(m_hComFile,dwInQueue,dwOutQueue);}
    BOOL GetCommState(LPDCB lpDCB)      {return ::GetCommState(m_hComFile,lpDCB);};
    BOOL SetCommState(LPDCB lpDCB)      {return ::GetCommState(m_hComFile,lpDCB);}; 
    BOOL SetCommMask(DWORD dwEvtMask)   {return ::SetCommMask(m_hComFile,dwEvtMask);}; 
    BOOL GetCommMask(LPDWORD lpEvtMask)             {return ::GetCommMask(m_hComFile,lpEvtMask);}; 
    BOOL GetCommTimeouts(LPCOMMTIMEOUTS lpCommTimeouts) {return ::GetCommTimeouts(m_hComFile,lpCommTimeouts); }; 
    BOOL SetCommTimeouts(LPCOMMTIMEOUTS lpCommTimeouts) {return ::SetCommTimeouts(m_hComFile,lpCommTimeouts); };     
    BOOL EscapeCommFunction(DWORD dwFunc)           {return ::EscapeCommFunction(m_hComFile,dwFunc); }; 
    BOOL GetCommModemStatus(LPDWORD lpModemStat)    {return ::GetCommModemStatus(m_hComFile,lpModemStat);}; 
    BOOL PurgeComm(DWORD dwFlags)       {return ::PurgeComm(m_hComFile,dwFlags);}
    BOOL WaitCommEvent(LPDWORD lpEvtMask, LPOVERLAPPED lpOverlapped)    {return ::WaitCommEvent(m_hComFile,lpEvtMask,lpOverlapped);}; 
    BOOL SetCommBreak()                 {return ::SetCommBreak(m_hComFile); }; 
    BOOL ClearCommBreak()               {return ::ClearCommBreak(m_hComFile); };
    BOOL ClearCommError(LPDWORD lpErrors,LPCOMSTAT lpStat)  {return ::ClearCommError(m_hComFile,lpErrors,lpStat);}; 

public:
    HANDLE              m_hComFile;
    OVERLAPPED          m_overLapped;
};

#endif