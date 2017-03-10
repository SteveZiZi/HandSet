/// @file
///
/// @brief
///     GOOSE���ƿ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.27
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "GooseCb.h"

#define NODE_Gocb_Description           L"Description"                  ///< ���ƿ�����
#define NODE_Gocb_PortName              L"PortName"                     ///< �շ��˿�
#define NODE_Gocb_APPID                 L"APPID"                        ///< APPID
#define NODE_Gocb_SRC_MAC               L"SrcMAC"                       ///< ԴMAC
#define NODE_Gocb_DES_MAC               L"DesMAC"                       ///< Ŀ��MAC
#define NODE_Gocb_VLanID                L"VLanID"                       ///< VLAN ID
#define NODE_Gocb_VLanPriority          L"VLanPriority"                 ///< VLAN���ȼ�
#define NODE_Gocb_ConfigRevision        L"ConfigRevision"               ///< ���ð汾��
#define NODE_Gocb_TestFlag              L"TestFlag"                     ///< TestFlag
#define NODE_Gocb_NeedComm              L"NeedComm"                     ///< NeedComm
#define NODE_Gocb_GocbRef               L"GocbRef"                      ///< GocbRef
#define NODE_Gocb_DataSet               L"DataSet"                      ///< ���ݼ�
#define NODE_Gocb_GoID                  L"GoID"                         ///< GoID
#define NODE_Gocb_ChannelGroup          L"ChannelGroup"                 ///< ChannelGroup
#define NODE_Channel                    L"Channel"                      ///< ͨ��
#define NODE_Channel_Description        L"Description"                  ///< ͨ������
#define NODE_Channel_Type               L"Type"                         ///< ͨ������
#define NODE_Channel_VirtualChannel     L"VirtualChannel"               ///< ӳ��
#define GOOSE_DATA_LEN_MAX              (1233)                          ///< GOOSE��������������󳤶�

#if !CHUN_HUA
#define NODE_Gocb_VLanTag                L"VLanTag"                       ///< VLAN tag
#endif

// Ԥ�����Ŀ��MAC��ַ
static const unsigned char PREDEFINED_GOOSE_DES_MAC[ETHERNET_MAC_LEN] = 
{
    0x01,
    0x0c,
    0xcd,
    0x04,
    0x00,
    0x01
};

///
/// @brief
///    ���캯��
///
CGooseCb::CGooseCb(void)
{
}

///
/// @brief
///     ���캯��
///
/// @param[in]  
///     pParamConfigBase    ָ��������ù�������ָ��
///
CGooseCb::CGooseCb(CParamConfigBase* pParamConfigBase)
:CNetCb(pParamConfigBase)
,m_bTestFlag(false)
,m_bNeedComm(false)
,m_strGocbRef(L"gocbRef")
,m_strDataSet(L"dataSet")
,m_strGoID(L"goID")
,m_strSecurity(L"")
,m_stNum(1)
,m_bIsSendValueChanged(false)
{
    m_eFrameType    = FRAME_TYPE_NET_GOOSE;
    m_ePortName     = NET_PORT_LIGHT_A;
    m_iAPPID        = 0x2001;
#if SHENG_ZE
    m_strDescription= L"����GOOSE����";
#else
	m_strDescription= L"����GSE����";
#endif
    // ���ƫ����
    memset(&m_stPduPblcOff, 0, sizeof(m_stPduPblcOff));

    // Ŀ��MAC��ַ
    memcpy_s(m_ucDesMAC, ETHERNET_MAC_LEN, PREDEFINED_GOOSE_DES_MAC, ETHERNET_MAC_LEN);
}

///
/// @brief
///    ��������
///
CGooseCb::~CGooseCb(void)
{
}

///
/// @brief
///    ���TestFlag
///
/// @return
///     bool    TestFlag
///
bool CGooseCb::GetTestFlag()
{
    return m_bTestFlag;
}

///
/// @brief
///    ����TestFlag
///
/// @param[in]  bTestFlag  �µ�TestFlag
///
void CGooseCb::SetTestFlag(bool bTestFlag)
{
    if (m_bTestFlag != bTestFlag)
    {
        m_bTestFlag = bTestFlag;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���NeedComm
///
/// @return
///     bool    NeedComm
///
bool CGooseCb::GetNeedComm()
{
    return m_bNeedComm;
}

///
/// @brief
///    ����NeedComm
///
/// @param[in]  bNeedComm  �µ�NeedComm
///
void CGooseCb::SetNeedComm(bool bNeedComm)
{
    if (m_bNeedComm != bNeedComm)
    {
        m_bNeedComm = bNeedComm;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���gocbRef
///
/// @return
///     wstring    gocbRef
///
std::wstring CGooseCb::GetGocbRef()
{
    return m_strGocbRef;
}

///
/// @brief
///    ����gocbRef
///
/// @param[in]  strGocbRef  gocbRef
///
void CGooseCb::SetGocbRef(std::wstring strGocbRef)
{
    if (m_strGocbRef != strGocbRef)
    {
        m_strGocbRef = strGocbRef;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ������ݼ�
///
/// @return
///     wstring    ���ݼ�
///
std::wstring CGooseCb::GetDataSet()
{
    return m_strDataSet;
}

///
/// @brief
///    �������ݼ�
///
/// @param[in]  strDataSet  ���ݼ�
///
void CGooseCb::SetDataSet(std::wstring strDataSet)
{
    if (m_strDataSet != strDataSet)
    {
        m_strDataSet = strDataSet;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���goID
///
/// @return
///     wstring    goID
///
std::wstring CGooseCb::GetGoID()
{
    return m_strGoID;
}

///
/// @brief
///    ����goID
///
/// @param[in]  strGoID  goID
///
void CGooseCb::SetGoID(std::wstring strGoID)
{
    if (m_strGoID != strGoID)
    {
        m_strGoID = strGoID;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ��ȿ���һ��GOOSE���ƿ�
///
/// @param[in]
///     pGocb    -   GOOSE���ƿ�ָ��
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CGooseCb::__Copy( CGooseCb* pGocb )
{
    // ����������
    if (NULL == pGocb)
    {
        return false;
    }

    // �������������
    if (!CNetCb::__Copy(pGocb))
    {
        return false;
    }

    // ����CGooseCb������
    m_bTestFlag     = pGocb->m_bTestFlag;
    m_bNeedComm     = pGocb->m_bNeedComm;
    m_strGocbRef    = pGocb->m_strGocbRef;
    m_strDataSet    = pGocb->m_strDataSet;
    m_strGoID       = pGocb->m_strGoID;
    m_strSecurity   = pGocb->m_strSecurity;

    // ����������Ϣ�����ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ��״̬����ʱ���õ���
    m_stPduPblcOff  = pGocb->m_stPduPblcOff;

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
bool CGooseCb::__Copy(GooseMsg* pGooseMsg)
{
    GOOSE_PDU_PUBLIC    stPublicInfo;


    // ����������
    if (NULL == pGooseMsg)
    {
        return false;
    }

    // �������������
    if (!CNetCb::__Copy(pGooseMsg))
    {
        return false;
    }

    // ��ȡ����֡�е�GoosePdu�Ĺ�����Ϣ
    if (!pGooseMsg->GetPduPublicInfo(stPublicInfo))
    {
        return false;
    }

    // ����CGooseCb������
    m_bTestFlag         = stPublicInfo.test? true: false;    
    m_bNeedComm         = stPublicInfo.ndsCom? true: false;
    m_iConfigRevision   = stPublicInfo.confRev;
    m_strGocbRef.assign(stPublicInfo.gocbRef.begin(), stPublicInfo.gocbRef.end());
    m_strDataSet.assign(stPublicInfo.datSet.begin(), stPublicInfo.datSet.end());
    m_strGoID.assign(stPublicInfo.goID.begin(), stPublicInfo.goID.end());

    return true;
}

///
/// @brief
///    ���Channel
///
/// @return
///     CChannel*    ����ӵ�Channel�����ָ��
///
CChannel* CGooseCb::AddChannel()
{
    int     iTotalLen = 0;


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
    if (0 == GetChannelCount())
    {
        pChannel->SetDescription(_T("GOOSEͨ��"));                      ///< ͨ������
        pChannel->SetChannelType(CHANNEL_TYPE_GOOSE_POINT);             ///< ����
    }
    else
    {
        pChannel->__Copy(LastChannel());                                ///< ����ǰһ��ͨ��
    }

    // �ж�ͨ�������Ƿ���
    iTotalLen = GetChannelValueLen();
    if (iTotalLen + pChannel->GetValueLength() > GOOSE_DATA_LEN_MAX)
    {
        delete pChannel;
        return NULL;
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
///    ��ü����ַ���
///
/// @return
///     std::wstring    �����ַ���
///
std::wstring CGooseCb::GetSecurity()
{
    return m_strSecurity;
}

///
/// @brief
///    ���ü����ַ���
///
/// @param[in]  strNewSecurity    �µļ����ַ���
///
void CGooseCb::SetSecurity( std::wstring strNewSecurity )
{
    if (m_strSecurity != strNewSecurity)
    {
        m_strSecurity = strNewSecurity;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���״̬���
///
/// @return
///     int    ״̬���
///
int CGooseCb::GetStNum()
{
    return m_stNum;
}

///
/// @brief
///    ����״̬���
///
/// @param[in]  iNewStNum    �µ�״̬���
///
void CGooseCb::SetStNum( int iNewStNum )
{
    if (m_stNum != iNewStNum)
    {
        m_stNum = iNewStNum;
    }
}

///
/// @brief
///    ���ƿ���ͨ���ķ���ֵ�Ƿ����˸ı�
///
/// @return
///     bool    ����ֵ�Ƿ񱻸ı�
///
bool CGooseCb::IsSendValueChanged()
{
    return m_bIsSendValueChanged;
}

///
/// @brief
///    ���ÿ��ƿ���ͨ���ķ���ֵ�Ƿ����˸ı�
///
/// @param[in]  bChanged    ����ֵ�Ƿ񱻸ı�
///
void CGooseCb::SetSendValueChanged( bool bChanged )
{
    if (m_bIsSendValueChanged != bChanged)
    {
        m_bIsSendValueChanged = bChanged;
    }
}

///
/// @brief
///    �ѵ�ǰ���ƿ���ָ�����ƿ���Ƚϣ��ж�ͨ������ֵ�Ƿ����
///
/// @param[in]  pOldGocb    ָ��GOOSE���ƿ�����ָ��
///
/// @return
///     bool    ��ȷ���true�����򷵻�false
///
bool CGooseCb::__CompareChannelSendValue( CGooseCb* pOldGocb )
{
    bool                            bRet        = true;
    CChannel*                       pOldChannel = NULL;
    CChannel*                       pNewChannel = NULL;


    // �ж�ͨ�����Ƿ����
    if (m_iChannelCount != pOldGocb->m_iChannelCount)
    {
        return false;
    }

    // ���αȽ�ÿһ��ͨ���ķ���ֵ�Ƿ����
    for (pOldChannel = pOldGocb->m_pChannelHead, pNewChannel = m_pChannelHead;
        pOldChannel != NULL && pNewChannel != NULL;
        pOldChannel = pOldChannel->m_pNext, pNewChannel = pNewChannel->m_pNext)
    {
        if (CHANNEL_TYPE_GOOSE_TIME != pNewChannel->GetChannelType())
        {
            // �Ƚ�ͨ������ֵ
            if (!pNewChannel->__CompareSendValue(pOldChannel))
            {
                // ���ͨ���ķ���ֵ�����˸ı�
                pNewChannel->SetSendValueChanged(true);
                bRet = false;
            }
        }
    }

    return bRet;
}

///
/// @brief
///    DO(s, q, t)
///
/// @return
///     bool    �ɹ�����true��ʧ�ܷ���false
///
bool CGooseCb::DO_s_q_t()
{
    CChannel*                       pChannel   = NULL;


    // �������ƿ������ͨ��
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // ��ͨ����������Ϊ�ṹ����
        if (!pChannel->SetChannelType(CHANNEL_TYPE_GOOSE_STRUCT))
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    DA(s, q, t)
///
/// @return
///     bool    �ɹ�����true��ʧ�ܷ���false
///
bool CGooseCb::DA_s_q_t()
{
    CChannel*                       pChannel    = NULL;
    enumChannelType                 eNewType    = CHANNEL_TYPE_GOOSE_POINT;


    // �������ƿ������ͨ��
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        bool    bRet = false;


        // ����ͨ������
        if (CHANNEL_TYPE_GOOSE_POINT == eNewType)
        {
            bRet = pChannel->SetChannelType(eNewType);
            eNewType = CHANNEL_TYPE_GOOSE_QUALITY;
        }
        else if (CHANNEL_TYPE_GOOSE_QUALITY == eNewType)
        {
            bRet = pChannel->SetChannelType(eNewType);
            eNewType = CHANNEL_TYPE_GOOSE_TIME;
        }
        else if (CHANNEL_TYPE_GOOSE_TIME == eNewType)
        {
            bRet = pChannel->SetChannelType(eNewType);
            eNewType = CHANNEL_TYPE_GOOSE_POINT;
        }

        // ��鷵��ֵ
        if (!bRet)
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    DA(s, q)
///
/// @return
///     bool    �ɹ�����true��ʧ�ܷ���false
///
bool CGooseCb::DA_s_q()
{
    CChannel*                       pChannel    = NULL;
    enumChannelType                 eNewType    = CHANNEL_TYPE_GOOSE_POINT;


    // �������ƿ������ͨ��
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        bool    bRet = false;


        // ����ͨ������
        if (CHANNEL_TYPE_GOOSE_POINT == eNewType)
        {
            bRet = pChannel->SetChannelType(eNewType);
            eNewType = CHANNEL_TYPE_GOOSE_QUALITY;
        }
        else if (CHANNEL_TYPE_GOOSE_QUALITY == eNewType)
        {
            bRet = pChannel->SetChannelType(eNewType);
            eNewType = CHANNEL_TYPE_GOOSE_POINT;
        }

        // ��鷵��ֵ
        if (!bRet)
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    DA(s)
///
/// @return
///     bool    �ɹ�����true��ʧ�ܷ���false
///
bool CGooseCb::DA_SinglePoint()
{
    CChannel*                       pChannel   = NULL;


    // �������ƿ������ͨ��
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // ��ͨ����������Ϊ��������
        if (!pChannel->SetChannelType(CHANNEL_TYPE_GOOSE_POINT))
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    DA(˫��)
///
/// @return
///     bool    �ɹ�����true��ʧ�ܷ���false
///
bool CGooseCb::DA_DoublePoint()
{
    CChannel*                       pChannel   = NULL;


    // �������ƿ������ͨ��
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // ��ͨ����������Ϊ˫������
        if (!pChannel->SetChannelType(CHANNEL_TYPE_GOOSE_DOUBLE_POINT))
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    ��ÿ��ƿ�����ͨ��ֵ��ͨ�ű����еĳ���
///
/// @return
///     int    ͨ�����ĳ���
///
int CGooseCb::GetChannelValueLen()
{
    int                             iTotalLen   = 0;
    CChannel*                       pChannel    = NULL;


    // �������ƿ������ͨ��
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // �ۼ�ÿһ��ͨ���ĳ���
        iTotalLen += pChannel->GetValueLength();
    }

    return iTotalLen;
}

///
/// @brief
///    ������ƿ�������ͨ����ƫ����
///
/// @param[in]  pGooseMsg    GOOSE���ĵı����ӿ�
///
/// @return
///     bool    �ɹ�����true
///
bool CGooseCb::CalcChannelOffset( GooseMsg* pGooseMsg )
{
    int                             i           = 0;
    CChannel*                       pChannel    = NULL;
    GOOSE_DATA*                     pGooseData  = NULL;


    // ����������
    if (NULL == pGooseMsg)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CGooseCb::__CalcChannelOffset(): Parameter error \n"));
        return false;
    }

    // �������ƿ������ͨ��
    for (i = 0, pChannel = m_pChannelHead; pChannel != NULL; i++, pChannel = pChannel->m_pNext)
    {
        // ���ͨ��ƫ����
        pGooseData = pGooseMsg->GetAChannel(i);
        if (NULL == pGooseData)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CGooseCb::__CalcChannelOffset(): GetAChannel failed \n"));
            return false;
        }

        // ����ÿһ��ͨ���ڱ���֡�е�ƫ��
        if (!pChannel->__CalcChannelOffset(pGooseData))
        {
            return false;
        }
    }

    // ���GoosePdu�Ĺ�����Ϣ�����ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
    pGooseMsg->GetPduPublicInfoOff(m_stPduPblcOff);

    return true;
}

///
/// @brief
///    ����<Gocb>���ƿ�
///
/// @param[in]  pXml ָ��XML���������
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CGooseCb::__LoadConfig( CMarkup& xml )
{
    // <Description>
    if (!xml.FindElem(NODE_Gocb_Description))
    {
        return false;
    }
    m_strDescription = xml.GetData();

    // <PortName>
    if (!xml.FindElem(NODE_Gocb_PortName))
    {
        return false;
    }
    m_ePortName = (DsmPortName)_wtoi(xml.GetData().c_str());

    // <APPID>
    if (!xml.FindElem(NODE_Gocb_APPID))
    {
        return false;
    }
    m_iAPPID = _wtoi(xml.GetData().c_str());

    // <SrcMAC>
    unsigned char cBuffer[ETHERNET_MAC_LEN] = {'0'};
    if (!xml.FindElem(NODE_Gocb_SRC_MAC))
    {
        return false;
    }
    ConvMacWStr2Char(xml.GetData(), cBuffer, ETHERNET_MAC_LEN);
    memcpy_s(m_ucSrcMAC, ETHERNET_MAC_LEN, cBuffer, ETHERNET_MAC_LEN);

    // <DesMAC>
    if (!xml.FindElem(NODE_Gocb_DES_MAC))
    {
        return false;
    }
    ConvMacWStr2Char(xml.GetData(), cBuffer, ETHERNET_MAC_LEN);
    memcpy_s(m_ucDesMAC, ETHERNET_MAC_LEN, cBuffer, ETHERNET_MAC_LEN);

#if !CHUN_HUA
	// <VLanTag>
	if (!xml.FindElem(NODE_Gocb_VLanTag))
	{
		return false;
	}
	m_bHasVlanFlag = _wtoi(xml.GetData().c_str());
#endif

    // <VLanID>
    if (!xml.FindElem(NODE_Gocb_VLanID))
    {
        return false;
    }
    m_iVLanID = _wtoi(xml.GetData().c_str());

    // <VLanPriority>
    if (!xml.FindElem(NODE_Gocb_VLanPriority))
    {
        return false;
    }
    m_iVLanPriority = _wtoi(xml.GetData().c_str());

    // <ConfigRevision>
    if (!xml.FindElem(NODE_Gocb_ConfigRevision))
    {
        return false;
    }
    m_iConfigRevision = _wtoi(xml.GetData().c_str());

    // <TestFlag>
    if (!xml.FindElem(NODE_Gocb_TestFlag))
    {
        return false;
    }
    m_bTestFlag = (_wtoi(xml.GetData().c_str())? true: false);

    // <NeedComm>
    if (!xml.FindElem(NODE_Gocb_NeedComm))
    {
        return false;
    }
    m_bNeedComm = (_wtoi(xml.GetData().c_str())? true: false);

    // <GocbRef>
    if (!xml.FindElem(NODE_Gocb_GocbRef))
    {
        return false;
    }
    m_strGocbRef = xml.GetData();

    // <DataSet>
    if (!xml.FindElem(NODE_Gocb_DataSet))
    {
        return false;
    }
    m_strDataSet = xml.GetData();

    // <GoID>
    if (!xml.FindElem(NODE_Gocb_GoID))
    {
        return false;
    }
    m_strGoID = xml.GetData();

    // <ChannelGroup>
    if (xml.FindElem(NODE_Gocb_ChannelGroup) && xml.IntoElem())
    {
        //
        // find <Channel>
        //
        while(xml.FindElem(NODE_Channel))
        {
            CChannel*   pChannel = new CChannel(this, NULL, m_pParamConfigBase);
            if (NULL == pChannel)
            {
                return false;
            }

            // ����ͨ��ѡ��״̬
            int iSelected = _wtoi(xml.GetAttrib(L"selected").c_str());
            if (iSelected != 0)
            {
                pChannel->m_bSelected   = true;
            }

            // inside <Channel>
            if (!xml.IntoElem())
            {
                delete pChannel;
                return false;
            }

            // ����ͨ��
            if (!pChannel->__LoadGooseConfig(xml))
            {
                delete pChannel;
                return false;
            }

            // outside <Channel>
            if (!xml.OutOfElem())
            {
                delete pChannel;
                return false;
            }

            // ���GOOSE���ƿ����ָ�뵽����
            if (!__AddChannel(pChannel))
            {
                delete pChannel;
                return false;
            }
        }

        // quit <ChannelGroup>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    ����<Gocb>���ƿ�
///
/// @param[in]  pXml ָ��XML���������
///
/// @return
///     bool    true��ʾ����ɹ�
///
bool CGooseCb::__SaveConfig( CMarkup& xml )
{
    // <Description>
    if (!xml.AddElem(NODE_Gocb_Description) || !xml.SetData(GetDescription()))
    {
        return false;
    }

    // <PortName>
    if (!xml.AddElem(NODE_Gocb_PortName) || !xml.SetData((int)GetPortName()))
    {
        return false;
    }

    // <APPID>
    if (!xml.AddElem(NODE_Gocb_APPID) || !xml.SetData(GetAPPID()))
    {
        return false;
    }

    // <SrcMAC>
    unsigned char cBuffer[ETHERNET_MAC_LEN] = {'0'};
    GetSrcMAC(cBuffer, ETHERNET_MAC_LEN);
    if (!xml.AddElem(NODE_Gocb_SRC_MAC) || !xml.SetData(ConvMacChar2WStr(cBuffer, ETHERNET_MAC_LEN)))
    {
        return false;
    }

    // <DesMAC>
    GetDesMAC(cBuffer, ETHERNET_MAC_LEN);
    if (!xml.AddElem(NODE_Gocb_DES_MAC) || !xml.SetData(ConvMacChar2WStr(cBuffer, ETHERNET_MAC_LEN)))
    {
        return false;
    }

#if !CHUN_HUA
	// <VLanTag>
	if (!xml.AddElem(NODE_Gocb_VLanTag) || !xml.SetData(GetVlanFlag()))
	{
		return false;
	}
#endif

    // <VLanID>
    if (!xml.AddElem(NODE_Gocb_VLanID) || !xml.SetData(GetVlanID()))
    {
        return false;
    }

    // <VLanPriority>
    if (!xml.AddElem(NODE_Gocb_VLanPriority) || !xml.SetData(GetVLanPriority()))
    {
        return false;
    }

    // <ConfigRevision>
    if (!xml.AddElem(NODE_Gocb_ConfigRevision) || !xml.SetData(GetConfigRevision()))
    {
        return false;
    }

    // <TestFlag>
    if (!xml.AddElem(NODE_Gocb_TestFlag) || !xml.SetData(GetTestFlag()))
    {
        return false;
    }

    // <NeedComm>
    if (!xml.AddElem(NODE_Gocb_NeedComm) || !xml.SetData(GetNeedComm()))
    {
        return false;
    }

    // <GocbRef>
    if (!xml.AddElem(NODE_Gocb_GocbRef) || !xml.SetData(GetGocbRef()))
    {
        return false;
    }

    // <DataSet>
    if (!xml.AddElem(NODE_Gocb_DataSet) || !xml.SetData(GetDataSet()))
    {
        return false;
    }

    // <GoID>
    if (!xml.AddElem(NODE_Gocb_GoID) || !xml.SetData(GetGoID()))
    {
        return false;
    }

    //
    // ������������������Channel
    //
    if (GetChannelCount() > 0)
    {
        // <ChannelGroup>
        if (!xml.AddElem(NODE_Gocb_ChannelGroup) || !xml.IntoElem())
        {
            return false;
        }

        // ����ͨ��
        CChannel*   pChannel    = FirstChannel();
        while(pChannel)
        {
            // <Channel>
            if (!xml.AddElem(NODE_Channel))
            {
                return false;
            }

            // <Channel selected="">
            int iSelected = pChannel->IsSelected()? 1: 0;
            if (!xml.SetAttrib(L"selected", iSelected))
            {
                return false;
            }

            // inside <Channel>
            if (!xml.IntoElem())
            {
                return false;
            }

            // ����Channel
            if (!pChannel->__SaveGooseConfig(xml))
            {
                return false;
            }

            // outside <Channel>
            if (!xml.OutOfElem())
            {
                return false;
            }

            pChannel = NextChannel(pChannel);
        }

        // Quit <ChannelGroup>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    �޸�GOOSE���ƿ��е���/˫������ͨ��ʱ����Ҫ�Զ��޸�����/�����ʱ��ͨ����ֵ
///    �˺����Զ�Ѱ��Ѱ�ұ�λ�ĵ���/˫��ͨ����Ȼ��ı����ڻ�����ʱ��ͨ����ֵ
///
/// @param[in]  stNewTime    ����/˫��ͨ���ı�λʱ��
///
void CGooseCb::__UpdateChannelTime( CEventTime stNewTime )
{
    CChannel*                       pSrcChannel     = NULL;
    CChannel*                       pChannel        = NULL;
    CChannel*                       pNextChannel    = NULL;
    CChannel*                       pThirdChannel   = NULL;


    // �������ƿ������ͨ��
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // ���ұ�λ��ͨ��
        if (pChannel->IsSendValueChanged())
        {
            // �����λ���ǵ��㡢˫�����͵�ͨ��
            if (CHANNEL_TYPE_GOOSE_POINT == pChannel->GetChannelType() ||
                CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pChannel->GetChannelType())
            {
                // ������ڵ�ͨ����ʱ������
                pNextChannel = NextChannel(pChannel);
                if (pNextChannel && CHANNEL_TYPE_GOOSE_TIME == pNextChannel->GetChannelType())
                {
                    // �������ڵ�ʱ��ͨ����ֵ
                    GOOSE_DATA_VARIANT& stChannelValue = pNextChannel->GetGooseSendValue();
                    stChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                    stChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;

                    // ����Դʱ��ͨ����ֵ
                    pSrcChannel = pNextChannel->m_pSourceChannel;
                    if (pSrcChannel)
                    {
                        GOOSE_DATA_VARIANT& stSrcChannelValue = pSrcChannel->GetGooseSendValue();
                        stSrcChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                        stSrcChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;
                    }
                }
                // ������ڵ�ͨ����Ʒ������
                else if (pNextChannel && CHANNEL_TYPE_GOOSE_QUALITY == pNextChannel->GetChannelType())
                {
                    // �жϼ����ͨ���Ƿ���ʱ������
                    pThirdChannel = NextChannel(pNextChannel);
                    if (pThirdChannel && CHANNEL_TYPE_GOOSE_TIME == pThirdChannel->GetChannelType())
                    {
                        // ���¼����ʱ��ͨ����ֵ
                        GOOSE_DATA_VARIANT& stChannelValue = pThirdChannel->GetGooseSendValue();
                        stChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                        stChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;

                        // ����Դʱ��ͨ����ֵ
                        pSrcChannel = pThirdChannel->m_pSourceChannel;
                        if (pSrcChannel)
                        {
                            GOOSE_DATA_VARIANT& stSrcChannelValue = pSrcChannel->GetGooseSendValue();
                            stSrcChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                            stSrcChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;
                        }
                    }
                }
            }
            // �����λ���ǽṹ���͵�ͨ��
            else if (CHANNEL_TYPE_GOOSE_STRUCT == pChannel->GetChannelType())
            {
                // ������ͨ����ʱ��
                pChannel->__UpdateSubChannelTime(stNewTime);
            }
        }
    }
}

///
/// @brief
///     �Է����˱�λ�Ŀ��ƿ�ͨ���������±���
///
/// @param[in]  pucFrameHead    ָ����֡֡ͷ��ָ��
///
/// @return
///     bool    �ɹ�����true
///
bool CGooseCb::__RecodeChangedChannels( unsigned char* pucFrameHead )
{
    CChannel*                       pChannel    = NULL;


    // �������ƿ������ͨ��
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // ���ұ�λ��ͨ��
        if (pChannel->IsSendValueChanged())
        {
            // ����ǵ�������ͨ��
            if (CHANNEL_TYPE_GOOSE_POINT == pChannel->GetChannelType())
            {
                // ���ͨ������ֵ
                GOOSE_DATA_VARIANT& stSendValue = pChannel->GetGooseSendValue();
                if (GSEDT_BOOLEAN != stSendValue.m_stUnKnown.id.type)
                {
                    return false;
                }

                // �޸Ļ������е���ͨ���ı���ֵ
                stSendValue.CodeGooseData(pucFrameHead + stSendValue.m_stUtcTime.id.offset, 3);
            }
            // �����˫������ͨ��
            else if (CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pChannel->GetChannelType())
            {
                // ���ͨ������ֵ
                GOOSE_DATA_VARIANT& stSendValue = pChannel->GetGooseSendValue();
                if (GSEDT_BIT_STR != stSendValue.m_stUnKnown.id.type)
                {
                    return false;
                }

                // �޸Ļ�������˫��ͨ���ı���ֵ
                stSendValue.CodeGooseData(pucFrameHead + stSendValue.m_stUtcTime.id.offset, 4);
            }
            // ����ǽṹ������ͨ��
            else if (CHANNEL_TYPE_GOOSE_STRUCT == pChannel->GetChannelType())
            {
                // �Է����˱�λ����ͨ���������±���
                if (!pChannel->__RecodeChangedSubChannels(pucFrameHead))
                {
                    return false;
                }
            }

            // ��λ��λ��ʶ
            pChannel->SetSendValueChanged(false);
        }
    }

    return true;
}

///
/// @brief
///     ����GOOSE���ƿ���������Ҫ��λ��ʱ��ͨ������ͨ��ָ�뱣�浽������
///
/// @param[out]  listShouldChangeTimeChannel    ������Ҫ��λʱ��ͨ����ָ��
///
void CGooseCb::__SaveTimeChannel( std::list<CChannel*>& listShouldChangeTimeChannel )
{
    CChannel*                       pChannel        = NULL;
    CChannel*                       pNextChannel    = NULL;
    CChannel*                       pThirdChannel   = NULL;


    // �������
    listShouldChangeTimeChannel.clear();

    // �������ƿ������ͨ��
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // ���ұ�λ��ͨ��
        if (pChannel->IsSendValueChanged())
        {
            // �����λ���ǵ��㡢˫�����͵�ͨ��
            if (CHANNEL_TYPE_GOOSE_POINT == pChannel->GetChannelType() ||
                CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pChannel->GetChannelType())
            {
                // ������ڵ�ͨ����ʱ������
                pNextChannel = NextChannel(pChannel);
                if (pNextChannel && CHANNEL_TYPE_GOOSE_TIME == pNextChannel->GetChannelType())
                {
                    // ��¼��Ҫ��λ��ʱ��ͨ��
                    listShouldChangeTimeChannel.push_back(pNextChannel);
                }
                // ������ڵ�ͨ����Ʒ������
                else if (pNextChannel && CHANNEL_TYPE_GOOSE_QUALITY == pNextChannel->GetChannelType())
                {
                    // �жϼ����ͨ���Ƿ���ʱ������
                    pThirdChannel = NextChannel(pNextChannel);
                    if (pThirdChannel && CHANNEL_TYPE_GOOSE_TIME == pThirdChannel->GetChannelType())
                    {
                        // ��¼��Ҫ��λ��ʱ��ͨ��
                        listShouldChangeTimeChannel.push_back(pThirdChannel);
                    }
                }
            }
            // �����λ���ǽṹ���͵�ͨ��
            else if (CHANNEL_TYPE_GOOSE_STRUCT == pChannel->GetChannelType())
            {
                // ����������Ҫ��λ��ʱ����ͨ��
                pChannel->__SaveSubTimeChannel(listShouldChangeTimeChannel);
            }
        }
    }
}
