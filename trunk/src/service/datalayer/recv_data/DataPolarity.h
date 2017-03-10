/// @file
///
/// @brief
///     ���弫��������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.18
///
/// �޸���ʷ��
///

#pragma once

#include "src\service\datalayer\recv_data\DataRecvBase.h"

class CDataPolarityChannelInfo : public CDataRecvChannelBase
{
public:
    int32_t     m_nPolarity;                ///< ָʾͨ��������Ϣ��ֵ����0Ϊ�����ԣ�С��0Ϊ������

    CDataPolarityChannelInfo(): m_nPolarity(0)
    {
    }
};


class CDataPolarity : public CDataRecvBase
{
public:
    CDataPolarity(void);
    ~CDataPolarity(void);

public:
    ///
    /// @brief
    ///    ���һ��ͨ��
    ///
    /// @param[in]  rEffectiveChannelInfo ͨ����Ϣ
    ///
    /// @return
    ///     bool ��ӳɹ�����true
    ///
    bool AddOneChannelInfo(const CDataPolarityChannelInfo& rChannelInfo);

    ///
    /// @brief
    ///    ��ȡĳһ��ͨ��������
    ///
    /// @param[in]  nIndex Ҫ��ȡ��ͨ������
    ///
    /// @return
    ///     CEffectiveChanelInfo* ��ȡ��ͨ����Ϣָ��
    ///
    CDataPolarityChannelInfo* GetOneChannelInfo(uint32_t nIndex);

    ///
    /// @brief
    ///    ��ȡͨ���ļ�����Ϣ
    ///
    /// @param[in]      nIndex  ͨ����ţ���0��ʼ
    ///
    /// @return
    ///     int32_t     ���ؼ�����Ϣ������0��ʾ�����ԣ�С��0��ʾ������
    ///
    int32_t GetChannelPolarity(uint32_t nIndex);

    ///
    /// @brief
    ///    �������ͨ����Ϣ
    ///
    void ClearChannelInfo();

private:
    ///
    /// @brief
    ///    ����ͨ���ļ�����Ϣ
    ///
    /// @param[in]      nIndex  ͨ����ţ���0��ʼ
    /// @param[in]      nPorlarity  ͨ���ļ�����Ϣ
    ///
    /// @return
    ///     int32_t     ���ü�����Ϣ�����óɹ��ϻ�true
    ///
    bool SetChannelPolarity(uint32_t nIndex, int32_t nPorlarity);

    ///
    /// @brief
    ///    �ж�ĳһ��ͨ���Ƿ����
    ///
    /// @param[in]  rEffectiveChannelInfo ͨ����Ϣ
    ///
    /// @return
    ///     bool �Ѿ����ڷ���true
    ///
    bool _IsChannelInVec(const CDataPolarityChannelInfo& rChannelInfo);

private:
    std::vector<CDataPolarityChannelInfo*>       m_vecChannels;      ///< �������ͨ���ļ�����Ϣ
};
