/**
 * \file
 *      EditPassword.h
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
     *      设置背景颜色
     */
    void _SetBkColor(COLORREF color)
    {
        m_bkColor = color;

        m_bkBrush.DeleteObject();
        m_bkBrush.CreateSolidBrush(color);
    }

    /**
     *      设置文本颜色
     */
    void _SetTextColor(COLORREF color)
    {
        m_clrText = color;
    }

    /**
     *      设置边框颜色
     */
    void _SetBorderColor(COLORREF color)
    {
        m_bkBorder = color;
    }

    /**
     *      设置文本垂直居中
     */
    void _SetVCenter();

    /**
     *      获取密码值
     */
    CString _GetPassword()
    {
        CString csPassword;
        GetWindowText(csPassword);

        return csPassword;
    }

private:

    COLORREF                m_clrText;          ///< 文本颜色
    COLORREF                m_bkColor;          ///< 背景颜色
    COLORREF                m_bkBorder;         ///< 边框颜色
    CBrush                  m_bkBrush;          ///< 背景色画刷

    CRect m_rectNCBottom;
    CRect m_rectNCTop;

};


