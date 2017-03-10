#ifndef MESSAGE_RECORD_H_
#define MESSAGE_RECORD_H_

#include <string>
#include "../scl/SclManager.h"

///
/// @brief
///     �����и���¼�����ı�����ɫ����
///
/************************************************************************************
#define CTRL_RECORD_COLOR             RGB(135,206,250)  ///< ������
#define CTRL_TEXT_COLOR               RGB(0,0,0)
#define CHANNEL_TITLE_RECORD_COLOR    RGB(176,196,222)  ///< ������
#define CHANNEL_TITLE_TEXT_COLOR      RGB(0,0,0)
#define CHANNEL_RECORD_COLOR          RGB(173,216,230)  ///< ǳ��
#define CHANNEL_TEXT_COLOR            RGB(0,0,0)
#define SUBCHANNEL_TITLE_RECORD_COLOR RGB(85,107,47)    ///< �������
#define SUBCHANNEL_TITLE_TEXT_COLOR   RGB(0,0,0)
#define SUBCHANNEL_RECORD_COLOR       RGB(154,205,50)   ///< ����
#define SUBCHANNEL_TEXT_COLOR         RGB(0,0,0)
****************************************************************************************/

//�޸���ɫ

#define CTRL_RECORD_COLOR             RGB(217,223,231)  ///< ������
#define CTRL_TEXT_COLOR               RGB(0,0,0)
#define CHANNEL_TITLE_RECORD_COLOR    RGB(176,196,222)  ///< ������
#define CHANNEL_TITLE_TEXT_COLOR      RGB(0,0,0)
#define CHANNEL_RECORD_COLOR          RGB(205,210,220)  ///< ǳ��
#define CHANNEL_TEXT_COLOR            RGB(0,0,0)
#define SUBCHANNEL_TITLE_RECORD_COLOR RGB(85,107,47)    ///< �������
#define SUBCHANNEL_TITLE_TEXT_COLOR   RGB(0,0,0)
#define SUBCHANNEL_RECORD_COLOR       RGB(154,205,50)   ///< ����
#define SUBCHANNEL_TEXT_COLOR         RGB(0,0,0)


///
/// @brief
///     SMV������SMV���ƿ��¼�ĸ��еı�ʶ�������ţ�
///
#define COLUMN_SMV_CTRL_IDX     0
#define COLUMN_SMV_APPID        1
#define COLUMN_SMV_MAC_ADDR     2
#define COLUMN_SMV_CHANNEL_NUM  3
#define COLUMN_SMV_IED          4
#define COLUMN_SMV_DATASETREF   5
#define COLUMN_SMV_SVID         6
#define COLUMN_SMV_SMPRATE      7
#define COLUMN_SMV_NOFASDU      8
#define COLUMN_SMV_VLANID       9
#define COLUMN_SMV_VLAN_PRI     10
#define COLUMN_SMV_IS_MULTICAST 11
#define COLUMN_SMV_REFRTM_OPT   12
#define COLUMN_SMV_SMPRATE_OPT  13
#define COLUMN_SMV_DATASET_OPT  14
#define COLUMN_SMV_SMPSYNC_OPT  15
#define COLUMN_SMV_SECURITY_OPT 16
#define COLUMN_SMV_DATASET_DESC 17
#define COLUMN_SMV_SVCBREF      18
#define COLUMN_SMV_CONFREV      19

///
/// @brief
///     SMV�����е�ͨ����¼�ĸ��еı�ʶ�������ţ�
///
#define COLUMN_SMV_CHANNEL_IDX  COLUMN_SMV_CTRL_IDX
#define COLUMN_SMV_CHANNEL_TYPE COLUMN_SMV_APPID
#define COLUMN_SMV_PHASE        COLUMN_SMV_MAC_ADDR
#define COLUMN_SMV_LNDESC       COLUMN_SMV_CHANNEL_NUM
#define COLUMN_SMV_DODESC       COLUMN_SMV_IED
#define COLUMN_SMV_DUDESC       COLUMN_SMV_DATASETREF
#define COLUMN_SMV_CHANNEL_REF  COLUMN_SMV_SVID

///
/// @brief
///     GOOSE������GOOSE���ƿ��¼�ĸ��еı�ʶ�������ţ�
///
#define COLUMN_GOOSE_CTRL_IDX     0
#define COLUMN_GOOSE_APPID        1
#define COLUMN_GOOSE_MAC_ADDR     2
#define COLUMN_GOOSE_CHANNEL_NUM  3
#define COLUMN_GOOSE_IED          4
#define COLUMN_GOOSE_DATASETREF   5
#define COLUMN_GOOSE_GOID         6
#define COLUMN_GOOSE_GOCBREF      7
#define COLUMN_GOOSE_VLANID       8
#define COLUMN_GOOSE_VLAN_PRI     9
#define COLUMN_GOOSE_MINTIME      10
#define COLUMN_GOOSE_MAXTIME      11
#define COLUMN_GOOSE_DATASET_DESC 12
#define COLUMN_GOOSE_CONFREV      13

///
/// @brief
///     GOOSE�����е�ͨ����¼�ĸ��еı�ʶ�������ţ�
///
#define COLUMN_GOOSE_CHANNEL_IDX     COLUMN_GOOSE_CTRL_IDX
#define COLUMN_GOOSE_CHANNEL_TYPE    COLUMN_GOOSE_APPID
#define COLUMN_GOOSE_SUB_CHANNEL_NUM COLUMN_GOOSE_MAC_ADDR
#define COLUMN_GOOSE_LNDESC          COLUMN_GOOSE_CHANNEL_NUM
#define COLUMN_GOOSE_DODESC          COLUMN_GOOSE_IED
#define COLUMN_GOOSE_DUDESC          COLUMN_GOOSE_DATASETREF
#define COLUMN_GOOSE_CHANNEL_REF     COLUMN_GOOSE_GOID

///
/// @brief
///     GOOSE�����е���ͨ����¼�ĸ��еı�ʶ�������ţ�
///
#define COLUMN_GOOSE_SUB_CHANNEL_IDX     COLUMN_GOOSE_CTRL_IDX
#define COLUMN_GOOSE_SUB_CHANNEL_TYPE    COLUMN_GOOSE_APPID
#define COLUMN_GOOSE_SUB_SUB_CHANNEL_NUM COLUMN_GOOSE_MAC_ADDR

//
//=============================================================================
// SMV���ƿ��¼
//=============================================================================
//
class CSmvCtrlRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CSmvCtrlRecord)
public:
	CSmvCtrlRecord();

	///
	/// @brief
	///     ���캯��
	///
	/// @param[in]
	///     ctrlIdx  - ���ƿ����
	/// @param[in]
	///     ctrlInfo - ���ƿ���Ϣ
	///
	CSmvCtrlRecord(unsigned int ctrlIdx, SCDSMV92ANALYSIS_IEDSMV_INFO &ctrlInfo);

	virtual ~CSmvCtrlRecord();

	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	///
	/// @brief
	///     ��ȡ�����еĿ��ƿ���Ϣ
	///
	/// @param[in]
	///     pCtrlInfo - ������ƿ���Ϣ��SCDSMV92ANALYSIS_IEDSMV_INFO���ͣ��Ľṹ���ָ��
	///
	void GetRecordInfo(void *pCtrlInfo);
};

//
//=============================================================================
// SMVͨ����Ϣ��¼�ı���
//=============================================================================
//
class CSmvChannelTitleRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CSmvChannelTitleRecord)
public:
	CSmvChannelTitleRecord();
	virtual ~CSmvChannelTitleRecord();

	// ����Ĭ�ϼ�¼
	virtual void CreateDefaultRecord();

	// ��ȡ���ڻ��Ʋ����ļ�¼��ָ��
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);
};

//
//=============================================================================
// SMVͨ����Ϣ��¼������һ��ͨ��
//=============================================================================
//
class CSmvChannelRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CSmvChannelRecord)
public:
	CSmvChannelRecord();

	///
	/// @param[in]
	///     channelIdx  - ͨ�����
	/// @param[in]
	///     cType       - ͨ������
	/// @param[in]
	///     phase       - ͨ�����
	/// @param[in]
	///     iedName     - ����IED������
	/// @param[in]
	///     channelInfo - ͨ����Ϣ
	///
	CSmvChannelRecord(
		unsigned int channelIdx, CHANNEL_TYPE_SMV cType, CHANNEL_PHASE_SMV phase,
		std::wstring &iedName, SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &channelInfo
		);

	virtual ~CSmvChannelRecord();

	// ��ȡ���ڻ��Ʋ����ļ�¼��ָ��
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	///
	/// @brief
	///     ��ȡ�����е�ͨ����Ϣ
	///
	/// @param[in]
	///     pChannelInfo - ����ͨ����Ϣ��SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO���ͣ��Ľṹ���ָ��
	///
	void GetRecordInfo(void *pChannelInfo);
};

//
//=============================================================================
// Goose���ƿ��¼
//=============================================================================
//
class CGooseCtrlRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CGooseCtrlRecord)
public:
	CGooseCtrlRecord();

	///
	/// @param[in]
	///     ctrlIdx  - ���ƿ����
	/// @param[in]
	///     ctrlInfo - ���ƿ���Ϣ
	///
	CGooseCtrlRecord(unsigned int ctrlIdx, SCDGOOSEANALYSIS_IEDGOOSE_INFO &ctrlInfo);

	virtual ~CGooseCtrlRecord();

	// ��ȡ���ڻ��Ʋ����ļ�¼��ָ��
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	///
	/// @brief
	///     ��ȡ�����е�GOOSE���ƿ���Ϣ
	///
	/// @param[in]
	///     pCtrlInfo - ������ƿ���Ϣ��SCDGOOSEANALYSIS_IEDGOOSE_INFO���ͣ��Ľṹ���ָ��
	///
	void GetRecordInfo(void *pCtrlInfo);
};

//
//=============================================================================
// Gooseͨ����Ϣ��¼�ı���
//=============================================================================
//
class CGooseChannelTitleRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CGooseChannelTitleRecord)
public:
	CGooseChannelTitleRecord();
	virtual ~CGooseChannelTitleRecord();

	// ����Ĭ�ϼ�¼
	virtual void CreateDefaultRecord();

	// ��ȡ���ڻ��Ʋ����ļ�¼��ָ��
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);
};

//
//=============================================================================
// Gooseͨ����Ϣ��¼������һ��ͨ��
//=============================================================================
//
class CGooseChannelRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CGooseChannelRecord)
public:
	CGooseChannelRecord();
	///
	/// @param[in]
	///     channelIdx    - ͨ�����
	/// @param[in]
	///     cType         - ͨ������
	/// @param[in]
	///     subChannelNum - ��ͨ����
	/// @param[in]
	///     iedName       - ����IED������
	/// @param[in]
	///     channelInfo   - ͨ����Ϣ
	///
	CGooseChannelRecord(
		unsigned int channelIdx, CHANNEL_TYPE_GOOSE cType, int subChannelNum,
		std::wstring &iedName, SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo
		);

	virtual ~CGooseChannelRecord();

	// ��ȡ���ڻ��Ʋ����ļ�¼��ָ��
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	///
	/// @brief
	///     ��ȡ�����е�GOOSEͨ����Ϣ
	///
	/// @param[in]
	///     pChannelInfo - ����ͨ����Ϣ��SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO���ͣ��Ľṹ���ָ��
	///
	void GetRecordInfo(void *pChannelInfo);
};

//
//=============================================================================
// Goose��ͨ����Ϣ��¼�ı���
//=============================================================================
//
class CGooseSubChannelTitleRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CGooseSubChannelTitleRecord)
public:
	CGooseSubChannelTitleRecord();
	virtual ~CGooseSubChannelTitleRecord();

	// ����Ĭ�ϼ�¼
	virtual void CreateDefaultRecord();

	// ��ȡ���ڻ��Ʋ����ļ�¼��ָ��
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);
};

//
//=============================================================================
// Goose��ͨ����Ϣ��¼������һ����ͨ��
//=============================================================================
//
class CGooseSubChannelRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CGooseSubChannelRecord)
public:
	CGooseSubChannelRecord();
	///
	/// @param[in]
	///     level      - ����ͨ�����ڵĲ㼶����һ����ͨ���Ĳ㼶Ϊ0��
	/// @param[in]
	///     channelIdx - ����ͨ����˳���
	/// @param[in]
	///     cType      - ����ͨ������
	/// @param[in]
	///     subChannelNum - ����ͨ����������ͨ����Ŀ
	///
	CGooseSubChannelRecord(
		 int level, unsigned int channelIdx, CHANNEL_TYPE_GOOSE cType, int subChannelNum 
		);

	virtual ~CGooseSubChannelRecord();

	// ��ȡ���ڻ��Ʋ����ļ�¼��ָ��
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	///
	/// @brief
	///     ��ȡ�����е�GOOSE��ͨ����Ϣ
	///
	/// @param[in]
	///     pChannelInfo - ������ͨ����Ϣ��SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO���ͣ��Ľṹ���ָ��
	///
	void GetRecordInfo(void *pChannelInfo);

	///
	/// @brief
	///     ��ȡ����ͨ�����ڵĲ㼶
	///
	int  GetChannelLevel();

protected:
	int m_iChannelLevel;
};

#endif // MESSAGE_REPORT_H_
