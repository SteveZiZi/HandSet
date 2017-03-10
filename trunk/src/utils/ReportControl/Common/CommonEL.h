#pragma once



namespace CommonEL
{


#define ASSERT_BREAK(f)   DEBUG_ONLY((void) ((f) || (AfxDebugBreak(), 0) || !::AfxAssertFailedLine(THIS_FILE, __LINE__)))


// �����η���
enum TriangleIconDir
{
    TRI_LEFT,
    TRI_UP,
    TRI_RIGHT,
    TRI_DOWN
};

// չ��ͼ����ʽ
enum ExpandIconStyle
{
    EXP_ADDSUB,
    EXP_TRIANGLE
};


void  DrawGradient (CDC* pDC, CRect& rect, COLORREF begClr, COLORREF endClr, 
        DWORD mode = GRADIENT_FILL_RECT_H);

void  DrawTriangleIcon (CDC* pDC, const CPoint& center, int rad, COLORREF color,  int penWidth, TriangleIconDir dir, BOOL isClosed, BOOL isFill);

void  DrawExpandIcon (CDC* pDC, const CPoint& center, int rad, COLORREF color, BOOL isExpand, ExpandIconStyle style);

//
//-----------------------------------------------------------------------------
// DC����״̬������ָ�������
// �÷�:
// OnDraw(pDC)
//      CSaveDC  __sdc(pDC, CommonEL::CSaveDC::SAVE_PENBRUSH);
//      ...gdi����...
//      ...
//      __sdc.RestoreDC();
//-----------------------------------------------------------------------------
//
class CSaveDC
{
    CDC*                    m_dc;
    
    CPen*                   m_oldpen;
    CBrush*                 m_oldbrs;
    CFont*                  m_oldfont;
    CBitmap*                m_oldbmp;

public:
    enum SaveFlag
    {
        SAVE_PEN            = 0x1,
        SAVE_BRUSH          = 0x2,
        SAVE_FONT           = 0x4,
        SAVE_BITMAP         = 0x8,
        SAVE_PENBRUSH       = SAVE_PEN | SAVE_BRUSH,
    };

public:
    CSaveDC (CDC* pDC, SaveFlag flag): m_dc(NULL), 
        m_oldpen(NULL), 
        m_oldbrs(NULL), 
        m_oldfont(NULL), 
        m_oldbmp(NULL)
    {
        ASSERT_BREAK(pDC);
        m_dc = pDC;

        if (flag & SAVE_PEN) m_oldpen = pDC->GetCurrentPen();
        if (flag & SAVE_BRUSH) m_oldbrs = pDC->GetCurrentBrush();
        if (flag & SAVE_FONT) m_oldfont = pDC->GetCurrentFont();
        if (flag & SAVE_BITMAP) m_oldbmp = pDC->GetCurrentBitmap();
    }
    ~CSaveDC ()
    {
        // �����û��ڶ�������ǰ�Ѿ��ֹ�ȥRestoreDC��, 
        // ��ΪҪ��֤CPenCBrush�ȶ�������˳������, ������ܵ���������CPen�ȶ���, ��RestoreDC�����, ����������᲻��������
        ASSERT_BREAK(m_oldpen == NULL && m_oldbrs == NULL && m_oldfont == NULL && m_oldbmp == NULL);

        RestoreDC();
    }
    void  RestoreDC ()
    {
        ASSERT_BREAK(m_dc);

        if (m_oldpen) m_dc->SelectObject(m_oldpen);
        if (m_oldbrs) m_dc->SelectObject(m_oldbrs);
        if (m_oldfont) m_dc->SelectObject(m_oldfont);
        if (m_oldbmp) m_dc->SelectObject(m_oldbmp);

        m_oldpen = NULL;
        m_oldbrs = NULL;
        m_oldfont = NULL;
        m_oldbmp = NULL;
    }
};

}

