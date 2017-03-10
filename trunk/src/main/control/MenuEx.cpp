/**
 * \file
 *      MenuEx.cpp
 *
 * \brief
 *      �Զ�����ർ���˵�ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/16
 */

#include "StdAfx.h"
#include "menuex.h"

#include "src/main/common.h"

CMenuEx::CMenuEx(void)
{
}

CMenuEx::~CMenuEx(void)
{
}

BEGIN_MESSAGE_MAP(CMenuEx, CListBoxEL)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CMenuEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    int direct = 0;
    if (nChar == VK_UP || nChar == VK_LEFT)
    {
        direct = -1;
    }
    else if (nChar == VK_DOWN || nChar == VK_RIGHT)
    {
        direct = 1;
    }

    int  targetSel = GetCurSel() + direct;
    if (targetSel >= 0 && targetSel <GetCount())
    {
        if(LB_OKAY == SetCurSel(targetSel))
        {
            // ����ѡ��item��Ϣ
            CWnd* pOwner = GetOwner();

            if(pOwner && IsWindow(pOwner->m_hWnd))
            {
                pOwner->SendMessage(
                    WM_COMMAND, 
                    MAKEWPARAM(GetDlgCtrlID(), 
                    LBN_SELCHANGE), 
                    (LPARAM)m_hWnd);
            }
        }
    }

    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CMenuEx::SetHeight( UINT itemHeight )
{
    int nCounts = GetCount();
    for(int i = 0; i < nCounts; i++)
    {
        SetItemHeight(i, itemHeight);
    }
}
