/// @file
///
/// @brief
///     ����ֵ��������ģ��
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.13
///
/// �޸���ʷ��
///

#pragma once

#include <vector>
#include <map>

#include "src\include\stdint.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\include\datatypes.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"
#include "src\service\datalayer\scl\SCLManager.h"


///
/// @brief
///    ����ͨ����Ϣ
///
class CSmvRecvChannelInfo
{
public:
    uint32_t                            nIndex;                         ///< ͨ�����
    enumChannelType                     enChannelType;                  ///< ͨ������
    ChannelPhase                        enChannelPhase;                 ///< ͨ�����
    float                               fPrimaryRatedValue;             ///< һ�ζֵ
    float                               fSecondaryRatedValue;           ///< ���ζֵ
    enumChannelValueType                enChannelValueType;             ///< ָ��ͨ�����������Ǳ���ֵ���ǲ���ֵ
    std::wstring                        strChannelDesc;                 ///< ͨ��������Ϣ
    CBaseDetectItem*                    pOwnerCtrlBlk;                  ///< ��ǰͨ�������Ŀ��ƿ�
    CSmvRecvChannelInfo();
};
inline CSmvRecvChannelInfo::CSmvRecvChannelInfo() : nIndex(0)
                                                  , enChannelType(CHANNEL_TYPE_UNKOWN)
                                                  , enChannelPhase(CHANNEL_PHASE_UNKOWN)
                                                  , fPrimaryRatedValue(0.0f)
                                                  , fSecondaryRatedValue(0.0f)
                                                  , strChannelDesc(L"")
                                                  , enChannelValueType(CHANNEL_VALUE_TYPE_UNKNOWN)
                                                  , pOwnerCtrlBlk(NULL)
{
}

class CSmvRecvConfig
{
public:
    CSmvRecvConfig(CBaseDetectItem* pDetectItem);
    ~CSmvRecvConfig(void);

public:
    ///
    /// @brief
    ///    ��ʼ���Ѿ���ӽ����Ŀ��ƿ�ͨ����Ϣ
    ///
    void init();
//
//=============================================================================
// ���ƿ���ز���
//=============================================================================
//
    ///
    /// @brief
    ///    ��ȡ���ƿ�ͨ����Ŀ
    ///
    /// @return
    ///     size_t ���ؿ��ƿ�ͨ����Ŀ
    ///
    size_t GetChannelCount();

    ///
    /// @brief
    ///    ��ȡ�����ö�Ӧ�Ŀ��ƿ���Ϣ
    ///
    CBaseDetectItem* GetCtrlBlk()
    {
        return m_pCurrentCtrlBlk;
    }

    ///
    /// @brief
    ///    ��ȡĳ��������Ӧ��ͨ����Ϣ
    ///
    /// @param[in]  nIndex  Ҫ��ȡ��ͨ��������
    ///
    /// @return
    ///     CSmvRecvChannelInfo* ����ĳһͨ������Ϣָ��
    ///
    CSmvRecvChannelInfo* GetOneChannelInfo( uint32_t nIndex);

    ///
    /// @brief
    ///    ����ĳ��������Ӧ��ͨ����Ϣ
    ///
    /// @param[in]  nIndex  Ҫ���õ�ͨ��������
    /// @param[in] pSmvRecvChannelInfo ����ָ��ָ��������Ϣ���õ���Ӧ������ͨ����
    ///
    /// @return
    ///     bool ���óɹ�����true
    ///
    bool SetOneChannelInfo( uint32_t nIndex, CSmvRecvChannelInfo* pSmvRecvChannelInfo );

    ///
    /// @brief
    ///    ���һ��ͨ����Ϣ
    ///
    /// @param[in] pSmvRecvChannelInfo ����ָ��ָ��������Ϣ���õ���Ӧ������ͨ����
    ///
    /// @return
    ///     bool ��ӳɹ�����true
    ///
    bool AddOneChannelInfo( CSmvRecvChannelInfo* pSmvRecvChannelInfo );

    ///
    /// @brief
    ///    ���÷��͸�DSPʹ�õĵ�ѹ�����Ϣ
    ///
    bool SetDspSendVolInfo( uint32_t nVolPhaseA, uint32_t nVolPhaseB, uint32_t nVolPhaseC );

    ///
    /// @brief
    ///    ���÷��͸�DSPʹ�õĵ��������Ϣ
    ///
    bool SetDspSendCurInfo( uint32_t nAmpPhaseA, uint32_t nAmpPhaseB, uint32_t nAmpPhaseC );

    ///
    /// @brief
    ///    ��ȡ��ǰ���͸�DSPʹ�õĵ�ѹ�����Ϣ
    ///
    bool GetDspSendVolInfo( uint32_t& nVolPhaseA, uint32_t& nVolPhaseB, uint32_t& nVolPhaseC );

    ///
    /// @brief
    ///    ��ȡ��ǰ���͸�DSPʹ�õĵ��������Ϣ
    ///
    bool GetDspSendCurInfo( uint32_t& nAmpPhaseA, uint32_t& nAmpPhaseB, uint32_t& nAmpPhaseC );

    ///
    /// @brief
    ///    ���ú���smv1����
    ///
    bool SetPhaseCheckSmv1Channels(uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC );

    ///
    /// @brief
    ///    ��ȡ����svm1����
    ///
    bool GetPhaseCheckSmv1Channels(uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC );

    ///
    /// @brief
    ///    ���ú���smv2����
    ///
    bool SetPhaseCheckSmv2Channels(uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC );

    ///
    /// @brief
    ///    ��ȡ����svm2����
    ///
    bool GetPhaseCheckSmv2Channels(uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC );

private:
    ///
    /// @brief
    ///    �������������Ϣ
    ///
    void _ClearAllContainer();

private:
    CBaseDetectItem*                            m_pCurrentCtrlBlk;      ///< ��ǰ���ƿ���Ϣ

    typedef std::vector<CSmvRecvChannelInfo*>   VecChannelInfo;         ///< һ�����ƿ��ͨ����Ϣ
    VecChannelInfo                              m_vecItemChannelInfo;   ///< ������ƿ��Ӧ��ͨ����Ϣ

    // ���÷��͸�dspʱָ���ĵ�ѹ/����ABC���,�û����湦�ʡ�������ѹ����ͨ����Ϣ
    uint32_t        m_nChannelTypeVoltage[3];                           ///< ���õ�ѹͨ���ֱ��Ӧ���ƿ��ABC����
    uint32_t        m_nChannelTypeCurrent[3];                           ///< ���õ���ͨ���ֱ��Ӧ���ƿ��ABC����

    /*
    ���ú���ʹ�õĿ��ƿ�ABC�����Ϣ,������������ƿ������ͬ��
    ��Ҫ����������ƿ��3��ͨ��,ǰ����Ԫ�ر�ʾ��һ�����ƿ��3��ͨ����
    ��3��Ԫ�ر�ʾ�ڶ������ƿ��Ӧ��3��ͨ����Ϣ
    */
    uint32_t        m_nPhaseCheckABCChannels[6];
};
