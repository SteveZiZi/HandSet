/// @file
///
/// @brief
///     描述一个控制块的通道有效性，是否同步等信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.23
///
/// 修改历史：
///

#pragma once

#include <vector>
#include "DataRecvBase.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"

class CStatusWatchChannelInfo : public CDataRecvChannelBase
{
public:
    uint32_t            m_nIndex;               ///< 通道索引
    uint32_t            m_nQuality;             ///< 品质因数
    bool                m_bChannelValid;        ///< 通道是否有效
    bool                m_bChannelTestFlag;     ///< 通道是否检修
    bool                m_bChannelDerived;      ///< 通道是否合成量
    std::wstring        m_strChannelDesc;       ///< 通道描述


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
// 获取信息接口
//=============================================================================
//
    ///
    /// @brief
    ///    获取数据帧类型
    ///
    enumFrameType GetFrameType();

    ///
    /// @brief
    ///    获取通道品质因数
    ///
    /// @param[in]  nIndex  通道索引号
    /// @param[out] nQuality 保存获取的品质因数
    ///
    /// @return
    ///     bool    获取品质因数成功返回true
    ///
    bool GetOneChannelQuality( uint32_t nIndex, uint32_t& nQuality );

    ///
    /// @brief
    ///    获取通道是否有效
    ///
    /// @param[in]  nIndex  通道索引号
    ///
    /// @return
    ///     bool    返回通道数据是否有效，true标识有效
    ///
    bool GetOneChannelIsValid(uint32_t nIndex);

    ///
    /// @brief
    ///    获取mu是否同步,9-2，9-1，ft3都可用
    ///
    /// @return
    ///     bool    返回mu是否同步，true标识同步
    ///
    bool GetMuIsSync();

    ///
    /// @brief
    ///    返回一个通道信息
    ///
    CStatusWatchChannelInfo GetOneChannelInfo(uint32_t nIndex);

    ///
    /// @brief
    ///    获取FT3状态字
    ///
    bool GetFt3StatusWords(uint16_t& nStatusWrod1, uint16_t& nStatusWord2);

    ///
    /// @brief
    ///    获取ft3/9-1故障报警信息
    ///
    /// @return
    ///     int32_t 0: 良好； 1：警告或报警（要求维修）
    ///
    int32_t GetFt3FaultAlarm();

    ///
    /// @brief
    ///    获取FT3/9-1是否检修
    ///                      
    /// @return
    ///     int32_t 0: 接通（正常运行）； 1：试验
    ///
    int32_t GetFt3TestFlag();

    ///
    /// @brief
    ///    获取唤醒接通信息
    ///
    /// @return
    ///     int32_t 0: 接通（正常运行),数据有效； 1：唤醒时间，数据无效
    ///
    int32_t GetFt3WakeupValid();

//
//=============================================================================
// 设置接口
//=============================================================================
//
private:
    friend class CFrameRecv;
    ///
    /// @brief
    ///    设置控制块信息
    ///
    void SetCtrlBlk(CBaseDetectItem* pDetectItem);

    ///
    /// @brief
    ///    设置mu是否同步，此设置针对9-2. FT3/9-1的mu同步信息已经在2个状态字里面
    ///
    void SetMuSync(bool bSync);

    ///
    /// @brief
    ///    设置通道品质因数,针对9-2
    ///
    bool SetChannelQuality(uint32_t nChannelIndex, uint32_t nQuality);

    ///
    /// @brief
    ///    设置控制块状态字，针对ft3/9-1
    ///
    bool SetCtrlBlkStatusWords(uint16_t nStatusWord1, uint16_t nStatusWord2);

private:
    ///
    /// @brief
    ///    根据品质因数返回9-2是否检修
    ///
    /// @return
    ///     检修返回true
    ///
    bool _GetQualityTestFlag(uint32_t nQuality);

    ///
    /// @brief
    ///    根据品质因数返回9-2是否合成
    ///
    /// @return
    ///     合成返回true
    ///
    bool _GetQualityDerived(uint32_t nQuality);

private:
    CSmvCtrlBlkQuality              m_cSmvCtrBlkQuality;

};
