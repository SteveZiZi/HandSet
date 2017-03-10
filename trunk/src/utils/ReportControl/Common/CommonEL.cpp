#include "stdafx.h"

#include "CommonEL.h"


namespace CommonEL
{


//
//-----------------------------------------------------------------------------
// 绘制渐变区域
//-----------------------------------------------------------------------------
//
void  DrawGradient (CDC* pDC, CRect& rect, COLORREF begClr, COLORREF endClr, DWORD mode)
{
    TRIVERTEX        vert[2];
    GRADIENT_RECT    gRect;
    vert [0] .x      = rect.left;
    vert [0] .y      = rect.top;
    vert [0] .Red    = GetRValue(begClr) << 8;
    vert [0] .Green  = GetGValue(begClr) << 8;
    vert [0] .Blue   = GetBValue(begClr) << 8;
    vert [0] .Alpha  = 0x0000;

    vert [1] .x      = rect.right;
    vert [1] .y      = rect.bottom; 
    vert [1] .Red    = GetRValue(endClr) << 8;
    vert [1] .Green  = GetGValue(endClr) << 8;
    vert [1] .Blue   = GetBValue(endClr) << 8;
    vert [1] .Alpha  = 0x00FF;

    gRect.UpperLeft  = 0;
    gRect.LowerRight = 1;
    ::GradientFill(pDC->GetSafeHdc(), vert, 2, &gRect, 1, mode);
}

//
//-----------------------------------------------------------------------------
// 绘制三角形图标, combobox
//-----------------------------------------------------------------------------
//
void  DrawTriangleIcon (CDC* pDC, const CPoint& center, int rad, COLORREF color, int penWidth, TriangleIconDir dir, BOOL isClosed, BOOL isFill)
{
#define TRIANGLE_POINT_NUM  3

    CSaveDC  sdc(pDC, CSaveDC::SAVE_PENBRUSH);

    // 选择画笔
    CPen  pen(PS_SOLID, penWidth, color);
    CBrush  brs(color);
    pDC->SelectObject(&pen);
    if (isFill)
    {
        pDC->SelectObject(&brs);
    }
    else
    {
        pDC->SelectStockObject(NULL_BRUSH);
    }

    // 计算位置
    CRect  triangleRectUpDown(center.x-rad, center.y-rad/2, center.x+rad, center.y+rad/2+1);
    CRect  triangleRectLeftRight(center.x-rad/2, center.y-rad, center.x+rad/2+1, center.y+rad);
    CPoint  trianglePoints[TRIANGLE_POINT_NUM];
    switch (dir)
    {
    case TRI_LEFT:
        trianglePoints[0] = CPoint(triangleRectLeftRight.right, triangleRectLeftRight.top);
        trianglePoints[1] = CPoint(triangleRectLeftRight.left, triangleRectLeftRight.CenterPoint().y);
        trianglePoints[2] = CPoint(triangleRectLeftRight.right, triangleRectLeftRight.bottom);
        break;
    case TRI_UP:
        trianglePoints[0] = CPoint(triangleRectUpDown.left, triangleRectUpDown.bottom);
        trianglePoints[1] = CPoint(triangleRectUpDown.CenterPoint().x, triangleRectUpDown.top);
        trianglePoints[2] = CPoint(triangleRectUpDown.right, triangleRectUpDown.bottom);
        break;
    case TRI_RIGHT:
        trianglePoints[0] = CPoint(triangleRectLeftRight.left, triangleRectLeftRight.top);
        trianglePoints[1] = CPoint(triangleRectLeftRight.right, triangleRectLeftRight.CenterPoint().y);
        trianglePoints[2] = CPoint(triangleRectLeftRight.left, triangleRectLeftRight.bottom);
        break;
    case TRI_DOWN:
        trianglePoints[0] = CPoint(triangleRectUpDown.left, triangleRectUpDown.top);
        trianglePoints[1] = CPoint(triangleRectUpDown.CenterPoint().x, triangleRectUpDown.bottom);
        trianglePoints[2] = CPoint(triangleRectUpDown.right, triangleRectUpDown.top);
        break;
    }

    if (isClosed)
    {
        pDC->Polygon(trianglePoints, TRIANGLE_POINT_NUM);
    }
    else
    {
        pDC->Polyline(trianglePoints, TRIANGLE_POINT_NUM);
    }

    sdc.RestoreDC();
}

//
//-----------------------------------------------------------------------------
// 绘制展开按钮图标 + -
//-----------------------------------------------------------------------------
//
void  DrawExpandIcon (CDC* pDC, const CPoint& center, int rad, COLORREF color, BOOL isExpand, ExpandIconStyle style)
{
    switch (style)
    {
    case EXP_TRIANGLE:  // 绘制三角形符号
        {
            if (isExpand)
            {
                DrawTriangleIcon(pDC, center, rad, color, 1, TRI_RIGHT, TRUE, TRUE);
            }
            else
            {
                DrawTriangleIcon(pDC, center, rad, color, 1, TRI_DOWN, TRUE, TRUE);
            }
        }
        break;
    default:  // 绘制普通的 + - 号
        {
            CSaveDC  sdc(pDC, CSaveDC::SAVE_PENBRUSH);

            CPen  pen(PS_SOLID, 1, color);
            pDC->SelectObject(&pen);
            pDC->SelectStockObject(NULL_BRUSH);

            CRect  rcOuter(center.x - rad, center.y - rad, center.x + rad+1, center.y + rad+1);
            pDC->Rectangle(rcOuter);

            CRect  rcInner = rcOuter;
            rcInner.DeflateRect(3, 3);
            // 画水平线
            pDC->FillSolidRect(rcInner.left, (rcInner.top + rcInner.bottom) / 2, rcInner.Width(), 1, color);
            if (isExpand)
            {
                // 画垂直线
                pDC->FillSolidRect((rcInner.left + rcInner.right) / 2, rcInner.top, 1, rcInner.Height(), color);
            }


            sdc.RestoreDC();
        }
        break;
    }
}

}