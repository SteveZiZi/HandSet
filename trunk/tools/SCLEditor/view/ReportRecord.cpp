/// @file
///     ReportRecord.cpp
///
/// @brief
///     �Զ��屨���еļ�¼
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.06.03
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.07.27     |1.0.1.727  |ȡ���ڻ�ȡGooseͨ����Ϣʱ�Զ���ͨ����DoName��DaName��     |
///    |2013.07.27     |1.0.1.727  |ȡ���ڻ�ȡGooseͨ����Ϣʱ�Զ���ͨ����DoName��DaName��     |
///    |2013.11.04     |1.0.2.1104 |SMV/GOOSE���ƿ���Ϣ����Ӱ汾����Ϣ                       |
///

#include "stdafx.h"
#include "../SCLEditor.h"
#include "ReportRecord.h"

#include "ReportRecordItem.h"
#include "../ResourceStr.h"
#include "../base/StrManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_SERIAL(CSmvCtrlRecord, CXTPReportRecord, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)
IMPLEMENT_SERIAL(CSmvChannelTitleRecord, CSmvChannelRecord, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)
IMPLEMENT_SERIAL(CSmvChannelRecord, CSmvChannelRecord, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)

IMPLEMENT_SERIAL(CGooseCtrlRecord, CXTPReportRecord, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)
IMPLEMENT_SERIAL(CGooseChannelTitleRecord, CSmvChannelRecord, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)
IMPLEMENT_SERIAL(CGooseChannelRecord, CSmvChannelRecord, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)
IMPLEMENT_SERIAL(CGooseSubChannelTitleRecord, CSmvChannelRecord, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)
IMPLEMENT_SERIAL(CGooseSubChannelRecord, CSmvChannelRecord, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)

//
//=============================================================================
// SMV���ƿ��¼
//=============================================================================
//
CSmvCtrlRecord::CSmvCtrlRecord()
{
}

CSmvCtrlRecord::CSmvCtrlRecord(unsigned int ctrlIdx, SCDSMV92ANALYSIS_IEDSMV_INFO &ctrlInfo)
{
	CString str;
	// ���ƿ����
	CXTPReportRecordItem *pItem = AddItem(new CXTPReportRecordItemNumber(ctrlIdx));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// AppID
	pItem = AddItem(new CRecordItemAppID(ctrlInfo.smvAppID));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// Mac��ַ
	pItem = AddItem(new CRecordItemMac(ctrlInfo.smvMAC));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// ͨ����Ŀ(�ݶ����32��)
	CRecordItemNumber *pNumItem = new CRecordItemNumber(ctrlInfo.channelNum);
	pNumItem->SetRange(1,32);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// IED��Ϣ("[����]����")
	str.Format(_T("[%s]%s"),ctrlInfo.iedName.c_str(),ctrlInfo.iedDesc.c_str());
	pItem = AddItem(new CXTPReportRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);
	pItem->SetEditable(FALSE);

	// ���ݼ�������"ldName/LLN0$dataSetName"��
	str.Format(_T("%s%s%s%s"),ctrlInfo.iedName.c_str(),ctrlInfo.iedLdInst.c_str(),_T("/LLN0$"),ctrlInfo.dataSetName.c_str());
	pItem = AddItem(new CXTPReportRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);
	pItem->SetEditable(FALSE);

	// svID
	str.Format(_T("%s"),ctrlInfo.smvCbSmvID.c_str());
	pItem = AddItem(new CRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// ������
	pNumItem = new CRecordItemNumber(ctrlInfo.smvCbSmpRate);
	pNumItem->SetRange(1,0xFFFFFFFF);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// ASDU��Ŀ
	pNumItem = new CRecordItemNumber(ctrlInfo.smvCbNofASDU);
	pNumItem->SetRange(1,8);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// vlan ID
	pNumItem = new CRecordItemNumber(ctrlInfo.smvVlanID);
	pNumItem->SetRange(0,4095);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// vlan ���ȼ�
	pNumItem = new CRecordItemNumber(ctrlInfo.smvVlanPri);
	pNumItem->SetRange(0,7);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// �Ƿ�Ϊ�鲥
	pItem = AddItem(new CRecordItemCheck(ctrlInfo.smvCbMulticast));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// ���Ŀ�ѡ�ˢ��ʱ�䣩
	pItem = AddItem(new CRecordItemCheck(ctrlInfo.smvCbOptRefreshTime));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// ���Ŀ�ѡ������ʣ�
	pItem = AddItem(new CRecordItemCheck(ctrlInfo.smvCbOptSmpRate));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// ���Ŀ�ѡ����ݼ�������
	pItem = AddItem(new CRecordItemCheck(ctrlInfo.smvCbOptDataRef));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// ���Ŀ�ѡ�ͬ����־��
	pItem = AddItem(new CRecordItemCheck(ctrlInfo.smvCbOptSmpSync));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// ���Ŀ�ѡ���ȫ����Ϣ��
	pItem = AddItem(new CRecordItemCheck(ctrlInfo.smvCbOptSecurity));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// ���ݼ�����
	str.Format(_T("%s"),ctrlInfo.dataSetDesc.c_str());
	pItem = AddItem(new CRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// ���ƿ�������"ldName/LLN0$MS$cbName"��
	str.Format(_T("%s%s%s%s"),ctrlInfo.iedName.c_str(),ctrlInfo.iedLdInst.c_str(),_T("/LLN0$MS$"),ctrlInfo.smvCbName.c_str());
	pItem = AddItem(new CXTPReportRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);
	pItem->SetEditable(FALSE);

	// �汾��
	pNumItem = new CRecordItemNumber(ctrlInfo.smvCbConfRev);
	pNumItem->SetRange(1,0xFFFFFFFF);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);
}

CSmvCtrlRecord::~CSmvCtrlRecord()
{

}

void CSmvCtrlRecord::GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics)
{
	CXTPReportRecord::GetItemMetrics(pDrawArgs, pItemMetrics);
	pItemMetrics->clrBackground = CTRL_RECORD_COLOR;
}

void CSmvCtrlRecord::GetRecordInfo(void *pCtrlInfo)
{
	CString str;
	SCDSMV92ANALYSIS_IEDSMV_INFO *pInfo = (SCDSMV92ANALYSIS_IEDSMV_INFO *)pCtrlInfo;

	// ��ȡAppID
	pInfo->smvAppID = ((CRecordItemAppID *)GetItem(COLUMN_SMV_APPID))->GetValue();

	// ��ȡMac
	((CRecordItemMac *)GetItem(COLUMN_SMV_MAC_ADDR))->GetValue(pInfo->smvMAC);

	// ��ȡIED��Ϣ
	str = ((CXTPReportRecordItemText *)GetItem(COLUMN_SMV_IED))->GetValue();
	int pos1 = str.Find(_T("["));
	int pos2 = str.Find(_T("]"));
	pInfo->iedName = str.Mid(pos1+1,pos2-pos1-1);
	pInfo->iedDesc = str.Mid(pos2+1);

	// ��ȡDataSet��Ϣ
	pInfo->channelNum = (unsigned short)((CRecordItemNumber *)GetItem(COLUMN_SMV_CHANNEL_NUM))->GetValue();
	pInfo->dataSetDesc = ((CRecordItemText *)GetItem(COLUMN_SMV_DATASET_DESC))->GetValue();
	str = ((CXTPReportRecordItemText *)GetItem(COLUMN_SMV_DATASETREF))->GetValue();
	pos1 = str.Find(_T("$"));
	pInfo->smvCbDataSet = pInfo->dataSetName = str.Mid(pos1+1);

	// ��ȡLdName
	pos1 = pInfo->iedName.length();
	pos2 = str.Find(_T("/"));
	pInfo->iedLdInst = str.Mid(pos1,pos2-pos1);

	// ��ȡsvID
	str = ((CRecordItemText *)GetItem(COLUMN_SMV_SVID))->GetValue();
	pInfo->smvCbSmvID = str;

	// ��ȡ������
	pInfo->smvCbSmpRate = (unsigned int)((CRecordItemNumber *)GetItem(COLUMN_SMV_SMPRATE))->GetValue();

	// ��ȡNofASDU
	pInfo->smvCbNofASDU = (unsigned int)((CRecordItemNumber *)GetItem(COLUMN_SMV_NOFASDU))->GetValue();

	// ��ȡVLan ID
	pInfo->smvVlanID = (unsigned short)((CRecordItemNumber *)GetItem(COLUMN_SMV_VLANID))->GetValue();

	// ��ȡVlan ���ȼ�
	pInfo->smvVlanPri = (unsigned char)((CRecordItemNumber *)GetItem(COLUMN_SMV_VLAN_PRI))->GetValue();

	// ��ȡMulticast����
	pInfo->smvCbMulticast = ((CRecordItemCheck *)GetItem(COLUMN_SMV_IS_MULTICAST))->GetValue();

	// ��ȡRefreshTime����
	pInfo->smvCbOptRefreshTime = ((CRecordItemCheck *)GetItem(COLUMN_SMV_REFRTM_OPT))->GetValue();

	// ��ȡSmpRate����
	pInfo->smvCbOptSmpRate = ((CRecordItemCheck *)GetItem(COLUMN_SMV_SMPRATE_OPT))->GetValue();

	// ��ȡDataRef����
	pInfo->smvCbOptDataRef = ((CRecordItemCheck *)GetItem(COLUMN_SMV_DATASET_OPT))->GetValue();

	// ��ȡSmpSync����
	pInfo->smvCbOptSmpSync = ((CRecordItemCheck *)GetItem(COLUMN_SMV_SMPSYNC_OPT))->GetValue();

	// ��ȡSecurity����
	pInfo->smvCbOptSecurity = ((CRecordItemCheck *)GetItem(COLUMN_SMV_SECURITY_OPT))->GetValue();

	// ��ȡ���ƿ�����
	str = ((CXTPReportRecordItemText *)GetItem(COLUMN_SMV_SVCBREF))->GetValue();
	pos1 = str.Find(_T("$"));
	pos2 = str.Find(_T("$"),pos1+1);
	pInfo->smvCbName = str.Mid(pos2+1);

	// ��ȡ�汾��
	pInfo->smvCbConfRev = (unsigned int)((CRecordItemNumber *)GetItem(COLUMN_SMV_CONFREV))->GetValue();
}

//
//=============================================================================
// SMVͨ����Ϣ��¼�ı���
//=============================================================================
//
CSmvChannelTitleRecord::CSmvChannelTitleRecord()
{
	CreateDefaultRecord();
}

CSmvChannelTitleRecord::~CSmvChannelTitleRecord()
{

}

void CSmvChannelTitleRecord::CreateDefaultRecord()
{
	// Initialize record items with empty values

	CXTPReportRecordItem *pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_SMV_CHANNEL_IDX)));
	pItem->SetTextColor(CHANNEL_TITLE_TEXT_COLOR);
	pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_SMV_CHANNEL_TYPE)));
	pItem->SetTextColor(CHANNEL_TITLE_TEXT_COLOR);
	pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_SMV_PHASE)));
	pItem->SetTextColor(CHANNEL_TITLE_TEXT_COLOR);
	pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_SMV_LNDESC)));
	pItem->SetTextColor(CHANNEL_TITLE_TEXT_COLOR);
	pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_SMV_DODESC)));
	pItem->SetTextColor(CHANNEL_TITLE_TEXT_COLOR);
	pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_SMV_DUDESC)));
	pItem->SetTextColor(CHANNEL_TITLE_TEXT_COLOR);
// 	pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_SMV_CHANNEL_REF)));
// 	pItem->SetTextColor(CHANNEL_TITLE_TEXT_COLOR);
}

void CSmvChannelTitleRecord::GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics)
{
	CXTPReportRecord::GetItemMetrics(pDrawArgs, pItemMetrics);
	pItemMetrics->clrBackground = CHANNEL_TITLE_RECORD_COLOR;
}

//
//=============================================================================
// SMVͨ����Ϣ��¼������һ��ͨ��
//=============================================================================
//
CSmvChannelRecord::CSmvChannelRecord()
{

}

CSmvChannelRecord::CSmvChannelRecord(
									 unsigned int channelIdx, CHANNEL_TYPE_SMV cType, CHANNEL_PHASE_SMV phase,
									 std::wstring &iedName, SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &channelInfo
							   )
{
	CString str;
	// ͨ�����
	CXTPReportRecordItem *pItem = AddItem(new CRecordItemNumber(channelIdx));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// ͨ������
	pItem = AddItem(new CRecordItemSmvChannelType(cType));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// ͨ�����
	pItem = AddItem(new CRecordItemSmvPhase(phase));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// ͨ�����߼��ڵ�����
	str.Format(_T("%s"),channelInfo.lnDesc.c_str());
	pItem = AddItem(new CRecordItemText(str,RT_SMV_CHANNEL));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// ͨ��������ʵ������
	str.Format(_T("%s"),channelInfo.doDesc.c_str());
	pItem = AddItem(new CRecordItemText(str,RT_SMV_CHANNEL));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// ͨ����dU����
	str.Format(_T("%s"),channelInfo.daVal.c_str());
	pItem = AddItem(new CRecordItemText(str,RT_SMV_CHANNEL));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// ͨ������
// 	std::wstring lnName = channelInfo.lnPrefix+channelInfo.lnClass+channelInfo.lnInst;
// 	str.Format(_T("%s%s/%s.%s"),iedName.c_str(),channelInfo.ldInst.c_str(),lnName.c_str(),channelInfo.doName.c_str());
// 	pItem = AddItem(new CXTPReportRecordItemText(str));
// 	pItem->SetTextColor(CHANNEL_TEXT_COLOR);
// 	pItem->SetEditable(FALSE);
}

CSmvChannelRecord::~CSmvChannelRecord()
{

}

void CSmvChannelRecord::GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics)
{
	CXTPReportRecord::GetItemMetrics(pDrawArgs, pItemMetrics);
	pItemMetrics->clrBackground = CHANNEL_RECORD_COLOR;
}

void CSmvChannelRecord::GetRecordInfo(void *pChannelInfo)
{
	CString str;
	SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *pInfo = (SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *)pChannelInfo;

	// ��ȡ�����е���Ϣ
	pInfo->idxSeq = (int)((CRecordItemNumber *)GetItem(COLUMN_SMV_CHANNEL_IDX))->GetValue()-1;
	pInfo->lnDesc = ((CRecordItemText *)GetItem(COLUMN_SMV_LNDESC))->GetValue();
	pInfo->doDesc = ((CRecordItemText *)GetItem(COLUMN_SMV_DODESC))->GetValue();
	pInfo->daVal  = ((CRecordItemText *)GetItem(COLUMN_SMV_DUDESC))->GetValue();
}

//
//=============================================================================
// Goose���ƿ��¼
//=============================================================================
//
CGooseCtrlRecord::CGooseCtrlRecord()
{

}

CGooseCtrlRecord::CGooseCtrlRecord(unsigned int ctrlIdx, SCDGOOSEANALYSIS_IEDGOOSE_INFO &ctrlInfo)
{
	CString str;

	// ���ƿ����
	CXTPReportRecordItem *pItem = AddItem(new CXTPReportRecordItemNumber(ctrlIdx));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// AppID
	pItem = AddItem(new CRecordItemAppID(ctrlInfo.gooseAppID,RT_GOOSE_CTRL));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// Mac��ַ
	pItem = AddItem(new CRecordItemMac(ctrlInfo.gooseMAC,RT_GOOSE_CTRL));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// ͨ����Ŀ
	CRecordItemNumber *pNumItem = new CRecordItemNumber(ctrlInfo.channelNum,RT_GOOSE_CTRL);
	pNumItem->SetRange(1,500);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// IED��Ϣ��[IEDName]������
	str.Format(_T("[%s]%s"),ctrlInfo.iedName.c_str(),ctrlInfo.iedDesc.c_str());
	pItem = AddItem(new CXTPReportRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);
	pItem->SetEditable(FALSE);

	// ���ݼ�������"ldName/LLN0$dataSetName"��
	str.Format(_T("%s%s%s%s"),ctrlInfo.iedName.c_str(),ctrlInfo.iedLdInst.c_str(),_T("/LLN0$"),ctrlInfo.dataSetName.c_str());
	pItem = AddItem(new CXTPReportRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);
	pItem->SetEditable(FALSE);

	// goID
	str.Format(_T("%s"),ctrlInfo.gooseID.c_str());
 	pItem = AddItem(new CRecordItemText(str,RT_GOOSE_CTRL));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// goCbRef��"ldName/LLN0$GO$cbName"��
 	str.Format(_T("%s%s%s%s"),ctrlInfo.iedName.c_str(),ctrlInfo.iedLdInst.c_str(),_T("/LLN0$GO$"),ctrlInfo.cbName.c_str());
	pItem = AddItem(new CXTPReportRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);
	pItem->SetEditable(FALSE);

	// vlan ID
	pNumItem = new CRecordItemNumber(ctrlInfo.gooseVlanID,RT_GOOSE_CTRL);
	pNumItem->SetRange(0,4095);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// vlan ���ȼ�
	pNumItem = new CRecordItemNumber(ctrlInfo.gooseVlanPri,RT_GOOSE_CTRL);
	pNumItem->SetRange(0,7);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// ��λ���ķ��������ʱ����<=T0/8
	pNumItem = new CRecordItemNumber(ctrlInfo.gooseMinTime,RT_GOOSE_CTRL);
	pNumItem->SetRange(1,7499);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// ����ʱ��T0
	pNumItem = new CRecordItemNumber(ctrlInfo.gooseMaxTime,RT_GOOSE_CTRL);
	pNumItem->SetRange(1,59999);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// ���ݼ�����
	str.Format(_T("%s"),ctrlInfo.dataSetDesc.c_str());
	pItem = AddItem(new CRecordItemText(str,RT_GOOSE_CTRL));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// �汾��
	pNumItem = new CRecordItemNumber(ctrlInfo.confRev,RT_GOOSE_CTRL);
	pNumItem->SetRange(1,0xFFFFFFFF);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);
}

CGooseCtrlRecord::~CGooseCtrlRecord()
{

}

void CGooseCtrlRecord::GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics)
{
	CXTPReportRecord::GetItemMetrics(pDrawArgs, pItemMetrics);
	pItemMetrics->clrBackground = CTRL_RECORD_COLOR;
}

void CGooseCtrlRecord::GetRecordInfo(void *pCtrlInfo)
{
	CString str;
	SCDGOOSEANALYSIS_IEDGOOSE_INFO *pInfo = (SCDGOOSEANALYSIS_IEDGOOSE_INFO *)pCtrlInfo;

	// ��ȡAppID
	pInfo->gooseAppID = ((CRecordItemAppID *)GetItem(COLUMN_GOOSE_APPID))->GetValue();

	// ��ȡMac
	((CRecordItemMac *)GetItem(COLUMN_GOOSE_MAC_ADDR))->GetValue(pInfo->gooseMAC);

	// ��ȡIED��Ϣ
	str = ((CXTPReportRecordItemText *)GetItem(COLUMN_GOOSE_IED))->GetValue();
	int pos1 = str.Find(_T("["));
	int pos2 = str.Find(_T("]"));
	pInfo->iedName = str.Mid(pos1+1,pos2-pos1-1);
	pInfo->iedDesc = str.Mid(pos2+1);

	// ��ȡDataSet��Ϣ
	pInfo->channelNum = (unsigned short)((CRecordItemNumber *)GetItem(COLUMN_GOOSE_CHANNEL_NUM))->GetValue();
	pInfo->dataSetDesc = ((CRecordItemText *)GetItem(COLUMN_GOOSE_DATASET_DESC))->GetValue();
	str = ((CXTPReportRecordItemText *)GetItem(COLUMN_GOOSE_DATASETREF))->GetValue();
	pos1 = str.Find(_T("$"));
	pInfo->dataSet = pInfo->dataSetName = str.Mid(pos1+1);

	// ��ȡLdName
	pos1 = pInfo->iedName.length();
	pos2 = str.Find(_T("/"));
	pInfo->iedLdInst = str.Mid(pos1,pos2-pos1);

	// ��ȡgoID
	str = ((CRecordItemText *)GetItem(COLUMN_GOOSE_GOID))->GetValue();
	pInfo->gooseID = str;

	// ��ȡVLan ID
	pInfo->gooseVlanID = (unsigned short)((CRecordItemNumber *)GetItem(COLUMN_GOOSE_VLANID))->GetValue();

	// ��ȡVlan ���ȼ�
	pInfo->gooseVlanPri = (unsigned char)((CRecordItemNumber *)GetItem(COLUMN_GOOSE_VLAN_PRI))->GetValue();

	// ��ȡminTime
	pInfo->gooseMinTime = (unsigned int)((CRecordItemNumber *)GetItem(COLUMN_GOOSE_MINTIME))->GetValue();

	// ��ȡmaxTime
	pInfo->gooseMaxTime = (unsigned int)((CRecordItemNumber *)GetItem(COLUMN_GOOSE_MAXTIME))->GetValue();

	// ��ȡ���ƿ�����
	str = ((CXTPReportRecordItemText *)GetItem(COLUMN_GOOSE_GOCBREF))->GetValue();
	pos1 = str.Find(_T("$"));
	pos2 = str.Find(_T("$"),pos1+1);
	pInfo->cbName = str.Mid(pos2+1);

	// ��ȡ�汾��
	pInfo->confRev = (unsigned int)((CRecordItemNumber *)GetItem(COLUMN_GOOSE_CONFREV))->GetValue();
}

//
//=============================================================================
// Gooseͨ����Ϣ��¼�ı���
//=============================================================================
//
CGooseChannelTitleRecord::CGooseChannelTitleRecord()
{
	CreateDefaultRecord();
}

CGooseChannelTitleRecord::~CGooseChannelTitleRecord()
{

}

void CGooseChannelTitleRecord::CreateDefaultRecord()
{
	// Initialize record items with empty values

	CXTPReportRecordItem *pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_GOOSE_CHANNEL_IDX)));
	pItem->SetTextColor(CHANNEL_TITLE_TEXT_COLOR);
	pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_GOOSE_CHANNEL_TYPE)));
	pItem->SetTextColor(CHANNEL_TITLE_TEXT_COLOR);
	pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_GOOSE_SUB_CHANNEL_NUM)));
	pItem->SetTextColor(CHANNEL_TITLE_TEXT_COLOR);
	pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_GOOSE_LNDESC)));
	pItem->SetTextColor(CHANNEL_TITLE_TEXT_COLOR);
	pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_GOOSE_DODESC)));
	pItem->SetTextColor(CHANNEL_TITLE_TEXT_COLOR);
	pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_GOOSE_DUDESC)));
	pItem->SetTextColor(CHANNEL_TITLE_TEXT_COLOR);
// 	pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_GOOSE_CHANNEL_REF)));
// 	pItem->SetTextColor(CHANNEL_TITLE_TEXT_COLOR);
}

void CGooseChannelTitleRecord::GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics)
{
	CXTPReportRecord::GetItemMetrics(pDrawArgs, pItemMetrics);
	pItemMetrics->clrBackground = CHANNEL_TITLE_RECORD_COLOR;
}

//
//=============================================================================
// Gooseͨ����Ϣ��¼������һ��ͨ��
//=============================================================================
//
CGooseChannelRecord::CGooseChannelRecord()
{

}

CGooseChannelRecord::CGooseChannelRecord(
	unsigned int channelIdx, CHANNEL_TYPE_GOOSE cType, int subChannelNum,
	std::wstring &iedName, SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo )
{
	CString str;
	// ͨ�����
	CXTPReportRecordItem *pItem = AddItem(new CXTPReportRecordItemNumber(channelIdx));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// ͨ������
	pItem = AddItem(new CRecordItemGooseChannelType(cType));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// ��ͨ����Ŀ
	CRecordItemNumber *pNumItem = new CRecordItemNumber(subChannelNum,RT_GOOSE_CHANNEL);
	pNumItem->SetRange(0,10);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// ͨ�����߼��ڵ�����
	str.Format(_T("%s"),channelInfo.lnDesc.c_str());
	pItem = AddItem(new CRecordItemText(str,RT_GOOSE_CHANNEL));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// ͨ��������ʵ������
	str.Format(_T("%s"),channelInfo.doDesc.c_str());
	pItem = AddItem(new CRecordItemText(str,RT_GOOSE_CHANNEL));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// ͨ����dU����
	str.Format(_T("%s"),channelInfo.dUVal.c_str());
	pItem = AddItem(new CRecordItemText(str,RT_GOOSE_CHANNEL));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// ͨ������
// 	std::wstring lnName = channelInfo.lnPrefix+channelInfo.lnClass+channelInfo.lnInst;
// 	str.Format(_T("%s%s/%s.%s"),iedName.c_str(),channelInfo.ldInst.c_str(),lnName.c_str(),channelInfo.doName.c_str());
// 	pItem = AddItem(new CXTPReportRecordItemText(str));
// 	pItem->SetTextColor(CHANNEL_TEXT_COLOR);
}

CGooseChannelRecord::~CGooseChannelRecord()
{

}

void CGooseChannelRecord::GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics)
{
	CXTPReportRecord::GetItemMetrics(pDrawArgs, pItemMetrics);
	pItemMetrics->clrBackground = CHANNEL_RECORD_COLOR;
}

void CGooseChannelRecord::GetRecordInfo(void *pChannelInfo)
{
	CString str;
	SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *pInfo = (SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *)pChannelInfo;

	pInfo->idxSeq = (int)((CRecordItemNumber *)GetItem(COLUMN_GOOSE_CHANNEL_IDX))->GetValue()-1;
	pInfo->lnDesc = ((CRecordItemText *)GetItem(COLUMN_GOOSE_LNDESC))->GetValue();
	pInfo->doDesc = ((CRecordItemText *)GetItem(COLUMN_GOOSE_DODESC))->GetValue();
	pInfo->dUVal  = ((CRecordItemText *)GetItem(COLUMN_GOOSE_DUDESC))->GetValue();
}

//
//=============================================================================
// Goose��ͨ����Ϣ��¼�ı���
//=============================================================================
//
CGooseSubChannelTitleRecord::CGooseSubChannelTitleRecord()
{
	CreateDefaultRecord();
}

CGooseSubChannelTitleRecord::~CGooseSubChannelTitleRecord()
{

}

void CGooseSubChannelTitleRecord::CreateDefaultRecord()
{
	// Initialize record items with empty values

	CXTPReportRecordItem *pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_GOOSE_SUB_CHANNEL_IDX)));
	pItem->SetTextColor(SUBCHANNEL_TITLE_TEXT_COLOR);
	pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_GOOSE_SUB_CHANNEL_TYPE)));
	pItem->SetTextColor(SUBCHANNEL_TITLE_TEXT_COLOR);
	pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_GOOSE_SUB_CHANNEL_NUM)));
	pItem->SetTextColor(SUBCHANNEL_TITLE_TEXT_COLOR);
// 	pItem = AddItem(new CXTPReportRecordItemText(CStrManager::Get(RCSTR_GOOSE_SUB_CHANNEL_DANAME)));
// 	pItem->SetTextColor(SUBCHANNEL_TITLE_TEXT_COLOR);
}

void CGooseSubChannelTitleRecord::GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics)
{
	CXTPReportRecord::GetItemMetrics(pDrawArgs, pItemMetrics);
	pItemMetrics->clrBackground = SUBCHANNEL_TITLE_RECORD_COLOR;
}

//
//=============================================================================
// Goose��ͨ����Ϣ��¼��������һ��ͨ��
//=============================================================================
//
CGooseSubChannelRecord::CGooseSubChannelRecord()
{

}

CGooseSubChannelRecord::CGooseSubChannelRecord( int level, unsigned int channelIdx, CHANNEL_TYPE_GOOSE cType, int subChannelNum )
{
	CString str;

	// ����ͨ�����
	CXTPReportRecordItem *pItem = AddItem(new CXTPReportRecordItemNumber(channelIdx));
	pItem->SetTextColor(SUBCHANNEL_TEXT_COLOR);

	// ����ͨ������
	pItem = AddItem(new CRecordItemGooseChannelType(cType,RT_GOOSE_SUB_CHANNEL));
	pItem->SetTextColor(SUBCHANNEL_TEXT_COLOR);

	// ����ͨ������ͨ����Ŀ
	CRecordItemNumber *pNumItem = new CRecordItemNumber(subChannelNum,RT_GOOSE_SUB_CHANNEL);
	pNumItem->SetRange(0,10);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	m_iChannelLevel = level;

	// ��ͨ����ָ���DA������
//	str.Format(_T("%s"),daName.c_str());
// 	pItem = AddItem(new CXTPReportRecordItemText(str));
// 	pItem->SetTextColor(SUBCHANNEL_TEXT_COLOR);
}

CGooseSubChannelRecord::~CGooseSubChannelRecord()
{

}

void CGooseSubChannelRecord::GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics)
{
	CXTPReportRecord::GetItemMetrics(pDrawArgs, pItemMetrics);
	pItemMetrics->clrBackground = SUBCHANNEL_RECORD_COLOR;
}

void CGooseSubChannelRecord::GetRecordInfo(void *pChannelInfo)
{
	SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *pInfo = (SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *)pChannelInfo;

	pInfo->idxSeq = (int)((CRecordItemNumber *)GetItem(COLUMN_GOOSE_SUB_CHANNEL_IDX))->GetValue()-1;
// 
// 	// ����ͨ�����ͣ�����Ĭ��daName
// 	CHANNEL_TYPE_GOOSE channelType = ((CRecordItemGooseChannelType *)GetItem(COLUMN_GOOSE_CHANNEL_TYPE))->GetValue();
// 	CSCLManager::getInstance()->GetDefDataTemplate_DaName(channelType,pInfo->daName);
}

int  CGooseSubChannelRecord::GetChannelLevel()
{
	return m_iChannelLevel;
}
