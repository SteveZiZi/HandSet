/// @file
///
/// @brief
///     ״̬���е�״̬��
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.6.14
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "Status.h"
#include "ParamConfigSMVSend.h"
#include "src\service\datalayer\config\ParamConfigBasic.h"

#define NODE_StateSwitch                L"StateSwitch"                  ///< ״̬�л�
#define NODE_StateSwitch_Type           L"Type"                         ///< ״̬�л�����
#define NODE_StateSwitch_Delay          L"Delay"                        ///< ״̬�л���ʱ
#define NODE_DIGroup                    L"DIGroup"                      ///< ���������
#define NODE_DIGroup_Logic              L"logic"                        ///< �������л��߼����㷽ʽ
#define NODE_DI                         L"DI"                           ///< DI
#define NODE_DI_NAME                    L"name"                         ///< name
#define NODE_DOGroup                    L"DOGroup"                      ///< DOGroup
#define NODE_DO                         L"DO"                           ///< DO
#define NODE_DO_NAME                    L"name"                         ///< name
#define NODE_VirtualChannelGroup        L"VirtualChannelGroup"          ///< VirtualChannelGroup
#define NODE_VirtualChannel             L"VirtualChannel"               ///< VirtualChannel

#define VIRTUAL_CHANNEL_COUNT           20                              ///< ����ͨ������

///
/// @brief
///    ���캯��
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
///    ���캯��
///
/// @param[in]  nID    ״̬����Ψһ��ʶID
/// @param[in]  pParamConfigBase    ָ��������ù�������ָ��
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


    // �������������Ĭ��ֵ
    m_mapDIGroup.clear();
    for (i = 0; i < sizeof(PREDEFINED_DI_INFO)/sizeof(PREDEFINED_DI_INFO[0]); i++)
    {
        m_mapDIGroup.insert(std::pair<std::wstring, bool>(PREDEFINED_DI_INFO[i].m_strName, PREDEFINED_DI_INFO[i].m_bChecked));
    }

    // �򿪳����������Ĭ��ֵ
    m_mapDOGroup.clear();
    for (i = 0; i < sizeof(PREDEFINED_DO_INFO)/sizeof(PREDEFINED_DO_INFO[0]); i++)
    {
        m_mapDOGroup.insert(std::pair<std::wstring, bool>(PREDEFINED_DO_INFO[i].m_strName, PREDEFINED_DO_INFO[i].m_bChecked));
    }

    m_listSMVSendVirtualChannelGroup.clear();
}

///
/// @brief
///    ��������
///
CStatus::~CStatus(void)
{
    __ClearSMVSendVirtualChannel();
}

///
/// @brief
///    ��ȿ���һ��״̬����
///
/// @param[in]
///     pOldStatus    -   �Ѵ��ڵ�״̬����
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CStatus::__Copy( CStatus* pOldStatus )
{
    // ����������
    if (NULL == pOldStatus)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CStatus::__Copy(): Parameter error \n"));
        return false;
    }

    // ���ƻ�������
    m_eSwitchType       = pOldStatus->m_eSwitchType;
    m_iDelay            = pOldStatus->m_iDelay;
    m_eLogicalOperator  = pOldStatus->m_eLogicalOperator;

    // DI
    m_mapDIGroup.clear();
    m_mapDIGroup.insert(pOldStatus->m_mapDIGroup.begin(), pOldStatus->m_mapDIGroup.end());

    // DO
    m_mapDOGroup.clear();
    m_mapDOGroup.insert(pOldStatus->m_mapDOGroup.begin(), pOldStatus->m_mapDOGroup.end());

    // SMV��������ͨ��
    __ClearSMVSendVirtualChannel();
    std::list<CSMVSendVirtualChannel*>::iterator iter = pOldStatus->m_listSMVSendVirtualChannelGroup.begin();
    while (pOldStatus->m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        CSMVSendVirtualChannel* pOldVirtualChannel = *iter;
        if (NULL != pOldVirtualChannel)
        {
            // ����һ���µ�����ͨ��
            CSMVSendVirtualChannel* pNewVirtualChannel = new CSMVSendVirtualChannel(FUNCTION_TYPE_STATE_SEQUENCE, m_pParamConfigBase);
            if (NULL == pNewVirtualChannel)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CStatus::__Copy(): new CSMVSendVirtualChannel object failed \n"));
                return false;
            }

            // �����ϵ�����ͨ��
            if (!pNewVirtualChannel->__Copy(pOldVirtualChannel))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CStatus::__Copy(): pNewVirtualChannel->__Copy(pOldVirtualChannel) failed \n"));
                delete pNewVirtualChannel;
                return false;
            }

            // �����ͨ����������
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
///    ���״̬����Ψһ��ʶID
///
/// @return
///     int    ��ʶID
///
int CStatus::__GetID()
{
    return m_nID;
}

///
/// @brief
///    ����<Status>
///
/// @param[in]  xml    xml�ļ���������
///
/// @return
///     bool    true��ʾ���سɹ�
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
        // DI��
        strName = xml.GetAttrib(NODE_DI_NAME);
        if (3 != strName.size())
        {
            return false;
        }

        // DIֵ
        bValue = _wtoi(xml.GetData().c_str())? true: false;

        // ����(strName, bValue)��������������
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
        // DO��
        strName = xml.GetAttrib(NODE_DO_NAME);
        if (3 != strName.size())
        {
            return false;
        }

        // DOֵ
        bValue = _wtoi(xml.GetData().c_str())? true: false;

        // ����(strName, bValue)��������������
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

            // ��������ͨ��
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

            // �������ͨ������ָ�뵽����
            if (!__AddSMVSendVirtualChannel(pVirtualChannel))
            {
                delete pVirtualChannel;
                return false;
            }
        }

        // �������ͨ������
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
///    ����<Status>
///
/// @param[in]  xml    xml�ļ���������
///
/// @return
///     bool    true��ʾ����ɹ�
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

            // DI��
            if (!xml.AddElem(NODE_DI) || !xml.SetAttrib(NODE_DI_NAME, strName))
            {
                return false;
            }

            // DIֵ
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

            // DO��
            if (!xml.AddElem(NODE_DO) || !xml.SetAttrib(NODE_DO_NAME, strName))
            {
                return false;
            }

            // DOֵ
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

                // ��������ͨ��
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
///     �ͷ�����SMV��������ͨ������
///
void CStatus::__ClearSMVSendVirtualChannel()
{
    // �ͷ������е���������ͨ������
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

    // �������
    m_listSMVSendVirtualChannelGroup.clear();
}

///
/// @brief
///    ���SMV��������ͨ����������
///
/// @param[in]  pVirtualChannel    SMV��������ͨ��ָ��
///
/// @return
///     bool    true��ʾ��ӳɹ�
///
bool CStatus::__AddSMVSendVirtualChannel(CSMVSendVirtualChannel* pVirtualChannel)
{
    // ����������
    if (NULL == pVirtualChannel)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CStatus::__AddSMVSendVirtualChannel(): Parameter error \n"));
        return false;
    }

    // �������ͨ���Ƿ��Ѵ���
    if (__GetSMVSendVirtualChannel(pVirtualChannel->GetName()) != NULL)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CStatus::__AddSMVSendVirtualChannel(): Virtual channel already exist \n"));
        return false;
    }

    // ������ͨ������ָ�뱣�浽������
    m_listSMVSendVirtualChannelGroup.push_back(pVirtualChannel);
    return true;
}

///
/// @brief
///    ���״̬�л���ʽ
///
/// @return
///     eSwitchType    ״̬�л���ʽ
///
eSwitchType CStatus::GetSwitchType()
{
    return m_eSwitchType;
}

///
/// @brief
///    ����״̬�л���ʽ
///
/// @param[in]  eNewType    �µ�״̬�л���ʽ
///
void CStatus::SetSwitchType( eSwitchType eNewType )
{
    if (m_eSwitchType != eNewType)
    {
        m_eSwitchType = eNewType;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���״̬�л���ʱ
///
/// @return
///     eSwitchType    ״̬�л���ʱ
///
int CStatus::GetDelay()
{
    return m_iDelay;
}

///
/// @brief
///    ����״̬�л���ʱ
///
/// @param[in]  iDelay    �µ�״̬�л���ʱ
///
void CStatus::SetDelay( int iDelay )
{
    // �����л���ʱ
    if (iDelay < 1 || iDelay > 60000)
    {
        return;
    }

    if (m_iDelay != iDelay)
    {
        m_iDelay = iDelay;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ��ÿ������л��߼����㷽ʽ
///
/// @return
///     eLogicalOperator    �߼����㷽ʽ
///
eLogicalOperator CStatus::GetLogicalOperator()
{
    return m_eLogicalOperator;
}

///
/// @brief
///    ���ÿ������л��߼����㷽ʽ
///
/// @param[in]  eNewLogical    �߼����㷽ʽ
///
void CStatus::SetLogicalOperator( eLogicalOperator eNewLogical )
{
    if (m_eLogicalOperator != eNewLogical)
    {
        m_eLogicalOperator = eNewLogical;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ö�ٵ�һ����ӳ���SMV��������ͨ��
///
/// @return
///     CVirtualChannel*    ָ������ͨ�������ָ�룬ʧ��ʱ����NULL
///
CSMVSendVirtualChannel* CStatus::FirstSMVSendVirtualChannel()
{
    CSMVSendVirtualChannel*                    pVirtualChannel = NULL;


    // ���ҵ�һ����ӳ���˵�����ͨ��
    std::list<CSMVSendVirtualChannel*>::iterator iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // �������ͨ������ָ��
        pVirtualChannel = *iter;
        if (pVirtualChannel)
        {
            // �������ʵͨ��ӳ�䵽������ͨ������ô��������ͨ����ָ��
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
///    �����һ����ӳ���SMV��������ͨ��
///
/// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
///
/// @return
///     CVirtualChannel*    ָ������ͨ�������ָ�룬ʧ��ʱ����NULL
///
CSMVSendVirtualChannel* CStatus::NextSMVSendVirtualChannel( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    CSMVSendVirtualChannel*                    pVirtualChannel = NULL;


    // ����������
    if (NULL == pCurVirtualChannel)
    {
        return NULL;
    }

    // ��õ�ǰ����ͨ��λ��
    std::list<CSMVSendVirtualChannel*>::iterator iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // �������ͨ������ָ��
        pVirtualChannel = *iter;
        if (pVirtualChannel == pCurVirtualChannel)
        {
            break;
        }

        iter++;
    }

    // ����Ƿ��ѵ������β
    if (m_listSMVSendVirtualChannelGroup.end() == iter)
    {
        return NULL;
    }
    else
    {
        // ָ����һ������ͨ��
        while(m_listSMVSendVirtualChannelGroup.end() != ++iter)
        {
            // �������ͨ������ָ��
            pVirtualChannel = *iter;
            if (pVirtualChannel)
            {
                // �������ʵͨ��ӳ�䵽������ͨ������ô��������ͨ����ָ��
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
///    ����ָ�����ֵ�SMV��������ͨ��
///
/// @param[in]  strChannelName    ����ͨ����
///
/// @return
///     CSMVSendVirtualChannel*    ָ������ͨ�������ָ��
///
CSMVSendVirtualChannel* CStatus::__GetSMVSendVirtualChannel( std::wstring strChannelName )
{
    CSMVSendVirtualChannel*                    pVirtualChannel = NULL;


    // ���ҵ�һ����ӳ���˵�����ͨ��
    std::list<CSMVSendVirtualChannel*>::iterator iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // �������ͨ������ָ��
        pVirtualChannel = *iter;
        if (pVirtualChannel)
        {
            // �������ʵͨ��ӳ�䵽������ͨ������ô��������ͨ����ָ��
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
///    ���ָ��DIͨ���Ĺ�ѡ״̬
///
/// @param[in]  strDIName    DIͨ����
///
/// @return
///     bool    DIͨ���Ĺ�ѡ״̬
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
///    ����ָ��DIͨ���Ĺ�ѡ״̬
///
/// @param[in]  strDIName    DIͨ����
/// @param[in]  bChecked    �Ƿ񱻹�ѡ
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

            // ���Ϊ���޸�
            if (NULL != m_pParamConfigBase)
            {
                m_pParamConfigBase->SetModified(true);
            }
        }
    }
}

///
/// @brief
///    ���ָ��DOͨ����ֵ
///
/// @param[in]  strDOName    DOͨ����
///
/// @return
///     bool    DOͨ��ֵ
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
///    ����ָ��DOͨ����ֵ
///
/// @param[in]  strDOName    DOͨ����
/// @param[in]  bNewValue    ͨ������ֵ
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

            // ���Ϊ���޸�
            if (NULL != m_pParamConfigBase)
            {
                m_pParamConfigBase->SetModified(true);
            }
        }
    }
}

///
/// @brief
///    ��״̬�������Ĭ�ϵ�����ͨ����
///
/// @return
///     bool    �ɹ�����true
///
bool CStatus::__AddDefaultSMVSendVirtualChannels()
{
    // �ͷ�����SMV��������ͨ������
    __ClearSMVSendVirtualChannel();

    // ��û����������ù�����
    CParamConfigBasic* pParamConfigBasic = CParamConfigBasic::getInstance();
    if (NULL == pParamConfigBasic)
    {
        return false;
    }

    //
    // ����Ԥ�����SMV��������ͨ��ֵ����������ͨ��������ӵ�������
    //
    for (int i = 0; i < sizeof(PREDEFINED_SMV_SEND_VC_INFO)/sizeof(PREDEFINED_SMV_SEND_VC_INFO[0]); i++)
    {
        CSMVSendVirtualChannel*    pVirtualChannel = new CSMVSendVirtualChannel(FUNCTION_TYPE_STATE_SEQUENCE, m_pParamConfigBase);
        if (NULL == pVirtualChannel)
        {
            return false;
        }

        // ����Ĭ��ֵ
        pVirtualChannel->m_strName      = PREDEFINED_SMV_SEND_VC_INFO[i].m_strName;
        pVirtualChannel->m_dPhaseAngle  = PREDEFINED_SMV_SEND_VC_INFO[i].m_dPhaseAngle;
        pVirtualChannel->m_dFrequency   = PREDEFINED_SMV_SEND_VC_INFO[i].m_dFrequency;

        // ��������ͨ�����ͣ����÷�ֵ
        if (L'U' == pVirtualChannel->m_strName[0])                      ///< ��ѹ��
        {
            pVirtualChannel->m_dAmplitude   = pParamConfigBasic->GetRatedPrimaryVoltage() * 1000 / sqrt(3.0f);
        }
        else if (L'I' == pVirtualChannel->m_strName[0])                 ///< ������
        {
            pVirtualChannel->m_dAmplitude   = pParamConfigBasic->GetRatedPrimaryCurrent();
        }
        else                                                            ///< ʱ����
        {
            swscanf_s(pParamConfigBasic->GetMuRatedDelay().c_str(), L"%lf", &(pVirtualChannel->m_dAmplitude), sizeof(pVirtualChannel->m_dAmplitude));
        }

        // ��ӵ�������
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
///    ���SMV��������ͨ��������
///
/// @return
///     int    ����ͨ��������
///
int CStatus::GetSMVSendVirtualChannelCount()
{
    return (int)m_listSMVSendVirtualChannelGroup.size();
}

///
/// @brief
///    ��ָ����״̬����Ƚ�SMV���ⷢ��ͨ���Ƿ���ͬ
///
/// @param[in]  eCompareType    �ȽϷ�ʽ
/// @param[in]  pOldStatus      ָ��״̬�����ָ��
///
/// @return
///     bool    ��ͬ����true
///
bool CStatus::__CompareSMVSendVirtualChannels( enumCompareType eCompareType, CStatus* pOldStatus )
{
    bool                                                        bRet = true;
    std::list<CSMVSendVirtualChannel*>::iterator                oldIter;
    std::list<CSMVSendVirtualChannel*>::iterator                newIter;


    // ���αȽ�ÿһ������ͨ���ķ�ֵ����ǡ�Ƶ��
    oldIter = pOldStatus->m_listSMVSendVirtualChannelGroup.begin();
    newIter = m_listSMVSendVirtualChannelGroup.begin();
    while((oldIter != pOldStatus->m_listSMVSendVirtualChannelGroup.end()) && 
        (newIter != m_listSMVSendVirtualChannelGroup.end()))
    {
        CSMVSendVirtualChannel* pOldSMVSendVirtualChannel = *oldIter;
        CSMVSendVirtualChannel* pNewSMVSendVirtualChannel = *newIter;
        if ((NULL != pOldSMVSendVirtualChannel) && (NULL != pNewSMVSendVirtualChannel) && (pOldSMVSendVirtualChannel->GetChannelCount() > 0) )
        {
            // �Ƚ�����ͨ��
            if (!pNewSMVSendVirtualChannel->Compare(eCompareType, pOldSMVSendVirtualChannel))
            {
                // �����Ҫ�ҳ����еĲ�ͬ����ô���ñ�λ��ʶ
                if (TYPE_FIND_ALL_DIFFERENT == eCompareType)
                {
                    bRet = false;
                }
                // ���ֻ���ҳ�һ����ͬ����ôֱ�ӷ���
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
///     ����״̬�����б仯��DO
///
/// @param[in]  pOldStatus          ���Ƚϵ�״̬
/// @param[out] mapChangedDoArray   ����ֵ�����˱仯��DOͨ�������ֺ�ֵ
///
/// @return
///     bool    �б仯��DOʱ����true
///
bool CStatus::__FindChangedDO( CStatus* pOldStatus, std::map<std::wstring, bool>& mapChangedDoArray )
{
    bool            bFlag = false;
    bool            bNewValue;
    bool            bOldValue;
    std::wstring    strDOName;
    std::map<std::wstring, bool>::iterator newIter;
    std::map<std::wstring, bool>::iterator oldIter;


    // ����ÿһ��DOͨ�����Ƚ�����DOͨ����ֵ
    mapChangedDoArray.clear();
    for (newIter = m_mapDOGroup.begin(); newIter != m_mapDOGroup.end(); newIter++)
    {
        // ���DOͨ������ͨ��ֵ
        strDOName   = newIter->first;
        bNewValue   = newIter->second;

        // �������֮�Աȵ�״̬����ô�Ƚ�����DOͨ����ֵ
        if (pOldStatus)
        {
            // ������״̬��ͬ����DOͨ��
            oldIter = pOldStatus->m_mapDOGroup.find(strDOName);
            if (oldIter == pOldStatus->m_mapDOGroup.end())
            {
                continue;
            }

            // �Ƚ�����DOͨ����ֵ
            bOldValue   = oldIter->second;
            if (bNewValue != bOldValue)
            {
                // ����ֵ�����˱仯��DOͨ��������
                mapChangedDoArray.insert(std::make_pair(strDOName, bNewValue));
                bFlag   = true;
            }
        }
        // ���û����֮�Աȵ�״̬����ô��Ϊ���е�DOͨ���������˱仯
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
///    �ȷ�ֵ
///
/// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
///
/// @return
///     bool    �ɹ�����true��ʧ�ܷ���false
///
bool CStatus::EqualAmplitude( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    std::wstring                strName;
    double                      dCurAmplitude;


    // ����������
    if (NULL == pCurVirtualChannel)
    {
        return false;
    }

    // ��õ�ǰ����ͨ�������ֺͷ�ֵ
    strName = pCurVirtualChannel->GetName();
    dCurAmplitude = pCurVirtualChannel->m_dAmplitude;

    // ���õ�������ѹ����ͨ���ķ�ֵ
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
    // ����ʱ������ͨ���ķ�ֵ
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
///    ����ָ�����ֵ�����ͨ���ķ�ֵ
///
/// @param[in]  strChannelName    ����ͨ����
/// @param[in]  dNewAmplitude    �µķ�ֵ
///
/// @return
///     bool    �ɹ�����true
///
bool CStatus::__SetSMVSendVCAmplitude( std::wstring strChannelName, double dNewAmplitude )
{
    CSMVSendVirtualChannel*     pVirtualChannel;
    std::list<CSMVSendVirtualChannel*>::iterator iter;


    // ����ָ�����ֵ�����ͨ��
    pVirtualChannel = __GetSMVSendVirtualChannel(strChannelName);
    if (pVirtualChannel)
    {
        // �޸�����ͨ���ķ�ֵ
        return pVirtualChannel->SetAmplitude(dNewAmplitude);
    }

    return false;
}

///
/// @brief
///    ����ָ�����ֵ�����ͨ���ķ�ֵ��һ��ֵ��
///
/// @param[in]  strChannelName    ����ͨ����
/// @param[in]  dNewAmplitude    �µķ�ֵ��һ��ֵ��
///
/// @return
///     bool    �ɹ�����true
///
bool CStatus::__SetSMVSendVCPrimaryAmplitude( std::wstring strChannelName, double dNewAmplitude )
{
    CSMVSendVirtualChannel*     pVirtualChannel;
    std::list<CSMVSendVirtualChannel*>::iterator iter;


    // ����ָ�����ֵ�����ͨ��
    pVirtualChannel = __GetSMVSendVirtualChannel(strChannelName);
    if (pVirtualChannel)
    {
        // �޸�����ͨ���ķ�ֵ��һ��ֵ��
        pVirtualChannel->m_dAmplitude = dNewAmplitude;
        return true;
    }

    return false;
}

///
/// @brief
///    �ֵ
///
/// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
///
/// @return
///     bool    �ɹ�����true��ʧ�ܷ���false
///
bool CStatus::RatedAmplitude( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    // ����������
    if (NULL == pCurVirtualChannel)
    {
        return false;
    }

    // ��������ͨ���Ķֵ
    return pCurVirtualChannel->SetRatedAmplitude();
}

///
/// @brief
///    ����ƽ��
///
/// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
///
/// @return
///     bool    �ɹ�����true��ʧ�ܷ���false
///
bool CStatus::ThreePhaseBalance( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    std::wstring                strName;
    double                      dCurAmplitude;


    // ����������
    if (NULL == pCurVirtualChannel)
    {
        return false;
    }

    // ��õ�ǰ����ͨ�������ֺͷ�ֵ
    strName = pCurVirtualChannel->GetName();
    dCurAmplitude = pCurVirtualChannel->m_dAmplitude;

    // ���õ�������ѹ����ͨ���ķ�ֵ
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
///    �����
///
/// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
///
/// @return
///     bool    �ɹ�����true��ʧ�ܷ���false
///
bool CStatus::EqualPhaseAngle( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    std::wstring                strName;
    double                      dCurPhaseAngle;


    // ����������
    if (NULL == pCurVirtualChannel)
    {
        return false;
    }

    // ��õ�ǰ����ͨ�������ֺͷ�ֵ
    strName = pCurVirtualChannel->GetName();
    dCurPhaseAngle = pCurVirtualChannel->GetPhaseAngle();

    // ���õ�������ѹ����ͨ���ķ�ֵ
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
///    ����ָ�����ֵ�����ͨ�������
///
/// @param[in]  strChannelName    ����ͨ����
/// @param[in]  dNewPhaseAngle    �µ����
///
/// @return
///     bool    �ɹ�����true
///
bool CStatus::__SetSMVSendVCPhaseAngle( std::wstring strChannelName, double dNewPhaseAngle )
{
    CSMVSendVirtualChannel*     pVirtualChannel;


    // ����ָ�����ֵ�����ͨ��
    pVirtualChannel = __GetSMVSendVirtualChannel(strChannelName);
    if (pVirtualChannel)
    {
        // �޸�����ͨ�������
        return pVirtualChannel->SetPhaseAngle(dNewPhaseAngle);
    }

    return false;
}

///
/// @brief
///    �������
///
/// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
///
/// @return
///     bool    �ɹ�����true��ʧ�ܷ���false
///
bool CStatus::PhaseAnglePositiveSequence( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    std::wstring                strName;
    double                      dCurPhaseAngle;


    // ����������
    if (NULL == pCurVirtualChannel)
    {
        return false;
    }

    // ��õ�ǰ����ͨ�������ֺͷ�ֵ
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
///    ��Ƿ���
///
/// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
///
/// @return
///     bool    �ɹ�����true��ʧ�ܷ���false
///
bool CStatus::PhaseAngleReverseSequence( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    std::wstring                strName;
    double                      dCurPhaseAngle;


    // ����������
    if (NULL == pCurVirtualChannel)
    {
        return false;
    }

    // ��õ�ǰ����ͨ�������ֺͷ�ֵ
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
///    ���ָ�����ֵ�����ͨ�������
///
/// @param[in]  strChannelName    ����ͨ����
///
/// @return
///     double      ���ֵ
///
double CStatus::__GetSMVSendVCPhaseAngle( std::wstring strChannelName )
{
    CSMVSendVirtualChannel*     pVirtualChannel;


    // ����ָ�����ֵ�����ͨ��
    pVirtualChannel = __GetSMVSendVirtualChannel(strChannelName);
    if (pVirtualChannel)
    {
        // �������ͨ�������
        return pVirtualChannel->GetPhaseAngle();
    }

    return (0.0f);
}

///
/// @brief
///    ��Ƶ��
///
/// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
///
/// @return
///     bool    �ɹ�����true��ʧ�ܷ���false
///
bool CStatus::EqualFrequency( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    double                      dCurFrequency;
    CSMVSendVirtualChannel*     pVirtualChannel;
    std::list<CSMVSendVirtualChannel*>::iterator iter;


    // ����������
    if (NULL == pCurVirtualChannel)
    {
        return false;
    }

    // ������������ͨ����Ƶ��
    dCurFrequency = pCurVirtualChannel->GetFrequency();
    iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // �������ͨ������ָ��
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
///    ������������ͨ����Ƶ��Ϊָ��ֵ
///
/// @param[in]  dNewFrequency    ����ͨ��Ƶ��
///
/// @return
///     bool    �ɹ�����true
///
bool CStatus::SetAllVCFrequency( double dNewFrequency )
{
    CSMVSendVirtualChannel*     pVirtualChannel;
    std::list<CSMVSendVirtualChannel*>::iterator iter;


    // ����������
    if (dNewFrequency < 0)
    {
        return false;
    }

    // ������������ͨ����Ƶ��
    iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // �������ͨ������ָ��
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
///    ʹ��������ͨ����г��������ͬ
///
/// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
///
/// @return
///     bool    �ɹ�����true
///
bool CStatus::EqualHarmonicType( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    enumHarmonicType            eNewType;
    CSMVSendVirtualChannel*     pVirtualChannel;
    std::list<CSMVSendVirtualChannel*>::iterator iter;


    // ����������
    if ((NULL == pCurVirtualChannel) || (pCurVirtualChannel->GetName().length() != 3))
    {
        return false;
    }

    // ������������ͨ����Ƶ��
    eNewType = pCurVirtualChannel->GetHarmonic()->GetHarmonicType();
    iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // �������ͨ������ָ��
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
///    ʹͬ������ͨ����г����ֵ���
///
/// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
///
/// @return
///     bool    �ɹ�����true
///
bool CStatus::EqualHarmonicAmplitude( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    double                      dNewAmplitude;
    CSMVSendVirtualChannel*     pVirtualChannel;
    std::list<CSMVSendVirtualChannel*>::iterator iter;


    // ����������
    if ((NULL == pCurVirtualChannel) || (pCurVirtualChannel->GetName().length() != 3))
    {
        return false;
    }

    // ������������ͨ����Ƶ��
    dNewAmplitude = pCurVirtualChannel->GetHarmonic()->GetAmplitude();
    iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // �������ͨ������ָ��
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
///    ʹͬ������ͨ����г��������
///
/// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
///
/// @return
///     bool    �ɹ�����true
///
bool CStatus::EqualHarmonicPhaseAngle( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    double                      dNewPhaseAngle;
    CSMVSendVirtualChannel*     pVirtualChannel;
    std::list<CSMVSendVirtualChannel*>::iterator iter;


    // ����������
    if ((NULL == pCurVirtualChannel) || (pCurVirtualChannel->GetName().length() != 3))
    {
        return false;
    }

    // ������������ͨ����Ƶ��
    dNewPhaseAngle = pCurVirtualChannel->GetHarmonic()->GetPhaseAngle();
    iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // �������ͨ������ָ��
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
///    ���״̬���Խ��
///
/// @return
///     CStateTestResult*    ָ����Խ�������ָ��
///
CStateTestResult* CStatus::GetStateTestResult() const
{
    return m_pStateTestResult;
}

///
/// @brief
///    ����״̬���Խ��
///
/// @param[in]  pNewStateTestResult    ���ò��Խ�������ָ��
///
void CStatus::SetStateTestResult( CStateTestResult* pNewStateTestResult )
{
    m_pStateTestResult = pNewStateTestResult;
}
