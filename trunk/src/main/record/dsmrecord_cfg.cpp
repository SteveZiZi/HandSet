/**
 * \file
 *      dsmrecord_cfg.cpp
 *
 * \brief
 *      报文参数配置记录实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/2
 */

#include "stdafx.h"


#include "src/main/resourcestr.h"
#include "src/service/datalayer/dsmdatalayer.h"
#include "src/service/datalayer/config/Channel.h"
#include "src/utils/MessageCodec/include/goose_types.h"
#include "src/service/datalayer/config/smvrecvconfig.h"
#include "src/utils/CommonControl/MacEditCtrl/MacEditCtrl.h"
#include "src/utils/reportcontrol/listctrlel/elrecorditemtext.h"


CDsmBasicSmvSendRecord::CDsmBasicSmvSendRecord( CString csCaption, CString csContent, int nRecordType, CString csMargin /*= L""*/ )
{
    CELRecordItem * pItem = AddItem(new CELRecordItemText(csMargin));
    pItem->SetItemData(nRecordType);

    pItem = AddItem(new CELRecordItemText(csCaption));
    pItem->SetEditable(FALSE);
    pItem = AddItem(new CELRecordItemText(csContent));
    pItem->SetEditable(TRUE);
}

CDsmBasicSmvSendRecord::CDsmBasicSmvSendRecord( CString csCaption, int nContent, int nRecordType, CString csMargin /*= L""*/ )
{
    CString csContent;
    csContent.Format(L"%d", nContent);

    CELRecordItem * pItem = AddItem(new CELRecordItemText(csMargin));
    pItem->SetItemData(nRecordType);
    AddItem(new CELRecordItemText(csCaption));
    AddItem(new CELRecordItemText(csContent));
}

CDsmStatusWordSetRecord::CDsmStatusWordSetRecord( CString csCaption, CString csContent)
{
    AddItem(new CELRecordItemText(csCaption));
    AddItem(new CELRecordItemText(csContent));
}

CDsmBasicACDCSetRecord::CDsmBasicACDCSetRecord(CString csCaption1, eCurrentType nValue1)//, CString csCaption2, eCurrentType nValue2)
{
    CString csContent;

    //
    //电压
    //
    AddItem(new CELRecordItemText(csCaption1));

    //直流设置
    if (nValue1 == CURRENT_TYPE_DC)
    {
        csContent = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_DC);
    }
    else
    {
        csContent = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_AC);
    }
    CELRecordItemText* pItem = (CELRecordItemText*)AddItem(new CELRecordItemText(csContent));
    pItem->HasCheckbox(TRUE);
    pItem->SetEditable(TRUE);
    if (nValue1 == CURRENT_TYPE_DC)
    {
        pItem->SetChecked(TRUE);
    }

    ////电流
    //AddItem(new CELRecordItemText(csCaption2));

    ////直流设置
    //if (nValue2 == CURRENT_TYPE_DC)
    //{
    //    csContent = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_DC);
    //}
    //else
    //{
    //    csContent = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_AC);
    //}

    //pItem = (CELRecordItemText*)AddItem(new CELRecordItemText(csContent));
    //pItem->HasCheckbox(TRUE);
    //pItem->SetEditable(TRUE);

    //if (nValue2 == CURRENT_TYPE_DC)
    //{
    //    pItem->SetChecked(TRUE);
    //}
}


CDsmEditSmvSendChanRecord::CDsmEditSmvSendChanRecord(CChannel* pChannel, int nIndex)
{
    if (pChannel == NULL)
    {
        return;
    }
    //索引
    CString strIndex;
    strIndex.Format(L"%d", nIndex);

    CELRecordItemText* pItem = (CELRecordItemText*)AddItem(new CDsmRecordItemText(strIndex));

    //通道描述

    pItem = (CELRecordItemText*)AddItem(new CELRecordItemText(pChannel->GetDescription().c_str()));
    pItem->SetEditable(TRUE);

    //通道类型
    pItem = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
    pItem->SetEditable(TRUE);
    pItem->GetEditOptions(NULL)->AddComboButton();
    pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UNKNOWN),CHANNEL_TYPE_UNKOWN);
    pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_AMPER),CHANNEL_TYPE_SMV_CURRENT );
    pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_VOLT),CHANNEL_TYPE_SMV_VOLTAGE );
    pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_TIMETICK), CHANNEL_TYPE_SMV_TIME );
    switch(pChannel->GetChannelType())
    {
    case CHANNEL_TYPE_SMV_CURRENT:
        {
            if (pChannel->IsMeasure())
            {
                pItem->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_AMPER_MEASURE));
            }
            else
            {
                pItem->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_AMPER));
            }
        }
        break;
    case CHANNEL_TYPE_SMV_VOLTAGE:
        {
            pItem->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_VOLT));
        }
        break;
    case CHANNEL_TYPE_SMV_TIME:
        {
            pItem->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_TIMETICK));
        }
        break;
    default:
        {
            pItem->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UNKNOWN));
            pChannel->SetChannelType(CHANNEL_TYPE_UNKOWN);
        }
    }

    //相位
    pItem = (CDsmRecordItemText*)AddItem(new CDsmRecordItemText(_T("")));
    pItem->GetEditOptions(NULL)->AddComboButton();
    pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_UNKNOWN),CHANNEL_PHASE_UNKOWN);
    pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_A),CHANNEL_PHASE_A );
    pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_B),CHANNEL_PHASE_B );
    pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_C), CHANNEL_PHASE_C );
    pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_X),CHANNEL_PHASE_X );
    pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_N), CHANNEL_PHASE_N );

    CELRecordItemConstraint* pItemCon = pItem->GetEditOptions(NULL)->FindConstraint (pChannel->GetChannelPhase());
    if(pItemCon && (pChannel->GetChannelType() == CHANNEL_TYPE_SMV_CURRENT || pChannel->GetChannelType() == CHANNEL_TYPE_SMV_VOLTAGE))
    {
        pItem->SetValue(pItemCon->GetConstraintCaption());
        pItem->SetEditable(TRUE);
    }
    else
    {
        pItem->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_UNKNOWN));
        pChannel->SetChannelPhase(CHANNEL_PHASE_UNKOWN);
        pItem->SetEditable(FALSE);
    }

    //
    //一次值 二次值
    //
    CString strValue = _T("");
    double dValue = 0.0;

    CELRecordItemText* pItemOnce = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
    CELRecordItemText* pItemTwice = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
    pItemOnce->SetEditable(TRUE);
    pItemTwice->SetEditable(TRUE);

    switch(pChannel->GetChannelType())
    {
    case CHANNEL_TYPE_SMV_CURRENT:
        {
            dValue = pChannel->GetOnceRatings();
            strValue.Format(_T("%.03lf"), dValue);
            pItemOnce->SetValue(strValue);
            pItemOnce->SetFormatString(_T("%s A"));

            dValue = pChannel->GetSecondaryRatings();
            strValue.Format(_T("%.03lf"), dValue);
            pItemTwice->SetValue(strValue);
            pItemTwice->SetFormatString(_T("%s A"));

        }
        break;
    case CHANNEL_TYPE_SMV_VOLTAGE:
        {
            dValue = pChannel->GetOnceRatings();
            dValue = dValue/1000.0;
            strValue.Format(_T("%.03lf"), dValue);
            pItemOnce->SetValue(strValue);
            pItemOnce->SetFormatString(_T("%s kV"));

            dValue = pChannel->GetSecondaryRatings();
            strValue.Format(_T("%.03lf"), dValue);
            pItemTwice->SetValue(strValue);
            pItemTwice->SetFormatString(_T("%s V"));
        }
        break;
    case CHANNEL_TYPE_SMV_TIME:
        {

            dValue = pChannel->GetOnceRatings();

            strValue.Format(_T("%.03lf"), dValue);
            pItemOnce->SetValue(strValue);
            pItemOnce->SetFormatString(_T("%s μs"));

            dValue = pChannel->GetSecondaryRatings();
            strValue.Format(_T("%.03lf"), dValue);
            pItemTwice->SetValue(strValue);
            pItemTwice->SetFormatString(_T("%s μs"));
        }
        break;
    default:
        {
            strValue = _T("1.000");
            pItemOnce->SetValue(strValue);
            pItemTwice->SetValue(strValue);
        }
        break;
    }

    //映射
    pItem = (CDsmRecordItemText*)AddItem(new CDsmRecordItemText(_T("")));
    pItem->GetEditOptions(NULL)->AddComboButton();

    switch(pChannel->GetChannelType())
    {
    case CHANNEL_TYPE_SMV_CURRENT:
        {
            pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Ia1"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Ib1"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Ic1"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Ix1"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Ia2"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Ib2"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Ic2"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Ix2"));
        }
        break;
    case CHANNEL_TYPE_SMV_VOLTAGE:
        {
            pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Ua1"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Ub1"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Uc1"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Ux1"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Ua2"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Ub2"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Uc2"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("Ux2"));
        }
        break;
    case CHANNEL_TYPE_SMV_TIME:
        {
            pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("t1"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("t2"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("t3"));
            pItem->GetEditOptions(NULL)->AddConstraint(_T("t4"));
        }
        break;
    default:
        {
        }
        break;
    }

    pItemCon = pItem->GetEditOptions(NULL)->FindConstraint (pChannel->GetVirtualChannel().c_str());
    if(pItemCon)
    {
        pItem->SetValue(pItemCon->GetConstraintCaption());
        pItem->SetEditable(TRUE);
    }
    else
    {
        pItem->SetValue(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
        pChannel->SetVirtualChannel((LPCTSTR)dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
        if (pChannel->GetChannelType() != CHANNEL_TYPE_UNKOWN)
        {
            pItem->SetEditable(TRUE);
        }
    }

}

CDsmEditSmvSendCtrlBlockRecord::CDsmEditSmvSendCtrlBlockRecord( CString csCaption, CString csContent, int nRecordType)
{
    CELRecordItem* pItem = AddItem(new CELRecordItemText(csCaption));
    pItem->SetItemData(nRecordType);
    AddItem(new CDsmRecordItemText(csContent));

}

CDsmEditSmvSendCtrlBlockRecord::CDsmEditSmvSendCtrlBlockRecord( CString csCaption, int nContent, int nRecordType)
{
    CELRecordItem* pItem = AddItem(new CELRecordItemText(csCaption));
    pItem->SetItemData(nRecordType);

    CString csContent;
    csContent.Format(L"%d", nContent);
    AddItem(new CDsmRecordItemText(csContent));
}


CDsmSelSmvCtrlFromSclRecord::CDsmSelSmvCtrlFromSclRecord(CIEC9_2Cb* pIEC92Cb)
{
    ASSERT(pIEC92Cb);
    if (pIEC92Cb == NULL)
    {
        return;
    }
    CString strTemp = _T("");
    CELRecordItem* pItem = NULL;
   

    //appid
    strTemp = _T("");
    strTemp.Format( _T("0x%04X"), pIEC92Cb->GetAPPID());
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);
    //通道数
    strTemp.Format(_T("%d"), pIEC92Cb->GetChannelCount());
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);
    //
    pItem = AddItem(new CELRecordItemText(pIEC92Cb->GetDescription().c_str()));
    pItem->SetFocusable(FALSE);

    //端口
    //switch(pIEC92Cb->GetPortName())
    //{
    //case NET_PORT_LIGHT_A:
    //    {
    //        strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_NET_PORT_TX1);
    //    }
    //    break;
    //case NET_PORT_LIGHT_B:
    //    {
    //        strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_NET_PORT_TX2);
    //    }
    //    break;
    //case FT3_PORT_LIGHT_C:
    //    {
    //        strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_SER_PORT_TX);
    //    }
    //    break;
    //default:
    //    {
    //        strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN);
    //    }
    //    break;
    //}
    //
    //端口默认显示为 “--”
    //
    strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN);
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);


	
	//checkbox
	pItem = (CELRecordItemText* )AddItem(new CELRecordItemText(_T("")));
	pItem->HasCheckbox(TRUE);
	pItem->SetChecked(pIEC92Cb->IsSelected());
	pItem->SetEditable(TRUE);
}

CDsmSelSmvCtrlFromScanRecord::CDsmSelSmvCtrlFromScanRecord(CNetDetectItem* pNetDetectItem)
{
    if (pNetDetectItem == NULL)
    {
        return;
    }

    CString strTemp = _T("");

	CELRecordItem* pItem = NULL;

    //appid
    strTemp = _T("");
    strTemp.Format( _T("0x%04X"), pNetDetectItem->appId);
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);

    //通道数
    strTemp.Format(_T("%d"), pNetDetectItem->nChannelCount);
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);

    //
    CMacEditCtrl::MacAddr2Str(pNetDetectItem->macAddr, strTemp);
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);

    //端口
    switch(pNetDetectItem->enPortName)
    {
    case NET_PORT_LIGHT_A:
        {
            strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_NET_PORT_TX1);
        }
        break;
    case NET_PORT_LIGHT_B:
        {
            strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_NET_PORT_TX2);
        }
        break;
    case FT3_PORT_LIGHT_C:
        {
            strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_SER_PORT_TX);
        }
        break;
    case NET_PORT_ELECTRONIC:
        {
            strTemp = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ELEC_PORT);
        }
        break;
    default:
        {
            strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN);
        }
        break;
    }
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);


	//checkbox
	pItem = AddItem(new CELRecordItemText(_T("")));
	pItem->HasCheckbox(TRUE);
	pItem->SetEditable(TRUE);
}



CDsmSelSmvCtrlFromScanRecord::CDsmSelSmvCtrlFromScanRecord(CFt3DetectItem* pFt3DetectItem)
{
    if (pFt3DetectItem == NULL)
    {
        return;
    }

    CString strTemp = _T("");

	CELRecordItem* pItem = NULL;
    
    //appid
    strTemp = _T("");
    strTemp.Format( _T("0x%04X"), pFt3DetectItem->nLDName);
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);

    //通道数
    strTemp.Format(_T("%d"), pFt3DetectItem->nChannelCount);
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);

    //
    //CMacEditCtrl::MacAddr2Str(pFt3DetectItem->macAddr, strTemp);
    pItem = AddItem(new CELRecordItemText(_T("")));
    pItem->SetFocusable(FALSE);

    //端口
    switch(pFt3DetectItem->enPortName)
    {
    case NET_PORT_LIGHT_A:
        {
            strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_NET_PORT_TX1);
        }
        break;
    case NET_PORT_LIGHT_B:
        {
            strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_NET_PORT_TX2);
        }
        break;
    case FT3_PORT_LIGHT_C:
        {
            strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_SER_PORT_TX);
        }
        break;
        //case FT3_PORT_LIGHT_C:
        //    {
        //        strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_SER_PORT_TX);
        //    }
        break;
    default:
        {
            strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN);
        }
        break;
    }
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);

	//checkbox
	pItem = AddItem(new CELRecordItemText(_T("")));
	pItem->HasCheckbox(TRUE);
	pItem->SetEditable(TRUE);
}


CDsmBasicGooseSendRecord::CDsmBasicGooseSendRecord( CString csCaption, CString csContent, CString csMargin /*= L""*/ )
{
    AddItem(new CELRecordItemText(csMargin));
    AddItem(new CELRecordItemText(csCaption));
    AddItem(new CELRecordItemText(csContent));
}

CDsmBasicGooseSendRecord::CDsmBasicGooseSendRecord( CString csCaption, int nContent, CString csMargin /*= L""*/ )
{
    CString csContent;
    csContent.Format(L"%d", nContent);

    AddItem(new CELRecordItemText(csMargin));
    AddItem(new CELRecordItemText(csCaption));
    AddItem(new CELRecordItemText(csContent));
}

CDsmGooseCtrlInfoRecord::CDsmGooseCtrlInfoRecord( CString csCaption, CString csContent )
{
    AddItem(new CELRecordItemText(csCaption));
    AddItem(new CELRecordItemText(csContent));
}

CDsmGooseCtrlInfoRecord::CDsmGooseCtrlInfoRecord( CString csCaption, int nContent)
{
    CString csContent;
    csContent.Format(L"%d", nContent);

    AddItem(new CELRecordItemText(csCaption));
    AddItem(new CELRecordItemText(csContent));
}

CDsmSelGooseCtrlFromSclRecord::CDsmSelGooseCtrlFromSclRecord(CGooseCb* pGooseCb, eDsmGooseType eGooseType)
{
    ASSERT(pGooseCb);
    if (pGooseCb == NULL)
    {
        return;
    }
    CString strTemp = _T("");
	CELRecordItem* pItem = NULL;

    

    //appid
    strTemp = _T("");
    strTemp.Format( _T("0x%04X"), pGooseCb->GetAPPID());
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);

    //通道数
    strTemp.Format(_T("%d"), pGooseCb->GetChannelCount());
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);

    //
    pItem = AddItem(new CELRecordItemText(pGooseCb->GetDescription().c_str()));
    pItem->SetFocusable(FALSE);

    //端口
    //switch(pGooseCb->GetPortName())
    //{
    //case NET_PORT_LIGHT_A:
    //    {
    //        if (eGooseType == eGooseSend)
    //        {
    //            strTemp = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NET_PORT_TX1);
    //        }
    //        else
    //        {
    //            strTemp = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NET_PORT_RX1);
    //        }
    //    }
    //    break;
    //case NET_PORT_LIGHT_B:
    //    {
    //        if (eGooseType == eGooseSend)
    //        {
    //            strTemp = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NET_PORT_TX2);
    //        }
    //        else
    //        {
    //            strTemp = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NET_PORT_RX2);
    //        }
    //    }
    //    break;
    //default:
    //    {
    //        strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN);
    //    }
    //    break;
    //}
    //
    //端口默认显示 “--”
    //
    strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN);
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);

	//checkbox
	pItem = AddItem(new CELRecordItemText(_T("")));
	pItem->HasCheckbox(TRUE);
	pItem->SetChecked(pGooseCb->IsSelected());
	pItem->SetEditable(TRUE);
}


CDsmSelGooseCtrlFromScanRecord::CDsmSelGooseCtrlFromScanRecord(CNetDetectItem* pNetDetectItem, eDsmGooseType eGooseType)
{
    if (pNetDetectItem == NULL)
    {
        return;
    }

    CString strTemp = _T("");
	CELRecordItem* pItem = NULL;
    
    //appid
    strTemp = _T("");
    strTemp.Format( _T("0x%04X"), pNetDetectItem->appId);
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);

    //通道数
    strTemp.Format(_T("%d"), pNetDetectItem->nChannelCount);
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);

    //
    CMacEditCtrl::MacAddr2Str(pNetDetectItem->macAddr, strTemp);
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);

    //端口
    switch(pNetDetectItem->enPortName)
    {
    case NET_PORT_LIGHT_A:
        {
            if (eGooseType == eGooseSend)
            {
                strTemp = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NET_PORT_TX1);
            }
            else
            {
                strTemp = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NET_PORT_RX1);
            }
        }
        break;
    case NET_PORT_LIGHT_B:
        {
            if (eGooseType == eGooseSend)
            {
                strTemp = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NET_PORT_TX2);
            }
            else
            {
                strTemp = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NET_PORT_RX2);
            }
        }
        break;
    case NET_PORT_ELECTRONIC:
        {
            strTemp = dsm_stringMgr::Get(NET_PORT_ELECTRONIC);
        }
        break;
    default:
        {
            strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN);
        }
        break;
    }
    pItem = AddItem(new CELRecordItemText(strTemp));
    pItem->SetFocusable(FALSE);


	//checkbox
	pItem = AddItem(new CELRecordItemText(_T("")));
	pItem->HasCheckbox(TRUE);
	pItem->SetEditable(TRUE);

}



CDsmEditGooseSendChannelRecord::CDsmEditGooseSendChannelRecord( CChannel* pChannel, int nIndex, int nSubIndex)
{
    if (pChannel == NULL)
    {
        return;
    }

    CString strIndex = _T("");
    if (nSubIndex > 0)
    {
        strIndex.Format(_T("%d.%d"), nIndex, nSubIndex);
    }
    else
    {
        strIndex.Format(_T("%d"), nIndex);
    }

    //索引
    AddItem(new CELRecordItemText(strIndex));
    //描述
    CELRecordItem* pItemDesc = AddItem(new CELRecordItemText(pChannel->GetDescription().c_str()));
    pItemDesc->SetEditable(TRUE);


    //类型
    CELRecordItemText* pItemType = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));

    pItemType->SetEditable(TRUE);
    pItemType->GetEditOptions(NULL)->AddComboButton();
    if (nSubIndex == 0)
    {
        pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_STRUCT),CHANNEL_TYPE_GOOSE_STRUCT );
    }
    pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_POINT),CHANNEL_TYPE_GOOSE_POINT );
    pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_DUALPOINT),CHANNEL_TYPE_GOOSE_DOUBLE_POINT );
    pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_QUALITY),CHANNEL_TYPE_GOOSE_QUALITY );
    pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_TIMETICK),CHANNEL_TYPE_GOOSE_TIME );
    pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_FLOAT),CHANNEL_TYPE_GOOSE_FLOAT );
    pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_INT8),CHANNEL_TYPE_GOOSE_INT8 );
    pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_INT16),CHANNEL_TYPE_GOOSE_INT16 );
    pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_INT24),CHANNEL_TYPE_GOOSE_INT24 );
    pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_INT32),CHANNEL_TYPE_GOOSE_INT32 );
    pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UINT8),CHANNEL_TYPE_GOOSE_UINT8 );
    pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UINT16),CHANNEL_TYPE_GOOSE_UINT16 );
    pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UINT24),CHANNEL_TYPE_GOOSE_UINT24 );
    pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UINT32),CHANNEL_TYPE_GOOSE_UINT32 );

    CELRecordItemConstraint* pItemCon = pItemType->GetEditOptions(NULL)->FindConstraint (pChannel->GetChannelType());
    if (pItemCon)
    {
        pItemType->SetValue(pItemCon->GetConstraintCaption());
    }
    else
    {
        pItemType->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UNKNOWN));
    }
    pItemType->SetEditable(TRUE);

    //映射
    CELRecordItemText* pItemMap = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));

    CString strTemp = _T("");
    pItemMap->GetEditOptions(NULL)->AddComboButton();
    pItemMap->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MAP_NULL));
    for (int i = 0; i < 6; ++i)
    {
        strTemp.Format(_T("DO%d"), i+1);
        pItemMap->GetEditOptions(NULL)->AddConstraint(strTemp);
    }

    if (pChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_POINT || 
        pChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_DOUBLE_POINT)
    {
        pItemMap->SetEditable(TRUE);
        if (pChannel->GetVirtualChannel()!= _T(""))
        { 
            pItemCon = pItemMap->GetEditOptions(NULL)->FindConstraint (pChannel->GetVirtualChannel().c_str());
            if (pItemCon)
            {
                pItemMap->SetValue(pItemCon->GetConstraintCaption());
            }
            else
            {
                pItemMap->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UNKNOWN));
            }
        }
        else
        {
            pItemMap->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UNKNOWN));
        }
    }
    else
    {
        pItemMap->SetEditable(FALSE);
    }
}

CDsmBasicGooseRecRecord::CDsmBasicGooseRecRecord( CString csCaption, CString csContent, CString csMargin /*= L""*/ )
{
    AddItem(new CELRecordItemText(csMargin));
    AddItem(new CELRecordItemText(csCaption));
    AddItem(new CELRecordItemText(csContent));
}



CDsmEditGooseRecChannelRecord::CDsmEditGooseRecChannelRecord( CChannel* pChannel, int nIndex, int nSubIndex)
{
    if (pChannel == NULL)
    {
        return;
    }

    CString strIndex = _T("");
    if (nSubIndex > 0)
    {
        strIndex.Format(_T("%d.%d"), nIndex, nSubIndex);
    }
    else
    {
        strIndex.Format(_T("%d"), nIndex);
    }

    //索引
    AddItem(new CELRecordItemText(strIndex));

    //描述
    AddItem(new CELRecordItemText(pChannel->GetDescription().c_str()));


    //通道类型
    CString strChanType = _T("");
    switch(pChannel->GetChannelType())
    {
    case CHANNEL_TYPE_SMV_CURRENT:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_AMPER);
        break;
    case CHANNEL_TYPE_SMV_VOLTAGE:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_VOLT);
        break;
    case CHANNEL_TYPE_SMV_TIME:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_TIMETICK);
        break;
    case CHANNEL_TYPE_GOOSE_TIME:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_TIMETICK);
        break;
    case CHANNEL_TYPE_GOOSE_POINT:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_POINT);
        break;
    case CHANNEL_TYPE_GOOSE_DOUBLE_POINT:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_DUALPOINT);
        break;
    case CHANNEL_TYPE_GOOSE_QUALITY:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_QUALITY);
        break;
    case CHANNEL_TYPE_GOOSE_FLOAT:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_FLOAT);
        break;
    case CHANNEL_TYPE_GOOSE_STRUCT:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_STRUCT);
        break;
    case CHANNEL_TYPE_GOOSE_INT8:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_INT8);
        break;
    case CHANNEL_TYPE_GOOSE_INT16:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_INT16);
        break;
    case CHANNEL_TYPE_GOOSE_INT24:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_INT24);
        break;
    case CHANNEL_TYPE_GOOSE_INT32:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_INT32);
        break;
    case CHANNEL_TYPE_GOOSE_UINT8:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UINT8);
        break;
    case CHANNEL_TYPE_GOOSE_UINT16:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UINT16);
        break;
    case CHANNEL_TYPE_GOOSE_UINT24:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UINT24);
        break;
    case CHANNEL_TYPE_GOOSE_UINT32:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UINT32);
        break;
    default:
        strChanType = dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UNKNOWN);
    }

    AddItem(new CELRecordItemText(strChanType));


    //开入映射
    CELRecordItemText* pItem =  (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
    if (pChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_POINT ||
        pChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_DOUBLE_POINT)
    {
        pItem->SetEditable(TRUE);
        pItem->HasCheckbox(TRUE);

        if (pChannel->IsSelected())
        {
            pItem->SetChecked(TRUE);
        }
    }
    else
    {
        pItem->SetEditable(FALSE);
    }
}

CDsmViewGooseRecMapRecord::CDsmViewGooseRecMapRecord(CString strCaption, CString strCtrlBlockDesc, CString strChanDesc)
{
    AddItem(new CELRecordItemText(strCaption));
    AddItem(new CELRecordItemText(strCtrlBlockDesc));
    AddItem(new CELRecordItemText(strChanDesc));
}


