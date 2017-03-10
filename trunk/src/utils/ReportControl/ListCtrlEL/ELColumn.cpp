#include "stdafx.h"

#include "ListCtrlEL.h"


CELColumn::CELColumn ()
{
    m_bEditable = FALSE;
    m_nAlignment = EL_CENTER;
    m_nHeaderAlignment = EL_LEFT;
    m_crBkColor = EL_COLOR_DEFAULT;
    m_nMarginLeft = EL_ITEMDRAW_MARGIN;
    m_nMarginRight = EL_ITEMDRAW_MARGIN;
    m_bBoldText = FALSE;
    m_pEditOptions = new CELRecordItemEditOptions();
}


CELColumn::~CELColumn ()
{

    delete m_pEditOptions;
}