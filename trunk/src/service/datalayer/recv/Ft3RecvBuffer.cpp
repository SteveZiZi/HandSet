/// @file
///
/// @brief
///     ����Ft3���ݽ��ջ���������ȡ������������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.3.27
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "Ft3RecvBuffer.h"

//����ft3֡ǰ�Ƿ���ʱ�����ʱ�������fpga�汾0.17֮����е�
#define FT3_USE_TIME_STAMP 1

///
/// @brief
///    ���캯��
/// @param[in] 
///     pData ���ݻ�������ַ
/// @param[in]
///     nBufferLen ��������С
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
///    ��ȡ����������֡��Ŀ
///
/// @return
///     ���ػ�����������֡����Ŀ
///
uint32_t CFt3RecvBuffer::GetBufferFrameCount()
{
    return m_nFrameCount;
}

///
/// @brief
///    ��ȡ����������֡����
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
///    ��ȡһ������֡����
///
/// @param[in] 
///     nIndex  Ҫ��ȡ������֡����ţ���0��ʼ
/// @parma[in]
///     pBuffer ����Ҫ��ȡ������֡������
/// @param[in] 
///     nBufferLen �������Ĵ�С
/// @param[out]
///     nFrameLen  ���滺������һ������֡�ĳ���
///
/// @return
///     true��ʾ����֡��ȡ�ɹ���false��ʾʧ��
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
///    ��ȡ����������֡�Ĳ�����
///
FT3_BAUD_RATE CFt3RecvBuffer::GetBufferFrameBaudRate()
{
    FT3_BAUD_RATE enBaudRate = FT3_BAUD_RATE_UNKOWN;
    uint32_t nBaud = m_stBufferHeader.Ft3Header.stCtrlWord.CtrlBit.bBaudRate;
    //ö�����Ͷ������ֵ��FT3�������в�������ֵ����ͬ��
    enBaudRate = static_cast<FT3_BAUD_RATE>(nBaud);
    return enBaudRate;
}

///
/// @brief
///    ��ȡ���������ݵĲ�����
///
uint32_t CFt3RecvBuffer::GetBufferFrameSampleRate()
{
    uint32_t nSampleRate = 0;
    FT3_SAMP_RATE enSampleRate = FT3_SAMP_RATE_4K;
    nSampleRate = m_stBufferHeader.Ft3Header.stCtrlWord.CtrlBit.bSampleRate;
    //ö�����Ͷ������ֵ��FT3�������в�������ֵ����ͬ��
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
///    ��ȡ�Ƿ�ʱ
///
/// @return
///     ��ʱ����true
///
bool CFt3RecvBuffer::IsTimeOut()
{
    return m_stBufferHeader.Ft3Header.nIsTimeOut > 0 ? true: false;
}

///
/// @brief
///    ���û�����ͷ����Ϣ
///
void CFt3RecvBuffer::SetBufferHeader( const BufferHeader& rBufferHeader )
{
    m_stBufferHeader = rBufferHeader;
}

///
/// @brief
///    ��ȡ������һ֡����֡����
///    ///
/// @brief
///    ��ȡ������һ֡����֡����
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
///    ���ݻ�����ͷ��Ϣ��ȡ����Ft3����֡����
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
///    ��ȡһ������֡�ĵ�ַ������������֡���ݳ���
///
/// @param[in] 
///     nIndex  Ҫ��ȡ������֡����ţ���0��ʼ
/// @param[out]
///     pFrameAddr ����Ҫ��ȡ������֡�ĵ�ַ
/// @param[out]
///     nFrameLen  ʵ������֡�ĳ���
///
/// @return
///     true��ʾ����֡��ַ��ȡ�ɹ���false��ʾʧ��
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
///    ��ȡһ������֡ʱ�����Ϣ
///
/// @param[in] 
///     nIndex  Ҫ��ȡ������֡����ţ���0��ʼ
/// @param[out]
///     stTimeStamp ����Ҫ��ȡ������֡��ʱ��
///
/// @return
///     true��ʾʱ�����ȡ�ɹ���false��ʾʧ��
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
///    ��ȡһ������֡ʱ�����Ϣ
///
/// @param[in] 
///     nIndex  Ҫ��ȡ������֡����ţ���0��ʼ
/// @param[out]
///     stFrameTimeStamp ����Ҫ��ȡ������֡��ʱ��
///
/// @return
///     true��ʾʱ�����ȡ�ɹ���false��ʾʧ��
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
