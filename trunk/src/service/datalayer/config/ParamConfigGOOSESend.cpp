/// @file
///
/// @brief
///     GOOSE发送配置
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.27
///
/// 修改历史：
///

#include "StdAfx.h"
#include "ParamConfigGOOSESend.h"

///
/// @brief
///     虚拟通道名
///
std::wstring strGOOSESendVCName[] =
{
    L"DO1",
    L"DO2",
    L"DO3",
    L"DO4",
    L"DO5",
    L"DO6"
};

///
/// @brief
///     构造函数
///
CParamConfigGOOSESend::CParamConfigGOOSESend(void)
:m_iHeartbeatInterval(5000)
,m_iMinimumInterval(2)
{
    CGOOSESendVirtualChannel*   pVirtualChannel   = NULL;
    std::map<std::wstring, CGOOSESendVirtualChannel*>::iterator iter;


    for (int i = 0;i < sizeof(strGOOSESendVCName)/sizeof(strGOOSESendVCName[0]) ; i++)
    {
        // new一个GOOSE接收虚拟通道对象
        pVirtualChannel = new CGOOSESendVirtualChannel;
        if (NULL == pVirtualChannel)
        {
            return ;
        }
        pVirtualChannel->SetName(strGOOSESendVCName[i]);

        // 添加虚拟通道对象到容器中
        iter = m_mapGOOSESendVirtualChannelGroup.find(strGOOSESendVCName[i]);
        if (iter != m_mapGOOSESendVirtualChannelGroup.end())
        {
            delete pVirtualChannel;
            return ;
        }
        m_mapGOOSESendVirtualChannelGroup.insert(std::pair<std::wstring, CGOOSESendVirtualChannel*>(strGOOSESendVCName[i], pVirtualChannel));
    }
}

///
/// @brief
///     析构函数
///
CParamConfigGOOSESend::~CParamConfigGOOSESend(void)
{
    CGOOSESendVirtualChannel*   pVirtualChannel   = NULL;
    std::map<std::wstring, CGOOSESendVirtualChannel*>::iterator iter;


    // 释放虚拟通道对象
    iter = m_mapGOOSESendVirtualChannelGroup.begin();
    while(m_mapGOOSESendVirtualChannelGroup.end() != iter)
    {
        pVirtualChannel = iter->second;
        if (pVirtualChannel)
        {
            delete pVirtualChannel;
        }

        iter++;
    }

    // 清空容器
    m_mapGOOSESendVirtualChannelGroup.clear();
}

///
/// @brief
///     获取配置实例接口
///
CParamConfigGOOSESend* CParamConfigGOOSESend::getInstance()
{
    static CParamConfigGOOSESend s_paramConfigGOOSESend;
    return &s_paramConfigGOOSESend;
}

///
/// @brief
///    加载配置文件
///
/// @param[in]  strConfigFile 配置文件完整路径
///
/// @return
///     bool    true表示加载成功
///
bool CParamConfigGOOSESend::LoadConfig( std::wstring strConfigFile )
{
    CMarkup                             xml;                            ///< 解析xml使用的CMarkUp对象


    if (strConfigFile.empty())
    {
        return false;
    }
    m_strConfigFile = strConfigFile;

    // 加载配置文件
    if (!xml.Load(m_strConfigFile))
    {
        return false;
    }

    // 清除所有GOOSE控制块
    ClearAllGocb();

    //
    // find and inside <config>
    //
    if (!xml.FindElem(NODE_ROOT) || !xml.IntoElem())
    {
        return false;
    }

    //
    // find <HeartbeatInterval>
    //
    if(!xml.FindElem(NODE_HeartbeatInterval))
    {
        return false;
    }
    m_iHeartbeatInterval = _wtoi(xml.GetData().c_str());

    //
    // find <MinimumInterval>
    //
    if(!xml.FindElem(NODE_MinimumInterval))
    {
        return false;
    }
    m_iMinimumInterval = _wtoi(xml.GetData().c_str());

    //
    // find and inside <GocbGroup>
    //
    if (xml.FindElem(NODE_GocbGroup) && xml.IntoElem())
    {
        //
        // find <Gocb>
        //
        while(xml.FindElem(NODE_Gocb))
        {
            CGooseCb*      pGocb = new CGooseCb(this);
            if (NULL == pGocb)
            {
                return false;
            }

            // 设置GOOSE控制块选中状态
            int iSelected = _wtoi(xml.GetAttrib(L"selected").c_str());
            if (iSelected != 0)
            {
                pGocb->m_bSelected  = true;
            }

            // inside <Gocb>
            if (!xml.IntoElem())
            {
                delete pGocb;
                return false;
            }

            // 加载GOOSE控制块
            if (!pGocb->__LoadConfig(xml))
            {
                delete pGocb;
                return false;
            }

            // outside <Gocb>
            if (!xml.OutOfElem())
            {
                delete pGocb;
                return false;
            }

            // 添加GOOSE控制块对象指针到容器
            if (!__AddGocb(pGocb))
            {
                delete pGocb;
                return false;
            }
        }

        // quit <GocbGroup>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    // 标记为未修改
    SetModified(false);
    return true;
}

///
/// @brief
///    保存配置文件
///
/// @return
///     bool    true表示保存成功
///
bool CParamConfigGOOSESend::SaveConfig()
{
    CMarkup                             xml;                            ///< 解析xml使用的CMarkUp对象


    // 如果没有任何改动，那么不保存
    if (!m_bModified)
    {
        return true;
    }

    // add and inside <config>
    if (!xml.AddElem(NODE_ROOT) || !xml.IntoElem())
    {
        return false;
    }

    // <HeartbeatInterval>
    if(!xml.AddElem(NODE_HeartbeatInterval) || !xml.SetData(m_iHeartbeatInterval))
    {
        return false;
    }

    // <MinimumInterval>
    if(!xml.AddElem(NODE_MinimumInterval) || !xml.SetData(m_iMinimumInterval))
    {
        return false;
    }

    //
    // 遍历容器，保存所有GOOSE控制块
    //
    if (m_listGocbGroup.size() > 0)
    {
        // <GocbGroup>
        if (!xml.AddElem(NODE_GocbGroup) || !xml.IntoElem())
        {
            return false;
        }

        // 单个GOOSE控制块
        std::list<CGooseCb*>::iterator iter = m_listGocbGroup.begin();
        while (m_listGocbGroup.end() != iter)
        {
            CGooseCb* pGocb = *iter;
            if (pGocb)
            {
                // <Gocb>
                if (!xml.AddElem(NODE_Gocb))
                {
                    return false;
                }

                // <Gocb selected="">
                int iSelected = pGocb->IsSelected()? 1: 0;
                if (!xml.SetAttrib(L"selected", iSelected))
                {
                    return false;
                }

                // inside <Gocb>
                if (!xml.IntoElem())
                {
                    return false;
                }

                // 保存GOOSE控制块
                if (!pGocb->__SaveConfig(xml))
                {
                    return false;
                }

                // outside <Gocb>
                if (!xml.OutOfElem())
                {
                    return false;
                }
            }

            iter++;
        }

        // Quit <GocbGroup>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    // 添加xml头
    xml.ResetPos();
    xml.InsertNode( CMarkup::MNT_PROCESSING_INSTRUCTION, L"xml" );
    xml.SetAttrib( L"version", L"1.0" );
    xml.SetAttrib( L"encoding", L"utf-8" );

    // 保存xml文件
    if (!xml.Save(m_strConfigFile))
    {
        return false;
    }

    // 标记为未修改
    SetModified(false);
    return true;
}

///
/// @brief
///    获得心跳间隔T0
///
/// @return
///     int     心跳间隔(ms)
///
int CParamConfigGOOSESend::GetHeartbeatInterval()
{
    return m_iHeartbeatInterval;
}

///
/// @brief
///    设置心跳间隔T0
///
/// @param[in]  iNewInterval 心跳间隔T0(ms)
///
/// @return
///     bool    true表示保存成功
///
bool CParamConfigGOOSESend::SetHeartbeatInterval(int iNewInterval)
{
    if ((iNewInterval < 500) || (iNewInterval > 65535))
    {
        return false;
    }

    if (m_iHeartbeatInterval != iNewInterval)
    {
        m_iHeartbeatInterval = iNewInterval;

        // 标记为已修改
        SetModified(true);
    }

    return true;
}

///
/// @brief
///    获得最小间隔T1
///
/// @return
///     int     最小间隔(ms)，失败时返回-1
///
int CParamConfigGOOSESend::GetMinimumInterval()
{
    return m_iMinimumInterval;
}

///
/// @brief
///    设置最小间隔T1
///
/// @param[in]  iNewInterval 最小间隔T1(ms)
///
/// @return
///     bool    true表示保存成功
///
bool CParamConfigGOOSESend::SetMinimumInterval(int iNewInterval)
{
    if ((iNewInterval < 1) || (iNewInterval > 65535))
    {
        return false;
    }

    if (m_iMinimumInterval != iNewInterval)
    {
        m_iMinimumInterval = iNewInterval;

        // 标记为已修改
        SetModified(true);
    }

    return true;
}

///
/// @brief
///    枚举第一个GOOSE发送虚拟通道
///
///
/// @return
///     CGOOSESendVirtualChannel*    虚拟通道指针，失败时返回NULL
///
CGOOSESendVirtualChannel* CParamConfigGOOSESend::FirstGOOSESendVirtualChannel()
{
    std::map<std::wstring, CGOOSESendVirtualChannel*>::iterator iter;


    // 生成所有GOOSE发送虚拟通道的真实通道映射表
    RebuildGOOSESendChannelMap();

    // 查找第一个虚拟通道
    iter = m_mapGOOSESendVirtualChannelGroup.begin();
    if (m_mapGOOSESendVirtualChannelGroup.end() != iter)
    {
        return (iter->second);
    }

    return NULL;
}

///
/// @brief
///    枚举下一个GOOSE发送虚拟通道
///
/// @param[in]  pCurVirtualChannel    指向当前虚拟通道的指针
///
/// @return
///     CGOOSESendVirtualChannel*    下一个虚拟通道指针，失败时返回NULL
///
CGOOSESendVirtualChannel* CParamConfigGOOSESend::NextGOOSESendVirtualChannel( CGOOSESendVirtualChannel* pCurVirtualChannel )
{
    CGOOSESendVirtualChannel*               pVirtualChannel = NULL;


    // 输入参数检查
    if (NULL == pCurVirtualChannel)
    {
        return NULL;
    }

    // 获得当前虚拟通道位置
    std::map<std::wstring, CGOOSESendVirtualChannel*>::iterator iter = m_mapGOOSESendVirtualChannelGroup.find(pCurVirtualChannel->GetName());
    if (m_mapGOOSESendVirtualChannelGroup.end() == iter)
    {
        return NULL;
    }

    // 指向下一个虚拟通道
    if (m_mapGOOSESendVirtualChannelGroup.end() != ++iter)
    {
        // 获得虚拟通道对象指针
        pVirtualChannel = iter->second;
    }

    return pVirtualChannel;
}

///
/// @brief
///    生成所有GOOSE发送虚拟通道的真实通道映射表
///    注意：调用FirstGOOSESendVirtualChannel()函数时会自动调用此函数，
///           如果需要重新生成映射表，可以手动调用此函数
///
/// @return
///     bool    成功返回true
///
bool CParamConfigGOOSESend::RebuildGOOSESendChannelMap()
{
    std::map<std::wstring, CGOOSESendVirtualChannel*>::iterator iter;
    CGooseCb*                           pGocb           = NULL;
    CChannel*                           pChannel        = NULL;
    CChannel*                           pSubChannel     = NULL;
    CGOOSESendVirtualChannel*           pVirtualChannel = NULL;


    // 清除先前的映射
    iter = m_mapGOOSESendVirtualChannelGroup.begin();
    while (m_mapGOOSESendVirtualChannelGroup.end() != iter)
    {
        pVirtualChannel = iter->second;
        if (pVirtualChannel)
        {
            pVirtualChannel->m_vecChannelGroup.clear();
        }

        iter++;
    }

    //
    // 遍历所有控制块，生成虚拟通道映射表
    //
    pGocb = FirstGocb();
    while(pGocb)
    {
        // 如果控制块被选中
        if (pGocb->IsSelected())
        {
            pChannel = pGocb->FirstChannel();
            while(pChannel)
            {
                // 如果是结构体类型的通道
                enumChannelType eChannelType = pChannel->GetChannelType();
                if (eChannelType == CHANNEL_TYPE_GOOSE_STRUCT)
                {
                    pSubChannel = pChannel->FirstSubChannel();
                    while(pSubChannel)
                    {
                        if ((pSubChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_POINT) ||
                            (pSubChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_DOUBLE_POINT))
                        {
                            // 如果子通道设置了虚拟通道
                            std::wstring strVCName  = pSubChannel->GetVirtualChannel();
                            if (strVCName.size())
                            {
                                iter = m_mapGOOSESendVirtualChannelGroup.find(strVCName);
                                if ((m_mapGOOSESendVirtualChannelGroup.end() != iter) && (NULL != iter->second))
                                {
                                    // 添加虚拟通道关联的真实通道
                                    iter->second->m_vecChannelGroup.push_back(pSubChannel);
                                }
                            }
                        }

                        pSubChannel = pChannel->NextSubChannel(pSubChannel);
                    }
                }
                else if((CHANNEL_TYPE_GOOSE_POINT       == eChannelType) ||
                    (CHANNEL_TYPE_GOOSE_DOUBLE_POINT    == eChannelType))
                {
                    // 如果是普通通道且设置了虚拟通道名
                    std::wstring strVCName  = pChannel->GetVirtualChannel();
                    if (strVCName.size())
                    {
                        iter = m_mapGOOSESendVirtualChannelGroup.find(strVCName);
                        if ((m_mapGOOSESendVirtualChannelGroup.end() != iter) && (NULL != iter->second))
                        {
                            // 添加虚拟通道关联的真实通道
                            iter->second->m_vecChannelGroup.push_back(pChannel);
                        }
                    }
                }

                pChannel = pGocb->NextChannel(pChannel);
            }
        }

        pGocb = NextGocb(pGocb);
    }

    return true;
}

///
/// @brief
///    根据DO名获得GOOSE发送虚拟通道对象
///
/// @param[in]  strChannelName    GOOSE发送虚拟通道的名字
///
/// @return
///     CGOOSESendVirtualChannel*    指向GOOSE发送虚拟通道对象的指针
///
CGOOSESendVirtualChannel* CParamConfigGOOSESend::GetGOOSESendVirtualChannel( std::wstring strChannelName )
{
    CGOOSESendVirtualChannel*                   pVirtualChannel = NULL;
    std::map<std::wstring, CGOOSESendVirtualChannel*>::iterator iter;


    // 查找指定名字的虚拟通道
    iter = m_mapGOOSESendVirtualChannelGroup.find(strChannelName);
    if ((m_mapGOOSESendVirtualChannelGroup.end() != iter))
    {
        pVirtualChannel = iter->second;
        if (pVirtualChannel && (pVirtualChannel->GetName() == strChannelName))
        {
            return pVirtualChannel;
        }
    }

    return NULL;
}

///
/// @brief
///     更新映射到指定虚拟通道的所有真实通道的值
///
/// @param[in]  mapDoChangedArray   虚拟通道名和值
///
/// @return
///     bool    成功返回true
///
bool CParamConfigGOOSESend::UpdateGooseChannelSendValue( std::map<std::wstring, bool>& mapDoChangedArray)
{
    CGOOSESendVirtualChannel*   pVirtualChannel = NULL;
    std::map<std::wstring, bool>::iterator iterDoChanged;
    std::map<std::wstring, CGOOSESendVirtualChannel*>::iterator iterVirtualChannel;


    // 遍历每一个值发生了改变的DO通道
    for (iterDoChanged = mapDoChangedArray.begin(); iterDoChanged != mapDoChangedArray.end(); iterDoChanged++)
    {
        std::wstring    strDoName   = iterDoChanged->first;
        bool            bNewValue   = iterDoChanged->second;


        // 查找指定名字的GOOSE发送虚拟通道
        iterVirtualChannel = m_mapGOOSESendVirtualChannelGroup.find(strDoName);
        if (m_mapGOOSESendVirtualChannelGroup.end() == iterVirtualChannel)
        {
            return false;
        }

        // 获得GOOSE发送虚拟通道对象的指针
        pVirtualChannel = iterVirtualChannel->second;
        if ((NULL == pVirtualChannel) || (pVirtualChannel->GetName() != strDoName))
        {
            return false;
        }

        // 设置所有映射到此虚拟通道的真实通道的值
        if (!pVirtualChannel->SetChannelSendValue(bNewValue))
        {
            return false;
        }
    }

    return true;
}
