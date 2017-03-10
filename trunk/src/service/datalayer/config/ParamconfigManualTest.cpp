/// @file
///
/// @brief
///     手动试验参数配置
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.06.07
///
/// 修改历史：
///

#include "StdAfx.h"
#include "ParamconfigManualTest.h"
#include "ParamConfigSMVSend.h"
#include "src\service\datalayer\config\ParamConfigGOOSERecv.h"
#include "src\service\datalayer\config\ParamConfigBasic.h"

#define NODE_ROOT                       L"config"                       ///< 根节点
#define NODE_SMVChangeItem              L"SMVChangeItem"                ///< SMV变化项
#define NODE_ModifyEffectiveMode        L"ModifyEffectiveMode"          ///< 修改生效方式
#define NODE_StartMode                  L"StartMode"                    ///< 试验开始方式
#define NODE_VirtualChannelGroup        L"VirtualChannelGroup"          ///< 虚拟通道组
#define NODE_VirtualChannel             L"VirtualChannel"               ///< VirtualChannel

#define VIRTUAL_CHANNEL_COUNT           20                              ///< 虚拟通道总数

///
/// @brief
///     构造函数
///
CParamconfigManualTest::CParamconfigManualTest(void)
: m_eSMVChangeItem(CHANGE_ITEM_AMPLITUDE)
, m_eModifyEffectiveMode(EFFECTIVE_MODE_MANUALLY)
, m_eStartMode(START_MODE_APART)
, m_bIsShowDI(false)
{
}

///
/// @brief
///     析构函数
///
CParamconfigManualTest::~CParamconfigManualTest(void)
{
    __ClearSMVSendVirtualChannel();
}

///
/// @brief
///     释放所有SMV发送虚拟通道对象
///
void CParamconfigManualTest::__ClearSMVSendVirtualChannel()
{
    // 释放容器中的所有虚拟通道对象
    std::list<CSMVSendVirtualChannel*>::iterator iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        CSMVSendVirtualChannel* pVirtualChannel = *iter;
        if (NULL != pVirtualChannel)
        {
            delete pVirtualChannel;
        }

        iter++;
    }

    // 清空容器
    m_listSMVSendVirtualChannelGroup.clear();
}

///
/// @brief
///     获取配置实例接口
///
CParamconfigManualTest* CParamconfigManualTest::getInstance()
{
    static CParamconfigManualTest s_paramConfigManualTest;
    return &s_paramConfigManualTest;
}

///
/// @brief
///    加载配置文件
///
/// @param[in]  strConfigFile    文件路径
///
/// @return
///     bool    true表示加载成功
///
bool CParamconfigManualTest::LoadConfig( std::wstring strConfigFile)
{
    CMarkup                             xml;                            ///< 解析xml使用的CMarkUp对象


    // 检查输入参数
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

    // 释放所有SMV发送虚拟通道对象
    __ClearSMVSendVirtualChannel();

    // <config>
    if (!xml.FindElem(NODE_ROOT) || !xml.IntoElem())
    {
        return false;
    }

    // <SMVChangeItem>
    if (!xml.FindElem(NODE_SMVChangeItem))
    {
        return false;
    }
    m_eSMVChangeItem = ((eSMVChangeItem)_wtoi(xml.GetData().c_str()));

    // <ModifyEffectiveMode>
    if (!xml.FindElem(NODE_ModifyEffectiveMode))
    {
        return false;
    }
    m_eModifyEffectiveMode = ((eModifyEffectiveMode)_wtoi(xml.GetData().c_str()));

    // <StartMode>
    if (!xml.FindElem(NODE_StartMode))
    {
        return false;
    }
    m_eStartMode = ((eStartMode)_wtoi(xml.GetData().c_str()));

    //
    // find and inside <VirtualChannelGroup>
    //
    if (xml.FindElem(NODE_VirtualChannelGroup) && xml.IntoElem())
    {
        //
        // find <VirtualChannel>
        //
        while(xml.FindElem(NODE_VirtualChannel))
        {
            CSMVSendVirtualChannel*    pVirtualChannel = new CSMVSendVirtualChannel(FUNCTION_TYPE_MANUAL_TEST, this);
            if (NULL == pVirtualChannel)
            {
                return false;
            }

            // inside <VirtualChannel>
            if (!xml.IntoElem())
            {
                delete pVirtualChannel;
                return false;
            }

            // 加载虚拟通道
            if (!pVirtualChannel->__LoadConfig(xml))
            {
                delete pVirtualChannel;
                return false;
            }

            // outside <VirtualChannel>
            if (!xml.OutOfElem())
            {
                delete pVirtualChannel;
                return false;
            }

            // 添加虚拟通道对象指针到容器
            if (!__AddSMVSendVirtualChannel(pVirtualChannel))
            {
                delete pVirtualChannel;
                return false;
            }
        }

        // 检查虚拟通道总数
        if (m_listSMVSendVirtualChannelGroup.size() != VIRTUAL_CHANNEL_COUNT)
        {
            __ClearSMVSendVirtualChannel();
            return false;
        }

        // quit <VirtualChannelGroup>
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
bool CParamconfigManualTest::SaveConfig()
{
    CMarkup                             xml;                            ///< 解析xml使用的CMarkUp对象
    wchar_t                             cBuffer[128] = {0};


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

    // <SMVChangeItem>
    memset(cBuffer, 0, sizeof(cBuffer));
    _itow_s(m_eSMVChangeItem, cBuffer, sizeof(cBuffer)/ sizeof(cBuffer[0]), 10);
    if (!xml.AddElem(NODE_SMVChangeItem) || !xml.SetData(cBuffer))
    {
        return false;
    }

    // <ModifyEffectiveMode>
    memset(cBuffer, 0, sizeof(cBuffer));
    _itow_s(m_eModifyEffectiveMode, cBuffer, sizeof(cBuffer)/ sizeof(cBuffer[0]), 10);
    if (!xml.AddElem(NODE_ModifyEffectiveMode) || !xml.SetData(cBuffer))
    {
        return false;
    }

    // <StartMode>
    memset(cBuffer, 0, sizeof(cBuffer));
    _itow_s(m_eStartMode, cBuffer, sizeof(cBuffer)/ sizeof(cBuffer[0]), 10);
    if (!xml.AddElem(NODE_StartMode) || !xml.SetData(cBuffer))
    {
        return false;
    }

    // 检查虚拟通道总数
    if (m_listSMVSendVirtualChannelGroup.size() != VIRTUAL_CHANNEL_COUNT)
    {
        return false;
    }

    // <VirtualChannelGroup>
    if (!xml.AddElem(NODE_VirtualChannelGroup) || !xml.IntoElem())
    {
        return false;
    }

    std::list<CSMVSendVirtualChannel*>::iterator iter = m_listSMVSendVirtualChannelGroup.begin();
    while(m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        CSMVSendVirtualChannel* pVirtualChannel = *iter;
        if (NULL != pVirtualChannel)
        {
            // <VirtualChannel>
            if (!xml.AddElem(NODE_VirtualChannel) || !xml.IntoElem())
            {
                return false;
            }

            // 保存虚拟通道
            if (!pVirtualChannel->__SaveConfig(xml))
            {
                delete pVirtualChannel;
                return false;
            }

            // outside <VirtualChannel>
            if (!xml.OutOfElem())
            {
                return false;
            }
        }

        iter++;
    }

    // quit <VirtualChannelGroup>
    if (!xml.OutOfElem())
    {
        return false;
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

    // 标记为未更改
    SetModified(false);
    return true;
}

///
/// @brief
///    添加SMV发送虚拟通道到容器中
///
/// @param[in]  pVirtualChannel    SMV发送虚拟通道指针
///
/// @return
///     bool    true表示添加成功
///
bool CParamconfigManualTest::__AddSMVSendVirtualChannel(CSMVSendVirtualChannel* pVirtualChannel)
{
    // 检查输入参数
    if (NULL == pVirtualChannel)
    {
        return false;
    }

    // 检查虚拟通道是否已存在
    if (__GetSMVSendVirtualChannel(pVirtualChannel->GetName()) != NULL)
    {
        return false;
    }

    // 将虚拟通道对象指针保存到容器中
    m_listSMVSendVirtualChannelGroup.push_back(pVirtualChannel);
    return true;
}

///
/// @brief
///    枚举第一个被映射的SMV发送虚拟通道
///
/// @param[in]  bRebuildMap     是否需要重建SMV发送映射表、GOOSE接收映射表
///
/// @return
///     CVirtualChannel*    指向虚拟通道对象的指针，失败时返回NULL
///
CSMVSendVirtualChannel* CParamconfigManualTest::FirstSMVSendVirtualChannel( bool bRebuildMap /*= true*/ )
{
    CSMVSendVirtualChannel*                    pVirtualChannel = NULL;


    if (bRebuildMap)
    {
        // 生成所有虚拟通道的真实通道映射表
        RebuildSMVSendChannelMap();

        // 获得GOOSE接收参数配置管理器，并生成所有GOOSE接收虚拟通道的真实通道映射表
        CParamConfigGOOSERecv* pParamConfigGooseRecv = CParamConfigGOOSERecv::getInstance();
        if (pParamConfigGooseRecv)
        {
            pParamConfigGooseRecv->RebuildGOOSERecvChannelMap();
        }
    }

    // 查找第一个被映射了的虚拟通道
    std::list<CSMVSendVirtualChannel*>::iterator iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // 获得虚拟通道对象指针
        pVirtualChannel = *iter;
        if (pVirtualChannel)
        {
            // 如果有真实通道映射到此虚拟通道，那么返回虚拟通道的指针
            if (pVirtualChannel->GetChannelCount())
            {
                return pVirtualChannel;
            }
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    获得下一个被映射的SMV发送虚拟通道
///
/// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
///
/// @return
///     CVirtualChannel*    指向虚拟通道对象的指针，失败时返回NULL
///
CSMVSendVirtualChannel* CParamconfigManualTest::NextSMVSendVirtualChannel(CSMVSendVirtualChannel* pCurVirtualChannel)
{
    CSMVSendVirtualChannel*                    pVirtualChannel = NULL;


    // 输入参数检查
    if (NULL == pCurVirtualChannel)
    {
        return NULL;
    }

    // 获得当前虚拟通道位置
    std::list<CSMVSendVirtualChannel*>::iterator iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // 获得虚拟通道对象指针
        pVirtualChannel = *iter;
        if (pVirtualChannel == pCurVirtualChannel)
        {
            break;
        }

        iter++;
    }

    // 检查是否已到链表结尾
    if (m_listSMVSendVirtualChannelGroup.end() == iter)
    {
        return NULL;
    }
    else
    {
        // 指向下一个虚拟通道
        while(m_listSMVSendVirtualChannelGroup.end() != ++iter)
        {
            // 获得虚拟通道对象指针
            pVirtualChannel = *iter;
            if (pVirtualChannel)
            {
                // 如果有真实通道映射到此虚拟通道，那么返回虚拟通道的指针
                if (pVirtualChannel->GetChannelCount())
                {
                    return pVirtualChannel;
                }
            }
        }
    }

    return NULL;
}

///
/// @brief
///    生成所有SMV发送虚拟通道的真实通道映射表
///    注意：调用FirstSMVSendVirtualChannel()函数时会自动调用此函数，
///           如果需要重新生成映射表，可以手动调用此函数
///
/// @return
///     bool    成功返回true
///
bool CParamconfigManualTest::RebuildSMVSendChannelMap()
{
    CSMVSendVirtualChannel*                    pVirtualChannel = NULL;


    std::list<CSMVSendVirtualChannel*>::iterator iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // 获得虚拟通道对象指针
        pVirtualChannel = *iter;
        if (pVirtualChannel)
        {
            // 查找映射虚拟通道的真实通道，把真实通道的指针添加虚拟通道的容器中
            if (!__FillSMVSendChannelGroup(pVirtualChannel))
            {
                return false;
            }
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    查找映射到指定SMV发送虚拟通道的真实通道，把真实通道的指针添加虚拟通道的容器中
///
/// @param[in]  pVirtualChannel    虚拟通道指针
///
/// @return
///     bool    成功返回true
///
bool CParamconfigManualTest::__FillSMVSendChannelGroup(CSMVSendVirtualChannel* pVirtualChannel)
{
    if (NULL == pVirtualChannel)
    {
        return false;
    }

    // 获得虚拟通道的名字
    std::wstring strVirtualChannelName = pVirtualChannel->GetName();
    if (strVirtualChannelName.empty())
    {
        return false;
    }

    // 获得SMV发送参数配置管理器
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return false;
    }

    // 设置虚拟通道的交直流类型
    pVirtualChannel->m_eACDCType = pParamConfigSMVSend->GetCurrentType(strVirtualChannelName);

    //
    // 根据当前SMV类型，获取被选中控制块中映射到当前虚拟通道的真实通道
    //
    pVirtualChannel->m_vecChannelGroup.clear();
    switch(pParamConfigSMVSend->GetSMVType())
    {
    case FRAME_TYPE_NET_9_1:                                    ///< ///< IEC61850-9-1
        {
            // 遍历每一个IEC81850-9-1控制块
            CIEC9_1Cb* pIEC9_1Cb = pParamConfigSMVSend->FirstIEC9_1Cb();
            while(pIEC9_1Cb)
            {
                // 如果控制块被勾选
                if (pIEC9_1Cb->IsSelected())
                {
                    // 遍历每一个通道
                    CChannel* pChannel = pIEC9_1Cb->FirstChannel();
                    while(pChannel)
                    {
                        // 如果该真实通道映射到了当前虚拟通道
                        if (pChannel->GetVirtualChannel() == strVirtualChannelName)
                        {
                            // 添加真实通道指针到虚拟通道的真实通道容器中
                            pVirtualChannel->m_vecChannelGroup.push_back(pChannel);
                        }
                        pChannel = pIEC9_1Cb->NextChannel(pChannel);
                    }
                }
                pIEC9_1Cb = pParamConfigSMVSend->NextIEC9_1Cb(pIEC9_1Cb);
            }
        }
        break;

    case FRAME_TYPE_NET_9_2:                                    ///< IEC61850-9-2
        {
            // 遍历每一个IEC81850-9-2控制块
            CIEC9_2Cb* pIEC9_2Cb = pParamConfigSMVSend->FirstIEC9_2Cb();
            while(pIEC9_2Cb)
            {
                // 如果控制块被勾选
                if (pIEC9_2Cb->IsSelected())
                {
                    // 遍历每一个通道
                    CChannel* pChannel = pIEC9_2Cb->FirstChannel();
                    while(pChannel)
                    {
                        // 如果该真实通道映射到了当前虚拟通道
                        if (pChannel->GetVirtualChannel() == strVirtualChannelName)
                        {
                            // 添加真实通道指针到虚拟通道的真实通道容器中
                            pVirtualChannel->m_vecChannelGroup.push_back(pChannel);
                        }
                        pChannel = pIEC9_2Cb->NextChannel(pChannel);
                    }
                }
                pIEC9_2Cb = pParamConfigSMVSend->NextIEC9_2Cb(pIEC9_2Cb);
            }
        }
        break;

    case FRAME_TYPE_FT3_NANRUI:                                 ///< FT3
        {
            // 遍历每一个FT3控制块
            CFT3Cb* pFt3Cb = pParamConfigSMVSend->FirstFt3Cb();
            while(pFt3Cb)
            {
                // 如果控制块被勾选
                if (pFt3Cb->IsSelected())
                {
                    // 遍历每一个通道
                    CChannel* pChannel = pFt3Cb->FirstChannel();
                    while(pChannel)
                    {
                        // 如果该真实通道映射到了当前虚拟通道
                        if (pChannel->GetVirtualChannel() == strVirtualChannelName)
                        {
                            // 添加真实通道指针到虚拟通道的真实通道容器中
                            pVirtualChannel->m_vecChannelGroup.push_back(pChannel);
                        }
                        pChannel = pFt3Cb->NextChannel(pChannel);
                    }
                }
                pFt3Cb = pParamConfigSMVSend->NextFt3Cb(pFt3Cb);
            }
        }
        break;

    case FRAME_TYPE_FT3_GUOWANG:                                ///< FT3扩展
        {
            // 遍历每一个FT3扩展控制块
            CFT3ExpandCb* pFt3ExpandCb = pParamConfigSMVSend->FirstFt3ExpandCb();
            while(pFt3ExpandCb)
            {
                // 如果控制块被勾选
                if (pFt3ExpandCb->IsSelected())
                {
                    // 遍历每一个通道
                    CChannel* pChannel = pFt3ExpandCb->FirstChannel();
                    while(pChannel)
                    {
                        // 如果该真实通道映射到了当前虚拟通道
                        if (pChannel->GetVirtualChannel() == strVirtualChannelName)
                        {
                            // 添加真实通道指针到虚拟通道的真实通道容器中
                            pVirtualChannel->m_vecChannelGroup.push_back(pChannel);
                        }
                        pChannel = pFt3ExpandCb->NextChannel(pChannel);
                    }
                }
                pFt3ExpandCb = pParamConfigSMVSend->NextFt3ExpandCb(pFt3ExpandCb);
            }
        }
        break;

    default:
        return false;
    }

    // 如果被勾选的通道的索引超限，那么重置为0
    if (pVirtualChannel->GetSelectedIndex() >= (int)pVirtualChannel->m_vecChannelGroup.size())
    {
        pVirtualChannel->SetSelectedIndex(0);
    }

    return true;
}

///
/// @brief
///    获得SMV变化项
///
/// @return
///     eSMVChangeItem    SMV变化项
///
eSMVChangeItem CParamconfigManualTest::GetSMVChangeItem()
{
    return m_eSMVChangeItem;
}

///
/// @brief
///    设置SMV变化项
///
/// @param[in]  newSMVChangeItem    新的SMV变化项
///
/// @return
///     bool    成功返回true
///
bool CParamconfigManualTest::SetSMVChangeItem( eSMVChangeItem newSMVChangeItem )
{
    if (m_eSMVChangeItem != newSMVChangeItem)
    {
        m_eSMVChangeItem = newSMVChangeItem;

        // 设置修改标识
        SetModified(true);
    }

    return true;
}

///
/// @brief
///    获得修改生效方式
///
/// @return
///     eModifyEffectiveMode    修改生效方式
///
eModifyEffectiveMode CParamconfigManualTest::GetModifyEffectiveMode()
{
    return m_eModifyEffectiveMode;
}

///
/// @brief
///    设置修改生效方式
///
/// @param[in]  newModifyEffectiveMode    新的修改生效方式
///
/// @return
///     bool    成功返回true
///
bool CParamconfigManualTest::SetModifyEffectiveMode( eModifyEffectiveMode newModifyEffectiveMode )
{
    if (m_eModifyEffectiveMode != newModifyEffectiveMode)
    {
        m_eModifyEffectiveMode = newModifyEffectiveMode;

        // 设置修改标识
        SetModified(true);
    }

    return true;
}

///
/// @brief
///    获得试验开始方式
///
/// @return
///     eStartMode    试验开始方式
///
eStartMode CParamconfigManualTest::GetStartMode()
{
    return m_eStartMode;
}

///
/// @brief
///    设置试验开始方式
///
/// @param[in]  newStartMode    新的试验开始方式
///
/// @return
///     bool    成功返回true
///
bool CParamconfigManualTest::SetStartMode( eStartMode newStartMode )
{
    if (m_eStartMode != newStartMode)
    {
        m_eStartMode = newStartMode;

        // 设置修改标识
        SetModified(true);
    }

    return true;
}

///
/// @brief
///    获得指定名字的虚拟通道
///
/// @param[in]  strChannelName    虚拟通道名
///
/// @return
///     CSMVSendVirtualChannel*    指向虚拟通道对象的指针
///
CSMVSendVirtualChannel* CParamconfigManualTest::__GetSMVSendVirtualChannel( std::wstring strChannelName )
{
    CSMVSendVirtualChannel*                    pVirtualChannel = NULL;


    // 查找第一个被映射了的虚拟通道
    std::list<CSMVSendVirtualChannel*>::iterator iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // 获得虚拟通道对象指针
        pVirtualChannel = *iter;
        if (pVirtualChannel)
        {
            // 如果有真实通道映射到此虚拟通道，那么返回虚拟通道的指针
            if (pVirtualChannel->GetName() == strChannelName)
            {
                return pVirtualChannel;
            }
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    等幅值
///
/// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
///
/// @return
///     bool    成功返回true，失败返回false
///
bool CParamconfigManualTest::EqualAmplitude( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    std::wstring                strName;
    double                      dCurAmplitude;


    // 检查输入参数
    if (NULL == pCurVirtualChannel)
    {
        return false;
    }

    // 获得当前虚拟通道的名字和幅值
    strName = pCurVirtualChannel->GetName();
    dCurAmplitude = pCurVirtualChannel->m_dAmplitude;

    // 设置电流、电压虚拟通道的幅值
    if ((3 == strName.size()) && (L'I' == strName[0] || L'U' == strName[0]))
    {
        strName[1] = L'a';
        __SetSMVSendVCPrimaryAmplitude(strName, dCurAmplitude);

        strName[1] = L'b';
        __SetSMVSendVCPrimaryAmplitude(strName, dCurAmplitude);

        strName[1] = L'c';
        __SetSMVSendVCPrimaryAmplitude(strName, dCurAmplitude);

        strName[1] = L'x';
        __SetSMVSendVCPrimaryAmplitude(strName, dCurAmplitude);
    }
    // 设置时间虚拟通道的幅值
    else if ((2 == strName.size()) && (L't' == strName[0]))
    {
        strName[1] = L'1';
        __SetSMVSendVCPrimaryAmplitude(strName, dCurAmplitude);

        strName[1] = L'2';
        __SetSMVSendVCPrimaryAmplitude(strName, dCurAmplitude);

        strName[1] = L'3';
        __SetSMVSendVCPrimaryAmplitude(strName, dCurAmplitude);

        strName[1] = L'4';
        __SetSMVSendVCPrimaryAmplitude(strName, dCurAmplitude);
    }

    return true;
}

///
/// @brief
///    设置指定名字的虚拟通道的幅值（一次值）
///
/// @param[in]  strChannelName    虚拟通道名
/// @param[in]  dNewAmplitude    新的幅值（一次值）
///
/// @return
///     bool    成功返回true
///
bool CParamconfigManualTest::__SetSMVSendVCPrimaryAmplitude( std::wstring strChannelName, double dNewAmplitude )
{
    CSMVSendVirtualChannel*     pVirtualChannel;


    // 查找指定名字的虚拟通道
    pVirtualChannel = __GetSMVSendVirtualChannel(strChannelName);
    if (pVirtualChannel)
    {
        // 修改虚拟通道的幅值（一次值）
        pVirtualChannel->m_dAmplitude = dNewAmplitude;
        return true;
    }

    return false;
}

///
/// @brief
///    额定值
///
/// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
///
/// @return
///     bool    成功返回true，失败返回false
///
bool CParamconfigManualTest::RatedAmplitude( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    // 检查输入参数
    if (NULL == pCurVirtualChannel)
    {
        return false;
    }

    // 设置虚拟通道的额定值
    return pCurVirtualChannel->SetRatedAmplitude();
}

///
/// @brief
///    三相平衡
///
/// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
///
/// @return
///     bool    成功返回true，失败返回false
///
bool CParamconfigManualTest::ThreePhaseBalance( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    std::wstring                strName;
    double                      dCurAmplitude;


    // 检查输入参数
    if (NULL == pCurVirtualChannel)
    {
        return false;
    }

    // 获得当前虚拟通道的名字和幅值
    strName = pCurVirtualChannel->GetName();
    dCurAmplitude = pCurVirtualChannel->m_dAmplitude;

    // 设置电流、电压虚拟通道的幅值
    if ((3 == strName.size()) && (L'I' == strName[0] || L'U' == strName[0]))
    {
        strName[1] = L'a';
        __SetSMVSendVCPrimaryAmplitude(strName, dCurAmplitude);
        __SetSMVSendVCPhaseAngle(strName, 0);

        strName[1] = L'b';
        __SetSMVSendVCPrimaryAmplitude(strName, dCurAmplitude);
        __SetSMVSendVCPhaseAngle(strName, -120);

        strName[1] = L'c';
        __SetSMVSendVCPrimaryAmplitude(strName, dCurAmplitude);
        __SetSMVSendVCPhaseAngle(strName, 120);
    }

    return true;
}

///
/// @brief
///    等相角
///
/// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
///
/// @return
///     bool    成功返回true，失败返回false
///
bool CParamconfigManualTest::EqualPhaseAngle( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    std::wstring                strName;
    double                      dCurPhaseAngle;


    // 检查输入参数
    if (NULL == pCurVirtualChannel)
    {
        return false;
    }

    // 获得当前虚拟通道的名字和幅值
    strName = pCurVirtualChannel->GetName();
    dCurPhaseAngle = pCurVirtualChannel->GetPhaseAngle();

    // 设置电流、电压虚拟通道的幅值
    if ((3 == strName.size()) && (L'I' == strName[0] || L'U' == strName[0]))
    {
        strName[1] = L'a';
        __SetSMVSendVCPhaseAngle(strName, dCurPhaseAngle);

        strName[1] = L'b';
        __SetSMVSendVCPhaseAngle(strName, dCurPhaseAngle);

        strName[1] = L'c';
        __SetSMVSendVCPhaseAngle(strName, dCurPhaseAngle);

        strName[1] = L'x';
        __SetSMVSendVCPhaseAngle(strName, dCurPhaseAngle);
    }

    return true;
}

///
/// @brief
///    设置指定名字的虚拟通道的相角
///
/// @param[in]  strChannelName    虚拟通道名
/// @param[in]  dNewPhaseAngle    新的相角
///
/// @return
///     bool    成功返回true
///
bool CParamconfigManualTest::__SetSMVSendVCPhaseAngle( std::wstring strChannelName, double dNewPhaseAngle )
{
    CSMVSendVirtualChannel*     pVirtualChannel;


    // 查找指定名字的虚拟通道
    pVirtualChannel = __GetSMVSendVirtualChannel(strChannelName);
    if (pVirtualChannel)
    {
        // 修改虚拟通道的相角
        return pVirtualChannel->SetPhaseAngle(dNewPhaseAngle);
    }

    return false;
}

///
/// @brief
///    相角正序
///
/// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
///
/// @return
///     bool    成功返回true，失败返回false
///
bool CParamconfigManualTest::PhaseAnglePositiveSequence( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    std::wstring                strName;
    double                      dCurPhaseAngle;


    // 检查输入参数
    if (NULL == pCurVirtualChannel)
    {
        return false;
    }

    // 获得当前虚拟通道的名字和幅值
    strName = pCurVirtualChannel->GetName();
    if ((3 == strName.size()) && (L'I' == strName[0] || L'U' == strName[0]))
    {
        strName[1] = L'a';
        dCurPhaseAngle = __GetSMVSendVCPhaseAngle(strName);

        strName[1] = L'b';
        __SetSMVSendVCPhaseAngle(strName, dCurPhaseAngle - 120);

        strName[1] = L'c';
        __SetSMVSendVCPhaseAngle(strName, dCurPhaseAngle + 120);
    }

    return true;
}

///
/// @brief
///    相角反序
///
/// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
///
/// @return
///     bool    成功返回true，失败返回false
///
bool CParamconfigManualTest::PhaseAngleReverseSequence( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    std::wstring                strName;
    double                      dCurPhaseAngle;


    // 检查输入参数
    if (NULL == pCurVirtualChannel)
    {
        return false;
    }

    // 获得当前虚拟通道的名字和幅值
    strName = pCurVirtualChannel->GetName();
    if ((3 == strName.size()) && (L'I' == strName[0] || L'U' == strName[0]))
    {
        strName[1] = L'a';
        dCurPhaseAngle = __GetSMVSendVCPhaseAngle(strName);

        strName[1] = L'b';
        __SetSMVSendVCPhaseAngle(strName, dCurPhaseAngle + 120);

        strName[1] = L'c';
        __SetSMVSendVCPhaseAngle(strName, dCurPhaseAngle - 120);
    }

    return true;
}

///
/// @brief
///    获得指定名字的虚拟通道的相角
///
/// @param[in]  strChannelName    虚拟通道名
///
/// @return
///     double    相角值
///
double CParamconfigManualTest::__GetSMVSendVCPhaseAngle( std::wstring strChannelName )
{
    CSMVSendVirtualChannel*     pVirtualChannel;


    // 查找指定名字的虚拟通道
    pVirtualChannel = __GetSMVSendVirtualChannel(strChannelName);
    if (pVirtualChannel)
    {
        // 获得虚拟通道的相角
        return pVirtualChannel->GetPhaseAngle();
    }

    return (0.0f);
}

///
/// @brief
///    等频率
///
/// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
///
/// @return
///     bool    成功返回true，失败返回false
///
bool CParamconfigManualTest::EqualFrequency( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    double                      dCurFrequency;
    CSMVSendVirtualChannel*     pVirtualChannel;
    std::list<CSMVSendVirtualChannel*>::iterator iter;


    // 检查输入参数
    if (NULL == pCurVirtualChannel)
    {
        return false;
    }

    // 设置所有虚拟通道的频率
    dCurFrequency = pCurVirtualChannel->GetFrequency();
    iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // 获得虚拟通道对象指针
        pVirtualChannel = *iter;
        if (pVirtualChannel)
        {
            pVirtualChannel->SetFrequency(dCurFrequency);
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    设置所有虚拟通道的频率为指定值
///
/// @param[in]  dNewFrequency    虚拟通道频率
///
/// @return
///     bool    成功返回true
///
bool CParamconfigManualTest::SetAllVCFrequency( double dNewFrequency )
{
    CSMVSendVirtualChannel*     pVirtualChannel;
    std::list<CSMVSendVirtualChannel*>::iterator iter;


    // 检查输入参数
    if (dNewFrequency < 0)
    {
        return false;
    }

    // 设置所有虚拟通道的频率
    iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // 获得虚拟通道对象指针
        pVirtualChannel = *iter;
        if (pVirtualChannel)
        {
            pVirtualChannel->SetFrequency(dNewFrequency);
        }

        iter++;
    }

    return true;
}
