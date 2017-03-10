#ifndef __X_COM_CE__H_
#define __X_COM_CE__H_

//定义串口线程接收回调函数
//pUserPram是用户在BeginRecvThread中传入的自定义参数
//pBuf指向接收到的数据
//len代表接收到的有效数据的长度
typedef  void (*PFN_ON_SERIAL_RECEIVE)(void *pUserPram,
                const void *pBuf,unsigned int len); 

class CxComCE
{
public:
    CxComCE();
    ~CxComCE();

    //CxCom并不是线程安全的，多线程共享一个CxCom对象,调用其方法时，都必须同步
    //打开串口
    BOOL Open(int nPort,
        int nBaudRate=CBR_115200,
        int iDataBit=8,                     //数据位数,有效值为5,6,7,8
        BYTE bStopBit=ONESTOPBIT,           //停止位数,有效值同DCB::StopBits
        BYTE parity=NOPARITY,               //校验方式，有效值同DCB::Parity
        BYTE dtrCtrl=DTR_CONTROL_DISABLE,   //DTR流控方式,有效值同DCB::fDtrControl,默认值不使用 
        BYTE rtsCtrl=RTS_CONTROL_DISABLE    //RTS流控方式,有效值同DCB::fRtsControl,默认值为不使用 
        );

    void CloseCom();

    //一组读写的同步API
    //如果指针没有设置默认值为NULL，则表示其值不能为NULL
    BOOL Read(void *pBuf,int iBufLen,size_t *pReadLen);
    BOOL Write(const void *pBuf,int iBufLen,size_t *pWriteLen=NULL);
    BOOL WriteNByte(const void *pBuf,unsigned int  N,size_t *pWriteLen=NULL);
    BOOL ReadNByte(void *pBuf,unsigned int N,size_t *pReadLen);
    BOOL ReadTimeOut(void *pBuf,int iBufLen,DWORD dwTimeOut,size_t *pReadLen);
    BOOL ReadNByteTimeOut(void *pBuf,int N,DWORD dwTimeOut,size_t *pReadLen);
    
    //定义一个内置的线程接收方法,但使用默认的线程接收以后，除非终止，否则请不要调用
    //同步的读串口函数,另外，最好也不要调用清除接收缓冲区等可能影响到接收的操作
    //这些操作最好在当前没有进行接收时来调用
    BOOL BeginRecvThread(PFN_ON_SERIAL_RECEIVE pfnOnRecv,void *pUserParam);
    void StopRecvThread();

    

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

    //以下变量与与接收线程相关
    HANDLE              m_hRecvThread;          //接收线程句柄
    volatile BOOL       m_bKillRecvThread;      //是否杀死接收线程
    PFN_ON_SERIAL_RECEIVE m_pfnOnRecv;          //用户传入的回调函数指针
    void                *m_pUserParam;          //用户传入的自定义参数
    static DWORD CALLBACK RecvThreadProc(void *pParam); //线程函数    
};

#endif