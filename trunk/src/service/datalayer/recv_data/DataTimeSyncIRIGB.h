/// @file
///
/// @brief
///     B���ʱ����
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.6.20
///
/// �޸���ʷ��
///

#pragma once
#include "Winbase.h"

#define IRIGB_GET_LSP(n)                  (n & 0x01)                    ///< ����Ԥ��
#define IRIGB_GET_LS(n)                   (n>>1 & 0x01)                 ///< �����־,0�����룬1������
#define IRIGB_GET_DSP(n)                  (n>>2 & 0x01)                 ///< ��ʱ��Ԥ��
#define IRIGB_GET_DST(n)                  (n>>3 & 0x01)                 ///< ��ʱ��ʱ��1
#define IRIGB_GET_OFFSET_SIGN(n)          (n>>4 & 0x01)                 ///< ʱ��ƫ�Ʒ���λ��0��"+", 1:"-"
#define IRIGB_GET_OFFSET(n)               (n>>5 & 0x0F)                 ///< ʱ��ƫ��Сʱ��irigbʱ��-utcʱ��
#define IRIGB_GET_OFFSET_0_5(n)           (n>>9 & 0x01)                 ///< 0������ʱ��ƫ������1����ʱ��ƫ����0.5Сʱ
#define IRIGB_GET_QUALITY(n)              (n>>10 & 0x0F)                ///< ʱ������

class CDataTimeSyncIRIGB
{
public:
    CDataTimeSyncIRIGB() : m_nLSP(0)
                         , m_nLS(0)
                         , m_nDSP(0)
                         , m_nDST(0)
                         , m_fTimeOffset(0)
                         , m_nTimeQuality(0)
    {
        memset(&m_stSysTime, 0, sizeof(m_stSysTime));
    }
    
    ~CDataTimeSyncIRIGB()
    {
    }

public:
    ///
    /// @brief
    ///    ��ȡʱ������
    ///
    SYSTEMTIME GetDateTime()
    {
        return m_stSysTime;
    }

    ///
    /// @brief
    ///    ��ȡ����Ԥ���־
    ///
    uint32_t GetLSP()
    {
        return m_nLSP;
    }

    ///
    /// @brief
    ///    ��ȡ�����־
    ///
    uint32_t GetLS()
    {
        return m_nLS;
    }

    ///
    /// @brief
    ///    ��ȡ��ʱ��Ԥ���־
    ///
    uint32_t GetDSP()
    {
        return m_nDSP;
    }

    ///
    /// @brief
    ///    ��ȡ��ʱ�Ʊ�־
    ///
    uint32_t GetDST()
    {
        return m_nDST;
    }

    ///
    /// @brief
    ///    ��ȡʱ��ƫ�ƣ�Сʱ��
    ///
    float GetTimeOffset()
    {
        return m_fTimeOffset;
    }

    ///
    /// @brief
    ///    ��ȡʱ������
    ///
    uint32_t GetTimeQuality()
    {
        return m_nTimeQuality;
    }

    ///
    /// @brief
    ///    ������������
    ///
    void ResetData()
    {
        m_nDSP = 0;
        m_nDST = 0;
        m_nLSP = 0;
        m_nLS  = 0;
        m_fTimeOffset = 0;
        m_nTimeQuality = 0;
        memset(&m_stSysTime, 0, sizeof(m_stSysTime));
    }

private:
    friend class CDsmDataLayer;
    ///
    /// @brief
    ///     ����ʱ������
    ///
    void SetDateTime(const SYSTEMTIME* pTime)
    {
        if (pTime)
        {
            m_stSysTime = *pTime;
        }
    }

    ///
    /// @brief
    ///    ��������Ԥ���־
    ///
    void SetLSP(uint32_t nLSP)
    {
        m_nLSP = nLSP;
    }

    ///
    /// @brief
    ///    ���������־
    ///
    void SetLS(uint32_t nLS)
    {
        m_nLS = nLS;
    }

    ///
    /// @brief
    ///    ������ʱ��Ԥ���־
    ///
    void SetDSP(uint32_t nDSP)
    {
        m_nDSP = nDSP;
    }

    ///
    /// @brief
    ///    ������ʱ�Ʊ�־
    ///
    void SetDST(uint32_t nDST)
    {
        m_nDST = nDST;
    }

    ///
    /// @brief
    ///    ����ʱ��ƫ�ƣ�Сʱ��
    ///
    void SetTimeOffset(float fTimeOffset)
    {
        m_fTimeOffset = fTimeOffset;
    }

    ///
    /// @brief
    ///    ����ʱ������
    ///
    void SetTimeQuality(uint32_t nQuality)
    {
        m_nTimeQuality = nQuality;
    }

private:
    SYSTEMTIME                      m_stSysTime;                        ///< ʱ��������Ϣ
    uint32_t                        m_nLSP;                             ///< ����Ԥ���־
    uint32_t                        m_nLS;                              ///< �����־
    uint32_t                        m_nDSP;                             ///< ��ʱ��Ԥ���־
    uint32_t                        m_nDST;                             ///< ��ʱ�Ʊ�־
    float                           m_fTimeOffset;                      ///< ʱ��ƫ�ƣ�Сʱ��
/*
ʱ���������������
0x0����������״̬��ʱ��ͬ������
0x1��ʱ��ͬ���쳣��ʱ��׼ȷ������1 ns
0x2��ʱ��ͬ���쳣��ʱ��׼ȷ������10 ns
0x3��ʱ��ͬ���쳣��ʱ��׼ȷ������100 ns
0x4��ʱ��ͬ���쳣��ʱ��׼ȷ������1��s
0x5��ʱ��ͬ���쳣��ʱ��׼ȷ������10��s
0x6��ʱ��ͬ���쳣��ʱ��׼ȷ������100��s
0x7��ʱ��ͬ���쳣��ʱ��׼ȷ������1 ms
0x8��ʱ��ͬ���쳣��ʱ��׼ȷ������10 ms
0x9��ʱ��ͬ���쳣��ʱ��׼ȷ������100 ms
0xA��ʱ��ͬ���쳣��ʱ��׼ȷ������1 s
0xB��ʱ��ͬ���쳣��ʱ��׼ȷ������10 s
0xF��ʱ�����ع��ϣ�ʱ����Ϣ��������
*/
    uint32_t                        m_nTimeQuality;                     ///< ʱ������

};