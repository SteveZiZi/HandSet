/// @file
///
/// @brief
///     网络数据接收缓冲区信息获取
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.3.26
///
/// 修改历史：
///


#include "StdAfx.h"
#include "NetRecvBuffer.h"
#include "src\service\datalayer\include\config.h"

///
/// @brief
///    构造函数
/// @param[in] 
///     pData 数据缓冲区地址
/// @param[in]
///     nBufferLen 缓冲区大小
///
CNetRecvBuffer::CNetRecvBuffer( uint8_t* pData, uint32_t nFrameCount, uint32_t nBufferLen ) : m_pDataBuffer(pData)
                                                                      , m_nRecvBufferLen(nBufferLen)
                                                                      , m_bAnalysised(false)
                                                                      , m_enFrameType(FRAME_TYPE_UNKOWN)
                                                                      , m_enNetMode(_RECV_NONE)
                                                                      , m_nFrameCount(nFrameCount)
                                                                      , m_pArrayFrameInfo(NULL)
{

}

CNetRecvBuffer::~CNetRecvBuffer(void)
{
    if (NULL != m_pArrayFrameInfo)
    {
        delete[] m_pArrayFrameInfo;
        m_pArrayFrameInfo = NULL;
    }
}

///
/// @brief
///    分析缓冲区信息，生成数据帧信息映射表
///
void CNetRecvBuffer::_AnalisysBuffer()
{
     if (!m_bAnalysised)
     {
         //缓冲区信息没有被分析过，分析数据
         if (NULL == m_pDataBuffer)
         {
             RETAILMSG(OUTPUT_LOG_MSG, (_T("NetRecvBuffer::_AnalisysBuffer(): Null Buffer Pointer \n")));
             return;
         }
         if (NULL == m_pArrayFrameInfo)
         {
             m_pArrayFrameInfo = new FrameInfo[m_nFrameCount];
         }

         uint8_t *pBufferEnd = m_pDataBuffer + m_nRecvBufferLen;
         FrameInfo stFrameInfo = {0};
         BUFFER_PCAP_PACKET_HEADER *pHeader = reinterpret_cast<BUFFER_PCAP_PACKET_HEADER*>(m_pDataBuffer);
         stFrameInfo.nOffset = sizeof(BUFFER_PCAP_PACKET_HEADER);
         stFrameInfo.capLen = pHeader->capLen;
         stFrameInfo.timeStamp.ts_sec = pHeader->timeStamp.ts_sec;
         stFrameInfo.timeStamp.ts_usec = pHeader->timeStamp.ts_usec;
         stFrameInfo.nIndex = 0;
         m_pArrayFrameInfo[0] = stFrameInfo;
         uint32_t nAddr = 0;
         for (uint32_t i = 1; i < m_nFrameCount; ++i)
         {
             //
             // 缓冲区内存是16位对齐，需要对地址进行对齐处理
             //
             nAddr = reinterpret_cast<uint32_t>(pHeader) + pHeader->capLen + sizeof(BUFFER_PCAP_PACKET_HEADER);
             pHeader = reinterpret_cast<BUFFER_PCAP_PACKET_HEADER*> ( nAddr % 2 ? nAddr + 1 : nAddr);
             if (reinterpret_cast<uint32_t>(pHeader) >= reinterpret_cast<uint32_t>(pBufferEnd))
             {
                 RETAILMSG(OUTPUT_LOG_MSG, (_T("NetRecvBuffer::_AnalisysBuffer(): Pointer Beyond BufferEnd \r\n")));
                 return;
             }
             nAddr = stFrameInfo.nOffset + stFrameInfo.capLen + sizeof(BUFFER_PCAP_PACKET_HEADER);
             nAddr = nAddr % 2 ? nAddr + 1 : nAddr;
             stFrameInfo.nOffset = nAddr;
             stFrameInfo.capLen = pHeader->capLen;
             stFrameInfo.timeStamp.ts_sec = pHeader->timeStamp.ts_sec;
             stFrameInfo.timeStamp.ts_usec = pHeader->timeStamp.ts_usec;
             stFrameInfo.nIndex  = i;

             m_pArrayFrameInfo[i] = stFrameInfo;
         }
     }
     m_bAnalysised = true;
}

///
/// @brief
///    获取缓冲区数据帧数目
///
/// @return
///     返回缓冲区数据帧数目
///
uint32_t CNetRecvBuffer::GetBufferFrameCount()
{
    return m_nFrameCount;
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
///     nFrameLen   实际数据帧的长度
///
/// @return
///     true表示数据帧获取成功，false表示失败
///
bool CNetRecvBuffer::GetBufferFrameContent( uint32_t nIndex, uint8_t *pBuffer, uint32_t nBufferLen, uint32_t &nFrameLen )
{
    if (NULL == m_pDataBuffer || nIndex >= m_nFrameCount)
    {
        return false;
    }
    if (!m_bAnalysised)
    {
        _AnalisysBuffer();
    }

    if (m_pArrayFrameInfo && m_pArrayFrameInfo[nIndex].nIndex == nIndex)
    {
        uint32_t nOffset = 0;
        nOffset = m_pArrayFrameInfo[nIndex].nOffset;
        uint8_t *pBufferSrc = reinterpret_cast<uint8_t*>(reinterpret_cast<uint32_t>(m_pDataBuffer) + nOffset);
        if ( m_pArrayFrameInfo[nIndex].capLen > NET_MAX_FRAME_LEN 
            ||reinterpret_cast<uint32_t>(pBufferSrc) >= reinterpret_cast<uint32_t>(m_pDataBuffer) + m_nRecvBufferLen)
        {
            return false;
        }
        nFrameLen = m_pArrayFrameInfo[nIndex].capLen;
        memcpy_s(pBuffer, nBufferLen, pBufferSrc, nFrameLen);
        return true;
    }
    return false;
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
bool CNetRecvBuffer::GetBufferFrameAddr( uint32_t nIndex, uint8_t** pFrameAddr, uint32_t& nFrameLen )
{
    if (NULL == m_pDataBuffer || nIndex >= m_nFrameCount)
    {
        return false;
    }
    if (!m_bAnalysised)
    {
        _AnalisysBuffer();
    }


    if (m_pArrayFrameInfo && m_pArrayFrameInfo[nIndex].nIndex == nIndex)
    {
        uint32_t nOffset = 0;
        nOffset = m_pArrayFrameInfo[nIndex].nOffset;
        uint8_t *pBufferSrc = reinterpret_cast<uint8_t*>(reinterpret_cast<uint32_t>(m_pDataBuffer) + nOffset);
        if ( m_pArrayFrameInfo[nIndex].capLen > NET_MAX_FRAME_LEN 
            ||reinterpret_cast<uint32_t>(pBufferSrc) >= reinterpret_cast<uint32_t>(m_pDataBuffer) + m_nRecvBufferLen)
        {
            return false;
        }
        nFrameLen = m_pArrayFrameInfo[nIndex].capLen;
        *pFrameAddr = pBufferSrc;
        return true;
    }
    return false;
}

///
/// @brief
///    获取某个数据帧的信息
///
/// @param[in] nIndex       要获取的数据帧序号
/// @param[out] stFrameInfo` 获取信息保存的结构体
///
/// @return
///     true表示获取信息成功
///
bool CNetRecvBuffer::GetBufferFrameInfo( uint32_t nIndex, FrameInfo& stFrameInfo )
{
    if (nIndex >= m_nFrameCount)
    {
        return false;
    }
    if (!m_bAnalysised)
    {
        _AnalisysBuffer();
    }

    if (m_pArrayFrameInfo && m_pArrayFrameInfo[nIndex].nIndex == nIndex)
    {
        stFrameInfo.nIndex    = nIndex;
        stFrameInfo.capLen    = m_pArrayFrameInfo[nIndex].capLen;
        stFrameInfo.nOffset   = m_pArrayFrameInfo[nIndex].nOffset;
        stFrameInfo.timeStamp = m_pArrayFrameInfo[nIndex].timeStamp;
        return true;
    }
    return false;
}

///
/// @brief
///    获取一个数据帧的类型
///
enumFrameType CNetRecvBuffer::GetFrameType( uint8_t* pFrame, uint32_t nFrameLen )
{
    enumFrameType enFrameType = FRAME_TYPE_UNKOWN;
    if (nFrameLen < ETHERNET_TYPE_INDEX + sizeof(uint16_t))
    {
        return enFrameType;
    }
    uint16_t nEtherType = ntohs(*reinterpret_cast<uint16_t*>(reinterpret_cast<uint32_t>(pFrame) + ETHERNET_TYPE_INDEX));
    bool bHasVlan = false;
    if (ETHERNET_TYPE_VLAN == nEtherType)
    {
        bHasVlan = true;
        // 加ETHERNET_VLAN_OFFSET是因为有vlan标签
        nEtherType = ntohs(*reinterpret_cast<uint16_t*>(reinterpret_cast<uint32_t>(pFrame) + ETHERNET_TYPE_INDEX + ETHERNET_VLAN_OFFSET));
    }

    if (ETHERNET_TYPE_GOOSE == nEtherType)          // GOOSE
    {
        enFrameType = FRAME_TYPE_NET_GOOSE;
    }
    else if (ETHERNET_TYPE_9_1_2 == nEtherType)    // 有可能是是9-1 或者9-2
    {
        uint8_t nTpye912 = *reinterpret_cast<uint8_t*>(reinterpret_cast<uint32_t>(pFrame) + ETHERNET_9_1_2_TAG_INDEX + (bHasVlan ? ETHERNET_VLAN_OFFSET : 0));
        if (TAG_9_1 == nTpye912)
        {
            enFrameType = FRAME_TYPE_NET_9_1;
        }
        else if (TAG_9_2 == nTpye912)
        {
            enFrameType = FRAME_TYPE_NET_9_2;
        }
    }
    return enFrameType;
}
