/// @file
///
/// @brief
///     �������û���
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.27
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "ParamConfigBase.h"

///
/// @brief
///    ���캯��
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
///    ��������
///
CParamConfigBase::~CParamConfigBase(void)
{
}

///
/// @brief
///    ��ò������ù������޸�״̬
///
/// @return
///     bool    ����true��ʾ�������ù������ѱ�����
///
bool CParamConfigBase::IsModified()
{
    return m_bModified;
}

///
/// @brief
///    ���ò������ù������޸�״̬
///
/// @param[in]  bModified ���ù������޸�״̬
///
void CParamConfigBase::SetModified(bool bModified)
{
    m_bModified = bModified;
}

///
/// @brief
///    ��������ļ�����·��
///
/// @return
///     std::wstring    �����ļ�����·��
///
std::wstring CParamConfigBase::GetConfigFile()
{
    return m_strConfigFile;
}

///
/// @brief
///    9-1��FT3��FT3��չ���ƿ��״̬�ֻ�9-2ͨ����Ʒ���Ƿ����˸ı�
///
/// @return
///     bool    �����˸ı䷵��true
///
bool CParamConfigBase::IsStatusWordChanged() const
{
    return m_bStatusWordChanged;
}

///
/// @brief
///    ����9-1��FT3��FT3��չ���ƿ��״̬�ֻ�9-2ͨ����Ʒ���Ƿ����˸ı�
///
/// @param[in]  bNewValue    �Ƿ�ı�
///
void CParamConfigBase::SetStatusWordChanged( bool bNewValue )
{
    m_bStatusWordChanged = bNewValue;
}

///
/// @brief
///    ��ѯ9-2ͨ����Ʒ���Ƿ����˸ı�
///
/// @return
///     bool    �����˸ı䷵��true
///
bool CParamConfigBase::IsQualityChanged() const
{
    return m_bQualityChanged;
}

///
/// @brief
///    ����9-2ͨ����Ʒ���Ƿ����˸ı�
///
/// @param[in]  bNewValue    �Ƿ�ı�
///
void CParamConfigBase::SetQualityChanged( bool bNewValue )
{
    m_bQualityChanged = bNewValue;
}
