/// @file
///
/// @brief
///     描述goose控制块信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.6.6
///
/// 修改历史：
///

#pragma once

#include <vector>
#include "src\service\datalayer\include\datatypes.h"
#include "src\utils\MessageCodec\include\goose_types.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"

///
/// @brief
///    goose通道信息
///
class CGooseChannel
{
public:
    CGooseChannel(): m_nID(0)
                   , m_enChannelType(CHANNEL_TYPE_UNKOWN)
                   , m_strChannelDesc(L"")
                   , m_pChannelData(NULL)
                   , m_pParentChannel(NULL)
    {
        m_vecSubChannels.clear();
    }

    ~CGooseChannel()
    {
        if ( NULL != m_pChannelData)
        {
            GooseDataFree(m_pChannelData);
            m_pChannelData = NULL;
        }
        ClearSubChannels();
    }

    ///
    /// @brief
    ///    设置通道id
    ///
    void SetChannelID(uint32_t nID)
    {
        m_nID = nID;
    }

    ///
    /// @brief
    ///    获取通道id（从0开始）
    ///
    uint32_t GetChannelID()
    {
        return m_nID;
    }

    ///
    /// @brief
    ///    设置通道类型
    ///
    void SetChannelType(enumChannelType enChannelType)
    {
        m_enChannelType = enChannelType;
    }

    ///
    /// @brief
    ///    获取通道类型
    ///
    enumChannelType GetChannelType()
    {
        return m_enChannelType;
    }

    ///
    /// @brief
    ///    设置通道描述
    ///
    void SetChannelDesc(std::wstring strChannelDesc)
    {
        m_strChannelDesc = strChannelDesc;
    }

    ///
    /// @brief
    ///    获取通道描述
    ///
    std::wstring GetChannelDesc()
    {
        return m_strChannelDesc;
    }

    ///
    /// @brief
    ///    设置父通道
    ///
    void SetParentChannel(CGooseChannel* pChannel)
    {
        m_pParentChannel = pChannel;
    }

    ///
    /// @brief
    ///    获取父通道
    ///
    CGooseChannel* GetParentChannel()
    {
        return m_pParentChannel;
    }

    ///
    /// @brief
    ///    设置通道数据
    ///
    void SetChannelData(GOOSE_DATA* pData)
    {
        if (m_pChannelData)
        {
            GooseDataFree(m_pChannelData);
            m_pChannelData = NULL;
        }

        if (pData->type == GSEDT_STRUCT)
        {
            GOOSE_STRUCT* pGooseStruct = (GOOSE_STRUCT*)pData;
            std::vector<GOOSE_DATA *>::iterator it;
            CGooseChannel stSubChannel;
            uint32_t nSubChannelId = 0;
            for ( it = pGooseStruct->member.begin(); it != pGooseStruct->member.end(); it++, nSubChannelId++ )
            {
                stSubChannel.SetChannelID(nSubChannelId);
                stSubChannel.SetChannelData((*it));
                AddSubChannel(&stSubChannel);
            }
        }

        m_pChannelData = GooseDataCopy(pData);
    }

    ///
    /// @brief
    ///    获取通道数据指针
    ///
    GOOSE_DATA* GetChannelData()
    {
        return m_pChannelData;
    }

    ///
    /// @brief
    ///    添加一个子通道
    ///
    bool AddSubChannel(CGooseChannel* pSubChannel);

    ///
    /// @brief
    ///    移除一个子通道
    ///
    bool RemoveSubChannel(uint32_t nIndex);

    ///
    /// @brief
    ///    移除一个子通道
    ///
    bool RemoveSubChannel(CGooseChannel* pSubChannel);

    ///
    /// @brief
    ///    获取子通道数目
    ///
    uint32_t GetSubChannelCount();

    ///
    /// @brief
    ///    获取一个子通道
    ///
    CGooseChannel* GetSubChannelData(uint32_t nIndex);

    ///
    /// @brief
    ///    清除子通道数据
    ///
    void ClearSubChannels();

private:
    uint32_t                             m_nID;                         ///< 通道id
    enumChannelType                      m_enChannelType;               ///< 通道类型(从scl文件中获取)
    std::wstring                         m_strChannelDesc;              ///< 通道描述(从scl文件获取)
    GOOSE_DATA*                          m_pChannelData;                ///< 通道数据内容(从报文帧中获取)
    CGooseChannel*                       m_pParentChannel;              ///< 当前通道的父通道指针，为NULL表示没有父通道
    std::vector<CGooseChannel*>          m_vecSubChannels;              ///< 子通道信息，主要包括通道描述和类型（从SCL文件获取）
};

///
/// @brief
///    goose控制块信息
///
class CGooseInfo
{
public:
    CGooseInfo(): m_pCtrlBlk(NULL)
        , m_nStNumber(0)
        , m_nSqNumber(0)
        , m_nTTL(0)
    {
        memset(&m_stEventTime, 0, sizeof(m_stEventTime));
    }

    ~CGooseInfo()
    {
        ClearChannelData();
    }

    ///
    /// @brief
    ///    设置控制块指针
    ///
    void SetCtrlBlk(CBaseDetectItem* pItem)
    {
        m_pCtrlBlk = pItem;
    }

    ///
    /// @brief
    ///    获取控制块指针
    ///
    CBaseDetectItem* GetCtrlBlk()
    {
        return m_pCtrlBlk;
    }

    ///
    /// @brief
    ///    设置goose生存时间TTL值
    ///
    void SetGooseTTL(uint32_t nTTL)
    {
        m_nTTL = nTTL;
    }

    ///
    /// @brief
    ///    获取goose生存时间值
    ///
    uint32_t GetGooseTTL()
    {
        return m_nTTL;
    }

    ///
    /// @brief
    ///    设置goose变位次数
    ///
    void SetGooseStNum(uint32_t nStNumber)
    {
        m_nStNumber = nStNumber;
    }

    ///
    /// @brief
    ///    获取Goose变位次数
    ///
    uint32_t GetGooseStNum()
    {
        return m_nStNumber;
    }

    ///
    /// @brief
    ///    设置goose状态计数
    ///
    void SetGooseSqNum(uint32_t nSqNumber)
    {
        m_nSqNumber = nSqNumber;
    }

    ///
    /// @brief
    ///    获取goose状态计数
    ///
    uint32_t GetGooseSqNum()
    {
        return m_nSqNumber;
    }

    ///
    /// @brief
    ///    添加一个通道
    ///
    /// @param[in]      pChannel    要添加的通道数据指针
    ///
    /// @return
    ///     添加成功返回true
    ///
    /// @note
    ///     pChannel指针不会被直接添加进控制块
    ///     函数会新申请空间，拷贝pChannel指向的数据，然后添加进控制块
    ///
    bool AddChannel(CGooseChannel* pChannel);

    ///
    /// @brief
    ///    根据通道索引（从0开始）移除一个通道
    ///
    bool RemoveChannel(uint32_t nIndex);

    ///
    /// @brief
    ///    根据通道信息移除一个通道，内部还是使用通道索引进行通道移除
    ///
    bool RemoveChannel(CGooseChannel* pChannel);

    ///
    /// @brief
    ///    清空通道数据
    ///
    void ClearChannelData();

    ///
    /// @brief
    ///    获取通道数目
    ///
    uint32_t GetChannelCounts()
    {
        return m_vecChannels.size();
    }

    ///
    /// @brief
    ///    获取一个通道数据
    ///
    /// @param[in]      nIndex  通道索引，从0到GetChannelCounts()
    ///
    /// @return
    ///     CGooseChannel* 返回对应通道数据指针，失败返回NULL
    ///
    CGooseChannel* GetChanneData(uint32_t nIndex);

    ///
    /// @brief
    ///    设置goose变位时间
    ///
    void SetEventTime(uint32_t t_s, uint32_t t_ns, uint8_t t_q)
    {
        m_stEventTime.id.type = GSEDT_UTC_TIME;
        m_stEventTime.t_s = t_s;
        m_stEventTime.t_ns = t_ns;
        m_stEventTime.t_q = t_q;
    }

    ///
    /// @brief
    ///    获取goose变位时间
    ///
    GOOSE_UTC_TIME GetEventTime()
    {
        return m_stEventTime;
    }

    ///
    /// @brief
    ///    根据scl文件，填充通道描述/类型等信息
    ///
    /// @param[in]  pGooseInfo  要填充的gooseInfo信息指针
    ///
    /// @return
    ///     bool 从scl总获取信息成功并填充返回true
    ///
    static bool FillGooseChannelDescAndType(CGooseInfo* pGooseInfo);


private:
    CBaseDetectItem*                     m_pCtrlBlk;                    ///< 对应的控制块指针
    uint32_t                             m_nTTL;                        ///< goose数据生存时间
    uint32_t                             m_nStNumber;                   ///< goose变位次数计数
    uint32_t                             m_nSqNumber;                   ///< goose同一状态数据计数
    GOOSE_UTC_TIME                       m_stEventTime;                 ///< Goose变位时间
    std::vector<CGooseChannel*>          m_vecChannels;                 ///< 控制块对应的通道信息
};
