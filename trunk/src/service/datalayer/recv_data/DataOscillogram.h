/// @file
///
/// @brief
///     ���岨��ͼ��Ҫ������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.24
///
/// �޸���ʷ��
///

#pragma once
#include <vector>
#include "src\service\datalayer\recv_data\DataEffectiveVal.h"


typedef std::vector<float>  OscillogramSmv;

class COscillogramChannelInfo : public CEffectiveChanelInfo
{
public:
    OscillogramSmv      m_vecChannelData;       ///< ÿ��ͨ������Ĳ���������
};

class CDataOscillogram : public CDataRecvBase
{
public:
    CDataOscillogram(void);
    ~CDataOscillogram(void);

public:
    ///
    /// @brief
    ///    ��ȡһ��ͨ������Ĳ�������Ŀ
    ///
    uint32_t GetOneChannelSmvCount();

    ///
    /// @brief
    ///    ��ȡĳһ��ͨ��������
    ///
    /// @param[in]  nIndex Ҫ��ȡ��ͨ������
    ///
    /// @return
    ///     COscillogramChannelInfo* ��ȡ��ͨ����Ϣָ��
    ///
    COscillogramChannelInfo* GetOneChannelInfo(uint32_t nIndex);

    ///
    /// @brief
    ///    ���һ��ͨ��
    ///
    /// @param[in]  rChannelInfo ͨ����Ϣ
    ///
    /// @return
    ///     bool ��ӳɹ�����true
    ///
    bool AddOneChannelInfo(COscillogramChannelInfo& rChannelInfo);

    ///
    /// @brief
    ///    ɾ��һ��ͨ��
    ///
    /// @param[in]  rChannelInfo ͨ����Ϣ
    ///
    /// @return
    ///     bool �Ƴ��ɹ�����true
    ///
    bool RemoveOneChannelInfo(const COscillogramChannelInfo& rChannelInfo);


    ///
    /// @brief
    ///    ����ͨ����������
    ///
    void ClearChannelVec();

    ///
    /// @brief
    ///    �Զ�����N����ͨ��
    ///
    void GenNChannels(uint32_t nChannelCount, uint32_t nVecReserveSize = 0);

private:
    ///
    /// @brief
    ///    �ж�ĳһ��ͨ���Ƿ����
    ///
    /// @param[in]  rEffectiveChannelInfo ͨ����Ϣ
    ///
    /// @return
    ///     bool �Ѿ����ڷ���true
    ///
    bool _IsChannelInVec(const COscillogramChannelInfo& rChannelInfo);

protected:
    uint32_t                                m_nVecFloatSize;    ///< ����һ��ͨ���������ݵ���
    std::vector<COscillogramChannelInfo*>   m_vecChannels;      ///< �������ͨ����������
};
