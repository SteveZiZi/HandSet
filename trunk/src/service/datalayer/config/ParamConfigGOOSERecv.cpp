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

#include "StdAfx.h"
#include "ParamConfigGOOSERecv.h"
#include "src\utils\MessageCodec\include\goose_types.h"
#include "src\service\datalayer\send\FrameSend.h"
#include "src\service\datalayer\DsmDataLayer.h"

///
/// @brief
///     构造函数
///
CParamConfigGOOSERecv::CParamConfigGOOSERecv(void)
{
    CGOOSERecvVirtualChannel*   pVirtualChannel   = NULL;
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter;


    for (int i = 0;i < sizeof(strGOOSERecvVCName)/sizeof(strGOOSERecvVCName[0]) ; i++)
    {
        // new一个GOOSE接收虚拟通道对象
        pVirtualChannel = new CGOOSERecvVirtualChannel;
        if (NULL == pVirtualChannel)
        {
            return ;
        }
        pVirtualChannel->SetName(strGOOSERecvVCName[i]);

        // 添加虚拟通道对象到容器中
        iter = m_mapGOOSERecvVirtualChannelGroup.find(strGOOSERecvVCName[i]);
        if (iter != m_mapGOOSERecvVirtualChannelGroup.end())
        {
            delete pVirtualChannel;
            return ;
        }
        m_mapGOOSERecvVirtualChannelGroup.insert(std::pair<std::wstring, CGOOSERecvVirtualChannel*>(strGOOSERecvVCName[i], pVirtualChannel));
    }
}

///
/// @brief
///     析构函数
///
CParamConfigGOOSERecv::~CParamConfigGOOSERecv(void)
{
    CGOOSERecvVirtualChannel*   pVirtualChannel   = NULL;
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter;


    // 释放虚拟通道对象
    iter = m_mapGOOSERecvVirtualChannelGroup.begin();
    while(m_mapGOOSERecvVirtualChannelGroup.end() != iter)
    {
        pVirtualChannel = iter->second;
        if (pVirtualChannel)
        {
            delete pVirtualChannel;
        }

        iter++;
    }

    // 清空容器
    m_mapGOOSERecvVirtualChannelGroup.clear();
}

///
/// @brief
///     获取配置实例接口
///
CParamConfigGOOSERecv* CParamConfigGOOSERecv::getInstance()
{
    static CParamConfigGOOSERecv s_paramConfigGOOSERecv;
    return &s_paramConfigGOOSERecv;
}

///
/// @brief
///    生成所有GOOSE接收虚拟通道的真实通道映射表
///    注意：调用FirstGOOSERecvVirtualChannel()函数时会自动调用此函数，
///           如果需要重新生成映射表，可以手动调用此函数
///
/// @return
///     bool    成功返回true
///
bool CParamConfigGOOSERecv::RebuildGOOSERecvChannelMap()
{
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter;
    CGOOSERecvVirtualChannel*           pVirtualChannel = NULL;
    CGooseCb*                           pGocb           = NULL;
    CChannel*                           pChannel        = NULL;
    CChannel*                           pSubChannel     = NULL;
    int                                 iCount          = 0;


    //
    // 清除先前的所有映射
    //
    iter = m_mapGOOSERecvVirtualChannelGroup.begin();
    while (m_mapGOOSERecvVirtualChannelGroup.end() != iter)
    {
        pVirtualChannel = iter->second;
        if (pVirtualChannel)
        {
            pVirtualChannel->m_pChannel = NULL;
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
                if (pChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_STRUCT)
                {
                    pSubChannel = pChannel->FirstSubChannel();
                    while(pSubChannel)
                    {
                        // 如果子通道是单点或双点类型，并且被选中
                        if ((CHANNEL_TYPE_GOOSE_POINT        == pSubChannel->GetChannelType()   || 
                             CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pSubChannel->GetChannelType())  && 
                             pSubChannel->IsSelected())
                        {
                            iter = m_mapGOOSERecvVirtualChannelGroup.find(strGOOSERecvVCName[iCount]);
                            if ((m_mapGOOSERecvVirtualChannelGroup.end() != iter) && (NULL != iter->second))
                            {
                                // 修改虚拟通道关联的真实通道
                                iter->second->m_pChannel = pSubChannel;

                                // 如果计数到虚拟通道的上限，那么跳出循环
                                iCount++;
                                if (iCount >= sizeof(strGOOSERecvVCName)/sizeof(strGOOSERecvVCName[0]))
                                {
                                    return true;
                                }
                            }
                        }

                        pSubChannel = pChannel->NextSubChannel(pSubChannel);
                    }
                }
                else
                {
                    // 如果是普通的单点或双点通道，并且被选中
                    if ((CHANNEL_TYPE_GOOSE_POINT       == pChannel->GetChannelType()   || 
                        CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pChannel->GetChannelType())  && 
                        pChannel->IsSelected())
                    {
                        iter = m_mapGOOSERecvVirtualChannelGroup.find(strGOOSERecvVCName[iCount]);
                        if ((m_mapGOOSERecvVirtualChannelGroup.end() != iter) && (NULL != iter->second))
                        {
                            // 修改虚拟通道关联的真实通道
                            iter->second->m_pChannel = pChannel;

                            // 如果计数到虚拟通道的上限，那么跳出循环
                            iCount++;
                            if (iCount >= sizeof(strGOOSERecvVCName)/sizeof(strGOOSERecvVCName[0]))
                            {
                                return true;
                            }
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
///    清除GOOSE接收虚拟通道的真实通道映射表
///
void CParamConfigGOOSERecv::ClearGOOSERecvChannelMap()
{
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter;
    CGOOSERecvVirtualChannel*           pVirtualChannel = NULL;
    CGooseCb*                           pGocb           = NULL;
    CChannel*                           pChannel        = NULL;
    CChannel*                           pSubChannel     = NULL;


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
                // 取消通道勾选状态
                pChannel->Selected(false);

                // 遍历子通道
                pSubChannel = pChannel->FirstSubChannel();
                while(pSubChannel)
                {
                    // 取消子通道勾选状态
                    pSubChannel->Selected(false);
                    pSubChannel = pChannel->NextSubChannel(pSubChannel);
                }

                pChannel = pGocb->NextChannel(pChannel);
            }
        }

        pGocb = NextGocb(pGocb);
    }

    //
    // 清除先前的所有映射
    //
    iter = m_mapGOOSERecvVirtualChannelGroup.begin();
    while (m_mapGOOSERecvVirtualChannelGroup.end() != iter)
    {
        pVirtualChannel = iter->second;
        if (pVirtualChannel)
        {
            pVirtualChannel->m_pChannel = NULL;
        }

        iter++;
    }
}

///
/// @brief
///    枚举第一个GOOSE接收虚拟通道
///
///
/// @return
///     CGOOSERecvVirtualChannel*    虚拟通道指针
///
CGOOSERecvVirtualChannel* CParamConfigGOOSERecv::FirstGOOSERecvVirtualChannel()
{
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter;
    CGOOSERecvVirtualChannel*           pVirtualChannel = NULL;


    // 生成所有GOOSE接收虚拟通道的真实通道映射表
    if (!RebuildGOOSERecvChannelMap())
    {
        return NULL;
    }

    // 查找第一个虚拟通道
    iter = m_mapGOOSERecvVirtualChannelGroup.begin();
    if (m_mapGOOSERecvVirtualChannelGroup.end() != iter)
    {
        pVirtualChannel = iter->second;
    }

    return pVirtualChannel;
}

///
/// @brief
///    枚举下一个GOOSE接收虚拟通道
///
/// @param[in]  pCurVirtualChannel    指向当前虚拟通道的指针
///
/// @return
///     CGOOSERecvVirtualChannel*    下一个虚拟通道指针，失败时返回NULL
///
CGOOSERecvVirtualChannel* CParamConfigGOOSERecv::NextGOOSERecvVirtualChannel( CGOOSERecvVirtualChannel* pCurVirtualChannel )
{
    CGOOSERecvVirtualChannel*               pVirtualChannel = NULL;


    // 输入参数检查
    if (NULL == pCurVirtualChannel)
    {
        return NULL;
    }

    // 获得当前虚拟通道位置
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter = m_mapGOOSERecvVirtualChannelGroup.find(pCurVirtualChannel->GetName());
    if (m_mapGOOSERecvVirtualChannelGroup.end() == iter)
    {
        return NULL;
    }

    // 指向下一个虚拟通道
    if (m_mapGOOSERecvVirtualChannelGroup.end() != ++iter)
    {
        // 获得虚拟通道对象指针
        pVirtualChannel = iter->second;
    }

    return pVirtualChannel;
}

///
/// @brief
///    获得指定名字的虚拟通道
///
/// @param[in]  strVCName    虚拟通道名
///
/// @return
///     CGOOSERecvVirtualChannel*    指向虚拟通道对象的指针，失败时返回NULL
///
CGOOSERecvVirtualChannel* CParamConfigGOOSERecv::GetVirtualChannelByName( std::wstring strVCName )
{
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter;


    // 查找指定名字的虚拟通道
    iter = m_mapGOOSERecvVirtualChannelGroup.find(strVCName);
    if (m_mapGOOSERecvVirtualChannelGroup.end() == iter)
    {
        return NULL;
    }

    return (iter->second);
}

///
/// @brief
///    设置通道的实时值
///
/// @param[in]  iAPPID      通道所属控制块的APPID
/// @param[in]  pGooseMsg   GOOSE报文的编解码对象，内含GOOSE通道值
/// @param[in]  stFrameInfo GOOSE报文头
///
void CParamConfigGOOSERecv::SetChannelRecvValue( int iAPPID, GooseMsg* pGooseMsg, FrameInfo& stFrameInfo )
{
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter;
    eFunctionType                       eCurFunctionType;
    CEventTime                          stDITestStartTime;
    CEventTime                          stDINewChangeTime;
    int                                 nIndex          = 0;            ///< 通道（子通道）在所属控制块（父通道）中的索引
    bool                                bChangeFlag     = false;        ///< 指示是否有通道（子通道）的接收值发生了改变
    CGOOSERecvVirtualChannel*           pVirtualChannel = NULL;         ///< 虚拟通道指针
    CChannel*                           pChannel        = NULL;         ///< 通道指针
    CChannel*                           pParentChannel  = NULL;         ///< 父通道指针
    CBaseCb*                            pBaseCb         = NULL;         ///< 控制块指针
    GOOSE_DATA*                         pstGooseData    = NULL;         ///< 通道数据指针


    // 检查输入参数
    if (NULL == pGooseMsg)
    {
        return ;
    }

    // 获得数据发送管理器
    CFrameSend* pFrameSend = CFrameSend::getInstance();
    if (NULL == pFrameSend)
    {
        return ;
    }

    // 获得帧接收时间
    stDINewChangeTime.ts_s  = stFrameInfo.timeStamp.ts_sec;
    stDINewChangeTime.ts_ns = stFrameInfo.timeStamp.ts_usec * 1000;

    // 如果当前没有进行手动试验和状态序列
    eCurFunctionType = pFrameSend->GetCurFunctionType();
    if (FUNCTION_TYPE_UNKNOWN == eCurFunctionType)
    {
        // 停止数据接收
        CDsmDataLayer* pDsmDataLayer = CDsmDataLayer::getInstance();
        if (pDsmDataLayer)
        {
            pDsmDataLayer->StopRecvData();
        }

        return ;
    }

    // 遍历所有虚拟通道
    for (iter = m_mapGOOSERecvVirtualChannelGroup.begin(); m_mapGOOSERecvVirtualChannelGroup.end() != iter; iter++)
    {
        // 获得虚拟通道对象的指针
        pVirtualChannel = iter->second;
        if (NULL == pVirtualChannel)
        {
            continue;
        }

        // 获得虚拟通道绑定的指针通道（子通道）对象的指针
        pChannel = pVirtualChannel->GetChannel();
        if (NULL== pChannel)
        {
            continue;
        }

        // 获得通道（子通道）所属的控制块
        pBaseCb = pChannel->GetParentBaseCb();
        if (NULL == pBaseCb)
        {
            continue;
        }

        // 判断控制块的APPID
        if (iAPPID != pBaseCb->GetAPPID())
        {
            continue;
        }

        // 检查真实通道是否有父通道
        pParentChannel = pChannel->GetParentChannel();
        if (pParentChannel)                                             ///< 如果有父通道
        {
            // 获得父通道在所属控制块中的索引
            nIndex = pParentChannel->GetIndex();
            if (-1 == nIndex)
            {
                continue;
            }

            // 获得保存父通道值的结构体
            GOOSE_STRUCT* pstGooseStruct = (GOOSE_STRUCT*)pGooseMsg->GetAChannel(nIndex);
            if (NULL == pstGooseStruct)
            {
                continue;
            }

            // 获得子通道在所属父通道中的索引
            nIndex = pChannel->GetIndex();
            if ((-1 == nIndex) || (nIndex >= (int)pstGooseStruct->member.size()))
            {
                continue;
            }

            // 获得保存子通道值的结构体
            pstGooseData = pstGooseStruct->member[nIndex];
        }
        else                                                            ///< 如果没有父通道
        {
            // 获得真实通道在所属控制块中的索引号
            nIndex = pChannel->GetIndex();
            if (-1 == nIndex)
            {
                continue;
            }

            // 获得保存通道值的结构体
            pstGooseData = pGooseMsg->GetAChannel(nIndex);
        }

        //
        // 改变真实通道的接收值
        //
        if (pstGooseData)
        {
            GOOSE_DATA_VARIANT stNewGooseData(pstGooseData);

            // 如果值有变化，那么改变通道的实时值
            if (pChannel->GetGooseRecvValue() != stNewGooseData)
            {
                CHistoryData*       pHistoryData        = NULL;
                CStateTestResult*   pStateTestResult    = NULL;


                // 根据当前事务类型，获得实验开始时间和测试结果集对象
                if (FUNCTION_TYPE_MANUAL_TEST == eCurFunctionType)
                {
                    // 获得最新的手动试验结果集对象
                    pStateTestResult    = pFrameSend->LastManualTestResult();
                    stDITestStartTime   = pFrameSend->GetManualTestStartTime();
                }
                else if (FUNCTION_TYPE_STATE_SEQUENCE == eCurFunctionType)
                {
                    // 获得当前状态号对应的测试结果集对象
                    pStateTestResult    = pFrameSend->GetStateSequenceResult(pFrameSend->GetCurStateIndex());
                    stDITestStartTime   = pFrameSend->GetStateSequenceStartTime();
                }

                // 保存接收值到结果集对象中
                if (pStateTestResult)
                {
                    // 获得结果集中保存指定虚拟通道名历史值的对象
                    CChannelRecvValues* pChannelRecvValues = pStateTestResult->GetChannelRecvValues(pVirtualChannel->GetName());
                    if (pChannelRecvValues)
                    {
                        pChannelRecvValues->SaveRecvValue(stDINewChangeTime - stDITestStartTime, stNewGooseData);
                    }
                }

                // 设置通道新的实时值
                pChannel->SetGooseRecvValue(stNewGooseData);
                bChangeFlag = true;
            }
        }
    }

    // 如果有通道的值发生了改变
    if (bChangeFlag)
    {
        CFrameSend* pFrameSend = CFrameSend::getInstance();
        if (pFrameSend)
        {
            // 向界面层发送通道值改变消息
            PostMessage(pFrameSend->GetNotifyHwnd(), WM_DI_CHANGED, 0, 0);
        }
    }
}

///
/// @brief
///    加载配置文件
///
/// @param[in]  strConfigFile 文件路径
///
/// @return
///     bool    true表示加载成功
///
bool CParamConfigGOOSERecv::LoadConfig( std::wstring strConfigFile )
{
    CMarkup                             xml;                            ///< 解析xml使用的CMarkUp对象


    if (strConfigFile.empty())
    {
        return false;
    }
    m_strConfigFile = strConfigFile;

    // 加载XML文件
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

    // 标记为未更改
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
bool CParamConfigGOOSERecv::SaveConfig()
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
