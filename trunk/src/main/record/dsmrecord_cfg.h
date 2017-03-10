/**
 * \file
 *      dsmrecord_cfg.h
 *
 * \brief
 *      ���Ĳ������ü�¼ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
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
///      ��ʶ��goose���ͻ���goose����
///
enum eDsmGooseType
{
    eGooseSend = 0, 
    eGooseRec,
};


///
/// @brief
///      SMV���ͼ�¼��
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
///      SMV���ͼ�¼��
///
class CDsmStatusWordSetRecord : public CELRecord
{
public:
    CDsmStatusWordSetRecord(CString csCaption, CString csContent);
    virtual ~CDsmStatusWordSetRecord(void) {}
};


///
/// @brief
///      SMV��ֱ�����ü�¼��
///
class CDsmBasicACDCSetRecord : public CELRecord
{
public:
    CDsmBasicACDCSetRecord(CString csCaption1, eCurrentType nValue1);//, CString csCaption2, eCurrentType nValue2);
    virtual ~CDsmBasicACDCSetRecord(void) {}
};

///
/// @brief
///      �༭SMV���ƿ�ͨ����¼��
///
class CDsmEditSmvSendChanRecord : public CELRecord
{
public:
    CDsmEditSmvSendChanRecord(CChannel* pChannel, int nIndex);
    virtual ~CDsmEditSmvSendChanRecord(void) {}
};


///
/// @brief
///      �༭SMV���ƿ���Ϣ��¼��
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
///      ��scl��ѡ����ƿ���Ϣ��¼�� 
///
class CDsmSelSmvCtrlFromSclRecord : public CELRecord
{
public:
    CDsmSelSmvCtrlFromSclRecord(CIEC9_2Cb* pIEC92Cb);
    virtual ~CDsmSelSmvCtrlFromSclRecord(void) {}
};


///
/// @brief
///      ��scan��ѡ����ƿ���Ϣ��¼�� 
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
///      GOOSE���ͼ�¼��
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
///      GOOSE�����б༭���ƿ��¼�� �� GOOSE�����в鿴���ƿ��¼��
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
///      ��scl��ѡ����ƿ���Ϣ��¼�� 
///
class CDsmSelGooseCtrlFromSclRecord : public CELRecord
{
public:
    CDsmSelGooseCtrlFromSclRecord(CGooseCb* pGooseCb, eDsmGooseType eGooseType);
    virtual ~CDsmSelGooseCtrlFromSclRecord(void) {}
};

///
/// @brief
///      ��scan��ѡ����ƿ���Ϣ��¼�� 
///
class CDsmSelGooseCtrlFromScanRecord : public CELRecord
{
public:
    CDsmSelGooseCtrlFromScanRecord(CNetDetectItem* pNetDetectItem, eDsmGooseType eGooseType);
    virtual ~CDsmSelGooseCtrlFromScanRecord(void) {}
};


///
/// @brief
///      �༭GOOSE���͵�ͨ����Ϣ��¼��
///
class CDsmEditGooseSendChannelRecord : public CELRecord
{
public:
    CDsmEditGooseSendChannelRecord( CChannel* pChannel, int nIndex, int SubIndex = 0);
    virtual ~CDsmEditGooseSendChannelRecord(void) {}
};


///
/// @brief
///      GOOSE���տ��ƿ��¼��
///
class CDsmBasicGooseRecRecord : public CELRecord
{
public:
    CDsmBasicGooseRecRecord(CString csCaption, CString csContent, CString csMargin = L"");
    virtual ~CDsmBasicGooseRecRecord(void) {}
};

///
/// @brief
///      �༭GOOSE���յ�ͨ����Ϣ��¼�ѡ��ͨ������ӳ�䣩 
///

class CDsmEditGooseRecChannelRecord : public CELRecord
{
public:
    CDsmEditGooseRecChannelRecord( CChannel* pChannel, int nIndex, int nSubIndex = 0);
    virtual ~CDsmEditGooseRecChannelRecord(void) {}
};



///
/// @brief
///      �༭GOOSE���յĿ���ӳ��� 
///
class CDsmViewGooseRecMapRecord : public CELRecord
{
public:
    CDsmViewGooseRecMapRecord(CString strCaption = _T(""), CString strCtrlBlockDesc = _T(""), CString strChanDesc = _T(""));
    virtual ~CDsmViewGooseRecMapRecord(void) {}
};

