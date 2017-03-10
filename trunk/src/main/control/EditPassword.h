/**
 * \file
 *      EditPassword.h
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
#pragma once



// CEditPassword

class CEditPassword : public CEdit
{
	DECLARE_DYNAMIC(CEditPassword)

public:
	CEditPassword();
	virtual ~CEditPassword();

protected:
	DECLARE_MESSAGE_MAP()

public:
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    afx_msg void  OnBtnBackspace();
    afx_msg void OnPaint();
    afx_msg void  OnUpdateBtnBackspace(CCmdUI *pCmdUI);


public:

    /**
     *      ���ñ�����ɫ
     */
    void _SetBkColor(COLORREF color)
    {
        m_bkColor = color;

        m_bkBrush.DeleteObject();
        m_bkBrush.CreateSolidBrush(color);
    }

    /**
     *      �����ı���ɫ
     */
    void _SetTextColor(COLORREF color)
    {
        m_clrText = color;
    }

    /**
     *      ���ñ߿���ɫ
     */
    void _SetBorderColor(COLORREF color)
    {
        m_bkBorder = color;
    }

    /**
     *      �����ı���ֱ����
     */
    void _SetVCenter();

    /**
     *      ��ȡ����ֵ
     */
    CString _GetPassword()
    {
        CString csPassword;
        GetWindowText(csPassword);

        return csPassword;
    }

private:

    COLORREF                m_clrText;          ///< �ı���ɫ
    COLORREF                m_bkColor;          ///< ������ɫ
    COLORREF                m_bkBorder;         ///< �߿���ɫ
    CBrush                  m_bkBrush;          ///< ����ɫ��ˢ

    CRect m_rectNCBottom;
    CRect m_rectNCTop;

};


