/// @file
///     ReportRecord.cpp
///
/// @brief
///     自定义报表中的记录
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.06.03
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.07.27     |1.0.1.727  |取消在获取Goose通道信息时自定义通道的DoName、DaName等     |
///    |2013.07.27     |1.0.1.727  |取消在获取Goose通道信息时自定义通道的DoName、DaName等     |
///    |2013.11.04     |1.0.2.1104 |SMV/GOOSE控制块信息中添加版本号信息                       |
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
// SMV控制块记录
//=============================================================================
//
CSmvCtrlRecord::CSmvCtrlRecord()
{
}

CSmvCtrlRecord::CSmvCtrlRecord(unsigned int ctrlIdx, SCDSMV92ANALYSIS_IEDSMV_INFO &ctrlInfo)
{
	CString str;
	// 控制块序号
	CXTPReportRecordItem *pItem = AddItem(new CXTPReportRecordItemNumber(ctrlIdx));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// AppID
	pItem = AddItem(new CRecordItemAppID(ctrlInfo.smvAppID));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// Mac地址
	pItem = AddItem(new CRecordItemMac(ctrlInfo.smvMAC));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// 通道数目(暂定最多32个)
	CRecordItemNumber *pNumItem = new CRecordItemNumber(ctrlInfo.channelNum);
	pNumItem->SetRange(1,32);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// IED信息("[名称]描述")
	str.Format(_T("[%s]%s"),ctrlInfo.iedName.c_str(),ctrlInfo.iedDesc.c_str());
	pItem = AddItem(new CXTPReportRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);
	pItem->SetEditable(FALSE);

	// 数据集索引（"ldName/LLN0$dataSetName"）
	str.Format(_T("%s%s%s%s"),ctrlInfo.iedName.c_str(),ctrlInfo.iedLdInst.c_str(),_T("/LLN0$"),ctrlInfo.dataSetName.c_str());
	pItem = AddItem(new CXTPReportRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);
	pItem->SetEditable(FALSE);

	// svID
	str.Format(_T("%s"),ctrlInfo.smvCbSmvID.c_str());
	pItem = AddItem(new CRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// 采样率
	pNumItem = new CRecordItemNumber(ctrlInfo.smvCbSmpRate);
	pNumItem->SetRange(1,0xFFFFFFFF);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// ASDU数目
	pNumItem = new CRecordItemNumber(ctrlInfo.smvCbNofASDU);
	pNumItem->SetRange(1,8);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// vlan ID
	pNumItem = new CRecordItemNumber(ctrlInfo.smvVlanID);
	pNumItem->SetRange(0,4095);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// vlan 优先级
	pNumItem = new CRecordItemNumber(ctrlInfo.smvVlanPri);
	pNumItem->SetRange(0,7);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// 是否为组播
	pItem = AddItem(new CRecordItemCheck(ctrlInfo.smvCbMulticast));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// 报文可选项（刷新时间）
	pItem = AddItem(new CRecordItemCheck(ctrlInfo.smvCbOptRefreshTime));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// 报文可选项（采样率）
	pItem = AddItem(new CRecordItemCheck(ctrlInfo.smvCbOptSmpRate));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// 报文可选项（数据集索引）
	pItem = AddItem(new CRecordItemCheck(ctrlInfo.smvCbOptDataRef));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// 报文可选项（同步标志）
	pItem = AddItem(new CRecordItemCheck(ctrlInfo.smvCbOptSmpSync));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// 报文可选项（安全性信息）
	pItem = AddItem(new CRecordItemCheck(ctrlInfo.smvCbOptSecurity));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// 数据集描述
	str.Format(_T("%s"),ctrlInfo.dataSetDesc.c_str());
	pItem = AddItem(new CRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// 控制块索引（"ldName/LLN0$MS$cbName"）
	str.Format(_T("%s%s%s%s"),ctrlInfo.iedName.c_str(),ctrlInfo.iedLdInst.c_str(),_T("/LLN0$MS$"),ctrlInfo.smvCbName.c_str());
	pItem = AddItem(new CXTPReportRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);
	pItem->SetEditable(FALSE);

	// 版本号
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

	// 获取AppID
	pInfo->smvAppID = ((CRecordItemAppID *)GetItem(COLUMN_SMV_APPID))->GetValue();

	// 获取Mac
	((CRecordItemMac *)GetItem(COLUMN_SMV_MAC_ADDR))->GetValue(pInfo->smvMAC);

	// 获取IED信息
	str = ((CXTPReportRecordItemText *)GetItem(COLUMN_SMV_IED))->GetValue();
	int pos1 = str.Find(_T("["));
	int pos2 = str.Find(_T("]"));
	pInfo->iedName = str.Mid(pos1+1,pos2-pos1-1);
	pInfo->iedDesc = str.Mid(pos2+1);

	// 获取DataSet信息
	pInfo->channelNum = (unsigned short)((CRecordItemNumber *)GetItem(COLUMN_SMV_CHANNEL_NUM))->GetValue();
	pInfo->dataSetDesc = ((CRecordItemText *)GetItem(COLUMN_SMV_DATASET_DESC))->GetValue();
	str = ((CXTPReportRecordItemText *)GetItem(COLUMN_SMV_DATASETREF))->GetValue();
	pos1 = str.Find(_T("$"));
	pInfo->smvCbDataSet = pInfo->dataSetName = str.Mid(pos1+1);

	// 获取LdName
	pos1 = pInfo->iedName.length();
	pos2 = str.Find(_T("/"));
	pInfo->iedLdInst = str.Mid(pos1,pos2-pos1);

	// 获取svID
	str = ((CRecordItemText *)GetItem(COLUMN_SMV_SVID))->GetValue();
	pInfo->smvCbSmvID = str;

	// 获取采样率
	pInfo->smvCbSmpRate = (unsigned int)((CRecordItemNumber *)GetItem(COLUMN_SMV_SMPRATE))->GetValue();

	// 获取NofASDU
	pInfo->smvCbNofASDU = (unsigned int)((CRecordItemNumber *)GetItem(COLUMN_SMV_NOFASDU))->GetValue();

	// 获取VLan ID
	pInfo->smvVlanID = (unsigned short)((CRecordItemNumber *)GetItem(COLUMN_SMV_VLANID))->GetValue();

	// 获取Vlan 优先级
	pInfo->smvVlanPri = (unsigned char)((CRecordItemNumber *)GetItem(COLUMN_SMV_VLAN_PRI))->GetValue();

	// 获取Multicast设置
	pInfo->smvCbMulticast = ((CRecordItemCheck *)GetItem(COLUMN_SMV_IS_MULTICAST))->GetValue();

	// 获取RefreshTime设置
	pInfo->smvCbOptRefreshTime = ((CRecordItemCheck *)GetItem(COLUMN_SMV_REFRTM_OPT))->GetValue();

	// 获取SmpRate设置
	pInfo->smvCbOptSmpRate = ((CRecordItemCheck *)GetItem(COLUMN_SMV_SMPRATE_OPT))->GetValue();

	// 获取DataRef设置
	pInfo->smvCbOptDataRef = ((CRecordItemCheck *)GetItem(COLUMN_SMV_DATASET_OPT))->GetValue();

	// 获取SmpSync设置
	pInfo->smvCbOptSmpSync = ((CRecordItemCheck *)GetItem(COLUMN_SMV_SMPSYNC_OPT))->GetValue();

	// 获取Security设置
	pInfo->smvCbOptSecurity = ((CRecordItemCheck *)GetItem(COLUMN_SMV_SECURITY_OPT))->GetValue();

	// 获取控制块名称
	str = ((CXTPReportRecordItemText *)GetItem(COLUMN_SMV_SVCBREF))->GetValue();
	pos1 = str.Find(_T("$"));
	pos2 = str.Find(_T("$"),pos1+1);
	pInfo->smvCbName = str.Mid(pos2+1);

	// 获取版本号
	pInfo->smvCbConfRev = (unsigned int)((CRecordItemNumber *)GetItem(COLUMN_SMV_CONFREV))->GetValue();
}

//
//=============================================================================
// SMV通道信息记录的标题
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
// SMV通道信息记录，代表一个通道
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
	// 通道序号
	CXTPReportRecordItem *pItem = AddItem(new CRecordItemNumber(channelIdx));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// 通道类型
	pItem = AddItem(new CRecordItemSmvChannelType(cType));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// 通道相别
	pItem = AddItem(new CRecordItemSmvPhase(phase));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// 通道的逻辑节点描述
	str.Format(_T("%s"),channelInfo.lnDesc.c_str());
	pItem = AddItem(new CRecordItemText(str,RT_SMV_CHANNEL));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// 通道的数据实例描述
	str.Format(_T("%s"),channelInfo.doDesc.c_str());
	pItem = AddItem(new CRecordItemText(str,RT_SMV_CHANNEL));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// 通道的dU描述
	str.Format(_T("%s"),channelInfo.daVal.c_str());
	pItem = AddItem(new CRecordItemText(str,RT_SMV_CHANNEL));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// 通道索引
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

	// 获取报表中的信息
	pInfo->idxSeq = (int)((CRecordItemNumber *)GetItem(COLUMN_SMV_CHANNEL_IDX))->GetValue()-1;
	pInfo->lnDesc = ((CRecordItemText *)GetItem(COLUMN_SMV_LNDESC))->GetValue();
	pInfo->doDesc = ((CRecordItemText *)GetItem(COLUMN_SMV_DODESC))->GetValue();
	pInfo->daVal  = ((CRecordItemText *)GetItem(COLUMN_SMV_DUDESC))->GetValue();
}

//
//=============================================================================
// Goose控制块记录
//=============================================================================
//
CGooseCtrlRecord::CGooseCtrlRecord()
{

}

CGooseCtrlRecord::CGooseCtrlRecord(unsigned int ctrlIdx, SCDGOOSEANALYSIS_IEDGOOSE_INFO &ctrlInfo)
{
	CString str;

	// 控制块序号
	CXTPReportRecordItem *pItem = AddItem(new CXTPReportRecordItemNumber(ctrlIdx));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// AppID
	pItem = AddItem(new CRecordItemAppID(ctrlInfo.gooseAppID,RT_GOOSE_CTRL));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// Mac地址
	pItem = AddItem(new CRecordItemMac(ctrlInfo.gooseMAC,RT_GOOSE_CTRL));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// 通道数目
	CRecordItemNumber *pNumItem = new CRecordItemNumber(ctrlInfo.channelNum,RT_GOOSE_CTRL);
	pNumItem->SetRange(1,500);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// IED信息（[IEDName]描述）
	str.Format(_T("[%s]%s"),ctrlInfo.iedName.c_str(),ctrlInfo.iedDesc.c_str());
	pItem = AddItem(new CXTPReportRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);
	pItem->SetEditable(FALSE);

	// 数据集索引（"ldName/LLN0$dataSetName"）
	str.Format(_T("%s%s%s%s"),ctrlInfo.iedName.c_str(),ctrlInfo.iedLdInst.c_str(),_T("/LLN0$"),ctrlInfo.dataSetName.c_str());
	pItem = AddItem(new CXTPReportRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);
	pItem->SetEditable(FALSE);

	// goID
	str.Format(_T("%s"),ctrlInfo.gooseID.c_str());
 	pItem = AddItem(new CRecordItemText(str,RT_GOOSE_CTRL));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// goCbRef（"ldName/LLN0$GO$cbName"）
 	str.Format(_T("%s%s%s%s"),ctrlInfo.iedName.c_str(),ctrlInfo.iedLdInst.c_str(),_T("/LLN0$GO$"),ctrlInfo.cbName.c_str());
	pItem = AddItem(new CXTPReportRecordItemText(str));
	pItem->SetTextColor(CTRL_TEXT_COLOR);
	pItem->SetEditable(FALSE);

	// vlan ID
	pNumItem = new CRecordItemNumber(ctrlInfo.gooseVlanID,RT_GOOSE_CTRL);
	pNumItem->SetRange(0,4095);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// vlan 优先级
	pNumItem = new CRecordItemNumber(ctrlInfo.gooseVlanPri,RT_GOOSE_CTRL);
	pNumItem->SetRange(0,7);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// 变位报文发出的最短时间间隔<=T0/8
	pNumItem = new CRecordItemNumber(ctrlInfo.gooseMinTime,RT_GOOSE_CTRL);
	pNumItem->SetRange(1,7499);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// 心跳时间T0
	pNumItem = new CRecordItemNumber(ctrlInfo.gooseMaxTime,RT_GOOSE_CTRL);
	pNumItem->SetRange(1,59999);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// 数据集描述
	str.Format(_T("%s"),ctrlInfo.dataSetDesc.c_str());
	pItem = AddItem(new CRecordItemText(str,RT_GOOSE_CTRL));
	pItem->SetTextColor(CTRL_TEXT_COLOR);

	// 版本号
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

	// 获取AppID
	pInfo->gooseAppID = ((CRecordItemAppID *)GetItem(COLUMN_GOOSE_APPID))->GetValue();

	// 获取Mac
	((CRecordItemMac *)GetItem(COLUMN_GOOSE_MAC_ADDR))->GetValue(pInfo->gooseMAC);

	// 获取IED信息
	str = ((CXTPReportRecordItemText *)GetItem(COLUMN_GOOSE_IED))->GetValue();
	int pos1 = str.Find(_T("["));
	int pos2 = str.Find(_T("]"));
	pInfo->iedName = str.Mid(pos1+1,pos2-pos1-1);
	pInfo->iedDesc = str.Mid(pos2+1);

	// 获取DataSet信息
	pInfo->channelNum = (unsigned short)((CRecordItemNumber *)GetItem(COLUMN_GOOSE_CHANNEL_NUM))->GetValue();
	pInfo->dataSetDesc = ((CRecordItemText *)GetItem(COLUMN_GOOSE_DATASET_DESC))->GetValue();
	str = ((CXTPReportRecordItemText *)GetItem(COLUMN_GOOSE_DATASETREF))->GetValue();
	pos1 = str.Find(_T("$"));
	pInfo->dataSet = pInfo->dataSetName = str.Mid(pos1+1);

	// 获取LdName
	pos1 = pInfo->iedName.length();
	pos2 = str.Find(_T("/"));
	pInfo->iedLdInst = str.Mid(pos1,pos2-pos1);

	// 获取goID
	str = ((CRecordItemText *)GetItem(COLUMN_GOOSE_GOID))->GetValue();
	pInfo->gooseID = str;

	// 获取VLan ID
	pInfo->gooseVlanID = (unsigned short)((CRecordItemNumber *)GetItem(COLUMN_GOOSE_VLANID))->GetValue();

	// 获取Vlan 优先级
	pInfo->gooseVlanPri = (unsigned char)((CRecordItemNumber *)GetItem(COLUMN_GOOSE_VLAN_PRI))->GetValue();

	// 获取minTime
	pInfo->gooseMinTime = (unsigned int)((CRecordItemNumber *)GetItem(COLUMN_GOOSE_MINTIME))->GetValue();

	// 获取maxTime
	pInfo->gooseMaxTime = (unsigned int)((CRecordItemNumber *)GetItem(COLUMN_GOOSE_MAXTIME))->GetValue();

	// 获取控制块名称
	str = ((CXTPReportRecordItemText *)GetItem(COLUMN_GOOSE_GOCBREF))->GetValue();
	pos1 = str.Find(_T("$"));
	pos2 = str.Find(_T("$"),pos1+1);
	pInfo->cbName = str.Mid(pos2+1);

	// 获取版本号
	pInfo->confRev = (unsigned int)((CRecordItemNumber *)GetItem(COLUMN_GOOSE_CONFREV))->GetValue();
}

//
//=============================================================================
// Goose通道信息记录的标题
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
// Goose通道信息记录，代表一个通道
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
	// 通道序号
	CXTPReportRecordItem *pItem = AddItem(new CXTPReportRecordItemNumber(channelIdx));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// 通道类型
	pItem = AddItem(new CRecordItemGooseChannelType(cType));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// 子通道数目
	CRecordItemNumber *pNumItem = new CRecordItemNumber(subChannelNum,RT_GOOSE_CHANNEL);
	pNumItem->SetRange(0,10);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// 通道的逻辑节点描述
	str.Format(_T("%s"),channelInfo.lnDesc.c_str());
	pItem = AddItem(new CRecordItemText(str,RT_GOOSE_CHANNEL));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// 通道的数据实例描述
	str.Format(_T("%s"),channelInfo.doDesc.c_str());
	pItem = AddItem(new CRecordItemText(str,RT_GOOSE_CHANNEL));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// 通道的dU描述
	str.Format(_T("%s"),channelInfo.dUVal.c_str());
	pItem = AddItem(new CRecordItemText(str,RT_GOOSE_CHANNEL));
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	// 通道索引
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
// Goose子通道信息记录的标题
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
// Goose子通道信息记录，代表子一个通道
//=============================================================================
//
CGooseSubChannelRecord::CGooseSubChannelRecord()
{

}

CGooseSubChannelRecord::CGooseSubChannelRecord( int level, unsigned int channelIdx, CHANNEL_TYPE_GOOSE cType, int subChannelNum )
{
	CString str;

	// 该子通道序号
	CXTPReportRecordItem *pItem = AddItem(new CXTPReportRecordItemNumber(channelIdx));
	pItem->SetTextColor(SUBCHANNEL_TEXT_COLOR);

	// 该子通道类型
	pItem = AddItem(new CRecordItemGooseChannelType(cType,RT_GOOSE_SUB_CHANNEL));
	pItem->SetTextColor(SUBCHANNEL_TEXT_COLOR);

	// 该子通道的子通道数目
	CRecordItemNumber *pNumItem = new CRecordItemNumber(subChannelNum,RT_GOOSE_SUB_CHANNEL);
	pNumItem->SetRange(0,10);
	pItem = AddItem(pNumItem);
	pItem->SetTextColor(CHANNEL_TEXT_COLOR);

	m_iChannelLevel = level;

	// 子通道所指向的DA的名称
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
// 	// 根据通道类型，设置默认daName
// 	CHANNEL_TYPE_GOOSE channelType = ((CRecordItemGooseChannelType *)GetItem(COLUMN_GOOSE_CHANNEL_TYPE))->GetValue();
// 	CSCLManager::getInstance()->GetDefDataTemplate_DaName(channelType,pInfo->daName);
}

int  CGooseSubChannelRecord::GetChannelLevel()
{
	return m_iChannelLevel;
}
