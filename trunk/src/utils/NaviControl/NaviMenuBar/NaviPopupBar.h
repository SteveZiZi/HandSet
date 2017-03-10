#pragma once


#include "MBDefines.h"

#include "src/utils/ReportControl/Template/ListBoxExt.h"


int DoModal_NaviPopupBar(UINT& retval, int& retmmPos, const CRect& rcItem, CWnd* pParentWnd, CNaviMenu* menu);
int DoModal_NaviPopupBarEx(UINT& retval, int& retmmPos, const CRect& rcItem, CWnd* pParentWnd, CNaviMenu* menu, CNaviPopupBar* popupBar);


//
//-----------------------------------------------------------------------------
// CNaviPopupBar
//
//  �ӵ�����ť�����Ķ���popup�˵�����, ����XTPPopupBar
//-----------------------------------------------------------------------------
//
class CNaviPopupBar: public CListBoxT_FlatBorder
{
public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CNaviPopupBar ();
    virtual ~CNaviPopupBar ();

    int  AddMenuItem (int pos);
    int  GoModal (const CRect& rcItem, CWnd* pParentWnd, CNaviMenu* subMenu);
    int  RunModalLoopEx (DWORD dwFlags = 0);

    UINT  GetSelCmdId ();
    int  GetSelmmPos ();

protected:
    //
    //=============================================================================
    // ��Ϣ����
    //=============================================================================
    //
    DECLARE_MESSAGE_MAP()

    afx_msg UINT  OnGetDlgCode ();
    afx_msg void  OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void  OnKillFocus (CWnd* pNewWnd);
    afx_msg void  OnSetFocus (CWnd* pOldWnd);
    afx_msg void  OnCaptureChanged (CWnd *pWnd);

    virtual BOOL  PreTranslateMessage (MSG* pMsg);
    virtual void  DrawItem (LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void  MeasureItem (LPMEASUREITEMSTRUCT lpMeasureItemStruct);

private:
    //
    //=============================================================================
    // ����
    //=============================================================================
    //
    void  Cancel ();
    void  Apply ();
    void  Close (INT_PTR result);

private:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
    //=============================================================================
    //
    CNaviMenu*              m_submenu;          // ��pop�������������Ӳ˵���
    UINT                    m_selCmdId;         // �û�ѡ���CmdId
    int                     m_selmmPos;         // �û�ѡ��Ĳ˵�����λ��
    BOOL                    m_bApply;

};
//
//-----------------------------------------------------------------------------
// ����û�ѡ�е�����ID
//-----------------------------------------------------------------------------
//
inline UINT  CNaviPopupBar::GetSelCmdId ()
{
    return m_selCmdId;
}
//
//-----------------------------------------------------------------------------
// ����û�ѡ�е����ڵ�ǰ�˵�������λ��
//-----------------------------------------------------------------------------
//
inline int  CNaviPopupBar::GetSelmmPos ()
{
    return m_selmmPos;
}
