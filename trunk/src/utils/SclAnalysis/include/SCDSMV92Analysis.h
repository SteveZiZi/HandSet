/**
  \file SCDSMV92Analysis.h

  \brief SCD文件中IEC61850-9-2配置信息解析

  \note
      根据Mac、AppID获取控制块及通道等信息信息的接口只适用于AppID不改变的情况
      根据地址索引获取控制块及通道等信息信息的接口只适用于iedName、ldInst、cbName、dataSetName不改变的情况

  \copyright 

  \author  chao

  \version 1.0.x.x

  \note 更改记录
      ： 时间         : 版本      :  详细信息    
      :-------------- :-----------:----------------------------------------------------------
      |2013.01.18     |1.0.1.118  |添加SMV采样控制块信息                                    |
	  |2013.01.20     |1.0.1.120  |将字符串类型由string更改为wstring 提供对多语言的支持     |
	  |2013.01.28     |1.0.2.128  |更改IED的SMV信息结构命名和接口相关注释                   |
      |2013.02.18     |1.0.3.218  |添加采样值的虚端子连线的信息（SMV INPUT）的解析          |
	  |2013.05.27     |1.0.4.527  |将加载\卸载接口移至ISCLCacheManager处                    |
	  |2013.06.01     |1.0.4.620  |SMV控制块信息中添加所控制数据集的概要信息                |
	  |2013.06.20     |1.0.4.620  |添加按地址索引获取控制块、通道信息的接口                 |
	  |2013.06.20     |1.0.4.620  |添加根据通道信息判断通道类型、相别的接口                 |
	  |2013.06.20     |1.0.4.620  |添加修改控制块、通道信息的接口                           |
	  |2013.07.25     |1.0.5.725  |修正移除指定通道时，该通道之后的通道顺序号没有更新的Bug  |
	  |2013.07.25     |1.0.5.725  |修正作为Map的Key的关键信息发生改变时，只更新对应的value值却没更新Key值的Bug|
	  |2013.08.21     |1.0.5.821  |修改SMV通道信息的设置接口，顺序号超出范围不自动添加，且通道信息中仅描述信息有效，其他信息设置无效|
	  |2013.08.21     |1.0.5.821  |添加SMV通道信息的添加接口，仅描述信息有效，其他信息设置无效|
 */
#ifndef _SCDSMV92ANALYSIS_H_
#define _SCDSMV92ANALYSIS_H_

#include <string>
#include "sclanalysis.h"

/**
  \brief 自定义类型(BOOL)
 */
typedef int SMV92_BOOLEAN;

#define SMV92_TRUE  SMV92_BOOLEAN(1)

#define SMV92_FALSE SMV92_BOOLEAN(0) 

/**
  \brief SCD配置文件中MAC地址大小(字节)
 */
#define SCDSMV92ANALYSIS_MAC_SIZE  (6)

///
/// @brief
///    描述SMV通道类型信息
///
typedef enum{
	CT_SMV_UNKNOW = 0,             ///< 通道类型未知
	CT_SMV_TIME,                   ///< 通道类型为时间
	CT_SMV_CURRENT,                ///< 通道类型为电流
	CT_SMV_VOLTAGE                 ///< 通道类型为电压
}CHANNEL_TYPE_SMV;

///
/// @brief
///    描述通道相别信息
///
typedef enum{
	CP_SMV_PHASE_UNKNOW = 0,       ///< 通道相别未知
	CP_SMV_PHASE_A,                ///< 通道相别为A相
	CP_SMV_PHASE_B,                ///< 通道相别为B相
	CP_SMV_PHASE_C,                ///< 通道相别为C相
	CP_SMV_PHASE_N,                ///< 通道相别为N相，中性点/零序
	CP_SMV_PHASE_X                 ///< 通道相别为X相，同期/母线/抽取
}CHANNEL_PHASE_SMV;

#pragma region // 数据类型定义

/**
  \brief IED的SMV控制块信息(使用smvMAC和smvAppID可标识该信息的惟一性)
 */
typedef struct  
{
    std::wstring    iedName;                           /**< IED名称(关联SMV配置与IED配置) */
    std::wstring    iedDesc;                           /**< IED描述 */
    std::wstring    iedAPName;                         /**< IED的AccessPoint名称 
                                                           (关联SMV配置与IED的AccessPoint)*/
    std::wstring    iedAPDesc;                         /**< IED的AccessPoint描述 */
    std::wstring    iedLdInst;                         /**< IED逻辑设备实例名称 
                                                           (关联SMV配置与IED的LDevie) */
    std::wstring    iedLdDesc;                         /**< IED的LDevice描述 */

	std::wstring    dataSetName;                       /**< SMV控制块所控制数据集的名称 */
	std::wstring    dataSetDesc;                       /**< SMV控制块所控制数据集的描述 */
	int             channelNum;                        /**< SMV控制块所控制数据集的通道数 */

    unsigned char   smvMAC[SCDSMV92ANALYSIS_MAC_SIZE]; /**< SMV通信组播MAC地址 */
    unsigned short  smvAppID;                          /**< SMV通信APPID(标准要求全站惟一) */
    unsigned short  smvVlanID;                         /**< SMV通信VLAN-ID */
    unsigned char   smvVlanPri;                        /**< SMV通信VLAN-PRIORITY */
    std::wstring    smvDesc;                           /**< SMV通信描述 */
    std::wstring    smvCbName;                         /**< SMV采样控制块名称 */
    unsigned int    smvCbConfRev;                      /**< SMV采样控制块配置版本 */
    SMV92_BOOLEAN   smvCbMulticast;                    /**< SMV采样值是否广播 */
    std::wstring    smvCbDataSet;                      /**< SMV采样数据集名称(关联数据集定义) */
    unsigned int    smvCbNofASDU;                      /**< SMV采样数据包中ASDU的个数 */
    unsigned int    smvCbSmpRate;                      /**< SMV采样率 */
    std::wstring    smvCbSmvID;                        /**< SMV采样ID */
    SMV92_BOOLEAN   smvCbOptRefreshTime;               /**< SMV采样刷新时间可选项设置 */
    SMV92_BOOLEAN   smvCbOptSmpRate;                   /**< SMV采样率可选项设置 */
	SMV92_BOOLEAN   smvCbOptDataRef;                   /**< SMV数据集索引可选项设置 */
    SMV92_BOOLEAN   smvCbOptSmpSync;                   /**< SMV采样同步可选项设置 */
    SMV92_BOOLEAN   smvCbOptSecurity;                  /**< SMV采样安全可选项设置 */

}SCDSMV92ANALYSIS_IEDSMV_INFO;

/**
  \brief IED的SMV数据集信息（单个通道）
 */
typedef struct  
{
    int idxSeq;                /**< 数据集信息在DataSet中定义的顺序号 */
    std::wstring  dataSetName; /**< 所属数据集的名称 */
    std::wstring  dataSetDesc; /**< 所属数据集的描述 */
    std::wstring  ldInst;      /**< 逻辑设备实例 */
    std::wstring  lnPrefix;    /**< 逻辑节点前缀 */
    std::wstring  lnClass;     /**< 逻辑节点类型 */
    std::wstring  lnInst;      /**< 逻辑节点实例 */
	std::wstring  lnDesc;      /**< 逻辑节点描述 */
    std::wstring  doName;      /**< 数据对象名称 */
    std::wstring  doDesc;      /**< 数据对象描述 */
    std::wstring  daFc;        /**< 数据属性功能约束 */
    std::wstring  daName;      /**< 数据属性名称 */
    std::wstring  dasAddr;     /**< 数据属性地址信息 */
    std::wstring  daVal;       /**< 数据属性默认值 */

}SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO;

/**
\brief IED的SMV输入项及与之相连的外部信号的信息
*/
typedef struct
{
	int           idxSeq_Out;                          /**< 外部信号在Inputs中的顺序号，从0开始 */
	std::wstring  iedName_Out;                         /**< 外部信号所在的IED的名称 */
	std::wstring  ldInst_Out;                          /**< 外部信号所在逻辑设备的实例号 */
	std::wstring  lnName_Out;                          /**< 外部信号所在逻辑节点的名称 */
	std::wstring  doName_Out;                          /**< 外部信号所标识的DO的名称 */
	std::wstring  daName_Out;                          /**< 外部信号所标识的DA的名称。其值为空时，表示外部信号为DO的所有数据属性。 */
	std::wstring  lnDesc_Out;                          /**< 外部信号所在的LN的描述 */
	std::wstring  doDesc_Out;                          /**< 外部信号所在的DO的描述 */
	std::wstring  dUDesc_Out;                          /**< 外部信号所在的DO的dU数据属性的Val */
	int           idxSeq_In;                           /**< 外部信号在Inputs中的顺序号，从0开始 */
	std::wstring  iedName_In;                          /**< 外部信号所绑定的内部地址的IED的名称 */
	std::wstring  ldInst_In;                           /**< 外部信号所绑定的内部地址的逻辑设备的实例号 */
	std::wstring  lnName_In;                           /**< 外部信号所绑定的内部地址的逻辑节点的名称 */
	std::wstring  doName_In;                           /**< 外部信号所绑定的内部地址所标识的DO的名称 */
	std::wstring  daName_In;                           /**< 外部信号所绑定的内部地址所标识的DA的名称。其值为空时，表示外部信号为DO的所有数据属性。 */
	std::wstring  lnDesc_In;                           /**< 外部信号所绑定的内部地址的LN的描述 */
	std::wstring  doDesc_In;                           /**< 外部信号所绑定的内部地址的DO的描述 */
	std::wstring  dUDesc_In;                           /**< 外部信号所绑定的内部地址的DO的dU数据属性的Val */
}SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO;

#pragma endregion

/**
  \brief SCD文件SMV92配置解析接口
 */
class ISCDSMV92Analysis
{
public:
    
	ISCDSMV92Analysis();
    virtual ~ISCDSMV92Analysis();

	///
	/// @brief
	///     获取配置文件中所有的SMV控制块的数量（不包含未配置通信参数的控制块）
	///
	/// @return
	///     SMV控制块的数量 - 操作成功； -1 - 操作失败
	///
    virtual int GetIEDSMVNum( void ) = 0;

	///
	/// @brief
	///     获取配置文件中所有的SMV控制块信息（不包含未配置通信参数的控制块）
	///
	/// @note
	///     SCL文件中某控制块信息定义出错则该控制块信息不予返回
	///
	/// @param[in]
	///     iedSMVInfoArr - 保存SMV控制块信息的数组
	/// @param[in]
	///     arrSize       - 数据的最大容量
	///
	/// @return
	///     数组中保存信息的数量：操作成功；  -1：操作失败
	///
    virtual int GetAllIEDSMVInfo( 
                 SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfoArr, 
                 const int arrSize ) = 0;

	///
	/// @brief
	///     获取指定MAC和APPID的SMV控制块信息
	///
	/// @note
	///     存在多个相同AppID的控制块时，默认选择第一个
	///
	/// @param[in]
	///     smvMAC     - SMV通信组播MAC地址
	/// @param[in]
	///     smvAPPID   - SMV通信对应的APPID(标准要求全站惟一)
	/// @param[in]
	///     iedSMVInfo - 保存SMV控制块信息的结构体
	///
	/// @return
	///     0：操作成功；  -1：失败
	///
    virtual int GetOneIEDSMVInfo( 
                const unsigned char *smvMAC, 
                const unsigned short smvAppID,
                SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfo ) = 0;

	///
	/// @brief
	///     获取指定MAC、APPID相关的SMV控制块所控制的DataSet的通道（FCDA）的数量
	///
	/// @note
	///     存在多个相同AppID的控制块时，默认选择第一个
	///
	/// @param[in]
	///     smvMAC     - SMV通信组播MAC地址
	/// @param[in]
	///     smvAPPID   - SMV通信对应的APPID(标准要求全站惟一)
	///
	/// @return
	///     DataSet中FCDA的数量：操作成功；  -1：失败
	///
    virtual int GetIEDSMVDataSetNum( 
                    const unsigned char *smvMAC, 
                    const unsigned short smvAppID )= 0;

	///
	/// @brief
	///     获取指定MAC、APPID相关的SMV控制块所控制的DataSet的所有通道（FCDA）信息
	///
	/// @note
	///     存在多个相同AppID的控制块时，默认选择第一个
	///     SCL文件中某通道信息定义出错则该通道信息不予返回
	///
	/// @param[in]
	///     smvMAC         - SMV通信组播MAC地址
	/// @param[in]
	///     smvAPPID       - SMV通信对应的APPID(标准要求全站惟一)
	/// @param[in]
	///     dataSetInfoArr - 保存SMV的通道信息的数组
	/// @param[in]
	///     arrSize        - 数组的最大容量
	///
	/// @return
	///     数组保存通道信息的数量：操作成功；  -1：失败
	///
    virtual int GetAllIEDSMVDataSetInfo(
                    const unsigned char *smvMAC, 
                    const unsigned short smvAppID,
                    SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfoArr,
                    const int arrSize ) = 0;

	///
	/// @brief
	///     获取指定MAC、APPID相关的SMV控制块所控制的DataSet的某一顺序号的通道（FCDA）信息
	///
	/// @note
	///     存在多个相同AppID的控制块时，默认选择第一个
	///
	/// @param[in]
	///     smvMAC      - SMV通信组播MAC地址
	/// @param[in]
	///     smvAPPID    - SMV通信对应的APPID(标准要求全站惟一)
	/// @param[in]
	///     idxSeq      - 通道的顺序号(从0开始)
	/// @param[in]
	///     dataSetInfo - 通道信息
	///
	/// @return
	///     0：操作成功；  -1：失败
	///
    virtual int GetOneIEDSMVDataSetInfo(
                    const unsigned char *smvMAC, 
                    const unsigned short smvAppID,
                    const int            idxSeq,
                    SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo ) = 0;

	///
	/// @brief
	///     获取指定MAC、APPID相关的SMV控制块所控制的DataSet的某一通道（FCDA）的顺序号
	///
	/// @note
	///     存在多个相同AppID的控制块时，默认选择第一个
	///
	/// @param[in]
	///     smvMAC      - SMV通信组播MAC地址
	/// @param[in]
	///     smvAPPID    - SMV通信对应的APPID(标准要求全站惟一)
	/// @param[in]
	///     dataSetInfo - 包含通道信息的结构体
	///
	/// @return
	///     通道的顺序号：操作成功；  -1：失败
	///
    virtual int GetOneIEDSMVDataSetIdxSeq(
                    const unsigned char *smvMAC, 
                    const unsigned short smvAppID,
                    const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo ) = 0;

	///
	/// @brief
	///     获取某一IED的所绑定的外部信号（ExtRef，即SMV输入信号）的数量
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
	virtual int GetOneIEDSVINExtRef(
		const std::wstring &iedName,
		SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO *inputsInfo,
		const int arrSize) = 0;

	///
	/// @brief
	///     获取控制某一外部信号输入的SMV控制块的信息
	///
	/// @param[in]
	///     inputsInfo - 外部信号的连线信息
	/// @param[in]
	///     iedSMVInfo - 控制该外部信号输入的SMV控制块的信息
	///
	/// @return
	///     0 - 操作成功； -1 - 操作失败
	///
	virtual int GetOutAddrSMVInfo(
		const SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO &inputsInfo,
		SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfo) = 0;

	///
	/// @brief
	///     获取全部的SMV控制块的数目（包含未配置通信参数的控制块）
	///
	/// @return
	///     控制块的数目：操作成功；  -1：失败
	///
	virtual int GetAllIEDSMVNumByCtrlRef( void ) = 0;

	///
	/// @brief
	///     获取全部的SMV控制块的信息（包含未配置通信参数的控制块），通过控制块索引进行遍历查找
	///
	/// @note
	///     SCL文件中某控制块信息定义出错则该控制块信息不予返回
	///     未配置部分进行默认设置
	///
	/// @param[in]
	///     iedSMVInfoArr - 保存SMV控制块信息的数组
	/// @param[in]
	///     arrSize       - 数组大小
	///
	/// @return
	///     数组中保存的控制块的数量：操作成功；  -1：失败
	///
	virtual int GetAllIEDSMVInfoByCtrlRef(
		SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfoArr, 
		const int arrSize ) = 0;

	///
	/// @brief
	///     设置一个SMV控制块，通过控制块索引进行遍历查找
	///
	/// @param[in]
	///     iedSMVInfo - SMV控制块信息
	///
	/// @return
	///     0：操作成功；  -1：失败
	///
	virtual int SetOneIEDSMVInfoByCtrlRef(const SCDSMV92ANALYSIS_IEDSMV_INFO &iedSMVInfo) = 0;

	///
	/// @brief
	///     获取指定索引地址的SMV控制块所控制的DataSet的通道（FCDA）的数量
	///
	/// @param[in]
	///     iedName        - SMV控制块所在的IED的名称
	/// @param[in]
	///     ldInst         - SMV控制块所在的LDevice的Inst号
	/// @param[in]
	///     cbName         - SMV控制块的名称
	///
	/// @return
	///     DataSet中FCDA的数量：操作成功；  -1：失败
	///
	virtual int GetIEDSMVDataSetNum(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName) = 0;

	///
	/// @brief
	///     获取指定索引地址的SMV控制块所控制的DataSet的所有通道（FCDA）信息
	///
	/// @param[in]
	///     iedName        - SMV控制块所在的IED的名称
	/// @param[in]
	///     ldInst         - SMV控制块所在的LDevice的Inst号
	/// @param[in]
	///     cbName         - SMV控制块的名称
	/// @param[in]
	///     dataSetInfoArr - 保存SMV的通道信息的数组
	/// @param[in]
	///     arrSize        - 数组的最大容量
	///
	/// @return
	///     数组保存通道信息的数量：操作成功；  -1：失败
	///
	virtual int GetAllIEDSMVDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfoArr,
		const int arrSize ) = 0;

	///
	/// @brief
	///     获取指定索引地址的SMV控制块所控制的DataSet的某一顺序号的通道（FCDA）信息
	///
	/// @param[in]
	///     iedName        - SMV控制块所在的IED的名称
	/// @param[in]
	///     ldInst         - SMV控制块所在的LDevice的Inst号
	/// @param[in]
	///     cbName         - SMV控制块的名称
	/// @param[in]
	///     idxSeq         - SMV通道的顺序号
	/// @param[in]
	///     dataSetInfo    - 指向保存SMV的通道信息的结构体的指针
	///
	/// @return
	///     0：操作成功；  -1：失败
	///
	virtual int GetOneIEDSMVDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		const int idxSeq,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo) = 0;

	///
	/// @brief
	///     设置SMV控制块所控制数据集中的一个通道（FCDA）
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
	virtual int SetOneIEDSMVDataSetInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_SMV eChannelType ) = 0;

	///
	/// @brief
	///     在SMV控制块所控制数据集末尾添加一个通道（FCDA）
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
	virtual int AddOneIEDSMVDataSetInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_SMV eChannelType ) = 0;

	///
	/// @brief
	///     在SMV通道集的末尾，将该通道集的某一通道的信息拷贝n份
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
	virtual int CopyIEDSMVDataSetInfoToSetEnd(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &dataSetName,
		int idxSeq, int iCopyCount ) = 0;

	///
	/// @brief
	///     移除SMV控制块所控制数据集中的一个通道
	///
	/// @param[in]
	///     iedName    - 该通道（FCDA）的定义信息所在的IED的名称
	/// @param[in]
	///     ldInst     - 该通道（FCDA）的定义信息所在的LDevice的Inst号
	/// @param[in]
	///     datSetName - 该通道（FCDA）的定义信息所在的DataSet的名称
	/// @param[in]
	///     idxSeq     - 该通道（FCDA）的顺序号
	///
	/// @return
	///     0：操作成功；  -1：失败
	///
	virtual int RemoveOneIEDSMVDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &datSetName,
		const int idxSeq) = 0;

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
	///     stSmvChannelInfo - 采样值数据集中的数据通道信息
	///
	/// @return
	///     返回对应的通道类型信息 时间/电压/电流...
	///
	virtual CHANNEL_TYPE_SMV GetChannelType(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo) = 0;

	///
	/// @brief
	///     获取通道的相别信息
	///
	/// @param[in]
	///     stSmvChannelInfo - 采样值数据集中的数据通道信息
	/// @param[in]
	///     eChannelType     - 通道类型
	///
	/// @return
	///     返回对应的通道相别 A/B/C相
	///
	virtual CHANNEL_PHASE_SMV GetChannelPhase(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo, CHANNEL_TYPE_SMV eChannelType) = 0;

};

class ISCLCacheManager;

///
/// @brief
///     SCD文件SMV92配置解析接口接口对象实例
///
/// @param[in]
///     pSclCacheMgr - 指向Scl缓存管理器的指针
///
/// @return
///     SCD文件SMV92配置解析接口
///
SCLANALYSISDLL_API ISCDSMV92Analysis *CreateSCDSMV92AnalysisImpl( ISCLCacheManager *pSclCacheMgr );

///
/// @brief
///     释放SCD文件SMV92配置解析接口对象实例
///
/// @param[in]
///     objPtr - 指向接口对象实例的指针
///
SCLANALYSISDLL_API void  RelaseSCDSMV92AnalysisImpl(ISCDSMV92Analysis *objPtr);

#endif
