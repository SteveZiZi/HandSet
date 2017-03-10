/**
 * \file
 *      EditPassword.cpp
 *
 * \brief
 *      自定义密码输入框头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/30
 */

// EditPassword.cpp : 实现文件
//

#include "stdafx.h"
#include "EditPassword.h"


#define EditPassword_CMD_BACKSPACE  32799


// CEditPassword

IMPLEMENT_DYNAMIC(CEditPassword, CEdit)

CEditPassword::CEditPassword()
{
    m_clrText = RGB(0, 0, 0);
    m_bkColor = RGB(255, 255, 255);
    m_bkBrush.CreateSolidBrush(RGB(255, 255, 255));
}

CEditPassword::~CEditPassword()
{

}


BEGIN_MESSAGE_MAP(CEditPassword, CEdit)
    ON_WM_CTLCOLOR_REFLECT()
    ON_COMMAND(EditPassword_CMD_BACKSPACE, &CEditPassword::OnBtnBackspace)
    ON_UPDATE_COMMAND_UI(EditPassword_CMD_BACKSPACE, &CEditPassword::OnUpdateBtnBackspace)
    ON_WM_PAINT()
END_MESSAGE_MAP()



// CEditPassword 消息处理程序


HBRUSH CEditPassword::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
    // TODO:  在此更改 DC 的任何属性

    pDC->SetTextColor(m_clrText);
    pDC->SetBkColor(m_bkColor);

    // TODO:  如果不应调用父级的处理程序，则返回非空画笔
    return m_bkBrush;
}

void CEditPassword::_SetVCenter()
{
    this->SetMargins(0, 0);

    CRect rcWnd;
    this->GetClientRect(&rcWnd);

    CDC* pDC = this->GetDC();
    CFont* oldfont = pDC->SelectObject(GetFont());

    TEXTMETRIC tm;
    ::ZeroMemory(&tm, sizeof(tm));
    pDC->GetTextMetrics(&tm);
    pDC->SelectObject(oldfont);
    this->ReleaseDC(pDC);

    int nFontHeight = tm.tmHeight + tm.tmExternalLeading;
    int nMargin = (rcWnd.Height() - nFontHeight) / 2;
    rcWnd.DeflateRect(10, nMargin);
    this->SetRectNP(&rcWnd);
}


void CEditPassword::OnBtnBackspace()
{
    PostMessage(WM_CHAR, VK_BACK, 0);
}

void CEditPassword::OnUpdateBtnBackspace( CCmdUI *pCmdUI )
{
    pCmdUI->Enable(1);
}


void CEditPassword::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CEdit::OnPaint()

    CRect  rcClient;
    GetClientRect(&rcClient);

    // 绘制自定义的边框
    CPen  pen(PS_SOLID, 2, m_bkBorder);
    CPen*  oldpen = dc.SelectObject(&pen);
    dc.SelectStockObject(NULL_BRUSH);
    {
        CRect  rcFrame = rcClient;
        rcFrame.DeflateRect(1, 1);
        dc.Rectangle(rcFrame);
    }
    dc.SelectObject(oldpen);

    // 调用默认的画法
    DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, (LPARAM)0);
}
