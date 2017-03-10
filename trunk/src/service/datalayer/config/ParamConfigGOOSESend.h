/// @file
///
/// @brief
///     GOOSE��������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.27
///
/// �޸���ʷ��
///

#ifndef __PARAM_CONFIG_GOOSE_SEND_H__
#define __PARAM_CONFIG_GOOSE_SEND_H__
#include <map>
#include "ParamConfigGOOSEBase.h"
#include "GOOSESendVirtualChannel.h"

///
/// @brief
///    GOOSE���Ͳ���������
///
class CParamConfigGOOSESend : public CParamConfigGOOSEBase
{
    friend class CStateTestResult;

protected:
    CParamConfigGOOSESend(void);
    ~CParamConfigGOOSESend(void);

public:
    ///
    /// @brief
    ///     ��ȡ����ʵ���ӿ�
    ///
    static CParamConfigGOOSESend* getInstance();

    ///
    /// @brief
    ///    ���������ļ�
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
    ///    ����������T0
    ///
    /// @return
    ///     int     �������(ms)
    ///
    int GetHeartbeatInterval();

    ///
    /// @brief
    ///    �����������T0
    ///
    /// @param[in]  iNewInterval �������T0(ms)
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool SetHeartbeatInterval(int iNewInterval);

    ///
    /// @brief
    ///    �����С���T1
    ///
    /// @return
    ///     int     ��С���(ms)��ʧ��ʱ����-1
    ///
    int GetMinimumInterval();

    ///
    /// @brief
    ///    ������С���T1
    ///
    /// @param[in]  iNewInterval ��С���T1(ms)
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool SetMinimumInterval(int iNewInterval);

    ///
    /// @brief
    ///    ö�ٵ�һ��GOOSE��������ͨ��
    ///
    ///
    /// @return
    ///     CGOOSESendVirtualChannel*    ����ͨ��ָ�룬ʧ��ʱ����NULL
    ///
    CGOOSESendVirtualChannel* FirstGOOSESendVirtualChannel();

    ///
    /// @brief
    ///    ö����һ��GOOSE��������ͨ��
    ///
    /// @param[in]  pCurVirtualChannel    ָ��ǰ����ͨ����ָ��
    ///
    /// @return
    ///     CGOOSESendVirtualChannel*    ��һ������ͨ��ָ�룬ʧ��ʱ����NULL
    ///
    CGOOSESendVirtualChannel* NextGOOSESendVirtualChannel(CGOOSESendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    ����DO�����GOOSE��������ͨ������
    ///
    /// @param[in]  strChannelName    GOOSE��������ͨ��������
    ///
    /// @return
    ///     CGOOSESendVirtualChannel*    ָ��GOOSE��������ͨ�������ָ��
    ///
    CGOOSESendVirtualChannel* GetGOOSESendVirtualChannel(std::wstring strChannelName);

    ///
    /// @brief
    ///    ��������GOOSE��������ͨ������ʵͨ��ӳ���
    ///    ע�⣺����FirstGOOSESendVirtualChannel()����ʱ���Զ����ô˺�����
    ///           �����Ҫ��������ӳ��������ֶ����ô˺���
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool RebuildGOOSESendChannelMap();

    ///
    /// @brief
    ///     ����ӳ�䵽ָ������ͨ����������ʵͨ����ֵ
    ///
    /// @param[in]  mapDoChangedArray   ����ͨ������ֵ
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool UpdateGooseChannelSendValue(std::map<std::wstring, bool>& mapDoChangedArray);

private:
    int                                 m_iHeartbeatInterval;           ///< �������T0
    int                                 m_iMinimumInterval;             ///< ��С���T1

    std::map<std::wstring, CGOOSESendVirtualChannel*>   m_mapGOOSESendVirtualChannelGroup;  ///< ����ͨ������
};
#endif
