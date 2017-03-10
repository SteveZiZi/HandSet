/// @file
///
/// @brief
///     定义/管理波形图数据缓冲区
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.12.5
///
/// 修改历史：
///

#ifndef __COSCILLOGRAM_BUF_H
#define __COSCILLOGRAM_BUF_H

#include "src\include\stdint.h"
#include "src\dspclient\dsp\lock.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"

#define PING_PONG_BUFFER_LOG  0

#define MAX_ONE_WAVE_POINT    256
#define CHANNEL_DATA_COUNT    (MAX_ONE_WAVE_POINT * (DSM_MAX_OSCILLOGRAM_WAVE_COUNT + 1))
#define MAX_CHANNEL_COUNT     (NET_9_2_LE_MAX_CHANNELS_PER_ASDU)

class COscillogramBuf
{
public:
    COscillogramBuf() : m_n11WaveSmpCount(0)
                      , m_nCurBuf(0)
                      , m_pDetectItem(NULL)
    {
        memset(m_pBuf, 0, sizeof(m_pBuf));
        memset(m_nBufIndex, 0, sizeof(m_nBufIndex));
        memset(m_bBufFul, 0, sizeof(m_bBufFul));
        memset(m_nBufBeginSmpCnt, 0, sizeof(m_nBufBeginSmpCnt));
    }

    ~COscillogramBuf()
    {
    }

public:
    ///
    /// @brief
    ///    添加一个数据
    ///
    bool AddValue(const SMV_VALUE& stValue);

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
    bool GetSmpValue(uint32_t& nSmpCount, uint32_t& nBufIndex);

    ///
    /// @brief
    ///    释放缓冲区锁
    ///
    void ReleaseBufferLock()
    {
        m_lockBuf.Release();
    }

    ///
    /// @brief
    ///    获取通道数目
    ///
    CBaseDetectItem* GetCtrlBlk()
    {
        return m_pDetectItem;
    }

    ///
    /// @brief
    ///    清空缓存数据
    ///
    void ClearData();

private:
    ///
    /// @brief
    ///    判断某个缓冲区是否满
    ///
    bool _IsBufferFul(uint32_t nBufIndex);

    ///
    /// @brief
    ///    填充一个SMV_VALUE到缓存
    ///
    void _FillBuffer(const SMV_VALUE& stValue);

private:
    uint32_t                m_n11WaveSmpCount;                                      ///< 当前控制块10个周波采样点数
    CBaseDetectItem*        m_pDetectItem;                                          ///< 缓存数据是哪个控制块的
    uint32_t                m_nCurBuf;                                              ///< 当前存入数据的是哪一个缓冲区
    uint32_t                m_nBufIndex[2];                                         ///< 指向各自缓冲区第一个空闲索引
    bool                    m_bBufFul[2];                                           ///< bool值表示ping/pong缓冲区是否是满的
public:
    float                   m_pBuf[2][MAX_CHANNEL_COUNT][CHANNEL_DATA_COUNT];       ///< ping/pong缓存
    Lock                    m_lockBuf;                                              ///< 访问两个缓冲区的锁
    uint32_t                m_nBufBeginSmpCnt[2];                                   ///< 每个缓冲区开始的采样序号
};


#endif //__COSCILLOGRAM_BUF_H
