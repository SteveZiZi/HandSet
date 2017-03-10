/// @file
///
/// @brief
///     SCL�����ļ�����ʹ��src\utils\SclAnalysisģ��
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///     chao        2013.4.7
///
/// �޸���ʷ��
///     Modified by:    chao
///     Modified date:  2013.6.1
///     Version:        1.0.1
///     Descriptions:   ��ȡ��SCL�ļ��п��ƿ����ϸ����Ϣ
///

#include "StdAfx.h"
#include "SCLManager.h"

// �����Ϣ�ж�ģ��
static std::wstring PHASE_A_TEMPLATE[]={L"A��", L"a��", L"����", L"����"};
static std::wstring PHASE_B_TEMPLATE[]={L"B��", L"b��", L"����", L"����"};
static std::wstring PHASE_C_TEMPLATE[]={L"C��", L"c��", L"����", L"����"};
static std::wstring PHASE_X_TEMPLATE[]={L"X��", L"x��", L"ͬ��", L"ĸ��", L"��ȡ", L"����", L"����"};
static std::wstring PHASE_N_TEMPLATE[]={L"N��", L"n��", L"���Ե�", L"����", L"����"};
static std::wstring TIME_92_TEMPLATE[]={L"��ʱ", L"ʱ��"};

///
/// @brief
///    ���캯��
///
CSCLManager::CSCLManager(void)
:m_bSclLoaded(false)
{
}

///
/// @brief
///    ��������
///
CSCLManager::~CSCLManager(void)
{
    UnLoad();
}

///
/// @brief
///    ����scl�ļ�
///
/// @param[in]  strFileName     Ҫ���ص�scl�ļ���
///
/// @return
///     true ���سɹ�
///
bool CSCLManager::Load(std::wstring strFileName )
{
    // ����������
    if (strFileName.empty())
    {
        return false;
    }

    // �������ǰ�����������Ϣ
    UnLoad();

    // ���ļ����з���
    if (!__Analysis(strFileName))
    {
        return false;
    }

    // ���Ϊ�ѳɹ���ʼ��
    m_bSclLoaded = true;
    return true;
}

///
/// @brief
///    ж��SCL�ļ�
///
void CSCLManager::UnLoad()
{
    // ���map������������Ϣ
    __ClearAllMap();

    // ���Ϊδ��ʼ��
    m_bSclLoaded = false;
}

///
/// @brief
///    ָʾSCL�������Ƿ��ѳɹ�������SCL�ļ�
///
/// @return
///     true ��ʾSCL�������Ѿ��ɹ�������SCL�ļ�
///
bool CSCLManager::IsSclLoaded()
{
    return m_bSclLoaded;
}

///
/// @brief
///    ��scl�ļ����з�������ȡscl����Ҫ����Ϣ
///
/// @param[in]  strFileName     Ҫ������scl�ļ���
///
/// @return
///     true ��ʶscl��Ϣ������ȡ�ɹ�
///
bool CSCLManager::__Analysis(std::wstring strFileName)
{
    bool                                        bRet                = true;


    // ����������
    if (strFileName.empty())
    {
        return false;
    }

    // ���SCL�ļ���������ʵ��
    ISCLCacheManager* pSclCacheMgr = CreateSCLCacheManagerImpl();
    if (NULL == pSclCacheMgr)
    {
        return false;
    }

    // ����SCL�����ļ�����������Ӧ�Ľ���
    if( -1 == pSclCacheMgr->LoadSCL(strFileName))
    {
        bRet = false;
        goto exitHandle;
    }

    // ����GOOSE���ƿ�
    if (!__LoadGooseCb(pSclCacheMgr))
    {
        bRet = false;
        goto exitHandle;
    }

    // ����IEC61850-9-2���ƿ�
    if (!__LoadIEC9_2Cb(pSclCacheMgr))
    {
        bRet = false;
        goto exitHandle;
    }

exitHandle:
    // ж�ش򿪵�SCL�����ļ����ͷŽ�������
    if (NULL != pSclCacheMgr)
    {
        pSclCacheMgr->UnloadSCL();
        delete pSclCacheMgr;
    }

    return bRet;
}

///
/// @brief
///    ����GOOSE���ƿ�
///
/// @param[in]  pSclCacheMgr scl����������ڼ���ж��scl�ļ�
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CSCLManager::__LoadGooseCb( ISCLCacheManager* pSclCacheMgr)
{
    bool                                        bRet                = true;
    ISCDGOOSEAnalysis*                          pGooseAnalysis      = NULL;
    SCDGOOSEANALYSIS_IEDGOOSE_INFO*             pGooseInfo          = NULL;


    // ����������
    if (NULL == pSclCacheMgr)
    {
        return false;
    }

    // ����SCD�ļ�GOOSE���ý����ӿڶ���ʵ��
    pGooseAnalysis = CreateSCDGOOSEAnalysisImpl(pSclCacheMgr);
    if (NULL == pGooseAnalysis)
    {
        return false;
    }

    // ��ȡ�����ļ�������IED��������ͨ�Ų�����GOOSE���ƿ������
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

    // ������GOOSE���ƿ���Ϣ�Ŀռ�
    pGooseInfo  = new SCDGOOSEANALYSIS_IEDGOOSE_INFO[iCount];
    if ((NULL == pGooseInfo))
    {
        bRet = false;
        goto exitHandle;
    }

    // �������GOOSE���ƿ����Ϣ
    iCount = pGooseAnalysis->GetAllIEDGOOSEInfo(pGooseInfo, iCount);
    if (-1 == iCount)
    {
        goto exitHandle;
    }

    //
    // ��GOOSE���ƿ����Ϣ��ŵ�m_mapGooseCbMap������
    //
    CGooseCb*       pGooseCb    = NULL;
    for (int i = 0; i < iCount; ++i)
    {
        // newһ��GOOSE���ƿ����
        pGooseCb = new CGooseCb(NULL);
        if (NULL == pGooseCb)
        {
            bRet = false;
            goto exitHandle;
        }

        //
        // ���ÿ��ƿ�����
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

        // �ж�ָ��APPID�Ŀ��ƿ��Ƿ��Ѵ���
        if (m_mapGooseCbGroup.find(pGooseCb->GetAPPID()) != m_mapGooseCbGroup.end())
        {
            delete pGooseCb;
            continue;
        }

        // ���ؿ��ƿ��ͨ��
        if (!__LoadGooseChannel(pGooseAnalysis, &pGooseInfo[i], pGooseCb))
        {
            delete pGooseCb;

            bRet = false;
            goto exitHandle;
        }

        // ���GOOSE���ƿ�ͨ����
        if (0 == pGooseCb->GetChannelCount())
        {
            delete pGooseCb;
            continue;
        }

        // �����ƿ��ŵ�������
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
    // �ͷŴ��GOOSE���ƿ���Ϣ�Ŀռ�
    if (NULL != pGooseInfo)
    {
        delete[] pGooseInfo;
        pGooseInfo = NULL;
    }

    // �ͷ�SCD�ļ�GOOSE���ý����ӿڶ���ʵ��
    if (NULL != pGooseAnalysis)
    {
        delete pGooseAnalysis;
        pGooseAnalysis = NULL;
    }

    return bRet;
}

///
/// @brief
///    ����GOOSE���ƿ��ͨ��
///
/// @param[in]  pGooseAnalysis ָ��GOOSE���ý����ӿڶ����ָ��
/// @param[in]  pGooseInfo ָ��GOOSE���ƿ���Ϣ�ṹ���ָ��
/// @param[in]  pGooseCb ָ��GOOSE���ƿ�����ָ��
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CSCLManager::__LoadGooseChannel(ISCDGOOSEAnalysis* pGooseAnalysis, SCDGOOSEANALYSIS_IEDGOOSE_INFO* pGooseInfo, CGooseCb* pGooseCb)
{
    bool                                        bRet                = true;
    SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO*     pChannelInfo        = NULL;


    // ����������
    if ((NULL == pGooseAnalysis) || (NULL == pGooseCb))
    {
        return false;
    }

    // ��ÿ��ƿ��MAC��ַ
    unsigned char cMacBuffer[ETHERNET_MAC_LEN] = {'0'};
    pGooseCb->GetDesMAC(cMacBuffer, ETHERNET_MAC_LEN);

    // ���ָ��APPID���ƿ��ͨ����
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

    // ������GOOSE���ƿ�DataSet��Ϣ�Ŀռ�
    pChannelInfo = new SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO[iCount];
    if (NULL == pChannelInfo)
    {
        bRet = false;
        goto exitHandle;
    }

    // ���ָ��APPID���ƿ��DataSet
    if (-1 == pGooseAnalysis->GetAllIEDGOOSEDataSetInfo(pGooseInfo->iedName, pGooseInfo->iedLdInst, pGooseInfo->cbName, pChannelInfo, iCount))
    {
        goto exitHandle;
    }

    //
    // ��DataSet����Ϣ��ŵ����ƿ��ͨ��������
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
        // ����ͨ������
        //
        pChannel->SetChannelType(__GetChannelType(pChannelInfo[i]));
        pChannel->m_strDescription  = pChannelInfo[i].doDesc;

        // ���ͨ����������
        if (!pGooseCb->__AddChannel(pChannel))
        {
            delete pChannel;

            bRet = false;
            goto exitHandle;
        }
    }

exitHandle:
    // �ͷŴ��GOOSE���ƿ�DataSet��Ϣ�Ŀռ�
    if (NULL != pChannelInfo)
    {
        delete[] pChannelInfo;
        pChannelInfo = NULL;
    }

    return bRet;
}

///
/// @brief
///     ����GOOSE���ƿ����ͨ��
///
/// @param[in]
///     pGooseAnalysis  -   ָ��GOOSE���ý����ӿڶ����ָ��
/// @param[in]
///     pGooseInfo      -   ָ��GOOSE���ƿ���Ϣ�ṹ���ָ��
/// @param[in]  
///     pGooseCb        -   ָ��GOOSE���ƿ�����ָ��
/// @param[in]  
///     stChannelInfo   -   IED��GOOSE���ݼ���Ϣ(����ͨ��)
/// @param[in]  
///     pChannel        -   ָ��GOOSEͨ����ָ��
///
/// @return
///     bool    true��ʾ���سɹ�
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


    // ����������
    if ((NULL == pGooseAnalysis) || (NULL == pGooseCb))
    {
        return false;
    }

    // ��ÿ��ƿ��MAC��ַ
    unsigned char cMacBuffer[ETHERNET_MAC_LEN] = {'0'};
    pGooseCb->GetDesMAC(cMacBuffer, ETHERNET_MAC_LEN);

    // ���ָ��ͨ������ͨ����
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

    // ��������ͨ����Ϣ�Ŀռ�
    pSubChannelInfo = new SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO[iSubCount];
    if (NULL == pSubChannelInfo)
    {
        bRet = false;
        goto exitHandle;
    }

    // ���ָ��APPID���ƿ��DataSet
    if (-1 == pGooseAnalysis->GetAllIEDGOOSESubChannelInfo(pGooseInfo->iedName, pGooseInfo->iedLdInst, stChannelInfo, pSubChannelInfo, iSubCount))
    {
        goto exitHandle;
    }

    //
    // ��DataSet����Ϣ��ŵ����ƿ��ͨ��������
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

        // ���ͨ����������
        if (!pChannel->__AddSubChannel(pSubChannel))
        {
            delete pSubChannel;

            bRet = false;
            goto exitHandle;
        }

        //
        // ����ͨ������
        //
        pSubChannel->SetChannelType(__GetChannelType(pSubChannelInfo[i]));
        pSubChannel->m_strDescription   = pSubChannelInfo[i].doDesc;
    }

exitHandle:
    // �ͷŴ����ͨ����Ϣ�Ŀռ�
    if (NULL != pSubChannelInfo)
    {
        delete[] pSubChannelInfo;
        pSubChannelInfo = NULL;
    }

    return bRet;
}

///
/// @brief
///    ����IEC61850-9-2���ƿ�
///
/// @param[in]  pSclCacheMgr scl����������ڼ���ж��scl�ļ�
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CSCLManager::__LoadIEC9_2Cb(ISCLCacheManager* pSclCacheMgr)
{
    bool                                        bRet                = true;
    ISCDSMV92Analysis*                          pSmvAnalysis        = NULL;
    SCDSMV92ANALYSIS_IEDSMV_INFO*               pSmvInfo            = NULL;


    // ����������
    if (NULL == pSclCacheMgr)
    {
        return false;
    }

    // ����SCD�ļ�SMV92���ý����ӿڶ���ʵ��
    pSmvAnalysis = CreateSCDSMV92AnalysisImpl(pSclCacheMgr);
    if (NULL == pSmvAnalysis)
    {
        return false;
    }

    // ��ÿ��ƿ�����
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

    // �����ſ��ƿ���Ϣ�Ŀռ�
    pSmvInfo = new SCDSMV92ANALYSIS_IEDSMV_INFO[iCount];
    if (NULL == pSmvInfo)
    {
        bRet = false;
        goto exitHandle;
    }

    // ������п��ƿ����Ϣ
    iCount = pSmvAnalysis->GetAllIEDSMVInfo(pSmvInfo, iCount);
    if (-1 == iCount)
    {
        goto exitHandle;
    }

    //
    // �����ƿ����Ϣ��ŵ�m_mapIEC9_2CbGroup������
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

        // �ж�ָ��APPID�Ŀ��ƿ��Ƿ��Ѵ���
        if (m_mapIEC9_2CbGroup.find(pIEC9_2Cb->GetAPPID()) != m_mapIEC9_2CbGroup.end())
        {
            delete pIEC9_2Cb;
            continue;
        }

        // ���ؿ��ƿ��ͨ��
        if (!__LoadIEC9_2Channel(pSmvAnalysis, &pSmvInfo[i], pIEC9_2Cb))
        {
            delete pIEC9_2Cb;

            bRet = false;
            goto exitHandle;
        }

        // �����ƿ�ͨ����
        if (0 == pIEC9_2Cb->GetChannelCount())
        {
            delete pIEC9_2Cb;
            continue;
        }

        // �����ƿ��ŵ�������
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
    // �ͷŴ�ſ��ƿ���Ϣ�Ŀռ�
    if (NULL != pSmvInfo)
    {
        delete[] pSmvInfo;
        pSmvInfo = NULL;
    }

    // �ͷ�SCD�ļ�SMV92���ý����ӿڶ���ʵ��
    if (NULL != pSmvAnalysis)
    {
        delete pSmvAnalysis;
        pSmvAnalysis = NULL;
    }

    return bRet;
}

///
/// @brief
///    ����IEC61850-9-2���ƿ��ͨ��
///
/// @param[in]  pSmvAnalysis ָ��SCD�ļ�SMV92���ý����ӿڵ�ָ��
/// @param[in]  pSmvInfo ָ��SMV92���ƿ���Ϣ�Ľṹ���ָ��
/// @param[in]  pIEC9_2Cb ָ��IEC61850-9-2���ƿ�����ָ��
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CSCLManager::__LoadIEC9_2Channel(ISCDSMV92Analysis* pSmvAnalysis, SCDSMV92ANALYSIS_IEDSMV_INFO* pSmvInfo, CIEC9_2Cb* pIEC9_2Cb)
{
    bool                                        bRet             = true;
    SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO*       pChannelInfo     = NULL;


    // ����������
    if ((NULL == pSmvAnalysis) || (NULL == pIEC9_2Cb))
    {
        return false;
    }

    // ��ÿ��ƿ��MAC��ַ
    unsigned char cMacBuffer[ETHERNET_MAC_LEN] = {'0'};
    pIEC9_2Cb->GetDesMAC(cMacBuffer, ETHERNET_MAC_LEN);

    // ���ָ��APPID���ƿ��ͨ����
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

    // �����ſ��ƿ�DataSet��Ϣ�Ŀռ�
    pChannelInfo = new SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO[iCount];
    if (NULL == pChannelInfo)
    {
        bRet = false;
        goto exitHandle;
    }

    // ���ָ��APPID���ƿ��DataSet
    if (-1 == pSmvAnalysis->GetAllIEDSMVDataSetInfo(pSmvInfo->iedName, pSmvInfo->iedLdInst, pSmvInfo->smvCbName, pChannelInfo, iCount))
    {
        goto exitHandle;
    }

    //
    // ��DataSet����Ϣ��ŵ����ƿ��ͨ��������
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
        // ����ͨ������
        //
        pChannel->SetChannelType(__GetChannelType(pChannelInfo[i]));
        pChannel->m_strDescription  = pChannelInfo[i].doDesc;
        pChannel->m_eChannelPhase   = __GetChannelPhase(pChannelInfo[i]);
        pChannel->m_bMeasure        = __GetMeasure(pChannel->m_eChannelType, pChannelInfo[i].doDesc);

        // ����ͨ��ӳ��
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

        // ���ͨ����������
        if (!pIEC9_2Cb->__AddChannel(pChannel))
        {
            delete pChannel;

            bRet = false;
            goto exitHandle;
        }
    }

exitHandle:
    // �ͷŴ�ſ��ƿ�DataSet��Ϣ�Ŀռ�
    if (NULL != pChannelInfo)
    {
        delete[] pChannelInfo;
        pChannelInfo = NULL;
    }

    return bRet;
}

///
/// @brief
///    ���map�������е���Ϣ
///
void CSCLManager::__ClearAllMap()
{
    // �����������ͷ�IEC61850-9-2���ƿ����
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

        // �������
        m_mapIEC9_2CbGroup.clear();
    }

    // �����������ͷ�GOOSE���ƿ����
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

        // �������
        m_mapGooseCbGroup.clear();
    }
}

///
/// @brief
///    ��ȡͨ���������Ϣ
///
/// @param[in] stSmvChannelInfo ����ֵ���ݼ��е�����ͨ����Ϣ
///
/// @return
///     ���ض�Ӧ��ͨ����� A/B/C��
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
///    ��ȡͨ����������Ϣ
///
/// @param[in] stSmvChannelInfo ����ֵ���ݼ��е�����ͨ����Ϣ
///
/// @return
///     ���ض�Ӧ��ͨ��������Ϣ ʱ��/��ѹ/����...
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
        if (stSmvChannelInfo.daVal.find(L"��ѹ") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_VOLTAGE;
        }
        else if (stSmvChannelInfo.daVal.find(L"����") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_CURRENT;
        }
        else if (stSmvChannelInfo.doDesc.find(L"��ѹ") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_VOLTAGE;
        }
        else if (stSmvChannelInfo.doDesc.find(L"����") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_CURRENT;
        }
        else if (stSmvChannelInfo.lnDesc.find(L"��ѹ") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_VOLTAGE;
        }                                           
        else if (stSmvChannelInfo.lnDesc.find(L"����") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_CURRENT;
        }
    }
    return enChannelType;
}

///
/// @brief
///    ��ȡͨ����������Ϣ
///
/// @param[in] stGooseChannelInfo goose���ݼ��е�����ͨ����Ϣ
///
/// @return
///     ���ض�Ӧ��ͨ��������Ϣ ����/˫��/ʱ��...
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
///    ��ȡ��̬ʵ����ȫ��ʹ��һ��������
///
CSCLManager* CSCLManager::getInstance()
{
    static CSCLManager s_SclManager;
    return &s_SclManager;
}

///
/// @brief
///     ö�ٵ�һ��GOOSE���ƿ�
///
/// @return
///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
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
///     ö����һ��GOOSE���ƿ�
///
/// @param[in]  pCurGocb ��ǰλ��GOOSE���ƿ��ָ��
///
/// @return
///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
///
CGooseCb* CSCLManager::NextGooseCb(CGooseCb* pCurGocb)
{
    CGooseCb*                                   pGocb   = NULL;
    std::map<uint32_t, CGooseCb*>::iterator     iter;


    if (NULL == pCurGocb)
    {
        return NULL;
    }

    // ���ҵ�ǰGOOSE���ƿ��λ��
    iter = m_mapGooseCbGroup.find(pCurGocb->GetAPPID());
    if (m_mapGooseCbGroup.end() != iter)
    {
        pGocb = iter->second;
        if ((pGocb == pCurGocb))
        {
            // �����һ��GOOSE���ƿ��λ��
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
///     ���ָ��APPID��GOOSE���ƿ�
///
/// @param[in]  
///     iAPPID  -   ���ƿ��APPID
///
/// @return
///     CGooseCb*   �ɹ�ʱ���ؿ��ƿ��ָ�룬ʧ��ʱ����NULL
///
CGooseCb* CSCLManager::GetGooseCb(int iAPPID)
{
    CGooseCb*                                   pGooseCb    = NULL;
    std::map<uint32_t, CGooseCb*>::iterator     iter;


    // �ж�ָ��APPID�Ŀ��ƿ��Ƿ����
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
///     ���GOOSE���ƿ����
///
/// @return
///     int    ���ƿ����
///
int CSCLManager::GetGooseCbCount()
{
    return (int)m_mapGooseCbGroup.size();
}

///
/// @brief
///     ѡ��ָ��APPID��GOOSE���ƿ�
///
void CSCLManager::SelectGooseCb(int iAPPID)
{
    CGooseCb*                                   pGocb   = NULL;
    std::map<uint32_t, CGooseCb*>::iterator     iter;


    // �ж�ָ��APPID�Ŀ��ƿ��Ƿ����
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
///    �������GOOSE���ƿ��ѡ��״̬
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
///     ��ñ�ѡ�е�GOOSE���ƿ����
///
/// @return
///     int    ��ѡ�еĿ��ƿ����
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

        // �жϿ��ƿ��Ƿ�ѡ��
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
///     ö�ٵ�һ��IEC81850-9-2���ƿ�
///
/// @return
///     CIEC9_2Cb*    �ɹ�ʱ���ص�һ�����ƿ��ָ�룬ʧ��ʱ����NULL
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
///     ö����һ��IEC81850-9-2���ƿ�
///
/// @param[in]  
///     pCurIEC9_2Cb    -   ��ǰλ��IEC81850-9-2���ƿ��ָ��
///
/// @return
///     CIEC9_2Cb*  �ɹ�ʱ������һ�����ƿ��ָ�룬ʧ��ʱ����NULL
///
CIEC9_2Cb* CSCLManager::NextIEC9_2Cb(CIEC9_2Cb* pCurIEC9_2Cb)
{
    CIEC9_2Cb*                                  pIEC9_2Cb   = NULL;
    std::map<uint32_t, CIEC9_2Cb*>::iterator    iter;


    if (NULL == pCurIEC9_2Cb)
    {
        return NULL;
    }

    // ���ҵ�ǰIEC81850-9-2���ƿ��λ��
    iter = m_mapIEC9_2CbGroup.find(pCurIEC9_2Cb->GetAPPID());
    if (m_mapIEC9_2CbGroup.end() != iter)
    {
        pIEC9_2Cb = iter->second;
        if ((pIEC9_2Cb == pCurIEC9_2Cb))
        {
            // �����һ��IEC81850-9-2���ƿ��λ��
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
///     ���ָ��APPID��IEC81850-9-2���ƿ�
///
/// @param[in]  
///     iAPPID    -   ���ƿ��APPID
///
/// @return
///     CIEC9_2Cb*  �ɹ�ʱ���ؿ��ƿ��ָ�룬ʧ��ʱ����NULL
///
CIEC9_2Cb* CSCLManager::GetIEC9_2Cb(int iAPPID)
{
    CIEC9_2Cb*                                  pIEC9_2Cb   = NULL;
    std::map<uint32_t, CIEC9_2Cb*>::iterator    iter;


    // �ж�ָ��APPID�Ŀ��ƿ��Ƿ����
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
///    ���IEC81850-9-2���ƿ����
///
/// @return
///     int    ���ƿ����
///
int CSCLManager::GetIEC9_2CbCount()
{
    return (int)m_mapIEC9_2CbGroup.size();
}

///
/// @brief
///    ѡ��ָ��APPID��IEC81850-9-2���ƿ�
///
void CSCLManager::SelectIEC9_2Cb(int iAPPID)
{
    CIEC9_2Cb*                                  pIEC9_2Cb   = NULL;
    std::map<uint32_t, CIEC9_2Cb*>::iterator    iter;


    // �ж�ָ��APPID�Ŀ��ƿ��Ƿ����
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
///    �������IEC81850-9-2���ƿ��ѡ��״̬
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
///    ��ñ�ѡ�е�IEC81850-9-2���ƿ����
///
/// @return
///     int    ��ѡ�еĿ��ƿ����
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

        // �жϿ��ƿ��Ƿ�ѡ��
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
///    ����ͨ���������ж�ͨ���Ƿ��ǲ�������
///
/// @param[in]  eChannelType    ͨ������
/// @param[in]  strDesc    ͨ������
///
/// @return
///     bool    true��ʾ�ǲ�������
///
bool CSCLManager::__GetMeasure( enumChannelType eChannelType, std::wstring strDesc )
{
    return ((CHANNEL_TYPE_SMV_CURRENT == eChannelType) && (strDesc.find(_T("����")) != std::wstring::npos));
}
