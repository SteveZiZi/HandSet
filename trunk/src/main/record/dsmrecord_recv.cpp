/**
 * \file
 *      dsmrecord_recv.cpp
 *
 * \brief
 *      接收业务共用的record类实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/2
 */

#include "stdafx.h"

#include "src/main/resourcestr.h"
#include "src/service/datalayer/config/smvrecvconfig.h"


CDsmSmvPhasorRecord::CDsmSmvPhasorRecord( CBaseDetectItem* pCtrlBlk, CString csCaption, CString csValue )
{
    CELRecordItem* pItemCaption = AddItem(new CDsmRecordItem(csCaption));
    CELRecordItem* pItemVal     = AddItem(new CDsmRecordItem(csValue));

    pItemVal->SetEditable(TRUE);
    pItemCaption->SetEditable(FALSE);

    // 没传入控制块信息则不加载列表
    if(!pCtrlBlk)
    {
        return;
    }

    CSmvRecvConfig* pConfig = pCtrlBlk->pRecvConfig;

    if(!pConfig)
    {
        return;
    }

    CSmvRecvChannelInfo* pInfo      = NULL;
    size_t nCounts                  = pConfig->GetChannelCount();
    CELRecordItemEditOptions* pEdit = pItemVal->GetEditOptions(NULL);
    pEdit->AddComboButton();

    // 初始化可选通道的下拉列表
    for(size_t i = 0; i < nCounts; i++)
    {
        pInfo = pConfig->GetOneChannelInfo(i);

        if(CHANNEL_TYPE_SMV_VOLTAGE != pInfo->enChannelType &&
            CHANNEL_TYPE_SMV_CURRENT != pInfo->enChannelType)
        {
            continue;
        }

        pEdit->AddConstraint(pInfo->strChannelDesc.c_str(), pInfo->nIndex);
    }
}

CDsmSmvPackeInfoRecord::CDsmSmvPackeInfoRecord( LPCTSTR lpszTime, LPCTSTR lpszType, int nLen, int nIndex )
{
    AddItem(new CDsmRecordItem(nIndex + 1));
    AddItem(new CDsmRecordItem(lpszTime));
    AddItem(new CDsmRecordItem(lpszType));
    AddItem(new CDsmRecordItem(nLen));

    m_nPacketIndex = nIndex;
}


