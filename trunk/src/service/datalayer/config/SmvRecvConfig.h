/// @file
///
/// @brief
///     采样值接收配置模块
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.13
///
/// 修改历史：
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
///    定义通道信息
///
class CSmvRecvChannelInfo
{
public:
    uint32_t                            nIndex;                         ///< 通道序号
    enumChannelType                     enChannelType;                  ///< 通道类型
    ChannelPhase                        enChannelPhase;                 ///< 通道相别
    float                               fPrimaryRatedValue;             ///< 一次额定值
    float                               fSecondaryRatedValue;           ///< 二次额定值
    enumChannelValueType                enChannelValueType;             ///< 指定通道数据类型是保护值还是测量值
    std::wstring                        strChannelDesc;                 ///< 通道描述信息
    CBaseDetectItem*                    pOwnerCtrlBlk;                  ///< 当前通道所属的控制块
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
    ///    初始化已经添加进来的控制块通道信息
    ///
    void init();
//
//=============================================================================
// 控制块相关操作
//=============================================================================
//
    ///
    /// @brief
    ///    获取控制块通道数目
    ///
    /// @return
    ///     size_t 返回控制块通道数目
    ///
    size_t GetChannelCount();

    ///
    /// @brief
    ///    获取此配置对应的控制块信息
    ///
    CBaseDetectItem* GetCtrlBlk()
    {
        return m_pCurrentCtrlBlk;
    }

    ///
    /// @brief
    ///    获取某个索引对应的通道信息
    ///
    /// @param[in]  nIndex  要获取的通道的索引
    ///
    /// @return
    ///     CSmvRecvChannelInfo* 返回某一通道的信息指针
    ///
    CSmvRecvChannelInfo* GetOneChannelInfo( uint32_t nIndex);

    ///
    /// @brief
    ///    设置某个索引对应的通道信息
    ///
    /// @param[in]  nIndex  要设置的通道的索引
    /// @param[in] pSmvRecvChannelInfo 将此指针指向对象的信息设置到对应索引的通道中
    ///
    /// @return
    ///     bool 设置成功返回true
    ///
    bool SetOneChannelInfo( uint32_t nIndex, CSmvRecvChannelInfo* pSmvRecvChannelInfo );

    ///
    /// @brief
    ///    添加一个通道信息
    ///
    /// @param[in] pSmvRecvChannelInfo 将此指针指向对象的信息设置到对应索引的通道中
    ///
    /// @return
    ///     bool 添加成功返回true
    ///
    bool AddOneChannelInfo( CSmvRecvChannelInfo* pSmvRecvChannelInfo );

    ///
    /// @brief
    ///    设置发送给DSP使用的电压相别信息
    ///
    bool SetDspSendVolInfo( uint32_t nVolPhaseA, uint32_t nVolPhaseB, uint32_t nVolPhaseC );

    ///
    /// @brief
    ///    设置发送给DSP使用的电流相别信息
    ///
    bool SetDspSendCurInfo( uint32_t nAmpPhaseA, uint32_t nAmpPhaseB, uint32_t nAmpPhaseC );

    ///
    /// @brief
    ///    获取当前发送给DSP使用的电压相别信息
    ///
    bool GetDspSendVolInfo( uint32_t& nVolPhaseA, uint32_t& nVolPhaseB, uint32_t& nVolPhaseC );

    ///
    /// @brief
    ///    获取当前发送给DSP使用的电流相别信息
    ///
    bool GetDspSendCurInfo( uint32_t& nAmpPhaseA, uint32_t& nAmpPhaseB, uint32_t& nAmpPhaseC );

    ///
    /// @brief
    ///    设置核相smv1三相
    ///
    bool SetPhaseCheckSmv1Channels(uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC );

    ///
    /// @brief
    ///    获取核相svm1三相
    ///
    bool GetPhaseCheckSmv1Channels(uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC );

    ///
    /// @brief
    ///    设置核相smv2三相
    ///
    bool SetPhaseCheckSmv2Channels(uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC );

    ///
    /// @brief
    ///    获取核相svm2三相
    ///
    bool GetPhaseCheckSmv2Channels(uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC );

private:
    ///
    /// @brief
    ///    清空所有容器信息
    ///
    void _ClearAllContainer();

private:
    CBaseDetectItem*                            m_pCurrentCtrlBlk;      ///< 当前控制块信息

    typedef std::vector<CSmvRecvChannelInfo*>   VecChannelInfo;         ///< 一个控制块的通道信息
    VecChannelInfo                              m_vecItemChannelInfo;   ///< 保存控制块对应的通道信息

    // 设置发送给dsp时指定的电压/电流ABC相别,用户保存功率、序量电压电流通道信息
    uint32_t        m_nChannelTypeVoltage[3];                           ///< 设置电压通道分别对应控制块的ABC三相
    uint32_t        m_nChannelTypeCurrent[3];                           ///< 设置电流通道分别对应控制块的ABC三相

    /*
    设置核相使用的控制块ABC相别信息,核相的两个控制块可能相同，
    需要单独保存控制块的3个通道,前三个元素表示第一个控制块的3个通道，
    后3个元素表示第二个控制块对应的3个通道信息
    */
    uint32_t        m_nPhaseCheckABCChannels[6];
};
