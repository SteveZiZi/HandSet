#pragma once


//
//=============================================================================
// 版本信息
//=============================================================================
//
#define _MB_VERSION_                    MAKELONG(1, 0)

#define _MB_VERSION_MAJOR               0

#define _MB_VERSION_MINOR               1

#define _MB_VERSION_REVISION            1



// 调试
#ifdef _DEBUG
#define MB_DEBUGLOG  NKDbgPrintfW
#else
#define MB_DEBUGLOG(...)
#endif

#define MB_STRING2(x) #x
#define MB_STRING(x) EL_STRING2(x)


//
//=============================================================================
// 所有类的声明
//=============================================================================
//
class CNaviToolBar;
class CNaviPopupBar;
class CNaviMenu;
class CNaviCmdUI;


//
//=============================================================================
// 常量定义
//=============================================================================
//
#define MBSUPER             CListCtrlEL


// 按键重新映射

#define MBVK_ESC            VK_ESCAPE
#define MBVK_OK             VK_RETURN

// UI美化模板
#define MBT_DRAWTEXTSHADOW          1                                   // 是否绘制阴影
#define MBT_DRAWTEXTSHADOW_COLOR    RGB(32, 32, 32)                     // 阴影颜色
#define MBT_TEXTLFHEIGHT            -16                                 // 推荐字体大小, 需要外层应用调用SetFont
#define MBT_TEXTCOLOR               RGB(255, 255, 255)                  // 推荐文字颜色
#define MBT_TEXTHLCOLOR             RGB(255, 255, 91)                   // 文字高亮颜色
#define MBT_TEXTGRYCOLOR            RGB(127, 127, 127)                  // 文字变灰颜色
#define MBT_TEXTBLACKCOLOR          RGB(40,  50,  68)                   // 文字变黑颜色



//
//=============================================================================
// 通知消息定义
//=============================================================================
//





//
//=============================================================================
// 枚举
//=============================================================================
//
