#pragma once
#include "../../../inc/DspArmProtocol.h"
#include "../dsp/lock.h"
class CDspDataSet;
class CDspController;

/******************************************************************************/
/*            请在程序生命周期的开始和结束时初始化和释放Dsp资源               */
/******************************************************************************/
// 初始化Dsp
bool DspInit();

// 释放Dsp
void DspRelease();
/******************************************************************************/

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
    CDsp();
    CDsp(CDspDataSet *pDspDataSet);
    bool SendSmv(const SmvData &smvData);
    bool SetConfig(const Config &dspConfig);
    bool ResetCache(int nType = 0);
    bool SendMsgTime(const MsgTimeData &msgTimeData);
    bool GetDspInfo();

    // 专门为波形生成定义的接口
    int SmvGenCalc(DWORD dwTimeout, const SmvGenData &smvGenData, 
        SMV_GEN_DATA_CALC_RT *pSmvGenCalcRT);
    int WaitForObject(Type type, DWORD dwMilliseconds);
    
    bool IsReading();
    bool StartRead();
    void StopRead();
    virtual ~CDsp();
private:
    

    // 用以控制输入输出
    static const int TypeNum = 6;    
    static HANDLE m_iEvents[TypeNum];           ///< 写互斥量
    static HANDLE m_oEvents[TypeNum];           ///< 读互斥量
    static bool m_fRead;                        ///< 读标志，指示Dsp读状态
	static SMV_GEN_DATA_CALC_RT *m_pSmvGenCalcData;        ///< 生成的波形数据指针
    static CDspController *m_pDspCtrl;          ///< 存储DspController
    static CDspDataSet *m_pDspDataSet;          ///< 存储DSP计算结果的数据集
    static int m_nErrno[TypeNum];               ///< 各种计算返回的错误码 
    static HANDLE m_hThreadReadData;                   ///< 读线程句柄    

// 用于后扩展
#if 0
    DspInfo *m_pDspInfo;                            
    SMV_DATA_CALC_RT *m_pSmvCalcData;               ///< 谐波等计算结果
#endif

// 用于以后扩展
#if 0
    stdext::hash_map<UINT8, void *> m_ptrMap;     ///< 指针映射
    stdext::hash_map<UINT8, UINT32> m_lengthMap;  ///< 长度映射
#endif

    static DWORD ReadDataThread(LPVOID pArg);
    DWORD CheckSend(Type type, DWORD dwTimeout);    ///< 检测发送条件 
};