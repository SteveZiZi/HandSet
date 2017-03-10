#ifndef PROGRESSSLIDER_H__
#define PROGRESSSLIDER_H__


#define WM_COMCTRL_PROGRESSSLIDER_CHANGE            (PRIVATE_WND_USERID + 300)


/**
 *      �������ؼ�
 */
class CProgressSlider : public CStatic
{

public:
    CProgressSlider();

    virtual ~CProgressSlider()
    {
    }

public:
    /**
     *      ����ǰ��ɫ������ɫ���ı���ɫ
     */
    void _SetProgressColor(COLORREF bkcolor, COLORREF fgcolor, COLORREF textcolor);

    /**
     *      ��ȡ�ؼ�ֵ
     */
    int _GetVal();

    /**
     *      ���ÿؼ�ֵ
     */
    void _SetVal(int nVal);

    /**
     *      ģ̬��ʾ�ؼ�
     */
    int _GoModal(const CRect& rcItem, CWnd* pParentWnd);

    /**
     *      ���ÿؼ�ֵ��Χ
     */
    void _SetRange(int nMin, int nMax);

    /**
     *      ���òٿط�Χ
     *
     *  \note
     *      �÷�Χ��ֵ��Χ��ͬ
     *      ����ֵ��Χ��0��100
     *      �ٿط�Χ��30��80
     *      ��˵��ֵ�������õķ�Χ��30��80
     */
    void _SetRangeOp(int nMinOp, int nMaxOp);

    /**
     *      ��������
     */
    void _SetFont(CFont* font)
    {
        m_ftText = font;
    }

    /**
     *      ��ȡ����
     */
    CFont* _GetFont()
    {
        return m_ftText;
    }

    /**
     *      ������ʾ��ʽ���ַ���
     */
    void _SetDisplayFmt(CString csFmt)
    {
        m_csDisplayFmt = csFmt;
    }


protected:

    DECLARE_MESSAGE_MAP()

    afx_msg UINT OnGetDlgCode();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnCaptureChanged(CWnd *pWnd);

    virtual BOOL  PreTranslateMessage (MSG* pMsg);

private:
    /**
     *      ����ֵ�ı���Ϣ
     */
    void _sendMessage();

private:
    int                     m_min;              ///< �ؼ���Сֵ
    int                     m_max;              ///< �ؼ����ֵ
    int                     m_val;              ///< �ؼ���ǰֵ
    int                     m_minOp;            ///< �ؼ�������Сֵ
    int                     m_maxOp;            ///< �ؼ��������ֵ
    CFont*                  m_ftText;           ///< �ı�����
    COLORREF                m_bkcolor;          ///< ����ɫ
    COLORREF                m_fgcolor;          ///< ǰ��ɫ
    COLORREF                m_textcolor;        ///< �ı�ɫ

    CString                 m_csDisplayFmt;     ///< ��ʾ��ʽ
};



#endif // PROGRESSSLIDER_H__