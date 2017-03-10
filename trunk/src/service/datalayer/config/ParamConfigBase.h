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

#ifndef __PARAM_CONFIG_BASE_H__
#define __PARAM_CONFIG_BASE_H__
#include <string>
#include "src/utils/CMarkup/Markup.h"

#define MSGPARAM_SMV_SEND_91_4K_SELECTED_MAX            4               ///< ���ѡ�е�IEC61850-9-1���Ϳ��ƿ飨4K�����ʣ�
#define MSGPARAM_SMV_SEND_91_12P8K_SELECTED_MAX         2               ///< ���ѡ�е�IEC61850-9-1���Ϳ��ƿ飨12.8K�����ʣ�

#define MSGPARAM_SMV_SEND_92_4K_SELECTED_MAX            4               ///< ���ѡ�е�IEC61850-9-2���Ϳ��ƿ飨4K�����ʣ�
#define MSGPARAM_SMV_SEND_92_12P8K_SELECTED_MAX         2               ///< ���ѡ�е�IEC61850-9-2���Ϳ��ƿ飨12.8K�����ʣ�

#define MSGPARAM_SMV_SEND_FT3_CTRL_SELECTED_MAX         1               ///< ���ѡ�е�FT3���Ϳ��ƿ�
#define MSGPARAM_SMV_SEND_FT3_EXT_CTRL_SELECTED_MAX     1               ///< ���ѡ�е�FT3��չ���Ϳ��ƿ�
#define MSGPARAM_GOOSE_SEND_CTRL_SELECTED_MAX           4               ///< ���ѡ�е�goose���Ϳ��ƿ�
#define MSGPARAM_GOOSE_REC_CTRL_SELECTED_MAX            8               ///< ���ѡ�е�goose���տ��ƿ�

class CParamConfigBase
{
protected:
    CParamConfigBase(void);
    virtual ~CParamConfigBase(void);

public:
    ///
    /// @brief
    ///    ���������ļ�
    ///
    /// @param[in]  strConfigFile �ļ�·��
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    virtual bool LoadConfig( std::wstring strConfigFile ) = 0;

    ///
    /// @brief
    ///    ���������ļ�
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    virtual bool SaveConfig() = 0;

    ///
    /// @brief
    ///    ��ò������ù������޸�״̬
    ///
    /// @return
    ///     bool    ����true��ʾ�������ù������ѱ�����
    ///
    bool IsModified();

    ///
    /// @brief
    ///    ���ò������ù������޸�״̬
    ///
    /// @param[in]  bModified ���ù������޸�״̬
    ///
    void SetModified(bool bModified);

    ///
    /// @brief
    ///    ��������ļ�����·��
    ///
    /// @return
    ///     std::wstring    �����ļ�����·��
    ///
    std::wstring GetConfigFile();

    ///
    /// @brief
    ///    ��ѯ9-1��FT3��FT3��չ���ƿ��״̬���Ƿ����˸ı�
    ///
    /// @return
    ///     bool    �����˸ı䷵��true
    ///
    bool IsStatusWordChanged() const;

    ///
    /// @brief
    ///    ����9-1��FT3��FT3��չ���ƿ��״̬���Ƿ����˸ı�
    ///
    /// @param[in]  bNewValue    �Ƿ�ı�
    ///
    void SetStatusWordChanged(bool bNewValue);

    ///
    /// @brief
    ///    ��ѯ9-2ͨ����Ʒ���Ƿ����˸ı�
    ///
    /// @return
    ///     bool    �����˸ı䷵��true
    ///
    bool IsQualityChanged() const;

    ///
    /// @brief
    ///    ����9-2ͨ����Ʒ���Ƿ����˸ı�
    ///
    /// @param[in]  bNewValue    �Ƿ�ı�
    ///
    void SetQualityChanged(bool bNewValue);

protected:
    bool                                m_bModified;                    ///< �Ƿ����޸�
    bool                                m_bStatusWordChanged;           ///< 9-1��FT3��FT3��չ���ƿ��״̬�ַ����˸ı�
    bool                                m_bQualityChanged;              ///< 9-2ͨ����Ʒ�ʷ����˸ı�
    std::wstring                        m_strConfigFile;                ///< �����ļ�����·��
};
#endif
