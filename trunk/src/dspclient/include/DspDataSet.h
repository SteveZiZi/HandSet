#pragma once
#include <hash_map>
#include "../../../inc/DspArmProtocol.h"
#include "../dsp/lock.h"
#include <types.h>
class CDspDataSet
{
public:
    CDspDataSet();
    bool Write(UINT8 nCommandID, void *pData, UINT32 nDataLength);
    UINT32 GetDataCount();
    void GetError(Error &err);
    void GetDspInfo(DspInfo &dspInfo);
    void GetCalcData(SMV_DATA_CALC_RT &smvCalcData);
    void GetGenCalcData(SMV_GEN_DATA_CALC_RT &smvGenCalcData);
    bool GetMsgDisp(float *buf, UINT32 bufLength);
private:
    UINT32 m_nDataCount;                          ///< 从DSP读取数据计数
    Error m_err;                                  ///< 执行ConfigDsp后返回的错误状态码
    DspInfo m_dspInfo;                            
    //SmvData m_smvData;                          ///< 实际值计算结果
    SMV_DATA_CALC_RT m_smvCalcData;               ///< 谐波等计算结果
    ///< 生成的波形数据
    //float m_smvGenData[MAX_SMV_PER_CIRCLE*MAX_SMV_CIRCLE_COUNT];
    //UINT32 m_smvGenSize;                          ///< 生成的波形样点个数
    SMV_GEN_DATA_CALC_RT m_smvGenCalcData;        ///< 生成的波形数据
    float m_msgDisp[11];                          ///< 报文离散度  
    stdext::hash_map<UINT8, void *> m_ptrMap;     ///< 指针映射
    stdext::hash_map<UINT8, UINT32> m_lengthMap;  ///< 长度映射
    stdext::hash_map<UINT8, Lock*> m_lockMap;     ///< 锁映射
    Lock m_wrLocks[6];                            ///< 读写锁
};

