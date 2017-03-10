/// @file
///
/// @brief
///     GOOSE参数配置基类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.27
///
/// 修改历史：
///

#ifndef __PARAM_CONFIG_GOOSE_BASE_H__
#define __PARAM_CONFIG_GOOSE_BASE_H__
#include "GooseCb.h"
#include "ParamConfigBase.h"
#include "src/utils/MessageCodec/include/goose_msg.h"
#include "src/service/datalayer/recv/FrameRecvStruct.h"

#define NODE_ROOT                       L"config"                       ///< 根节点
#define NODE_HeartbeatInterval          L"HeartbeatInterval"            ///< 心跳间隔T0
#define NODE_MinimumInterval            L"MinimumInterval"              ///< 最小间隔T1
#define NODE_GocbGroup                  L"GocbGroup"                    ///< GOOSE控制块组
#define NODE_Gocb                       L"Gocb"                         ///< GOOSE控制块

class CParamConfigGOOSEBase : public CParamConfigBase
{
protected:
    CParamConfigGOOSEBase(void);
    virtual ~CParamConfigGOOSEBase(void);

public:
    ///
    /// @brief
    ///    添加GOOSE控制块
    ///
    /// @return
    ///     CGocb*    新添加的GOOSE控制块对象的指针
    ///
    CGooseCb* AddGocb();

    ///
    /// @brief
    ///     添加GOOSE控制块（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
    ///
    /// @param[in]
    ///     pGocb    -   GOOSE控制块指针
    ///
    /// @return
    ///     CGooseCb*    成功时返回新添加的GOOSE控制块指针，失败时返回NULL
    ///
    CGooseCb* AddGocb(CGooseCb* pGocb);

    ///
    /// @brief
    ///     添加扫描列表中的GOOSE控制块到容器中（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
    ///
    /// @param[in]
    ///     pDetectedGooseItem    -   扫描列表中的GOOSE控制块指针
    ///
    /// @return
    ///     CGooseCb*    成功时返回新添加的GOOSE控制块指针，失败时返回NULL
    ///
    CGooseCb* AddGocb(CDetectItemFrame* pDetectedGooseItem);

    ///
    /// @brief
    ///    删除GOOSE控制块
    ///
    /// @param[in]  pCurGocb GOOSE控制块指针
    ///
    /// @return
    ///     bool    true表示删除成功
    ///
    bool DeleteGocb(CGooseCb* pCurGocb);

    ///
    /// @brief
    ///    删除指定APPID的GOOSE控制块
    ///
    /// @param[in]  iAPPID 待删除的控制块APPID
    ///
    /// @return
    ///     bool    true表示删除成功
    ///
    bool DeleteGocb(int iAPPID);

    ///
    /// @brief
    ///    清除所有GOOSE控制块
    ///
    void ClearAllGocb();

    ///
    /// @brief
    ///    枚举第一个GOOSE控制块
    ///
    /// @return
    ///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
    ///
    CGooseCb* FirstGocb();

    ///
    /// @brief
    ///    枚举下一个GOOSE控制块
    ///
    /// @param[in]  pCurGocb 当前位置GOOSE控制块的指针
    ///
    /// @return
    ///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
    ///
    CGooseCb* NextGocb(CGooseCb* pCurGocb);

    ///
    /// @brief
    ///    枚举上一个GOOSE控制块
    ///
    /// @param[in]  pCurGocb 当前位置GOOSE控制块的指针
    ///
    /// @return
    ///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
    ///
    CGooseCb* PrevGocb(CGooseCb* pCurGocb);

    ///
    /// @brief
    ///    枚举最后一个GOOSE控制块
    ///
    /// @return
    ///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
    ///
    CGooseCb* LastGocb();

    ///
    /// @brief
    ///    获得控制块个数
    ///
    /// @return
    ///     int    控制块个数
    ///
    int GetGocbCount();

    ///
    /// @brief
    ///    获得被选中的控制块个数
    ///
    /// @return
    ///     int    被选中的控制块个数
    ///
    int GetSelectedGocbCount();

protected:
    ///
    /// @brief
    ///    添加GOOSE控制块
    ///
    /// @param[in]  pNewGocb 指向GOOSE控制块对象的指针
    ///
    /// @return
    ///     bool    true表示添加成功
    ///
    bool __AddGocb(CGooseCb* pNewGocb);

    ///
    /// @brief
    ///     获得指定APPID的GOOSE控制块（注意相同APPID的控制块可能有多个，此处只获得第一个匹配的控制块）
    ///
    /// @param[in]  
    ///     iAPPID  -   控制块的APPID
    ///
    /// @return
    ///     CGooseCb*   成功时返回控制块的指针，失败时返回NULL
    ///
    CGooseCb* __GetGocb(int iAPPID);

protected:
    std::list<CGooseCb *>               m_listGocbGroup;                ///< GOOSE控制块容器
};
#endif
