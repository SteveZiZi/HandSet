/// @file
///     ReportRecordItme.cpp
///
/// @brief
///     自定义报表中的记录的记录项
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
///    |2013.07.27     |1.0.1.727  |修改原来以AppID为参考的自定义通道的方式为以iedName+ldInst+dataSetName为参考，
///                                 避免因AppID改变而引起搜索不到某通道等Bug|
///    |2013.08.07     |1.0.1.807  |添加AppID记录项初始化时对字母进行大写转换操作            |
///    |2013.08.09     |1.0.1.809  |修正AppID修改无效时重置AppID没进行大写字母转换的Bug      |
///

#include "stdafx.h"
#include "../SCLEditor.h"
#include "ReportRecordItem.h"

#include "../MainFrm.h"
#include "ReportRecord.h"
#include "../ResourceStr.h"
#include "../base/StrManager.h"
#include "src/utils/CommonControl/MacEditCtrl/MacEditCtrl.h"

///
/// @brief
///     添加Goose子通道信息
///
void AddGooseSubChannelRecord(int channelLevel, const SCDGOOSEANALYSIS_IEDGOOSE_INFO &ctrlInfo,
							  const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &parentChannel,
							  int subChannelNum, CXTPReportRecord *pParentRecord )
{
	// 获取并添加子通道信息
	CXTPReportRecord *pTitleItem = pParentRecord->GetChilds()->Add(new CGooseSubChannelTitleRecord());
	pTitleItem->SetEditable(FALSE);
	SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfoArr = new SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO[subChannelNum];
	if ( CSCLManager::getInstance()->GetAllIEDGOOSESubChannelInfo(ctrlInfo.iedName,ctrlInfo.iedLdInst,
		parentChannel,subChannelInfoArr,subChannelNum) != -1 )
	{
		for ( int i = 0; i < subChannelNum; i++ )
		{
			CHANNEL_TYPE_GOOSE channelType = CSCLManager::getInstance()->GetChannelType(subChannelInfoArr[i]);
			int subSubChannelNum = CSCLManager::getInstance()->GetIEDGOOSESubChannelNum(
				ctrlInfo.iedName,ctrlInfo.iedLdInst,subChannelInfoArr[i]);
			CGooseSubChannelRecord *subChannelRecord = new CGooseSubChannelRecord(channelLevel,i+1,channelType,subSubChannelNum);
			pParentRecord->GetChilds()->Add(subChannelRecord);

			if ( subSubChannelNum > 0)
			{
				AddGooseSubChannelRecord(channelLevel+1,ctrlInfo,subChannelInfoArr[i],subSubChannelNum,subChannelRecord);
			}
		}
	}
	delete[] subChannelInfoArr;
}

//
//=============================================================================
// 复选框记录项
//=============================================================================
//
CRecordItemCheck::CRecordItemCheck(BOOL bCheck /* = FALSE */, RECORD_TYPE eRecordType /* = RT_SMV_CTRL */)
{
	HasCheckbox(TRUE);
	SetChecked(bCheck);
	m_eRecordType = eRecordType;
}

///
/// @brief
///     获取当前Check状态对应的在CStrManager中的字符串ID
///
int CRecordItemCheck::GetGroupCaptionID(CXTPReportColumn* /*pColumn*/)
{
	return IsChecked()? RCSTR_CHECK_STATE_TRUE: RCSTR_CHECK_STATE_TRUE;
}

int CRecordItemCheck::Compare(CXTPReportColumn* /*pColumn*/, CXTPReportRecordItem* pItem)
{
	return int(IsChecked()) - int(pItem->IsChecked());
}

void  CRecordItemCheck::OnClick(XTP_REPORTRECORDITEM_CLICKARGS* pClickArgs)
{
	CXTPReportRecordItem::OnClick(pClickArgs);

	// 更新控制块信息
	switch ( m_eRecordType )
	{
	case RT_SMV_CTRL:
		UpdateSmvCtrlRecord(pClickArgs);
		break;
	default:
		break;
	}
}

BOOL CRecordItemCheck::GetValue()
{
	return m_bChecked;
}

///
/// @brief
///     更新SMV控制块信息
///
void CRecordItemCheck::UpdateSmvCtrlRecord(XTP_REPORTRECORDITEM_CLICKARGS* pClickArgs)
{
	SCDSMV92ANALYSIS_IEDSMV_INFO ctrlInfo;
	((CSmvCtrlRecord *)GetRecord())->GetRecordInfo((void *)&ctrlInfo);
	CSCLManager::getInstance()->SetOneIEDSMVInfoByCtrlRef(ctrlInfo);
}

//
//=============================================================================
// AppID数据记录项
//=============================================================================
//
CRecordItemAppID::CRecordItemAppID(unsigned short appID, RECORD_TYPE eRecordType /* = RT_SMV_CTRL */)
{
	CString str;
	str.Format(_T("%04x"),appID);
	str.MakeUpper();
	SetValue(str);
	m_eRecordType = eRecordType;
	m_sAppID   = appID;
}

void CRecordItemAppID::OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText)
{
	// 字符串处理:小写->大写，缺少部分补0
	CString str = szText;
	str.MakeUpper();
	int pad = 4 - str.GetLength();
	for ( int i = 0; i < pad; i++ )
	{
		str.Insert(0,'0');
	}
	CXTPReportRecordItemText::OnEditChanged(pItemArgs, str);

	// 获取appID值
	unsigned int ui;
	str = this->m_strText;
	swscanf_s(str.GetString(),L"%x",&ui);
	unsigned short appID  = (unsigned short)ui;

	// 更新AppID信息
	switch ( m_eRecordType )
	{
	case RT_SMV_CTRL:
		UpdateSmvCtrlRecord(appID);
		break;
	case RT_GOOSE_CTRL:
		UpdateGooseCtrlRecord(appID);
		break;
	default:
		break;
	}
}

BOOL CRecordItemAppID::OnEditChanging(XTP_REPORTRECORDITEM_ARGS* pItemArgs, CString& rstrNewText)
{
	// 限制字符数
	if ( rstrNewText.GetLength() > 4 )
		return FALSE;

	// 限制字符类型
	for ( int i = 0; i < rstrNewText.GetLength(); i++ )
	{
		if ( !(( rstrNewText.GetAt(i) >= '0' && rstrNewText.GetAt(i) <= '9' ) || 
			( rstrNewText.GetAt(i) >= 'A' && rstrNewText.GetAt(i) <= 'F' ) ||
			( rstrNewText.GetAt(i) >= 'a' && rstrNewText.GetAt(i) <= 'f' )) )
		{
			return FALSE;
		}
	}
	return CXTPReportRecordItemText::OnEditChanging(pItemArgs,rstrNewText);
}

unsigned short CRecordItemAppID::GetValue()
{
	return m_sAppID;
}

void CRecordItemAppID::UpdateSmvCtrlRecord(unsigned short appID)
{
	if ( appID < 0x4000 || appID > 0x7FFF ||
		CSCLManager::getInstance()->IsSmvAppIDExisted(appID) == 0 )
	{
		// AppID超出范围或已存在，则恢复原AppID
		CString str;
		str.Format(_T("%04x"),m_sAppID);
		str.MakeUpper();
		SetValue(str);

		// 在状态栏显示提示信息
		CMainFrame *pMain =(CMainFrame *)AfxGetMainWnd();
		if ( appID < 0x4000 || appID > 0x7FFF )
			pMain->SetStatusBarText(RCSTR_APPID_SMV_RANGE_ERROR);
		else
			pMain->SetStatusBarText(RCSTR_APPID_EXISTED);
	}
	else
	{
		// 更新AppID信息
		m_sAppID = appID;
		SCDSMV92ANALYSIS_IEDSMV_INFO ctrlInfo;
		((CSmvCtrlRecord *)GetRecord())->GetRecordInfo((void *)&ctrlInfo);
		CSCLManager::getInstance()->SetOneIEDSMVInfoByCtrlRef(ctrlInfo);
	}
}

void CRecordItemAppID::UpdateGooseCtrlRecord(unsigned short appID)
{
	if ( appID < 0x0000 || appID > 0x3FFF ||
		CSCLManager::getInstance()->IsGOOSEAppIDExisted(appID) == 0 )
	{
		// AppID超出范围或已存在，则恢复原AppID
		CString str;
		str.Format(_T("%04x"),m_sAppID);
		str.MakeUpper();
		SetValue(str);

		// 在状态栏显示提示信息
		CMainFrame *pMain =(CMainFrame *)AfxGetMainWnd();
		if ( appID < 0x0000 || appID > 0x3FFF )
			pMain->SetStatusBarText(RCSTR_APPID_GOOSE_RANGE_ERROR);
		else
			pMain->SetStatusBarText(RCSTR_APPID_EXISTED);
	}
	else
	{
		// 更新AppID信息
		m_sAppID = appID;
		SCDGOOSEANALYSIS_IEDGOOSE_INFO ctrlInfo;
		((CGooseCtrlRecord *)GetRecord())->GetRecordInfo((void *)&ctrlInfo);
		CSCLManager::getInstance()->SetOneIEDGOOSEInfoByCtrlRef(ctrlInfo);
	}
}

//
//=============================================================================
// Mac地址记录项
//=============================================================================
//
CRecordItemMac::CRecordItemMac(unsigned char *mac ,RECORD_TYPE eRecordType /* = RT_SMV_CTRL */)
{
	CString str;
	str.Format(_T("%02x-%02x-%02x-%02x-%02x-%02x"),mac[0],mac[1],mac[2],
		mac[3],mac[4],mac[5]);
	str.MakeUpper();
	SetValue(str);
	m_eRecordType = eRecordType;
}

void CRecordItemMac::OnDblClick(XTP_REPORTRECORDITEM_CLICKARGS* pClickArgs)
{
	if (!pClickArgs || !pClickArgs->pControl)
		return;

	// 获取Mac
	unsigned char mac[6];
	CMacEditCtrl macEdit;
	CString str1 = ((CXTPReportRecordItemText *)pClickArgs->pItem)->GetValue();
	macEdit.Str2MacAddr(str1,mac);

	
	CXTPReportControl* pControl = pClickArgs->pControl;

	// 设置编辑控件的字体
	CFont *pFont = pControl->GetFont();
	if ( pFont == NULL )
	{
		HGDIOBJ hFont = GetStockObject(DEFAULT_GUI_FONT);
		m_cFont.Attach(hFont);
	}
	else
	{
		m_cFont.Attach(pFont->GetSafeHandle());
	}
	macEdit.SetTextFont(&m_cFont);

	// 显示编辑控件,并获取编辑结果
	CPoint pt(pClickArgs->ptClient);
	pControl->ClientToScreen(&pt);
	CRect rect(pClickArgs->rcItem);
	rect.DeflateRect(1,1);
	macEdit.GoModal(CWnd::FromHandle(WindowFromPoint(pt)),rect,mac);
	
	// 判断Mac是否合法，并更新Mac信息
	switch ( m_eRecordType )
	{
	case RT_SMV_CTRL:
		{
			if ( mac[0] == 0x01 && mac[1] == 0x0C && mac[2] == 0xCD && mac[3] == 0x04)
			{
				CString str2;
				macEdit.MacAddr2Str(mac,str2);
				if ( str1.Compare(str2) != 0 )
				{
					((CXTPReportRecordItemText *)pClickArgs->pItem)->SetValue(str2);
					UpdateSmvCtrlRecord();
				}
			}
			else
			{
				// 在状态栏显示提示信息
				CMainFrame *pMain =(CMainFrame *)AfxGetMainWnd();
				pMain->SetStatusBarText(RCSTR_MAC_SMV_ERROR);
			}
		}
		break;
	case RT_GOOSE_CTRL:
		{
			if ( mac[0] == 0x01 && mac[1] == 0x0C && mac[2] == 0xCD && mac[3] == 0x01 )
			{
				CString str2;
				macEdit.MacAddr2Str(mac,str2);
				if ( str1.Compare(str2) != 0 )
				{
					((CXTPReportRecordItemText *)pClickArgs->pItem)->SetValue(str2);
					UpdateGooseCtrlRecord();
				}
			}
			else
			{
				// 在状态栏显示提示信息
				CMainFrame *pMain =(CMainFrame *)AfxGetMainWnd();
				pMain->SetStatusBarText(RCSTR_MAC_GOOSE_ERROR);
			}
		}
		break;
	default:
		break;
	}

	m_cFont.DeleteObject();
}

void CRecordItemMac::GetValue(unsigned char *mac)
{
	int j = 0;
	CString macStr = this->m_strText;
	for ( int i=0 ; i<17 ;i = i+3)
	{
		unsigned int ui;
		CString str = macStr.Mid(i,2);
		swscanf_s(str.GetString(),L"%x",&ui);
		mac[j] = (unsigned char)ui;
		j++;
	}
}

void CRecordItemMac::UpdateSmvCtrlRecord()
{
	SCDSMV92ANALYSIS_IEDSMV_INFO ctrlInfo;
	((CSmvCtrlRecord *)GetRecord())->GetRecordInfo((void *)&ctrlInfo);
	CSCLManager::getInstance()->SetOneIEDSMVInfoByCtrlRef(ctrlInfo);
}

void CRecordItemMac::UpdateGooseCtrlRecord()
{
	SCDGOOSEANALYSIS_IEDGOOSE_INFO ctrlInfo;
	((CGooseCtrlRecord *)GetRecord())->GetRecordInfo((void *)&ctrlInfo);
	CSCLManager::getInstance()->SetOneIEDGOOSEInfoByCtrlRef(ctrlInfo);
}

//
//=============================================================================
// 一般文本记录项
//=============================================================================
//
CRecordItemText::CRecordItemText(const CString & str,RECORD_TYPE eRecordType /* = RT_SMV_CTRL */)
:CXTPReportRecordItemText(str)
{
	m_eRecordType = eRecordType;
}

void CRecordItemText::OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText)
{
	CXTPReportRecordItemText::OnEditChanged(pItemArgs, szText);

	// 更新信息
	switch ( m_eRecordType )
	{
	case RT_SMV_CTRL:
		UpdateSmvCtrlRecord(pItemArgs,szText);
		break;
	case RT_SMV_CHANNEL:
		UpdateSmvChannelRecord(pItemArgs,szText);
		break;
	case RT_GOOSE_CTRL:
		UpdateGooseCtrlRecord(pItemArgs,szText);
		break;
	case RT_GOOSE_CHANNEL:
		UpdateGooseChannelRecord(pItemArgs,szText);
		break;
	default:
		break;
	}
}

void CRecordItemText::UpdateSmvCtrlRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText)
{
	SCDSMV92ANALYSIS_IEDSMV_INFO ctrlInfo;
	((CSmvCtrlRecord *)GetRecord())->GetRecordInfo((void *)&ctrlInfo);
	CSCLManager::getInstance()->SetOneIEDSMVInfoByCtrlRef(ctrlInfo);
}

void CRecordItemText::UpdateSmvChannelRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText)
{
	// 获取控制块信息
	SCDSMV92ANALYSIS_IEDSMV_INFO ctrlInfo;
	((CSmvCtrlRecord *)(GetRecord()->GetParentRecord()))->GetRecordInfo((void *)&ctrlInfo);

	// 获取通道信息
	SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO channelInfo;
	((CSmvChannelRecord *)GetRecord())->GetRecordInfo(&channelInfo);
	channelInfo.dataSetName = ctrlInfo.dataSetName;
	channelInfo.dataSetDesc = ctrlInfo.dataSetDesc;
	channelInfo.ldInst      = ctrlInfo.iedLdInst;

	CHANNEL_TYPE_SMV eChannelType = ((CRecordItemSmvChannelType *)(GetRecord()->GetItem(COLUMN_SMV_CHANNEL_TYPE)))->GetValue();

	// 更新缓存信息
	// 由于该通道所属逻辑节点或数据在其他地方用到，直接更改需同步修改，比较繁琐
	// 因此直接替换为新的自定义通道（即添加自定义数据）
	CSCLManager::getInstance()->SetOneIEDSMVChannelInfo_Def(ctrlInfo.iedName,ctrlInfo.iedLdInst,channelInfo,eChannelType);
}

void CRecordItemText::UpdateGooseCtrlRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText)
{
	SCDGOOSEANALYSIS_IEDGOOSE_INFO ctrlInfo;
	((CGooseCtrlRecord *)GetRecord())->GetRecordInfo((void *)&ctrlInfo);
	CSCLManager::getInstance()->SetOneIEDGOOSEInfoByCtrlRef(ctrlInfo);
}

void CRecordItemText::UpdateGooseChannelRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText)
{
	// 获取控制块信息
	SCDGOOSEANALYSIS_IEDGOOSE_INFO ctrlInfo;
	((CGooseCtrlRecord *)(GetRecord()->GetParentRecord()))->GetRecordInfo((void *)&ctrlInfo);

	// 获取通道信息
	SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO channelInfo;
	((CGooseChannelRecord *)GetRecord())->GetRecordInfo(&channelInfo);
	CHANNEL_TYPE_GOOSE eChannelType = ((CRecordItemGooseChannelType *)(GetRecord()->GetItem(COLUMN_GOOSE_CHANNEL_TYPE)))->GetValue();
	channelInfo.dataSetName = ctrlInfo.dataSetName;
	channelInfo.dataSetDesc = ctrlInfo.dataSetDesc;
	channelInfo.ldInst      = ctrlInfo.iedLdInst;

	// 更新缓存信息
	CSCLManager::getInstance()->SetOneIEDGOOSEChannelInfo_Def(ctrlInfo.iedName,ctrlInfo.iedLdInst,channelInfo,eChannelType);
}

//
//=============================================================================
// 一般数值记录项
//=============================================================================
//
CRecordItemNumber::CRecordItemNumber(const double dVal,RECORD_TYPE eRecordType /* = RT_SMV_CTRL */)
:CXTPReportRecordItemNumber(dVal)
{
	m_eRecordType = eRecordType;
	m_dMinVal     = 2.22507e-308;
	m_dMaxVal     = 1.79769e+308;

	CString str;
	str.Format(_T("%d"),(int)m_dMinVal);
	int len1 = str.GetLength();
	str.Format(_T("%d"),(int)m_dMaxVal);
	int len2 = str.GetLength();
	m_iLenLimit = max(len1,len2);
}

BOOL CRecordItemNumber::OnEditChanging(XTP_REPORTRECORDITEM_ARGS* pItemArgs, CString& rstrNewText)
{
	// 限制字符数
	if ( rstrNewText.GetLength() > m_iLenLimit )
		return FALSE;

	return CXTPReportRecordItemNumber::OnEditChanging(pItemArgs,rstrNewText);
}

void CRecordItemNumber::OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText)
{
	double prevVal = GetValue();

	CXTPReportRecordItemNumber::OnEditChanged(pItemArgs, szText);

	// 限制数值范围
	double curVal = GetValue();
	if ( curVal > m_dMaxVal )
	{
		SetValue(m_dMaxVal);
		curVal = m_dMaxVal;
		UINT columnID = pItemArgs->pColumn->GetIndex();
		UpdateStatusBarText(m_eRecordType,columnID);
	}
	if ( curVal < m_dMinVal )
	{
		SetValue(m_dMinVal);
		curVal = m_dMinVal;
		UINT columnID = pItemArgs->pColumn->GetIndex();
		UpdateStatusBarText(m_eRecordType,columnID);
	}

	// 更新缓存信息
	switch ( m_eRecordType )
	{
	case RT_SMV_CTRL:
		UpdateSmvCtrlRecord(pItemArgs,(int)curVal,(int)prevVal);
		break;
	case RT_GOOSE_CTRL:
		UpdateGooseCtrlRecord(pItemArgs,(int)curVal,(int)prevVal);
		break;
	case RT_GOOSE_CHANNEL:
		UpdateGooseChannelRecord(pItemArgs,(int)curVal,(int)prevVal);
		break;
	case RT_GOOSE_SUB_CHANNEL:
		UpdateGooseSubChannelRecord(pItemArgs,(int)curVal,(int)prevVal);
		break;
	default:
		break;
	}

	pItemArgs->pControl->Populate();
}

void CRecordItemNumber::SetRange(const double minVal, const double maxVal)
{
	m_dMinVal = minVal;
	m_dMaxVal = maxVal;

	CString str;
	str.Format(_T("%d"),(int)m_dMinVal);
	int len1 = str.GetLength();
	str.Format(_T("%d"),(int)m_dMaxVal);
	int len2 = str.GetLength();
	m_iLenLimit = max(len1,len2);
}

void CRecordItemNumber::UpdateSmvCtrlRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, int val, int prevVal)
{
	// 获取SMV控制块的信息
	SCDSMV92ANALYSIS_IEDSMV_INFO ctrlInfo;
	((CSmvCtrlRecord *)GetRecord())->GetRecordInfo((void *)&ctrlInfo);

	// 该数值为控制块关联的通道数量，则更新通道信息
	if ( pItemArgs->pColumn->GetIndex() == COLUMN_SMV_CHANNEL_NUM )
	{
		// 获取控制块关联的通道信息
		int channelNum = CSCLManager::getInstance()->GetIEDSMVChannelNum(ctrlInfo.iedName,ctrlInfo.iedLdInst,ctrlInfo.smvCbName);
		if ( channelNum < 0 )
			return;

		//  移除多余的Record
		CXTPReportRecords *pRecords = pItemArgs->pRow->GetRecord()->GetChilds();
		int count  = pRecords->GetCount();
		if ( val < channelNum )
		{
			for ( int i = channelNum ; i > val; i-- )
			{
				if ( i >= 0 && i < count)
				{
					pRecords->RemoveAt(i);
					CSCLManager::getInstance()->RemoveOneIEDSMVChannelInfo(ctrlInfo.iedName,ctrlInfo.iedLdInst,
						ctrlInfo.dataSetName,i-1);
				}
			}
		}
		else if ( val > channelNum )
		{
			SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO channelInfo;
			if ( channelNum == 0 )
			{
				// 添加一个单点通道
				channelInfo.idxSeq = 0;
				channelInfo.dataSetName = ctrlInfo.dataSetName;
				channelInfo.dataSetDesc = ctrlInfo.dataSetDesc;
				channelInfo.ldInst      = ctrlInfo.iedLdInst;
				if ( CSCLManager::getInstance()->AddOneIEDSMVChannelInfo_Def(ctrlInfo.iedName,
					ctrlInfo.iedLdInst,channelInfo,CT_SMV_CURRENT) == -1 )
				{
					TRACE(_T("CRecordItemNumber::UpdateSmvCtrlRecord:AddOneIEDSMVChannelInfo_Def Error\r\n"));
					return;
				}
				// 拷贝通道信息
				if ( CSCLManager::getInstance()->CopyIEDSMVChannelInfoToSetEnd(ctrlInfo.iedName,
					ctrlInfo.iedLdInst,ctrlInfo.dataSetName,0,val-1) == -1 )
				{
					TRACE(_T("CRecordItemNumber::UpdateSmvCtrlRecord:CopyIEDSMVChannelInfoToSetEnd Error\r\n"));
					return;
				}
			}
			else
			{
				// 获取末尾通道的信息
				if ( CSCLManager::getInstance()->GetOneIEDSMVDataSetInfo(ctrlInfo.iedName,
					ctrlInfo.iedLdInst,ctrlInfo.smvCbName,channelNum-1,&channelInfo) == -1 )
				{
					TRACE(_T("CRecordItemNumber::UpdateSmvCtrlRecord:GetOneIEDSMVDataSetInfo Error\r\n"));
					return;
				}

				// 拷贝通道信息
				if ( CSCLManager::getInstance()->CopyIEDSMVChannelInfoToSetEnd(ctrlInfo.iedName,
					ctrlInfo.iedLdInst,ctrlInfo.dataSetName,channelNum-1,val-channelNum) == -1 )
				{
					TRACE(_T("CRecordItemNumber::UpdateSmvCtrlRecord:CopyIEDSMVChannelInfoToSetEnd Error\r\n"));
					return;
				}
			}

			// 更新报表的通道信息
			if ( channelNum == 0 )
			{
				pRecords->Add(new CSmvChannelTitleRecord());
			}
			CHANNEL_TYPE_SMV eChannelType = CSCLManager::getInstance()->GetChannelType(channelInfo);
			CHANNEL_PHASE_SMV phase = CSCLManager::getInstance()->GetChannelPhase(channelInfo,eChannelType);
			for ( int i = channelNum; i < val; i++ )
			{
				channelInfo.idxSeq = i;
				pRecords->Add(new CSmvChannelRecord(i+1,eChannelType,phase,ctrlInfo.iedName,channelInfo));
			}
		}
	}

	// 更新SMV控制块的信息
	CSCLManager::getInstance()->SetOneIEDSMVInfoByCtrlRef(ctrlInfo);
}

void CRecordItemNumber::UpdateGooseCtrlRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, int val, int prevVal)
{
	// 获取GOOSE控制块的信息
	SCDGOOSEANALYSIS_IEDGOOSE_INFO ctrlInfo;
	((CGooseCtrlRecord *)GetRecord())->GetRecordInfo((void *)&ctrlInfo);

	// 该数值为控制块关联的通道数量，则更新通道信息
	if ( pItemArgs->pColumn->GetIndex() == COLUMN_GOOSE_CHANNEL_NUM )
	{
		// 获取控制块关联的通道信息
		int channelNum = CSCLManager::getInstance()->GetIEDGOOSEChannelNum(ctrlInfo.iedName,ctrlInfo.iedLdInst,ctrlInfo.cbName);
		if ( channelNum < 0 )
			return;

		//  移除多余的Record
		CXTPReportRecords *pRecords = pItemArgs->pRow->GetRecord()->GetChilds();
		int count  = pRecords->GetCount();
		if ( val < channelNum )
		{
			for ( int i = channelNum ; i > val; i-- )
			{
				if ( i >= 0 && i < count)
				{
					CSCLManager::getInstance()->RemoveOneIEDGOOSEChannelInfo(ctrlInfo.iedName,ctrlInfo.iedLdInst,
						ctrlInfo.dataSetName,i-1);
					pRecords->RemoveAt(i);
				}
			}
		}
		else if ( val > channelNum )
		{
			
			SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO channelInfo;

			if ( channelNum == 0 )
			{
				// 添加一个单点通道
				channelInfo.idxSeq = 0;
				channelInfo.dataSetName = ctrlInfo.dataSetName;
				channelInfo.dataSetDesc = ctrlInfo.dataSetDesc;
				channelInfo.ldInst      = ctrlInfo.iedLdInst;
				if ( CSCLManager::getInstance()->AddOneIEDGOOSEDataSetInfo_Def(ctrlInfo.iedName,
					 ctrlInfo.iedLdInst,channelInfo,CT_GOOSE_POINT) == -1 )
				{
					TRACE(_T("CRecordItemNumber::UpdateGooseCtrlRecord:AddOneIEDGOOSEDataSetInfo_Def Error\r\n"));
					return;
				}
				// 拷贝通道信息
				if ( CSCLManager::getInstance()->CopyIEDGOOSEDataSetInfoToSetEnd(ctrlInfo.iedName,
					ctrlInfo.iedLdInst,ctrlInfo.dataSetName,0,val-1) == -1 )
				{
					TRACE(_T("CRecordItemNumber::UpdateGooseCtrlRecord:CopyIEDGOOSEDataSetInfoToSetEnd Error\r\n"));
					return;
				}
			}
			else
			{
				// 获取末尾通道的信息
				if ( CSCLManager::getInstance()->GetOneIEDGOOSEDataSetInfo(ctrlInfo.iedName,
					 ctrlInfo.iedLdInst,ctrlInfo.cbName,channelNum-1,&channelInfo) == -1 )
				{
					TRACE(_T("CRecordItemNumber::UpdateGooseCtrlRecord:GetOneIEDGOOSEDataSetInfo Error\r\n"));
					return;
				}

				// 拷贝通道信息
				if ( CSCLManager::getInstance()->CopyIEDGOOSEDataSetInfoToSetEnd(ctrlInfo.iedName,
					 ctrlInfo.iedLdInst,ctrlInfo.dataSetName,channelNum-1,val-channelNum) == -1 )
				{
					TRACE(_T("CRecordItemNumber::UpdateGooseCtrlRecord:CopyIEDGOOSEDataSetInfoToSetEnd Error\r\n"));
					return;
				}
			}
			
			// 更新报表的通道信息
			if ( channelNum == 0 )
			{
				pRecords->Add(new CGooseChannelTitleRecord());
			}
			CHANNEL_TYPE_GOOSE eChannelType = CSCLManager::getInstance()->GetChannelType(channelInfo);
			for ( int i = channelNum; i < val; i++ )
			{
				int subChannelNum = CSCLManager::getInstance()->GetIEDGOOSESubChannelNum(
					ctrlInfo.iedName,ctrlInfo.iedLdInst,channelInfo);
				CXTPReportRecord *pChannelRecord = pRecords->Add(new CGooseChannelRecord(i+1,eChannelType,
					subChannelNum,ctrlInfo.iedName,channelInfo));
				if ( subChannelNum > 0 )
				{
					AddGooseSubChannelRecord(0,ctrlInfo,channelInfo,subChannelNum,pChannelRecord);
				}
			}
		}
	}

	// 更新GOOSE控制块的信息
	if ( CSCLManager::getInstance()->SetOneIEDGOOSEInfoByCtrlRef(ctrlInfo) == -1 )
	{
		TRACE(_T("UpdateGooseCtrlRecord:SetOneIEDGOOSEInfoByCtrlRef Error\r\n"));
	}
}

void CRecordItemNumber::UpdateGooseChannelRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, int val, int prevVal)
{
	// 获取GOOSE控制块的信息
	SCDGOOSEANALYSIS_IEDGOOSE_INFO ctrlInfo;
	((CGooseCtrlRecord *)(GetRecord()->GetParentRecord()))->GetRecordInfo((void *)&ctrlInfo);

	// 获取当前通道信息
 	CGooseChannelRecord *pChannelRecord = (CGooseChannelRecord *)GetRecord();
	CHANNEL_TYPE_GOOSE eChannelType = ((CRecordItemGooseChannelType *)(pChannelRecord->GetItem(COLUMN_GOOSE_CHANNEL_TYPE)))->GetValue();
  	SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO channelInfo;

	pChannelRecord->GetRecordInfo(&channelInfo);
	channelInfo.dataSetName = ctrlInfo.dataSetName;
	channelInfo.dataSetDesc = ctrlInfo.dataSetDesc;
	channelInfo.ldInst      = ctrlInfo.iedLdInst;
	
	// 更新GOOSE通道信息
	CSCLManager::getInstance()->SetOneIEDGOOSEChannelInfo_Def(ctrlInfo.iedName,ctrlInfo.iedLdInst,channelInfo,eChannelType);

	// 该数值为控制块关联的通道数量，则更新通道信息
	if ( pItemArgs->pColumn->GetIndex() == COLUMN_GOOSE_SUB_CHANNEL_NUM )
	{
		if ( eChannelType == CT_GOOSE_STRUCT )
		{
			//  移除多余的Record
			CXTPReportRecords *pRecords = pChannelRecord->GetChilds();
			int count  = pRecords->GetCount();
			if ( val < prevVal )
			{
				// 限制子通道数不为0
				if ( val == 0 )
				{
					SetValue(1.0);
					val = 1;
					// 在状态栏显示提示信息
					CMainFrame *pMain =(CMainFrame *)AfxGetMainWnd();
					pMain->SetStatusBarText(RCSTR_GOOSE_SUB_CHANNEL_NUM_RANGE_ERROR1);
				}
				for ( int i = prevVal ; i > val; i-- )
				{
					if ( i >= 0 && i < count)
					{
						int channelIdx = i - 1;
						if ( CSCLManager::getInstance()->RemoveOneIEDGOOSESubChannelInfo(ctrlInfo.iedName,
							 ctrlInfo.iedLdInst,channelInfo,&channelIdx,1) == -1 )
						{
							TRACE(_T("CRecordItemNumber::UpdateGooseChannelRecord:RemoveOneIEDGOOSESubChannelInfo Error\r\n"));
							return;
						}
						pRecords->RemoveAt(i);
					}
				}
			}
			else if ( val > prevVal )
			{
				// 获取当前通道的子通道数
				int subChannelNum = CSCLManager::getInstance()->GetIEDGOOSESubChannelNum(
					ctrlInfo.iedName,ctrlInfo.iedLdInst,channelInfo);
				if ( subChannelNum <= 0 )
					return;

				// 拷贝当前通道的末尾子通道
				int subChannelIdx = subChannelNum-1;
				if ( CSCLManager::getInstance()->CopyIEDGooseSubChannelToEnd(ctrlInfo.iedName,ctrlInfo.iedLdInst,channelInfo,&subChannelIdx,1,val-subChannelNum) == -1 )
				{
					TRACE(_T("CRecordItemNumber::UpdateGooseChannelRecord:CopyIEDGooseSubChannelToEnd Error\r\n"));
					return;
				}

				// 获取当前通道的末尾子通道信息
				SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO subChannelInfo;
				if ( CSCLManager::getInstance()->GetOneIEDGOOSESubChannelInfo(ctrlInfo.iedName,ctrlInfo.iedLdInst,channelInfo,subChannelIdx,subChannelInfo) == -1 )
				{
					TRACE(_T("CRecordItemNumber::UpdateGooseChannelRecord:GetOneIEDGOOSESubChannelInfo Error\r\n"));
					return;
				}

				// 更新报表的通道信息
				CHANNEL_TYPE_GOOSE eChannelType = CSCLManager::getInstance()->GetChannelType(subChannelInfo);
				for ( int i = subChannelNum; i < val; i++ )
				{
					int subSubChannelNum = CSCLManager::getInstance()->GetIEDGOOSESubChannelNum(
						ctrlInfo.iedName,ctrlInfo.iedLdInst,subChannelInfo);
					CXTPReportRecord *pSubChannelRecord = pChannelRecord->GetChilds()->Add(new CGooseSubChannelRecord(0,i+1,eChannelType,subSubChannelNum));
					if ( subSubChannelNum > 0 )
					{
						AddGooseSubChannelRecord(1,ctrlInfo,subChannelInfo,subSubChannelNum,pSubChannelRecord);
					}
				}
			}
		}
		else
		{
			SetValue((double)prevVal);
			// 在状态栏显示提示信息
			CMainFrame *pMain =(CMainFrame *)AfxGetMainWnd();
			pMain->SetStatusBarText(RCSTR_GOOSE_SUB_CHANNEL_NUM_RANGE_ERROR2);
			return;
		}
	}
}

void CRecordItemNumber::UpdateGooseSubChannelRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, int val, int prevVal)
{
	// 获取当前子通道所在的层级及类型
	CGooseSubChannelRecord *pSubChannelRecord = (CGooseSubChannelRecord *)GetRecord();
	CHANNEL_TYPE_GOOSE eChannelType = ((CRecordItemGooseChannelType *)(pSubChannelRecord->GetItem(COLUMN_GOOSE_SUB_CHANNEL_TYPE)))->GetValue();
	int subChannelLevel = pSubChannelRecord->GetChannelLevel();
	ASSERT( subChannelLevel >= 0 );

	// 获取从控制块控制的通道下的第0层子通道开始至当前子通道的顺序号信息
	int *subChannelIdxList = new int[subChannelLevel+2];
	subChannelIdxList[subChannelLevel] = (int)(((CRecordItemNumber *)(pSubChannelRecord->GetItem(COLUMN_GOOSE_SUB_CHANNEL_IDX)))->GetValue())-1;
	for ( int i = subChannelLevel-1; i >= 0; i-- )
	{
		pSubChannelRecord = (CGooseSubChannelRecord *)(pSubChannelRecord->GetParentRecord());
		subChannelIdxList[i] = (int)(((CRecordItemNumber *)(pSubChannelRecord->GetItem(COLUMN_GOOSE_SUB_CHANNEL_IDX)))->GetValue())-1;
	}

	CGooseChannelRecord *pChannelRecord = (CGooseChannelRecord *)(pSubChannelRecord->GetParentRecord());
	
	// 获取控制块信息
	SCDGOOSEANALYSIS_IEDGOOSE_INFO ctrlInfo;
	((CGooseCtrlRecord *)(pChannelRecord->GetParentRecord()))->GetRecordInfo((void *)&ctrlInfo);

	// 获取控制块控制数据集中的通道信息（顶层通道）
	SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO channelInfo;
	int channelIdx = (int)(((CXTPReportRecordItemNumber *)pChannelRecord->GetItem(COLUMN_GOOSE_CHANNEL_IDX))->GetValue())-1;
	CSCLManager::getInstance()->GetOneIEDGOOSEDataSetInfo(ctrlInfo.iedName,ctrlInfo.iedLdInst,ctrlInfo.cbName,
		channelIdx,&channelInfo);

	// 更新子通道信息
	CSCLManager::getInstance()->SetOneIEDGOOSESubChannelInfo(ctrlInfo.iedName,ctrlInfo.iedLdInst,channelInfo,
		subChannelIdxList,subChannelLevel+1,eChannelType);

	// 该数值为控制块关联的通道数量，则更新通道信息
	if ( pItemArgs->pColumn->GetIndex() == COLUMN_GOOSE_SUB_SUB_CHANNEL_NUM )
	{
		if ( eChannelType == CT_GOOSE_STRUCT )
		{
			//  移除多余的Record
			CXTPReportRecords *pRecords = pSubChannelRecord->GetChilds();
			int count  = pRecords->GetCount();
			if ( val < prevVal )
			{
				// 限制子通道数不为0
				if ( val == 0 )
				{
					SetValue(1.0);
					val = 1;
					// 在状态栏显示提示信息
					CMainFrame *pMain =(CMainFrame *)AfxGetMainWnd();
					pMain->SetStatusBarText(RCSTR_GOOSE_SUB_CHANNEL_NUM_RANGE_ERROR1);
				}
				for ( int i = prevVal ; i > val; i-- )
				{
					if ( i >= 0 && i < count)
					{
						subChannelIdxList[subChannelLevel+1] = i-1;
						CSCLManager::getInstance()->RemoveOneIEDGOOSESubChannelInfo(ctrlInfo.iedName,ctrlInfo.iedLdInst,
							channelInfo,subChannelIdxList,subChannelLevel+2);
						pRecords->RemoveAt(i);
					}
				}
			}
			else if ( val > prevVal )
			{
				// 获取当前通道的末尾子通道的信息
				SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO parentChannelInfo = channelInfo;
				SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO subChannelInfo;
				subChannelIdxList[subChannelLevel+1] = prevVal-1;
				for ( int i = 0; i < subChannelLevel+2; i++ )
				{
					if ( CSCLManager::getInstance()->GetOneIEDGOOSESubChannelInfo(ctrlInfo.iedName,ctrlInfo.iedLdInst,parentChannelInfo,subChannelIdxList[i],subChannelInfo) == -1 )
					{
						TRACE(_T("CRecordItemNumber::UpdateGooseChannelRecord:GetOneIEDGOOSESubChannelInfo Error\r\n"));
						return;
					}
					parentChannelInfo = subChannelInfo;
				}

				// 拷贝当前通道的末尾子通道
				if ( CSCLManager::getInstance()->CopyIEDGooseSubChannelToEnd(ctrlInfo.iedName,ctrlInfo.iedLdInst,channelInfo,subChannelIdxList,subChannelLevel+2,val-prevVal) == -1 )
				{
					TRACE(_T("CRecordItemNumber::UpdateGooseChannelRecord:CopyIEDGooseSubChannelToEnd Error\r\n"));
					return;
				}

				// 更新报表的通道信息
				CHANNEL_TYPE_GOOSE eChannelType = CSCLManager::getInstance()->GetChannelType(subChannelInfo);
				for ( int i = prevVal; i < val; i++ )
				{
					int subSubChannelNum = CSCLManager::getInstance()->GetIEDGOOSESubChannelNum(
						ctrlInfo.iedName,ctrlInfo.iedLdInst,subChannelInfo);
					CXTPReportRecord *pSubSubChannelRecord = pSubChannelRecord->GetChilds()->Add(new CGooseSubChannelRecord(subChannelLevel+1,i+1,eChannelType,subSubChannelNum));
					if ( subSubChannelNum > 0 )
					{
						AddGooseSubChannelRecord(subChannelLevel+2,ctrlInfo,subChannelInfo,subSubChannelNum,pSubSubChannelRecord);
					}
				}
			}
		}
		else
		{
			SetValue((double)prevVal);
			// 在状态栏显示提示信息
			CMainFrame *pMain =(CMainFrame *)AfxGetMainWnd();
			pMain->SetStatusBarText(RCSTR_GOOSE_SUB_CHANNEL_NUM_RANGE_ERROR2);
			return;
		}
	}
}

void CRecordItemNumber::UpdateStatusBarText(RECORD_TYPE eRecordType, UINT columnID)
{
	UINT uStrID;
	switch ( eRecordType )
	{
	case RT_SMV_CTRL:
		{
			switch ( columnID )
			{
			case COLUMN_SMV_CHANNEL_NUM:
				uStrID = RCSTR_SMV_CHANNEL_NUM_RANGE_ERROR;
				break;
			case COLUMN_SMV_SMPRATE:
				uStrID = RCSTR_SMV_SMPRATE_RANGE_ERROR;
				break;
			case COLUMN_SMV_NOFASDU:
				uStrID = RCSTR_ASDU_RANGE_ERROR;
				break;
			case COLUMN_SMV_VLANID:
				uStrID = RCSTR_VLAN_ID_RANGE_ERROR;
				break;
			case COLUMN_SMV_VLAN_PRI:
				uStrID = RCSTR_VLAN_PRI_RANGE_ERROR;
				break;
			default:
				uStrID = RCSTR_SMV_CTRL_REPORT;
			}
		}
		break;
	case RT_GOOSE_CTRL:
		{
			switch ( columnID )
			{
			case COLUMN_GOOSE_CHANNEL_NUM:
				uStrID = RCSTR_GOOSE_CHANNEL_NUM_RANGE_ERROR;
				break;
			case COLUMN_GOOSE_VLANID:
				uStrID = RCSTR_VLAN_ID_RANGE_ERROR;
				break;
			case COLUMN_GOOSE_VLAN_PRI:
				uStrID = RCSTR_VLAN_PRI_RANGE_ERROR;
				break;
			case COLUMN_GOOSE_MINTIME:
				uStrID = RCSTR_GOOSE_MIN_TIME_RANGE_ERROR;
				break;
			case COLUMN_GOOSE_MAXTIME:
				uStrID = RCSTR_GOOSE_MAX_TIME_RANGE_ERROR;
				break;		
			default:
				uStrID = RCSTR_GOOSE_CTRL_REPORT;
			}
		}
		break;
	case RT_GOOSE_CHANNEL:
		{
			switch ( columnID )
			{
			case COLUMN_GOOSE_SUB_CHANNEL_NUM:
				uStrID = RCSTR_GOOSE_SUB_CHANNEL_NUM_RANGE_ERROR1;
				break;	
			default:
				uStrID = RCSTR_GOOSE_CTRL_REPORT;
			}
		}
		break;
	default:
		;
	}
	// 在状态栏显示提示信息
	CMainFrame *pMain =(CMainFrame *)AfxGetMainWnd();
	pMain->SetStatusBarText(uStrID);
}

//
//=============================================================================
// SMV相别记录项
//=============================================================================
//
CRecordItemSmvPhase::CRecordItemSmvPhase(CHANNEL_PHASE_SMV smvPhase /* = CP_SMV_PHASE_UNKNOW */)
{
	m_eSmvPhase = smvPhase;
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_SMV_PHASE_UNKONW),CP_SMV_PHASE_UNKNOW);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_SMV_PHASE_A),CP_SMV_PHASE_A);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_SMV_PHASE_B),CP_SMV_PHASE_B);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_SMV_PHASE_C),CP_SMV_PHASE_C);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_SMV_PHASE_N),CP_SMV_PHASE_N);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_SMV_PHASE_X),CP_SMV_PHASE_X);
	GetEditOptions(NULL)->m_bConstraintEdit = TRUE;
	GetEditOptions(NULL)->m_bAllowEdit = FALSE;
	GetEditOptions(NULL)->AddComboButton();
}

CString CRecordItemSmvPhase::GetCaption(CXTPReportColumn* pColumn)
{
	CXTPReportRecordItemConstraint* pConstraint = GetEditOptions(NULL)->FindConstraint(m_eSmvPhase);
	ASSERT(pConstraint);
	return pConstraint->m_strConstraint;
}

void CRecordItemSmvPhase::OnConstraintChanged(XTP_REPORTRECORDITEM_ARGS* /*pItemArgs*/, CXTPReportRecordItemConstraint* pConstraint)
{	
	CHANNEL_PHASE_SMV ePrevPhase = m_eSmvPhase;
	m_eSmvPhase = (CHANNEL_PHASE_SMV)pConstraint->m_dwData;

	CSmvChannelRecord *pChannelRecord = (CSmvChannelRecord *)GetRecord();
	// 通道类型为时间，则无相别之分
	if ( ((CRecordItemSmvChannelType *)(pChannelRecord->GetItem(COLUMN_SMV_CHANNEL_TYPE)))->GetValue() == CT_SMV_TIME )
	{
		m_eSmvPhase = CP_SMV_PHASE_UNKNOW;
		return;
	}

	// 获取控制块信息
	SCDSMV92ANALYSIS_IEDSMV_INFO ctrlInfo;
	((CSmvCtrlRecord *)(pChannelRecord->GetParentRecord()))->GetRecordInfo((void *)&ctrlInfo);
	
	// 获取当前通道信息
	SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO channelInfo;
	pChannelRecord->GetRecordInfo(&channelInfo);
	channelInfo.dataSetName = ctrlInfo.dataSetName;
	channelInfo.dataSetDesc = ctrlInfo.dataSetDesc;
	channelInfo.ldInst      = ctrlInfo.iedLdInst;

	// 更新通道描述信息
	UpdateChannelRecordDesc(ePrevPhase,m_eSmvPhase,channelInfo);
	((CXTPReportRecordItemText *)pChannelRecord->GetItem(COLUMN_SMV_LNDESC))->SetValue(channelInfo.lnDesc.c_str());
	((CXTPReportRecordItemText *)pChannelRecord->GetItem(COLUMN_SMV_DODESC))->SetValue(channelInfo.doDesc.c_str());
	((CXTPReportRecordItemText *)pChannelRecord->GetItem(COLUMN_SMV_DUDESC))->SetValue(channelInfo.daVal.c_str());

	CHANNEL_TYPE_SMV eChannelType = ((CRecordItemSmvChannelType *)(GetRecord()->GetItem(COLUMN_SMV_CHANNEL_TYPE)))->GetValue();

	// 更新缓存信息
	// 由于该通道所属逻辑节点或数据在其他地方用到，直接更改需同步修改，比较繁琐
	// 因此直接替换为新的自定义通道（即添加自定义数据）
	CSCLManager::getInstance()->SetOneIEDSMVChannelInfo_Def(ctrlInfo.iedName,ctrlInfo.iedLdInst,channelInfo,eChannelType);
}

void CRecordItemSmvPhase::UpdateChannelRecordDesc(const CHANNEL_PHASE_SMV ePrevPhase, const CHANNEL_PHASE_SMV eCurPhase,
												  SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &channelInfo)
{
	//相别信息判断模板
	static std::wstring PHASE_NONE_TEMPLATE[]={L""};
	static std::wstring PHASE_A_TEMPLATE[]={L"A相", L"a相", L"Ａ相"};
	static std::wstring PHASE_B_TEMPLATE[]={L"B相", L"b相", L"Ｂ相"};
	static std::wstring PHASE_C_TEMPLATE[]={L"C相", L"c相", L"Ｃ相"};
	static std::wstring PHASE_X_TEMPLATE[]={L"X相", L"x相", L"Ｘ相", L"同期", L"母线", L"抽取"};
	static std::wstring PHASE_N_TEMPLATE[]={L"N相", L"n相", L"Ｎ相", L"中性点", L"零序", L"间隙"};
	static std::wstring *pTmplt[] = {PHASE_NONE_TEMPLATE,PHASE_A_TEMPLATE,PHASE_B_TEMPLATE,PHASE_C_TEMPLATE,PHASE_N_TEMPLATE,PHASE_X_TEMPLATE};
	static int tmpltSize[] = {0,3,3,3,6,6};

	if ( ePrevPhase == eCurPhase )
		return;

	bool bLnReplace = false;
	bool bDoReplace = false;
	bool bDuReplace = false;
	for ( int i = 0; i < tmpltSize[ePrevPhase]; i++ )
	{
		// 更新ln描述信息
		int pos = channelInfo.lnDesc.find(pTmplt[ePrevPhase][i]);
		if ( pos != std::wstring::npos )
		{
			channelInfo.lnDesc.replace(pos,pTmplt[ePrevPhase][i].length(),pTmplt[eCurPhase][0]);
			bLnReplace = true;
		}
		// 更新do描述信息
		pos = channelInfo.doDesc.find(pTmplt[ePrevPhase][i]);
		if ( pos != std::wstring::npos )
		{
			channelInfo.doDesc.replace(pos,pTmplt[ePrevPhase][i].length(),pTmplt[eCurPhase][0]);
			bDoReplace = true;
		}
		// 更新du描述信息
		pos = channelInfo.daVal.find(pTmplt[ePrevPhase][i]);
		if ( pos != std::wstring::npos )
		{
			channelInfo.daVal.replace(pos,pTmplt[ePrevPhase][i].length(),pTmplt[eCurPhase][0]);
			bDuReplace = true;
		}
	}
	// 描述信息中不含有相别信息，则填充
	if ( !bLnReplace )
	{
		channelInfo.lnDesc.insert(0,pTmplt[eCurPhase][0]);
	}
	if ( !bDoReplace )
	{
		channelInfo.doDesc.insert(0,pTmplt[eCurPhase][0]);
	}
	if ( !bDuReplace )
	{
		channelInfo.daVal.insert(0,pTmplt[eCurPhase][0]);
	}
}

//
//=============================================================================
// SMV通道类型记录项
//=============================================================================
//
CRecordItemSmvChannelType::CRecordItemSmvChannelType(CHANNEL_TYPE_SMV channelType /* = CT_SMV_UNKNOW */)
{
	m_eChannelType = channelType;
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_SMV_CHANNEL_UNKNOW),CT_SMV_UNKNOW);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_SMV_CHANNEL_TIME),CT_SMV_TIME);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_SMV_CHANNEL_CURRENT),CT_SMV_CURRENT);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_SMV_CHANNEL_VOLTAGE),CT_SMV_VOLTAGE);
	GetEditOptions(NULL)->m_bConstraintEdit = TRUE;
	GetEditOptions(NULL)->m_bAllowEdit = FALSE;
	GetEditOptions(NULL)->AddComboButton();
}

CString CRecordItemSmvChannelType::GetCaption(CXTPReportColumn* pColumn)
{
	CXTPReportRecordItemConstraint* pConstraint = GetEditOptions(NULL)->FindConstraint(m_eChannelType);
	ASSERT(pConstraint);
	return pConstraint->m_strConstraint;
}

void CRecordItemSmvChannelType::OnConstraintChanged(XTP_REPORTRECORDITEM_ARGS* /*pItemArgs*/, CXTPReportRecordItemConstraint* pConstraint)
{
	CHANNEL_TYPE_SMV preChannelType = m_eChannelType;
	m_eChannelType = (CHANNEL_TYPE_SMV)pConstraint->m_dwData;

	CSmvChannelRecord *pChannelRecord = (CSmvChannelRecord *)GetRecord();
	// 获取控制块信息
	SCDSMV92ANALYSIS_IEDSMV_INFO ctrlInfo;
	((CSmvCtrlRecord *)(pChannelRecord->GetParentRecord()))->GetRecordInfo((void *)&ctrlInfo);

	// 获取当前通道信息
	SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO channelInfo;
	pChannelRecord->GetRecordInfo(&channelInfo);
	channelInfo.dataSetName = ctrlInfo.dataSetName;
	channelInfo.dataSetDesc = ctrlInfo.dataSetDesc;
	channelInfo.ldInst      = ctrlInfo.iedLdInst;

	// 更新通道描述信息
	UpdateChannelRecordDesc(preChannelType,m_eChannelType,channelInfo);
	((CXTPReportRecordItemText *)pChannelRecord->GetItem(COLUMN_SMV_LNDESC))->SetValue(channelInfo.lnDesc.c_str());
	((CXTPReportRecordItemText *)pChannelRecord->GetItem(COLUMN_SMV_DODESC))->SetValue(channelInfo.doDesc.c_str());
	((CXTPReportRecordItemText *)pChannelRecord->GetItem(COLUMN_SMV_DUDESC))->SetValue(channelInfo.daVal.c_str());

	// 更新通道相别
	CHANNEL_PHASE_SMV phase = CSCLManager::getInstance()->GetChannelPhase(channelInfo,m_eChannelType);
	((CRecordItemSmvPhase *)pChannelRecord->GetItem(COLUMN_SMV_PHASE))->SetValue(phase);

	// 更新缓存信息
	// 由于该通道所属逻辑节点或数据在其他地方用到，直接更改需同步修改，比较繁琐
	// 因此直接替换为新的自定义通道（即添加自定义数据）
	CSCLManager::getInstance()->SetOneIEDSMVChannelInfo_Def(ctrlInfo.iedName,ctrlInfo.iedLdInst,channelInfo,m_eChannelType);
}

void CRecordItemSmvChannelType::UpdateChannelRecordDesc(CHANNEL_TYPE_SMV prevChannleType, CHANNEL_TYPE_SMV curChannleType,
														SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &channelInfo)
{
	// 时间/电压/电流信息判断模板
	static std::wstring TIME_TEMPLATE[]={L"延时", L"时间", L"DelayTRtg"};
	static std::wstring CURRENT_TEMPLATE[]={L"电流", L"current"};
	static std::wstring VOLTAGE_TEMPLATE[]={L"电压", L"voltage"};
	static std::wstring *pTmplt[] = {NULL,TIME_TEMPLATE,CURRENT_TEMPLATE,VOLTAGE_TEMPLATE};
	static int tmpltSize[] = {0,3,2,2};

	if ( curChannleType == CT_SMV_UNKNOW )
		return;

	for ( int i = 0; i < tmpltSize[prevChannleType]; i++ )
	{
		// 更新ln描述信息
		int pos = channelInfo.lnDesc.find(pTmplt[prevChannleType][i]);
		if ( pos != std::wstring::npos )
		{
			channelInfo.lnDesc.replace(pos,pTmplt[prevChannleType][i].length(),pTmplt[curChannleType][0]);
		}
		// 更新do描述信息
		pos = channelInfo.doDesc.find(pTmplt[prevChannleType][i]);
		if ( pos != std::wstring::npos )
		{
			channelInfo.doDesc.replace(pos,pTmplt[prevChannleType][i].length(),pTmplt[curChannleType][0]);
		}
		// 更新du描述信息
		pos = channelInfo.daVal.find(pTmplt[prevChannleType][i]);
		if ( pos != std::wstring::npos )
		{
			channelInfo.daVal.replace(pos,pTmplt[prevChannleType][i].length(),pTmplt[curChannleType][0]);
		}
	}
}

//
//=============================================================================
// Goose通道类型记录项
//=============================================================================
//
CRecordItemGooseChannelType::CRecordItemGooseChannelType(CHANNEL_TYPE_GOOSE eChannelType /* = CT_GOOSE_UNKNOW */, RECORD_TYPE eRecordType /* = RT_GOOSE_CHANNEL */)
{
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_UNKNOW),CT_GOOSE_UNKNOW);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_STRUCT),CT_GOOSE_STRUCT);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_POINT),CT_GOOSE_POINT);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_DPOINT),CT_GOOSE_DOUBLE_POINT);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_QUALITY),CT_GOOSE_QUALITY);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_TIME_STAMP),CT_GOOSE_TIME_STAMP);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_ENTRY_TIME),CT_GOOSE_ENTRY_TIME);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_INT8),CT_GOOSE_INT8);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_INT16),CT_GOOSE_INT16);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_INT24),CT_GOOSE_INT24);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_INT32),CT_GOOSE_INT32);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_INT128),CT_GOOSE_INT128);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_UINT8),CT_GOOSE_UINT8);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_UINT16),CT_GOOSE_UINT16);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_UINT24),CT_GOOSE_UINT24);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_UINT32),CT_GOOSE_UINT32);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_FLOAT32),CT_GOOSE_FLOAT32);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_FLOAT64),CT_GOOSE_FLOAT64);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_OCTET),CT_GOOSE_OCTET);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_VISSTR),CT_GOOSE_VISSTRING);
	GetEditOptions(NULL)->AddConstraint(CStrManager::Get(RCSTR_GOOSE_CHANNEL_UNICODE),CT_GOOSE_UNICODE);

	GetEditOptions(NULL)->m_bConstraintEdit = TRUE;
	GetEditOptions(NULL)->m_bAllowEdit = FALSE;
	GetEditOptions(NULL)->AddComboButton();

	m_eChannelType = eChannelType;
	m_eRecordType  = eRecordType;
	m_bPopulate    = FALSE;
}

CString CRecordItemGooseChannelType::GetCaption(CXTPReportColumn* pColumn)
{
	CXTPReportRecordItemConstraint* pConstraint = GetEditOptions(NULL)->FindConstraint(m_eChannelType);
	ASSERT(pConstraint);
	return pConstraint->m_strConstraint;
}

void CRecordItemGooseChannelType::OnConstraintChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, CXTPReportRecordItemConstraint* pConstraint)
{
	CHANNEL_TYPE_GOOSE eChannelType = (CHANNEL_TYPE_GOOSE)pConstraint->m_dwData;

	switch ( m_eRecordType )
	{
	case RT_GOOSE_CHANNEL:
		UpdateGooseChannelRecord(eChannelType);
		break;
	case RT_GOOSE_SUB_CHANNEL:
		UpdateGooseSubChannelRecord(eChannelType);
		break;
	default:
		break;
	}

	if ( m_bPopulate )
	{
		pItemArgs->pControl->Populate();
	}
}

void CRecordItemGooseChannelType::UpdateGooseChannelRecord(CHANNEL_TYPE_GOOSE eChannelType)
{
	CGooseChannelRecord *pChannelRecord = (CGooseChannelRecord *)GetRecord();
	// 获取控制块信息
	SCDGOOSEANALYSIS_IEDGOOSE_INFO ctrlInfo;
	((CGooseCtrlRecord *)(pChannelRecord->GetParentRecord()))->GetRecordInfo((void *)&ctrlInfo);

	// 获取当前通道信息
	SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO channelInfo;
	pChannelRecord->GetRecordInfo(&channelInfo);
	channelInfo.dataSetName = ctrlInfo.dataSetName;
	channelInfo.dataSetDesc = ctrlInfo.dataSetDesc;
	channelInfo.ldInst      = ctrlInfo.iedLdInst;

	// 更新缓存的通道信息
	CSCLManager::getInstance()->SetOneIEDGOOSEChannelInfo_Def(ctrlInfo.iedName,ctrlInfo.iedLdInst,channelInfo,eChannelType);

	// 更新报表的通道信息
	if ( eChannelType == CT_GOOSE_STRUCT )
	{
		int subChannelNum = CSCLManager::getInstance()->GetIEDGOOSESubChannelNum(
			ctrlInfo.iedName,ctrlInfo.iedLdInst,channelInfo);
		if ( subChannelNum > 0 )
		{
			((CRecordItemNumber *)pChannelRecord->GetItem(COLUMN_GOOSE_SUB_CHANNEL_NUM))->SetValue((double)subChannelNum);
			AddGooseSubChannelRecord(0,ctrlInfo,channelInfo,subChannelNum,pChannelRecord);
		}
		m_bPopulate = TRUE;
	}
	else
	{
		CXTPReportRecords *pRecords = pChannelRecord->GetChilds();
		int count  = pRecords->GetCount();
		if ( count > 0 )
		{
			// 原通道类型为结构体，则移除多余部分
			for ( int i = count-1 ; i >= 0; i-- )
				pRecords->RemoveAt(i);
			m_bPopulate = TRUE;
		}
		else
			m_bPopulate = FALSE;
		((CRecordItemNumber *)pChannelRecord->GetItem(COLUMN_GOOSE_SUB_CHANNEL_NUM))->SetValue(0);
	}

	m_eChannelType = eChannelType;
}
void CRecordItemGooseChannelType::UpdateGooseSubChannelRecord(CHANNEL_TYPE_GOOSE eChannelType)
{
	// 获取当前的子通道层级
	CGooseSubChannelRecord *pCurChannelRecord = (CGooseSubChannelRecord *)GetRecord();
	int subChannelLevel = pCurChannelRecord->GetChannelLevel();
	ASSERT( subChannelLevel >= 0 );

	// 从第0层级之后的子通道不允许设置Struct类型通道
	if ( eChannelType == CT_GOOSE_STRUCT && subChannelLevel > 0 )
	{
		return;
	}
	
	// 获取从控制块控制的通道下的第1层子通道开始至当前通道的顺序号信息
	int *subChannelIdxList = new int[subChannelLevel+1];
	subChannelIdxList[subChannelLevel] = (int)(((CRecordItemNumber *)(pCurChannelRecord->GetItem(COLUMN_GOOSE_SUB_CHANNEL_IDX)))->GetValue())-1;
	CGooseSubChannelRecord *pUpChannelRecord = pCurChannelRecord;
	for ( int i = subChannelLevel-1; i >= 0; i-- )
	{
		pUpChannelRecord = (CGooseSubChannelRecord *)(pUpChannelRecord->GetParentRecord());
		subChannelIdxList[i] = (int)(((CRecordItemNumber *)(pUpChannelRecord->GetItem(COLUMN_GOOSE_SUB_CHANNEL_IDX)))->GetValue())-1;
	}

	CGooseChannelRecord *pChannelRecord = (CGooseChannelRecord *)(pUpChannelRecord->GetParentRecord());
	
	// 获取控制块信息
	SCDGOOSEANALYSIS_IEDGOOSE_INFO ctrlInfo;
	((CGooseCtrlRecord *)(pChannelRecord->GetParentRecord()))->GetRecordInfo((void *)&ctrlInfo);

	// 获取控制块控制数据集中的通道信息（顶层通道）
	SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO channelInfo;
	int channelIdx = (int)(((CXTPReportRecordItemNumber *)pChannelRecord->GetItem(COLUMN_GOOSE_CHANNEL_IDX))->GetValue())-1;
	CSCLManager::getInstance()->GetOneIEDGOOSEDataSetInfo(ctrlInfo.iedName,ctrlInfo.iedLdInst,ctrlInfo.cbName,
		channelIdx,&channelInfo);

	// 更新子通道信息
	CSCLManager::getInstance()->SetOneIEDGOOSESubChannelInfo(ctrlInfo.iedName,ctrlInfo.iedLdInst,channelInfo,
		subChannelIdxList,subChannelLevel+1,eChannelType);

	if ( eChannelType == CT_GOOSE_STRUCT )
	{
		// 获取当前子通道信息
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO parentChannelInfo = channelInfo;
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO subChannelInfo;		
		for ( int i = 0; i < subChannelLevel+1; i++ )
		{
			if ( CSCLManager::getInstance()->GetOneIEDGOOSESubChannelInfo(ctrlInfo.iedName,ctrlInfo.iedLdInst,parentChannelInfo,subChannelIdxList[i],subChannelInfo) == -1 )
			{
				TRACE(_T("CRecordItemNumber::UpdateGooseChannelRecord:GetOneIEDGOOSESubChannelInfo Error\r\n"));
				return;
			}
			parentChannelInfo = subChannelInfo;
		}

		int subSubChannelNum = CSCLManager::getInstance()->GetIEDGOOSESubChannelNum(
			ctrlInfo.iedName,ctrlInfo.iedLdInst,subChannelInfo);
		
		((CRecordItemNumber *)(pCurChannelRecord->GetItem(COLUMN_GOOSE_SUB_SUB_CHANNEL_NUM)))->SetValue((double)subSubChannelNum);
		// 更新报表的通道信息
		if ( subSubChannelNum > 0 )
		{
			AddGooseSubChannelRecord(subChannelLevel+1,ctrlInfo,subChannelInfo,subSubChannelNum,GetRecord());
		}
		m_bPopulate = TRUE;
	}
	else
	{
		CXTPReportRecords *pRecords = pCurChannelRecord->GetChilds();
		int count  = pRecords->GetCount();
		if ( count > 0 )
		{
			// 原通道类型为结构体，则移除多余部分
			for ( int i = count-1 ; i >= 0; i-- )
				pRecords->RemoveAt(i);
			m_bPopulate = TRUE;
		}
		else
			m_bPopulate = FALSE;
		((CRecordItemNumber *)pCurChannelRecord->GetItem(COLUMN_GOOSE_SUB_SUB_CHANNEL_NUM))->SetValue(0);
	}

	m_eChannelType = eChannelType;
}
