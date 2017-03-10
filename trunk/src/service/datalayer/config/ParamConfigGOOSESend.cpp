/// @file
///
/// @brief
///     GOOSE��������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.27
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "ParamConfigGOOSESend.h"

///
/// @brief
///     ����ͨ����
///
std::wstring strGOOSESendVCName[] =
{
    L"DO1",
    L"DO2",
    L"DO3",
    L"DO4",
    L"DO5",
    L"DO6"
};

///
/// @brief
///     ���캯��
///
CParamConfigGOOSESend::CParamConfigGOOSESend(void)
:m_iHeartbeatInterval(5000)
,m_iMinimumInterval(2)
{
    CGOOSESendVirtualChannel*   pVirtualChannel   = NULL;
    std::map<std::wstring, CGOOSESendVirtualChannel*>::iterator iter;


    for (int i = 0;i < sizeof(strGOOSESendVCName)/sizeof(strGOOSESendVCName[0]) ; i++)
    {
        // newһ��GOOSE��������ͨ������
        pVirtualChannel = new CGOOSESendVirtualChannel;
        if (NULL == pVirtualChannel)
        {
            return ;
        }
        pVirtualChannel->SetName(strGOOSESendVCName[i]);

        // �������ͨ������������
        iter = m_mapGOOSESendVirtualChannelGroup.find(strGOOSESendVCName[i]);
        if (iter != m_mapGOOSESendVirtualChannelGroup.end())
        {
            delete pVirtualChannel;
            return ;
        }
        m_mapGOOSESendVirtualChannelGroup.insert(std::pair<std::wstring, CGOOSESendVirtualChannel*>(strGOOSESendVCName[i], pVirtualChannel));
    }
}

///
/// @brief
///     ��������
///
CParamConfigGOOSESend::~CParamConfigGOOSESend(void)
{
    CGOOSESendVirtualChannel*   pVirtualChannel   = NULL;
    std::map<std::wstring, CGOOSESendVirtualChannel*>::iterator iter;


    // �ͷ�����ͨ������
    iter = m_mapGOOSESendVirtualChannelGroup.begin();
    while(m_mapGOOSESendVirtualChannelGroup.end() != iter)
    {
        pVirtualChannel = iter->second;
        if (pVirtualChannel)
        {
            delete pVirtualChannel;
        }

        iter++;
    }

    // �������
    m_mapGOOSESendVirtualChannelGroup.clear();
}

///
/// @brief
///     ��ȡ����ʵ���ӿ�
///
CParamConfigGOOSESend* CParamConfigGOOSESend::getInstance()
{
    static CParamConfigGOOSESend s_paramConfigGOOSESend;
    return &s_paramConfigGOOSESend;
}

///
/// @brief
///    ���������ļ�
///
/// @param[in]  strConfigFile �����ļ�����·��
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CParamConfigGOOSESend::LoadConfig( std::wstring strConfigFile )
{
    CMarkup                             xml;                            ///< ����xmlʹ�õ�CMarkUp����


    if (strConfigFile.empty())
    {
        return false;
    }
    m_strConfigFile = strConfigFile;

    // ���������ļ�
    if (!xml.Load(m_strConfigFile))
    {
        return false;
    }

    // �������GOOSE���ƿ�
    ClearAllGocb();

    //
    // find and inside <config>
    //
    if (!xml.FindElem(NODE_ROOT) || !xml.IntoElem())
    {
        return false;
    }

    //
    // find <HeartbeatInterval>
    //
    if(!xml.FindElem(NODE_HeartbeatInterval))
    {
        return false;
    }
    m_iHeartbeatInterval = _wtoi(xml.GetData().c_str());

    //
    // find <MinimumInterval>
    //
    if(!xml.FindElem(NODE_MinimumInterval))
    {
        return false;
    }
    m_iMinimumInterval = _wtoi(xml.GetData().c_str());

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

            // ����GOOSE���ƿ�ѡ��״̬
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

            // ����GOOSE���ƿ�
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

            // ���GOOSE���ƿ����ָ�뵽����
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

    // ���Ϊδ�޸�
    SetModified(false);
    return true;
}

///
/// @brief
///    ���������ļ�
///
/// @return
///     bool    true��ʾ����ɹ�
///
bool CParamConfigGOOSESend::SaveConfig()
{
    CMarkup                             xml;                            ///< ����xmlʹ�õ�CMarkUp����


    // ���û���κθĶ�����ô������
    if (!m_bModified)
    {
        return true;
    }

    // add and inside <config>
    if (!xml.AddElem(NODE_ROOT) || !xml.IntoElem())
    {
        return false;
    }

    // <HeartbeatInterval>
    if(!xml.AddElem(NODE_HeartbeatInterval) || !xml.SetData(m_iHeartbeatInterval))
    {
        return false;
    }

    // <MinimumInterval>
    if(!xml.AddElem(NODE_MinimumInterval) || !xml.SetData(m_iMinimumInterval))
    {
        return false;
    }

    //
    // ������������������GOOSE���ƿ�
    //
    if (m_listGocbGroup.size() > 0)
    {
        // <GocbGroup>
        if (!xml.AddElem(NODE_GocbGroup) || !xml.IntoElem())
        {
            return false;
        }

        // ����GOOSE���ƿ�
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

                // ����GOOSE���ƿ�
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

    // ���xmlͷ
    xml.ResetPos();
    xml.InsertNode( CMarkup::MNT_PROCESSING_INSTRUCTION, L"xml" );
    xml.SetAttrib( L"version", L"1.0" );
    xml.SetAttrib( L"encoding", L"utf-8" );

    // ����xml�ļ�
    if (!xml.Save(m_strConfigFile))
    {
        return false;
    }

    // ���Ϊδ�޸�
    SetModified(false);
    return true;
}

///
/// @brief
///    ����������T0
///
/// @return
///     int     �������(ms)
///
int CParamConfigGOOSESend::GetHeartbeatInterval()
{
    return m_iHeartbeatInterval;
}

///
/// @brief
///    �����������T0
///
/// @param[in]  iNewInterval �������T0(ms)
///
/// @return
///     bool    true��ʾ����ɹ�
///
bool CParamConfigGOOSESend::SetHeartbeatInterval(int iNewInterval)
{
    if ((iNewInterval < 500) || (iNewInterval > 65535))
    {
        return false;
    }

    if (m_iHeartbeatInterval != iNewInterval)
    {
        m_iHeartbeatInterval = iNewInterval;

        // ���Ϊ���޸�
        SetModified(true);
    }

    return true;
}

///
/// @brief
///    �����С���T1
///
/// @return
///     int     ��С���(ms)��ʧ��ʱ����-1
///
int CParamConfigGOOSESend::GetMinimumInterval()
{
    return m_iMinimumInterval;
}

///
/// @brief
///    ������С���T1
///
/// @param[in]  iNewInterval ��С���T1(ms)
///
/// @return
///     bool    true��ʾ����ɹ�
///
bool CParamConfigGOOSESend::SetMinimumInterval(int iNewInterval)
{
    if ((iNewInterval < 1) || (iNewInterval > 65535))
    {
        return false;
    }

    if (m_iMinimumInterval != iNewInterval)
    {
        m_iMinimumInterval = iNewInterval;

        // ���Ϊ���޸�
        SetModified(true);
    }

    return true;
}

///
/// @brief
///    ö�ٵ�һ��GOOSE��������ͨ��
///
///
/// @return
///     CGOOSESendVirtualChannel*    ����ͨ��ָ�룬ʧ��ʱ����NULL
///
CGOOSESendVirtualChannel* CParamConfigGOOSESend::FirstGOOSESendVirtualChannel()
{
    std::map<std::wstring, CGOOSESendVirtualChannel*>::iterator iter;


    // ��������GOOSE��������ͨ������ʵͨ��ӳ���
    RebuildGOOSESendChannelMap();

    // ���ҵ�һ������ͨ��
    iter = m_mapGOOSESendVirtualChannelGroup.begin();
    if (m_mapGOOSESendVirtualChannelGroup.end() != iter)
    {
        return (iter->second);
    }

    return NULL;
}

///
/// @brief
///    ö����һ��GOOSE��������ͨ��
///
/// @param[in]  pCurVirtualChannel    ָ��ǰ����ͨ����ָ��
///
/// @return
///     CGOOSESendVirtualChannel*    ��һ������ͨ��ָ�룬ʧ��ʱ����NULL
///
CGOOSESendVirtualChannel* CParamConfigGOOSESend::NextGOOSESendVirtualChannel( CGOOSESendVirtualChannel* pCurVirtualChannel )
{
    CGOOSESendVirtualChannel*               pVirtualChannel = NULL;


    // ����������
    if (NULL == pCurVirtualChannel)
    {
        return NULL;
    }

    // ��õ�ǰ����ͨ��λ��
    std::map<std::wstring, CGOOSESendVirtualChannel*>::iterator iter = m_mapGOOSESendVirtualChannelGroup.find(pCurVirtualChannel->GetName());
    if (m_mapGOOSESendVirtualChannelGroup.end() == iter)
    {
        return NULL;
    }

    // ָ����һ������ͨ��
    if (m_mapGOOSESendVirtualChannelGroup.end() != ++iter)
    {
        // �������ͨ������ָ��
        pVirtualChannel = iter->second;
    }

    return pVirtualChannel;
}

///
/// @brief
///    ��������GOOSE��������ͨ������ʵͨ��ӳ���
///    ע�⣺����FirstGOOSESendVirtualChannel()����ʱ���Զ����ô˺�����
///           �����Ҫ��������ӳ��������ֶ����ô˺���
///
/// @return
///     bool    �ɹ�����true
///
bool CParamConfigGOOSESend::RebuildGOOSESendChannelMap()
{
    std::map<std::wstring, CGOOSESendVirtualChannel*>::iterator iter;
    CGooseCb*                           pGocb           = NULL;
    CChannel*                           pChannel        = NULL;
    CChannel*                           pSubChannel     = NULL;
    CGOOSESendVirtualChannel*           pVirtualChannel = NULL;


    // �����ǰ��ӳ��
    iter = m_mapGOOSESendVirtualChannelGroup.begin();
    while (m_mapGOOSESendVirtualChannelGroup.end() != iter)
    {
        pVirtualChannel = iter->second;
        if (pVirtualChannel)
        {
            pVirtualChannel->m_vecChannelGroup.clear();
        }

        iter++;
    }

    //
    // �������п��ƿ飬��������ͨ��ӳ���
    //
    pGocb = FirstGocb();
    while(pGocb)
    {
        // ������ƿ鱻ѡ��
        if (pGocb->IsSelected())
        {
            pChannel = pGocb->FirstChannel();
            while(pChannel)
            {
                // ����ǽṹ�����͵�ͨ��
                enumChannelType eChannelType = pChannel->GetChannelType();
                if (eChannelType == CHANNEL_TYPE_GOOSE_STRUCT)
                {
                    pSubChannel = pChannel->FirstSubChannel();
                    while(pSubChannel)
                    {
                        if ((pSubChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_POINT) ||
                            (pSubChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_DOUBLE_POINT))
                        {
                            // �����ͨ������������ͨ��
                            std::wstring strVCName  = pSubChannel->GetVirtualChannel();
                            if (strVCName.size())
                            {
                                iter = m_mapGOOSESendVirtualChannelGroup.find(strVCName);
                                if ((m_mapGOOSESendVirtualChannelGroup.end() != iter) && (NULL != iter->second))
                                {
                                    // �������ͨ����������ʵͨ��
                                    iter->second->m_vecChannelGroup.push_back(pSubChannel);
                                }
                            }
                        }

                        pSubChannel = pChannel->NextSubChannel(pSubChannel);
                    }
                }
                else if((CHANNEL_TYPE_GOOSE_POINT       == eChannelType) ||
                    (CHANNEL_TYPE_GOOSE_DOUBLE_POINT    == eChannelType))
                {
                    // �������ͨͨ��������������ͨ����
                    std::wstring strVCName  = pChannel->GetVirtualChannel();
                    if (strVCName.size())
                    {
                        iter = m_mapGOOSESendVirtualChannelGroup.find(strVCName);
                        if ((m_mapGOOSESendVirtualChannelGroup.end() != iter) && (NULL != iter->second))
                        {
                            // �������ͨ����������ʵͨ��
                            iter->second->m_vecChannelGroup.push_back(pChannel);
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
///    ����DO�����GOOSE��������ͨ������
///
/// @param[in]  strChannelName    GOOSE��������ͨ��������
///
/// @return
///     CGOOSESendVirtualChannel*    ָ��GOOSE��������ͨ�������ָ��
///
CGOOSESendVirtualChannel* CParamConfigGOOSESend::GetGOOSESendVirtualChannel( std::wstring strChannelName )
{
    CGOOSESendVirtualChannel*                   pVirtualChannel = NULL;
    std::map<std::wstring, CGOOSESendVirtualChannel*>::iterator iter;


    // ����ָ�����ֵ�����ͨ��
    iter = m_mapGOOSESendVirtualChannelGroup.find(strChannelName);
    if ((m_mapGOOSESendVirtualChannelGroup.end() != iter))
    {
        pVirtualChannel = iter->second;
        if (pVirtualChannel && (pVirtualChannel->GetName() == strChannelName))
        {
            return pVirtualChannel;
        }
    }

    return NULL;
}

///
/// @brief
///     ����ӳ�䵽ָ������ͨ����������ʵͨ����ֵ
///
/// @param[in]  mapDoChangedArray   ����ͨ������ֵ
///
/// @return
///     bool    �ɹ�����true
///
bool CParamConfigGOOSESend::UpdateGooseChannelSendValue( std::map<std::wstring, bool>& mapDoChangedArray)
{
    CGOOSESendVirtualChannel*   pVirtualChannel = NULL;
    std::map<std::wstring, bool>::iterator iterDoChanged;
    std::map<std::wstring, CGOOSESendVirtualChannel*>::iterator iterVirtualChannel;


    // ����ÿһ��ֵ�����˸ı��DOͨ��
    for (iterDoChanged = mapDoChangedArray.begin(); iterDoChanged != mapDoChangedArray.end(); iterDoChanged++)
    {
        std::wstring    strDoName   = iterDoChanged->first;
        bool            bNewValue   = iterDoChanged->second;


        // ����ָ�����ֵ�GOOSE��������ͨ��
        iterVirtualChannel = m_mapGOOSESendVirtualChannelGroup.find(strDoName);
        if (m_mapGOOSESendVirtualChannelGroup.end() == iterVirtualChannel)
        {
            return false;
        }

        // ���GOOSE��������ͨ�������ָ��
        pVirtualChannel = iterVirtualChannel->second;
        if ((NULL == pVirtualChannel) || (pVirtualChannel->GetName() != strDoName))
        {
            return false;
        }

        // ��������ӳ�䵽������ͨ������ʵͨ����ֵ
        if (!pVirtualChannel->SetChannelSendValue(bNewValue))
        {
            return false;
        }
    }

    return true;
}
