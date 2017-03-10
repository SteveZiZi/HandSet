/**
 * \file
 *      dsmrecord_recv.h
 *
 * \brief
 *      ����ҵ����record���ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/7/2
 */

#pragma once

#include "src/main/record/dsmrecord.h"


/**
 *      SMV���� - ���� ͨ����¼
 *
 *  \note
 *      margin����������ֵ
 */
class CDsmSmvPhasorRecord : public CELRecord
{
public:
    CDsmSmvPhasorRecord(CBaseDetectItem* pCtrlBlk, CString csCaption, CString csValue);

    virtual ~CDsmSmvPhasorRecord() {}
};


/**
 *      SMV���� - ֡ժҪ��Ϣ�б�
 *
 *  \note
 *      ʱ�䡢���͡����ȡ�����
 */
class CDsmSmvPackeInfoRecord : public CELRecord
{
public:
    CDsmSmvPackeInfoRecord(LPCTSTR lpszTime, LPCTSTR lpszType, int nLen, int nIndex);
    virtual ~CDsmSmvPackeInfoRecord() {}


public:
    int GetIndex()
    {
        return m_nPacketIndex;
    }

private:
    int          m_nPacketIndex;         ///< ��¼������֡����
};


