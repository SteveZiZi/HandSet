/// @file
///
/// @brief
///     参数配置基类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.27
///
/// 修改历史：
///

#ifndef __PARAM_CONFIG_BASE_H__
#define __PARAM_CONFIG_BASE_H__
#include <string>
#include "src/utils/CMarkup/Markup.h"

#define MSGPARAM_SMV_SEND_91_4K_SELECTED_MAX            4               ///< 最多选中的IEC61850-9-1发送控制块（4K采样率）
#define MSGPARAM_SMV_SEND_91_12P8K_SELECTED_MAX         2               ///< 最多选中的IEC61850-9-1发送控制块（12.8K采样率）

#define MSGPARAM_SMV_SEND_92_4K_SELECTED_MAX            4               ///< 最多选中的IEC61850-9-2发送控制块（4K采样率）
#define MSGPARAM_SMV_SEND_92_12P8K_SELECTED_MAX         2               ///< 最多选中的IEC61850-9-2发送控制块（12.8K采样率）

#define MSGPARAM_SMV_SEND_FT3_CTRL_SELECTED_MAX         1               ///< 最多选中的FT3发送控制块
#define MSGPARAM_SMV_SEND_FT3_EXT_CTRL_SELECTED_MAX     1               ///< 最多选中的FT3扩展发送控制块
#define MSGPARAM_GOOSE_SEND_CTRL_SELECTED_MAX           4               ///< 最多选中的goose发送控制块
#define MSGPARAM_GOOSE_REC_CTRL_SELECTED_MAX            8               ///< 最多选中的goose接收控制块

class CParamConfigBase
{
protected:
    CParamConfigBase(void);
    virtual ~CParamConfigBase(void);

public:
    ///
    /// @brief
    ///    加载配置文件
    ///
    /// @param[in]  strConfigFile 文件路径
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    virtual bool LoadConfig( std::wstring strConfigFile ) = 0;

    ///
    /// @brief
    ///    保存配置文件
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    virtual bool SaveConfig() = 0;

    ///
    /// @brief
    ///    获得参数配置管理器修改状态
    ///
    /// @return
    ///     bool    返回true表示参数配置管理器已被更改
    ///
    bool IsModified();

    ///
    /// @brief
    ///    设置参数配置管理器修改状态
    ///
    /// @param[in]  bModified 配置管理器修改状态
    ///
    void SetModified(bool bModified);

    ///
    /// @brief
    ///    获得配置文件保存路径
    ///
    /// @return
    ///     std::wstring    配置文件保存路径
    ///
    std::wstring GetConfigFile();

    ///
    /// @brief
    ///    查询9-1、FT3、FT3扩展控制块的状态字是否发生了改变
    ///
    /// @return
    ///     bool    发生了改变返回true
    ///
    bool IsStatusWordChanged() const;

    ///
    /// @brief
    ///    设置9-1、FT3、FT3扩展控制块的状态字是否发生了改变
    ///
    /// @param[in]  bNewValue    是否改变
    ///
    void SetStatusWordChanged(bool bNewValue);

    ///
    /// @brief
    ///    查询9-2通道的品质是否发生了改变
    ///
    /// @return
    ///     bool    发生了改变返回true
    ///
    bool IsQualityChanged() const;

    ///
    /// @brief
    ///    设置9-2通道的品质是否发生了改变
    ///
    /// @param[in]  bNewValue    是否改变
    ///
    void SetQualityChanged(bool bNewValue);

protected:
    bool                                m_bModified;                    ///< 是否已修改
    bool                                m_bStatusWordChanged;           ///< 9-1、FT3、FT3扩展控制块的状态字发生了改变
    bool                                m_bQualityChanged;              ///< 9-2通道的品质发生了改变
    std::wstring                        m_strConfigFile;                ///< 配置文件保存路径
};
#endif
