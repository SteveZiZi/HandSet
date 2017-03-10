#pragma once
#include "../../../inc/DspArmProtocol.h"
#include "../dsp/lock.h"
#include <afxmt.h>
class CDspDataSet;
class CDspController;
class CDsp
{
public:
	enum Type{
		T_SMV,
		T_CONFIG,
		T_RESET_CACHE,
		T_SMV_GEN,
		T_MSG_TIME,
		T_DSP_INFO
	};
    static CDsp *GetInstance();
    int SmvCalc(DWORD dwTimeout, const SmvData &smvData, SMV_DATA_CALC_RT *pSmvCalcRT);
    int SetConfig(DWORD dwTimeout, const Config &dspConfig);
    int ResetCache(DWORD dwTimeout, int nType);
    int SmvGenCalc(DWORD dwTimeout, const SmvGenData &smvGenData, 
            SMV_GEN_DATA_CALC_RT *pSmvGenCalcRT);
    int MsgTimeCalc(DWORD dwTimeout, const MsgTimeData &msgTimeData, 
            MSG_DISP_CALC_RT *pMsgDispRT);
    int GetDspInfo(DWORD dwTimeout, DspInfo *pDspInfo);
    int WaitForObject(Type type, DWORD dwMilliseconds);
    virtual ~CDsp();
private: 
    static const int TypeNum = 6;
    static Lock m_singleLock;
    //Lock m_ioLock;                        ///< 读写锁，保证一次读写操作正确
    static bool m_fRead;                    ///< 读标志，指示Dsp读状态
    static HANDLE m_iMutexs[TypeNum];             ///< 写互斥量
    static HANDLE m_oMutexs[TypeNum];             ///< 读互斥量

    //CDspDataSet &m_dspDataSet;            ///< 存储DSP计算结果的数据集引用
    CDspController *m_pDspCtrl;    
    int m_errno[TypeNum];                         ///< 各种计算返回的错误码
    HANDLE m_hThreadReadData;               ///< 读线程句柄

    static DWORD ReadDataThread(LPVOID pArg);

    CDsp();
    DWORD CheckSend(Type type);               ///< 检测发送条件    
    void ReadData();
    bool StartRead();
    void StopRead();
};
