#include "stdafx.h"

#include "ListCtrlEL.h"

//
//=============================================================================
// EL_RECORDITEM_ARGS
//=============================================================================
//

EL_RECORDITEM_ARGS::EL_RECORDITEM_ARGS ()
{
    pControl = NULL;
    rowIndex = NULL;
    colIndex = NULL;
    pItem = NULL;
    rcItem.SetRectEmpty();
}

EL_RECORDITEM_ARGS::EL_RECORDITEM_ARGS (CListCtrlEL* pControl, int rowIndex, int colIndex)
{
    ASSERT_BREAK(pControl);

    this->pControl = pControl;
    this->rowIndex = rowIndex;
    this->colIndex = colIndex;
    this->pItem = pControl->GetRecordRow(rowIndex)->GetItem(colIndex);
    this->rcItem = pControl->GetRowItemRect(rowIndex, colIndex);
}

EL_RECORDITEM_ARGS::~EL_RECORDITEM_ARGS ()
{

}

//
//=============================================================================
// CELRecordItemConstraints
//=============================================================================
//

CELRecordItemConstraint::CELRecordItemConstraint()
{
    m_strConstraint = L"";
    m_dwData = 0;
    m_nIndex = 0;
}


//
//=============================================================================
// CELRecordItemConstraints
//=============================================================================
//

CELRecordItemConstraints::CELRecordItemConstraints ()
{

}

CELRecordItemConstraints::~CELRecordItemConstraints ()
{
    RemoveAll();
}

//
//-----------------------------------------------------------------------------
// �����������Ԫ�ظ���
//-----------------------------------------------------------------------------
//
int  CELRecordItemConstraints::GetCount ()
{
    return m_arrConstraints.GetSize();
}
//
//-----------------------------------------------------------------------------
// ɾ����������Ԫ��
//-----------------------------------------------------------------------------
//
void  CELRecordItemConstraints::RemoveAll ()
{
    for (int i = 0; i < GetCount(); i++)
    {
        if (m_arrConstraints[i])
        {
            delete m_arrConstraints[i];
        }
    }
    m_arrConstraints.RemoveAll();
}
//
//-----------------------------------------------------------------------------
// ���ָ��������Ԫ��
//-----------------------------------------------------------------------------
//
CELRecordItemConstraint*  CELRecordItemConstraints::GetAt (int nIndex)
{
	return m_arrConstraints.GetAt(nIndex);
}


//
//=============================================================================
// CELRecordItemEditOptions
//=============================================================================
//

CELRecordItemEditOptions::CELRecordItemEditOptions ()
{
    m_bAllowEdit = FALSE;
    m_bConstraintEdit = FALSE;
    m_pConstraints = new CELRecordItemConstraints();
    m_bSelectTextOnEdit = TRUE;
}

CELRecordItemEditOptions::~CELRecordItemEditOptions ()
{
    RemoveButtons();

    delete m_pConstraints;
}

//
//-----------------------------------------------------------------------------
// ���һ������Ԫ��
//-----------------------------------------------------------------------------
//
CELRecordItemConstraint*  CELRecordItemEditOptions::AddConstraint (LPCTSTR lpszConstraint, DWORD_PTR dwData)
{
    CELRecordItemConstraint*  pConstaint = new CELRecordItemConstraint();

    pConstaint->m_strConstraint = lpszConstraint;
    pConstaint->m_dwData = dwData;
    pConstaint->m_nIndex = (int)m_pConstraints->m_arrConstraints.Add(pConstaint);

    return pConstaint;
}
//
//-----------------------------------------------------------------------------
// ����һ��������Ԫ��
//-----------------------------------------------------------------------------
//
CELRecordItemConstraint*  CELRecordItemEditOptions::FindConstraint (DWORD_PTR dwData)
{
    for (int i = 0; i < m_pConstraints->GetCount(); i++)
    {
        CELRecordItemConstraint*  pConstaint = m_pConstraints->GetAt(i);
        if (pConstaint->m_dwData == dwData)
        {
            return pConstaint;
        }
    }
    return NULL;
}
CELRecordItemConstraint*  CELRecordItemEditOptions::FindConstraint (LPCTSTR lpszConstraint)
{
    for (int i = 0; i < m_pConstraints->GetCount(); i++)
    {
        CELRecordItemConstraint*  pConstaint = m_pConstraints->GetAt(i);
        if (pConstaint->m_strConstraint == lpszConstraint)
        {
            return pConstaint;
        }
    }
    return NULL;
}
//
//-----------------------------------------------------------------------------
// ������е�����Ԫ�ؼ���
//-----------------------------------------------------------------------------
//
CELRecordItemConstraints*  CELRecordItemEditOptions::GetConstraints ()
{
    return m_pConstraints;
}
//
//-----------------------------------------------------------------------------
// ���һ����Ͽ�ť
//-----------------------------------------------------------------------------
//
CELInplaceButton*  CELRecordItemEditOptions::AddComboButton ()
{
    CELInplaceButton*  pButton = new CELInplaceButton(EL_BTNTYPE_COMBOBUTTON);
    if (pButton)
    {
        arrInplaceButtons.Add(pButton);
    }
    return pButton;
}
//
//-----------------------------------------------------------------------------
// ���һ����չ��ť
//-----------------------------------------------------------------------------
//
CELInplaceButton*  CELRecordItemEditOptions::AddExpandButton ()
{
    CELInplaceButton*  pButton = new CELInplaceButton(EL_BTNTYPE_EXPANDBUTTON);
    if (pButton)
    {
        arrInplaceButtons.Add(pButton);
    }
    return pButton;
}
//
//-----------------------------------------------------------------------------
// ���ָ���İ�ť����
//-----------------------------------------------------------------------------
//
CELInplaceButton*  CELRecordItemEditOptions::GetButton (int nIndex)
{
    if (nIndex >= arrInplaceButtons.GetSize())
    {
        ASSERT_BREAK(FALSE);
        return NULL;
    }
    return arrInplaceButtons.GetAt(nIndex);
}
//
//-----------------------------------------------------------------------------
// ɾ�����а�ť����
//-----------------------------------------------------------------------------
//
void  CELRecordItemEditOptions::RemoveButtons ()
{
    for (int j = 0; j < arrInplaceButtons.GetSize(); j++)
    {
        if (arrInplaceButtons[j])
        {
            delete arrInplaceButtons[j];
        }
    }

    arrInplaceButtons.RemoveAll();
}

//
//=============================================================================
// CELRecordItem
//=============================================================================
//


CELRecordItem::CELRecordItem ()
{
    static const CString  g_strFormatStringDef = _T("%s");

    m_clrText = EL_COLOR_DEFAULT;
    m_clrBackground = EL_COLOR_DEFAULT;
    m_bBoldText = FALSE;
    m_bEditable = FALSE;
    m_pRecord = NULL;
    m_strCaption = L"";
    m_strFormatString = g_strFormatStringDef;
    m_bFocusable = TRUE;
    m_bChecked = FALSE;
    m_bHasCheckbox = FALSE;
    m_dwData = 0;
    m_pEditOptions = NULL;
}

CELRecordItem::~CELRecordItem ()
{
    if (m_pEditOptions)
    {
        delete m_pEditOptions;
    }
}



//
//-----------------------------------------------------------------------------
// ����item
//-----------------------------------------------------------------------------
//
static void  __DrawText (CDC* pDC, const CString& str, CRect rcItem, ELAlignment align)
{
    ASSERT_BREAK(pDC);

    pDC->DrawText(str, rcItem, align | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);
}
static void  __DrawCheckboxHlp (CDC* pDC, const CRect& rcItem, BOOL bChecked, CBitmap* checkBitmap, CBitmap* uncheckBitmap)
{
    ASSERT_BREAK(pDC);
    CBitmap*  checkboxBitmap = (bChecked)? checkBitmap: uncheckBitmap;

    // ��ú��ʵĻ�ͼλ��
    CRect  rcDraw = rcItem;
    rcDraw.DeflateRect(4, 4);
    rcDraw.OffsetRect(-4, 0);

    if (checkboxBitmap)
    {
        // ����λͼ
        CDC  dcMemBmp;
        dcMemBmp.CreateCompatibleDC(pDC);
        CBitmap*  oldbmp = (CBitmap*)dcMemBmp.SelectObject(checkboxBitmap);
        {
            pDC->BitBlt(rcDraw.left, rcDraw.top + 7, rcDraw.Width(), rcDraw.Height(), 
                &dcMemBmp, 0, 0, SRCCOPY);
			///<  ������ԲȦ�л�������
			
			
        }
        dcMemBmp.SelectObject(oldbmp);
    }
    else
    {
        UINT  nState = DFCS_BUTTONCHECK;
        if (bChecked)
        {
            nState |= DFCS_CHECKED;
        }
        pDC->DrawFrameControl(rcDraw, DFC_BUTTON, nState);
    }
}
void  CELRecordItem::Draw (EL_RECORDITEM_DRAWARGS* pDrawArgs)
{
    ASSERT_BREAK(pDrawArgs && pDrawArgs->pDC && pDrawArgs->pControl);
    CELColumn*  colExAttr = pDrawArgs->pControl->GetColumnExAttr(pDrawArgs->colIndex);
    ASSERT_BREAK(colExAttr);

    int  nLeftToRightOffset = colExAttr->m_nMarginLeft;

    // ����checkbox
    if (m_bHasCheckbox)
    {
        // ����λ��
        CRect  rectCheckbox = pDrawArgs->rcItem;
        rectCheckbox.left += nLeftToRightOffset;
        rectCheckbox.right = rectCheckbox.left + rectCheckbox.Height();

        // ����box
        CBitmap*  checkBitmap;
        CBitmap*  uncheckBitmap;
#if !SU_FEI
		CBitmap*  checkfocusBitmap;
		CBitmap*  uncheckfocusBitmap;
#endif
		
#if SU_FEI
        pDrawArgs->pControl->GetCheckImage(checkBitmap, uncheckBitmap);
#else
		pDrawArgs->pControl->GetCheckImage(checkBitmap, uncheckBitmap, checkfocusBitmap, uncheckfocusBitmap);

#endif
			
		//pDrawArgs->pControl->GetItemState()
        if (GetFocus() == pDrawArgs->pControl->GetSafeHwnd() &&
            pDrawArgs->isFocusedRow&&pDrawArgs->isFocusedCol) 
		{
			__DrawCheckboxHlp(pDrawArgs->pDC, rectCheckbox, m_bChecked, checkfocusBitmap, uncheckfocusBitmap);
		}
		else
		{
			__DrawCheckboxHlp(pDrawArgs->pDC, rectCheckbox, m_bChecked, checkBitmap, uncheckBitmap);
		}

        nLeftToRightOffset += rectCheckbox.Width();
    }
     
    // ��������
    CRect  rectText = pDrawArgs->rcItem;
    rectText.left += nLeftToRightOffset;
    rectText.right -= colExAttr->m_nMarginRight;

    __DrawText(pDrawArgs->pDC, GetDispCaption(), rectText, pDrawArgs->nTextAlign);
    nLeftToRightOffset += rectText.Width();

    // ���ư�ť
    if (pDrawArgs->isFocusedRow && pDrawArgs->isFocusedCol)
    {
        CELRecordItemEditOptions*  opts = GetEditOptions(colExAttr);
        ASSERT_BREAK(opts);

        CELInplaceButtons* pInpaceButtons = &opts->arrInplaceButtons;
        if (pInpaceButtons->GetSize() > 0)
        {
            pInpaceButtons->GetAt(0)->Draw(pDrawArgs);
        }
    }
}

//
//-----------------------------------------------------------------------------
// ��Ӧ�ⲿ��Ϣ, ���û��ڵ�ǰ����item���������ʱ��, ������
//  ����ֵ������item�Ѿ������˸���Ϣ
//-----------------------------------------------------------------------------
//
BOOL  CELRecordItem::OnChar (EL_RECORDITEM_ARGS* pItemArgs, UINT nChar)
{
    ASSERT_BREAK(pItemArgs && pItemArgs->pControl);

    //
    // ����ѡ��
    //
    if ((nChar == ELVK_OK) && IsEditable() && m_bHasCheckbox)
    {
        // ʹ��checkbox��ѡ
        SetChecked(!IsChecked());
        pItemArgs->pControl->RedrawRowItem(pItemArgs->rowIndex, pItemArgs->colIndex);

        // ֪ͨ��������һ���ѡ��
        pItemArgs->pControl->SendMessageToParent(pItemArgs->rowIndex, this, 
            pItemArgs->colIndex, ELNM_ITEM_CHECKED);
        return TRUE;
    }

    //
    // ����inplaceedit�ؼ�����
    //
    if (IsAllowEdit(pItemArgs))
    {
        pItemArgs->pControl->EditItem(pItemArgs);

        CELInplaceEdit*  pEdit = pItemArgs->pControl->GetInplaceEdit();

        if (pEdit->GetSafeHwnd() && pEdit->GetItem() == this)
        {
            pEdit->SetFocus();
            pEdit->SetSel(0, -1);
            if (nChar != VK_TAB && nChar != ELVK_OK)
            {
                pEdit->SendMessage(WM_CHAR, nChar);
            }
        }
        return TRUE;
    }

    return FALSE;
}
//
//-----------------------------------------------------------------------------
// ��Ӧitem�ڲ���ť������¼�
//-----------------------------------------------------------------------------
//
void  CELRecordItem::OnInplaceButtonDown (EL_RECORDITEM_ARGS* pItemArgs, CELInplaceButton* pButton)
{
    ASSERT_BREAK(pItemArgs && pItemArgs->pControl && pButton);
    CListCtrlEL*  pControl = pItemArgs->pControl;

    // �������ڷ���Ϣ, ������inplace��ť������
    ELNM_INPLACEBUTTON_ARGS  nm;
    ::ZeroMemory(&nm, sizeof(nm));

    nm.pButton = pButton;
    nm.pItem = this;
    nm.rowIndex = pItemArgs->rowIndex;
    nm.colIndex = pItemArgs->colIndex;


    // bug2013.06.19: 
    //  �����η���, ����ڴ���ELNM_INPLACEBUTTONDOWNʱ, �л���ҳ��(Destroy�˱��ؼ�����)
    //  �ᵼ�º����������, ԭ���Ǻ����EditItem(NULL)�ڲ������ȡ������, ��������m_pActiveItem->OnEndEdit(this);
    //  �����ؼ����ٺ�, ���е�item����m_pActiveItem��ʵ���Ѿ�������, ���¶�m_pActiveItem�ĵ����Ǵ����
    //  Ŀǰ�Ĵ���������CELRecordItem::OnEndEdit�м���IsWindow�ı���, ����Ȼ���ܴ��������pItem���������ٵ����, 
    //  ���������CListCtrlEL������Item������ʱ��, �жϵ�ǰ�Ƿ��ڱ༭״̬m_pActiveItem!=NULL
    //
    //  ����������漰���ܶ����Ƶ�ELNM Notify��Ϣ�ķ��ͺʹ������, ���л����������Ƶ�����,
    //  �����Ľ����������SendNotifyMessageToOwner��, ���ٽ��жԱ��ؼ���pItem���õĴ���, ����ֱ��return������windows��Ϣ����
    HWND  hOwnerWndBackup = pControl->GetSafeHwnd();
    if (pControl->SendNotifyMessageToOwner(ELNM_INPLACEBUTTONDOWN, (NMHDR*)&nm))
    {

        // ��֤������û�а�Owner���� bug2013.06.19
        ASSERT_BREAK(::IsWindow(hOwnerWndBackup));


        // �������Ѿ��������Ϣ
        pControl->EditItem(NULL);  // ȡ������
        return;
    }

    // �����ť��combo, �򼤻�������ؼ�
    if (pButton->GetID() == EL_BTNTYPE_COMBOBUTTON)
    {
        CELInplaceList*  pList = pControl->GetInplaceList();
        ASSERT_BREAK(pList);
        EL_RECORDITEM_ARGS  itemArgs = *pItemArgs;
        ASSERT_BREAK(itemArgs.pItem == this);

        CELRecordItemEditOptions* pEditOptions = GetEditOptions(
            pControl->GetColumnExAttr(itemArgs.colIndex));
        ASSERT_BREAK(pEditOptions);
        if (pEditOptions->GetConstraints()->GetCount() > 0)
        {
            // ���������ؼ�
            pList->Create(&itemArgs, pEditOptions->GetConstraints());

            // ������㴰��, �û���������������, ���Ѿ�������������
            ELNM_INPLACELIST_ARGS  nm;
            ::ZeroMemory(&nm, sizeof(nm));

            nm.pInplaceList = pList;
            nm.pConstraint = (pList->GetCurSel() != LB_ERR)? 
                (CELRecordItemConstraint*)pList->GetItemDataPtr(pList->GetCurSel()): NULL;
            nm.pItem = this;
            nm.rowIndex = pItemArgs->rowIndex;
            nm.colIndex = pItemArgs->colIndex;

            pControl->SendNotifyMessageToOwner(ELNM_ITEM_AFTERLISTDROPDOWN, (NMHDR*)&nm);
        }
    }
}
//
//-----------------------------------------------------------------------------
// ��Ӧ�ⲿ��Ϣ, ���û���������inplace-edit��ʱ�򱻵���
//  ͨ���ڱ����������inplace�ؼ�
//-----------------------------------------------------------------------------
//
void  CELRecordItem::OnBeginEdit (EL_RECORDITEM_ARGS* pItemArgs)
{
    ASSERT_BREAK(pItemArgs);
    if (!pItemArgs)
    {
        return;
    }

    if (IsEditable())
    {
        CListCtrlEL*  pControl = pItemArgs->pControl;
        CELInplaceEdit*  pEdit = pControl->GetInplaceEdit();
        CRect  captionRect = CalcCaptionRect(pItemArgs->rcItem);

        CELRecordItemEditOptions* pEditOptions = 
            GetEditOptions(pControl->GetColumnExAttr(pItemArgs->colIndex));
        ASSERT_BREAK(pEditOptions);

        CELInplaceButtons* pInpaceButtons = &pEditOptions->arrInplaceButtons;

        //
        // ��pEditOptions����༭ʱ, ����Edit�ؼ�
        //
        if (pEditOptions->m_bAllowEdit)
        {
            if (m_bHasCheckbox)
            {
                return;
            }
            else
            {
                pEdit->SetItemArgs(pItemArgs);
                pEdit->Create(pItemArgs);

                // ֪ͨ������, �û�׼����ʼ�༭, Edit�ؼ��Ѿ�����
                pControl->SendMessageToParent(pItemArgs->rowIndex, this, pItemArgs->colIndex, ELNM_ITEM_BEGINEDIT);
            }
        }

        //
        // ��pEditOptions������༭ʱ, ����List�����б�ؼ�
        //
        if (!pEditOptions->m_bAllowEdit && pInpaceButtons->GetSize() > 0)
        {
            CELInplaceButton*  pBtn = pInpaceButtons->GetAt(0);
            OnInplaceButtonDown(pItemArgs, pBtn);
        }
    }

}
//
//-----------------------------------------------------------------------------
// Ҫ�����༭ʱ���øú���, ��OnBeginEdit��Ӧ, 
//  ͨ���ú�����������/�����Ѿ�Create��Inplace�ؼ�
//-----------------------------------------------------------------------------
//
void  CELRecordItem::OnEndEdit (CListCtrlEL* pControl)
{
    // ��������ؼ��༭ʱ, �����Ѿ�������, ��ֱ���˳�, 
    // ������û�а취����this�����Ѿ������ٵ���� bug2013.06.19
    ASSERT_BREAK(::IsWindow(pControl->GetSafeHwnd()));

    if (pControl == NULL)
    {
        ASSERT_BREAK(FALSE);
        return;
    }
    

    // bug20130715 ��ELNM_ITEM_ENDEDIT �·ŵ���������inplace�ؼ�����ǰ
    // ��ΪҪ��Begin���, ��������ʹ����ʹ���ϵ��鷳, ELNM_ITEM_BEGINEDIT��ELNM_ITEM_ENDEDIT���
    // ELNM_ITEM_AFTERLISTDROPDOWN �� ELNM_ITEM_ENDLISTDROPDOWN���
    //
    // ֪ͨ������, �û����ڽ����༭, �������ٿؼ�
    // Ŀǰ����ȡ��rowIndex��colIndex
    //pControl->SendMessageToParent(0, this, 0, ELNM_ITEM_ENDEDIT);

    //
    // ����inplace-List�ؼ�
    //
    CELInplaceList*  pInpaceList = pControl->GetInplaceList();
    ASSERT_BREAK(pInpaceList);
    if (pInpaceList->GetSafeHwnd())
    {
        // ֪ͨ������, �û����ڽ����༭, �������ٿؼ�
        // Ŀǰ����ȡ��rowIndex��colIndex  bug20130715
        pControl->SendMessageToParent(0, this, 0, ELNM_ITEM_ENDLISTDROPDOWN);

        pInpaceList->HideWindow();
        pInpaceList->SetItemArgs(0);
        pInpaceList->DestroyWindow();  // ?? ��Ҫ������
    }

    //
    // ����inplace-Edit�ؼ�
    //
    CELInplaceEdit*  pEdit = pControl->GetInplaceEdit();
    ASSERT_BREAK(pEdit);
    if (pEdit->GetSafeHwnd())
    {
        // ֪ͨ������, �û����ڽ����༭, �������ٿؼ�
        // Ŀǰ����ȡ��rowIndex��colIndex  bug20130715
        pControl->SendMessageToParent(0, this, 0, ELNM_ITEM_ENDEDIT);

        pEdit->HideWindow();
        pEdit->SetItemArgs(0);
        pEdit->DestroyWindow();  // ?? ��Ҫ������
    }
}
//
//-----------------------------------------------------------------------------
// ��Ӧ�ⲿ��Ϣ, ��inplace-edit�е��ȷ��ʱ, ������
//  �����û����OK/Enter��, �û������ڸú�������֤�û�����, ������itemֵ�ı�
//-----------------------------------------------------------------------------
//
void  CELRecordItem::OnValidateEdit (EL_RECORDITEM_ARGS* pItemArgs)
{
    ASSERT_BREAK(pItemArgs);
    if (!pItemArgs)
    {
        return;
    }

    CListCtrlEL*  pControl = pItemArgs->pControl;
    CELInplaceEdit*  pEdit = pControl->GetInplaceEdit();

    if (pEdit->GetSafeHwnd() && pEdit->GetItem() == this)
    {
        BOOL  bRedraw = FALSE;

        if (GetEditOptions(
                pControl->GetColumnExAttr(pItemArgs->colIndex)
            )->m_bConstraintEdit)
        {
            if (pEdit->m_pSelectedConstraint)
            {
                OnConstraintChanged(pItemArgs, pEdit->m_pSelectedConstraint);
                bRedraw = TRUE;
            }
        }
        else
        {
            CString  strValue;
            pEdit->GetWindowText(strValue);

            CString  sOldValue = GetCaption();
            if (sOldValue != strValue)
            {
                // ��ʵ��ֵ����item
                OnEditChanged(pItemArgs, strValue);
                bRedraw = TRUE;
            }
        }

        if (bRedraw)
        {
            // ������㴰��, �û�ִ���˱༭, �ı���ֵ
            pControl->SendMessageToParent(pItemArgs->rowIndex, this, pItemArgs->colIndex, ELNM_ITEM_VALUECHANGED);
        }
    }
}
//
//-----------------------------------------------------------------------------
// ��Ӧ�ⲿ��Ϣ, ��inplace-edit�е��ȡ���༭ʱ, ������, 
//  �����û���ESCȡ�����α༭
//-----------------------------------------------------------------------------
//
void  CELRecordItem::OnCancelEdit (EL_RECORDITEM_ARGS* pItemArgs)
{
    ASSERT_BREAK(pItemArgs && pItemArgs->pControl);

    // ������㴰��, �û�ȡ���˱༭
    pItemArgs->pControl->SendMessageToParent(pItemArgs->rowIndex, this, pItemArgs->colIndex, ELNM_ITEM_EDIT_CANCELED);
}
//
//-----------------------------------------------------------------------------
// ��Ӧ�ⲿ��Ϣ, ��inplace-list���û�ѡ��ı���, ������
//-----------------------------------------------------------------------------
//
void  CELRecordItem::OnConstraintChanged (EL_RECORDITEM_ARGS* pItemArgs, CELRecordItemConstraint* pConstraint)
{
    // ������㴰��, �û�������ĳ���������ѡ��
    ELNM_INPLACELIST_ARGS  nm;
    ::ZeroMemory(&nm, sizeof(nm));

    nm.pInplaceList = pItemArgs->pControl->GetInplaceList();
    nm.pConstraint = pConstraint;
    nm.pItem = this;
    nm.rowIndex = pItemArgs->rowIndex;
    nm.colIndex = pItemArgs->colIndex;

    pItemArgs->pControl->SendNotifyMessageToOwner(ELNM_ITEM_CONSTRAINTCHANGED, (NMHDR*)&nm);

    // ��ʵ��ֵ����item
    OnEditChanged(pItemArgs, pConstraint->m_strConstraint);
}
//
//-----------------------------------------------------------------------------
// ��õ�ǰItem���󶨵�inplace-list������, ��CELRecordItemConstraint::m_dwData ���Ӧ
//  ��ȷ�ķ��ظ�ֵ����ʹ�������ؼ�����ʱ, �Զ����㵽��Ӧ��ѡ��, �μ�Xtreme:TaskListView����
//-----------------------------------------------------------------------------
//
DWORD  CELRecordItem::GetSelectedConstraintData (EL_RECORDITEM_ARGS* pItemArgs)
{
    return -1;
}
//
//-----------------------------------------------------------------------------
// ʵʩ�༭�������ݸı�, �����ݸ�ֵ����item��
//-----------------------------------------------------------------------------
//
void  CELRecordItem::OnEditChanged (EL_RECORDITEM_ARGS* pItemArgs, LPCTSTR szText)
{
    SetCaption(szText);
}
//
//-----------------------------------------------------------------------------
// �༭�����������ڱ��ı�, ����TRUE��������û����޸�, ���򷵻�FALSE
//-----------------------------------------------------------------------------
//
BOOL  CELRecordItem::OnEditChanging (EL_RECORDITEM_ARGS* pItemArgs, CString& rstrNewText)
{
    UNREFERENCED_PARAMETER(pItemArgs);
    UNREFERENCED_PARAMETER(rstrNewText);
    return TRUE;
}

//
//-----------------------------------------------------------------------------
// ��Ȿitem�Ƿ�����༭
//  ԭ���ϱ�������̽�������item, ��item������col, ���б�Ctrl�ؼ��Ƿ�ɱ༭, 
//  ��ͳһ�ó�һ������, �����ڱ��ؼ����򵥵ķ�ʽʵ��, ���Ծ�ʡ���������ж�
//-----------------------------------------------------------------------------
//
BOOL  CELRecordItem::IsAllowEdit (EL_RECORDITEM_ARGS* pItemArgs)
{
    return IsEditable();
}
//
//-----------------------------------------------------------------------------
// ��ȡ��item�����Ŀ�ѡ�����
//  ���������ȡ��item�����, ����col�����Ŀ�ѡ�����, 
//  ���itemû��ָ����ѡ��, ��ʹ�ñ��еĿ�ѡ��
//-----------------------------------------------------------------------------
//
CELRecordItemEditOptions*  CELRecordItem::GetEditOptions (CELColumn* pColumn)
{
    if (m_pEditOptions)
    {
        return m_pEditOptions;
    }

    if (pColumn)
    {
        return pColumn->GetEditOptions();
    }

    return m_pEditOptions = new CELRecordItemEditOptions();
}

//
//-----------------------------------------------------------------------------
// ��ȡ��item�ı������λ�ô�С
//  ����rcItem�Ǳ�Item��ԭʼ���δ�С
//  ������ο�����Ҫȥ�� Icon, checkbox��λ��
//-----------------------------------------------------------------------------
//
CRect  CELRecordItem::CalcCaptionRect (const CRect& rcItem)
{
    CRect  captionRect = rcItem;

    // ȥ��checkboxλ��
    if (GetHasCheckbox())
    {
        captionRect.left += rcItem.Height();
    }

    return captionRect;
}