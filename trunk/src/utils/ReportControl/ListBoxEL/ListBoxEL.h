#pragma once


#define LISTBOXEL_CLASSNAME  L"ListBoxEL"

#define LB_ASSERTINDEX(i, begin)  ASSERT_BREAK((i) >= (begin) && (i) < GetCount())


// ����
#define LB_DEBUGLOG  NKDbgPrintfW

//
//-----------------------------------------------------------------------------
// list��һ��Item������
//-----------------------------------------------------------------------------
//
struct LB_ItemInfo
{
    CString                 caption;            // item��ʾ���ı�
    DWORD_PTR               data;               // item�������û�data
    int                     cyHeight;           // item����ʾ�߶�, �뵱ǰ����(��һ�����item), ���û�ǿ�������й�(SetItemHeight)

public:
    LB_ItemInfo ()
    {
        //caption
        data = NULL;
        cyHeight = 0;
    }
};
typedef CArray<LB_ItemInfo*, LB_ItemInfo*>  LB_ItemInfoArray;


//
//-----------------------------------------------------------------------------
// CListBoxEL
//
//  ֧���Ի��ListBox, ��ͬ��CListBox��LBS_OWNERDRAWVARIABLE
//-----------------------------------------------------------------------------
//
class CListBoxEL: public CWnd
{
public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CListBoxEL ();
    virtual ~CListBoxEL ();

    virtual BOOL  Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
    virtual BOOL  CreateEx (DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
    void  PostInitTemplateStyle ();

    void  SetFont (CFont* pFont, BOOL bRedraw = TRUE);
    CFont*  GetFont ();

    void  SetTextColor (COLORREF color);
    COLORREF  GetTextColor ();

    void  SetBkColor (COLORREF color);
    COLORREF  GetBkColor ();

    void  SetSelStyle (BOOL bShowSelAlways, COLORREF selHighlightColor, 
        COLORREF textHighlightColor, COLORREF selLowlightColor);

    void  SetMargin (int left, int top, int right, int bottom);


public:
    //
    //=============================================================================
    // ���ݲ���
    //=============================================================================
    //
    int  GetCount ();
    int  AddString (LPCTSTR lpszItem, DWORD_PTR data = NULL);
    int  InsertString (int nIndex, LPCTSTR lpszItem, DWORD_PTR data = NULL);
    int  DeleteString (int nIndex);


    void  ResetContent ();

    void  GetText (int nIndex, CString& rString);
    DWORD_PTR  GetItemData (int nIndex);
    int  SetItemData (int nIndex, DWORD_PTR dwItemData);
    void*  GetItemDataPtr (int nIndex);
    int  SetItemDataPtr (int nIndex, void* pData);

    //
    //=============================================================================
    // �ؼ�����
    //=============================================================================
    //
    void  EnsureVisible (int nIndex, BOOL bPartialOK);
    BOOL  IsItemVisible (int nIndex, BOOL bPartialOK);
    int  GetTopIndex ();
    int  SetTopIndex (int nIndex);
    int  GetCurSel ();
    int  SetCurSel (int nSelect);

    int  GetItemRect (int nIndex, LPRECT lpRect);
    int  SetItemHeight (int nIndex, UINT cyItemHeight);
    int  GetItemHeight (int nIndex);



protected:
    //
    //=============================================================================
    // ��Ϣ����
    //=============================================================================
    //
    DECLARE_MESSAGE_MAP()
    afx_msg UINT  OnGetDlgCode ();
    afx_msg void  OnPaint ();
    afx_msg BOOL  OnEraseBkgnd (CDC* pDC);
    afx_msg void  OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void  OnDestroy ();
    afx_msg void  OnKillFocus (CWnd* pNewWnd);
    afx_msg void  OnSetFocus (CWnd* pOldWnd);

    virtual BOOL  PreTranslateMessage (MSG* pMsg);
    virtual void  PaintNC (CDC* pDC);
    virtual void  FillBkgnd (CDC* pDC);
    virtual void  Paint (CDC* pDC);
    virtual void  DrawItem (LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void  MeasureItem (LPMEASUREITEMSTRUCT lpMeasureItemStruct);


protected:
    //
    //=============================================================================
    // ����
    //=============================================================================
    //
    void  GetMarginClientRect (LPRECT lpRect);
    void  UpdateDefItemHeight (CFont* font);
    void  GetTextMetricsHelper (CFont* font, LPTEXTMETRIC lptm);
    void  UpdateListBox (int nIndex = -1);
    void  CallbackDrawItem (int nIndex, LB_ItemInfo* item, 
        UINT itemAction, UINT itemState, HDC hDC, RECT rcItem);
    void  CallbackMeasureItem (int nIndex, LB_ItemInfo* item);
    static void  AdjustWindowRectInsideScreen(const CRect& surroundRect, IN OUT CRect& retRectFix);
    static BOOL  RegisterWndClass (HINSTANCE hInstance, LPCTSTR lpszClassName, UINT style, HICON hIcon = 0, int bgStockBrush = NULL_BRUSH);
    

protected:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
    //=============================================================================
    //
    LB_ItemInfoArray        m_itemArray;        // item��������
    int                     m_curSelIndex;      // ��ǰ��ѡ�е�item, -1����û��ѡ��, ��ɾ��itemʱҪ����Ӧ����
    int                     m_topIndex;         // ��ǰ����ʾ�ڵ�һ�е�item, Ĭ����0, ��ɾ��itemʱҪ����Ӧ����
    int                     m_defItemHeight;    // ��ǰĬ�ϵ�item��ʾ�߶�, ���ݵ�ǰ��font
    CFont*                  m_font;             // ��ǰʹ�õ�����
    COLORREF                m_textColor;        // ������ɫ
    COLORREF                m_bkColor;          // ������ɫ

    BOOL                    m_bShowSelAlways;   // �Ƿ��ڿؼ���ʧ����ʱ, ����ʾѡ����ĵ�����ɫ
    COLORREF                m_selHighlightColor;    // ѡ���еĸ�����ɫ
    COLORREF                m_textHighlightColor;   // ѡ���еĸ����ı���ɫ
    COLORREF                m_selLowlightColor;     // ѡ���еĵ�����ɫ, �����ڿؼ�ʧȥ����, ����m_bShowSelAlways=1��־ʱ

    int                     m_marginLeft;       // ��߿հ�����, Ĭ��0
    int                     m_marginTop;        // �ϱ߿հ�����, Ĭ��0
    int                     m_marginRight;      // �ұ߿հ�����, Ĭ��0
    int                     m_marginBottom;     // �±߿հ�����, Ĭ��0
};

//
//-----------------------------------------------------------------------------
// �ӳٵ���ʽ��ʼ��
//-----------------------------------------------------------------------------
//
inline void  CListBoxEL::PostInitTemplateStyle ()
{
}
//
//=============================================================================
// ������ɫ����
//=============================================================================
//
inline void  CListBoxEL::SetTextColor (COLORREF color)
{
    m_textColor = color;

    UpdateListBox();
}
inline COLORREF  CListBoxEL::GetTextColor ()
{
    return m_textColor;
}
inline void  CListBoxEL::SetBkColor (COLORREF color)
{
    m_bkColor = color;

    UpdateListBox();
}
inline COLORREF  CListBoxEL::GetBkColor ()
{
    return m_bkColor;
}
inline void  CListBoxEL::SetSelStyle (BOOL bShowSelAlways, COLORREF selHighlightColor, 
                   COLORREF textHighlightColor, COLORREF selLowlightColor)
{
    m_bShowSelAlways = bShowSelAlways;
    m_selHighlightColor = selHighlightColor;
    m_textHighlightColor = textHighlightColor;
    m_selLowlightColor = selLowlightColor;

    UpdateListBox();
}
