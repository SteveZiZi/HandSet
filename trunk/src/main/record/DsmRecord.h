/**
 * \file
 *      DsmRecord.h
 *
 * \brief
*      与接收，发送业务无关的record类头文件

 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/2
 */

#pragma once


#include "src/utils/reportcontrol/listctrlel/elrecord.h"


class CBaseDetectItem;

/**
 *      通用配置的记录项
 *
 *  \note
 *      记录有三列内容
 *      边框、标题、内容
 */
class CDsmRecord : public CELRecord
{
public:
    CDsmRecord(CString csCaption, CString csContent);
    CDsmRecord(CString csCaption, int nContent);
    virtual ~CDsmRecord(void);
};

/**
 *      基本设置记录项
 */
class CDsmBasicRecord : public CELRecord
{
public:

    CDsmBasicRecord(CString csCaption, CString csContent);
    CDsmBasicRecord(CString csCaption, int nContent);
    virtual ~CDsmBasicRecord(void) {}
};

