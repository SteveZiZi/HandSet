/// @file
///
/// @brief
///     状态序列的状态类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.6.14
///
/// 修改历史：
///

#include "StdAfx.h"
#include "Status.h"
#include "ParamConfigSMVSend.h"
#include "src\service\datalayer\config\ParamConfigBasic.h"

#define NODE_StateSwitch                L"StateSwitch"                  ///< 状态切换
#define NODE_StateSwitch_Type           L"Type"                         ///< 状态切换类型
#define NODE_StateSwitch_Delay          L"Delay"                        ///< 状态切换延时
#define NODE_DIGroup                    L"DIGroup"                      ///< 开入变量组
#define NODE_DIGroup_Logic              L"logic"                        ///< 开入量切换逻辑运算方式
#define NODE_DI                         L"DI"                           ///< DI
#define NODE_DI_NAME                    L"name"                         ///< name
#define NODE_DOGroup                    L"DOGroup"                      ///< DOGroup
#define NODE_DO                         L"DO"                           ///< DO
#define NODE_DO_NAME                    L"name"                         ///< name
#define NODE_VirtualChannelGroup        L"VirtualChannelGroup"          ///< VirtualChannelGroup
#define NODE_VirtualChannel             L"VirtualChannel"               ///< VirtualChannel

#define VIRTUAL_CHANNEL_COUNT           20                              ///< 虚拟通道总数

///
/// @brief
///    构造函数
///
CStatus::CStatus(void)
:m_nID(0)
,m_pParamConfigBase(NULL)
,m_eSwitchType(SWITCH_TYPE_TIME)
,m_iDelay(1000)
,m_eLogicalOperator(LOGICAL_OR)
{
    m_mapDIGroup.clear();
    m_mapDOGroup.clear();
    m_listSMVSendVirtualChannelGroup.clear();
}

///
/// @brief
///    构造函数
///
/// @param[in]  nID    状态对象唯一标识ID
/// @param[in]  pParamConfigBase    指向参数配置管理器的指针
///
CStatus::CStatus( int nID, CParamConfigBase* pParamConfigBase )
: m_nID(nID)
, m_pParamConfigBase(pParamConfigBase)
, m_eSwitchType(SWITCH_TYPE_TIME)
, m_iDelay(1000)
, m_eLogicalOperator(LOGICAL_OR)
, m_pStateTestResult(NULL)
{
    int     i;


    // 向开入量容器填充默认值
    m_mapDIGroup.clear();
    for (i = 0; i < sizeof(PREDEFINED_DI_INFO)/sizeof(PREDEFINED_DI_INFO[0]); i++)
    {
        m_mapDIGroup.insert(std::pair<std::wstring, bool>(PREDEFINED_DI_INFO[i].m_strName, PREDEFINED_DI_INFO[i].m_bChecked));
    }

    // 向开出量容器填充默认值
    m_mapDOGroup.clear();
    for (i = 0; i < sizeof(PREDEFINED_DO_INFO)/sizeof(PREDEFINED_DO_INFO[0]); i++)
    {
        m_mapDOGroup.insert(std::pair<std::wstring, bool>(PREDEFINED_DO_INFO[i].m_strName, PREDEFINED_DO_INFO[i].m_bChecked));
    }

    m_listSMVSendVirtualChannelGroup.clear();
}

///
/// @brief
///    析构函数
///
CStatus::~CStatus(void)
{
    __ClearSMVSendVirtualChannel();
}

///
/// @brief
///    深度拷贝一个状态对象
///
/// @param[in]
///     pOldStatus    -   已存在的状态对象
///
/// @return
///     bool    拷贝成功返回TRUE
///
bool CStatus::__Copy( CStatus* pOldStatus )
{
    // 检查输入参数
    if (NULL == pOldStatus)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CStatus::__Copy(): Parameter error \n"));
        return false;
    }

    // 复制基本属性
    m_eSwitchType       = pOldStatus->m_eSwitchType;
    m_iDelay            = pOldStatus->m_iDelay;
    m_eLogicalOperator  = pOldStatus->m_eLogicalOperator;

    // DI
    m_mapDIGroup.clear();
    m_mapDIGroup.insert(pOldStatus->m_mapDIGroup.begin(), pOldStatus->m_mapDIGroup.end());

    // DO
    m_mapDOGroup.clear();
    m_mapDOGroup.insert(pOldStatus->m_mapDOGroup.begin(), pOldStatus->m_mapDOGroup.end());

    // SMV发送虚拟通道
    __ClearSMVSendVirtualChannel();
    std::list<CSMVSendVirtualChannel*>::iterator iter = pOldStatus->m_listSMVSendVirtualChannelGroup.begin();
    while (pOldStatus->m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        CSMVSendVirtualChannel* pOldVirtualChannel = *iter;
        if (NULL != pOldVirtualChannel)
        {
            // 生成一个新的虚拟通道
            CSMVSendVirtualChannel* pNewVirtualChannel = new CSMVSendVirtualChannel(FUNCTION_TYPE_STATE_SEQUENCE, m_pParamConfigBase);
            if (NULL == pNewVirtualChannel)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CStatus::__Copy(): new CSMVSendVirtualChannel object failed \n"));
                return false;
            }

            // 复制老的虚拟通道
            if (!pNewVirtualChannel->__Copy(pOldVirtualChannel))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CStatus::__Copy(): pNewVirtualChannel->__Copy(pOldVirtualChannel) failed \n"));
                delete pNewVirtualChannel;
                return false;
            }

            // 添加新通道到容器中
            if (!__AddSMVSendVirtualChannel(pNewVirtualChannel))
            {
                delete pNewVirtualChannel;
                return false;
            }
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    获得状态对象唯一标识ID
///
/// @return
///     int    标识ID
///
int CStatus::__GetID()
{
    return m_nID;
}

///
/// @brief
///    加载<Status>
///
/// @param[in]  xml    xml文件对象引用
///
/// @return
///     bool    true表示加载成功
///
bool CStatus::__LoadConfig( CMarkup& xml )
{
    std::wstring        strName;
    bool                bValue;


    // find and inside <StateSwitch>
    if (!xml.FindElem(NODE_StateSwitch) || !xml.IntoElem())
    {
        return false;
    }

    // <Type>
    if (!xml.FindElem(NODE_StateSwitch_Type))
    {
        return false;
    }
    m_eSwitchType  = (eSwitchType)_wtoi(xml.GetData().c_str());

    // <Delay>
    if (!xml.FindElem(NODE_StateSwitch_Delay))
    {
        return false;
    }
    m_iDelay       = _wtoi(xml.GetData().c_str());

    // <DIGroup>
    if (!xml.FindElem(NODE_DIGroup))
    {
        return false;
    }
    m_eLogicalOperator = (eLogicalOperator)_wtoi(xml.GetAttrib(NODE_DIGroup_Logic).c_str());

    // inside <DIGroup>
    if (!xml.IntoElem())
    {
        return false;
    }

    // <DI>
    while(xml.FindElem(NODE_DI))
    {
        // DI名
        strName = xml.GetAttrib(NODE_DI_NAME);
        if (3 != strName.size())
        {
            return false;
        }

        // DI值
        bValue = _wtoi(xml.GetData().c_str())? true: false;

        // 保存(strName, bValue)到开入量容器中
        std::map<std::wstring, bool>::iterator iter = m_mapDIGroup.find(strName);
        if (m_mapDIGroup.end() == iter)
        {
            return false;
        }
        iter->second = bValue;
    }

    // quit <DIGroup>
    if (!xml.OutOfElem())
    {
        return false;
    }

    // quit <StateSwitch>
    if (!xml.OutOfElem())
    {
        return false;
    }

    // find and inside <DOGroup>
    if (!xml.FindElem(NODE_DOGroup) || !xml.IntoElem())
    {
        return false;
    }

    // <DO>
    while(xml.FindElem(NODE_DO))
    {
        // DO名
        strName = xml.GetAttrib(NODE_DO_NAME);
        if (3 != strName.size())
        {
            return false;
        }

        // DO值
        bValue = _wtoi(xml.GetData().c_str())? true: false;

        // 保存(strName, bValue)到开出量容器中
        std::map<std::wstring, bool>::iterator iter = m_mapDOGroup.find(strName);
        if (m_mapDOGroup.end() == iter)
        {
            return false;
        }
        iter->second = bValue;
    }

    // quit <DOGroup>
    if (!xml.OutOfElem())
    {
        return false;
    }

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
            CSMVSendVirtualChannel*    pVirtualChannel = new CSMVSendVirtualChannel(FUNCTION_TYPE_STATE_SEQUENCE, m_pParamConfigBase);
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

    return true;
}

///
/// @brief
///    保存<Status>
///
/// @param[in]  xml    xml文件对象引用
///
/// @return
///     bool    true表示保存成功
///
bool CStatus::__SaveConfig( CMarkup& xml )
{
    std::wstring        strName;
    bool                bValue;


    // add and inside <StateSwitch>
    if (!xml.AddElem(NODE_StateSwitch) || !xml.IntoElem())
    {
        return false;
    }

    // <Type>
    if (!xml.AddElem(NODE_StateSwitch_Type) || !xml.SetData((int)m_eSwitchType))
    {
        return false;
    }

    // <Delay>
    if (!xml.AddElem(NODE_StateSwitch_Delay) || !xml.SetData(m_iDelay))
    {
        return false;
    }

    if (m_mapDIGroup.size())
    {
        // <DIGroup>
        if (!xml.AddElem(NODE_DIGroup) || !xml.SetAttrib(NODE_DIGroup_Logic, (int)m_eLogicalOperator))
        {
            return false;
        }

        // inside <DIGroup>
        if (!xml.IntoElem())
        {
            return false;
        }

        // <DI>
        std::map<std::wstring, bool>::iterator iter = m_mapDIGroup.begin();
        while(m_mapDIGroup.end() != iter)
        {
            strName = iter->first;
            if (3 != strName.size())
            {
                return false;
            }

            // DI名
            if (!xml.AddElem(NODE_DI) || !xml.SetAttrib(NODE_DI_NAME, strName))
            {
                return false;
            }

            // DI值
            bValue  = iter->second;
            if (!xml.SetData((bValue? 1: 0)))
            {
                return false;
            }

            iter++;
        }

        // quit <DIGroup>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    // quit <StateSwitch>
    if (!xml.OutOfElem())
    {
        return false;
    }

    if (m_mapDOGroup.size())
    {
        // add and inside <DOGroup>
        if (!xml.AddElem(NODE_DOGroup) || !xml.IntoElem())
        {
            return false;
        }

        // <DO>
        std::map<std::wstring, bool>::iterator iter = m_mapDOGroup.begin();
        while(m_mapDOGroup.end() != iter)
        {
            strName = iter->first;
            if (3 != strName.size())
            {
                return false;
            }

            // DO名
            if (!xml.AddElem(NODE_DO) || !xml.SetAttrib(NODE_DO_NAME, strName))
            {
                return false;
            }

            // DO值
            bValue  = iter->second;
            if (!xml.SetData((bValue? 1: 0)))
            {
                return false;
            }

            iter++;
        }

        // quit <DOGroup>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    if (m_listSMVSendVirtualChannelGroup.size())
    {
        // add and inside <VirtualChannelGroup>
        if (!xml.AddElem(NODE_VirtualChannelGroup) || !xml.IntoElem())
        {
            return false;
        }

        // <VirtualChannel>
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
    }

    return true;
}

///
/// @brief
///     释放所有SMV发送虚拟通道对象
///
void CStatus::__ClearSMVSendVirtualChannel()
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
///    添加SMV发送虚拟通道到容器中
///
/// @param[in]  pVirtualChannel    SMV发送虚拟通道指针
///
/// @return
///     bool    true表示添加成功
///
bool CStatus::__AddSMVSendVirtualChannel(CSMVSendVirtualChannel* pVirtualChannel)
{
    // 检查输入参数
    if (NULL == pVirtualChannel)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CStatus::__AddSMVSendVirtualChannel(): Parameter error \n"));
        return false;
    }

    // 检查虚拟通道是否已存在
    if (__GetSMVSendVirtualChannel(pVirtualChannel->GetName()) != NULL)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CStatus::__AddSMVSendVirtualChannel(): Virtual channel already exist \n"));
        return false;
    }

    // 将虚拟通道对象指针保存到容器中
    m_listSMVSendVirtualChannelGroup.push_back(pVirtualChannel);
    return true;
}

///
/// @brief
///    获得状态切换方式
///
/// @return
///     eSwitchType    状态切换方式
///
eSwitchType CStatus::GetSwitchType()
{
    return m_eSwitchType;
}

///
/// @brief
///    设置状态切换方式
///
/// @param[in]  eNewType    新的状态切换方式
///
void CStatus::SetSwitchType( eSwitchType eNewType )
{
    if (m_eSwitchType != eNewType)
    {
        m_eSwitchType = eNewType;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得状态切换延时
///
/// @return
///     eSwitchType    状态切换延时
///
int CStatus::GetDelay()
{
    return m_iDelay;
}

///
/// @brief
///    设置状态切换延时
///
/// @param[in]  iDelay    新的状态切换延时
///
void CStatus::SetDelay( int iDelay )
{
    // 限制切换延时
    if (iDelay < 1 || iDelay > 60000)
    {
        return;
    }

    if (m_iDelay != iDelay)
    {
        m_iDelay = iDelay;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得开入量切换逻辑运算方式
///
/// @return
///     eLogicalOperator    逻辑运算方式
///
eLogicalOperator CStatus::GetLogicalOperator()
{
    return m_eLogicalOperator;
}

///
/// @brief
///    设置开入量切换逻辑运算方式
///
/// @param[in]  eNewLogical    逻辑运算方式
///
void CStatus::SetLogicalOperator( eLogicalOperator eNewLogical )
{
    if (m_eLogicalOperator != eNewLogical)
    {
        m_eLogicalOperator = eNewLogical;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    枚举第一个被映射的SMV发送虚拟通道
///
/// @return
///     CVirtualChannel*    指向虚拟通道对象的指针，失败时返回NULL
///
CSMVSendVirtualChannel* CStatus::FirstSMVSendVirtualChannel()
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
CSMVSendVirtualChannel* CStatus::NextSMVSendVirtualChannel( CSMVSendVirtualChannel* pCurVirtualChannel )
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
///    查找指定名字的SMV发送虚拟通道
///
/// @param[in]  strChannelName    虚拟通道名
///
/// @return
///     CSMVSendVirtualChannel*    指向虚拟通道对象的指针
///
CSMVSendVirtualChannel* CStatus::__GetSMVSendVirtualChannel( std::wstring strChannelName )
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
///    获得指定DI通道的勾选状态
///
/// @param[in]  strDIName    DI通道名
///
/// @return
///     bool    DI通道的勾选状态
///
bool CStatus::IsDIChecked( std::wstring strDIName )
{
    std::map<std::wstring, bool>::iterator iter;


    iter = m_mapDIGroup.find(strDIName);
    if (m_mapDIGroup.end() != iter)
    {
        return (iter->second);
    }

    return false;
}

///
/// @brief
///    设置指定DI通道的勾选状态
///
/// @param[in]  strDIName    DI通道名
/// @param[in]  bChecked    是否被勾选
///
void CStatus::SetDIChecked( std::wstring strDIName, bool bChecked )
{
    std::map<std::wstring, bool>::iterator iter;


    iter = m_mapDIGroup.find(strDIName);
    if (m_mapDIGroup.end() != iter)
    {
        if (iter->second != bChecked)
        {
            iter->second = bChecked;

            // 标记为已修改
            if (NULL != m_pParamConfigBase)
            {
                m_pParamConfigBase->SetModified(true);
            }
        }
    }
}

///
/// @brief
///    获得指定DO通道的值
///
/// @param[in]  strDOName    DO通道名
///
/// @return
///     bool    DO通道值
///
bool CStatus::GetDOValue( std::wstring strDOName )
{
    std::map<std::wstring, bool>::iterator iter;


    iter = m_mapDOGroup.find(strDOName);
    if (m_mapDOGroup.end() != iter)
    {
        return (iter->second);
    }

    return false;
}

///
/// @brief
///    设置指定DO通道的值
///
/// @param[in]  strDOName    DO通道名
/// @param[in]  bNewValue    通道布尔值
///
void CStatus::SetDOValue( std::wstring strDOName, bool bNewValue )
{
    std::map<std::wstring, bool>::iterator iter;


    iter = m_mapDOGroup.find(strDOName);
    if (m_mapDOGroup.end() != iter)
    {
        if (iter->second != bNewValue)
        {
            iter->second = bNewValue;

            // 标记为已修改
            if (NULL != m_pParamConfigBase)
            {
                m_pParamConfigBase->SetModified(true);
            }
        }
    }
}

///
/// @brief
///    给状态对象添加默认的虚拟通道组
///
/// @return
///     bool    成功返回true
///
bool CStatus::__AddDefaultSMVSendVirtualChannels()
{
    // 释放所有SMV发送虚拟通道对象
    __ClearSMVSendVirtualChannel();

    // 获得基本参数配置管理器
    CParamConfigBasic* pParamConfigBasic = CParamConfigBasic::getInstance();
    if (NULL == pParamConfigBasic)
    {
        return false;
    }

    //
    // 根据预定义的SMV发送虚拟通道值，生成虚拟通道对象，添加到容器中
    //
    for (int i = 0; i < sizeof(PREDEFINED_SMV_SEND_VC_INFO)/sizeof(PREDEFINED_SMV_SEND_VC_INFO[0]); i++)
    {
        CSMVSendVirtualChannel*    pVirtualChannel = new CSMVSendVirtualChannel(FUNCTION_TYPE_STATE_SEQUENCE, m_pParamConfigBase);
        if (NULL == pVirtualChannel)
        {
            return false;
        }

        // 设置默认值
        pVirtualChannel->m_strName      = PREDEFINED_SMV_SEND_VC_INFO[i].m_strName;
        pVirtualChannel->m_dPhaseAngle  = PREDEFINED_SMV_SEND_VC_INFO[i].m_dPhaseAngle;
        pVirtualChannel->m_dFrequency   = PREDEFINED_SMV_SEND_VC_INFO[i].m_dFrequency;

        // 根据虚拟通道类型，设置幅值
        if (L'U' == pVirtualChannel->m_strName[0])                      ///< 电压型
        {
            pVirtualChannel->m_dAmplitude   = pParamConfigBasic->GetRatedPrimaryVoltage() * 1000 / sqrt(3.0f);
        }
        else if (L'I' == pVirtualChannel->m_strName[0])                 ///< 电流型
        {
            pVirtualChannel->m_dAmplitude   = pParamConfigBasic->GetRatedPrimaryCurrent();
        }
        else                                                            ///< 时间型
        {
            swscanf_s(pParamConfigBasic->GetMuRatedDelay().c_str(), L"%lf", &(pVirtualChannel->m_dAmplitude), sizeof(pVirtualChannel->m_dAmplitude));
        }

        // 添加到容器中
        if (!__AddSMVSendVirtualChannel(pVirtualChannel))
        {
            delete pVirtualChannel;
            return false;
        }
    }

    return true;
}

///
/// @brief
///    获得SMV发送虚拟通道的数量
///
/// @return
///     int    虚拟通道的数量
///
int CStatus::GetSMVSendVirtualChannelCount()
{
    return (int)m_listSMVSendVirtualChannelGroup.size();
}

///
/// @brief
///    与指定的状态对象比较SMV虚拟发送通道是否相同
///
/// @param[in]  eCompareType    比较方式
/// @param[in]  pOldStatus      指向状态对象的指针
///
/// @return
///     bool    相同返回true
///
bool CStatus::__CompareSMVSendVirtualChannels( enumCompareType eCompareType, CStatus* pOldStatus )
{
    bool                                                        bRet = true;
    std::list<CSMVSendVirtualChannel*>::iterator                oldIter;
    std::list<CSMVSendVirtualChannel*>::iterator                newIter;


    // 依次比较每一个虚拟通道的幅值、相角、频率
    oldIter = pOldStatus->m_listSMVSendVirtualChannelGroup.begin();
    newIter = m_listSMVSendVirtualChannelGroup.begin();
    while((oldIter != pOldStatus->m_listSMVSendVirtualChannelGroup.end()) && 
        (newIter != m_listSMVSendVirtualChannelGroup.end()))
    {
        CSMVSendVirtualChannel* pOldSMVSendVirtualChannel = *oldIter;
        CSMVSendVirtualChannel* pNewSMVSendVirtualChannel = *newIter;
        if ((NULL != pOldSMVSendVirtualChannel) && (NULL != pNewSMVSendVirtualChannel) && (pOldSMVSendVirtualChannel->GetChannelCount() > 0) )
        {
            // 比较虚拟通道
            if (!pNewSMVSendVirtualChannel->Compare(eCompareType, pOldSMVSendVirtualChannel))
            {
                // 如果需要找出所有的不同，那么设置变位标识
                if (TYPE_FIND_ALL_DIFFERENT == eCompareType)
                {
                    bRet = false;
                }
                // 如果只需找出一处不同，那么直接返回
                else if (TYPE_FIND_ONE_DIFFERENT == eCompareType)
                {
                    return false;
                }
            }
        }

        oldIter++;
        newIter++;
    }

    return bRet;
}

///
/// @brief
///     查找状态对象中变化的DO
///
/// @param[in]  pOldStatus          待比较的状态
/// @param[out] mapChangedDoArray   保存值发生了变化的DO通道的名字和值
///
/// @return
///     bool    有变化的DO时返回true
///
bool CStatus::__FindChangedDO( CStatus* pOldStatus, std::map<std::wstring, bool>& mapChangedDoArray )
{
    bool            bFlag = false;
    bool            bNewValue;
    bool            bOldValue;
    std::wstring    strDOName;
    std::map<std::wstring, bool>::iterator newIter;
    std::map<std::wstring, bool>::iterator oldIter;


    // 遍历每一个DO通道，比较新老DO通道的值
    mapChangedDoArray.clear();
    for (newIter = m_mapDOGroup.begin(); newIter != m_mapDOGroup.end(); newIter++)
    {
        // 获得DO通道名和通道值
        strDOName   = newIter->first;
        bNewValue   = newIter->second;

        // 如果有与之对比的状态，那么比较新老DO通道的值
        if (pOldStatus)
        {
            // 查找老状态中同名的DO通道
            oldIter = pOldStatus->m_mapDOGroup.find(strDOName);
            if (oldIter == pOldStatus->m_mapDOGroup.end())
            {
                continue;
            }

            // 比较新老DO通道的值
            bOldValue   = oldIter->second;
            if (bNewValue != bOldValue)
            {
                // 保存值发生了变化的DO通道的名字
                mapChangedDoArray.insert(std::make_pair(strDOName, bNewValue));
                bFlag   = true;
            }
        }
        // 如果没有与之对比的状态，那么认为所有的DO通道都发生了变化
        else
        {
            mapChangedDoArray.insert(std::make_pair(strDOName, bNewValue));
            bFlag   = true;
        }
    }

    return bFlag;
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
bool CStatus::EqualAmplitude( CSMVSendVirtualChannel* pCurVirtualChannel )
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
///    设置指定名字的虚拟通道的幅值
///
/// @param[in]  strChannelName    虚拟通道名
/// @param[in]  dNewAmplitude    新的幅值
///
/// @return
///     bool    成功返回true
///
bool CStatus::__SetSMVSendVCAmplitude( std::wstring strChannelName, double dNewAmplitude )
{
    CSMVSendVirtualChannel*     pVirtualChannel;
    std::list<CSMVSendVirtualChannel*>::iterator iter;


    // 查找指定名字的虚拟通道
    pVirtualChannel = __GetSMVSendVirtualChannel(strChannelName);
    if (pVirtualChannel)
    {
        // 修改虚拟通道的幅值
        return pVirtualChannel->SetAmplitude(dNewAmplitude);
    }

    return false;
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
bool CStatus::__SetSMVSendVCPrimaryAmplitude( std::wstring strChannelName, double dNewAmplitude )
{
    CSMVSendVirtualChannel*     pVirtualChannel;
    std::list<CSMVSendVirtualChannel*>::iterator iter;


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
bool CStatus::RatedAmplitude( CSMVSendVirtualChannel* pCurVirtualChannel )
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
bool CStatus::ThreePhaseBalance( CSMVSendVirtualChannel* pCurVirtualChannel )
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
bool CStatus::EqualPhaseAngle( CSMVSendVirtualChannel* pCurVirtualChannel )
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
bool CStatus::__SetSMVSendVCPhaseAngle( std::wstring strChannelName, double dNewPhaseAngle )
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
bool CStatus::PhaseAnglePositiveSequence( CSMVSendVirtualChannel* pCurVirtualChannel )
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
bool CStatus::PhaseAngleReverseSequence( CSMVSendVirtualChannel* pCurVirtualChannel )
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
///     double      相角值
///
double CStatus::__GetSMVSendVCPhaseAngle( std::wstring strChannelName )
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
bool CStatus::EqualFrequency( CSMVSendVirtualChannel* pCurVirtualChannel )
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
bool CStatus::SetAllVCFrequency( double dNewFrequency )
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

///
/// @brief
///    使所有虚拟通道的谐波类型相同
///
/// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
///
/// @return
///     bool    成功返回true
///
bool CStatus::EqualHarmonicType( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    enumHarmonicType            eNewType;
    CSMVSendVirtualChannel*     pVirtualChannel;
    std::list<CSMVSendVirtualChannel*>::iterator iter;


    // 检查输入参数
    if ((NULL == pCurVirtualChannel) || (pCurVirtualChannel->GetName().length() != 3))
    {
        return false;
    }

    // 设置所有虚拟通道的频率
    eNewType = pCurVirtualChannel->GetHarmonic()->GetHarmonicType();
    iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // 获得虚拟通道对象指针
        pVirtualChannel = *iter;
        if (pVirtualChannel)
        {
            pVirtualChannel->GetHarmonic()->SetHarmonicType(eNewType);
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    使同组虚拟通道的谐波幅值相等
///
/// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
///
/// @return
///     bool    成功返回true
///
bool CStatus::EqualHarmonicAmplitude( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    double                      dNewAmplitude;
    CSMVSendVirtualChannel*     pVirtualChannel;
    std::list<CSMVSendVirtualChannel*>::iterator iter;


    // 检查输入参数
    if ((NULL == pCurVirtualChannel) || (pCurVirtualChannel->GetName().length() != 3))
    {
        return false;
    }

    // 设置所有虚拟通道的频率
    dNewAmplitude = pCurVirtualChannel->GetHarmonic()->GetAmplitude();
    iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // 获得虚拟通道对象指针
        pVirtualChannel = *iter;
        if ((NULL != pVirtualChannel)
            && (pVirtualChannel->GetName().length() == 3)
            && (pVirtualChannel->GetName().at(0) == pCurVirtualChannel->GetName().at(0)))
        {
            pVirtualChannel->GetHarmonic()->SetAmplitude(dNewAmplitude);
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    使同组虚拟通道的谐波相角相等
///
/// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
///
/// @return
///     bool    成功返回true
///
bool CStatus::EqualHarmonicPhaseAngle( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    double                      dNewPhaseAngle;
    CSMVSendVirtualChannel*     pVirtualChannel;
    std::list<CSMVSendVirtualChannel*>::iterator iter;


    // 检查输入参数
    if ((NULL == pCurVirtualChannel) || (pCurVirtualChannel->GetName().length() != 3))
    {
        return false;
    }

    // 设置所有虚拟通道的频率
    dNewPhaseAngle = pCurVirtualChannel->GetHarmonic()->GetPhaseAngle();
    iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // 获得虚拟通道对象指针
        pVirtualChannel = *iter;
        if ((NULL != pVirtualChannel)
            && (pVirtualChannel->GetName().length() == 3)
            && (pVirtualChannel->GetName().at(0) == pCurVirtualChannel->GetName().at(0)))
        {
            pVirtualChannel->GetHarmonic()->SetPhaseAngle(dNewPhaseAngle);
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    获得状态测试结果
///
/// @return
///     CStateTestResult*    指向测试结果对象的指针
///
CStateTestResult* CStatus::GetStateTestResult() const
{
    return m_pStateTestResult;
}

///
/// @brief
///    设置状态测试结果
///
/// @param[in]  pNewStateTestResult    设置测试结果对象的指针
///
void CStatus::SetStateTestResult( CStateTestResult* pNewStateTestResult )
{
    m_pStateTestResult = pNewStateTestResult;
}
