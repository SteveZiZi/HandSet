/// @file
///     sclcachewithmap.cpp
///
/// @brief
///     SCL文件解析数据缓存区
///
/// @note
///     使用了STL中的Map，提供添加、查询和清空数据的接口
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.1.24
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.02.01     |1.0.1.201  |由于FCDA信息来自不同地方，将AddFCDAIntoDatSet拆分3个        |
///    |2013.02.18     |1.0.3.218  |添加对GOOSE和采样值的虚端子连线的信息（INPUT）添加及获取接口|
///    |2013.05.22     |1.0.4.522  |添加对各个元素的数量和按顺序号进行索引的获取信息接口        |
///    |2013.06.07     |1.0.4.607  |添加对SCL中的各个元素信息的设置接口                         |
///    |2013.07.25     |1.0.5.725  |修正移除指定通道或者子通道时，该通道之后的通道顺序号没有更新的Bug|
///    |2013.07.25     |1.0.5.725  |修正作为Map的Key的关键信息发生改变时，只更新对应的value值却没更新Key值的Bug|
///    |2013.08.09     |1.0.5.809  |由于SMV、GOOSE通信参数信息的缓存结构改变，故改变其信息设置、获取、释放等接口的实现|
///    |2013.08.15     |1.0.5.815  |修改BDA_MAP的key为BDA元素在DaType中的顺序号（从0开始）      |
///    |2013.08.15     |1.0.5.815  |添加移除指定DaType中的某一BDA的接口                         |
///    |2013.08.22     |1.0.5.822  |添加对new操作的异常处理                                     |
///
#include "stdafx.h"
#include "sclcachewithmap.h"
#include <xstring>

SCLCacheWithMap::SCLCacheWithMap()
{
	m_SCLMap              = new SCL_MAP;
	m_SCLMap->iedMap      = NULL;
	m_SCLMap->smvCommnMap = NULL;
	m_SCLMap->gseCommnMap = NULL;
	m_SCLMap->lnTypeMap   = NULL;
	m_SCLMap->doTypeMap   = NULL;
	m_SCLMap->daTypeMap   = NULL;
}

SCLCacheWithMap::~SCLCacheWithMap()
{
	if ( m_SCLMap != NULL )
	{
		ClearSCLMap(m_SCLMap);
		delete m_SCLMap;
	}
}

///
/// @brief
///     清空Cache
///
void SCLCacheWithMap::ClearCache()
{
	if ( m_SCLMap != NULL )
	{
		ClearSCLMap(m_SCLMap);


#ifdef WINCE
		// 压缩堆，使系统回收释放的内存，但会影响效率
		// Windows CE只支持在堆中分配固定（fixed）的块，这使得堆在分配和释放一段时间后会产生碎片。
		// 当堆里已经清空的时候，仍然会占用大量的虚拟内存页，
		// 因为系统不能在堆中内存页没有完全释放的时候回收这些页。
		HANDLE heapHdl = GetProcessHeap();
		HeapCompact(heapHdl,HEAP_NO_SERIALIZE);
#endif
	}
}

///
/// @brief
///     向Cache中添加IED
///
/// @param[in]
///     iedAttrib - 所要添加的IED的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddIED(const SCL_IED_ATTRIB &iedAttrib)
{
	IED_MAP::iterator it_IED;
	IED_INFO_CACHE *iedInfo;

	// IEDMap不存在则新建
	if ( m_SCLMap->iedMap == NULL )
	{
		m_SCLMap->iedMap = new IED_MAP;
		if ( m_SCLMap->iedMap == NULL )
			return false;
	}

	// 在IEDMap中添加/更新IED信息
	if ( FindIEDInMap(iedAttrib.iedName,it_IED) )
	{
		it_IED->second->iedAttrib = iedAttrib;
	}
	else
	{
		iedInfo = new IED_INFO_CACHE;
		if ( iedInfo == NULL )
			return false;
		iedInfo->iedAttrib = iedAttrib;
		iedInfo->lDeviceMap = NULL;
		m_SCLMap->iedMap->insert(IED_MAP::value_type(iedAttrib.iedName,iedInfo));
	}
	return true;
}

///
/// @brief
///     向Cache中某一IED添加LDevice
///
/// @param[in]
///     iedName  - LDevice所在的IED的名称
/// @param[in]
///     apAttrib - 所要添加的LDevice所连接的访问点的属性信息
/// @param[in]
///     ldAttrib - 所要添加的LDevice的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddLDeviceIntoIED(const SCL_STR &iedName, const SCL_ACCESS_POINT_ATTRIB &apAttrib, const SCL_LDEVICE_ATTRIB &ldAttrib)
{
	IED_MAP::iterator it_IED;
	LDEVICE_MAP::iterator it_LD;
	LDEVICE_INFO_CACHE *ldInfo;

	// 在IEDMap中搜索指定IED
	if ( !FindIEDInMap(iedName,it_IED) )
		return false;

	// LDeviceMap不存在则新建
	if ( it_IED->second->lDeviceMap == NULL )
	{
		it_IED->second->lDeviceMap = new LDEVICE_MAP;
		if ( it_IED->second->lDeviceMap == NULL )
			return false;
	}

	// 在LDeviceMap中添加/更新LDevice信息
	if ( FindLDeviceInIED(it_IED,ldAttrib.ldInst,it_LD) )
	{
		ldInfo = it_LD->second;
		ldInfo->apAttrib     = apAttrib;
		ldInfo->ldAttrib     = ldAttrib;
	}
	else
	{
		ldInfo = new LDEVICE_INFO_CACHE;
		if ( ldInfo == NULL )
			return false;
		ldInfo->apAttrib     = apAttrib;
		ldInfo->ldAttrib     = ldAttrib;
		ldInfo->ln0InfoCache = NULL;
		ldInfo->lnMap        = NULL;
		it_IED->second->lDeviceMap->insert(LDEVICE_MAP::value_type(ldAttrib.ldInst,ldInfo));
	}
	return true;
}

///
/// @brief
///     向Cache中某一IED的某一LDevice添加LN0
///
/// @param[in]
///     iedName   - LN0所在的IED的名称
/// @param[in]
///     ldInst    - LN0所在的LDevice的实例号
/// @param[in]
///     ln0Attrib - 所要添加的LN0的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddLN0IntoLD(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_LN0_ATTRIB &ln0Attrib)
{
	LDEVICE_MAP::iterator it_LD;
	LN0_INFO_CACHE *ln0Info;

	// 搜索指定LDevice
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	// 添加/更新LN0信息
	if ( it_LD->second->ln0InfoCache == NULL )
	{
		ln0Info = new LN0_INFO_CACHE;
		if ( ln0Info == NULL )
			return false;
		ln0Info->ln0Attrib      = ln0Attrib;
		ln0Info->datasetMap     = NULL;
		ln0Info->smvCtrlMap     = NULL;
		ln0Info->gseCtrlMap     = NULL;
		ln0Info->doiMap         = NULL;
		ln0Info->extRefMap      = NULL;
		it_LD->second->ln0InfoCache = ln0Info;
	}
	else
	{
		it_LD->second->ln0InfoCache->ln0Attrib =ln0Attrib;
	}
	return true;
}

///
/// @brief
///     向Cache中某一IED的某一LDevice添加LN
///
/// @param[in]
///     iedName  - LN0所在的IED的名称
/// @param[in]
///     ldInst   - LN所在的LDevice的实例号
/// @param[in]
///     lnAttrib - 所要添加的LN的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddLNIntoLD(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_LN_ATTRIB &lnAttrib)
{
	LDEVICE_MAP::iterator it_LD;
	LN_MAP::iterator it_LN;
	LN_INFO_CACHE *lnInfo;
	SCL_STR lnName = lnAttrib.lnPrefix+lnAttrib.lnClass+lnAttrib.lnInst;

	// 搜索指定LDevice
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;


	// LNMap不存在则新建
	if ( it_LD->second->lnMap == NULL )
	{
		it_LD->second->lnMap = new LN_MAP;
		if ( it_LD->second->lnMap == NULL )
			return false;
	}

	// 在LNMap中添加/更新LN信息
	if ( FindLNInLDevice(it_LD,lnName,it_LN ) )
	{
		lnInfo           = it_LN->second;
		lnInfo->lnAttrib = lnAttrib;
	}
	else
	{
		lnInfo            = new LN_INFO_CACHE;
		if ( lnInfo == NULL )
			return false;
		lnInfo->lnAttrib  = lnAttrib;
		lnInfo->doiMap    = NULL;
		lnInfo->extRefMap = NULL;
		it_LD->second->lnMap->insert(LN_MAP::value_type(lnName,lnInfo));
	}
	return true;
}

///
/// @brief
///     向Cache中某一IED的某一LDevice的LN0添加DataSet
///
/// @param[in]
///     iedName       - DataSet所在的IED的名称
/// @param[in]
///     ldInst        - DataSet所在的LDevice的实例号
/// @param[in]
///     datasetAttrib - 所要添加的DataSet的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddDataSetIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_DATASET_ATTRIB &datasetAttrib)
{
	LN0_INFO_CACHE *ln0Info;
	DATASET_INFO_CACHE *dataSetInfo;
	DATASET_MAP::iterator it_DataSet;

	//  搜索指定LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0Info) )
		return false;

	// DataSetMap不存在则新建
	if ( ln0Info->datasetMap ==NULL )
	{
		ln0Info->datasetMap = new DATASET_MAP;
		if ( ln0Info->datasetMap ==NULL )
			return false;
	}

	// 在DataSetMap中添加/更新DataSet信息
	if ( FindDataSetInLN0(ln0Info,datasetAttrib.dataSetName,it_DataSet) )
	{
		it_DataSet->second->datSetAttrib = datasetAttrib;
	}
	else
	{
		dataSetInfo = new DATASET_INFO_CACHE;
		if ( dataSetInfo == NULL )
			return false;
		dataSetInfo->datSetAttrib       = datasetAttrib;
		dataSetInfo->fcdaMap            = NULL;
		ln0Info->datasetMap->insert(DATASET_MAP::value_type(datasetAttrib.dataSetName,dataSetInfo));
	}
	return true;
}

///
/// @brief
///     向Cache中某一IED的某一LDevice的LN0添加SampledValueControl
///
/// @param[in]
///     iedName   - SampledValueControl所在的IED的名称
/// @param[in]
///     ldInst    - SampledValueControl所在的LDevice的实例号
/// @param[in]
///     smvAttrib - 所要添加的SampledValueControl的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddSMVCtrlIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_SMVCTRL_ATTRIB &smvAttrib)
{
	LN0_INFO_CACHE *ln0Info;
	SMV_CTRL_MAP::iterator it_SMVCtrl;

	//  搜索指定LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0Info) )
		return false;

	// SMVCtrlMap不存在则新建
	if ( ln0Info->smvCtrlMap == NULL )
	{
		ln0Info->smvCtrlMap = new SMV_CTRL_MAP;
		if ( ln0Info->smvCtrlMap == NULL )
			return false;
	}

	// 添加/更新SMV控制块信息
	if ( FindSMVCtrlInLN0(ln0Info,smvAttrib.cbName,it_SMVCtrl) )
		it_SMVCtrl->second = smvAttrib;
	else
		ln0Info->smvCtrlMap->insert(SMV_CTRL_MAP::value_type(smvAttrib.cbName,smvAttrib));

	return true;
}

///
/// @brief
///     向Cache中某一IED的某一LDevice的LN0添加GSEControl
///
/// @param[in]
///     iedName   - GSEControl所在的IED的名称
/// @param[in]
///     ldInst    - GSEControl所在的LDevice的实例号
/// @param[in]
///     gseAttrib - 所要添加的GSEControl的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddGSECtrlIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_GSECTRL_ATTRIB &gseAttrib)
{
	LN0_INFO_CACHE *ln0Info;
	GSE_CTRL_MAP::iterator it_GSECtrl;

	//  搜索指定LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0Info) )
		return false;

	// GSECtrlMap不存在则新建
	if ( ln0Info->gseCtrlMap == NULL )
	{
		ln0Info->gseCtrlMap = new GSE_CTRL_MAP;
		if ( ln0Info->gseCtrlMap == NULL )
			return false;
	}

	// 添加/更新GSE控制块信息
	if ( FindGSECtrlInLN0(ln0Info,gseAttrib.cbName,it_GSECtrl) )
		it_GSECtrl->second = gseAttrib;
	else
		ln0Info->gseCtrlMap->insert(GSE_CTRL_MAP::value_type(gseAttrib.cbName,gseAttrib));

	return true;
}

///
/// @brief
///     向Cache中某一IED的某一LDevice的LN0添加DOI
///
/// @param[in]
///     iedName   - DOI所在的IED的名称
/// @param[in]
///     ldInst    - DOI所在的LDevice的实例号
/// @param[in]
///     doiAttrib - 所要添加的DOI的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddDOIIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_DOI_ATTRIB &doiAttrib)
{
	LN0_INFO_CACHE *ln0Info;
	DOI_MAP::iterator it_DOI;

	//  搜索指定LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0Info) )
		return false;

	// DOIMap不存在则新建
	if ( ln0Info->doiMap == NULL )
	{
		ln0Info->doiMap = new DOI_MAP;
		if ( ln0Info->doiMap == NULL )
			return false;
	}

	// 添加/更新DOI信息
	if ( FindDOIInLN0(ln0Info,doiAttrib.doName,it_DOI) )
		it_DOI->second = doiAttrib;
	else
		ln0Info->doiMap->insert(DOI_MAP::value_type(doiAttrib.doName,doiAttrib));

	return true;
}

///
/// @brief
///     向Cache中某一IED的某一LDevice的LN添加DOI
///
/// @param[in]
///     iedName   - DOI所在的IED的名称
/// @param[in]
///     ldInst    - DOI所在的LDevice的实例号
/// @param[in]
///     lnName    - DOI所在的LN的名称
/// @param[in]
///     doiAttrib - 所要添加的DOI的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddDOIIntoLN(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_DOI_ATTRIB &doiAttrib)
{
	LN_MAP::iterator it_LN;
	DOI_MAP::iterator it_DOI;

	//  搜索指定LN
	if ( !FindLNInMap(iedName,ldInst,lnName,it_LN) )
		return false;

	// DOIMap不存在则新建
	if ( it_LN->second->doiMap == NULL )
	{
		it_LN->second->doiMap = new DOI_MAP;
		if ( it_LN->second->doiMap == NULL )
			return false;
	}

	// 添加/更新DOI信息
	if ( FindDOIInLN(it_LN,doiAttrib.doName,it_DOI) )
		it_DOI->second = doiAttrib;
	else
		it_LN->second->doiMap->insert(DOI_MAP::value_type(doiAttrib.doName,doiAttrib));

	return true;
}

///
/// @brief
///     向Cache中某一IED的某一LDevice的LN0添加ExtRef
///
/// @param[in]
///     iedName      - DOI所在的IED的名称
/// @param[in]
///     ldInst       - DOI所在的LDevice的实例号
/// @param[in]
///     extRefAttrib - 所要添加的ExtRef的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddExtRefIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_EXTREF_ATTRIB &extRefAttrib)
{
	LN0_INFO_CACHE *ln0Info;
	EXTREF_MAP::iterator it_ExtRef;

	//  搜索指定LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0Info) )
		return false;

	// DOIMap不存在则新建
	if ( ln0Info->extRefMap == NULL )
	{
		ln0Info->extRefMap = new EXTREF_MAP;
		if ( ln0Info->extRefMap == NULL )
			return false;
	}

	// 添加/更新DOI信息
	if ( FindExtRefInLN0(ln0Info,extRefAttrib.idxSeq,it_ExtRef) )
		it_ExtRef->second = extRefAttrib;
	else
		ln0Info->extRefMap->insert(EXTREF_MAP::value_type(ln0Info->extRefMap->size(),extRefAttrib));

	return true;
}

///
/// @brief
///     向Cache中某一IED的某一LDevice的LN添加ExtRef
///
/// @param[in]
///     iedName      - ExtRef所在的IED的名称
/// @param[in]
///     ldInst       - ExtRef所在的LDevice的实例号
/// @param[in]
///     lnName       - ExtRef所在的LN的名称
/// @param[in]
///     extRefAttrib - 所要添加的ExtRef的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddExtRefIntoLN(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_EXTREF_ATTRIB &extRefAttrib)
{
	LN_MAP::iterator it_LN;
	EXTREF_MAP::iterator it_ExtRef;

	//  搜索指定LN
	if ( !FindLNInMap(iedName,ldInst,lnName,it_LN) )
		return false;

	// DOIMap不存在则新建
	if ( it_LN->second->extRefMap == NULL )
	{
		it_LN->second->extRefMap = new EXTREF_MAP;
		if ( it_LN->second->extRefMap == NULL )
			return false;
	}

	// 添加/更新DOI信息
	if ( FindExtRefInLN(it_LN,extRefAttrib.idxSeq,it_ExtRef) )
		it_ExtRef->second = extRefAttrib;
	else
		it_LN->second->extRefMap->insert(EXTREF_MAP::value_type(it_LN->second->extRefMap->size(),extRefAttrib));

	return true;
}

///
/// @brief
///     向Cache中某一IED的某一LDevice的某一LNode的某一DataSet添加FCDA
///
/// @param[in]
///     iedName     - FCDA所在的IED的名称
/// @param[in]
///     ldInst      - FCDA所在的LDevice的实例号
/// @param[in]
///     lnName      - FCDA所在的LNode的名称，LN0逻辑节点的名称固定为“LLN0”
/// @param[in]
///     dataSetName - FCDA所在的DataSet的名称
/// @param[in]
///     fcdaAttrib  - FCDA的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddFCDAIntoDatSet(const SCL_STR &iedName, const SCL_STR &ldInst,
										const SCL_STR &lnName, const SCL_STR &dataSetName,
										const SCL_FCDA_ATTRIB &fcdaAttrib)
{
	DATASET_MAP::iterator it_DataSet;
	FCDA_MAP::iterator it_FCDA;
	FCDA_INFO_CACHE *info;
	SCL_DOI_ATTRIB doiAttrib;
	SCL_DAINDOTYPE_ATTRIB daAttrib;

	// 搜索指定DataSet
	if ( lnName == _T("LLN0") )
	{
		if ( !FindDataSetInLN0InMap(iedName,ldInst,dataSetName,it_DataSet) )
			return false;
	}
	else
	{
		//		if ( !FindDataSetInLNInMap(iedName,ldInst,lnName,dataSetName,it_DataSet) )
		return false;
	}

	// FCDAMap不存在则新建
	if ( it_DataSet->second->fcdaMap == NULL )
	{
		it_DataSet->second->fcdaMap = new FCDA_MAP;
		if ( it_DataSet->second->fcdaMap == NULL )
			return false;
	}

	// 在FCDAMap中添加/更新FCDA信息
	if ( FindFCDAInDataSet(it_DataSet,fcdaAttrib.idxSeq,it_FCDA) )
	{
		info = it_FCDA->second;
		info->fcdaAttrib = fcdaAttrib;
	}
	else
	{
		info = new FCDA_INFO_CACHE;
		if ( info == NULL )
			return false;
		info->fcdaAttrib = fcdaAttrib;
		it_DataSet->second->fcdaMap->insert(FCDA_MAP::value_type(it_DataSet->second->fcdaMap->size(),info));
	}

	return true;
}

///
/// @brief
///     向Cache中添加某一SMV控制块所关联的通信参数信息
///
/// @param[in]
///     conntAPAttrib - 通信参数所关联的控制块所在的访问点的信息
/// @param[in]
///     smvAttrib     - 通信参数信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddSMVCommn(const SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, const SCL_SMV_ATTRIB &smvAttrib)
{
	SMV_COMMN_MAP::iterator it_SMV;
	SMV_COMMN_INFO_CACHE *info;
	unsigned int appID;

	SCL_SSCANF(smvAttrib.appID.c_str(),_T("%x"),&appID);
	// SMVCommnMap不存在则新建
	if ( m_SCLMap->smvCommnMap == NULL )
	{
		m_SCLMap->smvCommnMap = new SMV_COMMN_MAP;
		if ( m_SCLMap->smvCommnMap == NULL )
			return false;
	}

	// 在SMVCommnMap中添加/更新SMVCommn信息
	if ( FindSMVCommnInMap(appID,it_SMV) )
	{
		info = it_SMV->second;
		SMV_COMMN_INFO_CACHE *info2 = new SMV_COMMN_INFO_CACHE;
		if ( info2 == NULL )
			return false;
		info2->cnntAPAttrib = conntAPAttrib;
		info2->smvAttrib    = smvAttrib;
		info2->pNextCache   = info->pNextCache;
		info->pNextCache    = info2;
	}
	else
	{
		info = new SMV_COMMN_INFO_CACHE;
		if ( info == NULL )
			return false;
		info->cnntAPAttrib = conntAPAttrib;
		info->smvAttrib    = smvAttrib;
		info->pNextCache   = NULL;
		m_SCLMap->smvCommnMap->insert(SMV_COMMN_MAP::value_type(appID,info));
	}
	return true;
}

///
/// @brief
///     向Cache中添加某一GSE控制块所关联的通信参数信息
///
/// @param[in]
///     conntAPAttrib - 通信参数所关联的控制块所在的访问点的信息
/// @param[in]
///     gseAttrib     - 通信参数信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddGSECommn(const SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, const SCL_GSE_ATTRIB &gseAttrib)
{
	GSE_COMMN_MAP::iterator it_GSE;
	GSE_COMMN_INFO_CACHE *info;
	unsigned int appID;

	SCL_SSCANF(gseAttrib.appID.c_str(),_T("%x"),&appID);

	// GSECommnMap不存在则新建
	if ( m_SCLMap->gseCommnMap == NULL )
	{
		m_SCLMap->gseCommnMap = new GSE_COMMN_MAP;
		if ( m_SCLMap->gseCommnMap == NULL )
			return false;
	}

	if ( FindGSECommnInMap(appID,it_GSE) )
	{
		info = it_GSE->second;
		GSE_COMMN_INFO_CACHE *info2 = new GSE_COMMN_INFO_CACHE;
		if ( info2 == NULL )
			return false;
		info2->cnntAPAttrib = conntAPAttrib;
		info2->gseAttrib    = gseAttrib;
		info2->pNextCache   = info->pNextCache;
		info->pNextCache    = info2;
	}
	else
	{
		info = new GSE_COMMN_INFO_CACHE;
		if ( info == NULL )
			return false;
		info->cnntAPAttrib = conntAPAttrib;
		info->gseAttrib    = gseAttrib;
		info->pNextCache   = NULL;
		m_SCLMap->gseCommnMap->insert(GSE_COMMN_MAP::value_type(appID,info));
	}
	return true;
}

///
/// @brief
///     向Cache中添加某一LNType信息
///
/// @param[in]
///     lnTypeAttrib - LNType的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddLNType(const SCL_LNTYPE_ATTRIB &lnTypeAttrib)
{
	LNTYPE_MAP::iterator it_LNType;
	LNTYPE_INFO_CACHE *lnTypeInfo;

	if ( m_SCLMap->lnTypeMap == NULL )
	{
		m_SCLMap->lnTypeMap = new LNTYPE_MAP;
		if ( m_SCLMap->lnTypeMap == NULL )
			return false;
	}

	// 添加/更新LNType信息
	if ( FindLNTypeInMap(lnTypeAttrib.id,it_LNType))
	{
		lnTypeInfo = it_LNType->second;
		lnTypeInfo->lnTypeAttrib = lnTypeAttrib;
	}
	else
	{
		lnTypeInfo = new LNTYPE_INFO_CACHE;
		if ( lnTypeInfo == NULL )
			return false;
		lnTypeInfo->lnTypeAttrib = lnTypeAttrib;
		lnTypeInfo->doMap = NULL;
		m_SCLMap->lnTypeMap->insert(LNTYPE_MAP::value_type(lnTypeAttrib.id,lnTypeInfo));
	}
	return true;
}

///
/// @brief
///     向Cache中某一LNType添加DO信息
///
/// @param[in]
///     lnTypeID - LNType的ID
/// @param[in]
///     doAttrib - DO的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddDOIntoLNType(const SCL_STR &lnTypeID, const SCL_DOINLNTYPE_ATTRIB &doAttrib)
{
	LNTYPE_MAP::iterator it_LNType;
	DO_MAP::iterator it_DO;

	//  搜索指定LNType
	if ( !FindLNTypeInMap(lnTypeID,it_LNType) )
		return false;

	// DOMap不存在则新建
	if ( it_LNType->second->doMap == NULL )
	{
		it_LNType->second->doMap = new DO_MAP;
		if ( it_LNType->second->doMap == NULL )
			return false;
	}

	// 添加/更新DO信息
	if ( FindDOInLNType(it_LNType,doAttrib.name,it_DO) )
		it_DO->second = doAttrib;
	else
		it_LNType->second->doMap->insert(DO_MAP::value_type(doAttrib.name,doAttrib));

	return true;
}

///
/// @brief
///     向Cache中添加某一DOType信息
///
/// @param[in]
///     doTypeAttrib - DOType的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddDOType(const SCL_DOTYPE_ATTRIB &doTypeAttrib)
{
	DOTYPE_MAP::iterator it_DOType;
	DOTYPE_INFO_CACHE *doTypeInfo;

	if ( m_SCLMap->doTypeMap == NULL )
	{
		m_SCLMap->doTypeMap = new DOTYPE_MAP;
		if ( m_SCLMap->doTypeMap == NULL )
			return false;
	}

	// 添加/更新DOType信息
	if ( FindDOTypeInMap(doTypeAttrib.id,it_DOType) )
	{
		doTypeInfo = it_DOType->second;
		doTypeInfo->doTypeAttrib = doTypeAttrib;
	}
	else
	{
		doTypeInfo = new DOTYPE_INFO_CACHE;
		if ( doTypeInfo == NULL )
			return false;
		doTypeInfo->doTypeAttrib = doTypeAttrib;
		doTypeInfo->daMap = NULL;
		m_SCLMap->doTypeMap->insert(DOTYPE_MAP::value_type(doTypeAttrib.id,doTypeInfo));
	}
	return true;
}

///
/// @brief
///     向Cache中某一DOType添加DA信息
///
/// @param[in]
///     doTypeID - DOType的ID
/// @param[in]
///     daAttrib - DA的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddDAIntoDOType(const SCL_STR &doTypeID, const SCL_DAINDOTYPE_ATTRIB &daAttrib)
{
	DOTYPE_MAP::iterator it_DOType;
	DA_MAP::iterator it_DA;

	// 搜索指定DOType
	if ( !FindDOTypeInMap(doTypeID,it_DOType) )
		return false;

	if ( it_DOType->second->daMap == NULL )
	{
		it_DOType->second->daMap = new DA_MAP;
		if ( it_DOType->second->daMap == NULL )
			return false;
	}

	// 添加/更新DA信息
	if ( FindDAInDOType(it_DOType,daAttrib.name,it_DA) )
	{
		it_DA->second = daAttrib;
	}
	else
	{
		it_DOType->second->daMap->insert(DA_MAP::value_type((int)it_DOType->second->daMap->size(),daAttrib));
	}
	return true;
}

///
/// @brief
///     向Cache中添加某一DAType信息
///
/// @param[in]
///     daTypeAttrib - DAType的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddDAType(const SCL_DATYPE_ATTRIB &daTypeAttrib)
{
	DATYPE_MAP::iterator it_DAType;
	DATYPE_INFO_CACHE *daTypeInfo;

	if ( m_SCLMap->daTypeMap == NULL )
	{
		m_SCLMap->daTypeMap = new DATYPE_MAP;
		if ( m_SCLMap->daTypeMap == NULL )
			return false;
	}

	// 添加/更新DAType信息
	if ( FindDATypeInMap(daTypeAttrib.id,it_DAType) )
	{
		daTypeInfo = it_DAType->second;
		daTypeInfo->daTypeAttrib = daTypeAttrib;
	}
	else
	{
		daTypeInfo = new DATYPE_INFO_CACHE;
		if ( daTypeInfo == NULL )
			return false;
		daTypeInfo->daTypeAttrib = daTypeAttrib;
		daTypeInfo->bdaMap = NULL;
		m_SCLMap->daTypeMap->insert(DATYPE_MAP::value_type(daTypeAttrib.id,daTypeInfo));
	}
	return true;
}

///
/// @brief
///     向Cache中某一DAType添加BDA信息
///
/// @param[in]
///     daTypeID  - DAType的ID
/// @param[in]
///     bdaAttrib - BDA的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::AddBDAIntoDAType(const SCL_STR &daTypeID, const SCL_BDAINDATYPE_ATTRIB &bdaAttrib)
{
	DATYPE_MAP::iterator it_DAType;
	BDA_MAP::iterator it_BDA;

	// 搜索指定DAType
	if ( !FindDATypeInMap(daTypeID,it_DAType) )
		return false;

	if ( it_DAType->second->bdaMap == NULL )
	{
		it_DAType->second->bdaMap = new BDA_MAP;
		if ( it_DAType->second->bdaMap == NULL )
			return false;
	}

	// 添加/更新BDA信息
	if ( FindBDAInDAType(it_DAType,bdaAttrib.name,it_BDA) )
	{
		it_BDA->second = bdaAttrib;
	}
	else
	{
		it_DAType->second->bdaMap->insert(BDA_MAP::value_type((int)it_DAType->second->bdaMap->size(),bdaAttrib));
	}
	return true;
}

///
/// @brief
///     获取Cache中的IED数量
///
/// @return
///     Cache中的IED数量
///
int  SCLCacheWithMap::GetIEDNum()
{
	if ( m_SCLMap == NULL || m_SCLMap->iedMap == NULL )
		return 0;
	return (int)m_SCLMap->iedMap->size();
}

///
/// @brief
///     设置Cache中的IED信息
///
/// @param[in]
///     iedName   - IED的名称
/// @param[in]
///     iedAttrib - IED的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetIEDAttrib(const SCL_STR &iedName, const SCL_IED_ATTRIB &iedAttrib)
{
	// 在IEDMap中搜索指定IED
	IED_MAP::iterator it_IED;
	if ( !FindIEDInMap(iedName,it_IED) )
		return false;

	if ( it_IED->second == NULL )
		return false;

	it_IED->second->iedAttrib = iedAttrib;

	return true;
}

///
/// @brief
///     设置Cache中某一顺序号的IED信息
///
/// @param[in]
///     idxSeq    - IED的顺序号，从0开始
/// @param[in]
///     iedAttrib - IED的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetIEDAttrib(const int idxSeq, const SCL_IED_ATTRIB &iedAttrib)
{
	IED_MAP::iterator it_IED;
	int i = 0;

	if ( m_SCLMap == NULL || m_SCLMap->iedMap == NULL )
		return false;

	for ( it_IED = m_SCLMap->iedMap->begin(); it_IED != m_SCLMap->iedMap->end(); it_IED++)
	{
		if ( i == idxSeq )
		{
			if ( it_IED->second == NULL )
				return false;

			// 作为Map Key的iedName不变则直接更新属性信息，否则需替换
			// 注意，替换后IED在Map中的顺序可能会变化，Map内部按Key的大小排序
			if ( it_IED->second->iedAttrib.iedName == iedAttrib.iedName )
			{
				it_IED->second->iedAttrib = iedAttrib;
			}
			else
			{
				// 判断用于替换的IED是否已存在（IED唯一）
				IED_MAP::iterator it_NewIED;
				if ( FindIEDInMap(iedAttrib.iedName,it_NewIED) )
					return false;
				// 添加新IED
				if ( !AddIED(iedAttrib) )
					return false;
				if ( !FindIEDInMap(iedAttrib.iedName,it_NewIED) )
					return false;
				it_NewIED->second->lDeviceMap = it_IED->second->lDeviceMap;
				it_IED->second->lDeviceMap    = NULL;
				// 移除旧IED
				delete it_IED->second;
				it_IED->second = NULL;
				m_SCLMap->iedMap->erase(it_IED);
			}
			
			return true;
		}
		i++;
	}

	return false;
}

///
/// @brief
///     从Cache中获取IED信息
///
/// @param[in]
///     iedName   - IED的名称
/// @param[in]
///     iedAttrib - 保存所要获取的IED的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetIEDAttrib(const SCL_STR &iedName, SCL_IED_ATTRIB &iedAttrib)
{
	// 在IEDMap中搜索指定IED
	IED_MAP::iterator it_IED;
	if ( !FindIEDInMap(iedName,it_IED) )
		return false;

	if ( it_IED->second == NULL )
		return false;

	iedAttrib = it_IED->second->iedAttrib;

	return true;
}

///
/// @brief
///     从Cache中获取某一顺序号的IED信息
///
/// @param[in]
///     idxSeq    - IED的顺序号，从0开始
/// @param[in]
///     iedAttrib - 保存所要获取的IED的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetIEDAttrib(const int idxSeq, SCL_IED_ATTRIB &iedAttrib)
{
	IED_MAP::iterator it_IED;
	int i = 0;

	if ( m_SCLMap == NULL || m_SCLMap->iedMap == NULL )
		return false;

	for ( it_IED = m_SCLMap->iedMap->begin(); it_IED != m_SCLMap->iedMap->end(); it_IED++)
	{
		if ( i == idxSeq )
		{
			if ( it_IED->second == NULL )
				return false;

			iedAttrib = it_IED->second->iedAttrib;
			return true;
		}
		i++;
	}

	return false;
}

///
/// @brief
///     获取Cache中的某一IED中的LDevice的数量
///
/// @param[in]
///     iedName  - LDevice所在的IED的名称
///
/// @return
///     LDevice的数量
///
int  SCLCacheWithMap::GetLDeviceNum(const SCL_STR &iedName)
{
	IED_MAP::iterator it_IED;
	if ( !FindIEDInMap(iedName,it_IED) )
		return 0;

	if ( it_IED->second == NULL || it_IED->second->lDeviceMap == NULL )
		return 0;

	return (int)it_IED->second->lDeviceMap->size();
}

///
/// @brief
///     设置Cache的某一IED中某一实例号为ldInst的LDevice的信息
///
/// @param[in]
///     iedName  - LDevice所在的IED的名称
/// @param[in]
///     ldInst   - LDevice的实例号
/// @param[in]
///     apAttrib - LDevice所连接的访问点的属性信息
/// @param[in]
///     ldAttrib - LDevice的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetLDeviceAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_ACCESS_POINT_ATTRIB &apAttrib, const SCL_LDEVICE_ATTRIB &ldAttrib)
{
	// 搜索指定LDevice
	LDEVICE_MAP::iterator it_LD;
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	if ( it_LD->second == NULL )
		return false;

	it_LD->second->apAttrib = apAttrib;
	it_LD->second->ldAttrib = ldAttrib;

	return true;
}

///
/// @brief
///     设置Cache的某一IED中某一项LDevice的信息
///
/// @param[in]
///     iedName  - LDevice所在的IED的名称
/// @param[in]
///     idxSeq   - LDevice的顺序号，从0开始
/// @param[in]
///     apAttrib - LDevice所连接的访问点的属性信息
/// @param[in]
///     ldAttrib - LDevice的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetLDeviceAttrib(const SCL_STR &iedName, const int idxSeq, const SCL_ACCESS_POINT_ATTRIB &apAttrib, const SCL_LDEVICE_ATTRIB &ldAttrib)
{
	IED_MAP::iterator it_IED;
	LDEVICE_MAP::iterator it_LD;
	int i = 0;

	// 搜索指定IED
	if ( !FindIEDInMap(iedName,it_IED) )
		return false;

	if ( it_IED->second == NULL || it_IED->second->lDeviceMap == NULL )
		return false;

	for ( it_LD = it_IED->second->lDeviceMap->begin(); it_LD != it_IED->second->lDeviceMap->end(); it_LD++)
	{
		if ( i == idxSeq )
		{
			if ( it_LD->second == NULL )
				return false;

			// 作为Map Key的ldInst不变则直接更新属性信息，否则需替换
			// 注意，替换后LDevice在Map中的顺序可能会变化，Map内部按Key的大小排序
			if ( it_LD->second->ldAttrib.ldInst == ldAttrib.ldInst )
			{
				it_LD->second->apAttrib = apAttrib;
				it_LD->second->ldAttrib = ldAttrib;
			}
			else
			{
				// 判断用于替换的LDevice是否已存在（ldInst在IED中唯一）
				LDEVICE_MAP::iterator it_NewLD;
				if ( FindLDeviceInMap(iedName,ldAttrib.ldInst,it_NewLD) )
					return false;
				// 添加新LDevice
				if ( !AddLDeviceIntoIED(iedName,apAttrib,ldAttrib) )
					return false;
				if ( !FindLDeviceInMap(iedName,ldAttrib.ldInst,it_NewLD) )
					return false;
				it_NewLD->second->ln0InfoCache = it_LD->second->ln0InfoCache;
				it_NewLD->second->lnMap        = it_LD->second->lnMap;
				// 移除旧LDevice
				it_LD->second->ln0InfoCache = NULL;
				it_LD->second->lnMap        = NULL;
				delete it_LD->second;
				it_LD->second = NULL;
				it_IED->second->lDeviceMap->erase(it_LD);
			}
			
			return true;
		}
		i++;
	}

	return false;
}

///
/// @brief
///     从Cache的某一IED中获取某一实例号为ldInst的LDevice的信息
///
/// @param[in]
///     iedName  - LDevice所在的IED的名称
/// @param[in]
///     ldInst   - LDevice的实例号
/// @param[in]
///     apAttrib - 保存所要获取的LDevice所连接的访问点的属性信息
/// @param[in]
///     ldAttrib - 保存所要获取的LDevice的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetLDeviceAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, SCL_ACCESS_POINT_ATTRIB &apAttrib, SCL_LDEVICE_ATTRIB &ldAttrib)
{
	// 搜索指定LDevice
	LDEVICE_MAP::iterator it_LD;
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	if ( it_LD->second == NULL )
		return false;

	apAttrib = it_LD->second->apAttrib;
	ldAttrib = it_LD->second->ldAttrib;

	return true;
}

///
/// @brief
///     从Cache的某一IED中获取某一项LDevice的信息
///
/// @param[in]
///     iedName  - LDevice所在的IED的名称
/// @param[in]
///     idxSeq   - LDevice的顺序号，从0开始
/// @param[in]
///     apAttrib - 保存所要获取的LDevice所连接的访问点的属性信息
/// @param[in]
///     ldAttrib - 保存所要获取的LDevice的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetLDeviceAttrib(const SCL_STR &iedName, const int idxSeq, SCL_ACCESS_POINT_ATTRIB &apAttrib, SCL_LDEVICE_ATTRIB &ldAttrib)
{
	IED_MAP::iterator it_IED;
	LDEVICE_MAP::iterator it_LD;
	int i = 0;

	// 搜索指定IED
	if ( !FindIEDInMap(iedName,it_IED) )
		return false;

	if ( it_IED->second == NULL || it_IED->second->lDeviceMap == NULL )
		return false;

	for ( it_LD = it_IED->second->lDeviceMap->begin(); it_LD != it_IED->second->lDeviceMap->end(); it_LD++)
	{
		if ( i == idxSeq )
		{
			if ( it_LD->second == NULL )
				return false;

			apAttrib = it_LD->second->apAttrib;
			ldAttrib = it_LD->second->ldAttrib;
			return true;
		}
		i++;
	}

	return false;
}

///
/// @brief
///     设置Cache的某一IED的某一LDevice中LN0的信息
///
/// @param[in]
///     iedName   - LN0所在的IED的名称
/// @param[in]
///     ldInst    - LN0所在的LDevice的实例号
/// @param[in]
///     ln0Attrib - LN0的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetLN0Attrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_LN0_ATTRIB &ln0Attrib)
{
	// 搜索指定LN0
	LN0_INFO_CACHE *ln0;
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	if ( ln0 == NULL )
		return false;

	ln0->ln0Attrib = ln0Attrib;

	return true;
}

///
/// @brief
///     从Cache的某一IED的某一LDevice中获取LN0的信息
///
/// @param[in]
///     iedName   - LN0所在的IED的名称
/// @param[in]
///     ldInst    - LN0所在的LDevice的实例号
/// @param[in]
///     ln0Attrib - 保存所要获取的LN0的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetLN0Attrib(const SCL_STR &iedName, const SCL_STR &ldInst, SCL_LN0_ATTRIB &ln0Attrib)
{
	// 搜索指定LN0
	LN0_INFO_CACHE *ln0;
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	if ( ln0 == NULL )
		return false;

	ln0Attrib = ln0->ln0Attrib;

	return true;
}

///
/// @brief
///     获取Cache中某一IED的某一LDevice的LN的数量
///
/// @param[in]
///     iedName   - LN所在的IED的名称
/// @param[in]
///     ldInst    - LN所在的LDevice的实例号
///
/// @return
///     LN的数量
///
int  SCLCacheWithMap::GetLNNum(const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// 搜索指定LDevice
	LDEVICE_MAP::iterator it_LD;
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return 0;

	if ( it_LD->second == NULL || it_LD->second->lnMap == NULL )
		return 0;

	return (int)it_LD->second->lnMap->size();
}

///
/// @brief
///     设置Cache的某一IED的某一LDevice中的LN的信息，不包括LLN0
///
/// @param[in]
///     iedName   - LN所在的IED的名称
/// @param[in]
///     ldInst    - LN所在的LDevice的实例号
/// @param[in]
///     lnName    - LN的名称
/// @param[in]
///     lnAttrib  - LN的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_LN_ATTRIB &lnAttrib)
{
	// 搜索指定LN
	LN_MAP::iterator it_LN;
	if ( !FindLNInMap(iedName,ldInst,lnName,it_LN) )
		return false;

	if ( it_LN->second == NULL )
		return false;

	it_LN->second->lnAttrib = lnAttrib;

	return true;
}

///
/// @brief
///     设置Cache的某一IED的某一LDevice中某一顺序号的LN的信息
///
/// @param[in]
///     iedName   - LN所在的IED的名称
/// @param[in]
///     ldInst    - LN所在的LDevice的实例号
/// @param[in]
///     idxSeq    - LN的顺序号，从0开始
/// @param[in]
///     lnAttrib  - LN的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_LN_ATTRIB &lnAttrib)
{
	// 搜索指定LDevice
	LDEVICE_MAP::iterator it_LD;
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	if ( it_LD->second == NULL || it_LD->second->lnMap == NULL )
		return false;

	LN_MAP::iterator it_LN;
	int i = 0;
	for ( it_LN = it_LD->second->lnMap->begin(); it_LN != it_LD->second->lnMap->end(); it_LN++)
	{
		if ( i == idxSeq )
		{
			if ( it_LN->second == NULL )
				return false;

			// 作为Map Key的lnName不变则直接更新属性信息，否则需替换
			// 注意，替换后LN在Map中的顺序可能会变化，Map内部按Key的大小排序
			if ( it_LN->second->lnAttrib.lnPrefix == lnAttrib.lnPrefix &&
				 it_LN->second->lnAttrib.lnClass  == lnAttrib.lnClass &&
				 it_LN->second->lnAttrib.lnInst   == lnAttrib.lnInst )
			{
				it_LN->second->lnAttrib = lnAttrib;
			}
			else
			{
				// 判断用于替换的LN是否已存在（lnName在LDevice中唯一）
				LN_MAP::iterator it_NewLN;
				SCL_STR lnName = lnAttrib.lnPrefix + lnAttrib.lnClass + lnAttrib.lnInst;
				if ( FindLNInMap(iedName,ldInst,lnName,it_NewLN) )
					return false;
				// 添加新LN
				if ( !AddLNIntoLD(iedName,ldInst,lnAttrib) )
					return false;
				if ( !FindLNInMap(iedName,ldInst,lnName,it_NewLN) )
					return false;
				it_NewLN->second->doiMap    = it_LN->second->doiMap;
				it_NewLN->second->extRefMap = it_LN->second->extRefMap;
				// 移除旧LN
				it_LN->second->doiMap    = NULL;
				it_LN->second->extRefMap = NULL;
				delete it_LN->second;
				it_LN->second = NULL;
				it_LD->second->lnMap->erase(it_LN);
			}
			return true;
		}
		i++;
	}

	return false;
}

///
/// @brief
///     从Cache的某一IED的某一LDevice中获取LN的信息，不包括LLN0
///
/// @param[in]
///     iedName   - LN所在的IED的名称
/// @param[in]
///     ldInst    - LN所在的LDevice的实例号
/// @param[in]
///     lnName    - LN的名称
/// @param[in]
///     lnAttrib  - 保存所要获取的LN的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, SCL_LN_ATTRIB &lnAttrib)
{
	// 搜索指定LN
	LN_MAP::iterator it_LN;
	if ( !FindLNInMap(iedName,ldInst,lnName,it_LN) )
		return false;

	if ( it_LN->second == NULL )
		return false;

	lnAttrib = it_LN->second->lnAttrib;

	return true;
}

///
/// @brief
///     从Cache的某一IED的某一LDevice中获取某一顺序号的LN的信息
///
/// @param[in]
///     iedName   - LN所在的IED的名称
/// @param[in]
///     ldInst    - LN所在的LDevice的实例号
/// @param[in]
///     idxSeq    - LN的顺序号，从0开始
/// @param[in]
///     lnAttrib  - 保存所要获取的LN的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_LN_ATTRIB &lnAttrib)
{
	// 搜索指定LDevice
	LDEVICE_MAP::iterator it_LD;
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	if ( it_LD->second == NULL || it_LD->second->lnMap == NULL )
		return false;

	LN_MAP::iterator it_LN;
	int i = 0;
	for ( it_LN = it_LD->second->lnMap->begin(); it_LN != it_LD->second->lnMap->end(); it_LN++)
	{
		if ( i == idxSeq )
		{
			if ( it_LN->second == NULL )
				return false;

			lnAttrib = it_LN->second->lnAttrib;
			return true;
		}
		i++;
	}

	return false;
}

///
/// @brief
///     从Cache的某一IED某一的LDevice的LN0中的DataSet的数量
///
/// @param[in]
///     iedName       - IED的名称
/// @param[in]
///     ldInst        - LDevice的实例号
///
/// @return
///     DataSet的数量
///
int  SCLCacheWithMap::GetDataSetNumInLN0(const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// 搜索指定LN0
	LN0_INFO_CACHE *ln0;
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return 0;

	if ( ln0 == NULL || ln0->datasetMap == NULL )
		return 0;

	return (int)ln0->datasetMap->size();
}

///
/// @brief
///     设置Cache的某一IED某一的LDevice的LN0中某一名称为dataSetName的DataSet信息
///
/// @param[in]
///     iedName       - DataSet所在的IED的名称
/// @param[in]
///     ldInst        - DataSet所在的LDevice的实例号
/// @param[in]
///     dataSetName   - DataSet的名称
/// @param[in]
///     dataSetAttrib - DataSet的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, const SCL_DATASET_ATTRIB &dateSetAttrib)
{
	// 搜索指定DataSet
	DATASET_MAP::iterator it_DataSet;
	if ( !FindDataSetInLN0InMap(iedName,ldInst,dataSetName,it_DataSet) )
		return false;

	if ( it_DataSet->second == NULL )
		return false;

	it_DataSet->second->datSetAttrib = dateSetAttrib;

	return true;
}

///
/// @brief
///     设置Cache的某一IED某一LDevice的LN0中第idxSeq项的DataSet信息
///
/// @param[in]
///     iedName       - DataSet所在的IED的名称
/// @param[in]
///     ldInst        - DataSet所在的LDevice的实例号
/// @param[in]
///     idxSeq        - DataSet的顺序号，从0开始
/// @param[in]
///     dataSetAttrib - DataSet的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_DATASET_ATTRIB &dateSetAttrib)
{
	LN0_INFO_CACHE *ln0;
	DATASET_MAP::iterator it_DataSet;
	int i = 0;

	// 搜索指定LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	if ( ln0 == NULL || ln0->datasetMap == NULL )
		return false;

	for ( it_DataSet = ln0->datasetMap->begin(); it_DataSet != ln0->datasetMap->end(); it_DataSet++)
	{
		if ( i == idxSeq )
		{
			if ( it_DataSet->second == NULL )
				return false;

			// 作为Map Key的dataSetName不变则直接更新属性信息，否则需替换
			// 注意，替换后dataSet在Map中的顺序可能会变化，Map内部按Key的大小排序
			if ( it_DataSet->second->datSetAttrib.dataSetName == dateSetAttrib.dataSetName )
			{
				it_DataSet->second->datSetAttrib = dateSetAttrib;
			}
			else
			{
				// 判断用于替换的LN是否已存在（dataSetName在LN0中唯一）
				DATASET_MAP::iterator it_NewDataSet;
				if ( FindDataSetInLN0(ln0,dateSetAttrib.dataSetName,it_NewDataSet) )
					return false;
				// 添加新dataSet
				if ( !AddDataSetIntoLN0(iedName,ldInst,dateSetAttrib) )
					return false;
				if ( !FindDataSetInLN0(ln0,dateSetAttrib.dataSetName,it_NewDataSet) )
					return false;
				it_NewDataSet->second->fcdaMap = it_DataSet->second->fcdaMap;
				it_DataSet->second->fcdaMap = NULL;
				// 移除旧dataSet
				delete it_DataSet->second;
				it_DataSet->second = NULL;
				ln0->datasetMap->erase(it_DataSet);
			}
			return true;
		}
		i++;
	}

	return false;
}

///
/// @brief
///     从Cache的某一IED某一的LDevice的LN0中获取某一名称为dataSetName的DataSet信息
///
/// @param[in]
///     iedName       - DataSet所在的IED的名称
/// @param[in]
///     ldInst        - DataSet所在的LDevice的实例号
/// @param[in]
///     dataSetName   - DataSet的名称
/// @param[in]
///     dataSetAttrib - 保存所要获取的DataSet的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, SCL_DATASET_ATTRIB &dateSetAttrib)
{
	// 搜索指定DataSet
	DATASET_MAP::iterator it_DataSet;
	if ( !FindDataSetInLN0InMap(iedName,ldInst,dataSetName,it_DataSet) )
		return false;

	if ( it_DataSet->second == NULL )
		return false;

	dateSetAttrib = it_DataSet->second->datSetAttrib;

	return true;
}

///
/// @brief
///     从Cache的某一IED某一LDevice的LN0中获取第idxSeq项的DataSet信息
///
/// @param[in]
///     iedName       - DataSet所在的IED的名称
/// @param[in]
///     ldInst        - DataSet所在的LDevice的实例号
/// @param[in]
///     idxSeq        - DataSet的顺序号，从0开始
/// @param[in]
///     dataSetAttrib - 保存所要获取的DataSet的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_DATASET_ATTRIB &dateSetAttrib)
{
	LN0_INFO_CACHE *ln0;
	DATASET_MAP::iterator it_DataSet;
	int i = 0;

	// 搜索指定LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	if ( ln0 == NULL || ln0->datasetMap == NULL )
		return false;

	for ( it_DataSet = ln0->datasetMap->begin(); it_DataSet != ln0->datasetMap->end(); it_DataSet++)
	{
		if ( i == idxSeq )
		{
			if ( it_DataSet->second == NULL )
				return false;

			dateSetAttrib = it_DataSet->second->datSetAttrib;
			return true;
		}
		i++;
	}

	return false;
}

///
/// @brief
///     获取Cache中某一IED的某一LDevice的LN0中的SampledValueControl的数量
///
/// @param[in]
///     iedName       - SampledValueControl所在的IED的名称
/// @param[in]
///     ldInst        - SampledValueControl所在的LDevice的实例号
///
/// @return
///     SampledValueControl的数量
///
int  SCLCacheWithMap::GetSMVCtrlNumInLN0(const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// 搜索指定LN0
	LN0_INFO_CACHE *ln0;
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return 0;

	if ( ln0 == NULL || ln0->smvCtrlMap == NULL )
		return 0;

	return (int)ln0->smvCtrlMap->size();
}

///
/// @brief
///     设置Cache的某一IED的某一LDevice的LN0中获取某一顺序号的SampledValueControl的信息
///
/// @param[in]
///     iedName       - SampledValueControl所在的IED的名称
/// @param[in]
///     ldInst        - SampledValueControl所在的LDevice的实例号
/// @param[in]
///     idxSeq        - SampledValueControl的顺序号，从0开始
/// @param[in]
///     smvCtrlAttrib - SampledValueControl的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
	LN0_INFO_CACHE *ln0;
	SMV_CTRL_MAP::iterator it_SmvCtrl;
	int i = 0;

	// 搜索指定LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	if ( ln0 == NULL || ln0->smvCtrlMap == NULL )
		return false;

	for ( it_SmvCtrl = ln0->smvCtrlMap->begin(); it_SmvCtrl != ln0->smvCtrlMap->end(); it_SmvCtrl++)
	{
		if ( i == idxSeq )
		{
			// 作为Map Key的cbName不变则直接更新属性信息，否则需替换
			// 注意，替换后SmvCtrl在Map中的顺序可能会变化，Map内部按Key的大小排序
			if ( it_SmvCtrl->second.cbName == smvCtrlAttrib.cbName )
			{
				it_SmvCtrl->second = smvCtrlAttrib;
			}
			else
			{
				// 判断用于替换的SmvCtrl是否已存在（cbName在LN0中唯一）
				SMV_CTRL_MAP::iterator it_NewSmvCtrl;
				if ( FindSMVCtrlInLN0(ln0,smvCtrlAttrib.cbName,it_NewSmvCtrl) )
					return false;
				// 添加新SmvCtrl
				if ( !AddSMVCtrlIntoLN0(iedName,ldInst,smvCtrlAttrib) )
					return false;
				// 移除旧SmvCtrl
				ln0->smvCtrlMap->erase(it_SmvCtrl);
			}

			return true;
		}
		i++;
	}

	return false;
}

///
/// @brief
///     设置Cache的某一IED的某一LDevice的LN0中某一SampledValueControl的信息
///
/// @param[in]
///     iedName       - SampledValueControl所在的IED的名称
/// @param[in]
///     ldInst        - SampledValueControl所在的LDevice的实例号
/// @param[in]
///     cbName        - SampledValueControl的名称
/// @param[in]
///     smvCtrlAttrib - SampledValueControl的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, const SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
	// 搜索指定SMV控制块
	SMV_CTRL_MAP::iterator it_SMVCtrl;
	if ( !FindSMVCtrlInMap(iedName,ldInst,cbName,it_SMVCtrl) )
		return false;

	it_SMVCtrl->second = smvCtrlAttrib;

	return true;
}

///
/// @brief
///     设置Cache的某一IED的某一LDevice的LN0中控制某一DataSet的SampledValueControl的信息
///
/// @param[in]
///     iedName       - SampledValueControl所在的IED的名称
/// @param[in]
///     ldInst        - SampledValueControl所在的LDevice的实例号
/// @param[in]
///     dataSetName   - SampledValueControl所控制的DataSet的名称
/// @param[in]
///     smvCtrlAttrib - SampledValueControl的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetSMVCtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, const SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
	LDEVICE_MAP::iterator it_LD;
	SMV_CTRL_MAP *smvCtrlMap;
	SMV_CTRL_MAP::iterator it_SmvCtrl;

	// 搜索指定LDevice
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	if ( it_LD->second == NULL || it_LD->second->ln0InfoCache == NULL || it_LD->second->ln0InfoCache->smvCtrlMap == NULL )
		return false;

	smvCtrlMap = it_LD->second->ln0InfoCache->smvCtrlMap;

	// 搜索控制某一数据集的控制块
	for ( it_SmvCtrl = smvCtrlMap->begin(); it_SmvCtrl != smvCtrlMap->end(); it_SmvCtrl++)
	{
		if ( it_SmvCtrl->second.dataSet == dataSetName )
		{
			// 作为Map Key的cbName不变则直接更新属性信息，否则需替换
			// 注意，替换后SmvCtrl在Map中的顺序可能会变化，Map内部按Key的大小排序
			if ( it_SmvCtrl->second.cbName == smvCtrlAttrib.cbName )
			{
				it_SmvCtrl->second = smvCtrlAttrib;
			}
			else
			{
				// 判断用于替换的SmvCtrl是否已存在（cbName在LN0中唯一）
				SMV_CTRL_MAP::iterator it_NewSmvCtrl;
				if ( FindSMVCtrlInLN0(it_LD->second->ln0InfoCache,smvCtrlAttrib.cbName,it_NewSmvCtrl) )
					return false;
				// 添加新SmvCtrl
				if ( !AddSMVCtrlIntoLN0(iedName,ldInst,smvCtrlAttrib) )
					return false;
				// 移除旧SmvCtrl
				it_LD->second->ln0InfoCache->smvCtrlMap->erase(it_SmvCtrl);
			}
			return true;
		}
	}
	return false;
}

///
/// @brief
///     从Cache的某一IED的某一LDevice的LN0中获取某一顺序号的SampledValueControl的信息
///
/// @param[in]
///     iedName       - SampledValueControl所在的IED的名称
/// @param[in]
///     ldInst        - SampledValueControl所在的LDevice的实例号
/// @param[in]
///     idxSeq        - SampledValueControl的顺序号，从0开始
/// @param[in]
///     smvCtrlAttrib - 保存所要获取的SampledValueControl的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
	LN0_INFO_CACHE *ln0;
	SMV_CTRL_MAP::iterator it_SmvCtrl;
	int i = 0;

	// 搜索指定LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	if ( ln0 == NULL || ln0->smvCtrlMap == NULL )
		return false;

	for ( it_SmvCtrl = ln0->smvCtrlMap->begin(); it_SmvCtrl != ln0->smvCtrlMap->end(); it_SmvCtrl++)
	{
		if ( i == idxSeq )
		{
			smvCtrlAttrib = it_SmvCtrl->second;
			return true;
		}
		i++;
	}

	return false;
}

///
/// @brief
///     从Cache的某一IED的某一LDevice的LN0中获取某一SampledValueControl的信息
///
/// @param[in]
///     iedName       - SampledValueControl所在的IED的名称
/// @param[in]
///     ldInst        - SampledValueControl所在的LDevice的实例号
/// @param[in]
///     cbName        - SampledValueControl的名称
/// @param[in]
///     smvCtrlAttrib - 保存所要获取的SampledValueControl的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
	// 搜索指定SMV控制块
	SMV_CTRL_MAP::iterator it_SMVCtrl;
	if ( !FindSMVCtrlInMap(iedName,ldInst,cbName,it_SMVCtrl) )
		return false;

	smvCtrlAttrib = it_SMVCtrl->second;

	return true;
}

///
/// @brief
///     从Cache的某一IED的某一LDevice的LN0中获取控制某一DataSet的SampledValueControl的信息
///
/// @param[in]
///     iedName       - SampledValueControl所在的IED的名称
/// @param[in]
///     ldInst        - SampledValueControl所在的LDevice的实例号
/// @param[in]
///     dataSetName   - SampledValueControl所控制的DataSet的名称
/// @param[in]
///     smvCtrlAttrib - 保存所要获取的SampledValueControl的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetSMVCtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
	LDEVICE_MAP::iterator it_LD;
	SMV_CTRL_MAP *smvCtrlMap;
	SMV_CTRL_MAP::iterator it_SMVCtrl;

	// 搜索指定LDevice
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	if ( it_LD->second == NULL || it_LD->second->ln0InfoCache == NULL || it_LD->second->ln0InfoCache->smvCtrlMap == NULL )
		return false;

	smvCtrlMap = it_LD->second->ln0InfoCache->smvCtrlMap;

	// 搜索控制某一数据集的控制块
	for ( it_SMVCtrl = smvCtrlMap->begin(); it_SMVCtrl != smvCtrlMap->end(); it_SMVCtrl++)
	{
		if ( it_SMVCtrl->second.dataSet == dataSetName )
		{
			smvCtrlAttrib = it_SMVCtrl->second;
			return true;
		}
	}
	return false;
}

///
/// @brief
///     获取Cache中某一IED的某一LDevice的LN0中的GSEControl的数量
///
/// @param[in]
///     iedName       - GSEControl所在的IED的名称
/// @param[in]
///     ldInst        - GSEControl所在的LDevice的实例号
///
/// @return
///     GSEControl的数量
///
int  SCLCacheWithMap::GetGSECtrlNumInLN0(const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// 搜索指定LN0
	LN0_INFO_CACHE *ln0;
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return 0;

	if ( ln0 == NULL || ln0->gseCtrlMap == NULL )
		return 0;

	return (int)ln0->gseCtrlMap->size();
}

///
/// @brief
///     设置Cache的某一IED的某一LDevice的LN0中的某一顺序号的GSEControl的信息
///
/// @param[in]
///     iedName       - GSEControl所在的IED的名称
/// @param[in]
///     ldInst        - GSEControl所在的LDevice的实例号
/// @param[in]
///     idxSeq        - GSEControl的顺序号，从0开始
/// @param[in]
///     gseCtrlAttrib - GSEControl的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
	LN0_INFO_CACHE *ln0;
	GSE_CTRL_MAP::iterator it_GseCtrl;
	int i = 0;

	// 搜索指定LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	if ( ln0 == NULL || ln0->gseCtrlMap == NULL )
		return false;

	for ( it_GseCtrl = ln0->gseCtrlMap->begin(); it_GseCtrl != ln0->gseCtrlMap->end(); it_GseCtrl++)
	{
		if ( i == idxSeq )
		{
			// 作为Map Key的cbName不变则直接更新属性信息，否则需替换
			// 注意，替换后GseCtrl在Map中的顺序可能会变化，Map内部按Key的大小排序
			if ( it_GseCtrl->second.cbName == gseCtrlAttrib.cbName )
			{
				it_GseCtrl->second = gseCtrlAttrib;
			}
			else
			{
				// 判断用于替换的GseCtrl是否已存在（cbName在LN0中唯一）
				GSE_CTRL_MAP::iterator it_NewGseCtrl;
				if ( FindGSECtrlInLN0(ln0,gseCtrlAttrib.cbName,it_NewGseCtrl) )
					return false;
				// 添加新GseCtrl
				if ( !AddGSECtrlIntoLN0(iedName,ldInst,gseCtrlAttrib) )
					return false;
				// 移除旧GseCtrl
				ln0->gseCtrlMap->erase(it_GseCtrl);
			}
			return true;
		}
		i++;
	}

	return false;
}

///
/// @brief
///     设置Cache的某一IED的某一LDevice的LN0中某一GSEControl的信息
///
/// @param[in]
///     iedName       - GSEControl所在的IED的名称
/// @param[in]
///     ldInst        - GSEControl所在的LDevice的实例号
/// @param[in]
///     cbName        - GSEControl的名称
/// @param[in]
///     gseCtrlAttrib - GSEControl的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, const SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
	// 搜索指定GSE控制块
	GSE_CTRL_MAP::iterator it_GSECtrl;
	if ( !FindGSECtrlInMap(iedName,ldInst,cbName,it_GSECtrl) )
		return false;

	it_GSECtrl->second = gseCtrlAttrib;

	return true;
}

///
/// @brief
///     设置Cache的某一IED的某一LDevice的LN0中控制某一DataSet的GSEControl的信息
///
/// @param[in]
///     iedName       - GSEeControl所在的IED的名称
/// @param[in]
///     ldInst        - GSEControl所在的LDevice的实例号
/// @param[in]
///     dataSetName   - GSEControl所控制的DataSet的名称
/// @param[in]
///     gseCtrlAttrib - GSEControl的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetGSECtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, const SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
	LDEVICE_MAP::iterator it_LD;
	GSE_CTRL_MAP *gseCtrlMap;
	GSE_CTRL_MAP::iterator it_GseCtrl;

	// 搜索指定LDevice
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	if ( it_LD->second == NULL || it_LD->second->ln0InfoCache == NULL || it_LD->second->ln0InfoCache->gseCtrlMap == NULL )
		return false;

	gseCtrlMap = it_LD->second->ln0InfoCache->gseCtrlMap;

	// 搜索控制某一数据集的控制块
	for ( it_GseCtrl = gseCtrlMap->begin(); it_GseCtrl != gseCtrlMap->end(); it_GseCtrl++)
	{
		if ( it_GseCtrl->second.dataSet == dataSetName )
		{
			// 作为Map Key的cbName不变则直接更新属性信息，否则需替换
			// 注意，替换后GseCtrl在Map中的顺序可能会变化，Map内部按Key的大小排序
			if ( it_GseCtrl->second.cbName == gseCtrlAttrib.cbName )
			{
				it_GseCtrl->second = gseCtrlAttrib;
			}
			else
			{
				// 判断用于替换的GseCtrl是否已存在（cbName在LN0中唯一）
				GSE_CTRL_MAP::iterator it_NewGseCtrl;
				if ( FindGSECtrlInLN0(it_LD->second->ln0InfoCache,gseCtrlAttrib.cbName,it_NewGseCtrl) )
					return false;
				// 添加新GseCtrl
				if ( !AddGSECtrlIntoLN0(iedName,ldInst,gseCtrlAttrib) )
					return false;
				// 移除旧GseCtrl
				it_LD->second->ln0InfoCache->gseCtrlMap->erase(it_GseCtrl);
			}
			return true;
		}
	}
	return false;
}

///
/// @brief
///     从Cache的某一IED的某一LDevice的LN0中获取某一顺序号的GSEControl的信息
///
/// @param[in]
///     iedName       - GSEControl所在的IED的名称
/// @param[in]
///     ldInst        - GSEControl所在的LDevice的实例号
/// @param[in]
///     idxSeq        - GSEControl的顺序号，从0开始
/// @param[in]
///     gseCtrlAttrib - 保存所要获取的GSEControl的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
	LN0_INFO_CACHE *ln0;
	GSE_CTRL_MAP::iterator it_GseCtrl;
	int i = 0;

	// 搜索指定LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	if ( ln0 == NULL || ln0->gseCtrlMap == NULL )
		return false;

	for ( it_GseCtrl = ln0->gseCtrlMap->begin(); it_GseCtrl != ln0->gseCtrlMap->end(); it_GseCtrl++)
	{
		if ( i == idxSeq )
		{
			gseCtrlAttrib = it_GseCtrl->second;
			return true;
		}
		i++;
	}

	return false;
}

///
/// @brief
///     从Cache的某一IED的某一LDevice的LN0中获取某一GSEControl的信息
///
/// @param[in]
///     iedName       - GSEControl所在的IED的名称
/// @param[in]
///     ldInst        - GSEControl所在的LDevice的实例号
/// @param[in]
///     cbName        - GSEControl的名称
/// @param[in]
///     gseCtrlAttrib - 保存所要获取的GSEControl的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
	// 搜索指定GSE控制块
	GSE_CTRL_MAP::iterator it_GSECtrl;
	if ( !FindGSECtrlInMap(iedName,ldInst,cbName,it_GSECtrl) )
		return false;

	gseCtrlAttrib = it_GSECtrl->second;

	return true;
}

///
/// @brief
///     从Cache的某一IED的某一LDevice的LN0中获取控制某一DataSet的GSEControl的信息
///
/// @param[in]
///     iedName       - GSEeControl所在的IED的名称
/// @param[in]
///     ldInst        - GSEControl所在的LDevice的实例号
/// @param[in]
///     dataSetName   - GSEControl所控制的DataSet的名称
/// @param[in]
///     gseCtrlAttrib - 保存所要获取的GSEControl的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetGSECtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
	LDEVICE_MAP::iterator it_LD;
	GSE_CTRL_MAP *gseCtrlMap;
	GSE_CTRL_MAP::iterator it_GSECtrl;

	// 搜索指定LDevice
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	if ( it_LD->second == NULL || it_LD->second->ln0InfoCache == NULL || it_LD->second->ln0InfoCache->gseCtrlMap == NULL )
		return false;

	gseCtrlMap = it_LD->second->ln0InfoCache->gseCtrlMap;

	// 搜索控制某一数据集的控制块
	for ( it_GSECtrl = gseCtrlMap->begin(); it_GSECtrl != gseCtrlMap->end(); it_GSECtrl++)
	{
		if ( it_GSECtrl->second.dataSet == dataSetName )
		{
			gseCtrlAttrib = it_GSECtrl->second;
			return true;
		}
	}
	return false;
}

///
/// @brief
///     获取Cache中某一IED的某一LDevice的某一LN或LN0中的DOI的数量
///
/// @param[in]
///     iedName   - DOI所在的IED的名称
/// @param[in]
///     ldInst    - DOI所在的LDevice的实例号
/// @param[in]
///     lnName    - DOI所在逻辑节点的名称
///
/// @return
///     DOI的数量
///
int  SCLCacheWithMap::GetDOINumInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName)
{
	if ( lnName == _T("LLN0") )
	{
		// 搜索指定LN0
		LN0_INFO_CACHE *ln0;
		if ( !FindLN0InMap(iedName,ldInst,&ln0) )
			return 0;

		if ( ln0 == NULL || ln0->doiMap == NULL )
			return 0;
		return (int)ln0->doiMap->size();
	}
	else
	{
		// 搜索指定LN
		LN_MAP::iterator it_LN;
		if ( !FindLNInMap(iedName,ldInst,lnName,it_LN) )
			return 0;

		if ( it_LN->second == NULL || it_LN->second->doiMap == NULL )
			return 0;
		return (int)it_LN->second->doiMap->size();
	}
}

///
/// @brief
///     设置Cache的某一IED的某一LDevice的某一LN或LN0中的某一顺序号的DOI的信息
///
/// @param[in]
///     iedName   - DOI所在的IED的名称
/// @param[in]
///     ldInst    - DOI所在的LDevice的实例号
/// @param[in]
///     lnName    - DOI所在逻辑节点的名称
/// @param[in]
///     idxSeq    - DOI的顺序号，从0开始
/// @param[in]
///     doiAttrib - DOI的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, 
											const SCL_STR &lnName, const int idxSeq, const SCL_DOI_ATTRIB &doiAttrib)
{
	DOI_MAP *doiMap = NULL;
	if ( lnName == _T("LLN0") )
	{
		// 搜索指定LN0
		LN0_INFO_CACHE *ln0;
		if ( !FindLN0InMap(iedName,ldInst,&ln0) )
			return false;

		if ( ln0 == NULL || ln0->doiMap == NULL )
			return false;
		doiMap = ln0->doiMap;
	}
	else
	{
		// 搜索指定LN
		LN_MAP::iterator it_LN;
		if ( !FindLNInMap(iedName,ldInst,lnName,it_LN) )
			return false;

		if ( it_LN->second == NULL || it_LN->second->doiMap == NULL )
			return false;
		doiMap = it_LN->second->doiMap;
	}

	int i = 0;
	DOI_MAP::iterator it_DOI;
	for ( it_DOI = doiMap->begin(); it_DOI != doiMap->end(); it_DOI++)
	{
		if ( i == idxSeq )
		{
			// 作为Map Key的doName不变则直接更新属性信息，否则需替换
			// 注意，替换后DOI在Map中的顺序可能会变化，Map内部按Key的大小排序
			if ( it_DOI->second.doName == doiAttrib.doName )
			{
				it_DOI->second = doiAttrib;
			}
			else
			{
				// 判断用于替换的DOI是否已存在（doName在LN0/LN中唯一）
				DOI_MAP::iterator it_NewDOI;
				it_NewDOI = doiMap->find(doiAttrib.doName);
				if ( it_NewDOI == doiMap->end() )
					return false;
				// 添加新DOI
				if ( lnName == _T("LLN0") )
				{
					if ( !AddDOIIntoLN0(iedName,ldInst,doiAttrib) )
						return false;
				}
				else
				{
					if ( !AddDOIIntoLN(iedName,ldInst,lnName,doiAttrib) )
						return false;
				}
				// 移除旧DOI
				doiMap->erase(it_DOI);
			}
			return true;
		}
		i++;
	}

	return false;
}

///
/// @brief
///     设置Cache的某一IED的某一LDevice的某一LN或LN0中的某一DOI的信息
///
/// @param[in]
///     iedName   - DOI所在的IED的名称
/// @param[in]
///     ldInst    - DOI所在的LDevice的实例号
/// @param[in]
///     lnName    - DOI所在逻辑节点的名称
/// @param[in]
///     doiName   - DOI的名称
/// @param[in]
///     doiAttrib - DOI的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, 
											const SCL_STR &lnName, const SCL_STR &doiName, const SCL_DOI_ATTRIB &doiAttrib)
{
	// 搜索指定DOI
	DOI_MAP::iterator it_DOI;
	if ( !FindDOIInMap(iedName,ldInst,lnName,doiName,it_DOI) )
		return false;

	it_DOI->second = doiAttrib;

	return true;
}

///
/// @brief
///     从Cache的某一IED的某一LDevice的某一LN或LN0中获取某一顺序号的DOI的信息
///
/// @param[in]
///     iedName   - DOI所在的IED的名称
/// @param[in]
///     ldInst    - DOI所在的LDevice的实例号
/// @param[in]
///     lnName    - DOI所在逻辑节点的名称
/// @param[in]
///     idxSeq    - DOI的顺序号，从0开始
/// @param[in]
///     doiAttrib - 保存所要获取的DOI的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const int idxSeq, SCL_DOI_ATTRIB &doiAttrib)
{
	DOI_MAP *doiMap = NULL;
	if ( lnName == _T("LLN0") )
	{
		// 搜索指定LN0
		LN0_INFO_CACHE *ln0;
		if ( !FindLN0InMap(iedName,ldInst,&ln0) )
			return false;

		if ( ln0 == NULL || ln0->doiMap == NULL )
			return false;
		doiMap = ln0->doiMap;
	}
	else
	{
		// 搜索指定LN
		LN_MAP::iterator it_LN;
		if ( !FindLNInMap(iedName,ldInst,lnName,it_LN) )
			return false;

		if ( it_LN->second == NULL || it_LN->second->doiMap == NULL )
			return false;
		doiMap = it_LN->second->doiMap;
	}

	int i = 0;
	DOI_MAP::iterator it_DOI;
	for ( it_DOI = doiMap->begin(); it_DOI != doiMap->end(); it_DOI++)
	{
		if ( i == idxSeq )
		{
			doiAttrib = it_DOI->second;
			return true;
		}
		i++;
	}

	return false;
}

///
/// @brief
///     从Cache的某一IED的某一LDevice的某一LN或LN0中获取某一DOI的信息
///
/// @param[in]
///     iedName   - DOI所在的IED的名称
/// @param[in]
///     ldInst    - DOI所在的LDevice的实例号
/// @param[in]
///     lnName    - DOI所在逻辑节点的名称
/// @param[in]
///     doiName   - DOI的名称
/// @param[in]
///     doiAttrib - 保存所要获取的DOI的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &doiName, SCL_DOI_ATTRIB &doiAttrib)
{
	// 搜索指定DOI
	DOI_MAP::iterator it_DOI;
	if ( !FindDOIInMap(iedName,ldInst,lnName,doiName,it_DOI) )
		return false;

	doiAttrib = it_DOI->second;

	return true;
}

///
/// @brief
///     从Cache的某一IED的全部LDevice的LN0中ExtRef的数量
///
/// @param[in]
///     iedName      - IED的名称
///
/// @return
///     ExtRef的数量：成功；  -1：失败
///
int SCLCacheWithMap::GetInputsExtRefNumInIED(const SCL_STR &iedName)
{
	IED_MAP::iterator it_IED;
	LDEVICE_MAP::iterator it_LD;
	int extRefNum = 0;

	// 搜索指定IED
	if ( !FindIEDInMap(iedName,it_IED) )
		return -1;

	if ( it_IED->second == NULL || it_IED->second->lDeviceMap == NULL )
		return false;

	for ( it_LD = it_IED->second->lDeviceMap->begin(); it_LD != it_IED->second->lDeviceMap->end(); it_LD++ )
	{
		if ( it_LD->second == NULL )
			continue;
		// 获取LN0中的ExtRef数量
		if ( it_LD->second->ln0InfoCache != NULL &&
			it_LD->second->ln0InfoCache->extRefMap != NULL )
		{
			extRefNum += (int)it_LD->second->ln0InfoCache->extRefMap->size();
		}
		// 获取LN中的ExtRef数量
		if (it_LD->second->lnMap != NULL)
		{
			LN_MAP::iterator it_LN;
			for ( it_LN = it_LD->second->lnMap->begin(); it_LN != it_LD->second->lnMap->end(); it_LN++ )
			{
				if ( it_LN->second != NULL && it_LN->second->extRefMap != NULL )
				{
					extRefNum += (int)it_LN->second->extRefMap->size();
				}
			}
		}
	}

	return extRefNum;
}

///
/// @brief
///     从Cache的某一IED的全部LDevice的LN0中ExtRef（外部信号连线信息）
///
/// @param[in]
///     iedName           - IED的名称
/// @param[in]
///     extRefAttribArray - 保存ExtRef（外部信号连线信息）的数组
/// @param[in]
///     arrSize           - 数组的大小
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetAllInputsExtRefInIED(const SCL_STR &iedName, SCL_EXTREF_ATTRIB *extRefAttribArray, const int arrSize)
{
	IED_MAP::iterator it_IED;
	LDEVICE_MAP::iterator it_LD;
	EXTREF_MAP::iterator it_ExtRef;
	int i = 0;

	// 搜索指定IED
	if ( !FindIEDInMap(iedName,it_IED) )
		return false;

	if ( it_IED->second == NULL || it_IED->second->lDeviceMap == NULL )
		return false;

	for ( it_LD = it_IED->second->lDeviceMap->begin(); it_LD != it_IED->second->lDeviceMap->end(); it_LD++ )
	{
		if ( it_LD->second == NULL )
			continue;

		// 获取LN0中的ExtRef
		if ( it_LD->second->ln0InfoCache != NULL &&
			it_LD->second->ln0InfoCache->extRefMap != NULL )
		{
			for ( it_ExtRef = it_LD->second->ln0InfoCache->extRefMap->begin(); it_ExtRef != it_LD->second->ln0InfoCache->extRefMap->end(); it_ExtRef++ )
			{
				extRefAttribArray[i] = it_ExtRef->second;
				i++;
				if ( i >= arrSize )
					return true;
			}
		}
		// 获取LN中的ExtRef
		if (it_LD->second->lnMap != NULL)
		{
			LN_MAP::iterator it_LN;
			for ( it_LN = it_LD->second->lnMap->begin(); it_LN != it_LD->second->lnMap->end(); it_LN++ )
			{
				if ( it_LN->second != NULL && it_LN->second->extRefMap != NULL )
				{
					for ( it_ExtRef = it_LN->second->extRefMap->begin(); it_ExtRef != it_LN->second->extRefMap->end(); it_ExtRef++ )
					{
						extRefAttribArray[i] = it_ExtRef->second;
						i++;
						if ( i >= arrSize )
							return true;
					}
				}
			}
		}
	}

	return true;
}

///
/// @brief
///     获取Cache中某一IED的某一LDevice的某一LN或LN0中的ExtRef的数量
///
/// @param[in]
///     iedName      - ExtRef所在的IED的名称
/// @param[in]
///     ldInst       - ExtRef所在的LDevice的实例号
/// @param[in]
///     lnName       - ExtRef所在逻辑节点的名称
///
/// @return
///     ExtRef的数量
///
int  SCLCacheWithMap::GetExtRefNumInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName)
{
	if ( lnName == _T("LLN0") )
	{
		// 搜索指定LN0
		LN0_INFO_CACHE *ln0;
		if ( !FindLN0InMap(iedName,ldInst,&ln0) )
			return 0;

		if ( ln0 == NULL || ln0->extRefMap == NULL )
			return 0;
		return (int)ln0->extRefMap->size();
	}
	else
	{
		// 搜索指定LN
		LN_MAP::iterator it_LN;
		if ( !FindLNInMap(iedName,ldInst,lnName,it_LN) )
			return 0;

		if ( it_LN->second == NULL || it_LN->second->extRefMap == NULL )
			return 0;
		return (int)it_LN->second->extRefMap->size();
	}
}

///
/// @brief
///     设置Cache的某一IED的某一LDevice的某一LN或LN0中的某一顺序号的ExtRef的信息
///
/// @param[in]
///     iedName      - ExtRef所在的IED的名称
/// @param[in]
///     ldInst       - ExtRef所在的LDevice的实例号
/// @param[in]
///     lnName       - ExtRef所在逻辑节点的名称
/// @param[in]
///     idxSeq       - ExtRef在Map中的顺序号
/// @param[in]
///     extRefAttrib - ExtRef的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetExtRefAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
									  const int &idxSeq, const SCL_EXTREF_ATTRIB &extRefAttrib)
{
	if ( idxSeq != extRefAttrib.idxSeq )
		return false;

	// 搜索指定ExtRef
	EXTREF_MAP::iterator it_ExtRef;
	if ( !FindExtRefInMap(iedName,ldInst,lnName,idxSeq,it_ExtRef) )
		return false;

	it_ExtRef->second = extRefAttrib;

	return true;
}

///
/// @brief
///     从Cache的某一IED的某一LDevice的某一LN或LN0中获取某一顺序号的ExtRef的信息
///
/// @param[in]
///     iedName      - ExtRef所在的IED的名称
/// @param[in]
///     ldInst       - ExtRef所在的LDevice的实例号
/// @param[in]
///     lnName       - ExtRef所在逻辑节点的名称
/// @param[in]
///     idxSeq       - ExtRef在Map中的顺序号
/// @param[in]
///     extRefAttrib - 保存所要获取的ExtRef的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetExtRefAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
									  const int &idxSeq, SCL_EXTREF_ATTRIB &extRefAttrib)
{
	// 搜索指定ExtRef
	EXTREF_MAP::iterator it_ExtRef;
	if ( !FindExtRefInMap(iedName,ldInst,lnName,idxSeq,it_ExtRef) )
		return false;

	extRefAttrib = it_ExtRef->second;

	return true;
}

///
/// @brief
///     设置Cache的某一IED的某一LDevice的某一LNode的某一DataSet中的某一顺序号的FCDA的信息
///
/// @param[in]
///     iedName     - FCDA所在的IED的名称
/// @param[in]
///     ldInst      - FCDA所在的LDevice的实例号
/// @param[in]
///     lnName      - FCDA所在的LNode的名称,LN0默认为“LLN0”
/// @param[in]
///     dataSetName - FCDA所在的DataSet
/// @param[in]
///     idxSeq      - FCDA在Map的顺序号，从0开始，与在SCL中的顺序一致
/// @param[in]
///     fcdaAttrib  - FCDA的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetFCDAAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &datSetName,
									const int idxSeq, const SCL_FCDA_ATTRIB &fcdaAttrib)
{
	if ( idxSeq != fcdaAttrib.idxSeq )
		return false;

	// 搜索指定FCDA
	FCDA_MAP::iterator it_FCDA;
	if ( !FindFCDAInMap(iedName,ldInst,lnName,datSetName,idxSeq,it_FCDA) )
		return false;

	if ( it_FCDA->second == NULL )
		return false;

	it_FCDA->second->fcdaAttrib = fcdaAttrib;

	return true;
}

///
/// @brief
///     从Cache的某一IED的某一LDevice的某一LNode的某一DataSet中获取某一顺序号的FCDA的信息
///
/// @param[in]
///     iedName     - FCDA所在的IED的名称
/// @param[in]
///     ldInst      - FCDA所在的LDevice的实例号
/// @param[in]
///     lnName      - FCDA所在的LNode的名称,LN0默认为“LLN0”
/// @param[in]
///     dataSetName - FCDA所在的DataSet
/// @param[in]
///     idxSeq      - FCDA在Map的顺序号，从0开始，与在SCL中的顺序一致
/// @param[in]
///     fcdaAttrib  - 保存所要获取的FCDA的属性信息 
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetFCDAAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &datSetName,
									const int idxSeq, SCL_FCDA_ATTRIB &fcdaAttrib)
{
	// 搜索指定FCDA
	FCDA_MAP::iterator it_FCDA;
	if ( !FindFCDAInMap(iedName,ldInst,lnName,datSetName,idxSeq,it_FCDA) )
		return false;

	if ( it_FCDA->second == NULL )
		return false;

	fcdaAttrib = it_FCDA->second->fcdaAttrib;

	return true;
}

///
/// @brief
///     从Cache的某一IED的某一LDevice的某一LNode的某一DataSet中获取某一FCDA在其DataSet中的顺序号
///
/// @param[in]
///     iedName     - FCDA所在的IED的名称
/// @param[in]
///     ldInst      - FCDA所在的LDevice的实例号
/// @param[in]
///     lnName      - FCDA所在的LNode的名称,LN0默认为“LLN0”
/// @param[in]
///     dataSetName - FCDA所在的DataSet
/// @param[in]
///     fcdaAttrib  - FCDA的属性信息 
///
/// @return
///     SCL文件中FCDA在其DataSet中的顺序号：成功；  false：-1
///
int SCLCacheWithMap::GetFCDAIdxSeqInDataSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &datSetName, const SCL_FCDA_ATTRIB &fcdaAttrib)
{
	DATASET_MAP::iterator it_DataSet;
	FCDA_MAP::iterator it_FCDA;
	int i = 0;

	// 搜索指定DataSet
	if ( lnName == _T("LLN0") )
	{
		if ( !FindDataSetInLN0InMap(iedName,ldInst,datSetName,it_DataSet) )
			return -1;
	}
	else
	{
		//		if ( !FindDataSetInLNInMap(iedName,ldInst,lnName,dataSetName,it_DataSet) )
		return -1;
	}

	if ( it_DataSet->second == NULL || it_DataSet->second->fcdaMap == NULL )
		return -1;

	for ( it_FCDA = it_DataSet->second->fcdaMap->begin(); it_FCDA != it_DataSet->second->fcdaMap->end(); it_FCDA++ )
	{
		if ( it_FCDA->second->fcdaAttrib.ldInst   == fcdaAttrib.ldInst &&
			it_FCDA->second->fcdaAttrib.lnPrefix == fcdaAttrib.lnPrefix &&
			it_FCDA->second->fcdaAttrib.lnClass  == fcdaAttrib.lnClass &&
			it_FCDA->second->fcdaAttrib.lnInst   == fcdaAttrib.lnInst &&
			it_FCDA->second->fcdaAttrib.doName   == fcdaAttrib.doName &&
			it_FCDA->second->fcdaAttrib.daName   == fcdaAttrib.daName)
			return i;
		i++;
	}

	return -1;
}

///
/// @brief
///     从Cache的某一IED的某一LDevice的某一LNode中获取某一FCDA在其DataSet中的顺序号
///
/// @param[in]
///     iedName     - FCDA所在的IED的名称
/// @param[in]
///     ldInst      - FCDA所在的LDevice的实例号
/// @param[in]
///     lnName      - FCDA所在的LNode的名称,LN0默认为“LLN0”
/// @param[in]
///     fcdaAttrib  - FCDA的属性信息 
///
/// @return
///     SCL文件中FCDA在其DataSet中的顺序号：成功；  false：-1
///
int SCLCacheWithMap::GetFCDAIdxSeqInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_FCDA_ATTRIB &fcdaAttrib)
{
	LN0_INFO_CACHE *ln0;
	LN_MAP::iterator it_LN;
	DATASET_MAP *dataSetMap;
	DATASET_MAP::iterator it_DataSet;
	FCDA_MAP::iterator it_FCDA;
	int i = 0;

	if ( lnName == _T("LLN0") )
	{
		if ( !FindLN0InMap(iedName,ldInst,&ln0) )
			return -1;
		if ( ln0 == NULL || ln0->datasetMap == NULL )
			return -1;
		dataSetMap = ln0->datasetMap;
	}
	// 	else
	// 	{
	// 		if ( !FindLNInMap(iedName,ldInst,lnName,it_LN) )
	// 			return -1;
	// 		dataSetMap = it_LN->second->datasetMap;
	// 	}


	for ( it_DataSet = dataSetMap->begin(); it_DataSet != dataSetMap->end(); it_DataSet++ )
	{
		if ( it_DataSet->second == NULL || it_DataSet->second->fcdaMap == NULL )
			return -1;
		for ( it_FCDA = it_DataSet->second->fcdaMap->begin(); it_FCDA != it_DataSet->second->fcdaMap->end(); it_FCDA++ )
		{
			if ( it_FCDA->second->fcdaAttrib.ldInst   == fcdaAttrib.ldInst &&
				it_FCDA->second->fcdaAttrib.lnPrefix == fcdaAttrib.lnPrefix &&
				it_FCDA->second->fcdaAttrib.lnClass  == fcdaAttrib.lnClass &&
				it_FCDA->second->fcdaAttrib.lnInst   == fcdaAttrib.lnInst &&
				it_FCDA->second->fcdaAttrib.doName   == fcdaAttrib.doName &&
				it_FCDA->second->fcdaAttrib.daName   == fcdaAttrib.daName )
				return i;
			i++;
		}
	}
	return -1;
}

///
/// @brief
///     获取Cache中LNType的数量
///
/// @return
///     LNType的数量
///
int  SCLCacheWithMap::GetLNTypeNum()
{
	if ( m_SCLMap == NULL || m_SCLMap->lnTypeMap == NULL )
		return 0;
	return (int)m_SCLMap->lnTypeMap->size();
}

///
/// @brief
///     设置Cache中的某一顺序号的LNType的信息
///
/// @param[in]
///     idxSeq       - LNType的顺序号
/// @param[in]
///     lnTypeAttrib - LNType的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetLNType(const int idxSeq, const SCL_LNTYPE_ATTRIB &lnTypeAttrib)
{
	if ( m_SCLMap == NULL || m_SCLMap->lnTypeMap == NULL )
		return false;
	LNTYPE_MAP::iterator it_LnType;
	int i = 0;
	for ( it_LnType = m_SCLMap->lnTypeMap->begin(); it_LnType != m_SCLMap->lnTypeMap->end(); it_LnType++ )
	{
		if ( i == idxSeq )
		{
			if ( it_LnType->second == NULL )
				return false;

			// 作为Map Key的lnType的ID不变则直接更新属性信息，否则需替换
			// 注意，替换后lnType在Map中的顺序可能会变化，Map内部按Key的大小排序
			if ( it_LnType->second->lnTypeAttrib.id == lnTypeAttrib.id )
			{
				it_LnType->second->lnTypeAttrib = lnTypeAttrib;
			}
			else
			{
				// 判断用于替换的lnType是否已存在（ID在Map中唯一）
				LNTYPE_MAP::iterator it_NewLnType;
				if ( FindLNTypeInMap(lnTypeAttrib.id,it_NewLnType) )
					return false;
				// 添加新lnType
				if ( !AddLNType(lnTypeAttrib) )
					return false;
				if ( !FindLNTypeInMap(lnTypeAttrib.id,it_NewLnType) )
					return false;
				it_NewLnType->second->doMap = it_LnType->second->doMap;
				// 移除旧lnType
				it_LnType->second->doMap = NULL;
				delete it_LnType->second;
				it_LnType->second = NULL;
				m_SCLMap->lnTypeMap->erase(it_LnType);
			}

			return true;
		}
		i++;
	}
	return false;
}

///
/// @brief
///     设置Cache中的某一ID的LNType的信息
///
/// @param[in]
///     lnTypeID     - LNType的ID
/// @param[in]
///     lnTypeAttrib - LNType的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetLNType(const SCL_STR &lnTypeID, const SCL_LNTYPE_ATTRIB &lnTypeAttrib)
{
	if ( m_SCLMap == NULL || m_SCLMap->lnTypeMap == NULL )
		return false;
	LNTYPE_MAP::iterator it_LnType;
	if ( !FindLNTypeInMap(lnTypeID,it_LnType) )
		return false;

	if ( it_LnType->second == NULL )
		return false;

	it_LnType->second->lnTypeAttrib = lnTypeAttrib;
	return true;
}

///
/// @brief
///     从Cache中获取某一顺序号的LNType的信息
///
/// @param[in]
///     idxSeq       - LNType的顺序号
/// @param[in]
///     lnTypeAttrib - 保存所要获取的LNType的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetLNType(const int idxSeq, SCL_LNTYPE_ATTRIB &lnTypeAttrib)
{
	if ( m_SCLMap == NULL || m_SCLMap->lnTypeMap == NULL )
		return false;
	LNTYPE_MAP::iterator it_LnType;
	int i = 0;
	for ( it_LnType = m_SCLMap->lnTypeMap->begin(); it_LnType != m_SCLMap->lnTypeMap->end(); it_LnType++ )
	{
		if ( i == idxSeq )
		{
			if ( it_LnType->second == NULL )
				return false;

			lnTypeAttrib = it_LnType->second->lnTypeAttrib;
			return true;
		}
		i++;
	}
	return false;
}

///
/// @brief
///     从Cache中获取某一ID的LNType的信息
///
/// @param[in]
///     lnTypeID     - LNType的ID
/// @param[in]
///     lnTypeAttrib - LNType的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetLNType(const SCL_STR &lnTypeID, SCL_LNTYPE_ATTRIB &lnTypeAttrib)
{
	if ( m_SCLMap == NULL || m_SCLMap->lnTypeMap == NULL )
		return false;
	LNTYPE_MAP::iterator it_LnType;
	if ( !FindLNTypeInMap(lnTypeID,it_LnType) )
		return false;

	if ( it_LnType->second == NULL )
		return false;

	lnTypeAttrib = it_LnType->second->lnTypeAttrib;
	return true;
}

///
/// @brief
///     获取Cache中的某一LNType中的DO的数量
///
/// @param[in]
///     lnType     - DO所在LNType的ID
///
/// @return
///     DO的数量
///
int  SCLCacheWithMap::GetDONumInLNType(const SCL_STR &lnType)
{
	LNTYPE_MAP::iterator it_LnType;
	if ( !FindLNTypeInMap(lnType,it_LnType) )
		return 0;
	if ( it_LnType->second == NULL || it_LnType->second->doMap == NULL )
		return 0;
	return (int)it_LnType->second->doMap->size();
}

///
/// @brief
///     设置Cache的某一LNType中的某一顺序号的DO的信息
///
/// @param[in]
///     lnType     - DO所在LNType的ID
/// @param[in]
///     idxSeq     - DO的顺序号，从0开始
/// @param[in]
///     doAttrib   - DO的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetDOAttrib(const SCL_STR &lnType, const int idxSeq, const SCL_DOINLNTYPE_ATTRIB &doAttrib)
{
	LNTYPE_MAP::iterator it_LnType;
	if ( !FindLNTypeInMap(lnType,it_LnType) )
		return false;
	if ( it_LnType->second == NULL || it_LnType->second->doMap == NULL )
		return false;

	DO_MAP::iterator it_DO;
	int i = 0;
	for ( it_DO = it_LnType->second->doMap->begin(); it_DO != it_LnType->second->doMap->end(); it_DO++ )
	{
		if ( i == idxSeq )
		{
			// 作为Map Key的doName不变则直接更新属性信息，否则需替换
			// 注意，替换后DO在Map中的顺序可能会变化，Map内部按Key的大小排序
			if ( it_DO->second.name == doAttrib.name )
			{
				it_DO->second = doAttrib;
			}
			else
			{
				// 判断用于替换的DO是否已存在（doName在LNType中唯一）
				DO_MAP::iterator it_NewDO;
				if ( FindDOInLNType(it_LnType,doAttrib.name,it_NewDO) )
					return false;
				// 添加新DO
				if ( !AddDOIntoLNType(lnType,doAttrib) )
					return false;
				// 移除旧DO
				it_LnType->second->doMap->erase(it_DO);
			}

			return true;
		}
		i++;
	}
	return false;
}

///
/// @brief
///     设置Cache的某一LNType中的某一DO的信息
///
/// @param[in]
///     lnType     - DO所在LNType的ID
/// @param[in]
///     doName     - DO的名称
/// @param[in]
///     doAttrib   - DO的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetDOAttrib(const SCL_STR &lnType, const SCL_STR &doName, const SCL_DOINLNTYPE_ATTRIB &doAttrib)
{
	DO_MAP::iterator it_DO;

	// 搜索指定DO
	if ( !FindDOInMap(lnType,doName,it_DO) )
		return false;

	it_DO->second = doAttrib;

	return true;
}

///
/// @brief
///     从Cache的某一LNType中获取某一顺序号的DO的信息
///
/// @param[in]
///     lnType     - DO所在LNType的ID
/// @param[in]
///     idxSeq     - DO的顺序号，从0开始
/// @param[in]
///     doAttrib   - 保存所要获取的DO的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetDOAttrib(const SCL_STR &lnType, const int idxSeq, SCL_DOINLNTYPE_ATTRIB &doAttrib)
{
	LNTYPE_MAP::iterator it_LnType;
	if ( !FindLNTypeInMap(lnType,it_LnType) )
		return false;
	if ( it_LnType->second == NULL || it_LnType->second->doMap == NULL )
		return false;

	DO_MAP::iterator it_DO;
	int i = 0;
	for ( it_DO = it_LnType->second->doMap->begin(); it_DO != it_LnType->second->doMap->end(); it_DO++ )
	{
		if ( i == idxSeq )
		{
			doAttrib = it_DO->second;
			return true;
		}
		i++;
	}
	return false;
}

///
/// @brief
///     从Cache的某一LNType中获取某一DO的信息
///
/// @param[in]
///     lnType     - DO所在LNType的ID
/// @param[in]
///     doName     - DO的名称
/// @param[in]
///     doAttrib   - 保存所要获取的DO的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetDOAttrib(const SCL_STR &lnType, const SCL_STR &doName, SCL_DOINLNTYPE_ATTRIB &doAttrib)
{
	DO_MAP::iterator it_DO;

	// 搜索指定DO
	if ( !FindDOInMap(lnType,doName,it_DO) )
		return false;

	doAttrib = it_DO->second;

	return true;
}

///
/// @brief
///     获取Cache中DOType的数量
///
/// @return
///     DOType的数量
///
int  SCLCacheWithMap::GetDOTypeNum()
{
	if ( m_SCLMap == NULL || m_SCLMap->doTypeMap == NULL )
		return 0;
	return (int)m_SCLMap->doTypeMap->size();
}

///
/// @brief
///     设置Cache中的某一顺序号的DOType的信息
///
/// @param[in]
///     idxSeq       - DOType的顺序号
/// @param[in]
///     doTypeAttrib - 保存所要获取的DOType的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetDOType(const int idxSeq, const SCL_DOTYPE_ATTRIB &doTypeAttrib)
{
	if ( m_SCLMap == NULL || m_SCLMap->doTypeMap == NULL )
		return false;
	DOTYPE_MAP::iterator it_DoType;
	int i = 0;
	for ( it_DoType = m_SCLMap->doTypeMap->begin(); it_DoType != m_SCLMap->doTypeMap->end(); it_DoType++ )
	{
		if ( i == idxSeq )
		{
			if ( it_DoType->second == NULL )
				return false;

			// 作为Map Key的doType的ID不变则直接更新属性信息，否则需替换
			// 注意，替换后doType在Map中的顺序可能会变化，Map内部按Key的大小排序
			if ( it_DoType->second->doTypeAttrib.id == doTypeAttrib.id )
			{
				it_DoType->second->doTypeAttrib = doTypeAttrib;
			}
			else
			{
				// 判断用于替换的doType是否已存在（ID在Map中唯一）
				DOTYPE_MAP::iterator it_NewDoType;
				if ( FindDOTypeInMap(doTypeAttrib.id,it_NewDoType) )
					return false;
				// 添加新doType
				if ( !AddDOType(doTypeAttrib) )
					return false;
				if ( !FindDOTypeInMap(doTypeAttrib.id,it_NewDoType) )
					return false;
				it_NewDoType->second->daMap = it_DoType->second->daMap;
				// 移除旧doType
				it_DoType->second->daMap = NULL;
				delete it_DoType->second;
				it_DoType->second = NULL;
				m_SCLMap->doTypeMap->erase(it_DoType);
			}
			
			return true;
		}
		i++;
	}
	return false;
}

///
/// @brief
///     设置Cache中的某一ID的DOType的信息
///
/// @param[in]
///     doTypeID     - DOType的ID
/// @param[in]
///     doTypeAttrib - 保存所要获取的DOType的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetDOType(const SCL_STR &doTypeID, const SCL_DOTYPE_ATTRIB &doTypeAttrib)
{
	if ( m_SCLMap == NULL || m_SCLMap->doTypeMap == NULL )
		return false;
	DOTYPE_MAP::iterator it_DoType;
	if ( !FindDOTypeInMap(doTypeID,it_DoType) )
		return false;

	if ( it_DoType->second == NULL )
		return false;
	it_DoType->second->doTypeAttrib = doTypeAttrib;
	return true;
}

///
/// @brief
///     从Cache中获取某一顺序号的DOType的信息
///
/// @param[in]
///     idxSeq       - DOType的顺序号
/// @param[in]
///     doTypeAttrib - 保存所要获取的DOType的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetDOType(const int idxSeq, SCL_DOTYPE_ATTRIB &doTypeAttrib)
{
	if ( m_SCLMap == NULL || m_SCLMap->doTypeMap == NULL )
		return false;
	DOTYPE_MAP::iterator it_DoType;
	int i = 0;
	for ( it_DoType = m_SCLMap->doTypeMap->begin(); it_DoType != m_SCLMap->doTypeMap->end(); it_DoType++ )
	{
		if ( i == idxSeq )
		{
			if ( it_DoType->second == NULL )
				return false;
			doTypeAttrib = it_DoType->second->doTypeAttrib;
			return true;
		}
		i++;
	}
	return false;
}

///
/// @brief
///     从Cache中获取某一ID的DOType的信息
///
/// @param[in]
///     doTypeID     - DOType的ID
/// @param[in]
///     doTypeAttrib - 保存所要获取的DOType的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetDOType(const SCL_STR &doTypeID, SCL_DOTYPE_ATTRIB &doTypeAttrib)
{
	if ( m_SCLMap == NULL || m_SCLMap->doTypeMap == NULL )
		return false;
	DOTYPE_MAP::iterator it_DoType;
	if ( !FindDOTypeInMap(doTypeID,it_DoType) )
		return false;

	if ( it_DoType->second == NULL )
		return false;
	doTypeAttrib = it_DoType->second->doTypeAttrib;
	return true;
}

///
/// @brief
///     获取Cache中的某一DOType中的DA的数量
///
/// @param[in]
///     doType     - DA所在DOType的ID
///
/// @return
///     DA的数量
///
int  SCLCacheWithMap::GetDANumInDOType(const SCL_STR &doType)
{
	DOTYPE_MAP::iterator it_DoType;
	if ( !FindDOTypeInMap(doType,it_DoType) )
		return 0;
	if ( it_DoType->second == NULL || it_DoType->second->daMap == NULL )
		return 0;
	return (int)it_DoType->second->daMap->size();
}

///
/// @brief
///     设置Cache的某一DOType中的某一顺序号的DA的信息
///
/// @param[in]
///     doType     - DA所在DOType的ID
/// @param[in]
///     idxSeq     - DA的顺序号，从0开始
/// @param[in]
///     daAttrib   - DA的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetDAAttrib(const SCL_STR &doType, const int idxSeq, const SCL_DAINDOTYPE_ATTRIB &daAttrib)
{
	DOTYPE_MAP::iterator it_DoType;
	if ( !FindDOTypeInMap(doType,it_DoType) )
		return false;
	if ( it_DoType->second == NULL || it_DoType->second->daMap == NULL )
		return false;

	DA_MAP::iterator it_DA;
	it_DA = it_DoType->second->daMap->find(idxSeq);
	if ( it_DA == it_DoType->second->daMap->end() )
		return false;

	it_DA->second = daAttrib;
	return true;
}

///
/// @brief
///     设置Cache的某一DOType中的某一DA的信息
///
/// @param[in]
///     doType     - DA所在DOType的ID
/// @param[in]
///     daName     - DA的名称
/// @param[in]
///     daAttrib   - DA的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetDAAttrib(const SCL_STR &doType, const SCL_STR &daName, const SCL_DAINDOTYPE_ATTRIB &daAttrib)
{
	DA_MAP::iterator it_DA;

	// 搜索指定DA
	if ( !FindDAInMap(doType,daName,it_DA) )
		return false;

	it_DA->second = daAttrib;

	return true;
}

///
/// @brief
///     从Cache的某一DOType中获取某一顺序号的DA的信息
///
/// @param[in]
///     doType     - DA所在DOType的ID
/// @param[in]
///     idxSeq     - DA的顺序号，从0开始
/// @param[in]
///     daAttrib   - 保存所要获取的DA的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetDAAttrib(const SCL_STR &doType, const int idxSeq, SCL_DAINDOTYPE_ATTRIB &daAttrib)
{
	DOTYPE_MAP::iterator it_DoType;
	if ( !FindDOTypeInMap(doType,it_DoType) )
		return false;
	if ( it_DoType->second == NULL || it_DoType->second->daMap == NULL )
		return false;

	DA_MAP::iterator it_DA;
	it_DA = it_DoType->second->daMap->find(idxSeq);
	if ( it_DA == it_DoType->second->daMap->end() )
		return false;

	daAttrib = it_DA->second;
	return true;
}

///
/// @brief
///     从Cache的某一DOType中获取某一DA的信息
///
/// @param[in]
///     doType     - DA所在DOType的ID
/// @param[in]
///     daName     - DA的名称
/// @param[in]
///     daAttrib   - 保存所要获取的DA的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetDAAttrib(const SCL_STR &doType, const SCL_STR &daName, SCL_DAINDOTYPE_ATTRIB &daAttrib)
{
	DA_MAP::iterator it_DA;

	// 搜索指定DA
	if ( !FindDAInMap(doType,daName,it_DA) )
		return false;

	daAttrib = it_DA->second;

	return true;
}

///
/// @brief
///     获取Cache中DAType的数量
///
/// @return
///     DAType的数量
///
int  SCLCacheWithMap::GetDATypeNum()
{
	if ( m_SCLMap == NULL || m_SCLMap->daTypeMap == NULL )
		return 0;
	return (int)m_SCLMap->daTypeMap->size();
}

///
/// @brief
///     设置Cache中的某一顺序号的DAType的信息
///
/// @param[in]
///     idxSeq       - DAType的顺序号
/// @param[in]
///     daTypeAttrib - DAType的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetDAType(const int idxSeq, const SCL_DATYPE_ATTRIB &daTypeAttrib)
{
	if ( m_SCLMap == NULL || m_SCLMap->daTypeMap == NULL )
		return false;
	DATYPE_MAP::iterator it_DaType;
	int i = 0;
	for ( it_DaType = m_SCLMap->daTypeMap->begin(); it_DaType != m_SCLMap->daTypeMap->end(); it_DaType++ )
	{
		if ( i == idxSeq )
		{
			if ( it_DaType->second == NULL )
				return false;
			// 作为Map Key的daType的ID不变则直接更新属性信息，否则需替换
			// 注意，替换后daType在Map中的顺序可能会变化，Map内部按Key的大小排序
			if ( it_DaType->second->daTypeAttrib.id == daTypeAttrib.id )
			{
				it_DaType->second->daTypeAttrib = daTypeAttrib;
			}
			else
			{
				// 判断用于替换的daType是否已存在（ID在Map中唯一）
				DATYPE_MAP::iterator it_NewDaType;
				if ( FindDATypeInMap(daTypeAttrib.id,it_NewDaType) )
					return false;
				// 添加新daType
				if ( !AddDAType(daTypeAttrib) )
					return false;
				if ( !FindDATypeInMap(daTypeAttrib.id,it_NewDaType) )
					return false;
				it_NewDaType->second->bdaMap = it_DaType->second->bdaMap;
				// 移除旧daType
				it_DaType->second->bdaMap = NULL;
				delete it_DaType->second;
				it_DaType->second = NULL;
				m_SCLMap->daTypeMap->erase(it_DaType);
			}

			return true;
		}
		i++;
	}
	return false;
}

///
/// @brief
///     设置Cache中的某一ID的DAType的信息
///
/// @param[in]
///     daTypeID     - DAType的ID
/// @param[in]
///     daTypeAttrib - DAType的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetDAType(const SCL_STR &daTypeID, const SCL_DATYPE_ATTRIB &daTypeAttrib)
{
	if ( m_SCLMap == NULL || m_SCLMap->daTypeMap == NULL )
		return false;
	DATYPE_MAP::iterator it_DaType;
	if ( !FindDATypeInMap(daTypeID,it_DaType) )
		return false;

	if ( it_DaType->second == NULL )
		return false;
	it_DaType->second->daTypeAttrib = daTypeAttrib;
	return true;
}

///
/// @brief
///     从Cache中获取某一顺序号的DAType的信息
///
/// @param[in]
///     idxSeq       - DAType的顺序号
/// @param[in]
///     daTypeAttrib - 保存所要获取的DAType的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetDAType(const int idxSeq, SCL_DATYPE_ATTRIB &daTypeAttrib)
{
	if ( m_SCLMap == NULL || m_SCLMap->daTypeMap == NULL )
		return false;
	DATYPE_MAP::iterator it_DaType;
	int i = 0;
	for ( it_DaType = m_SCLMap->daTypeMap->begin(); it_DaType != m_SCLMap->daTypeMap->end(); it_DaType++ )
	{
		if ( i == idxSeq )
		{
			if ( it_DaType->second == NULL )
				return false;
			daTypeAttrib = it_DaType->second->daTypeAttrib;
			return true;
		}
		i++;
	}
	return false;
}

///
/// @brief
///     从Cache中获取某一ID的DAType的信息
///
/// @param[in]
///     daTypeID     - DAType的ID
/// @param[in]
///     daTypeAttrib - DAType的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetDAType(const SCL_STR &daTypeID, SCL_DATYPE_ATTRIB &daTypeAttrib)
{
	if ( m_SCLMap == NULL || m_SCLMap->daTypeMap == NULL )
		return false;
	DATYPE_MAP::iterator it_DaType;
	if ( !FindDATypeInMap(daTypeID,it_DaType) )
		return false;

	if ( it_DaType->second == NULL )
		return false;
	daTypeAttrib = it_DaType->second->daTypeAttrib;
	return true;
}

///
/// @brief
///     获取Cache中的某一DAType中的BDA的数量
///
/// @param[in]
///     daType     - BDA所在DAType的ID
///
/// @return
///     BDA的数量
///
int  SCLCacheWithMap::GetBDANumInDAType(const SCL_STR &daType)
{
	DATYPE_MAP::iterator it_DaType;
	if ( !FindDATypeInMap(daType,it_DaType) )
		return 0;
	if ( it_DaType->second == NULL || it_DaType->second->bdaMap == NULL )
		return 0;
	return (int)it_DaType->second->bdaMap->size();
}

///
/// @brief
///     设置Cache的某一DAType中的某一顺序号的BDA的信息
///
/// @param[in]
///     daType     - BDA所在DAType的ID
/// @param[in]
///     idxSeq     - BDA的顺序号，从0开始
/// @param[in]
///     bdaAttrib  - BDA的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetBDAAttrib(const SCL_STR &daType, const int idxSeq, const SCL_BDAINDATYPE_ATTRIB &bdaAttrib)
{
	DATYPE_MAP::iterator it_DaType;
	if ( !FindDATypeInMap(daType,it_DaType) )
		return false;
	if ( it_DaType->second == NULL || it_DaType->second->bdaMap == NULL )
		return false;

	BDA_MAP::iterator it_BDA;
	it_BDA = it_DaType->second->bdaMap->find(idxSeq);
	if ( it_BDA == it_DaType->second->bdaMap->end() )
		return false;

	it_BDA->second = bdaAttrib;
	return true;
}

///
/// @brief
///     设置Cache的某一DAType中的某一BDA的信息
///
/// @param[in]
///     daType     - BDA所在DAType的ID
/// @param[in]
///     bdaName    - BDA的名称
/// @param[in]
///     bdaAttrib  - BDA的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetBDAAttrib(const SCL_STR &daType, const SCL_STR &bdaName, const SCL_BDAINDATYPE_ATTRIB &bdaAttrib)
{
	BDA_MAP::iterator it_BDA;

	// 搜索指定BDA
	if ( !FindBDAInMap(daType,bdaName,it_BDA) )
		return false;

	it_BDA->second = bdaAttrib;

	return true;
}

///
/// @brief
///     从Cache的某一DAType中获取某一顺序号的BDA的信息
///
/// @param[in]
///     daType     - BDA所在DAType的ID
/// @param[in]
///     idxSeq     - BDA的顺序号，从0开始
/// @param[in]
///     bdaAttrib  - 保存所要获取的BDA的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetBDAAttrib(const SCL_STR &daType, const int idxSeq, SCL_BDAINDATYPE_ATTRIB &bdaAttrib)
{
	DATYPE_MAP::iterator it_DaType;
	if ( !FindDATypeInMap(daType,it_DaType) )
		return false;
	if ( it_DaType->second == NULL || it_DaType->second->bdaMap == NULL )
		return false;

	BDA_MAP::iterator it_BDA;
	int i = 0;
	for ( it_BDA = it_DaType->second->bdaMap->begin(); it_BDA != it_DaType->second->bdaMap->end(); it_BDA++ )
	{
		if ( i == idxSeq )
		{
			bdaAttrib = it_BDA->second;
			return true;
		}
		i++;
	}
	return false;
}

///
/// @brief
///     从Cache的某一DAType中获取某一BDA的信息
///
/// @param[in]
///     daType     - BDA所在DAType的ID
/// @param[in]
///     bdaName    - BDA的名称
/// @param[in]
///     bdaAttrib  - 保存所要获取的BDA的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetBDAAttrib(const SCL_STR &daType, const SCL_STR &bdaName, SCL_BDAINDATYPE_ATTRIB &bdaAttrib)
{
	BDA_MAP::iterator it_BDA;

	// 搜索指定BDA
	if ( !FindBDAInMap(daType,bdaName,it_BDA) )
		return false;

	bdaAttrib = it_BDA->second;

	return true;
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
int SCLCacheWithMap::IsSMVAppIDExisted(const unsigned short appID)
{
	// 搜索指定SMV
	SMV_COMMN_MAP::iterator it_SMV;
	if ( !FindSMVCommnInMap(appID,it_SMV) )
		return -1;
	if ( it_SMV->second == NULL )
		return -1;
	else
		return 0;
}

///
/// @brief
///     获取Cache中SampledValueControl的数目
///
/// @return
///     SampledValueControl的数目
///
int SCLCacheWithMap::GetSMVCommnNumInCache()
{
	if ( m_SCLMap == NULL || m_SCLMap->smvCommnMap == NULL )
		return 0;

	SMV_COMMN_MAP::iterator it_smv;
	int i = 0;
	for ( it_smv = m_SCLMap->smvCommnMap->begin(); it_smv != m_SCLMap->smvCommnMap->end(); it_smv++)
	{
		SMV_COMMN_INFO_CACHE *pInfo = it_smv->second;
		while ( pInfo != NULL )
		{
			i++;
			pInfo = pInfo->pNextCache;
		}
	}
	return i;
}

///
/// @brief
///     获取Cache中全部的SampledValueControl的通信参数信息
///
/// @param[in]
///     cnntAPAttribArray - 通信参数所关联的控制块所在的访问点的信息
/// @param[in]
///     smvAttribArray    - SampledValueControl的通信参数信息
/// @param[in]
///    arraySize          - 保存通信参数信息的数组的Size
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetAllSMVCommn(SCL_CONNECTEDAP_ATTRIB *cnnctAPAttribArray, SCL_SMV_ATTRIB *smvAttribArray, const int arraySize)
{
	if ( m_SCLMap == NULL || m_SCLMap->smvCommnMap == NULL )
		return false;

	SMV_COMMN_MAP::iterator it_smv;
	int i = 0;
	for ( it_smv = m_SCLMap->smvCommnMap->begin(); it_smv != m_SCLMap->smvCommnMap->end(); it_smv++)
	{
		SMV_COMMN_INFO_CACHE *pInfo = it_smv->second;
		while ( pInfo != NULL )
		{
			cnnctAPAttribArray[i] = pInfo->cnntAPAttrib;
			smvAttribArray[i] = pInfo->smvAttrib;
			i++;
			pInfo = pInfo->pNextCache;
			if (i >= arraySize)
				return true;
		}
	}

	if ( i == 0 )
		return false;
	return true;
}

///
/// @brief
///     设置Cache中的某一以iedName、ldInst和cbName为索引的SampledValueControl所关联的全部通信参数信息
///
/// @param[in]
///     iedName       - SampledValueControl所在的IED的名称
/// @param[in]
///     ldInst        - SampledValueControl所在的LDevice的实例号
/// @param[in]
///     cbName        - SampledValueControl的名称
/// @param[in]
///     conntAPAttrib - SampledValueControl所在的访问点的信息
/// @param[in]
///     smvAddr       - SampledValueControl的通信参数信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetSMVCommnWithIdx(const std::wstring iedName, const std::wstring ldInst, 
										 const std::wstring cbName, const SCL_CONNECTEDAP_ATTRIB &conntAPAttrib,
										 const SCL_SMV_ATTRIB &smvAddr)
{
	if ( m_SCLMap == NULL || m_SCLMap->smvCommnMap == NULL )
		return false;

	SMV_COMMN_MAP::iterator it_SMV;
	for ( it_SMV = m_SCLMap->smvCommnMap->begin(); it_SMV != m_SCLMap->smvCommnMap->end(); it_SMV++ )
	{
		SMV_COMMN_INFO_CACHE *pInfo = it_SMV->second;
		SMV_COMMN_INFO_CACHE *pPrevInfo = NULL;
		while ( pInfo != NULL )
		{
			if ( pInfo->cnntAPAttrib.iedName == iedName &&
				pInfo->smvAttrib.ldInst == ldInst &&
				pInfo->smvAttrib.cbName == cbName )
			{
				if ( pInfo->smvAttrib.appID == smvAddr.appID )
				{
					// AppID不变，则直接替换
					pInfo->cnntAPAttrib = conntAPAttrib;
					pInfo->smvAttrib = smvAddr;
				}
				else
				{
					// AppID改变，则删掉旧的，并重新添加
					if ( pPrevInfo == NULL )
						it_SMV->second = pInfo->pNextCache;
					else
						pPrevInfo->pNextCache = pInfo->pNextCache;
					// 移除旧SmvCommn
					if ( it_SMV->second == NULL )
					{
						m_SCLMap->smvCommnMap->erase(it_SMV);
					}
					delete pInfo;
					// 添加新SmvCommn
					if ( !AddSMVCommn(conntAPAttrib,smvAddr) )
						return false;
				}
				return true;
			}
			pPrevInfo = pInfo;
			pInfo = pInfo->pNextCache;
		}
	}
	return false;
}

///
/// @brief
///     从Cache中获取某一SampledValueControl所关联的全部通信参数信息
///     若存在多个相同APPID，则返回第一个
///
/// @param[in]
///     appID         - 通信参数中的appID项,全站唯一
/// @param[in]
///     conntAPAttrib - 保存所要获取的通信参数所关联的SampledValueControl所在的访问点的信息
/// @param[in]
///     smvAddr       - 保存所要获取的通信参数信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetSMVCommn(const unsigned short appID, SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, SCL_SMV_ATTRIB &smvAddr)
{
	// 搜索指定SMV
	SMV_COMMN_MAP::iterator it_SMV;
	if ( !FindSMVCommnInMap(appID,it_SMV) )
		return false;

	if ( it_SMV->second == NULL )
		return false;

	conntAPAttrib = it_SMV->second->cnntAPAttrib;
	smvAddr = it_SMV->second->smvAttrib;

	return true;
}

///
/// @brief
///     从Cache中获取某一以iedName、ldInst和cbName为索引的SampledValueControl所关联的全部通信参数信息
///
/// @param[in]
///     iedName       - SampledValueControl所在的IED的名称
/// @param[in]
///     ldInst        - SampledValueControl所在的LDevice的实例号
/// @param[in]
///     cbName        - SampledValueControl的名称
/// @param[in]
///     conntAPAttrib - 保存所要获取的通信参数所关联的SampledValueControl所在的访问点的信息
/// @param[in]
///     smvAddr       - 保存所要获取的通信参数信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetSMVCommnWithIdx(const std::wstring iedName, const std::wstring ldInst, const std::wstring cbName, SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, SCL_SMV_ATTRIB &smvAddr)
{
	if ( m_SCLMap == NULL || m_SCLMap->smvCommnMap == NULL )
		return false;

	SMV_COMMN_MAP::iterator it_SMV;
	for ( it_SMV = m_SCLMap->smvCommnMap->begin(); it_SMV != m_SCLMap->smvCommnMap->end(); it_SMV++ )
	{
		SMV_COMMN_INFO_CACHE *pInfo = it_SMV->second;

		while ( pInfo != NULL )
		{
			if ( pInfo->cnntAPAttrib.iedName == iedName &&
				 pInfo->smvAttrib.ldInst == ldInst &&
				 pInfo->smvAttrib.cbName == cbName )
			{
				conntAPAttrib = pInfo->cnntAPAttrib;
				smvAddr = pInfo->smvAttrib;
				return true;
			}
			pInfo = pInfo->pNextCache;
		}
	}
	return false;
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
int SCLCacheWithMap::IsGSEAppIDExisted(const unsigned short appID)
{
	// 搜索指定GSE
	GSE_COMMN_MAP::iterator it_GSE;
	if ( !FindGSECommnInMap(appID,it_GSE) )
		return -1;
	if ( it_GSE->second == NULL )
		return -1;
	else
		return 0;
}

///
/// @brief
///     获取Cache中GSEControl的数目
///
/// @return
///     GSEControl的数目
///
int SCLCacheWithMap::GetGSECommnNumInCache()
{
	if ( m_SCLMap == NULL || m_SCLMap->gseCommnMap == NULL )
		return 0;

	GSE_COMMN_MAP::iterator it_GSE;
	int i = 0;
	for ( it_GSE = m_SCLMap->gseCommnMap->begin(); it_GSE != m_SCLMap->gseCommnMap->end(); it_GSE++)
	{
		GSE_COMMN_INFO_CACHE *pInfo = it_GSE->second;
		while ( pInfo != NULL )
		{
			i++;
			pInfo = pInfo->pNextCache;
		}
	}
	return i;
}

///
/// @brief
///     获取Cache中全部的GSEControl的通信参数信息
///
/// @param[in]
///     cnntAPAttribArray - 通信参数所关联的控制块所在的访问点的信息
/// @param[in]
///     gseAttribArray    - GSEControl的通信参数信息
/// @param[in]
///    arraySize          - 保存通信参数信息的数组的Size
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetAllGSECommn(SCL_CONNECTEDAP_ATTRIB *cnnctAPAttribArray, SCL_GSE_ATTRIB *gseAttribArray, const int arraySize)
{
	if ( m_SCLMap == NULL || m_SCLMap->gseCommnMap == NULL )
		return false;

	GSE_COMMN_MAP::iterator it_GSE;
	int i = 0;
	for ( it_GSE = m_SCLMap->gseCommnMap->begin(); it_GSE != m_SCLMap->gseCommnMap->end(); it_GSE++)
	{
		GSE_COMMN_INFO_CACHE *pInfo = it_GSE->second;
		while ( pInfo != NULL )
		{
			cnnctAPAttribArray[i] = pInfo->cnntAPAttrib;
			gseAttribArray[i] = pInfo->gseAttrib;
			i++;
			pInfo = pInfo->pNextCache;
			if (i >= arraySize)
				return true;
		}
	}

	if ( i == 0 )
		return false;
	return true;
}

///
/// @brief
///     设置Cache中的某一以iedName、ldInst和cbName为索引的GSEControl所关联的全部通信参数信息
///
/// @param[in]
///     iedName       - GSEControl所在的IED的名称
/// @param[in]
///     ldInst        - GSEControl所在的LDevice的实例号
/// @param[in]
///     cbName        - GSEControl的名称
/// @param[in]
///     conntAPAttrib - GSEControl所在的访问点的信息
/// @param[in]
///     smvAddr       - GSEControl的通信参数信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::SetGSECommnWithIdx(const SCL_STR iedName, const SCL_STR ldInst, const SCL_STR cbName,
										 const SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, const SCL_GSE_ATTRIB &gseAddr)
{
	if ( m_SCLMap == NULL || m_SCLMap->gseCommnMap == NULL )
		return false;

	GSE_COMMN_MAP::iterator it_GSE;
	for ( it_GSE = m_SCLMap->gseCommnMap->begin(); it_GSE != m_SCLMap->gseCommnMap->end(); it_GSE++ )
	{
		GSE_COMMN_INFO_CACHE *pInfo = it_GSE->second;
		GSE_COMMN_INFO_CACHE *pPrevInfo = NULL;
		while ( pInfo != NULL )
		{
			if ( pInfo->cnntAPAttrib.iedName == iedName &&
				pInfo->gseAttrib.ldInst == ldInst &&
				pInfo->gseAttrib.cbName == cbName )
			{
				if ( pInfo->gseAttrib.appID == gseAddr.appID )
				{
					// AppID不变，则直接替换
					pInfo->cnntAPAttrib = conntAPAttrib;
					pInfo->gseAttrib = gseAddr;
				}
				else
				{
					// AppID改变，则删掉旧的，并重新添加
					if ( pPrevInfo == NULL )
						it_GSE->second = pInfo->pNextCache;
					else
						pPrevInfo->pNextCache = pInfo->pNextCache;
					// 移除旧GseCommn
					if ( it_GSE->second == NULL )
					{
						m_SCLMap->gseCommnMap->erase(it_GSE);
					}
					delete pInfo;
					// 添加新GseCommn
					if ( !AddGSECommn(conntAPAttrib,gseAddr) )
						return false;
				}
				return true;
			}
			pPrevInfo = pInfo;
			pInfo = pInfo->pNextCache;
		}
	}
	return false;
}

///
/// @brief
///     从Cache中获取某一GSEControl所关联的全部通信参数信息
///     若存在多个相同APPID，则返回第一个
///
/// @param[in]
///     appID         - 通信参数中的appID项，全站唯一
/// @param[in]
///     conntAPAttrib - 保存所要获取的通信参数所关联的GSEControl所在的访问点的信息
/// @param[in]
///     gseAddr       - 保存所要获取的通信参数信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetGSECommn(const unsigned short appID, SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, SCL_GSE_ATTRIB &gseAddr)
{
	// 搜索指定GSE
	GSE_COMMN_MAP::iterator it_GSE;
	if ( ! FindGSECommnInMap(appID,it_GSE) )
		return false;

	if ( it_GSE->second == NULL )
		return false;
	conntAPAttrib = it_GSE->second->cnntAPAttrib;
	gseAddr = it_GSE->second->gseAttrib;

	return true;
}

///
/// @brief
///     从Cache中获取某一以iedName、ldInst和cbName为索引的GSEControl所关联的全部通信参数信息
///
/// @param[in]
///     iedName       - GSEControl所在的IED的名称
/// @param[in]
///     ldInst        - GSEControl所在的LDevice的实例号
/// @param[in]
///     cbName        - GSEControl的名称
/// @param[in]
///     conntAPAttrib - 保存所要获取的通信参数所关联的GSEControl所在的访问点的信息
/// @param[in]
///     smvAddr       - 保存所要获取的通信参数信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::GetGSECommnWithIdx(const SCL_STR iedName, const SCL_STR ldInst, const SCL_STR cbName, SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, SCL_GSE_ATTRIB &gseAddr)
{
	if ( m_SCLMap == NULL || m_SCLMap->gseCommnMap == NULL )
		return false;

	GSE_COMMN_MAP::iterator it_GSE;
	for ( it_GSE = m_SCLMap->gseCommnMap->begin(); it_GSE != m_SCLMap->gseCommnMap->end(); it_GSE++ )
	{
		GSE_COMMN_INFO_CACHE *pInfo = it_GSE->second;

		while ( pInfo != NULL )
		{
			if ( pInfo->cnntAPAttrib.iedName == iedName &&
				 pInfo->gseAttrib.ldInst == ldInst &&
				 pInfo->gseAttrib.cbName == cbName )
			{
				conntAPAttrib = pInfo->cnntAPAttrib;
				gseAddr = pInfo->gseAttrib;
				return true;
			}
			pInfo = pInfo->pNextCache;
		}
	}
	return false;
}

///
/// @brief
///     从Cache中移除某一以iedName、ldInst、lnName和dataSetName为索引的某一顺序号的FCDA
///
/// @param[in]
///     iedName       - FCDA所在的IED的名称
/// @param[in]
///     ldInst        - FCDA所在的LDevice的实例号
/// @param[in]
///     lnName        - FCDA所在的LN的名称
/// @param[in]
///     dataSetName   - FCDA所在的DataSet的名称
/// @param[in]
///     idxSeq        - FCDA的顺序号（从0开始）
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::RemoveFCDAAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
									   const SCL_STR &datSetName, const int idxSeq)
{
	// 搜索指定DataSet
	DATASET_MAP::iterator it_DataSet;
	if ( lnName == _T("LLN0") )
	{
		if ( !FindDataSetInLN0InMap(iedName,ldInst,datSetName,it_DataSet) )
			return false;
	}
	else
	{
//		if ( !FindDataSetInLNInMap(iedName,ldInst,lnName,dataSetName,it_DataSet) )
		return false;
	}

	if ( it_DataSet->second == NULL || it_DataSet->second->fcdaMap == NULL )
		return false;

	// 在DataSet中搜索FCDA
	FCDA_MAP *fcdaMap = it_DataSet->second->fcdaMap;
	FCDA_MAP::iterator it_FCDA = fcdaMap->find(idxSeq);

	if ( it_FCDA == fcdaMap->end() )
		return false;

	// 释放FCDA
	if ( it_FCDA->second != NULL )
	{
		delete it_FCDA->second;
		it_FCDA->second = NULL;
	}

	// 目标FCDA之后的部分向前移1
	FCDA_MAP::iterator it_NextFCDA = fcdaMap->find(idxSeq+1);
	for ( ; it_NextFCDA != fcdaMap->end(); it_NextFCDA++ )
	{
		it_NextFCDA->second->fcdaAttrib.idxSeq--;
		it_FCDA->second = it_NextFCDA->second;
		it_FCDA         = it_NextFCDA;
	}

	// 移除Map中最后一项纪录
	it_FCDA->second = NULL;
	fcdaMap->erase(it_FCDA);

	return true;
}

///
/// @brief
///     从Cache中移除指定DoType中的某一顺序号的DA
///
/// @param[in]
///     doTypeID      - doType的ID
/// @param[in]
///     idxSeq        - DA的顺序号（从0开始）
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::RemoveDAAttrib(const SCL_STR &doTypeID, const int idxSeq)
{
	// 搜索指定DoType
	DOTYPE_MAP::iterator it_DoType;
	if ( !FindDOTypeInMap(doTypeID,it_DoType) )
		return false;
	if ( it_DoType->second == NULL || it_DoType->second->daMap == NULL )
		return false;

	// 在DoType中搜索DA
	DA_MAP *daMap = it_DoType->second->daMap;
	DA_MAP::iterator it_DA;
	it_DA = daMap->find(idxSeq);
	if ( it_DA == daMap->end() )
		return false;

	// 目标DA之后的部分向前移1
	DA_MAP::iterator it_NextDA = daMap->find(idxSeq+1);
	for ( ; it_NextDA != daMap->end(); it_NextDA++ )
	{
		it_DA->second = it_NextDA->second;
		it_DA         = it_NextDA;
	}

	// 移除Map中最后一项纪录
	daMap->erase(it_DA);
	
	return true;
}

///
/// @brief
///     从Cache中移除指定DaType中的某一顺序号的BDA
///
/// @param[in]
///     daTypeID      - daType的ID
/// @param[in]
///     idxSeq        - BDA的顺序号（从0开始）
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::RemoveBDAAttrib(const SCL_STR &daTypeID, const int idxSeq)
{
	// 搜索指定DaType
	DATYPE_MAP::iterator it_DaType;
	if ( !FindDATypeInMap(daTypeID,it_DaType) )
		return false;
	if ( it_DaType->second == NULL || it_DaType->second->bdaMap == NULL )
		return false;

	// 在DaType中搜索BDA
	BDA_MAP *bdaMap = it_DaType->second->bdaMap;
	BDA_MAP::iterator it_BDA;
	it_BDA = bdaMap->find(idxSeq);
	if ( it_BDA == bdaMap->end() )
		return false;

	// 目标BDA之后的部分向前移1
	BDA_MAP::iterator it_NextBDA = bdaMap->find(idxSeq+1);
	for ( ; it_NextBDA != bdaMap->end(); it_NextBDA++ )
	{
		it_BDA->second = it_NextBDA->second;
		it_BDA         = it_NextBDA;
	}

	// 移除Map中最后一项纪录
	bdaMap->erase(it_BDA);

	return true;
}

///
/// @brief
///     获取Cache中某一IED的引用
///
/// @param[in]
///     iedName - IED的名称
/// @param[in]
///     it_IED  - 保存所要获取的IED的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindIEDInMap(const SCL_STR &iedName, IED_MAP::iterator &it_IED)
{
	if ( m_SCLMap == NULL || m_SCLMap->iedMap == NULL )
		return false;

	// 在IEDMap中搜索指定IED
	it_IED = m_SCLMap->iedMap->find(iedName);
	if ( it_IED == m_SCLMap->iedMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取某一IED中的某一LDevice的引用
///
/// @param[in]
///     it_IED  - IED的引用
/// @param[in]
///     ldInst  - LDevice的实例号
/// @param[in]
///     it_LD   - 保存所要获取的LDevice的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindLDeviceInIED(const IED_MAP::iterator &it_IED, const SCL_STR &ldInst, LDEVICE_MAP::iterator &it_LD)
{
	if ( it_IED->second == NULL || it_IED->second->lDeviceMap == NULL )
		return false;

	// 在IDeviceMap中搜索指定LDevice
	it_LD = it_IED->second->lDeviceMap->find(ldInst);
	if ( it_LD == it_IED->second->lDeviceMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取某一LDevice中的LN0的引用
///
/// @param[in]
///     it_LD   - LDevice的引用
/// @param[in]
///     ln0     - 指向目标LN0的引用的指针
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindLN0InLDevice(const LDEVICE_MAP::iterator &it_LD, LN0_INFO_CACHE **ln0)
{
	if ( it_LD->second == NULL )
		return false;

	//  在LDevice中搜索LN0
	*ln0 = it_LD->second->ln0InfoCache;
	if ( *ln0 == NULL )
		return false;

	return true;
}

///
/// @brief
///     获取某一LDevice的某一LN的引用
///
/// @param[in]
///     it_LD   - LDevice的引用
/// @param[in]
///     lnName  - 逻辑节点名称
/// @param[in]
///     it_LN   - 保存所要获取的LN的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindLNInLDevice(const LDEVICE_MAP::iterator &it_LD, const SCL_STR &lnName, LN_MAP::iterator &it_LN)
{
	if ( it_LD->second == NULL || it_LD->second->lnMap == NULL )
		return false;

	// 在LNMap中搜索指定LN
	it_LN = it_LD->second->lnMap->find(lnName);
	if ( it_LN == it_LD->second->lnMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取某一LN0中某一DataSet的引用
///
/// @param[in]
///     ln0         - LN0的引用
/// @param[in]
///     dataSetName - DataSet的名称
/// @param[in]
///     it_DataSet  - 保存所要获取的DataSet的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindDataSetInLN0(const LN0_INFO_CACHE * const ln0, const SCL_STR &dataSetName, DATASET_MAP::iterator &it_DataSet)
{
	if ( ln0->datasetMap == NULL )
		return false;

	// 在LN0在搜索DataSet
	it_DataSet = ln0->datasetMap->find(dataSetName);
	if ( it_DataSet == ln0->datasetMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取某一DataSet中的FCDA的引用
///
/// @param[in]
///     it_DataSet - DataSet的引用
/// @param[in]
///     idxSeq     - FCDA在DataSet中的顺序号，从0开始
/// @param[in]
///     it_FCDA    - 保存所要获取的FCDA的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindFCDAInDataSet(const DATASET_MAP::iterator &it_DataSet, const int idxSeq, FCDA_MAP::iterator &it_FCDA)
{
	if ( it_DataSet->second == NULL || it_DataSet->second->fcdaMap == NULL )
		return false;

	// 在DataSet中搜索FCDA
	it_FCDA = it_DataSet->second->fcdaMap->find(idxSeq);
	if ( it_FCDA == it_DataSet->second->fcdaMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取某一LN0中的SampledValueControl的引用
///
/// @param[in]
///     ln0        - LN0的引用
/// @param[in]
///     cbName     - 控制块的名称
/// @param[in]
///     it_SMVCtrl - 保存所要获取的SampledValueControl的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindSMVCtrlInLN0(const LN0_INFO_CACHE * const ln0, const SCL_STR &cbName, SMV_CTRL_MAP::iterator &it_SMVCtrl)
{
	if ( ln0->smvCtrlMap == NULL )
		return false;

	// 在LN0中搜索SMV控制块
	it_SMVCtrl = ln0->smvCtrlMap->find(cbName);
	if ( it_SMVCtrl == ln0->smvCtrlMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取某一LN0中的GSEControl的引用
///
/// @param[in]
///     ln0        - LN0的引用
/// @param[in]
///     cbName     - 控制块的名称
/// @param[in]
///     it_GSECtrl - 保存所要获取的GSEControl的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindGSECtrlInLN0(const LN0_INFO_CACHE * const ln0, const SCL_STR &cbName, GSE_CTRL_MAP::iterator &it_GSECtrl)
{
	if ( ln0->gseCtrlMap == NULL )
		return false;

	// 在LN0中搜索GSE控制块
	it_GSECtrl = ln0->gseCtrlMap->find(cbName);
	if ( it_GSECtrl == ln0->gseCtrlMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取某一LN0中的DOI的引用
///
/// @param[in]
///     ln0        - LN0的引用
/// @param[in]
///     doiName    - DOI的名称
/// @param[in]
///     it_DOI     - 保存所要获取的DOI的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindDOIInLN0(const LN0_INFO_CACHE * const ln0, const SCL_STR &doiName, DOI_MAP::iterator &it_DOI)
{
	if ( ln0->doiMap == NULL )
		return false;

	// 在LN0中搜索DOI
	it_DOI = ln0->doiMap->find(doiName);
	if ( it_DOI == ln0->doiMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取某一LN中的DOI的引用
///
/// @param[in]
///     it_LN      - LN的引用
/// @param[in]
///     doiName    - DOI的名称
/// @param[in]
///     it_DOI     - 保存所要获取的DOI的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindDOIInLN(const LN_MAP::iterator &it_LN, const SCL_STR &doiName, DOI_MAP::iterator &it_DOI)
{
	if ( it_LN->second == NULL || it_LN->second->doiMap == NULL )
		return false;

	// 在LN0中搜索DOI
	it_DOI = it_LN->second->doiMap->find(doiName);
	if ( it_DOI == it_LN->second->doiMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取某一LN0中的ExtRef的引用
///
/// @param[in]
///     ln0        - LN0的引用
/// @param[in]
///     idxSeq     - ExtRef的顺序号，从0开始
/// @param[in]
///     it_ExtRef  - 保存所要获取的ExtRef的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindExtRefInLN0(const LN0_INFO_CACHE * const ln0, const int &idxSeq, EXTREF_MAP::iterator &it_ExtRef)
{
	if ( ln0->extRefMap == NULL )
		return false;

	// 在LN0中搜索ExtRef
	it_ExtRef = ln0->extRefMap->find(idxSeq);
	if ( it_ExtRef == ln0->extRefMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取某一LN中的ExtRef的引用
///
/// @param[in]
///     it_LN      - LN的引用
/// @param[in]
///     idxSeq     - ExtRef的顺序号，从0开始
/// @param[in]
///     it_ExtRef  - 保存所要获取的ExtRef的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindExtRefInLN(const LN_MAP::iterator &it_LN, const int &idxSeq, EXTREF_MAP::iterator &it_ExtRef)
{
	if ( it_LN->second == NULL || it_LN->second->extRefMap == NULL )
		return false;

	// 在LN0中搜索ExtRef
	it_ExtRef = it_LN->second->extRefMap->find(idxSeq);
	if ( it_ExtRef == it_LN->second->extRefMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取Cache中某一IED的LDevice的引用
///
/// @param[in]
///     iedName - IED的名称
/// @param[in]
///     ldInst  - LDevice的实例号
/// @param[in]
///     it_LD   - 保存所要获取的LDevice的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindLDeviceInMap(const SCL_STR &iedName, const SCL_STR &ldInst, LDEVICE_MAP::iterator &it_LD)
{
	// 在IEDMap中搜索指定IED
	IED_MAP::iterator it_IED;
	if ( !FindIEDInMap(iedName,it_IED) )
		return false;

	return FindLDeviceInIED(it_IED,ldInst,it_LD);
}

///
/// @brief
///     获取Cache中某一IED的某一LDevice的LN0的引用
///
/// @param[in]
///     iedName - IED的名称
/// @param[in]
///     ldInst  - LDevice的实例号
/// @param[in]
///     ln0     - 指向所要获取的LN0的引用的指针
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindLN0InMap(const SCL_STR &iedName, const SCL_STR &ldInst ,LN0_INFO_CACHE **ln0)
{
	// 搜索指定LDevice
	LDEVICE_MAP::iterator it_LD;
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	return FindLN0InLDevice(it_LD,ln0);
}

///
/// @brief
///     获取Cache中某一IED的某一LDevice的LN的引用
///
/// @param[in]
///     iedName - IED的名称
/// @param[in]
///     ldInst  - LDevice的实例号
/// @param[in]
///     lnName  - LN的名称
/// @param[in]
///     it_LN   - 保存所要获取的LN的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindLNInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, LN_MAP::iterator &it_LN)
{
	// 搜索指定LDevice
	LDEVICE_MAP::iterator it_LD;
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	return FindLNInLDevice(it_LD,lnName,it_LN);
}

///
/// @brief
///     获取Cache中某一IED的某一LDevice的LN0中的DataSet引用
///
/// @param[in]
///     iedName     - IED的名称
/// @param[in]
///     ldInst      - LDevice的实例号
/// @param[in]
///     dataSetName - DataSet的名称
/// @param[in]
///     it_DataSet  - 保存所要获取的DatasET的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindDataSetInLN0InMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, DATASET_MAP::iterator &it_DataSet)
{
	// 搜索指定LN0
	LN0_INFO_CACHE *ln0;
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	return FindDataSetInLN0(ln0,dataSetName,it_DataSet);
}

///
/// @brief
///     获取Cache中某一IED的某一LDevice的某一LNode的某一DataSet中的某一顺序号的FCDA的引用
///
/// @param[in]
///     iedName     - IED的名称
/// @param[in]
///     ldInst      - LDevice的实例号
/// @param[in]
///     lnName      - LNode的名称，LN0的逻辑节点名称固定为“LLN0”
/// @param[in]
///     dataSetname - DataSet的名称
/// @param[in]
///     idxSeq      - FCDA在DataSet中的顺序号
/// @param[in]
///     it_FCDA     - 保存所要获取的FCDA的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindFCDAInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
									const SCL_STR &dataSetName,const int idxSeq, FCDA_MAP::iterator &it_FCDA)
{
	// 搜索指定DataSet
	DATASET_MAP::iterator it_DataSet;
	if ( lnName == _T("LLN0") )
	{
		if ( !FindDataSetInLN0InMap(iedName,ldInst,dataSetName,it_DataSet) )
			return false;
	}
	else
	{
//		if ( !FindDataSetInLNInMap(iedName,ldInst,lnName,dataSetName,it_DataSet) )
		return false;
	}

	return FindFCDAInDataSet(it_DataSet,idxSeq,it_FCDA);
}

///
/// @brief
///     获取Cache中某一IED的某一LDevice的SampledValueControl的引用
///
/// @param[in]
///     iedName    - IED的名称
/// @param[in]
///     ldInst     - LDevice的实例号
/// @param[in]
///     cbName     - SampledValueControl的名称
/// @param[in]
///     it_SMVCtrl - 保存所要获取的SampledValueControl的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindSMVCtrlInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, SMV_CTRL_MAP::iterator &it_SMVCtrl)
{
	// 搜索指定LN0
	LN0_INFO_CACHE *ln0;
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	return FindSMVCtrlInLN0(ln0,cbName,it_SMVCtrl);
}

///
/// @brief
///     获取Cache中某一IED的某一LDevice的GSEControl的引用
///
/// @param[in]
///     iedName    - IED的名称
/// @param[in]
///     ldInst     - LDevice的实例号
/// @param[in]
///     cbName     - GSEControl的名称
/// @param[in]
///     it_GSECtrl - 保存所要获取的GSEControl的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindGSECtrlInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, GSE_CTRL_MAP::iterator &it_GSECtrl)
{
	// 搜索指定LN0
	LN0_INFO_CACHE *ln0;
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	return FindGSECtrlInLN0(ln0,cbName,it_GSECtrl);
}

///
/// @brief
///     获取Cache中某一IED的某一LDevice的某一LNode的某一LN中的DOI的引用
///
/// @param[in]
///     iedName     - IED的名称
/// @param[in]
///     ldInst      - LDevice的实例号
/// @param[in]
///     lnName      - LNode的名称，LN0的逻辑节点名称固定为“LLN0”
/// @param[in]
///     doiName     - DOI的名称
/// @param[in]
///     it_DOI      - 保存所要获取的DOI的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindDOIInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
								   const SCL_STR &doiName, DOI_MAP::iterator &it_DOI)
{
	if ( lnName == _T("LLN0") )
	{
		// 搜索指定LN0
		LN0_INFO_CACHE *ln0;
		if ( !FindLN0InMap(iedName,ldInst,&ln0) )
			return false;
		return FindDOIInLN0(ln0,doiName,it_DOI);
	}
	else
	{
		// 搜索指定LN
		LN_MAP::iterator it_LN;
		if ( !FindLNInMap(iedName,ldInst,lnName,it_LN) )
			return false;
		return FindDOIInLN(it_LN,doiName,it_DOI);
	}
}

///
/// @brief
///     获取Cache中某一IED的某一LDevice的某一LNode的某一LN中的ExtRef的引用
///
/// @param[in]
///     iedName     - IED的名称
/// @param[in]
///     ldInst      - LDevice的实例号
/// @param[in]
///     lnName      - LNode的名称，LN0的逻辑节点名称固定为“LLN0”
/// @param[in]
///     idxSeq      - ExtRef的顺序号，从0开始
/// @param[in]
///     it_ExtRef   - 保存所要获取的ExtRef的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindExtRefInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const int &idxSeq, EXTREF_MAP::iterator &it_ExtRef)
{
	if ( lnName == _T("LLN0") )
	{
		// 搜索指定LN0
		LN0_INFO_CACHE *ln0;
		if ( !FindLN0InMap(iedName,ldInst,&ln0) )
			return false;
		return FindExtRefInLN0(ln0,idxSeq,it_ExtRef);
	}
	else
	{
		// 搜索指定LN
		LN_MAP::iterator it_LN;
		if ( !FindLNInMap(iedName,ldInst,lnName,it_LN) )
			return false;
		return FindExtRefInLN(it_LN,idxSeq,it_ExtRef);
	}
}

///
/// @brief
///     获取Cache中某一SampledValueControl的通信参数的引用
///
/// @param[in]
///     appID  - 通信参数中的appID项
/// @param[in]
///     it_SMV - 保存所要获取的通信参数Map的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindSMVCommnInMap(const unsigned short appID, SMV_COMMN_MAP::iterator &it_SMV)
{
	if ( m_SCLMap == NULL || m_SCLMap->smvCommnMap == NULL )
		return false;

	// 在SMVMap中搜索指定SMV
	it_SMV = m_SCLMap->smvCommnMap->find(appID);
	if ( it_SMV == m_SCLMap->smvCommnMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取Cache中某一GSEControl的通信参数的引用
///
/// @param[in]
///     appID  - 通信参数中的appID项
/// @param[in]
///     it_GSE- 保存所要获取的通信参数Map的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindGSECommnInMap(const unsigned short appID, GSE_COMMN_MAP::iterator &it_GSE)
{
	if ( m_SCLMap == NULL || m_SCLMap->gseCommnMap == NULL )
		return false;

	// 在IEDMap中搜索指定IED
	it_GSE = m_SCLMap->gseCommnMap->find(appID);
	if ( it_GSE == m_SCLMap->gseCommnMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取Cache中某一LNType的引用
///
/// @param[in]
///     lnTypeID  - LNType的id属性值
/// @param[in]
///     it_LNType - 保存所要获取的LNType的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindLNTypeInMap(const SCL_STR &lnTypeID, LNTYPE_MAP::iterator &it_LNType)
{
	if ( m_SCLMap == NULL || m_SCLMap->lnTypeMap == NULL )
		return false;

	// 在LNTypeMap中搜索指定lnType
	it_LNType = m_SCLMap->lnTypeMap->find(lnTypeID);
	if ( it_LNType == m_SCLMap->lnTypeMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取某一LNType中的DO的引用
///
/// @param[in]
///     it_LNType - LNType的引用
/// @param[in]
///     doName    - DO的名称
/// @param[in]
///     it_DO     - 保存所要获取的DO的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindDOInLNType(const LNTYPE_MAP::iterator &it_LNType, const SCL_STR &doName, DO_MAP::iterator &it_DO)
{
	if ( it_LNType->second == NULL || it_LNType->second->doMap == NULL )
		return false;

	// 在LNType中搜索DO
	it_DO = it_LNType->second->doMap->find(doName);
	if ( it_DO == it_LNType->second->doMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取Cache中某一DO的引用
///
/// @param[in]
///     lnTypeID  - LNType的ID
/// @param[in]
///     doName    - DO的名称
/// @param[in]
///     it_DO     - 保存所要获取的DO的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindDOInMap(const SCL_STR &lnTypeID, const SCL_STR &doName, DO_MAP::iterator &it_DO)
{
	// 搜索指定LNType
	LNTYPE_MAP::iterator it_LNType;
	if ( !FindLNTypeInMap(lnTypeID,it_LNType) )
		return false;

	return FindDOInLNType(it_LNType,doName,it_DO);
}

///
/// @brief
///     获取Cache中某一DOType的引用
///
/// @param[in]
///     doTypeID  - DOType的id属性值
/// @param[in]
///     it_DOType - 保存所要获取的DOType的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindDOTypeInMap(const SCL_STR &doTypeID, DOTYPE_MAP::iterator &it_DOType)
{
	if ( m_SCLMap == NULL || m_SCLMap->doTypeMap == NULL )
		return false;

	// 在DOTypeMap中搜索指定doType
	it_DOType = m_SCLMap->doTypeMap->find(doTypeID);
	if ( it_DOType == m_SCLMap->doTypeMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取某一DOType中的DA的引用
///
/// @param[in]
///     it_DOType - DOType的引用
/// @param[in]
///     daName    - DA的名称
/// @param[in]
///     it_DA     - 保存所要获取的DA的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindDAInDOType(const DOTYPE_MAP::iterator &it_DOType, const SCL_STR &daName, DA_MAP::iterator &it_DA)
{
	if ( it_DOType->second == NULL || it_DOType->second->daMap == NULL )
		return false;

	// 在DOType中搜索DA
	for ( it_DA = it_DOType->second->daMap->begin(); it_DA != it_DOType->second->daMap->end(); it_DA++ )
	{
		if ( it_DA->second.name.compare(daName) == 0 )
			return true;
	}
	return false;
//	it_DA = it_DOType->second->daMap->find(daName);
// 	if ( it_DA == it_DOType->second->daMap->end() )
// 		return false;
// 
// 	return true;
}

///
/// @brief
///     获取Cache中某一DA的引用
///
/// @param[in]
///     doTypeID  - DOType的ID
/// @param[in]
///     daName    - DA的名称
/// @param[in]
///     it_DA     - 保存所要获取的DA的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindDAInMap(const SCL_STR &doTypeID, const SCL_STR &daName, DA_MAP::iterator &it_DA)
{
	// 搜索指定DOType
	DOTYPE_MAP::iterator it_DOType;
	if ( !FindDOTypeInMap(doTypeID,it_DOType) )
		return false;
	return FindDAInDOType(it_DOType,daName,it_DA);
}

///
/// @brief
///     获取Cache中某一DAType的引用
///
/// @param[in]
///     daTypeID  - DAType的id属性值
/// @param[in]
///     it_DAType - 保存所要获取的DAType的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindDATypeInMap(const SCL_STR &daTypeID, DATYPE_MAP::iterator &it_DAType)
{
	if ( m_SCLMap == NULL || m_SCLMap->daTypeMap == NULL )
		return false;

	// 在DATypeMap中搜索指定daType
	it_DAType = m_SCLMap->daTypeMap->find(daTypeID);
	if ( it_DAType == m_SCLMap->daTypeMap->end() )
		return false;

	return true;
}

///
/// @brief
///     获取某一DAType中的BDA的引用
///
/// @param[in]
///     it_DAType - DAType的引用
/// @param[in]
///     bdaName   - BDA的名称
/// @param[in]
///     it_BDA    - 保存所要获取的BDA的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindBDAInDAType(const DATYPE_MAP::iterator &it_DAType, const SCL_STR & bdaName, BDA_MAP::iterator &it_BDA)
{
	if ( it_DAType->second == NULL || it_DAType->second->bdaMap == NULL )
		return false;

	// 在DAType中搜索BDA
	for ( it_BDA = it_DAType->second->bdaMap->begin(); it_BDA != it_DAType->second->bdaMap->end(); it_BDA++ )
	{
		if ( it_BDA->second.name.compare(bdaName) == 0 )
			return true;
	}
	return false;
// 
// 	// 在DAType中搜索BDA
// 	it_BDA = it_DAType->second->bdaMap->find(bdaName);
// 	if ( it_BDA == it_DAType->second->bdaMap->end() )
// 		return false;
// 	return true;
}

///
/// @brief
///     获取Cache中某一BDA的引用
///
/// @param[in]
///     daTypeID  - DAType的ID
/// @param[in]
///     bdaName   - BDA的名称
/// @param[in]
///     it_BDA    - 保存所要获取的BDA的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheWithMap::FindBDAInMap(const SCL_STR &daTypeID, const SCL_STR & bdaName, BDA_MAP::iterator &it_BDA)
{
	// 搜索指定DAType
	DATYPE_MAP::iterator it_DAType;
	if ( !FindDATypeInMap(daTypeID,it_DAType) )
		return false;
	return FindBDAInDAType(it_DAType,bdaName,it_BDA);
}

///
/// @brief
///     清空Cache某一FCDA Map中的数据
///
/// @param[in]
///     fcdaMap - 指向FCDA Map的指针
///
void SCLCacheWithMap::ClearFCDAMap(FCDA_MAP *fcdaMap)
{
	if ( fcdaMap == NULL )
		return ;

	FCDA_MAP::iterator it_FCDA;
	for ( it_FCDA = fcdaMap->begin(); it_FCDA != fcdaMap->end(); it_FCDA++ )
	{
		if ( it_FCDA->second != NULL )
		{
			delete it_FCDA->second;
		}
	}
	fcdaMap->clear();
}

///
/// @brief
///     清空Cache某一DataSet Map中的数据
///
/// @param[in]
///     dataSetMap - 指向DataSet Map的指针
///
void SCLCacheWithMap::ClearDataSetMap(DATASET_MAP *dataSetMap)
{
	if ( dataSetMap == NULL )
		return ;

	DATASET_MAP::iterator it_DataSet;
	for ( it_DataSet = dataSetMap->begin(); it_DataSet != dataSetMap->end(); it_DataSet++ )
	{
		if ( it_DataSet->second != NULL )
		{
			if ( it_DataSet->second->fcdaMap != NULL )
			{
				ClearFCDAMap(it_DataSet->second->fcdaMap);
				delete it_DataSet->second->fcdaMap;
			}
			delete it_DataSet->second;
		}
	}
	dataSetMap->clear();
}

///
/// @brief
///     清空Cache某一LN0中的数据
///
/// @param[in]
///     ln0 - 指向LN0的指针
///
void SCLCacheWithMap::ClearLN0Cache(LN0_INFO_CACHE *ln0)
{
	if ( ln0 == NULL )
		return ;

	if ( ln0->datasetMap != NULL )
	{
		ClearDataSetMap(ln0->datasetMap);
		delete ln0->datasetMap;
	}
	if ( ln0->gseCtrlMap != NULL )
	{
		ln0->gseCtrlMap->clear();
		delete ln0->gseCtrlMap;
	}
	if ( ln0->smvCtrlMap != NULL )
	{
		ln0->smvCtrlMap->clear();
		delete ln0->smvCtrlMap;
	}
	if ( ln0->doiMap != NULL )
	{
		ln0->doiMap->clear();
		delete ln0->doiMap;
	}
	if ( ln0->extRefMap != NULL )
	{
		ln0->extRefMap->clear();
		delete ln0->extRefMap;
	}
}

///
/// @brief
///     清空Cache中某一LN Map中的数据
///
/// @param[in]
///     lnMap - 指向LN Map的指针
///
void SCLCacheWithMap::ClearLNCache(LN_MAP *lnMap)
{
	if ( lnMap == NULL )
		return ;

	LN_MAP::iterator it_LN;
	for ( it_LN = lnMap->begin(); it_LN != lnMap->end(); it_LN++ )
	{
		if ( it_LN->second != NULL )
		{
			if ( it_LN->second->doiMap != NULL )
			{
				it_LN->second->doiMap->clear();
				delete it_LN->second->doiMap;
			}
			if ( it_LN->second->extRefMap != NULL )
			{
				it_LN->second->extRefMap->clear();
				delete it_LN->second->extRefMap;
			}
			delete it_LN->second;
		}
	}
	lnMap->clear();
}

///
/// @brief
///     清空Cache某一LDevice Map中的数据
///
/// @param[in]
///     ldMap - 指向LDevice Map的指针
///
void SCLCacheWithMap::ClearLDeviceMap(LDEVICE_MAP *ldMap)
{
	if ( ldMap == NULL )
		return ;

	LDEVICE_MAP::iterator it_LD;
	for ( it_LD = ldMap->begin(); it_LD != ldMap->end(); it_LD++ )
	{
		if ( it_LD->second != NULL )
		{
			if ( it_LD->second->ln0InfoCache != NULL )
			{
				ClearLN0Cache(it_LD->second->ln0InfoCache);
				delete it_LD->second->ln0InfoCache;
			}
			if ( it_LD->second->lnMap != NULL )
			{
				ClearLNCache(it_LD->second->lnMap);
				delete it_LD->second->lnMap;
			}
			delete it_LD->second;
		}
	}
	ldMap->clear();
}

///
/// @brief
///     清空Cache某一IED Map中的数据
///
/// @param[in]
///     iedMap - 指向IED Map的指针
///
void SCLCacheWithMap::ClearIEDMap(IED_MAP *iedMap)
{
	if ( iedMap == NULL )
		return ;

	IED_MAP::iterator it_IED;
	for ( it_IED = iedMap->begin(); it_IED != iedMap->end(); it_IED++ )
	{
		if ( it_IED->second != NULL)
		{
			if ( it_IED->second->lDeviceMap != NULL )
			{
				ClearLDeviceMap(it_IED->second->lDeviceMap);
				delete it_IED->second->lDeviceMap;
			}
			delete it_IED->second;
		}
	}
	iedMap->clear();
}

///
/// @brief
///     清空Cache某一SMVCommn Map中的数据
///
/// @param[in]
///     smvCommnMap - 指向SMVCommn Map的指针
///
void SCLCacheWithMap::ClearSMVCommnMap(SMV_COMMN_MAP *smvCommnMap)
{
	if ( smvCommnMap == NULL )
		return ;

	SMV_COMMN_MAP::iterator it_SMV;
	for ( it_SMV = smvCommnMap->begin(); it_SMV != smvCommnMap->end(); it_SMV++ )
	{
		SMV_COMMN_INFO_CACHE *pInfo = it_SMV->second;
		SMV_COMMN_INFO_CACHE *pNextInfo = NULL;
		while ( pInfo != NULL )
		{
			pNextInfo = pInfo->pNextCache;
			delete pInfo;
			pInfo     = pNextInfo;
		}
	}
	smvCommnMap->clear();
}

///
/// @brief
///     清空Cache某一GSECommn Map中的数据
///
/// @param[in]
///     gseCommnMap - 指向GSECommn Map的指针
///
void SCLCacheWithMap::ClearGSECommnMap(GSE_COMMN_MAP *gseCommnMap)
{
	if ( gseCommnMap == NULL )
		return ;

	GSE_COMMN_MAP::iterator it_GSE;
	for( it_GSE = gseCommnMap->begin(); it_GSE != gseCommnMap->end(); it_GSE++ )
	{
		GSE_COMMN_INFO_CACHE *pInfo = it_GSE->second;
		GSE_COMMN_INFO_CACHE *pNextInfo = NULL;
		while ( pInfo != NULL )
		{
			pNextInfo = pInfo->pNextCache;
			delete pInfo;
			pInfo     = pNextInfo;
		}
	}
	gseCommnMap->clear();
}

///
/// @brief
///     清空Cache某一LNType Map中的数据
///
/// @param[in]
///     lnTypeMap - 指向LNType Map的指针
///
void SCLCacheWithMap::ClearLNTypeMap(LNTYPE_MAP *lnTypeMap)
{
	if ( lnTypeMap == NULL )
		return ;

	LNTYPE_MAP::iterator it_LNType;
	for ( it_LNType = lnTypeMap->begin(); it_LNType != lnTypeMap->end(); it_LNType++ )
	{
		if ( it_LNType->second != NULL )
		{
			if ( it_LNType->second->doMap != NULL )
			{
				it_LNType->second->doMap->clear();
				delete it_LNType->second->doMap;
			}
			delete it_LNType->second;
		}
	}
	lnTypeMap->clear();
}

///
/// @brief
///     清空Cache某一DOType Map中的数据
///
/// @param[in]
///     doTypeMap - 指向DOType Map的指针
///
void SCLCacheWithMap::ClearDOTypeMap(DOTYPE_MAP *doTypeMap)
{
	if ( doTypeMap == NULL )
		return ;

	DOTYPE_MAP::iterator it_DOType;
	for ( it_DOType = doTypeMap->begin(); it_DOType != doTypeMap->end(); it_DOType++ )
	{
		if ( it_DOType->second != NULL )
		{
			if ( it_DOType->second->daMap != NULL )
			{
				it_DOType->second->daMap->clear();
				delete it_DOType->second->daMap;
			}
			delete it_DOType->second;
		}
	}
	doTypeMap->clear();
}

///
/// @brief
///     清空Cache某一LNType Map中的数据
///
/// @param[in]
///     lnTypeMap - 指向LNType Map的指针
///
void SCLCacheWithMap::ClearDATypeMap(DATYPE_MAP *daTypeMap)
{
	if ( daTypeMap == NULL )
		return ;

	DATYPE_MAP::iterator it_DAType;
	for ( it_DAType = daTypeMap->begin(); it_DAType != daTypeMap->end(); it_DAType++ )
	{
		if ( it_DAType->second != NULL )
		{
			if ( it_DAType->second->bdaMap != NULL )
			{
				it_DAType->second->bdaMap->clear();
				delete it_DAType->second->bdaMap;
			}
			delete it_DAType->second;
		}
	}
	daTypeMap->clear();
}

///
/// @brief
///     清空Cache中SCL Map中的数据
///
/// @param[in]
///     sclMap - 指向SCL Map的指针
///
void SCLCacheWithMap::ClearSCLMap(SCL_MAP *sclMap)
{
	if ( sclMap == NULL )
		return ;

	if ( sclMap->iedMap != NULL )
	{
		ClearIEDMap(sclMap->iedMap);
		delete sclMap->iedMap;
		sclMap->iedMap = NULL;
	}
	if ( sclMap->smvCommnMap != NULL )
	{
		ClearSMVCommnMap(sclMap->smvCommnMap);
		delete sclMap->smvCommnMap;
		sclMap->smvCommnMap = NULL;
	}
	if ( sclMap->gseCommnMap != NULL )
	{
		ClearGSECommnMap(sclMap->gseCommnMap);
		delete sclMap->gseCommnMap;
		sclMap->gseCommnMap = NULL;
	}
	if ( sclMap->lnTypeMap != NULL )
	{
		ClearLNTypeMap(sclMap->lnTypeMap);
		delete sclMap->lnTypeMap;
		sclMap->lnTypeMap = NULL;
	}
	if ( sclMap->doTypeMap != NULL )
	{
		ClearDOTypeMap(sclMap->doTypeMap);
		delete sclMap->doTypeMap;
		sclMap->doTypeMap = NULL;
	}
	if ( sclMap->daTypeMap != NULL )
	{
		ClearDATypeMap(sclMap->daTypeMap);
		delete sclMap->daTypeMap;
		sclMap->daTypeMap = NULL;
	}
}

