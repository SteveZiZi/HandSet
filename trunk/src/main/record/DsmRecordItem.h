#pragma once
#include "src/utils/reportcontrol/listctrlel/elrecorditem.h"
#include "src/utils/reportcontrol/listctrlel/elrecorditemtext.h"

class CDsmRecordItem : public CELRecordItem
{
public:
    CDsmRecordItem(CString csCapion = L"");
    CDsmRecordItem(int nCaption);
    virtual ~CDsmRecordItem(void);
};

class CDsmRecordItemText : public CELRecordItemText
{
public:
    CDsmRecordItemText(LPCTSTR szValue);
    virtual ~CDsmRecordItemText();


public:

    virtual void  OnEditChanged (EL_RECORDITEM_ARGS* pItemArgs, LPCTSTR szText);
};




/**
 *      ‘≠√‹¬Îitem
 */

class CDsmRecordItemPassword : public CELRecordItemText
{
public:

    CDsmRecordItemPassword(LPCTSTR strValue);

    virtual ~CDsmRecordItemPassword();

public:
    virtual CString GetDispCaption();
};

/**
 *      –¬√‹¬Îitem
 */
class CDsmRecordItemPasswordNew : public CELRecordItemText
{
public:

    CDsmRecordItemPasswordNew(LPCTSTR strValue);

    virtual ~CDsmRecordItemPasswordNew();

public:
    virtual CString GetDispCaption();

public:

    void SetPasswordValue(LPCTSTR csPassword);

    CString GetPasswordValue();

protected:

    CString         m_csPassword;
};
