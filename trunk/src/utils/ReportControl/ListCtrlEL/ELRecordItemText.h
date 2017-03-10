#pragma once



#include "ELDefines.h"



//
//-----------------------------------------------------------------------------
// CELRecordItemText
//
//  �ı����͵�Item, Ŀǰ��֪������ڵ�����, ��Xtreme������, ����Ҳ�ͼ�����
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
// ����ԭʼ���ı�����
//-----------------------------------------------------------------------------
//
inline void  CELRecordItemText::SetValue (LPCTSTR szText)
{
    m_strText = szText;
}
//
//-----------------------------------------------------------------------------
// ����ԭʼ���ı�����, (�����GetCaption, ���߿��Խ��и�ʽ��)
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
//  ��ֵ��Item
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
// ����ԭʼ����ֵ����, (�����GetCaption, ���߿��Խ��и�ʽ��)
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
