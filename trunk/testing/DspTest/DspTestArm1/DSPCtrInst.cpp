/// @file
///
/// @brief
///     DSP�˿������ĵ���
///
/// @note
///     ����DSP�˿������ĵ����Ĵ�����ɾ��
///
/// Copyright (c) 2012 IUnknownBase Inc
///
/// ���ߣ�
///    chao  2012.12.19
///
/// �޸���ʷ��
///

#include "stdafx.h"
#include "DSPCtrInst.h"

///
/// @brief
///     ָ��DSP��Controllor��ָ��
///
DSPControllor *DSPCtrInst::m_pInstance = NULL;

///
/// @brief
///     ��ȡDSP��Controllor�ĵ���
///
/// @return
///     ����ָ��DSP��Controllor��ָ��
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
///     ɾ��DSP��Controllor�ĵ������ͷ��ڴ�
///
void DSPCtrInst::DeleteInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
	}
}
