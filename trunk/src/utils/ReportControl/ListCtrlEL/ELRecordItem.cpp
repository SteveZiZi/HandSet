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
// 获得所有限制元素个数
//-----------------------------------------------------------------------------
//
int  CELRecordItemConstraints::GetCount ()
{
    return m_arrConstraints.GetSize();
}
//
//-----------------------------------------------------------------------------
// 删除所有限制元素
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
// 获得指定的限制元素
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
// 添加一个限制元素
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
// 查找一个限制项元素
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
// 获得所有的限制元素集合
//-----------------------------------------------------------------------------
//
CELRecordItemConstraints*  CELRecordItemEditOptions::GetConstraints ()
{
    return m_pConstraints;
}
//
//-----------------------------------------------------------------------------
// 添加一个组合框按钮
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
// 添加一个扩展按钮
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
// 获得指定的按钮对象
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
// 删除所有按钮对象
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
// 绘制item
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

    // 获得合适的绘图位置
    CRect  rcDraw = rcItem;
    rcDraw.DeflateRect(4, 4);
    rcDraw.OffsetRect(-4, 0);

    if (checkboxBitmap)
    {
        // 绘制位图
        CDC  dcMemBmp;
        dcMemBmp.CreateCompatibleDC(pDC);
        CBitmap*  oldbmp = (CBitmap*)dcMemBmp.SelectObject(checkboxBitmap);
        {
            pDC->BitBlt(rcDraw.left, rcDraw.top + 7, rcDraw.Width(), rcDraw.Height(), 
                &dcMemBmp, 0, 0, SRCCOPY);
			///<  怎样把圆圈中画出来。
			
			
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

    // 绘制checkbox
    if (m_bHasCheckbox)
    {
        // 计算位置
        CRect  rectCheckbox = pDrawArgs->rcItem;
        rectCheckbox.left += nLeftToRightOffset;
        rectCheckbox.right = rectCheckbox.left + rectCheckbox.Height();

        // 绘制box
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
     
    // 绘制文字
    CRect  rectText = pDrawArgs->rcItem;
    rectText.left += nLeftToRightOffset;
    rectText.right -= colExAttr->m_nMarginRight;

    __DrawText(pDrawArgs->pDC, GetDispCaption(), rectText, pDrawArgs->nTextAlign);
    nLeftToRightOffset += rectText.Width();

    // 绘制按钮
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
// 响应外部消息, 当用户在当前焦点item点击按键的时候, 被调用
//  返回值代表当次item已经处理了该消息
//-----------------------------------------------------------------------------
//
BOOL  CELRecordItem::OnChar (EL_RECORDITEM_ARGS* pItemArgs, UINT nChar)
{
    ASSERT_BREAK(pItemArgs && pItemArgs->pControl);

    //
    // 处理复选框
    //
    if ((nChar == ELVK_OK) && IsEditable() && m_bHasCheckbox)
    {
        // 使得checkbox反选
        SetChecked(!IsChecked());
        pItemArgs->pControl->RedrawRowItem(pItemArgs->rowIndex, pItemArgs->colIndex);

        // 通知父窗口有一个项被选中
        pItemArgs->pControl->SendMessageToParent(pItemArgs->rowIndex, this, 
            pItemArgs->colIndex, ELNM_ITEM_CHECKED);
        return TRUE;
    }

    //
    // 处理inplaceedit控件弹出
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
// 响应item内部按钮被点击事件
//-----------------------------------------------------------------------------
//
void  CELRecordItem::OnInplaceButtonDown (EL_RECORDITEM_ARGS* pItemArgs, CELInplaceButton* pButton)
{
    ASSERT_BREAK(pItemArgs && pItemArgs->pControl && pButton);
    CListCtrlEL*  pControl = pItemArgs->pControl;

    // 给父窗口发消息, 告诉有inplace按钮按下了
    ELNM_INPLACEBUTTON_ARGS  nm;
    ::ZeroMemory(&nm, sizeof(nm));

    nm.pButton = pButton;
    nm.pItem = this;
    nm.rowIndex = pItemArgs->rowIndex;
    nm.colIndex = pItemArgs->colIndex;


    // bug2013.06.19: 
    //  刘启鑫发现, 如果在处理ELNM_INPLACEBUTTONDOWN时, 切换了页面(Destroy了本控件窗口)
    //  会导致后续处理崩溃, 原因是后面的EditItem(NULL)内部会进行取消簿记, 进而调用m_pActiveItem->OnEndEdit(this);
    //  而本控件销毁后, 所有的item包括m_pActiveItem事实上已经被析构, 导致对m_pActiveItem的调用是错误的
    //  目前的处理方法是在CELRecordItem::OnEndEdit中加上IsWindow的保护, 但仍然不能处理如果本pItem被单独销毁的情况, 
    //  我想可以在CListCtrlEL析构或本Item析构的时候, 判断当前是否在编辑状态m_pActiveItem!=NULL
    //
    //  这种问题会涉及到很多类似的ELNM Notify消息的发送和处理过程, 都有机会碰到类似的问题,
    //  根本的解决方法是在SendNotifyMessageToOwner后, 不再进行对本控件或pItem调用的代码, 而是直接return出来到windows消息过程
    HWND  hOwnerWndBackup = pControl->GetSafeHwnd();
    if (pControl->SendNotifyMessageToOwner(ELNM_INPLACEBUTTONDOWN, (NMHDR*)&nm))
    {

        // 保证调用者没有把Owner销毁 bug2013.06.19
        ASSERT_BREAK(::IsWindow(hOwnerWndBackup));


        // 父窗口已经处理该消息
        pControl->EditItem(NULL);  // 取消簿记
        return;
    }

    // 如果按钮是combo, 则激活弹出下拉控件
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
            // 创建下拉控件
            pList->Create(&itemArgs, pEditOptions->GetConstraints());

            // 告诉外层窗口, 用户进行了下拉操作, 并已经弹出下拉窗口
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
// 响应外部消息, 当用户在想启动inplace-edit的时候被调用
//  通常在本函数里呼出inplace控件
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
        // 当pEditOptions允许编辑时, 弹出Edit控件
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

                // 通知父窗口, 用户准备开始编辑, Edit控件已经创建
                pControl->SendMessageToParent(pItemArgs->rowIndex, this, pItemArgs->colIndex, ELNM_ITEM_BEGINEDIT);
            }
        }

        //
        // 当pEditOptions不允许编辑时, 弹出List下拉列表控件
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
// 要结束编辑时调用该函数, 与OnBeginEdit对应, 
//  通常该函数用于销毁/隐藏已经Create的Inplace控件
//-----------------------------------------------------------------------------
//
void  CELRecordItem::OnEndEdit (CListCtrlEL* pControl)
{
    // 如果结束控件编辑时, 窗口已经被销毁, 则直接退出, 
    // 但还是没有办法处理本this对象已经被销毁的情况 bug2013.06.19
    ASSERT_BREAK(::IsWindow(pControl->GetSafeHwnd()));

    if (pControl == NULL)
    {
        ASSERT_BREAK(FALSE);
        return;
    }
    

    // bug20130715 把ELNM_ITEM_ENDEDIT 下放到下面具体的inplace控件销毁前
    // 因为要与Begin配对, 否则会造成使用者使用上的麻烦, ELNM_ITEM_BEGINEDIT与ELNM_ITEM_ENDEDIT配对
    // ELNM_ITEM_AFTERLISTDROPDOWN 与 ELNM_ITEM_ENDLISTDROPDOWN配对
    //
    // 通知父窗口, 用户正在结束编辑, 正在销毁控件
    // 目前不能取得rowIndex和colIndex
    //pControl->SendMessageToParent(0, this, 0, ELNM_ITEM_ENDEDIT);

    //
    // 隐藏inplace-List控件
    //
    CELInplaceList*  pInpaceList = pControl->GetInplaceList();
    ASSERT_BREAK(pInpaceList);
    if (pInpaceList->GetSafeHwnd())
    {
        // 通知父窗口, 用户正在结束编辑, 正在销毁控件
        // 目前不能取得rowIndex和colIndex  bug20130715
        pControl->SendMessageToParent(0, this, 0, ELNM_ITEM_ENDLISTDROPDOWN);

        pInpaceList->HideWindow();
        pInpaceList->SetItemArgs(0);
        pInpaceList->DestroyWindow();  // ?? 需要这样吗
    }

    //
    // 隐藏inplace-Edit控件
    //
    CELInplaceEdit*  pEdit = pControl->GetInplaceEdit();
    ASSERT_BREAK(pEdit);
    if (pEdit->GetSafeHwnd())
    {
        // 通知父窗口, 用户正在结束编辑, 正在销毁控件
        // 目前不能取得rowIndex和colIndex  bug20130715
        pControl->SendMessageToParent(0, this, 0, ELNM_ITEM_ENDEDIT);

        pEdit->HideWindow();
        pEdit->SetItemArgs(0);
        pEdit->DestroyWindow();  // ?? 需要这样吗
    }
}
//
//-----------------------------------------------------------------------------
// 响应外部消息, 在inplace-edit中点击确认时, 被调用
//  比如用户点击OK/Enter键, 用户可以在该函数中验证用户输入, 并触发item值改变
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
                // 真实赋值到本item
                OnEditChanged(pItemArgs, strValue);
                bRedraw = TRUE;
            }
        }

        if (bRedraw)
        {
            // 告诉外层窗口, 用户执行了编辑, 改变了值
            pControl->SendMessageToParent(pItemArgs->rowIndex, this, pItemArgs->colIndex, ELNM_ITEM_VALUECHANGED);
        }
    }
}
//
//-----------------------------------------------------------------------------
// 响应外部消息, 在inplace-edit中点击取消编辑时, 被调用, 
//  比如用户按ESC取消当次编辑
//-----------------------------------------------------------------------------
//
void  CELRecordItem::OnCancelEdit (EL_RECORDITEM_ARGS* pItemArgs)
{
    ASSERT_BREAK(pItemArgs && pItemArgs->pControl);

    // 告诉外层窗口, 用户取消了编辑
    pItemArgs->pControl->SendMessageToParent(pItemArgs->rowIndex, this, pItemArgs->colIndex, ELNM_ITEM_EDIT_CANCELED);
}
//
//-----------------------------------------------------------------------------
// 响应外部消息, 在inplace-list中用户选择改变了, 被调用
//-----------------------------------------------------------------------------
//
void  CELRecordItem::OnConstraintChanged (EL_RECORDITEM_ARGS* pItemArgs, CELRecordItemConstraint* pConstraint)
{
    // 告诉外层窗口, 用户进行了某个限制项的选择
    ELNM_INPLACELIST_ARGS  nm;
    ::ZeroMemory(&nm, sizeof(nm));

    nm.pInplaceList = pItemArgs->pControl->GetInplaceList();
    nm.pConstraint = pConstraint;
    nm.pItem = this;
    nm.rowIndex = pItemArgs->rowIndex;
    nm.colIndex = pItemArgs->colIndex;

    pItemArgs->pControl->SendNotifyMessageToOwner(ELNM_ITEM_CONSTRAINTCHANGED, (NMHDR*)&nm);

    // 真实赋值到本item
    OnEditChanged(pItemArgs, pConstraint->m_strConstraint);
}
//
//-----------------------------------------------------------------------------
// 获得当前Item所绑定的inplace-list的数据, 与CELRecordItemConstraint::m_dwData 项对应
//  正确的返回该值可以使得下拉控件弹出时, 自动焦点到对应的选项, 参见Xtreme:TaskListView例子
//-----------------------------------------------------------------------------
//
DWORD  CELRecordItem::GetSelectedConstraintData (EL_RECORDITEM_ARGS* pItemArgs)
{
    return -1;
}
//
//-----------------------------------------------------------------------------
// 实施编辑器的内容改变, 将内容赋值到本item上
//-----------------------------------------------------------------------------
//
void  CELRecordItem::OnEditChanged (EL_RECORDITEM_ARGS* pItemArgs, LPCTSTR szText)
{
    SetCaption(szText);
}
//
//-----------------------------------------------------------------------------
// 编辑器的内容正在被改变, 返回TRUE代表接受用户的修改, 否则返回FALSE
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
// 检测本item是否允许编辑
//  原则上本函数会探测包括本item, 本item所在列col, 还有本Ctrl控件是否可编辑, 
//  来统一得出一个结论, 但由于本控件按简单的方式实现, 所以就省略了其他判断
//-----------------------------------------------------------------------------
//
BOOL  CELRecordItem::IsAllowEdit (EL_RECORDITEM_ARGS* pItemArgs)
{
    return IsEditable();
}
//
//-----------------------------------------------------------------------------
// 获取本item关联的可选项对象
//  本函数会获取本item管理的, 本列col关联的可选项对象, 
//  如果item没有指定可选项, 则使用本列的可选项
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
// 获取本item的标题矩形位置大小
//  输入rcItem是本Item的原始矩形大小
//  标题矩形可能需要去除 Icon, checkbox等位置
//-----------------------------------------------------------------------------
//
CRect  CELRecordItem::CalcCaptionRect (const CRect& rcItem)
{
    CRect  captionRect = rcItem;

    // 去除checkbox位置
    if (GetHasCheckbox())
    {
        captionRect.left += rcItem.Height();
    }

    return captionRect;
}