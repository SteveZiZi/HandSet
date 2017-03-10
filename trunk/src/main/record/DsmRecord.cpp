/**
 * \file
 *      DsmRecord.cpp
 *
 * \brief
 *      与接收，发送业务无关的record类实现
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/2
 */

#include "StdAfx.h"
#include "DsmRecord.h"


CDsmRecord::CDsmRecord(CString csCaption, CString csContent )
{
    AddItem(new CDsmRecordItem(csCaption));
    AddItem(new CDsmRecordItem(csContent));
}

CDsmRecord::CDsmRecord(CString csCaption, int nContent)
{
    AddItem(new CDsmRecordItem(csCaption));
    AddItem(new CDsmRecordItem(nContent));
}


CDsmRecord::~CDsmRecord(void)
{
}


CDsmBasicRecord::CDsmBasicRecord( CString csCaption, CString csContent )
{
    AddItem(new CELRecordItemText(csCaption));
    AddItem(new CDsmRecordItemText(csContent));
}

CDsmBasicRecord::CDsmBasicRecord( CString csCaption, int nContent )
{
    CString csContent;
    csContent.Format(L"%d", nContent);

    AddItem(new CELRecordItemText(csCaption));
    AddItem(new CDsmRecordItemText(csContent));
}















