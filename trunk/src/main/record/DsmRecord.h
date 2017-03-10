/**
 * \file
 *      DsmRecord.h
 *
 * \brief
*      ����գ�����ҵ���޹ص�record��ͷ�ļ�

 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/7/2
 */

#pragma once


#include "src/utils/reportcontrol/listctrlel/elrecord.h"


class CBaseDetectItem;

/**
 *      ͨ�����õļ�¼��
 *
 *  \note
 *      ��¼����������
 *      �߿򡢱��⡢����
 */
class CDsmRecord : public CELRecord
{
public:
    CDsmRecord(CString csCaption, CString csContent);
    CDsmRecord(CString csCaption, int nContent);
    virtual ~CDsmRecord(void);
};

/**
 *      �������ü�¼��
 */
class CDsmBasicRecord : public CELRecord
{
public:

    CDsmBasicRecord(CString csCaption, CString csContent);
    CDsmBasicRecord(CString csCaption, int nContent);
    virtual ~CDsmBasicRecord(void) {}
};

