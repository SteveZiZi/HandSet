/// @file
///
/// @brief
///      �������ֵ���ݽ��ջ���
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.23
///
/// �޸���ʷ��
///

#pragma once

#include <string>
#include "src\include\stdint.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"
#include "src\service\datalayer\config\SmvRecvConfig.h"

///
/// @brief
///    ͨ����Ϣ����
///
class CDataRecvChannelBase
{
public:
    uint32_t            m_nIndex;               ///< ͨ�����
    bool                m_bChannelValid;        ///< ͨ���Ƿ���Ч

    CDataRecvChannelBase();
};
inline CDataRecvChannelBase::CDataRecvChannelBase() : m_nIndex(0)
                                                    , m_bChannelValid(true)
{
}


///
/// @brief
///    �������ݻ���
///
class CDataRecvBase
{
public:
    CDataRecvBase(void);
    ~CDataRecvBase(void);

public:
    ///
    /// @brief
    ///    ��ȡ�˿��ƿ����ݶ�Ӧ�Ŀ��ƿ�
    ///
    CBaseDetectItem* GetCtrlBlk();

    ///
    /// @brief
    ///    ��ȡͨ����Ŀ
    ///
    size_t GetChannelCount();

    ///
    /// @brief
    ///    ��ȡͨ������
    ///
    enumChannelType GetChannelType(uint32_t nIndex);

    ///
    /// @brief
    ///    ��ȡͨ�����
    ///
    ChannelPhase GetChannelPhase(uint32_t nIndex);

    ///
    /// @brief
    ///    ��ȡͨ������
    ///
    std::wstring GetChannelDesc(uint32_t nIndex);

    ///
    /// @brief
    ///    ����ĳһ��ͨ���ĵ�ѹ/��������
    ///
    /// @param[in]  nIndex Ҫ���õ�ͨ����
    /// @param[in]  enChannelType   Ҫ�趨��ͨ������
    ///
    /// @return
    /// bool    true��ʾ���óɹ�
    ///
    bool SetChannelType(uint32_t nIndex, enumChannelType enChannelType);

    ///
    /// @brief
    ///    ����ĳһ��ͨ�������
    ///
    /// @param[in]  nIndex Ҫ���õ�ͨ����
    /// @param[in]  enChannelPhase   Ҫ�趨��ͨ�����
    ///
    /// @return
    /// bool    true��ʾ���óɹ�
    ///
    bool  SetChannelPhase(uint32_t nIndex, ChannelPhase enChannelPhase);


protected:
    friend class CFrameRecv;
    ///
    /// @brief
    ///    ���ÿ��ƿ�ָ��
    ///
    void SetCtrlBlk(CBaseDetectItem* pCtrlBlk);


protected:
    CBaseDetectItem*    m_pCtrlBlk;                     ///< ��ǰ�Ŀ��ƿ�ָ��
};
