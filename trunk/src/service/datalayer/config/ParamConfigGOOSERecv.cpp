/// @file
///
/// @brief
///     GOOSE���ղ�������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.27
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "ParamConfigGOOSERecv.h"
#include "src\utils\MessageCodec\include\goose_types.h"
#include "src\service\datalayer\send\FrameSend.h"
#include "src\service\datalayer\DsmDataLayer.h"

///
/// @brief
///     ���캯��
///
CParamConfigGOOSERecv::CParamConfigGOOSERecv(void)
{
    CGOOSERecvVirtualChannel*   pVirtualChannel   = NULL;
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter;


    for (int i = 0;i < sizeof(strGOOSERecvVCName)/sizeof(strGOOSERecvVCName[0]) ; i++)
    {
        // newһ��GOOSE��������ͨ������
        pVirtualChannel = new CGOOSERecvVirtualChannel;
        if (NULL == pVirtualChannel)
        {
            return ;
        }
        pVirtualChannel->SetName(strGOOSERecvVCName[i]);

        // �������ͨ������������
        iter = m_mapGOOSERecvVirtualChannelGroup.find(strGOOSERecvVCName[i]);
        if (iter != m_mapGOOSERecvVirtualChannelGroup.end())
        {
            delete pVirtualChannel;
            return ;
        }
        m_mapGOOSERecvVirtualChannelGroup.insert(std::pair<std::wstring, CGOOSERecvVirtualChannel*>(strGOOSERecvVCName[i], pVirtualChannel));
    }
}

///
/// @brief
///     ��������
///
CParamConfigGOOSERecv::~CParamConfigGOOSERecv(void)
{
    CGOOSERecvVirtualChannel*   pVirtualChannel   = NULL;
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter;


    // �ͷ�����ͨ������
    iter = m_mapGOOSERecvVirtualChannelGroup.begin();
    while(m_mapGOOSERecvVirtualChannelGroup.end() != iter)
    {
        pVirtualChannel = iter->second;
        if (pVirtualChannel)
        {
            delete pVirtualChannel;
        }

        iter++;
    }

    // �������
    m_mapGOOSERecvVirtualChannelGroup.clear();
}

///
/// @brief
///     ��ȡ����ʵ���ӿ�
///
CParamConfigGOOSERecv* CParamConfigGOOSERecv::getInstance()
{
    static CParamConfigGOOSERecv s_paramConfigGOOSERecv;
    return &s_paramConfigGOOSERecv;
}

///
/// @brief
///    ��������GOOSE��������ͨ������ʵͨ��ӳ���
///    ע�⣺����FirstGOOSERecvVirtualChannel()����ʱ���Զ����ô˺�����
///           �����Ҫ��������ӳ��������ֶ����ô˺���
///
/// @return
///     bool    �ɹ�����true
///
bool CParamConfigGOOSERecv::RebuildGOOSERecvChannelMap()
{
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter;
    CGOOSERecvVirtualChannel*           pVirtualChannel = NULL;
    CGooseCb*                           pGocb           = NULL;
    CChannel*                           pChannel        = NULL;
    CChannel*                           pSubChannel     = NULL;
    int                                 iCount          = 0;


    //
    // �����ǰ������ӳ��
    //
    iter = m_mapGOOSERecvVirtualChannelGroup.begin();
    while (m_mapGOOSERecvVirtualChannelGroup.end() != iter)
    {
        pVirtualChannel = iter->second;
        if (pVirtualChannel)
        {
            pVirtualChannel->m_pChannel = NULL;
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
                if (pChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_STRUCT)
                {
                    pSubChannel = pChannel->FirstSubChannel();
                    while(pSubChannel)
                    {
                        // �����ͨ���ǵ����˫�����ͣ����ұ�ѡ��
                        if ((CHANNEL_TYPE_GOOSE_POINT        == pSubChannel->GetChannelType()   || 
                             CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pSubChannel->GetChannelType())  && 
                             pSubChannel->IsSelected())
                        {
                            iter = m_mapGOOSERecvVirtualChannelGroup.find(strGOOSERecvVCName[iCount]);
                            if ((m_mapGOOSERecvVirtualChannelGroup.end() != iter) && (NULL != iter->second))
                            {
                                // �޸�����ͨ����������ʵͨ��
                                iter->second->m_pChannel = pSubChannel;

                                // �������������ͨ�������ޣ���ô����ѭ��
                                iCount++;
                                if (iCount >= sizeof(strGOOSERecvVCName)/sizeof(strGOOSERecvVCName[0]))
                                {
                                    return true;
                                }
                            }
                        }

                        pSubChannel = pChannel->NextSubChannel(pSubChannel);
                    }
                }
                else
                {
                    // �������ͨ�ĵ����˫��ͨ�������ұ�ѡ��
                    if ((CHANNEL_TYPE_GOOSE_POINT       == pChannel->GetChannelType()   || 
                        CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pChannel->GetChannelType())  && 
                        pChannel->IsSelected())
                    {
                        iter = m_mapGOOSERecvVirtualChannelGroup.find(strGOOSERecvVCName[iCount]);
                        if ((m_mapGOOSERecvVirtualChannelGroup.end() != iter) && (NULL != iter->second))
                        {
                            // �޸�����ͨ����������ʵͨ��
                            iter->second->m_pChannel = pChannel;

                            // �������������ͨ�������ޣ���ô����ѭ��
                            iCount++;
                            if (iCount >= sizeof(strGOOSERecvVCName)/sizeof(strGOOSERecvVCName[0]))
                            {
                                return true;
                            }
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
///    ���GOOSE��������ͨ������ʵͨ��ӳ���
///
void CParamConfigGOOSERecv::ClearGOOSERecvChannelMap()
{
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter;
    CGOOSERecvVirtualChannel*           pVirtualChannel = NULL;
    CGooseCb*                           pGocb           = NULL;
    CChannel*                           pChannel        = NULL;
    CChannel*                           pSubChannel     = NULL;


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
                // ȡ��ͨ����ѡ״̬
                pChannel->Selected(false);

                // ������ͨ��
                pSubChannel = pChannel->FirstSubChannel();
                while(pSubChannel)
                {
                    // ȡ����ͨ����ѡ״̬
                    pSubChannel->Selected(false);
                    pSubChannel = pChannel->NextSubChannel(pSubChannel);
                }

                pChannel = pGocb->NextChannel(pChannel);
            }
        }

        pGocb = NextGocb(pGocb);
    }

    //
    // �����ǰ������ӳ��
    //
    iter = m_mapGOOSERecvVirtualChannelGroup.begin();
    while (m_mapGOOSERecvVirtualChannelGroup.end() != iter)
    {
        pVirtualChannel = iter->second;
        if (pVirtualChannel)
        {
            pVirtualChannel->m_pChannel = NULL;
        }

        iter++;
    }
}

///
/// @brief
///    ö�ٵ�һ��GOOSE��������ͨ��
///
///
/// @return
///     CGOOSERecvVirtualChannel*    ����ͨ��ָ��
///
CGOOSERecvVirtualChannel* CParamConfigGOOSERecv::FirstGOOSERecvVirtualChannel()
{
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter;
    CGOOSERecvVirtualChannel*           pVirtualChannel = NULL;


    // ��������GOOSE��������ͨ������ʵͨ��ӳ���
    if (!RebuildGOOSERecvChannelMap())
    {
        return NULL;
    }

    // ���ҵ�һ������ͨ��
    iter = m_mapGOOSERecvVirtualChannelGroup.begin();
    if (m_mapGOOSERecvVirtualChannelGroup.end() != iter)
    {
        pVirtualChannel = iter->second;
    }

    return pVirtualChannel;
}

///
/// @brief
///    ö����һ��GOOSE��������ͨ��
///
/// @param[in]  pCurVirtualChannel    ָ��ǰ����ͨ����ָ��
///
/// @return
///     CGOOSERecvVirtualChannel*    ��һ������ͨ��ָ�룬ʧ��ʱ����NULL
///
CGOOSERecvVirtualChannel* CParamConfigGOOSERecv::NextGOOSERecvVirtualChannel( CGOOSERecvVirtualChannel* pCurVirtualChannel )
{
    CGOOSERecvVirtualChannel*               pVirtualChannel = NULL;


    // ����������
    if (NULL == pCurVirtualChannel)
    {
        return NULL;
    }

    // ��õ�ǰ����ͨ��λ��
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter = m_mapGOOSERecvVirtualChannelGroup.find(pCurVirtualChannel->GetName());
    if (m_mapGOOSERecvVirtualChannelGroup.end() == iter)
    {
        return NULL;
    }

    // ָ����һ������ͨ��
    if (m_mapGOOSERecvVirtualChannelGroup.end() != ++iter)
    {
        // �������ͨ������ָ��
        pVirtualChannel = iter->second;
    }

    return pVirtualChannel;
}

///
/// @brief
///    ���ָ�����ֵ�����ͨ��
///
/// @param[in]  strVCName    ����ͨ����
///
/// @return
///     CGOOSERecvVirtualChannel*    ָ������ͨ�������ָ�룬ʧ��ʱ����NULL
///
CGOOSERecvVirtualChannel* CParamConfigGOOSERecv::GetVirtualChannelByName( std::wstring strVCName )
{
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter;


    // ����ָ�����ֵ�����ͨ��
    iter = m_mapGOOSERecvVirtualChannelGroup.find(strVCName);
    if (m_mapGOOSERecvVirtualChannelGroup.end() == iter)
    {
        return NULL;
    }

    return (iter->second);
}

///
/// @brief
///    ����ͨ����ʵʱֵ
///
/// @param[in]  iAPPID      ͨ���������ƿ��APPID
/// @param[in]  pGooseMsg   GOOSE���ĵı��������ں�GOOSEͨ��ֵ
/// @param[in]  stFrameInfo GOOSE����ͷ
///
void CParamConfigGOOSERecv::SetChannelRecvValue( int iAPPID, GooseMsg* pGooseMsg, FrameInfo& stFrameInfo )
{
    std::map<std::wstring, CGOOSERecvVirtualChannel*>::iterator iter;
    eFunctionType                       eCurFunctionType;
    CEventTime                          stDITestStartTime;
    CEventTime                          stDINewChangeTime;
    int                                 nIndex          = 0;            ///< ͨ������ͨ�������������ƿ飨��ͨ�����е�����
    bool                                bChangeFlag     = false;        ///< ָʾ�Ƿ���ͨ������ͨ�����Ľ���ֵ�����˸ı�
    CGOOSERecvVirtualChannel*           pVirtualChannel = NULL;         ///< ����ͨ��ָ��
    CChannel*                           pChannel        = NULL;         ///< ͨ��ָ��
    CChannel*                           pParentChannel  = NULL;         ///< ��ͨ��ָ��
    CBaseCb*                            pBaseCb         = NULL;         ///< ���ƿ�ָ��
    GOOSE_DATA*                         pstGooseData    = NULL;         ///< ͨ������ָ��


    // ����������
    if (NULL == pGooseMsg)
    {
        return ;
    }

    // ������ݷ��͹�����
    CFrameSend* pFrameSend = CFrameSend::getInstance();
    if (NULL == pFrameSend)
    {
        return ;
    }

    // ���֡����ʱ��
    stDINewChangeTime.ts_s  = stFrameInfo.timeStamp.ts_sec;
    stDINewChangeTime.ts_ns = stFrameInfo.timeStamp.ts_usec * 1000;

    // �����ǰû�н����ֶ������״̬����
    eCurFunctionType = pFrameSend->GetCurFunctionType();
    if (FUNCTION_TYPE_UNKNOWN == eCurFunctionType)
    {
        // ֹͣ���ݽ���
        CDsmDataLayer* pDsmDataLayer = CDsmDataLayer::getInstance();
        if (pDsmDataLayer)
        {
            pDsmDataLayer->StopRecvData();
        }

        return ;
    }

    // ������������ͨ��
    for (iter = m_mapGOOSERecvVirtualChannelGroup.begin(); m_mapGOOSERecvVirtualChannelGroup.end() != iter; iter++)
    {
        // �������ͨ�������ָ��
        pVirtualChannel = iter->second;
        if (NULL == pVirtualChannel)
        {
            continue;
        }

        // �������ͨ���󶨵�ָ��ͨ������ͨ���������ָ��
        pChannel = pVirtualChannel->GetChannel();
        if (NULL== pChannel)
        {
            continue;
        }

        // ���ͨ������ͨ���������Ŀ��ƿ�
        pBaseCb = pChannel->GetParentBaseCb();
        if (NULL == pBaseCb)
        {
            continue;
        }

        // �жϿ��ƿ��APPID
        if (iAPPID != pBaseCb->GetAPPID())
        {
            continue;
        }

        // �����ʵͨ���Ƿ��и�ͨ��
        pParentChannel = pChannel->GetParentChannel();
        if (pParentChannel)                                             ///< ����и�ͨ��
        {
            // ��ø�ͨ�����������ƿ��е�����
            nIndex = pParentChannel->GetIndex();
            if (-1 == nIndex)
            {
                continue;
            }

            // ��ñ��游ͨ��ֵ�Ľṹ��
            GOOSE_STRUCT* pstGooseStruct = (GOOSE_STRUCT*)pGooseMsg->GetAChannel(nIndex);
            if (NULL == pstGooseStruct)
            {
                continue;
            }

            // �����ͨ����������ͨ���е�����
            nIndex = pChannel->GetIndex();
            if ((-1 == nIndex) || (nIndex >= (int)pstGooseStruct->member.size()))
            {
                continue;
            }

            // ��ñ�����ͨ��ֵ�Ľṹ��
            pstGooseData = pstGooseStruct->member[nIndex];
        }
        else                                                            ///< ���û�и�ͨ��
        {
            // �����ʵͨ�����������ƿ��е�������
            nIndex = pChannel->GetIndex();
            if (-1 == nIndex)
            {
                continue;
            }

            // ��ñ���ͨ��ֵ�Ľṹ��
            pstGooseData = pGooseMsg->GetAChannel(nIndex);
        }

        //
        // �ı���ʵͨ���Ľ���ֵ
        //
        if (pstGooseData)
        {
            GOOSE_DATA_VARIANT stNewGooseData(pstGooseData);

            // ���ֵ�б仯����ô�ı�ͨ����ʵʱֵ
            if (pChannel->GetGooseRecvValue() != stNewGooseData)
            {
                CHistoryData*       pHistoryData        = NULL;
                CStateTestResult*   pStateTestResult    = NULL;


                // ���ݵ�ǰ�������ͣ����ʵ�鿪ʼʱ��Ͳ��Խ��������
                if (FUNCTION_TYPE_MANUAL_TEST == eCurFunctionType)
                {
                    // ������µ��ֶ�������������
                    pStateTestResult    = pFrameSend->LastManualTestResult();
                    stDITestStartTime   = pFrameSend->GetManualTestStartTime();
                }
                else if (FUNCTION_TYPE_STATE_SEQUENCE == eCurFunctionType)
                {
                    // ��õ�ǰ״̬�Ŷ�Ӧ�Ĳ��Խ��������
                    pStateTestResult    = pFrameSend->GetStateSequenceResult(pFrameSend->GetCurStateIndex());
                    stDITestStartTime   = pFrameSend->GetStateSequenceStartTime();
                }

                // �������ֵ�������������
                if (pStateTestResult)
                {
                    // ��ý�����б���ָ������ͨ������ʷֵ�Ķ���
                    CChannelRecvValues* pChannelRecvValues = pStateTestResult->GetChannelRecvValues(pVirtualChannel->GetName());
                    if (pChannelRecvValues)
                    {
                        pChannelRecvValues->SaveRecvValue(stDINewChangeTime - stDITestStartTime, stNewGooseData);
                    }
                }

                // ����ͨ���µ�ʵʱֵ
                pChannel->SetGooseRecvValue(stNewGooseData);
                bChangeFlag = true;
            }
        }
    }

    // �����ͨ����ֵ�����˸ı�
    if (bChangeFlag)
    {
        CFrameSend* pFrameSend = CFrameSend::getInstance();
        if (pFrameSend)
        {
            // �����㷢��ͨ��ֵ�ı���Ϣ
            PostMessage(pFrameSend->GetNotifyHwnd(), WM_DI_CHANGED, 0, 0);
        }
    }
}

///
/// @brief
///    ���������ļ�
///
/// @param[in]  strConfigFile �ļ�·��
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CParamConfigGOOSERecv::LoadConfig( std::wstring strConfigFile )
{
    CMarkup                             xml;                            ///< ����xmlʹ�õ�CMarkUp����


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
bool CParamConfigGOOSERecv::SaveConfig()
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
