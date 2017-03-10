/// @file
///
/// @brief
///     ����һ�����ƿ��ͨ����Ч�ԣ��Ƿ�ͬ������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.23
///
/// �޸���ʷ��
///

#pragma once

#include <vector>
#include "DataRecvBase.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"

class CStatusWatchChannelInfo : public CDataRecvChannelBase
{
public:
    uint32_t            m_nIndex;               ///< ͨ������
    uint32_t            m_nQuality;             ///< Ʒ������
    bool                m_bChannelValid;        ///< ͨ���Ƿ���Ч
    bool                m_bChannelTestFlag;     ///< ͨ���Ƿ����
    bool                m_bChannelDerived;      ///< ͨ���Ƿ�ϳ���
    std::wstring        m_strChannelDesc;       ///< ͨ������


    CStatusWatchChannelInfo();
};
inline CStatusWatchChannelInfo::CStatusWatchChannelInfo() : m_nQuality(0)
                                                          , m_nIndex(0)
                                                          , m_bChannelDerived(false)
                                                          , m_bChannelValid(false)
                                                          , m_bChannelTestFlag(false)
                                                          , m_strChannelDesc(L"")
{
}

class CDataStatusWatch : public CDataRecvBase
{
public:
    CDataStatusWatch(void);
    ~CDataStatusWatch(void);

public:
//
//=============================================================================
// ��ȡ��Ϣ�ӿ�
//=============================================================================
//
    ///
    /// @brief
    ///    ��ȡ����֡����
    ///
    enumFrameType GetFrameType();

    ///
    /// @brief
    ///    ��ȡͨ��Ʒ������
    ///
    /// @param[in]  nIndex  ͨ��������
    /// @param[out] nQuality �����ȡ��Ʒ������
    ///
    /// @return
    ///     bool    ��ȡƷ�������ɹ�����true
    ///
    bool GetOneChannelQuality( uint32_t nIndex, uint32_t& nQuality );

    ///
    /// @brief
    ///    ��ȡͨ���Ƿ���Ч
    ///
    /// @param[in]  nIndex  ͨ��������
    ///
    /// @return
    ///     bool    ����ͨ�������Ƿ���Ч��true��ʶ��Ч
    ///
    bool GetOneChannelIsValid(uint32_t nIndex);

    ///
    /// @brief
    ///    ��ȡmu�Ƿ�ͬ��,9-2��9-1��ft3������
    ///
    /// @return
    ///     bool    ����mu�Ƿ�ͬ����true��ʶͬ��
    ///
    bool GetMuIsSync();

    ///
    /// @brief
    ///    ����һ��ͨ����Ϣ
    ///
    CStatusWatchChannelInfo GetOneChannelInfo(uint32_t nIndex);

    ///
    /// @brief
    ///    ��ȡFT3״̬��
    ///
    bool GetFt3StatusWords(uint16_t& nStatusWrod1, uint16_t& nStatusWord2);

    ///
    /// @brief
    ///    ��ȡft3/9-1���ϱ�����Ϣ
    ///
    /// @return
    ///     int32_t 0: ���ã� 1������򱨾���Ҫ��ά�ޣ�
    ///
    int32_t GetFt3FaultAlarm();

    ///
    /// @brief
    ///    ��ȡFT3/9-1�Ƿ����
    ///                      
    /// @return
    ///     int32_t 0: ��ͨ���������У��� 1������
    ///
    int32_t GetFt3TestFlag();

    ///
    /// @brief
    ///    ��ȡ���ѽ�ͨ��Ϣ
    ///
    /// @return
    ///     int32_t 0: ��ͨ����������),������Ч�� 1������ʱ�䣬������Ч
    ///
    int32_t GetFt3WakeupValid();

//
//=============================================================================
// ���ýӿ�
//=============================================================================
//
private:
    friend class CFrameRecv;
    ///
    /// @brief
    ///    ���ÿ��ƿ���Ϣ
    ///
    void SetCtrlBlk(CBaseDetectItem* pDetectItem);

    ///
    /// @brief
    ///    ����mu�Ƿ�ͬ�������������9-2. FT3/9-1��muͬ����Ϣ�Ѿ���2��״̬������
    ///
    void SetMuSync(bool bSync);

    ///
    /// @brief
    ///    ����ͨ��Ʒ������,���9-2
    ///
    bool SetChannelQuality(uint32_t nChannelIndex, uint32_t nQuality);

    ///
    /// @brief
    ///    ���ÿ��ƿ�״̬�֣����ft3/9-1
    ///
    bool SetCtrlBlkStatusWords(uint16_t nStatusWord1, uint16_t nStatusWord2);

private:
    ///
    /// @brief
    ///    ����Ʒ����������9-2�Ƿ����
    ///
    /// @return
    ///     ���޷���true
    ///
    bool _GetQualityTestFlag(uint32_t nQuality);

    ///
    /// @brief
    ///    ����Ʒ����������9-2�Ƿ�ϳ�
    ///
    /// @return
    ///     �ϳɷ���true
    ///
    bool _GetQualityDerived(uint32_t nQuality);

private:
    CSmvCtrlBlkQuality              m_cSmvCtrBlkQuality;

};
