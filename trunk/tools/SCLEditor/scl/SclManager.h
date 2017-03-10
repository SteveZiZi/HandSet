#ifndef SCL_MANAGER_H_

#define SCL_MANAGER_H_

#include "src\utils\SclAnalysis\include\sclanalysis.h"
#include "src\utils\SclAnalysis\include\ISCLCacheManager.h"
#include "src\utils\SclAnalysis\include\SCDGOOSEAnalysis.h"
#include "src\utils\SclAnalysis\include\SCDSMV92Analysis.h"

class CSCLManager
{
private:
	CSCLManager(void);
	~CSCLManager(void);

public:
	///
	/// @brief
	///    获取静态实例，全局使用一个管理器
	///
	static CSCLManager* getInstance();

	///
	/// @brief
	///    加载scl文件
	///
	int Load(const std::wstring strFileName);

	///
	/// @brief
	///    卸载SCL文件
	///
	void UnLoad();

	///
	/// @brief
	///    保存缓存至scl文件
	///
	int Save(const std::wstring strFileName);

	///
	/// @brief
	///     获取全部的SMV控制块的数目（包含未配置通信参数的控制块）
	///
	/// @return
	///     控制块的数目：操作成功；  -1：失败
	///
	int GetAllIEDSMVNumByCtrlRef( void );

	///
	/// @brief
	///     获取全部的SMV控制块的信息（包含未配置通信参数的控制块），通过控制块索引进行遍历查找
	///     其中，未配置部分进行默认设置
	///
	/// @param[in]
	///     iedSMVInfoArr - 保存SMV控制块信息的数组
	/// @param[in]
	///     arrSize       - 数组大小
	///
	/// @return
	///     数组中保存的控制块的数量：操作成功；  -1：失败
	///
	int GetAllIEDSMVInfoByCtrlRef(
		SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfoArr, 
		const int arrSize );

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
	int SetOneIEDSMVInfoByCtrlRef(const SCDSMV92ANALYSIS_IEDSMV_INFO &iedSMVInfo);

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
	int GetIEDSMVChannelNum(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName);

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
	int GetAllIEDSMVChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfoArr,
		const int arrSize );

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
	int GetOneIEDSMVDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		const int idxSeq,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo);

	///
	/// @brief
	///     设置SMV控制块所控制数据集中的一个通道（FCDA）
	///     注意，对通道信息中的lnPrefix、lnClass、lnInst、doName、daName、daFc、daBType、dUsAddr属性的设置无效
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
	int SetOneIEDSMVChannelInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_SMV eChannelType );

	///
	/// @brief
	///     在SMV控制块所控制数据集末尾添加一个通道（FCDA）
	///     注意，对通道信息中的lnPrefix、lnClass、lnInst、doName、daName、daFc、daBType、dUsAddr属性的设置无效
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
	int AddOneIEDSMVChannelInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_SMV eChannelType );

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
	int CopyIEDSMVChannelInfoToSetEnd(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &dataSetName,
		int idxSeq, int iCopyCount );

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
	int RemoveOneIEDSMVChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &datSetName,
		const int idxSeq);

	///
	/// @brief
	///     判断某AppID的SMV控制块是否存在
	///
	/// @param[in]
	///     appID - 控制块的AppID值
	///
	/// @return
	///     0 - 存在； -1 - 不存在
	///
	int IsSmvAppIDExisted(const unsigned short smvAppID);

	///
	/// @brief
	///     获取全部的GOOSE控制块的数目（包含未配置通信参数的控制块）
	///
	/// @return
	///     控制块的数目：操作成功；  -1：失败
	///
	int GetAllIEDGOOSENumByCtrlRef( void );

	///
	/// @brief
	///     获取全部的GOOSE控制块的信息（包含未配置通信参数的控制块），通过控制块索引进行遍历查找
	///     其中，未配置部分进行默认设置
	///
	/// @param[in]
	///     iedGOOSEInfoArr - 保存SMV控制块信息的数组
	/// @param[in]
	///     arrSize         - 数组大小
	///
	/// @return
	///     数组中保存的控制块的数量：操作成功；  -1：失败
	///
	int GetAllIEDGOOSEInfoByCtrlRef(
		SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfoArr, 
		const int arrSize );

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
	int SetOneIEDGOOSEInfoByCtrlRef(const SCDGOOSEANALYSIS_IEDGOOSE_INFO &iedGOOSEInfo);

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
	int GetIEDGOOSEChannelNum(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName);

	///
	/// @brief
	///     获取指定索引地址的GOOSE控制块所控制的DataSet的所有通道（FCDA）信息
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
	int GetAllIEDGOOSEChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr,
		const int arrSize );

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
	int GetOneIEDGOOSEDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		const int idxSeq,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo);

	///
	/// @brief
	///     设置Goose控制块所控制数据集中的一个通道（FCDA）
	///     注意，对通道信息中的lnPrefix、lnClass、lnInst、doName、daName、daFc、daBType、dUsAddr属性的设置无效
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
	int SetOneIEDGOOSEChannelInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_GOOSE eChannelType );

	///
	/// @brief
	///     在Goose控制块所控制数据集末尾添加一个通道（FCDA）
	///     注意，对通道信息中的lnPrefix、lnClass、lnInst、doName、daName、daFc、daBType、dUsAddr属性的设置无效
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
	int AddOneIEDGOOSEDataSetInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_GOOSE eChannelType );

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
	int CopyIEDGOOSEDataSetInfoToSetEnd(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &dataSetName,
		int idxSeq,
		int iCopyCount);

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
	int RemoveOneIEDGOOSEChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &datSetName,
		const int idxSeq);

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
	int GetIEDGOOSESubChannelNum(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo);

	///
	/// @brief
	///     获取指定索引地址的GOOSE控制块的控制的某一父通道的全部子通道的信息
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
	int GetAllIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfoArr,
		const int arrSize );

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
	int GetOneIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		int idxSeq,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &subChannelInfo );

	///
	/// @brief
	///     设置Goose控制块所控制数据集中的某一通道中的一个子通道
	///     注意，父通道为非自定义通道时会自动转换为自定义通道
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
	int SetOneIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize,
		CHANNEL_TYPE_GOOSE eChannelType);

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
	int CopyIEDGooseSubChannelToEnd(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize,
		int iCopyCount);

	///
	/// @brief
	///     移除Goose控制块所控制数据集中的某一通道中的一个子通道
	///
	/// @param[in]
	///     iedName    - 该子通道所在的IED的名称
	/// @param[in]
	///     ldInst     - 该子通道所在的LDevice的Inst号
	/// @param[in]
	///     lnName     - 该子通道所在的lNode的名称
	/// @param[in]
	///     doName     - 该子通道所在的DOI的名称
	/// @param[in]
	///     idxSeq     - 该子通道的顺序号
	///
	/// @return
	///     0：操作成功；  -1：失败
	///
	int RemoveOneIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize);

	///
	/// @brief
	///     判断某AppID的GOOSE控制块是否存在
	///
	/// @param[in]
	///     appID - 控制块的AppID值
	///
	/// @return
	///     0 - 存在； -1 - 不存在
	///
	int IsGOOSEAppIDExisted(const unsigned short appID);

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
	CHANNEL_TYPE_SMV GetChannelType(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo);

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
	CHANNEL_PHASE_SMV GetChannelPhase(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo, CHANNEL_TYPE_SMV eChannelType);

	///
	/// @brief
	///     获取通道的类型信息
	///
	/// @param[in]
	///     stGooseChannelInfo - goose数据集中的数据通道信息
	///
	/// @return
	///     返回对应的通道类型信息 单电/双点/时间...
	///
	CHANNEL_TYPE_GOOSE GetChannelType(const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO& stGooseChannelInfo);

	///
	/// @brief
	///     获取GOOSE自定义数据类型模板中的DaName信息
	///
	/// @param[in]
	///     eChannelType - goose通道类型
	/// @param[in]
	///     daName       - 保存自定义数据类型模板中的DaName信息
	///
	/// @return
	///     
	///
	void GetDefDataTemplate_DaName(CHANNEL_TYPE_GOOSE eChannelType, std::wstring &daName);

protected:

private:
	ISCLCacheManager  *m_pSclCache;
	ISCDSMV92Analysis *m_pSmv92Analysis;
	ISCDGOOSEAnalysis *m_pGooseAnalysis;
};

#endif
