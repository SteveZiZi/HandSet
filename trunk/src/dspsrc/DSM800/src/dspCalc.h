#ifndef _DSPCALC_H
#define _DSPCALC_H

#include "DspArmProtocol.h"

typedef struct _SmvCalcData
{
    uint8_t  nChannelCount;                        ///< 实际传输的通道数目
    ///< 实际传输的数据一个周波的采样数
    uint16_t nSmvPerCircle;
    uint8_t  nChannelVCType[MAX_SMV_CHANNELS];     ///< 每个通道的电压/电流类型
    ///< 一个周波内各个通道的采样数据
    float    fChannelData[MAX_SMV_CHANNELS][MAX_SMV_CALC_COUNT];
}SmvCalcData, *PSmvCalcData;

#define FREQ_CALC_PERIOD_NUM              1      	///< 多少个10周期计算一次频率

extern void SmvGenCalc(const SmvGenData *pSmvGenData, SMV_GEN_DATA_CALC_RT *pSmvGenRT);
extern void RealSmvCalc(const SmvData *pSmvRawData, SmvData *pSmvRealData);
extern void SmvDataCalc(const SmvCalcData *pSmvCalcData, const Config *pConfig, SMV_DATA_CALC_RT *pSmvCalcRT);
extern void MsgDispCalc(const MSGTIME pMsgTimes[], const Config *pConfig, float *pMsgDisp);
#endif
