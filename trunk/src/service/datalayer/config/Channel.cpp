/// @file
///
/// @brief
///     通道
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.28
///
/// 修改历史：
///

#include "StdAfx.h"
#include "Channel.h"
#include "GooseCb.h"
#include "src\service\datalayer\send\FrameSend.h"
#include "src\service\datalayer\utility\data_utility.h"

#define MAX_HISTORY_SEND_RECORD         3                               ///< 最大历史发送记录数
#define MAX_HISTORY_RECV_RECORD         3                               ///< 最大历史接收记录数

#define NODE_Channel_Description        L"Description"                  ///< 通道描述
#define NODE_Channel_Type               L"Type"                         ///< 通道类型
#define NODE_Channel_Measure            L"measure"                      ///< 通道是否属于测量类型
#define NODE_Channel_Phase              L"Phase"                        ///< 相位
#define NODE_Channel_OnceRatings        L"OnceRatings"                  ///< 一次额定值
#define NODE_Channel_SecondaryRatings   L"SecondaryRatings"             ///< 二次额定值
#define NODE_Channel_VirtualChannel     L"VirtualChannel"               ///< 映射
#define NODE_Channel_Value              L"Value"                        ///< 通道GOOSE值
#define NODE_Channel_SubChannel         L"SubChannel"                   ///< 子通道

///
/// @brief
///    定义预定义通道信息结构体
///
struct PREDEFINED_GOOSE_SUBCHANNEL_INFO
{
    uint32_t                nIndex;                                     ///< 通道索引
    std::wstring            strChannelDesc;                             ///< 通道描述
    enumChannelType         enChannelType;                              ///< 通道类型
};

///
/// @brief
///    南瑞Ft3预定通道信息，9-1同南瑞ft3
///
static const PREDEFINED_GOOSE_SUBCHANNEL_INFO PREDEFINED_SUBCHANNEL_INFO_GOOSE[] = {
    { 0,    L"stVal",   CHANNEL_TYPE_GOOSE_POINT},
    { 1,    L"q",       CHANNEL_TYPE_GOOSE_QUALITY},
    { 2,    L"t",       CHANNEL_TYPE_GOOSE_TIME}
};

///
/// @brief
///    构造函数
///
CChannel::CChannel(void)
{
}

///
/// @brief
///     构造函数
///
/// @param[in]  pParentBaseCb       指向父控制块的指针
/// @param[in]  pParentChannel      指向父通道的指针
/// @param[in]  pParamConfigBase    指向参数配置管理器的指针
///
CChannel::CChannel(CBaseCb* pParentBaseCb, CChannel* pParentChannel, CParamConfigBase* pParamConfigBase)
:m_pParentBaseCb(pParentBaseCb)
,m_pParentChannel(pParentChannel)
,m_pParamConfigBase(pParamConfigBase)
,m_bSelected(false)
,m_strDescription(L"")
,m_eChannelType(CHANNEL_TYPE_UNKOWN)
,m_usQuality(0)
,m_bMeasure(false)
,m_eChannelPhase(CHANNEL_PHASE_UNKOWN)
,m_dOnceRatings(1.0f)
,m_dSecondaryRatings(1.0f)
,m_strVirtualChannel(L"")
,m_bIsSendValueChanged(false)
,m_pSourceChannel(NULL)
,m_pPrev(NULL)
,m_pNext(NULL)
{
}

///
/// @brief
///    析构函数
///
CChannel::~CChannel(void)
{
    // 清除所有子通道
    ClearAllSubChannel();
}

///
/// @brief
///    获得通道（子通道）在所属控制块（父通道）中的索引号（从0开始计数）
///
/// @return
///     int    通道的索引号，-1表示失败。
///
int CChannel::GetIndex()
{
    int nIndex = -1;


    if (NULL != m_pParentChannel)
    {
        // 遍历父通道的所有子通道，得到当前子通道在父通道中的索引
        CChannel* pSubChannel = m_pParentChannel->FirstSubChannel();
        while(pSubChannel)
        {
            nIndex++;
            if (pSubChannel == this)
            {
                break;
            }

            pSubChannel = m_pParentChannel->NextSubChannel(pSubChannel);
        }
    }
    else if(NULL != m_pParentBaseCb)
    {
        // 遍历控制块的所有通道，得到当前通道在控制块中的索引
        CChannel* pChannel = m_pParentBaseCb->FirstChannel();
        while(pChannel)
        {
            nIndex++;
            if (pChannel == this)
            {
                break;
            }

            pChannel = m_pParentBaseCb->NextChannel(pChannel);
        }
    }

    return nIndex;
}

///
/// @brief
///    获得控制块选中状态
///
/// @return
///     bool    返回true表示控制块被选中
///
bool CChannel::IsSelected()
{
    return m_bSelected;
}

///
/// @brief
///    设置控制块选中状态
///
/// @param[in]  bSelected  控制块选中状态
///
void CChannel::Selected(bool bSelected)
{
    if (m_bSelected != bSelected)
    {
        m_bSelected = bSelected;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得通道描述信息
///
/// @return
///     wstring    通道描述信息
///
std::wstring CChannel::GetDescription()
{
    return m_strDescription;
}

///
/// @brief
///    设置通道描述信息
///
/// @param[in]  strDescription  通道描述信息
///
void CChannel::SetDescription(std::wstring strDescription)
{
    if (m_strDescription != strDescription)
    {
        m_strDescription = strDescription;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得通道类型
///
/// @return
///     enumChannelType    通道类型
///
enumChannelType CChannel::GetChannelType()
{
    return m_eChannelType;
}

///
/// @brief
///    设置通道类型
///
/// @param[in]  eNewChannelType  通道类型
///
/// @return
///     bool    成功返回true
///
bool CChannel::SetChannelType(enumChannelType eNewChannelType)
{
    if (m_eChannelType != eNewChannelType)
    {
        // 获得基本参数配置管理器
        CParamConfigBasic* pParamConfigBasic = CParamConfigBasic::getInstance();
        if (NULL == pParamConfigBasic)
        {
            return false;
        }

        // 设置通道类型，并修改默认的一次值、二次值
        switch(eNewChannelType)
        {
        case CHANNEL_TYPE_UNKOWN:                                       ///< 未知
            {
                // 修改一次、二次值
                m_dOnceRatings          = 1.0f;
                m_dSecondaryRatings     = 1.0f;

                // 清空虚拟通道映射
                m_strVirtualChannel     = L"";
            }
            break;

        case CHANNEL_TYPE_SMV_CURRENT:                                  ///< 电流
            {
                // 修改描述信息
                m_strDescription = GenerateChannelDes(m_strDescription, m_eChannelType, eNewChannelType, m_eChannelPhase, m_eChannelPhase, m_bMeasure, m_bMeasure);

                // 修改一次、二次值
                m_dOnceRatings          = pParamConfigBasic->GetRatedPrimaryCurrent();
                m_dSecondaryRatings     = pParamConfigBasic->GetRatedSecondaryCurrent();

                // 清空虚拟通道映射
                m_strVirtualChannel     = L"";
            }
            break;

        case CHANNEL_TYPE_SMV_VOLTAGE:                                  ///< 电压
            {
                // 修改描述信息
                m_strDescription = GenerateChannelDes(m_strDescription, m_eChannelType, eNewChannelType, m_eChannelPhase, m_eChannelPhase, m_bMeasure, m_bMeasure);

                // 修改一次、二次值
                m_dOnceRatings          = pParamConfigBasic->GetRatedPrimaryVoltage() * 1000;
                m_dSecondaryRatings     = pParamConfigBasic->GetRatedSecondaryVoltage();

                // 清空虚拟通道映射
                m_strVirtualChannel     = L"";
            }
            break;

        case CHANNEL_TYPE_SMV_TIME:                                     ///< 时间
            {
                // 修改描述信息
                m_strDescription = GenerateChannelDes(m_strDescription, m_eChannelType, eNewChannelType, m_eChannelPhase, m_eChannelPhase, m_bMeasure, m_bMeasure);

                // 修改一次、二次值
                swscanf_s(pParamConfigBasic->GetMuRatedDelay().c_str(), L"%lf", &m_dOnceRatings, sizeof(m_dOnceRatings));
                swscanf_s(pParamConfigBasic->GetMuRatedDelay().c_str(), L"%lf", &m_dSecondaryRatings, sizeof(m_dSecondaryRatings));

                // 清空虚拟通道映射
                m_strVirtualChannel     = L"";
            }
            break;

        case CHANNEL_TYPE_GOOSE_POINT:                                  ///< 单点
            {
                m_stGooseSendValue.__SetType(GSEDT_BOOLEAN);
                m_stGooseRecvValue.__SetType(GSEDT_BOOLEAN);
            }
            break;

        case CHANNEL_TYPE_GOOSE_DOUBLE_POINT:                           ///< 双点
            {
                m_stGooseSendValue.__SetType(GSEDT_BIT_STR);
                m_stGooseSendValue.m_stBitString.SetString(strDPValue[DP_VALUE_INIT]);

                m_stGooseRecvValue.__SetType(GSEDT_BIT_STR);
                m_stGooseRecvValue.m_stBitString.SetString(strDPValue[DP_VALUE_INIT]);
            }
            break;

        case CHANNEL_TYPE_GOOSE_QUALITY:                                ///< 品质
            {
                m_stGooseSendValue.__SetType(GSEDT_BIT_STR);
                m_stGooseSendValue.m_stBitString.SetString(L"0000000000000");

                m_stGooseRecvValue.__SetType(GSEDT_BIT_STR);
                m_stGooseRecvValue.m_stBitString.SetString(L"0000000000000");
            }
            break;

        case CHANNEL_TYPE_GOOSE_TIME:                                   ///< GOOSE时间
            {
                m_stGooseSendValue.__SetType(GSEDT_UTC_TIME);
                m_stGooseRecvValue.__SetType(GSEDT_UTC_TIME);
            }
            break;

        case CHANNEL_TYPE_GOOSE_FLOAT:                                  ///< 浮点数
            {
                m_stGooseSendValue.__SetType(GSEDT_FLOAT);
                m_stGooseRecvValue.__SetType(GSEDT_FLOAT);
            }
            break;

        case CHANNEL_TYPE_GOOSE_INT8:                                   ///< INT8
            {
                m_stGooseSendValue.__SetType(GSEDT_INT8);
                m_stGooseRecvValue.__SetType(GSEDT_INT8);
            }
            break;

        case CHANNEL_TYPE_GOOSE_INT16:                                  ///< INT16
            {
                m_stGooseSendValue.__SetType(GSEDT_INT16);
                m_stGooseRecvValue.__SetType(GSEDT_INT16);
            }
            break;

        case CHANNEL_TYPE_GOOSE_INT24:                                  ///< INT24
            {
                m_stGooseSendValue.__SetType(GSEDT_INT24);
                m_stGooseRecvValue.__SetType(GSEDT_INT24);
            }
            break;

        case CHANNEL_TYPE_GOOSE_INT32:                                  ///< INT32
            {
                m_stGooseSendValue.__SetType(GSEDT_INT32);
                m_stGooseRecvValue.__SetType(GSEDT_INT32);
            }
            break;

        case CHANNEL_TYPE_GOOSE_UINT8:                                  ///< UINT8
            {
                m_stGooseSendValue.__SetType(GSEDT_UINT8);
                m_stGooseRecvValue.__SetType(GSEDT_UINT8);
            }
            break;

        case CHANNEL_TYPE_GOOSE_UINT16:                                 ///< UINT16
            {
                m_stGooseSendValue.__SetType(GSEDT_UINT16);
                m_stGooseRecvValue.__SetType(GSEDT_UINT16);
            }
            break;

        case CHANNEL_TYPE_GOOSE_UINT24:                                 ///< UINT24
            {
                m_stGooseSendValue.__SetType(GSEDT_UINT24);
                m_stGooseRecvValue.__SetType(GSEDT_UINT24);
            }
            break;

        case CHANNEL_TYPE_GOOSE_UINT32:                                 ///< UINT32
            {
                m_stGooseSendValue.__SetType(GSEDT_UINT32);
                m_stGooseRecvValue.__SetType(GSEDT_UINT32);
            }
            break;

        case CHANNEL_TYPE_GOOSE_STRUCT:                                 ///< 结构
            {
                //
                // 判断通道是否含有子通道，如果没有，那么添加默认子通道
                //
                if (m_listSubChannelGroup.size() == 0)
                {
                    // 向通道添加默认子通道
                    for (int i = 0; i < sizeof(PREDEFINED_SUBCHANNEL_INFO_GOOSE)/ sizeof(PREDEFINED_SUBCHANNEL_INFO_GOOSE[0]); i++)
                    {
                        // 添加子通道
                        CChannel*   pChannel = __AddSubChannel();
                        if (NULL == pChannel)
                        {
                            return false;
                        }

                        // 设置子通道描述信息
                        pChannel->SetChannelType(PREDEFINED_SUBCHANNEL_INFO_GOOSE[i].enChannelType);
                        pChannel->m_strDescription  = PREDEFINED_SUBCHANNEL_INFO_GOOSE[i].strChannelDesc;
                    }
                }
            }
            break;

        default:
            {
                return false;
            }
        }

        // 设置通道类型
        m_eChannelType = eNewChannelType;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    获得通道测量类型
///
/// @return
///     bool    通道是否属于测量类型
///
bool CChannel::IsMeasure()
{
    return m_bMeasure;
}

///
/// @brief
///    设置通道测量类型
///
/// @param[in]  bNewMeasure  通道是否属于测量类型
///
void CChannel::SetMeasure(bool bNewMeasure)
{
    if (m_bMeasure != bNewMeasure)
    {
        // 修改通道描述信息
        m_strDescription = GenerateChannelDes(m_strDescription, m_eChannelType, m_eChannelType, m_eChannelPhase, m_eChannelPhase, m_bMeasure, bNewMeasure);

        // 保存测量/保护标记
        m_bMeasure = bNewMeasure;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得通道相位
///
/// @return
///     enumChannelType    通道相位
///
ChannelPhase CChannel::GetChannelPhase()
{
    return m_eChannelPhase;
}

///
/// @brief
///    设置通道相位
///
/// @param[in]  eNewChannelPhase  通道相位
///
void CChannel::SetChannelPhase(ChannelPhase eNewChannelPhase)
{
    if (m_eChannelPhase != eNewChannelPhase)
    {
        // 修改通道描述信息
        m_strDescription = GenerateChannelDes(m_strDescription, m_eChannelType, m_eChannelType, m_eChannelPhase, eNewChannelPhase, m_bMeasure, m_bMeasure);

        // 保存新的相角类型
        m_eChannelPhase = eNewChannelPhase;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得一次额定值（电流单位为A，电压单位为V）
///
/// @return
///     double    一次额定值
///
double CChannel::GetOnceRatings()
{
    return m_dOnceRatings;
}

///
/// @brief
///    设置一次额定值（电流单位为A，电压单位为V）
///
/// @param[in]  dOnceRatings  一次额定值
///
void CChannel::SetOnceRatings(double dOnceRatings)
{
    if (fabs(m_dOnceRatings - dOnceRatings) >= FLOAT_PRECISION)
    {
        m_dOnceRatings = dOnceRatings;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得二次额定值（电流单位为A，电压单位为V）
///
/// @return
///     double    二次额定值
///
double CChannel::GetSecondaryRatings()
{
    return m_dSecondaryRatings;
}

///
/// @brief
///    设置二次额定值（电流单位为A，电压单位为V）
///
/// @param[in]  dSecondaryRatings  二次额定值
///
void CChannel::SetSecondaryRatings(double dSecondaryRatings)
{
    if (fabs(m_dSecondaryRatings - dSecondaryRatings) >= FLOAT_PRECISION)
    {
        m_dSecondaryRatings = dSecondaryRatings;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得通道映射
///
/// @return
///     wstring    通道映射
///
std::wstring CChannel::GetVirtualChannel()
{
    return m_strVirtualChannel;
}

///
/// @brief
///    设置通道映射
///
/// @param[in]  strVirtualChannel  通道映射
///
void CChannel::SetVirtualChannel(std::wstring strVirtualChannel)
{
    if (m_strVirtualChannel != strVirtualChannel)
    {
        m_strVirtualChannel = strVirtualChannel;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    添加子通道
///
/// @param[in]  pNewChannel 指向Channel对象的指针
///
/// @return
///     bool    true表示添加成功
///
bool CChannel::__AddSubChannel(CChannel* pNewChannel)
{
    if (NULL == pNewChannel)
    {
        return false;
    }

    // 添加GOOSE控制块到容器中
    m_listSubChannelGroup.push_back(pNewChannel);
    pNewChannel->__SetParentChannel(this);

    return true;
}

///
/// @brief
///    添加子通道
///
/// @return
///     CChannel*    成功时返回子通道指针，失败时返回NULL
///
CChannel* CChannel::__AddSubChannel()
{
    // new一个子通道
    CChannel*  pSubChannel = new CChannel(m_pParentBaseCb, this, m_pParamConfigBase);
    if (NULL == pSubChannel)
    {
        return NULL;
    }

    // 添加GOOSE控制块到容器中
    m_listSubChannelGroup.push_back(pSubChannel);
    return pSubChannel;
}

///
/// @brief
///    枚举第一个子通道
///
/// @return
///     CChannel*    成功时返回通道指针，失败时返回NULL
///
CChannel* CChannel::FirstSubChannel()
{
    std::list<CChannel*>::iterator iter = m_listSubChannelGroup.begin();
    if (m_listSubChannelGroup.end() != iter)
    {
        return *iter;
    }

    return NULL;
}

///
/// @brief
///    获得下一个子通道
///
/// @param[in]  pCurSubChannel 当前子通道的指针
///
/// @return
///     CChannel*    成功时返回子通道指针，失败时返回NULL
///
CChannel* CChannel::NextSubChannel(CChannel* pCurSubChannel)
{
    CChannel*                       pSubChannel   = NULL;
    std::list<CChannel*>::iterator  iter;


    iter = m_listSubChannelGroup.begin();
    while (m_listSubChannelGroup.end() != iter)
    {
        pSubChannel = *iter;

        // 查找当前GOOSE控制块的位置
        if (pSubChannel == pCurSubChannel)
        {
            // 获得下一个GOOSE控制块的位置
            iter++;
            if (m_listSubChannelGroup.end() == iter)
            {
                return NULL;
            }

            return (*iter);
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    获得上一个子通道
///
/// @param[in]  pCurSubChannel 当前子通道的指针
///
/// @return
///     CChannel*    成功时返回子通道指针，失败时返回NULL
///
CChannel* CChannel::PrevSubChannel(CChannel* pCurSubChannel)
{
    CChannel*                               pSubChannel   = NULL;
    std::list<CChannel*>::reverse_iterator  rIter;


    rIter = m_listSubChannelGroup.rbegin();
    while (m_listSubChannelGroup.rend() != rIter)
    {
        pSubChannel = *rIter;

        // 查找当前GOOSE控制块的位置
        if (pSubChannel == pCurSubChannel)
        {
            // 获得上一个GOOSE控制块的位置
            rIter++;
            if (m_listSubChannelGroup.rend() == rIter)
            {
                return NULL;
            }

            return (*rIter);
        }

        rIter++;
    }

    return NULL;
}

///
/// @brief
///    清除所有子通道
///
void CChannel::ClearAllSubChannel()
{
    // 遍历容器，释放对象
    std::list<CChannel*>::iterator iter = m_listSubChannelGroup.begin();
    while (m_listSubChannelGroup.end() != iter)
    {
        CChannel* pSubChannel = *iter;
        if (pSubChannel)
        {
            delete pSubChannel;
        }

        iter++;
    }

    if (m_listSubChannelGroup.size())
    {
        // 清空子通道
        m_listSubChannelGroup.clear();

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得子通道数
///
/// @return
///     int    子通道数
///
int CChannel::GetSubChannelCount()
{
    return (int)m_listSubChannelGroup.size();
}

///
/// @brief
///    深度拷贝一个通道
///
/// @param[in]  pChannel    指向Channel对象的指针
///
/// @return
///     bool    拷贝成功返回TRUE
///
bool CChannel::__Copy( CChannel* pChannel )
{
    CChannel*       pNewSubChannel  = NULL;


    // 检查输入参数
    if (NULL == pChannel)
    {
        return false;
    }
    m_pSourceChannel            = pChannel;

    // 复制基本属性
    SetChannelType(pChannel->m_eChannelType);
    m_strDescription            = pChannel->m_strDescription;
    m_usQuality                 = pChannel->m_usQuality;
    m_bMeasure                  = pChannel->m_bMeasure;
    m_eChannelPhase             = pChannel->m_eChannelPhase;
    m_strVirtualChannel         = pChannel->m_strVirtualChannel;
    m_stGooseRecvValue          = pChannel->m_stGooseRecvValue;
    m_stGooseSendValue          = pChannel->m_stGooseSendValue;

    // 清空所有子通道
    ClearAllSubChannel();

    // 添加子通道
    std::list<CChannel*>::iterator iter = pChannel->m_listSubChannelGroup.begin();
    while (pChannel->m_listSubChannelGroup.end() != iter)
    {
        CChannel* pSubChannel = *iter;
        if (pSubChannel)
        {
            pNewSubChannel = new CChannel(GetParentBaseCb(), this, m_pParamConfigBase);
            if (NULL == pNewSubChannel)
            {
                return false;
            }

            // 深度拷贝子通道
            if (!pNewSubChannel->__Copy(pSubChannel))
            {
                delete pNewSubChannel;
                return false;
            }

            // 添加子通道到容器中
            if (!__AddSubChannel(pNewSubChannel))
            {
                delete pNewSubChannel;
                return false;
            }
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    深度拷贝一个GOOSE_DATA对象
///
/// @param[in]  pGooseData 指向GOOSE_DATA对象的指针
///
/// @return
///     bool    拷贝成功返回TRUE
///
bool CChannel::__Copy(GOOSE_DATA* pGooseData)
{
    CChannel*       pNewSubChannel  = NULL;
    enumChannelType eChannelType    = CHANNEL_TYPE_UNKOWN;


    // 检查输入参数
    if (NULL == pGooseData)
    {
        return false;
    }

    //
    // 设置通道类型
    //
    if (GSEDT_UTC_TIME == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_TIME;
    }
    else if (GSEDT_BOOLEAN == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_POINT;
    }
    else if (GSEDT_BIT_STR == pGooseData->type)
    {
        GOOSE_BIT_STRING*   pGooseBitString = (GOOSE_BIT_STRING*)pGooseData;
        int                 iBitCount       = (pGooseBitString->strSize * 8) - pGooseBitString->padding;

        // 根据位数判断通道类型
        if (2 == iBitCount)
        {
            eChannelType = CHANNEL_TYPE_GOOSE_DOUBLE_POINT;
        }
        else if (13 == iBitCount)
        {
            eChannelType = CHANNEL_TYPE_GOOSE_QUALITY;
        }
    }
    else if (GSEDT_INT8 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_INT8;
    }
    else if (GSEDT_INT16 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_INT16;
    }
    else if (GSEDT_INT24 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_INT24;
    }
    else if (GSEDT_INT32 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_INT32;
    }
    else if (GSEDT_UINT8 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_UINT8;
    }
    else if (GSEDT_UINT16 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_UINT16;
    }
    else if (GSEDT_UINT24 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_UINT24;
    }
    else if (GSEDT_UINT32 == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_UINT32;
    }
    else if (GSEDT_FLOAT == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_FLOAT;
    }
    else if (GSEDT_STRUCT == pGooseData->type)
    {
        eChannelType = CHANNEL_TYPE_GOOSE_STRUCT;                       ///< 通道类型

        // 清空所有子通道
        ClearAllSubChannel();

        //
        // 添加子通道
        //
        GOOSE_STRUCT* pGooseStruct = (GOOSE_STRUCT*)pGooseData;
        if (NULL == pGooseStruct)
        {
            return false;
        }

        std::vector<GOOSE_DATA*>::iterator iter = pGooseStruct->member.begin();
        while(pGooseStruct->member.end() != iter)
        {
            GOOSE_DATA* pSubGooseData = *iter;
            if (pSubGooseData)
            {
                pNewSubChannel = new CChannel(GetParentBaseCb(), this, m_pParamConfigBase);
                if (NULL == pNewSubChannel)
                {
                    return false;
                }

                // 深度拷贝子通道
                if (!pNewSubChannel->__Copy(pSubGooseData))
                {
                    delete pNewSubChannel;
                    return false;
                }

                // 添加子通道到容器中
                if (!__AddSubChannel(pNewSubChannel))
                {
                    delete pNewSubChannel;
                    return false;
                }
            }

            iter++;
        }
    }

    // 设置通道类型
    SetChannelType(eChannelType);

    // 设置通道名
    {
        CString     csText;

        if (m_pParentChannel)
        {
            csText.Format(_T("子通道%d"), m_pParentChannel->GetSubChannelCount() + 1);
        }
        else if (m_pParentBaseCb)
        {
            csText.Format(_T("GOOSE通道%d"), m_pParentBaseCb->GetChannelCount() + 1);
        }

        m_strDescription = csText.GetString();
    }

    // 设置接收值
    m_stGooseRecvValue = pGooseData;

    return true;
}

///
/// @brief
///    设置通道所属的控制块
///
/// @param[in]  pBaseCb    控制块指针
///
/// @return
///     void    
///
void CChannel::__SetParentBaseCb( CBaseCb* pBaseCb )
{
    m_pParentBaseCb = pBaseCb;
}

///
/// @brief
///     获得通道所属控制块对象的指针
///
/// @return
///     CBaseCb*    通道所属控制块对象的指针
///
CBaseCb* CChannel::GetParentBaseCb()
{
    return m_pParentBaseCb;
}

///
/// @brief
///    设置通道所属的父通道指针
///
/// @param[in]  pChannel    父通道指针
///
/// @return
///     void    
///
void CChannel::__SetParentChannel( CChannel* pChannel )
{
    m_pParentChannel = pChannel;
}

///
/// @brief
///    获得父通道指针
///
/// @return
///     CChannel*    父通道指针
///
CChannel* CChannel::GetParentChannel()
{
    return m_pParentChannel;
}

///
/// @brief
///     获得GOOSE通道的发送值
///
/// @return
///     GOOSE_DATA_VARIANT    GOOSE通道的初始值
///
GOOSE_DATA_VARIANT& CChannel::GetGooseSendValue()
{
    return m_stGooseSendValue;
}

///
/// @brief
///    设置GOOSE通道的发送值
///
/// @param[in]  stNewGooseValue    GOOSE通道的发送值
///
/// @return
///     bool    成功返回true
///
bool CChannel::SetGooseSendValue( GOOSE_DATA_VARIANT& stNewGooseValue )
{
    if (m_stGooseSendValue != stNewGooseValue)
    {
        m_stGooseSendValue = stNewGooseValue;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    获得GOOSE通道的实时值
///
/// @return
///     GOOSE_DATA_VARIANT    GOOSE通道的实时值
///
GOOSE_DATA_VARIANT& CChannel::GetGooseRecvValue()
{
    return m_stGooseRecvValue;
}

///
/// @brief
///    设置GOOSE通道的实时值
///
/// @param[in]  stGooseValue    GOOSE通道的实时值
///
/// @return
///     bool    成功返回true
///
bool CChannel::SetGooseRecvValue( GOOSE_DATA_VARIANT& stGooseValue )
{
    if (m_stGooseRecvValue != stGooseValue)
    {
        m_stGooseRecvValue = stGooseValue;
    }

    return true;
}

///
/// @brief
///    获得通道品质值
///
/// @return
///     unsigned short    通道品质值
///
unsigned short CChannel::GetQuality()
{
    return m_usQuality;
}

///
/// @brief
///    设置通道品质值
///
/// @param[in]  usNewQuality    新的品质值
///
/// @return
///     bool    成功返回true
///
bool CChannel::SetQuality( unsigned short usNewQuality )
{
    if (m_usQuality != usNewQuality)
    {
        m_usQuality = usNewQuality;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
            m_pParamConfigBase->SetQualityChanged(true);
        }
    }

    return true;
}

///
/// @brief
///    判断通道发送值是否与指定通道的相等
///
/// @param[in]  pOldChannel    指向通道对象的指针
///
/// @return
///     bool    通道值相等返回true
///
bool CChannel::__CompareSendValue( CChannel* pOldChannel )
{
    // 结构体类型的通道需要分别比较子通道的值
    if (CHANNEL_TYPE_GOOSE_STRUCT == m_eChannelType)
    {
        bool                            bRet = true;
        std::list<CChannel *>::iterator oldIter;
        std::list<CChannel *>::iterator newIter;


        // 判断子通道数是否相等
        if (m_listSubChannelGroup.size() != pOldChannel->m_listSubChannelGroup.size())
        {
            return false;
        }

        // 依次比较每一个子通道的发送值是否相等
        oldIter = pOldChannel->m_listSubChannelGroup.begin();
        newIter = m_listSubChannelGroup.begin();
        while((oldIter != pOldChannel->m_listSubChannelGroup.end()) && (newIter != m_listSubChannelGroup.end()))
        {
            CChannel* pOldSubChannel = *oldIter;
            CChannel* pNewSubChannel = *newIter;
            if ((NULL != pOldSubChannel) && (NULL != pNewSubChannel))
            {
                // 比较子通道发送值
                if (!pNewSubChannel->__CompareSendValue(pOldSubChannel))
                {
                    // 标记为已修改
                    pNewSubChannel->SetSendValueChanged(true);
                    bRet = false;
                }
            }

            oldIter++;
            newIter++;
        }

        return bRet;
    }
    // 屏蔽时间类型的GOOSE通道
    else if (CHANNEL_TYPE_GOOSE_TIME == m_eChannelType)
    {
        return true;
    }
    // 非结构体类型的通道直接比较GOOSE发送值
    else
    {
        bool bRet = !(m_stGooseSendValue != pOldChannel->m_stGooseSendValue);
        return bRet;
    }
}

///
/// @brief
///     通道的发送值是否发生了改变
///
/// @return
///     bool    true表示通道的发送值发生了改变
///
bool CChannel::IsSendValueChanged()
{
    return m_bIsSendValueChanged;
}

///
/// @brief
///    设置通道的发送值是否发生了改变
///
/// @param[in]  bIsChanged    通道值是否发生了改变
///
void CChannel::SetSendValueChanged( bool bIsChanged )
{
    if (m_bIsSendValueChanged != bIsChanged)
    {
        m_bIsSendValueChanged = bIsChanged;
    }
}

///
/// @brief
///    获得通道值在通信报文中所占字节数
///
/// @return
///     int    字节数
///
int CChannel::GetValueLength()
{
    int     iValueLen = 4;


    switch(m_eChannelType)
    {
    case CHANNEL_TYPE_SMV_CURRENT:
    case CHANNEL_TYPE_SMV_VOLTAGE:
    case CHANNEL_TYPE_SMV_TIME:
    case CHANNEL_TYPE_GOOSE_DOUBLE_POINT:
        iValueLen = 4;
        break;

    case CHANNEL_TYPE_GOOSE_STRUCT:
        {
            // 统计所有子通道在报文中所占字节数
            iValueLen = 2;
            std::list<CChannel*>::iterator iter = m_listSubChannelGroup.begin();
            while (m_listSubChannelGroup.end() != iter)
            {
                CChannel* pSubChannel = *iter;
                if (pSubChannel)
                {
                    iValueLen += pSubChannel->GetValueLength();
                }

                iter++;
            }
        }
        break;

    case CHANNEL_TYPE_GOOSE_POINT:
        iValueLen = 3;
        break;

    case CHANNEL_TYPE_GOOSE_QUALITY:
        iValueLen = 5;
        break;

    case CHANNEL_TYPE_GOOSE_TIME:
        iValueLen = 10;
        break;

    case CHANNEL_TYPE_GOOSE_FLOAT:
        iValueLen = 7;
        break;

    case CHANNEL_TYPE_GOOSE_INT8:
    case CHANNEL_TYPE_GOOSE_UINT8:
        iValueLen = 3;
        break;

    case CHANNEL_TYPE_GOOSE_INT16:
    case CHANNEL_TYPE_GOOSE_UINT16:
        iValueLen = 4;
        break;

    case CHANNEL_TYPE_GOOSE_INT24:
    case CHANNEL_TYPE_GOOSE_UINT24:
        iValueLen = 5;
        break;

    case CHANNEL_TYPE_GOOSE_INT32:
    case CHANNEL_TYPE_GOOSE_UINT32:
        iValueLen = 6;
        break;

    default:
        break;
    }

    return iValueLen;
}

///
/// @brief
///    加载SMV<Channel>
///
/// @param[in]  pXml 指向XML对象的引用
///
/// @return
///     bool    true表示加载成功
///
bool CChannel::__LoadSmvConfig( CMarkup& xml )
{
    // <Description>
    if (!xml.FindElem(NODE_Channel_Description))
    {
        return false;
    }
    m_strDescription = xml.GetData();

    // <Type>
    if (!xml.FindElem(NODE_Channel_Type))
    {
        return false;
    }
    m_eChannelType = (enumChannelType)_wtoi(xml.GetData().c_str());

    // 通道是否属于测量类型
    if (_wtoi(xml.GetAttrib(NODE_Channel_Measure).c_str()))
    {
        m_bMeasure = true;
    }

    // <Phase>
    if (!xml.FindElem(NODE_Channel_Phase))
    {
        return false;
    }
    m_eChannelPhase = (ChannelPhase)_wtoi(xml.GetData().c_str());

    // <OnceRatings>
    if (!xml.FindElem(NODE_Channel_OnceRatings))
    {
        return false;
    }
    swscanf_s(xml.GetData().c_str(), L"%lf", &(m_dOnceRatings), sizeof(m_dOnceRatings));

    // <SecondaryRatings>
    if (!xml.FindElem(NODE_Channel_SecondaryRatings))
    {
        return false;
    }
    swscanf_s(xml.GetData().c_str(), L"%lf", &(m_dSecondaryRatings), sizeof(m_dSecondaryRatings));

    // <VirtualChannel>
    if (!xml.FindElem(NODE_Channel_VirtualChannel))
    {
        return false;
    }
    m_strVirtualChannel = xml.GetData();

    return true;
}

///
/// @brief
///    保存SMV<Channel>
///
/// @param[in]  pXml 指向XML对象的引用
///
/// @return
///     bool    true表示加载成功
///
bool CChannel::__SaveSmvConfig( CMarkup& xml )
{
    wchar_t     cBuffer[128] = {0};


    // <Description>
    if (!xml.AddElem(NODE_Channel_Description) || !xml.SetData(GetDescription()))
    {
        return false;
    }

    // <Type>
    if (!xml.AddElem(NODE_Channel_Type) || !xml.SetData(GetChannelType()))
    {
        return false;
    }

    // <Type measure="0">
    if (!xml.SetAttrib(NODE_Channel_Measure, (IsMeasure()? 1: 0)))
    {
        return false;
    }

    // <Phase>
    if (!xml.AddElem(NODE_Channel_Phase) || !xml.SetData((int)GetChannelPhase()))
    {
        return false;
    }

    // <OnceRatings>
    memset(cBuffer, 0, sizeof(cBuffer));
    swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]) - 1, L"%lf", GetOnceRatings());
    if (!xml.AddElem(NODE_Channel_OnceRatings) || !xml.SetData(cBuffer))
    {
        return false;
    }

    // <SecondaryRatings>
    memset(cBuffer, 0, sizeof(cBuffer));
    swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]) - 1, L"%lf", GetSecondaryRatings());
    if (!xml.AddElem(NODE_Channel_SecondaryRatings) || !xml.SetData(cBuffer))
    {
        return false;
    }

    // <VirtualChannel>
    if (!xml.AddElem(NODE_Channel_VirtualChannel) || !xml.SetData(GetVirtualChannel()))
    {
        return false;
    }

    return true;
}

///
/// @brief
///    加载GOOSE<Channel>
///
/// @param[in]  pXml 指向XML对象的引用
///
/// @return
///     bool    true表示加载成功
///
bool CChannel::__LoadGooseConfig( CMarkup& xml )
{
    // <Description>
    if (!xml.FindElem(NODE_Channel_Description))
    {
        return false;
    }
    m_strDescription = xml.GetData();

    // <Type>
    if (!xml.FindElem(NODE_Channel_Type))
    {
        return false;
    }
    m_eChannelType = (enumChannelType)_wtoi(xml.GetData().c_str());

    // 根据通道类型，设置接收值对象的值类型
    switch(m_eChannelType)
    {
    case CHANNEL_TYPE_GOOSE_POINT:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_BOOLEAN;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_BOOLEAN;
        }
        break;

    case CHANNEL_TYPE_GOOSE_DOUBLE_POINT:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_BIT_STR;
            m_stGooseSendValue.m_stBitString.SetString(strDPValue[DP_VALUE_INIT]);

            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_BIT_STR;
            m_stGooseRecvValue.m_stBitString.SetString(strDPValue[DP_VALUE_INIT]);
        }
        break;

    case CHANNEL_TYPE_GOOSE_QUALITY:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_BIT_STR;
            m_stGooseSendValue.m_stBitString.SetString(L"0000000000000");

            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_BIT_STR;
            m_stGooseRecvValue.m_stBitString.SetString(L"0000000000000");
        }
        break;

    case CHANNEL_TYPE_GOOSE_TIME:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_UTC_TIME;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_UTC_TIME;
        }
        break;

    case CHANNEL_TYPE_GOOSE_FLOAT:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_FLOAT;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_FLOAT;
        }
        break;

    case CHANNEL_TYPE_GOOSE_INT8:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_INT8;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_INT8;
        }
        break;

    case CHANNEL_TYPE_GOOSE_INT16:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_INT16;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_INT16;
        }
        break;

    case CHANNEL_TYPE_GOOSE_INT24:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_INT24;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_INT24;
        }
        break;

    case CHANNEL_TYPE_GOOSE_INT32:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_INT32;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_INT32;
        }
        break;

    case CHANNEL_TYPE_GOOSE_UINT8:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_UINT8;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_UINT8;
        }
        break;

    case CHANNEL_TYPE_GOOSE_UINT16:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_UINT16;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_UINT16;
        }
        break;

    case CHANNEL_TYPE_GOOSE_UINT24:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_UINT24;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_UINT24;
        }
        break;

    case CHANNEL_TYPE_GOOSE_UINT32:
        {
            m_stGooseSendValue.m_stUnKnown.id.type        = GSEDT_UINT32;
            m_stGooseRecvValue.m_stUnKnown.id.type        = GSEDT_UINT32;
        }
        break;
    }

    // <Value>
    if (xml.FindElem(NODE_Channel_Value))
    {
        int iValueType = _wtoi(xml.GetAttrib(L"type").c_str());
        switch(iValueType)
        {
        case GSEDT_BOOLEAN:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_BOOLEAN;
                if (1 == _wtoi(xml.GetData().c_str()))
                {
                    m_stGooseSendValue.m_stBoolean.boolVal = true;
                } 
                else
                {
                    m_stGooseSendValue.m_stBoolean.boolVal = false;
                }
            }
            break;

        case GSEDT_BIT_STR:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_BIT_STR;
                m_stGooseSendValue.m_stBitString.SetString(xml.GetData());
            }
            break;

        case GSEDT_INT8:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_INT8;
                m_stGooseSendValue.m_stInt8.datVal         = (int8_t)_wtoi(xml.GetData().c_str());
            }
            break;

        case GSEDT_INT16:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_INT16;
                m_stGooseSendValue.m_stInt16.datVal        = (int16_t)_wtoi(xml.GetData().c_str());
            }
            break;

        case GSEDT_INT24:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_INT24;
                m_stGooseSendValue.m_stInt24.datVal        = (int32_t)_wtoi(xml.GetData().c_str());
            }
            break;

        case GSEDT_INT32:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_INT32;
                m_stGooseSendValue.m_stInt32.datVal        = (int32_t)_wtoi(xml.GetData().c_str());
            }
            break;

        case GSEDT_UINT8:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_UINT8;
                m_stGooseSendValue.m_stUInt8.datVal        = (uint8_t)_wtoi(xml.GetData().c_str());
            }
            break;

        case GSEDT_UINT16:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_UINT16;
                m_stGooseSendValue.m_stUInt16.datVal       = (uint16_t)_wtoi(xml.GetData().c_str());
            }
            break;

        case GSEDT_UINT24:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_UINT24;
                m_stGooseSendValue.m_stUInt24.datVal       = _wtoi(xml.GetData().c_str());
            }
            break;

        case GSEDT_UINT32:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_UINT32;
                m_stGooseSendValue.m_stUInt32.datVal       = _wtol(xml.GetData().c_str());
            }
            break;

        case GSEDT_FLOAT:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_FLOAT;
                swscanf_s(
                    xml.GetData().c_str(), 
                    L"%f", 
                    &(m_stGooseSendValue.m_stFloat.datVal), 
                    sizeof(m_stGooseSendValue.m_stFloat.datVal)
                    );
            }
            break;

        case GSEDT_OCTET_STR:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_OCTET_STR;
            }
            break;

        case GSEDT_VISIBLE_STR:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_VISIBLE_STR;
            }
            break;

        case GSEDT_UTC_TIME:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_UTC_TIME;
                m_stGooseSendValue.m_stUtcTime.SetUctTime(xml.GetData());
            }
            break;

        default:
            {
                m_stGooseSendValue.m_stUnKnown.id.type     = GSEDT_UNKNOW;
            }
            break;
        }
    }

    // <VirtualChannel>
    if (xml.FindElem(NODE_Channel_VirtualChannel))
    {
        m_strVirtualChannel   = xml.GetData();
    }

    // <SubChannel>
    if (xml.FindElem(NODE_Channel_SubChannel) && xml.IntoElem())
    {
        //
        // find <Channel>
        //
        while(xml.FindElem(NODE_Channel))
        {
            CChannel*   pSubChannel = new CChannel(GetParentBaseCb(), this, m_pParamConfigBase);
            if (NULL == pSubChannel)
            {
                return false;
            }

            // 设置通道选中状态
            int iSelected = _wtoi(xml.GetAttrib(L"selected").c_str());
            if (iSelected != 0)
            {
                pSubChannel->m_bSelected   = true;
            }

            // inside <Channel>
            if (!xml.IntoElem())
            {
                delete pSubChannel;
                return false;
            }

            // 加载通道
            if (!pSubChannel->__LoadGooseConfig(xml))
            {
                delete pSubChannel;
                return false;
            }

            // outside <Channel>
            if (!xml.OutOfElem())
            {
                delete pSubChannel;
                return false;
            }

            // 添加GOOSE控制块对象指针到容器
            if (!__AddSubChannel(pSubChannel))
            {
                delete pSubChannel;
                return false;
            }
        }

        // quit <SubChannel>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    保存GOOSE<Channel>
///
/// @param[in]  pXml 指向XML对象的引用
///
/// @return
///     bool    true表示加载成功
///
bool CChannel::__SaveGooseConfig( CMarkup& xml )
{
    // <Description>
    if (!xml.AddElem(NODE_Channel_Description) || !xml.SetData(GetDescription()))
    {
        return false;
    }

    // <Type>
    if (!xml.AddElem(NODE_Channel_Type) || !xml.SetData(GetChannelType()))
    {
        return false;
    }

    // <Value>
    GOOSE_DATA_TYPE eValueType  = m_stGooseSendValue.GetType();
    if (GSEDT_UNKNOW != eValueType)
    {
        if (!xml.AddElem(NODE_Channel_Value))
        {
            return false;
        }

        // <Value type="">
        if (!xml.SetAttrib(L"type", eValueType))
        {
            return false;
        }

        // 值
        switch(eValueType)
        {
        case GSEDT_BOOLEAN:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stUnKnown.id.type ? 1: 0))
                {
                    return false;
                }
            }
            break;

        case GSEDT_BIT_STR:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stBitString.GetString().c_str()))
                {
                    return false;
                }
            }
            break;

        case GSEDT_INT8:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stInt8.datVal))
                {
                    return false;
                }
            }
            break;

        case GSEDT_INT16:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stInt16.datVal))
                {
                    return false;
                }
            }
            break;

        case GSEDT_INT24:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stInt24.datVal))
                {
                    return false;
                }
            }
            break;

        case GSEDT_INT32:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stInt32.datVal))
                {
                    return false;
                }
            }
            break;

        case GSEDT_UINT8:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stUInt8.datVal))
                {
                    return false;
                }
            }
            break;

        case GSEDT_UINT16:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stUInt16.datVal))
                {
                    return false;
                }
            }
            break;

        case GSEDT_UINT24:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stUInt24.datVal))
                {
                    return false;
                }
            }
            break;

        case GSEDT_UINT32:
            {
                wchar_t     cBuffer[128]    = {0};

                _ltow_s(
                    m_stGooseSendValue.m_stUInt32.datVal,
                    cBuffer,
                    sizeof(cBuffer)/sizeof(cBuffer[0]),
                    10
                    );
                if (!xml.SetData(cBuffer))
                {
                    return false;
                }
            }
            break;

        case GSEDT_FLOAT:
            {
                wchar_t     cBuffer[128]    = {0};


                swprintf_s(
                    cBuffer, 
                    sizeof(cBuffer)/sizeof(cBuffer[0]), 
                    L"%f", 
                    m_stGooseSendValue.m_stFloat.datVal
                    );
                if (!xml.SetData(cBuffer))
                {
                    return false;
                }
            }
            break;

        case GSEDT_OCTET_STR:
            {

            }
            break;

        case GSEDT_VISIBLE_STR:
            {

            }
            break;

        case GSEDT_UTC_TIME:
            {
                if (!xml.SetData(m_stGooseSendValue.m_stUtcTime.GetUtcString().c_str()))
                {
                    return false;
                }
            }
            break;

        default:
            {

            }
            break;
        }
    }

    // <VirtualChannel>
    if (!xml.AddElem(NODE_Channel_VirtualChannel) || !xml.SetData(GetVirtualChannel()))
    {
        return false;
    }

    //
    // 遍历容器，保存所有子通道
    //
    if (GetSubChannelCount() > 0)
    {
        // <SubChannel>
        if (!xml.AddElem(NODE_Channel_SubChannel) || !xml.IntoElem())
        {
            return false;
        }

        // 单个通道
        CChannel*   pSubChannel    = FirstSubChannel();
        while(pSubChannel)
        {
            // <Channel>
            if (!xml.AddElem(NODE_Channel))
            {
                return false;
            }

            // <Channel selected="">
            int iSelected = pSubChannel->IsSelected()? 1: 0;
            if (!xml.SetAttrib(L"selected", iSelected))
            {
                return false;
            }

            // inside <Channel>
            if (!xml.IntoElem())
            {
                return false;
            }

            // 保存Channel
            if (!pSubChannel->__SaveGooseConfig(xml))
            {
                return false;
            }

            // outside <Channel>
            if (!xml.OutOfElem())
            {
                return false;
            }

            pSubChannel = NextSubChannel(pSubChannel);
        }

        // Quit <SubChannel>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    修改GOOSE控制块中单点/双点类型通道时，需要自动修改相邻/间隔的时间通道的值
///    此函数自动寻找寻找变位的单点/双点通道，然后改变相邻或间隔的时间通道的值
///
/// @param[in]  stNewTime    单点/双点通道的变位时间
///
void CChannel::__UpdateSubChannelTime( CEventTime stNewTime )
{
    CChannel*                       pSrcChannel         = NULL;
    CChannel*                       pSubChannel         = NULL;
    CChannel*                       pNextSubChannel     = NULL;
    CChannel*                       pThirdSubChannel    = NULL;
    std::list<CChannel*>::iterator  iter;


    // 遍历控制块的所有通道
    iter = m_listSubChannelGroup.begin();
    while (m_listSubChannelGroup.end() != iter)
    {
        pSubChannel = *iter;

        // 查找变位的通道
        if (pSubChannel && pSubChannel->IsSendValueChanged())
        {
            // 如果变位的是单点、双点类型的通道
            if (CHANNEL_TYPE_GOOSE_POINT == pSubChannel->GetChannelType() ||
                CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pSubChannel->GetChannelType())
            {
                // 如果相邻的通道是时间类型
                pNextSubChannel = NextSubChannel(pSubChannel);
                if (pNextSubChannel && CHANNEL_TYPE_GOOSE_TIME == pNextSubChannel->GetChannelType())
                {
                    // 更新相邻的时间通道的值
                    GOOSE_DATA_VARIANT& stChannelValue = pNextSubChannel->GetGooseSendValue();
                    stChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                    stChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;

                    // 更新源时间通道的值
                    pSrcChannel = pNextSubChannel->m_pSourceChannel;
                    if (pSrcChannel)
                    {
                        GOOSE_DATA_VARIANT& stSrcChannelValue = pSrcChannel->GetGooseSendValue();
                        stSrcChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                        stSrcChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;
                    }
                }
                // 如果相邻的通道是品质类型
                else if (pNextSubChannel && CHANNEL_TYPE_GOOSE_QUALITY == pNextSubChannel->GetChannelType())
                {
                    // 判断间隔的通道是否是时间类型
                    pThirdSubChannel = NextSubChannel(pNextSubChannel);
                    if (pThirdSubChannel && CHANNEL_TYPE_GOOSE_TIME == pThirdSubChannel->GetChannelType())
                    {
                        // 更新间隔的时间通道的值
                        GOOSE_DATA_VARIANT& stChannelValue = pThirdSubChannel->GetGooseSendValue();
                        stChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                        stChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;

                        // 更新源时间通道的值
                        pSrcChannel = pThirdSubChannel->m_pSourceChannel;
                        if (pSrcChannel)
                        {
                            GOOSE_DATA_VARIANT& stSrcChannelValue = pSrcChannel->GetGooseSendValue();
                            stSrcChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                            stSrcChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;
                        }
                    }
                }
            }
            // 如果变位的是结构类型的通道
            else if (CHANNEL_TYPE_GOOSE_STRUCT == pSubChannel->GetChannelType())
            {
                // 更新子通道的时间
                pSubChannel->__UpdateSubChannelTime(stNewTime);
            }
        }

        iter++;
    }
}

///
/// @brief
///     对发生了变位的子通道进行重新编码
///
/// @param[in]  pucFrameHead    指向发送帧帧头的指针
///
/// @return
///     bool    成功返回true
///
bool CChannel::__RecodeChangedSubChannels( unsigned char* pucFrameHead )
{
    CChannel*       pSubChannel = NULL;
    std::list<CChannel*>::iterator  iter;


    // 遍历控制块的所有通道
    for (iter = m_listSubChannelGroup.begin(); iter != m_listSubChannelGroup.end(); iter++)
    {
        pSubChannel = *iter;
        if (pSubChannel && pSubChannel->IsSendValueChanged())
        {
            // 如果是单点类型通道
            if (CHANNEL_TYPE_GOOSE_POINT == pSubChannel->GetChannelType())
            {
                // 获得通道发送值
                GOOSE_DATA_VARIANT& stSendValue = pSubChannel->GetGooseSendValue();
                if (GSEDT_BOOLEAN != stSendValue.m_stUnKnown.id.type)
                {
                    return false;
                }

                // 修改缓冲区中单点通道的报文值
                stSendValue.CodeGooseData(pucFrameHead + stSendValue.m_stUtcTime.id.offset, 3);
            }
            // 如果是双点类型通道
            else if (CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pSubChannel->GetChannelType())
            {
                // 获得通道发送值
                GOOSE_DATA_VARIANT& stSendValue = pSubChannel->GetGooseSendValue();
                if (GSEDT_BIT_STR != stSendValue.m_stUnKnown.id.type)
                {
                    return false;
                }

                // 修改缓冲区中双点通道的报文值
                stSendValue.CodeGooseData(pucFrameHead + stSendValue.m_stUtcTime.id.offset, 4);
            }
            // 如果是结构体类型通道
            else if (CHANNEL_TYPE_GOOSE_STRUCT == pSubChannel->GetChannelType())
            {
                if (!pSubChannel->__RecodeChangedSubChannels(pucFrameHead))
                {
                    return false;
                }
            }

            // 复位变位标识
            pSubChannel->SetSendValueChanged(false);
        }
    }

    return true;
}

///
/// @brief
///     查找所有需要变位的时间子通道，将子通道指针保存到容器中
///
/// @param[out]  listShouldChangeTimeChannel    保存需要变位时间通道的指针
///
void CChannel::__SaveSubTimeChannel( std::list<CChannel*>& listShouldChangeTimeChannel )
{
    CChannel*       pSubChannel     = NULL;
    CChannel*       pNextSubChannel = NULL;
    CChannel*       pThirdSubChannel= NULL;
    std::list<CChannel*>::iterator  iter;


    // 遍历控制块的所有子通道
    iter = m_listSubChannelGroup.begin();
    while (m_listSubChannelGroup.end() != iter)
    {
        pSubChannel = *iter;

        // 查找变位的通道
        if (pSubChannel && pSubChannel->IsSendValueChanged())
        {
            // 如果变位的是单点、双点类型的通道
            if (CHANNEL_TYPE_GOOSE_POINT == pSubChannel->GetChannelType() ||
                CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pSubChannel->GetChannelType())
            {
                // 如果相邻的通道是时间类型
                pNextSubChannel = NextSubChannel(pSubChannel);
                if (pNextSubChannel && CHANNEL_TYPE_GOOSE_TIME == pNextSubChannel->GetChannelType())
                {
                    // 记录需要变位的时间通道
                    listShouldChangeTimeChannel.push_back(pNextSubChannel);
                }
                // 如果相邻的通道是品质类型
                else if (pNextSubChannel && CHANNEL_TYPE_GOOSE_QUALITY == pNextSubChannel->GetChannelType())
                {
                    // 判断间隔的通道是否是时间类型
                    pThirdSubChannel = NextSubChannel(pNextSubChannel);
                    if (pThirdSubChannel && CHANNEL_TYPE_GOOSE_TIME == pThirdSubChannel->GetChannelType())
                    {
                        // 记录需要变位的时间通道
                        listShouldChangeTimeChannel.push_back(pThirdSubChannel);
                    }
                }
            }
            // 如果变位的是结构类型的通道
            else if (CHANNEL_TYPE_GOOSE_STRUCT == pSubChannel->GetChannelType())
            {
                // 查找所有需要变位的时间子通道
                pSubChannel->__SaveSubTimeChannel(listShouldChangeTimeChannel);
            }
        }

        iter++;
    }
}

///
/// @brief
///    计算通道的偏移量
///
/// @param[in]  pGooseData      保存GOOSE通道信息的结构体
///
/// @return
///     bool    成功返回true
///
bool CChannel::__CalcChannelOffset( GOOSE_DATA* pGooseData )
{
    unsigned int    nIndex          = 0;
    GOOSE_STRUCT*   pGooseStruct    = NULL;


    // 检查输入参数
    if (NULL == pGooseData)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CChannel::__CalcChannelOffset(): Parameter error \n"));
        return false;
    }

    // 如果是结构体通道，那么保存每一个子通道的偏移量
    if (CHANNEL_TYPE_GOOSE_STRUCT == m_eChannelType)
    {
        // 转换为结构体类型的指针
        pGooseStruct = (GOOSE_STRUCT*)pGooseData;

        // 遍历每一个子通道
        std::list<CChannel *>::iterator iterChannel = m_listSubChannelGroup.begin();
        for (nIndex = 0; nIndex < pGooseStruct->member.size() && iterChannel != m_listSubChannelGroup.end(); nIndex++, iterChannel++)
        {
            CChannel* pSubChannel = *iterChannel;
            if (NULL == pSubChannel)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CChannel::__CalcChannelOffset(): get sub channel failed \n"));
                return false;
            }

            // 获得子通道的偏移量
            GOOSE_DATA* pSubGooseData = pGooseStruct->member.at(nIndex);
            if (NULL == pSubGooseData)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CChannel::__CalcChannelOffset(): get sub channel info failed \n"));
                return false;
            }

            // 检查子通道类型
            if ((GSEDT_STRUCT == pSubGooseData->type) || (CHANNEL_TYPE_GOOSE_STRUCT == pSubChannel->GetChannelType()))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CChannel::__CalcChannelOffset(): sub channel type error \n"));
                return false;
            }

            // 保存通道偏移量到GOOSE发送值结构体中
            pSubChannel->m_stGooseSendValue.m_stUnKnown.id.offset = pSubGooseData->offset;

            // 修改源通道的偏移量
            if (pSubChannel->m_pSourceChannel)
            {
                pSubChannel->m_pSourceChannel->m_stGooseSendValue.m_stUnKnown.id.offset = pSubGooseData->offset;
            }
        }
    }
    // 如果是普通通道，那么保存通道的偏移量
    else
    {
        // 保存通道偏移量到GOOSE发送值结构体中
        m_stGooseSendValue.m_stUnKnown.id.offset = pGooseData->offset;

        // 修改源通道的偏移量
        if (m_pSourceChannel)
        {
            m_pSourceChannel->m_stGooseSendValue.m_stUnKnown.id.offset = pGooseData->offset;
        }
    }

    return true;
}
