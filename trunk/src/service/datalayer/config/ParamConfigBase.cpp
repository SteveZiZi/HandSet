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

#include "StdAfx.h"
#include "ParamConfigBase.h"

///
/// @brief
///    构造函数
///
CParamConfigBase::CParamConfigBase(void)
: m_bModified(false)
, m_bStatusWordChanged(false)
, m_bQualityChanged(false)
, m_strConfigFile(_T(""))
{
}

///
/// @brief
///    析构函数
///
CParamConfigBase::~CParamConfigBase(void)
{
}

///
/// @brief
///    获得参数配置管理器修改状态
///
/// @return
///     bool    返回true表示参数配置管理器已被更改
///
bool CParamConfigBase::IsModified()
{
    return m_bModified;
}

///
/// @brief
///    设置参数配置管理器修改状态
///
/// @param[in]  bModified 配置管理器修改状态
///
void CParamConfigBase::SetModified(bool bModified)
{
    m_bModified = bModified;
}

///
/// @brief
///    获得配置文件保存路径
///
/// @return
///     std::wstring    配置文件保存路径
///
std::wstring CParamConfigBase::GetConfigFile()
{
    return m_strConfigFile;
}

///
/// @brief
///    9-1、FT3、FT3扩展控制块的状态字或9-2通道的品质是否发生了改变
///
/// @return
///     bool    发生了改变返回true
///
bool CParamConfigBase::IsStatusWordChanged() const
{
    return m_bStatusWordChanged;
}

///
/// @brief
///    设置9-1、FT3、FT3扩展控制块的状态字或9-2通道的品质是否发生了改变
///
/// @param[in]  bNewValue    是否改变
///
void CParamConfigBase::SetStatusWordChanged( bool bNewValue )
{
    m_bStatusWordChanged = bNewValue;
}

///
/// @brief
///    查询9-2通道的品质是否发生了改变
///
/// @return
///     bool    发生了改变返回true
///
bool CParamConfigBase::IsQualityChanged() const
{
    return m_bQualityChanged;
}

///
/// @brief
///    设置9-2通道的品质是否发生了改变
///
/// @param[in]  bNewValue    是否改变
///
void CParamConfigBase::SetQualityChanged( bool bNewValue )
{
    m_bQualityChanged = bNewValue;
}
