/// @file
///
/// @brief
///     GOOSE参数配置基类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.27
///
/// 修改历史：
///

#include "StdAfx.h"
#include "ParamConfigGOOSEBase.h"
#include "src/service/datalayer/utility/data_utility.h"


///
/// @brief
///     构造函数
///
CParamConfigGOOSEBase::CParamConfigGOOSEBase(void)
{
}

///
/// @brief
///     析构函数
///
CParamConfigGOOSEBase::~CParamConfigGOOSEBase(void)
{
    // 清除所有GOOSE控制块
    ClearAllGocb();
}

///
/// @brief
///    添加GOOSE控制块
///
/// @param[in]  pNewGocb 指向GOOSE控制块对象的指针
///
/// @return
///     bool    true表示添加成功
///
bool CParamConfigGOOSEBase::__AddGocb(CGooseCb* pNewGocb)
{
    if (NULL == pNewGocb)
    {
        return false;
    }

    // 添加GOOSE控制块到容器中
    m_listGocbGroup.push_back(pNewGocb);
    return true;
}

///
/// @brief
///    添加GOOSE控制块
///
/// @return
///     CGocb*    新添加的GOOSE控制块对象的指针
///
CGooseCb* CParamConfigGOOSEBase::AddGocb()
{
    // new一个GOOSE控制块对象
    CGooseCb*  pGocb = new CGooseCb(this);
    if (NULL == pGocb)
    {
        return NULL;
    }

    //
    // 如果当前存在GOOSE控制块，那么拷贝最后一个控制块
    //
    if (m_listGocbGroup.size() > 0)
    {
        // 拷贝已存在的最后一个控制块
        pGocb->__Copy(LastGocb());

        // APPID加1
        pGocb->m_iAPPID += 1;

        // MAC地址末字节加1
        pGocb->m_ucDesMAC[ETHERNET_MAC_LEN - 1] = pGocb->m_ucDesMAC[ETHERNET_MAC_LEN - 1] + 1;
    }

    // 尝试勾选控制块
    if (GetSelectedGocbCount() < MSGPARAM_GOOSE_SEND_CTRL_SELECTED_MAX)
    {
        pGocb->Selected(true);
    }

    // 添加GOOSE控制块到容器中
    if (!__AddGocb(pGocb))
    {
        delete pGocb;
        return NULL;
    }

    // 标记为已修改
    SetModified(true);
    return pGocb;
}

///
/// @brief
///     添加GOOSE控制块（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
///
/// @param[in]
///     pGocb   -   GOOSE控制块指针
///
/// @return
///     CGooseCb*   成功时返回新添加的GOOSE控制块指针，失败时返回NULL
///
CGooseCb* CParamConfigGOOSEBase::AddGocb(CGooseCb* pGocb)
{
    CGooseCb*       pNewGocb    = NULL;


    // 检查输入参数
    if (NULL == pGocb)
    {
        return NULL;
    }

    // 检查指定APPID的控制块是否已存在
    if (__GetGocb(pGocb->GetAPPID()))
    {
        return NULL;
    }

    // new一个GOOSE控制块
    pNewGocb = new CGooseCb(this);
    if (NULL == pNewGocb)
    {
        return NULL;
    }

    // 深度拷贝GOOSE控制块
    if (!pNewGocb->__Copy(pGocb))
    {
        delete pNewGocb;
        return NULL;
    }

    // 尝试勾选控制块
    if (GetSelectedGocbCount() < MSGPARAM_GOOSE_SEND_CTRL_SELECTED_MAX)
    {
        pNewGocb->Selected(true);
    }

    // 将新的GOOSE控制块添加到容器中
    if (!__AddGocb(pNewGocb))
    {
        delete pNewGocb;
        return NULL;
    }
    
    // 标记为已修改
    SetModified(true);
    return pNewGocb;
}

///
/// @brief
///     添加扫描列表中的GOOSE控制块到容器中（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
///
/// @param[in]
///     pDetectedGooseItem    -   扫描列表中的GOOSE控制块指针
///
/// @return
///     CGooseCb*    成功时返回新添加的GOOSE控制块指针，失败时返回NULL
///
CGooseCb* CParamConfigGOOSEBase::AddGocb(CDetectItemFrame* pDetectedGooseItem)
{
    CGooseCb*       pNewGocb    = NULL;
    GooseMsg*       pGooseMsg   = NULL;
    bool            bFlag       = false;


    // 检查输入参数
    if (NULL == pDetectedGooseItem)
    {
        return NULL;
    }

    // 检查指定APPID的控制块是否已存在
    CNetDetectItem* pNetDetectItem  = (CNetDetectItem*)pDetectedGooseItem->GetDetectItem();
    if ((NULL == pNetDetectItem) || __GetGocb(pNetDetectItem->appId))
    {
        return NULL;
    }

    // new一个GOOSE控制块
    pNewGocb = new CGooseCb(this);
    if (NULL == pNewGocb)
    {
        return NULL;
    }

    // 设置收发端口
    pNewGocb->m_ePortName   = pNetDetectItem->enPortName;

    // 创建GOOSE报文的编解码接口对象
    pGooseMsg = CreateGooseMsgImpl();
    if (NULL == pGooseMsg)
    {
        bFlag = true;
        goto ExitHandle;
    }

    // 对Goose报文帧进行解码
    if (0 == pGooseMsg->Decode(pDetectedGooseItem->GetFrameData(), pDetectedGooseItem->GetFrameLen()))
    {
        bFlag = true;
        goto ExitHandle;
    }

    // 深度拷贝扫描列表中的GOOSE控制块
    if (!pNewGocb->__Copy(pGooseMsg))
    {
        bFlag = true;
        goto ExitHandle;
    }

    // 尝试勾选控制块
    if (GetSelectedGocbCount() < MSGPARAM_GOOSE_SEND_CTRL_SELECTED_MAX)
    {
        pNewGocb->Selected(true);
    }

    // 将新的GOOSE控制块添加到容器中
    if (!__AddGocb(pNewGocb))
    {
        bFlag = true;
        goto ExitHandle;
    }

ExitHandle:
    // 如果有错误出现
    if (bFlag)
    {
        // 如果添加失败，那么释放控制块对象
        if (pNewGocb)
        {
            delete pNewGocb;
            pNewGocb = NULL;
        }
    }
    else
    {
        // 标记为已修改
        SetModified(true);
    }

    // 释放GOOSE报文的编解码接口对象
    if(pGooseMsg)
    {
        ReleaseGooseMsgImpl(pGooseMsg);
    }

    return pNewGocb;
}

///
/// @brief
///    删除GOOSE控制块
///
/// @param[in]  pCurGocb GOOSE控制块指针
///
/// @return
///     bool    true表示删除成功
///
bool CParamConfigGOOSEBase::DeleteGocb(CGooseCb* pCurGocb)
{
    // 检查输入参数
    if (NULL == pCurGocb)
    {
        return false;
    }

    // 遍历容器，释放对象
    std::list<CGooseCb*>::iterator iter = m_listGocbGroup.begin();
    while (m_listGocbGroup.end() != iter)
    {
        CGooseCb* pGocb = *iter;
        if (pGocb == pCurGocb)
        {
            m_listGocbGroup.remove(pGocb);
            delete pGocb;

            // 标记为已修改
            SetModified(true);
            return true;
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    删除指定APPID的GOOSE控制块
///
/// @param[in]  iAPPID 待删除的控制块APPID
///
/// @return
///     bool    true表示删除成功
///
bool CParamConfigGOOSEBase::DeleteGocb(int iAPPID)
{
    // APPID大于等于1
    if (0 >= iAPPID)
    {
        return false;
    }

    // 遍历容器，释放对象
    std::list<CGooseCb*>::iterator iter = m_listGocbGroup.begin();
    while (m_listGocbGroup.end() != iter)
    {
        CGooseCb* pGocb = *iter;
        if (pGocb  && (pGocb->GetAPPID() == iAPPID))
        {
            m_listGocbGroup.remove(pGocb);
            delete pGocb;

            // 标记为已修改
            SetModified(true);
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    清除所有GOOSE控制块
///
void CParamConfigGOOSEBase::ClearAllGocb()
{
    // 遍历容器，释放对象
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
        // 清空容器
        m_listGocbGroup.clear();

        // 标记为已修改
        SetModified(true);
    }
}

///
/// @brief
///    枚举第一个GOOSE控制块
///
/// @return
///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
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
///    枚举下一个GOOSE控制块
///
/// @param[in]  pCurGocb 当前位置GOOSE控制块的指针
///
/// @return
///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
///
CGooseCb* CParamConfigGOOSEBase::NextGocb(CGooseCb* pCurGocb)
{
    std::list<CGooseCb*>::iterator  iter;
    
    
    iter = m_listGocbGroup.begin();
    while (m_listGocbGroup.end() != iter)
    {
        // 查找当前GOOSE控制块的位置
        if (*iter == pCurGocb)
        {
            // 获得下一个GOOSE控制块的位置
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
///    枚举上一个GOOSE控制块
///
/// @param[in]  pCurGocb 当前位置GOOSE控制块的指针
///
/// @return
///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
///
CGooseCb* CParamConfigGOOSEBase::PrevGocb(CGooseCb* pCurGocb)
{
    std::list<CGooseCb*>::reverse_iterator  rIter;


    rIter = m_listGocbGroup.rbegin();
    while (m_listGocbGroup.rend() != rIter)
    {
        // 查找当前GOOSE控制块的位置
        if (*rIter == pCurGocb)
        {
            // 获得上一个GOOSE控制块的位置
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
///    枚举最后一个GOOSE控制块
///
/// @return
///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
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
///     获得指定APPID的GOOSE控制块（注意相同APPID的控制块可能有多个，此处只获得第一个匹配的控制块）
///
/// @param[in]  
///     iAPPID  -   控制块的APPID
///
/// @return
///     CGooseCb*   成功时返回控制块的指针，失败时返回NULL
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
///    获得控制块个数
///
/// @return
///     int    控制块个数
///
int CParamConfigGOOSEBase::GetGocbCount()
{
    return (int)m_listGocbGroup.size();
}

///
/// @brief
///    获得被选中的控制块个数
///
/// @return
///     int    被选中的控制块个数
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

        // 判断控制块是否被选中
        if (pGocb && pGocb->IsSelected())
        {
            iCount++;
        }

        iter++;
    }

    return iCount;
}
