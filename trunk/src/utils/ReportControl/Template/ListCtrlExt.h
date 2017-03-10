#pragma once

#include "../ListCtrlEL/ListCtrlEL.h"
#include "../ListBoxEL/TreeBoxEL.h"
#include "ListBoxExt.h"

//
//=============================================================================
// 一号表格模板, 适用于大部分的使用report表格的情况
//=============================================================================
//
#define ELT_T1_TEXTLFHEIGHT         -16                                 // 推荐字体大小
#define ELT_T1_HDRHEIGHT            38                                 // 表头高度
#if SHENG_ZE
#define ELT_T1_ROWHEIGHT            41                                 // 内容行高
#else
#define ELT_T1_ROWHEIGHT            37                                 // 内容行高
#endif
#define ELT_T1_HDRGRIDCOLOR         RGB(49,  61,  81)               // 表头grid线颜色
#define ELT_T1_HDRBKCOLOR           RGB(60,  74,  98)                   // 表头背景颜色
#define ELT_T1_HDRTEXTCOLOR         RGB(197, 201, 208)                  // 表头文字颜色

#define ELT_T1_BKCOLOR              RGB(58,  82,  100)                   // 表体背景颜色
#define ELT_T1_TEXTCOLOR            RGB(255, 255, 255)                  // 表体文本颜色
#define ELT_T1_HLCOLOR              RGB(239, 145, 16)                   // 高亮颜色
#define ELT_T1_HLTEXTCOLOR          RGB(0,   0,   0)                    // 文本高亮颜色
#define ELT_T1_LLCOLOR              RGB(157, 160, 168)                  // 低亮颜色

#define ELT_T1_SUBHLCOLOR           ELT_T1_HLCOLOR/*RGB(247, 219, 91)*/                   // 子项(单个格子)高亮颜色
#define ELT_T1_EDITBKCOLOR          LBT_FB2_BKCOLOR                     // Inplace-edit的背景颜色
#define ELT_T1_EDITTEXTCOLOR        ELT_T1_TEXTCOLOR                    // Inplace-edit的文字颜色
#define ELT_T1_EDITBORDERCOLOR      LBT_FB_BORDERCOLOR                  // inplace-edit的边框颜色
#define ELT_T1_LISTBKCOLOR          LBT_FB_BKCOLOR                     // Inplace-list的背景颜色
#define ELT_T1_LISTTEXTCOLOR        LBT_FB_TEXTCOLOR                    // Inplace-list的文字颜色
#define ELT_T1_LISTHLCOLOR          LBT_FB_HLCOLOR                      // Inplace-list的高亮颜色
#define ELT_T1_LISTHLTEXTCOLOR      LBT_FB_HLTEXTCOLOR                  // Inplace-list的文字高亮颜色



//
//=============================================================================
// CListCtrlT_VLine   带表头的Report控件扩展模板 (垂直Grid线), 通常用于数据采集展示界面
//=============================================================================
//

class CListCtrlT_VLine: public CListCtrlEL
{
public:
    CListCtrlT_VLine ();
    virtual ~CListCtrlT_VLine ();

    void  PostInitTemplateStyle ();
};



//
//=============================================================================
// CListCtrlT_HLine   不带表头的Report控件扩展模板 (水平Grid线), 通常用于系统参数设置界面
//=============================================================================
//

class CListCtrlT_HLine: public CListCtrlEL
{
public:
    CListCtrlT_HLine ();
    virtual ~CListCtrlT_HLine ();

    void  PostInitTemplateStyle ();
};


//
//=============================================================================
// CListCtrlT_VLineMidH    垂直分割线与水平中线的混合的美化ListCtrl, 可以用于相量, 核相等页面
//=============================================================================
//

class CListCtrlT_VLineMidH: public CListCtrlT_VLine
{
public:
    CListCtrlT_VLineMidH ();
    virtual ~CListCtrlT_VLineMidH ();

    void  PostInitTemplateStyle ();

protected:
    virtual void  FillGridline (CDC* pDC);
};


//
//=============================================================================
// CTreeBoxT_VLine    垂直分割线的美化TreeBox
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// CTreeBoxT_VLine
//-----------------------------------------------------------------------------
//
class CTreeBoxT_VLine: public CTreeBoxEL
{
public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CTreeBoxT_VLine ();
    virtual ~CTreeBoxT_VLine ();

    void  PostInitTemplateStyle ();

protected:
    //
    //=============================================================================
    // 消息处理
    //=============================================================================
    //

protected:
    //
    //=============================================================================
    // 辅助
    //=============================================================================
    //

protected:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //

};


//
//=============================================================================
// CListCtrlT_AboutCtrl  关于页面, 根据美工设计
//=============================================================================
//

#define ELT_ABT_TEXTLFHEIGHT        -15                                 // 推荐字体大小
#define ELT_ABT_COLTITLE_MARGIN     20                                  // 标题列的左边距
#define ELT_ABT_COLTITLE_WIDTH      105                                 // 标题列的宽度
#define ELT_ABT_COLTITLE_COLOR      RGB(0xFF, 0xFF, 0xFF)               // 标题列的文本颜色

#define ELT_ABT_COLSUBTITLE_WIDTH   80                                  // 子标题列的宽度


class CListCtrlT_AboutCtrl: public CListCtrlT_HLine
{
public:
    // 列定义
    enum AboutColumnIndex
    {
        COL_TITLE       = 0,  // 主标题列
        COL_SUBTITLE    = 1,  // 副标题列
        COL_DESC        = 2   // 描述信息列
    };

    // 行定义
    enum AboutRowIndex
    {
        ROW_TOP_BLANK       = 0,    // 空白行, 用于占位
        ROW_PRODUCT_NAME    = 1,    // 产品名字
        ROW_HW_VER          = 2,    // 硬件版本
        ROW_SYS_VER         = 3,    // 操作系统版本
        ROW_SW_VER          = 4,    // 软件版本
        ROW_COMPANY_NAME    = 5,    // 公司名字
        ROW_COMPANY_ADDR    = 6,    // 公司地址
        ROW_SUPPORT         = 7,    // 技术支持
        ROW_BOT_BLANK       = 8     // 底部空白行
    };

public:
    CListCtrlT_AboutCtrl ();
    virtual ~CListCtrlT_AboutCtrl ();

    void  PostInitTemplateStyle ();


public:
    //
    // 文字描述设置
    //
    void  SetAboutItemText (AboutRowIndex rowIndex, AboutColumnIndex colIndex, const CString& str);


protected:
    virtual void  FillGridline (CDC* pDC);
};

inline void  CListCtrlT_AboutCtrl::SetAboutItemText (AboutRowIndex rowIndex, AboutColumnIndex colIndex, const CString& str)
{
    SetRowItemText(rowIndex, colIndex, str);
}