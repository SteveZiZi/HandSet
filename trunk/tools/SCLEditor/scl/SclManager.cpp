#include "stdafx.h"
#include "SclManager.h"

CSCLManager::CSCLManager()
{
	m_pSclCache      = CreateSCLCacheManagerImpl();
	m_pSmv92Analysis = CreateSCDSMV92AnalysisImpl(m_pSclCache);
	m_pGooseAnalysis = CreateSCDGOOSEAnalysisImpl(m_pSclCache);
	m_pSclCache->EnableSmvAnalysis();
	m_pSclCache->EnableGseAnalysis();
}

CSCLManager::~CSCLManager()
{
	RelaseSCDGOOSEAnalysisImpl(m_pGooseAnalysis);
	RelaseSCDSMV92AnalysisImpl(m_pSmv92Analysis);
	RelaseSCLCacheManagerImpl(m_pSclCache);
}

///
/// @brief
///    获取静态实例，全局使用一个管理器
///
CSCLManager* CSCLManager::getInstance()
{
	static CSCLManager s_SclManager;
	return &s_SclManager;
}

///
/// @brief
///    加载scl文件
///
int CSCLManager::Load(const std::wstring strFileName)
{
	return m_pSclCache->LoadSCL(strFileName);
}

///
/// @brief
///    卸载SCL文件
///
void CSCLManager::UnLoad()
{
	m_pSclCache->UnloadSCL();
}

///
/// @brief
///    保存缓存至scl文件
///
int CSCLManager::Save(const std::wstring strFileName)
{
	return m_pSclCache->SaveSCL(strFileName);
}

///
/// @brief
///     获取全部的SMV控制块的数目（包含未配置通信参数的控制块）
///
/// @return
///     控制块的数目：操作成功；  -1：失败
///
int CSCLManager::GetAllIEDSMVNumByCtrlRef( void )
{
	return m_pSmv92Analysis->GetAllIEDSMVNumByCtrlRef();
}

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
int CSCLManager::GetAllIEDSMVInfoByCtrlRef(SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfoArr, const int arrSize )
{
	return m_pSmv92Analysis->GetAllIEDSMVInfoByCtrlRef(iedSMVInfoArr,arrSize);
}

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
int CSCLManager::SetOneIEDSMVInfoByCtrlRef(const SCDSMV92ANALYSIS_IEDSMV_INFO &iedSMVInfo)
{
	return m_pSmv92Analysis->SetOneIEDSMVInfoByCtrlRef(iedSMVInfo);
}

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
int CSCLManager::GetIEDSMVChannelNum(const std::wstring &iedName, const std::wstring &ldInst, const std::wstring &cbName)
{
	return m_pSmv92Analysis->GetIEDSMVDataSetNum(iedName,ldInst,cbName);
}

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
int CSCLManager::GetAllIEDSMVChannelInfo( const std::wstring &iedName, const std::wstring &ldInst,
										  const std::wstring &cbName, SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfoArr,
										  const int arrSize )
{
	return m_pSmv92Analysis->GetAllIEDSMVDataSetInfo(iedName,ldInst,cbName,dataSetInfoArr,arrSize);
}

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
int CSCLManager::GetOneIEDSMVDataSetInfo(const std::wstring &iedName, const std::wstring &ldInst,
							             const std::wstring &cbName, const int idxSeq,
										 SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo)
{
	return m_pSmv92Analysis->GetOneIEDSMVDataSetInfo(iedName,ldInst,cbName,idxSeq,dataSetInfo);
}

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
int CSCLManager::SetOneIEDSMVChannelInfo_Def( const std::wstring &iedName, const std::wstring &ldInst,
											  SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
											  CHANNEL_TYPE_SMV eChannelType )
{
	return m_pSmv92Analysis->SetOneIEDSMVDataSetInfo_Def(iedName,ldInst,dataSetInfo,eChannelType);
}

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
int CSCLManager::AddOneIEDSMVChannelInfo_Def( const std::wstring &iedName, const std::wstring &ldInst,
											  SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
											  CHANNEL_TYPE_SMV eChannelType )
{
	return m_pSmv92Analysis->AddOneIEDSMVDataSetInfo_Def(iedName,ldInst,dataSetInfo,eChannelType);
}

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
int CSCLManager::CopyIEDSMVChannelInfoToSetEnd( const std::wstring &iedName, const std::wstring &ldInst,
											    const std::wstring &dataSetName, int idxSeq, int iCopyCount )
{
	return m_pSmv92Analysis->CopyIEDSMVDataSetInfoToSetEnd(iedName,ldInst,dataSetName,idxSeq,iCopyCount);
}

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
int CSCLManager::RemoveOneIEDSMVChannelInfo(const std::wstring &iedName, const std::wstring &ldInst,
											const std::wstring &datSetName, const int idxSeq)
{
	return m_pSmv92Analysis->RemoveOneIEDSMVDataSetInfo(iedName,ldInst,datSetName,idxSeq);
}

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
int CSCLManager::IsSmvAppIDExisted(const unsigned short smvAppID)
{
	return m_pSmv92Analysis->IsAppIDExisted(smvAppID);
}

///
/// @brief
///     获取全部的GOOSE控制块的数目（包含未配置通信参数的控制块）
///
/// @return
///     控制块的数目：操作成功；  -1：失败
///
int CSCLManager::GetAllIEDGOOSENumByCtrlRef( void )
{
	return m_pGooseAnalysis->GetAllIEDGOOSENumByCtrlRef();
}

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
int CSCLManager::GetAllIEDGOOSEInfoByCtrlRef(SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfoArr, const int arrSize )
{
	return m_pGooseAnalysis->GetAllIEDGOOSEInfoByCtrlRef(iedGOOSEInfoArr,arrSize);
}

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
int CSCLManager::SetOneIEDGOOSEInfoByCtrlRef(const SCDGOOSEANALYSIS_IEDGOOSE_INFO &iedGOOSEInfo)
{
	return m_pGooseAnalysis->SetOneIEDGOOSEInfoByCtrlRef(iedGOOSEInfo);
}

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
int CSCLManager::GetIEDGOOSEChannelNum(const std::wstring &iedName, const std::wstring &ldInst, const std::wstring &cbName)
{
	return m_pGooseAnalysis->GetIEDGOOSEDataSetNum(iedName,ldInst,cbName);
}

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
int CSCLManager::GetAllIEDGOOSEChannelInfo( const std::wstring &iedName, const std::wstring &ldInst,
										    const std::wstring &cbName, SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr,
											const int arrSize )
{
	return m_pGooseAnalysis->GetAllIEDGOOSEDataSetInfo(iedName,ldInst,cbName,dataSetInfoArr,arrSize);
}

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
int CSCLManager::GetOneIEDGOOSEDataSetInfo(const std::wstring &iedName, const std::wstring &ldInst,
							               const std::wstring &cbName, const int idxSeq,
										   SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo)
{
	return m_pGooseAnalysis->GetOneIEDGOOSEDataSetInfo(iedName,ldInst,cbName,idxSeq,dataSetInfo);
}

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
int CSCLManager::SetOneIEDGOOSEChannelInfo_Def(const std::wstring &iedName, const std::wstring &ldInst,
							               SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
										   CHANNEL_TYPE_GOOSE eChannelType)
{
	return m_pGooseAnalysis->SetOneIEDGOOSEDataSetInfo_Def(iedName,ldInst,channelInfo,eChannelType);
}

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
int CSCLManager::AddOneIEDGOOSEDataSetInfo_Def(const std::wstring &iedName, const std::wstring &ldInst,
											   SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
											   CHANNEL_TYPE_GOOSE eChannelType )
{
	return m_pGooseAnalysis->AddOneIEDGOOSEDataSetInfo_Def(iedName,ldInst,dataSetInfo,eChannelType);
}

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
int CSCLManager::CopyIEDGOOSEDataSetInfoToSetEnd(const std::wstring &iedName, const std::wstring &ldInst,
												 const std::wstring &dataSetName, int idxSeq,
												 int iCopyCount)
{
	return m_pGooseAnalysis->CopyIEDGOOSEDataSetInfoToSetEnd(iedName,ldInst,dataSetName,idxSeq,iCopyCount);
}

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
int CSCLManager::RemoveOneIEDGOOSEChannelInfo(const std::wstring &iedName, const std::wstring &ldInst,
	                                          const std::wstring &datSetName, const int idxSeq)
{
	return m_pGooseAnalysis->RemoveOneIEDGOOSEDataSetInfo(iedName,ldInst,datSetName,idxSeq);
}

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
int CSCLManager::GetIEDGOOSESubChannelNum(const std::wstring &iedName, const std::wstring &ldInst,
										  const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo)
{
	return m_pGooseAnalysis->GetIEDGOOSESubChannelNum(iedName,ldInst,dataSetInfo);
}

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
///     0 - 操作成功； -1 - 操作失败
///
int CSCLManager::GetAllIEDGOOSESubChannelInfo(const std::wstring &iedName, const std::wstring &ldInst,
	                                          const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
	                                          SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfoArr,
	                                          const int arrSize )
{
	return m_pGooseAnalysis->GetAllIEDGOOSESubChannelInfo(iedName,ldInst,dataSetInfo,subChannelInfoArr,arrSize);
}

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
int CSCLManager::GetOneIEDGOOSESubChannelInfo( const std::wstring &iedName, const std::wstring &ldInst,
											   const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
											   int idxSeq,
											   SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &subChannelInfo )
{
	return m_pGooseAnalysis->GetOneIEDGOOSESubChannelInfo(iedName,ldInst,dataSetInfo,idxSeq,subChannelInfo);
}

///
/// @brief
///     设置Goose控制块所控制数据集中的某一通道中的一个子通道
///     注意，父通道为非自定义通道时会自动转换为自定义通道
///
/// @param[in]
///     iedName      - 该子通道所在的IED的名称
/// @param[in]
///     ldInst       - 该子通道所在的LDevice的Inst号
/// @param[in]
///     lnName       - 该子通道所在的lNode的名称
/// @param[in]
///     doName       - 该子通道所在的DOI的名称
/// @param[in]
///     idxSeq       - 该子通道的顺序号
/// @param[in]
///     eChannelType - 该子通道的类型（单点、双点...）
///
/// @return
///     0：操作成功；  -1：失败
///
int CSCLManager::SetOneIEDGOOSESubChannelInfo(const std::wstring &iedName, const std::wstring &ldInst,
											  SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
											  const int *subChannelIdxList, int listSize,
											  CHANNEL_TYPE_GOOSE eChannelType)
{
	return m_pGooseAnalysis->SetOneIEDGOOSESubChannelInfo_Def(iedName,ldInst,channelInfo,subChannelIdxList,listSize,eChannelType);
}

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
int CSCLManager::CopyIEDGooseSubChannelToEnd(const std::wstring &iedName, const std::wstring &ldInst,
											 SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
											 const int *subChannelIdxList, int listSize, int iCopyCount)
{
	return m_pGooseAnalysis->CopyIEDGooseSubChannelToEnd(iedName,ldInst,channelInfo,subChannelIdxList,
		listSize,iCopyCount);
}

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
int CSCLManager::RemoveOneIEDGOOSESubChannelInfo(const std::wstring &iedName, const std::wstring &ldInst,
												 SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
												 const int *subChannelIdxList, int listSize)
{
	return m_pGooseAnalysis->RemoveOneIEDGOOSESubChannelInfo(iedName,ldInst,channelInfo,subChannelIdxList,listSize);
}

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
int CSCLManager::IsGOOSEAppIDExisted(const unsigned short appID)
{
	return m_pGooseAnalysis->IsAppIDExisted(appID);
}

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
CHANNEL_PHASE_SMV CSCLManager::GetChannelPhase(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo, CHANNEL_TYPE_SMV eChannelType)
{
	return m_pSmv92Analysis->GetChannelPhase(stSmvChannelInfo,eChannelType);
}

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
CHANNEL_TYPE_SMV CSCLManager::GetChannelType(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo )
{
	return m_pSmv92Analysis->GetChannelType(stSmvChannelInfo);
}

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
CHANNEL_TYPE_GOOSE CSCLManager::GetChannelType(const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO& stGooseChannelInfo )
{
	return m_pGooseAnalysis->GetChannelType(stGooseChannelInfo);
}

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
void CSCLManager::GetDefDataTemplate_DaName(CHANNEL_TYPE_GOOSE eChannelType, std::wstring &daName)
{
	m_pGooseAnalysis->GetDefDataTemplate_DaName(eChannelType,daName);
}
