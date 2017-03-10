#ifndef _DSPCALC_H
#define _DSPCALC_H

#include "DspArmProtocol.h"

typedef struct _SmvCalcData
{
    uint8_t  nChannelCount;                        ///< ʵ�ʴ����ͨ����Ŀ
    ///< ʵ�ʴ��������һ���ܲ��Ĳ�����
    uint16_t nSmvPerCircle;
    uint8_t  nChannelVCType[MAX_SMV_CHANNELS];     ///< ÿ��ͨ���ĵ�ѹ/��������
    ///< һ���ܲ��ڸ���ͨ���Ĳ�������
    float    fChannelData[MAX_SMV_CHANNELS][MAX_SMV_CALC_COUNT];
}SmvCalcData, *PSmvCalcData;

#define FREQ_CALC_PERIOD_NUM              1      	///< ���ٸ�10���ڼ���һ��Ƶ��

extern void SmvGenCalc(const SmvGenData *pSmvGenData, SMV_GEN_DATA_CALC_RT *pSmvGenRT);
extern void RealSmvCalc(const SmvData *pSmvRawData, SmvData *pSmvRealData);
extern void SmvDataCalc(const SmvCalcData *pSmvCalcData, const Config *pConfig, SMV_DATA_CALC_RT *pSmvCalcRT);
extern void MsgDispCalc(const MSGTIME pMsgTimes[], const Config *pConfig, float *pMsgDisp);
#endif
