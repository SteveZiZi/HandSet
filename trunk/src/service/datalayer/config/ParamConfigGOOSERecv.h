/// @file
///
/// @brief
///     GOOSE���ղ�������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.27
///
/// �޸���ʷ��
///

#ifndef __PARAM_CONFIG_GOOSE_RECV_H__
#define __PARAM_CONFIG_GOOSE_RECV_H__
#include <map>
#include "ParamConfigGOOSEBase.h"
#include "GOOSERecvVirtualChannel.h"

class CParamConfigGOOSERecv : public CParamConfigGOOSEBase
{
protected:
    CParamConfigGOOSERecv(void);
    ~CParamConfigGOOSERecv(void);

public:
    ///
    /// @brief
    ///     ��ȡ����ʵ���ӿ�
    ///
    static CParamConfigGOOSERecv* getInstance();

    ///
    /// @brief
    ///    ���������ļ�
    ///
    /// @param[in]  strConfigFile �ļ�·��
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool LoadConfig( std::wstring strConfigFile );

    ///
    /// @brief
    ///    ���������ļ�
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool SaveConfig();

    ///
    /// @brief
    ///    ��������GOOSE��������ͨ������ʵͨ��ӳ���
    ///    ע�⣺����FirstGOOSERecvVirtualChannel()����ʱ���Զ����ô˺�����
    ///           �����Ҫ��������ӳ��������ֶ����ô˺���
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool RebuildGOOSERecvChannelMap();

    ///
    /// @brief
    ///    ���GOOSE��������ͨ������ʵͨ��ӳ���
    ///
    void ClearGOOSERecvChannelMap();

    ///
    /// @brief
    ///    ö�ٵ�һ��GOOSE��������ͨ��
    ///
    ///
    /// @return
    ///     CGOOSERecvVirtualChannel*    ����ͨ��ָ�룬ʧ��ʱ����NULL
    ///
    CGOOSERecvVirtualChannel* FirstGOOSERecvVirtualChannel();

    ///
    /// @brief
    ///    ö����һ��GOOSE��������ͨ��
    ///
    /// @param[in]  pCurVirtualChannel    ָ��ǰ����ͨ����ָ��
    ///
    /// @return
    ///     CGOOSERecvVirtualChannel*    ��һ������ͨ��ָ�룬ʧ��ʱ����NULL
    ///
    CGOOSERecvVirtualChannel* NextGOOSERecvVirtualChannel(CGOOSERecvVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    ���ָ�����ֵ�����ͨ��
    ///
    /// @param[in]  strVCName    ����ͨ����
    ///
    /// @return
    ///     CGOOSERecvVirtualChannel*    ָ������ͨ�������ָ�룬ʧ��ʱ����NULL
    ///
    CGOOSERecvVirtualChannel* GetVirtualChannelByName(std::wstring strVCName);

    ///
    /// @brief
    ///    ����ͨ����ʵʱֵ
    ///
    /// @param[in]  iAPPID      ͨ���������ƿ��APPID
    /// @param[in]  pGooseMsg   GOOSE���ĵı��������ں�GOOSEͨ��ֵ
    /// @param[in]  stFrameInfo GOOSE����ͷ
    ///
    void SetChannelRecvValue(int iAPPID, GooseMsg* pGooseMsg, FrameInfo& stFrameInfo);

private:
    std::map<std::wstring, CGOOSERecvVirtualChannel*>   m_mapGOOSERecvVirtualChannelGroup;  ///< ����ͨ������
};
#endif
