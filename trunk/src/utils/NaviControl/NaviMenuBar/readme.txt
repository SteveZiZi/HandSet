名称: 导航菜单控件


功能: 屏幕底部F1-F6的导航菜单控件, 类似界面库的CommandBar, XTPToolBar, XTPPopupBar


用法: 包含NaviToolBar.h


框架:
========================================================================================
|
|   CNaviToolBar: CListCtrlEL ---------- CNaviMenu: CMenu <-- CNaviCmdUI: CCmdUI
|       |                                   |
|       |--> CNaviPopupBar: CListBox --------
|
========================================================================================


部件功能:
CNaviToolBar -- 导航大按钮显示控件窗口, 类似XTPToolBar, 一级菜单
CNaviPopupBar -- 从导航大按钮弹出的二级popup菜单窗口, 类似XTPPopupBar
CNaviMenu -- 菜单项容器, 负责管理所有菜单项属性, 不负责显示界面方面的工作



CNaviMenu模型:
尽管有CMenu, 但wince的CMenu太难用, 很多xp上的功能都没有, 自己做一个还要好, 中间可以做一个LoadFromMenu(CMenu*)的辅助接口

Menu====item============================item===========item============item==============
         |
         |--type: string, separator
         |
         |--caption
         |
         |--CmdId
         |
         |--subitem: Menu* 一旦进入就接管
         |
         |--isHighlight
         |
         |--isEnable
         |
         |--isGrayed
         |
         |--isChecked
         |
         |--icon: normal, highlight, grayed


消息流程:
MainFrameDlg:m_mainToolBar

Init: Target --> CNaviMenu::RegUpdateMenuBar --> m_mainToolBar::SetMenu

                                                不处理                                不处理
Command: VK_XXX --> FocusedWnd::PreTransMessage ------> MainFrameDlg::PreTransMessage ------> m_mainToolBar::PreTransMessage --> Target::OnCommand(ID)

Timer: m_mainToolBar::SendTarget(CN_UPDATE_COMMAND_UI, CCmdUI) --> Target::CCmdUI::SetEnable --> CNaviMenu::SetEnable --> CNaviMenu::Update, CNaviPopupBar::Update




版本更新:
*********************** 计划功能 ***********************

美化重绘ListBox




*********************** ${last_version} ***********************



*********************** CListCtrlEL V0.1.0 ***********************

完成第一个版本功能, 可以设置CNaviMenu, 载入CMenu, 并绑定CNaviToolBar显示
