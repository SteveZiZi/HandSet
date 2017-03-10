#ifndef MESSAGE_RECORD_ITEM_H_
#define MESSAGE_RECORD_ITEM_H_

#include <string>
#include "../scl/SclManager.h"

///
/// @brief
///     ��¼�������ļ�¼������
///
typedef enum {RT_SMV_CTRL = 0, RT_SMV_CHANNEL, RT_GOOSE_CTRL, RT_GOOSE_CHANNEL, RT_GOOSE_SUB_CHANNEL}RECORD_TYPE;

//
//=============================================================================
// ��ѡ���¼��
//=============================================================================
//
class CRecordItemCheck : public CXTPReportRecordItem
{
public:
	CRecordItemCheck(BOOL bCheck = FALSE, RECORD_TYPE eRecordType = RT_SMV_CTRL);

	///
	/// @brief
	///     ��ȡ��ǰCheck״̬��Ӧ����CStrManager�е��ַ���ID
	///
	virtual int GetGroupCaptionID(CXTPReportColumn* pColumn);

	///
	/// @brief
	///     �Ƚ����е�Check״̬
	///
	virtual int Compare(CXTPReportColumn* pColumn, CXTPReportRecordItem* pItem);

	virtual void OnClick(XTP_REPORTRECORDITEM_CLICKARGS* pClickArgs);

	BOOL GetValue();

protected:
	void UpdateSmvCtrlRecord(XTP_REPORTRECORDITEM_CLICKARGS* pClickArgs);

	RECORD_TYPE m_eRecordType;
};

//
//=============================================================================
// AppID���ݼ�¼��
//=============================================================================
//
class CRecordItemAppID:public CXTPReportRecordItemText
{
public:
	CRecordItemAppID(unsigned short appID, RECORD_TYPE eRecordType = RT_SMV_CTRL);

	void OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);

	BOOL OnEditChanging(XTP_REPORTRECORDITEM_ARGS* pItemArgs, CString& rstrNewText);

	unsigned short GetValue();

protected:
	void UpdateSmvCtrlRecord(unsigned short appID);
	void UpdateGooseCtrlRecord(unsigned short appID);

	RECORD_TYPE m_eRecordType;
	unsigned short m_sAppID;
};

//
//=============================================================================
// Mac��ַ��¼��
//=============================================================================
//
class CRecordItemMac:public CXTPReportRecordItemText
{
public:
	CRecordItemMac(unsigned char *mac ,RECORD_TYPE eRecordType = RT_SMV_CTRL);

	void OnDblClick(XTP_REPORTRECORDITEM_CLICKARGS* pClickArgs);

	void GetValue(unsigned char *mac);

protected:
	void UpdateSmvCtrlRecord();
	void UpdateGooseCtrlRecord();

	RECORD_TYPE m_eRecordType;
	CFont       m_cFont;
};

//
//=============================================================================
// һ���ı���¼��
//=============================================================================
//
class CRecordItemText:public CXTPReportRecordItemText
{
public:
	CRecordItemText(const CString & str,RECORD_TYPE eRecordType = RT_SMV_CTRL);

	void OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);

protected:
	void UpdateSmvCtrlRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);
	void UpdateSmvChannelRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);
	void UpdateGooseCtrlRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);
	void UpdateGooseChannelRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);

	RECORD_TYPE m_eRecordType;
};

//
//=============================================================================
// һ����ֵ��¼��
//=============================================================================
//
class CRecordItemNumber:public CXTPReportRecordItemNumber
{
public:
	CRecordItemNumber(const double dVal,RECORD_TYPE eRecordType = RT_SMV_CTRL);

	BOOL OnEditChanging(XTP_REPORTRECORDITEM_ARGS* pItemArgs, CString& rstrNewText);

	void OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);

	void SetRange(const double minVal, const double maxVal);

protected:
	void UpdateSmvCtrlRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, int val, int prevVal);
	void UpdateGooseCtrlRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, int val, int prevVal);
	void UpdateGooseChannelRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, int val, int prevVal);
	void UpdateGooseSubChannelRecord(XTP_REPORTRECORDITEM_ARGS* pItemArgs, int val, int prevVal);
	void UpdateStatusBarText(RECORD_TYPE eRecordType, UINT columnID);

	RECORD_TYPE m_eRecordType;
	double      m_dMinVal;
	double      m_dMaxVal;
	int         m_iLenLimit;
};

//
//=============================================================================
// SMV����¼��
//=============================================================================
//
class CRecordItemSmvPhase : public CXTPReportRecordItem
{
public:
	CRecordItemSmvPhase(CHANNEL_PHASE_SMV smvPhase = CP_SMV_PHASE_UNKNOW);

	CString GetCaption(CXTPReportColumn* pColumn);

	void OnConstraintChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, CXTPReportRecordItemConstraint* pConstraint);

	void SetValue(CHANNEL_PHASE_SMV smvPhase);
	CHANNEL_PHASE_SMV GetValue();

protected:
	void UpdateChannelRecordDesc(const CHANNEL_PHASE_SMV ePrevPhase, const CHANNEL_PHASE_SMV eCurPhase,
		                         SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &channelInfo);

	CHANNEL_PHASE_SMV m_eSmvPhase;   ///< SMVͨ�����
};

inline void CRecordItemSmvPhase::SetValue(CHANNEL_PHASE_SMV smvPhase)
{
	m_eSmvPhase = smvPhase;
}

inline CHANNEL_PHASE_SMV CRecordItemSmvPhase::GetValue()
{
	return m_eSmvPhase;
}

//
//=============================================================================
// SMVͨ�����ͼ�¼��
//=============================================================================
//
class CRecordItemSmvChannelType : public CXTPReportRecordItem
{
public:
	CRecordItemSmvChannelType(CHANNEL_TYPE_SMV channelType = CT_SMV_UNKNOW);

	CString GetCaption(CXTPReportColumn* pColumn);

	void OnConstraintChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, CXTPReportRecordItemConstraint* pConstraint);

	void SetValue(CHANNEL_TYPE_SMV channelType);
	CHANNEL_TYPE_SMV GetValue();

protected:
	void UpdateChannelRecordDesc(CHANNEL_TYPE_SMV prevChannleType, CHANNEL_TYPE_SMV curChannleType,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &channelInfo);

	CHANNEL_TYPE_SMV m_eChannelType;   ///< SMVͨ������
};

inline void CRecordItemSmvChannelType::SetValue(CHANNEL_TYPE_SMV channelType)
{
	m_eChannelType = channelType;
}

inline CHANNEL_TYPE_SMV CRecordItemSmvChannelType::GetValue()
{
	return m_eChannelType;
}

//
//=============================================================================
// Gooseͨ�����ͼ�¼��
//=============================================================================
//
class CRecordItemGooseChannelType : public CXTPReportRecordItem
{
public:
	CRecordItemGooseChannelType(CHANNEL_TYPE_GOOSE eChannelType = CT_GOOSE_UNKNOW, RECORD_TYPE eRecordType = RT_GOOSE_CHANNEL);

	CString GetCaption(CXTPReportColumn* pColumn);

	void OnConstraintChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, CXTPReportRecordItemConstraint* pConstraint);

	void SetValue(CHANNEL_TYPE_GOOSE eChannelType);

	CHANNEL_TYPE_GOOSE GetValue();

protected:
	void UpdateGooseChannelRecord(CHANNEL_TYPE_GOOSE eChannelType);
	void UpdateGooseSubChannelRecord(CHANNEL_TYPE_GOOSE eChannelType);

	CHANNEL_TYPE_GOOSE m_eChannelType;   ///< Gooseͨ������
	RECORD_TYPE        m_eRecordType;
	BOOL               m_bPopulate;
};

inline void CRecordItemGooseChannelType::SetValue(CHANNEL_TYPE_GOOSE eChannelType)
{
	m_eChannelType = eChannelType;
}

inline CHANNEL_TYPE_GOOSE CRecordItemGooseChannelType::GetValue()
{
	return m_eChannelType;
}

#endif // MESSAGE_RECORD_ITME_H_
