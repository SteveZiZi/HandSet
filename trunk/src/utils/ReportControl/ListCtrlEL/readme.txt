名称: 简化的ListCtrl类Report扩展


功能: 继承CListCtrl控件, 实现多Item, InplaceEdit操作, 重绘美化控件界面, 改变字体, 改变颜色等


用法: 参考CListCtrl, 包含 ListCtrlEL.h


框架:
========================================================================================
|
|   CListCtrlEL: CListCtrl
|       |
|       |--> CHeaderCtrlEL: CHeaderCtrl <== CELColumn ----------|
|       |                                                       |
|       |--> LVITEM[], LVITEM <== CELRecord                     |
|       |                           |                           |
|       |                           |          CELRecordItemEditOptions
|       |                           |           |       |
|       |                           |           |       |--> CELRecordItemConstraints[], CELRecordItemConstraint
|       |                           |           |
|       |                           |--> CELRecordItem <== CELRecordItemText, CELRecordItemNumber
|       | 
|       |                                         
|       |--> CELInplaceCtrl <== CELInplaceEdit, CELInplaceList
|
========================================================================================


CListCtrl与CListCtrlEL的命名对应关系:
CListCtrl     CListCtrlEL
header        header（对象暂不开放，由Ctrl提供访问方法）
column        column(对象暂不开放，由Ctrl配合colIndex提供访问方法)
colIndex      colIndex
item          rowindex, record
subitem       recorditem



Inplace控件控件激活响应流程:

按键激活 --> CHeaderCtrlEL::OnChar() --> CHeaderCtrlEL::EditItem(), m_pActiveItem簿记 --

--> CELRecordItem::OnBeginEdit() --> CELInplaceEdit::Create(), SetItemArgs(), 显示Inplace按钮 --> 用户编辑 --> CELInplaceEdit::OnEnKillfocus() --

                  要判断字符串是真的发生了改变
   /--> Apply --> CELRecordItem::OnValidateEdit, OnEditChanged \
--                                                               --> CHeaderCtrlEL::EditItem(NULL), m_pActiveItem簿记 --
   \--> Cancel --> CELRecordItem::OnCancelEdit                 /

--> CELRecordItem::OnEndEdit() --> CELInplaceEdit::Hide(), SetItemArgs(0), DestroyWindow()


其中在CELRecordItem::OnBeginEdit()的时候, 如果禁止编辑, 则CELRecordItem::OnInplaceButtonDown --> 给用户处理, 用户不处理则 --

--> if (pButton->GetID() == COMBOBUTTON) --> CELInplaceList::Create



需要注意的问题:
1. 不能假设当前正在编辑的item就是当前焦点item


2.         // bug2013.06.19: 
        //  刘启鑫发现, 如果在处理ELNM_INPLACEBUTTONDOWN时, 切换了页面(Destroy了本控件窗口)
        //  会导致后续处理崩溃, 原因是后面的EditItem(NULL)内部会进行取消簿记, 进而调用m_pActiveItem->OnEndEdit(this);
        //  而本控件销毁后, 所有的item包括m_pActiveItem事实上已经被析构, 导致对m_pActiveItem的调用是错误的
        //  目前的处理方法是在CELRecordItem::OnEndEdit中加上IsWindow的保护, 但仍然不能处理如果本pItem被单独销毁的情况, 
        //  我想可以在CListCtrlEL析构或本Item析构的时候, 判断当前是否在编辑状态m_pActiveItem!=NULL
        //
        //  这种问题会涉及到很多类似的ELNM Notify消息的发送和处理过程, 都有机会碰到类似的问题,
        //  根本的解决方法是在SendNotifyMessageToOwner后, 不再进行对本控件或pItem调用的代码, 而是直接return出来到windows消息过程





版本更新:
*********************** 计划功能 ***********************

2. inplaceEdit支持进度条方式调整数字, 也就是说要可扩展inplaceEdit, 目前来看, 
如果不动主类功能, 则可以通过响应按钮按下动作, 显示模态窗口实现, 
参考xtreme, TaskListView的Show_XTPDatePicker

3. 树形report支持, 可以展开和收缩, 每个record下面有一个children容器, 有反向rowindex



*********************** ${last_version} ***********************

[修正] 修正Item的InplaceButton响应后, 没有取消簿记问题EditItem(NULL)--m_pActiveItem

[添加] 添加ItemText, ItemNum类型, 用于支持字符串,数字, 但ItemText似乎没有太多必要

[添加] 添加inplaceEdit, inplaceList文字颜色, 背景色的调节功能, 在GetInplaceEdit返回的对象中设置

[改进] 修改CListCtrlEL与CHeaderCtrlEL为公有继承 CListCtrl..., 但这样有个问题, 使用者应该只使用CListCtrlEL提供的功能, 而不要轻易使用父类的功能

[改进] 添加ELNM_ITEM_AFTERLISTDROPDOWN, ELNM_ITEM_CONSTRAINTCHANGED通知消息, 通知父窗口List被下拉, 和一个项被选中

[改进] 加入editchanging消息,  对数字item的inplaceEdit加入数字字符过滤

[修正] 删除CELRecordItem的CObject继承, 与DECLARE_DYNAMIC特性, 依赖C++的RTTI机制速度更快, 但需要在工程中开启/GR

[修正] inplaceList截获按键消息, 让父窗口不能拿到ESC Enter按键消息, 实现类似模态的效果

[添加] 添加ELNM_ITEM_BEGINEDIT, ELNM_ITEM_ENDEDIT消息, 用于通知父窗口用户准备编辑和结束编辑, 并且inplaceEdit弹出时可以响应主菜单的WM_COMMAND: EL_COMMAND_INPLACEEDIT_BACKSPACE 


*********************** CListCtrlEL V0.1.0 ***********************

[添加] 按框架设计实现的基本功能: 基本表格, 样式调整, 可扩展的Record, 可扩展的Item, 支持CheckBox/Inplace-Edit/Inplace-List

