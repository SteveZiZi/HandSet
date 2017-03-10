/**
 * \file
 *      dsmrecord_cfg.h
 *
 * \brief
 *      报文参数配置记录头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/2
 */
#pragma once


#include "src/service/datalayer/recv/framerecvstruct.h"


class CChannel;
class CIEC9_2Cb;
class CGooseCb;



///
/// @brief
///      标识是goose发送还是goose接收
///
enum eDsmGooseType
{
    eGooseSend = 0, 
    eGooseRec,
};


///
/// @brief
///      SMV发送记录项
///
class CDsmBasicSmvSendRecord : public CELRecord
{
public:
    CDsmBasicSmvSendRecord(CString csCaption, CString csContent, int nRecordType, CString csMargin = L"");
    CDsmBasicSmvSendRecord(CString csCaption, int nContent, int nRecordType, CString csMargin = L"");
    virtual ~CDsmBasicSmvSendRecord(void) {}
};



///
/// @brief
///      SMV发送记录项
///
class CDsmStatusWordSetRecord : public CELRecord
{
public:
    CDsmStatusWordSetRecord(CString csCaption, CString csContent);
    virtual ~CDsmStatusWordSetRecord(void) {}
};


///
/// @brief
///      SMV交直流设置记录项
///
class CDsmBasicACDCSetRecord : public CELRecord
{
public:
    CDsmBasicACDCSetRecord(CString csCaption1, eCurrentType nValue1);//, CString csCaption2, eCurrentType nValue2);
    virtual ~CDsmBasicACDCSetRecord(void) {}
};

///
/// @brief
///      编辑SMV控制块通道记录项
///
class CDsmEditSmvSendChanRecord : public CELRecord
{
public:
    CDsmEditSmvSendChanRecord(CChannel* pChannel, int nIndex);
    virtual ~CDsmEditSmvSendChanRecord(void) {}
};


///
/// @brief
///      编辑SMV控制块信息记录项
///
class CDsmEditSmvSendCtrlBlockRecord : public CELRecord
{
public:
    CDsmEditSmvSendCtrlBlockRecord(CString csCaption, CString csContent, int nRecordType);
    CDsmEditSmvSendCtrlBlockRecord(CString csCaption, int nContent, int nRecordType);
    virtual ~CDsmEditSmvSendCtrlBlockRecord(void) {}
};


///
/// @brief
///      从scl中选择控制块信息记录项 
///
class CDsmSelSmvCtrlFromSclRecord : public CELRecord
{
public:
    CDsmSelSmvCtrlFromSclRecord(CIEC9_2Cb* pIEC92Cb);
    virtual ~CDsmSelSmvCtrlFromSclRecord(void) {}
};


///
/// @brief
///      从scan中选择控制块信息记录项 
///
class CDsmSelSmvCtrlFromScanRecord : public CELRecord
{
public:
    CDsmSelSmvCtrlFromScanRecord(CNetDetectItem* pNetDetectItem);
    CDsmSelSmvCtrlFromScanRecord(CFt3DetectItem* pFt3DetectItem);
    virtual ~CDsmSelSmvCtrlFromScanRecord(void) {}
};


///
/// @brief
///      GOOSE发送记录项
///
class CDsmBasicGooseSendRecord : public CELRecord
{
public:
    CDsmBasicGooseSendRecord(CString csCaption, CString csContent, CString csMargin = L"");
    CDsmBasicGooseSendRecord(CString csCaption, int nContent, CString csMargin = L"");
    virtual ~CDsmBasicGooseSendRecord(void) {}
};


///
/// @brief
///      GOOSE发送中编辑控制块记录项 和 GOOSE接收中查看控制块记录项
///
class CDsmGooseCtrlInfoRecord : public CELRecord
{
public:
    CDsmGooseCtrlInfoRecord(CString csCaption, CString csContent);
    CDsmGooseCtrlInfoRecord(CString csCaption, int nContent);
    virtual ~CDsmGooseCtrlInfoRecord(void) {}
};

///
/// @brief
///      从scl中选择控制块信息记录项 
///
class CDsmSelGooseCtrlFromSclRecord : public CELRecord
{
public:
    CDsmSelGooseCtrlFromSclRecord(CGooseCb* pGooseCb, eDsmGooseType eGooseType);
    virtual ~CDsmSelGooseCtrlFromSclRecord(void) {}
};

///
/// @brief
///      从scan中选择控制块信息记录项 
///
class CDsmSelGooseCtrlFromScanRecord : public CELRecord
{
public:
    CDsmSelGooseCtrlFromScanRecord(CNetDetectItem* pNetDetectItem, eDsmGooseType eGooseType);
    virtual ~CDsmSelGooseCtrlFromScanRecord(void) {}
};


///
/// @brief
///      编辑GOOSE发送的通道信息记录项
///
class CDsmEditGooseSendChannelRecord : public CELRecord
{
public:
    CDsmEditGooseSendChannelRecord( CChannel* pChannel, int nIndex, int SubIndex = 0);
    virtual ~CDsmEditGooseSendChannelRecord(void) {}
};


///
/// @brief
///      GOOSE接收控制块记录项
///
class CDsmBasicGooseRecRecord : public CELRecord
{
public:
    CDsmBasicGooseRecRecord(CString csCaption, CString csContent, CString csMargin = L"");
    virtual ~CDsmBasicGooseRecRecord(void) {}
};

///
/// @brief
///      编辑GOOSE接收的通道信息记录项（选中通道设置映射） 
///

class CDsmEditGooseRecChannelRecord : public CELRecord
{
public:
    CDsmEditGooseRecChannelRecord( CChannel* pChannel, int nIndex, int nSubIndex = 0);
    virtual ~CDsmEditGooseRecChannelRecord(void) {}
};



///
/// @brief
///      编辑GOOSE接收的开入映射表 
///
class CDsmViewGooseRecMapRecord : public CELRecord
{
public:
    CDsmViewGooseRecMapRecord(CString strCaption = _T(""), CString strCtrlBlockDesc = _T(""), CString strChanDesc = _T(""));
    virtual ~CDsmViewGooseRecMapRecord(void) {}
};

