#pragma once



#include "ELDefines.h"



//
//-----------------------------------------------------------------------------
// CELRecordItemText
//
//  文本类型的Item, 目前不知道其存在的意义, 但Xtreme里面有, 这里也就加上了
//-----------------------------------------------------------------------------
//
class CELRecordItemText : public CELRecordItem
{


public:
    CELRecordItemText (LPCTSTR szText = _T(""));

public:
    virtual CString  GetDispCaption ();
    virtual CString  GetCaption ();
    virtual void  OnEditChanged (EL_RECORDITEM_ARGS* pItemArgs, LPCTSTR szText);

public:
    void  SetValue (LPCTSTR szText);
    CString  GetValue ();

protected:
    CString                 m_strText;          // Item text value.
};
//
//-----------------------------------------------------------------------------
// 设置原始的文本内容
//-----------------------------------------------------------------------------
//
inline void  CELRecordItemText::SetValue (LPCTSTR szText)
{
    m_strText = szText;
}
//
//-----------------------------------------------------------------------------
// 返回原始的文本内容, (相对于GetCaption, 后者可以进行格式化)
//-----------------------------------------------------------------------------
//
inline CString  CELRecordItemText::GetValue()
{
    return m_strText;
}


//
//-----------------------------------------------------------------------------
// CELRecordItemNumber
//
//  数值型Item
//-----------------------------------------------------------------------------
//
class CELRecordItemNumber : public CELRecordItem
{


public:
    CELRecordItemNumber (double dValue = 0);
    CELRecordItemNumber (double dValue, LPCTSTR strFormat);

public:
    virtual CString  GetDispCaption ();
    virtual CString  GetCaption ();
    virtual void  OnEditChanged (EL_RECORDITEM_ARGS* pItemArgs, LPCTSTR szText);
    virtual BOOL  OnEditChanging (EL_RECORDITEM_ARGS* pItemArgs, CString& rstrNewText);

public:
    double  GetValue ();
    void  SetValue (double dValue);


protected:
    double                  m_dValue;           // Cell value.
};
//
//-----------------------------------------------------------------------------
// 设置原始的数值内容, (相对于GetCaption, 后者可以进行格式化)
//-----------------------------------------------------------------------------
//
inline void  CELRecordItemNumber::SetValue (double dValue)
{
    m_dValue = dValue;
}
inline double  CELRecordItemNumber::GetValue ()
{
    return m_dValue;
}
