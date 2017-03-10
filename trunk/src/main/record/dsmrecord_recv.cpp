/**
 * \file
 *      dsmrecord_recv.cpp
 *
 * \brief
 *      ����ҵ���õ�record��ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
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

    // û������ƿ���Ϣ�򲻼����б�
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

    // ��ʼ����ѡͨ���������б�
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


