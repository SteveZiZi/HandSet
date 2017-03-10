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

#pragma once

#include <map>
#include "src\include\stdint.h"
#include "FrameRecvStruct.h"
#include <WinSock2.h>

#pragma comment(lib,"Ws2.lib")

//��̫������ֵ������֡�е�λ�ã�ƫ��ֵ��,�����vlan������Ҫ����4���ֽڵ�ƫ��
static const uint32_t ETHERNET_TYPE_INDEX = 12;
static const uint32_t ETHERNET_9_1_2_TAG_INDEX = 22;
static const uint32_t ETHERNET_VLAN_OFFSET = 4;

static const uint16_t ETHERNET_TYPE_GOOSE = 0x88B8;
static const uint16_t ETHERNET_TYPE_9_1_2 = 0x88BA;
static const uint16_t ETHERNET_TYPE_VLAN = 0x8100;
static const uint8_t  TAG_9_1 = 0x80;
static const uint8_t  TAG_9_2 = 0x60;

class CNetRecvBuffer
{
public:
    ///
    /// @brief
    ///    ���캯��
    /// @param[in] 
    ///     pData ���ݻ�������ַ
    /// @param[in]
    ///     nBufferLen ��������С
    ///
    CNetRecvBuffer(uint8_t* pData, uint32_t nFrameCount, uint32_t nBufferLen);
    ~CNetRecvBuffer(void);


public:
    ///
    /// @brief
    ///    ��ȡ����������֡��Ŀ
    ///
    uint32_t GetBufferFrameCount();

    ///
    /// @brief
    ///    ��ȡĳ������֡����Ϣ
    ///
    bool GetBufferFrameInfo(uint32_t nIndex, FrameInfo& stFrameInfo);

    ///
    /// @brief
    ///    ��ȡһ������֡����
    ///
    /// @param[in] 
    ///     nIndex  Ҫ��ȡ������֡����ţ���0��ʼ
    /// @param[in]
    ///     pBuffer ����Ҫ��ȡ������֡������
    /// @param[in] 
    ///     nBufferLen �������Ĵ�С
    /// @param[out]
    ///     nFrameLen   ʵ������֡�ĳ���
    ///
    /// @return
    ///     true��ʾ����֡��ȡ�ɹ���false��ʾʧ��
    ///
    bool GetBufferFrameContent(uint32_t nIndex, uint8_t *pBuffer, uint32_t nBufferLen, uint32_t &nFrameLen);

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
    bool GetBufferFrameAddr(uint32_t nIndex, uint8_t** pFrameAddr, uint32_t& nFrameLen);

    ///
    /// @brief
    ///    ��ȡһ������֡������
    ///
    static enumFrameType GetFrameType(uint8_t* pFrame, uint32_t nFrameLen);

private:
    ///
    /// @brief
    ///    �������������ݣ���ȡ������Ϣ
    ///
    void _AnalisysBuffer();

private:
    uint8_t*        m_pDataBuffer;                      ///< ָ�����ڴ��ڲ�ʵ�����ݵĵ�ַָ��
    uint32_t        m_nRecvBufferLen;                   ///< �����ڴ����ݻ������ܳ���
    bool            m_bAnalysised;                      ///< �������������Ƿ񱻽�����
    NET_MODE        m_enNetMode;                        ///< ��ȡ��ǰ����������ģʽ��������̽�⻹�ǹ��˺�Ľ��
    enumFrameType   m_enFrameType;                      ///< ����������������
    uint32_t        m_nFrameCount;                      ///< ����֡��Ŀ
    FrameInfo*      m_pArrayFrameInfo;                  ///< ��������֡��λ�ã�����ʱ�����Ϣ
};
