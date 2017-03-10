/// @file
///
/// @brief
///     管理Ft3数据接收缓冲区，获取缓冲区数据信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.3.27
///
/// 修改历史：
///

#include "StdAfx.h"
#include "Ft3RecvBuffer.h"

//定义ft3帧前是否有时间戳，时间戳是在fpga版本0.17之后才有的
#define FT3_USE_TIME_STAMP 1

///
/// @brief
///    构造函数
/// @param[in] 
///     pData 数据缓冲区地址
/// @param[in]
///     nBufferLen 缓冲区大小
///
CFt3RecvBuffer::CFt3RecvBuffer( uint8_t* pData,  uint32_t nFrameCount,uint32_t nBufferLen ) : m_nRecvBufferLen(nBufferLen)
                                                                        , m_pDataBuffer(pData)
                                                                        , m_nFrameCount(nFrameCount)
{

}

CFt3RecvBuffer::~CFt3RecvBuffer(void)
{
}


///
/// @brief
///    获取缓冲区数据帧数目
///
/// @return
///     返回缓冲区中数据帧的数目
///
uint32_t CFt3RecvBuffer::GetBufferFrameCount()
{
    return m_nFrameCount;
}

///
/// @brief
///    获取缓冲区数据帧类型
///
enumFrameType CFt3RecvBuffer::GetBufferFrameType()
{
    Ft3CtlWordRecv stRecvCtlWorl = m_stBufferHeader.Ft3Header.stCtrlWord;
    if (_FT3_TYPE_NANRUI == stRecvCtlWorl.CtrlBit.bFrameType)
    {
        return FRAME_TYPE_FT3_NANRUI;
    }
    else if (_FT3_TYPE_GUOWANG == stRecvCtlWorl.CtrlBit.bFrameType)
    {
        return FRAME_TYPE_FT3_GUOWANG;
    }
    return FRAME_TYPE_UNKOWN;
}

///
/// @brief
///    获取一个数据帧内容
///
/// @param[in] 
///     nIndex  要获取的数据帧的序号，从0开始
/// @parma[in]
///     pBuffer 保存要获取的数据帧的内容
/// @param[in] 
///     nBufferLen 缓冲区的大小
/// @param[out]
///     nFrameLen  保存缓冲区中一个数据帧的长度
///
/// @return
///     true表示数据帧获取成功，false表示失败
///
bool CFt3RecvBuffer::GetBufferFrameContent( uint32_t nIndex, uint8_t *pBuffer, uint32_t nBufferLen, uint32_t& nFrameLen )
{
    uint32_t nTimeStampLen = 0;
#if FT3_USE_TIME_STAMP
    nTimeStampLen = FT3_TIME_STAMP_LEN;
#endif
    nFrameLen = GetBufferAFrameLen();
    if (NULL == m_pDataBuffer || nIndex >= m_nFrameCount || (nIndex + 1) * (nFrameLen + nTimeStampLen) > m_nRecvBufferLen)
    {
        return false;
    }
    uint8_t* pSrc = (m_pDataBuffer + nIndex * (nFrameLen + nTimeStampLen) + nTimeStampLen);
    memcpy_s(pBuffer, nBufferLen, pSrc, nFrameLen);
    return true;
}

///
/// @brief
///    获取缓冲区数据帧的波特率
///
FT3_BAUD_RATE CFt3RecvBuffer::GetBufferFrameBaudRate()
{
    FT3_BAUD_RATE enBaudRate = FT3_BAUD_RATE_UNKOWN;
    uint32_t nBaud = m_stBufferHeader.Ft3Header.stCtrlWord.CtrlBit.bBaudRate;
    //枚举类型定义的数值和FT3控制字中采样率数值是相同的
    enBaudRate = static_cast<FT3_BAUD_RATE>(nBaud);
    return enBaudRate;
}

///
/// @brief
///    获取缓冲区数据的采样率
///
uint32_t CFt3RecvBuffer::GetBufferFrameSampleRate()
{
    uint32_t nSampleRate = 0;
    FT3_SAMP_RATE enSampleRate = FT3_SAMP_RATE_4K;
    nSampleRate = m_stBufferHeader.Ft3Header.stCtrlWord.CtrlBit.bSampleRate;
    //枚举类型定义的数值和FT3控制字中采样率数值是相同的
    enSampleRate = static_cast<FT3_SAMP_RATE>(nSampleRate);
    switch (enSampleRate)
    {
    case FT3_SAMP_RATE_4K:
        nSampleRate = 4000;
        break;
    case FT3_SAMP_RATE_5K:
        nSampleRate = 5000;
        break;
    case FT3_SAMP_RATE_8K:
        nSampleRate = 8000;
        break;
    case FT3_SAMP_RATE_10K:
        nSampleRate = 10000;
        break;
    case FT3_SAMP_RATE_12P8K:
        nSampleRate = 12800;
        break;
    default:
        break;
    }
    return nSampleRate;
}

///
/// @brief
///    获取是否超时
///
/// @return
///     超时返回true
///
bool CFt3RecvBuffer::IsTimeOut()
{
    return m_stBufferHeader.Ft3Header.nIsTimeOut > 0 ? true: false;
}

///
/// @brief
///    设置缓冲区头部信息
///
void CFt3RecvBuffer::SetBufferHeader( const BufferHeader& rBufferHeader )
{
    m_stBufferHeader = rBufferHeader;
}

///
/// @brief
///    获取缓冲区一帧数据帧长度
///    ///
/// @brief
///    获取缓冲区一帧数据帧长度
///
uint32_t CFt3RecvBuffer::GetBufferAFrameLen()
{
    uint32_t nRet = FT3_FRAME_LEN[_FT3_TYPE_GUOWANG];
    switch(GetBufferFrameType())
    {
    case FRAME_TYPE_FT3_GUOWANG:
        nRet = FT3_FRAME_LEN[_FT3_TYPE_GUOWANG];
        break;
    case FRAME_TYPE_FT3_NANRUI:
        nRet = FT3_FRAME_LEN[_FT3_TYPE_NANRUI];
        break;
    default:
        break;
    }
    return nRet;
}

///
/// @brief
///    根据缓冲区头信息获取单个Ft3数据帧长度
///
uint32_t CFt3RecvBuffer::GetBufferAFrameLen( BufferHeader* pBufferHeader )
{
    if (!pBufferHeader)
    {
        return 0;
    }
    if (_FT3_TYPE_NANRUI == pBufferHeader->Ft3Header.stCtrlWord.CtrlBit.bFrameType)
    {
        return FT3_FRAME_LEN[_FT3_TYPE_NANRUI];
    }
    else if (_FT3_TYPE_GUOWANG == pBufferHeader->Ft3Header.stCtrlWord.CtrlBit.bFrameType)
    {
        return FT3_FRAME_LEN[_FT3_TYPE_GUOWANG];
    }

    return 0;
}

///
/// @brief
///    获取一个数据帧的地址，不复制数据帧内容出来
///
/// @param[in] 
///     nIndex  要获取的数据帧的序号，从0开始
/// @param[out]
///     pFrameAddr 保存要获取的数据帧的地址
/// @param[out]
///     nFrameLen  实际数据帧的长度
///
/// @return
///     true表示数据帧地址获取成功，false表示失败
///
bool CFt3RecvBuffer::GetBufferFrameAddr( uint32_t nIndex, uint8_t** pFrameAddr, uint32_t& nFrameLen )
{
    uint32_t nTimeStampLen = 0;
#if FT3_USE_TIME_STAMP
    nTimeStampLen = FT3_TIME_STAMP_LEN;
#endif
    nFrameLen = GetBufferAFrameLen();
    if (NULL == m_pDataBuffer || nIndex >= m_nFrameCount || (nIndex + 1) * (nFrameLen + nTimeStampLen) > m_nRecvBufferLen)
    {
        return false;
    }
    uint8_t* pSrc = (m_pDataBuffer + nIndex * (nFrameLen + nTimeStampLen) + nTimeStampLen);
    *pFrameAddr = pSrc;
    return true;
}

///
/// @brief
///    获取一个数据帧时间戳信息
///
/// @param[in] 
///     nIndex  要获取的数据帧的序号，从0开始
/// @param[out]
///     stTimeStamp 保存要获取的数据帧的时间
///
/// @return
///     true表示时间戳获取成功，false表示失败
///
bool CFt3RecvBuffer::GetBufferFrameTimeStamp( uint32_t nIndex, UTC_TIME_t& stTimeStamp )
{
    uint32_t nTimeStampLen = 0;
#if FT3_USE_TIME_STAMP
    nTimeStampLen = FT3_TIME_STAMP_LEN;
#else
    return false;
#endif
    uint32_t nFrameLen = GetBufferAFrameLen();
    if (NULL == m_pDataBuffer || nIndex >= m_nFrameCount || (nIndex + 1) * (nFrameLen + nTimeStampLen) > m_nRecvBufferLen)
    {
        return false;
    }
    uint8_t* pSrc = (m_pDataBuffer + nIndex * (nFrameLen + nTimeStampLen));
    stTimeStamp = *(reinterpret_cast<UTC_TIME_t*>(pSrc));
    return true;
}

///
/// @brief
///    获取一个数据帧时间戳信息
///
/// @param[in] 
///     nIndex  要获取的数据帧的序号，从0开始
/// @param[out]
///     stFrameTimeStamp 保存要获取的数据帧的时间
///
/// @return
///     true表示时间戳获取成功，false表示失败
///
bool CFt3RecvBuffer::GetBufferFrameTimeStamp( uint32_t nIndex, FrameTimeStamp& stFrameTimeStamp )
{
    UTC_TIME_t stUtcTime;
    if (GetBufferFrameTimeStamp(nIndex, stUtcTime))
    {
        stFrameTimeStamp.ts_sec = stUtcTime.uSecondSinceEpoch;
        stFrameTimeStamp.ts_usec = stUtcTime.uFractionOfSecond;
        return true;
    }
    return false;
}
