/// @file
///     msg_data_elem.h
///
/// @brief
///     数据元素对象的编解码接口
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.3.18
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.05.14     |1.0.0.514  |区分元素的元素和元素数据值的偏移地址及长度的信息获取     |
///
#ifndef MSG_DATA_ELEM_H_

#define MSG_DATA_ELEM_H_

#include "src\include\stdint.h"

///
/// @brief
///     无Tag值
///
/// @note
///     TAG为TAG_NONE表示一般数据,其他则为TLV数据
///
#define TAG_NONE                  0x00

///
/// @brief
///     Sav91报文中PDU的Tag值
///
#define SAV91_TAG_SAV_PDU         0x80

///
/// @brief
///     Sav91扩展报文（南京新宁光电）中PDU的Tag值
///
#define SAV91_EX_TAG_SAV_PDU      0x80

///
/// @brief
///     Sav92报文中各TLV数据的Tag值
///
#define SAV92_TAG_SAV_PDU         0x60         ///< Sav92报文PDU
#define SAV92_TAG_NO_ASDU         0x80         ///< noASDU（ASDU数目）
#define SAV92_TAG_SECURITY        0x81         ///< security（安全信息）
#define SAV92_TAG_SEQ_ASDU        0xA2         ///< Sequence of ASDU（ASDU序列）
#define SAV92_TAG_ASDU            0x30         ///< ASDU
#define SAV92_TAG_SV_ID           0x80         ///< svID（采样值控制块ID）
#define SAV92_TAG_DATA_SET        0x81         ///< datset（采样数据集）
#define SAV92_TAG_SMP_CNT         0x82         ///< smpCnt（采样计数器）
#define SAV92_TAG_CONF_REV        0x83         ///< confRev（配置版本）
#define SAV92_TAG_REFR_TM         0x84         ///< refrTm（刷新时间）
#define SAV92_TAG_SMP_SYNCH       0x85         ///< smpSynch（同步标识位）
#define SAV92_TAG_SMP_RATE        0x86         ///< smpRate（采样率）
#define SAV92_TAG_SEQ_OF_DATA     0x87         ///< Sequence of Data(采样值序列)

///
/// @brief
///     GOOSE报文中各TLV数据的Tag值
///
#define GOOSE_TAG_GOOSE_PDU       0x61         ///< GOOSE报文的PDU
#define GOOSE_TAG_GOCB_REF        0x80         ///< gocbRef（GOOSE控制块索引）
#define GOOSE_TAG_TIME_ALLOWED    0x81         ///< timeAllowedtoLive（报文允许生存时间）
#define GOOSE_TAG_DATA_SET        0x82         ///< datSet（GOOSE数据集）
#define GOOSE_TAG_GO_ID           0x83         ///< goID（GOOSE报文的唯一性标识）
#define GOOSE_TAG_EVENT_TIME      0x84         ///< t(GOOSE数据变位时间及该时间的品质)
#define GOOSE_TAG_ST_NUM          0x85         ///< stNum（状态序号）
#define GOOSE_TAG_SQ_NUM          0x86         ///< sqNum（报文顺序号）
#define GOOSE_TAG_TEST            0x87         ///< test（检修标识）
#define GOOSE_TAG_CONF_REV        0x88         ///< confRev（配制版本号）
#define GOOSE_TAG_NDS_COM         0x89         ///< ndsCom（GOOSE是否需重置）
#define GOOSE_TAG_NUM_DS_ENTRIES  0x8A         ///< numDatSetEntries（数据集条目数）
#define GOOSE_TAG_ALL_DATA        0xAB         ///< allData（GOOSE数据序列）
#define GOOSE_TAG_SECURITY        0x8C         ///< security（安全信息）

///
/// @brief
///     GOOSE报文中数据序列(allData)的数据类型的Tag值
///
#define GOOSE_DATA_TAG_ARRAY         0xA1      ///< 数组类型
#define GOOSE_DATA_TAG_STRUCT        0xA2      ///< 结构体类型
#define GOOSE_DATA_TAG_BOOLEAN       0x83      ///< 布尔型
#define GOOSE_DATA_TAG_BIT_STR       0x84      ///< 位串型
#define GOOSE_DATA_TAG_INTEGER       0x85      ///< 整型
#define GOOSE_DATA_TAG_UNSIGNED      0x86      ///< 无符号整型
#define GOOSE_DATA_TAG_FLOAT_POINT   0x87      ///< 浮点型
#define GOOSE_DATA_TAG_OCTET_STR     0x89      ///< 8位位组串类型
#define GOOSE_DATA_TAG_VISIBLE_STR   0x8A      ///< 可视串类型
#define GOOSE_DATA_TAG_GENERAL_TIME  0x8B      ///< 广义时间类型
#define GOOSE_DATA_TAG_BINARY_TIME   0x8C      ///< 天数时间类型
#define GOOSE_DATA_TAG_BCD           0x8D      ///< BCD码类型
#define GOOSE_DATA_TAG_BOOLEAN_ARRAY 0x8E      ///< 布尔量数组类型
#define GOOSE_DATA_TAG_OBJ_ID        0x8F      ///< 对象ID类型
#define GOOSE_DATA_TAG_MMS_STR       0x90      ///< MMS串类型
#define GOOSE_DATA_TAG_UTC_TIME      0x91      ///< UTC时间类型

///
/// @brief
///     数据元素的类型
///
typedef enum{DT_BASE_DATA = 0, DT_TLV_DATA} DATA_ELEM_TYPE;

///
/// @brief
///     数据元素的操作接口
///
class MsgDataElem
{
public:
	virtual ~MsgDataElem(){};

	///
	/// @brief
	///     以特定编码规则解码一个数据元素
	///
	/// @param[in]
	///     offset  - 该数据元素在帧中的偏移地址
	/// @param[in]
	///     inData  - 待解码的数据
	/// @param[in]
	///     datSize - 待解码数据的长度
	///
	/// @return
	///     实际的解码数据大小 - 成功； 0 - 失败
	///
	virtual size_t Decode(const size_t offset, const uint8_t *inData, const size_t datSize) = 0;

	///
	/// @brief
	///     根据设置的信息以特定编码规则进行编码，获得一个数据元素
	///
	/// @param[in]
	///     outData - 保存数据元素的数组
	/// @param[in]
	///     datSize - 数组的元素数目
	///
	/// @return
	///     数据元素的实际大小 - 成功； 0 - 失败
	///
	virtual size_t Code(uint8_t *outData, const size_t datSize) = 0;
	
	///
	/// @brief
	///     获取该数据元素的标签值
	///
	/// @return
	///     数据元素的标签值
	///
	virtual uint8_t GetTag() const = 0;

	///
	/// @brief
	///     获取该数据元素在帧中的偏移地址，若为TLV数据，则为该TLV数据的在帧中的偏移地址
	///
	/// @return
	///     数据元素在帧中的偏移地址
	///
	virtual size_t GetElemOffset() const = 0;

	///
	/// @brief
	///     获取该数据元素的长度，若为TLV数据，则为其Value部分的长度
	///
	/// @return
	///     数据元素的长度
	///
	virtual size_t GetElemLen() const = 0;

	///
	/// @brief
	///     获取该数据元素在帧中的偏移地址，若为TLV数据，则为该TLV数据的在帧中的偏移地址
	///
	/// @return
	///     数据元素在帧中的偏移地址
	///
	virtual size_t GetValueOffset() const = 0;

	///
	/// @brief
	///     获取该数据元素的长度，若为TLV数据，则为其Value部分的长度
	///
	/// @return
	///     数据元素的长度
	///
	virtual size_t GetValueLen() const = 0;

	///
	/// @brief
	///     获取该数据元素的值信息，若为TLV数据，则为指向其Value数据
	///
	/// @return
	///     指向元素的值数据的指针;失败则返回NULL
	///
	virtual const uint8_t * GetValue() const = 0;

	///
	/// @brief
	///     设置该数据元素的标签值
	///
	/// @param[in]
	///     tag - 数据元素的标签值
	///
	/// @return
	///     
	///
	virtual void SetTag(const uint8_t tag) = 0;

	///
	/// @brief
	///     设置该数据元素的值信息，若为TLV数据，则为设置其Value值
	///
	/// @param[in]
	///     inData  - 数据元素的值信息
	/// @param[in]
	///     datSize - 数据元素的值信息的长度
	///
	/// @return
	///     
	///
	virtual void SetValue(const uint8_t *inData, const size_t datSize) = 0;
};

///
/// @brief
///     创建数据元素对象的实例
///
/// @return
///     指向MsgDataElem实例的指针
///
MsgDataElem *CreateMsgDataElemImpl(DATA_ELEM_TYPE dt);

///
/// @brief
///     释放数据元素对象的实例
///
/// @param[in]
///     impl - 指向MsgDataElem实例的指针
///
/// @return
///     
///
void ReleaseMsgDataElemImpl(MsgDataElem *impl);

#endif
