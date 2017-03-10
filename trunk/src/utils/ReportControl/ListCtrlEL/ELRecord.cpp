#include "stdafx.h"

#include "ListCtrlEL.h"


//
//=============================================================================
// CELRecord
//=============================================================================
//

CELRecord::CELRecord ()
{
    m_data = NULL;
    m_bFocusable = TRUE;
}


CELRecord::~CELRecord ()
{
    RemoveAll();
}

//
//-----------------------------------------------------------------------------
// 释放record所管理的所有item对象
//-----------------------------------------------------------------------------
//
void CELRecord::RemoveAll()
{
    for (int nItem = m_arrItems.GetSize() - 1; nItem >= 0; nItem--)
    {
        CELRecordItem* pItem = m_arrItems.GetAt(nItem);
        ASSERT_BREAK(pItem);
        
        delete pItem;
    }
    m_arrItems.RemoveAll();
}


//
//-----------------------------------------------------------------------------
// 向record添加一个item, 添加完成后, item生命就由该record拥有
//-----------------------------------------------------------------------------
//
CELRecordItem*  CELRecord::AddItem (CELRecordItem* pItem)
{
    m_arrItems.Add(pItem);
    pItem->m_pRecord = this;                    // 初始化反向索引
    return pItem;
}

//
//-----------------------------------------------------------------------------
// 从左到右找到该record第一个可焦点的item的colIndex, 
//  如果没有返回-1
//-----------------------------------------------------------------------------
//
int  CELRecord::FindLtoRFocusableItemIndex (int colIndexBegin)
{
    for (int i = colIndexBegin; i < m_arrItems.GetSize(); i++)
    {
        if (m_arrItems.GetAt(i)->IsFocusable())
        {
            return i;
        }
    }
    return -1;
}
//
//-----------------------------------------------------------------------------
// 从右到左找到该record最后一个可焦点的item的colIndex
//  如果没有返回-1
//-----------------------------------------------------------------------------
//
int  CELRecord::FindRtoLFocusableItemIndex (int colIndexBegin)
{
    for (int i = colIndexBegin; i >= 0; i--)
    {
        if (m_arrItems.GetAt(i)->IsFocusable())
        {
            return i;
        }
    }
    return -1;
}

//
//=============================================================================
// CELDefaultRecord
//=============================================================================
//

CELDefaultRecord::CELDefaultRecord (int colCount)
{
    for (int i = 0; i < colCount; i++)
    {
        CELRecordItem*  newitem = new CELRecordItem();
        // newitem->SetFormatString(L"my%s");
        // newitem->SetEditable(TRUE);
        AddItem(newitem);
    }
}

CELDefaultRecord::~CELDefaultRecord ()
{

}
