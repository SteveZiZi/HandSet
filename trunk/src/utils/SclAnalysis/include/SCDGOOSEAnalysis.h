/// @file
///     SCDGOOSEAnalysis.h
///
/// @brief
///     SCD文件中GOOSE配置信息解析
///
/// @note
///     根据Mac、AppID获取控制块及通道等信息信息的接口只适用于AppID不改变的情况
///     根据地址索引获取控制块及通道等信息信息的接口只适用于iedName、ldInst、cbName、dataSetName不改变的情况
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.2.4
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.02.18     |1.0.3.218  |添加GOOSE的虚端子连线的信息（GOOSE INPUT）的解析          |
///    |2013.05.27     |1.0.4.527  |将加载\卸载接口移至ISCLCacheManager处                     |
///    |2013.06.01     |1.0.4.601  |GOOSE控制块信息中添加所控制数据集的概要信息               |
///    |2013.06.01     |1.0.4.601  |添加数据通道的子通道信息的获取接口                        |
///    |2013.06.20     |1.0.4.620  |添加按地址索引获取控制块、通道、子通道信息的接口          |
///    |2013.06.20     |1.0.4.620  |添加根据通道信息判断通道类型的接口                        |
///    |2013.06.20     |1.0.4.620  |添加修改控制块、通道、子通道信息的接口                    |
///    |2013.07.25     |1.0.5.725  |修正移除指定通道或者子通道时，该通道之后的通道顺序号没有更新的Bug|
///    |2013.07.25     |1.0.5.725  |修正作为Map的Key的关键信息发生改变时，只更新对应的value值却没更新Key值的Bug|
///    |2013.08.07     |1.0.5.807  |获取GOOSE通道信息时，添加对通道的DOI信息不存在时的处理    |
///    |2013.08.08     |1.0.5.808  |修改某逻辑节点是否为LLN0的判断条件                        |
///    |2013.08.13     |1.0.5.813  |修正以AppID和Mac为索引获取全部控制块信息且部分AppID对应的控制块没有定义时信息获取失败的Bug|
///    |2013.08.15     |1.0.5.815  |修改Goose通道和子通道信息的操作（获取、设置、移除）实现，使其支持通道为DA模式且为类型为结构体的情况|
///    |2013.08.16     |1.0.5.816  |修改Goose子通道信息的拷贝和移除操作使用自定义的子通道，防止之后对其信息设置影响其他通道的信息|
///    |2013.08.21     |1.0.5.821  |添加Goose通道信息的添加接口，仅描述信息有效，其他信息设置无效|
///
#ifndef _SCD_GOOSE_ANALYSIS_H_
#define _SCD_GOOSE_ANALYSIS_H_

#include <string>
#include "sclanalysis.h"

///
/// @brief
///     自定义类型(BOOL)
///
typedef int GOOSE_BOOL;

#define GOOSE_TRUE  GOOSE_BOOL(1)

#define GOOSE_FALSE GOOSE_BOOL(0) 

///
/// @brief
///     SCD配置文件中MAC地址大小(字节)
///
#define GOOSEANALYSIS_MAC_SIZE  (6)

///
/// @brief
///    描述GOOSE通道类型信息
///
typedef enum{
	CT_GOOSE_UNKNOW = 0,           ///< 通道类型未知
	CT_GOOSE_STRUCT,               ///< 结构体
	CT_GOOSE_POINT,                ///< 单点
	CT_GOOSE_DOUBLE_POINT,         ///< 双点
	CT_GOOSE_QUALITY,              ///< 品质
	CT_GOOSE_TIME_STAMP,           ///< 时间标签
	CT_GOOSE_ENTRY_TIME,           ///< 条目时间
	CT_GOOSE_INT8,                 ///< 整型（INT8）
	CT_GOOSE_INT16,                ///< 整型（INT16）
	CT_GOOSE_INT24,                ///< 整型（INT24）
	CT_GOOSE_INT32,                ///< 整型（INT32）
	CT_GOOSE_INT128,               ///< 整型（INT128）
	CT_GOOSE_UINT8,                ///< 整型（UINT8）
	CT_GOOSE_UINT16,               ///< 整型（UINT16）
	CT_GOOSE_UINT24,               ///< 整型（UINT24）
	CT_GOOSE_UINT32,               ///< 整型（UINT32）
	CT_GOOSE_FLOAT32,              ///< 单精度
	CT_GOOSE_FLOAT64,              ///< 双精度
	CT_GOOSE_OCTET,                ///< 八位位组字符串
	CT_GOOSE_VISSTRING,            ///< 可视字符串
	CT_GOOSE_UNICODE               ///< UNICODE字符串
}CHANNEL_TYPE_GOOSE;

#pragma region // 数据类型定义

///
/// @brief
///     IED的GOOSE控制块信息(使用gooseMAC和gooseAppID可标识该信息的惟一性)
///
typedef struct  
{
	std::wstring    iedName;                          ///< GOOSE控制块所在的IED的名称
	std::wstring    iedDesc;                          ///< IED描述

	std::wstring    iedAPName;                        ///< GOOSE控制块所在的AccessPoint名称 
	std::wstring    iedAPDesc;                        ///< IED的AccessPoint描述

	std::wstring    iedLdInst;                        ///< GOOSE控制块所在的逻辑设备的实例号
	std::wstring    iedLdDesc;                        ///< IED的LDevice描述

	std::wstring    dataSetName;                      ///< GOOSE控制块所控制数据集的名称
	std::wstring    dataSetDesc;                      ///< GOOSE控制块所控制数据集的描述
	int             channelNum;                       ///< GOOSE控制块所控制数据集的通道数

	unsigned char   gooseMAC[GOOSEANALYSIS_MAC_SIZE]; ///< GOOSE通信组播MAC地址
	unsigned short  gooseAppID;                       ///< GOOSE通信APPID(标准要求全站惟一)
	unsigned short  gooseVlanID;                      ///< GOOSE通信VLAN-ID
	unsigned char   gooseVlanPri;                     ///< GOOSE通信VLAN-PRIORITY
	unsigned int    gooseMinTime;                     ///< GOOSE数据变位后立即补发的时间间隔，以ms为单位
	unsigned int    gooseMaxTime;                     ///< 心跳时间T0,以ms为单位
	std::wstring    cbName;                           ///< 控制块的名称
	std::wstring    cbDesc;                           ///< 控制块的文字描述
	unsigned int    confRev;                          ///< 控制块的配制版本号
	std::wstring    gooseID;                          ///< 控制块ID
	std::wstring    dataSet;                          ///< 应被GOOSE报文发送的数据集的名称
	std::wstring    type;                             ///< 控制块类型，有“GOOSE”和“GSSE”两类
}SCDGOOSEANALYSIS_IEDGOOSE_INFO;

///
/// @brief
///     IED的GOOSE数据集信息(单个通道)
///
typedef struct  
{
	int           idxSeq;                             ///< 数据集信息在DataSet中定义的顺序号
	std::wstring  dataSetName;                        ///< 所属数据集的名称
	std::wstring  dataSetDesc;                        ///< 所属数据集的描述
	std::wstring  ldInst;                             ///< 逻辑设备实例
	std::wstring  lnPrefix;                           ///< 逻辑节点前缀
	std::wstring  lnClass;                            ///< 逻辑节点类型
	std::wstring  lnInst;                             ///< 逻辑节点实例
	std::wstring  lnDesc;                             ///< 逻辑节点描述
	std::wstring  doName;                             ///< 数据对象名称
	std::wstring  doDesc;                             ///< 数据对象描述
	std::wstring  dUsAddr;                            ///< 数据对象中数据属性DU的短地址信息
	std::wstring  dUVal;                              ///< 数据对象中数据属性DU的默认值
	std::wstring  daName;                             ///< 数据属性名称
	std::wstring  daFc;                               ///< 数据属性功能约束
	std::wstring  daBType;                            ///< 数据属性的基本类型
}SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO;

///
/// @brief
///     IED的GOOSE输入项及与之相连的外部信号的信息
///
typedef struct
{
	int           idxSeq_Out;                         ///< 外部信号在Inputs中的顺序号，从0开始
	std::wstring  iedName_Out;                        ///< 外部信号所在的IED的名称
	std::wstring  ldInst_Out;                         ///< 外部信号所在逻辑设备的实例号
	std::wstring  lnName_Out;                         ///< 外部信号所在逻辑节点的名称
	std::wstring  doName_Out;                         ///< 外部信号所标识的DO的名称
	std::wstring  daName_Out;                         ///< 外部信号所标识的DA的名称。其值为空时，表示外部信号为DO的所有数据属性。
	std::wstring  lnDesc_Out;                         ///< 外部信号所在的LN的描述
	std::wstring  doDesc_Out;                         ///< 外部信号所在的DO的描述
	std::wstring  dUDesc_Out;                         ///< 外部信号所在的DO的dU数据属性的Val
	int           idxSeq_In;                          ///< 外部信号在Inputs中的顺序号，从0开始
	std::wstring  iedName_In;                         ///< 外部信号所绑定的内部地址的IED的名称
	std::wstring  ldInst_In;                          ///< 外部信号所绑定的内部地址的逻辑设备的实例号
	std::wstring  lnName_In;                          ///< 外部信号所绑定的内部地址的逻辑节点的名称
	std::wstring  doName_In;                          ///< 外部信号所绑定的内部地址所标识的DO的名称
	std::wstring  daName_In;                          ///< 外部信号所绑定的内部地址所标识的DA的名称。其值为空时，表示外部信号为DO的所有数据属性。
	std::wstring  lnDesc_In;                          ///< 外部信号所绑定的内部地址的LN的描述
	std::wstring  doDesc_In;                          ///< 外部信号所绑定的内部地址的DO的描述
	std::wstring  dUDesc_In;                          ///< 外部信号所绑定的内部地址的DO的dU数据属性的Val
}SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO;

#pragma endregion

///
/// @brief
///     SCD文件GOOSE配置解析接口
///
class ISCDGOOSEAnalysis
{
public:

	ISCDGOOSEAnalysis();
	virtual ~ISCDGOOSEAnalysis();

	///
	/// @brief
	///     获取配置文件中所有IED的已配置通信参数的GOOSE控制块的数量
	///
	/// @param[in]
	///     strFile - SCD配置文件名(带路径)
	///
	/// @return
	///     GOOSE控制块的数量 - 操作成功； -1 - 操作失败
	///
	virtual int GetIEDGOOSENum( void ) = 0;

	///
	/// @brief
	///     获取配置文件中所有已配置通信参数的GOOSE控制块信息
	///
	/// @note
	///     SCL文件中某控制块信息定义出错则该控制块信息不予返回
	///
	/// @param[in]
	///     iedGOOSEInfoArr - 保存GOOSE控制块信息的数组
	/// @param[in]
	///     arrSize         - 数据的最大容量
	///
	/// @return
	///     数组中保存信息的数量 - 操作成功； -1 - 操作失败
	///
	virtual int GetAllIEDGOOSEInfo( 
		SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfoArr,
		const int arrSize ) = 0;

	///
	/// @brief
	///     获取指定MAC和APPID的GOOSE控制块的信息
	///
	/// @note
	///     存在多个相同AppID的控制块时，默认选择第一个
	///
	/// @param[in]
	///     gooseMAC     - GOOSE通信组播MAC地址
	/// @param[in]
	///     gooseAPPID   - GOOSE通信对应的APPID(标准要求全站惟一)
	/// @param[in]
	///     iedGOOSEInfo - 保存GOOSE控制块信息的结构体
	///
	/// @return
	///     0 - 操作成功； -1 - 操作失败
	///
	virtual int GetOneIEDGOOSEInfo( 
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfo ) = 0;

	///
	/// @brief
	///     获取指定MAC、APPID相关的GOOSE控制块所控制的DataSet的通道（FCDA）的数量
	///
	/// @note
	///     存在多个相同AppID的控制块时，默认选择第一个
	///
	/// @param[in]
	///     gooseMAC     - GOOSE通信组播MAC地址
	/// @param[in]
	///     gooseAPPID   - GOOSE通信对应的APPID(标准要求全站惟一)
	///
	/// @return
	///     DataSet中FCDA的数量 - 操作成功； -1 - 操作失败
	///
	virtual int GetIEDGOOSEDataSetNum( 
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID )= 0;

	///
	/// @brief
	///     获取指定MAC、APPID相关的GOOSE控制块所控制的DataSet的所有通道（FCDA）信息
	///
	/// @note
	///     存在多个相同AppID的控制块时，默认选择第一个
	///     SCL文件中某通道信息定义出错则该通道信息不予返回
	///
	/// @param[in]
	///     gooseMAC       - GOOSE通信组播MAC地址
	/// @param[in]
	///     gooseAPPID     - GOOSE通信对应的APPID(标准要求全站惟一)
	// @param[in]
	///     dataSetInfoArr - 保存GOOSE的通道信息的数组
	// @param[in]
	///     arrSize        - 数组的最大容量
	///
	/// @return
	///     数组保存的通道（FCDA）信息的数量 - 操作成功； -1 - 操作失败
	///
	virtual int GetAllIEDGOOSEDataSetInfo(
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr,
		const int arrSize ) = 0;

	///
	/// @brief
	///     获取指定MAC、APPID相关的GOOSE控制块所控制的DataSet的某一顺序号的通道（FCDA）信息
	///
	/// @note
	///     存在多个相同AppID的控制块时，默认选择第一个
	///
	/// @param[in]
	///     gooseMAC       - GOOSE通信组播MAC地址
	/// @param[in]
	///     gooseAPPID     - GOOSE通信对应的APPID(标准要求全站惟一)
	// @param[in]
	///     idxSeq         - 通道的顺序号(从0开始)
	// @param[in]
	///     dataSetInfo    - 包含通道信息的结构体
	///
	/// @return
	///     0 - 操作成功； -1 - 操作失败
	///
	virtual int GetOneIEDGOOSEDataSetInfo(
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		const int            idxSeq,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo ) = 0;

	///
	/// @brief
	///     获取指定MAC、APPID相关的GOOSE控制块所控制的DataSet的某一通道（FCDA）的顺序号
	///
	/// @note
	///     存在多个相同AppID的控制块时，默认选择第一个
	///
	/// @param[in]
	///     gooseMAC       - GOOSE通信组播MAC地址
	/// @param[in]
	///     gooseAPPID     - GOOSE通信对应的APPID(标准要求全站惟一)
	// @param[in]
	///     dataSetInfo    - 包含通道信息的结构体
	///
	/// @return
	///     通道（FCDA）的顺序号 - 操作成功； -1 - 操作失败
	///
	virtual int GetOneIEDGOOSEDataSetIdxSeq(
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo ) = 0;

	///
	/// @brief
	///     获取指定MAC、APPID的GOOSE控制块的控制的某一父通道的子通道的数目
	///
	/// @note
	///     存在多个相同AppID的控制块时，默认选择第一个
	///
	/// @param[in]
	///     gooseMAC       - GOOSE通信组播MAC地址
	/// @param[in]
	///     gooseAPPID     - GOOSE通信对应的APPID(标准要求全站惟一)
	// @param[in]
	///     dataSetInfo    - 父通道信息
	///
	/// @return
	///     子通道数目 - 操作成功； -1 - 操作失败
	///
	virtual int GetIEDGOOSESubChannelNum(
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo) = 0;

	///
	/// @brief
	///     获取指定MAC、APPID的GOOSE控制块的控制的某一父通道的全部子通道的信息
	///
	/// @note
	///     存在多个相同AppID的控制块时，默认选择第一个
	///     SCL文件中某通道信息定义出错则该通道信息不予返回
	///
	/// @param[in]
	///     gooseMAC          - GOOSE通信组播MAC地址
	/// @param[in]
	///     gooseAPPID        - GOOSE通信对应的APPID(标准要求全站惟一)
	// @param[in]
	///     dataSetInfo       - 父通道信息
	// @param[in]
	///     subChannelInfoArr - 保存子通道信息的数组
	// @param[in]
	///     arrSize           - 数组大小
	///
	/// @return
	///     实际的子通道数 - 操作成功； -1 - 操作失败
	///
	virtual int GetAllIEDGOOSESubChannelInfo(
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfoArr,
		const int arrSize ) = 0;

	///
	/// @brief
	///     获取某一IED的所绑定的外部信号（ExtRef，即GOOSE输入信号）的数量
	///
	/// @note
	///     每个装置的LLN0部分含有该装置的GOOSE和采样值输入的连线信息
	///
	/// @param[in]
	///     iedName      - IED的名称
	///
	/// @return
	///     ExtRef的数量：成功；  -1：失败
	///
	virtual int GetOneIEDInputsExtRefNum(
		const std::wstring &iedName) = 0;

	///
	/// @brief
	///     获取指定IED的所绑定的全部外部信号的连线信息
	///
	/// @param[in]
	///     iedName    - IED名称
	/// @param[in]
	///     inputsInfo - 保存外部信号的连线信息的数组
	/// @param[in]
	///     arrSize    - 保存外部信号的连线信息的数组的大小
	///
	/// @return
	///     数组中保存的信息的数量 - 操作成功； -1 - 操作失败
	///
	virtual int GetOneIEDGOINExtRef(
		const std::wstring &iedName,
		SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO *inputsInfo,
		const int arrSize) = 0;

	///
	/// @brief
	///     获取控制某一外部信号输入的GOOSE控制块的信息
	///
	/// @param[in]
	///     inputsInfo   - 外部信号的连线信息
	/// @param[in]
	///     iedGOOSEInfo - 外部信号相关的GOOSE控制块的信息
	///
	/// @return
	///     0 - 操作成功； -1 - 操作失败
	///
	virtual int GetOutAddrGOOSEInfo(
		const SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO &inputsInfo,
		SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfo) = 0;

	///
	/// @brief
	///     获取全部的GOOSE控制块的数目（包含未配置通信参数的控制块）
	///
	/// @return
	///     控制块的数目：操作成功；  -1：失败
	///
	virtual int GetAllIEDGOOSENumByCtrlRef( void ) = 0;

	///
	/// @brief
	///     获取全部的GOOSE控制块的信息（包含未配置通信参数的控制块），通过控制块索引进行遍历查找
	///
	/// @note
	///     SCL文件中某控制块信息定义出错则该控制块信息不予返回
	///     未配置部分进行默认设置
	///
	/// @param[in]
	///     iedGOOSEInfoArr - 保存SMV控制块信息的数组
	/// @param[in]
	///     arrSize         - 数组大小
	///
	/// @return
	///     数组中保存的控制块的数量：操作成功；  -1：失败
	///
	virtual int GetAllIEDGOOSEInfoByCtrlRef(
		SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfoArr, 
		const int arrSize ) = 0;

	///
	/// @brief
	///     设置一个GOOSE控制块，通过控制块索引进行遍历查找
	///
	/// @param[in]
	///     iedGOOSEInfo - GOOSE控制块信息
	///
	/// @return
	///     0：操作成功；  -1：失败
	///
	virtual int SetOneIEDGOOSEInfoByCtrlRef(const SCDGOOSEANALYSIS_IEDGOOSE_INFO &iedGOOSEInfo) = 0;

	///
	/// @brief
	///     获取指定索引地址的GOOSE控制块所控制的DataSet的通道（FCDA）的数量
	///
	/// @param[in]
	///     iedName        - GOOSE控制块所在的IED的名称
	/// @param[in]
	///     ldInst         - GOOSE控制块所在的LDevice的Inst号
	/// @param[in]
	///     cbName         - GOOSE控制块的名称
	///
	/// @return
	///     DataSet中FCDA的数量：操作成功；  -1：失败
	///
	virtual int GetIEDGOOSEDataSetNum(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName) = 0;

	///
	/// @brief
	///     获取指定索引地址的GOOSE控制块所控制的DataSet的所有通道（FCDA）信息
	///
	/// @note
	///     SCL文件中某通道信息定义出错则该通道信息不予返回
	///
	/// @param[in]
	///     iedName        - GOOSE控制块所在的IED的名称
	/// @param[in]
	///     ldInst         - GOOSE控制块所在的LDevice的Inst号
	/// @param[in]
	///     cbName         - GOOSE控制块的名称
	/// @param[in]
	///     dataSetInfoArr - 保存GOOSE的通道信息的数组
	/// @param[in]
	///     arrSize        - 数组的最大容量
	///
	/// @return
	///     数组保存通道信息的数量：操作成功；  -1：失败
	///
	virtual int GetAllIEDGOOSEDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr,
		const int arrSize ) = 0;

	///
	/// @brief
	///     获取指定索引地址的GOOSE控制块所控制的DataSet的某一顺序号的通道（FCDA）信息
	///
	/// @param[in]
	///     iedName        - GOOSE控制块所在的IED的名称
	/// @param[in]
	///     ldInst         - GOOSE控制块所在的LDevice的Inst号
	/// @param[in]
	///     cbName         - GOOSE控制块的名称
	/// @param[in]
	///     idxSeq         - GOOSE通道的顺序号
	/// @param[in]
	///     dataSetInfo    - 指向保存GOOSE的通道信息的结构体的指针
	///
	/// @return
	///     0：操作成功；  -1：失败
	///
	virtual int GetOneIEDGOOSEDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		const int idxSeq,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo) = 0;

	///
	/// @brief
	///     设置Goose控制块所控制数据集中的一个通道（FCDA）
	///
	/// @note
	///     对通道信息中的lnPrefix、lnClass、lnInst、doName、daName、daFc、daBType、dUsAddr属性的设置无效
	///
	/// @param[in]
	///     iedName      - 该通道所在的IED的名称
	/// @param[in]
	///     ldInst       - 该通道所在的LDevice的Inst号
	/// @param[in]
	///     dataSetInfo  - 该通道的索引和描述信息
	/// @param[in]
	///     eChannelType - 该通道所要设置的通道类型
	///
	/// @return
	///     0：操作成功；  -1：失败
	///
	virtual int SetOneIEDGOOSEDataSetInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_GOOSE eChannelType) = 0;

	///
	/// @brief
	///     在Goose控制块所控制数据集末尾添加一个通道（FCDA）
	///
	/// @note
	///     对通道信息中的lnPrefix、lnClass、lnInst、doName、daName、daFc、daBType、dUsAddr属性的设置无效
	///
	/// @param[in]
	///     iedName      - 该通道所在的IED的名称
	/// @param[in]
	///     ldInst       - 该通道所在的LDevice的Inst号
	/// @param[in]
	///     dataSetInfo  - 该通道的索引和描述信息
	/// @param[in]
	///     eChannelType - 该通道的类型
	///
	/// @return
	///     0：操作成功；  -1：失败
	///
	virtual int AddOneIEDGOOSEDataSetInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_GOOSE eChannelType ) = 0;

	///
	/// @brief
	///     在GOOSE通道集的末尾，将该通道集的某一通道的信息拷贝n份
	///
	/// @param[in]
	///     iedName      - 通道所在的IED的名称
	/// @param[in]
	///     ldInst       - 通道所在的LDevice的Inst号
	/// @param[in]
	///     dataSetName  - 通道所在的数据集的名称
	/// @param[in]
	///     idxSeq       - 要拷贝的通道的顺序号（从0开始）
	/// @param[in]
	///     iCopyCount   - 拷贝的数目
	///
	/// @return
	///     0：操作成功；  -1：失败
	///
	virtual int CopyIEDGOOSEDataSetInfoToSetEnd(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &dataSetName,
		int idxSeq,
		int iCopyCount) = 0;

	///
	/// @brief
	///     移除Goose控制块所控制数据集中的一个通道（FCDA）
	///
	/// @param[in]
	///     iedName    - 该通道所在的IED的名称
	/// @param[in]
	///     ldInst     - 该通道所在的LDevice的Inst号
	/// @param[in]
	///     datSetName - 该通道所在的DataSet的名称
	/// @param[in]
	///     idxSeq     - 该通道的顺序号
	///
	/// @return
	///     0：操作成功；  -1：失败
	///
	virtual int RemoveOneIEDGOOSEDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &datSetName,
		const int idxSeq) = 0;

	///
	/// @brief
	///     获取指定索引地址的GOOSE控制块的控制的某一父通道的子通道的数目
	///
	/// @param[in]
	///     iedName        - 该子通道所在的IED的名称
	/// @param[in]
	///     ldInst         - 该子通道所在的LDevice的Inst号
	// @param[in]
	///     dataSetInfo    - 父通道信息
	///
	/// @return
	///     子通道数目 - 操作成功； -1 - 操作失败
	///
	virtual int GetIEDGOOSESubChannelNum(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo) = 0;

	///
	/// @brief
	///     获取指定索引地址的GOOSE控制块的控制的某一父通道的全部子通道的信息
	///
	/// @note
	///     SCL文件中某通道信息定义出错则该通道信息不予返回
	///
	/// @param[in]
	///     iedName           - 该子通道所在的IED的名称
	/// @param[in]
	///     ldInst            - 该子通道所在的LDevice的Inst号
	// @param[in]
	///     dataSetInfo       - 父通道信息
	// @param[in]
	///     subChannelInfoArr - 保存子通道信息的数组
	// @param[in]
	///     arrSize           - 数组大小
	///
	/// @return
	///     实际的子通道数 - 操作成功； -1 - 操作失败
	///
	virtual int GetAllIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfoArr,
		const int arrSize ) = 0;

	///
	/// @brief
	///     获取指定索引地址的GOOSE控制块的控制的某一父通道的某一子通道的信息
	///
	/// @param[in]
	///     iedName           - 该子通道所在的IED的名称
	/// @param[in]
	///     ldInst            - 该子通道所在的LDevice的Inst号
	// @param[in]
	///     dataSetInfo       - 父通道信息
	// @param[in]
	///     idxSeq            - 子通道的顺序号（从0开始）
	// @param[in]
	///     subChannelInfo    - 保存子通道信息的数组
	///
	/// @return
	///     0 - 操作成功； -1 - 操作失败
	///
	virtual int GetOneIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		int idxSeq,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &subChannelInfo ) = 0;

	///
	/// @brief
	///     设置Goose控制块所控制数据集中的某一通道中的一个子通道
	///
	/// @note
	///     父通道为非自定义通道时会自动转换为自定义通道
	///
	/// @param[in]
	///     iedName           - 该子通道所在的IED的名称
	/// @param[in]
	///     ldInst            - 该子通道所在的LDevice的Inst号
	/// @param[in]
	///     channelInfo       - 该子通道所对应的最顶层的通道信息（即控制块所控制数据集中的某一通道）
	/// @param[in]
	///     subChannelIdxList - 从顶层通道下的第一层子通道开始至当前子通道，各子通道的顺序号列表
	/// @param[in]
	///     listSize          - 顺序号列表的大小
	/// @param[in]
	///     eChannelType      - 该子通道所要设置的通道类型（单点、双点...）
	///
	/// @return
	///     0：操作成功；  -1：失败
	///
	virtual int SetOneIEDGOOSESubChannelInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize,
		CHANNEL_TYPE_GOOSE eChannelType) = 0;

	///
	/// @brief
	///     在Goose通道的某一子通道集末尾，将该子通道集中的一个子通道拷贝n份
	///
	/// @param[in]
	///     iedName           - 该子通道所在的IED的名称
	/// @param[in]
	///     ldInst            - 该子通道所在的LDevice的Inst号
	/// @param[in]
	///     channelInfo       - 该子通道所对应的最顶层的通道信息（即控制块所控制数据集中的某一通道）
	/// @param[in]
	///     subChannelIdxList - 从顶层通道下的第一层子通道开始至当前子通道，各子通道的顺序号列表
	/// @param[in]
	///     listSize          - 顺序号列表的大小
	/// @param[in]
	///     iCopyCount        - 拷贝的数量
	///
	/// @return
	///     0：操作成功；  -1：失败
	///
	virtual int CopyIEDGooseSubChannelToEnd(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize,
		int iCopyCount) = 0;

	///
	/// @brief
	///     移除Goose控制块所控制数据集中的某一通道中的一个子通道
	///
	/// @param[in]
	///     iedName           - 该子通道所在的IED的名称
	/// @param[in]
	///     ldInst            - 该子通道所在的LDevice的Inst号
	/// @param[in]
	///     channelInfo       - 该子通道所对应的最顶层的通道信息（即控制块所控制数据集中的某一通道）
	/// @param[in]
	///     subChannelIdxList - 从顶层通道下的第一层子通道开始至当前子通道，各子通道的顺序号列表
	/// @param[in]
	///     listSize          - 顺序号列表的大小
	///
	/// @return
	///     0：操作成功；  -1：失败
	///
	virtual int RemoveOneIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize) = 0;

	///
	/// @brief
	///     判断某AppID的控制块是否存在
	///
	/// @param[in]
	///     appID - 控制块的AppID值
	///
	/// @return
	///     0 - 存在； -1 - 不存在
	///
	virtual int IsAppIDExisted(const unsigned short appID) = 0;

	///
	/// @brief
	///     获取通道的类型信息
	///
	/// @param[in]
	///     channelInfo - goose数据集中的数据通道信息
	///
	/// @return
	///     返回对应的通道类型信息 单电/双点/时间...
	///
	virtual CHANNEL_TYPE_GOOSE GetChannelType(const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO& channelInfo) = 0;

	///
	/// @brief
	///     获取自定义数据类型模板中的DaName信息
	///
	/// @param[in]
	///     eChannelType - goose通道类型
	/// @param[in]
	///     daName       - 保存自定义数据类型模板中的DaName信息
	///
	/// @return
	///     
	///
	virtual void GetDefDataTemplate_DaName(CHANNEL_TYPE_GOOSE eChannelType, std::wstring &daName) = 0;
};


class ISCLCacheManager;

///
/// @brief
///     SCD文件GOOSE配置解析接口接口对象实例
///
/// @param[in]
///     pSclCacheMgr - 指向Scl缓存管理器的指针
///
/// @return
///     SCD文件GOOSE配置解析接口
///
SCLANALYSISDLL_API ISCDGOOSEAnalysis *CreateSCDGOOSEAnalysisImpl( ISCLCacheManager *pSclCacheMgr );

///
/// @brief
///     释放SCD文件goose92配置解析接口对象实例
///
/// @param[in]
///      objPtr - 指向接口对象实例的指针
///
/// @return
///     
///
SCLANALYSISDLL_API void  RelaseSCDGOOSEAnalysisImpl(ISCDGOOSEAnalysis *objPtr);

#endif
