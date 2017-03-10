/**
 * \file
 *      dsmrecord_send.cpp
 *
 * \brief
 *      发送业务记录的实现文件
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
#include "src/service/datalayer/config/ParamConfigGOOSERecv.h"
#include "src/main/utils/utility.h"
#include "src/utils/CommonControl/TimeEditCtrl_MS/TimeEditCtrl_MS.h"

#define DSM_SMV_GSE_CHANGE_ITEM_TEXT_COLOR   RGB(255, 0, 0)
#define DSM_SMV_GSE_CHANGE_ITEM_BK_COLOR     RGB(0, 0, 0)

CDsmManualSmvSendRecord::CDsmManualSmvSendRecord(CSMVSendVirtualChannel* pVirChannel)
{
    if (pVirChannel == NULL)
    {
        return;
    }
//#ifndef ZY_PRJ
//#define ZY_PRJ
//#endif
    //
    //通道
    //
    CString strChanName = _T("");
    strChanName = pVirChannel->GetName().c_str();

#ifdef ZY_PRJ
	CELRecordItemText* pItemChan = (CELRecordItemText*)AddItem(new CELRecordItemText(strChanName));
    //CELRecordItemText* pItemSel = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
    CELRecordItemText* pItemAmpl = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
    CELRecordItemText* pItemPhase = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));

	CELRecordItemText* pItemFrequence = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
	CELRecordItemText* pItemStep = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
#else
	CELRecordItemText* pItemChan = (CELRecordItemText*)AddItem(new CELRecordItemText(strChanName));
	//CELRecordItemText* pItemSel = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
	CELRecordItemText* pItemAmpl = (CELRecordItemText*)AddItem(new CELRecordItemText(_T(""))); 
	CELRecordItemText* pItemAmplStep = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
	CELRecordItemText* pItemPhase = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));	
	CELRecordItemText* pItemPhaseStep = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
#endif
    
    ASSERT(strChanName.GetLength()>0);

    //设置有checkbox
    pItemChan->HasCheckbox(TRUE);
    pItemChan->SetChecked(pVirChannel->IsSelected());
    pItemChan->SetEditable(TRUE);
    //幅值
    CString strContent = _T("");
    if (strChanName.GetLength() == 0)
    {
        return;
    }

    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }

    if (strChanName[0] == _T('U'))
    {        
        //电压
        if (pParamConfigSMVSend->GetSVDisplay() == SMV_VALUE_TYPE_PRIMARY)
        {
            strContent.Format(_T("%.03lf"), pVirChannel->GetAmplitude()/1000.0f);
            pItemAmpl->SetFormatString(_T("%s kV"));
        }
        else
        {
            strContent.Format(_T("%.03lf"), pVirChannel->GetAmplitude());
            pItemAmpl->SetFormatString(_T("%s V"));
        }
    }
    else if (strChanName[0] == _T('I'))
    {
        //电流
        strContent.Format(_T("%.03lf"), pVirChannel->GetAmplitude());
        pItemAmpl->SetFormatString(_T("%s A"));
    }
    else if (strChanName[0] == _T('t'))
    {
        //时间
        strContent.Format(_T("%.03lf"), pVirChannel->GetAmplitude());
        pItemAmpl->SetFormatString(_T("%s μs"));
    }
    pItemAmpl->SetValue(strContent);
    pItemAmpl->SetEditable(TRUE);

    //
    //相角 频率
    //
    if (strChanName[0] == _T('U') || strChanName[0] == _T('I'))
    {
        if (pVirChannel->GetACDCType() == CURRENT_TYPE_AC)
        {
            //交流

            //相角
            strContent.Format(_T("%.03lf"), pVirChannel->GetPhaseAngle());
            pItemPhase->SetValue(strContent);
            pItemPhase->SetFormatString(_T("%s°"));
            pItemPhase->SetEditable(TRUE);
            //频率
#ifdef ZY_PRJ
            strContent.Format(_T("%.03lf"), pVirChannel->GetFrequency());
            pItemFrequence->SetValue(strContent);
            pItemFrequence->SetFormatString(_T("%s Hz"));
            pItemFrequence->SetEditable(TRUE);
#endif

        }
        else
        {   //直流

            //相角
            strContent = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);

            pItemPhase->SetValue(strContent);
            pItemPhase->SetEditable(FALSE);
            //频率
#ifdef ZY_PRJ
            pItemFrequence ->SetValue(strContent);
            pItemFrequence->SetEditable(FALSE);
#endif
        }
    }
    else 
    {
        strContent = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);

        pItemPhase->SetValue(strContent);
        pItemPhase->SetEditable(FALSE);
        //频率
#ifdef ZY_PRJ
		pItemFrequence ->SetValue(strContent);
		pItemFrequence->SetEditable(FALSE);
#endif
    }

    //
    //步长
    //
    CString strAmplStep = _T("");
    CString strAngleStep = _T("");
    //幅值步长
    if (strChanName[0] == _T('U'))
    {
        //电压
        if (pParamConfigSMVSend->GetSVDisplay() == SMV_VALUE_TYPE_PRIMARY)
        {
            strAmplStep.Format(_T("%.03lf"), pVirChannel->GetAmplitudeStep()/1000.0f);
			pItemAmplStep->SetFormatString(_T("%s kV"));
        }
        else
        {
            strAmplStep.Format(_T("%.03lf"), pVirChannel->GetAmplitudeStep());
			pItemAmplStep->SetFormatString(_T("%s V"));
        }
    }
    else if (strChanName[0] == _T('I'))
    {
        strAmplStep.Format(_T("%.03lf"), pVirChannel->GetAmplitudeStep());
		pItemAmplStep->SetFormatString(_T("%s A"));
    }
    else if (strChanName[0] == _T('t'))
    {
        strAmplStep.Format(_T("%.03lf"), pVirChannel->GetAmplitudeStep());
		pItemAmplStep->SetFormatString(_T("%s μs"));
    }

    //相角步长,只有交流类型可以改变
    if (strChanName[0] == _T('U') || strChanName[0] == _T('I'))
    {
        if (pVirChannel->GetACDCType() == CURRENT_TYPE_AC)
        {
            strAngleStep.Format(_T("%.03lf"), pVirChannel->GetPhaseAngleStep());
        }
        else
        {
            strAngleStep = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);
        }
    }
    else
    {
        strAngleStep = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);
    }
#ifdef	ZY_PRJ
    strContent = strAmplStep + strAngleStep;    
    pItemStep->SetValue(strContent);
    pItemStep->SetEditable(TRUE);
#else
	
	pItemAmplStep->SetValue(strAmplStep);
	pItemAmplStep->SetEditable(TRUE);
	pItemPhaseStep->SetValue(strAngleStep);
	pItemPhaseStep->SetFormatString(_T("%s°"));
	pItemPhaseStep->SetEditable(TRUE);
#endif

#ifdef ZY_PRJ
#undef ZY_PRJ
#endif

    ////步长
    //if (eChangeItem == CHANGE_ITEM_AMPLITUDE)
    //{
    //    //改变幅值，步长均可编辑
    //    if (strChanName[0] == _T('U'))
    //    {
    //        //电压
    //        if (pParamConfigSMVSend->GetSVDisplay() == SMV_VALUE_TYPE_PRIMARY)
    //        {
    //            pItemStep->SetFormatString(_T("%s kV"));
    //            strContent.Format(_T("%.03lf"), pVirChannel->GetStep()/1000.0f);
    //        }
    //        else
    //        {
    //            pItemStep->SetFormatString(_T("%s V"));
    //            strContent.Format(_T("%.03lf"), pVirChannel->GetStep());
    //        }
    //    }
    //    else if (strChanName[0] == _T('I'))
    //    {
    //        pItemStep->SetFormatString(_T("%s A"));
    //        strContent.Format(_T("%.03lf"), pVirChannel->GetStep());
    //    }
    //    else if (strChanName[0] == _T('t'))
    //    {
    //        pItemStep->SetFormatString(_T("%s μs"));
    //        strContent.Format(_T("%.03lf"), pVirChannel->GetStep());
    //    }
    //    pItemStep->SetEditable(TRUE);
    //    pItemStep->SetValue(strContent);
    //}
    //else
    //{
    //    //改变类型相角,只有交流类型可以改变
    //    if (strChanName[0] == _T('U') || strChanName[0] == _T('I'))
    //    {
    //        if (pVirChannel->GetACDCType() == CURRENT_TYPE_AC)
    //        {
    //            //
    //            if (eChangeItem == CHANGE_ITEM_PHASE_ANGLE)
    //            {
    //                pItemStep->SetFormatString(_T("%s°"));
    //            }
    //            else if(eChangeItem == CHANGE_ITEM_FREQUENCY)
    //            {
    //                pItemStep->SetFormatString(_T("%s Hz"));
    //            }
    //            strContent.Format(_T("%.03lf"), pVirChannel->GetStep());
    //            pItemStep->SetValue(strContent);
    //            pItemStep->SetEditable(TRUE);
    //        }
    //        else
    //        {
    //            strContent = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);
    //            pItemStep->SetValue(strContent);
    //            pItemStep->SetEditable(FALSE);
    //        }
    //    }
    //    else
    //    {
    //        strContent = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);
    //        pItemStep->SetValue(strContent);
    //        pItemStep->SetEditable(FALSE);
    //    }
    //}
}

CDsmManualStepRecord::CDsmManualStepRecord(CString csCaption, CString csContent)
{
    AddItem(new CELRecordItemText(csCaption));
    AddItem(new CELRecordItemText(csContent));
}

CDsmManualGooseSendRecord::CDsmManualGooseSendRecord(CChannel* pChannel, int nIndex, int nSubIndex)
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

    //通道值

    CString strChanValue = _T("");
    GOOSE_DATA_VARIANT gooseDataVar = pChannel->GetGooseSendValue();

    CELRecordItemText* pItemValue = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));

    switch(pChannel->GetChannelType())
    {
    case CHANNEL_TYPE_GOOSE_TIME:
        {
            //时间不显示值，不可修改，保留通道
            //SYSTEMTIME sysTime;
            //SYSTEMTIME localTime;
            //sysTime = gooseDataVar.m_stUtcTime.GetUtcTime();
            //DM_SystemTimeToTzSpecificLocalTime(&sysTime, &localTime);
            //CString strTime = _T("");
            //pItemValue->SetValue(strTime);
            //CTimeEditCtrl_MS::SysTime2Str(localTime, strTime);
            //pItemValue->SetValue(strTime);
            //pItemValue->GetEditOptions(NULL)->AddExpandButton();
            //pItemValue->SetEditable(TRUE);
            pItemValue->SetValue(_T(""));
        }
        break;
    case CHANNEL_TYPE_GOOSE_POINT:
        {
            //pItemValue->GetEditOptions(NULL)->AddComboButton();
            //pItemValue->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_POINT_OFF), 0);
            //pItemValue->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_POINT_ON), 1);
            if (gooseDataVar.m_stBoolean.boolVal)
            {
                pItemValue->SetValue(dsm_stringMgr::Get(DSM_STR_MANUAL_POINT_ON));
            }
            else
            {
                pItemValue->SetValue(dsm_stringMgr::Get(DSM_STR_MANUAL_POINT_OFF));
            }
        }
        break;
    case CHANNEL_TYPE_GOOSE_DOUBLE_POINT:
        {
            pItemValue->GetEditOptions(NULL)->AddComboButton();
            pItemValue->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_INIT), DP_VALUE_INIT);
            pItemValue->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_OFF), DP_VALUE_OFF);
            pItemValue->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_ON), DP_VALUE_ON);
            pItemValue->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_BAD), DP_VALUE_BAD);

            std::wstring strValue = gooseDataVar.m_stBitString.GetString();
            eDPValueIndex eValueIndex = DP_VALUE_INIT;

            for (int i =DP_VALUE_INIT; i <= DP_VALUE_BAD; ++i)
            {
                if (strDPValue[i] == strValue)
                {
                    eValueIndex = (eDPValueIndex) i;
                }
            }

            CELRecordItemConstraint* pItemCon = pItemValue->GetEditOptions(NULL)->FindConstraint(eValueIndex);
            if (pItemCon)
            {
                pItemValue->SetValue(pItemCon->GetConstraintCaption());
            }
            else
            {
                pItemValue->SetValue(dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN));
            }
            pItemValue->SetEditable(TRUE);
        }
        break;
    case CHANNEL_TYPE_GOOSE_QUALITY:
        {
            pItemValue->SetValue(gooseDataVar.m_stBitString.GetString().c_str());
            pItemValue->GetEditOptions(NULL)->AddExpandButton();
            pItemValue->SetEditable(TRUE);
        }
        break;
    case CHANNEL_TYPE_GOOSE_FLOAT:
        {
            CString strTemp;
            strTemp.Format(_T("%.03f"), gooseDataVar.m_stFloat.datVal);
            pItemValue->SetValue(strTemp);
            pItemValue->SetEditable(TRUE);
        }
        break;
    case CHANNEL_TYPE_GOOSE_STRUCT:
        {
            pItemValue->SetValue(_T(""));
            pItemValue->SetEditable(FALSE);
        }
        break;
    case CHANNEL_TYPE_GOOSE_INT8:
        {
            strChanValue.Format(_T("%d"), gooseDataVar.m_stInt8.datVal);
            pItemValue->SetValue(strChanValue);
            pItemValue->SetEditable(TRUE);
        }
        break;
    case CHANNEL_TYPE_GOOSE_INT16:
        {
            strChanValue.Format(_T("%d"), gooseDataVar.m_stInt16.datVal);
            pItemValue->SetValue(strChanValue);
            pItemValue->SetEditable(TRUE);
        }
        break;
    case CHANNEL_TYPE_GOOSE_INT24:
        {
            strChanValue.Format(_T("%d"), gooseDataVar.m_stInt24.datVal);
            pItemValue->SetValue(strChanValue);
            pItemValue->SetEditable(TRUE);
        }
        break;
    case CHANNEL_TYPE_GOOSE_INT32:
        {
            strChanValue.Format(_T("%d"), gooseDataVar.m_stInt32.datVal);
            pItemValue->SetValue(strChanValue);
            pItemValue->SetEditable(TRUE);
        }
        break;
    case CHANNEL_TYPE_GOOSE_UINT8:
        {
            strChanValue.Format(_T("%u"), gooseDataVar.m_stUInt8.datVal);
            pItemValue->SetValue(strChanValue);
            pItemValue->SetEditable(TRUE);
        }
        break;
    case CHANNEL_TYPE_GOOSE_UINT16:
        {
            strChanValue.Format(_T("%u"), gooseDataVar.m_stUInt16.datVal);
            pItemValue->SetValue(strChanValue);
            pItemValue->SetEditable(TRUE);
        }
        break;
    case CHANNEL_TYPE_GOOSE_UINT24:
        {
            strChanValue.Format(_T("%u"), gooseDataVar.m_stUInt24.datVal);
            pItemValue->SetValue(strChanValue);
            pItemValue->SetEditable(TRUE);
        }
        break;
    case CHANNEL_TYPE_GOOSE_UINT32:
        {
            strChanValue.Format(_T("%u"), gooseDataVar.m_stUInt32.datVal);
            pItemValue->SetValue(strChanValue);
            pItemValue->SetEditable(TRUE);
        }
        break;
    default:
        {
            pItemValue->SetValue(dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN));
            pItemValue->SetEditable(FALSE);
        }
        break;
    }
}


CDsmManualSetRecord::CDsmManualSetRecord(CString strCaption, CString strContent)
{
    AddItem(new CELRecordItemText(strCaption));
    AddItem(new CELRecordItemText(strContent));
}


CDsmManualResultListRecord::CDsmManualResultListRecord(CStateTestResult* pStateTestResult, int nIndex)
{
    if (pStateTestResult == NULL)
    {
        return;
    }
    CString strIndex = _T("");
    CString strTime = _T("");
    CString strResult = _T("");

    //索引
    strIndex.Format(_T("%d"), nIndex);
    AddItem(new CELRecordItemText(strIndex));

    strTime = pStateTestResult->GetStartTime().c_str();

    //时间
    AddItem(new CELRecordItemText(strTime));

    //变化项
    CString strSmvGooseDesc = _T("");

    if (pStateTestResult->GetTestType() == TEST_TYPE_SMV)
    {
        if (pStateTestResult->IsSmvChannelChanged())
        {
            strSmvGooseDesc = dsm_stringMgr::Get(DSM_STR_MANUAL_SMV_CHAN_ACTION);
        }
        else if (pStateTestResult->IsStatusWordChanged())
        {
            strSmvGooseDesc = dsm_stringMgr::Get(DSM_STR_MANUAL_SMV_STATUS_WORD_CHANGE);
        }
        else if (pStateTestResult->IsQualityChanged())
        {
            strSmvGooseDesc = dsm_stringMgr::Get(DSM_STR_MANUAL_SMV_CHAN_QUALITY_CHANGE);
        }
        else
        {
            strSmvGooseDesc = dsm_stringMgr::Get(DSM_STR_MANUAL_START_SEND_SMV);
        }
    }
    else if (pStateTestResult->GetTestType() == TEST_TYPE_GOOSE)
    {
        if (pStateTestResult->IsGooseChannelChanged())
        {
            strSmvGooseDesc = dsm_stringMgr::Get(DSM_STR_MANUAL_GOOSE_CHAN_ACTION);
        }
        else
        {
            strSmvGooseDesc = dsm_stringMgr::Get(DSM_STR_MANUAL_START_SEND_GOOSE);
        }
    }
    AddItem(new CELRecordItemText(strSmvGooseDesc));

    //开入量动作
    CChannelRecvValues* pRecvValues = NULL;
    GOOSE_DATA_VARIANT* pGooseDataVar = NULL;
    CString strChangeDesc  = _T("");
    CString strDIChangeDesc = _T("");

    CString strName = _T("");
    for (int i = 0; i < 8; ++i)
    {
        strDIChangeDesc = _T("");
        strName.Format(_T("DI%d"), i + 1);
        pRecvValues = pStateTestResult->GetChannelRecvValues((LPCTSTR)strName);
        if (pRecvValues != NULL)
        {
            if (pRecvValues->GetChangeTimes() > 0)
            {
                //变位
                CHistoryData* pHisData = pRecvValues->FirstRecvData();
                if (pHisData != NULL)
                {
                    pGooseDataVar = pHisData->GetGooseData();
                    if (pGooseDataVar == NULL)
                    {
                        break;
                    }
                    if (pGooseDataVar->GetType() == GSEDT_BOOLEAN)
                    {
                        //单点类型
                        if (pGooseDataVar->m_stBoolean.boolVal)
                        {
                            //on
                            strDIChangeDesc.Format(_T("%s(%s),"),
                                strName,
                                dsm_stringMgr::Get(DSM_STR_STATE_POINT_ON));
                        }
                        else
                        {
                            //off
                            strDIChangeDesc.Format(_T("%s(%s),"),
                                strName,
                                dsm_stringMgr::Get(DSM_STR_STATE_POINT_OFF));
                        }
                    }
                    else if (pGooseDataVar->GetType() == GSEDT_BIT_STR)
                    {
                        if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_INIT])
                        {
                            strDIChangeDesc.Format(_T("%s(%s),"),
                                strName,
                                dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_INIT));
                        }
                        else if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_OFF])
                        {
                            strDIChangeDesc.Format(_T("%s(%s),"),
                                strName,
                                dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_OFF));
                        }
                        else if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_ON])
                        {
                            strDIChangeDesc.Format(_T("%s(%s),"),
                                strName,
                                dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_ON));
                        }
                        else if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_BAD])
                        {
                            strDIChangeDesc.Format(_T("%s(%s),"),
                                strName,
                                dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_BAD));
                        }
                    }
                }
            }
            ;
            strChangeDesc += strDIChangeDesc;
        }
        else
        {
            continue;
        }
    }

    if (strChangeDesc == _T(""))
    {
        //无开入量变化
        strChangeDesc = dsm_stringMgr::Get(DSM_STR_STATE_DI_NO_CHANGE);
    }

    AddItem(new CELRecordItemText(strChangeDesc));

}


//
//合成变化项的值
//
static CString s_FormatChangeItem(CString strPreValue, CString strCurValue)
{
    ASSERT(strPreValue.GetLength() > 0);
    ASSERT(strCurValue.GetLength() > 0);
    CString strTemp = _T("");
    strTemp.Format(_T("%s->%s"), strPreValue, strCurValue);
    return strTemp;
}

CDsmManualResultDetailSmvRecord::CDsmManualResultDetailSmvRecord(CSMVSendVirtualChannel* pVirChannel, CSMVSendVirtualChannel* pPreVirChannel/* = NULL*/)
{
    if (pVirChannel == NULL)
    {
        return;
    }

    CELRecordItemText* pItemChan = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
    CELRecordItemText* pItemAmpl = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
    CELRecordItemText* pItemPhaseAngle = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
    CELRecordItemText* pItemFrequence = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
    if (pItemChan == NULL || 
        pItemAmpl == NULL ||
        pItemPhaseAngle == NULL ||
        pItemFrequence == NULL)
    {
        return;
    }
    
    CString strPreValue = _T("");
    CString strCurValue = _T("");

    CString strChanName = _T("");
    CString strAmpl = _T("");
    CString strPhaseAngle = _T("");
    CString strFrequence = _T("");

    //通道
    strChanName = pVirChannel->GetName().c_str();
    pItemChan->SetValue(strChanName);

    //幅值
    if (pVirChannel->IsAmplitudeChanged() && pPreVirChannel)
    {
        if (strChanName[0] == _T('U'))
        {
            //电压
            strPreValue.Format(_T("%.03lf V"), pPreVirChannel->GetAmplitude());
            strCurValue.Format(_T("%.03lf V"), pVirChannel->GetAmplitude());
            strAmpl = s_FormatChangeItem(strPreValue, strCurValue);
        }
        else if (strChanName[0] == _T('I'))
        {
            //电流
            strPreValue.Format(_T("%.03lf A"), pPreVirChannel->GetAmplitude());
            strCurValue.Format(_T("%.03lf A"), pVirChannel->GetAmplitude());
            strAmpl = s_FormatChangeItem(strPreValue, strCurValue);
        }
        else if (strChanName[0] == _T('t'))
        {
            //时间
            strPreValue.Format(_T("%.03lf μs"), pPreVirChannel->GetAmplitude());
            strCurValue.Format(_T("%.03lf μs"), pVirChannel->GetAmplitude());
            strAmpl = s_FormatChangeItem(strPreValue, strCurValue);
        }
        pItemAmpl->SetColor(DSM_SMV_GSE_CHANGE_ITEM_TEXT_COLOR, DSM_SMV_GSE_CHANGE_ITEM_BK_COLOR);
    }
    else
    {
        if (strChanName[0] == _T('U'))
        {
            //电压
            strAmpl.Format(_T("%.03lf V"), pVirChannel->GetAmplitude());
        }
        else if (strChanName[0] == _T('I'))
        {
            //电流
            strAmpl.Format(_T("%.03lf A"), pVirChannel->GetAmplitude());
        }
        else if (strChanName[0] == _T('t'))
        {
            //时间
            strAmpl.Format(_T("%.03lf μs"), pVirChannel->GetAmplitude());
        }
    }
    pItemAmpl->SetValue(strAmpl);

    //相角
    if (pVirChannel->GetACDCType() == CURRENT_TYPE_AC)
    {
        if (pVirChannel->IsPhaseAngleChanged() && pPreVirChannel)
        {
            strPreValue.Format(_T("%.03lf°"), pPreVirChannel->GetPhaseAngle());
            strCurValue.Format(_T("%.03lf°"), pVirChannel->GetPhaseAngle());
            strPhaseAngle = s_FormatChangeItem(strPreValue, strCurValue);
            pItemPhaseAngle->SetColor(DSM_SMV_GSE_CHANGE_ITEM_TEXT_COLOR, DSM_SMV_GSE_CHANGE_ITEM_BK_COLOR);
        }
        else 
        {
            strPhaseAngle.Format(_T("%.03lf°"), pVirChannel->GetPhaseAngle());
        }
    }
    else
    {
        strPhaseAngle = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);
    }
    pItemPhaseAngle->SetValue(strPhaseAngle);

    //频率
    if (pVirChannel->GetACDCType() == CURRENT_TYPE_AC)
    {
        if (pVirChannel->IsFrequencyChanged())
        {
            strPreValue.Format(_T("%.03lf Hz"), pPreVirChannel->GetFrequency());
            strCurValue.Format(_T("%.03lf Hz"), pVirChannel->GetFrequency());
            strFrequence = s_FormatChangeItem(strPreValue, strCurValue);
            pItemFrequence->SetColor(DSM_SMV_GSE_CHANGE_ITEM_TEXT_COLOR, DSM_SMV_GSE_CHANGE_ITEM_BK_COLOR);
        }
        else
        {
            strFrequence.Format(_T("%.03lf Hz"), pVirChannel->GetFrequency());
        }
    }
    else
    {
        strFrequence = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);
    }

    pItemFrequence->SetValue(strFrequence);

}

CDsmManualResultDetailGooseRecord::CDsmManualResultDetailGooseRecord(CChannel* pChannel, int nIndex, int nSubIndex, CChannel* pPreChannel/* = NULL*/)
{
    ASSERT(pChannel != NULL);
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


    //通道值

    CString strChanValue = _T("");
    GOOSE_DATA_VARIANT preGooseDataVar;
    if (pChannel->IsSendValueChanged() && pPreChannel)
    {
        preGooseDataVar = pPreChannel->GetGooseSendValue();
    }
    GOOSE_DATA_VARIANT gooseDataVar = pChannel->GetGooseSendValue();

    CELRecordItemText* pItemValue = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));

    CString strPreValue = _T("");
    CString strCurValue = _T("");
    switch(pChannel->GetChannelType())
    {
    case CHANNEL_TYPE_GOOSE_TIME:
        {
            //
            //暂不提供时间变化显示
            //

            //if (pChannel->IsSendValueChanged() && pPreChannel)
            //{
            //    SYSTEMTIME sysTime;
            //    SYSTEMTIME localTime;
            //    sysTime = preGooseDataVar.m_stUtcTime.GetUtcTime();
            //    DM_SystemTimeToTzSpecificLocalTime(&sysTime, &localTime);
            //    CString strPreTime = _T("");
            //    CTimeEditCtrl_MS::SysTime2Str(localTime, strPreTime);
            //    sysTime = gooseDataVar.m_stUtcTime.GetUtcTime();
            //    DM_SystemTimeToTzSpecificLocalTime(&sysTime, &localTime);
            //    CString strTime = _T("");
            //    CTimeEditCtrl_MS::SysTime2Str(localTime, strTime);
            //    strChanValue = s_FormatChangeItem(strPreTime, strTime);
            //}
            //else
            //{
            //    strChanValue = gooseDataVar.m_stUtcTime.GetUtcString().c_str();
            //}
            strChanValue = _T("");
        }
        break;
    case CHANNEL_TYPE_GOOSE_POINT:
        {
            if (pChannel->IsSendValueChanged() && pPreChannel)
            {
                //前值
                if (preGooseDataVar.m_stBoolean.boolVal)
                {
                    strPreValue = dsm_stringMgr::Get(DSM_STR_MANUAL_POINT_ON);
                }
                else
                {
                    strPreValue = dsm_stringMgr::Get(DSM_STR_MANUAL_POINT_OFF);
                }
                //现值
                if (gooseDataVar.m_stBoolean.boolVal)
                {
                    strCurValue = dsm_stringMgr::Get(DSM_STR_MANUAL_POINT_ON);
                }
                else
                {
                    strCurValue = dsm_stringMgr::Get(DSM_STR_MANUAL_POINT_OFF);
                }
                strChanValue = s_FormatChangeItem(strPreValue, strCurValue);
            }
            else
            {
                if (gooseDataVar.m_stBoolean.boolVal)
                {
                    strChanValue = dsm_stringMgr::Get(DSM_STR_MANUAL_POINT_ON);
                }
                else
                {
                    strChanValue = dsm_stringMgr::Get(DSM_STR_MANUAL_POINT_OFF);
                }
            }
        }
        break;
    case CHANNEL_TYPE_GOOSE_DOUBLE_POINT:
        {
            if (pChannel->IsSendValueChanged() && pPreChannel)
            {
                std::wstring strPreDp = preGooseDataVar.m_stBitString.GetString();
                std::wstring strCurDp = gooseDataVar.m_stBitString.GetString();

                if (strDPValue[DP_VALUE_INIT] == strPreDp)
                {
                    strPreValue = dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_INIT);
                }
                else if (strDPValue[DP_VALUE_OFF] == strPreDp)
                {
                    strPreValue = dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_OFF);
                }
                else if (strDPValue[DP_VALUE_ON] == strPreDp)
                {
                    strPreValue = dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_ON);
                }
                else if (strDPValue[DP_VALUE_BAD] == strPreDp)
                {
                    strPreValue = dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_BAD);
                }
                else
                {
                    strPreValue = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);
                }

                if (strDPValue[DP_VALUE_INIT] == strCurDp)
                {
                    strCurValue = dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_INIT);
                }
                else if (strDPValue[DP_VALUE_OFF] == strCurDp)
                {
                    strCurValue = dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_OFF);
                }
                else if (strDPValue[DP_VALUE_ON] == strCurDp)
                {
                    strCurValue = dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_ON);
                }
                else if (strDPValue[DP_VALUE_BAD] == strCurDp)
                {
                    strCurValue = dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_BAD);
                }
                else
                {
                    strCurValue = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);
                }
                strChanValue = s_FormatChangeItem(strPreValue, strCurValue);
            }
            else
            {
                std::wstring strCurDp = gooseDataVar.m_stBitString.GetString();
                if (strDPValue[DP_VALUE_INIT] == strCurDp)
                {
                    strChanValue = dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_INIT);
                }
                else if (strDPValue[DP_VALUE_OFF] == strCurDp)
                {
                    strChanValue = dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_OFF);
                }
                else if (strDPValue[DP_VALUE_ON] == strCurDp)
                {
                    strChanValue = dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_ON);
                }
                else if (strDPValue[DP_VALUE_BAD] == strCurDp)
                {
                    strChanValue = dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_BAD);
                }
                else
                {
                    strChanValue = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);
                }
            }
        }
        break;
    case CHANNEL_TYPE_GOOSE_QUALITY:
        {
            if (pChannel->IsSendValueChanged() && pPreChannel)
            {
                strPreValue = preGooseDataVar.m_stBitString.GetString().c_str();
                strCurValue = gooseDataVar.m_stBitString.GetString().c_str();
                strChanValue = s_FormatChangeItem(strPreValue, strCurValue);
            }
            else
            {
                strChanValue = gooseDataVar.m_stBitString.GetString().c_str();
            }
        }
        break;
    case CHANNEL_TYPE_GOOSE_FLOAT:
        {
            if (pChannel->IsSendValueChanged() && pPreChannel)
            {
                strPreValue.Format(_T("%.03f"), preGooseDataVar.m_stFloat.datVal);
                strCurValue.Format(_T("%.03f"), gooseDataVar.m_stFloat.datVal);
                strChanValue = s_FormatChangeItem(strPreValue, strCurValue);
            }
            else
            {
                strChanValue.Format(_T("%.03f"), gooseDataVar.m_stFloat.datVal);
            }
        }
        break;
    case CHANNEL_TYPE_GOOSE_STRUCT:
        {
            strChanValue =  _T("");
        }
        break;
    case CHANNEL_TYPE_GOOSE_INT8:
        {
            if (pChannel->IsSendValueChanged() && pPreChannel)
            {
                strPreValue.Format(_T("%d"), preGooseDataVar.m_stInt8.datVal);
                strCurValue.Format(_T("%d"), gooseDataVar.m_stInt8.datVal);
                strChanValue = s_FormatChangeItem(strPreValue, strCurValue);
            }
            else
            {
                strChanValue.Format(_T("%d"), gooseDataVar.m_stInt8.datVal);
            }
        }
        break;
    case CHANNEL_TYPE_GOOSE_INT16:
        {
            if (pChannel->IsSendValueChanged() && pPreChannel)
            {
                strPreValue.Format(_T("%d"), preGooseDataVar.m_stInt16.datVal);
                strCurValue.Format(_T("%d"), gooseDataVar.m_stInt16.datVal);
                strChanValue = s_FormatChangeItem(strPreValue, strCurValue);
            }
            else
            {
                strChanValue.Format(_T("%d"), gooseDataVar.m_stInt16.datVal);
            }
        }
        break;
    case CHANNEL_TYPE_GOOSE_INT24:
        {
            if (pChannel->IsSendValueChanged() && pPreChannel)
            {
                strPreValue.Format(_T("%d"), preGooseDataVar.m_stInt24.datVal);
                strCurValue.Format(_T("%d"), gooseDataVar.m_stInt24.datVal);
                strChanValue = s_FormatChangeItem(strPreValue, strCurValue);
            }
            else
            {
                strChanValue.Format(_T("%d"), gooseDataVar.m_stInt24.datVal);
            }
        }
        break;
    case CHANNEL_TYPE_GOOSE_INT32:
        {
            if (pChannel->IsSendValueChanged() && pPreChannel)
            {
                strPreValue.Format(_T("%d"), preGooseDataVar.m_stInt32.datVal);
                strCurValue.Format(_T("%d"), gooseDataVar.m_stInt32.datVal);
                strChanValue = s_FormatChangeItem(strPreValue, strCurValue);
            }
            else
            {
                strChanValue.Format(_T("%d"), gooseDataVar.m_stInt32.datVal);
            }
        }
        break;
    case CHANNEL_TYPE_GOOSE_UINT8:
        {
            if (pChannel->IsSendValueChanged() && pPreChannel)
            {
                strPreValue.Format(_T("%u"), preGooseDataVar.m_stUInt8.datVal);
                strCurValue.Format(_T("%u"), gooseDataVar.m_stUInt8.datVal);
                strChanValue = s_FormatChangeItem(strPreValue, strCurValue);
            }
            else
            {
                strChanValue.Format(_T("%u"), gooseDataVar.m_stUInt8.datVal);
            }
        }
        break;
    case CHANNEL_TYPE_GOOSE_UINT16:
        {
            if (pChannel->IsSendValueChanged() && pPreChannel)
            {
                strPreValue.Format(_T("%u"), preGooseDataVar.m_stUInt16.datVal);
                strCurValue.Format(_T("%u"), gooseDataVar.m_stUInt16.datVal);
                strChanValue = s_FormatChangeItem(strPreValue, strCurValue);
            }
            else
            {
                strChanValue.Format(_T("%u"), gooseDataVar.m_stUInt16.datVal);
            }
        }
        break;
    case CHANNEL_TYPE_GOOSE_UINT24:
        {
            if (pChannel->IsSendValueChanged() && pPreChannel)
            {
                strPreValue.Format(_T("%u"), preGooseDataVar.m_stUInt24.datVal);
                strCurValue.Format(_T("%u"), gooseDataVar.m_stUInt24.datVal);
                strChanValue = s_FormatChangeItem(strPreValue, strCurValue);
            }
            else
            {
                strChanValue.Format(_T("%u"), gooseDataVar.m_stUInt24.datVal);
            }
        }
        break;
    case CHANNEL_TYPE_GOOSE_UINT32:
        {
            if (pChannel->IsSendValueChanged() && pPreChannel)
            {
                strPreValue.Format(_T("%u"), preGooseDataVar.m_stUInt32.datVal);
                strCurValue.Format(_T("%u"), gooseDataVar.m_stUInt32.datVal);
                strChanValue = s_FormatChangeItem(strPreValue, strCurValue);
            }
            else
            {
                strChanValue.Format(_T("%u"), gooseDataVar.m_stUInt32.datVal);
            }
        }
        break;
    default:
        {
            strChanValue = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);
        }
        break;
    }
    if (pChannel->IsSendValueChanged() && pChannel->GetChannelType() != CHANNEL_TYPE_GOOSE_STRUCT)
    {
        pItemValue->SetColor(DSM_SMV_GSE_CHANGE_ITEM_TEXT_COLOR, DSM_SMV_GSE_CHANGE_ITEM_BK_COLOR);
    }
    pItemValue->SetValue(strChanValue);
}

CDsmManualResultDIActionRecord::CDsmManualResultDIActionRecord(CChannelRecvValues* pChannelRecvValues)
{
    if (pChannelRecvValues == NULL)
    {
        return;
    }

    //DI
    AddItem(new CELRecordItemText(pChannelRecvValues->GetChannelName().c_str()));

    //变位次数
    CString strChangeTimes = _T("");
    strChangeTimes.Format(_T("%d"), pChannelRecvValues->GetChangeTimes());
    AddItem(new CELRecordItemText(strChangeTimes));

    //变位
    int n = 0;
    CHistoryData* pHisData = pChannelRecvValues->FirstRecvData();
    GOOSE_DATA_VARIANT* pGooseDataVar;
    CString strChangeDesc = _T("");

    while(pHisData && n < 3)
    {
        pGooseDataVar = pHisData->GetGooseData();
        if (pGooseDataVar == NULL)
        {
            break;
        }
        if (pGooseDataVar->GetType() == GSEDT_BOOLEAN)
        {
            //单点类型
            if (pGooseDataVar->m_stBoolean.boolVal)
            {
                //on
                strChangeDesc.Format(_T("%s(%s)"),
                    dsm_stringMgr::Get(DSM_STR_STATE_POINT_ON),
                    pHisData->GetChangeTimeString().c_str());
            }
            else
            {
                //off
                strChangeDesc.Format(_T("%s(%s)"),
                    dsm_stringMgr::Get(DSM_STR_STATE_POINT_OFF),
                    pHisData->GetChangeTimeString().c_str());
            }
        }
        else if (pGooseDataVar->GetType() == GSEDT_BIT_STR)
        {
            if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_INIT])
            {
                strChangeDesc.Format(_T("%s(%s)"),
                    dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_INIT),
                    pHisData->GetChangeTimeString().c_str());
            }
            else if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_OFF])
            {
                strChangeDesc.Format(_T("%s(%s)"),
                    dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_OFF),
                    pHisData->GetChangeTimeString().c_str());
            }
            else if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_ON])
            {
                strChangeDesc.Format(_T("%s(%s)"),
                    dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_ON),
                    pHisData->GetChangeTimeString().c_str());
            }
            else if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_BAD])
            {
                strChangeDesc.Format(_T("%s(%s)"),
                    dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_BAD),
                    pHisData->GetChangeTimeString().c_str());
            }
        }

        AddItem(new CELRecordItemText(strChangeDesc));
        ++n;
        pHisData = pChannelRecvValues->NextHistoryData(pHisData);
    }

    for(; n < 3; ++n)
    {
        AddItem(new CELRecordItemText(_T("")));
    }
}

CDsmManualSmvCtrlListRecord::CDsmManualSmvCtrlListRecord(CString csIndex, CString csDesc)
{
    AddItem(new CELRecordItemText(csIndex));
    AddItem(new CELRecordItemText(csDesc));
}

CDsmManualSmvStatusWordRecord::CDsmManualSmvStatusWordRecord(CString csStatusWord, CString csValue)
{
    AddItem(new CELRecordItemText(csStatusWord));
    CELRecordItem* pItem = AddItem(new CELRecordItemText(csValue));
    pItem->SetEditable(TRUE);
}

CDsmManualSmvChanQualityRecord::CDsmManualSmvChanQualityRecord(CChannel* pChannel, int nIndex)
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

    //品质
    CString strQuality = _T("");
    strQuality.Format(_T("%04X"), pChannel->GetQuality());
    pItem = (CELRecordItemText*)AddItem(new CELRecordItemText(strQuality));
    pItem->SetEditable(TRUE);

#if !CHUN_HUA

	pItem = (CELRecordItemText*)AddItem(new CDsmRecordItemText(L""));

	//设置有checkbox
	pItem->HasCheckbox(TRUE);
	pItem->SetChecked(pChannel->IsSelected());
	pItem->SetEditable(TRUE);
#endif


}

CDsmManualQualitySetRecord::CDsmManualQualitySetRecord(CString csCaption, CString csContent)
{
    AddItem(new CELRecordItemText(csCaption));
    AddItem(new CELRecordItemText(csContent));
}

CDsmStateSequenceRecord::CDsmStateSequenceRecord(int nIndex, CStatus* pStatus)
{
    if (nIndex < 1 || pStatus == NULL)
    {
        return;
    }
    //索引
    CString strIndex = _T("");
    strIndex.Format(_T("%d"), nIndex);
    AddItem(new CELRecordItemText(strIndex));

    //状态设置
    CString strSet = _T("");

    eSwitchType switchType = pStatus->GetSwitchType();
    switch (switchType)
    {
    case SWITCH_TYPE_TIME:
        {
            strSet.Format(_T("%s : %d ms"),
                dsm_stringMgr::Get(DSM_STR_STATE_SET_CT_TIME_CHANGE),
                pStatus->GetDelay());
        };
        break;
    case SWITCH_TYPE_DI:
        {
            CString strDIChangeType = _T("");
            switch(pStatus->GetLogicalOperator())
            {
            case LOGICAL_AND:
                {
                    strDIChangeType = dsm_stringMgr::Get(DSM_STR_STATE_SET_DI_CT_AND);
                }
                break;
            case LOGICAL_OR:
                {
                    strDIChangeType = dsm_stringMgr::Get(DSM_STR_STATE_SET_DI_CT_OR);
                }
                break;
            default:
                strDIChangeType = dsm_stringMgr::Get(DSM_STR_STATE_UNKNOWN);
                break;
            }
            strSet.Format(_T("%s:%s"), dsm_stringMgr::Get(DSM_STR_STATE_SET_CT_DI_CHANGE),
                strDIChangeType);
        };
        break;
    case SWITCH_TYPE_MANUAL:
        {
            strSet.Format(_T("%s"), dsm_stringMgr::Get(DSM_STR_STATE_SET_CT_MANUAL_CHANGE));
        };
        break;
    }
    AddItem(new CELRecordItemText(strSet));


    //状态数据
    CString strStateData = _T("");
    CString strTemp =  _T("");
    CString strChanName = _T("");

    CSMVSendVirtualChannel* pVirChannel = pStatus->FirstSMVSendVirtualChannel();
    while(pVirChannel)
    {
        strChanName = pVirChannel->GetName().c_str();
        if (strChanName.GetLength() > 0)
        {
            if (strChanName[0] == _T('U'))
            {
                //电压
                strTemp.Format(_T("%s=%.03lf V, "), strChanName, pVirChannel->GetAmplitude());
            }
            else if (strChanName[0] == _T('I'))
            {
                //电流
                strTemp.Format(_T("%s=%.03lf A, "), strChanName, pVirChannel->GetAmplitude());
            }
            else if (strChanName[0] == _T('t'))
            {
                //时间
                strTemp.Format(_T("%s=%.03lf μs, "), strChanName, pVirChannel->GetAmplitude());
            }
            strStateData += strTemp;
        }
        pVirChannel = pStatus->NextSMVSendVirtualChannel(pVirChannel);
    }
    if (strStateData.GetLength() == 0)
    {
        strStateData = dsm_stringMgr::Get(DSM_STR_STATE_SMV_SEND_NO_CHAN);
    }
    AddItem(new CELRecordItemText(strStateData));
}

CDsmStateSetRecord::CDsmStateSetRecord(CString strCaption, CString strContent)
{
    AddItem(new CELRecordItemText(strCaption));
    AddItem(new CELRecordItemText(strContent));
}

#if !SHENG_ZE
CDsmStateSetRecordDI::CDsmStateSetRecordDI(CString strCaption0, CString strCaption, CString strContent,CString strCaption1, CString strContent1)
{
	AddItem(new CELRecordItemText(strCaption0));
	AddItem(new CELRecordItemText(strCaption));
	AddItem(new CELRecordItemText(strContent));
	AddItem(new CELRecordItemText(strCaption1));
	AddItem(new CELRecordItemText(strContent1));
}
#endif

CDsmStateSmvDataRecord::CDsmStateSmvDataRecord( CSMVSendVirtualChannel* pVirChannel)
{
    if (pVirChannel == NULL)
    {
        return;
    }

    //
    //通道
    //
    CString strChanName = _T("");
    strChanName = pVirChannel->GetName().c_str();
    AddItem(new CELRecordItemText(strChanName));
    CELRecordItemText* pItemAmpl = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
    CELRecordItemText* pItemPhase = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
    CELRecordItemText* pItemFrequence = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));

    ASSERT(strChanName.GetLength()>0);


    //幅值
    CString strContent = _T("");
    if (strChanName.GetLength() == 0)
    {
        return;
    }

    if (strChanName[0] == _T('U'))
    {
        //电压
        CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
        if (NULL == pParamConfigSMVSend)
        {
            return ;
        }
        if (pParamConfigSMVSend->GetSVDisplay() == SMV_VALUE_TYPE_PRIMARY)
        {
            strContent.Format(_T("%.03lf"), pVirChannel->GetAmplitude()/1000.0f);
            pItemAmpl->SetFormatString(_T("%s kV"));
        }
        else
        {
            strContent.Format(_T("%.03lf"), pVirChannel->GetAmplitude());
            pItemAmpl->SetFormatString(_T("%s V"));
        }
        
    }
    else if (strChanName[0] == _T('I'))
    {
        //电流
        strContent.Format(_T("%.03lf"), pVirChannel->GetAmplitude());
        pItemAmpl->SetFormatString(_T("%s A"));
    }
    else if (strChanName[0] == _T('t'))
    {
        //时间
        strContent.Format(_T("%.03lf"), pVirChannel->GetAmplitude());
        pItemAmpl->SetFormatString(_T("%s μs"));
    }

    pItemAmpl->SetValue(strContent);
    pItemAmpl->SetEditable(TRUE);
    //
    //
    //
    if (strChanName[0] == _T('U') || strChanName[0] == _T('I'))
    {
        if (pVirChannel->GetACDCType() == CURRENT_TYPE_AC)
        {
            //交流

            //相角
            strContent.Format(_T("%.03lf"), pVirChannel->GetPhaseAngle());
            pItemPhase->SetValue(strContent);
            pItemPhase->SetFormatString(_T("%s°"));
            pItemPhase->SetEditable(TRUE);
            //频率
            strContent.Format(_T("%.03lf"), pVirChannel->GetFrequency());
            pItemFrequence->SetValue(strContent);
            pItemFrequence->SetFormatString(_T("%s Hz"));
            pItemFrequence->SetEditable(TRUE);

        }
        else
        {   //直流

            //相角
            strContent = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);

            pItemPhase->SetValue(strContent);
            pItemPhase->SetEditable(FALSE);
            //频率
            pItemFrequence ->SetValue(strContent);
            pItemFrequence->SetEditable(FALSE);
        }
    }
    else 
    {
        strContent = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);

        pItemPhase->SetValue(strContent);
        pItemPhase->SetEditable(FALSE);
        //频率
        pItemFrequence ->SetValue(strContent);
        pItemFrequence->SetEditable(FALSE);
    }
}

///
/// @brief
///      状态序列 谐波设置 记录项
///

CDsmStateHarmonicRecord::CDsmStateHarmonicRecord( CSMVSendVirtualChannel* pVirChannel)
{

    if (pVirChannel == NULL)
    {
        return;
    }
    
    CString csTemp = _T("");
    
    //添加ITEM
    CString strChanName = _T("");
    strChanName = pVirChannel->GetName().c_str();
    AddItem(new CELRecordItemText(strChanName));
    CELRecordItemText* pItemHarmnic = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
    CELRecordItemText* pItemAmpl = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));
    CELRecordItemText* pItemPhase = (CELRecordItemText*)AddItem(new CELRecordItemText(_T("")));

    CHarmonic* pHarmonic = pVirChannel->GetHarmonic();
    ASSERT(pHarmonic);
    if (pHarmonic == NULL)
    {
        return;
    }

    //谐波
    CELRecordItemEditOptions* pOption  = pItemHarmnic->GetEditOptions(NULL);
    pOption->AddComboButton();
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_HARM_NONE), HARMONIC_NONE);
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_HARM_DC), HARMONIC_DC);
    
    for (int i = 2; i <= 20; ++i)
    {
        csTemp.Format(_T("%d%s"), i, dsm_stringMgr::Get(DSM_STR_STATE_HARM_TIMES));
        pOption->AddConstraint(csTemp, HARMONIC_NONE + i);
    }

    CELRecordItemConstraint * pItemCon = pOption->FindConstraint(pHarmonic->GetHarmonicType());
    if (pItemCon)
    {
        pItemHarmnic->SetValue(pItemCon->GetConstraintCaption());
    }
    else
    {
        pItemHarmnic->SetValue(dsm_stringMgr::Get(DSM_STR_STATE_EMPTY));
    }
    pItemHarmnic->SetEditable(TRUE);

    //幅值
    CString strContent = _T("");
    if (pHarmonic->GetHarmonicType() == HARMONIC_NONE)
    {
        pItemAmpl->SetValue(dsm_stringMgr::Get(DSM_STR_STATE_EMPTY));
    }
    else
    {
        strContent.Format(_T("%.03lf"), pHarmonic->GetAmplitude());
        pItemAmpl->SetValue(strContent);
        pItemAmpl->SetEditable(TRUE);
        if (strChanName.GetLength() == 0)
        {
            return;
        }

        if (strChanName[0] == _T('U'))
        {
            //电压
            pItemAmpl->SetFormatString(_T("%s V"));
        }
        else if (strChanName[0] == _T('I'))
        {
            //电流
            pItemAmpl->SetFormatString(_T("%s A"));
        }
    }

    
    //
    //相角
    //
    if (pHarmonic->GetHarmonicType() == HARMONIC_NONE || 
        pHarmonic->GetHarmonicType() == HARMONIC_DC)
    {
        pItemPhase->SetValue(dsm_stringMgr::Get(DSM_STR_STATE_EMPTY));
    }
    else
    {
        strContent.Format(_T("%.03lf"), pHarmonic->GetPhaseAngle());
        pItemPhase->SetValue(strContent);
        pItemPhase->SetFormatString(_T("%s°"));
        pItemPhase->SetEditable(TRUE);
    }
}

CDsmStateFaultCalcRecord::CDsmStateFaultCalcRecord(CString strCaption, CString strContent)
{
    AddItem(new CELRecordItemText(strCaption));
    AddItem(new CELRecordItemText(strContent));
}

CDsmStateFaultCompensateRecord::CDsmStateFaultCompensateRecord(CString strCaption0, CString strContent0,CString strCaption1, CString strContent1)
{
    AddItem(new CELRecordItemText(strCaption0))->SetFocusable(FALSE);
    AddItem(new CELRecordItemText(strContent0))->SetFocusable(TRUE);
    AddItem(new CELRecordItemText(strCaption1))->SetFocusable(FALSE);
    AddItem(new CELRecordItemText(strContent1))->SetFocusable(TRUE);
}

CDsmStateFaultImpedanceRecord::CDsmStateFaultImpedanceRecord(CString strCaption0, CString strContent0,CString strCaption1, CString strContent1)
{
    AddItem(new CELRecordItemText(strCaption0))->SetFocusable(FALSE);
    AddItem(new CELRecordItemText(strContent0))->SetFocusable(TRUE);
    AddItem(new CELRecordItemText(strCaption1))->SetFocusable(FALSE);
    AddItem(new CELRecordItemText(strContent1))->SetFocusable(TRUE);
}
CDsmStateResultListRecord::CDsmStateResultListRecord(CStateTestResult* pStateTestResult, int nIndex)
{
    if (pStateTestResult == NULL || !pStateTestResult->IsTested())
    {
        return;
    }
    CString strIndex = _T("");
    CString strTime = _T("");
    CString strResult = _T("");

    //索引
    strIndex.Format(_T("%d"), nIndex);
    AddItem(new CELRecordItemText(strIndex));

    strTime = pStateTestResult->GetStartTime().c_str();
    //时间
    AddItem(new CELRecordItemText(strTime));

    //开入量动作
    CChannelRecvValues* pRecvValues = NULL;
    GOOSE_DATA_VARIANT* pGooseDataVar = NULL;
    CString strChangeDesc  = _T("");
    CString strDIChangeDesc = _T("");
    
    CString strName = _T("");
    for (int i = 0; i < 8; ++i)
    {
        strDIChangeDesc = _T("");
        strName.Format(_T("DI%d"), i + 1);
        pRecvValues = pStateTestResult->GetChannelRecvValues((LPCTSTR)strName);
        if (pRecvValues != NULL)
        {
            if (pRecvValues->GetChangeTimes() > 0)
            {
                //变位
                CHistoryData* pHisData = pRecvValues->FirstRecvData();
                if (pHisData != NULL)
                {
                    pGooseDataVar = pHisData->GetGooseData();
                    if (pGooseDataVar == NULL)
                    {
                        break;
                    }
                    if (pGooseDataVar->GetType() == GSEDT_BOOLEAN)
                    {
                        //单点类型
                        if (pGooseDataVar->m_stBoolean.boolVal)
                        {
                            //on
                            strDIChangeDesc.Format(_T("%s(%s),"),
                                strName,
                                dsm_stringMgr::Get(DSM_STR_STATE_POINT_ON));
                        }
                        else
                        {
                            //off
                            strDIChangeDesc.Format(_T("%s(%s),"),
                                strName,
                                dsm_stringMgr::Get(DSM_STR_STATE_POINT_OFF));
                        }
                    }
                    else if (pGooseDataVar->GetType() == GSEDT_BIT_STR)
                    {
                        if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_INIT])
                        {
                            strDIChangeDesc.Format(_T("%s(%s),"),
                                strName,
                                dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_INIT));
                        }
                        else if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_OFF])
                        {
                            strDIChangeDesc.Format(_T("%s(%s),"),
                                strName,
                                dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_OFF));
                        }
                        else if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_ON])
                        {
                            strDIChangeDesc.Format(_T("%s(%s),"),
                                strName,
                                dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_ON));
                        }
                        else if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_BAD])
                        {
                            strDIChangeDesc.Format(_T("%s(%s),"),
                                strName,
                                dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_BAD));
                        }
                    }
                }
            }
            ;
            strChangeDesc += strDIChangeDesc;
        }
        else
        {
            continue;
        }
    }

    if (strChangeDesc == _T(""))
    {
        //无开入量变化
        strChangeDesc = dsm_stringMgr::Get(DSM_STR_STATE_DI_NO_CHANGE);
    }

    AddItem(new CELRecordItemText(strChangeDesc));
    
}

CDsmStateResultDIActionRecord::CDsmStateResultDIActionRecord(CChannelRecvValues* pChannelRecvValues)
{
    if (pChannelRecvValues == NULL)
    {
        return;
    }

    //DI
    AddItem(new CELRecordItemText(pChannelRecvValues->GetChannelName().c_str()));
    
    //变位次数
    CString strChangeTimes = _T("");
    strChangeTimes.Format(_T("%d"), pChannelRecvValues->GetChangeTimes());
    AddItem(new CELRecordItemText(strChangeTimes));
    
    //变位 选择三次变位数据
    int n = 0;
    CHistoryData* pHisData = pChannelRecvValues->FirstRecvData();
    GOOSE_DATA_VARIANT* pGooseDataVar;
    CString strChangeDesc = _T("");

    while(pHisData && n < 3)
    {
        pGooseDataVar = pHisData->GetGooseData();
        if (pGooseDataVar == NULL)
        {
            break;
        }
        if (pGooseDataVar->GetType() == GSEDT_BOOLEAN)
        {
            //单点类型
            if (pGooseDataVar->m_stBoolean.boolVal)
            {
                //on
                strChangeDesc.Format(_T("%s(%s)"),
                    dsm_stringMgr::Get(DSM_STR_STATE_POINT_ON),
                    pHisData->GetChangeTimeString().c_str());
            }
            else
            {
                //off
                strChangeDesc.Format(_T("%s(%s)"),
                    dsm_stringMgr::Get(DSM_STR_STATE_POINT_OFF),
                    pHisData->GetChangeTimeString().c_str());
            }
        }
        else if (pGooseDataVar->GetType() == GSEDT_BIT_STR)
        {
            if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_INIT])
            {
                strChangeDesc.Format(_T("%s(%s)"),
                    dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_INIT),
                    pHisData->GetChangeTimeString().c_str());
            }
            else if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_OFF])
            {
                strChangeDesc.Format(_T("%s(%s)"),
                    dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_OFF),
                    pHisData->GetChangeTimeString().c_str());
            }
            else if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_ON])
            {
                strChangeDesc.Format(_T("%s(%s)"),
                    dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_ON),
                    pHisData->GetChangeTimeString().c_str());
            }
            else if (pGooseDataVar->m_stBitString.GetString() == strDPValue[DP_VALUE_BAD])
            {
                strChangeDesc.Format(_T("%s(%s)"),
                    dsm_stringMgr::Get(DSM_STR_STATE_D_POINT_BAD),
                    pHisData->GetChangeTimeString().c_str());
            }
        }

        AddItem(new CELRecordItemText(strChangeDesc));
        ++n;
        pHisData = pChannelRecvValues->NextHistoryData(pHisData);
    }
    
    for(; n < 3; ++n)
    {
        AddItem(new CELRecordItemText(_T("")));
    }
}
