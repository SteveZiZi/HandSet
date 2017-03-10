#include "stdafx.h"
#include "../include/DspDataSet.h"

CDspDataSet::CDspDataSet()
{
    m_ptrMap[DSP2ARM_CONTROLCODE_INFO] = &m_dspInfo;
    //ptrMap[DSP2ARM_CONTROLCODE_SMV_DATA] = &m_smvData;
    m_ptrMap[DSP2ARM_CONTROLCODE_SMV_CALC_RT] = &m_smvCalcData;
    m_ptrMap[DSP2ARM_CONTROLCODE_SMV_GEN_RT] = &m_smvGenCalcData;
    m_ptrMap[DSP2ARM_CONTROLCODE_MSG_DISP_RT] = &m_msgDisp;
    m_ptrMap[DSP2ARM_CONTROLCODE_ERROR] = &m_err;

    m_lengthMap[DSP2ARM_CONTROLCODE_INFO] = sizeof(DspInfo);
    //lengthMap[DSP2ARM_CONTROLCODE_SMV_DATA] = sizeof(SmvData);
    m_lengthMap[DSP2ARM_CONTROLCODE_SMV_CALC_RT] = sizeof(SMV_DATA_CALC_RT);
    m_lengthMap[DSP2ARM_CONTROLCODE_SMV_GEN_RT] = sizeof(SMV_GEN_DATA_CALC_RT);        
    m_lengthMap[DSP2ARM_CONTROLCODE_MSG_DISP_RT] =11*sizeof(float);
    m_lengthMap[DSP2ARM_CONTROLCODE_ERROR] = sizeof(Error);
    
    m_lockMap[DSP2ARM_CONTROLCODE_INFO] = &m_wrLocks[0];
    //lockMap[DSP2ARM_CONTROLCODE_SMV_DATA] = &wr_locks[1];
    m_lockMap[DSP2ARM_CONTROLCODE_SMV_CALC_RT] = &m_wrLocks[2];
    m_lockMap[DSP2ARM_CONTROLCODE_SMV_GEN_RT] = &m_wrLocks[3];
    m_lockMap[DSP2ARM_CONTROLCODE_MSG_DISP_RT] = &m_wrLocks[4];
    m_lockMap[DSP2ARM_CONTROLCODE_ERROR] = &m_wrLocks[5];
}
///
/// @brief
///    取得读取的数据个数
/// @return
///    UINT32
///
UINT32 CDspDataSet::GetDataCount()
{
    return m_nDataCount;
}

///
/// @brief
///    写入数据
/// @param[in] 
///    nCommandID               命令码
///    pData                    数据源指针（对应于相应命令的数据）
///    nDataLength              数据长度
/// @return
///    bool                     成功返回true
///                             数据长度与对应命令的数据不相符返回false
///
bool CDspDataSet::Write( UINT8 nCommandID, void *pData, UINT32 nDataLength )
{
    AutoLock autoLock(*m_lockMap[nCommandID]);
    if(nDataLength > m_lengthMap[nCommandID])
    {
        return false;
    }
    memcpy(m_ptrMap[nCommandID], pData, nDataLength);
    /*if(nCommandID == DSP2ARM_CONTROLCODE_SMV_GEN_RT)
    {
        m_smvGenSize = nDataLength/sizeof(float);
    }*/
    m_nDataCount++;
    return true;
}

///
/// @brief
///    返回Error结构体数据（设置DSP参数后返回的错误信息）
/// @param[out]
///    Error
///
void CDspDataSet::GetError(Error &err)
{
    AutoLock autoLock(*m_lockMap[DSP2ARM_CONTROLCODE_ERROR]);
    err = m_err;
}

///
/// @brief
///    返回DspInfo信息
/// @param[out]
///    DspInfo
///
void CDspDataSet::GetDspInfo(DspInfo &dspInfo)
{
    AutoLock autoLock(*m_lockMap[DSP2ARM_CONTROLCODE_INFO]);
    dspInfo = m_dspInfo;
}

///
/// @brief
///    返回Dsp计算结果信息（谐波，功率等）
/// @param[out]
///    SMV_DATA_CACL_RT
///
void CDspDataSet::GetCalcData(SMV_DATA_CALC_RT &smvCalcData)
{
    AutoLock autoLock(*m_lockMap[DSP2ARM_CONTROLCODE_SMV_CALC_RT]);
    smvCalcData = m_smvCalcData;
}

///
/// @brief
///    返回波形生成数据
/// @param[out] 
///    smvGenData           
/// @return
///    bool                 成功返回true
///                         缓冲区不够返回false
///
void CDspDataSet::GetGenCalcData(SMV_GEN_DATA_CALC_RT &smvGenData )
{
    AutoLock autoLock(*m_lockMap[DSP2ARM_CONTROLCODE_SMV_GEN_RT]);
    smvGenData = m_smvGenCalcData;
}
///
/// @brief
///    返回报文离散度数据
/// @param[out] 
///    buf                  输出缓冲区
///    bufLength            输出缓冲区大小（字节）
/// @return
///    bool                 成功返回true
///                         缓冲区不够返回false
///
bool CDspDataSet::GetMsgDisp( float *buf, UINT32 bufLength)
{
    AutoLock autoLock(*m_lockMap[DSP2ARM_CONTROLCODE_MSG_DISP_RT]);
    if(bufLength < 11 * sizeof(float))
    {
        return false;
    }
    memcpy(buf, m_ptrMap[DSP2ARM_CONTROLCODE_MSG_DISP_RT], 11 * sizeof(float));
    //size = 11;
    return true;
}