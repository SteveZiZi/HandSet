����: �򻯵�ListCtrl��Report��չ


����: �̳�CListCtrl�ؼ�, ʵ�ֶ�Item, InplaceEdit����, �ػ������ؼ�����, �ı�����, �ı���ɫ��


�÷�: �ο�CListCtrl, ���� ListCtrlEL.h


���:
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


CListCtrl��CListCtrlEL��������Ӧ��ϵ:
CListCtrl     CListCtrlEL
header        header�������ݲ����ţ���Ctrl�ṩ���ʷ�����
column        column(�����ݲ����ţ���Ctrl���colIndex�ṩ���ʷ���)
colIndex      colIndex
item          rowindex, record
subitem       recorditem



Inplace�ؼ��ؼ�������Ӧ����:

�������� --> CHeaderCtrlEL::OnChar() --> CHeaderCtrlEL::EditItem(), m_pActiveItem���� --

--> CELRecordItem::OnBeginEdit() --> CELInplaceEdit::Create(), SetItemArgs(), ��ʾInplace��ť --> �û��༭ --> CELInplaceEdit::OnEnKillfocus() --

                  Ҫ�ж��ַ�������ķ����˸ı�
   /--> Apply --> CELRecordItem::OnValidateEdit, OnEditChanged \
--                                                               --> CHeaderCtrlEL::EditItem(NULL), m_pActiveItem���� --
   \--> Cancel --> CELRecordItem::OnCancelEdit                 /

--> CELRecordItem::OnEndEdit() --> CELInplaceEdit::Hide(), SetItemArgs(0), DestroyWindow()


������CELRecordItem::OnBeginEdit()��ʱ��, �����ֹ�༭, ��CELRecordItem::OnInplaceButtonDown --> ���û�����, �û��������� --

--> if (pButton->GetID() == COMBOBUTTON) --> CELInplaceList::Create



��Ҫע�������:
1. ���ܼ��赱ǰ���ڱ༭��item���ǵ�ǰ����item


2.         // bug2013.06.19: 
        //  �����η���, ����ڴ���ELNM_INPLACEBUTTONDOWNʱ, �л���ҳ��(Destroy�˱��ؼ�����)
        //  �ᵼ�º����������, ԭ���Ǻ����EditItem(NULL)�ڲ������ȡ������, ��������m_pActiveItem->OnEndEdit(this);
        //  �����ؼ����ٺ�, ���е�item����m_pActiveItem��ʵ���Ѿ�������, ���¶�m_pActiveItem�ĵ����Ǵ����
        //  Ŀǰ�Ĵ���������CELRecordItem::OnEndEdit�м���IsWindow�ı���, ����Ȼ���ܴ��������pItem���������ٵ����, 
        //  ���������CListCtrlEL������Item������ʱ��, �жϵ�ǰ�Ƿ��ڱ༭״̬m_pActiveItem!=NULL
        //
        //  ����������漰���ܶ����Ƶ�ELNM Notify��Ϣ�ķ��ͺʹ������, ���л����������Ƶ�����,
        //  �����Ľ����������SendNotifyMessageToOwner��, ���ٽ��жԱ��ؼ���pItem���õĴ���, ����ֱ��return������windows��Ϣ����





�汾����:
*********************** �ƻ����� ***********************

2. inplaceEdit֧�ֽ�������ʽ��������, Ҳ����˵Ҫ����չinplaceEdit, Ŀǰ����, 
����������๦��, �����ͨ����Ӧ��ť���¶���, ��ʾģ̬����ʵ��, 
�ο�xtreme, TaskListView��Show_XTPDatePicker

3. ����report֧��, ����չ��������, ÿ��record������һ��children����, �з���rowindex



*********************** ${last_version} ***********************

[����] ����Item��InplaceButton��Ӧ��, û��ȡ����������EditItem(NULL)--m_pActiveItem

[���] ���ItemText, ItemNum����, ����֧���ַ���,����, ��ItemText�ƺ�û��̫���Ҫ

[���] ���inplaceEdit, inplaceList������ɫ, ����ɫ�ĵ��ڹ���, ��GetInplaceEdit���صĶ���������

[�Ľ�] �޸�CListCtrlEL��CHeaderCtrlELΪ���м̳� CListCtrl..., �������и�����, ʹ����Ӧ��ֻʹ��CListCtrlEL�ṩ�Ĺ���, ����Ҫ����ʹ�ø���Ĺ���

[�Ľ�] ���ELNM_ITEM_AFTERLISTDROPDOWN, ELNM_ITEM_CONSTRAINTCHANGED֪ͨ��Ϣ, ֪ͨ������List������, ��һ���ѡ��

[�Ľ�] ����editchanging��Ϣ,  ������item��inplaceEdit���������ַ�����

[����] ɾ��CELRecordItem��CObject�̳�, ��DECLARE_DYNAMIC����, ����C++��RTTI�����ٶȸ���, ����Ҫ�ڹ����п���/GR

[����] inplaceList�ػ񰴼���Ϣ, �ø����ڲ����õ�ESC Enter������Ϣ, ʵ������ģ̬��Ч��

[���] ���ELNM_ITEM_BEGINEDIT, ELNM_ITEM_ENDEDIT��Ϣ, ����֪ͨ�������û�׼���༭�ͽ����༭, ����inplaceEdit����ʱ������Ӧ���˵���WM_COMMAND: EL_COMMAND_INPLACEEDIT_BACKSPACE 


*********************** CListCtrlEL V0.1.0 ***********************

[���] ��������ʵ�ֵĻ�������: �������, ��ʽ����, ����չ��Record, ����չ��Item, ֧��CheckBox/Inplace-Edit/Inplace-List

