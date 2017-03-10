// ElTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ReportControlDemo.h"
#include "ElTestDlg.h"


#define EL_ID_REPORT_CONTROL  1000
#define EL_ID_MENU_CONTROL    1002


// CElTestDlg �Ի���

IMPLEMENT_DYNAMIC(CElTestDlg, CDialog)

CElTestDlg::CElTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CElTestDlg::IDD, pParent)
{

    m_hdrFont.CreateFont(ELT_T1_TEXTLFHEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_COMPAT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"΢���ź�");

    m_bodyFont.CreateFont(ELT_T1_TEXTLFHEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_COMPAT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"΢���ź�");

    m_menuFont.CreateFont(MBT_TEXTLFHEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"΢���ź�");


    m_bmp3.Attach(::SHLoadDIBitmap(L"\\FlashDisk2\\bmp\\3.bmp"));
    m_bmp4.Attach(::SHLoadDIBitmap(L"\\FlashDisk2\\bmp\\4.bmp"));
    m_defMenuBkImage.Attach(::SHLoadDIBitmap(L"\\FlashDisk2\\defmenubk.bmp"));


    
    // ����Inplace�ؼ�ר�ò˵�, ���ڵ����ؼ�ʱװ��
    CMenu  mm;
    mm.LoadMenu(IDR_MENU_EDIT);
    m_inplaceEditMenu.LoadMenu(&mm);
    mm.DestroyMenu();
    mm.LoadMenu(IDR_MENU_LIST);
    m_inplaceListMenu.LoadMenu(&mm);
    m_lastMenu = NULL;
}

CElTestDlg::~CElTestDlg()
{
}

void CElTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CElTestDlg, CDialog)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTON1, &CElTestDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CElTestDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, &CElTestDlg::OnBnClickedButton3)
    ON_NOTIFY(ELNM_INPLACEBUTTONDOWN, EL_ID_REPORT_CONTROL, OnInplaceButtonClick)
    ON_NOTIFY(ELNM_ITEM_AFTERLISTDROPDOWN, EL_ID_REPORT_CONTROL, OnInplaceListAction)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, EL_ID_REPORT_CONTROL, OnInplaceListAction)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, EL_ID_REPORT_CONTROL, OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, EL_ID_REPORT_CONTROL, OnInplaceEditAction)

    ON_COMMAND(ID_BTN_SCOPE, &CElTestDlg::OnBtnScope)
    ON_UPDATE_COMMAND_UI(ID_BTN_SCOPE, &CElTestDlg::OnUpdateBtnScope)
END_MESSAGE_MAP()


// CElTestDlg ��Ϣ�������

void  CElTestDlg::InitReportCtrl ()
{
    // �������ڶ���
    m_ctlListEL.Create(
        WS_VISIBLE|WS_CHILD|WS_TABSTOP|LVS_REPORT|LVS_SINGLESEL,
        CRect(5, 5, 605, 355), this, EL_ID_REPORT_CONTROL);

    // ��������
    m_ctlListEL.SetFont (&m_bodyFont);
    m_ctlListEL.SetHeaderFont (&m_hdrFont);                             // ������SetFont����, ���򱻸���
    ASSERT(m_ctlListEL.GetFont() == &m_bodyFont);
    ASSERT(m_ctlListEL.GetHeaderFont() == &m_hdrFont);

    // ����checkboxͼƬ
    m_ctlListEL.SetCheckImage(&m_bmp3, NULL);

    m_ctlListEL.PostInitTemplateStyle();


    // ������Ԫ��
    m_ctlListEL.InsertColumn(0,_T("����"), EL_CENTER, 100);
    m_ctlListEL.InsertColumn(1,_T("���"), EL_LEFT, 100);
    int  col2 = m_ctlListEL.InsertColumn(2,_T("����"), EL_CENTER, 100);
    int  col3 = m_ctlListEL.InsertColumn(3,_T("����ʱ��"), EL_LEFT, 300, FALSE, EL_RIGHT);


    // ����ѡ�񽹵���ʽ
    m_ctlListEL.SetFreezeSelectionVisible(0);
    m_ctlListEL.SetFreezeColumnsCount(2);
    m_ctlListEL.SetSubItemFocusEnable(1);
    m_ctlListEL.SetRowSelectionVisible(FALSE);
    m_ctlListEL.SetRowFocusEnable(TRUE);


    // ��������
    for (int i = 0; i < 22; i++)
    {
        CString  str;
        str.Format(L"%d", i);
        int  nItem = m_ctlListEL.InsertRecordRow(i, str);
        m_ctlListEL.SetRowItemText(nItem, 1, L"178CM");
        m_ctlListEL.SetRowItemText(nItem, 2, L"70KG");
        m_ctlListEL.SetRowItemText(nItem, 3, L"2009��1��15��23ʱ40��");

        // ��ֹ�б�����
        if (i >=5 && i<=11)
        {
            m_ctlListEL.GetRecordRow(i)->SetFocusable(FALSE);
        }
    }
    // ѡ���һ��
    m_ctlListEL.SetFocusedSelectionRowIndex(0);
    m_ctlListEL.SetFocusedColIndex(2);

    // ����Զ����item
    CELRecord*  rec1 = new CELRecord();
    rec1->AddItem(new CELRecordItemText(L"�˺�"));
    rec1->AddItem(new CELRecordItemText(L"����"));
    rec1->AddItem(new CELRecordItemNumber(12.4, L"ky=%f"));
    rec1->AddItem(new CELRecordItemText(L"ëë��"))->SetFormatString(L"my=%s");
    int  customRow = m_ctlListEL.InsertRecordRow(m_ctlListEL.GetRecordRowCount(), rec1);



    // ��ֹĳ��item������
    m_ctlListEL.GetRecordRow(3)->GetItem(2)->SetFocusable(FALSE);

    // checkbox����, item����editable
    m_ctlListEL.GetRecordRow(2)->GetItem(3)->SetEditable(1);
    m_ctlListEL.GetRecordRow(2)->GetItem(3)->HasCheckbox(1);

    m_ctlListEL.GetRecordRow(4)->GetItem(2)->SetEditable(1);
    m_ctlListEL.GetRecordRow(4)->GetItem(2)->HasCheckbox(1);

    m_ctlListEL.GetRecordRow(4)->GetItem(0)->SetEditable(1);
    m_ctlListEL.GetRecordRow(4)->GetItem(0)->HasCheckbox(1);

    // inplace-edit����
    m_ctlListEL.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(1);
    m_ctlListEL.GetColumnExAttr(3)->GetEditOptions()->SetAllowEdit(1);
    m_ctlListEL.GetRecordRow(0)->GetItem(3)->SetEditable(1);
    m_ctlListEL.GetRecordRow(customRow)->GetItem(2)->SetEditable(1);

    // ���inplece��ť(��չ)
    m_ctlListEL.GetColumnExAttr(col2)->GetEditOptions()->AddExpandButton();
    m_ctlListEL.GetColumnExAttr(col3)->GetEditOptions()->AddComboButton();
    m_ctlListEL.GetRecordRow(1)->GetItem(3)->GetEditOptions(NULL)->AddExpandButton();  // һ����item��������EditOptions, ��item�Ͳ�����col��EditOptions
    m_ctlListEL.GetRecordRow(1)->GetItem(3)->SetEditable(1);

    // ���inplace combo ��ť
    m_ctlListEL.GetRecordRow(3)->GetItem(3)->GetEditOptions(NULL)->SetAllowEdit(0);  // һ����item��������EditOptions, ��item�Ͳ�����col��EditOptions
    m_ctlListEL.GetRecordRow(3)->GetItem(3)->GetEditOptions(NULL)->AddComboButton();
    m_ctlListEL.GetRecordRow(3)->GetItem(3)->SetEditable(1);
    m_ctlListEL.GetRecordRow(3)->GetItem(3)->GetEditOptions(NULL)->AddConstraint(L"aaaa", 1);
    m_ctlListEL.GetRecordRow(3)->GetItem(3)->GetEditOptions(NULL)->AddConstraint(L"bbbb", 2);
    m_ctlListEL.GetRecordRow(3)->GetItem(3)->GetEditOptions(NULL)->AddConstraint(L"ccc", 3);


    m_ctlListEL.GetRecordRow(12)->GetItem(3)->GetEditOptions(NULL)->SetAllowEdit(0);  // һ����item��������EditOptions, ��item�Ͳ�����col��EditOptions
    m_ctlListEL.GetRecordRow(12)->GetItem(3)->GetEditOptions(NULL)->AddComboButton();
    m_ctlListEL.GetRecordRow(12)->GetItem(3)->SetEditable(1);
    m_ctlListEL.GetRecordRow(12)->GetItem(3)->GetEditOptions(NULL)->AddConstraint(L"�ҵ�", 1);
    m_ctlListEL.GetRecordRow(12)->GetItem(3)->GetEditOptions(NULL)->AddConstraint(L"���", 2);
    m_ctlListEL.GetRecordRow(12)->GetItem(3)->GetEditOptions(NULL)->AddConstraint(L"�л���", 3);
    m_ctlListEL.GetRecordRow(12)->GetItem(3)->GetEditOptions(NULL)->AddConstraint(L"�л���", 3);
    m_ctlListEL.GetRecordRow(12)->GetItem(3)->GetEditOptions(NULL)->AddConstraint(L"�л���", 3);
    m_ctlListEL.GetRecordRow(12)->GetItem(3)->GetEditOptions(NULL)->AddConstraint(L"�л���", 3);
    m_ctlListEL.GetRecordRow(12)->GetItem(3)->GetEditOptions(NULL)->AddConstraint(L"�л���", 3);
    m_ctlListEL.GetRecordRow(12)->GetItem(3)->GetEditOptions(NULL)->AddConstraint(L"�л���", 3);
    m_ctlListEL.GetRecordRow(12)->GetItem(3)->GetEditOptions(NULL)->AddConstraint(L"�л���", 3);
    m_ctlListEL.GetRecordRow(12)->GetItem(3)->GetEditOptions(NULL)->AddConstraint(L"�л���", 3);
    m_ctlListEL.GetRecordRow(12)->GetItem(3)->GetEditOptions(NULL)->AddConstraint(L"�л���", 3);
    m_ctlListEL.GetRecordRow(12)->GetItem(3)->GetEditOptions(NULL)->AddConstraint(L"�л���", 3);

}


void  CElTestDlg::InitMenuCtrl()
{
    m_ctlMenu.Create(107, 6, CRect(0, 355, 0+640, 355+36), this, EL_ID_MENU_CONTROL, TRUE);

    // ������ʽ
    m_ctlMenu.SetHasGridline(0, 1);
    m_ctlMenu.SetBkColor(RGB(0x21, 0x5D, 0x84));
    //m_ctlMenu.SetBkColor(RGB(255, 255, 255));
    //m_ctlMenu.SetTextColor(RGB(255, 255, 255));
    m_ctlMenu.SetFont(&m_menuFont);
    m_ctlMenu.SetDefaultItemBkImage(NULL);

    // ���ò˵�������Ŀ�괰��
    CMenu  tempMenu;

    tempMenu.LoadMenu(IDR_MENU1);
    m_menu.LoadMenu(&tempMenu);
    //m_menu.GetSubMenu(0)->HighlightMenuItem(2, TRUE);
    

    tempMenu.DestroyMenu();
    tempMenu.LoadMenu(IDR_MENU2);
    m_menu2.LoadMenu(&tempMenu);

    m_ctlMenu.SetCommandTarget(this);
    m_ctlMenu.SetNaviMenu(&m_menu);
    m_ctlMenu.SetDefaultItemBkImage(&m_defMenuBkImage);
}


BOOL CElTestDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    // �������ڴ�С
    MoveWindow(0, 0, 640, 480, FALSE);

    // ��ʼ������ؼ�
    InitReportCtrl();

    // ��ʼ���˵��ؼ�
    InitMenuCtrl();



    // ��ʱˢ������
    //SetTimer(1, 300, NULL);


    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}


void CElTestDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    static int  counter = 0;

    CString  str1;
    CString  str2;
    str1.Format(L"%d.%d", counter, counter);
    str2.Format(L"%d.%d", counter+1, counter+2);

    //m_ctlListEL.SetRedraw(FALSE);               // ����ˢ��


    for (int i = 0; i < 10 ; i++)
    {
        m_ctlListEL.SetRowItemText(i, 1, str1);
        m_ctlListEL.SetRowItemText(i, 3, str2);
    }

    //m_ctlListEL.SetRedraw(TRUE);

    counter++;


    CDialog::OnTimer(nIDEvent);
}

void CElTestDlg::OnBnClickedButton1()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    /*
    KillTimer(1);
    m_ctlListEL.DeleteRecordRow(m_ctlListEL.GetRecordRowCount()-1);
    m_ctlListEL.DeleteAllRecordRow();
    */

    if (m_ctlMenu.GetNaviMenu() == &m_menu)
    {
        m_ctlMenu.SetNaviMenu(&m_menu2);
    }
    else
    {
        m_ctlMenu.SetNaviMenu(&m_menu);
    }
}

void CElTestDlg::OnBnClickedButton2()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    m_ctlListEL.ScrollPageUp();
    //m_ctlListEL.ScollRowUp();
}

void CElTestDlg::OnBnClickedButton3()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    m_ctlListEL.ScrollPageDown();
    //m_ctlListEL.ScollRowDown();
}

void CElTestDlg::OnBtnScope()
{
    AfxMessageBox(L"hello");
}
void CElTestDlg::OnUpdateBtnScope(CCmdUI *pCmdUI)
{
    MB_DEBUGLOG(L"OnUpdateBtnScope\n");

    pCmdUI->Enable(1);

}

BOOL CElTestDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���

    // ���������˵�, ����û��Ҫ�������Ϣ
    if (m_ctlMenu.PreTranslateMessage(pMsg))
    {
        return TRUE;
    }

    if (pMsg->hwnd == m_ctlListEL.GetSafeHwnd())
    {
        if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
        {
            AfxMessageBox(L"ok");
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}


// ����ģ̬���ڹ���������ֵ, �����Ұ���
class CProgressSlider : public CStatic
{
    int                     m_min;
    int                     m_max;
    int                     m_val;
    COLORREF                m_bkcolor;
    COLORREF                m_fgcolor;
    COLORREF                m_textcolor;

public:
    CProgressSlider()
    {
        m_min = 0;
        m_max = 0;
        m_val = 0;
        m_bkcolor = RGB(255, 255, 255);
        m_fgcolor = RGB(0, 255, 0);
        m_textcolor = RGB(0, 0, 255);
    }
    virtual ~CProgressSlider()
    {
    }

    void SetProgressColor (COLORREF bkcolor, COLORREF fgcolor, COLORREF textcolor)
    {
        if (bkcolor != -1)
        {
            m_bkcolor = bkcolor;
        }
        if (fgcolor != -1)
        {
            m_fgcolor = fgcolor;
        }
        if (textcolor != -1)
        {
            m_textcolor = textcolor;
        }
    }
    int GetVal()
    {
        return m_val;
    }

public:
    int GoModal(const CRect& rcItem, CWnd* pParentWnd, int val, int min, int max)
    {
        if (::IsWindow(m_hWnd)) 
        {
            ASSERT(FALSE);
            return FALSE;
        }

        // ��ʼ��
        m_min = min;
        m_max = max;
        m_val = val;

        // ��������
        BOOL bCreate = CStatic::Create(L"", WS_CHILD|WS_VISIBLE, rcItem, pParentWnd);
        ASSERT(bCreate);

        // Enable this window
        EnableWindow(TRUE);
        CWnd* pFocusWnd = SetFocus();
        SetCapture();


        m_nFlags |= WF_CONTINUEMODAL;
        int nResult = m_nModalResult;
        if (ContinueModal())
        {
            // enter modal loop
            DWORD dwFlags = MLF_SHOWONIDLE;
            if (GetStyle() & DS_NOIDLEMSG)
            {
                dwFlags |= MLF_NOIDLEMSG;
            }
            nResult = RunModalLoop(dwFlags);
        }

        ReleaseCapture();
        DestroyWindow();

        if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd()))
        {
            pFocusWnd->SetFocus();
        }

        return (nResult == IDOK);
    }

protected:
    DECLARE_MESSAGE_MAP()


    UINT OnGetDlgCode()
    {
        return DLGC_WANTALLKEYS;
    }

    BOOL OnEraseBkgnd(CDC* pDC)
    {
        return TRUE;
    }

    void OnPaint()
    {
        CPaintDC dc(this); // device context for painting

        // ��䱳��
        CRect clientRect;
        GetClientRect(&clientRect);
        

        // ���ǰ��
        CRect fgRect = clientRect;
        CRect bkRect = clientRect;
        int width = (int)(double(m_val)*clientRect.Width() / double(m_max - m_min));
        fgRect.right = fgRect.left + width;
        bkRect.left = bkRect.left + width;
        dc.FillSolidRect(fgRect, m_fgcolor);
        dc.FillSolidRect(bkRect, m_bkcolor);

        // �������
        CString str;
        str.Format(L"%d", m_val);
        dc.SetTextColor(m_textcolor);
        dc.SetBkMode(TRANSPARENT);
        dc.DrawText(str, clientRect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

    }

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        if (nChar == VK_LEFT)
        {
            if ((m_val-1) > m_min)
            {
                m_val = m_val - 1;
            }
            else
            {
                m_val = m_min;
            }
            Invalidate();
        }
        else if (nChar == VK_RIGHT)
        {
            if ((m_val+1) < m_max)
            {
                m_val = m_val + 1;
            }
            else
            {
                m_val = m_max;
            }
            Invalidate();
        }
        else if (nChar == VK_ESCAPE)
        {
            EndModalLoop(IDCANCEL);
        }
        else if (nChar == VK_RETURN)
        {
            EndModalLoop(IDOK);
        }
        else
        {
            CStatic::OnKeyDown(nChar, nRepCnt, nFlags);
        }
    }

    void OnCaptureChanged(CWnd *pWnd)    
    {   
        EndModalLoop(IDCANCEL);
        CStatic::OnCaptureChanged(pWnd);   
    }   
};
BEGIN_MESSAGE_MAP(CProgressSlider, CStatic)
    ON_WM_GETDLGCODE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_KEYDOWN()
    ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()


int DoModal_ProgressSlider(int& retval, const CRect& rcItem, CWnd* pParentWnd, 
                           int val, int min, int max, 
                           COLORREF bkcolor = -1, COLORREF fgcolor = -1, COLORREF textcolor = -1)
{
    CProgressSlider wndPS;
    wndPS.SetProgressColor(bkcolor, fgcolor, textcolor);

    int dlgret = wndPS.GoModal(rcItem, pParentWnd, val, min, max);
    if (dlgret == IDOK)
    {
        retval = wndPS.GetVal();
    }
    else
    {
        retval = val;
    }

    return dlgret;
}



void CElTestDlg::OnInplaceButtonClick(NMHDR * pNotifyStruct, LRESULT* pResult)
{
    ELNM_INPLACEBUTTON_ARGS*  pItemNotify = (ELNM_INPLACEBUTTON_ARGS*) pNotifyStruct;
    ASSERT(pItemNotify->pButton);

    // ����ģ̬���ڹ�������item��ֵ, �����Ұ���
    if (pItemNotify->pButton->GetID() == EL_BTNTYPE_EXPANDBUTTON && pItemNotify->rowIndex < 10)
    {
        CString  str = m_ctlListEL.GetRowItemText(pItemNotify->rowIndex, pItemNotify->colIndex);

        int  retval;
        int  val;
        swscanf(str, L"%d", &val);
        int  dlgret = DoModal_ProgressSlider(retval, 
            m_ctlListEL.GetRowItemRect(pItemNotify->rowIndex, pItemNotify->colIndex), &m_ctlListEL, 
            val, 0, 100);
        if (dlgret == IDOK)
        {
            str.Format(L"%d", retval);
            m_ctlListEL.SetRowItemText(pItemNotify->rowIndex, pItemNotify->colIndex, str);
        }

        *pResult = TRUE;  // ���߿ؼ����Ѿ������˸���Ϣ
        return;
    }
    
    *pResult = FALSE;  // ���߿ؼ��һ�û�д������Ϣ
}

void CElTestDlg::OnInplaceListAction(NMHDR * pNotifyStruct, LRESULT* pResult)
{
    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    if (pItemNotify->hdr.code == ELNM_ITEM_AFTERLISTDROPDOWN)
    {
        //  inplaceEdit����ʱ, �л��˵�
        m_lastMenu = m_ctlMenu.GetNaviMenu();
        m_ctlMenu.SetNaviMenu(&m_inplaceListMenu);
        


        // ��ȡѡ����
        ASSERT(pItemNotify->pInplaceList);
        int  sel = pItemNotify->pInplaceList->GetCurSel();
        if (sel != LB_ERR)
        {
            CString  str;
            pItemNotify->pInplaceList->GetText(sel, str);
            EL_DEBUGLOG(L"ELNM_ITEM_AFTERLISTDROPDOWN sel=%s\n", str);
        }
    }
    else if (pItemNotify->hdr.code == ELNM_ITEM_CONSTRAINTCHANGED)
    {
        // ��ȡѡ����
        if (pItemNotify->pConstraint)
        {
            EL_DEBUGLOG(L"ELNM_ITEM_CONSTRAINTCHANGED sel=%s\n", 
                pItemNotify->pConstraint->GetConstraintCaption());
            
        }
    }
}

void CElTestDlg::OnInplaceEditAction(NMHDR * pNotifyStruct, LRESULT* pResult)
{
    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    // inplaceEdit����ʱ, �л��˵�
    if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
    {
        m_lastMenu = m_ctlMenu.GetNaviMenu();
        m_ctlMenu.SetCommandTarget(m_ctlListEL.GetInplaceEdit());
        m_ctlMenu.SetNaviMenu(&m_inplaceEditMenu);
    }
    else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
    {
        if (m_lastMenu)
        {
            m_ctlMenu.SetNaviMenu(m_lastMenu);
            m_ctlMenu.SetCommandTarget(this);
            m_lastMenu = NULL;
        }
    }
}