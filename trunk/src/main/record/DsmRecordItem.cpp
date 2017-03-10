#include "StdAfx.h"
#include "DsmRecordItem.h"

#include "src/utils/reportcontrol/listctrlel/elrecord.h"

CDsmRecordItem::CDsmRecordItem(CString csCaption /* = L"" */ )
{
    SetCaption(csCaption);
}

CDsmRecordItem::CDsmRecordItem( int nCaption )
{
    CString csCaption;
    csCaption.Format(L"%d", nCaption);
    SetCaption(csCaption);
}

CDsmRecordItem::~CDsmRecordItem(void)
{
}

CDsmRecordItemPassword::CDsmRecordItemPassword( LPCTSTR strValue )
    : CELRecordItemText(strValue)
{

}

CDsmRecordItemPassword::~CDsmRecordItemPassword()
{

}

CString CDsmRecordItemPassword::GetDispCaption()
{
    if(!m_pRecord->IsFocusable())
    {
        return L"";
    }

    if (m_strFormatString == _T("%s"))
    {
        return m_strText;
    }

    CString strCaption;
    strCaption.Format(m_strFormatString, (LPCTSTR)m_strText);

    return strCaption;
}

CDsmRecordItemPasswordNew::CDsmRecordItemPasswordNew( LPCTSTR strValue )
    : m_csPassword(strValue)
{

}

CDsmRecordItemPasswordNew::~CDsmRecordItemPasswordNew()
{

}

CString CDsmRecordItemPasswordNew::GetDispCaption()
{
    if(!m_pRecord->IsFocusable())
    {
        return L"";
    }

    int nLen = m_csPassword.GetLength();

    CString strCaption;

    for(int i = 0; i < nLen; i++)
    {
        strCaption += L"*";
    }

    return strCaption;
}

void CDsmRecordItemPasswordNew::SetPasswordValue( LPCTSTR strPassword )
{
    m_csPassword = strPassword;
}

CString CDsmRecordItemPasswordNew::GetPasswordValue()
{
    return m_csPassword;
}

CDsmRecordItemText::CDsmRecordItemText( LPCTSTR szValue )
: CELRecordItemText(szValue)
{

}

CDsmRecordItemText::~CDsmRecordItemText()
{

}

void CDsmRecordItemText::OnEditChanged( EL_RECORDITEM_ARGS* pItemArgs, LPCTSTR szText )
{
    UNREFERENCED_PARAMETER(pItemArgs);

    CString csText(szText);

    if(!csText.IsEmpty())
    {
        SetValue(szText);
    }
}
