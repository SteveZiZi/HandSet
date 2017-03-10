#ifndef MESSAGE_RECORD_H_
#define MESSAGE_RECORD_H_

#include <string>
#include "../scl/SclManager.h"

///
/// @brief
///     报表中各记录及其文本的颜色设置
///
/************************************************************************************
#define CTRL_RECORD_COLOR             RGB(135,206,250)  ///< 淡天蓝
#define CTRL_TEXT_COLOR               RGB(0,0,0)
#define CHANNEL_TITLE_RECORD_COLOR    RGB(176,196,222)  ///< 淡钢蓝
#define CHANNEL_TITLE_TEXT_COLOR      RGB(0,0,0)
#define CHANNEL_RECORD_COLOR          RGB(173,216,230)  ///< 浅蓝
#define CHANNEL_TEXT_COLOR            RGB(0,0,0)
#define SUBCHANNEL_TITLE_RECORD_COLOR RGB(85,107,47)    ///< 深橄榄绿
#define SUBCHANNEL_TITLE_TEXT_COLOR   RGB(0,0,0)
#define SUBCHANNEL_RECORD_COLOR       RGB(154,205,50)   ///< 黄绿
#define SUBCHANNEL_TEXT_COLOR         RGB(0,0,0)
****************************************************************************************/

//修改配色

#define CTRL_RECORD_COLOR             RGB(217,223,231)  ///< 淡天蓝
#define CTRL_TEXT_COLOR               RGB(0,0,0)
#define CHANNEL_TITLE_RECORD_COLOR    RGB(176,196,222)  ///< 淡钢蓝
#define CHANNEL_TITLE_TEXT_COLOR      RGB(0,0,0)
#define CHANNEL_RECORD_COLOR          RGB(205,210,220)  ///< 浅蓝
#define CHANNEL_TEXT_COLOR            RGB(0,0,0)
#define SUBCHANNEL_TITLE_RECORD_COLOR RGB(85,107,47)    ///< 深橄榄绿
#define SUBCHANNEL_TITLE_TEXT_COLOR   RGB(0,0,0)
#define SUBCHANNEL_RECORD_COLOR       RGB(154,205,50)   ///< 黄绿
#define SUBCHANNEL_TEXT_COLOR         RGB(0,0,0)


///
/// @brief
///     SMV报表中SMV控制块记录的各列的标识（索引号）
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
///     SMV报表中的通道记录的各列的标识（索引号）
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
///     GOOSE报表中GOOSE控制块记录的各列的标识（索引号）
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
///     GOOSE报表中的通道记录的各列的标识（索引号）
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
///     GOOSE报表中的子通道记录的各列的标识（索引号）
///
#define COLUMN_GOOSE_SUB_CHANNEL_IDX     COLUMN_GOOSE_CTRL_IDX
#define COLUMN_GOOSE_SUB_CHANNEL_TYPE    COLUMN_GOOSE_APPID
#define COLUMN_GOOSE_SUB_SUB_CHANNEL_NUM COLUMN_GOOSE_MAC_ADDR

//
//=============================================================================
// SMV控制块记录
//=============================================================================
//
class CSmvCtrlRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CSmvCtrlRecord)
public:
	CSmvCtrlRecord();

	///
	/// @brief
	///     构造函数
	///
	/// @param[in]
	///     ctrlIdx  - 控制块序号
	/// @param[in]
	///     ctrlInfo - 控制块信息
	///
	CSmvCtrlRecord(unsigned int ctrlIdx, SCDSMV92ANALYSIS_IEDSMV_INFO &ctrlInfo);

	virtual ~CSmvCtrlRecord();

	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	///
	/// @brief
	///     获取报表中的控制块信息
	///
	/// @param[in]
	///     pCtrlInfo - 保存控制块信息（SCDSMV92ANALYSIS_IEDSMV_INFO类型）的结构体的指针
	///
	void GetRecordInfo(void *pCtrlInfo);
};

//
//=============================================================================
// SMV通道信息记录的标题
//=============================================================================
//
class CSmvChannelTitleRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CSmvChannelTitleRecord)
public:
	CSmvChannelTitleRecord();
	virtual ~CSmvChannelTitleRecord();

	// 创建默认记录
	virtual void CreateDefaultRecord();

	// 获取基于绘制参数的记录项指标
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);
};

//
//=============================================================================
// SMV通道信息记录，代表一个通道
//=============================================================================
//
class CSmvChannelRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CSmvChannelRecord)
public:
	CSmvChannelRecord();

	///
	/// @param[in]
	///     channelIdx  - 通道序号
	/// @param[in]
	///     cType       - 通道类型
	/// @param[in]
	///     phase       - 通道相别
	/// @param[in]
	///     iedName     - 所属IED的名称
	/// @param[in]
	///     channelInfo - 通道信息
	///
	CSmvChannelRecord(
		unsigned int channelIdx, CHANNEL_TYPE_SMV cType, CHANNEL_PHASE_SMV phase,
		std::wstring &iedName, SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &channelInfo
		);

	virtual ~CSmvChannelRecord();

	// 获取基于绘制参数的记录项指标
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	///
	/// @brief
	///     获取报表中的通道信息
	///
	/// @param[in]
	///     pChannelInfo - 保存通道信息（SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO类型）的结构体的指针
	///
	void GetRecordInfo(void *pChannelInfo);
};

//
//=============================================================================
// Goose控制块记录
//=============================================================================
//
class CGooseCtrlRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CGooseCtrlRecord)
public:
	CGooseCtrlRecord();

	///
	/// @param[in]
	///     ctrlIdx  - 控制块序号
	/// @param[in]
	///     ctrlInfo - 控制块信息
	///
	CGooseCtrlRecord(unsigned int ctrlIdx, SCDGOOSEANALYSIS_IEDGOOSE_INFO &ctrlInfo);

	virtual ~CGooseCtrlRecord();

	// 获取基于绘制参数的记录项指标
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	///
	/// @brief
	///     获取报表中的GOOSE控制块信息
	///
	/// @param[in]
	///     pCtrlInfo - 保存控制块信息（SCDGOOSEANALYSIS_IEDGOOSE_INFO类型）的结构体的指针
	///
	void GetRecordInfo(void *pCtrlInfo);
};

//
//=============================================================================
// Goose通道信息记录的标题
//=============================================================================
//
class CGooseChannelTitleRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CGooseChannelTitleRecord)
public:
	CGooseChannelTitleRecord();
	virtual ~CGooseChannelTitleRecord();

	// 创建默认记录
	virtual void CreateDefaultRecord();

	// 获取基于绘制参数的记录项指标
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);
};

//
//=============================================================================
// Goose通道信息记录，代表一个通道
//=============================================================================
//
class CGooseChannelRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CGooseChannelRecord)
public:
	CGooseChannelRecord();
	///
	/// @param[in]
	///     channelIdx    - 通道序号
	/// @param[in]
	///     cType         - 通道类型
	/// @param[in]
	///     subChannelNum - 子通道数
	/// @param[in]
	///     iedName       - 所属IED的名称
	/// @param[in]
	///     channelInfo   - 通道信息
	///
	CGooseChannelRecord(
		unsigned int channelIdx, CHANNEL_TYPE_GOOSE cType, int subChannelNum,
		std::wstring &iedName, SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo
		);

	virtual ~CGooseChannelRecord();

	// 获取基于绘制参数的记录项指标
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	///
	/// @brief
	///     获取报表中的GOOSE通道信息
	///
	/// @param[in]
	///     pChannelInfo - 保存通道信息（SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO类型）的结构体的指针
	///
	void GetRecordInfo(void *pChannelInfo);
};

//
//=============================================================================
// Goose子通道信息记录的标题
//=============================================================================
//
class CGooseSubChannelTitleRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CGooseSubChannelTitleRecord)
public:
	CGooseSubChannelTitleRecord();
	virtual ~CGooseSubChannelTitleRecord();

	// 创建默认记录
	virtual void CreateDefaultRecord();

	// 获取基于绘制参数的记录项指标
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);
};

//
//=============================================================================
// Goose子通道信息记录，代表一个子通道
//=============================================================================
//
class CGooseSubChannelRecord : public CXTPReportRecord
{
	DECLARE_SERIAL(CGooseSubChannelRecord)
public:
	CGooseSubChannelRecord();
	///
	/// @param[in]
	///     level      - 该子通道所在的层级（第一层子通道的层级为0）
	/// @param[in]
	///     channelIdx - 该子通道的顺序号
	/// @param[in]
	///     cType      - 该子通道类型
	/// @param[in]
	///     subChannelNum - 该子通道包含的子通道数目
	///
	CGooseSubChannelRecord(
		 int level, unsigned int channelIdx, CHANNEL_TYPE_GOOSE cType, int subChannelNum 
		);

	virtual ~CGooseSubChannelRecord();

	// 获取基于绘制参数的记录项指标
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	///
	/// @brief
	///     获取报表中的GOOSE子通道信息
	///
	/// @param[in]
	///     pChannelInfo - 保存子通道信息（SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO类型）的结构体的指针
	///
	void GetRecordInfo(void *pChannelInfo);

	///
	/// @brief
	///     获取该子通道所在的层级
	///
	int  GetChannelLevel();

protected:
	int m_iChannelLevel;
};

#endif // MESSAGE_REPORT_H_
