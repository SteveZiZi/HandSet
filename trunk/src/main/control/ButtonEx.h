/**
 * \file
 *      ButtonEx.h
 *
 * \brief
 *      �Զ��尴ť��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/8
 */

#pragma once


// CButtonEx

#define DSM_BUTTONEX_MODE_COLOR             0x01        ///< ��ɫģʽ
#define DSM_BUTTONEX_MODE_BMP               0x02        ///< ͼƬģʽ

class CButtonEx : public CButton
{
	DECLARE_DYNAMIC(CButtonEx)

public:
	CButtonEx(const CString& csCaption = L"buttonex");
	virtual ~CButtonEx();

protected:
	DECLARE_MESSAGE_MAP()
public:
    virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
    afx_msg UINT OnGetDlgCode();

protected:
    virtual void PreSubclassWindow();

public:
    /**
     *      ���ð�ť����
     *
     *  \note
     *      ���GetWindowText��ͬ
     */
    void _SetCaption(const CString& rcsCaption)
    {
        m_csCaption = rcsCaption;
    }

    /**
     *      
     */
    void _SetColorMode(COLORREF bk, COLORREF text, COLORREF focusbk, COLORREF focustext)
    {
        m_nMode             = DSM_BUTTONEX_MODE_COLOR;
        m_crBkColor         = bk;
        m_crTextColor       = text;
        m_crFocusBkColor    = focusbk;
        m_crFocusTextColor  = focustext;
    }

    void _SetBmp(UINT normal, UINT focus, UINT push);

private:
    /**
     *      ����button
     *
     *  \param[in]
     *      dwStatu         DSM_BUTTONEX_STATU_FOCUS:   ����״̬
     *                      DSM_BUTTONEX_STATU_UNFOCUS: �޽���״̬
     */
    void _drawItem(HDC hdc, DWORD dwStatu);

private:
    CString                 m_csCaption;            ///< �Ի�����ʾ����

    // ��ɫģʽ��   DSM_BUTTONEX_MODE_COLOR   0x01
    // ͼƬģʽ��   DSM_BUTTONEX_MODE_BMP     0x02
    int                     m_nMode;                ///< ��ɫ��ͼƬģʽ

    // ��ɫ
    COLORREF                m_crBkColor;            ///< ����ɫ
    COLORREF                m_crTextColor;          ///< ������ɫ
    COLORREF                m_crFocusBkColor;       ///< ��������ɫ
    COLORREF                m_crFocusTextColor;     ///< ����������ɫ

    // ͼƬ
    HBITMAP                 m_bitmapNormal;         ///< ��ͨģʽ
    HBITMAP                 m_bitmapFocus;          ///< ����ģʽ
    HBITMAP                 m_bitmapPress;          ///< ����ģʽ

};


