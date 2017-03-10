#pragma once
#include "./../src/dspclient/include/DspDataSet.h"
#include "../../src/dspclient/dsp/lock.h"
class CDataManager
{
public:
    ~CDataManager(void);
    CDspDataSet &GetDspDataSet()
    {
        return m_dspDataSet;
    };
    SMV_GEN_DATA_CALC_RT GetSmvGenData();
    static CDataManager &GetInstance();
private:
    CDspDataSet m_dspDataSet;
    SMV_GEN_DATA_CALC_RT m_smvGenData;
    CDataManager(void);
    static Lock m_singleLock;        ///< µ¥ÀýËø
};
