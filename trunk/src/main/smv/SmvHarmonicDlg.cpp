// SmvHarmonicDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SmvHarmonicDlg.h"
#include "src/main/eventproc.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/dsmdatalayer.h"
#include "src/main/mgr/statusmgr.h"
#include "src/utils/res/dsm_fontmgr.h"
#include "src/main/pagedata/pagedata.h"

// smv - 谐波频率
// 
#if SU_FEI
#define DSM_HARM_REP_WIDTH_CHAN_INDEX      40
#define DSM_HARM_REP_WIDTH_CHAN            135
#define DSM_HARM_REP_COL_WIDTH             85
#define DSM_HARM_REP_COL_SHOW_COUNT        5

#define DSM_RATE_REP_WIDTH_CHAN_INDEX      40
#define DSM_RATE_REP_WIDTH_CHAN            135
#define DSM_RATE_REP_COL_WIDTH             85
#define DSM_RATE_REP_COL_SHOW_COUNT        5

#else
#define DSM_HARM_REP_WIDTH_CHAN_INDEX      40
#define DSM_HARM_REP_WIDTH_CHAN            159
#define DSM_HARM_REP_COL_WIDTH             82
#define DSM_HARM_REP_COL_SHOW_COUNT        4

#define DSM_RATE_REP_WIDTH_CHAN_INDEX      40
#define DSM_RATE_REP_WIDTH_CHAN            159
#define DSM_RATE_REP_COL_WIDTH             82
#define DSM_RATE_REP_COL_SHOW_COUNT        4
#endif

#define DSM_HARMONIC_HIS_CHAN_SHOW_COUNT   3

#define DSM_HARMONIC_DISP_FONT_SIZE        -13          ///< 谐波通道字体大小
#define DSM_HARMONIC_INFO_FONT_SIZE        -13          ///< 谐波图内字体大小


enum SHShowReportType
{
    shShowHarmReport = 0,			///< 有效值类型
#if SU_FEI
    shShowHistogram,				///< 柱状图类型
#endif
    shShowRateReport,				///< 含有率类型
};


//
//THD DC 基波 次
//
#define H_THD_DC_TIMES_COUNT                23

/**
 *     谐波 含有率
 *
 *  \note
 *      通道号 通道 THD 直流 基波 n次
 */
class CDsmHarmRateRecord : public CELRecord
{
public:

    CDsmHarmRateRecord()
    {
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
    }
    ~CDsmHarmRateRecord() {}
};

/**
 *     谐波 有效值
 *
 *  \note
 *      通道号 通道 THD 直流 基波 n次
 */
class CDsmHarmonicRecord : public CELRecord
{
public:

    CDsmHarmonicRecord()
    {
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
    }
    ~CDsmHarmonicRecord() {}
};


/**
 *      谐波控制块相关数据
 */
class CCtrlBlkDataHormonic : public ICtrlBlkData
{
public:
    CCtrlBlkDataHormonic(): ICtrlBlkData()
        , m_eShowType(shShowRateReport)
        , m_nHarmRepLeftColIndex(0)
        , m_nHisCurPageIndex(0)
        , m_nRateRepLeftColIndex(0)
    {

    }

    virtual ~CCtrlBlkDataHormonic() {}

public:
    SHShowReportType     m_eShowType;
    int                  m_nHisCurPageIndex;
    int                  m_nHarmRepLeftColIndex;
    int                  m_nRateRepLeftColIndex;
};



// CSmvHarmonicDlg 对话框

IMPLEMENT_DYNAMIC(CSmvHarmonicDlg, DSM_SMVBASEDLG)

CSmvHarmonicDlg::CSmvHarmonicDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSmvHarmonicDlg::IDD, DSM_STR_HARMONIC_CAPTION,  pParent)
    , m_eShowType(shShowRateReport)
    , m_nHarmRepLeftColIndex(0)
    , m_nHisCurPageIndex(0)
    , m_nChannelCount(0)
    , m_nRateRepLeftColIndex(0)
#if !SU_FEI
	, m_bHisFlag(FALSE)
#endif
{


}

CSmvHarmonicDlg::~CSmvHarmonicDlg()
{
}

void CSmvHarmonicDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSmvHarmonicDlg, DSM_SMVBASEDLG)
    ON_WM_TIMER()
    ON_COMMAND(ID_MENU_SMV_F4, &CSmvHarmonicDlg::OnMenuF4)

#if !SU_FEI
	ON_COMMAND(ID_MENU_SMV_F5, &CSmvHarmonicDlg::OnMenuF5)
#endif
END_MESSAGE_MAP()


// CSmvHarmonicDlg 消息处理程序

BOOL CSmvHarmonicDlg::OnInitDialog()
{
    DSM_SMVBASEDLG::OnInitDialog();

    // TODO:  在此添加额外的初始化

    if (m_pDataLayer == NULL )
    {
        return FALSE;
    }
    if (m_pCurrentCtrlBlk != NULL)
    {
        m_nChannelCount = m_pCurrentCtrlBlk->nChannelCount;
    }
    else 
    {
        return FALSE;
    }
    _updateCaption();
    _restorePage();
    switch(m_eShowType)
    {
    case shShowRateReport:
        {
            _showRateReport();
        }
        break;
    case shShowHarmReport:
        {
            _showHarmReport();
        }
        break;
#if SU_FEI
    case shShowHistogram:
        {
            _showHistogram();
        }
        break;
#endif
    default:
        _showRateReport();
        break;
    }

#if !SU_FEI
	if (m_bHisFlag)
	{
		_showHistogram();
	}
#endif

    _beginTimer();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CSmvHarmonicDlg::_createRateReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    //报表
    m_ctlRateReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_SMV_HARMONIC_RET);

    // 设置字体
    m_ctlRateReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlRateReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_ctlRateReport.PostInitTemplateStyle();

    m_ctlRateReport.InsertColumn(0, 
        _T(""), 
        EL_CENTER, DSM_RATE_REP_WIDTH_CHAN_INDEX);                 //通道索引

    m_ctlRateReport.InsertColumn(1, 
        dsm_stringMgr::Get(DSM_STR_HARMONIC_CHANNEL), 
        EL_LEFT, DSM_RATE_REP_WIDTH_CHAN);                       //通道

    for (int i = 0; i < DSM_RATE_REP_COL_SHOW_COUNT; ++i)
    {
        m_ctlRateReport.InsertColumn(i + 2, 
            _T(""), 
            EL_CENTER, DSM_RATE_REP_COL_WIDTH);                 //
    }
    _resetRateReport();
    _updateRateReportColumnTitle();

    // 设置选择焦点样式
    m_ctlRateReport.SetRowSelectionVisible(FALSE);
    m_ctlRateReport.SetRowFocusEnable(FALSE);
    m_ctlRateReport.SetFreezeSelectionVisible(TRUE);
    m_ctlRateReport.SetFocusedColIndex(0);
}
void CSmvHarmonicDlg::_showRateReport()
{
    if(m_ctlRateReport.GetSafeHwnd())
    {
        _updateRateReportColumnTitle();
        m_ctlRateReport.ShowWindow(SW_SHOW);
    }
    else
    {
        _createRateReport();
    }

    if(m_ctlHarmonicHistogram.GetSafeHwnd())
    {
        m_ctlHarmonicHistogram.ShowWindow(SW_HIDE);
    }

    if(m_ctlHarmonicReport.GetSafeHwnd())
    {
        m_ctlHarmonicReport.ShowWindow(SW_HIDE);
    }

    m_ctlRateReport.SetFocus();
    m_eShowType = shShowRateReport;

#if !SU_FEI
	
	m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_RATE_SEL), NULL, NULL);

	m_bHisFlag = false;

	m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_HARMONIC_UNSEL), NULL, NULL);

#endif

    //m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_HARM_RATE), NULL, NULL);
    m_pNaviBar->UpdateNaviMenu();
}


void CSmvHarmonicDlg::_resetRateReport()
{
    if (m_ctlRateReport.GetSafeHwnd())
    {
        m_ctlRateReport.DeleteAllRecordRow();
        for (int i = 0; i < m_nChannelCount; ++i)
        {
            m_ctlRateReport.InsertRecordRow(i, new CDsmHarmRateRecord);
        }
    }
}
void CSmvHarmonicDlg::_updateRateReport()
{
    if(!::IsWindow(m_ctlRateReport.GetSafeHwnd()))
    {
        return;
    }
    
    if (_checkChannel())
    {
        return;
    }

    if (m_pDataLayer == NULL)
    {
        return;
    }
    //
    //刷新数据
    //

    if (!m_pDataLayer->GetHarmonicData(&m_dataHarmonic))
    {
        return;
    }

    CDsmHarmRateRecord* pRecord = NULL;
    CString strIndex = _T("");
    CString strFormat  = _T("");
    
    CSmvRecvChannelInfo* pSmvRecChanInfo = NULL;
    for (int i = 0; i < m_nChannelCount; ++i)
    {
        pRecord = (CDsmHarmRateRecord*)m_ctlRateReport.GetRecordRow(i);

        strIndex.Format(_T("%d"), i + 1);
        pRecord->GetItem(0)->SetCaption(strIndex);
        pSmvRecChanInfo = m_pCurrentCtrlBlk->pRecvConfig->GetOneChannelInfo(i);
        if (pSmvRecChanInfo)
        {
           pRecord->GetItem(1)->SetCaption(pSmvRecChanInfo->strChannelDesc.c_str());
        }
        else
        {
            pRecord->GetItem(1)->SetCaption(_T(""));
        }

        for (int k = 0; k < DSM_RATE_REP_COL_SHOW_COUNT; ++k)
        {
            if( (m_nRateRepLeftColIndex + k + 2 ) == 2)
            {
                //THD
                strFormat.Format(_T("%.03f"), m_dataHarmonic.GetChannelHarmTHD(i));
            }
            else if ((m_nRateRepLeftColIndex + k + 2 ) == 3)
            {
                //直流
                strFormat.Format(_T("%.03f"), m_dataHarmonic.GetChannelDcTHD(i));
            }
            else
            {
                //n次
                float fHarmTHD = 0.0f;
                fHarmTHD = m_dataHarmonic.GetChannelHarmTHD(i, (k + m_nRateRepLeftColIndex -1));
                strFormat.Format(_T("%.03f"), fHarmTHD);
            }
            pRecord->GetItem(k + 2)->SetCaption(strFormat);
        }
    }
    if (m_ctlRateReport.GetRecordRowCount() > 0)
    {
        m_ctlRateReport.RedrawRowItems(0, m_ctlRateReport.GetRecordRowCount()-1);
    }
}
void CSmvHarmonicDlg::_updateRateReportColumnTitle()
{
    if(!::IsWindow(m_ctlRateReport.GetSafeHwnd()))
    {
        return;
    }

    CString strTitle = _T("");
    LVCOLUMN column;
    column.mask=LVCF_TEXT;
    for (int i = 0; i < DSM_RATE_REP_COL_SHOW_COUNT; ++i)
    {

        //
        //重置标题
        //
        if( (m_nRateRepLeftColIndex + i + 2 ) == 2)
        {
            //THD
            strTitle = dsm_stringMgr::Get(DSM_STR_HARMONIC_THD);
        }
        else if ((m_nRateRepLeftColIndex + i + 2 ) == 3)
        {
            //直流
            strTitle = dsm_stringMgr::Get(DSM_STR_HARMONIC_DC);
        }
        else if((m_nRateRepLeftColIndex + i + 2 ) == 4)
        {
            //基波
            strTitle = dsm_stringMgr::Get(DSM_STR_HARMONIC_FUND_WAVE);
        }
        else
        {
            //n次
            strTitle.Format(_T("%d%s"),  i + m_nRateRepLeftColIndex -1, dsm_stringMgr::Get(DSM_STR_HARMONIC_TIMES));
        }
        strTitle += _T("(%)");
        column.pszText = strTitle.GetBuffer();
        m_ctlRateReport.SetColumn(i + 2, &column);
        strTitle.ReleaseBuffer();
    }

    _updateRateReport();
}
void CSmvHarmonicDlg::_createHarmReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    //报表
    m_ctlHarmonicReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_SMV_HARMONIC_RET);

    // 设置字体
    m_ctlHarmonicReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlHarmonicReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlHarmonicReport.PostInitTemplateStyle();

    m_ctlHarmonicReport.InsertColumn(0, 
        _T(""), 
        EL_CENTER, DSM_HARM_REP_WIDTH_CHAN_INDEX);                 //通道索引

    m_ctlHarmonicReport.InsertColumn(1, 
        dsm_stringMgr::Get(DSM_STR_HARMONIC_CHANNEL), 
        EL_LEFT, DSM_HARM_REP_WIDTH_CHAN);                       //通道

    for (int i = 0; i < DSM_HARM_REP_COL_SHOW_COUNT; ++i)
    {
        m_ctlHarmonicReport.InsertColumn(i + 2, 
            _T(""), 
            EL_LEFT, DSM_HARM_REP_COL_WIDTH);                 //
    }
    _resetHarmReport();
    _updateHarmReportColumnTitle();

    // 设置选择焦点样式
    m_ctlHarmonicReport.SetRowSelectionVisible(FALSE);
    m_ctlHarmonicReport.SetRowFocusEnable(FALSE);
    m_ctlHarmonicReport.SetFreezeSelectionVisible(TRUE);
    m_ctlHarmonicReport.SetFocusedColIndex(0);
}


void CSmvHarmonicDlg::_showHarmReport()
{
    if(m_ctlHarmonicReport.GetSafeHwnd())
    {
        _updateHarmReportColumnTitle();
        m_ctlHarmonicReport.ShowWindow(SW_SHOW);
    }
    else
    {
        _createHarmReport();
    }

    if(m_ctlHarmonicHistogram.GetSafeHwnd())
    {
        m_ctlHarmonicHistogram.ShowWindow(SW_HIDE);
    }

    if(m_ctlRateReport.GetSafeHwnd())
    {
        m_ctlRateReport.ShowWindow(SW_HIDE);
    }

    m_ctlHarmonicReport.SetFocus();
    m_eShowType = shShowHarmReport;

#if !SU_FEI
	m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_EFFECTIVE_SEL), NULL, NULL);	
	m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_HARMONIC_UNSEL), NULL, NULL);
	m_bHisFlag = FALSE;
#endif

   // m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_HARM_HARM), NULL, NULL);
    m_pNaviBar->UpdateNaviMenu();
}

void CSmvHarmonicDlg::_resetHarmReport()
{
    if (m_ctlHarmonicReport.GetSafeHwnd())
    {
        m_ctlHarmonicReport.DeleteAllRecordRow();
        for (int i = 0; i < m_nChannelCount; ++i)
        {
            m_ctlHarmonicReport.InsertRecordRow(i, new CDsmHarmonicRecord);
        }
    }
}
void CSmvHarmonicDlg::_updateHarmReport()
{
    if(!::IsWindow(m_ctlHarmonicReport.GetSafeHwnd()))
    {
        return;
    }
    
    if (_checkChannel())
    {
        return;
    }
    if (m_pDataLayer == NULL)
    {
        return;
    }
    //
    //刷新数据
    //

    if (!m_pDataLayer->GetHarmonicData(&m_dataHarmonic))
    {
        return;
    }

    CDsmHarmonicRecord* pRecord = NULL;
    CString strIndex = _T("");
    CString strFormat  = _T("");
    float   fHarmContent = 0.0f;
    float   fHarmAngle = 0.0f;
    
    CSmvRecvChannelInfo* pSmvRecChanInfo = NULL;

    for (int i = 0; i < m_nChannelCount; ++i)
    {
        pRecord = (CDsmHarmonicRecord*)m_ctlHarmonicReport.GetRecordRow(i);
        
        strIndex.Format(_T("%d"), i + 1);
        pRecord->GetItem(0)->SetCaption(strIndex);
        
        pSmvRecChanInfo = m_pCurrentCtrlBlk->pRecvConfig->GetOneChannelInfo(i);
        if (pSmvRecChanInfo)
        {
            pRecord->GetItem(1)->SetCaption(pSmvRecChanInfo->strChannelDesc.c_str());
        }
        else
        {
            pRecord->GetItem(1)->SetCaption(_T(""));
        }

        for (int k = 0; k < DSM_HARM_REP_COL_SHOW_COUNT; ++k)
        {
            if( (m_nHarmRepLeftColIndex + k + 2 ) == 2)
            {
                //THD
                strFormat.Format(_T("%.03f%%"), m_dataHarmonic.GetChannelHarmTHD(i));
            }
            else if ((m_nHarmRepLeftColIndex + k + 2 ) == 3)
            {
                //直流
                strFormat.Format(_T("%.03f"), m_dataHarmonic.GetChannelDCValue(i));
            }
            else
            {
                //n次
                m_dataHarmonic.GetChannelHarmInfo(i, (k + m_nHarmRepLeftColIndex -1), fHarmContent, fHarmAngle);
                strFormat.Format(_T("%.03f"), fHarmContent);
            }
            pRecord->GetItem(k + 2)->SetCaption(strFormat);
        }
    }

    if (m_ctlHarmonicReport.GetRecordRowCount() > 0)
    {
        m_ctlHarmonicReport.RedrawRowItems(0, m_ctlRateReport.GetRecordRowCount()-1);
    }
}

void CSmvHarmonicDlg::_updateHarmReportColumnTitle()
{
    if(!::IsWindow(m_ctlHarmonicReport.GetSafeHwnd()))
    {
        return;
    }

    CString strTitle = _T("");
    LVCOLUMN column;
    column.mask=LVCF_TEXT;
    for (int i = 0; i < DSM_HARM_REP_COL_SHOW_COUNT; ++i)
    {

        //
        //重置标题
        //
        if( (m_nHarmRepLeftColIndex + i + 2 ) == 2)
        {
            //THD
            strTitle = dsm_stringMgr::Get(DSM_STR_HARMONIC_THD);
        }
        else if ((m_nHarmRepLeftColIndex + i + 2 ) == 3)
        {
            //直流
            strTitle = dsm_stringMgr::Get(DSM_STR_HARMONIC_DC);
        }
        else if((m_nHarmRepLeftColIndex + i + 2 ) == 4)
        {
            //基波
            strTitle = dsm_stringMgr::Get(DSM_STR_HARMONIC_FUND_WAVE);
        }
        else
        {
            //n次
            strTitle.Format(_T("%d%s"),  i + m_nHarmRepLeftColIndex -1, dsm_stringMgr::Get(DSM_STR_HARMONIC_TIMES));
        }
        column.pszText = strTitle.GetBuffer();
        m_ctlHarmonicReport.SetColumn(i + 2, &column);
        strTitle.ReleaseBuffer();
    }

    _updateHarmReport();
}

void CSmvHarmonicDlg::_createHistogram()
{
    CRect rcReport;
    GetClientRect(rcReport);
    rcReport.DeflateRect(10, 10, 10, 10);

    //柱状图
    m_ctlHarmonicHistogram.Create(_T(""), WS_CHILD | WS_VISIBLE|WS_TABSTOP , 
        rcReport, this, IDC_SMV_HARMONIC_HISTOGRAM);
    m_ctlHarmonicHistogram.SetGramCount(1, 3);
    m_ctlHarmonicHistogram.ModifyStyle(0,SS_OWNERDRAW);
    m_ctlHarmonicHistogram.SetTitleVisible(TRUE);
    m_ctlHarmonicHistogram.RemoveASpecialPillar(0);
    m_ctlHarmonicHistogram.AddASpecialPillar(_T("THD"));
    m_ctlHarmonicHistogram.AddASpecialPillar(_T("DC"));
    m_ctlHarmonicHistogram.SetCommonPillarCount(H_THD_DC_TIMES_COUNT-2);
#if !CHUN_HUA
	m_ctlHarmonicHistogram.SetPillarAreaGird(7, 5);
#else
	m_ctlHarmonicHistogram.SetPillarAreaGird(7, 4);
#endif
    m_ctlHarmonicHistogram.SetScaleInterval(0, 3, 25, _T("%"));
    m_ctlHarmonicHistogram.SetScaleInterval(1, 3, 25, _T("%"));
    m_ctlHarmonicHistogram.SetScaleInterval(2, 3, 25, _T("%"));
    m_ctlHarmonicHistogram.SetPillarColor(0,RGB(235,67,21));//柱状图
    m_ctlHarmonicHistogram.SetCursorColor(0,RGB(235,250,30));
    m_ctlHarmonicHistogram.SetPillarColor(1,RGB(24,230,30));
    m_ctlHarmonicHistogram.SetCursorColor(1,RGB(235,250,30));
    m_ctlHarmonicHistogram.SetPillarColor(2,RGB(44,54,244));
    m_ctlHarmonicHistogram.SetCursorColor(2,RGB(235,250,30));
    m_ctlHarmonicHistogram.SetTitleColor( RGB(128,128,135));


    m_ctlHarmonicHistogram.SetFeaturePillar(0, 2);
    m_ctlHarmonicHistogram.SetFeaturePillar(1, 4);
    m_ctlHarmonicHistogram.SetFeaturePillar(2, 6);
    m_ctlHarmonicHistogram.SetFeaturePillar(3, 8);
    m_ctlHarmonicHistogram.SetFeaturePillar(4, 10);

    m_ctlHarmonicHistogram.SetBkgndColor(DSM_COLOR_BK);

    m_ctlHarmonicHistogram.SetTitleFont(dsm_fontmgr::GetInstance()->GetFont(DSM_HARMONIC_DISP_FONT_SIZE));
    m_ctlHarmonicHistogram.SetFeatureInfoFont(dsm_fontmgr::GetInstance()->GetFont(DSM_HARMONIC_INFO_FONT_SIZE));
    m_ctlHarmonicHistogram.SetPillarDescFont(dsm_fontmgr::GetInstance()->GetFont(DSM_HARMONIC_INFO_FONT_SIZE));
    m_ctlHarmonicHistogram.SetScaleValFont(dsm_fontmgr::GetInstance()->GetFont(DSM_HARMONIC_INFO_FONT_SIZE));

    //m_ctlHarmonicHistogram.Init();

    //
    //设置标题
    //
    _updateHistorgramTitle();
}


void CSmvHarmonicDlg::_showHistogram()
{

    if(m_ctlHarmonicHistogram.GetSafeHwnd())
    {
        m_ctlHarmonicHistogram.ShowWindow(SW_SHOW);
    }
    else
    {
        _createHistogram();
    }

    if(m_ctlHarmonicReport.GetSafeHwnd())
    {
        m_ctlHarmonicReport.ShowWindow(SW_HIDE);
    }

    if(m_ctlRateReport.GetSafeHwnd())
    {
        m_ctlRateReport.ShowWindow(SW_HIDE);
    }

    m_ctlHarmonicHistogram.SetFocus();

#if SU_FEI
    m_eShowType = shShowHistogram;
#endif
    m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_RATE_EFFECTIVE_UNSEL), NULL, NULL);

	m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_HARMONIC_SEL), NULL, NULL);
	//m_menuMain.SetMenuItemCaption(4, L"柱状图");
	m_bHisFlag = TRUE;

    m_pNaviBar->UpdateNaviMenu();
}

void CSmvHarmonicDlg::_updateHistorgramTitle()
{
    if (m_pCurrentCtrlBlk == NULL || !m_ctlHarmonicHistogram.GetSafeHwnd())
    {
        return;
    }
    
    CString strTitle = _T("");
    int nStartIndex = m_nHisCurPageIndex*DSM_HARMONIC_HIS_CHAN_SHOW_COUNT;
    
    CSmvRecvChannelInfo* pSmvRecChanInfo = NULL;
    for (int i = 0; i < DSM_HARMONIC_HIS_CHAN_SHOW_COUNT; ++i)
    {
        if (( nStartIndex + i )< m_nChannelCount)
        {
            pSmvRecChanInfo = m_pCurrentCtrlBlk->pRecvConfig->GetOneChannelInfo(nStartIndex + i);
            if (pSmvRecChanInfo)
            {
                strTitle.Format(_T("%d:%s"),
                    (nStartIndex + i + 1),
                    pSmvRecChanInfo->strChannelDesc.c_str());
            }
            else
            {
                strTitle.Format(_T("%d:"), (nStartIndex + i + 1));
            }
        }
        else
        {
            strTitle = _T("");
        }
        m_ctlHarmonicHistogram.SetTitle(i, strTitle);
    }
    m_ctlHarmonicHistogram.Init();

    _updateHistorgram();
}
void CSmvHarmonicDlg::_updateHistorgram()
{
    if(!::IsWindow(m_ctlHarmonicHistogram.GetSafeHwnd()))
    {
        return;
    }

    if (_checkChannel())
    {
        return;
    }

    if (m_pDataLayer == NULL)
    {
        return;
    }

    CString strTitle = _T("");

    if (!m_pDataLayer->GetHarmonicData(&m_dataHarmonic))
    {
        return;
    }

    int nStartIndex = m_nHisCurPageIndex*DSM_HARMONIC_HIS_CHAN_SHOW_COUNT;
    static float  fCmnHarmAngle[21];
    static float  fCmnHarmContent[21];
    static float  fCmnHarmRate[21];
    static float  fSplPillarData[2];


    CString strPillarTitle = _T("");
    m_ctlHarmonicHistogram.ErasePillar();

    CString strHarmContent = _T("");
    CString strPhaseAngle = _T("");
    CString strRate = _T("");
    CString strFrequence = _T("");

    strHarmContent = dsm_stringMgr::Get(DSM_STR_HARMONIC_HARM_CONTENT);
    strPhaseAngle = dsm_stringMgr::Get(DSM_STR_HARMONIC_PHASE_ANGLE);
    strRate = dsm_stringMgr::Get(DSM_STR_EFF_VAL_RATE);
    strFrequence = dsm_stringMgr::Get(DSM_STR_EFF_VAL_FREQUENCE);

    for (int i = 0; i < DSM_HARMONIC_HIS_CHAN_SHOW_COUNT; ++i)
    {
        memset(fCmnHarmAngle, 0, sizeof(fCmnHarmAngle));
        memset(fCmnHarmContent, 0, sizeof(fCmnHarmContent));
        memset(fSplPillarData, 0, sizeof(fSplPillarData));
        memset(fCmnHarmRate, 0, sizeof(fCmnHarmRate));

        if (( nStartIndex + i )< m_nChannelCount)
        {
            for (int k = 0; k < 21; ++k)
            {
                m_dataHarmonic.GetChannelHarmInfo(nStartIndex + i, k + 1, fCmnHarmContent[k], fCmnHarmAngle[k]);
                fCmnHarmRate[k] = m_dataHarmonic.GetChannelHarmTHD(nStartIndex+i, k + 1);
            }

            fSplPillarData[0] = m_dataHarmonic.GetChannelHarmTHD(nStartIndex+i);
            fSplPillarData[1] = m_dataHarmonic.GetChannelDcTHD(nStartIndex+i);

            int curPos  = m_ctlHarmonicHistogram.GetCursorPos();

            CString str1,str2;
            if ( curPos == 0)
            {
                strPillarTitle.Format(_T("THD:%.03f%%"),fSplPillarData[0]);
            }
            else if(curPos == 1)
            {
                strPillarTitle.Format(_T("DC:%.03f%%"),fSplPillarData[1]);
            }
            else
            {
                strPillarTitle.Format(_T("[%d]%s:%.03f %s:%.03f%% %s:%.03f %s:%dHz"), 
                    curPos-1,
                    strHarmContent, fCmnHarmContent[curPos - 2], 
                    strRate, fCmnHarmRate[curPos-2],
                    strPhaseAngle, fCmnHarmAngle[curPos - 2],
                    strFrequence, (curPos-1)*50
                );
            }
            m_ctlHarmonicHistogram.DrawHistogram(i, fCmnHarmRate, fSplPillarData, strPillarTitle );
        }
    }
    m_ctlHarmonicHistogram.Invalidate();
}

void CSmvHarmonicDlg::_changeShowType()
{
    //
    //改变显示方式
    //
    if (m_eShowType == shShowRateReport)
    {
        //含有率->有效值
        _showHarmReport();
    }
    else if(m_eShowType == shShowHarmReport)
#if SU_FEI
	{
        //有效值->柱状图
        _showHistogram();
    }
    else
    {
        //柱状图->含有率
        _showRateReport();
    }
#else
	{
		 //有效值->含有率
		_showRateReport();
	}
#endif
    Invalidate(TRUE);
}

#if !SU_FEI
void CSmvHarmonicDlg::OnMenuF5()
{
		_showHistogram();
		m_bHisFlag = TRUE;
		
}
#endif

void CSmvHarmonicDlg::OnMenuF4()
{
#if !SU_FEI
	m_bHisFlag = FALSE;
	
#endif
    _changeShowType();
}

void CSmvHarmonicDlg::_modifyMenu()
{
    __super::_modifyMenu();

#if !SU_FEI
	if (m_bHisFlag)
	{
		///< 选择柱状图显示
		m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_HARMONIC_SEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_RATE_EFFECTIVE_UNSEL), NULL, NULL);
	}
	else
	{
		switch(m_eShowType)
		{
			///< 含有率
		case shShowRateReport:
			{
				m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_RATE_SEL), NULL, NULL);

			}
			break;
			///< 有效值
		case shShowHarmReport:
			{
				m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_EFFECTIVE_SEL), NULL, NULL);
			}
			break;
		
			///< 柱状图
	   /* case shShowHistogram:
			{
				m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_HARM_HISTOGRAM), NULL, NULL);
			}
			break;*/
		default:
			break;
		}


		///< 没有选中柱状图时的显示
		m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_HARMONIC_UNSEL), NULL, NULL);
	}	

	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_HARMONIC_CAPTION));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);

	//需要根据设置中选中的一次值还是二次值进行判断，才能正常显示
	CSmvRecvConfigPublic* pCfg = m_pDataLayer->GetSmvConfigPublic();

	if (pCfg->GetSmvDispType() == SMV_VALUE_TYPE_SECONDARY)
	{
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SECOND_SEL), NULL, NULL);
	}
	else if (pCfg->GetSmvDispType() == SMV_VALUE_TYPE_PRIMARY)
	{
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_FIRST_SEL), NULL, NULL);
	}
#endif
}

bool CSmvHarmonicDlg::_switchCtrlBlk(UINT nCmdID)
{
    bool bRet = __super::_switchCtrlBlk(nCmdID);
    if (bRet)
    {
        m_nChannelCount = m_pCurrentCtrlBlk->nChannelCount;

        _resetRateReport();
        _resetHarmReport();
        
        //
        //调整当前页显示的通道，如果当前页显示通道索引的超过总通道数，则设置为从通道0开始
        //
        int nPageCount = m_nChannelCount/DSM_HARMONIC_HIS_CHAN_SHOW_COUNT;
        if (m_nChannelCount%DSM_HARMONIC_HIS_CHAN_SHOW_COUNT > 0)
        {
            ++nPageCount;
        }
        if (m_nHisCurPageIndex >= nPageCount)
        {
            m_nHisCurPageIndex = 0;
            _updateHistorgramTitle();
        }
    }
    return bRet;
}


bool CSmvHarmonicDlg::_checkChannel()
{

    // 通道发生变化，
    if(m_pCurrentCtrlBlk->nChannelCount != m_pDataLayer->GetCurrentCtrlBlkRealTimeChannelCounts())
    {
        if(CStatusMgr::ShowTipsStyle() == 0x00)
        {
            // 界面tips提示
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_CHANNEL_NUM_CHANGE));
        }

        if(!m_bChannelChange)
        {
            m_bChannelChange = true;
            //
            //含有率
            //
            if (m_ctlRateReport.GetSafeHwnd())
            {
                CDsmHarmRateRecord* pRecord = NULL;
                CString strIndex = _T("");
                CSmvRecvChannelInfo* pSmvRecChanInfo = NULL;
                for (int i = 0; i < m_nChannelCount; ++i)
                {
                    pRecord = (CDsmHarmRateRecord*)m_ctlRateReport.GetRecordRow(i);

                    strIndex.Format(_T("%d"), i + 1);
                    pRecord->GetItem(0)->SetCaption(strIndex);
                    pSmvRecChanInfo = m_pCurrentCtrlBlk->pRecvConfig->GetOneChannelInfo(i);
                    if (pSmvRecChanInfo)
                    {
                        pRecord->GetItem(1)->SetCaption(pSmvRecChanInfo->strChannelDesc.c_str());
                    }
                    else
                    {
                        pRecord->GetItem(1)->SetCaption(_T(""));
                    }
                    for (int k = 0; k < DSM_RATE_REP_COL_SHOW_COUNT; ++k)
                    {
                        pRecord->GetItem(k + 2)->SetCaption(g_csBlankValue);
                    }
                }
                if (m_ctlRateReport.GetRecordRowCount() > 0)
                {
                    m_ctlRateReport.RedrawRowItems(0, m_ctlRateReport.GetRecordRowCount()-1);
                }
            }
            //
            //有效值
            //
            if (m_ctlHarmonicReport.GetSafeHwnd())
            {
                CDsmHarmonicRecord* pRecord = NULL;
                CString strIndex = _T("");
                CSmvRecvChannelInfo* pSmvRecChanInfo = NULL;
                for (int i = 0; i < m_nChannelCount; ++i)
                {
                    pRecord = (CDsmHarmonicRecord*)m_ctlHarmonicReport.GetRecordRow(i);

                    strIndex.Format(_T("%d"), i + 1);
                    pRecord->GetItem(0)->SetCaption(strIndex);

                    pSmvRecChanInfo = m_pCurrentCtrlBlk->pRecvConfig->GetOneChannelInfo(i);
                    if (pSmvRecChanInfo)
                    {
                        pRecord->GetItem(1)->SetCaption(pSmvRecChanInfo->strChannelDesc.c_str());
                    }
                    else
                    {
                        pRecord->GetItem(1)->SetCaption(_T(""));
                    }

                    for (int k = 0; k < DSM_HARM_REP_COL_SHOW_COUNT; ++k)
                    {
                        pRecord->GetItem(k + 2)->SetCaption(g_csBlankValue);
                    }
                }
            }
            //
            //柱状图
            //
            if (m_ctlHarmonicHistogram.GetSafeHwnd())
            {
                _updateHistorgramTitle();
                

                int nStartIndex = m_nHisCurPageIndex*DSM_HARMONIC_HIS_CHAN_SHOW_COUNT;
                float  fCmnHarmAngle[21];
                float  fCmnHarmContent[21];
                float  fCmnHarmRate[21];
                float  fSplPillarData[2];

                CString strPillarTitle = _T("");
                m_ctlHarmonicHistogram.ErasePillar();

                memset(fCmnHarmAngle, 0, sizeof(fCmnHarmAngle));
                memset(fCmnHarmContent, 0, sizeof(fCmnHarmContent));
                memset(fSplPillarData, 0, sizeof(fSplPillarData));
                memset(fCmnHarmRate, 0, sizeof(fCmnHarmRate));

                for (int i = 0; i < DSM_HARMONIC_HIS_CHAN_SHOW_COUNT; ++i)
                {
                    if (( nStartIndex + i )< m_nChannelCount)
                    {
                        m_ctlHarmonicHistogram.DrawHistogram(i, fCmnHarmRate, fSplPillarData, strPillarTitle );
                    }
                }
                m_ctlHarmonicHistogram.Invalidate();

            }

            // 隐藏『暂停开始』菜单标题
            //m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, L"");
            m_pNaviBar->UpdateNaviMenu();
        }

        return true;
    }
    else
    {
        if(m_bChannelChange)
        {
            m_bChannelChange = false;
            CStatusMgr::SetTips(L"", 0);

            // 恢复暂停开始菜单标题
          //  m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
            m_pNaviBar->UpdateNaviMenu();
        }
    }

    return false;
}


BOOL CSmvHarmonicDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    switch(pMsg->message)
    {
    case WM_KEYDOWN:
        {
            //按键按下，处理报表的上下左右移动和柱状图的翻页
            switch (pMsg->wParam)
            {
            case VK_UP:
                {
#if SU_FEI
                    if (m_eShowType == shShowHistogram)
#else
					if (m_bHisFlag)
#endif
                    {
                        //柱状图显示上一页
                        if (m_nHisCurPageIndex > 0)
                        {
                            --m_nHisCurPageIndex;
                            _updateHistorgramTitle();
                        }
                        return TRUE;
                    }
                    else
                    {
                        //默认报表处理
                        return DSM_SMVBASEDLG::PreTranslateMessage(pMsg);
                    }
                }
                break;
            case VK_LEFT:
                {
                    if (m_eShowType == shShowRateReport)
                    {
                        //显示列左移一页
                        if(m_nRateRepLeftColIndex > 0)
                        {
                            if(m_nRateRepLeftColIndex -  DSM_HARM_REP_COL_SHOW_COUNT>= 0)
                            {
                                m_nRateRepLeftColIndex -= DSM_HARM_REP_COL_SHOW_COUNT;
                            }
                            else
                            {
                                m_nRateRepLeftColIndex = 0;
                            }
                            _updateRateReportColumnTitle();
                        }
                        return TRUE;
                    }
                    else if(m_eShowType == shShowHarmReport)
                    {
                        //显示列左移一页
                        if(m_nHarmRepLeftColIndex > 0)
                        {
                            if(m_nHarmRepLeftColIndex -  DSM_HARM_REP_COL_SHOW_COUNT>= 0)
                            {
                                m_nHarmRepLeftColIndex -= DSM_HARM_REP_COL_SHOW_COUNT;
                            }
                            else
                            {
                                m_nHarmRepLeftColIndex = 0;
                            }
                            _updateHarmReportColumnTitle();
                        }
                        return TRUE;
                    }
                }
                break;
            case VK_DOWN:
                {
#if SU_FEI
                    if (m_eShowType == shShowHistogram)
#else
					if (m_bHisFlag)
#endif
                    {
                        //柱状图显示下一页
                        if ((m_nHisCurPageIndex + 1)*DSM_HARMONIC_HIS_CHAN_SHOW_COUNT < m_nChannelCount)
                        {
                            ++m_nHisCurPageIndex;
                            _updateHistorgramTitle();
                        }
                        return TRUE;
                    }
                    else
                    {
                        //交给报表默认处理
                        return DSM_SMVBASEDLG::PreTranslateMessage(pMsg);
                    }
                }
                break;
            case VK_RIGHT:
                {
                    if (m_eShowType == shShowRateReport)
                    { 
                        //显示列右移一页
                        if ( (m_nRateRepLeftColIndex + DSM_HARM_REP_COL_SHOW_COUNT) < H_THD_DC_TIMES_COUNT )
                        {
                            if (m_nRateRepLeftColIndex + 2 * DSM_HARM_REP_COL_SHOW_COUNT <= H_THD_DC_TIMES_COUNT)
                            {
                                m_nRateRepLeftColIndex += DSM_HARM_REP_COL_SHOW_COUNT;
                            }
                            else
                            {
                                m_nRateRepLeftColIndex = H_THD_DC_TIMES_COUNT - DSM_HARM_REP_COL_SHOW_COUNT;
                            }
                            _updateRateReportColumnTitle();
                        }
                        return TRUE;
                    }
                    else if (m_eShowType == shShowHarmReport)
                    {
                        //显示列右移一页
                        if ( (m_nHarmRepLeftColIndex + DSM_HARM_REP_COL_SHOW_COUNT) < H_THD_DC_TIMES_COUNT )
                        {
                            if (m_nHarmRepLeftColIndex + 2 * DSM_HARM_REP_COL_SHOW_COUNT <= H_THD_DC_TIMES_COUNT)
                            {
                                m_nHarmRepLeftColIndex += DSM_HARM_REP_COL_SHOW_COUNT;
                            }
                            else
                            {
                                m_nHarmRepLeftColIndex = H_THD_DC_TIMES_COUNT - DSM_HARM_REP_COL_SHOW_COUNT;
                            }
                            _updateHarmReportColumnTitle();
                        }
                        return TRUE;
                    }
                }
                break;
            default:
                break;
            }
        }
        break;
    case WM_KEYUP:
        {
            //按键弹起，处理柱状图的左右移动
            switch (pMsg->wParam)
            {
            case VK_LEFT:
                {
#if SU_FEI
                    if (m_eShowType == shShowHistogram)
#else
					if (m_bHisFlag)
#endif
                    {
                        m_ctlHarmonicHistogram.CursorMoveLeft();
                    }
                    return TRUE;
                }
                break;
            case VK_RIGHT:
                {
#if SU_FEI
					if (m_eShowType == shShowHistogram)
#else
					if (m_bHisFlag)
#endif
                    { 
                        m_ctlHarmonicHistogram.CursorMoveRight();
                    }
                    return TRUE;
                }
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
    return DSM_SMVBASEDLG::PreTranslateMessage(pMsg);
}

void CSmvHarmonicDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == m_nTimerID)
    {
        if (m_eShowType == shShowRateReport)
        {
            _updateRateReport();
        }
        else if(m_eShowType == shShowHarmReport)
        {
            _updateHarmReport();
        }
#if SU_FEI
        else
        {
            _updateHistorgram();
        }
#else
		if (m_bHisFlag)
		{
			_updateHistorgram();
		}
#endif
    }
    CSMVBaseDlg::OnTimer(nIDEvent);
}


void CSmvHarmonicDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    CCtrlBlkDataHormonic* pData = dynamic_cast<CCtrlBlkDataHormonic*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
    if(pData == NULL)
    {
        pData = new CCtrlBlkDataHormonic();
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);
    }
    ASSERT(pData);

    if(pData)
    {
        pData->m_eShowType = m_eShowType;
        pData->m_nHarmRepLeftColIndex = m_nHarmRepLeftColIndex;
        pData->m_nRateRepLeftColIndex = m_nRateRepLeftColIndex;
        pData->m_nHisCurPageIndex = m_nHisCurPageIndex;
    }
}

bool CSmvHarmonicDlg::_restorePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);
    ASSERT(CSmvHarmonicDlg::IDD == m_pPageDataExtra->GetPageData()->m_nPageID);

    // 获取当前的控制块索引
    CCtrlBlkDataHormonic* pData = dynamic_cast<CCtrlBlkDataHormonic*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));


    // 当前控制块若没绑定相关信息，则返回
    if(!pData)
    {
        return false;
    }

    // 根据当前页面计算需要显示的通道索引

    m_eShowType = pData->m_eShowType;
    m_nHarmRepLeftColIndex = pData->m_nHarmRepLeftColIndex;
    m_nRateRepLeftColIndex = pData->m_nRateRepLeftColIndex;
    m_nHisCurPageIndex = pData->m_nHisCurPageIndex;
    return true;
}

