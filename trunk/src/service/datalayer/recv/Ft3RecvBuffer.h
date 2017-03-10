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

#pragma once
#include "src\include\stdint.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\include\datatypes.h"
#include "FrameRecvStruct.h"

class CFt3RecvBuffer
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
    CFt3RecvBuffer(uint8_t* pData, uint32_t nFrameCount, uint32_t nBufferLen);
    ~CFt3RecvBuffer(void);

public:
    ///
    /// @brief
    ///    ��ȡ����������֡��Ŀ
    ///
    uint32_t GetBufferFrameCount();

    ///
    /// @brief
    ///    ��ȡ����������֡����
    ///
    enumFrameType GetBufferFrameType();

    ///
    /// @brief
    ///    ��ȡ������һ֡����֡����
    ///
    uint32_t GetBufferAFrameLen();

    ///
    /// @brief
    ///    ���ݻ�����ͷ��Ϣ��ȡ����Ft3����֡����
    ///
    static uint32_t GetBufferAFrameLen(BufferHeader* pBufferHeader);

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
    ///     nFrameLen  ���滺������һ������֡�ĳ���
    ///
    /// @return
    ///     true��ʾ����֡��ȡ�ɹ���false��ʾʧ��
    ///
    bool GetBufferFrameContent(uint32_t nIndex, uint8_t *pBuffer, uint32_t nBufferLen, uint32_t& nFrameLen);

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
    ///    ��ȡһ������֡ʱ�����Ϣ
    ///
    /// @param[in] 
    ///     nIndex  Ҫ��ȡ������֡����ţ���0��ʼ
    /// @param[out]
    ///     stTimeStamp ����Ҫ��ȡ������֡��ʱ����Ϣ
    ///
    /// @return
    ///     true��ʾʱ�����ȡ�ɹ���false��ʾʧ��
    ///
    bool GetBufferFrameTimeStamp(uint32_t nIndex, UTC_TIME_t& stTimeStamp);

    ///
    /// @brief
    ///    ��ȡһ������֡ʱ�����Ϣ
    ///
    /// @param[in] 
    ///     nIndex  Ҫ��ȡ������֡����ţ���0��ʼ
    /// @param[out]
    ///     stFrameTimeStamp ����Ҫ��ȡ������֡��ʱ����Ϣ
    ///
    /// @return
    ///     true��ʾʱ�����ȡ�ɹ���false��ʾʧ��
    ///
    bool GetBufferFrameTimeStamp(uint32_t nIndex, FrameTimeStamp& stFrameTimeStamp);

    ///
    /// @brief
    ///    ��ȡ����������֡�Ĳ�����
    ///
    FT3_BAUD_RATE GetBufferFrameBaudRate();

    ///
    /// @brief
    ///    ��ȡ���������ݵĲ�����
    ///
    uint32_t GetBufferFrameSampleRate();

    ///
    /// @brief
    ///    ��ȡ�Ƿ�ʱ
    ///
    /// @return
    ///     ��ʱ����true
    ///
    bool IsTimeOut();

private:
    friend class CFrameRecv;
    ///
    /// @brief
    ///    ���û�����ͷ����Ϣ
    ///
    void SetBufferHeader(const BufferHeader& rBufferHeader);

private:
    uint32_t        m_nRecvBufferLen;                   ///< ���ݻ���������
    uint8_t*        m_pDataBuffer;                      ///< ָ���ڲ�ʵ�����ݵĵ�ַָ��
    uint32_t        m_nFrameCount;                      ///< ����������֡��Ŀ
    BufferHeader    m_stBufferHeader;                   ///< ���滺����ͷ����Ϣ
};
