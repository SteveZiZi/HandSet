
#pragma once

#include "gpmCfg.h"

class _GPM_MDD_c;

typedef struct {
    UINT32 magic;
    UINT16 major;
    UINT16 minor;
    UINT32 thisZone;
    UINT32 sigFigs;
    UINT32 snapLen;
    UINT32 linkType;
}PCAP_HEADER_t;



typedef enum {
    PING_BUF = 0,
    PONG_BUF = 1
}RECORD_BUF_ITEM_e;


class Net_Base_c {
public:
    Net_Base_c(HANDLE hDevice, UINT16 nDeviceItem);
    ~Net_Base_c();
    BOOL Init(WCHAR *pNotifyEvtName, WCHAR *pMapFileName, UINT32 nMapFileLen, LPTHREAD_START_ROUTINE lpStartAddress=NULL);
    VOID DeInit(void);

    void ResetNet(void);

    void GetSmvVertifyRst(UINT32 &uTotalFrame, UINT32 &uErrorFrame);

    void SetFilePath(CString strFilePath);

    /*
     *  校验接收到的报文是否正确
     */
    virtual BOOL VerifySmvFrame(void) = 0;
    /*
     *  从串口中打印接收到的报文
     */
    virtual void PrintFrame(void) = 0;

    /*
     *  临界区加锁
     */
    virtual void LockInter(void) = 0;
    /*
     *  临界区解锁
     */
    virtual void UnLockInter(void) = 0;

public:
    /*
    **  各自需要的变量
    */
    HANDLE          m_hNotifyEvent;                                    //用于通知通道x指定的接收操作完成
    HANDLE          m_hMapping;                                        //通道x用来缓冲数据的内存映射

    Map_File_Header_t *m_ptMapHeader;                                   //指向内存映射文件的最前端
    PBYTE           m_dataBuf;                                          //指向通道x的缓冲区

    HANDLE          m_hRxThread;
    HANDLE          m_hExitEvent;

    BOOL            m_bRecv;                                           //通道x是否接收数据

    CRITICAL_SECTION m_cs;

    BOOL            m_bRecord;                                          //是否需要录波

    HANDLE          m_hDevice;
    UINT16          m_nDeviceItem;

    bool            m_bFromFile; 
    
    HANDLE          m_hRxEvent;

protected:
    CString m_strFilePath;
    TEST_RESULT_a m_uTestResult;
    TEST_RESULT_a m_uUtcTest;

private:
   
};

typedef BOOL (*pFUN_FIBERPROC)(PVOID pArg);

class _SMV_92_c : public Net_Base_c {
public:


    _SMV_92_c(HANDLE hDevice, UINT16 nDeviceItem):Net_Base_c(hDevice, nDeviceItem) { 
        m_pFiberProcFun[0] = userSmvProc;
        m_pFiberProcFun[1] = userGooseProc;
        m_pFiberProcFun[2] = userIEEE1588Proc;
        m_bGetSmpCntVal = TRUE;
        m_bGetGooseVal = TRUE;
        m_uGooseTxCnt = 0;
    }
    ~_SMV_92_c() {}

    static BOOL userSmvProc(PVOID pArg);
    static BOOL userGooseProc(PVOID pArg);
    static BOOL userIEEE1588Proc(PVOID pArg);

    void SetTxParam(IOCTRL_FIBER_IN_PARAM_t & tMuSvTxParam, bool bFromFile = TRUE);
    void SetFrameFlag(UINT8 uFrameFlag);
    bool StartMuDetect();
    bool StopMuDetect();
    bool MuTx();
    BOOL GetMuTxFrame(PVOID pData);
    BOOL GetMuRxFrame(PVOID pData);
    BOOL GetGooseTxFrame(OUT PVOID pData, IN BOOL bUpdate = FALSE);
    bool MuStop();
    bool VertifyBcodeTime();
    /*
     *  校验接收到的报文是否正确
     */
    virtual BOOL VerifySmvFrame(void);
    BOOL VerifyGooseFrame(void);
    BOOL VerifyIEEE1588Frame(void);
    /*
     *  从串口中打印接收到的报文
     */
    virtual void PrintFrame(void);
    
    //中断服务线程
    friend DWORD FiberThreadProcRx1(PVOID pArg);
    friend DWORD FiberThreadProcRx2(PVOID pArg);
    friend DWORD FiberThreadProcRx3(PVOID pArg);

    virtual void LockInter(void) {
        EnterCriticalSection(&m_cs);
        m_ptMapHeader->OpticalNetHeader.bAppDispDone = FALSE;
    }
    virtual void UnLockInter(void) {
        m_ptMapHeader->OpticalNetHeader.bAppDispDone = TRUE;
        LeaveCriticalSection(&m_cs);
    }

    /*
     *  SMV  GOOSE  IEEE1588  
     */
    pFUN_FIBERPROC m_pFiberProcFun[3];

    PUINT8   m_pSmvTxBuf;
    FIBER_RX_STYLE_e    m_RxStyle;

    //校验
    UINT16          m_uSmpCntVal;
    BOOL            m_bGetSmpCntVal;                                    //是否需要重新获取采样计数器的值
    BOOL            m_bGetGooseVal;                                     //是否需要重新获取GOOSE的StNum、SqNum
    UINT32          m_uStNumVal;
    UINT32          m_uSqNumVal;
    UTC_TIME_t      m_tUtcTime;

    PUINT8          m_pTxData;
    PUINT8          m_pTempTxData;
    
private:
    IOCTRL_FIBER_IN_PARAM_t  m_tMuSvTxParam;
    UINT8           m_uFrameFlag;
    UINT32 m_uGooseTxCnt; 
};









class _FT3_c : public Net_Base_c {
public:
    _FT3_c(HANDLE hDevice, UINT16 nDeviceItem):Net_Base_c(hDevice, nDeviceItem) { m_bGetSmpCntVal = TRUE;}
    ~_FT3_c();

    /*
     *  校验接收到的报文是否正确
     */
    virtual BOOL VerifySmvFrame(void);
    /*
     *  从串口中打印接收到的报文
     */
    virtual void PrintFrame(void);

    BOOL userFt3Proc(PVOID pVoid);
    
    //cw
    bool Ft3Tx();

    bool Ft3Rx();

    bool Ft3RxStop();

    void SetFT3TxInfo(Ft3SendCtlWord_t tCtrlW);

    void GetFT3TxInfo(Ft3SendCtlWord_t &tSendCtrlW);

    void SetFt3TxFrame(PVOID pData = NULL);

    void SetFt3TxAddress(PVOID pData);

    DWORD GetFT3TxFrame(PVOID pData);

    DWORD GetFT3RxFrame(PVOID pData);

    bool Ft3StopTx();

    virtual void LockInter(void) {
        EnterCriticalSection(&m_cs);
        m_ptMapHeader->ft3Header.bAppDispDone = FALSE;
    }
    virtual void UnLockInter(void) {
        m_ptMapHeader->ft3Header.bAppDispDone = TRUE;
        LeaveCriticalSection(&m_cs);
    }

    //中断服务线程
    friend DWORD ft3ThreadProcRx1(PVOID pArg);
    friend DWORD ft3ThreadProcRx2(PVOID pArg);

    UINT16          m_uSmpCntVal;
    UTC_TIME_t      m_tUtcTime;
    BOOL            m_bGetSmpCntVal;                                    //是否需要重新获取采样计数器的
    PUINT16         m_pFt3TxData;

private:
    FT3_TX_CFG_t m_tTxCfg;
};













#define RECORD_FILE_SIZE    20971520

BOOL VerifyUtcTime(UTC_TIME_t preTime, UTC_TIME_t curTime, DWORD dwSampleRate);
BOOL VerifyGooseFrame(IN PVOID pData, IN UINT16 uSize, OUT UINT16 *puStNumPos, OUT UINT16 *puSqNumPos);
