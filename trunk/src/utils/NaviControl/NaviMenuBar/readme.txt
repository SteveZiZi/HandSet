����: �����˵��ؼ�


����: ��Ļ�ײ�F1-F6�ĵ����˵��ؼ�, ���ƽ�����CommandBar, XTPToolBar, XTPPopupBar


�÷�: ����NaviToolBar.h


���:
========================================================================================
|
|   CNaviToolBar: CListCtrlEL ---------- CNaviMenu: CMenu <-- CNaviCmdUI: CCmdUI
|       |                                   |
|       |--> CNaviPopupBar: CListBox --------
|
========================================================================================


��������:
CNaviToolBar -- ������ť��ʾ�ؼ�����, ����XTPToolBar, һ���˵�
CNaviPopupBar -- �ӵ�����ť�����Ķ���popup�˵�����, ����XTPPopupBar
CNaviMenu -- �˵�������, ����������в˵�������, ��������ʾ���淽��Ĺ���



CNaviMenuģ��:
������CMenu, ��wince��CMenu̫����, �ܶ�xp�ϵĹ��ܶ�û��, �Լ���һ����Ҫ��, �м������һ��LoadFromMenu(CMenu*)�ĸ����ӿ�

Menu====item============================item===========item============item==============
         |
         |--type: string, separator
         |
         |--caption
         |
         |--CmdId
         |
         |--subitem: Menu* һ������ͽӹ�
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


��Ϣ����:
MainFrameDlg:m_mainToolBar

Init: Target --> CNaviMenu::RegUpdateMenuBar --> m_mainToolBar::SetMenu

                                                ������                                ������
Command: VK_XXX --> FocusedWnd::PreTransMessage ------> MainFrameDlg::PreTransMessage ------> m_mainToolBar::PreTransMessage --> Target::OnCommand(ID)

Timer: m_mainToolBar::SendTarget(CN_UPDATE_COMMAND_UI, CCmdUI) --> Target::CCmdUI::SetEnable --> CNaviMenu::SetEnable --> CNaviMenu::Update, CNaviPopupBar::Update




�汾����:
*********************** �ƻ����� ***********************

�����ػ�ListBox




*********************** ${last_version} ***********************



*********************** CListCtrlEL V0.1.0 ***********************

��ɵ�һ���汾����, ��������CNaviMenu, ����CMenu, ����CNaviToolBar��ʾ
