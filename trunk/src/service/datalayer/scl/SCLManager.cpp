/// @file
///
/// @brief
///     SCL配置文件管理，使用src\utils\SclAnalysis模块
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///     chao        2013.4.7
///
/// 修改历史：
///     Modified by:    chao
///     Modified date:  2013.6.1
///     Version:        1.0.1
///     Descriptions:   获取了SCL文件中控制块更详细的信息
///

#include "StdAfx.h"
#include "SCLManager.h"

// 相别信息判断模板
static std::wstring PHASE_A_TEMPLATE[]={L"A相", L"a相", L"Ａ相", L"ａ相"};
static std::wstring PHASE_B_TEMPLATE[]={L"B相", L"b相", L"Ｂ相", L"ｂ相"};
static std::wstring PHASE_C_TEMPLATE[]={L"C相", L"c相", L"Ｃ相", L"ｃ相"};
static std::wstring PHASE_X_TEMPLATE[]={L"X相", L"x相", L"同期", L"母线", L"抽取", L"Ｘ相", L"ｘ相"};
static std::wstring PHASE_N_TEMPLATE[]={L"N相", L"n相", L"中性点", L"Ｎ相", L"ｎ相"};
static std::wstring TIME_92_TEMPLATE[]={L"延时", L"时间"};

///
/// @brief
///    构造函数
///
CSCLManager::CSCLManager(void)
:m_bSclLoaded(false)
{
}

///
/// @brief
///    析构函数
///
CSCLManager::~CSCLManager(void)
{
    UnLoad();
}

///
/// @brief
///    加载scl文件
///
/// @param[in]  strFileName     要加载的scl文件名
///
/// @return
///     true 加载成功
///
bool CSCLManager::Load(std::wstring strFileName )
{
    // 检查输入参数
    if (strFileName.empty())
    {
        return false;
    }

    // 先清空以前缓冲的配置信息
    UnLoad();

    // 对文件进行分析
    if (!__Analysis(strFileName))
    {
        return false;
    }

    // 标记为已成功初始化
    m_bSclLoaded = true;
    return true;
}

///
/// @brief
///    卸载SCL文件
///
void CSCLManager::UnLoad()
{
    // 清空map保留的所有信息
    __ClearAllMap();

    // 标记为未初始化
    m_bSclLoaded = false;
}

///
/// @brief
///    指示SCL管理器是否已成功加载了SCL文件
///
/// @return
///     true 表示SCL管理器已经成功加载了SCL文件
///
bool CSCLManager::IsSclLoaded()
{
    return m_bSclLoaded;
}

///
/// @brief
///    对scl文件进行分析，获取scl中需要的信息
///
/// @param[in]  strFileName     要分析的scl文件名
///
/// @return
///     true 标识scl信息分析获取成功
///
bool CSCLManager::__Analysis(std::wstring strFileName)
{
    bool                                        bRet                = true;


    // 检查输入参数
    if (strFileName.empty())
    {
        return false;
    }

    // 获得SCL文件简化器对象实例
    ISCLCacheManager* pSclCacheMgr = CreateSCLCacheManagerImpl();
    if (NULL == pSclCacheMgr)
    {
        return false;
    }

    // 加载SCL配置文件，并进行相应的解析
    if( -1 == pSclCacheMgr->LoadSCL(strFileName))
    {
        bRet = false;
        goto exitHandle;
    }

    // 加载GOOSE控制块
    if (!__LoadGooseCb(pSclCacheMgr))
    {
        bRet = false;
        goto exitHandle;
    }

    // 加载IEC61850-9-2控制块
    if (!__LoadIEC9_2Cb(pSclCacheMgr))
    {
        bRet = false;
        goto exitHandle;
    }

exitHandle:
    // 卸载打开的SCL配置文件，释放解析缓存
    if (NULL != pSclCacheMgr)
    {
        pSclCacheMgr->UnloadSCL();
        delete pSclCacheMgr;
    }

    return bRet;
}

///
/// @brief
///    加载GOOSE控制块
///
/// @param[in]  pSclCacheMgr scl缓存管理，用于加载卸载scl文件
///
/// @return
///     bool    true表示加载成功
///
bool CSCLManager::__LoadGooseCb( ISCLCacheManager* pSclCacheMgr)
{
    bool                                        bRet                = true;
    ISCDGOOSEAnalysis*                          pGooseAnalysis      = NULL;
    SCDGOOSEANALYSIS_IEDGOOSE_INFO*             pGooseInfo          = NULL;


    // 检查输入参数
    if (NULL == pSclCacheMgr)
    {
        return false;
    }

    // 创建SCD文件GOOSE配置解析接口对象实例
    pGooseAnalysis = CreateSCDGOOSEAnalysisImpl(pSclCacheMgr);
    if (NULL == pGooseAnalysis)
    {
        return false;
    }

    // 获取配置文件中所有IED的已配置通信参数的GOOSE控制块的数量
    int iCount = pGooseAnalysis->GetIEDGOOSENum();
    if (iCount < 0)
    {
        bRet = false;
        goto exitHandle;
    }
    else if (iCount == 0)
    {
        goto exitHandle;
    }

    // 申请存放GOOSE控制块信息的空间
    pGooseInfo  = new SCDGOOSEANALYSIS_IEDGOOSE_INFO[iCount];
    if ((NULL == pGooseInfo))
    {
        bRet = false;
        goto exitHandle;
    }

    // 获得所有GOOSE控制块的信息
    iCount = pGooseAnalysis->GetAllIEDGOOSEInfo(pGooseInfo, iCount);
    if (-1 == iCount)
    {
        goto exitHandle;
    }

    //
    // 将GOOSE控制块的信息存放到m_mapGooseCbMap容器中
    //
    CGooseCb*       pGooseCb    = NULL;
    for (int i = 0; i < iCount; ++i)
    {
        // new一个GOOSE控制块对象
        pGooseCb = new CGooseCb(NULL);
        if (NULL == pGooseCb)
        {
            bRet = false;
            goto exitHandle;
        }

        //
        // 设置控制块属性
        //
        pGooseCb->m_strDescription  = pGooseInfo[i].iedDesc;
        pGooseCb->m_iAPPID          = pGooseInfo[i].gooseAppID;
        memcpy_s(pGooseCb->m_ucDesMAC, ETHERNET_MAC_LEN, pGooseInfo[i].gooseMAC, GOOSEANALYSIS_MAC_SIZE);
        pGooseCb->m_iVLanID         = pGooseInfo[i].gooseVlanID;
        pGooseCb->m_iVLanPriority   = pGooseInfo[i].gooseVlanPri;
        pGooseCb->m_iConfigRevision = pGooseInfo[i].confRev;
        pGooseCb->m_strGocbRef      = (pGooseInfo[i].iedName + pGooseInfo[i].iedLdInst + L"/LLN0$GO$" + pGooseInfo[i].cbName);
        pGooseCb->m_strDataSet      = (pGooseInfo[i].iedName + pGooseInfo[i].iedLdInst + L"/LLN0$" + pGooseInfo[i].dataSet);
        pGooseCb->m_strGoID         = pGooseInfo[i].gooseID;

        // 判断指定APPID的控制块是否已存在
        if (m_mapGooseCbGroup.find(pGooseCb->GetAPPID()) != m_mapGooseCbGroup.end())
        {
            delete pGooseCb;
            continue;
        }

        // 加载控制块的通道
        if (!__LoadGooseChannel(pGooseAnalysis, &pGooseInfo[i], pGooseCb))
        {
            delete pGooseCb;

            bRet = false;
            goto exitHandle;
        }

        // 检查GOOSE控制块通道数
        if (0 == pGooseCb->GetChannelCount())
        {
            delete pGooseCb;
            continue;
        }

        // 将控制块存放到容器中
        std::pair< std::map<uint32_t, CGooseCb*>::iterator, bool > retPair;
        retPair = m_mapGooseCbGroup.insert(std::make_pair(pGooseCb->GetAPPID(), pGooseCb));
        if (!retPair.second)
        {
            delete pGooseCb;

            bRet = false;
            goto exitHandle;
        }
    }

exitHandle:
    // 释放存放GOOSE控制块信息的空间
    if (NULL != pGooseInfo)
    {
        delete[] pGooseInfo;
        pGooseInfo = NULL;
    }

    // 释放SCD文件GOOSE配置解析接口对象实例
    if (NULL != pGooseAnalysis)
    {
        delete pGooseAnalysis;
        pGooseAnalysis = NULL;
    }

    return bRet;
}

///
/// @brief
///    加载GOOSE控制块的通道
///
/// @param[in]  pGooseAnalysis 指向GOOSE配置解析接口对象的指针
/// @param[in]  pGooseInfo 指向GOOSE控制块信息结构体的指针
/// @param[in]  pGooseCb 指向GOOSE控制块对象的指针
///
/// @return
///     bool    true表示加载成功
///
bool CSCLManager::__LoadGooseChannel(ISCDGOOSEAnalysis* pGooseAnalysis, SCDGOOSEANALYSIS_IEDGOOSE_INFO* pGooseInfo, CGooseCb* pGooseCb)
{
    bool                                        bRet                = true;
    SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO*     pChannelInfo        = NULL;


    // 检查输入参数
    if ((NULL == pGooseAnalysis) || (NULL == pGooseCb))
    {
        return false;
    }

    // 获得控制块的MAC地址
    unsigned char cMacBuffer[ETHERNET_MAC_LEN] = {'0'};
    pGooseCb->GetDesMAC(cMacBuffer, ETHERNET_MAC_LEN);

    // 获得指定APPID控制块的通道数
    int iCount = pGooseAnalysis->GetIEDGOOSEDataSetNum(pGooseInfo->iedName, pGooseInfo->iedLdInst, pGooseInfo->cbName);
    if (iCount < 0)
    {
        bRet = false;
        goto exitHandle;
    }
    else if (iCount == 0)
    {
        goto exitHandle;
    }

    // 申请存放GOOSE控制块DataSet信息的空间
    pChannelInfo = new SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO[iCount];
    if (NULL == pChannelInfo)
    {
        bRet = false;
        goto exitHandle;
    }

    // 获得指定APPID控制块的DataSet
    if (-1 == pGooseAnalysis->GetAllIEDGOOSEDataSetInfo(pGooseInfo->iedName, pGooseInfo->iedLdInst, pGooseInfo->cbName, pChannelInfo, iCount))
    {
        goto exitHandle;
    }

    //
    // 将DataSet的信息存放到控制块的通道容器中
    //
    CChannel*   pChannel    = NULL;
    for (int i = 0; i < iCount; ++i)
    {
        pChannel = new CChannel(pGooseCb, NULL, NULL);
        if (NULL == pChannel)
        {
            bRet = false;
            goto exitHandle;
        }

        if (!__LoadGooseSubChannel(pGooseAnalysis, pGooseInfo, pGooseCb, pChannelInfo[i], pChannel))
        {
            delete pChannel;

            bRet = false;
            goto exitHandle;
        }

        //
        // 设置通道属性
        //
        pChannel->SetChannelType(__GetChannelType(pChannelInfo[i]));
        pChannel->m_strDescription  = pChannelInfo[i].doDesc;

        // 添加通道到容器中
        if (!pGooseCb->__AddChannel(pChannel))
        {
            delete pChannel;

            bRet = false;
            goto exitHandle;
        }
    }

exitHandle:
    // 释放存放GOOSE控制块DataSet信息的空间
    if (NULL != pChannelInfo)
    {
        delete[] pChannelInfo;
        pChannelInfo = NULL;
    }

    return bRet;
}

///
/// @brief
///     加载GOOSE控制块的子通道
///
/// @param[in]
///     pGooseAnalysis  -   指向GOOSE配置解析接口对象的指针
/// @param[in]
///     pGooseInfo      -   指向GOOSE控制块信息结构体的指针
/// @param[in]  
///     pGooseCb        -   指向GOOSE控制块对象的指针
/// @param[in]  
///     stChannelInfo   -   IED的GOOSE数据集信息(单个通道)
/// @param[in]  
///     pChannel        -   指向GOOSE通道的指针
///
/// @return
///     bool    true表示加载成功
///
bool CSCLManager::__LoadGooseSubChannel(
                           ISCDGOOSEAnalysis*                          pGooseAnalysis, 
                           SCDGOOSEANALYSIS_IEDGOOSE_INFO*             pGooseInfo, 
                           CGooseCb*                                   pGooseCb, 
                           SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO&     stChannelInfo, 
                           CChannel*                                   pChannel
                           )
{
    bool                                        bRet                = true;
    SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO*     pSubChannelInfo     = NULL;


    // 检查输入参数
    if ((NULL == pGooseAnalysis) || (NULL == pGooseCb))
    {
        return false;
    }

    // 获得控制块的MAC地址
    unsigned char cMacBuffer[ETHERNET_MAC_LEN] = {'0'};
    pGooseCb->GetDesMAC(cMacBuffer, ETHERNET_MAC_LEN);

    // 获得指定通道的子通道数
    int iSubCount = pGooseAnalysis->GetIEDGOOSESubChannelNum(pGooseInfo->iedName, pGooseInfo->iedLdInst, stChannelInfo);
    if(iSubCount < 0)
    {
        bRet = false;
        goto exitHandle;
    }
    else if (iSubCount == 0)
    {
        goto exitHandle;
    }

    // 申请存放子通道信息的空间
    pSubChannelInfo = new SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO[iSubCount];
    if (NULL == pSubChannelInfo)
    {
        bRet = false;
        goto exitHandle;
    }

    // 获得指定APPID控制块的DataSet
    if (-1 == pGooseAnalysis->GetAllIEDGOOSESubChannelInfo(pGooseInfo->iedName, pGooseInfo->iedLdInst, stChannelInfo, pSubChannelInfo, iSubCount))
    {
        goto exitHandle;
    }

    //
    // 将DataSet的信息存放到控制块的通道容器中
    //
    CChannel*   pSubChannel    = NULL;
    for (int i = 0; i < iSubCount; ++i)
    {
        pSubChannel = new CChannel(pChannel->GetParentBaseCb(), pChannel, NULL);
        if (NULL == pSubChannel)
        {
            bRet = false;
            goto exitHandle;
        }

        // 添加通道到容器中
        if (!pChannel->__AddSubChannel(pSubChannel))
        {
            delete pSubChannel;

            bRet = false;
            goto exitHandle;
        }

        //
        // 设置通道属性
        //
        pSubChannel->SetChannelType(__GetChannelType(pSubChannelInfo[i]));
        pSubChannel->m_strDescription   = pSubChannelInfo[i].doDesc;
    }

exitHandle:
    // 释放存放子通道信息的空间
    if (NULL != pSubChannelInfo)
    {
        delete[] pSubChannelInfo;
        pSubChannelInfo = NULL;
    }

    return bRet;
}

///
/// @brief
///    加载IEC61850-9-2控制块
///
/// @param[in]  pSclCacheMgr scl缓存管理，用于加载卸载scl文件
///
/// @return
///     bool    true表示加载成功
///
bool CSCLManager::__LoadIEC9_2Cb(ISCLCacheManager* pSclCacheMgr)
{
    bool                                        bRet                = true;
    ISCDSMV92Analysis*                          pSmvAnalysis        = NULL;
    SCDSMV92ANALYSIS_IEDSMV_INFO*               pSmvInfo            = NULL;


    // 检查输入参数
    if (NULL == pSclCacheMgr)
    {
        return false;
    }

    // 创建SCD文件SMV92配置解析接口对象实例
    pSmvAnalysis = CreateSCDSMV92AnalysisImpl(pSclCacheMgr);
    if (NULL == pSmvAnalysis)
    {
        return false;
    }

    // 获得控制块数量
    int iCount = pSmvAnalysis->GetIEDSMVNum();
    if (iCount < 0)
    {
        bRet = false;
        goto exitHandle;
    }
    else if (iCount == 0)
    {
        goto exitHandle;
    }

    // 申请存放控制块信息的空间
    pSmvInfo = new SCDSMV92ANALYSIS_IEDSMV_INFO[iCount];
    if (NULL == pSmvInfo)
    {
        bRet = false;
        goto exitHandle;
    }

    // 获得所有控制块的信息
    iCount = pSmvAnalysis->GetAllIEDSMVInfo(pSmvInfo, iCount);
    if (-1 == iCount)
    {
        goto exitHandle;
    }

    //
    // 将控制块的信息存放到m_mapIEC9_2CbGroup容器中
    //
    CIEC9_2Cb*       pIEC9_2Cb    = NULL;
    for (int i = 0; i < iCount; ++i)
    {
        pIEC9_2Cb = new CIEC9_2Cb(NULL);
        if (NULL == pIEC9_2Cb)
        {
            bRet = false;
            goto exitHandle;
        }

        pIEC9_2Cb->m_strDescription     = pSmvInfo[i].iedDesc;
        pIEC9_2Cb->m_iAPPID             = pSmvInfo[i].smvAppID;
        memcpy_s(pIEC9_2Cb->m_ucDesMAC, ETHERNET_MAC_LEN, pSmvInfo[i].smvMAC, SCDSMV92ANALYSIS_MAC_SIZE);
        pIEC9_2Cb->m_iVLanID            = pSmvInfo[i].smvVlanID;
        pIEC9_2Cb->m_iVLanPriority      = pSmvInfo[i].smvVlanPri;
        pIEC9_2Cb->m_iConfigRevision    = pSmvInfo[i].smvCbConfRev;
        pIEC9_2Cb->m_strSvID            = pSmvInfo[i].smvCbSmvID;
        pIEC9_2Cb->m_strDataSetRef      = (pSmvInfo[i].iedName + pSmvInfo[i].iedLdInst + L"/LLN0$" + pSmvInfo[i].dataSetName);

        // 判断指定APPID的控制块是否已存在
        if (m_mapIEC9_2CbGroup.find(pIEC9_2Cb->GetAPPID()) != m_mapIEC9_2CbGroup.end())
        {
            delete pIEC9_2Cb;
            continue;
        }

        // 加载控制块的通道
        if (!__LoadIEC9_2Channel(pSmvAnalysis, &pSmvInfo[i], pIEC9_2Cb))
        {
            delete pIEC9_2Cb;

            bRet = false;
            goto exitHandle;
        }

        // 检查控制块通道数
        if (0 == pIEC9_2Cb->GetChannelCount())
        {
            delete pIEC9_2Cb;
            continue;
        }

        // 将控制块存放到容器中
        std::pair< std::map<uint32_t, CIEC9_2Cb*>::iterator, bool > retPair;
        retPair = m_mapIEC9_2CbGroup.insert(std::make_pair(pIEC9_2Cb->GetAPPID(), pIEC9_2Cb));
        if (!retPair.second)
        {
            delete pIEC9_2Cb;

            bRet = false;
            goto exitHandle;
        }
    }

exitHandle:
    // 释放存放控制块信息的空间
    if (NULL != pSmvInfo)
    {
        delete[] pSmvInfo;
        pSmvInfo = NULL;
    }

    // 释放SCD文件SMV92配置解析接口对象实例
    if (NULL != pSmvAnalysis)
    {
        delete pSmvAnalysis;
        pSmvAnalysis = NULL;
    }

    return bRet;
}

///
/// @brief
///    加载IEC61850-9-2控制块的通道
///
/// @param[in]  pSmvAnalysis 指向SCD文件SMV92配置解析接口的指针
/// @param[in]  pSmvInfo 指向SMV92控制块信息的结构体的指针
/// @param[in]  pIEC9_2Cb 指向IEC61850-9-2控制块对象的指针
///
/// @return
///     bool    true表示加载成功
///
bool CSCLManager::__LoadIEC9_2Channel(ISCDSMV92Analysis* pSmvAnalysis, SCDSMV92ANALYSIS_IEDSMV_INFO* pSmvInfo, CIEC9_2Cb* pIEC9_2Cb)
{
    bool                                        bRet             = true;
    SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO*       pChannelInfo     = NULL;


    // 检查输入参数
    if ((NULL == pSmvAnalysis) || (NULL == pIEC9_2Cb))
    {
        return false;
    }

    // 获得控制块的MAC地址
    unsigned char cMacBuffer[ETHERNET_MAC_LEN] = {'0'};
    pIEC9_2Cb->GetDesMAC(cMacBuffer, ETHERNET_MAC_LEN);

    // 获得指定APPID控制块的通道数
    int iCount = pSmvAnalysis->GetIEDSMVDataSetNum(pSmvInfo->iedName, pSmvInfo->iedLdInst, pSmvInfo->smvCbName);
    if (iCount < 0)
    {
        bRet = false;
        goto exitHandle;
    }
    else if (iCount == 0)
    {
        goto exitHandle;
    }

    // 申请存放控制块DataSet信息的空间
    pChannelInfo = new SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO[iCount];
    if (NULL == pChannelInfo)
    {
        bRet = false;
        goto exitHandle;
    }

    // 获得指定APPID控制块的DataSet
    if (-1 == pSmvAnalysis->GetAllIEDSMVDataSetInfo(pSmvInfo->iedName, pSmvInfo->iedLdInst, pSmvInfo->smvCbName, pChannelInfo, iCount))
    {
        goto exitHandle;
    }

    //
    // 将DataSet的信息存放到控制块的通道容器中
    //
    CChannel*   pChannel    = NULL;
    for (int i = 0; i < iCount; ++i)
    {
        pChannel = new CChannel(pIEC9_2Cb, NULL, NULL);
        if (NULL == pChannel)
        {
            bRet = false;
            goto exitHandle;
        }

        //
        // 设置通道属性
        //
        pChannel->SetChannelType(__GetChannelType(pChannelInfo[i]));
        pChannel->m_strDescription  = pChannelInfo[i].doDesc;
        pChannel->m_eChannelPhase   = __GetChannelPhase(pChannelInfo[i]);
        pChannel->m_bMeasure        = __GetMeasure(pChannel->m_eChannelType, pChannelInfo[i].doDesc);

        // 设置通道映射
        if (CHANNEL_TYPE_SMV_CURRENT == pChannel->m_eChannelType)
        {
            pChannel->m_strVirtualChannel = L"Ia1";
            if (CHANNEL_PHASE_B == pChannel->m_eChannelPhase)
            {
                pChannel->m_strVirtualChannel = L"Ib1";
            }
            else if (CHANNEL_PHASE_C == pChannel->m_eChannelPhase)
            {
                pChannel->m_strVirtualChannel = L"Ic1";
            }
            else if (CHANNEL_PHASE_X == pChannel->m_eChannelPhase)
            {
                pChannel->m_strVirtualChannel = L"Ix1";
            }
            else if (CHANNEL_PHASE_N == pChannel->m_eChannelPhase)
            {
                pChannel->m_strVirtualChannel = L"Ix1";
            }
        }
        else if (CHANNEL_TYPE_SMV_VOLTAGE == pChannel->m_eChannelType)
        {
            pChannel->m_strVirtualChannel = L"Ua1";
            if (CHANNEL_PHASE_B == pChannel->m_eChannelPhase)
            {
                pChannel->m_strVirtualChannel = L"Ub1";
            }
            else if (CHANNEL_PHASE_C == pChannel->m_eChannelPhase)
            {
                pChannel->m_strVirtualChannel = L"Uc1";
            }
            else if (CHANNEL_PHASE_X == pChannel->m_eChannelPhase)
            {
                pChannel->m_strVirtualChannel = L"Ux1";
            }
            else if (CHANNEL_PHASE_N == pChannel->m_eChannelPhase)
            {
                pChannel->m_strVirtualChannel = L"Ux1";
            }
        }
        else if (CHANNEL_TYPE_SMV_TIME == pChannel->m_eChannelType)
        {
            pChannel->m_strVirtualChannel = L"t1";
        }

        // 添加通道到容器中
        if (!pIEC9_2Cb->__AddChannel(pChannel))
        {
            delete pChannel;

            bRet = false;
            goto exitHandle;
        }
    }

exitHandle:
    // 释放存放控制块DataSet信息的空间
    if (NULL != pChannelInfo)
    {
        delete[] pChannelInfo;
        pChannelInfo = NULL;
    }

    return bRet;
}

///
/// @brief
///    清空map保留所有的信息
///
void CSCLManager::__ClearAllMap()
{
    // 遍历容器，释放IEC61850-9-2控制块对象
    {
        std::map<uint32_t, CIEC9_2Cb*>::iterator iter = m_mapIEC9_2CbGroup.begin();
        while (m_mapIEC9_2CbGroup.end() != iter)
        {
            CIEC9_2Cb* pIEC9_2Cb = iter->second;
            if (pIEC9_2Cb)
            {
                delete pIEC9_2Cb;
            }

            iter++;
        }

        // 清空容器
        m_mapIEC9_2CbGroup.clear();
    }

    // 遍历容器，释放GOOSE控制块对象
    {
        std::map<uint32_t, CGooseCb*>::iterator iter = m_mapGooseCbGroup.begin();
        while (m_mapGooseCbGroup.end() != iter)
        {
            CGooseCb* pGooseCb = iter->second;
            if (pGooseCb)
            {
                delete pGooseCb;
            }

            iter++;
        }

        // 清空容器
        m_mapGooseCbGroup.clear();
    }
}

///
/// @brief
///    获取通道的相别信息
///
/// @param[in] stSmvChannelInfo 采样值数据集中的数据通道信息
///
/// @return
///     返回对应的通道相别 A/B/C相
///
ChannelPhase CSCLManager::__GetChannelPhase(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo )
{
    int nSize = sizeof(PHASE_A_TEMPLATE)/sizeof(PHASE_A_TEMPLATE[0]);
    for (int i = 0; i < nSize; ++i)
    {
        if (stSmvChannelInfo.daVal.find(PHASE_A_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_A;
        }
        if (stSmvChannelInfo.doDesc.find(PHASE_A_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_A;
        }
        if (stSmvChannelInfo.lnDesc.find(PHASE_A_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_A;
        }
        if (stSmvChannelInfo.lnPrefix.find(PHASE_A_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_A;
        }
    }

    nSize = sizeof(PHASE_B_TEMPLATE)/sizeof(PHASE_B_TEMPLATE[0]);
    for (int i = 0; i < nSize; ++i)
    {
        if (stSmvChannelInfo.daVal.find(PHASE_B_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_B;
        }
        if (stSmvChannelInfo.doDesc.find(PHASE_B_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_B;
        }
        if (stSmvChannelInfo.lnDesc.find(PHASE_B_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_B;
        }
        if (stSmvChannelInfo.lnPrefix.find(PHASE_B_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_B;
        }
    }

    nSize = sizeof(PHASE_C_TEMPLATE)/sizeof(PHASE_C_TEMPLATE[0]);
    for (int i = 0; i < nSize; ++i)
    {
        if (stSmvChannelInfo.daVal.find(PHASE_C_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_C;
        }
        if (stSmvChannelInfo.doDesc.find(PHASE_C_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_C;
        }
        if (stSmvChannelInfo.lnDesc.find(PHASE_C_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_C;
        }
        if (stSmvChannelInfo.lnPrefix.find(PHASE_C_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_C;
        }
    }

    nSize = sizeof(PHASE_X_TEMPLATE)/sizeof(PHASE_X_TEMPLATE[0]);
    for (int i = 0; i < nSize; ++i)
    {
        if (stSmvChannelInfo.daVal.find(PHASE_X_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_X;
        }
        if (stSmvChannelInfo.doDesc.find(PHASE_X_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_X;
        }
        if (stSmvChannelInfo.lnDesc.find(PHASE_X_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_X;
        }
        if (stSmvChannelInfo.lnPrefix.find(PHASE_X_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_X;
        }
    }

    nSize = sizeof(PHASE_N_TEMPLATE)/sizeof(PHASE_N_TEMPLATE[0]);
    for (int i = 0; i < nSize; ++i)
    {
        if (stSmvChannelInfo.daVal.find(PHASE_N_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_N;
        }
        if (stSmvChannelInfo.doDesc.find(PHASE_N_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_N;
        }
        if (stSmvChannelInfo.lnDesc.find(PHASE_N_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_N;
        }
        if (stSmvChannelInfo.lnPrefix.find(PHASE_N_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_PHASE_N;
        }
    }

    return CHANNEL_PHASE_X;
}

///
/// @brief
///    获取通道的类型信息
///
/// @param[in] stSmvChannelInfo 采样值数据集中的数据通道信息
///
/// @return
///     返回对应的通道类型信息 时间/电压/电流...
///
enumChannelType CSCLManager::__GetChannelType(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo )
{
    enumChannelType enChannelType = CHANNEL_TYPE_UNKOWN;
    int nSize = sizeof(TIME_92_TEMPLATE)/sizeof(TIME_92_TEMPLATE[0]);
    for (int i = 0; i < nSize; ++i)
    {
        if (stSmvChannelInfo.doDesc.find(TIME_92_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_TYPE_SMV_TIME;
        }
    }

    if (stSmvChannelInfo.doName == L"DelayTRtg")
    {
        enChannelType = CHANNEL_TYPE_SMV_TIME;
    }
    else if (stSmvChannelInfo.lnClass == L"TVTR")
    {
        enChannelType = CHANNEL_TYPE_SMV_VOLTAGE;
    }
    else if (stSmvChannelInfo.lnClass == L"TCTR")
    {
        enChannelType = CHANNEL_TYPE_SMV_CURRENT;
    }
    else
    {
        if (stSmvChannelInfo.daVal.find(L"电压") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_VOLTAGE;
        }
        else if (stSmvChannelInfo.daVal.find(L"电流") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_CURRENT;
        }
        else if (stSmvChannelInfo.doDesc.find(L"电压") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_VOLTAGE;
        }
        else if (stSmvChannelInfo.doDesc.find(L"电流") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_CURRENT;
        }
        else if (stSmvChannelInfo.lnDesc.find(L"电压") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_VOLTAGE;
        }                                           
        else if (stSmvChannelInfo.lnDesc.find(L"电流") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_CURRENT;
        }
    }
    return enChannelType;
}

///
/// @brief
///    获取通道的类型信息
///
/// @param[in] stGooseChannelInfo goose数据集中的数据通道信息
///
/// @return
///     返回对应的通道类型信息 单电/双点/时间...
///
enumChannelType CSCLManager::__GetChannelType(const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO& stGooseChannelInfo )
{
    enumChannelType enChannelType = CHANNEL_TYPE_UNKOWN;
    if (stGooseChannelInfo.daBType == L"BOOLEAN")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_POINT;
    }
    else if (stGooseChannelInfo.daBType == L"Quality")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_QUALITY;
    }
    else if (stGooseChannelInfo.daBType == L"Timestamp")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_TIME;
    }
    else if (stGooseChannelInfo.daBType == L"Dbpos")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_DOUBLE_POINT;
    }
    else if (stGooseChannelInfo.daBType == L"FLOAT32")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_FLOAT;
    }
    else if (stGooseChannelInfo.daBType == L"INT8")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_INT8;
    }
    else if (stGooseChannelInfo.daBType == L"INT16")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_INT16;
    }
    else if (stGooseChannelInfo.daBType == L"INT24")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_INT24;
    }
    else if (stGooseChannelInfo.daBType == L"INT32")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_INT32;
    }
    else if (stGooseChannelInfo.daBType == L"INT8U")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_UINT8;
    }
    else if (stGooseChannelInfo.daBType == L"INT16U")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_UINT16;
    }
    else if (stGooseChannelInfo.daBType == L"INT24U")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_UINT24;
    }
    else if (stGooseChannelInfo.daBType == L"INT32U")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_UINT32;
    }
    else if ((stGooseChannelInfo.daBType.empty() && !stGooseChannelInfo.doName.empty()) || (stGooseChannelInfo.daBType == L"Struct"))
    {
        enChannelType = CHANNEL_TYPE_GOOSE_STRUCT;
    }
    else
    {
        enChannelType = CHANNEL_TYPE_UNKOWN;
    }

    return enChannelType;
}

///
/// @brief
///    获取静态实例，全局使用一个管理器
///
CSCLManager* CSCLManager::getInstance()
{
    static CSCLManager s_SclManager;
    return &s_SclManager;
}

///
/// @brief
///     枚举第一个GOOSE控制块
///
/// @return
///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
///
CGooseCb* CSCLManager::FirstGooseCb()
{
    std::map<uint32_t, CGooseCb*>::iterator iter = m_mapGooseCbGroup.begin();
    if (m_mapGooseCbGroup.end() != iter)
    {
        return iter->second;
    }

    return NULL;
}

///
/// @brief
///     枚举下一个GOOSE控制块
///
/// @param[in]  pCurGocb 当前位置GOOSE控制块的指针
///
/// @return
///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
///
CGooseCb* CSCLManager::NextGooseCb(CGooseCb* pCurGocb)
{
    CGooseCb*                                   pGocb   = NULL;
    std::map<uint32_t, CGooseCb*>::iterator     iter;


    if (NULL == pCurGocb)
    {
        return NULL;
    }

    // 查找当前GOOSE控制块的位置
    iter = m_mapGooseCbGroup.find(pCurGocb->GetAPPID());
    if (m_mapGooseCbGroup.end() != iter)
    {
        pGocb = iter->second;
        if ((pGocb == pCurGocb))
        {
            // 获得下一个GOOSE控制块的位置
            iter++;
            if (m_mapGooseCbGroup.end() != iter)
            {
                return iter->second;
            }
        }
    }

    return NULL;
}

///
/// @brief
///     获得指定APPID的GOOSE控制块
///
/// @param[in]  
///     iAPPID  -   控制块的APPID
///
/// @return
///     CGooseCb*   成功时返回控制块的指针，失败时返回NULL
///
CGooseCb* CSCLManager::GetGooseCb(int iAPPID)
{
    CGooseCb*                                   pGooseCb    = NULL;
    std::map<uint32_t, CGooseCb*>::iterator     iter;


    // 判断指定APPID的控制块是否存在
    iter = m_mapGooseCbGroup.find(iAPPID);
    if (m_mapGooseCbGroup.end() != iter)
    {
        pGooseCb = iter->second;
        if (pGooseCb && (pGooseCb->GetAPPID() == iAPPID))
        {
            return pGooseCb;
        }
    }

    return NULL;
}

///
/// @brief
///     获得GOOSE控制块个数
///
/// @return
///     int    控制块个数
///
int CSCLManager::GetGooseCbCount()
{
    return (int)m_mapGooseCbGroup.size();
}

///
/// @brief
///     选中指定APPID的GOOSE控制块
///
void CSCLManager::SelectGooseCb(int iAPPID)
{
    CGooseCb*                                   pGocb   = NULL;
    std::map<uint32_t, CGooseCb*>::iterator     iter;


    // 判断指定APPID的控制块是否存在
    iter = m_mapGooseCbGroup.find(iAPPID);
    if (m_mapGooseCbGroup.end() != iter)
    {
        pGocb = iter->second;
        if (pGocb && (pGocb->GetAPPID() == iAPPID))
        {
            pGocb->m_bSelected  = true;
        }
    }
}

///
/// @brief
///    清除所有GOOSE控制块的选中状态
///
void CSCLManager::UnSelectAllGooseCb()
{
    CGooseCb*                                   pGocb   = NULL;
    std::map<uint32_t, CGooseCb*>::iterator     iter;


    iter = m_mapGooseCbGroup.begin();
    while (m_mapGooseCbGroup.end() != iter)
    {
        pGocb = iter->second;
        if (pGocb)
        {
            pGocb->m_bSelected  = false;
        }

        iter++;
    }
}

///
/// @brief
///     获得被选中的GOOSE控制块个数
///
/// @return
///     int    被选中的控制块个数
///
int CSCLManager::GetSelectedGooseCbCount()
{
    int                                         iCount  = 0;
    CGooseCb*                                   pGocb   = NULL;
    std::map<uint32_t, CGooseCb*>::iterator     iter;


    iter = m_mapGooseCbGroup.begin();
    while (m_mapGooseCbGroup.end() != iter)
    {
        pGocb = iter->second;

        // 判断控制块是否被选中
        if (pGocb && pGocb->IsSelected())
        {
            iCount++;
        }

        iter++;
    }

    return iCount;
}

///
/// @brief
///     枚举第一个IEC81850-9-2控制块
///
/// @return
///     CIEC9_2Cb*    成功时返回第一个控制块的指针，失败时返回NULL
///
CIEC9_2Cb* CSCLManager::FirstIEC9_2Cb()
{
    std::map<uint32_t, CIEC9_2Cb*>::iterator iter = m_mapIEC9_2CbGroup.begin();
    if (m_mapIEC9_2CbGroup.end() != iter)
    {
        return iter->second;
    }

    return NULL;
}

///
/// @brief
///     枚举下一个IEC81850-9-2控制块
///
/// @param[in]  
///     pCurIEC9_2Cb    -   当前位置IEC81850-9-2控制块的指针
///
/// @return
///     CIEC9_2Cb*  成功时返回下一个控制块的指针，失败时返回NULL
///
CIEC9_2Cb* CSCLManager::NextIEC9_2Cb(CIEC9_2Cb* pCurIEC9_2Cb)
{
    CIEC9_2Cb*                                  pIEC9_2Cb   = NULL;
    std::map<uint32_t, CIEC9_2Cb*>::iterator    iter;


    if (NULL == pCurIEC9_2Cb)
    {
        return NULL;
    }

    // 查找当前IEC81850-9-2控制块的位置
    iter = m_mapIEC9_2CbGroup.find(pCurIEC9_2Cb->GetAPPID());
    if (m_mapIEC9_2CbGroup.end() != iter)
    {
        pIEC9_2Cb = iter->second;
        if ((pIEC9_2Cb == pCurIEC9_2Cb))
        {
            // 获得下一个IEC81850-9-2控制块的位置
            iter++;
            if (m_mapIEC9_2CbGroup.end() != iter)
            {
                return iter->second;
            }
        }
    }

    return NULL;
}

///
/// @brief
///     获得指定APPID的IEC81850-9-2控制块
///
/// @param[in]  
///     iAPPID    -   控制块的APPID
///
/// @return
///     CIEC9_2Cb*  成功时返回控制块的指针，失败时返回NULL
///
CIEC9_2Cb* CSCLManager::GetIEC9_2Cb(int iAPPID)
{
    CIEC9_2Cb*                                  pIEC9_2Cb   = NULL;
    std::map<uint32_t, CIEC9_2Cb*>::iterator    iter;


    // 判断指定APPID的控制块是否存在
    iter = m_mapIEC9_2CbGroup.find(iAPPID);
    if (m_mapIEC9_2CbGroup.end() != iter)
    {
        pIEC9_2Cb = iter->second;
        if (pIEC9_2Cb && (pIEC9_2Cb->GetAPPID() == iAPPID))
        {
            return pIEC9_2Cb;
        }
    }

    return NULL;
}

///
/// @brief
///    获得IEC81850-9-2控制块个数
///
/// @return
///     int    控制块个数
///
int CSCLManager::GetIEC9_2CbCount()
{
    return (int)m_mapIEC9_2CbGroup.size();
}

///
/// @brief
///    选中指定APPID的IEC81850-9-2控制块
///
void CSCLManager::SelectIEC9_2Cb(int iAPPID)
{
    CIEC9_2Cb*                                  pIEC9_2Cb   = NULL;
    std::map<uint32_t, CIEC9_2Cb*>::iterator    iter;


    // 判断指定APPID的控制块是否存在
    iter = m_mapIEC9_2CbGroup.find(iAPPID);
    if (m_mapIEC9_2CbGroup.end() != iter)
    {
        pIEC9_2Cb = iter->second;
        if (pIEC9_2Cb && (pIEC9_2Cb->GetAPPID() == iAPPID))
        {
            pIEC9_2Cb->m_bSelected  = true;
        }
    }
}

///
/// @brief
///    清除所有IEC81850-9-2控制块的选中状态
///
void CSCLManager::UnSelectAllIEC9_2Cb()
{
    CIEC9_2Cb*                                  pIEC9_2Cb   = NULL;
    std::map<uint32_t, CIEC9_2Cb*>::iterator    iter;


    iter = m_mapIEC9_2CbGroup.begin();
    while (m_mapIEC9_2CbGroup.end() != iter)
    {
        pIEC9_2Cb = iter->second;
        if (pIEC9_2Cb)
        {
            pIEC9_2Cb->m_bSelected  = false;
        }

        iter++;
    }
}

///
/// @brief
///    获得被选中的IEC81850-9-2控制块个数
///
/// @return
///     int    被选中的控制块个数
///
int CSCLManager::GetSelectedIEC9_2CbCount()
{
    int                                         iCount  = 0;
    CIEC9_2Cb*                                  pIEC9_2Cb   = NULL;
    std::map<uint32_t, CIEC9_2Cb*>::iterator    iter;


    iter = m_mapIEC9_2CbGroup.begin();
    while (m_mapIEC9_2CbGroup.end() != iter)
    {
        pIEC9_2Cb = iter->second;

        // 判断控制块是否被选中
        if (pIEC9_2Cb && pIEC9_2Cb->IsSelected())
        {
            iCount++;
        }

        iter++;
    }

    return iCount;
}

///
/// @brief
///    根据通道描述，判断通道是否是测量类型
///
/// @param[in]  eChannelType    通道类型
/// @param[in]  strDesc    通道描述
///
/// @return
///     bool    true表示是测量类型
///
bool CSCLManager::__GetMeasure( enumChannelType eChannelType, std::wstring strDesc )
{
    return ((CHANNEL_TYPE_SMV_CURRENT == eChannelType) && (strDesc.find(_T("测量")) != std::wstring::npos));
}
