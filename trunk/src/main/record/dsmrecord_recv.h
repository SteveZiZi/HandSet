/**
 * \file
 *      dsmrecord_recv.h
 *
 * \brief
 *      接收业务共用record类的头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/2
 */

#pragma once

#include "src/main/record/dsmrecord.h"


/**
 *      SMV接收 - 相量 通道记录
 *
 *  \note
 *      margin、标题名、值
 */
class CDsmSmvPhasorRecord : public CELRecord
{
public:
    CDsmSmvPhasorRecord(CBaseDetectItem* pCtrlBlk, CString csCaption, CString csValue);

    virtual ~CDsmSmvPhasorRecord() {}
};


/**
 *      SMV接收 - 帧摘要信息列表
 *
 *  \note
 *      时间、类型、长度、描述
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
    int          m_nPacketIndex;         ///< 记录关联的帧索引
};


