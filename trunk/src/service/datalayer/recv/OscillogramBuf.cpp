/// @file
///
/// @brief
///     ����/����ping/pong����������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.12.5
///
/// �޸���ʷ��
///


#include "stdafx.h"
#include "OscillogramBuf.h"

///
/// @brief
///    ���һ������
///
bool COscillogramBuf::AddValue( const SMV_VALUE& stValue )
{
    AutoLock autoLock(m_lockBuf);
    m_pDetectItem = stValue.pDetectItem;
    m_n11WaveSmpCount = stValue.pDetectItem->nSampleRate * (DSM_MAX_OSCILLOGRAM_WAVE_COUNT + 1)/ DSM_CONFIG_DEFAULT_ELEC_FREQUENCY;
#if DSM_DSP_CALC_128K_RESAMPLE_TO_4K
    if (stValue.pDetectItem->nSampleRate == 12800)
    {
        m_n11WaveSmpCount = 4000 * (DSM_MAX_OSCILLOGRAM_WAVE_COUNT + 1)/ DSM_CONFIG_DEFAULT_ELEC_FREQUENCY;
    }
#endif
    if (!_IsBufferFul(m_nCurBuf))
    {
        //��ǰ����û����������Ӷ�Ӧ����
        _FillBuffer(stValue);
    }
    else
    {
        //��ǰ�����Ѿ����ˣ��л������⻺����,Ȼ���������
        m_bBufFul[m_nCurBuf] = true;
        m_nCurBuf = (m_nCurBuf + 1) % 2;
        //������һ��������������0
        m_nBufIndex[m_nCurBuf] = 0;
        m_bBufFul[m_nCurBuf] = false;
        //�������
        _FillBuffer(stValue);
    }
    return true;
}

///
/// @brief
///    �ж�ĳ���������Ƿ���
///
bool COscillogramBuf::_IsBufferFul( uint32_t nBufIndex )
{
    if (m_n11WaveSmpCount > 0)
    {
        if (m_nBufIndex[nBufIndex % 2] == m_n11WaveSmpCount)
        {
            return true;
        }
        return false;
    }
    RETAILMSG(PING_PONG_BUFFER_LOG, (_T("COscillogramBuf::_IsBufferFul() Wrong BufIndex(%d) or m_n11WaveSmpCount(%d) too small \r\n"), nBufIndex, m_n11WaveSmpCount));
    return false;
}

///
/// @brief
///    ��ȡһ�β�������
///
/// @param[out] nSmpCount ������������
/// @param[out] nBufIndex �������е�ǰ��ȡ���ݵĻ�������ţ�0/1��
///
/// @return
///     bool true��ʾ��ȡ�ɹ�
///
/// @note
///     ����trueʱ�������Ѿ���ȡ���˻������������ڵ�����ʹ�û��������ݺ���Ҫ
///     �ֹ�����ReleaseBufferLock�����Ի���������������false��ʾ��ȡ����ʧ��
///
bool COscillogramBuf::GetSmpValue(uint32_t& nSmpCount, uint32_t& nBufIndex)
{
    m_lockBuf.Acquire();
    uint32_t nOtherBuf = (m_nCurBuf + 1) % 2;
    if (m_bBufFul[nOtherBuf])
    {
        nSmpCount = m_n11WaveSmpCount;
        nBufIndex = nOtherBuf;
        return true;
    }
    if (m_bBufFul[m_nCurBuf])
    {
        nSmpCount = m_n11WaveSmpCount;
        nBufIndex = m_nCurBuf;
        return true;
    }
    m_lockBuf.Release();
    return false;
}

///
/// @brief
///    ���һ��SMV_VALUE������
///
void COscillogramBuf::_FillBuffer( const SMV_VALUE& stValue)
{
    uint32_t nBufferIndex = m_nCurBuf % 2;
    uint32_t nChannelCount = stValue.nChannelCount;
    for (uint32_t i = 0; i < nChannelCount; ++i)
    {
        m_pBuf[nBufferIndex][i][m_nBufIndex[nBufferIndex]] = stValue.fSmvValue[i];
    }
    RETAILMSG(0, (_T("FillBuff(%d) Index(%d)\r\n"), nBufferIndex, m_nBufIndex[nBufferIndex]));
    //���������ǻ�������һ������ֵ�����¼����ֵ���
    if (m_nBufIndex[nBufferIndex] == 0)
    {
        m_nBufBeginSmpCnt[nBufferIndex] = stValue.nSmpCnt;
    }
    m_nBufIndex[nBufferIndex]++;
}

void COscillogramBuf::ClearData()
{
    AutoLock autoLock(m_lockBuf);
    memset(m_pBuf, 0, sizeof(m_pBuf));
    memset(m_nBufIndex, 0, sizeof(m_nBufIndex));
    memset(m_bBufFul, 0, sizeof(m_bBufFul));
    memset(m_nBufBeginSmpCnt, 0, sizeof(m_nBufBeginSmpCnt));
    m_nCurBuf  = 0;
    m_n11WaveSmpCount = 0;
    m_pDetectItem = NULL;
    RETAILMSG(PING_PONG_BUFFER_LOG, (_T("COscillogramBuf Data Clear \r\n")));
}
