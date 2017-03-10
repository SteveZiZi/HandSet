/// @file
///     sclcachemanager.cpp
///
/// @brief
///     SCL文件解析管理器
///
/// @note
///     利用SCLTools解析SCL文件，并将数据缓存到Cache中以供查询
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.1.28
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.01.30     |1.0.1.130  |修改CacheDataSetInLN0函数没有回到原DataSet处陷入死循环的BUG|
///    |2013.02.04     |1.0.2.204  |因SCLCache的实例接口改变，修改SCLCacheManager的构造析构函数|
///    |2013.02.18     |1.0.3.218  |添加采样值和GOOSE的虚端子连线的信息（INPUT）解析接口       |
///    |2013.05.27     |1.0.4.527  |延迟DOI信息的解析，使之只解析与已缓存的FCDA或ExtRef相关的DOI信息|
///    |2013.05.27     |1.0.4.527  |延迟LN信息的解析至DOI信息解析阶段，即某DOI所在的LN未缓存时再添加|
///    |2013.05.27     |1.0.4.527  |设置判断条件，过滤掉与Goose或Smv无关的LDevice信息          |
///    |2013.05.27     |1.0.4.527  |设置判断条件，过滤掉与Goose或Smv无关的DataSet信息          |
///    |2013.05.27     |1.0.4.527  |将是否解析Smv、Goose及与两者相关的Inputs信息设为可配置选项 |
///    |2013.05.27     |1.0.4.527  |增加将SCL解析数据写入新的SCL文件的接口，以实现对现有SCL文件的简化|
///    |2013.05.27     |1.0.4.527  |继承ISCLCacheManager，将部分接口设为外部接口，如LoadSCL等  |
///    |2013.06.04     |1.0.4.604  |过滤GSEControl中的GSSE部分                                 |
///    |2013.06.08     |1.0.4.608  |由于Cache中的接口均可能用到，因此直接返回Cache指针，删除原来的取值接口|
///    |2013.08.07     |1.0.5.807  |加载SCL文件的SMV通道信息时，增加对daName的判断以辨别FCDA是否为9-2LE的有效通道|
///    |2013.08.08     |1.0.5.808  |延迟局部变量的定义，避免变量值更新失败后沿用上一次的结果的BUG|
///    |2013.08.21     |1.0.5.821  |添加对当前可用内存的检查，判断是否足够用来进行SCL文件解析 |
///    |2013.08.21     |1.0.5.821  |添加对new操作的异常处理，防止程序崩溃                     |
///    |2013.08.26     |1.0.5.826  |在释放SCL解析数据缓存时添加内存压缩操作，以使系统回收内存 |
///
#include "stdafx.h"
// CreateFile;TRACE
#include <afx.h>
#include "sclcachemanager.h"

SCLCacheManager::SCLCacheManager()
: m_bSmvCtrlOpt(true)
, m_bGseCtrlOpt(true)
, m_bSmvInputsOpt(false)
, m_bGseInputsOpt(false)
{
	m_SclCache = CreateSCLCache(CACHE_WITH_MAP);
}

SCLCacheManager::~SCLCacheManager()
{
	ReleaseSCLCache(m_SclCache);
}

int newhandler( size_t size )
{
	TRACE(_T("New Handler Exception:"));
	throw std::bad_alloc();
	return 0;
}

///
/// @brief
///     加载SCL配置文件，并进行相应的解析
///
/// @param[in]
///     strFile - SCL配置文件名(带路径)
///
/// @return
///     0 - 操作成功； -1 - 操作失败
///
int SCLCacheManager::LoadSCL(const std::wstring &fileName)
{
	SCLTools scl;
	SCL_IED_ATTRIB iedAttrib;

	int nLoad = -1;
	_PNH oldHandler = _set_new_handler(newhandler);
	try
	{
		// 清空Cache
		m_SclCache->ClearCache();

#ifdef MEMORY_ENOUGH_CHECK
		if ( !IsMemoryEnough(fileName) )
			return -1;
#endif

		// 加载SCL文件
		if ( !scl.LoadSCL(fileName) )
			return -1;

		// 缓存SCL文件中的全部IED信息
		if ( !CacheAllIEDInFile(scl) )
			return -1;

		// 缓存SCL文件中的全部通信参数信息
		if ( !CacheCommnInfo(scl) )
			return -1;

		// 缓存SCL文件中的DataTypeTemplates的全部信息
		if ( !CacheDataTypeTemplates(scl) )
			return -1;
		nLoad = 0;
	}
	catch (...)
	{
		TRACE((_T("Load SCL FAIL.\r\n")));
		// 清空Cache
		m_SclCache->ClearCache();

		nLoad = -1;
	}

	_set_new_handler(oldHandler);

	return nLoad;
}

///
/// @brief
///     卸载（清空）缓存SCL解析数据的Cache
///
void SCLCacheManager::UnloadSCL()
{
	m_SclCache->ClearCache();
}

///
/// @brief
///     以SCL文件格式，保存解析缓存
///
/// @param[in]
///     strFile - 要保存到的SCL文件名(带路径)
///
/// @return
///     0 - 操作成功； -1 - 操作失败
///
int SCLCacheManager::SaveSCL(const std::wstring &fileName)
{
	SCLTools scl;

	// 添加SCL并定位到SCL
	if ( !scl.AddSCL() )
		return -1;
	// 写入通信配置信息到SCL
	if ( !WriteCommnInfoIntoSCL(scl) )
		return -1;
	// 写入IED信息到SCL
	if ( !WriteAllIEDIntoSCL(scl) )
		return -1;
	// 写入DataTypeTemplates到SCL
	if ( !WriteDataTypeTemplatesIntoSCL(scl) )
		return -1;
	// 保存到目标文件
	if ( !scl.SaveSCL(fileName) )
		return -1;

	return 0;
}

///
/// @brief
///     获取管理器所管理的Scl文件缓存的指针
///
/// @return
///     Scl文件缓存的指针
///
SCLCache * SCLCacheManager::GetCache()
{
	return m_SclCache;
}

bool SCLCacheManager::IsMemoryEnough(const std::wstring &fileName)
{
	bool bMemEnough = false;

	HANDLE handle = CreateFile(fileName.c_str(), GENERIC_READ,
		FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	if (handle != INVALID_HANDLE_VALUE)
	{  
		// 获取SCL文件解析所需的内存大小
		DWORD size = GetFileSize(handle, NULL);
		CloseHandle(handle);
		DWORD maxMem = (int)((double)size * 4.5) + 1024*1024*2;
		int nPos  = fileName.find_last_of(_T("."));
		if ( nPos != std::wstring::npos )
		{
			std::wstring fileEx = fileName.substr(nPos+1);
			if ( fileEx.compare(L"scx") == 0 || fileEx.compare(L"SCX") == 0 )
			{
				maxMem = (int)((double)size * 7.0) + 1024*1024*2;
			}
		}

		// 获取内存大小
		MEMORYSTATUS  memStat = {sizeof(MEMORYSTATUS)};
		::GlobalMemoryStatus(&memStat);

		// 判断内存是否足够使用
		if ( memStat.dwAvailPhys > maxMem )
			bMemEnough = true;
	}

	return bMemEnough;
}

///
/// @brief
///     从SCL文件中获取LN0中全部与Goose或SMV控制块相关联的DataSet的信息并缓存,
///     需先进入LN0元素（IntoElem）,且缓存有Goose或SMV控制块信息
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - DataSet所在的IED的名称
/// @param[in]
///     ldInst  - DataSet所在的LDevice的实例号
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheAllDataSetInLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	int i = 0;

	scl.ResetMainPos();
	while ( scl.FindDataSetInLN0() )
	{
		SCL_DATASET_ATTRIB datSetAttrib;
		if ( scl.GetDataSetAttrib(datSetAttrib) )
		{
			bool bAdd = false;
			bool bSmv = false;
			// 判断是否缓存该DataSet信息
			if ( m_bSmvCtrlOpt )
			{
				SCL_SMVCTRL_ATTRIB smvCtrlAttrib;
				if ( m_SclCache->GetSMVCtrlAttribWithDatSet(iedName,ldInst,datSetAttrib.dataSetName,smvCtrlAttrib) )
					bAdd = true;
				bSmv = true;
			}
			if ( !bAdd && m_bGseCtrlOpt )
			{
				SCL_GSECTRL_ATTRIB gseCtrlAttrib;
				if ( m_SclCache->GetGSECtrlAttribWithDatSet(iedName,ldInst,datSetAttrib.dataSetName,gseCtrlAttrib) )
					bAdd = true;
				bSmv = false;
			}

			// 缓存DataSet及其FCDA信息
			if ( bAdd )
			{
				if ( !m_SclCache->AddDataSetIntoLN0(iedName,ldInst,datSetAttrib) )
					return false;
				// 缓存DataSet中的FCDA属性信息
				if ( bSmv )
				{
					static int fcdaNum = 0;
					for ( i=0 ; i < datSetAttrib.fcdaNum ; i++)
					{
						SCL_FCDA_ATTRIB fcdaAttrib;
						if ( !scl.GetOneFCDAAttrib(fcdaAttrib,i) )
							break;
						// 判断该FCDA是否为有效通道（9-2LE）
						if ( fcdaAttrib.daName == _T("") || fcdaAttrib.daName.find(_T("instMag")) != std::wstring::npos )
						{
							fcdaAttrib.idxSeq = fcdaNum;
							fcdaAttrib.daName = _T("");
							if ( !m_SclCache->AddFCDAIntoDatSet(iedName,ldInst,_T("LLN0"),datSetAttrib.dataSetName,fcdaAttrib) )
								return false;
							fcdaNum++;
						}
					}
					datSetAttrib.fcdaNum = fcdaNum;
					m_SclCache->SetDataSetAttribInLN0(iedName,ldInst,datSetAttrib.dataSetName,datSetAttrib);
					fcdaNum = 0;
				}
				else
				{
					for ( i=0 ; i < datSetAttrib.fcdaNum ; i++)
					{
						SCL_FCDA_ATTRIB fcdaAttrib;
						if ( !scl.GetOneFCDAAttrib(fcdaAttrib,i) )
							break;
						if ( !m_SclCache->AddFCDAIntoDatSet(iedName,ldInst,_T("LLN0"),datSetAttrib.dataSetName,fcdaAttrib) )
							return false;
					}
				}
			}
		}
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取LN0中全部SampledValueControl的信息并缓存,需先进入LN0元素（IntoElem）
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - SampledValueControl所在的IED的名称
/// @param[in]
///     ldInst  - SampledValueControl所在的LDevice的实例号
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheAllSMVCtrlInLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// 缓存全部SampledValueControl信息
	scl.ResetMainPos();
	while ( scl.FindSMVCtrlInLN0() )
	{
		SCL_SMVCTRL_ATTRIB smvCtrlAttrib;
		if ( !scl.GetSMVCtrlAttrib(smvCtrlAttrib) )
			continue;
		if ( !m_SclCache->AddSMVCtrlIntoLN0(iedName,ldInst,smvCtrlAttrib) )
			return false;
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取LN0中全部GSEControl的信息并缓存,需先进入LN0元素（IntoElem）
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - GSEControl所在的IED的名称
/// @param[in]
///     ldInst  - GSEControl所在的LDevice的实例号
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheAllGSECtrlInLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// 缓存GSEControl信息
	scl.ResetMainPos();
	while ( scl.FindGSECtrlInLN0() )
	{
		SCL_GSECTRL_ATTRIB gseCtrlAttrib;
		if ( !scl.GetGSECtrlAttrib(gseCtrlAttrib) )
			continue;
		if ( gseCtrlAttrib.type.compare(_T("GSSE")) == 0 )
			continue;
		if ( !m_SclCache->AddGSECtrlIntoLN0(iedName,ldInst,gseCtrlAttrib) )
			return false;
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取LN0/LN中全部DOI的信息并缓存,需先进入LN0或LN元素（IntoElem）
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - DOI所在的IED的名称
/// @param[in]
///     ldInst  - DOI所在的LDevice的实例号
/// @param[in]
///     lnName  - DOI所在的LN的名称，LN0固定为“LLN0”
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheAllDOIIntoLNorLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName)
{
	// 缓存DOI信息
	scl.ResetMainPos();
	while ( scl.FindDOIInLNorLN0() )
	{
		SCL_DOI_ATTRIB doiAttrib;
		if ( !scl.GetDOIAttrib(doiAttrib) )
			continue;
		if ( lnName == _T("LLN0"))
		{
			if ( !m_SclCache->AddDOIIntoLN0(iedName,ldInst,doiAttrib) )
				return false;
		}
		else
		{
			if ( !m_SclCache->AddDOIIntoLN(iedName,ldInst,lnName,doiAttrib) )
				return false;
		}
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取IED中与已缓存的全部FCDA相关联的DOI的信息并缓存,需先进入IED元素（IntoElem）
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - DOI所在的IED的名称
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheAllDOIFromFCDAIntoIED(SCLTools &scl, const SCL_STR &iedName)
{
	// 获取已缓存的LDevice数量
	int ldNum = m_SclCache->GetLDeviceNum(iedName);
	for ( int i = 0; i < ldNum; i++ )
	{
		// 获取LDevice信息
		SCL_ACCESS_POINT_ATTRIB apAttrib;
		SCL_LDEVICE_ATTRIB ldAttrib;
		if ( !m_SclCache->GetLDeviceAttrib(iedName,i,apAttrib,ldAttrib) )
			continue;
		// 获取LN0中的数据集的数目
		int dataSetNum = m_SclCache->GetDataSetNumInLN0(iedName,ldAttrib.ldInst);

		// 遍历LN0中的FCDA，缓存FCDA指向的DOI信息
		for ( int j = 0; j < dataSetNum; j++ )
		{
			SCL_DATASET_ATTRIB dataSetAttrib;
			if ( !m_SclCache->GetDataSetAttribInLN0(iedName,ldAttrib.ldInst,j,dataSetAttrib) )
				continue;

			for ( int k = 0; k < dataSetAttrib.fcdaNum; k++ )
			{
				SCL_FCDA_ATTRIB fcdaAttrib;
				if ( !m_SclCache->GetFCDAAttrib(iedName,ldAttrib.ldInst,_T("LLN0"),dataSetAttrib.dataSetName,k,fcdaAttrib) )
					continue;

				// 根据FCDA，搜索指定的DOI
				scl.ResetMainPos();
				if ( scl.FindLDeviceInIED(fcdaAttrib.ldInst) )
				{
					if ( scl.IntoElem() )
					{
						scl.ResetMainPos();
						if ( scl.FindLNInLDevice(fcdaAttrib.lnPrefix,fcdaAttrib.lnClass,fcdaAttrib.lnInst) )
						{
							SCL_LN_ATTRIB lnAttrib;
							SCL_STR lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
							if ( fcdaAttrib.lnClass != _T("LLN0") && !m_SclCache->GetLNAttrib(iedName,fcdaAttrib.ldInst,lnName,lnAttrib))
							{
								// 缓存LN属性信息
								if ( !scl.GetLNAttrib(lnAttrib) )
									continue;
								if ( !m_SclCache->AddLNIntoLD(iedName,fcdaAttrib.ldInst,lnAttrib) )
									return false;
							}

							if ( scl.IntoElem() )
							{
								scl.ResetMainPos();
								if ( scl.FindDOIInLNorLN0(fcdaAttrib.doName) )
								{
									SCL_DOI_ATTRIB doiAttrib;
									if ( scl.GetDOIAttrib(doiAttrib) )
									{
										// 缓存DOI信息
										if ( fcdaAttrib.lnClass == _T("LLN0"))
										{
											if ( !m_SclCache->AddDOIIntoLN0(iedName,fcdaAttrib.ldInst,doiAttrib) )
												return false;
										}
										else
										{
											if ( !m_SclCache->AddDOIIntoLN(iedName,fcdaAttrib.ldInst,lnName,doiAttrib) )
												return false;
										}
									}
								}
								// 回到LN或者LN0
								scl.OutOfElem();
							}
						}
						// 回到LDevice
						scl.OutOfElem();
					}
					// 回到Server
					scl.OutOfElem();
					// 回到AccessPoint
					scl.OutOfElem();
				}
			}
		}
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取IED中与已缓存的全部Inputs相关联的DOI的信息并缓存,需先进入IED元素（IntoElem）
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - DOI所在的IED的名称
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheAllDOIFromInputsIntoIED(SCLTools &scl, const SCL_STR &iedName)
{
	// 获取已缓存的LDevice数量
	int ldNum = m_SclCache->GetLDeviceNum(iedName);
	for ( int i = 0; i < ldNum; i++ )
	{
		// 获取LDevice信息
		SCL_ACCESS_POINT_ATTRIB apAttrib;
		SCL_LDEVICE_ATTRIB ldAttrib;
		if ( !m_SclCache->GetLDeviceAttrib(iedName,i,apAttrib,ldAttrib) )
			continue;
		// 获取LDevice中已缓存的LN的数目
		int lnNum = m_SclCache->GetLNNum(iedName,ldAttrib.ldInst);

		// 遍历LN0和LN中的Inputs，缓存ExtRef指向的DOI信息
		for ( int j = 0; j < lnNum+1; j++ )
		{
			// 获取LnName
			SCL_STR lnName;
			if ( j < lnNum )
			{
				SCL_LN_ATTRIB      lnAttrib;
				if ( !m_SclCache->GetLNAttrib(iedName,ldAttrib.ldInst,j,lnAttrib) )
					continue;
				lnName = lnAttrib.lnPrefix + lnAttrib.lnClass + lnAttrib.lnInst;
			}
			else
			{
				lnName = _T("LLN0");
			}

			// 根据EXTREF的IntAddr所指向的DOI，搜索并缓存指定的DOI信息
			int extRefNum = m_SclCache->GetExtRefNumInLNorLN0(iedName,ldAttrib.ldInst,lnName);
			for ( int k = 0; k < extRefNum; k++ )
			{
				SCL_EXTREF_ATTRIB  extRefAttrib;
				if ( !m_SclCache->GetExtRefAttrib(iedName,ldAttrib.ldInst,lnName,k,extRefAttrib) )
					continue;
				SCL_STR extLdInst,extLnName,extDoName,extDaName;
				SplitExtRefIntAddr(extRefAttrib.intAddr,extLdInst,extLnName,extDoName,extDaName);
				scl.ResetMainPos();
				if ( scl.FindLDeviceInIED(extLdInst) )
				{
					if ( scl.IntoElem() )
					{
						scl.ResetMainPos();
						if ( scl.FindLNInLDevice(extLnName) )
						{
							SCL_LN_ATTRIB lnAttrib;
							if ( extLnName != _T("LLN0") && !m_SclCache->GetLNAttrib(iedName,extLdInst,extLnName,lnAttrib))
							{
								// 缓存LN属性信息
								if ( !scl.GetLNAttrib(lnAttrib) )
									continue;
								if ( !m_SclCache->AddLNIntoLD(iedName,extLdInst,lnAttrib) )
									return false;
							}

							if ( scl.IntoElem() )
							{
								scl.ResetMainPos();
								if ( scl.FindDOIInLNorLN0(extDoName) )
								{
									SCL_DOI_ATTRIB doiAttrib;
									if ( scl.GetDOIAttrib(doiAttrib) )
									{
										if ( extLnName == _T("LLN0"))
										{
											if ( !m_SclCache->AddDOIIntoLN0(iedName,extLdInst,doiAttrib) )
												return false;
										}
										else
										{
											if ( !m_SclCache->AddDOIIntoLN(iedName,extLdInst,extLnName,doiAttrib) )
												return false;
										}
									}
								}
								// 回到LN或者LN0
								scl.OutOfElem();
							}
						}
						// 回到LDevice
						scl.OutOfElem();
					}
					// 回到Server
					scl.OutOfElem();
					// 回到AccessPoint
					scl.OutOfElem();
				}
			}
		}
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取LN0/LN中Inputs中的Extref信息并缓存,需先进入LN0或LN元素（IntoElem）
///     目前只缓存与SMV或GSE相关Extref信息
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - DOI所在的IED的名称
/// @param[in]
///     ldInst  - DOI所在的LDevice的实例号
/// @param[in]
///     lnName  - DOI所在的LN的名称，LN0固定为“LLN0”
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheInputsIntoLNorIN0(SCLTools &scl, const SCL_STR &iedName,
											 const SCL_STR &ldInst, const SCL_STR &lnName)
{
	int i = 0;

	// 缓存Inputs信息
	scl.ResetMainPos();
	while ( scl.FindInputsInLNorLN0() )
	{
		if ( scl.IntoElem() )
		{
			i = 0;
			scl.ResetMainPos();
			while ( scl.FindExtRefInInputs() )
			{
				SCL_EXTREF_ATTRIB extRefAttrib;
				if ( !scl.GetExtRefAttrib(extRefAttrib) )
					continue;
				// 判断输入端子是否为Goose或SMV虚端子
				if ( ( m_bGseInputsOpt && extRefAttrib.intAddr.find(_T("GOIN")) != std::wstring::npos ) || 
					( m_bSmvInputsOpt && extRefAttrib.intAddr.find(_T("SVIN")) != std::wstring::npos ) )
				{
					extRefAttrib.idxSeq = i;
					if ( lnName == _T("LLN0"))
					{
						if ( !m_SclCache->AddExtRefIntoLN0(iedName,ldInst,extRefAttrib) )
							return false;
					}
					else
					{
						if ( !m_SclCache->AddExtRefIntoLN(iedName,ldInst,lnName,extRefAttrib) )
							return false;
					}
				}
				i++;
			}
			scl.OutOfElem();
		}
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取LDevice中LN0的信息并缓存,需先进入LDevice元素（IntoElem）
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - LN0所在的IED的名称
/// @param[in]
///     ldInst  - LN0所在的LDevice的名称
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheLN0InLDevice(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// 缓存LN0属性信息
	scl.ResetMainPos();
	if ( scl.FindLN0InLDevice() )
	{
		SCL_LN0_ATTRIB ln0Attrib;
		if ( scl.GetLN0Attrib(ln0Attrib) )
		{
			if ( !m_SclCache->AddLN0IntoLD(iedName,ldInst,ln0Attrib) )
				return false;

			// 进入LN0内部
			if ( scl.IntoElem() )
			{
				// 缓存LN0中的SMV控制块信息
				if ( m_bSmvCtrlOpt )
				{
					if ( !CacheAllSMVCtrlInLN0(scl,iedName,ldInst) )
						return false;
				}
				// 缓存LN0中的GSE控制块信息
				if ( m_bGseCtrlOpt )
				{
					if ( !CacheAllGSECtrlInLN0(scl,iedName,ldInst) )
						return false;
				}
				// 缓存LN0中与SMV或GSE控制块相关联的全部DataSet信息
				if ( !CacheAllDataSetInLN0(scl,iedName,ldInst) )
					return false;
// 				// 缓存LN0中的DOI信息
// 				if ( !CacheAllDOIIntoLNorLN0(scl,iedName,ldInst,_T("LLN0")) )
// 					return false;
				// 缓存LN0中的SMV或GSE的Inputs信息
				// 《IEC61850应用入门》：一般在每个装置的LLN0的Inputs部分定义该装置的Goose和
				// 采样值输入连线信息，且输入虚端子的逻辑节点的前缀一般含有“GOIN”和“SVIN”
				if ( m_bGseInputsOpt || m_bSmvInputsOpt )
				{
					if ( !CacheInputsIntoLNorIN0(scl,iedName,ldInst,_T("LLN0")) )
						return false;
				}

				// 回到LN0
				scl.OutOfElem();
			}
		}
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取LDevice中LN的信息并缓存,需先进入LDevice元素（IntoElem）
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - LN所在的IED的名称
/// @param[in]
///     ldInst  - LN所在的LDevice的名称
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheLNInLDevice(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	scl.ResetMainPos();
	while ( scl.FindLNInLDevice() )
	{
		// 缓存LN属性信息
		SCL_LN_ATTRIB lnAttrib;
		if ( !scl.GetLNAttrib(lnAttrib) )
			continue;
		if ( !m_SclCache->AddLNIntoLD(iedName,ldInst,lnAttrib) )
			return false;
// 		// 进入LN内部
// 		if ( scl.IntoElem() )
// 		{
// 			SCL_STR lnName = lnAttrib.lnPrefix+lnAttrib.lnClass+lnAttrib.lnInst;
// // 			// 缓存LN中的DOI信息
// // 			if ( !CacheAllDOIIntoLNorLN0(scl,iedName,ldInst,lnName) )
// // 				return false;
// 			// 缓存LN中的SMV或GSE的Inputs信息
// 			if ( m_bGseInputsOpt || m_bSmvInputsOpt )
// 			{
// 				if ( !CacheInputsIntoLNorIN0(scl,iedName,ldInst,lnName) )
// 					return false;
// 			}
// 			// 回到LN
// 			scl.OutOfElem();
// 		}
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取AccessPoint中全部LDevice的信息并缓存,需先进入AccessPoint元素（IntoElem）
///     目前只缓存LDevice->LN0
///
/// @param[in]
///     scl      - SCLTools实例的引用
/// @param[in]
///     iedName  - LDeivce所在的IED的名称
/// @param[in]
///     apAttrib - AccessPoint的属性信息
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheAllLDeviceInAP(SCLTools &scl, const SCL_STR &iedName,
										  const SCL_ACCESS_POINT_ATTRIB &apAttrib)
{
	// 进入AccessPoint内部
	if ( scl.IntoElem() )
	{
		scl.ResetMainPos();
		if ( scl.FindServerInAccessPoint() )
		{
			if ( scl.IntoElem() )
			{
				scl.ResetMainPos();
				while( scl.FindLDeviceInServer() )
				{
					// 缓存LDevice的属性信息
					SCL_LDEVICE_ATTRIB ldAttrib;
					if ( !scl.GetLDeviceAttrib(ldAttrib) )
						continue;

					// 判断LDevice中的LN0是否存在GSE、SMV控制块或者相关的Inputs信息
					if ( !IsHaveGseOrSmvOrInputsInLN0(scl,iedName,ldAttrib.ldInst) )
						continue;

					if ( !m_SclCache->AddLDeviceIntoIED(iedName,apAttrib,ldAttrib) )
						return false;

					if ( scl.IntoElem() )
					{
						// 缓存LDevice的LN0信息
						if ( !CacheLN0InLDevice(scl,iedName,ldAttrib.ldInst) )
							return false;
// 						// 缓存LDevice的LN信息
// 						if ( !CacheLNInLDevice(scl,iedName,ldAttrib.ldInst) )
// 							return false;
						// 回到LDevice
						scl.OutOfElem();
					}
				}
				// 回到Server
				scl.OutOfElem();
			}
		}
		// 回到AccessPoint
		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取IED中全部AccessPoint的信息并缓存,需先进入IED元素（IntoElem）
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - AccessPoint所在的IED的名称
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheAllAccessPointInIED(SCLTools &scl, const SCL_STR &iedName)
{
	scl.ResetMainPos();
	while( scl.FindAccessPointInIED() )
	{
		SCL_ACCESS_POINT_ATTRIB apAttrib;
		if ( !scl.GetAccessPointAttrib(apAttrib) )
			continue;

		// 缓存AccessPoint中的全部LDevice信息
		if ( !CacheAllLDeviceInAP(scl,iedName,apAttrib) )
			return false;
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取全部IED中的信息并缓存
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheAllIEDInFile(SCLTools &scl)
{
	// 进入根元素SCL
	scl.ResetMainPos();
	if ( !scl.FindSCL() )
		return false;
	if ( !scl.IntoElem() )
		return false;

	scl.ResetMainPos();
	while ( scl.FindIEDInSCL() )
	{
		// 缓存IED属性信息
		SCL_IED_ATTRIB iedAttrib;
		if ( !scl.GetIEDAttrib(iedAttrib) )
			continue;
		if ( !m_SclCache->AddIED(iedAttrib) )
			return false;

		if ( scl.IntoElem() )
		{
			// 缓存IED中的全部AccessPoint信息
			if ( !CacheAllAccessPointInIED(scl,iedAttrib.iedName) )
				return false;
			// 缓存与已缓存的FCDA相关联的DOI的信息
			if ( !CacheAllDOIFromFCDAIntoIED(scl,iedAttrib.iedName))
				return false;
			// 缓存与已缓存的Inputs相关联的DOI的信息
			if ( m_bGseInputsOpt || m_bSmvInputsOpt )
			{
				if ( !CacheAllDOIFromInputsIntoIED(scl,iedAttrib.iedName))
					return false;
			}

			// 回到IED
			scl.OutOfElem();
		}
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取SubNetwork的关于SMV的全部通信参数信息并缓存,需先进入SubNetwork元素（IntoElem）
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheSMVCommnInSubNetwork(SCLTools &scl)
{
	// 遍历并缓存SubNetwork中的ConnectedAP的SMV
	scl.ResetMainPos();
	while ( scl.FindConnectedAPInSubNetwork() )
	{
		SCL_CONNECTEDAP_ATTRIB cnntAPAttrib;
		if ( !scl.GetConnectedAPAttrib(cnntAPAttrib) )
			continue;
		if ( scl.IntoElem() )
		{
			scl.ResetMainPos();
			while ( scl.FindSMVInConnectedAP() )
			{
				SCL_SMV_ATTRIB smvAttrib;
				if ( scl.GetSMVAttrib(smvAttrib) )
				{
					// 通信参数不完整，则添加默认值
					if ( smvAttrib.appID.empty() )
					{
						smvAttrib.appID = _T("FFFF");
						smvAttrib.macAddr = _T("01-0C-CD-04-FF-FF");
					}
					if ( !m_SclCache->AddSMVCommn(cnntAPAttrib,smvAttrib) )
						return false;
				}
			}
			// 回到ConnectedAP
			scl.OutOfElem();
		}
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取SubNetwork的关于GOOSE/GSSE的全部通信参数信息并缓存,需先进入SubNetwork元素（IntoElem）
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheGSECommnInSubNetwork(SCLTools &scl)
{
	// 遍历并缓存SubNetwork中的ConnectedAP的GSE
	scl.ResetMainPos();
	while ( scl.FindConnectedAPInSubNetwork() )
	{
		SCL_CONNECTEDAP_ATTRIB cnntAPAttrib;
		if ( !scl.GetConnectedAPAttrib(cnntAPAttrib) )
			continue;
		if ( scl.IntoElem() )
		{
			scl.ResetMainPos();
			while ( scl.FindGSEInConnectedAP() )
			{
				SCL_GSE_ATTRIB gseAttrib;
				if ( scl.GetGSEAttrib(gseAttrib) )
				{
					// 通信参数不完整，则添加默认值
					if ( gseAttrib.appID.empty() )
					{
						gseAttrib.appID = _T("FFFF");
						gseAttrib.macAddr = _T("01-0C-CD-01-FF-FF");
					}
					if ( !m_SclCache->AddGSECommn(cnntAPAttrib,gseAttrib) )
						return false;
				}
			}
			// 回到ConnectedAP
			scl.OutOfElem();
		}
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取全部通信参数信息并缓存
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheCommnInfo(SCLTools &scl)
{
	// 进入根元素SCL
	if ( !scl.FindSCL() )
		return false;
	if ( !scl.IntoElem() )
		return false;

	if ( !scl.FindCommnInSCL() )
		return false;
	if ( !scl.IntoElem() )
		return false;
	scl.ResetMainPos();
	while ( scl.FindSubNetworkInCommn() )
	{
		if ( scl.IntoElem() )
		{
			// 缓存SMV通信信息
			if ( m_bSmvCtrlOpt )
			{
				if ( !CacheSMVCommnInSubNetwork(scl) )
					return false;
			}
			// 缓存GOOSE/GSSE通信信息
			if ( m_bGseCtrlOpt )
			{
				if ( !CacheGSECommnInSubNetwork(scl) )
					return false;

			}		
			scl.OutOfElem();
		}
	}
	return true;
}

///
/// @brief
///     从SCL文件中获取LNodeType中全部DO的信息并缓存,需先进入LNodeType元素（IntoElem）
///
/// @param[in]
///     scl    - SCLTools实例的引用
/// @param[in]
///     lnType - LNodeType的ID
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheDOInLNType(SCLTools &scl,const SCL_STR &lnType)
{
	scl.ResetMainPos();
	while ( scl.FindDOInLNType() )
	{
		SCL_DOINLNTYPE_ATTRIB doAttrib;
		if ( !scl.GetDOAttrib(doAttrib) )
			continue;
		if ( !m_SclCache->AddDOIntoLNType(lnType,doAttrib) )
			return false;
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取全部LNodeType信息并缓存
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheLNType(SCLTools &scl)
{
	scl.ResetMainPos();
	while ( scl.FindLNTypeInDataTypeTmplt() )
	{
		SCL_LNTYPE_ATTRIB lnTypeAttrib;
		if ( !scl.GetLNTypeAttrib(lnTypeAttrib) )
			continue;
		if ( !m_SclCache->AddLNType(lnTypeAttrib) )
			return false;
		if ( scl.IntoElem() )
		{
			if ( !CacheDOInLNType(scl,lnTypeAttrib.id) )
				return false;
			scl.OutOfElem();
		}
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取DOType中全部DA的信息并缓存,需先进入DOType元素（IntoElem）
///
/// @param[in]
///     scl    - SCLTools实例的引用
/// @param[in]
///     doType - DOType的ID
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheDAInDOType(SCLTools &scl,const SCL_STR &doType)
{
	scl.ResetMainPos();

	while ( scl.FindDAInDOType() )
	{
		SCL_DAINDOTYPE_ATTRIB daAttrib;
		if ( !scl.GetDAAttrib(daAttrib) )
			continue;
		if ( !m_SclCache->AddDAIntoDOType(doType,daAttrib) )
			return false;
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取全部DOType信息并缓存
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheDOType(SCLTools &scl)
{
	scl.ResetMainPos();

	while ( scl.FindDOTypeInDataTypeTmplt() )
	{
		SCL_DOTYPE_ATTRIB doTypeAttrib;
		if ( !scl.GetDOTypeAttrib(doTypeAttrib) )
			continue;
		if ( !m_SclCache->AddDOType(doTypeAttrib) )
			return false;
		if ( scl.IntoElem() )
		{
			if ( !CacheDAInDOType(scl,doTypeAttrib.id) )
				return false;
			scl.OutOfElem();
		}
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取DAType中全部BDA的信息并缓存,需先进入DAType元素（IntoElem）
///
/// @param[in]
///     scl    - SCLTools实例的引用
/// @param[in]
///     daType - DAType的ID
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheBDAInDAType(SCLTools &scl,const SCL_STR &daType)
{
	scl.ResetMainPos();

	while ( scl.FindBDAInDAType() )
	{
		SCL_BDAINDATYPE_ATTRIB bdaAttrib;
		if ( !scl.GetBDAAttrib(bdaAttrib) )
			continue;
		if ( !m_SclCache->AddBDAIntoDAType(daType,bdaAttrib) )
			return false;
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取全部DAType信息并缓存
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheDAType(SCLTools &scl)
{
	scl.ResetMainPos();

	while ( scl.FindDATypeInDataTypeTmplt() )
	{
		SCL_DATYPE_ATTRIB daTypeAttrib;
		if ( !scl.GetDATypeAttrib(daTypeAttrib) )
			continue;
		if ( !m_SclCache->AddDAType(daTypeAttrib) )
			return false;
		if ( scl.IntoElem() )
		{
			if ( !CacheBDAInDAType(scl,daTypeAttrib.id) )
				return false;
			scl.OutOfElem();
		}
	}

	return true;
}

///
/// @brief
///     从SCL文件中获取DataTypeTemplates的全部信息并缓存
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::CacheDataTypeTemplates(SCLTools &scl)
{
	// 进入根元素SCL
	if ( !scl.FindSCL() )
		return false;
	if ( !scl.IntoElem() )
		return false;

	// 进入DataTypeTemplates元素
	if ( !scl.FindDataTypeTmpltInSCL() )
		return false;
	if ( !scl.IntoElem() )
		return false;

	// 缓存SCL文件中的全部LNodeType信息
	if ( !CacheLNType(scl) )
		return false;

	// 缓存SCL文件中的全部DOType信息
	if ( !CacheDOType(scl) )
		return false;

	// 缓存SCL文件中的全部DAType信息
	if ( !CacheDAType(scl) )
		return false;

	return true;
}

///
/// @brief
///     将缓存中的特定LDevice的全部DataSet的信息写入LN0中,需先定位到LN0元素
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - DataSet所在的IED的名称
/// @param[in]
///     ldInst  - DataSet所在的LDevice的实例号
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteAllDataSetIntoLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	int dataSetNum = m_SclCache->GetDataSetNumInLN0(iedName,ldInst);

	for ( int i = 0; i < dataSetNum ; i++ )
	{
		SCL_DATASET_ATTRIB datSetAttrib;
		if ( !m_SclCache->GetDataSetAttribInLN0(iedName,ldInst,i,datSetAttrib) )
			continue;
		// 添加DataSet到LN0并定位到DataSet
		if ( !scl.AddDataSetIntoLN0(datSetAttrib) )
			continue;
		// 根据数据集属性中的数量添加FCDA
		for ( int j = 0; j < datSetAttrib.fcdaNum; j++ )
		{
			SCL_FCDA_ATTRIB fcdaAttrib;
			if ( !m_SclCache->GetFCDAAttrib(iedName,ldInst,_T("LLN0"),datSetAttrib.dataSetName,j,fcdaAttrib) )
				continue;
			// 添加FCDA到DataSet并定位到FCDA
			if ( !scl.AddFCDAIntoDataSet(fcdaAttrib) )
				return false;
			// 回到DataSet
			scl.OutOfElem();
		}
		// 回到LN0
		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///     将缓存中的特定LDevice的全部SampledValueControl的信息写入LN0中,需先定位到LN0元素
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - SampledValueControl所在的IED的名称
/// @param[in]
///     ldInst  - SampledValueControl所在的LDevice的实例号
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteAllSMVCtrlIntoLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// 写入SampledValueControl信息
	int num = m_SclCache->GetSMVCtrlNumInLN0(iedName,ldInst);

	for ( int i = 0; i < num ; i++ )
	{
		SCL_SMVCTRL_ATTRIB smvCtrlAttrib;
		if ( !m_SclCache->GetSMVCtrlAttribInLN0(iedName,ldInst,i,smvCtrlAttrib) )
			continue;
		if ( !scl.AddSMVCtrlIntoLN0(smvCtrlAttrib) )
			return false;
		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///     将缓存中的特定LDevice的全部GSEControl的信息写入LN0中,需先定位到LN0元素
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - GSEControl所在的IED的名称
/// @param[in]
///     ldInst  - GSEControl所在的LDevice的实例号
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteAllGSECtrlIntoLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// 写入GSEControl信息
	int num = m_SclCache->GetGSECtrlNumInLN0(iedName,ldInst);

	for ( int i = 0; i < num ; i++ )
	{
		SCL_GSECTRL_ATTRIB gseCtrlAttrib;
		if ( !m_SclCache->GetGSECtrlAttribInLN0(iedName,ldInst,i,gseCtrlAttrib) )
			continue;
		if ( !scl.AddGSECtrlIntoLN0(gseCtrlAttrib) )
			return false;
		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///      将缓存中的特定LNode的全部DOI的信息写入LNode中,需先定位到LN0或LN元素
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - DOI所在的IED的名称
/// @param[in]
///     ldInst  - DOI所在的LDevice的实例号
/// @param[in]
///     lnName  - DOI所在的LN的名称，LN0固定为“LLN0”
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteAllDOIIntoLNorLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName)
{
	// 写入DOI信息
	int num = m_SclCache->GetDOINumInLNorLN0(iedName,ldInst,lnName);

	for ( int i = 0; i < num ; i++ )
	{
		SCL_DOI_ATTRIB doiAttrib;
		if ( !m_SclCache->GetDOIAttribInLNorLN0(iedName,ldInst,lnName,i,doiAttrib) )
			continue;
		if ( !scl.AddDOIIntoLNorLN0(doiAttrib) )
			return false;
		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///     将缓存中的特定LNode的Inputs的全部Extref信息写入LNode中,需先定位到LN0或LN元素
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - DOI所在的IED的名称
/// @param[in]
///     ldInst  - DOI所在的LDevice的实例号
/// @param[in]
///     lnName  - DOI所在的LN的名称，LN0固定为“LLN0”
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteInputsIntoLNorIN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst,
											 const SCL_STR &lnName)
{
	int i = 0;

	// 写入ExtRef信息
	int num = m_SclCache->GetExtRefNumInLNorLN0(iedName,ldInst,lnName);
	if ( num > 0 )
	{
		// 写入Inputs
		if ( !scl.AddInputsIntoLNorLN0() )
			return false;

		for ( int i = 0; i < num ; i++ )
		{
			SCL_EXTREF_ATTRIB extRefAttrib;
			if ( !m_SclCache->GetExtRefAttrib(iedName,ldInst,lnName,i,extRefAttrib) )
				continue;
			if ( !scl.AddExtRefIntoInputs(extRefAttrib) )
				return false;
			scl.OutOfElem();
		}

		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///     将缓存中的特定LDevice中的LN0的信息写入LDeivce中,需先定位到LDevice元素
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - LN0所在的IED的名称
/// @param[in]
///     ldInst  - LN0所在的LDevice的名称
///
/// @return
///     true：成功；  false：失败;
///
bool SCLCacheManager::WriteLN0IntoLDevice(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	SCL_LN0_ATTRIB ln0Attrib;

	if ( !m_SclCache->GetLN0Attrib(iedName,ldInst,ln0Attrib) )
		return false;
	// 添加LN0到LDevice并定位到LN0
	if ( !scl.AddLN0IntoLDevice(ln0Attrib) )
		return false;

	// 写入DataSet信息到LN0
	if ( !WriteAllDataSetIntoLN0(scl,iedName,ldInst) )
		return false;
	// 写入DOI信息到LN0
	if ( !WriteAllDOIIntoLNorLN0(scl,iedName,ldInst,_T("LLN0")) )
		return false;
	// 写入SMV控制块信息到LN0
	if ( !WriteAllSMVCtrlIntoLN0(scl,iedName,ldInst) )
		return false;
	// 写入GSE控制块信息到LN0
	if ( !WriteAllGSECtrlIntoLN0(scl,iedName,ldInst) )
		return false;
	// 写入Inputs信息到LN0
	if ( !WriteInputsIntoLNorIN0(scl,iedName,ldInst,_T("LLN0")) )
		return false;

	// 回到LDevice
	scl.OutOfElem();

	return true;
}

///
/// @brief
///     将缓存中的特定LDevice中的LN的信息写入LDeivce中,需先定位到LDevice元素
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - LN所在的IED的名称
/// @param[in]
///     ldInst  - LN所在的LDevice的名称
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteLNIntoLDevice(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	int num = m_SclCache->GetLNNum(iedName,ldInst);
	for ( int i = 0; i < num; i++ )
	{
		SCL_LN_ATTRIB lnAttrib;
		if ( !m_SclCache->GetLNAttrib(iedName,ldInst,i,lnAttrib) )
			continue;
		// 添加LN到LDevice并定位到LN
		if ( !scl.AddLNIntoLDevice(lnAttrib) )
			return false;

		SCL_STR lnName = lnAttrib.lnPrefix + lnAttrib.lnClass + lnAttrib.lnInst;
		// 写入DOI信息到LN
		if ( !WriteAllDOIIntoLNorLN0(scl,iedName,ldInst,lnName) )
			return false;
		// 写入Inputs信息到LN
		if ( !WriteInputsIntoLNorIN0(scl,iedName,ldInst,lnName) )
			return false;

		// 回到LDevice
		scl.OutOfElem();		
	}

	return true;
}

///
/// @brief
///     将缓存中的特定IED的全部LDevice的信息写入IED中,需先定位到IED元素
///
/// @param[in]
///     scl      - SCLTools实例的引用
/// @param[in]
///     iedName  - LDeivce所在的IED的名称
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteAllLDeviceIntoIED(SCLTools &scl, const SCL_STR &iedName)
{
	// 获取LDevice数量
	int num = m_SclCache->GetLDeviceNum(iedName);

	for ( int i = 0; i < num; i++ )
	{
		// 获取LDevice信息
		SCL_ACCESS_POINT_ATTRIB apAttrib;
		SCL_LDEVICE_ATTRIB ldAttrib;
		if ( !m_SclCache->GetLDeviceAttrib(iedName,i,apAttrib,ldAttrib) )
			continue;

		if ( !scl.IntoElem() )
			return false;
		scl.ResetMainPos();
		if ( !scl.FindAccessPointInIED(apAttrib.apName) )
		{
			// AccessPoint不存在，则创建
			scl.OutOfElem();
			if ( !scl.AddAccessPointIntoIED(apAttrib) )
				return false;
			if ( !scl.AddServerIntoAccessPoint() )
				return false;
		}
		else
		{
			// AccessPoint存在，则进入
			if ( !scl.IntoElem() )
				return false;
			if ( !scl.FindServerInAccessPoint() )
				return false;
		}
		// 添加LDevice并定位到LDevice
		if ( !scl.AddLDeviceIntoServer(ldAttrib) )
			return false;
		// 写入LN0信息到LDevice
		if ( !WriteLN0IntoLDevice(scl,iedName,ldAttrib.ldInst) )
			return false;
		// 写入LN信息到LDevice
		if ( !WriteLNIntoLDevice(scl,iedName,ldAttrib.ldInst) )
			return false;
		// 回到Server
		scl.OutOfElem();
		// 回到AccessPoint
		scl.OutOfElem();
		// 回到IED
		scl.OutOfElem();
	}
	return true;
}

///
/// @brief
///      将缓存中的全部IED中的信息写入SCL元素中,需先定位到SCL元素
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteAllIEDIntoSCL(SCLTools &scl)
{
	int num = m_SclCache->GetIEDNum();
	for ( int i = 0; i < num; i++ )
	{
		// 获取IED信息
		SCL_IED_ATTRIB iedAttrib;
		if ( !m_SclCache->GetIEDAttrib(i,iedAttrib) )
			continue;
		// 添加IED并定位到IED
		if ( !scl.AddIEDIntoSCL(iedAttrib) )
			return false;
		// 写入全部LDevice到IED
		if ( !WriteAllLDeviceIntoIED(scl,iedAttrib.iedName) )
			return false;
		// 回到SCL
		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///     将缓存中的关于SMV的全部通信参数信息写入SubNetwork元素中,需先定位到SubNetwork元素
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteSMVCommnIntoSubNetwork(SCLTools &scl)
{
	int num = m_SclCache->GetSMVCommnNumInCache();
	if ( num <= 0 )
		return true;

	SCL_CONNECTEDAP_ATTRIB *cnntAPAttrib = new SCL_CONNECTEDAP_ATTRIB[num];
	SCL_SMV_ATTRIB *smvAttrib = new SCL_SMV_ATTRIB[num];
	if ( !m_SclCache->GetAllSMVCommn(cnntAPAttrib,smvAttrib,num) )
		goto errorHandle;

	for ( int i = 0; i < num; i++ )
	{
		if ( !scl.IntoElem() )
			goto errorHandle;
		scl.ResetMainPos();
		if ( !scl.FindConnectedAPInSubNetwork(cnntAPAttrib[i].apName,cnntAPAttrib[i].iedName) )
		{
			// ConnectedAP不存在，则创建
			scl.OutOfElem();
			if ( !scl.AddConnectedAPIntoSubNetwork(cnntAPAttrib[i]) )
				goto errorHandle;
		}
		// 添加SMV到ConnectedAP并定位到SMV
		if ( !scl.AddSMVIntoConnectedAP(smvAttrib[i]) )
			goto errorHandle;
		// 回到ConnectedAP
		scl.OutOfElem();
		// 回到SubNetwork
		scl.OutOfElem();
	}

	delete[] cnntAPAttrib;
	delete[] smvAttrib;
	return true;

errorHandle:
	delete[] cnntAPAttrib;
	delete[] smvAttrib;
	return false;
}

///
/// @brief
///     将缓存中的关于GOOSE/GSSE的全部通信参数信息写入SubNetwork元素中,需先定位到SubNetwork元素
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteGSECommnIntoSubNetwork(SCLTools &scl)
{
	int num = m_SclCache->GetGSECommnNumInCache();
	if ( num <= 0 )
		return true;

	SCL_CONNECTEDAP_ATTRIB *cnntAPAttrib = new SCL_CONNECTEDAP_ATTRIB[num];
	SCL_GSE_ATTRIB *gseAttrib = new SCL_GSE_ATTRIB[num];
	if ( !m_SclCache->GetAllGSECommn(cnntAPAttrib,gseAttrib,num) )
		goto errorHandle;

	for ( int i = 0; i < num; i++ )
	{
		if ( !scl.IntoElem() )
			goto errorHandle;
		scl.ResetMainPos();
		if ( !scl.FindConnectedAPInSubNetwork(cnntAPAttrib[i].apName,cnntAPAttrib[i].iedName) )
		{
			// ConnectedAP不存在，则创建
			scl.OutOfElem();
			if ( !scl.AddConnectedAPIntoSubNetwork(cnntAPAttrib[i]) )
				goto errorHandle;
		}
		// 添加GSE到ConnectedAP并定位到GSE
		if ( !scl.AddGSEIntoConnectedAP(gseAttrib[i]) )
			goto errorHandle;
		// 回到ConnectedAP
		scl.OutOfElem();
		// 回到SubNetwork
		scl.OutOfElem();
	}

	delete[] cnntAPAttrib;
	delete[] gseAttrib;
	return true;

errorHandle:
	delete[] cnntAPAttrib;
	delete[] gseAttrib;
	return false;
}

///
/// @brief
///     将缓存中的全部通信参数信息写入SCL元素中,需先定位到SCL元素
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteCommnInfoIntoSCL(SCLTools &scl)
{
	// 添加Communication并定位到Communication
	if ( !scl.AddCommnIntoSCL() )
		return false;
	// 添加SubNetwork并定位到SubNetwork
	SCL_SUBNETWORK_ATTRIB subNetworkAttrib = {_T("subnetwork")};
	if ( !scl.AddSubNetworkIntoCommn(subNetworkAttrib) )
		return false;
	// 写入SMV通信配置到SubNetwork
	if ( !WriteSMVCommnIntoSubNetwork(scl) )
		return false;
	// 写入GSE通信配置到SubNetwork
	if ( !WriteGSECommnIntoSubNetwork(scl) )
		return false;
	// 回到Communication
	scl.OutOfElem();
	// 回到SCL
	scl.OutOfElem();
	return true;
}

///
/// @brief
///     将缓存中的特定LNodeType中全部DO的信息写入LNodeType中,需先定位到LNodeType元素
///
/// @param[in]
///     scl    - SCLTools实例的引用
/// @param[in]
///     lnType - LNodeType的ID
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteDOIntoLNType(SCLTools &scl,const SCL_STR &lnType)
{
	int num = m_SclCache->GetDONumInLNType(lnType);

	for ( int i = 0; i < num; i++ )
	{
		SCL_DOINLNTYPE_ATTRIB doAttrib;
		if ( !m_SclCache->GetDOAttrib(lnType,i,doAttrib) )
			continue;
		// 添加DO信息并定位到DO
		if ( !scl.AddDOIntoLNType(doAttrib) )
			return false;
		// 回到LNType
		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///     将缓存中的全部LNodeType信息写入DataTypeTemplates中,需先定位到DataTypeTemplates元素
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteLNTypeIntoDataTypeTemplates(SCLTools &scl)
{
	int num = m_SclCache->GetLNTypeNum();
	for ( int i = 0; i < num; i++ )
	{
		SCL_LNTYPE_ATTRIB lnTypeAttrib;
		if ( !m_SclCache->GetLNType(i,lnTypeAttrib) )
			continue;
		// 添加LNType并定位到LNType
		if ( !scl.AddLNTypeIntoDataTypeTmplt(lnTypeAttrib) )
			return false;
		// 写入DO信息到LNType
		if ( !WriteDOIntoLNType(scl,lnTypeAttrib.id) )
			return false;
		// 回到DataTypeTemplates
		scl.OutOfElem();
	}
	return true;
}

///
/// @brief
///     将缓存中的特定DOType中全部DA的信息写入DOType中,需先定位到DOType元素
///
/// @param[in]
///     scl    - SCLTools实例的引用
/// @param[in]
///     doType - DOType的ID
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteDAIntoDOType(SCLTools &scl,const SCL_STR &doType)
{
	int num = m_SclCache->GetDANumInDOType(doType);

	for ( int i = 0; i < num; i++ )
	{
		SCL_DAINDOTYPE_ATTRIB daAttrib;
		if ( !m_SclCache->GetDAAttrib(doType,i,daAttrib) )
			continue;
		// 添加DA信息并定位到DA
		if ( !scl.AddDAIntoDOType(daAttrib) )
			return false;
		// 回到DOType
		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///     将缓存中的全部DOType信息写入DataTypeTemplates中,需先定位到DataTypeTemplates元素
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteDOTypeIntoDataTypeTemplates(SCLTools &scl)
{
	int num = m_SclCache->GetDOTypeNum();
	for ( int i = 0; i < num; i++ )
	{
		SCL_DOTYPE_ATTRIB doTypeAttrib;
		if ( !m_SclCache->GetDOType(i,doTypeAttrib) )
			continue;
		// 添加DOType并定位到DOType
		if ( !scl.AddDOTypeIntoDataTypeTmplt(doTypeAttrib) )
			return false;
		// 写入DA信息到DOType
		if ( !WriteDAIntoDOType(scl,doTypeAttrib.id) )
			return false;
		// 回到DataTypeTemplates
		scl.OutOfElem();
	}
	return true;
}

///
/// @brief
///     将缓存中的特定DAType中全部BDA的信息写入DAType中,需先定位到DAType元素
///
/// @param[in]
///     scl    - SCLTools实例的引用
/// @param[in]
///     daType - DAType的ID
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteBDAIntoDAType(SCLTools &scl,const SCL_STR &daType)
{
	int num = m_SclCache->GetBDANumInDAType(daType);

	for ( int i = 0; i < num; i++ )
	{
		SCL_BDAINDATYPE_ATTRIB bdaAttrib;
		if ( !m_SclCache->GetBDAAttrib(daType,i,bdaAttrib) )
			continue;
		// 添加BDA信息并定位到BDA
		if ( !scl.AddBDAIntoDAType(bdaAttrib) )
			return false;
		// 回到DAType
		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///     将缓存中的全部DAType信息写入DataTypeTemplates中,需先定位到DataTypeTemplates元素
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteDATypeIntoDataTypeTemplates(SCLTools &scl)
{
	int num = m_SclCache->GetDATypeNum();
	for ( int i = 0; i < num; i++ )
	{
		SCL_DATYPE_ATTRIB daTypeAttrib;
		if ( !m_SclCache->GetDAType(i,daTypeAttrib) )
			continue;
		// 添加DAType并定位到DAType
		if ( !scl.AddDATypeIntoDataTypeTmplt(daTypeAttrib) )
			return false;
		// 写入BDA信息到DAType
		if ( !WriteBDAIntoDAType(scl,daTypeAttrib.id) )
			return false;
		// 回到DataTypeTemplates
		scl.OutOfElem();
	}
	return true;
}

///
/// @brief
///     将缓存中的全部DataTypeTemplates写入SCL元素中,需先定位到SCL元素
///
/// @param[in]
///     scl     - SCLTools实例的引用
///
/// @return
///     true：成功；  false：失败
///
bool SCLCacheManager::WriteDataTypeTemplatesIntoSCL(SCLTools &scl)
{
	// 添加DataTypeTemplates并定位到DataTypeTemplates
	if ( !scl.AddDataTypeTmpltIntoSCL() )
		return false;
	// 写入LNType信息到DataTypeTemplates
	if ( !WriteLNTypeIntoDataTypeTemplates(scl) )
		return false;
	// 写入DOType信息到DataTypeTemplates
	if ( !WriteDOTypeIntoDataTypeTemplates(scl) )
		return false;
	// 写入DAType信息到DataTypeTemplates
	if ( !WriteDATypeIntoDataTypeTemplates(scl) )
		return false;
	// 回到SCL
	scl.OutOfElem();

	return true;
}

///
/// @brief
///     判断SCL文件中特定LDevice是否含有Gse、Smv或与两者相关的Inputs信息
///
/// @param[in]
///     scl     - SCLTools实例的引用
/// @param[in]
///     iedName - LN所在的IED的名称
/// @param[in]
///     ldInst  - LN所在的LDevice的名称
///
/// @return
///     true：有；  false：无
///
bool SCLCacheManager::IsHaveGseOrSmvOrInputsInLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	bool bIsHaveGseOrSmvOrInputs = false;
	// 进入LDevice内部
	if ( scl.IntoElem() )
	{
		// 搜索LN0
		if ( scl.FindLN0InLDevice() )
		{
			// 进入LN0内部
			if ( scl.IntoElem() )
			{
				// 判断是否存在SMV控制块
				scl.ResetMainPos();
				if ( m_bSmvCtrlOpt && scl.FindSMVCtrlInLN0() )
				{
					bIsHaveGseOrSmvOrInputs = true;
				}
				else
				{
					// 判断是否存在GSE控制块
					scl.ResetMainPos();
					if ( m_bGseCtrlOpt && scl.FindGSECtrlInLN0() )
					{
						bIsHaveGseOrSmvOrInputs = true;
					}
					else if ( m_bGseInputsOpt || m_bSmvInputsOpt )
					{
						// 判断是否存在Inputs
						scl.ResetMainPos();
						while ( scl.FindInputsInLNorLN0() )
						{
							if ( scl.IntoElem() )
							{
								scl.ResetMainPos();
								while ( scl.FindExtRefInInputs() )
								{
									SCL_EXTREF_ATTRIB extRefAttrib;
									if ( !scl.GetExtRefAttrib(extRefAttrib) )
										continue;
									// 判断输入端子是否为Goose或SMV虚端子
									if ( ( m_bGseInputsOpt && extRefAttrib.intAddr.find(_T("GOIN")) != std::wstring::npos ) || 
										( m_bSmvInputsOpt && extRefAttrib.intAddr.find(_T("SVIN")) != std::wstring::npos ) )
									{
										bIsHaveGseOrSmvOrInputs = true;
										break;
									}
								}
								// 回到Inputs
								scl.OutOfElem();
							}
							if ( bIsHaveGseOrSmvOrInputs )
								break;
						}
					}
				}
				// 回到LN0
				scl.OutOfElem();
			}
		}
		// 回到LDevice
		scl.OutOfElem();
	}
	if ( bIsHaveGseOrSmvOrInputs )
		return true;
	else
		return false;
}

///
/// @brief
///     分解ExtRef的IntAddr
///
/// @param[in]
///     intAddr - ExtRef的IntAddr
/// @param[in]
///     ldInst  - 保存LDevice的名称
/// @param[in]
///     lnName  - 保存LNode的名称
/// @param[in]
///     doName  - 保存DOI的名称
/// @param[in]
///     daName  - 保存DAI的名称
///
/// @return
///     
///
void SCLCacheManager::SplitExtRefIntAddr(const SCL_STR &intAddr,SCL_STR &ldInst,SCL_STR &lnName,SCL_STR &doName,SCL_STR &daName)
{
	size_t idx1 = intAddr.find(_T("/"));
	if ( idx1 == std::wstring::npos )
		ldInst = _T("");
	else
		ldInst = intAddr.substr(0,idx1);

	size_t idx2 = intAddr.find(_T("."),idx1+1);
	if ( idx2 == std::wstring::npos )
		lnName = _T("");
	else
		lnName = intAddr.substr(idx1+1,idx2-idx1-1);

	idx1 = intAddr.find(_T("."),idx2+1);
	if ( idx1 == std::wstring::npos )
	{
		doName = intAddr.substr(idx2+1);
		daName = _T("");
	}
	else
	{
		doName = intAddr.substr(idx2+1,idx1-idx2-1);
		daName = intAddr.substr(idx1+1);
	}
}
