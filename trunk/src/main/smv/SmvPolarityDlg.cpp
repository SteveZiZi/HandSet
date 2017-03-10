// SmvPolarityDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SmvPolarityDlg.h"
#include "src/main/resourcestr.h"
#include "src/main/resourcebmp.h"
#include "src/main/eventproc.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/pagedata/pagedata.h"
#include "src/main/common.h"


//////////////////////////////////////////////////////////////////////////
// smv - 极性
// 
#define DSM_POLARITY_TITLE_COL_HEIGHT       30

#if SU_FEI
#define DSM_POLARITY_RC_TITLE_ROW0          CRect(0  , 0  , 640, 30 )
#define DSM_POLARITY_RC_TITLE_ROW1          CRect(0  , 189, 640, 219 )

#define DSM_POLARITY_RC_CHEN_TITLE0         CRect(20 , 15 , 309, 42 )
#define DSM_POLARITY_RC_CHEN_TITLE1         CRect(332, 15 , 621, 42 )
#define DSM_POLARITY_RC_CHEN_TITLE2         CRect(20 , 204, 309, 231)
#define DSM_POLARITY_RC_CHEN_TITLE3         CRect(332, 204, 621, 231)

#define DSM_POLARITY_RC_POLAR0              CRect(20 , 15 , 309, 175)
#define DSM_POLARITY_RC_POLAR1              CRect(332, 15 , 621, 175)
#define DSM_POLARITY_RC_POLAR2              CRect(20 , 204, 309, 364)
#define DSM_POLARITY_RC_POLAR3              CRect(332, 204, 621, 364)

#define DSM_POLARITY_RC_POLAR_STD_BMP       CRect(0 , 0 , 252, 122)
#define DSM_POLARITY_RC_DAIL_IN_RADIUS      0
#define DSM_POLARITY_RC_DAIL_HAND_RADIUS    75
#define DSM_POLARITY_RC_DAIL_CIRCLE_CENTRE  CPoint(125,106)
#define DSM_POLARITY_RC_DAIL_DEGREE_START   0
#define DSM_POLARITY_RC_DAIL_DEGREE_END     180

#define DSM_POLARITY_RC_POLAR0_CTRL         CRect(38 , 47 , 290, 169)
#define DSM_POLARITY_RC_POLAR1_CTRL         CRect(350, 47 , 602, 169)
#define DSM_POLARITY_RC_POLAR2_CTRL         CRect(38 , 236, 290, 358)
#define DSM_POLARITY_RC_POLAR3_CTRL         CRect(350, 236, 602, 358)

#else

#define DSM_POLARITY_RC_CHEN_TITLE0         CRect(1 , 160 , 264, 203)
#define DSM_POLARITY_RC_CHEN_TITLE1         CRect(265, 160 , 528, 203 )
#define DSM_POLARITY_RC_CHEN_TITLE2         CRect(1 , 364, 264, 408)
#define DSM_POLARITY_RC_CHEN_TITLE3         CRect(265, 364, 528, 408)

#define DSM_POLARITY_RC_POLAR0              CRect(1 , 1 , 264, 160)
#define DSM_POLARITY_RC_POLAR1              CRect(265, 1 , 528, 160)
#define DSM_POLARITY_RC_POLAR2              CRect(1 , 205, 264, 364)
#define DSM_POLARITY_RC_POLAR3              CRect(265, 205, 528, 364)

#define DSM_POLARITY_RC_POLAR_STD_BMP       CRect(0 , 0 , 263, 159)//CRect(0 , 0 , 252, 122)
#define DSM_POLARITY_RC_DAIL_IN_RADIUS      0
#define DSM_POLARITY_RC_DAIL_HAND_RADIUS    65
#define DSM_POLARITY_RC_DAIL_CIRCLE_CENTRE  CPoint(131,116)
#define DSM_POLARITY_RC_DAIL_DEGREE_START   0
#define DSM_POLARITY_RC_DAIL_DEGREE_END     180

#define DSM_POLARITY_RC_POLAR0_CTRL         CRect(1, 1, 264, 160)//CRect(6 , 52 , 258, 174)
#define DSM_POLARITY_RC_POLAR1_CTRL         CRect(265, 1 , 528, 160)// CRect(272, 52 , 524, 174)
#define DSM_POLARITY_RC_POLAR2_CTRL         CRect(1 , 205, 264, 364)//CRect(6 , 241, 258, 363)
#define DSM_POLARITY_RC_POLAR3_CTRL         CRect(265, 205, 528, 364)//CRect(272, 241, 524, 363)

#if !SHENG_ZE
#define DSM_POLARITY_RC_X					CRect(0, 203, 528, 204)
#define DSM_POLARITY_RC_Y					CRect(264, 0, 265, 408)
#endif
#endif

//极性参数
#define DSM_POLARITY_CRBK                   DSM_REPORT_COLOR_BK/*RGB(41, 40, 49)*/
#define DSM_POLARITY_CTRL_CRBK              RGB(41, 40, 49)
#define DSM_POLARITY_TITLE_CRBK             DSM_REPORT_COLOR_BK/*RGB(41, 40, 49)*/
#define DSM_POLARITY_TITLE_CRTEXT           RGB(255, 255, 255)
#define DSM_POLARITY_CRGRID                 RGB(0, 0, 0)


#define  PO_PAGE_POLARITY_CTRL_COUNT   4

// CSmvPolarityDlg 对话框
/**
 *      有效值控制块相关数据
 */
class CCtrlBlkDataPolarity : public ICtrlBlkData
{
public:
    CCtrlBlkDataPolarity():ICtrlBlkData()
        , m_nCurPageIndex(0)
    {

    }

    virtual ~CCtrlBlkDataPolarity() {}

public:
    int  m_nCurPageIndex;
};


IMPLEMENT_DYNAMIC(CSmvPolarityDlg, DSM_SMVBASEDLG)

CSmvPolarityDlg::CSmvPolarityDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSmvPolarityDlg::IDD, DSM_STR_POLARITY_CAPTION, pParent)
    , m_nCurPageIndex(0)
    , m_nChannelCount(0)
{
}

CSmvPolarityDlg::~CSmvPolarityDlg()
{
}

void CSmvPolarityDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSmvPolarityDlg, DSM_SMVBASEDLG)
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_WM_ERASEBKGND()
    ON_COMMAND(ID_MENU_SMV_F3, &CSmvPolarityDlg::OnMenuF3)
#if SU_FEI
    ON_COMMAND(ID_MENU_SMV_F5, &CSmvPolarityDlg::OnMenuF5)
#else
	ON_COMMAND(ID_MENU_SMV_F4, &CSmvPolarityDlg::OnMenuF4)
#endif
END_MESSAGE_MAP()


// CSmvPolarityDlg 消息处理程序

BOOL CSmvPolarityDlg::OnInitDialog()
{
    DSM_SMVBASEDLG::OnInitDialog();

    // TODO:  在此添加额外的初始化

    if (m_pDataLayer == NULL)
    {
        return FALSE;
    }
    if (m_pCurrentCtrlBlk)
    {
        m_nChannelCount = m_pCurrentCtrlBlk->nChannelCount;
    }

    _beginTimer();
	_restorePage();
    _createReport();
    _updateCaption();
    _updatePolarityValue();
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CSmvPolarityDlg::OnPaint()
{

    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 DSM_SMVBASEDLG::OnPaint()
    //
    //绘制背景
    //
    
    CRect rcClient;
    GetClientRect(rcClient);

    CDC                     dcMem;
    CBitmap                 bmpMem;

    dcMem.CreateCompatibleDC(&dc);
    bmpMem.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
    CBitmap*  pOldbmp = dcMem.SelectObject(&bmpMem);

    CBrush brBk(DSM_COLOR_BK);
    CBrush brCtrlBk(DSM_POLARITY_CTRL_CRBK);
    CBrush brTitleBk(DSM_POLARITY_TITLE_CRBK);

    CBrush*  pOldBrush = dcMem.SelectObject(&brBk);
    //背景
    /*dcMem.Rectangle(rcClient);
    dcMem.SelectObject(&brCtrlBk);
    dcMem.Rectangle(DSM_POLARITY_RC_POLAR0);
    dcMem.Rectangle(DSM_POLARITY_RC_POLAR1);
    dcMem.Rectangle(DSM_POLARITY_RC_POLAR2);
    dcMem.Rectangle(DSM_POLARITY_RC_POLAR3);*/
#if SHENG_ZE
	dcMem.FillSolidRect(rcClient, RGB(83, 81, 93));
#else
	dcMem.FillSolidRect(rcClient, DSM_POLARITY_CTRL_CRBK);
	dcMem.FillSolidRect(DSM_POLARITY_RC_X, RGB(83, 81, 93));
	dcMem.FillSolidRect(DSM_POLARITY_RC_Y, RGB(83, 81, 93));
#endif
    //标题背景
    dcMem.SelectObject(&brTitleBk);
    dcMem.FillSolidRect(DSM_POLARITY_RC_CHEN_TITLE0, DSM_POLARITY_TITLE_CRBK );
    dcMem.FillSolidRect(DSM_POLARITY_RC_CHEN_TITLE1, DSM_POLARITY_TITLE_CRBK );
    dcMem.FillSolidRect(DSM_POLARITY_RC_CHEN_TITLE2, DSM_POLARITY_TITLE_CRBK );
    dcMem.FillSolidRect(DSM_POLARITY_RC_CHEN_TITLE3, DSM_POLARITY_TITLE_CRBK );
    //
    //每个标题
    //
    CString strTitle = _T("");

    int nOldBkMode = dcMem.SetBkMode(TRANSPARENT);
    COLORREF crOldColor = dcMem.SetTextColor(DSM_POLARITY_TITLE_CRTEXT);

    HGLOBAL hFont = dcMem.SelectObject(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    int nCurPageChannelCount(0);
    nCurPageChannelCount = (((m_nCurPageIndex+1) * PO_PAGE_POLARITY_CTRL_COUNT) <= m_nChannelCount) ? \
PO_PAGE_POLARITY_CTRL_COUNT : (m_nChannelCount - m_nCurPageIndex*PO_PAGE_POLARITY_CTRL_COUNT);
    
    if(m_pCurrentCtrlBlk)
    {
        CSmvRecvChannelInfo* pSmvRecChanInfo = NULL;
        if (nCurPageChannelCount >=1)
        {
            pSmvRecChanInfo = m_pCurrentCtrlBlk->pRecvConfig->GetOneChannelInfo(m_nCurPageIndex*4);
            if (pSmvRecChanInfo)
            {
                strTitle.Format(_T("%d:%s"), 
                    m_nCurPageIndex*PO_PAGE_POLARITY_CTRL_COUNT + 1, 
                    pSmvRecChanInfo->strChannelDesc.c_str());
            }
            else
            {
                strTitle.Format(_T("%d:"), 
                    m_nCurPageIndex*PO_PAGE_POLARITY_CTRL_COUNT + 1);
            }

            dcMem.DrawText(strTitle, DSM_POLARITY_RC_CHEN_TITLE0, DT_CENTER|DT_VCENTER| DT_NOPREFIX | DT_SINGLELINE);
        }
        if (nCurPageChannelCount >=2)
        {
            pSmvRecChanInfo = m_pCurrentCtrlBlk->pRecvConfig->GetOneChannelInfo(m_nCurPageIndex*4 + 1);
            if (pSmvRecChanInfo)
            {
                strTitle.Format(_T("%d:%s"), 
                    m_nCurPageIndex*PO_PAGE_POLARITY_CTRL_COUNT + 2, 
                    pSmvRecChanInfo->strChannelDesc.c_str());
            }
            else
            {
                strTitle.Format(_T("%d:"), 
                    m_nCurPageIndex*PO_PAGE_POLARITY_CTRL_COUNT + 2);
            }
            dcMem.DrawText(strTitle, DSM_POLARITY_RC_CHEN_TITLE1, DT_CENTER|DT_VCENTER| DT_NOPREFIX | DT_SINGLELINE);
        }
        if (nCurPageChannelCount >=3)
        {
            pSmvRecChanInfo = m_pCurrentCtrlBlk->pRecvConfig->GetOneChannelInfo(m_nCurPageIndex*4 + 2);
            if (pSmvRecChanInfo)
            {
                strTitle.Format(_T("%d:%s"), 
                    m_nCurPageIndex*PO_PAGE_POLARITY_CTRL_COUNT + 3, 
                    pSmvRecChanInfo->strChannelDesc.c_str());
            }
            else
            {
                strTitle.Format(_T("%d:"), 
                    m_nCurPageIndex*PO_PAGE_POLARITY_CTRL_COUNT + 3);
            }
            dcMem.DrawText(strTitle, DSM_POLARITY_RC_CHEN_TITLE2, DT_CENTER|DT_VCENTER| DT_NOPREFIX | DT_SINGLELINE);
        }
        if (nCurPageChannelCount >=4)
        {
            pSmvRecChanInfo = m_pCurrentCtrlBlk->pRecvConfig->GetOneChannelInfo(m_nCurPageIndex*4 + 3);
            if (pSmvRecChanInfo)
            {
                strTitle.Format(_T("%d:%s"), 
                    m_nCurPageIndex*PO_PAGE_POLARITY_CTRL_COUNT + 4, 
                    pSmvRecChanInfo->strChannelDesc.c_str());
            }
            else
            {
                strTitle.Format(_T("%d:"), 
                    m_nCurPageIndex*PO_PAGE_POLARITY_CTRL_COUNT + 4);
            }
            dcMem.DrawText(strTitle, DSM_POLARITY_RC_CHEN_TITLE3, DT_CENTER|DT_VCENTER| DT_NOPREFIX | DT_SINGLELINE);
        }
    }


    dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, SRCCOPY);
    
    dcMem.SetTextColor(crOldColor);
    dcMem.SelectObject(pOldBrush);
    dcMem.SelectObject(hFont);
    dcMem.SetBkMode(nOldBkMode);
    dcMem.SelectObject(pOldbmp);
    brTitleBk.DeleteObject();
    brBk.DeleteObject();
    dcMem.DeleteDC();

}

void  CSmvPolarityDlg::_createReport()
{
    CRect rcClient;
    GetClientRect(rcClient);

    CPoint ptCenter(rcClient.CenterPoint());

    m_polarityDialCtrl0.Create(_T("0"), WS_CHILD | WS_VISIBLE|WS_TABSTOP , 
        DSM_POLARITY_RC_POLAR0_CTRL, this, IDC_SMV_POLARITY_CTRL0);
    m_polarityDialCtrl1.Create(_T("1"), WS_CHILD | WS_VISIBLE|WS_TABSTOP , 
        DSM_POLARITY_RC_POLAR1_CTRL, this, IDC_SMV_POLARITY_CTRL1);
    m_polarityDialCtrl2.Create(_T("2"), WS_CHILD | WS_VISIBLE|WS_TABSTOP , 
        DSM_POLARITY_RC_POLAR2_CTRL, this, IDC_SMV_POLARITY_CTRL2);
    m_polarityDialCtrl3.Create(_T("3"), WS_CHILD | WS_VISIBLE|WS_TABSTOP , 
        DSM_POLARITY_RC_POLAR3_CTRL, this, IDC_SMV_POLARITY_CTRL3);

    HBITMAP hBmpDial = dsm_bmpmgr::Get(DSM_BMP_POLARITY_DAIL);
    m_bmpDail.Attach(hBmpDial);
    HBITMAP hBmpPoint = dsm_bmpmgr::Get(DSM_BMP_POLARITY_POINT);
    m_bmpPoint.Attach(hBmpPoint);
    //
    //创建显示通道的四个极性控件
    //
    m_polarityDialCtrl0.ModifyStyle(0,SS_OWNERDRAW);
    m_polarityDialCtrl0.SetBkgndBmp(&m_bmpDail);
    m_polarityDialCtrl0.SetHandBmp(&m_bmpPoint);
    m_polarityDialCtrl0.SetStdDialInRadii(DSM_POLARITY_RC_DAIL_IN_RADIUS);
    m_polarityDialCtrl0.SetStdDialHandRadii(DSM_POLARITY_RC_DAIL_HAND_RADIUS);
    m_polarityDialCtrl0.SetStdDialCircleCentre(DSM_POLARITY_RC_DAIL_CIRCLE_CENTRE);
    m_polarityDialCtrl0.SetDialDegree(DSM_POLARITY_RC_DAIL_DEGREE_START, DSM_POLARITY_RC_DAIL_DEGREE_END);
    m_polarityDialCtrl0.SetStdDialRect(DSM_POLARITY_RC_POLAR_STD_BMP);

    m_polarityDialCtrl0.Init();
    m_polarityDialCtrl0.EraseHand();
    m_polarityDialCtrl0.DrawHand(0, 50);
    m_polarityDialCtrl0.ShowWindow(TRUE);

    m_polarityDialCtrl1.ModifyStyle(0,SS_OWNERDRAW);
    m_polarityDialCtrl1.SetBkgndBmp(&m_bmpDail);
    m_polarityDialCtrl1.SetHandBmp(&m_bmpPoint);
    m_polarityDialCtrl1.SetStdDialInRadii(DSM_POLARITY_RC_DAIL_IN_RADIUS);
    m_polarityDialCtrl1.SetStdDialHandRadii(DSM_POLARITY_RC_DAIL_HAND_RADIUS);
    m_polarityDialCtrl1.SetStdDialCircleCentre(DSM_POLARITY_RC_DAIL_CIRCLE_CENTRE);
    m_polarityDialCtrl1.SetDialDegree(DSM_POLARITY_RC_DAIL_DEGREE_START, DSM_POLARITY_RC_DAIL_DEGREE_END);
    m_polarityDialCtrl1.SetStdDialRect(DSM_POLARITY_RC_POLAR_STD_BMP);
    m_polarityDialCtrl1.Init();
    m_polarityDialCtrl1.EraseHand();
    m_polarityDialCtrl1.DrawHand(0, 50);
    m_polarityDialCtrl1.ShowWindow(TRUE);

    m_polarityDialCtrl2.ModifyStyle(0,SS_OWNERDRAW);
    m_polarityDialCtrl2.SetBkgndBmp(&m_bmpDail);
    m_polarityDialCtrl2.SetHandBmp(&m_bmpPoint);
    m_polarityDialCtrl2.SetStdDialInRadii(DSM_POLARITY_RC_DAIL_IN_RADIUS);
    m_polarityDialCtrl2.SetStdDialHandRadii(DSM_POLARITY_RC_DAIL_HAND_RADIUS);
    m_polarityDialCtrl2.SetStdDialCircleCentre(DSM_POLARITY_RC_DAIL_CIRCLE_CENTRE);
    m_polarityDialCtrl2.SetDialDegree(DSM_POLARITY_RC_DAIL_DEGREE_START, DSM_POLARITY_RC_DAIL_DEGREE_END);
    m_polarityDialCtrl2.SetStdDialRect(DSM_POLARITY_RC_POLAR_STD_BMP);
    m_polarityDialCtrl2.Init();
    m_polarityDialCtrl2.EraseHand();
    m_polarityDialCtrl2.DrawHand(0, 50);
    m_polarityDialCtrl2.ShowWindow(TRUE);

    m_polarityDialCtrl3.ModifyStyle(0,SS_OWNERDRAW);
    m_polarityDialCtrl3.SetBkgndBmp(&m_bmpDail);
    m_polarityDialCtrl3.SetHandBmp(&m_bmpPoint);
    m_polarityDialCtrl3.SetStdDialInRadii(DSM_POLARITY_RC_DAIL_IN_RADIUS);
    m_polarityDialCtrl3.SetStdDialHandRadii(DSM_POLARITY_RC_DAIL_HAND_RADIUS);
    m_polarityDialCtrl3.SetStdDialCircleCentre(DSM_POLARITY_RC_DAIL_CIRCLE_CENTRE);
    m_polarityDialCtrl3.SetDialDegree(DSM_POLARITY_RC_DAIL_DEGREE_START, DSM_POLARITY_RC_DAIL_DEGREE_END);
    m_polarityDialCtrl3.SetStdDialRect(DSM_POLARITY_RC_POLAR_STD_BMP);
    m_polarityDialCtrl3.Init();
    m_polarityDialCtrl3.EraseHand();
    m_polarityDialCtrl3.DrawHand(0, 50);
    m_polarityDialCtrl3.ShowWindow(TRUE);
}

BOOL CSmvPolarityDlg::_setChannelValue(int nChanIndex, CDataPolarityChannelInfo* pPolarityChanInfo)
{
    if (pPolarityChanInfo == NULL)
    {
        return FALSE;
    }
    //
    //设置对应通道的值
    //
    if (nChanIndex >= m_nChannelCount)
    {
        return FALSE;
    }
    
    int nCtrlIndex = nChanIndex % PO_PAGE_POLARITY_CTRL_COUNT;

    ASSERT(nCtrlIndex >= 0 && nCtrlIndex <= 3);
    if (nCtrlIndex < 0 || nCtrlIndex > 3)
    {
        return FALSE;
    }
    
    //
    //如果通道值 < 0 则为负极性，== 0 则为中性， > 0 则为正极性 
    //
    float fValue = 0.0f;
    
    if (pPolarityChanInfo->m_nPolarity < 0)
    {
        fValue = 0.0f;
    }
    else if (pPolarityChanInfo->m_nPolarity >0)
    {
        fValue = 100.0f;
    }
    else
    {
        fValue = 50.0f;
    }

    switch (nCtrlIndex)
    {
    case 0:
        {
            m_polarityDialCtrl0.EraseHand();
            m_polarityDialCtrl0.DrawHand(0, fValue);
            m_polarityDialCtrl0.Invalidate();
        }
        break;
    case 1:
        {
            m_polarityDialCtrl1.EraseHand();
            m_polarityDialCtrl1.DrawHand(0, fValue);
            m_polarityDialCtrl1.Invalidate();
        }
        break;
    case 2:
        {
            m_polarityDialCtrl2.EraseHand();
            m_polarityDialCtrl2.DrawHand(0, fValue);
            m_polarityDialCtrl2.Invalidate();
        }
        break;
    case 3:
        {
            m_polarityDialCtrl3.EraseHand();
            m_polarityDialCtrl3.DrawHand(0, fValue);
            m_polarityDialCtrl3.Invalidate();
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

void CSmvPolarityDlg::_updateChannelCtrl()
{
    //
    //更新当前页应该显示的通道
    //
    int nCurPageChannelCount(0);
    nCurPageChannelCount = (((m_nCurPageIndex+1) * PO_PAGE_POLARITY_CTRL_COUNT) <= m_nChannelCount) ? \
                            PO_PAGE_POLARITY_CTRL_COUNT : (m_nChannelCount - m_nCurPageIndex*PO_PAGE_POLARITY_CTRL_COUNT);
    
    if (nCurPageChannelCount >=1)
    {
        m_polarityDialCtrl0.ShowWindow(SW_SHOW);
    }
    else
    {
        m_polarityDialCtrl0.ShowWindow(SW_HIDE);
    }
    
    if (nCurPageChannelCount >=2)
    {
        m_polarityDialCtrl1.ShowWindow(SW_SHOW);
    }
    else
    {
        m_polarityDialCtrl1.ShowWindow(SW_HIDE);
    }

    if (nCurPageChannelCount >=3)
    {
        m_polarityDialCtrl2.ShowWindow(SW_SHOW);
    }
    else
    {
        m_polarityDialCtrl2.ShowWindow(SW_HIDE);
    }

    if (nCurPageChannelCount >=4)
    {
        m_polarityDialCtrl3.ShowWindow(SW_SHOW);
    }
    else
    {
        m_polarityDialCtrl3.ShowWindow(SW_HIDE);
    }
    Invalidate(TRUE);
}

void CSmvPolarityDlg::_updatePolarityValue()
{
    ASSERT(m_pDataLayer);

    if (_checkChannel())
    {
        return;
    }
    if (!m_pDataLayer->GetPolarityData(&m_dataPolarity))
    {
        return;
    }

    CDataPolarityChannelInfo* pPolarityChanInfo = NULL;

    for(int i = m_nCurPageIndex*PO_PAGE_POLARITY_CTRL_COUNT; i < (m_nCurPageIndex + 1)*PO_PAGE_POLARITY_CTRL_COUNT; ++i)
    {
        if (i < m_nChannelCount)
        {
            pPolarityChanInfo = m_dataPolarity.GetOneChannelInfo(i);
            _setChannelValue(i, pPolarityChanInfo);
        }
    }

}

bool CSmvPolarityDlg::_switchCtrlBlk(UINT nCmdID)
{
    bool bRet = __super::_switchCtrlBlk(nCmdID);
    if (bRet)
    {
        m_nChannelCount = m_pCurrentCtrlBlk->nChannelCount;

        //
        //调整当前页显示的通道，如果当前页显示通道索引的超过总通道数，则设置为从通道0开始
        //
        int nPageCount = m_nChannelCount/PO_PAGE_POLARITY_CTRL_COUNT;
        if (m_nChannelCount%PO_PAGE_POLARITY_CTRL_COUNT > 0)
        {
            ++nPageCount;
        }
        if (m_nCurPageIndex >= nPageCount)
        {
            m_nCurPageIndex = 0;
        }
        _updateChannelCtrl();
    }
    return bRet;
}
void CSmvPolarityDlg::OnMenuF3()
{
    if(!m_bChannelChange)
    {
        m_pDataLayer->ResetAllPorlarity();
    }
}

#if SU_FEI
void CSmvPolarityDlg::OnMenuF5()
{
    CEventProc::GetInstance()->ShowPage(IDD_SMV_POLARITY_CFG_DLG);
}
#else
/************************************************************************/
/* 添加极性参数（额定值）功能键                                                                     */
/************************************************************************/
void CSmvPolarityDlg::OnMenuF4()
{
	CEventProc::GetInstance()->ShowPage(IDD_SMV_POLARITY_CFG_DLG);
}



#endif
void CSmvPolarityDlg::_modifyMenu()
{
    //
    __super::_modifyMenu();
    //
    //修改F3 为 重新开始
    //
    m_menuMain.SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_POLARITY_RESTART));

#if !SU_FEI
	m_menuMain.SetMenuItemCaption(3, L"整定值");
#endif


	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_POLARITY_CAPTION));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);

}
bool CSmvPolarityDlg::_checkChannel()
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

            // 隐藏『暂停开始』菜单标题
#if SU_FEI
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, L"");
			m_pNaviBar->UpdateNaviMenu();
#endif
        }

        return true;
    }
    else
    {
        if(m_bChannelChange)
        {
            m_bChannelChange = false;
            CStatusMgr::SetTips(L"", 300);

            // 恢复暂停开始菜单标题
#if	SU_FEI 
			m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_POLARITY_RESTART));
            m_pNaviBar->UpdateNaviMenu();
#endif
        }
    }

    return false;
}

void CSmvPolarityDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == m_nTimerID)
    {
        _updatePolarityValue();
    }

    DSM_SMVBASEDLG::OnTimer(nIDEvent);
}

BOOL CSmvPolarityDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    
    if (pMsg->message == WM_KEYDOWN)
    {
        switch (pMsg->wParam)
        {
        case VK_UP:
        case VK_LEFT:
            {
                //
                //通道翻页，显示前一页的通道
                //
                if (m_nCurPageIndex > 0)
                {
                    --m_nCurPageIndex;
                    _updateChannelCtrl();
                }
                return TRUE;
            }
        case VK_DOWN:
        case VK_RIGHT:
            {
                //
                //通道翻页，显示后一页的通道
                //
                if ((m_nCurPageIndex + 1)*PO_PAGE_POLARITY_CTRL_COUNT < m_nChannelCount)
                {
                    ++m_nCurPageIndex;
                    _updateChannelCtrl();
                }
                return TRUE;
            }

        default:
            break;
        }
    }

    
    return DSM_SMVBASEDLG::PreTranslateMessage(pMsg);
}

BOOL CSmvPolarityDlg::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_bmpDail.Detach();
    m_bmpPoint.Detach();

    return CSMVBaseDlg::DestroyWindow();
}

BOOL CSmvPolarityDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    return TRUE;
}

void CSmvPolarityDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    CCtrlBlkDataPolarity* pData = dynamic_cast<CCtrlBlkDataPolarity*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
    if(pData == NULL)
    {
        pData = new CCtrlBlkDataPolarity();
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);
    }
    ASSERT(pData);

    if(pData)
    {
        pData->m_nCurPageIndex = m_nCurPageIndex;

    }
}

bool CSmvPolarityDlg::_restorePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);
    ASSERT(CSmvPolarityDlg::IDD == m_pPageDataExtra->GetPageData()->m_nPageID);

    // 获取当前的控制块索引
    CCtrlBlkDataPolarity* pData = dynamic_cast<CCtrlBlkDataPolarity*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // 当前控制块若没绑定相关信息，则返回
    if(!pData)
    {
        return false;
    }
    
    m_nCurPageIndex = pData->m_nCurPageIndex;

    return true;
}
