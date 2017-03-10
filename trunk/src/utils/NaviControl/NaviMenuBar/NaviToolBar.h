#pragma once


//
//=============================================================================
// ����ͷ�ļ�
//=============================================================================
//
#include "MBDefines.h"

#include "NaviMenu.h"
#include "NaviPopupBar.h"

#include "src/utils/ReportControl/ListCtrlEL/ListCtrlEL.h"



//
//-----------------------------------------------------------------------------
// CNaviMenuRecordItem
//
//  �˵����Item
//-----------------------------------------------------------------------------
//
class CNaviMenuRecordItem : public CELRecordItem
{
public:
    CNaviMenuRecordItem (int nPos);

public:
    void  SetMenuItemPos (int nPos);

public:
    virtual void  Draw (EL_RECORDITEM_DRAWARGS* pDrawArgs);


protected:
    int                     m_menuItemPos;      // ��item��˵��������indexPosition


    friend class CNaviToolBar;
};


//
//-----------------------------------------------------------------------------
// CNaviMenuRecord
//
//  �˵���
//-----------------------------------------------------------------------------
//
class CNaviMenuRecord: public CELRecord
{

public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CNaviMenuRecord (int itemCount, CNaviMenu* menu = NULL);
    virtual ~CNaviMenuRecord ();

    void  SetNaviMenu (CNaviMenu* menu);

private:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
    //=============================================================================
    //
    CNaviMenu*              m_menu;             // ��record�����Ĳ˵�

    friend class CNaviMenuRecordItem;
    friend class CNaviToolBar;
};
//
//-----------------------------------------------------------------------------
// ������record�����Ĳ˵�����
//-----------------------------------------------------------------------------
//
inline void  CNaviMenuRecord::SetNaviMenu (CNaviMenu* menu)
{
    m_menu = menu;
}




//
//-----------------------------------------------------------------------------
// ���ñ�item��˵��������indexPosition
//-----------------------------------------------------------------------------
//
inline void  CNaviMenuRecordItem::SetMenuItemPos (int nPos)
{
    m_menuItemPos = nPos;
}




//
//-----------------------------------------------------------------------------
// CNaviToolBar
//
//  ������ť��ʾ�ؼ�����, ����XTPToolBar, һ���˵�
//-----------------------------------------------------------------------------
//
class CNaviToolBar: public MBSUPER
{
public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CNaviToolBar ();
    virtual ~CNaviToolBar ();
#if SU_FEI
	BOOL  Create (int itemWidth, int itemCount, const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL autoClip = FALSE);
#else
	BOOL  Create (int itemHeight, int itemCount, const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL autoClip = FALSE);
#endif  
    void  SetHasGridline (BOOL hasHline, BOOL hasVline);
    void  SetGridlineColor (COLORREF color);
    void  SetBkColor (COLORREF cr);
    void  SetTextColor (COLORREF cr);
    void  SetFont (CFont* pFont, BOOL bRedraw = TRUE);
    void  SetDefaultItemBkImage (CBitmap* bkImage);
    CBitmap*  GetDefaultItemBkImage ();
public:
    //
    //=============================================================================
    // �ؼ�����
    //=============================================================================
    //
    void  UpdateNaviMenu ();

    void  SetNaviMenu (CNaviMenu* menu, BOOL isAutoUpdateCmdUI = FALSE);
    CNaviMenu*  GetNaviMenu ();
    
    void  SetCommandTarget (CWnd* target);
    CWnd*  GetCommandTarget ();


protected:
    //
    //=============================================================================
    // ��Ϣ����
    //=============================================================================
    //
    DECLARE_MESSAGE_MAP()
    afx_msg void  OnTimer (UINT_PTR nIDEvent);
    afx_msg void  OnSetFocus (CWnd* pOldWnd);
    afx_msg void  OnPaint ();
public:
    virtual BOOL  PreTranslateMessage (MSG* pMsg);

private:
    //
    //=============================================================================
    // ����
    //=============================================================================
    //
    void  ResetNaviMenu ();
    BOOL  OnNaviKeyDown (int key);
    void  UpdateCmdUI (CNaviMenu* menu);
    void  CloseNaviPopupBar ();


private:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
    //=============================================================================
    //
    CNaviMenu*              m_naviMenu;         // ���ؼ������Ĳ˵�
    CWnd*                   m_commandTarget;    // �˵���û���ѡʱ, ����WM_COMMAND��Ŀ�괰��
    CBitmap*                m_defItemBkImage;   // Ĭ�ϵ�һ��Item�ı���ͼƬ, ������û�ȥά��������������
    UINT                    m_vkmapBegin;       // ������ʼӳ��, ���縳��VK_F1, �� VK_F1~VK_F6�Ͷ�Ӧ�û����Col0~Col5
    BOOL                    m_isAutoUpdateCmdUI;// �Ƿ��Զ���ʱ��CmdUI����ˢ�²˵���ǰ״̬
    CNaviPopupBar           m_popupBar;         // �����˵��ؼ�����

#if !SU_FEI
	CBitmap*				m_switchBkImage;	// ��ʾ���е���ͼƬ���и������ֵ�ͼƬ
	
#endif
};


//
//=============================================================================
// �򵥵�inline����ʵ��
//=============================================================================
//
inline CNaviMenu*  CNaviToolBar::GetNaviMenu ()
{
    return m_naviMenu;
}
inline CWnd*  CNaviToolBar::GetCommandTarget ()
{
    return m_commandTarget;
}
inline void  CNaviToolBar::SetHasGridline (BOOL hasHline, BOOL hasVline)
{
    MBSUPER::SetHasGridline(hasHline, hasVline);
}
inline void  CNaviToolBar::SetGridlineColor (COLORREF color)
{
    MBSUPER::SetHeaderGridlineColor(color);
}
inline void  CNaviToolBar::SetBkColor (COLORREF cr)
{
    MBSUPER::SetBkColor(cr);
}
inline void  CNaviToolBar::SetTextColor (COLORREF cr)
{
    MBSUPER::SetTextColor(cr);
}
inline void  CNaviToolBar::SetFont (CFont* pFont, BOOL /*bRedraw*/)
{
    MBSUPER::SetFont(pFont);
}
inline void  CNaviToolBar::SetDefaultItemBkImage (CBitmap* bkImage)
{
    m_defItemBkImage = bkImage;
}
inline CBitmap*  CNaviToolBar::GetDefaultItemBkImage ()
{
    return m_defItemBkImage;
}
