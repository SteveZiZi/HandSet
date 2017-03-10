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
// �ͷ�record�����������item����
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
// ��record���һ��item, �����ɺ�, item�������ɸ�recordӵ��
//-----------------------------------------------------------------------------
//
CELRecordItem*  CELRecord::AddItem (CELRecordItem* pItem)
{
    m_arrItems.Add(pItem);
    pItem->m_pRecord = this;                    // ��ʼ����������
    return pItem;
}

//
//-----------------------------------------------------------------------------
// �������ҵ���record��һ���ɽ����item��colIndex, 
//  ���û�з���-1
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
// ���ҵ����ҵ���record���һ���ɽ����item��colIndex
//  ���û�з���-1
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
