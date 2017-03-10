#pragma once

#include "ELDefines.h"

//
//-----------------------------------------------------------------------------
// CELRecord
//
//  数据层: 负责一行真实数据元素, 可以添加到report管理的数据集合records里面, 
//  可以有父子层次关系, 其作用相当于后台数据仓库, 其存在与显示没有直接关系
//  record不一定会显示在界面上, 或者可以以任何的样式排序或分组显示在界面上, 
//-----------------------------------------------------------------------------
//
class CELRecord
{
public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CELRecord ();
    virtual ~CELRecord ();

    void  SetFocusable (BOOL bFocusable);
    BOOL  IsFocusable ();

public:
    //
    //=============================================================================
    // 控件操作
    //=============================================================================
    //
    void  SetData (DWORD_PTR data);
    DWORD_PTR  GetData ();
    
    int  GetItemCount ();
    CELRecordItem*  GetItem (int nIndex);
	CELRecordItem*  AddItem (CELRecordItem* pItem);

    int  FindLtoRFocusableItemIndex (int colIndexBegin);
    int  FindRtoLFocusableItemIndex (int colIndexBegin);

private:
    //
    //=============================================================================
    // 辅助
    //=============================================================================
    //
    void  RemoveAll ();

private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    CArray<CELRecordItem*, CELRecordItem*>  m_arrItems;  // An internal storage for record items.
    DWORD_PTR               m_data;             // 与本record关联的用户自定义数据
    BOOL                    m_bFocusable;       // 是否可选中/焦点的行
};


//
//-----------------------------------------------------------------------------
// 设置获得与Record关联的用户自定义数据
//-----------------------------------------------------------------------------
//
inline void  CELRecord::SetData (DWORD_PTR data)
{
    m_data = data;
}
inline DWORD_PTR  CELRecord::GetData ()
{
    return m_data;
}
//
//-----------------------------------------------------------------------------
// 获得在record里面的所有item数目
//-----------------------------------------------------------------------------
//
inline int  CELRecord::GetItemCount ()
{
    return m_arrItems.GetCount();
}
//
//-----------------------------------------------------------------------------
// 获得在record里面的指定item对象
//-----------------------------------------------------------------------------
//
inline CELRecordItem*  CELRecord::GetItem (int nIndex)
{
    return m_arrItems.GetAt(nIndex);
}

//
//-----------------------------------------------------------------------------
// 设置该行对象是否可选中
//-----------------------------------------------------------------------------
//
inline void  CELRecord::SetFocusable (BOOL bFocusable)
{
    m_bFocusable = bFocusable;
}
inline BOOL  CELRecord::IsFocusable ()
{
    return m_bFocusable;
}


//
//-----------------------------------------------------------------------------
// CELDefaultRecord
//
//  默认的Record, 里面所有item都是普通item, 用于ClistCtrlEL的简单接口使用
//-----------------------------------------------------------------------------
//
class CELDefaultRecord: public CELRecord
{

public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CELDefaultRecord (int colCount);
    virtual ~CELDefaultRecord ();
};

