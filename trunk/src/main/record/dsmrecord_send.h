/**
 * \file
 *      dsmrecord_send.h
 *
 * \brief
 *      发送业务关联的记录头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
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
//手动测试 记录项
///
/// @brief
///      smv 发送 记录项
///

class CDsmManualSmvSendRecord : public CELRecord
{
public:
    CDsmManualSmvSendRecord(CSMVSendVirtualChannel* pVirChannel);

    virtual ~CDsmManualSmvSendRecord() {}
};

///
/// @brief
///      步长设置
///

class CDsmManualStepRecord : public CELRecord
{
public:
    CDsmManualStepRecord(CString csCaption, CString csContent);

    virtual ~CDsmManualStepRecord() {}
};


///
/// @brief
///      goose 发送 记录项
///
class CDsmManualGooseSendRecord : public CELRecord
{
public:
    CDsmManualGooseSendRecord(CChannel* pChannel, int nIndex, int SubIndex = 0);

    virtual ~CDsmManualGooseSendRecord() {}
};



///
/// @brief
///      goose 发送 记录项
///
class CDsmManualSetRecord : public CELRecord
{
public:
    CDsmManualSetRecord(CString strCaption, CString strContent);
    virtual ~CDsmManualSetRecord(){}
};

///
/// @brief
///      手动试验 结果列表
///
class CDsmManualResultListRecord : public CELRecord
{
public:
    CDsmManualResultListRecord(CStateTestResult* pStateTestResult, int nIndex);

    virtual ~CDsmManualResultListRecord(){}
};

///
/// @brief
///      手动试验 SMV 变化
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
///      手动试验 DI开入量结果
///
class CDsmManualResultDIActionRecord : public CELRecord
{
public:
    CDsmManualResultDIActionRecord(CChannelRecvValues* pChannelRecvValues);

    virtual ~CDsmManualResultDIActionRecord(){}
};

///
/// @brief
///      SMV发送列表
///
class CDsmManualSmvCtrlListRecord : public CELRecord
{
public:
    CDsmManualSmvCtrlListRecord(CString csIndex, CString csDesc);

    virtual ~CDsmManualSmvCtrlListRecord(){}
};

///
/// @brief
///      SMV状态字
///
class CDsmManualSmvStatusWordRecord : public CELRecord
{
public:
    CDsmManualSmvStatusWordRecord(CString csIndex, CString csDesc);

    virtual ~CDsmManualSmvStatusWordRecord(){}
};

///
/// @brief
///      SMV通道品质
///
class CDsmManualSmvChanQualityRecord : public CELRecord
{
public:
    CDsmManualSmvChanQualityRecord(CChannel* pChannel, int nIndex);
    virtual ~CDsmManualSmvChanQualityRecord(void) {}
};

///
/// @brief
///      SMV通道品质设置
///
class CDsmManualQualitySetRecord : public CELRecord
{
public:
    CDsmManualQualitySetRecord(CString csCaption, CString csContent);

    virtual ~CDsmManualQualitySetRecord(){}
};



///
/// @brief
///      状态序列 状态 记录项
///
class CDsmStateSequenceRecord : public CELRecord
{
public:
    CDsmStateSequenceRecord(int nIndex, CStatus* pStatus);
    virtual ~CDsmStateSequenceRecord(){}
};


///
/// @brief
///      状态序列 设置 记录项
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
///      状态设置 设置 DI
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
///      状态序列 SMV 映射数据 记录项
///
class CDsmStateSmvDataRecord : public CELRecord
{
public:
    CDsmStateSmvDataRecord(CSMVSendVirtualChannel* pVirChannel);
    virtual ~CDsmStateSmvDataRecord(){}
};


///
/// @brief
///      状态序列 谐波设置 记录项
///
class CDsmStateHarmonicRecord : public CELRecord
{
public:
    CDsmStateHarmonicRecord(CSMVSendVirtualChannel* pVirChannel);
    virtual ~CDsmStateHarmonicRecord(){}
};

///
/// @brief
///      状态序列 故障计算 记录项
///
class CDsmStateFaultCalcRecord : public CELRecord
{
public:
    CDsmStateFaultCalcRecord(CString strCaption = _T(""), CString strContent = _T(""));
    virtual ~CDsmStateFaultCalcRecord(){}
};

///
/// @brief
///      状态序列 零序补偿系数 记录项
///
class CDsmStateFaultCompensateRecord : public CELRecord
{
public:
    CDsmStateFaultCompensateRecord(CString strCaption0 = _T(""), CString strContent0 = _T(""),CString strCaption1 = _T(""), CString strContent1 = _T(""));
    virtual ~CDsmStateFaultCompensateRecord(){}
};

///
/// @brief
///      状态序列 阻抗 记录项
///
class CDsmStateFaultImpedanceRecord : public CELRecord
{
public:
    CDsmStateFaultImpedanceRecord(CString strCaption0 = _T(""), CString strContent0 = _T(""),CString strCaption1 = _T(""), CString strContent1 = _T(""));
    virtual ~CDsmStateFaultImpedanceRecord(){}
};

///
/// @brief
///      状态序列 结果列表
///
class CDsmStateResultListRecord : public CELRecord
{
public:
    CDsmStateResultListRecord(CStateTestResult* pStateTestResult, int nIndex);

    virtual ~CDsmStateResultListRecord(){}
};

///
/// @brief
///      状态序列 DI开入量结果
///
class CDsmStateResultDIActionRecord : public CELRecord
{
public:
    CDsmStateResultDIActionRecord(CChannelRecvValues* pChannelRecvValues);

    virtual ~CDsmStateResultDIActionRecord(){}
};
