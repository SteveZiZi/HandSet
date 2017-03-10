/// @file
///
/// @brief
///     ����/������ͼ���ݻ�����
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.12.5
///
/// �޸���ʷ��
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
    ///    ���һ������
    ///
    bool AddValue(const SMV_VALUE& stValue);

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
    bool GetSmpValue(uint32_t& nSmpCount, uint32_t& nBufIndex);

    ///
    /// @brief
    ///    �ͷŻ�������
    ///
    void ReleaseBufferLock()
    {
        m_lockBuf.Release();
    }

    ///
    /// @brief
    ///    ��ȡͨ����Ŀ
    ///
    CBaseDetectItem* GetCtrlBlk()
    {
        return m_pDetectItem;
    }

    ///
    /// @brief
    ///    ��ջ�������
    ///
    void ClearData();

private:
    ///
    /// @brief
    ///    �ж�ĳ���������Ƿ���
    ///
    bool _IsBufferFul(uint32_t nBufIndex);

    ///
    /// @brief
    ///    ���һ��SMV_VALUE������
    ///
    void _FillBuffer(const SMV_VALUE& stValue);

private:
    uint32_t                m_n11WaveSmpCount;                                      ///< ��ǰ���ƿ�10���ܲ���������
    CBaseDetectItem*        m_pDetectItem;                                          ///< �����������ĸ����ƿ��
    uint32_t                m_nCurBuf;                                              ///< ��ǰ�������ݵ�����һ��������
    uint32_t                m_nBufIndex[2];                                         ///< ָ����Ի�������һ����������
    bool                    m_bBufFul[2];                                           ///< boolֵ��ʾping/pong�������Ƿ�������
public:
    float                   m_pBuf[2][MAX_CHANNEL_COUNT][CHANNEL_DATA_COUNT];       ///< ping/pong����
    Lock                    m_lockBuf;                                              ///< ������������������
    uint32_t                m_nBufBeginSmpCnt[2];                                   ///< ÿ����������ʼ�Ĳ������
};


#endif //__COSCILLOGRAM_BUF_H
