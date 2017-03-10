/// @file
///
/// @brief
///     采样值接收配置模块
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.13
///
/// 修改历史：
///

#include "StdAfx.h"
#include "SmvRecvConfig.h"
#include "src\utils\config\dsm_config.h"
#include "src\service\datalayer\DsmDataLayer.h"
#include "src\service\datalayer\include\datatypes.h"
#include "src\service\datalayer\utility\data_utility.h"

//定义编译警告todo信息
//#pragma message(__FILE__ "(" STRING(__LINE__) "): 考虑Uninit时是否要先停止网络接口数据接收")
#define STRING2(x) #x
#define STRING(x) STRING2(x)

CSmvRecvConfig::CSmvRecvConfig(CBaseDetectItem* pDetectItem)
                               : m_pCurrentCtrlBlk(pDetectItem)
{
    memset(m_nChannelTypeCurrent, 0xFF, sizeof(m_nChannelTypeCurrent));
    memset(m_nChannelTypeVoltage, 0xFF, sizeof(m_nChannelTypeVoltage));
    memset(m_nPhaseCheckABCChannels, 0xFF, sizeof(m_nPhaseCheckABCChannels));
}

CSmvRecvConfig::~CSmvRecvConfig(void)
{
    _ClearAllContainer();
}


///
/// @brief
///    初始化m_vecChannelInfo/m_mapHarmonicType容器
///
void CSmvRecvConfig::init()
{
    if (!m_pCurrentCtrlBlk || m_pCurrentCtrlBlk->enFrameType == FRAME_TYPE_NET_GOOSE)
    {
        return;
    }

    //防止用户多次调用init造成容器数据重复的问题，先clear所有容器
    _ClearAllContainer();

    CSCLManager* pSclManager = CSCLManager::getInstance();

    CSmvRecvChannelInfo* pSmvRecvChannelInfo = NULL;

    float fPrimaryAmp = 0, fSecondaryAmp = 0;
    float fPrimaryVol = 0, fSecondaryVol = 0;
    uint32_t nRatedMuDelay = 0;
    CParamConfigBasic* pParamConfigBasic = CDsmDataLayer::getInstance()->GetParamConfigBasic();
    if (pParamConfigBasic)
    {
        double dfPrimaryAmp   = pParamConfigBasic->GetRatedPrimaryCurrent();
        double dfSecondaryAmp = pParamConfigBasic->GetRatedSecondaryCurrent();
        double dfPrimaryVol   = pParamConfigBasic->GetRatedPrimaryVoltage();
        double dfSecondaryVol = pParamConfigBasic->GetRatedSecondaryVoltage();

        std::wstring strMuDelay = pParamConfigBasic->GetMuRatedDelay();
        StrUint32Convert(strMuDelay, NULL, &nRatedMuDelay);

        //设置中保存的数据是按照kv/a为单位的
        fPrimaryAmp = static_cast<float>(dfPrimaryAmp);
        fPrimaryVol = static_cast<float>(1000 * dfPrimaryVol);
        fSecondaryAmp = static_cast<float>(dfSecondaryAmp);
        fSecondaryVol = static_cast<float>(dfSecondaryVol);
    }

    switch(m_pCurrentCtrlBlk->enFrameType)
    {
        //如果是9-2数据，先判断从配置文件读到的通道数目和网络探测到底通道数目是否相符
        //相符的话采用scl中描述信息，不相符的话，使用预定义信息
    case FRAME_TYPE_NET_9_2:
        {
            CNetDetectItem* pNetDetectItem = dynamic_cast<CNetDetectItem*>(m_pCurrentCtrlBlk);
            CIEC9_2Cb* pSmv92Cb = pSclManager->GetIEC9_2Cb(pNetDetectItem->appId);
            if (pSmv92Cb)
            {
                CChannel* pChannel = NULL;
                for (uint32_t i = 0; i < pNetDetectItem->nChannelCount; ++i)
                {
                    pSmvRecvChannelInfo = new CSmvRecvChannelInfo;
                    pSmvRecvChannelInfo->nIndex = i;
                    pSmvRecvChannelInfo->enChannelValueType = CHANNEL_VALUE_TYPE_UNKNOWN;
                    pSmvRecvChannelInfo->pOwnerCtrlBlk = m_pCurrentCtrlBlk;

                    pChannel = pSmv92Cb->GetChannelByIndex(i);
                    if (pChannel)
                    {
                        pSmvRecvChannelInfo->enChannelPhase = pChannel->GetChannelPhase();
                        pSmvRecvChannelInfo->enChannelType = pChannel->GetChannelType();
                        pSmvRecvChannelInfo->strChannelDesc = pChannel->GetDescription();
                        if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_CURRENT)
                        {
                            pSmvRecvChannelInfo->fPrimaryRatedValue = fPrimaryAmp;
                            pSmvRecvChannelInfo->fSecondaryRatedValue = fSecondaryAmp;
                        }
                        else if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
                        {
                            pSmvRecvChannelInfo->fPrimaryRatedValue = fPrimaryVol;
                            pSmvRecvChannelInfo->fSecondaryRatedValue = fSecondaryVol;
                        }
                        else if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_TIME)
                        {
                            pSmvRecvChannelInfo->fPrimaryRatedValue = static_cast<float>(nRatedMuDelay);
                            pSmvRecvChannelInfo->fSecondaryRatedValue = static_cast<float>(nRatedMuDelay);
                        }
                    }
                    else
                    {
                        pSmvRecvChannelInfo->enChannelPhase = CHANNEL_PHASE_N;
                        pSmvRecvChannelInfo->enChannelType = CHANNEL_TYPE_SMV_VOLTAGE;
                        pSmvRecvChannelInfo->fPrimaryRatedValue = fPrimaryVol;
                        pSmvRecvChannelInfo->fSecondaryRatedValue = fSecondaryVol;
                        pSmvRecvChannelInfo->strChannelDesc = L"";
                    }
                    m_vecItemChannelInfo.push_back(pSmvRecvChannelInfo);
                }
            }
            else
            {
                uint32_t nSizePredef9_2Channels = sizeof(PREDEFINED_CHANNEL_INFO_9_2LE)/sizeof(PREDEFINED_CHANNEL_INFO);
                uint32_t nSizePredef9_2ChannelsLess = sizeof(PREDEFINED_CHANNEL_INFO_9_2LE_LESS)/sizeof(PREDEFINED_CHANNEL_INFO);
                if (pNetDetectItem->nChannelCount <= nSizePredef9_2ChannelsLess)
                {
                    for (size_t i = 0; i < pNetDetectItem->nChannelCount; ++i)
                    {
                        pSmvRecvChannelInfo = new CSmvRecvChannelInfo;
                        pSmvRecvChannelInfo->nIndex = i;
                        pSmvRecvChannelInfo->pOwnerCtrlBlk = m_pCurrentCtrlBlk;
                        pSmvRecvChannelInfo->enChannelValueType = PREDEFINED_CHANNEL_INFO_9_2LE_LESS[i].enChannelValueType;
                        pSmvRecvChannelInfo->enChannelPhase = PREDEFINED_CHANNEL_INFO_9_2LE_LESS[i].enChannelPhase;
                        pSmvRecvChannelInfo->enChannelType  = PREDEFINED_CHANNEL_INFO_9_2LE_LESS[i].enChannelType;
                        pSmvRecvChannelInfo->strChannelDesc = PREDEFINED_CHANNEL_INFO_9_2LE_LESS[i].strChannelDesc;

                        if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_CURRENT)
                        {
                            pSmvRecvChannelInfo->fPrimaryRatedValue = fPrimaryAmp;
                            pSmvRecvChannelInfo->fSecondaryRatedValue = fSecondaryAmp;
                        }
                        else if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
                        {
                            pSmvRecvChannelInfo->fPrimaryRatedValue = fPrimaryVol;
                            pSmvRecvChannelInfo->fSecondaryRatedValue = fSecondaryVol;
                        }
                        else if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_TIME)
                        {
                            pSmvRecvChannelInfo->fPrimaryRatedValue = static_cast<float>(nRatedMuDelay);
                            pSmvRecvChannelInfo->fSecondaryRatedValue = static_cast<float>(nRatedMuDelay);
                        }
                        m_vecItemChannelInfo.push_back(pSmvRecvChannelInfo);
                    }
                }
                else
                {
                    for (size_t i = 0; i < pNetDetectItem->nChannelCount; ++i)
                    {
                        pSmvRecvChannelInfo = new CSmvRecvChannelInfo;
                        pSmvRecvChannelInfo->nIndex = i;
                        pSmvRecvChannelInfo->pOwnerCtrlBlk = m_pCurrentCtrlBlk;
                        if (i < nSizePredef9_2Channels)
                        {
                            pSmvRecvChannelInfo->enChannelValueType = PREDEFINED_CHANNEL_INFO_9_2LE[i].enChannelValueType;
                            pSmvRecvChannelInfo->enChannelPhase = PREDEFINED_CHANNEL_INFO_9_2LE[i].enChannelPhase;
                            pSmvRecvChannelInfo->enChannelType  = PREDEFINED_CHANNEL_INFO_9_2LE[i].enChannelType;
                            pSmvRecvChannelInfo->strChannelDesc = PREDEFINED_CHANNEL_INFO_9_2LE[i].strChannelDesc;

                            if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_CURRENT)
                            {
                                pSmvRecvChannelInfo->fPrimaryRatedValue = fPrimaryAmp;
                                pSmvRecvChannelInfo->fSecondaryRatedValue = fSecondaryAmp;
                            }
                            else if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
                            {
                                pSmvRecvChannelInfo->fPrimaryRatedValue = fPrimaryVol;
                                pSmvRecvChannelInfo->fSecondaryRatedValue = fSecondaryVol;
                            }
                            else if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_TIME)
                            {
                                pSmvRecvChannelInfo->fPrimaryRatedValue = static_cast<float>(nRatedMuDelay);
                                pSmvRecvChannelInfo->fSecondaryRatedValue = static_cast<float>(nRatedMuDelay);
                            }
                        }
                        else
                        {
                            pSmvRecvChannelInfo->enChannelValueType = CHANNEL_VALUE_TYPE_UNKNOWN;
                            pSmvRecvChannelInfo->enChannelPhase = CHANNEL_PHASE_X;
                            pSmvRecvChannelInfo->enChannelType  = CHANNEL_TYPE_SMV_VOLTAGE;
                            pSmvRecvChannelInfo->fPrimaryRatedValue = fPrimaryVol;
                            pSmvRecvChannelInfo->fSecondaryRatedValue = fSecondaryVol;
                            pSmvRecvChannelInfo->strChannelDesc = L"";
                        }
                        m_vecItemChannelInfo.push_back(pSmvRecvChannelInfo);
                    }
                }
            }
        }
        break;
        //如果是国网FT3，通道数目采用报文中获取的数目
    case FRAME_TYPE_FT3_GUOWANG:
        {
            uint32_t nChannelCount = m_pCurrentCtrlBlk->nChannelCount;
            uint32_t nChannelCountPreDef = sizeof(PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG)/sizeof(PREDEFINED_CHANNEL_INFO);
            for (size_t i = 0; i < nChannelCount ; ++i)
            {
                pSmvRecvChannelInfo = new CSmvRecvChannelInfo;
                pSmvRecvChannelInfo->nIndex = i;
                pSmvRecvChannelInfo->pOwnerCtrlBlk = m_pCurrentCtrlBlk;
                if (i < nChannelCountPreDef)
                {
                    pSmvRecvChannelInfo->enChannelValueType = PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].enChannelValueType;
                    pSmvRecvChannelInfo->enChannelPhase = PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].enChannelPhase;
                    pSmvRecvChannelInfo->enChannelType  = PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].enChannelType;
                    pSmvRecvChannelInfo->strChannelDesc = PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].strChannelDesc;

                    if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_CURRENT)
                    {
                        pSmvRecvChannelInfo->fPrimaryRatedValue = fPrimaryAmp;
                        pSmvRecvChannelInfo->fSecondaryRatedValue = fSecondaryAmp;
                    }
                    else if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
                    {
                        pSmvRecvChannelInfo->fPrimaryRatedValue = fPrimaryVol;
                        pSmvRecvChannelInfo->fSecondaryRatedValue = fSecondaryVol;
                    }
                }
                m_vecItemChannelInfo.push_back(pSmvRecvChannelInfo);
            }
        }
        break;
        //如果是9-1通道数目采用报文中获取的数目
        //通道描述直接采用9-1底固定通道描述信息,9-1通道信息与南瑞通道预定义信息相同
    case FRAME_TYPE_NET_9_1:
        //如果是南瑞FT3，通道数目采用报文中获取的数目
    case FRAME_TYPE_FT3_NANRUI:
        {
            uint32_t nChannelCount = m_pCurrentCtrlBlk->nChannelCount;
            uint32_t nChannelCountPreDef = sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8)/sizeof(PREDEFINED_CHANNEL_INFO);
            for (size_t i = 0; i < nChannelCount ; ++i)
            {
                pSmvRecvChannelInfo = new CSmvRecvChannelInfo;
                pSmvRecvChannelInfo->nIndex = i;
                pSmvRecvChannelInfo->pOwnerCtrlBlk = m_pCurrentCtrlBlk;
                if (i < nChannelCountPreDef)
                {
                    pSmvRecvChannelInfo->enChannelValueType = PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelValueType;
                    pSmvRecvChannelInfo->enChannelPhase = PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelPhase;
                    pSmvRecvChannelInfo->enChannelType  = PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelType;
                    pSmvRecvChannelInfo->strChannelDesc = PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].strChannelDesc;

                    if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_CURRENT)
                    {
                        pSmvRecvChannelInfo->fPrimaryRatedValue = fPrimaryAmp;
                        pSmvRecvChannelInfo->fSecondaryRatedValue = fSecondaryAmp;
                    }
                    else if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
                    {
                        pSmvRecvChannelInfo->fPrimaryRatedValue = fPrimaryVol;
                        pSmvRecvChannelInfo->fSecondaryRatedValue = fSecondaryVol;
                    }
                }
                m_vecItemChannelInfo.push_back(pSmvRecvChannelInfo);
            }
        }
        break;
    default:
        break;
    }
}

///
/// @brief
///    获取控制块通道数目
///
/// @return
///     size_t 返回控制块通道数目
///
size_t CSmvRecvConfig::GetChannelCount()
{
    return m_vecItemChannelInfo.size();
}

///
/// @brief
///    获取某个索引对应的通道信息
///
/// @param[in]  nIndex  要获取的通道的索引
///
/// @return
///     CSmvRecvChannelInfo* 返回某一通道的信息指针
///
CSmvRecvChannelInfo* CSmvRecvConfig::GetOneChannelInfo( uint32_t nIndex)
{
    if (nIndex < m_vecItemChannelInfo.size())
    {
        if (nIndex == m_vecItemChannelInfo[nIndex]->nIndex)
        {
            return m_vecItemChannelInfo[nIndex];
        }
    }
    return NULL;
}

///
/// @brief
///    设置某个索引对应的通道信息
///
/// @param[in]  nIndex  要设置的通道的索引
/// @param[in] pSmvRecvChannelInfo 将此指针指向对象的信息设置到对应索引的通道中
///
/// @return
///     bool 设置成功返回true
///
bool CSmvRecvConfig::SetOneChannelInfo( uint32_t nIndex, CSmvRecvChannelInfo* pSmvRecvChannelInfo )
{
    if (nIndex < m_vecItemChannelInfo.size())
    {
        if (nIndex == m_vecItemChannelInfo[nIndex]->nIndex)
        {
//            m_vecItemChannelInfo[nIndex]->nIndex               = nIndex;
            m_vecItemChannelInfo[nIndex]->pOwnerCtrlBlk        = m_pCurrentCtrlBlk;
            m_vecItemChannelInfo[nIndex]->enChannelPhase       = pSmvRecvChannelInfo->enChannelPhase;
            m_vecItemChannelInfo[nIndex]->enChannelType        = pSmvRecvChannelInfo->enChannelType;
            m_vecItemChannelInfo[nIndex]->enChannelValueType   = pSmvRecvChannelInfo->enChannelValueType;
            m_vecItemChannelInfo[nIndex]->fPrimaryRatedValue   = pSmvRecvChannelInfo->fPrimaryRatedValue;
            m_vecItemChannelInfo[nIndex]->fSecondaryRatedValue = pSmvRecvChannelInfo->fSecondaryRatedValue;
            m_vecItemChannelInfo[nIndex]->strChannelDesc       = pSmvRecvChannelInfo->strChannelDesc;
            return true;
        }
    }
    return false;
}

///
/// @brief
///    清空所有容器信息
///
void CSmvRecvConfig::_ClearAllContainer()
{
        VecChannelInfo::iterator itrVec = m_vecItemChannelInfo.begin();
        for (; itrVec != m_vecItemChannelInfo.end(); ++itrVec)
        {
            if (NULL != (*itrVec))
            {
                delete (*itrVec);
                (*itrVec) = NULL;
            }
        }
        m_vecItemChannelInfo.clear();
}

///
/// @brief
///    设置发送给DSP使用的电压相别信息
///
bool CSmvRecvConfig::SetDspSendVolInfo( uint32_t nVolPhaseA, uint32_t nVolPhaseB, uint32_t nVolPhaseC )
{
    m_nChannelTypeVoltage[0] = nVolPhaseA;
    m_nChannelTypeVoltage[1] = nVolPhaseB;
    m_nChannelTypeVoltage[2] = nVolPhaseC;
    return true;
}

///
/// @brief
///    设置发送给DSP使用的电流相别信息
///
bool CSmvRecvConfig::SetDspSendCurInfo( uint32_t nCurPhaseA, uint32_t nCurPhaseB, uint32_t nCurPhaseC )
{
    m_nChannelTypeCurrent[0] = nCurPhaseA;
    m_nChannelTypeCurrent[1] = nCurPhaseB;
    m_nChannelTypeCurrent[2] = nCurPhaseC;
    return true;
}

///
/// @brief
///    获取当前发送给DSP使用的电压相别信息
///
bool CSmvRecvConfig::GetDspSendVolInfo( uint32_t& nVolPhaseA, uint32_t& nVolPhaseB, uint32_t& nVolPhaseC )
{
    nVolPhaseA = m_nChannelTypeVoltage[0];
    nVolPhaseB = m_nChannelTypeVoltage[1];
    nVolPhaseC = m_nChannelTypeVoltage[2];
    return true;
}

///
/// @brief
///    获取当前发送给DSP使用的电流相别信息
///
bool CSmvRecvConfig::GetDspSendCurInfo( uint32_t& nCurPhaseA, uint32_t& nCurPhaseB, uint32_t& nCurPhaseC )
{
    nCurPhaseA = m_nChannelTypeCurrent[0];
    nCurPhaseB = m_nChannelTypeCurrent[1];
    nCurPhaseC = m_nChannelTypeCurrent[2];
    return true;
}

///
/// @brief
///    设置核相smv1三相
///
bool CSmvRecvConfig::SetPhaseCheckSmv1Channels( uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC )
{
    m_nPhaseCheckABCChannels[0] = nPhaseA;
    m_nPhaseCheckABCChannels[1] = nPhaseB;
    m_nPhaseCheckABCChannels[2] = nPhaseC;
    return true;
}

///
/// @brief
///    获取核相svm1三相
///
bool CSmvRecvConfig::GetPhaseCheckSmv1Channels( uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC )
{
    nPhaseA = m_nPhaseCheckABCChannels[0];
    nPhaseB = m_nPhaseCheckABCChannels[1];
    nPhaseC = m_nPhaseCheckABCChannels[2];
    return true;
}

///
/// @brief
///    设置核相smv2三相
///
bool CSmvRecvConfig::SetPhaseCheckSmv2Channels( uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC )
{
    m_nPhaseCheckABCChannels[3] = nPhaseA;
    m_nPhaseCheckABCChannels[4] = nPhaseB;
    m_nPhaseCheckABCChannels[5] = nPhaseC;
    return true;
}

///
/// @brief
///    获取核相svm2三相
///
bool CSmvRecvConfig::GetPhaseCheckSmv2Channels( uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC )
{
    nPhaseA = m_nPhaseCheckABCChannels[3];
    nPhaseB = m_nPhaseCheckABCChannels[4];
    nPhaseC = m_nPhaseCheckABCChannels[5];
    return true;
}

///
/// @brief
///    添加一个通道信息
///
/// @param[in] pSmvRecvChannelInfo 将此指针指向对象的信息设置到对应索引的通道中
///
/// @return
///     bool 添加成功返回true
///
bool CSmvRecvConfig::AddOneChannelInfo( CSmvRecvChannelInfo* pSmvRecvChannelInfo )
{
    if (pSmvRecvChannelInfo)
    {
        CSmvRecvChannelInfo*  pSmvRecvChannelInfoNew = new CSmvRecvChannelInfo;
        pSmvRecvChannelInfoNew->nIndex               = pSmvRecvChannelInfo->nIndex;
        pSmvRecvChannelInfoNew->pOwnerCtrlBlk        = m_pCurrentCtrlBlk;
        pSmvRecvChannelInfoNew->enChannelPhase       = pSmvRecvChannelInfo->enChannelPhase;
        pSmvRecvChannelInfoNew->enChannelType        = pSmvRecvChannelInfo->enChannelType;
        pSmvRecvChannelInfoNew->enChannelValueType   = pSmvRecvChannelInfo->enChannelValueType;
        pSmvRecvChannelInfoNew->fPrimaryRatedValue   = pSmvRecvChannelInfo->fPrimaryRatedValue;
        pSmvRecvChannelInfoNew->fSecondaryRatedValue = pSmvRecvChannelInfo->fSecondaryRatedValue;
        pSmvRecvChannelInfoNew->strChannelDesc       = pSmvRecvChannelInfo->strChannelDesc;
        m_vecItemChannelInfo.push_back(pSmvRecvChannelInfoNew);
        return true;
    }
    return false;
}
