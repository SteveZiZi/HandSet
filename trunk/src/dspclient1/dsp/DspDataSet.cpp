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
///    ȡ�ö�ȡ�����ݸ���
/// @return
///    UINT32
///
UINT32 CDspDataSet::GetDataCount()
{
    return m_nDataCount;
}

///
/// @brief
///    д������
/// @param[in] 
///    nCommandID               ������
///    pData                    ����Դָ�루��Ӧ����Ӧ��������ݣ�
///    nDataLength              ���ݳ���
/// @return
///    bool                     �ɹ�����true
///                             ���ݳ������Ӧ��������ݲ��������false
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
///    ����Error�ṹ�����ݣ�����DSP�����󷵻صĴ�����Ϣ��
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
///    ����DspInfo��Ϣ
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
///    ����Dsp��������Ϣ��г�������ʵȣ�
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
///    ���ز�����������
/// @param[out] 
///    smvGenData           
/// @return
///    bool                 �ɹ�����true
///                         ��������������false
///
void CDspDataSet::GetGenCalcData(SMV_GEN_DATA_CALC_RT &smvGenData )
{
    AutoLock autoLock(*m_lockMap[DSP2ARM_CONTROLCODE_SMV_GEN_RT]);
    smvGenData = m_smvGenCalcData;
}
///
/// @brief
///    ���ر�����ɢ������
/// @param[out] 
///    buf                  ���������
///    bufLength            �����������С���ֽڣ�
/// @return
///    bool                 �ɹ�����true
///                         ��������������false
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