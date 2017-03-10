#pragma once


#include "ELDefines.h"


//
//-----------------------------------------------------------------------------
// CELColumn
//
//  负责表格中一个列(field/col)的属性, 列名, 列宽度, 列对齐方式, 可选项等
//  如果列中的有些行的格子不服从列的属性, 则以该格子的设置为准
//
//  注意: 因为本类是简化的CListCtrl, 所以列的某些属性是用原生的CHeaderCtrl提供的
//  这里并没有相应的设置, 比如: 列的名字, 列的对齐方式, 列的宽度等, 
//  这些属性可以在CHeaderCtrl::GetItem获得
//-----------------------------------------------------------------------------
//
class CELColumn
{
public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CELColumn ();
    virtual ~CELColumn ();


    CELRecordItemEditOptions*  GetEditOptions ();


private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    BOOL                    m_bEditable;        // Indicates whether an edit box should be added to the items in the column.
    ELAlignment             m_nAlignment;       // 本列的所有元素的对齐方式
    ELAlignment             m_nHeaderAlignment; // Alignment mode for column text: DT_LEFT || DT_RIGHT || DT_CENTER
    COLORREF                m_crBkColor;        // 本列内容的背景色, 默认是EL_COLOR_DEFAULT
    int                     m_nMarginLeft;      // 绘制Item的内容时, 左边的留白区域
    int                     m_nMarginRight;     // 绘制Item的内容时, 右边的留白区域
    BOOL                    m_bBoldText;
    CELRecordItemEditOptions*  m_pEditOptions;  // Edit options of the column


    friend class CHeaderCtrlEL;
    friend class CListCtrlEL;
    friend class CELRecordItem;
};

//
//-----------------------------------------------------------------------------
// 获得本列关联的可选项对象, 该对象会影响本列的所有item
//  如果item没有指定可选项, 则使用本列的可选项
//-----------------------------------------------------------------------------
//
inline CELRecordItemEditOptions*  CELColumn::GetEditOptions ()
{
    return m_pEditOptions;
}

