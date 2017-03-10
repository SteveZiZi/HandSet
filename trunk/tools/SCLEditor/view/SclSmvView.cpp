/// @file
///     SclSmvView.cpp
///
/// @brief
///     SMV��Ϣ������ͼ
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
///    |2013.07.04     |1.0.1.704  |��ӱ�����仯ʱ��״̬����ʾ��ǰ������Ϣ                |
///    |2013.08.07     |1.0.1.807  |������������󣬲���IEDĿ������BUG                      |
///    |2013.08.16     |1.0.2.816  |�޸ı�����Smvͨ������Ӳ�������֧��Smvͨ����Ϊ0�����     |
///    |2013.11.04     |1.0.2.1104 |SMV���ƿ���Ϣ����Ӱ汾����Ϣ                             |
///

#include "stdafx.h"
#include "../MainFrm.h"
#include "../SCLEditor.h"
#include "../SCLEditorDoc.h"
#include "SclSmvView.h"

// Record
#include "ReportRecord.h"
#include "../scl/SclManager.h"
// CStrManager
#include "../ResourceStr.h"
#include "../base/StrManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSclSmvView

IMPLEMENT_DYNCREATE(CSclSmvView, CXTPReportView)

BEGIN_MESSAGE_MAP(CSclSmvView, CXTPReportView)
	ON_WM_CREATE()
	ON_MESSAGE(WM_UPDATE_SMV_REPORT,&CSclSmvView::OnUpdateReport)//�����Զ�����Ϣ
	ON_NOTIFY(XTP_NM_REPORT_HEADER_RCLICK, XTP_ID_REPORT_CONTROL, OnReportColumnRClick)
	ON_NOTIFY(XTP_NM_REPORT_FOCUS_CHANGING, XTP_ID_REPORT_CONTROL, OnReportFocusChanging)
END_MESSAGE_MAP()

// CSclSmvView construction/destruction

CSclSmvView::CSclSmvView()
{
	// TODO: add construction code here

}

CSclSmvView::~CSclSmvView()
{
}

BOOL CSclSmvView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CSclSmvView drawing

void CSclSmvView::OnDraw(CDC* /*pDC*/)
{
	CSCLEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CSclSmvView diagnostics

#ifdef _DEBUG
void CSclSmvView::AssertValid() const
{
	CView::AssertValid();
}

void CSclSmvView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSCLEditorDoc* CSclSmvView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSCLEditorDoc)));
	return (CSCLEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CSclSmvView message handlers

///
/// @brief
///     ��ʼ������
///
int CSclSmvView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPReportView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	CXTPReportControl& wndReport = GetReportCtrl();
	wndReport.ModifyStyle(0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_TABSTOP);
	wndReport.GetReportHeader()->AllowColumnRemove(FALSE);
	wndReport.GetReportHeader()->SetAutoColumnSizing(FALSE);

	// ��ӱ������,��ʼ������
	CXTPReportColumn * pColumn = wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_CTRL_IDX,
		CStrManager::Get(RCSTR_SMV_CTRL_IDX), 60));
	pColumn->SetTreeColumn(TRUE);
	pColumn->SetEditable(FALSE);
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_APPID, CStrManager::Get(RCSTR_SMV_APPID), 40));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_MAC_ADDR, CStrManager::Get(RCSTR_SMV_MAC), 110));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_CHANNEL_NUM, CStrManager::Get(RCSTR_SMV_CHANNEL_NUM), 55));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_IED, CStrManager::Get(RCSTR_SMV_IED), 200));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_DATASETREF, CStrManager::Get(RCSTR_SMV_DATASETREF), 200));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_SVID, CStrManager::Get(RCSTR_SMV_SVID), 150));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_SMPRATE, CStrManager::Get(RCSTR_SMV_SMPRATE), 52));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_NOFASDU, CStrManager::Get(RCSTR_SMV_NOFASDU), 53));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_VLANID, CStrManager::Get(RCSTR_SMV_VLANID), 55));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_VLAN_PRI, CStrManager::Get(RCSTR_SMV_VLANPRI), 75));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_IS_MULTICAST, CStrManager::Get(RCSTR_SMV_MULTICAST), 30, TRUE, XTP_REPORT_NOICON, TRUE, FALSE));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_REFRTM_OPT, CStrManager::Get(RCSTR_SMV_REFRTM_OPT), 80, TRUE, XTP_REPORT_NOICON, TRUE, FALSE));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_SMPRATE_OPT, CStrManager::Get(RCSTR_SMV_SMPRATE_OPT), 70, TRUE, XTP_REPORT_NOICON, TRUE, FALSE));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_DATASET_OPT, CStrManager::Get(RCSTR_SMV_DATAREF_OPT), 90, TRUE, XTP_REPORT_NOICON, TRUE, FALSE));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_SMPSYNC_OPT, CStrManager::Get(RCSTR_SMV_SMPSYNC_OPT), 105, TRUE, XTP_REPORT_NOICON, TRUE, FALSE));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_SECURITY_OPT, CStrManager::Get(RCSTR_SMV_SECURITY_OPT), 90, TRUE, XTP_REPORT_NOICON, TRUE, FALSE));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_DATASET_DESC, CStrManager::Get(RCSTR_SMV_DATASET_DESC), 150, TRUE, XTP_REPORT_NOICON, TRUE, FALSE));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_SVCBREF, CStrManager::Get(RCSTR_SMV_SVCBREF), 200, TRUE, XTP_REPORT_NOICON, TRUE, FALSE));
	wndReport.AddColumn(new CXTPReportColumn(COLUMN_SMV_CONFREV, CStrManager::Get(RCSTR_SMV_CONFREV), 50, TRUE, XTP_REPORT_NOICON, TRUE, FALSE));

	// ��Ӽ�¼��Ϣ
	AddSampleRecords(wndReport);

	// ��������ʾ����������
	wndReport.Populate();

	// ������ʾ�ı����
	wndReport.GetToolTipContext()->SetStyle(xtpToolTipOffice2007);
	// ���ñ���ı�ͷ�ķ��
	wndReport.GetPaintManager()->SetColumnStyle(xtpReportColumnOffice2007);
	// ���ñ���ı�ͷ����ɫ
	wndReport.GetPaintManager()->SetColumnOffice2007CustomThemeBaseColor(RGB(176,196,222));

// 	// ������ֱ������
// 	m_wndVScrollBar.Create(WS_CHILD | WS_VISIBLE | SBS_VERT, CRect(0, 0, 0, 0), this, 100);
// 	m_wndVScrollBar.SetScrollBarStyle(xtpScrollStyleOffice2007Dark);
// 	SetScrollBarCtrl(&m_wndVScrollBar);

	// ����ˮƽ������
// 	m_wndHScrollBar.Create(WS_CHILD | WS_VISIBLE | SBS_HORZ, CRect(0, 0, 0, 0), this, 100);
// 	m_wndHScrollBar.SetScrollBarStyle(xtpScrollStyleOffice2007Dark);
//	SetScrollBarCtrl(&m_wndHScrollBar,TRUE);

	// �����¼��Ŀ�۽�
	wndReport.FocusSubItems(TRUE);

	wndReport.GetPaintManager()->SetColumnWidthWYSIWYG(m_bWYSIWYG);

	// ���÷ָ��߷��
	wndReport.GetPaintManager()->SetHeaderRowsDividerStyle(xtpReportFixedRowsDividerBold);
	wndReport.GetPaintManager()->SetFooterRowsDividerStyle(xtpReportFixedRowsDividerBold);
	wndReport.GetPaintManager()->m_clrHeaderRowsDivider = RGB(255,0,0);
	wndReport.GetPaintManager()->m_clrFooterRowsDivider = RGB(0,0,255);

	wndReport.AllowEdit(TRUE);
	wndReport.EditOnClick(FALSE);
	wndReport.SetMultipleSelection(FALSE);

	// �����Ϸ�ģʽ��Vista Windows Explorer or XP Windows Explorer
	wndReport.m_bSortedDragDrop = TRUE;

	wndReport.RedrawControl();

	return 0;
}

///
/// @brief
///     ��Ӽ�¼��Ϣ��SMV���ƿ���Ϣ+��Ӧ��ͨ����Ϣ��
///
void CSclSmvView::AddSampleRecords(CXTPReportControl &wndReport)
{
	// ��ȡSMV���ƿ���Ŀ
	int ctrlNum = CSCLManager::getInstance()->GetAllIEDSMVNumByCtrlRef();
	if ( ctrlNum <= 0 )
		return;

	// ��ȡSMV���ƿ���Ϣ
	SCDSMV92ANALYSIS_IEDSMV_INFO *ctrlInfoArr = new SCDSMV92ANALYSIS_IEDSMV_INFO[ctrlNum];
	ctrlNum = CSCLManager::getInstance()->GetAllIEDSMVInfoByCtrlRef(ctrlInfoArr,ctrlNum);
	if ( ctrlNum == -1 )
	{
		delete[] ctrlInfoArr;
		return;
	}

	for ( int i = 0; i < ctrlNum; i++ )
	{
		// ��ȡ���ƿ������ͨ����Ϣ
 		int channelNum = CSCLManager::getInstance()->GetIEDSMVChannelNum(ctrlInfoArr[i].iedName,ctrlInfoArr[i].iedLdInst,ctrlInfoArr[i].smvCbName);
		if ( channelNum <= 0 )
		{
			ctrlInfoArr[i].channelNum = channelNum = 0;
			CSmvCtrlRecord *pCtrlRec = new CSmvCtrlRecord(i+1,ctrlInfoArr[i]);
			wndReport.AddRecord(pCtrlRec);
			continue;
		}

		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *channelInfoArr = new SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO[channelNum];
		int channelActNum = CSCLManager::getInstance()->GetAllIEDSMVChannelInfo(ctrlInfoArr[i].iedName,
			ctrlInfoArr[i].iedLdInst,ctrlInfoArr[i].smvCbName,channelInfoArr,channelNum);
		
		if ( channelActNum < 0 )
			channelActNum = 0;
		// �Ƴ���������ͨ����Ϣ
		if ( channelActNum < channelNum )
		{
			for ( int j = channelNum-1; j >= channelActNum; j-- )
			{
				CSCLManager::getInstance()->RemoveOneIEDSMVChannelInfo(ctrlInfoArr[i].iedName,
					ctrlInfoArr[i].iedLdInst, ctrlInfoArr[i].dataSetName,j);
			}
		}
		ctrlInfoArr[i].channelNum = channelActNum;

		// ��ӱ����¼(���ƿ�+��Ӧ��ͨ��)
		CSmvCtrlRecord *pParentRec = new CSmvCtrlRecord(i+1,ctrlInfoArr[i]);
		wndReport.AddRecord(pParentRec);

		if ( channelActNum > 0 )
		{
			CXTPReportRecord * pTitleItem = pParentRec->GetChilds()->Add(new CSmvChannelTitleRecord());
			pTitleItem->SetEditable(FALSE);
			for ( int j = 0; j < channelActNum; j++ )
			{
				CHANNEL_TYPE_SMV  channelType = CSCLManager::getInstance()->GetChannelType(channelInfoArr[j]);
				CHANNEL_PHASE_SMV phase = CSCLManager::getInstance()->GetChannelPhase(channelInfoArr[j],channelType);
				pParentRec->GetChilds()->Add(new CSmvChannelRecord(j+1,channelType,phase,ctrlInfoArr[i].iedName,channelInfoArr[j]));
			}
		}

		delete[] channelInfoArr;
	}

	delete[] ctrlInfoArr;
}

///
/// @brief
///     ���±����¼��Ϣ����
///
LRESULT CSclSmvView::OnUpdateReport(WPARAM wParam, LPARAM lParam)
{
	UpdateReport();
	return 0;
}

///
/// @brief
///     ���±����¼
///
void CSclSmvView::UpdateReport()
{
	CXTPReportControl& wndReport = GetReportCtrl();
	wndReport.ResetContent();
	AddSampleRecords(wndReport);
	wndReport.Populate();
}

///
/// @brief
///     �һ�����������Ӧ
///
void CSclSmvView::OnReportColumnRClick(NMHDR * pNotifyStruct, LRESULT * /*result*/)
{
	XTP_NM_REPORTRECORDITEM* pItemNotify = (XTP_NM_REPORTRECORDITEM*) pNotifyStruct;
	ASSERT(pItemNotify->pColumn);
	CPoint ptClick = pItemNotify->pt;

	CMenu menu;
	VERIFY(menu.CreatePopupMenu());

	// ����Menu
	menu.AppendMenu(MF_SEPARATOR, (UINT)-1, (LPCTSTR)NULL);
	menu.AppendMenu(MF_STRING, ID_COLUMN_BESTFIT, CStrManager::Get(RCSTR_BEST_FIT));

	CXTPReportColumns* pColumns = GetReportCtrl().GetColumns();
	CXTPReportColumn* pColumn   = pItemNotify->pColumn;
	int nColumnCount = pColumns->GetCount();
	int nColumn;

	// ����������ʾ/���ء��Ӳ˵�
	CMenu menuColumns;
	VERIFY(menuColumns.CreatePopupMenu());
	for (nColumn = 0; nColumn < nColumnCount; nColumn++)
	{
		CXTPReportColumn* pCol = pColumns->GetAt(nColumn);
		CString sCaption = pCol->GetCaption();
		//if (!sCaption.IsEmpty())
		menuColumns.AppendMenu(MF_STRING, ID_COLUMN_SHOW_OT_HIDE + nColumn, sCaption);
		menuColumns.CheckMenuItem(ID_COLUMN_SHOW_OT_HIDE + nColumn,
			MF_BYCOMMAND | (pCol->IsVisible() ? MF_CHECKED : MF_UNCHECKED) );
	}

	menu.InsertMenu(0, MF_BYPOSITION | MF_POPUP, (UINT_PTR) menuColumns.m_hMenu,
		CStrManager::Get(RCSTR_COLUMN_SHOW_OR_HIDE));

	// �������뷽ʽ�Ӳ˵�
	CMenu menuAlign;
	VERIFY(menuAlign.CreatePopupMenu());

	// �ı�����
	CMenu menuAlignText;
	VERIFY(menuAlignText.CreatePopupMenu());
	menuAlignText.AppendMenu(MF_STRING, ID_COLUMN_TEXT_ALIGMENT_LEFT,
		CStrManager::Get(RCSTR_ALIGN_LEFT));
	menuAlignText.AppendMenu(MF_STRING, ID_COLUMN_TEXT_ALIGMENT_RIGHT,
		CStrManager::Get(RCSTR_ALIGN_RIGHT));
	menuAlignText.AppendMenu(MF_STRING, ID_COLUMN_TEXT_ALIGMENT_CENTER,
		CStrManager::Get(RCSTR_ALIGN_CENTRE));
	int nAlignOption = 0;
	switch (pColumn->GetAlignment() & xtpColumnTextMask)
	{
	case xtpColumnTextLeft:
		nAlignOption = ID_COLUMN_TEXT_ALIGMENT_LEFT;
		break;
	case xtpColumnTextRight:
		nAlignOption = ID_COLUMN_TEXT_ALIGMENT_RIGHT;
		break;
	case xtpColumnTextCenter:
		nAlignOption = ID_COLUMN_TEXT_ALIGMENT_CENTER;
		break;
	}
	menuAlignText.CheckMenuItem(nAlignOption, MF_BYCOMMAND | MF_CHECKED);
	menuAlign.InsertMenu(1, MF_BYPOSITION | MF_POPUP, (UINT_PTR) menuAlignText.m_hMenu,
		CStrManager::Get(RCSTR_TEXT_ALIGN));

	// ͼ�����
	CMenu menuAlignIcon;
	VERIFY(menuAlignIcon.CreatePopupMenu());
	menuAlignIcon.AppendMenu(MF_STRING, ID_COLUMN_ICON_ALIGMENT_LEFT,
		CStrManager::Get(RCSTR_ALIGN_LEFT));
	menuAlignIcon.AppendMenu(MF_STRING, ID_COLUMN_ICON_ALIGMENT_RIGHT,
		CStrManager::Get(RCSTR_ALIGN_RIGHT));
	menuAlignIcon.AppendMenu(MF_STRING, ID_COLUMN_ICON_ALIGMENT_CENTER,
		CStrManager::Get(RCSTR_ALIGN_CENTRE));
	nAlignOption = 0;
	switch (pColumn->GetAlignment() & xtpColumnIconMask)
	{
	case xtpColumnIconLeft:
		nAlignOption = ID_COLUMN_ICON_ALIGMENT_LEFT;
		break;
	case xtpColumnIconRight:
		nAlignOption = ID_COLUMN_ICON_ALIGMENT_RIGHT;
		break;
	case xtpColumnIconCenter:
		nAlignOption = ID_COLUMN_ICON_ALIGMENT_CENTER;
		break;
	}
	menuAlignIcon.CheckMenuItem(nAlignOption, MF_BYCOMMAND | MF_CHECKED);
	menuAlign.InsertMenu(2, MF_BYPOSITION | MF_POPUP, (UINT_PTR) menuAlignIcon.m_hMenu,
		CStrManager::Get(RCSTR_ICON_ALIGN));

	menu.InsertMenu(1, MF_BYPOSITION | MF_POPUP, (UINT_PTR) menuAlign.m_hMenu,
		CStrManager::Get(RCSTR_ALIGN_METHOD));

	// ��ʾMenu
	int nMenuResult = CXTPCommandBars::TrackPopupMenu(&menu, TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTALIGN |TPM_RIGHTBUTTON, ptClick.x, ptClick.y, this, NULL);

	// ��������ʾ/���ء��Ӳ˵�
	if (nMenuResult >= ID_COLUMN_SHOW_OT_HIDE)
	{
		CXTPReportColumn* pCol = pColumns->GetAt(nMenuResult - ID_COLUMN_SHOW_OT_HIDE);
		if (pCol)
		{
			pCol->SetVisible(!pCol->IsVisible());
		}
	}

	// �����ı�����
	if (nMenuResult > ID_COLUMN_ALIGMENT_TEXT && nMenuResult < ID_COLUMN_ALIGMENT_ICON)
	{
		int nAlign = pColumn->GetAlignment();
		nAlign &= ~(xtpColumnTextLeft | xtpColumnTextCenter | xtpColumnTextRight);

		switch (nMenuResult)
		{
		case ID_COLUMN_TEXT_ALIGMENT_LEFT :
			pColumn->SetAlignment(nAlign | xtpColumnTextLeft);
			break;
		case ID_COLUMN_TEXT_ALIGMENT_RIGHT :
			pColumn->SetAlignment(nAlign | xtpColumnTextRight);
			break;
		case ID_COLUMN_TEXT_ALIGMENT_CENTER  :
			pColumn->SetAlignment(nAlign | xtpColumnTextCenter);
			break;
		}
	}

	// ����ͼ�����
	if (nMenuResult > ID_COLUMN_ALIGMENT_ICON && nMenuResult < ID_COLUMN_SHOW_OT_HIDE)
	{
		int nAlign = pColumn->GetAlignment();
		nAlign &= ~(xtpColumnIconLeft | xtpColumnIconCenter | xtpColumnIconRight);

		switch (nMenuResult)
		{
		case ID_COLUMN_ICON_ALIGMENT_LEFT:
			pColumn->SetAlignment(nAlign | xtpColumnIconLeft);
			break;
		case ID_COLUMN_ICON_ALIGMENT_RIGHT:
			pColumn->SetAlignment(nAlign | xtpColumnIconRight);
			break;
		case ID_COLUMN_ICON_ALIGMENT_CENTER:
			pColumn->SetAlignment(nAlign | xtpColumnIconCenter);
			break;
		}
	}

	// ���������˵���
	switch (nMenuResult)
	{
	case ID_COLUMN_BESTFIT:
		GetReportCtrl().GetColumns()->GetReportHeader()->BestFit(pColumn);
		break;
	}
}

void CSclSmvView::OnReportFocusChanging(NMHDR* pNMHDR, LRESULT* /*result*/)
{
	// TODO: Handle command.
	CMainFrame *pMain =(CMainFrame *)AfxGetMainWnd();
	pMain->SetStatusBarText(RCSTR_SMV_CTRL_REPORT);
}

///
/// @brief
///     ����ָ��IED
///
int CSclSmvView::FindIED(const CString &iedName, const int nIndex)
{
	CXTPReportControl& wndReport = GetReportCtrl();
	CXTPReportRows *pRows = wndReport.GetRows();
	int count = pRows->GetCount();
	if ( nIndex >= count )
	{
		return -1;
	}
	int i = nIndex;
	do 
	{
		CString str = ((CXTPReportRecordItemText *)(pRows->GetAt(i)->GetRecord()->GetItem(COLUMN_SMV_IED)))->GetValue();
		int pos1 = str.Find(_T("["));
		int pos2 = str.Find(_T("]"));
		CString ied = str.Mid(pos1+1,pos2-pos1-1);
		if ( ied.Find(iedName) != -1 )
		{
			wndReport.SetFocusedColumn(wndReport.GetColumns()->GetAt(COLUMN_SMV_IED));
			wndReport.SetFocusedRow(wndReport.GetRows()->GetAt(i));
			return i;
		}
		i++;
		if ( i >= count )
		{
			i = 0;
		}
	} while ( i != nIndex );
	return -1;
}

///
/// @brief
///     ��ȡ�����еĿ��ƿ���Ŀ
///
int CSclSmvView::GetCtrlCount()
{
	return GetReportCtrl().GetRecords()->GetCount();
}
