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

#include "stdafx.h"
#include "msgdispersioncalc.h"

///
/// @brief
///    hash���飬����ͨ������ʱ���ӳ�䱨��ʱ������䣬��ʼ��Ϊ1��ASDU
///
static uint8_t timeMsgHash1Asdu[MAX_HASH_COUNT]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,4,5,6,7,7,7,7,7,7,7,7,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9};


CMsgDisperisonCalc::CMsgDisperisonCalc(): m_ndispSize(0)
{
    memset(m_ndispArr, 0, sizeof(m_ndispArr));
    memcpy_s(m_nTimeMsgHash, MAX_HASH_COUNT, timeMsgHash1Asdu, sizeof(timeMsgHash1Asdu));
    _dispHashCreate(m_cConfig.nASDUCount);
}

CMsgDisperisonCalc::~CMsgDisperisonCalc()
{
}

///
/// @brief
///    ��һ��ʱ���ֵ���й�ϣ���ж������ĸ���Χ��
///
uint8_t CMsgDisperisonCalc::_dispHash( int64_t timeDif)
{
    int64_t nMaxTime = 250* m_cConfig.nASDUCount;
    if(timeDif >= nMaxTime)
    {
        return 10;
    }
    else if(timeDif <= -nMaxTime)
    {
        return 0;
    }
    else
    {
        uint64_t nIndex = timeDif + nMaxTime -1;
        if (nIndex < MAX_HASH_COUNT)
        {
            return m_nTimeMsgHash[nIndex];
        }
        else
        {
            return 10;
        }
    }
}

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
int32_t CMsgDisperisonCalc::_dispHashCreate( uint16_t nASDUCount )
{
    if(nASDUCount > MAX_ASDU_COUNT || nASDUCount == 0)
    {
        return -1;
    }
    memset(&m_nTimeMsgHash[0], 1, 200*nASDUCount);
    memset(&m_nTimeMsgHash[200*nASDUCount], 2, 50*nASDUCount-10);
    memset(&m_nTimeMsgHash[250*nASDUCount-10], 3, 8);
    memset(&m_nTimeMsgHash[250*nASDUCount-2], 4, 1);
    memset(&m_nTimeMsgHash[250*nASDUCount-1], 5, 1);
    memset(&m_nTimeMsgHash[250*nASDUCount], 6, 1);
    memset(&m_nTimeMsgHash[250*nASDUCount+1], 7, 8);
    memset(&m_nTimeMsgHash[250*nASDUCount+9], 8, 50*nASDUCount-10);
    memset(&m_nTimeMsgHash[300*nASDUCount-1], 9, 200*nASDUCount);
    return 0;
}

///
/// @brief
///     ���ñ���ͳ������
///
void CMsgDisperisonCalc::ResetData()
{
    memset(m_ndispArr, 0, sizeof(m_ndispArr));
    m_ndispSize = 0;
}


///
/// @brief
///    ��ȡ��ɢ��ͳ�ƽ��
///
bool CMsgDisperisonCalc::GetDispersion(float* dispArrRate, uint32_t nArraySize)
{
    if (nArraySize != DISPERISON_ARRAY_SIZE)
    {
        return false;
    }

    // ������ʰٷֱ�
    for(uint32_t i=0; i<DISPERISON_ARRAY_SIZE; i++)
    {
        if(m_ndispArr[i] == 0)
        {
            dispArrRate[i] = 0.0f;
        }
        else
        {
            dispArrRate[i] = mpysp_i(divsp_i((float)m_ndispArr[i],(float)m_ndispSize),100);
        }
    }
    return true;
}

///
/// @brief
///    ����һ��ʱ�����Ϣ
///
void CMsgDisperisonCalc::ParsingFrameTimeStamp( FrameTimeStamp& stFrameTimeStamp )
{
    if (m_stLastFrameTimeStamp.ts_sec == 0 && m_stLastFrameTimeStamp.ts_usec == 0)
    {
        m_stLastFrameTimeStamp = stFrameTimeStamp;
        return;
    }

    int64_t nTimeInterval = stFrameTimeStamp - m_stLastFrameTimeStamp;
    int64_t defaultTimeDif = m_cConfig.nASDUCount* 1000000 / m_cConfig.nSamplingFreq;

    // ����ʱ���
    int64_t difTime = 0;
    if(nTimeInterval > defaultTimeDif && nTimeInterval - defaultTimeDif > 2147483647)
    {
        difTime = 2147483647;
    }
    else if(nTimeInterval < defaultTimeDif && defaultTimeDif - nTimeInterval > 2147483648)
    {
        difTime = -2147483647;
    }
    else
    {
        difTime = nTimeInterval - defaultTimeDif;
    }

    // ����ʱ�����hash�±�
    int8_t hash = _dispHash(difTime);

    // ��Ӧ�������+1
    m_ndispArr[hash]++;

    //����ɢ�����ݼ���
    m_ndispSize++;

    //�������һ��ʱ���
    m_stLastFrameTimeStamp = stFrameTimeStamp;
}

