/// @file
///
/// @brief
///     GOOSE接收参数配置
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.27
///
/// 修改历史：
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
    ///     获取配置实例接口
    ///
    static CParamConfigGOOSERecv* getInstance();

    ///
    /// @brief
    ///    加载配置文件
    ///
    /// @param[in]  strConfigFile 文件路径
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    bool LoadConfig( std::wstring strConfigFile );

    ///
    /// @brief
    ///    保存配置文件
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    bool SaveConfig();

    ///
    /// @brief
    ///    生成所有GOOSE接收虚拟通道的真实通道映射表
    ///    注意：调用FirstGOOSERecvVirtualChannel()函数时会自动调用此函数，
    ///           如果需要重新生成映射表，可以手动调用此函数
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool RebuildGOOSERecvChannelMap();

    ///
    /// @brief
    ///    清除GOOSE接收虚拟通道的真实通道映射表
    ///
    void ClearGOOSERecvChannelMap();

    ///
    /// @brief
    ///    枚举第一个GOOSE接收虚拟通道
    ///
    ///
    /// @return
    ///     CGOOSERecvVirtualChannel*    虚拟通道指针，失败时返回NULL
    ///
    CGOOSERecvVirtualChannel* FirstGOOSERecvVirtualChannel();

    ///
    /// @brief
    ///    枚举下一个GOOSE接收虚拟通道
    ///
    /// @param[in]  pCurVirtualChannel    指向当前虚拟通道的指针
    ///
    /// @return
    ///     CGOOSERecvVirtualChannel*    下一个虚拟通道指针，失败时返回NULL
    ///
    CGOOSERecvVirtualChannel* NextGOOSERecvVirtualChannel(CGOOSERecvVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    获得指定名字的虚拟通道
    ///
    /// @param[in]  strVCName    虚拟通道名
    ///
    /// @return
    ///     CGOOSERecvVirtualChannel*    指向虚拟通道对象的指针，失败时返回NULL
    ///
    CGOOSERecvVirtualChannel* GetVirtualChannelByName(std::wstring strVCName);

    ///
    /// @brief
    ///    设置通道的实时值
    ///
    /// @param[in]  iAPPID      通道所属控制块的APPID
    /// @param[in]  pGooseMsg   GOOSE报文的编解码对象，内含GOOSE通道值
    /// @param[in]  stFrameInfo GOOSE报文头
    ///
    void SetChannelRecvValue(int iAPPID, GooseMsg* pGooseMsg, FrameInfo& stFrameInfo);

private:
    std::map<std::wstring, CGOOSERecvVirtualChannel*>   m_mapGOOSERecvVirtualChannelGroup;  ///< 虚拟通道容器
};
#endif
