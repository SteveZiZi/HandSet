/// @file
///
/// @brief
///     ����˫ad������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.24
///
/// �޸���ʷ��
///

#pragma once
#include "src\service\datalayer\recv_data\DataRecvBase.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"

#define DOUBLE_AD_GROUP_COUNT (6)     ///< ����˫AD�ıȽ�����

class CDataDoubleAD : public CDataRecvBase
{
public:
    CDataDoubleAD(void);
    ~CDataDoubleAD(void);

    ///
    /// @brief
    ///    ����AD1��Ӧ��ͨ����
    ///
    /// @param[in] nIndexADGroup ָ���ڼ���AD
    /// @param[in] nIndexChannel ָ��Ҫ���õ�ͨ����
    ///
    void SetAD1Channel(uint32_t nIndexADGroup, uint32_t nIndexChannel);

    ///
    /// @brief
    ///    ����AD2��Ӧ��ͨ����
    ///
    /// @param[in] nIndexADGroup ָ���ڼ���AD
    /// @param[in] nIndexChannel ָ��Ҫ���õ�ͨ����
    ///
    void SetAD2Channel(uint32_t nIndexADGroup, uint32_t nIndexChannel);

    ///
    /// @brief
    ///    ��ȡAD1��Ӧ��ͨ����
    ///
    /// @param[in] nIndexADGroup ָ���ڼ���AD
    ///
    /// @return
    ///     uint32_t ���ػ�ȡ����ͨ���ţ�����ͨ����Ŀ������Ч
    ///
    uint32_t GetAD1Channel(uint32_t nIndexADGroup);

    ///
    /// @brief
    ///    ��ȡAD2��Ӧ��ͨ����
    ///
    /// @param[in] nIndexADGroup ָ���ڼ���AD
    ///
    /// @return
    ///     uint32_t ���ػ�ȡ����ͨ���ţ�����ͨ����Ŀ������Ч
    ///
    uint32_t GetAD2Channel(uint32_t nIndexADGroup);

    ///
    /// @brief
    ///    ��ȡĳһ��˫AD������
    ///
    /// @param[in]  nIndexADGroup   Ҫ��ȡ��˫AD���
    /// @param[out] fVal1           Ҫ��ȡ�Ľ��1
    /// @param[out] fVAl2           Ҫ��ȡ�Ľ��2
    ///
    /// @return
    ///     bool    ��ȡ�ɹ�����true
    ///
    bool GetADGroupResult(uint32_t nIndexADGroup, float& fVal1, float& fVal2);

    ///
    /// @brief
    ///    ����˫ad������
    ///
    void SetDoubleAdSmvValue(const SMV_VALUE& stSmvValue);

    ///
    /// @brief
    ///    ���˫AD������Ϣ
    ///
    void ClearDoubleADData();

private:
    ///
    /// @brief
    ///    ����˫AD�Ƚ�ͨ����Ϣ
    ///
    /// @note
    ///     ���ݱ����ʽΪ��ά���飬ͨ�������ų���ͨ����Ϊ��Ч����
    ///     ������ʾ����ʾͨ��1��2Ϊһ�飬5��4Ϊһ�飬3��6Ϊһ��˫AD
    ///      1 2
    ///      5 4
    ///      3 6
    uint32_t        m_nADGroupChannels[DOUBLE_AD_GROUP_COUNT][2];
    SMV_VALUE       m_stSmvValue;
};
