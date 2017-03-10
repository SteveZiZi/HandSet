#ifndef __X_COM_CE__H_
#define __X_COM_CE__H_

//���崮���߳̽��ջص�����
//pUserPram���û���BeginRecvThread�д�����Զ������
//pBufָ����յ�������
//len������յ�����Ч���ݵĳ���
typedef  void (*PFN_ON_SERIAL_RECEIVE)(void *pUserPram,
                const void *pBuf,unsigned int len); 

class CxComCE
{
public:
    CxComCE();
    ~CxComCE();

    //CxCom�������̰߳�ȫ�ģ����̹߳���һ��CxCom����,�����䷽��ʱ��������ͬ��
    //�򿪴���
    BOOL Open(int nPort,
        int nBaudRate=CBR_115200,
        int iDataBit=8,                     //����λ��,��ЧֵΪ5,6,7,8
        BYTE bStopBit=ONESTOPBIT,           //ֹͣλ��,��ЧֵͬDCB::StopBits
        BYTE parity=NOPARITY,               //У�鷽ʽ����ЧֵͬDCB::Parity
        BYTE dtrCtrl=DTR_CONTROL_DISABLE,   //DTR���ط�ʽ,��ЧֵͬDCB::fDtrControl,Ĭ��ֵ��ʹ�� 
        BYTE rtsCtrl=RTS_CONTROL_DISABLE    //RTS���ط�ʽ,��ЧֵͬDCB::fRtsControl,Ĭ��ֵΪ��ʹ�� 
        );

    void CloseCom();

    //һ���д��ͬ��API
    //���ָ��û������Ĭ��ֵΪNULL�����ʾ��ֵ����ΪNULL
    BOOL Read(void *pBuf,int iBufLen,size_t *pReadLen);
    BOOL Write(const void *pBuf,int iBufLen,size_t *pWriteLen=NULL);
    BOOL WriteNByte(const void *pBuf,unsigned int  N,size_t *pWriteLen=NULL);
    BOOL ReadNByte(void *pBuf,unsigned int N,size_t *pReadLen);
    BOOL ReadTimeOut(void *pBuf,int iBufLen,DWORD dwTimeOut,size_t *pReadLen);
    BOOL ReadNByteTimeOut(void *pBuf,int N,DWORD dwTimeOut,size_t *pReadLen);
    
    //����һ�����õ��߳̽��շ���,��ʹ��Ĭ�ϵ��߳̽����Ժ󣬳�����ֹ�������벻Ҫ����
    //ͬ���Ķ����ں���,���⣬���Ҳ��Ҫ����������ջ������ȿ���Ӱ�쵽���յĲ���
    //��Щ��������ڵ�ǰû�н��н���ʱ������
    BOOL BeginRecvThread(PFN_ON_SERIAL_RECEIVE pfnOnRecv,void *pUserParam);
    void StopRecvThread();

    

    //���ô��ڹ��ܺ�����������������ͬ��windowAPI��ͬ�ɲ������API
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

    //���±�����������߳����
    HANDLE              m_hRecvThread;          //�����߳̾��
    volatile BOOL       m_bKillRecvThread;      //�Ƿ�ɱ�������߳�
    PFN_ON_SERIAL_RECEIVE m_pfnOnRecv;          //�û�����Ļص�����ָ��
    void                *m_pUserParam;          //�û�������Զ������
    static DWORD CALLBACK RecvThreadProc(void *pParam); //�̺߳���    
};

#endif