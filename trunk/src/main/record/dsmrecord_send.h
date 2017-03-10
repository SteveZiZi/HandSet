/**
 * \file
 *      dsmrecord_send.h
 *
 * \brief
 *      ����ҵ������ļ�¼ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/7/2
 */

#pragma once

#include "src/main/record/dsmrecord.h"


class CSMVSendVirtualChannel;

class CStatus;
class CChannel;
enum eSMVChangeItem;
class CStateTestResult;
class CChannelRecvValues;

enum eManualDoublePoint
{
    eMDPInit = 0,
    eMDPOff,
    eMDPOn ,
    eMDPBad,
};


//
//�ֶ����� ��¼��
///
/// @brief
///      smv ���� ��¼��
///

class CDsmManualSmvSendRecord : public CELRecord
{
public:
    CDsmManualSmvSendRecord(CSMVSendVirtualChannel* pVirChannel);

    virtual ~CDsmManualSmvSendRecord() {}
};

///
/// @brief
///      ��������
///

class CDsmManualStepRecord : public CELRecord
{
public:
    CDsmManualStepRecord(CString csCaption, CString csContent);

    virtual ~CDsmManualStepRecord() {}
};


///
/// @brief
///      goose ���� ��¼��
///
class CDsmManualGooseSendRecord : public CELRecord
{
public:
    CDsmManualGooseSendRecord(CChannel* pChannel, int nIndex, int SubIndex = 0);

    virtual ~CDsmManualGooseSendRecord() {}
};



///
/// @brief
///      goose ���� ��¼��
///
class CDsmManualSetRecord : public CELRecord
{
public:
    CDsmManualSetRecord(CString strCaption, CString strContent);
    virtual ~CDsmManualSetRecord(){}
};

///
/// @brief
///      �ֶ����� ����б�
///
class CDsmManualResultListRecord : public CELRecord
{
public:
    CDsmManualResultListRecord(CStateTestResult* pStateTestResult, int nIndex);

    virtual ~CDsmManualResultListRecord(){}
};

///
/// @brief
///      �ֶ����� SMV �仯
///
class CDsmManualResultDetailSmvRecord : public CELRecord
{
public:
    CDsmManualResultDetailSmvRecord(CSMVSendVirtualChannel* pVirChannel, CSMVSendVirtualChannel* pPreVirChannel = NULL);

    virtual ~CDsmManualResultDetailSmvRecord(){}
};

class CDsmManualResultDetailGooseRecord : public CELRecord
{
public:
    CDsmManualResultDetailGooseRecord(CChannel* pChannel, int nIndex, int nSubIndex, CChannel* pPreChannel = NULL);

    virtual ~CDsmManualResultDetailGooseRecord() {}
};

///
/// @brief
///      �ֶ����� DI���������
///
class CDsmManualResultDIActionRecord : public CELRecord
{
public:
    CDsmManualResultDIActionRecord(CChannelRecvValues* pChannelRecvValues);

    virtual ~CDsmManualResultDIActionRecord(){}
};

///
/// @brief
///      SMV�����б�
///
class CDsmManualSmvCtrlListRecord : public CELRecord
{
public:
    CDsmManualSmvCtrlListRecord(CString csIndex, CString csDesc);

    virtual ~CDsmManualSmvCtrlListRecord(){}
};

///
/// @brief
///      SMV״̬��
///
class CDsmManualSmvStatusWordRecord : public CELRecord
{
public:
    CDsmManualSmvStatusWordRecord(CString csIndex, CString csDesc);

    virtual ~CDsmManualSmvStatusWordRecord(){}
};

///
/// @brief
///      SMVͨ��Ʒ��
///
class CDsmManualSmvChanQualityRecord : public CELRecord
{
public:
    CDsmManualSmvChanQualityRecord(CChannel* pChannel, int nIndex);
    virtual ~CDsmManualSmvChanQualityRecord(void) {}
};

///
/// @brief
///      SMVͨ��Ʒ������
///
class CDsmManualQualitySetRecord : public CELRecord
{
public:
    CDsmManualQualitySetRecord(CString csCaption, CString csContent);

    virtual ~CDsmManualQualitySetRecord(){}
};



///
/// @brief
///      ״̬���� ״̬ ��¼��
///
class CDsmStateSequenceRecord : public CELRecord
{
public:
    CDsmStateSequenceRecord(int nIndex, CStatus* pStatus);
    virtual ~CDsmStateSequenceRecord(){}
};


///
/// @brief
///      ״̬���� ���� ��¼��
///
class CDsmStateSetRecord : public CELRecord
{
public:
    CDsmStateSetRecord(CString strCaption = _T(""), CString strContent = _T(""));
    virtual ~CDsmStateSetRecord(){}
};

#if !SHENG_ZE
///
/// @brief
///      ״̬���� ���� DI
///
class CDsmStateSetRecordDI : public CELRecord
{
public:
	CDsmStateSetRecordDI(CString strCaption0 = _T(""),CString strCaption = _T(""), CString strContent = _T(""),CString strCaption1 = _T(""), CString strContent1 = _T(""));
	virtual ~CDsmStateSetRecordDI(){}
};
#endif
///
/// @brief
///      ״̬���� SMV ӳ������ ��¼��
///
class CDsmStateSmvDataRecord : public CELRecord
{
public:
    CDsmStateSmvDataRecord(CSMVSendVirtualChannel* pVirChannel);
    virtual ~CDsmStateSmvDataRecord(){}
};


///
/// @brief
///      ״̬���� г������ ��¼��
///
class CDsmStateHarmonicRecord : public CELRecord
{
public:
    CDsmStateHarmonicRecord(CSMVSendVirtualChannel* pVirChannel);
    virtual ~CDsmStateHarmonicRecord(){}
};

///
/// @brief
///      ״̬���� ���ϼ��� ��¼��
///
class CDsmStateFaultCalcRecord : public CELRecord
{
public:
    CDsmStateFaultCalcRecord(CString strCaption = _T(""), CString strContent = _T(""));
    virtual ~CDsmStateFaultCalcRecord(){}
};

///
/// @brief
///      ״̬���� ���򲹳�ϵ�� ��¼��
///
class CDsmStateFaultCompensateRecord : public CELRecord
{
public:
    CDsmStateFaultCompensateRecord(CString strCaption0 = _T(""), CString strContent0 = _T(""),CString strCaption1 = _T(""), CString strContent1 = _T(""));
    virtual ~CDsmStateFaultCompensateRecord(){}
};

///
/// @brief
///      ״̬���� �迹 ��¼��
///
class CDsmStateFaultImpedanceRecord : public CELRecord
{
public:
    CDsmStateFaultImpedanceRecord(CString strCaption0 = _T(""), CString strContent0 = _T(""),CString strCaption1 = _T(""), CString strContent1 = _T(""));
    virtual ~CDsmStateFaultImpedanceRecord(){}
};

///
/// @brief
///      ״̬���� ����б�
///
class CDsmStateResultListRecord : public CELRecord
{
public:
    CDsmStateResultListRecord(CStateTestResult* pStateTestResult, int nIndex);

    virtual ~CDsmStateResultListRecord(){}
};

///
/// @brief
///      ״̬���� DI���������
///
class CDsmStateResultDIActionRecord : public CELRecord
{
public:
    CDsmStateResultDIActionRecord(CChannelRecvValues* pChannelRecvValues);

    virtual ~CDsmStateResultDIActionRecord(){}
};
