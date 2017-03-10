/// @file
///
/// @brief
///      定义采样值数据接收基类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.23
///
/// 修改历史：
///

#pragma once

#include <string>
#include "src\include\stdint.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"
#include "src\service\datalayer\config\SmvRecvConfig.h"

///
/// @brief
///    通道信息基类
///
class CDataRecvChannelBase
{
public:
    uint32_t            m_nIndex;               ///< 通道序号
    bool                m_bChannelValid;        ///< 通道是否有效

    CDataRecvChannelBase();
};
inline CDataRecvChannelBase::CDataRecvChannelBase() : m_nIndex(0)
                                                    , m_bChannelValid(true)
{
}


///
/// @brief
///    接收数据基类
///
class CDataRecvBase
{
public:
    CDataRecvBase(void);
    ~CDataRecvBase(void);

public:
    ///
    /// @brief
    ///    获取此控制块数据对应的控制块
    ///
    CBaseDetectItem* GetCtrlBlk();

    ///
    /// @brief
    ///    获取通道数目
    ///
    size_t GetChannelCount();

    ///
    /// @brief
    ///    获取通道类型
    ///
    enumChannelType GetChannelType(uint32_t nIndex);

    ///
    /// @brief
    ///    获取通道相别
    ///
    ChannelPhase GetChannelPhase(uint32_t nIndex);

    ///
    /// @brief
    ///    获取通道描述
    ///
    std::wstring GetChannelDesc(uint32_t nIndex);

    ///
    /// @brief
    ///    设置某一个通道的电压/电流类型
    ///
    /// @param[in]  nIndex 要设置的通道号
    /// @param[in]  enChannelType   要设定的通道类型
    ///
    /// @return
    /// bool    true表示设置成功
    ///
    bool SetChannelType(uint32_t nIndex, enumChannelType enChannelType);

    ///
    /// @brief
    ///    设置某一个通道的相别
    ///
    /// @param[in]  nIndex 要设置的通道号
    /// @param[in]  enChannelPhase   要设定的通道相别
    ///
    /// @return
    /// bool    true表示设置成功
    ///
    bool  SetChannelPhase(uint32_t nIndex, ChannelPhase enChannelPhase);


protected:
    friend class CFrameRecv;
    ///
    /// @brief
    ///    设置控制块指针
    ///
    void SetCtrlBlk(CBaseDetectItem* pCtrlBlk);


protected:
    CBaseDetectItem*    m_pCtrlBlk;                     ///< 当前的控制块指针
};
