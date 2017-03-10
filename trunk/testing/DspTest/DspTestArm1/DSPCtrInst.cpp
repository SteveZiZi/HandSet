/// @file
///
/// @brief
///     DSP端控制器的单例
///
/// @note
///     用于DSP端控制器的单例的创建和删除
///
/// Copyright (c) 2012 IUnknownBase Inc
///
/// 作者：
///    chao  2012.12.19
///
/// 修改历史：
///

#include "stdafx.h"
#include "DSPCtrInst.h"

///
/// @brief
///     指向DSP端Controllor的指针
///
DSPControllor *DSPCtrInst::m_pInstance = NULL;

///
/// @brief
///     获取DSP端Controllor的单例
///
/// @return
///     返回指向DSP端Controllor的指针
///
DSPControllor *DSPCtrInst::Instance()
{
	if (!m_pInstance)
	{
		m_pInstance = new DSPControllor;
	}
	return m_pInstance;
}

///
/// @brief
///     删除DSP端Controllor的单例，释放内存
///
void DSPCtrInst::DeleteInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
	}
}
