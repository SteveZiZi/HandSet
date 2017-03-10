/// @file
///
/// @brief
///     �ֶ������������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.06.07
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "ParamconfigManualTest.h"
#include "ParamConfigSMVSend.h"
#include "src\service\datalayer\config\ParamConfigGOOSERecv.h"
#include "src\service\datalayer\config\ParamConfigBasic.h"

#define NODE_ROOT                       L"config"                       ///< ���ڵ�
#define NODE_SMVChangeItem              L"SMVChangeItem"                ///< SMV�仯��
#define NODE_ModifyEffectiveMode        L"ModifyEffectiveMode"          ///< �޸���Ч��ʽ
#define NODE_StartMode                  L"StartMode"                    ///< ���鿪ʼ��ʽ
#define NODE_VirtualChannelGroup        L"VirtualChannelGroup"          ///< ����ͨ����
#define NODE_VirtualChannel             L"VirtualChannel"               ///< VirtualChannel

#define VIRTUAL_CHANNEL_COUNT           20                              ///< ����ͨ������

///
/// @brief
///     ���캯��
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
///     ��������
///
CParamconfigManualTest::~CParamconfigManualTest(void)
{
    __ClearSMVSendVirtualChannel();
}

///
/// @brief
///     �ͷ�����SMV��������ͨ������
///
void CParamconfigManualTest::__ClearSMVSendVirtualChannel()
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
///     ��ȡ����ʵ���ӿ�
///
CParamconfigManualTest* CParamconfigManualTest::getInstance()
{
    static CParamconfigManualTest s_paramConfigManualTest;
    return &s_paramConfigManualTest;
}

///
/// @brief
///    ���������ļ�
///
/// @param[in]  strConfigFile    �ļ�·��
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CParamconfigManualTest::LoadConfig( std::wstring strConfigFile)
{
    CMarkup                             xml;                            ///< ����xmlʹ�õ�CMarkUp����


    // ����������
    if (strConfigFile.empty())
    {
        return false;
    }
    m_strConfigFile = strConfigFile;

    // ����XML�ļ�
    if (!xml.Load(m_strConfigFile))
    {
        return false;
    }

    // �ͷ�����SMV��������ͨ������
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

    // ���Ϊδ����
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
bool CParamconfigManualTest::SaveConfig()
{
    CMarkup                             xml;                            ///< ����xmlʹ�õ�CMarkUp����
    wchar_t                             cBuffer[128] = {0};


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

    // �������ͨ������
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

    // ���Ϊδ����
    SetModified(false);
    return true;
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
bool CParamconfigManualTest::__AddSMVSendVirtualChannel(CSMVSendVirtualChannel* pVirtualChannel)
{
    // ����������
    if (NULL == pVirtualChannel)
    {
        return false;
    }

    // �������ͨ���Ƿ��Ѵ���
    if (__GetSMVSendVirtualChannel(pVirtualChannel->GetName()) != NULL)
    {
        return false;
    }

    // ������ͨ������ָ�뱣�浽������
    m_listSMVSendVirtualChannelGroup.push_back(pVirtualChannel);
    return true;
}

///
/// @brief
///    ö�ٵ�һ����ӳ���SMV��������ͨ��
///
/// @param[in]  bRebuildMap     �Ƿ���Ҫ�ؽ�SMV����ӳ���GOOSE����ӳ���
///
/// @return
///     CVirtualChannel*    ָ������ͨ�������ָ�룬ʧ��ʱ����NULL
///
CSMVSendVirtualChannel* CParamconfigManualTest::FirstSMVSendVirtualChannel( bool bRebuildMap /*= true*/ )
{
    CSMVSendVirtualChannel*                    pVirtualChannel = NULL;


    if (bRebuildMap)
    {
        // ������������ͨ������ʵͨ��ӳ���
        RebuildSMVSendChannelMap();

        // ���GOOSE���ղ������ù�����������������GOOSE��������ͨ������ʵͨ��ӳ���
        CParamConfigGOOSERecv* pParamConfigGooseRecv = CParamConfigGOOSERecv::getInstance();
        if (pParamConfigGooseRecv)
        {
            pParamConfigGooseRecv->RebuildGOOSERecvChannelMap();
        }
    }

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
CSMVSendVirtualChannel* CParamconfigManualTest::NextSMVSendVirtualChannel(CSMVSendVirtualChannel* pCurVirtualChannel)
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
///    ��������SMV��������ͨ������ʵͨ��ӳ���
///    ע�⣺����FirstSMVSendVirtualChannel()����ʱ���Զ����ô˺�����
///           �����Ҫ��������ӳ��������ֶ����ô˺���
///
/// @return
///     bool    �ɹ�����true
///
bool CParamconfigManualTest::RebuildSMVSendChannelMap()
{
    CSMVSendVirtualChannel*                    pVirtualChannel = NULL;


    std::list<CSMVSendVirtualChannel*>::iterator iter = m_listSMVSendVirtualChannelGroup.begin();
    while (m_listSMVSendVirtualChannelGroup.end() != iter)
    {
        // �������ͨ������ָ��
        pVirtualChannel = *iter;
        if (pVirtualChannel)
        {
            // ����ӳ������ͨ������ʵͨ��������ʵͨ����ָ���������ͨ����������
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
///    ����ӳ�䵽ָ��SMV��������ͨ������ʵͨ��������ʵͨ����ָ���������ͨ����������
///
/// @param[in]  pVirtualChannel    ����ͨ��ָ��
///
/// @return
///     bool    �ɹ�����true
///
bool CParamconfigManualTest::__FillSMVSendChannelGroup(CSMVSendVirtualChannel* pVirtualChannel)
{
    if (NULL == pVirtualChannel)
    {
        return false;
    }

    // �������ͨ��������
    std::wstring strVirtualChannelName = pVirtualChannel->GetName();
    if (strVirtualChannelName.empty())
    {
        return false;
    }

    // ���SMV���Ͳ������ù�����
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return false;
    }

    // ��������ͨ���Ľ�ֱ������
    pVirtualChannel->m_eACDCType = pParamConfigSMVSend->GetCurrentType(strVirtualChannelName);

    //
    // ���ݵ�ǰSMV���ͣ���ȡ��ѡ�п��ƿ���ӳ�䵽��ǰ����ͨ������ʵͨ��
    //
    pVirtualChannel->m_vecChannelGroup.clear();
    switch(pParamConfigSMVSend->GetSMVType())
    {
    case FRAME_TYPE_NET_9_1:                                    ///< ///< IEC61850-9-1
        {
            // ����ÿһ��IEC81850-9-1���ƿ�
            CIEC9_1Cb* pIEC9_1Cb = pParamConfigSMVSend->FirstIEC9_1Cb();
            while(pIEC9_1Cb)
            {
                // ������ƿ鱻��ѡ
                if (pIEC9_1Cb->IsSelected())
                {
                    // ����ÿһ��ͨ��
                    CChannel* pChannel = pIEC9_1Cb->FirstChannel();
                    while(pChannel)
                    {
                        // �������ʵͨ��ӳ�䵽�˵�ǰ����ͨ��
                        if (pChannel->GetVirtualChannel() == strVirtualChannelName)
                        {
                            // �����ʵͨ��ָ�뵽����ͨ������ʵͨ��������
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
            // ����ÿһ��IEC81850-9-2���ƿ�
            CIEC9_2Cb* pIEC9_2Cb = pParamConfigSMVSend->FirstIEC9_2Cb();
            while(pIEC9_2Cb)
            {
                // ������ƿ鱻��ѡ
                if (pIEC9_2Cb->IsSelected())
                {
                    // ����ÿһ��ͨ��
                    CChannel* pChannel = pIEC9_2Cb->FirstChannel();
                    while(pChannel)
                    {
                        // �������ʵͨ��ӳ�䵽�˵�ǰ����ͨ��
                        if (pChannel->GetVirtualChannel() == strVirtualChannelName)
                        {
                            // �����ʵͨ��ָ�뵽����ͨ������ʵͨ��������
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
            // ����ÿһ��FT3���ƿ�
            CFT3Cb* pFt3Cb = pParamConfigSMVSend->FirstFt3Cb();
            while(pFt3Cb)
            {
                // ������ƿ鱻��ѡ
                if (pFt3Cb->IsSelected())
                {
                    // ����ÿһ��ͨ��
                    CChannel* pChannel = pFt3Cb->FirstChannel();
                    while(pChannel)
                    {
                        // �������ʵͨ��ӳ�䵽�˵�ǰ����ͨ��
                        if (pChannel->GetVirtualChannel() == strVirtualChannelName)
                        {
                            // �����ʵͨ��ָ�뵽����ͨ������ʵͨ��������
                            pVirtualChannel->m_vecChannelGroup.push_back(pChannel);
                        }
                        pChannel = pFt3Cb->NextChannel(pChannel);
                    }
                }
                pFt3Cb = pParamConfigSMVSend->NextFt3Cb(pFt3Cb);
            }
        }
        break;

    case FRAME_TYPE_FT3_GUOWANG:                                ///< FT3��չ
        {
            // ����ÿһ��FT3��չ���ƿ�
            CFT3ExpandCb* pFt3ExpandCb = pParamConfigSMVSend->FirstFt3ExpandCb();
            while(pFt3ExpandCb)
            {
                // ������ƿ鱻��ѡ
                if (pFt3ExpandCb->IsSelected())
                {
                    // ����ÿһ��ͨ��
                    CChannel* pChannel = pFt3ExpandCb->FirstChannel();
                    while(pChannel)
                    {
                        // �������ʵͨ��ӳ�䵽�˵�ǰ����ͨ��
                        if (pChannel->GetVirtualChannel() == strVirtualChannelName)
                        {
                            // �����ʵͨ��ָ�뵽����ͨ������ʵͨ��������
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

    // �������ѡ��ͨ�����������ޣ���ô����Ϊ0
    if (pVirtualChannel->GetSelectedIndex() >= (int)pVirtualChannel->m_vecChannelGroup.size())
    {
        pVirtualChannel->SetSelectedIndex(0);
    }

    return true;
}

///
/// @brief
///    ���SMV�仯��
///
/// @return
///     eSMVChangeItem    SMV�仯��
///
eSMVChangeItem CParamconfigManualTest::GetSMVChangeItem()
{
    return m_eSMVChangeItem;
}

///
/// @brief
///    ����SMV�仯��
///
/// @param[in]  newSMVChangeItem    �µ�SMV�仯��
///
/// @return
///     bool    �ɹ�����true
///
bool CParamconfigManualTest::SetSMVChangeItem( eSMVChangeItem newSMVChangeItem )
{
    if (m_eSMVChangeItem != newSMVChangeItem)
    {
        m_eSMVChangeItem = newSMVChangeItem;

        // �����޸ı�ʶ
        SetModified(true);
    }

    return true;
}

///
/// @brief
///    ����޸���Ч��ʽ
///
/// @return
///     eModifyEffectiveMode    �޸���Ч��ʽ
///
eModifyEffectiveMode CParamconfigManualTest::GetModifyEffectiveMode()
{
    return m_eModifyEffectiveMode;
}

///
/// @brief
///    �����޸���Ч��ʽ
///
/// @param[in]  newModifyEffectiveMode    �µ��޸���Ч��ʽ
///
/// @return
///     bool    �ɹ�����true
///
bool CParamconfigManualTest::SetModifyEffectiveMode( eModifyEffectiveMode newModifyEffectiveMode )
{
    if (m_eModifyEffectiveMode != newModifyEffectiveMode)
    {
        m_eModifyEffectiveMode = newModifyEffectiveMode;

        // �����޸ı�ʶ
        SetModified(true);
    }

    return true;
}

///
/// @brief
///    ������鿪ʼ��ʽ
///
/// @return
///     eStartMode    ���鿪ʼ��ʽ
///
eStartMode CParamconfigManualTest::GetStartMode()
{
    return m_eStartMode;
}

///
/// @brief
///    �������鿪ʼ��ʽ
///
/// @param[in]  newStartMode    �µ����鿪ʼ��ʽ
///
/// @return
///     bool    �ɹ�����true
///
bool CParamconfigManualTest::SetStartMode( eStartMode newStartMode )
{
    if (m_eStartMode != newStartMode)
    {
        m_eStartMode = newStartMode;

        // �����޸ı�ʶ
        SetModified(true);
    }

    return true;
}

///
/// @brief
///    ���ָ�����ֵ�����ͨ��
///
/// @param[in]  strChannelName    ����ͨ����
///
/// @return
///     CSMVSendVirtualChannel*    ָ������ͨ�������ָ��
///
CSMVSendVirtualChannel* CParamconfigManualTest::__GetSMVSendVirtualChannel( std::wstring strChannelName )
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
///    �ȷ�ֵ
///
/// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
///
/// @return
///     bool    �ɹ�����true��ʧ�ܷ���false
///
bool CParamconfigManualTest::EqualAmplitude( CSMVSendVirtualChannel* pCurVirtualChannel )
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
///    ����ָ�����ֵ�����ͨ���ķ�ֵ��һ��ֵ��
///
/// @param[in]  strChannelName    ����ͨ����
/// @param[in]  dNewAmplitude    �µķ�ֵ��һ��ֵ��
///
/// @return
///     bool    �ɹ�����true
///
bool CParamconfigManualTest::__SetSMVSendVCPrimaryAmplitude( std::wstring strChannelName, double dNewAmplitude )
{
    CSMVSendVirtualChannel*     pVirtualChannel;


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
bool CParamconfigManualTest::RatedAmplitude( CSMVSendVirtualChannel* pCurVirtualChannel )
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
bool CParamconfigManualTest::ThreePhaseBalance( CSMVSendVirtualChannel* pCurVirtualChannel )
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
bool CParamconfigManualTest::EqualPhaseAngle( CSMVSendVirtualChannel* pCurVirtualChannel )
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
bool CParamconfigManualTest::__SetSMVSendVCPhaseAngle( std::wstring strChannelName, double dNewPhaseAngle )
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
bool CParamconfigManualTest::PhaseAnglePositiveSequence( CSMVSendVirtualChannel* pCurVirtualChannel )
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
bool CParamconfigManualTest::PhaseAngleReverseSequence( CSMVSendVirtualChannel* pCurVirtualChannel )
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
///     double    ���ֵ
///
double CParamconfigManualTest::__GetSMVSendVCPhaseAngle( std::wstring strChannelName )
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
bool CParamconfigManualTest::EqualFrequency( CSMVSendVirtualChannel* pCurVirtualChannel )
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
bool CParamconfigManualTest::SetAllVCFrequency( double dNewFrequency )
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
