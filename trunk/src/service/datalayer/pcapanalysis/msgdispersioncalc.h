/// @file
///
/// @brief
///     ��ɢ�ȼ���
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.11.11
///
/// �޸���ʷ��
///

#ifndef _MSG_DISPERISON_CALC_H
#define _MSG_DISPERISON_CALC_H

#include "src\include\stdint.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"

#define MAX_ASDU_COUNT          8
#define DISPERISON_ARRAY_SIZE   11

#define mpysp_i(a,b) ((a)*(b))
#define divsp_i(a,b) ((a)/(b))

///< ��������ʱ�������ɢ������hash����Ĵ�С
#define MAX_HASH_COUNT (500*MAX_ASDU_COUNT)

///
/// @brief
///    ���������ɢ��ʱ��������Ϣ�ṹ��
///
struct DispersionCalcConfig
{
    uint16_t nASDUCount;                                            ///< ASDU�ĸ���
    uint32_t nSamplingFreq;                                         ///< ����Ƶ��
    uint16_t nRatedFreq;                                            ///< �Ƶ��

    DispersionCalcConfig(): nASDUCount(1)
                          , nSamplingFreq(4000)
                          , nRatedFreq(50)
    {
    }

    bool operator == (const DispersionCalcConfig& r)
    {
        return nASDUCount == r.nASDUCount && nSamplingFreq == r.nSamplingFreq && nRatedFreq == r.nRatedFreq;
    }

    bool operator!= (const DispersionCalcConfig& rhs)
    {
        return !(*this == rhs);
    }
};

class CMsgDisperisonCalc
{
public:
    CMsgDisperisonCalc();
    ~CMsgDisperisonCalc();

public:
    ///
    /// @brief
    ///    ���ü������
    ///
    bool SetCalcConfig(const DispersionCalcConfig& stConfig)
    {
        bool bModified = false;
        if (stConfig.nASDUCount != 0 && stConfig.nASDUCount != m_cConfig.nASDUCount)
        {
            m_cConfig.nASDUCount = stConfig.nASDUCount;
            bModified = true;
        }
        if (stConfig.nRatedFreq != 0 && stConfig.nRatedFreq != m_cConfig.nRatedFreq)
        {
            m_cConfig.nRatedFreq = stConfig.nRatedFreq;
            bModified = true;
        }
        if (stConfig.nSamplingFreq != 0 && stConfig.nSamplingFreq != m_cConfig.nSamplingFreq)
        {
            m_cConfig.nSamplingFreq = stConfig.nSamplingFreq;
            bModified = true;
        }
        if (bModified)
        {
            ResetData();
            _dispHashCreate(m_cConfig.nASDUCount);
        }
        return true;
    }

    ///
    /// @brief
    ///    ��ȡ�������������Ϣ
    ///
    DispersionCalcConfig GetCalcConfig()
    {
        return m_cConfig;
    }

    ///
    /// @brief
    ///    ����һ��ʱ�����Ϣ
    ///
    void ParsingFrameTimeStamp(FrameTimeStamp& stFrameTimeStamp);

    ///
    /// @brief
    ///     ���ñ���ͳ������
    ///
    void ResetData();

    ///
    /// @brief
    ///    ��ȡ��ɢ��ͳ�ƽ��
    ///
    bool GetDispersion(float* dispArrRate, uint32_t nArraySize);

private:
    ///
    /// @brief
    ///     ���ɱ���ʱ����뱨�������ӳ��hash����
    /// @param[in] 
    ///    nASDUCount           ASDU�ĸ�����Ŀǰ���֧��8����
    ///    
    /// @return
    ///    int32_t              0���ɹ�
    ///                         -1��ASDU������������
    ///
    int32_t _dispHashCreate(uint16_t nASDUCount);

    ///
    /// @brief
    ///    ��һ��ʱ���ֵ���й�ϣ���ж������ĸ���Χ��
    ///
    uint8_t _dispHash(int64_t timeDif);


private:
    DispersionCalcConfig        m_cConfig;                          ///< ������ɢ�ȵ�������Ϣ

    uint64_t                    m_ndispArr[DISPERISON_ARRAY_SIZE];  ///< ����ɢ��������Ŀ
    uint64_t                    m_ndispSize;                        ///< ��ɢ��������������ȱ�����Ŀ��1

    uint8_t                     m_nTimeMsgHash[MAX_HASH_COUNT];     ///< hash���飬����ͨ������ʱ���ӳ�䱨��ʱ������䣬��ʼ��Ϊ1��ASDU

    FrameTimeStamp              m_stLastFrameTimeStamp;             ///< ��һ֡�ı���ʱ���
};

#endif //_MSG_DISPERISON_CALC_H