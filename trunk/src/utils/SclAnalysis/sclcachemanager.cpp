/// @file
///     sclcachemanager.cpp
///
/// @brief
///     SCL�ļ�����������
///
/// @note
///     ����SCLTools����SCL�ļ����������ݻ��浽Cache���Թ���ѯ
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.1.28
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.01.30     |1.0.1.130  |�޸�CacheDataSetInLN0����û�лص�ԭDataSet��������ѭ����BUG|
///    |2013.02.04     |1.0.2.204  |��SCLCache��ʵ���ӿڸı䣬�޸�SCLCacheManager�Ĺ�����������|
///    |2013.02.18     |1.0.3.218  |��Ӳ���ֵ��GOOSE����������ߵ���Ϣ��INPUT�������ӿ�       |
///    |2013.05.27     |1.0.4.527  |�ӳ�DOI��Ϣ�Ľ�����ʹֻ֮�������ѻ����FCDA��ExtRef��ص�DOI��Ϣ|
///    |2013.05.27     |1.0.4.527  |�ӳ�LN��Ϣ�Ľ�����DOI��Ϣ�����׶Σ���ĳDOI���ڵ�LNδ����ʱ�����|
///    |2013.05.27     |1.0.4.527  |�����ж����������˵���Goose��Smv�޹ص�LDevice��Ϣ          |
///    |2013.05.27     |1.0.4.527  |�����ж����������˵���Goose��Smv�޹ص�DataSet��Ϣ          |
///    |2013.05.27     |1.0.4.527  |���Ƿ����Smv��Goose����������ص�Inputs��Ϣ��Ϊ������ѡ�� |
///    |2013.05.27     |1.0.4.527  |���ӽ�SCL��������д���µ�SCL�ļ��Ľӿڣ���ʵ�ֶ�����SCL�ļ��ļ�|
///    |2013.05.27     |1.0.4.527  |�̳�ISCLCacheManager�������ֽӿ���Ϊ�ⲿ�ӿڣ���LoadSCL��  |
///    |2013.06.04     |1.0.4.604  |����GSEControl�е�GSSE����                                 |
///    |2013.06.08     |1.0.4.608  |����Cache�еĽӿھ������õ������ֱ�ӷ���Cacheָ�룬ɾ��ԭ����ȡֵ�ӿ�|
///    |2013.08.07     |1.0.5.807  |����SCL�ļ���SMVͨ����Ϣʱ�����Ӷ�daName���ж��Ա��FCDA�Ƿ�Ϊ9-2LE����Чͨ��|
///    |2013.08.08     |1.0.5.808  |�ӳپֲ������Ķ��壬�������ֵ����ʧ�ܺ�������һ�εĽ����BUG|
///    |2013.08.21     |1.0.5.821  |��ӶԵ�ǰ�����ڴ�ļ�飬�ж��Ƿ��㹻��������SCL�ļ����� |
///    |2013.08.21     |1.0.5.821  |��Ӷ�new�������쳣������ֹ�������                     |
///    |2013.08.26     |1.0.5.826  |���ͷ�SCL�������ݻ���ʱ����ڴ�ѹ����������ʹϵͳ�����ڴ� |
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
///     ����SCL�����ļ�����������Ӧ�Ľ���
///
/// @param[in]
///     strFile - SCL�����ļ���(��·��)
///
/// @return
///     0 - �����ɹ��� -1 - ����ʧ��
///
int SCLCacheManager::LoadSCL(const std::wstring &fileName)
{
	SCLTools scl;
	SCL_IED_ATTRIB iedAttrib;

	int nLoad = -1;
	_PNH oldHandler = _set_new_handler(newhandler);
	try
	{
		// ���Cache
		m_SclCache->ClearCache();

#ifdef MEMORY_ENOUGH_CHECK
		if ( !IsMemoryEnough(fileName) )
			return -1;
#endif

		// ����SCL�ļ�
		if ( !scl.LoadSCL(fileName) )
			return -1;

		// ����SCL�ļ��е�ȫ��IED��Ϣ
		if ( !CacheAllIEDInFile(scl) )
			return -1;

		// ����SCL�ļ��е�ȫ��ͨ�Ų�����Ϣ
		if ( !CacheCommnInfo(scl) )
			return -1;

		// ����SCL�ļ��е�DataTypeTemplates��ȫ����Ϣ
		if ( !CacheDataTypeTemplates(scl) )
			return -1;
		nLoad = 0;
	}
	catch (...)
	{
		TRACE((_T("Load SCL FAIL.\r\n")));
		// ���Cache
		m_SclCache->ClearCache();

		nLoad = -1;
	}

	_set_new_handler(oldHandler);

	return nLoad;
}

///
/// @brief
///     ж�أ���գ�����SCL�������ݵ�Cache
///
void SCLCacheManager::UnloadSCL()
{
	m_SclCache->ClearCache();
}

///
/// @brief
///     ��SCL�ļ���ʽ�������������
///
/// @param[in]
///     strFile - Ҫ���浽��SCL�ļ���(��·��)
///
/// @return
///     0 - �����ɹ��� -1 - ����ʧ��
///
int SCLCacheManager::SaveSCL(const std::wstring &fileName)
{
	SCLTools scl;

	// ���SCL����λ��SCL
	if ( !scl.AddSCL() )
		return -1;
	// д��ͨ��������Ϣ��SCL
	if ( !WriteCommnInfoIntoSCL(scl) )
		return -1;
	// д��IED��Ϣ��SCL
	if ( !WriteAllIEDIntoSCL(scl) )
		return -1;
	// д��DataTypeTemplates��SCL
	if ( !WriteDataTypeTemplatesIntoSCL(scl) )
		return -1;
	// ���浽Ŀ���ļ�
	if ( !scl.SaveSCL(fileName) )
		return -1;

	return 0;
}

///
/// @brief
///     ��ȡ�������������Scl�ļ������ָ��
///
/// @return
///     Scl�ļ������ָ��
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
		// ��ȡSCL�ļ�����������ڴ��С
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

		// ��ȡ�ڴ��С
		MEMORYSTATUS  memStat = {sizeof(MEMORYSTATUS)};
		::GlobalMemoryStatus(&memStat);

		// �ж��ڴ��Ƿ��㹻ʹ��
		if ( memStat.dwAvailPhys > maxMem )
			bMemEnough = true;
	}

	return bMemEnough;
}

///
/// @brief
///     ��SCL�ļ��л�ȡLN0��ȫ����Goose��SMV���ƿ��������DataSet����Ϣ������,
///     ���Ƚ���LN0Ԫ�أ�IntoElem��,�һ�����Goose��SMV���ƿ���Ϣ
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - DataSet���ڵ�IED������
/// @param[in]
///     ldInst  - DataSet���ڵ�LDevice��ʵ����
///
/// @return
///     true���ɹ���  false��ʧ��
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
			// �ж��Ƿ񻺴��DataSet��Ϣ
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

			// ����DataSet����FCDA��Ϣ
			if ( bAdd )
			{
				if ( !m_SclCache->AddDataSetIntoLN0(iedName,ldInst,datSetAttrib) )
					return false;
				// ����DataSet�е�FCDA������Ϣ
				if ( bSmv )
				{
					static int fcdaNum = 0;
					for ( i=0 ; i < datSetAttrib.fcdaNum ; i++)
					{
						SCL_FCDA_ATTRIB fcdaAttrib;
						if ( !scl.GetOneFCDAAttrib(fcdaAttrib,i) )
							break;
						// �жϸ�FCDA�Ƿ�Ϊ��Чͨ����9-2LE��
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
///     ��SCL�ļ��л�ȡLN0��ȫ��SampledValueControl����Ϣ������,���Ƚ���LN0Ԫ�أ�IntoElem��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - SampledValueControl���ڵ�IED������
/// @param[in]
///     ldInst  - SampledValueControl���ڵ�LDevice��ʵ����
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::CacheAllSMVCtrlInLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// ����ȫ��SampledValueControl��Ϣ
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
///     ��SCL�ļ��л�ȡLN0��ȫ��GSEControl����Ϣ������,���Ƚ���LN0Ԫ�أ�IntoElem��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - GSEControl���ڵ�IED������
/// @param[in]
///     ldInst  - GSEControl���ڵ�LDevice��ʵ����
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::CacheAllGSECtrlInLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// ����GSEControl��Ϣ
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
///     ��SCL�ļ��л�ȡLN0/LN��ȫ��DOI����Ϣ������,���Ƚ���LN0��LNԪ�أ�IntoElem��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - DOI���ڵ�IED������
/// @param[in]
///     ldInst  - DOI���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName  - DOI���ڵ�LN�����ƣ�LN0�̶�Ϊ��LLN0��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::CacheAllDOIIntoLNorLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName)
{
	// ����DOI��Ϣ
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
///     ��SCL�ļ��л�ȡIED�����ѻ����ȫ��FCDA�������DOI����Ϣ������,���Ƚ���IEDԪ�أ�IntoElem��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - DOI���ڵ�IED������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::CacheAllDOIFromFCDAIntoIED(SCLTools &scl, const SCL_STR &iedName)
{
	// ��ȡ�ѻ����LDevice����
	int ldNum = m_SclCache->GetLDeviceNum(iedName);
	for ( int i = 0; i < ldNum; i++ )
	{
		// ��ȡLDevice��Ϣ
		SCL_ACCESS_POINT_ATTRIB apAttrib;
		SCL_LDEVICE_ATTRIB ldAttrib;
		if ( !m_SclCache->GetLDeviceAttrib(iedName,i,apAttrib,ldAttrib) )
			continue;
		// ��ȡLN0�е����ݼ�����Ŀ
		int dataSetNum = m_SclCache->GetDataSetNumInLN0(iedName,ldAttrib.ldInst);

		// ����LN0�е�FCDA������FCDAָ���DOI��Ϣ
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

				// ����FCDA������ָ����DOI
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
								// ����LN������Ϣ
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
										// ����DOI��Ϣ
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
								// �ص�LN����LN0
								scl.OutOfElem();
							}
						}
						// �ص�LDevice
						scl.OutOfElem();
					}
					// �ص�Server
					scl.OutOfElem();
					// �ص�AccessPoint
					scl.OutOfElem();
				}
			}
		}
	}

	return true;
}

///
/// @brief
///     ��SCL�ļ��л�ȡIED�����ѻ����ȫ��Inputs�������DOI����Ϣ������,���Ƚ���IEDԪ�أ�IntoElem��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - DOI���ڵ�IED������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::CacheAllDOIFromInputsIntoIED(SCLTools &scl, const SCL_STR &iedName)
{
	// ��ȡ�ѻ����LDevice����
	int ldNum = m_SclCache->GetLDeviceNum(iedName);
	for ( int i = 0; i < ldNum; i++ )
	{
		// ��ȡLDevice��Ϣ
		SCL_ACCESS_POINT_ATTRIB apAttrib;
		SCL_LDEVICE_ATTRIB ldAttrib;
		if ( !m_SclCache->GetLDeviceAttrib(iedName,i,apAttrib,ldAttrib) )
			continue;
		// ��ȡLDevice���ѻ����LN����Ŀ
		int lnNum = m_SclCache->GetLNNum(iedName,ldAttrib.ldInst);

		// ����LN0��LN�е�Inputs������ExtRefָ���DOI��Ϣ
		for ( int j = 0; j < lnNum+1; j++ )
		{
			// ��ȡLnName
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

			// ����EXTREF��IntAddr��ָ���DOI������������ָ����DOI��Ϣ
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
								// ����LN������Ϣ
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
								// �ص�LN����LN0
								scl.OutOfElem();
							}
						}
						// �ص�LDevice
						scl.OutOfElem();
					}
					// �ص�Server
					scl.OutOfElem();
					// �ص�AccessPoint
					scl.OutOfElem();
				}
			}
		}
	}

	return true;
}

///
/// @brief
///     ��SCL�ļ��л�ȡLN0/LN��Inputs�е�Extref��Ϣ������,���Ƚ���LN0��LNԪ�أ�IntoElem��
///     Ŀǰֻ������SMV��GSE���Extref��Ϣ
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - DOI���ڵ�IED������
/// @param[in]
///     ldInst  - DOI���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName  - DOI���ڵ�LN�����ƣ�LN0�̶�Ϊ��LLN0��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::CacheInputsIntoLNorIN0(SCLTools &scl, const SCL_STR &iedName,
											 const SCL_STR &ldInst, const SCL_STR &lnName)
{
	int i = 0;

	// ����Inputs��Ϣ
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
				// �ж���������Ƿ�ΪGoose��SMV�����
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
///     ��SCL�ļ��л�ȡLDevice��LN0����Ϣ������,���Ƚ���LDeviceԪ�أ�IntoElem��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - LN0���ڵ�IED������
/// @param[in]
///     ldInst  - LN0���ڵ�LDevice������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::CacheLN0InLDevice(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// ����LN0������Ϣ
	scl.ResetMainPos();
	if ( scl.FindLN0InLDevice() )
	{
		SCL_LN0_ATTRIB ln0Attrib;
		if ( scl.GetLN0Attrib(ln0Attrib) )
		{
			if ( !m_SclCache->AddLN0IntoLD(iedName,ldInst,ln0Attrib) )
				return false;

			// ����LN0�ڲ�
			if ( scl.IntoElem() )
			{
				// ����LN0�е�SMV���ƿ���Ϣ
				if ( m_bSmvCtrlOpt )
				{
					if ( !CacheAllSMVCtrlInLN0(scl,iedName,ldInst) )
						return false;
				}
				// ����LN0�е�GSE���ƿ���Ϣ
				if ( m_bGseCtrlOpt )
				{
					if ( !CacheAllGSECtrlInLN0(scl,iedName,ldInst) )
						return false;
				}
				// ����LN0����SMV��GSE���ƿ��������ȫ��DataSet��Ϣ
				if ( !CacheAllDataSetInLN0(scl,iedName,ldInst) )
					return false;
// 				// ����LN0�е�DOI��Ϣ
// 				if ( !CacheAllDOIIntoLNorLN0(scl,iedName,ldInst,_T("LLN0")) )
// 					return false;
				// ����LN0�е�SMV��GSE��Inputs��Ϣ
				// ��IEC61850Ӧ�����š���һ����ÿ��װ�õ�LLN0��Inputs���ֶ����װ�õ�Goose��
				// ����ֵ����������Ϣ������������ӵ��߼��ڵ��ǰ׺һ�㺬�С�GOIN���͡�SVIN��
				if ( m_bGseInputsOpt || m_bSmvInputsOpt )
				{
					if ( !CacheInputsIntoLNorIN0(scl,iedName,ldInst,_T("LLN0")) )
						return false;
				}

				// �ص�LN0
				scl.OutOfElem();
			}
		}
	}

	return true;
}

///
/// @brief
///     ��SCL�ļ��л�ȡLDevice��LN����Ϣ������,���Ƚ���LDeviceԪ�أ�IntoElem��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - LN���ڵ�IED������
/// @param[in]
///     ldInst  - LN���ڵ�LDevice������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::CacheLNInLDevice(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	scl.ResetMainPos();
	while ( scl.FindLNInLDevice() )
	{
		// ����LN������Ϣ
		SCL_LN_ATTRIB lnAttrib;
		if ( !scl.GetLNAttrib(lnAttrib) )
			continue;
		if ( !m_SclCache->AddLNIntoLD(iedName,ldInst,lnAttrib) )
			return false;
// 		// ����LN�ڲ�
// 		if ( scl.IntoElem() )
// 		{
// 			SCL_STR lnName = lnAttrib.lnPrefix+lnAttrib.lnClass+lnAttrib.lnInst;
// // 			// ����LN�е�DOI��Ϣ
// // 			if ( !CacheAllDOIIntoLNorLN0(scl,iedName,ldInst,lnName) )
// // 				return false;
// 			// ����LN�е�SMV��GSE��Inputs��Ϣ
// 			if ( m_bGseInputsOpt || m_bSmvInputsOpt )
// 			{
// 				if ( !CacheInputsIntoLNorIN0(scl,iedName,ldInst,lnName) )
// 					return false;
// 			}
// 			// �ص�LN
// 			scl.OutOfElem();
// 		}
	}

	return true;
}

///
/// @brief
///     ��SCL�ļ��л�ȡAccessPoint��ȫ��LDevice����Ϣ������,���Ƚ���AccessPointԪ�أ�IntoElem��
///     Ŀǰֻ����LDevice->LN0
///
/// @param[in]
///     scl      - SCLToolsʵ��������
/// @param[in]
///     iedName  - LDeivce���ڵ�IED������
/// @param[in]
///     apAttrib - AccessPoint��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::CacheAllLDeviceInAP(SCLTools &scl, const SCL_STR &iedName,
										  const SCL_ACCESS_POINT_ATTRIB &apAttrib)
{
	// ����AccessPoint�ڲ�
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
					// ����LDevice��������Ϣ
					SCL_LDEVICE_ATTRIB ldAttrib;
					if ( !scl.GetLDeviceAttrib(ldAttrib) )
						continue;

					// �ж�LDevice�е�LN0�Ƿ����GSE��SMV���ƿ������ص�Inputs��Ϣ
					if ( !IsHaveGseOrSmvOrInputsInLN0(scl,iedName,ldAttrib.ldInst) )
						continue;

					if ( !m_SclCache->AddLDeviceIntoIED(iedName,apAttrib,ldAttrib) )
						return false;

					if ( scl.IntoElem() )
					{
						// ����LDevice��LN0��Ϣ
						if ( !CacheLN0InLDevice(scl,iedName,ldAttrib.ldInst) )
							return false;
// 						// ����LDevice��LN��Ϣ
// 						if ( !CacheLNInLDevice(scl,iedName,ldAttrib.ldInst) )
// 							return false;
						// �ص�LDevice
						scl.OutOfElem();
					}
				}
				// �ص�Server
				scl.OutOfElem();
			}
		}
		// �ص�AccessPoint
		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///     ��SCL�ļ��л�ȡIED��ȫ��AccessPoint����Ϣ������,���Ƚ���IEDԪ�أ�IntoElem��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - AccessPoint���ڵ�IED������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::CacheAllAccessPointInIED(SCLTools &scl, const SCL_STR &iedName)
{
	scl.ResetMainPos();
	while( scl.FindAccessPointInIED() )
	{
		SCL_ACCESS_POINT_ATTRIB apAttrib;
		if ( !scl.GetAccessPointAttrib(apAttrib) )
			continue;

		// ����AccessPoint�е�ȫ��LDevice��Ϣ
		if ( !CacheAllLDeviceInAP(scl,iedName,apAttrib) )
			return false;
	}

	return true;
}

///
/// @brief
///     ��SCL�ļ��л�ȡȫ��IED�е���Ϣ������
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::CacheAllIEDInFile(SCLTools &scl)
{
	// �����Ԫ��SCL
	scl.ResetMainPos();
	if ( !scl.FindSCL() )
		return false;
	if ( !scl.IntoElem() )
		return false;

	scl.ResetMainPos();
	while ( scl.FindIEDInSCL() )
	{
		// ����IED������Ϣ
		SCL_IED_ATTRIB iedAttrib;
		if ( !scl.GetIEDAttrib(iedAttrib) )
			continue;
		if ( !m_SclCache->AddIED(iedAttrib) )
			return false;

		if ( scl.IntoElem() )
		{
			// ����IED�е�ȫ��AccessPoint��Ϣ
			if ( !CacheAllAccessPointInIED(scl,iedAttrib.iedName) )
				return false;
			// �������ѻ����FCDA�������DOI����Ϣ
			if ( !CacheAllDOIFromFCDAIntoIED(scl,iedAttrib.iedName))
				return false;
			// �������ѻ����Inputs�������DOI����Ϣ
			if ( m_bGseInputsOpt || m_bSmvInputsOpt )
			{
				if ( !CacheAllDOIFromInputsIntoIED(scl,iedAttrib.iedName))
					return false;
			}

			// �ص�IED
			scl.OutOfElem();
		}
	}

	return true;
}

///
/// @brief
///     ��SCL�ļ��л�ȡSubNetwork�Ĺ���SMV��ȫ��ͨ�Ų�����Ϣ������,���Ƚ���SubNetworkԪ�أ�IntoElem��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::CacheSMVCommnInSubNetwork(SCLTools &scl)
{
	// ����������SubNetwork�е�ConnectedAP��SMV
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
					// ͨ�Ų����������������Ĭ��ֵ
					if ( smvAttrib.appID.empty() )
					{
						smvAttrib.appID = _T("FFFF");
						smvAttrib.macAddr = _T("01-0C-CD-04-FF-FF");
					}
					if ( !m_SclCache->AddSMVCommn(cnntAPAttrib,smvAttrib) )
						return false;
				}
			}
			// �ص�ConnectedAP
			scl.OutOfElem();
		}
	}

	return true;
}

///
/// @brief
///     ��SCL�ļ��л�ȡSubNetwork�Ĺ���GOOSE/GSSE��ȫ��ͨ�Ų�����Ϣ������,���Ƚ���SubNetworkԪ�أ�IntoElem��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::CacheGSECommnInSubNetwork(SCLTools &scl)
{
	// ����������SubNetwork�е�ConnectedAP��GSE
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
					// ͨ�Ų����������������Ĭ��ֵ
					if ( gseAttrib.appID.empty() )
					{
						gseAttrib.appID = _T("FFFF");
						gseAttrib.macAddr = _T("01-0C-CD-01-FF-FF");
					}
					if ( !m_SclCache->AddGSECommn(cnntAPAttrib,gseAttrib) )
						return false;
				}
			}
			// �ص�ConnectedAP
			scl.OutOfElem();
		}
	}

	return true;
}

///
/// @brief
///     ��SCL�ļ��л�ȡȫ��ͨ�Ų�����Ϣ������
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::CacheCommnInfo(SCLTools &scl)
{
	// �����Ԫ��SCL
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
			// ����SMVͨ����Ϣ
			if ( m_bSmvCtrlOpt )
			{
				if ( !CacheSMVCommnInSubNetwork(scl) )
					return false;
			}
			// ����GOOSE/GSSEͨ����Ϣ
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
///     ��SCL�ļ��л�ȡLNodeType��ȫ��DO����Ϣ������,���Ƚ���LNodeTypeԪ�أ�IntoElem��
///
/// @param[in]
///     scl    - SCLToolsʵ��������
/// @param[in]
///     lnType - LNodeType��ID
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��SCL�ļ��л�ȡȫ��LNodeType��Ϣ������
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��SCL�ļ��л�ȡDOType��ȫ��DA����Ϣ������,���Ƚ���DOTypeԪ�أ�IntoElem��
///
/// @param[in]
///     scl    - SCLToolsʵ��������
/// @param[in]
///     doType - DOType��ID
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��SCL�ļ��л�ȡȫ��DOType��Ϣ������
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��SCL�ļ��л�ȡDAType��ȫ��BDA����Ϣ������,���Ƚ���DATypeԪ�أ�IntoElem��
///
/// @param[in]
///     scl    - SCLToolsʵ��������
/// @param[in]
///     daType - DAType��ID
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��SCL�ļ��л�ȡȫ��DAType��Ϣ������
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��SCL�ļ��л�ȡDataTypeTemplates��ȫ����Ϣ������
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::CacheDataTypeTemplates(SCLTools &scl)
{
	// �����Ԫ��SCL
	if ( !scl.FindSCL() )
		return false;
	if ( !scl.IntoElem() )
		return false;

	// ����DataTypeTemplatesԪ��
	if ( !scl.FindDataTypeTmpltInSCL() )
		return false;
	if ( !scl.IntoElem() )
		return false;

	// ����SCL�ļ��е�ȫ��LNodeType��Ϣ
	if ( !CacheLNType(scl) )
		return false;

	// ����SCL�ļ��е�ȫ��DOType��Ϣ
	if ( !CacheDOType(scl) )
		return false;

	// ����SCL�ļ��е�ȫ��DAType��Ϣ
	if ( !CacheDAType(scl) )
		return false;

	return true;
}

///
/// @brief
///     �������е��ض�LDevice��ȫ��DataSet����Ϣд��LN0��,���ȶ�λ��LN0Ԫ��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - DataSet���ڵ�IED������
/// @param[in]
///     ldInst  - DataSet���ڵ�LDevice��ʵ����
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteAllDataSetIntoLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	int dataSetNum = m_SclCache->GetDataSetNumInLN0(iedName,ldInst);

	for ( int i = 0; i < dataSetNum ; i++ )
	{
		SCL_DATASET_ATTRIB datSetAttrib;
		if ( !m_SclCache->GetDataSetAttribInLN0(iedName,ldInst,i,datSetAttrib) )
			continue;
		// ���DataSet��LN0����λ��DataSet
		if ( !scl.AddDataSetIntoLN0(datSetAttrib) )
			continue;
		// �������ݼ������е��������FCDA
		for ( int j = 0; j < datSetAttrib.fcdaNum; j++ )
		{
			SCL_FCDA_ATTRIB fcdaAttrib;
			if ( !m_SclCache->GetFCDAAttrib(iedName,ldInst,_T("LLN0"),datSetAttrib.dataSetName,j,fcdaAttrib) )
				continue;
			// ���FCDA��DataSet����λ��FCDA
			if ( !scl.AddFCDAIntoDataSet(fcdaAttrib) )
				return false;
			// �ص�DataSet
			scl.OutOfElem();
		}
		// �ص�LN0
		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///     �������е��ض�LDevice��ȫ��SampledValueControl����Ϣд��LN0��,���ȶ�λ��LN0Ԫ��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - SampledValueControl���ڵ�IED������
/// @param[in]
///     ldInst  - SampledValueControl���ڵ�LDevice��ʵ����
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteAllSMVCtrlIntoLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// д��SampledValueControl��Ϣ
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
///     �������е��ض�LDevice��ȫ��GSEControl����Ϣд��LN0��,���ȶ�λ��LN0Ԫ��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - GSEControl���ڵ�IED������
/// @param[in]
///     ldInst  - GSEControl���ڵ�LDevice��ʵ����
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteAllGSECtrlIntoLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// д��GSEControl��Ϣ
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
///      �������е��ض�LNode��ȫ��DOI����Ϣд��LNode��,���ȶ�λ��LN0��LNԪ��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - DOI���ڵ�IED������
/// @param[in]
///     ldInst  - DOI���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName  - DOI���ڵ�LN�����ƣ�LN0�̶�Ϊ��LLN0��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteAllDOIIntoLNorLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName)
{
	// д��DOI��Ϣ
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
///     �������е��ض�LNode��Inputs��ȫ��Extref��Ϣд��LNode��,���ȶ�λ��LN0��LNԪ��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - DOI���ڵ�IED������
/// @param[in]
///     ldInst  - DOI���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName  - DOI���ڵ�LN�����ƣ�LN0�̶�Ϊ��LLN0��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteInputsIntoLNorIN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst,
											 const SCL_STR &lnName)
{
	int i = 0;

	// д��ExtRef��Ϣ
	int num = m_SclCache->GetExtRefNumInLNorLN0(iedName,ldInst,lnName);
	if ( num > 0 )
	{
		// д��Inputs
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
///     �������е��ض�LDevice�е�LN0����Ϣд��LDeivce��,���ȶ�λ��LDeviceԪ��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - LN0���ڵ�IED������
/// @param[in]
///     ldInst  - LN0���ڵ�LDevice������
///
/// @return
///     true���ɹ���  false��ʧ��;
///
bool SCLCacheManager::WriteLN0IntoLDevice(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	SCL_LN0_ATTRIB ln0Attrib;

	if ( !m_SclCache->GetLN0Attrib(iedName,ldInst,ln0Attrib) )
		return false;
	// ���LN0��LDevice����λ��LN0
	if ( !scl.AddLN0IntoLDevice(ln0Attrib) )
		return false;

	// д��DataSet��Ϣ��LN0
	if ( !WriteAllDataSetIntoLN0(scl,iedName,ldInst) )
		return false;
	// д��DOI��Ϣ��LN0
	if ( !WriteAllDOIIntoLNorLN0(scl,iedName,ldInst,_T("LLN0")) )
		return false;
	// д��SMV���ƿ���Ϣ��LN0
	if ( !WriteAllSMVCtrlIntoLN0(scl,iedName,ldInst) )
		return false;
	// д��GSE���ƿ���Ϣ��LN0
	if ( !WriteAllGSECtrlIntoLN0(scl,iedName,ldInst) )
		return false;
	// д��Inputs��Ϣ��LN0
	if ( !WriteInputsIntoLNorIN0(scl,iedName,ldInst,_T("LLN0")) )
		return false;

	// �ص�LDevice
	scl.OutOfElem();

	return true;
}

///
/// @brief
///     �������е��ض�LDevice�е�LN����Ϣд��LDeivce��,���ȶ�λ��LDeviceԪ��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - LN���ڵ�IED������
/// @param[in]
///     ldInst  - LN���ڵ�LDevice������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteLNIntoLDevice(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	int num = m_SclCache->GetLNNum(iedName,ldInst);
	for ( int i = 0; i < num; i++ )
	{
		SCL_LN_ATTRIB lnAttrib;
		if ( !m_SclCache->GetLNAttrib(iedName,ldInst,i,lnAttrib) )
			continue;
		// ���LN��LDevice����λ��LN
		if ( !scl.AddLNIntoLDevice(lnAttrib) )
			return false;

		SCL_STR lnName = lnAttrib.lnPrefix + lnAttrib.lnClass + lnAttrib.lnInst;
		// д��DOI��Ϣ��LN
		if ( !WriteAllDOIIntoLNorLN0(scl,iedName,ldInst,lnName) )
			return false;
		// д��Inputs��Ϣ��LN
		if ( !WriteInputsIntoLNorIN0(scl,iedName,ldInst,lnName) )
			return false;

		// �ص�LDevice
		scl.OutOfElem();		
	}

	return true;
}

///
/// @brief
///     �������е��ض�IED��ȫ��LDevice����Ϣд��IED��,���ȶ�λ��IEDԪ��
///
/// @param[in]
///     scl      - SCLToolsʵ��������
/// @param[in]
///     iedName  - LDeivce���ڵ�IED������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteAllLDeviceIntoIED(SCLTools &scl, const SCL_STR &iedName)
{
	// ��ȡLDevice����
	int num = m_SclCache->GetLDeviceNum(iedName);

	for ( int i = 0; i < num; i++ )
	{
		// ��ȡLDevice��Ϣ
		SCL_ACCESS_POINT_ATTRIB apAttrib;
		SCL_LDEVICE_ATTRIB ldAttrib;
		if ( !m_SclCache->GetLDeviceAttrib(iedName,i,apAttrib,ldAttrib) )
			continue;

		if ( !scl.IntoElem() )
			return false;
		scl.ResetMainPos();
		if ( !scl.FindAccessPointInIED(apAttrib.apName) )
		{
			// AccessPoint�����ڣ��򴴽�
			scl.OutOfElem();
			if ( !scl.AddAccessPointIntoIED(apAttrib) )
				return false;
			if ( !scl.AddServerIntoAccessPoint() )
				return false;
		}
		else
		{
			// AccessPoint���ڣ������
			if ( !scl.IntoElem() )
				return false;
			if ( !scl.FindServerInAccessPoint() )
				return false;
		}
		// ���LDevice����λ��LDevice
		if ( !scl.AddLDeviceIntoServer(ldAttrib) )
			return false;
		// д��LN0��Ϣ��LDevice
		if ( !WriteLN0IntoLDevice(scl,iedName,ldAttrib.ldInst) )
			return false;
		// д��LN��Ϣ��LDevice
		if ( !WriteLNIntoLDevice(scl,iedName,ldAttrib.ldInst) )
			return false;
		// �ص�Server
		scl.OutOfElem();
		// �ص�AccessPoint
		scl.OutOfElem();
		// �ص�IED
		scl.OutOfElem();
	}
	return true;
}

///
/// @brief
///      �������е�ȫ��IED�е���Ϣд��SCLԪ����,���ȶ�λ��SCLԪ��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteAllIEDIntoSCL(SCLTools &scl)
{
	int num = m_SclCache->GetIEDNum();
	for ( int i = 0; i < num; i++ )
	{
		// ��ȡIED��Ϣ
		SCL_IED_ATTRIB iedAttrib;
		if ( !m_SclCache->GetIEDAttrib(i,iedAttrib) )
			continue;
		// ���IED����λ��IED
		if ( !scl.AddIEDIntoSCL(iedAttrib) )
			return false;
		// д��ȫ��LDevice��IED
		if ( !WriteAllLDeviceIntoIED(scl,iedAttrib.iedName) )
			return false;
		// �ص�SCL
		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///     �������еĹ���SMV��ȫ��ͨ�Ų�����Ϣд��SubNetworkԪ����,���ȶ�λ��SubNetworkԪ��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
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
			// ConnectedAP�����ڣ��򴴽�
			scl.OutOfElem();
			if ( !scl.AddConnectedAPIntoSubNetwork(cnntAPAttrib[i]) )
				goto errorHandle;
		}
		// ���SMV��ConnectedAP����λ��SMV
		if ( !scl.AddSMVIntoConnectedAP(smvAttrib[i]) )
			goto errorHandle;
		// �ص�ConnectedAP
		scl.OutOfElem();
		// �ص�SubNetwork
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
///     �������еĹ���GOOSE/GSSE��ȫ��ͨ�Ų�����Ϣд��SubNetworkԪ����,���ȶ�λ��SubNetworkԪ��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
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
			// ConnectedAP�����ڣ��򴴽�
			scl.OutOfElem();
			if ( !scl.AddConnectedAPIntoSubNetwork(cnntAPAttrib[i]) )
				goto errorHandle;
		}
		// ���GSE��ConnectedAP����λ��GSE
		if ( !scl.AddGSEIntoConnectedAP(gseAttrib[i]) )
			goto errorHandle;
		// �ص�ConnectedAP
		scl.OutOfElem();
		// �ص�SubNetwork
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
///     �������е�ȫ��ͨ�Ų�����Ϣд��SCLԪ����,���ȶ�λ��SCLԪ��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteCommnInfoIntoSCL(SCLTools &scl)
{
	// ���Communication����λ��Communication
	if ( !scl.AddCommnIntoSCL() )
		return false;
	// ���SubNetwork����λ��SubNetwork
	SCL_SUBNETWORK_ATTRIB subNetworkAttrib = {_T("subnetwork")};
	if ( !scl.AddSubNetworkIntoCommn(subNetworkAttrib) )
		return false;
	// д��SMVͨ�����õ�SubNetwork
	if ( !WriteSMVCommnIntoSubNetwork(scl) )
		return false;
	// д��GSEͨ�����õ�SubNetwork
	if ( !WriteGSECommnIntoSubNetwork(scl) )
		return false;
	// �ص�Communication
	scl.OutOfElem();
	// �ص�SCL
	scl.OutOfElem();
	return true;
}

///
/// @brief
///     �������е��ض�LNodeType��ȫ��DO����Ϣд��LNodeType��,���ȶ�λ��LNodeTypeԪ��
///
/// @param[in]
///     scl    - SCLToolsʵ��������
/// @param[in]
///     lnType - LNodeType��ID
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteDOIntoLNType(SCLTools &scl,const SCL_STR &lnType)
{
	int num = m_SclCache->GetDONumInLNType(lnType);

	for ( int i = 0; i < num; i++ )
	{
		SCL_DOINLNTYPE_ATTRIB doAttrib;
		if ( !m_SclCache->GetDOAttrib(lnType,i,doAttrib) )
			continue;
		// ���DO��Ϣ����λ��DO
		if ( !scl.AddDOIntoLNType(doAttrib) )
			return false;
		// �ص�LNType
		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///     �������е�ȫ��LNodeType��Ϣд��DataTypeTemplates��,���ȶ�λ��DataTypeTemplatesԪ��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteLNTypeIntoDataTypeTemplates(SCLTools &scl)
{
	int num = m_SclCache->GetLNTypeNum();
	for ( int i = 0; i < num; i++ )
	{
		SCL_LNTYPE_ATTRIB lnTypeAttrib;
		if ( !m_SclCache->GetLNType(i,lnTypeAttrib) )
			continue;
		// ���LNType����λ��LNType
		if ( !scl.AddLNTypeIntoDataTypeTmplt(lnTypeAttrib) )
			return false;
		// д��DO��Ϣ��LNType
		if ( !WriteDOIntoLNType(scl,lnTypeAttrib.id) )
			return false;
		// �ص�DataTypeTemplates
		scl.OutOfElem();
	}
	return true;
}

///
/// @brief
///     �������е��ض�DOType��ȫ��DA����Ϣд��DOType��,���ȶ�λ��DOTypeԪ��
///
/// @param[in]
///     scl    - SCLToolsʵ��������
/// @param[in]
///     doType - DOType��ID
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteDAIntoDOType(SCLTools &scl,const SCL_STR &doType)
{
	int num = m_SclCache->GetDANumInDOType(doType);

	for ( int i = 0; i < num; i++ )
	{
		SCL_DAINDOTYPE_ATTRIB daAttrib;
		if ( !m_SclCache->GetDAAttrib(doType,i,daAttrib) )
			continue;
		// ���DA��Ϣ����λ��DA
		if ( !scl.AddDAIntoDOType(daAttrib) )
			return false;
		// �ص�DOType
		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///     �������е�ȫ��DOType��Ϣд��DataTypeTemplates��,���ȶ�λ��DataTypeTemplatesԪ��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteDOTypeIntoDataTypeTemplates(SCLTools &scl)
{
	int num = m_SclCache->GetDOTypeNum();
	for ( int i = 0; i < num; i++ )
	{
		SCL_DOTYPE_ATTRIB doTypeAttrib;
		if ( !m_SclCache->GetDOType(i,doTypeAttrib) )
			continue;
		// ���DOType����λ��DOType
		if ( !scl.AddDOTypeIntoDataTypeTmplt(doTypeAttrib) )
			return false;
		// д��DA��Ϣ��DOType
		if ( !WriteDAIntoDOType(scl,doTypeAttrib.id) )
			return false;
		// �ص�DataTypeTemplates
		scl.OutOfElem();
	}
	return true;
}

///
/// @brief
///     �������е��ض�DAType��ȫ��BDA����Ϣд��DAType��,���ȶ�λ��DATypeԪ��
///
/// @param[in]
///     scl    - SCLToolsʵ��������
/// @param[in]
///     daType - DAType��ID
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteBDAIntoDAType(SCLTools &scl,const SCL_STR &daType)
{
	int num = m_SclCache->GetBDANumInDAType(daType);

	for ( int i = 0; i < num; i++ )
	{
		SCL_BDAINDATYPE_ATTRIB bdaAttrib;
		if ( !m_SclCache->GetBDAAttrib(daType,i,bdaAttrib) )
			continue;
		// ���BDA��Ϣ����λ��BDA
		if ( !scl.AddBDAIntoDAType(bdaAttrib) )
			return false;
		// �ص�DAType
		scl.OutOfElem();
	}

	return true;
}

///
/// @brief
///     �������е�ȫ��DAType��Ϣд��DataTypeTemplates��,���ȶ�λ��DataTypeTemplatesԪ��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteDATypeIntoDataTypeTemplates(SCLTools &scl)
{
	int num = m_SclCache->GetDATypeNum();
	for ( int i = 0; i < num; i++ )
	{
		SCL_DATYPE_ATTRIB daTypeAttrib;
		if ( !m_SclCache->GetDAType(i,daTypeAttrib) )
			continue;
		// ���DAType����λ��DAType
		if ( !scl.AddDATypeIntoDataTypeTmplt(daTypeAttrib) )
			return false;
		// д��BDA��Ϣ��DAType
		if ( !WriteBDAIntoDAType(scl,daTypeAttrib.id) )
			return false;
		// �ص�DataTypeTemplates
		scl.OutOfElem();
	}
	return true;
}

///
/// @brief
///     �������е�ȫ��DataTypeTemplatesд��SCLԪ����,���ȶ�λ��SCLԪ��
///
/// @param[in]
///     scl     - SCLToolsʵ��������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheManager::WriteDataTypeTemplatesIntoSCL(SCLTools &scl)
{
	// ���DataTypeTemplates����λ��DataTypeTemplates
	if ( !scl.AddDataTypeTmpltIntoSCL() )
		return false;
	// д��LNType��Ϣ��DataTypeTemplates
	if ( !WriteLNTypeIntoDataTypeTemplates(scl) )
		return false;
	// д��DOType��Ϣ��DataTypeTemplates
	if ( !WriteDOTypeIntoDataTypeTemplates(scl) )
		return false;
	// д��DAType��Ϣ��DataTypeTemplates
	if ( !WriteDATypeIntoDataTypeTemplates(scl) )
		return false;
	// �ص�SCL
	scl.OutOfElem();

	return true;
}

///
/// @brief
///     �ж�SCL�ļ����ض�LDevice�Ƿ���Gse��Smv����������ص�Inputs��Ϣ
///
/// @param[in]
///     scl     - SCLToolsʵ��������
/// @param[in]
///     iedName - LN���ڵ�IED������
/// @param[in]
///     ldInst  - LN���ڵ�LDevice������
///
/// @return
///     true���У�  false����
///
bool SCLCacheManager::IsHaveGseOrSmvOrInputsInLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst)
{
	bool bIsHaveGseOrSmvOrInputs = false;
	// ����LDevice�ڲ�
	if ( scl.IntoElem() )
	{
		// ����LN0
		if ( scl.FindLN0InLDevice() )
		{
			// ����LN0�ڲ�
			if ( scl.IntoElem() )
			{
				// �ж��Ƿ����SMV���ƿ�
				scl.ResetMainPos();
				if ( m_bSmvCtrlOpt && scl.FindSMVCtrlInLN0() )
				{
					bIsHaveGseOrSmvOrInputs = true;
				}
				else
				{
					// �ж��Ƿ����GSE���ƿ�
					scl.ResetMainPos();
					if ( m_bGseCtrlOpt && scl.FindGSECtrlInLN0() )
					{
						bIsHaveGseOrSmvOrInputs = true;
					}
					else if ( m_bGseInputsOpt || m_bSmvInputsOpt )
					{
						// �ж��Ƿ����Inputs
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
									// �ж���������Ƿ�ΪGoose��SMV�����
									if ( ( m_bGseInputsOpt && extRefAttrib.intAddr.find(_T("GOIN")) != std::wstring::npos ) || 
										( m_bSmvInputsOpt && extRefAttrib.intAddr.find(_T("SVIN")) != std::wstring::npos ) )
									{
										bIsHaveGseOrSmvOrInputs = true;
										break;
									}
								}
								// �ص�Inputs
								scl.OutOfElem();
							}
							if ( bIsHaveGseOrSmvOrInputs )
								break;
						}
					}
				}
				// �ص�LN0
				scl.OutOfElem();
			}
		}
		// �ص�LDevice
		scl.OutOfElem();
	}
	if ( bIsHaveGseOrSmvOrInputs )
		return true;
	else
		return false;
}

///
/// @brief
///     �ֽ�ExtRef��IntAddr
///
/// @param[in]
///     intAddr - ExtRef��IntAddr
/// @param[in]
///     ldInst  - ����LDevice������
/// @param[in]
///     lnName  - ����LNode������
/// @param[in]
///     doName  - ����DOI������
/// @param[in]
///     daName  - ����DAI������
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
