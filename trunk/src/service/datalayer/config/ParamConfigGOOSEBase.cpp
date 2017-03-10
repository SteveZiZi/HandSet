/// @file
///
/// @brief
///     GOOSE�������û���
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.27
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "ParamConfigGOOSEBase.h"
#include "src/service/datalayer/utility/data_utility.h"


///
/// @brief
///     ���캯��
///
CParamConfigGOOSEBase::CParamConfigGOOSEBase(void)
{
}

///
/// @brief
///     ��������
///
CParamConfigGOOSEBase::~CParamConfigGOOSEBase(void)
{
    // �������GOOSE���ƿ�
    ClearAllGocb();
}

///
/// @brief
///    ���GOOSE���ƿ�
///
/// @param[in]  pNewGocb ָ��GOOSE���ƿ�����ָ��
///
/// @return
///     bool    true��ʾ��ӳɹ�
///
bool CParamConfigGOOSEBase::__AddGocb(CGooseCb* pNewGocb)
{
    if (NULL == pNewGocb)
    {
        return false;
    }

    // ���GOOSE���ƿ鵽������
    m_listGocbGroup.push_back(pNewGocb);
    return true;
}

///
/// @brief
///    ���GOOSE���ƿ�
///
/// @return
///     CGocb*    ����ӵ�GOOSE���ƿ�����ָ��
///
CGooseCb* CParamConfigGOOSEBase::AddGocb()
{
    // newһ��GOOSE���ƿ����
    CGooseCb*  pGocb = new CGooseCb(this);
    if (NULL == pGocb)
    {
        return NULL;
    }

    //
    // �����ǰ����GOOSE���ƿ飬��ô�������һ�����ƿ�
    //
    if (m_listGocbGroup.size() > 0)
    {
        // �����Ѵ��ڵ����һ�����ƿ�
        pGocb->__Copy(LastGocb());

        // APPID��1
        pGocb->m_iAPPID += 1;

        // MAC��ַĩ�ֽڼ�1
        pGocb->m_ucDesMAC[ETHERNET_MAC_LEN - 1] = pGocb->m_ucDesMAC[ETHERNET_MAC_LEN - 1] + 1;
    }

    // ���Թ�ѡ���ƿ�
    if (GetSelectedGocbCount() < MSGPARAM_GOOSE_SEND_CTRL_SELECTED_MAX)
    {
        pGocb->Selected(true);
    }

    // ���GOOSE���ƿ鵽������
    if (!__AddGocb(pGocb))
    {
        delete pGocb;
        return NULL;
    }

    // ���Ϊ���޸�
    SetModified(true);
    return pGocb;
}

///
/// @brief
///     ���GOOSE���ƿ飨�˺�������ȿ���һ���β���ָ����Ȼ����ӵ��ڲ������У�
///
/// @param[in]
///     pGocb   -   GOOSE���ƿ�ָ��
///
/// @return
///     CGooseCb*   �ɹ�ʱ��������ӵ�GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
///
CGooseCb* CParamConfigGOOSEBase::AddGocb(CGooseCb* pGocb)
{
    CGooseCb*       pNewGocb    = NULL;


    // ����������
    if (NULL == pGocb)
    {
        return NULL;
    }

    // ���ָ��APPID�Ŀ��ƿ��Ƿ��Ѵ���
    if (__GetGocb(pGocb->GetAPPID()))
    {
        return NULL;
    }

    // newһ��GOOSE���ƿ�
    pNewGocb = new CGooseCb(this);
    if (NULL == pNewGocb)
    {
        return NULL;
    }

    // ��ȿ���GOOSE���ƿ�
    if (!pNewGocb->__Copy(pGocb))
    {
        delete pNewGocb;
        return NULL;
    }

    // ���Թ�ѡ���ƿ�
    if (GetSelectedGocbCount() < MSGPARAM_GOOSE_SEND_CTRL_SELECTED_MAX)
    {
        pNewGocb->Selected(true);
    }

    // ���µ�GOOSE���ƿ���ӵ�������
    if (!__AddGocb(pNewGocb))
    {
        delete pNewGocb;
        return NULL;
    }
    
    // ���Ϊ���޸�
    SetModified(true);
    return pNewGocb;
}

///
/// @brief
///     ���ɨ���б��е�GOOSE���ƿ鵽�����У��˺�������ȿ���һ���β���ָ����Ȼ����ӵ��ڲ������У�
///
/// @param[in]
///     pDetectedGooseItem    -   ɨ���б��е�GOOSE���ƿ�ָ��
///
/// @return
///     CGooseCb*    �ɹ�ʱ��������ӵ�GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
///
CGooseCb* CParamConfigGOOSEBase::AddGocb(CDetectItemFrame* pDetectedGooseItem)
{
    CGooseCb*       pNewGocb    = NULL;
    GooseMsg*       pGooseMsg   = NULL;
    bool            bFlag       = false;


    // ����������
    if (NULL == pDetectedGooseItem)
    {
        return NULL;
    }

    // ���ָ��APPID�Ŀ��ƿ��Ƿ��Ѵ���
    CNetDetectItem* pNetDetectItem  = (CNetDetectItem*)pDetectedGooseItem->GetDetectItem();
    if ((NULL == pNetDetectItem) || __GetGocb(pNetDetectItem->appId))
    {
        return NULL;
    }

    // newһ��GOOSE���ƿ�
    pNewGocb = new CGooseCb(this);
    if (NULL == pNewGocb)
    {
        return NULL;
    }

    // �����շ��˿�
    pNewGocb->m_ePortName   = pNetDetectItem->enPortName;

    // ����GOOSE���ĵı����ӿڶ���
    pGooseMsg = CreateGooseMsgImpl();
    if (NULL == pGooseMsg)
    {
        bFlag = true;
        goto ExitHandle;
    }

    // ��Goose����֡���н���
    if (0 == pGooseMsg->Decode(pDetectedGooseItem->GetFrameData(), pDetectedGooseItem->GetFrameLen()))
    {
        bFlag = true;
        goto ExitHandle;
    }

    // ��ȿ���ɨ���б��е�GOOSE���ƿ�
    if (!pNewGocb->__Copy(pGooseMsg))
    {
        bFlag = true;
        goto ExitHandle;
    }

    // ���Թ�ѡ���ƿ�
    if (GetSelectedGocbCount() < MSGPARAM_GOOSE_SEND_CTRL_SELECTED_MAX)
    {
        pNewGocb->Selected(true);
    }

    // ���µ�GOOSE���ƿ���ӵ�������
    if (!__AddGocb(pNewGocb))
    {
        bFlag = true;
        goto ExitHandle;
    }

ExitHandle:
    // ����д������
    if (bFlag)
    {
        // ������ʧ�ܣ���ô�ͷſ��ƿ����
        if (pNewGocb)
        {
            delete pNewGocb;
            pNewGocb = NULL;
        }
    }
    else
    {
        // ���Ϊ���޸�
        SetModified(true);
    }

    // �ͷ�GOOSE���ĵı����ӿڶ���
    if(pGooseMsg)
    {
        ReleaseGooseMsgImpl(pGooseMsg);
    }

    return pNewGocb;
}

///
/// @brief
///    ɾ��GOOSE���ƿ�
///
/// @param[in]  pCurGocb GOOSE���ƿ�ָ��
///
/// @return
///     bool    true��ʾɾ���ɹ�
///
bool CParamConfigGOOSEBase::DeleteGocb(CGooseCb* pCurGocb)
{
    // ����������
    if (NULL == pCurGocb)
    {
        return false;
    }

    // �����������ͷŶ���
    std::list<CGooseCb*>::iterator iter = m_listGocbGroup.begin();
    while (m_listGocbGroup.end() != iter)
    {
        CGooseCb* pGocb = *iter;
        if (pGocb == pCurGocb)
        {
            m_listGocbGroup.remove(pGocb);
            delete pGocb;

            // ���Ϊ���޸�
            SetModified(true);
            return true;
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    ɾ��ָ��APPID��GOOSE���ƿ�
///
/// @param[in]  iAPPID ��ɾ���Ŀ��ƿ�APPID
///
/// @return
///     bool    true��ʾɾ���ɹ�
///
bool CParamConfigGOOSEBase::DeleteGocb(int iAPPID)
{
    // APPID���ڵ���1
    if (0 >= iAPPID)
    {
        return false;
    }

    // �����������ͷŶ���
    std::list<CGooseCb*>::iterator iter = m_listGocbGroup.begin();
    while (m_listGocbGroup.end() != iter)
    {
        CGooseCb* pGocb = *iter;
        if (pGocb  && (pGocb->GetAPPID() == iAPPID))
        {
            m_listGocbGroup.remove(pGocb);
            delete pGocb;

            // ���Ϊ���޸�
            SetModified(true);
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    �������GOOSE���ƿ�
///
void CParamConfigGOOSEBase::ClearAllGocb()
{
    // �����������ͷŶ���
    std::list<CGooseCb*>::iterator iter = m_listGocbGroup.begin();
    while (m_listGocbGroup.end() != iter)
    {
        CGooseCb* pGocb = *iter;
        if (pGocb)
        {
            delete pGocb;
        }

        iter++;
    }

    if (m_listGocbGroup.size())
    {
        // �������
        m_listGocbGroup.clear();

        // ���Ϊ���޸�
        SetModified(true);
    }
}

///
/// @brief
///    ö�ٵ�һ��GOOSE���ƿ�
///
/// @return
///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
///
CGooseCb* CParamConfigGOOSEBase::FirstGocb()
{
    std::list<CGooseCb*>::iterator iter = m_listGocbGroup.begin();
    if (m_listGocbGroup.end() != iter)
    {
        return *iter;
    }

    return NULL;
}

///
/// @brief
///    ö����һ��GOOSE���ƿ�
///
/// @param[in]  pCurGocb ��ǰλ��GOOSE���ƿ��ָ��
///
/// @return
///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
///
CGooseCb* CParamConfigGOOSEBase::NextGocb(CGooseCb* pCurGocb)
{
    std::list<CGooseCb*>::iterator  iter;
    
    
    iter = m_listGocbGroup.begin();
    while (m_listGocbGroup.end() != iter)
    {
        // ���ҵ�ǰGOOSE���ƿ��λ��
        if (*iter == pCurGocb)
        {
            // �����һ��GOOSE���ƿ��λ��
            if (m_listGocbGroup.end() == ++iter)
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
///    ö����һ��GOOSE���ƿ�
///
/// @param[in]  pCurGocb ��ǰλ��GOOSE���ƿ��ָ��
///
/// @return
///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
///
CGooseCb* CParamConfigGOOSEBase::PrevGocb(CGooseCb* pCurGocb)
{
    std::list<CGooseCb*>::reverse_iterator  rIter;


    rIter = m_listGocbGroup.rbegin();
    while (m_listGocbGroup.rend() != rIter)
    {
        // ���ҵ�ǰGOOSE���ƿ��λ��
        if (*rIter == pCurGocb)
        {
            // �����һ��GOOSE���ƿ��λ��
            if (m_listGocbGroup.rend() == ++rIter)
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
///    ö�����һ��GOOSE���ƿ�
///
/// @return
///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
///
CGooseCb* CParamConfigGOOSEBase::LastGocb()
{
    std::list<CGooseCb*>::reverse_iterator rIter = m_listGocbGroup.rbegin();
    if (m_listGocbGroup.rend() != rIter)
    {
        return *rIter;
    }

    return NULL;
}

///
/// @brief
///     ���ָ��APPID��GOOSE���ƿ飨ע����ͬAPPID�Ŀ��ƿ�����ж�����˴�ֻ��õ�һ��ƥ��Ŀ��ƿ飩
///
/// @param[in]  
///     iAPPID  -   ���ƿ��APPID
///
/// @return
///     CGooseCb*   �ɹ�ʱ���ؿ��ƿ��ָ�룬ʧ��ʱ����NULL
///
CGooseCb* CParamConfigGOOSEBase::__GetGocb(int iAPPID)
{
    CGooseCb*                       pGocb   = NULL;
    std::list<CGooseCb*>::iterator  iter;


    iter = m_listGocbGroup.begin();
    while (m_listGocbGroup.end() != iter)
    {
        pGocb = *iter;
        if (pGocb && (pGocb->GetAPPID() == iAPPID))
        {
            return pGocb;
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    ��ÿ��ƿ����
///
/// @return
///     int    ���ƿ����
///
int CParamConfigGOOSEBase::GetGocbCount()
{
    return (int)m_listGocbGroup.size();
}

///
/// @brief
///    ��ñ�ѡ�еĿ��ƿ����
///
/// @return
///     int    ��ѡ�еĿ��ƿ����
///
int CParamConfigGOOSEBase::GetSelectedGocbCount()
{
    int                             iCount  = 0;
    CGooseCb*                       pGocb   = NULL;
    std::list<CGooseCb*>::iterator  iter;


    iter = m_listGocbGroup.begin();
    while (m_listGocbGroup.end() != iter)
    {
        pGocb = *iter;

        // �жϿ��ƿ��Ƿ�ѡ��
        if (pGocb && pGocb->IsSelected())
        {
            iCount++;
        }

        iter++;
    }

    return iCount;
}
