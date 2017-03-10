/// @file
///
/// @brief
///     һ�����ƿ�ͨ������Чֵ/��λ��Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.22
///
/// �޸���ʷ��
///

#pragma once

#include <string>
#include <vector>
#include "DataRecvBase.h"
#include "src\include\stdint.h"
#include "src\service\datalayer\include\datatypes.h"

///
/// @brief
///    ��Чֵͨ����Ϣ����Ӧ������һ������
///
class CEffectiveChanelInfo : public CDataRecvChannelBase
{
public:
    float               m_fEffectiveValue;              ///< ��Чֵ
    float               m_fPhase;                       ///< ��λ
    bool                m_bIsDcValue;                   ///< �Ƿ���ֱ��

    CEffectiveChanelInfo();
};
inline CEffectiveChanelInfo::CEffectiveChanelInfo() : m_fEffectiveValue(0)
                                                    , m_fPhase(0)
                                                    , m_bIsDcValue(false)
{
}

class CDataEffectiveVal : public CDataRecvBase
{
public:
    CDataEffectiveVal(void);
    ~CDataEffectiveVal(void);

public:

    ///
    /// @brief
    ///    ��ȡĳһ��ͨ��������
    ///
    /// @param[in]  nIndex Ҫ��ȡ��ͨ������
    ///
    /// @return
    ///     CEffectiveChanelInfo* ��ȡ��ͨ����Ϣָ��
    ///
     CEffectiveChanelInfo* GetOneChannelInfo(uint32_t nIndex);

     ///
     /// @brief
     ///    ���һ��ͨ��
     ///
     /// @param[in]  rEffectiveChannelInfo ͨ����Ϣ
     ///
     /// @return
     ///     bool ��ӳɹ�����true
     ///
     bool AddOneChannelInfo(const CEffectiveChanelInfo& rEffectiveChannelInfo);

     ///
     /// @brief
     ///    ɾ��һ��ͨ��
     ///
     /// @param[in]  rEffectiveChannelInfo ͨ����Ϣ
     ///
     /// @return
     ///     bool �Ƴ��ɹ�����true
     ///
     bool RemoveOneChannelInfo(const CEffectiveChanelInfo& rEffectiveChannelInfo);

    ///
    /// @brief
    ///    ����ͨ����������
    ///
    void ClearChannelVec();

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
    bool _IsChannelInVec(const CEffectiveChanelInfo& rEffectiveChannelInfo);



protected:
    std::vector<CEffectiveChanelInfo*>      m_vecChannels;                  ///< ͨ������

};
