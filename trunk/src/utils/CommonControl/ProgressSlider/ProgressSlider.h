#ifndef PROGRESSSLIDER_H__
#define PROGRESSSLIDER_H__


#define WM_COMCTRL_PROGRESSSLIDER_CHANGE            (PRIVATE_WND_USERID + 300)


/**
 *      进度条控件
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
     *      设置前景色、背景色、文本颜色
     */
    void _SetProgressColor(COLORREF bkcolor, COLORREF fgcolor, COLORREF textcolor);

    /**
     *      获取控件值
     */
    int _GetVal();

    /**
     *      设置控件值
     */
    void _SetVal(int nVal);

    /**
     *      模态显示控件
     */
    int _GoModal(const CRect& rcItem, CWnd* pParentWnd);

    /**
     *      设置控件值范围
     */
    void _SetRange(int nMin, int nMax);

    /**
     *      设置操控范围
     *
     *  \note
     *      该范围与值范围不同
     *      比如值范围是0～100
     *      操控范围是30～80
     *      则说明值允许设置的范围是30～80
     */
    void _SetRangeOp(int nMinOp, int nMaxOp);

    /**
     *      设置字体
     */
    void _SetFont(CFont* font)
    {
        m_ftText = font;
    }

    /**
     *      获取字体
     */
    CFont* _GetFont()
    {
        return m_ftText;
    }

    /**
     *      设置显示格式化字符串
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
     *      发送值改变消息
     */
    void _sendMessage();

private:
    int                     m_min;              ///< 控件最小值
    int                     m_max;              ///< 控件最大值
    int                     m_val;              ///< 控件当前值
    int                     m_minOp;            ///< 控件操作最小值
    int                     m_maxOp;            ///< 控件操作最大值
    CFont*                  m_ftText;           ///< 文本字体
    COLORREF                m_bkcolor;          ///< 背景色
    COLORREF                m_fgcolor;          ///< 前景色
    COLORREF                m_textcolor;        ///< 文本色

    CString                 m_csDisplayFmt;     ///< 显示格式
};



#endif // PROGRESSSLIDER_H__