#pragma once


#define LISTBOXEL_CLASSNAME  L"ListBoxEL"

#define LB_ASSERTINDEX(i, begin)  ASSERT_BREAK((i) >= (begin) && (i) < GetCount())


// 调试
#define LB_DEBUGLOG  NKDbgPrintfW

//
//-----------------------------------------------------------------------------
// list中一个Item的属性
//-----------------------------------------------------------------------------
//
struct LB_ItemInfo
{
    CString                 caption;            // item显示的文本
    DWORD_PTR               data;               // item关联的用户data
    int                     cyHeight;           // item的显示高度, 与当前字体(第一次添加item), 或用户强制设置有关(SetItemHeight)

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
//  支持自绘的ListBox, 等同于CListBox的LBS_OWNERDRAWVARIABLE
//-----------------------------------------------------------------------------
//
class CListBoxEL: public CWnd
{
public:
    //
    //=============================================================================
    // 控件创建及样式
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
    // 数据操作
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
    // 控件操作
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
    // 消息处理
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
    // 辅助
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
    // 内部状态属性变量
    //=============================================================================
    //
    LB_ItemInfoArray        m_itemArray;        // item数组容器
    int                     m_curSelIndex;      // 当前被选中的item, -1代表没有选中, 在删除item时要做相应处理
    int                     m_topIndex;         // 当前的显示在第一行的item, 默认是0, 在删除item时要做相应处理
    int                     m_defItemHeight;    // 当前默认的item显示高度, 根据当前的font
    CFont*                  m_font;             // 当前使用的字体
    COLORREF                m_textColor;        // 文字颜色
    COLORREF                m_bkColor;          // 背景颜色

    BOOL                    m_bShowSelAlways;   // 是否在控件丢失焦点时, 还显示选择项的低亮颜色
    COLORREF                m_selHighlightColor;    // 选择行的高亮颜色
    COLORREF                m_textHighlightColor;   // 选择行的高亮文本颜色
    COLORREF                m_selLowlightColor;     // 选择行的低亮颜色, 比如在控件失去焦点, 但有m_bShowSelAlways=1标志时

    int                     m_marginLeft;       // 左边空白区域, 默认0
    int                     m_marginTop;        // 上边空白区域, 默认0
    int                     m_marginRight;      // 右边空白区域, 默认0
    int                     m_marginBottom;     // 下边空白区域, 默认0
};

//
//-----------------------------------------------------------------------------
// 延迟的样式初始化
//-----------------------------------------------------------------------------
//
inline void  CListBoxEL::PostInitTemplateStyle ()
{
}
//
//=============================================================================
// 设置颜色属性
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
