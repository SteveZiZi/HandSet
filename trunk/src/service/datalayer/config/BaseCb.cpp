///
/// @brief
///     ���ƿ����
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.29
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "BaseCb.h"
#include "src\service\datalayer\scl\SCLManager.h"
#include "src\service\datalayer\include\datatypes.h"

///
/// @brief
///     ���캯��
///
CBaseCb::CBaseCb(void)
{
}

///
/// @brief
///     ���캯��
///
/// @param[in]  
///     pParamConfigBase    ָ��������ù�������ָ��
///
CBaseCb::CBaseCb(CParamConfigBase* pParamConfigBase)
: m_pParamConfigBase(pParamConfigBase)
, m_bSelected(false)
, m_eFrameType(FRAME_TYPE_UNKOWN)
#if SHENG_ZE
, m_strDescription(L"����SMV����")
#else
, m_strDescription(L"����SV����")
#endif
, m_ePortName(DSM_PORT_UNKOWN)
, m_iAPPID(0x4001)
, m_pChannelHead(NULL)
, m_pChannelTail(NULL)
, m_iChannelCount(0)
{
}

///
/// @brief
///    ��������
///
CBaseCb::~CBaseCb(void)
{
    // �������ͨ��
    ClearAllChannel();
}

///
/// @brief
///    ��ÿ��ƿ�ѡ��״̬
///
/// @return
///     bool    ����true��ʾ���ƿ鱻ѡ��
///
bool CBaseCb::IsSelected()
{
    return m_bSelected;
}

///
/// @brief
///    ���ÿ��ƿ�ѡ��״̬
///
/// @param[in]  bSelected  ���ƿ�ѡ��״̬
///
void CBaseCb::Selected(bool bSelected)
{
    if (m_bSelected != bSelected)
    {
        m_bSelected = bSelected;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ��ÿ��ƿ�������Ϣ
///
/// @return
///     wstring    ���ƿ�������Ϣ
///
std::wstring CBaseCb::GetDescription()
{
    return m_strDescription;
}

///
/// @brief
///    ���ÿ��ƿ�����
///
/// @param[in]  strDescription  ���ƿ�����
///
void CBaseCb::SetDescription(std::wstring strDescription)
{
    if (m_strDescription != strDescription)
    {
        m_strDescription = strDescription;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ����շ��˿�
///
/// @return
///     DsmPortName    �շ��˿�
///
DsmPortName CBaseCb::GetPortName()
{
    return m_ePortName;
}

///
/// @brief
///    �����շ��˿�
///
/// @param[in]  eNewPortName  �շ��˿�
///
void CBaseCb::SetPortName(DsmPortName eNewPortName)
{
    if (m_ePortName != eNewPortName)
    {
        m_ePortName = eNewPortName;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���APPID
///
/// @return
///     int    APPID
///
int CBaseCb::GetAPPID()
{
    return m_iAPPID;
}

///
/// @brief
///    ����APPID
///
/// @param[in]  iNewAPPID  �µ�APPID
///
/// @return
///     bool    true��ʾ����ɹ�
///
bool CBaseCb::SetAPPID(int iNewAPPID)
{
    if ((iNewAPPID < 0) || (iNewAPPID > 65535))
    {
        return false;
    }

    if (m_iAPPID != iNewAPPID)
    {
        m_iAPPID = iNewAPPID;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    ���Channel
///
/// @param[in]  pNewChannel ָ��Channel�����ָ��
///
/// @return
///     bool    true��ʾ��ӳɹ�
///
bool CBaseCb::__AddChannel(CChannel* pNewChannel)
{
    if (NULL == pNewChannel)
    {
        return false;
    }

    // ����ͨ����ǰ��ָ��
    pNewChannel->m_pPrev    = m_pChannelTail;
    pNewChannel->m_pNext    = NULL;
    pNewChannel->__SetParentBaseCb(this);

    // ����ͨ������ָ��
    if (NULL == m_pChannelHead)
    {
        m_pChannelHead = pNewChannel;
    }

    // ����ͨ����βָ��
    if (m_pChannelTail)
    {
        m_pChannelTail->m_pNext = pNewChannel;
    }
    m_pChannelTail = pNewChannel;

    // ����ͨ����
    m_iChannelCount++;
    return true;
}

///
/// @brief
///    ���Channel
///
/// @return
///     CChannel*    ����ӵ�Channel�����ָ��
///
CChannel* CBaseCb::AddChannel()
{
    int     iChannelCount;


    // ���ͨ�����Ƿ��Ѵ��������
    iChannelCount = GetChannelCount();
    if (iChannelCount >= SMV_CHANNEL_NUM_MAX)
    {
        return NULL;
    }

    // newһ��ͨ������
    CChannel*  pChannel = new CChannel(this, NULL, m_pParamConfigBase);
    if (NULL == pChannel)
    {
        return NULL;
    }

    //
    // �����ǰ���ƿ�û��ͨ������ô����ͨ��һ��ֵ������ֵΪ���������е�ȱʡֵ
    // �����ǰ���ƿ���ͨ������ô�������һ��ͨ�������Ը�������ͨ��
    //
    if (0 == iChannelCount)
    {
        pChannel->SetDescription(_T("����ͨ��"));                      ///< ͨ������
        pChannel->SetChannelType(CHANNEL_TYPE_SMV_CURRENT);             ///< ����
        pChannel->SetChannelPhase(CHANNEL_PHASE_A);                     ///< ��λ
    }
    else
    {
        pChannel->__Copy(LastChannel());                                ///< ����ǰһ��ͨ��
    }

    // ���ͨ������ָ�뵽������
    if (!__AddChannel(pChannel))
    {
        delete pChannel;
        return NULL;
    }

    // ���Ϊ���޸�
    if (NULL != m_pParamConfigBase)
    {
        m_pParamConfigBase->SetModified(true);
    }

    return pChannel;
}

///
/// @brief
///    ɾ��ָ��ID��ͨ��
///
/// @param[in]  pDelChannel ͨ������ָ��
///
/// @return
///     bool    true��ʾɾ���ɹ�
///
bool CBaseCb::DeleteChannel(CChannel* pDelChannel)
{
    // ��ʶ���ڵ���1
    if ((NULL == pDelChannel) || (0 == m_iChannelCount))
    {
        return false;
    }

    bool    bFlag = false;
    // ���ֻ��һ��ͨ��
    if ((m_pChannelHead == m_pChannelTail) && (m_pChannelHead == pDelChannel))
    {
        m_pChannelHead  = NULL;
        m_pChannelTail  = NULL;
        bFlag = true;
    }
    // ����ж��ͨ��
    else
    {
        // ���ɾ����һ��ͨ��
        if (m_pChannelHead == pDelChannel)
        {
            m_pChannelHead  = m_pChannelHead->m_pNext;
            m_pChannelHead->m_pPrev = NULL;
            bFlag = true;
        }
        // ���ɾ�����һ��ͨ��
        else if (m_pChannelTail == pDelChannel)
        {
            m_pChannelTail  = m_pChannelTail->m_pPrev;
            m_pChannelTail->m_pNext = NULL;
            bFlag = true;
        }
        // ���ɾ������ͨ��
        else
        {
            // ��������ͨ��
            for (CChannel* pChannel = m_pChannelTail->m_pPrev; pChannel != NULL; pChannel = pChannel->m_pPrev)
            {
                // ����ҵ���ɾ����ͨ��
                if (pChannel == pDelChannel)
                {
                    pChannel->m_pPrev->m_pNext = pChannel->m_pNext;
                    pChannel->m_pNext->m_pPrev = pChannel->m_pPrev;
                    bFlag = true;
                    break;
                }
            }
        }
    }

    // ����ҵ��˴�ɾ����ͨ��
    if (bFlag)
    {
        m_iChannelCount--;
        delete pDelChannel;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return bFlag;
}

///
/// @brief
///    ö�ٵ�һ��Channel
///
/// @return
///     CChannel*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
///
CChannel* CBaseCb::FirstChannel()
{
    return m_pChannelHead;
}

///
/// @brief
///    ö����һ��Channel
///
/// @param[in]  pCurChannel ��ǰChannel��ָ��
///
/// @return
///     CChannel*    �ɹ�ʱ����Channelָ�룬ʧ��ʱ����NULL
///
CChannel* CBaseCb::NextChannel(CChannel* pCurChannel)
{
    if (NULL == pCurChannel)
    {
        return NULL;
    }

    return pCurChannel->m_pNext;
}

///
/// @brief
///    ö����һ��Channel
///
/// @param[in]  pCurChannel ��ǰChannel��ָ��
///
/// @return
///     CChannel*    �ɹ�ʱ����Channelָ�룬ʧ��ʱ����NULL
///
CChannel* CBaseCb::PrevChannel(CChannel* pCurChannel)
{
    if (NULL == pCurChannel)
    {
        return NULL;
    }

    return pCurChannel->m_pPrev;
}

///
/// @brief
///    ö�����һ��Channel
///
/// @return
///     CChannel*    �ɹ�ʱ����Channelָ�룬ʧ��ʱ����NULL
///
CChannel* CBaseCb::LastChannel()
{
    return m_pChannelTail;
}

///
/// @brief
///    �������ͨ��
///
void CBaseCb::ClearAllChannel()
{
    // ���������ͷŶ���
    for (CChannel* pChannel = m_pChannelTail; pChannel != NULL; pChannel = pChannel->m_pPrev)
    {
        delete pChannel;
    }

    // ��λ�����Ǳ���
    if (m_iChannelCount)
    {
        m_pChannelHead  = NULL;
        m_pChannelTail  = NULL;
        m_iChannelCount = 0;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���ͨ����
///
/// @return
///     int    ͨ����
///
int CBaseCb::GetChannelCount()
{
    return m_iChannelCount;
}

///
/// @brief
///    ��ñ�ѡ�е�ͨ����
///
/// @return
///     int    ��ѡ�е�ͨ����
///
int CBaseCb::GetSelectedChannelCount()
{
    int                             iCount      = 0;
    CChannel*                       pChannel    = NULL;


    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // �ж�ͨ���Ƿ�ѡ��
        if (pChannel && pChannel->IsSelected())
        {
            iCount++;
        }
    }

    return iCount;
}

///
/// @brief
///    ��ȿ���һ�����ƿ�
///
/// @param[in]  pBaseCb    ָ��������ƿ�����ָ��
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CBaseCb::__Copy( CBaseCb* pBaseCb )
{
    CChannel*           pChannel    = NULL;
    CChannel*           pNewChannel = NULL;


    // ����������
    if (NULL == pBaseCb)
    {
        return false;
    }

    // ������������
    m_strDescription    = pBaseCb->m_strDescription;
    m_ePortName         = pBaseCb->m_ePortName;
    m_iAPPID            = pBaseCb->m_iAPPID;



    // �������ͨ��
    ClearAllChannel();

    // ����ͨ����Ϣ
    for (pChannel = pBaseCb->m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        pNewChannel = new CChannel(this, NULL, m_pParamConfigBase);
        if (NULL == pNewChannel)
        {
            return false;
        }

        // ��ȿ���ͨ��
        if (!pNewChannel->__Copy(pChannel))
        {
            delete pNewChannel;
            return false;
        }

        // IEC61850 9-2ͨ��û�в�������
        if (FRAME_TYPE_NET_9_2 == m_eFrameType)
        {
            pNewChannel->m_bMeasure = false;
        }

        // ���ͨ����������
        if (!__AddChannel(pNewChannel))
        {
            delete pNewChannel;
            return false;
        }
    }

    return true;
}

///
/// @brief
///    ��ȿ���һ��ɨ���б��е�GOOSE���ƿ�
///
/// @param[in]
///     pGooseMsg    -   GOOSE���ĵı�������
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CBaseCb::__Copy(GooseMsg* pGooseMsg)
{
    int                 iChannelCount   = 0;

    CChannel*           pNewChannel     = NULL;
    CSCLManager*        pSCLManager     = NULL;
    CBaseCb*            pBasecb         = NULL;


    // ����������
    if (NULL == pGooseMsg)
    {
        return false;
    }

    IEC8802_3_PDU_PUBLIC    stPduPublic;
    pGooseMsg->GetIEC8802Info(stPduPublic);
    m_iAPPID                = stPduPublic.appID;                        ///< APPID

    // ���ȫվ�����ļ�����������
    pSCLManager = CSCLManager::getInstance();
    if (pSCLManager)
    {
        pBasecb = pSCLManager->GetGooseCb(m_iAPPID);
        if (pBasecb)
        {
            m_strDescription = pBasecb->GetDescription();               ///< ���ƿ�������Ϣ
        }
    }

    // �������ͨ��
    ClearAllChannel();

    //
    // ����ͨ��
    //
    iChannelCount = (int)pGooseMsg->GetChannelNum();
    for (int i = 0; i < iChannelCount; i++)
    {
        GOOSE_DATA* pGooseData = pGooseMsg->GetAChannel(i);
        if (pGooseData && (GSEDT_UNKNOW != pGooseData->type))
        {
            pNewChannel = new CChannel(this, NULL, m_pParamConfigBase);
            if (NULL == pNewChannel)
            {
                return false;
            }

            // ��ȿ���ͨ��
            if (!pNewChannel->__Copy(pGooseData))
            {
                delete pNewChannel;
                return false;
            }

            //
            // ����ͨ��������Ϣ
            //
            if (pBasecb)
            {
                // ���ȫվ�����ļ��ж�Ӧ��ŵ�ͨ��
                CChannel* pSclChannel = pBasecb->GetChannelByIndex(i);
                if (pSclChannel)
                {
                    // ����ȫվ�����ļ��ж�Ӧͨ����������Ϣ
                    pNewChannel->m_strDescription   = pSclChannel->GetDescription();
                }
            }

            // ���ͨ����������
            if (!__AddChannel(pNewChannel))
            {
                delete pNewChannel;
                return false;
            }
        }
    }

    return true;
}

///
/// @brief
///    ��ȿ���һ��ɨ���б��е�IEC61850-9-1���ƿ�
///
/// @param[in]
///     Sav91Msg    -   IEC61850-9-1���ĵı�������
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CBaseCb::__Copy(Sav91Msg* pSav91Msg)
{
    int                 iChannelCount   = 0;

    CChannel*           pNewChannel     = NULL;
    CSCLManager*        pSCLManager     = NULL;
    CBaseCb*            pBasecb         = NULL;


    // ����������
    if (NULL == pSav91Msg)
    {
        return false;
    }

    IEC8802_3_PDU_PUBLIC    stPduPublic;
    pSav91Msg->GetIEC8802Info(stPduPublic);
    m_iAPPID                = stPduPublic.appID;                        ///< APPID

    // ���ȫվ�����ļ�����������
    pSCLManager = CSCLManager::getInstance();
    if (pSCLManager)
    {
        pBasecb = pSCLManager->GetIEC9_2Cb(m_iAPPID);
        if (pBasecb)
        {
            m_strDescription = pBasecb->GetDescription();               ///< ���ƿ�������Ϣ
        }
    }

    // �������ͨ��
    ClearAllChannel();

    //
    // ����ͨ��
    //
    iChannelCount = (int)pSav91Msg->GetASDUDefaultSmpsNum();
    for (int i = 0; i < iChannelCount; i++)
    {
        pNewChannel = new CChannel(this, NULL, m_pParamConfigBase);
        if (NULL == pNewChannel)
        {
            return false;
        }

        // ���ȫվ�����ļ��д�����ͬAPPID�Ŀ��ƿ飬��ô����������
        if (pBasecb && (i < pBasecb->GetChannelCount()))
        {
            pNewChannel->__Copy(pBasecb->GetChannelByIndex(i));
        }
        // ���ȫվ�����ļ��в�����ͬAPPID�Ŀ��ƿ飬��ôʹ��ȱʡ����
        else
        {
            // �����޷���ͨ�������л��ͨ�����������͡���λ��������־�����Դ˴���ȱʡ��������ֵ
            if (i < sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8)/sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[0]))
            {
                // ͨ������
                pNewChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelType);

                // ͨ������
                pNewChannel->m_strDescription   = PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].strChannelDesc;

                // ��λ
                pNewChannel->m_eChannelPhase    = PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelPhase;

                // �Ƿ����ڲ�������
                pNewChannel->m_bMeasure         = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelValueType);

                // ͨ��ӳ��
                pNewChannel->m_strVirtualChannel= PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].strVirtualChannel;
            }
            else
            {
                wchar_t            cBuffer[100] = {0};


                // ͨ������
                _snwprintf(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]), L"ͨ��#%d", i + 1);
                pNewChannel->m_strDescription   = cBuffer;
            }
        }

        // ���ͨ����������
        if (!__AddChannel(pNewChannel))
        {
            delete pNewChannel;
            return false;
        }
    }

    return true;
}

///
/// @brief
///    ��ȿ���һ��ɨ���б��е�IEC61850-9-2���ƿ�
///
/// @param[in]
///     pSav92Msg    -   IEC61850-9-2���ĵı�������
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CBaseCb::__Copy(Sav92Msg* pSav92Msg)
{
    int                 iChannelCount   = 0;

    CChannel*           pNewChannel     = NULL;
    CSCLManager*        pSCLManager     = NULL;
    CBaseCb*            pBasecb         = NULL;


    // ����������
    if (NULL == pSav92Msg)
    {
        return false;
    }

    IEC8802_3_PDU_PUBLIC    stPduPublic;
    pSav92Msg->GetIEC8802Info(stPduPublic);
    m_iAPPID                = stPduPublic.appID;                        ///< APPID

    // ���ȫվ�����ļ�����������
    pSCLManager = CSCLManager::getInstance();
    if (pSCLManager)
    {
        pBasecb = pSCLManager->GetIEC9_2Cb(m_iAPPID);
        if (pBasecb)
        {
            m_strDescription = pBasecb->GetDescription();               ///< ���ƿ�������Ϣ
        }
    }

    // �������ͨ��
    ClearAllChannel();

    //
    // ����ͨ��
    //
    iChannelCount = (int)pSav92Msg->GetASDUSmpsNum(0);
    for (int i = 0; i < iChannelCount; i++)
    {
        pNewChannel = new CChannel(this, NULL, m_pParamConfigBase);
        if (NULL == pNewChannel)
        {
            return false;
        }

        // ���ȫվ�����ļ��д�����ͬAPPID�Ŀ��ƿ飬��ô����������
        if (pBasecb && (i < pBasecb->GetChannelCount()))
        {
            pNewChannel->__Copy(pBasecb->GetChannelByIndex(i));
            pNewChannel->m_bMeasure = false;
        }
        // ���ȫվ�����ļ��в�����ͬAPPID�Ŀ��ƿ飬��ôʹ��ȱʡ����
        else
        {
            // �����޷���ͨ�������л��ͨ�����������͡���λ��������־�����Դ˴���ȱʡ��������ֵ
            if (i < sizeof(PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO)/sizeof(PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[0]))
            {
                // ͨ������
                pNewChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[i].enChannelType);

                // ͨ������
                pNewChannel->m_strDescription   = PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[i].strChannelDesc;

                // ��λ
                pNewChannel->m_eChannelPhase    = PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[i].enChannelPhase;

                // �Ƿ����ڲ�������
                pNewChannel->m_bMeasure         = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[i].enChannelValueType);

                // ͨ��ӳ��
                pNewChannel->m_strVirtualChannel= PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[i].strVirtualChannel;
            }
            else
            {
                wchar_t            cBuffer[100] = {0};


                // ͨ������
                _snwprintf(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]), L"ͨ��#%d", i + 1);
                pNewChannel->m_strDescription   = cBuffer;
            }
        }

        // ���ͨ����������
        if (!__AddChannel(pNewChannel))
        {
            delete pNewChannel;
            return false;
        }
    }

    return true;
}

///
/// @brief
///    ��ȿ���һ��ɨ���б��е�FT3���ƿ�
///
/// @param[in]
///     pFt3Msg    -   FT3���ĵı�������
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CBaseCb::__Copy(NanRuiFT3Msg* pFt3Msg)
{
    int                 iChannelCount   = 0;

    CChannel*           pNewChannel     = NULL;
    CSCLManager*        pSCLManager     = NULL;
    CBaseCb*            pBasecb         = NULL;


    // ����������
    if (NULL == pFt3Msg)
    {
        return false;
    }

    // ���ȫվ�����ļ�����������
    pSCLManager = CSCLManager::getInstance();
    if (pSCLManager)
    {
        pBasecb = pSCLManager->GetIEC9_2Cb(m_iAPPID);
        if (pBasecb)
        {
            m_strDescription = pBasecb->GetDescription();               ///< ���ƿ�������Ϣ
        }
    }

    // �������ͨ��
    ClearAllChannel();

    //
    // ����ͨ��
    //
    iChannelCount = (int)pFt3Msg->GetASDUSmpsNum();
    for (int i = 0; i < iChannelCount; i++)
    {
        pNewChannel = new CChannel(this, NULL, m_pParamConfigBase);
        if (NULL == pNewChannel)
        {
            return false;
        }

        // ���ȫվ�����ļ��д�����ͬAPPID�Ŀ��ƿ飬��ô����������
        if (pBasecb && (i < pBasecb->GetChannelCount()))
        {
            pNewChannel->__Copy(pBasecb->GetChannelByIndex(i));
        }
        // ���ȫվ�����ļ��в�����ͬAPPID�Ŀ��ƿ飬��ôʹ��ȱʡ����
        else
        {
            // �����޷���ͨ�������л��ͨ�����������͡���λ��������־�����Դ˴���ȱʡ��������ֵ
            if (i < sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8)/sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[0]))
            {
                // ͨ������
                pNewChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelType);

                // ͨ������
                pNewChannel->m_strDescription   = PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].strChannelDesc;

                // ��λ
                pNewChannel->m_eChannelPhase    = PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelPhase;

                // �Ƿ����ڲ�������
                pNewChannel->m_bMeasure         = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelValueType);

                // ͨ��ӳ��
                pNewChannel->m_strVirtualChannel= PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].strVirtualChannel;
            }
            else
            {
                wchar_t            cBuffer[100] = {0};


                // ͨ������
                _snwprintf(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]), L"ͨ��#%d", i + 1);
                pNewChannel->m_strDescription   = cBuffer;
            }
        }

        // ���ͨ����������
        if (!__AddChannel(pNewChannel))
        {
            delete pNewChannel;
            return false;
        }
    }

    return true;
}

///
/// @brief
///    ��ȿ���һ��ɨ���б��е�FT3��չ���ƿ�
///
/// @param[in]
///     pFt3ExpandMsg    -   FT3��չ���ĵı�������
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CBaseCb::__Copy(GuoWangFT3Msg* pFt3ExpandMsg)
{
    int                 iChannelCount   = 0;

    CChannel*           pNewChannel     = NULL;
    CSCLManager*        pSCLManager     = NULL;
    CBaseCb*            pBasecb         = NULL;


    // ����������
    if (NULL == pFt3ExpandMsg)
    {
        return false;
    }

    // ���ȫվ�����ļ�����������
    pSCLManager = CSCLManager::getInstance();
    if (pSCLManager)
    {
        pBasecb = pSCLManager->GetIEC9_2Cb(m_iAPPID);
        if (pBasecb)
        {
            m_strDescription = pBasecb->GetDescription();               ///< ���ƿ�������Ϣ
        }
    }

    // �������ͨ��
    ClearAllChannel();

    //
    // ����ͨ��
    //
    iChannelCount = (int)pFt3ExpandMsg->GetASDUSmpsNum();
    for (int i = 0; i < iChannelCount; i++)
    {
        pNewChannel = new CChannel(this, NULL, m_pParamConfigBase);
        if (NULL == pNewChannel)
        {
            return false;
        }

        // ���ȫվ�����ļ��д�����ͬAPPID�Ŀ��ƿ飬��ô����������
        if (pBasecb && (i < pBasecb->GetChannelCount()))
        {
            pNewChannel->__Copy(pBasecb->GetChannelByIndex(i));
        }
        // ���ȫվ�����ļ��в�����ͬAPPID�Ŀ��ƿ飬��ôʹ��ȱʡ����
        else
        {
            // �����޷���ͨ�������л��ͨ�����������͡���λ��������־�����Դ˴���ȱʡ��������ֵ
            if (i < sizeof(PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG)/sizeof(PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[0]))
            {
                // ͨ������
                pNewChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].enChannelType);

                // ͨ������
                pNewChannel->m_strDescription   = PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].strChannelDesc;

                // ��λ
                pNewChannel->m_eChannelPhase    = PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].enChannelPhase;

                // �Ƿ����ڲ�������
                pNewChannel->m_bMeasure         = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].enChannelValueType);

                // ͨ��ӳ��
                pNewChannel->m_strVirtualChannel= PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].strVirtualChannel;
            }
            else
            {
                wchar_t            cBuffer[100] = {0};


                // ͨ������
                _snwprintf(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]), L"ͨ��#%d", i + 1);
                pNewChannel->m_strDescription   = cBuffer;
            }
        }

        // ���ͨ����������
        if (!__AddChannel(pNewChannel))
        {
            delete pNewChannel;
            return false;
        }
    }

    return true;
}

///
/// @brief
///    ��ÿ��ƿ�ĵ�nIndex����0��ʼ��������ͨ��
///
/// @param[in]  nIndex    ͨ�����
///
/// @return
///     CChannel*    �ɹ�ʱ����Channelָ�룬ʧ��ʱ����NULL
///
CChannel* CBaseCb::GetChannelByIndex( int nIndex )
{
    int                             iCount      = 0;
    CChannel*                       pChannel    = NULL;


    // ��������Ƿ��ںϷ���Χ
    if ((nIndex < 0) || (nIndex >= m_iChannelCount))
    {
        return NULL;
    }

    // ���ҵ�nIndex��ͨ��������ͨ��ָ��
    for(pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        if (iCount == nIndex)
        {
            return pChannel;
        }

        iCount++;
    }

    return NULL;
}

///
/// @brief
///    ����ͨ��������
///    �����ǰͨ���������µ�ͨ��������ôɾ��ĩβ�Ķ���ͨ��
///    �����ǰͨ����С���µ�ͨ��������ô���ͨ��
///
/// @param[in]  uiNewCount    �µ�ͨ����
///
/// @return
///     bool    �ɹ�����true��ʧ�ܷ���false
///
bool CBaseCb::SetChannelCount( unsigned int uiNewCount )
{
    unsigned int     uiOldCount;


    // ����������
    if (uiNewCount < 1)
    {
        return false;
    }

    // ����µ�ͨ��������ԭͨ����
    uiOldCount = m_iChannelCount;
    if (uiNewCount > uiOldCount)
    {
        // ������������µ�ͨ��
        for (unsigned int i = uiOldCount; i < uiNewCount; i++)
        {
            if (NULL == AddChannel())
            {
                return false;
            }
        }
    }
    // ����µ�ͨ����С��ԭͨ����
    else if (uiNewCount < uiOldCount)
    {
        for (unsigned int i = uiOldCount; i > uiNewCount; i--)
        {
            if (!DeleteChannel(LastChannel()))
            {
                return false;
            }
        }
    }
    // ����µ�ͨ��������ԭͨ����
    else
    {
        // �����κβ���
    }

    return true;
}

///
/// @brief
///    ʹ��ָ����ͨ��ģ���������ƿ�
///
/// @param[in]  eType    ģ������
///
/// @return
///     bool    �ɹ�����true
///
bool CBaseCb::UseChannelTemplate( enumSmvRecvConfigTemplateType eType )
{
    return false;
}

///
/// @brief
///    �������п��ƿ��е�ѹͨ����һ�ζֵ
///
/// @param[in]  dNewValue    �µĵ�ѹһ�ζֵ��V��
///
void CBaseCb::UpdateRatedPrimaryVoltage( double dNewValue )
{
    CChannel*                       pChannel    = NULL;


    // ��������ͨ��
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        if (pChannel->GetChannelType() == CHANNEL_TYPE_SMV_VOLTAGE)
        {
            pChannel->SetOnceRatings(dNewValue);
        }
    }
}

///
/// @brief
///    �������п��ƿ��е�ѹͨ���Ķ��ζֵ
///
/// @param[in]  dNewValue    �µĵ�ѹ���ζֵ��V��
///
void CBaseCb::UpdateRatedSecondaryVoltage( double dNewValue )
{
    CChannel*                       pChannel    = NULL;


    // ��������ͨ��
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        if (pChannel->GetChannelType() == CHANNEL_TYPE_SMV_VOLTAGE)
        {
            pChannel->SetSecondaryRatings(dNewValue);
        }
    }
}

///
/// @brief
///    �������п��ƿ��е���ͨ����һ�ζֵ
///
/// @param[in]  dNewValue    �µĵ���һ�ζֵ��A��
///
void CBaseCb::UpdateRatedPrimaryCurrent( double dNewValue )
{
    CChannel*                       pChannel    = NULL;


    // ��������ͨ��
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        if (pChannel->GetChannelType() == CHANNEL_TYPE_SMV_CURRENT)
        {
            pChannel->SetOnceRatings(dNewValue);
        }
    }
}

///
/// @brief
///    �������п��ƿ��е���ͨ���Ķ��ζֵ
///
/// @param[in]  dNewValue    �µĵ������ζֵ��A��
///
void CBaseCb::UpdateRatedSecondaryCurrent( double dNewValue )
{
    CChannel*                       pChannel    = NULL;


    // ��������ͨ��
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        if (pChannel->GetChannelType() == CHANNEL_TYPE_SMV_CURRENT)
        {
            pChannel->SetSecondaryRatings(dNewValue);
        }
    }
}

///
/// @brief
///    �������п��ƿ���ʱ��ͨ���Ķֵ
///
/// @param[in]  dNewValue    �µ�MU���ʱ��us��
///
void CBaseCb::UpdateMuRatedDelay( double dNewValue )
{
    CChannel*                       pChannel    = NULL;


    // ��������ͨ��
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        if (pChannel->GetChannelType() == CHANNEL_TYPE_SMV_TIME)
        {
            pChannel->SetOnceRatings(dNewValue);
            pChannel->SetSecondaryRatings(dNewValue);
        }
    }
}
