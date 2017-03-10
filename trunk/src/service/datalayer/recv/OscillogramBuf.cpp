/// @file
///
/// @brief
///     定义/管理ping/pong两个缓冲区
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.12.5
///
/// 修改历史：
///


#include "stdafx.h"
#include "OscillogramBuf.h"

///
/// @brief
///    添加一个数据
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
        //当前缓冲没有满，则添加对应数据
        _FillBuffer(stValue);
    }
    else
    {
        //当前缓冲已经满了，切换到另外缓冲区,然后添加数据
        m_bBufFul[m_nCurBuf] = true;
        m_nCurBuf = (m_nCurBuf + 1) % 2;
        //将另外一个缓冲区索引置0
        m_nBufIndex[m_nCurBuf] = 0;
        m_bBufFul[m_nCurBuf] = false;
        //添加数据
        _FillBuffer(stValue);
    }
    return true;
}

///
/// @brief
///    判断某个缓冲区是否满
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
///    获取一次采样数据
///
/// @param[out] nSmpCount 保存采样点个数
/// @param[out] nBufIndex 缓冲区中当前获取数据的缓冲区编号（0/1）
///
/// @return
///     bool true表示获取成功
///
/// @note
///     返回true时，函数已经获取到了缓冲区的锁，在调用者使用缓冲区数据后，需要
///     手工调用ReleaseBufferLock（）对缓冲区解锁，返回false表示获取数据失败
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
///    填充一个SMV_VALUE到缓存
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
    //如果放入的是缓冲区第一个采样值，则记录采样值编号
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
