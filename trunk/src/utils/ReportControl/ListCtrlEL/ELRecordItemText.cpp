#include "stdafx.h"

#include "ListCtrlEL.h"

//
//=============================================================================
// CELRecordItemText
//=============================================================================
//


CELRecordItemText::CELRecordItemText (LPCTSTR szText): CELRecordItem()
{
    m_strText = szText;
}

//
//-----------------------------------------------------------------------------
// 返回显示在控件上的文本, 该文本经过格式化m_strFormatString
//-----------------------------------------------------------------------------
//
CString  CELRecordItemText::GetDispCaption ()
{
    if (m_strFormatString == _T("%s"))
    {
        return m_strText;
    }

    CString strCaption;
    strCaption.Format(m_strFormatString, (LPCTSTR)m_strText);
    return strCaption;
}
//
//-----------------------------------------------------------------------------
// 返回item本身的关联文本
//-----------------------------------------------------------------------------
//
CString  CELRecordItemText::GetCaption ()
{
    return m_strText;
}
//
//-----------------------------------------------------------------------------
// 改变实际的值
//-----------------------------------------------------------------------------
//
void  CELRecordItemText::OnEditChanged (EL_RECORDITEM_ARGS* pItemArgs, LPCTSTR szText)
{
    SetValue(szText);
}


//
//=============================================================================
// CELRecordItemNumber
//=============================================================================
//



CELRecordItemNumber::CELRecordItemNumber (double dValue): CELRecordItem()
{
    m_dValue = dValue;

    static const CString  cstrNumberFormatDefault(_T("%0.f"));          // to avoid new string data allocation for each record
    m_strFormatString = cstrNumberFormatDefault;
}

CELRecordItemNumber::CELRecordItemNumber (double dValue, LPCTSTR strFormat): CELRecordItem()
{
    m_dValue = dValue;
    m_strFormatString = strFormat;
}

//
//-----------------------------------------------------------------------------
// 返回显示在控件上的文本, 该文本经过格式化m_strFormatString
//-----------------------------------------------------------------------------
//
CString  CELRecordItemNumber::GetDispCaption ()
{
    CString  strCaption;
    strCaption.Format(m_strFormatString, m_dValue);
    return strCaption;
}
//
//-----------------------------------------------------------------------------
// 返回item关联的文本
//-----------------------------------------------------------------------------
//
CString  CELRecordItemNumber::GetCaption ()
{
    CString  strCaption;
    strCaption.Format(L"%lf", m_dValue);
    return strCaption;
}
//
//-----------------------------------------------------------------------------
// 改变实际的值
//-----------------------------------------------------------------------------
//
static double  __StringToDouble (const CString& strText)
{
    double  res;
    swscanf(strText, _T("%lf"), &res);

    return res;
}
void  CELRecordItemNumber::OnEditChanged (EL_RECORDITEM_ARGS* pItemArgs, LPCTSTR szText)
{
    SetValue(__StringToDouble(szText));
}
//
//-----------------------------------------------------------------------------
// 不能输入其他字符, 只能输入数字
//-----------------------------------------------------------------------------
//
BOOL  CELRecordItemNumber::OnEditChanging (EL_RECORDITEM_ARGS* pItemArgs, CString& rstrNewText)
{
    int  dotnum = 0;
    int  nLen = rstrNewText.GetLength();
    for (int i = 0; i < nLen; i++)
    {
        TCHAR  ch = rstrNewText.GetAt(i);
        if (i == 0 && ch == L'-')
        {
            continue;
        }

        if (ch == L'.')
        {
            dotnum++;
            if (dotnum > 1)
            {
                return FALSE;
            }
        }

        if (_istdigit(ch) || ch == L'.')
        {
            continue;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}