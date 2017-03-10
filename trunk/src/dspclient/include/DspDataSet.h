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
    UINT32 m_nDataCount;                          ///< ��DSP��ȡ���ݼ���
    Error m_err;                                  ///< ִ��ConfigDsp�󷵻صĴ���״̬��
    DspInfo m_dspInfo;                            
    //SmvData m_smvData;                          ///< ʵ��ֵ������
    SMV_DATA_CALC_RT m_smvCalcData;               ///< г���ȼ�����
    ///< ���ɵĲ�������
    //float m_smvGenData[MAX_SMV_PER_CIRCLE*MAX_SMV_CIRCLE_COUNT];
    //UINT32 m_smvGenSize;                          ///< ���ɵĲ����������
    SMV_GEN_DATA_CALC_RT m_smvGenCalcData;        ///< ���ɵĲ�������
    float m_msgDisp[11];                          ///< ������ɢ��  
    stdext::hash_map<UINT8, void *> m_ptrMap;     ///< ָ��ӳ��
    stdext::hash_map<UINT8, UINT32> m_lengthMap;  ///< ����ӳ��
    stdext::hash_map<UINT8, Lock*> m_lockMap;     ///< ��ӳ��
    Lock m_wrLocks[6];                            ///< ��д��
};

