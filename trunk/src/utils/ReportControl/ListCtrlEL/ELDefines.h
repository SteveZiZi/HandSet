#pragma once


//
//=============================================================================
// 版本信息
//=============================================================================
//
#define _EL_VERSION_                    MAKELONG(1, 0)

#define _EL_VERSION_MAJOR               0

#define _EL_VERSION_MINOR               1

#define _EL_VERSION_REVISION            1



// 调试
#ifdef _DEBUG
#define EL_DEBUGLOG  NKDbgPrintfW
#else
#define EL_DEBUGLOG(...)
#endif

#define EL_STRING2(x) #x
#define EL_STRING(x) EL_STRING2(x)


// 默认开启双缓冲机制
#ifndef EL_DOUBLEBUFFER
#define EL_DOUBLEBUFFER
#endif



//
//=============================================================================
// 所有类的声明
//=============================================================================
//
class CListCtrlEL;
class CHeaderCtrlEL;
class CELColumn;
class CELRecord;

class CELRecordItem;
class CELRecordItemText;
class CELRecordItemNumber;
class CELRecordItemEditOptions;
class CELRecordItemConstraints;
class CELRecordItemConstraint;
class CELInplaceButtons;

class CELInplaceControl;
class CELInplaceEdit;
class CELInplaceList;
class CELInplaceButton;



//
//=============================================================================
// 常量定义
//=============================================================================
//
#define ELSUPER             CListCtrl
#define ELHDRSUPER          CHeaderCtrl

// 按键重新映射
#define ELVK_ESC            VK_ESCAPE
#define ELVK_OK             VK_RETURN

const COLORREF  EL_COLOR_DEFAULT = (COLORREF)-1;

// item绘制文字时的左右留白的间距
const int  EL_ITEMDRAW_MARGIN = 5;


//
//=============================================================================
// 通知消息定义
//=============================================================================
//


//
//-----------------------------------------------------------------------------
// ELNM_ITEM_CHECKED
//  通知父窗口有一个item项的复选框被选中/取消
//  传输的参数ELNM_RECORDITEM_ARGS
//-----------------------------------------------------------------------------
//
#define ELNM_ITEM_CHECKED   (NM_FIRST-53)
//
//-----------------------------------------------------------------------------
// ELNM_ITEM_VALUECHANGED
//  通知父窗口有一个item项的字符串值发生了改变
//  传输的参数ELNM_RECORDITEM_ARGS
//
// !!!注意: 在该消息中不能进行页面切换(控件销毁), 本Item删除操作 bug2013.06.19
//-----------------------------------------------------------------------------
//
#define ELNM_ITEM_VALUECHANGED  (NM_FIRST-57)
//
//-----------------------------------------------------------------------------
// ELNM_ITEM_INPLACEBUTTONDOWN
//  通知父窗口item的一个内置按钮被点击
//  传输的参数ELNM_INPLACEBUTTON_ARGS
//
// !!!注意: 在该消息中不能进行页面切换(控件销毁), 本Item删除操作 bug2013.06.19
//-----------------------------------------------------------------------------
//
#define ELNM_INPLACEBUTTONDOWN  (NM_FIRST-58)
//
//-----------------------------------------------------------------------------
// ELNM_ITEM_AFTERLISTDROPDOWN
//  通知父窗口item的一个内置LIST项被下拉, 
//  父窗口可以处理一些下拉后的处理, 比如填充自己的项目, 或者选中某个项
//  传输的参数ELNM_INPLACELIST_ARGS
//
// !!!注意: 在该消息中不能进行页面切换(控件销毁), 本Item删除操作 bug2013.06.19
//-----------------------------------------------------------------------------
//
#define ELNM_ITEM_AFTERLISTDROPDOWN  (NM_FIRST-59)
#define ELNM_ITEM_ENDLISTDROPDOWN    (NM_FIRST-60)
//
//-----------------------------------------------------------------------------
// ELNM_ITEM_CONSTRAINTCHANGED
//  通知父窗口item的一个内置LIST项被选中, 父窗口可以知道哪个项被选中了
//  传输的参数ELNM_INPLACELIST_ARGS
//
// !!!注意: 在该消息中不能进行页面切换(控件销毁), 本Item删除操作 bug2013.06.19
//-----------------------------------------------------------------------------
//
#define ELNM_ITEM_CONSTRAINTCHANGED  (NM_FIRST-62)
//
//-----------------------------------------------------------------------------
// ELNM_ITEM_BEGINEDIT
//  通知父窗口item的一个内置Edit已经弹出, 用户准备编辑(不包括List, List有ELNM_ITEM_AFTERLISTDROPDOWN消息)
//  或一个inplace-edit结束, 准备销毁inplace控件(只Edit, 不包括List)
//  传输参数ELNM_RECORDITEM_ARGS
//
// !!!注意: 在该消息中不能进行页面切换(控件销毁), 本Item删除操作 bug2013.06.19
//-----------------------------------------------------------------------------
//
#define ELNM_ITEM_BEGINEDIT (NM_FIRST-65)
#define ELNM_ITEM_ENDEDIT   (NM_FIRST-66)
//
//-----------------------------------------------------------------------------
// ELNM_ITEM_EDIT_CANCELED
//  通知父窗口item的edit控件被取消编辑
//  传输的参数ELNM_RECORDITEM_ARGS
//
// !!!注意: 在该消息中不能进行页面切换(控件销毁), 本Item删除操作 bug2013.06.19
//-----------------------------------------------------------------------------
//
#define ELNM_ITEM_EDIT_CANCELED  (NM_FIRST-72)
//
//-----------------------------------------------------------------------------
// ELNM_OKCLICK
//  通知父窗口OK按键被按下, 如果父窗口处理了该消息, 则应该返回TRUE
//  传输的参数NULL
//
// 在该消息中可以进行页面切换(本控件销毁),本Item删除等任何操作
//-----------------------------------------------------------------------------
//
#define ELNM_OKCLICK       (NM_FIRST-83)


//
//=============================================================================
// WM_COMMAND命令消息
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// EL_COMMAND_INPLACEEDIT_BACKSPACE
//  当inplace-edit弹出时, 响应主菜单的命令ID: 删除一个字符
//  注意: 本ID需要与主程序框架定义的ID一致, 否则应该修改
//-----------------------------------------------------------------------------
//
#define EL_COMMAND_INPLACEEDIT_BACKSPACE  32799


//
//=============================================================================
// 枚举
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// 对齐方式
//-----------------------------------------------------------------------------
//
enum ELAlignment
{
    EL_LEFT                 = DT_LEFT,
    EL_CENTER               = DT_CENTER,
    EL_RIGHT                = DT_RIGHT
};

//
//-----------------------------------------------------------------------------
// inplace按钮类型
//-----------------------------------------------------------------------------
//
enum ELInplaceButtonType 
{
    EL_BTNTYPE_EXPANDBUTTON = 100,
    EL_BTNTYPE_COMBOBUTTON  = 101
};

