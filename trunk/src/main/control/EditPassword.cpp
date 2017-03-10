/**
 * \file
 *      EditPassword.cpp
 *
 * \brief
 *      �Զ������������ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/30
 */

// EditPassword.cpp : ʵ���ļ�
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



// CEditPassword ��Ϣ�������


HBRUSH CEditPassword::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
    // TODO:  �ڴ˸��� DC ���κ�����

    pDC->SetTextColor(m_clrText);
    pDC->SetBkColor(m_bkColor);

    // TODO:  �����Ӧ���ø����Ĵ�������򷵻طǿջ���
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
    // TODO: �ڴ˴������Ϣ����������
    // ��Ϊ��ͼ��Ϣ���� CEdit::OnPaint()

    CRect  rcClient;
    GetClientRect(&rcClient);

    // �����Զ���ı߿�
    CPen  pen(PS_SOLID, 2, m_bkBorder);
    CPen*  oldpen = dc.SelectObject(&pen);
    dc.SelectStockObject(NULL_BRUSH);
    {
        CRect  rcFrame = rcClient;
        rcFrame.DeflateRect(1, 1);
        dc.Rectangle(rcFrame);
    }
    dc.SelectObject(oldpen);

    // ����Ĭ�ϵĻ���
    DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, (LPARAM)0);
}
