/// @file
///
/// @brief
///     �������ݽ��ջ�������Ϣ��ȡ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.3.26
///
/// �޸���ʷ��
///


#include "StdAfx.h"
#include "NetRecvBuffer.h"
#include "src\service\datalayer\include\config.h"

///
/// @brief
///    ���캯��
/// @param[in] 
///     pData ���ݻ�������ַ
/// @param[in]
///     nBufferLen ��������С
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
///    ������������Ϣ����������֡��Ϣӳ���
///
void CNetRecvBuffer::_AnalisysBuffer()
{
     if (!m_bAnalysised)
     {
         //��������Ϣû�б�����������������
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
             // �������ڴ���16λ���룬��Ҫ�Ե�ַ���ж��봦��
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
///    ��ȡ����������֡��Ŀ
///
/// @return
///     ���ػ���������֡��Ŀ
///
uint32_t CNetRecvBuffer::GetBufferFrameCount()
{
    return m_nFrameCount;
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
///     nFrameLen   ʵ������֡�ĳ���
///
/// @return
///     true��ʾ����֡��ȡ�ɹ���false��ʾʧ��
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
///    ��ȡĳ������֡����Ϣ
///
/// @param[in] nIndex       Ҫ��ȡ������֡���
/// @param[out] stFrameInfo` ��ȡ��Ϣ����Ľṹ��
///
/// @return
///     true��ʾ��ȡ��Ϣ�ɹ�
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
///    ��ȡһ������֡������
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
        // ��ETHERNET_VLAN_OFFSET����Ϊ��vlan��ǩ
        nEtherType = ntohs(*reinterpret_cast<uint16_t*>(reinterpret_cast<uint32_t>(pFrame) + ETHERNET_TYPE_INDEX + ETHERNET_VLAN_OFFSET));
    }

    if (ETHERNET_TYPE_GOOSE == nEtherType)          // GOOSE
    {
        enFrameType = FRAME_TYPE_NET_GOOSE;
    }
    else if (ETHERNET_TYPE_9_1_2 == nEtherType)    // �п�������9-1 ����9-2
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
