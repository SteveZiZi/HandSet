/// @file
///     sclcachewithmap.cpp
///
/// @brief
///     SCL�ļ��������ݻ�����
///
/// @note
///     ʹ����STL�е�Map���ṩ��ӡ���ѯ��������ݵĽӿ�
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.1.24
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.02.01     |1.0.1.201  |����FCDA��Ϣ���Բ�ͬ�ط�����AddFCDAIntoDatSet���3��        |
///    |2013.02.18     |1.0.3.218  |��Ӷ�GOOSE�Ͳ���ֵ����������ߵ���Ϣ��INPUT����Ӽ���ȡ�ӿ�|
///    |2013.05.22     |1.0.4.522  |��ӶԸ���Ԫ�ص������Ͱ�˳��Ž��������Ļ�ȡ��Ϣ�ӿ�        |
///    |2013.06.07     |1.0.4.607  |��Ӷ�SCL�еĸ���Ԫ����Ϣ�����ýӿ�                         |
///    |2013.07.25     |1.0.5.725  |�����Ƴ�ָ��ͨ��������ͨ��ʱ����ͨ��֮���ͨ��˳���û�и��µ�Bug|
///    |2013.07.25     |1.0.5.725  |������ΪMap��Key�Ĺؼ���Ϣ�����ı�ʱ��ֻ���¶�Ӧ��valueֵȴû����Keyֵ��Bug|
///    |2013.08.09     |1.0.5.809  |����SMV��GOOSEͨ�Ų�����Ϣ�Ļ���ṹ�ı䣬�ʸı�����Ϣ���á���ȡ���ͷŵȽӿڵ�ʵ��|
///    |2013.08.15     |1.0.5.815  |�޸�BDA_MAP��keyΪBDAԪ����DaType�е�˳��ţ���0��ʼ��      |
///    |2013.08.15     |1.0.5.815  |����Ƴ�ָ��DaType�е�ĳһBDA�Ľӿ�                         |
///    |2013.08.22     |1.0.5.822  |��Ӷ�new�������쳣����                                     |
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
///     ���Cache
///
void SCLCacheWithMap::ClearCache()
{
	if ( m_SCLMap != NULL )
	{
		ClearSCLMap(m_SCLMap);


#ifdef WINCE
		// ѹ���ѣ�ʹϵͳ�����ͷŵ��ڴ棬����Ӱ��Ч��
		// Windows CEֻ֧���ڶ��з���̶���fixed���Ŀ飬��ʹ�ö��ڷ�����ͷ�һ��ʱ���������Ƭ��
		// �������Ѿ���յ�ʱ����Ȼ��ռ�ô����������ڴ�ҳ��
		// ��Ϊϵͳ�����ڶ����ڴ�ҳû����ȫ�ͷŵ�ʱ�������Щҳ��
		HANDLE heapHdl = GetProcessHeap();
		HeapCompact(heapHdl,HEAP_NO_SERIALIZE);
#endif
	}
}

///
/// @brief
///     ��Cache�����IED
///
/// @param[in]
///     iedAttrib - ��Ҫ��ӵ�IED��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddIED(const SCL_IED_ATTRIB &iedAttrib)
{
	IED_MAP::iterator it_IED;
	IED_INFO_CACHE *iedInfo;

	// IEDMap���������½�
	if ( m_SCLMap->iedMap == NULL )
	{
		m_SCLMap->iedMap = new IED_MAP;
		if ( m_SCLMap->iedMap == NULL )
			return false;
	}

	// ��IEDMap�����/����IED��Ϣ
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
///     ��Cache��ĳһIED���LDevice
///
/// @param[in]
///     iedName  - LDevice���ڵ�IED������
/// @param[in]
///     apAttrib - ��Ҫ��ӵ�LDevice�����ӵķ��ʵ��������Ϣ
/// @param[in]
///     ldAttrib - ��Ҫ��ӵ�LDevice��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddLDeviceIntoIED(const SCL_STR &iedName, const SCL_ACCESS_POINT_ATTRIB &apAttrib, const SCL_LDEVICE_ATTRIB &ldAttrib)
{
	IED_MAP::iterator it_IED;
	LDEVICE_MAP::iterator it_LD;
	LDEVICE_INFO_CACHE *ldInfo;

	// ��IEDMap������ָ��IED
	if ( !FindIEDInMap(iedName,it_IED) )
		return false;

	// LDeviceMap���������½�
	if ( it_IED->second->lDeviceMap == NULL )
	{
		it_IED->second->lDeviceMap = new LDEVICE_MAP;
		if ( it_IED->second->lDeviceMap == NULL )
			return false;
	}

	// ��LDeviceMap�����/����LDevice��Ϣ
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
///     ��Cache��ĳһIED��ĳһLDevice���LN0
///
/// @param[in]
///     iedName   - LN0���ڵ�IED������
/// @param[in]
///     ldInst    - LN0���ڵ�LDevice��ʵ����
/// @param[in]
///     ln0Attrib - ��Ҫ��ӵ�LN0��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddLN0IntoLD(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_LN0_ATTRIB &ln0Attrib)
{
	LDEVICE_MAP::iterator it_LD;
	LN0_INFO_CACHE *ln0Info;

	// ����ָ��LDevice
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	// ���/����LN0��Ϣ
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
///     ��Cache��ĳһIED��ĳһLDevice���LN
///
/// @param[in]
///     iedName  - LN0���ڵ�IED������
/// @param[in]
///     ldInst   - LN���ڵ�LDevice��ʵ����
/// @param[in]
///     lnAttrib - ��Ҫ��ӵ�LN��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddLNIntoLD(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_LN_ATTRIB &lnAttrib)
{
	LDEVICE_MAP::iterator it_LD;
	LN_MAP::iterator it_LN;
	LN_INFO_CACHE *lnInfo;
	SCL_STR lnName = lnAttrib.lnPrefix+lnAttrib.lnClass+lnAttrib.lnInst;

	// ����ָ��LDevice
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;


	// LNMap���������½�
	if ( it_LD->second->lnMap == NULL )
	{
		it_LD->second->lnMap = new LN_MAP;
		if ( it_LD->second->lnMap == NULL )
			return false;
	}

	// ��LNMap�����/����LN��Ϣ
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
///     ��Cache��ĳһIED��ĳһLDevice��LN0���DataSet
///
/// @param[in]
///     iedName       - DataSet���ڵ�IED������
/// @param[in]
///     ldInst        - DataSet���ڵ�LDevice��ʵ����
/// @param[in]
///     datasetAttrib - ��Ҫ��ӵ�DataSet��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddDataSetIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_DATASET_ATTRIB &datasetAttrib)
{
	LN0_INFO_CACHE *ln0Info;
	DATASET_INFO_CACHE *dataSetInfo;
	DATASET_MAP::iterator it_DataSet;

	//  ����ָ��LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0Info) )
		return false;

	// DataSetMap���������½�
	if ( ln0Info->datasetMap ==NULL )
	{
		ln0Info->datasetMap = new DATASET_MAP;
		if ( ln0Info->datasetMap ==NULL )
			return false;
	}

	// ��DataSetMap�����/����DataSet��Ϣ
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
///     ��Cache��ĳһIED��ĳһLDevice��LN0���SampledValueControl
///
/// @param[in]
///     iedName   - SampledValueControl���ڵ�IED������
/// @param[in]
///     ldInst    - SampledValueControl���ڵ�LDevice��ʵ����
/// @param[in]
///     smvAttrib - ��Ҫ��ӵ�SampledValueControl��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddSMVCtrlIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_SMVCTRL_ATTRIB &smvAttrib)
{
	LN0_INFO_CACHE *ln0Info;
	SMV_CTRL_MAP::iterator it_SMVCtrl;

	//  ����ָ��LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0Info) )
		return false;

	// SMVCtrlMap���������½�
	if ( ln0Info->smvCtrlMap == NULL )
	{
		ln0Info->smvCtrlMap = new SMV_CTRL_MAP;
		if ( ln0Info->smvCtrlMap == NULL )
			return false;
	}

	// ���/����SMV���ƿ���Ϣ
	if ( FindSMVCtrlInLN0(ln0Info,smvAttrib.cbName,it_SMVCtrl) )
		it_SMVCtrl->second = smvAttrib;
	else
		ln0Info->smvCtrlMap->insert(SMV_CTRL_MAP::value_type(smvAttrib.cbName,smvAttrib));

	return true;
}

///
/// @brief
///     ��Cache��ĳһIED��ĳһLDevice��LN0���GSEControl
///
/// @param[in]
///     iedName   - GSEControl���ڵ�IED������
/// @param[in]
///     ldInst    - GSEControl���ڵ�LDevice��ʵ����
/// @param[in]
///     gseAttrib - ��Ҫ��ӵ�GSEControl��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddGSECtrlIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_GSECTRL_ATTRIB &gseAttrib)
{
	LN0_INFO_CACHE *ln0Info;
	GSE_CTRL_MAP::iterator it_GSECtrl;

	//  ����ָ��LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0Info) )
		return false;

	// GSECtrlMap���������½�
	if ( ln0Info->gseCtrlMap == NULL )
	{
		ln0Info->gseCtrlMap = new GSE_CTRL_MAP;
		if ( ln0Info->gseCtrlMap == NULL )
			return false;
	}

	// ���/����GSE���ƿ���Ϣ
	if ( FindGSECtrlInLN0(ln0Info,gseAttrib.cbName,it_GSECtrl) )
		it_GSECtrl->second = gseAttrib;
	else
		ln0Info->gseCtrlMap->insert(GSE_CTRL_MAP::value_type(gseAttrib.cbName,gseAttrib));

	return true;
}

///
/// @brief
///     ��Cache��ĳһIED��ĳһLDevice��LN0���DOI
///
/// @param[in]
///     iedName   - DOI���ڵ�IED������
/// @param[in]
///     ldInst    - DOI���ڵ�LDevice��ʵ����
/// @param[in]
///     doiAttrib - ��Ҫ��ӵ�DOI��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddDOIIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_DOI_ATTRIB &doiAttrib)
{
	LN0_INFO_CACHE *ln0Info;
	DOI_MAP::iterator it_DOI;

	//  ����ָ��LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0Info) )
		return false;

	// DOIMap���������½�
	if ( ln0Info->doiMap == NULL )
	{
		ln0Info->doiMap = new DOI_MAP;
		if ( ln0Info->doiMap == NULL )
			return false;
	}

	// ���/����DOI��Ϣ
	if ( FindDOIInLN0(ln0Info,doiAttrib.doName,it_DOI) )
		it_DOI->second = doiAttrib;
	else
		ln0Info->doiMap->insert(DOI_MAP::value_type(doiAttrib.doName,doiAttrib));

	return true;
}

///
/// @brief
///     ��Cache��ĳһIED��ĳһLDevice��LN���DOI
///
/// @param[in]
///     iedName   - DOI���ڵ�IED������
/// @param[in]
///     ldInst    - DOI���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName    - DOI���ڵ�LN������
/// @param[in]
///     doiAttrib - ��Ҫ��ӵ�DOI��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddDOIIntoLN(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_DOI_ATTRIB &doiAttrib)
{
	LN_MAP::iterator it_LN;
	DOI_MAP::iterator it_DOI;

	//  ����ָ��LN
	if ( !FindLNInMap(iedName,ldInst,lnName,it_LN) )
		return false;

	// DOIMap���������½�
	if ( it_LN->second->doiMap == NULL )
	{
		it_LN->second->doiMap = new DOI_MAP;
		if ( it_LN->second->doiMap == NULL )
			return false;
	}

	// ���/����DOI��Ϣ
	if ( FindDOIInLN(it_LN,doiAttrib.doName,it_DOI) )
		it_DOI->second = doiAttrib;
	else
		it_LN->second->doiMap->insert(DOI_MAP::value_type(doiAttrib.doName,doiAttrib));

	return true;
}

///
/// @brief
///     ��Cache��ĳһIED��ĳһLDevice��LN0���ExtRef
///
/// @param[in]
///     iedName      - DOI���ڵ�IED������
/// @param[in]
///     ldInst       - DOI���ڵ�LDevice��ʵ����
/// @param[in]
///     extRefAttrib - ��Ҫ��ӵ�ExtRef��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddExtRefIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_EXTREF_ATTRIB &extRefAttrib)
{
	LN0_INFO_CACHE *ln0Info;
	EXTREF_MAP::iterator it_ExtRef;

	//  ����ָ��LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0Info) )
		return false;

	// DOIMap���������½�
	if ( ln0Info->extRefMap == NULL )
	{
		ln0Info->extRefMap = new EXTREF_MAP;
		if ( ln0Info->extRefMap == NULL )
			return false;
	}

	// ���/����DOI��Ϣ
	if ( FindExtRefInLN0(ln0Info,extRefAttrib.idxSeq,it_ExtRef) )
		it_ExtRef->second = extRefAttrib;
	else
		ln0Info->extRefMap->insert(EXTREF_MAP::value_type(ln0Info->extRefMap->size(),extRefAttrib));

	return true;
}

///
/// @brief
///     ��Cache��ĳһIED��ĳһLDevice��LN���ExtRef
///
/// @param[in]
///     iedName      - ExtRef���ڵ�IED������
/// @param[in]
///     ldInst       - ExtRef���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName       - ExtRef���ڵ�LN������
/// @param[in]
///     extRefAttrib - ��Ҫ��ӵ�ExtRef��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddExtRefIntoLN(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_EXTREF_ATTRIB &extRefAttrib)
{
	LN_MAP::iterator it_LN;
	EXTREF_MAP::iterator it_ExtRef;

	//  ����ָ��LN
	if ( !FindLNInMap(iedName,ldInst,lnName,it_LN) )
		return false;

	// DOIMap���������½�
	if ( it_LN->second->extRefMap == NULL )
	{
		it_LN->second->extRefMap = new EXTREF_MAP;
		if ( it_LN->second->extRefMap == NULL )
			return false;
	}

	// ���/����DOI��Ϣ
	if ( FindExtRefInLN(it_LN,extRefAttrib.idxSeq,it_ExtRef) )
		it_ExtRef->second = extRefAttrib;
	else
		it_LN->second->extRefMap->insert(EXTREF_MAP::value_type(it_LN->second->extRefMap->size(),extRefAttrib));

	return true;
}

///
/// @brief
///     ��Cache��ĳһIED��ĳһLDevice��ĳһLNode��ĳһDataSet���FCDA
///
/// @param[in]
///     iedName     - FCDA���ڵ�IED������
/// @param[in]
///     ldInst      - FCDA���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName      - FCDA���ڵ�LNode�����ƣ�LN0�߼��ڵ�����ƹ̶�Ϊ��LLN0��
/// @param[in]
///     dataSetName - FCDA���ڵ�DataSet������
/// @param[in]
///     fcdaAttrib  - FCDA��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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

	// ����ָ��DataSet
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

	// FCDAMap���������½�
	if ( it_DataSet->second->fcdaMap == NULL )
	{
		it_DataSet->second->fcdaMap = new FCDA_MAP;
		if ( it_DataSet->second->fcdaMap == NULL )
			return false;
	}

	// ��FCDAMap�����/����FCDA��Ϣ
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
///     ��Cache�����ĳһSMV���ƿ���������ͨ�Ų�����Ϣ
///
/// @param[in]
///     conntAPAttrib - ͨ�Ų����������Ŀ��ƿ����ڵķ��ʵ����Ϣ
/// @param[in]
///     smvAttrib     - ͨ�Ų�����Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddSMVCommn(const SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, const SCL_SMV_ATTRIB &smvAttrib)
{
	SMV_COMMN_MAP::iterator it_SMV;
	SMV_COMMN_INFO_CACHE *info;
	unsigned int appID;

	SCL_SSCANF(smvAttrib.appID.c_str(),_T("%x"),&appID);
	// SMVCommnMap���������½�
	if ( m_SCLMap->smvCommnMap == NULL )
	{
		m_SCLMap->smvCommnMap = new SMV_COMMN_MAP;
		if ( m_SCLMap->smvCommnMap == NULL )
			return false;
	}

	// ��SMVCommnMap�����/����SMVCommn��Ϣ
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
///     ��Cache�����ĳһGSE���ƿ���������ͨ�Ų�����Ϣ
///
/// @param[in]
///     conntAPAttrib - ͨ�Ų����������Ŀ��ƿ����ڵķ��ʵ����Ϣ
/// @param[in]
///     gseAttrib     - ͨ�Ų�����Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddGSECommn(const SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, const SCL_GSE_ATTRIB &gseAttrib)
{
	GSE_COMMN_MAP::iterator it_GSE;
	GSE_COMMN_INFO_CACHE *info;
	unsigned int appID;

	SCL_SSCANF(gseAttrib.appID.c_str(),_T("%x"),&appID);

	// GSECommnMap���������½�
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
///     ��Cache�����ĳһLNType��Ϣ
///
/// @param[in]
///     lnTypeAttrib - LNType��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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

	// ���/����LNType��Ϣ
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
///     ��Cache��ĳһLNType���DO��Ϣ
///
/// @param[in]
///     lnTypeID - LNType��ID
/// @param[in]
///     doAttrib - DO��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddDOIntoLNType(const SCL_STR &lnTypeID, const SCL_DOINLNTYPE_ATTRIB &doAttrib)
{
	LNTYPE_MAP::iterator it_LNType;
	DO_MAP::iterator it_DO;

	//  ����ָ��LNType
	if ( !FindLNTypeInMap(lnTypeID,it_LNType) )
		return false;

	// DOMap���������½�
	if ( it_LNType->second->doMap == NULL )
	{
		it_LNType->second->doMap = new DO_MAP;
		if ( it_LNType->second->doMap == NULL )
			return false;
	}

	// ���/����DO��Ϣ
	if ( FindDOInLNType(it_LNType,doAttrib.name,it_DO) )
		it_DO->second = doAttrib;
	else
		it_LNType->second->doMap->insert(DO_MAP::value_type(doAttrib.name,doAttrib));

	return true;
}

///
/// @brief
///     ��Cache�����ĳһDOType��Ϣ
///
/// @param[in]
///     doTypeAttrib - DOType��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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

	// ���/����DOType��Ϣ
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
///     ��Cache��ĳһDOType���DA��Ϣ
///
/// @param[in]
///     doTypeID - DOType��ID
/// @param[in]
///     daAttrib - DA��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddDAIntoDOType(const SCL_STR &doTypeID, const SCL_DAINDOTYPE_ATTRIB &daAttrib)
{
	DOTYPE_MAP::iterator it_DOType;
	DA_MAP::iterator it_DA;

	// ����ָ��DOType
	if ( !FindDOTypeInMap(doTypeID,it_DOType) )
		return false;

	if ( it_DOType->second->daMap == NULL )
	{
		it_DOType->second->daMap = new DA_MAP;
		if ( it_DOType->second->daMap == NULL )
			return false;
	}

	// ���/����DA��Ϣ
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
///     ��Cache�����ĳһDAType��Ϣ
///
/// @param[in]
///     daTypeAttrib - DAType��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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

	// ���/����DAType��Ϣ
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
///     ��Cache��ĳһDAType���BDA��Ϣ
///
/// @param[in]
///     daTypeID  - DAType��ID
/// @param[in]
///     bdaAttrib - BDA��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::AddBDAIntoDAType(const SCL_STR &daTypeID, const SCL_BDAINDATYPE_ATTRIB &bdaAttrib)
{
	DATYPE_MAP::iterator it_DAType;
	BDA_MAP::iterator it_BDA;

	// ����ָ��DAType
	if ( !FindDATypeInMap(daTypeID,it_DAType) )
		return false;

	if ( it_DAType->second->bdaMap == NULL )
	{
		it_DAType->second->bdaMap = new BDA_MAP;
		if ( it_DAType->second->bdaMap == NULL )
			return false;
	}

	// ���/����BDA��Ϣ
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
///     ��ȡCache�е�IED����
///
/// @return
///     Cache�е�IED����
///
int  SCLCacheWithMap::GetIEDNum()
{
	if ( m_SCLMap == NULL || m_SCLMap->iedMap == NULL )
		return 0;
	return (int)m_SCLMap->iedMap->size();
}

///
/// @brief
///     ����Cache�е�IED��Ϣ
///
/// @param[in]
///     iedName   - IED������
/// @param[in]
///     iedAttrib - IED��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetIEDAttrib(const SCL_STR &iedName, const SCL_IED_ATTRIB &iedAttrib)
{
	// ��IEDMap������ָ��IED
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
///     ����Cache��ĳһ˳��ŵ�IED��Ϣ
///
/// @param[in]
///     idxSeq    - IED��˳��ţ���0��ʼ
/// @param[in]
///     iedAttrib - IED��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
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

			// ��ΪMap Key��iedName������ֱ�Ӹ���������Ϣ���������滻
			// ע�⣬�滻��IED��Map�е�˳����ܻ�仯��Map�ڲ���Key�Ĵ�С����
			if ( it_IED->second->iedAttrib.iedName == iedAttrib.iedName )
			{
				it_IED->second->iedAttrib = iedAttrib;
			}
			else
			{
				// �ж������滻��IED�Ƿ��Ѵ��ڣ�IEDΨһ��
				IED_MAP::iterator it_NewIED;
				if ( FindIEDInMap(iedAttrib.iedName,it_NewIED) )
					return false;
				// �����IED
				if ( !AddIED(iedAttrib) )
					return false;
				if ( !FindIEDInMap(iedAttrib.iedName,it_NewIED) )
					return false;
				it_NewIED->second->lDeviceMap = it_IED->second->lDeviceMap;
				it_IED->second->lDeviceMap    = NULL;
				// �Ƴ���IED
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
///     ��Cache�л�ȡIED��Ϣ
///
/// @param[in]
///     iedName   - IED������
/// @param[in]
///     iedAttrib - ������Ҫ��ȡ��IED��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetIEDAttrib(const SCL_STR &iedName, SCL_IED_ATTRIB &iedAttrib)
{
	// ��IEDMap������ָ��IED
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
///     ��Cache�л�ȡĳһ˳��ŵ�IED��Ϣ
///
/// @param[in]
///     idxSeq    - IED��˳��ţ���0��ʼ
/// @param[in]
///     iedAttrib - ������Ҫ��ȡ��IED��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��ȡCache�е�ĳһIED�е�LDevice������
///
/// @param[in]
///     iedName  - LDevice���ڵ�IED������
///
/// @return
///     LDevice������
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
///     ����Cache��ĳһIED��ĳһʵ����ΪldInst��LDevice����Ϣ
///
/// @param[in]
///     iedName  - LDevice���ڵ�IED������
/// @param[in]
///     ldInst   - LDevice��ʵ����
/// @param[in]
///     apAttrib - LDevice�����ӵķ��ʵ��������Ϣ
/// @param[in]
///     ldAttrib - LDevice��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetLDeviceAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_ACCESS_POINT_ATTRIB &apAttrib, const SCL_LDEVICE_ATTRIB &ldAttrib)
{
	// ����ָ��LDevice
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
///     ����Cache��ĳһIED��ĳһ��LDevice����Ϣ
///
/// @param[in]
///     iedName  - LDevice���ڵ�IED������
/// @param[in]
///     idxSeq   - LDevice��˳��ţ���0��ʼ
/// @param[in]
///     apAttrib - LDevice�����ӵķ��ʵ��������Ϣ
/// @param[in]
///     ldAttrib - LDevice��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetLDeviceAttrib(const SCL_STR &iedName, const int idxSeq, const SCL_ACCESS_POINT_ATTRIB &apAttrib, const SCL_LDEVICE_ATTRIB &ldAttrib)
{
	IED_MAP::iterator it_IED;
	LDEVICE_MAP::iterator it_LD;
	int i = 0;

	// ����ָ��IED
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

			// ��ΪMap Key��ldInst������ֱ�Ӹ���������Ϣ���������滻
			// ע�⣬�滻��LDevice��Map�е�˳����ܻ�仯��Map�ڲ���Key�Ĵ�С����
			if ( it_LD->second->ldAttrib.ldInst == ldAttrib.ldInst )
			{
				it_LD->second->apAttrib = apAttrib;
				it_LD->second->ldAttrib = ldAttrib;
			}
			else
			{
				// �ж������滻��LDevice�Ƿ��Ѵ��ڣ�ldInst��IED��Ψһ��
				LDEVICE_MAP::iterator it_NewLD;
				if ( FindLDeviceInMap(iedName,ldAttrib.ldInst,it_NewLD) )
					return false;
				// �����LDevice
				if ( !AddLDeviceIntoIED(iedName,apAttrib,ldAttrib) )
					return false;
				if ( !FindLDeviceInMap(iedName,ldAttrib.ldInst,it_NewLD) )
					return false;
				it_NewLD->second->ln0InfoCache = it_LD->second->ln0InfoCache;
				it_NewLD->second->lnMap        = it_LD->second->lnMap;
				// �Ƴ���LDevice
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
///     ��Cache��ĳһIED�л�ȡĳһʵ����ΪldInst��LDevice����Ϣ
///
/// @param[in]
///     iedName  - LDevice���ڵ�IED������
/// @param[in]
///     ldInst   - LDevice��ʵ����
/// @param[in]
///     apAttrib - ������Ҫ��ȡ��LDevice�����ӵķ��ʵ��������Ϣ
/// @param[in]
///     ldAttrib - ������Ҫ��ȡ��LDevice��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetLDeviceAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, SCL_ACCESS_POINT_ATTRIB &apAttrib, SCL_LDEVICE_ATTRIB &ldAttrib)
{
	// ����ָ��LDevice
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
///     ��Cache��ĳһIED�л�ȡĳһ��LDevice����Ϣ
///
/// @param[in]
///     iedName  - LDevice���ڵ�IED������
/// @param[in]
///     idxSeq   - LDevice��˳��ţ���0��ʼ
/// @param[in]
///     apAttrib - ������Ҫ��ȡ��LDevice�����ӵķ��ʵ��������Ϣ
/// @param[in]
///     ldAttrib - ������Ҫ��ȡ��LDevice��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetLDeviceAttrib(const SCL_STR &iedName, const int idxSeq, SCL_ACCESS_POINT_ATTRIB &apAttrib, SCL_LDEVICE_ATTRIB &ldAttrib)
{
	IED_MAP::iterator it_IED;
	LDEVICE_MAP::iterator it_LD;
	int i = 0;

	// ����ָ��IED
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
///     ����Cache��ĳһIED��ĳһLDevice��LN0����Ϣ
///
/// @param[in]
///     iedName   - LN0���ڵ�IED������
/// @param[in]
///     ldInst    - LN0���ڵ�LDevice��ʵ����
/// @param[in]
///     ln0Attrib - LN0��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetLN0Attrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_LN0_ATTRIB &ln0Attrib)
{
	// ����ָ��LN0
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
///     ��Cache��ĳһIED��ĳһLDevice�л�ȡLN0����Ϣ
///
/// @param[in]
///     iedName   - LN0���ڵ�IED������
/// @param[in]
///     ldInst    - LN0���ڵ�LDevice��ʵ����
/// @param[in]
///     ln0Attrib - ������Ҫ��ȡ��LN0��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetLN0Attrib(const SCL_STR &iedName, const SCL_STR &ldInst, SCL_LN0_ATTRIB &ln0Attrib)
{
	// ����ָ��LN0
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
///     ��ȡCache��ĳһIED��ĳһLDevice��LN������
///
/// @param[in]
///     iedName   - LN���ڵ�IED������
/// @param[in]
///     ldInst    - LN���ڵ�LDevice��ʵ����
///
/// @return
///     LN������
///
int  SCLCacheWithMap::GetLNNum(const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// ����ָ��LDevice
	LDEVICE_MAP::iterator it_LD;
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return 0;

	if ( it_LD->second == NULL || it_LD->second->lnMap == NULL )
		return 0;

	return (int)it_LD->second->lnMap->size();
}

///
/// @brief
///     ����Cache��ĳһIED��ĳһLDevice�е�LN����Ϣ��������LLN0
///
/// @param[in]
///     iedName   - LN���ڵ�IED������
/// @param[in]
///     ldInst    - LN���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName    - LN������
/// @param[in]
///     lnAttrib  - LN��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_LN_ATTRIB &lnAttrib)
{
	// ����ָ��LN
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
///     ����Cache��ĳһIED��ĳһLDevice��ĳһ˳��ŵ�LN����Ϣ
///
/// @param[in]
///     iedName   - LN���ڵ�IED������
/// @param[in]
///     ldInst    - LN���ڵ�LDevice��ʵ����
/// @param[in]
///     idxSeq    - LN��˳��ţ���0��ʼ
/// @param[in]
///     lnAttrib  - LN��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_LN_ATTRIB &lnAttrib)
{
	// ����ָ��LDevice
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

			// ��ΪMap Key��lnName������ֱ�Ӹ���������Ϣ���������滻
			// ע�⣬�滻��LN��Map�е�˳����ܻ�仯��Map�ڲ���Key�Ĵ�С����
			if ( it_LN->second->lnAttrib.lnPrefix == lnAttrib.lnPrefix &&
				 it_LN->second->lnAttrib.lnClass  == lnAttrib.lnClass &&
				 it_LN->second->lnAttrib.lnInst   == lnAttrib.lnInst )
			{
				it_LN->second->lnAttrib = lnAttrib;
			}
			else
			{
				// �ж������滻��LN�Ƿ��Ѵ��ڣ�lnName��LDevice��Ψһ��
				LN_MAP::iterator it_NewLN;
				SCL_STR lnName = lnAttrib.lnPrefix + lnAttrib.lnClass + lnAttrib.lnInst;
				if ( FindLNInMap(iedName,ldInst,lnName,it_NewLN) )
					return false;
				// �����LN
				if ( !AddLNIntoLD(iedName,ldInst,lnAttrib) )
					return false;
				if ( !FindLNInMap(iedName,ldInst,lnName,it_NewLN) )
					return false;
				it_NewLN->second->doiMap    = it_LN->second->doiMap;
				it_NewLN->second->extRefMap = it_LN->second->extRefMap;
				// �Ƴ���LN
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
///     ��Cache��ĳһIED��ĳһLDevice�л�ȡLN����Ϣ��������LLN0
///
/// @param[in]
///     iedName   - LN���ڵ�IED������
/// @param[in]
///     ldInst    - LN���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName    - LN������
/// @param[in]
///     lnAttrib  - ������Ҫ��ȡ��LN��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, SCL_LN_ATTRIB &lnAttrib)
{
	// ����ָ��LN
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
///     ��Cache��ĳһIED��ĳһLDevice�л�ȡĳһ˳��ŵ�LN����Ϣ
///
/// @param[in]
///     iedName   - LN���ڵ�IED������
/// @param[in]
///     ldInst    - LN���ڵ�LDevice��ʵ����
/// @param[in]
///     idxSeq    - LN��˳��ţ���0��ʼ
/// @param[in]
///     lnAttrib  - ������Ҫ��ȡ��LN��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_LN_ATTRIB &lnAttrib)
{
	// ����ָ��LDevice
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
///     ��Cache��ĳһIEDĳһ��LDevice��LN0�е�DataSet������
///
/// @param[in]
///     iedName       - IED������
/// @param[in]
///     ldInst        - LDevice��ʵ����
///
/// @return
///     DataSet������
///
int  SCLCacheWithMap::GetDataSetNumInLN0(const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// ����ָ��LN0
	LN0_INFO_CACHE *ln0;
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return 0;

	if ( ln0 == NULL || ln0->datasetMap == NULL )
		return 0;

	return (int)ln0->datasetMap->size();
}

///
/// @brief
///     ����Cache��ĳһIEDĳһ��LDevice��LN0��ĳһ����ΪdataSetName��DataSet��Ϣ
///
/// @param[in]
///     iedName       - DataSet���ڵ�IED������
/// @param[in]
///     ldInst        - DataSet���ڵ�LDevice��ʵ����
/// @param[in]
///     dataSetName   - DataSet������
/// @param[in]
///     dataSetAttrib - DataSet��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, const SCL_DATASET_ATTRIB &dateSetAttrib)
{
	// ����ָ��DataSet
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
///     ����Cache��ĳһIEDĳһLDevice��LN0�е�idxSeq���DataSet��Ϣ
///
/// @param[in]
///     iedName       - DataSet���ڵ�IED������
/// @param[in]
///     ldInst        - DataSet���ڵ�LDevice��ʵ����
/// @param[in]
///     idxSeq        - DataSet��˳��ţ���0��ʼ
/// @param[in]
///     dataSetAttrib - DataSet��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_DATASET_ATTRIB &dateSetAttrib)
{
	LN0_INFO_CACHE *ln0;
	DATASET_MAP::iterator it_DataSet;
	int i = 0;

	// ����ָ��LN0
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

			// ��ΪMap Key��dataSetName������ֱ�Ӹ���������Ϣ���������滻
			// ע�⣬�滻��dataSet��Map�е�˳����ܻ�仯��Map�ڲ���Key�Ĵ�С����
			if ( it_DataSet->second->datSetAttrib.dataSetName == dateSetAttrib.dataSetName )
			{
				it_DataSet->second->datSetAttrib = dateSetAttrib;
			}
			else
			{
				// �ж������滻��LN�Ƿ��Ѵ��ڣ�dataSetName��LN0��Ψһ��
				DATASET_MAP::iterator it_NewDataSet;
				if ( FindDataSetInLN0(ln0,dateSetAttrib.dataSetName,it_NewDataSet) )
					return false;
				// �����dataSet
				if ( !AddDataSetIntoLN0(iedName,ldInst,dateSetAttrib) )
					return false;
				if ( !FindDataSetInLN0(ln0,dateSetAttrib.dataSetName,it_NewDataSet) )
					return false;
				it_NewDataSet->second->fcdaMap = it_DataSet->second->fcdaMap;
				it_DataSet->second->fcdaMap = NULL;
				// �Ƴ���dataSet
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
///     ��Cache��ĳһIEDĳһ��LDevice��LN0�л�ȡĳһ����ΪdataSetName��DataSet��Ϣ
///
/// @param[in]
///     iedName       - DataSet���ڵ�IED������
/// @param[in]
///     ldInst        - DataSet���ڵ�LDevice��ʵ����
/// @param[in]
///     dataSetName   - DataSet������
/// @param[in]
///     dataSetAttrib - ������Ҫ��ȡ��DataSet��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, SCL_DATASET_ATTRIB &dateSetAttrib)
{
	// ����ָ��DataSet
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
///     ��Cache��ĳһIEDĳһLDevice��LN0�л�ȡ��idxSeq���DataSet��Ϣ
///
/// @param[in]
///     iedName       - DataSet���ڵ�IED������
/// @param[in]
///     ldInst        - DataSet���ڵ�LDevice��ʵ����
/// @param[in]
///     idxSeq        - DataSet��˳��ţ���0��ʼ
/// @param[in]
///     dataSetAttrib - ������Ҫ��ȡ��DataSet��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_DATASET_ATTRIB &dateSetAttrib)
{
	LN0_INFO_CACHE *ln0;
	DATASET_MAP::iterator it_DataSet;
	int i = 0;

	// ����ָ��LN0
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
///     ��ȡCache��ĳһIED��ĳһLDevice��LN0�е�SampledValueControl������
///
/// @param[in]
///     iedName       - SampledValueControl���ڵ�IED������
/// @param[in]
///     ldInst        - SampledValueControl���ڵ�LDevice��ʵ����
///
/// @return
///     SampledValueControl������
///
int  SCLCacheWithMap::GetSMVCtrlNumInLN0(const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// ����ָ��LN0
	LN0_INFO_CACHE *ln0;
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return 0;

	if ( ln0 == NULL || ln0->smvCtrlMap == NULL )
		return 0;

	return (int)ln0->smvCtrlMap->size();
}

///
/// @brief
///     ����Cache��ĳһIED��ĳһLDevice��LN0�л�ȡĳһ˳��ŵ�SampledValueControl����Ϣ
///
/// @param[in]
///     iedName       - SampledValueControl���ڵ�IED������
/// @param[in]
///     ldInst        - SampledValueControl���ڵ�LDevice��ʵ����
/// @param[in]
///     idxSeq        - SampledValueControl��˳��ţ���0��ʼ
/// @param[in]
///     smvCtrlAttrib - SampledValueControl��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
	LN0_INFO_CACHE *ln0;
	SMV_CTRL_MAP::iterator it_SmvCtrl;
	int i = 0;

	// ����ָ��LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	if ( ln0 == NULL || ln0->smvCtrlMap == NULL )
		return false;

	for ( it_SmvCtrl = ln0->smvCtrlMap->begin(); it_SmvCtrl != ln0->smvCtrlMap->end(); it_SmvCtrl++)
	{
		if ( i == idxSeq )
		{
			// ��ΪMap Key��cbName������ֱ�Ӹ���������Ϣ���������滻
			// ע�⣬�滻��SmvCtrl��Map�е�˳����ܻ�仯��Map�ڲ���Key�Ĵ�С����
			if ( it_SmvCtrl->second.cbName == smvCtrlAttrib.cbName )
			{
				it_SmvCtrl->second = smvCtrlAttrib;
			}
			else
			{
				// �ж������滻��SmvCtrl�Ƿ��Ѵ��ڣ�cbName��LN0��Ψһ��
				SMV_CTRL_MAP::iterator it_NewSmvCtrl;
				if ( FindSMVCtrlInLN0(ln0,smvCtrlAttrib.cbName,it_NewSmvCtrl) )
					return false;
				// �����SmvCtrl
				if ( !AddSMVCtrlIntoLN0(iedName,ldInst,smvCtrlAttrib) )
					return false;
				// �Ƴ���SmvCtrl
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
///     ����Cache��ĳһIED��ĳһLDevice��LN0��ĳһSampledValueControl����Ϣ
///
/// @param[in]
///     iedName       - SampledValueControl���ڵ�IED������
/// @param[in]
///     ldInst        - SampledValueControl���ڵ�LDevice��ʵ����
/// @param[in]
///     cbName        - SampledValueControl������
/// @param[in]
///     smvCtrlAttrib - SampledValueControl��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, const SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
	// ����ָ��SMV���ƿ�
	SMV_CTRL_MAP::iterator it_SMVCtrl;
	if ( !FindSMVCtrlInMap(iedName,ldInst,cbName,it_SMVCtrl) )
		return false;

	it_SMVCtrl->second = smvCtrlAttrib;

	return true;
}

///
/// @brief
///     ����Cache��ĳһIED��ĳһLDevice��LN0�п���ĳһDataSet��SampledValueControl����Ϣ
///
/// @param[in]
///     iedName       - SampledValueControl���ڵ�IED������
/// @param[in]
///     ldInst        - SampledValueControl���ڵ�LDevice��ʵ����
/// @param[in]
///     dataSetName   - SampledValueControl�����Ƶ�DataSet������
/// @param[in]
///     smvCtrlAttrib - SampledValueControl��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetSMVCtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, const SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
	LDEVICE_MAP::iterator it_LD;
	SMV_CTRL_MAP *smvCtrlMap;
	SMV_CTRL_MAP::iterator it_SmvCtrl;

	// ����ָ��LDevice
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	if ( it_LD->second == NULL || it_LD->second->ln0InfoCache == NULL || it_LD->second->ln0InfoCache->smvCtrlMap == NULL )
		return false;

	smvCtrlMap = it_LD->second->ln0InfoCache->smvCtrlMap;

	// ��������ĳһ���ݼ��Ŀ��ƿ�
	for ( it_SmvCtrl = smvCtrlMap->begin(); it_SmvCtrl != smvCtrlMap->end(); it_SmvCtrl++)
	{
		if ( it_SmvCtrl->second.dataSet == dataSetName )
		{
			// ��ΪMap Key��cbName������ֱ�Ӹ���������Ϣ���������滻
			// ע�⣬�滻��SmvCtrl��Map�е�˳����ܻ�仯��Map�ڲ���Key�Ĵ�С����
			if ( it_SmvCtrl->second.cbName == smvCtrlAttrib.cbName )
			{
				it_SmvCtrl->second = smvCtrlAttrib;
			}
			else
			{
				// �ж������滻��SmvCtrl�Ƿ��Ѵ��ڣ�cbName��LN0��Ψһ��
				SMV_CTRL_MAP::iterator it_NewSmvCtrl;
				if ( FindSMVCtrlInLN0(it_LD->second->ln0InfoCache,smvCtrlAttrib.cbName,it_NewSmvCtrl) )
					return false;
				// �����SmvCtrl
				if ( !AddSMVCtrlIntoLN0(iedName,ldInst,smvCtrlAttrib) )
					return false;
				// �Ƴ���SmvCtrl
				it_LD->second->ln0InfoCache->smvCtrlMap->erase(it_SmvCtrl);
			}
			return true;
		}
	}
	return false;
}

///
/// @brief
///     ��Cache��ĳһIED��ĳһLDevice��LN0�л�ȡĳһ˳��ŵ�SampledValueControl����Ϣ
///
/// @param[in]
///     iedName       - SampledValueControl���ڵ�IED������
/// @param[in]
///     ldInst        - SampledValueControl���ڵ�LDevice��ʵ����
/// @param[in]
///     idxSeq        - SampledValueControl��˳��ţ���0��ʼ
/// @param[in]
///     smvCtrlAttrib - ������Ҫ��ȡ��SampledValueControl��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
	LN0_INFO_CACHE *ln0;
	SMV_CTRL_MAP::iterator it_SmvCtrl;
	int i = 0;

	// ����ָ��LN0
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
///     ��Cache��ĳһIED��ĳһLDevice��LN0�л�ȡĳһSampledValueControl����Ϣ
///
/// @param[in]
///     iedName       - SampledValueControl���ڵ�IED������
/// @param[in]
///     ldInst        - SampledValueControl���ڵ�LDevice��ʵ����
/// @param[in]
///     cbName        - SampledValueControl������
/// @param[in]
///     smvCtrlAttrib - ������Ҫ��ȡ��SampledValueControl��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
	// ����ָ��SMV���ƿ�
	SMV_CTRL_MAP::iterator it_SMVCtrl;
	if ( !FindSMVCtrlInMap(iedName,ldInst,cbName,it_SMVCtrl) )
		return false;

	smvCtrlAttrib = it_SMVCtrl->second;

	return true;
}

///
/// @brief
///     ��Cache��ĳһIED��ĳһLDevice��LN0�л�ȡ����ĳһDataSet��SampledValueControl����Ϣ
///
/// @param[in]
///     iedName       - SampledValueControl���ڵ�IED������
/// @param[in]
///     ldInst        - SampledValueControl���ڵ�LDevice��ʵ����
/// @param[in]
///     dataSetName   - SampledValueControl�����Ƶ�DataSet������
/// @param[in]
///     smvCtrlAttrib - ������Ҫ��ȡ��SampledValueControl��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetSMVCtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
	LDEVICE_MAP::iterator it_LD;
	SMV_CTRL_MAP *smvCtrlMap;
	SMV_CTRL_MAP::iterator it_SMVCtrl;

	// ����ָ��LDevice
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	if ( it_LD->second == NULL || it_LD->second->ln0InfoCache == NULL || it_LD->second->ln0InfoCache->smvCtrlMap == NULL )
		return false;

	smvCtrlMap = it_LD->second->ln0InfoCache->smvCtrlMap;

	// ��������ĳһ���ݼ��Ŀ��ƿ�
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
///     ��ȡCache��ĳһIED��ĳһLDevice��LN0�е�GSEControl������
///
/// @param[in]
///     iedName       - GSEControl���ڵ�IED������
/// @param[in]
///     ldInst        - GSEControl���ڵ�LDevice��ʵ����
///
/// @return
///     GSEControl������
///
int  SCLCacheWithMap::GetGSECtrlNumInLN0(const SCL_STR &iedName, const SCL_STR &ldInst)
{
	// ����ָ��LN0
	LN0_INFO_CACHE *ln0;
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return 0;

	if ( ln0 == NULL || ln0->gseCtrlMap == NULL )
		return 0;

	return (int)ln0->gseCtrlMap->size();
}

///
/// @brief
///     ����Cache��ĳһIED��ĳһLDevice��LN0�е�ĳһ˳��ŵ�GSEControl����Ϣ
///
/// @param[in]
///     iedName       - GSEControl���ڵ�IED������
/// @param[in]
///     ldInst        - GSEControl���ڵ�LDevice��ʵ����
/// @param[in]
///     idxSeq        - GSEControl��˳��ţ���0��ʼ
/// @param[in]
///     gseCtrlAttrib - GSEControl��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
	LN0_INFO_CACHE *ln0;
	GSE_CTRL_MAP::iterator it_GseCtrl;
	int i = 0;

	// ����ָ��LN0
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	if ( ln0 == NULL || ln0->gseCtrlMap == NULL )
		return false;

	for ( it_GseCtrl = ln0->gseCtrlMap->begin(); it_GseCtrl != ln0->gseCtrlMap->end(); it_GseCtrl++)
	{
		if ( i == idxSeq )
		{
			// ��ΪMap Key��cbName������ֱ�Ӹ���������Ϣ���������滻
			// ע�⣬�滻��GseCtrl��Map�е�˳����ܻ�仯��Map�ڲ���Key�Ĵ�С����
			if ( it_GseCtrl->second.cbName == gseCtrlAttrib.cbName )
			{
				it_GseCtrl->second = gseCtrlAttrib;
			}
			else
			{
				// �ж������滻��GseCtrl�Ƿ��Ѵ��ڣ�cbName��LN0��Ψһ��
				GSE_CTRL_MAP::iterator it_NewGseCtrl;
				if ( FindGSECtrlInLN0(ln0,gseCtrlAttrib.cbName,it_NewGseCtrl) )
					return false;
				// �����GseCtrl
				if ( !AddGSECtrlIntoLN0(iedName,ldInst,gseCtrlAttrib) )
					return false;
				// �Ƴ���GseCtrl
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
///     ����Cache��ĳһIED��ĳһLDevice��LN0��ĳһGSEControl����Ϣ
///
/// @param[in]
///     iedName       - GSEControl���ڵ�IED������
/// @param[in]
///     ldInst        - GSEControl���ڵ�LDevice��ʵ����
/// @param[in]
///     cbName        - GSEControl������
/// @param[in]
///     gseCtrlAttrib - GSEControl��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, const SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
	// ����ָ��GSE���ƿ�
	GSE_CTRL_MAP::iterator it_GSECtrl;
	if ( !FindGSECtrlInMap(iedName,ldInst,cbName,it_GSECtrl) )
		return false;

	it_GSECtrl->second = gseCtrlAttrib;

	return true;
}

///
/// @brief
///     ����Cache��ĳһIED��ĳһLDevice��LN0�п���ĳһDataSet��GSEControl����Ϣ
///
/// @param[in]
///     iedName       - GSEeControl���ڵ�IED������
/// @param[in]
///     ldInst        - GSEControl���ڵ�LDevice��ʵ����
/// @param[in]
///     dataSetName   - GSEControl�����Ƶ�DataSet������
/// @param[in]
///     gseCtrlAttrib - GSEControl��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetGSECtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, const SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
	LDEVICE_MAP::iterator it_LD;
	GSE_CTRL_MAP *gseCtrlMap;
	GSE_CTRL_MAP::iterator it_GseCtrl;

	// ����ָ��LDevice
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	if ( it_LD->second == NULL || it_LD->second->ln0InfoCache == NULL || it_LD->second->ln0InfoCache->gseCtrlMap == NULL )
		return false;

	gseCtrlMap = it_LD->second->ln0InfoCache->gseCtrlMap;

	// ��������ĳһ���ݼ��Ŀ��ƿ�
	for ( it_GseCtrl = gseCtrlMap->begin(); it_GseCtrl != gseCtrlMap->end(); it_GseCtrl++)
	{
		if ( it_GseCtrl->second.dataSet == dataSetName )
		{
			// ��ΪMap Key��cbName������ֱ�Ӹ���������Ϣ���������滻
			// ע�⣬�滻��GseCtrl��Map�е�˳����ܻ�仯��Map�ڲ���Key�Ĵ�С����
			if ( it_GseCtrl->second.cbName == gseCtrlAttrib.cbName )
			{
				it_GseCtrl->second = gseCtrlAttrib;
			}
			else
			{
				// �ж������滻��GseCtrl�Ƿ��Ѵ��ڣ�cbName��LN0��Ψһ��
				GSE_CTRL_MAP::iterator it_NewGseCtrl;
				if ( FindGSECtrlInLN0(it_LD->second->ln0InfoCache,gseCtrlAttrib.cbName,it_NewGseCtrl) )
					return false;
				// �����GseCtrl
				if ( !AddGSECtrlIntoLN0(iedName,ldInst,gseCtrlAttrib) )
					return false;
				// �Ƴ���GseCtrl
				it_LD->second->ln0InfoCache->gseCtrlMap->erase(it_GseCtrl);
			}
			return true;
		}
	}
	return false;
}

///
/// @brief
///     ��Cache��ĳһIED��ĳһLDevice��LN0�л�ȡĳһ˳��ŵ�GSEControl����Ϣ
///
/// @param[in]
///     iedName       - GSEControl���ڵ�IED������
/// @param[in]
///     ldInst        - GSEControl���ڵ�LDevice��ʵ����
/// @param[in]
///     idxSeq        - GSEControl��˳��ţ���0��ʼ
/// @param[in]
///     gseCtrlAttrib - ������Ҫ��ȡ��GSEControl��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
	LN0_INFO_CACHE *ln0;
	GSE_CTRL_MAP::iterator it_GseCtrl;
	int i = 0;

	// ����ָ��LN0
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
///     ��Cache��ĳһIED��ĳһLDevice��LN0�л�ȡĳһGSEControl����Ϣ
///
/// @param[in]
///     iedName       - GSEControl���ڵ�IED������
/// @param[in]
///     ldInst        - GSEControl���ڵ�LDevice��ʵ����
/// @param[in]
///     cbName        - GSEControl������
/// @param[in]
///     gseCtrlAttrib - ������Ҫ��ȡ��GSEControl��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
	// ����ָ��GSE���ƿ�
	GSE_CTRL_MAP::iterator it_GSECtrl;
	if ( !FindGSECtrlInMap(iedName,ldInst,cbName,it_GSECtrl) )
		return false;

	gseCtrlAttrib = it_GSECtrl->second;

	return true;
}

///
/// @brief
///     ��Cache��ĳһIED��ĳһLDevice��LN0�л�ȡ����ĳһDataSet��GSEControl����Ϣ
///
/// @param[in]
///     iedName       - GSEeControl���ڵ�IED������
/// @param[in]
///     ldInst        - GSEControl���ڵ�LDevice��ʵ����
/// @param[in]
///     dataSetName   - GSEControl�����Ƶ�DataSet������
/// @param[in]
///     gseCtrlAttrib - ������Ҫ��ȡ��GSEControl��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetGSECtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
	LDEVICE_MAP::iterator it_LD;
	GSE_CTRL_MAP *gseCtrlMap;
	GSE_CTRL_MAP::iterator it_GSECtrl;

	// ����ָ��LDevice
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	if ( it_LD->second == NULL || it_LD->second->ln0InfoCache == NULL || it_LD->second->ln0InfoCache->gseCtrlMap == NULL )
		return false;

	gseCtrlMap = it_LD->second->ln0InfoCache->gseCtrlMap;

	// ��������ĳһ���ݼ��Ŀ��ƿ�
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
///     ��ȡCache��ĳһIED��ĳһLDevice��ĳһLN��LN0�е�DOI������
///
/// @param[in]
///     iedName   - DOI���ڵ�IED������
/// @param[in]
///     ldInst    - DOI���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName    - DOI�����߼��ڵ������
///
/// @return
///     DOI������
///
int  SCLCacheWithMap::GetDOINumInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName)
{
	if ( lnName == _T("LLN0") )
	{
		// ����ָ��LN0
		LN0_INFO_CACHE *ln0;
		if ( !FindLN0InMap(iedName,ldInst,&ln0) )
			return 0;

		if ( ln0 == NULL || ln0->doiMap == NULL )
			return 0;
		return (int)ln0->doiMap->size();
	}
	else
	{
		// ����ָ��LN
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
///     ����Cache��ĳһIED��ĳһLDevice��ĳһLN��LN0�е�ĳһ˳��ŵ�DOI����Ϣ
///
/// @param[in]
///     iedName   - DOI���ڵ�IED������
/// @param[in]
///     ldInst    - DOI���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName    - DOI�����߼��ڵ������
/// @param[in]
///     idxSeq    - DOI��˳��ţ���0��ʼ
/// @param[in]
///     doiAttrib - DOI��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, 
											const SCL_STR &lnName, const int idxSeq, const SCL_DOI_ATTRIB &doiAttrib)
{
	DOI_MAP *doiMap = NULL;
	if ( lnName == _T("LLN0") )
	{
		// ����ָ��LN0
		LN0_INFO_CACHE *ln0;
		if ( !FindLN0InMap(iedName,ldInst,&ln0) )
			return false;

		if ( ln0 == NULL || ln0->doiMap == NULL )
			return false;
		doiMap = ln0->doiMap;
	}
	else
	{
		// ����ָ��LN
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
			// ��ΪMap Key��doName������ֱ�Ӹ���������Ϣ���������滻
			// ע�⣬�滻��DOI��Map�е�˳����ܻ�仯��Map�ڲ���Key�Ĵ�С����
			if ( it_DOI->second.doName == doiAttrib.doName )
			{
				it_DOI->second = doiAttrib;
			}
			else
			{
				// �ж������滻��DOI�Ƿ��Ѵ��ڣ�doName��LN0/LN��Ψһ��
				DOI_MAP::iterator it_NewDOI;
				it_NewDOI = doiMap->find(doiAttrib.doName);
				if ( it_NewDOI == doiMap->end() )
					return false;
				// �����DOI
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
				// �Ƴ���DOI
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
///     ����Cache��ĳһIED��ĳһLDevice��ĳһLN��LN0�е�ĳһDOI����Ϣ
///
/// @param[in]
///     iedName   - DOI���ڵ�IED������
/// @param[in]
///     ldInst    - DOI���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName    - DOI�����߼��ڵ������
/// @param[in]
///     doiName   - DOI������
/// @param[in]
///     doiAttrib - DOI��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, 
											const SCL_STR &lnName, const SCL_STR &doiName, const SCL_DOI_ATTRIB &doiAttrib)
{
	// ����ָ��DOI
	DOI_MAP::iterator it_DOI;
	if ( !FindDOIInMap(iedName,ldInst,lnName,doiName,it_DOI) )
		return false;

	it_DOI->second = doiAttrib;

	return true;
}

///
/// @brief
///     ��Cache��ĳһIED��ĳһLDevice��ĳһLN��LN0�л�ȡĳһ˳��ŵ�DOI����Ϣ
///
/// @param[in]
///     iedName   - DOI���ڵ�IED������
/// @param[in]
///     ldInst    - DOI���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName    - DOI�����߼��ڵ������
/// @param[in]
///     idxSeq    - DOI��˳��ţ���0��ʼ
/// @param[in]
///     doiAttrib - ������Ҫ��ȡ��DOI��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const int idxSeq, SCL_DOI_ATTRIB &doiAttrib)
{
	DOI_MAP *doiMap = NULL;
	if ( lnName == _T("LLN0") )
	{
		// ����ָ��LN0
		LN0_INFO_CACHE *ln0;
		if ( !FindLN0InMap(iedName,ldInst,&ln0) )
			return false;

		if ( ln0 == NULL || ln0->doiMap == NULL )
			return false;
		doiMap = ln0->doiMap;
	}
	else
	{
		// ����ָ��LN
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
///     ��Cache��ĳһIED��ĳһLDevice��ĳһLN��LN0�л�ȡĳһDOI����Ϣ
///
/// @param[in]
///     iedName   - DOI���ڵ�IED������
/// @param[in]
///     ldInst    - DOI���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName    - DOI�����߼��ڵ������
/// @param[in]
///     doiName   - DOI������
/// @param[in]
///     doiAttrib - ������Ҫ��ȡ��DOI��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &doiName, SCL_DOI_ATTRIB &doiAttrib)
{
	// ����ָ��DOI
	DOI_MAP::iterator it_DOI;
	if ( !FindDOIInMap(iedName,ldInst,lnName,doiName,it_DOI) )
		return false;

	doiAttrib = it_DOI->second;

	return true;
}

///
/// @brief
///     ��Cache��ĳһIED��ȫ��LDevice��LN0��ExtRef������
///
/// @param[in]
///     iedName      - IED������
///
/// @return
///     ExtRef���������ɹ���  -1��ʧ��
///
int SCLCacheWithMap::GetInputsExtRefNumInIED(const SCL_STR &iedName)
{
	IED_MAP::iterator it_IED;
	LDEVICE_MAP::iterator it_LD;
	int extRefNum = 0;

	// ����ָ��IED
	if ( !FindIEDInMap(iedName,it_IED) )
		return -1;

	if ( it_IED->second == NULL || it_IED->second->lDeviceMap == NULL )
		return false;

	for ( it_LD = it_IED->second->lDeviceMap->begin(); it_LD != it_IED->second->lDeviceMap->end(); it_LD++ )
	{
		if ( it_LD->second == NULL )
			continue;
		// ��ȡLN0�е�ExtRef����
		if ( it_LD->second->ln0InfoCache != NULL &&
			it_LD->second->ln0InfoCache->extRefMap != NULL )
		{
			extRefNum += (int)it_LD->second->ln0InfoCache->extRefMap->size();
		}
		// ��ȡLN�е�ExtRef����
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
///     ��Cache��ĳһIED��ȫ��LDevice��LN0��ExtRef���ⲿ�ź�������Ϣ��
///
/// @param[in]
///     iedName           - IED������
/// @param[in]
///     extRefAttribArray - ����ExtRef���ⲿ�ź�������Ϣ��������
/// @param[in]
///     arrSize           - ����Ĵ�С
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetAllInputsExtRefInIED(const SCL_STR &iedName, SCL_EXTREF_ATTRIB *extRefAttribArray, const int arrSize)
{
	IED_MAP::iterator it_IED;
	LDEVICE_MAP::iterator it_LD;
	EXTREF_MAP::iterator it_ExtRef;
	int i = 0;

	// ����ָ��IED
	if ( !FindIEDInMap(iedName,it_IED) )
		return false;

	if ( it_IED->second == NULL || it_IED->second->lDeviceMap == NULL )
		return false;

	for ( it_LD = it_IED->second->lDeviceMap->begin(); it_LD != it_IED->second->lDeviceMap->end(); it_LD++ )
	{
		if ( it_LD->second == NULL )
			continue;

		// ��ȡLN0�е�ExtRef
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
		// ��ȡLN�е�ExtRef
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
///     ��ȡCache��ĳһIED��ĳһLDevice��ĳһLN��LN0�е�ExtRef������
///
/// @param[in]
///     iedName      - ExtRef���ڵ�IED������
/// @param[in]
///     ldInst       - ExtRef���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName       - ExtRef�����߼��ڵ������
///
/// @return
///     ExtRef������
///
int  SCLCacheWithMap::GetExtRefNumInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName)
{
	if ( lnName == _T("LLN0") )
	{
		// ����ָ��LN0
		LN0_INFO_CACHE *ln0;
		if ( !FindLN0InMap(iedName,ldInst,&ln0) )
			return 0;

		if ( ln0 == NULL || ln0->extRefMap == NULL )
			return 0;
		return (int)ln0->extRefMap->size();
	}
	else
	{
		// ����ָ��LN
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
///     ����Cache��ĳһIED��ĳһLDevice��ĳһLN��LN0�е�ĳһ˳��ŵ�ExtRef����Ϣ
///
/// @param[in]
///     iedName      - ExtRef���ڵ�IED������
/// @param[in]
///     ldInst       - ExtRef���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName       - ExtRef�����߼��ڵ������
/// @param[in]
///     idxSeq       - ExtRef��Map�е�˳���
/// @param[in]
///     extRefAttrib - ExtRef��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetExtRefAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
									  const int &idxSeq, const SCL_EXTREF_ATTRIB &extRefAttrib)
{
	if ( idxSeq != extRefAttrib.idxSeq )
		return false;

	// ����ָ��ExtRef
	EXTREF_MAP::iterator it_ExtRef;
	if ( !FindExtRefInMap(iedName,ldInst,lnName,idxSeq,it_ExtRef) )
		return false;

	it_ExtRef->second = extRefAttrib;

	return true;
}

///
/// @brief
///     ��Cache��ĳһIED��ĳһLDevice��ĳһLN��LN0�л�ȡĳһ˳��ŵ�ExtRef����Ϣ
///
/// @param[in]
///     iedName      - ExtRef���ڵ�IED������
/// @param[in]
///     ldInst       - ExtRef���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName       - ExtRef�����߼��ڵ������
/// @param[in]
///     idxSeq       - ExtRef��Map�е�˳���
/// @param[in]
///     extRefAttrib - ������Ҫ��ȡ��ExtRef��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetExtRefAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
									  const int &idxSeq, SCL_EXTREF_ATTRIB &extRefAttrib)
{
	// ����ָ��ExtRef
	EXTREF_MAP::iterator it_ExtRef;
	if ( !FindExtRefInMap(iedName,ldInst,lnName,idxSeq,it_ExtRef) )
		return false;

	extRefAttrib = it_ExtRef->second;

	return true;
}

///
/// @brief
///     ����Cache��ĳһIED��ĳһLDevice��ĳһLNode��ĳһDataSet�е�ĳһ˳��ŵ�FCDA����Ϣ
///
/// @param[in]
///     iedName     - FCDA���ڵ�IED������
/// @param[in]
///     ldInst      - FCDA���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName      - FCDA���ڵ�LNode������,LN0Ĭ��Ϊ��LLN0��
/// @param[in]
///     dataSetName - FCDA���ڵ�DataSet
/// @param[in]
///     idxSeq      - FCDA��Map��˳��ţ���0��ʼ������SCL�е�˳��һ��
/// @param[in]
///     fcdaAttrib  - FCDA��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetFCDAAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &datSetName,
									const int idxSeq, const SCL_FCDA_ATTRIB &fcdaAttrib)
{
	if ( idxSeq != fcdaAttrib.idxSeq )
		return false;

	// ����ָ��FCDA
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
///     ��Cache��ĳһIED��ĳһLDevice��ĳһLNode��ĳһDataSet�л�ȡĳһ˳��ŵ�FCDA����Ϣ
///
/// @param[in]
///     iedName     - FCDA���ڵ�IED������
/// @param[in]
///     ldInst      - FCDA���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName      - FCDA���ڵ�LNode������,LN0Ĭ��Ϊ��LLN0��
/// @param[in]
///     dataSetName - FCDA���ڵ�DataSet
/// @param[in]
///     idxSeq      - FCDA��Map��˳��ţ���0��ʼ������SCL�е�˳��һ��
/// @param[in]
///     fcdaAttrib  - ������Ҫ��ȡ��FCDA��������Ϣ 
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetFCDAAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &datSetName,
									const int idxSeq, SCL_FCDA_ATTRIB &fcdaAttrib)
{
	// ����ָ��FCDA
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
///     ��Cache��ĳһIED��ĳһLDevice��ĳһLNode��ĳһDataSet�л�ȡĳһFCDA����DataSet�е�˳���
///
/// @param[in]
///     iedName     - FCDA���ڵ�IED������
/// @param[in]
///     ldInst      - FCDA���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName      - FCDA���ڵ�LNode������,LN0Ĭ��Ϊ��LLN0��
/// @param[in]
///     dataSetName - FCDA���ڵ�DataSet
/// @param[in]
///     fcdaAttrib  - FCDA��������Ϣ 
///
/// @return
///     SCL�ļ���FCDA����DataSet�е�˳��ţ��ɹ���  false��-1
///
int SCLCacheWithMap::GetFCDAIdxSeqInDataSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &datSetName, const SCL_FCDA_ATTRIB &fcdaAttrib)
{
	DATASET_MAP::iterator it_DataSet;
	FCDA_MAP::iterator it_FCDA;
	int i = 0;

	// ����ָ��DataSet
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
///     ��Cache��ĳһIED��ĳһLDevice��ĳһLNode�л�ȡĳһFCDA����DataSet�е�˳���
///
/// @param[in]
///     iedName     - FCDA���ڵ�IED������
/// @param[in]
///     ldInst      - FCDA���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName      - FCDA���ڵ�LNode������,LN0Ĭ��Ϊ��LLN0��
/// @param[in]
///     fcdaAttrib  - FCDA��������Ϣ 
///
/// @return
///     SCL�ļ���FCDA����DataSet�е�˳��ţ��ɹ���  false��-1
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
///     ��ȡCache��LNType������
///
/// @return
///     LNType������
///
int  SCLCacheWithMap::GetLNTypeNum()
{
	if ( m_SCLMap == NULL || m_SCLMap->lnTypeMap == NULL )
		return 0;
	return (int)m_SCLMap->lnTypeMap->size();
}

///
/// @brief
///     ����Cache�е�ĳһ˳��ŵ�LNType����Ϣ
///
/// @param[in]
///     idxSeq       - LNType��˳���
/// @param[in]
///     lnTypeAttrib - LNType��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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

			// ��ΪMap Key��lnType��ID������ֱ�Ӹ���������Ϣ���������滻
			// ע�⣬�滻��lnType��Map�е�˳����ܻ�仯��Map�ڲ���Key�Ĵ�С����
			if ( it_LnType->second->lnTypeAttrib.id == lnTypeAttrib.id )
			{
				it_LnType->second->lnTypeAttrib = lnTypeAttrib;
			}
			else
			{
				// �ж������滻��lnType�Ƿ��Ѵ��ڣ�ID��Map��Ψһ��
				LNTYPE_MAP::iterator it_NewLnType;
				if ( FindLNTypeInMap(lnTypeAttrib.id,it_NewLnType) )
					return false;
				// �����lnType
				if ( !AddLNType(lnTypeAttrib) )
					return false;
				if ( !FindLNTypeInMap(lnTypeAttrib.id,it_NewLnType) )
					return false;
				it_NewLnType->second->doMap = it_LnType->second->doMap;
				// �Ƴ���lnType
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
///     ����Cache�е�ĳһID��LNType����Ϣ
///
/// @param[in]
///     lnTypeID     - LNType��ID
/// @param[in]
///     lnTypeAttrib - LNType��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��Cache�л�ȡĳһ˳��ŵ�LNType����Ϣ
///
/// @param[in]
///     idxSeq       - LNType��˳���
/// @param[in]
///     lnTypeAttrib - ������Ҫ��ȡ��LNType��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��Cache�л�ȡĳһID��LNType����Ϣ
///
/// @param[in]
///     lnTypeID     - LNType��ID
/// @param[in]
///     lnTypeAttrib - LNType��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��ȡCache�е�ĳһLNType�е�DO������
///
/// @param[in]
///     lnType     - DO����LNType��ID
///
/// @return
///     DO������
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
///     ����Cache��ĳһLNType�е�ĳһ˳��ŵ�DO����Ϣ
///
/// @param[in]
///     lnType     - DO����LNType��ID
/// @param[in]
///     idxSeq     - DO��˳��ţ���0��ʼ
/// @param[in]
///     doAttrib   - DO��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
			// ��ΪMap Key��doName������ֱ�Ӹ���������Ϣ���������滻
			// ע�⣬�滻��DO��Map�е�˳����ܻ�仯��Map�ڲ���Key�Ĵ�С����
			if ( it_DO->second.name == doAttrib.name )
			{
				it_DO->second = doAttrib;
			}
			else
			{
				// �ж������滻��DO�Ƿ��Ѵ��ڣ�doName��LNType��Ψһ��
				DO_MAP::iterator it_NewDO;
				if ( FindDOInLNType(it_LnType,doAttrib.name,it_NewDO) )
					return false;
				// �����DO
				if ( !AddDOIntoLNType(lnType,doAttrib) )
					return false;
				// �Ƴ���DO
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
///     ����Cache��ĳһLNType�е�ĳһDO����Ϣ
///
/// @param[in]
///     lnType     - DO����LNType��ID
/// @param[in]
///     doName     - DO������
/// @param[in]
///     doAttrib   - DO��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetDOAttrib(const SCL_STR &lnType, const SCL_STR &doName, const SCL_DOINLNTYPE_ATTRIB &doAttrib)
{
	DO_MAP::iterator it_DO;

	// ����ָ��DO
	if ( !FindDOInMap(lnType,doName,it_DO) )
		return false;

	it_DO->second = doAttrib;

	return true;
}

///
/// @brief
///     ��Cache��ĳһLNType�л�ȡĳһ˳��ŵ�DO����Ϣ
///
/// @param[in]
///     lnType     - DO����LNType��ID
/// @param[in]
///     idxSeq     - DO��˳��ţ���0��ʼ
/// @param[in]
///     doAttrib   - ������Ҫ��ȡ��DO��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��Cache��ĳһLNType�л�ȡĳһDO����Ϣ
///
/// @param[in]
///     lnType     - DO����LNType��ID
/// @param[in]
///     doName     - DO������
/// @param[in]
///     doAttrib   - ������Ҫ��ȡ��DO��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetDOAttrib(const SCL_STR &lnType, const SCL_STR &doName, SCL_DOINLNTYPE_ATTRIB &doAttrib)
{
	DO_MAP::iterator it_DO;

	// ����ָ��DO
	if ( !FindDOInMap(lnType,doName,it_DO) )
		return false;

	doAttrib = it_DO->second;

	return true;
}

///
/// @brief
///     ��ȡCache��DOType������
///
/// @return
///     DOType������
///
int  SCLCacheWithMap::GetDOTypeNum()
{
	if ( m_SCLMap == NULL || m_SCLMap->doTypeMap == NULL )
		return 0;
	return (int)m_SCLMap->doTypeMap->size();
}

///
/// @brief
///     ����Cache�е�ĳһ˳��ŵ�DOType����Ϣ
///
/// @param[in]
///     idxSeq       - DOType��˳���
/// @param[in]
///     doTypeAttrib - ������Ҫ��ȡ��DOType��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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

			// ��ΪMap Key��doType��ID������ֱ�Ӹ���������Ϣ���������滻
			// ע�⣬�滻��doType��Map�е�˳����ܻ�仯��Map�ڲ���Key�Ĵ�С����
			if ( it_DoType->second->doTypeAttrib.id == doTypeAttrib.id )
			{
				it_DoType->second->doTypeAttrib = doTypeAttrib;
			}
			else
			{
				// �ж������滻��doType�Ƿ��Ѵ��ڣ�ID��Map��Ψһ��
				DOTYPE_MAP::iterator it_NewDoType;
				if ( FindDOTypeInMap(doTypeAttrib.id,it_NewDoType) )
					return false;
				// �����doType
				if ( !AddDOType(doTypeAttrib) )
					return false;
				if ( !FindDOTypeInMap(doTypeAttrib.id,it_NewDoType) )
					return false;
				it_NewDoType->second->daMap = it_DoType->second->daMap;
				// �Ƴ���doType
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
///     ����Cache�е�ĳһID��DOType����Ϣ
///
/// @param[in]
///     doTypeID     - DOType��ID
/// @param[in]
///     doTypeAttrib - ������Ҫ��ȡ��DOType��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��Cache�л�ȡĳһ˳��ŵ�DOType����Ϣ
///
/// @param[in]
///     idxSeq       - DOType��˳���
/// @param[in]
///     doTypeAttrib - ������Ҫ��ȡ��DOType��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��Cache�л�ȡĳһID��DOType����Ϣ
///
/// @param[in]
///     doTypeID     - DOType��ID
/// @param[in]
///     doTypeAttrib - ������Ҫ��ȡ��DOType��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��ȡCache�е�ĳһDOType�е�DA������
///
/// @param[in]
///     doType     - DA����DOType��ID
///
/// @return
///     DA������
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
///     ����Cache��ĳһDOType�е�ĳһ˳��ŵ�DA����Ϣ
///
/// @param[in]
///     doType     - DA����DOType��ID
/// @param[in]
///     idxSeq     - DA��˳��ţ���0��ʼ
/// @param[in]
///     daAttrib   - DA��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ����Cache��ĳһDOType�е�ĳһDA����Ϣ
///
/// @param[in]
///     doType     - DA����DOType��ID
/// @param[in]
///     daName     - DA������
/// @param[in]
///     daAttrib   - DA��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetDAAttrib(const SCL_STR &doType, const SCL_STR &daName, const SCL_DAINDOTYPE_ATTRIB &daAttrib)
{
	DA_MAP::iterator it_DA;

	// ����ָ��DA
	if ( !FindDAInMap(doType,daName,it_DA) )
		return false;

	it_DA->second = daAttrib;

	return true;
}

///
/// @brief
///     ��Cache��ĳһDOType�л�ȡĳһ˳��ŵ�DA����Ϣ
///
/// @param[in]
///     doType     - DA����DOType��ID
/// @param[in]
///     idxSeq     - DA��˳��ţ���0��ʼ
/// @param[in]
///     daAttrib   - ������Ҫ��ȡ��DA��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��Cache��ĳһDOType�л�ȡĳһDA����Ϣ
///
/// @param[in]
///     doType     - DA����DOType��ID
/// @param[in]
///     daName     - DA������
/// @param[in]
///     daAttrib   - ������Ҫ��ȡ��DA��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetDAAttrib(const SCL_STR &doType, const SCL_STR &daName, SCL_DAINDOTYPE_ATTRIB &daAttrib)
{
	DA_MAP::iterator it_DA;

	// ����ָ��DA
	if ( !FindDAInMap(doType,daName,it_DA) )
		return false;

	daAttrib = it_DA->second;

	return true;
}

///
/// @brief
///     ��ȡCache��DAType������
///
/// @return
///     DAType������
///
int  SCLCacheWithMap::GetDATypeNum()
{
	if ( m_SCLMap == NULL || m_SCLMap->daTypeMap == NULL )
		return 0;
	return (int)m_SCLMap->daTypeMap->size();
}

///
/// @brief
///     ����Cache�е�ĳһ˳��ŵ�DAType����Ϣ
///
/// @param[in]
///     idxSeq       - DAType��˳���
/// @param[in]
///     daTypeAttrib - DAType��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
			// ��ΪMap Key��daType��ID������ֱ�Ӹ���������Ϣ���������滻
			// ע�⣬�滻��daType��Map�е�˳����ܻ�仯��Map�ڲ���Key�Ĵ�С����
			if ( it_DaType->second->daTypeAttrib.id == daTypeAttrib.id )
			{
				it_DaType->second->daTypeAttrib = daTypeAttrib;
			}
			else
			{
				// �ж������滻��daType�Ƿ��Ѵ��ڣ�ID��Map��Ψһ��
				DATYPE_MAP::iterator it_NewDaType;
				if ( FindDATypeInMap(daTypeAttrib.id,it_NewDaType) )
					return false;
				// �����daType
				if ( !AddDAType(daTypeAttrib) )
					return false;
				if ( !FindDATypeInMap(daTypeAttrib.id,it_NewDaType) )
					return false;
				it_NewDaType->second->bdaMap = it_DaType->second->bdaMap;
				// �Ƴ���daType
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
///     ����Cache�е�ĳһID��DAType����Ϣ
///
/// @param[in]
///     daTypeID     - DAType��ID
/// @param[in]
///     daTypeAttrib - DAType��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��Cache�л�ȡĳһ˳��ŵ�DAType����Ϣ
///
/// @param[in]
///     idxSeq       - DAType��˳���
/// @param[in]
///     daTypeAttrib - ������Ҫ��ȡ��DAType��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��Cache�л�ȡĳһID��DAType����Ϣ
///
/// @param[in]
///     daTypeID     - DAType��ID
/// @param[in]
///     daTypeAttrib - DAType��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��ȡCache�е�ĳһDAType�е�BDA������
///
/// @param[in]
///     daType     - BDA����DAType��ID
///
/// @return
///     BDA������
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
///     ����Cache��ĳһDAType�е�ĳһ˳��ŵ�BDA����Ϣ
///
/// @param[in]
///     daType     - BDA����DAType��ID
/// @param[in]
///     idxSeq     - BDA��˳��ţ���0��ʼ
/// @param[in]
///     bdaAttrib  - BDA��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ����Cache��ĳһDAType�е�ĳһBDA����Ϣ
///
/// @param[in]
///     daType     - BDA����DAType��ID
/// @param[in]
///     bdaName    - BDA������
/// @param[in]
///     bdaAttrib  - BDA��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::SetBDAAttrib(const SCL_STR &daType, const SCL_STR &bdaName, const SCL_BDAINDATYPE_ATTRIB &bdaAttrib)
{
	BDA_MAP::iterator it_BDA;

	// ����ָ��BDA
	if ( !FindBDAInMap(daType,bdaName,it_BDA) )
		return false;

	it_BDA->second = bdaAttrib;

	return true;
}

///
/// @brief
///     ��Cache��ĳһDAType�л�ȡĳһ˳��ŵ�BDA����Ϣ
///
/// @param[in]
///     daType     - BDA����DAType��ID
/// @param[in]
///     idxSeq     - BDA��˳��ţ���0��ʼ
/// @param[in]
///     bdaAttrib  - ������Ҫ��ȡ��BDA��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��Cache��ĳһDAType�л�ȡĳһBDA����Ϣ
///
/// @param[in]
///     daType     - BDA����DAType��ID
/// @param[in]
///     bdaName    - BDA������
/// @param[in]
///     bdaAttrib  - ������Ҫ��ȡ��BDA��������Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetBDAAttrib(const SCL_STR &daType, const SCL_STR &bdaName, SCL_BDAINDATYPE_ATTRIB &bdaAttrib)
{
	BDA_MAP::iterator it_BDA;

	// ����ָ��BDA
	if ( !FindBDAInMap(daType,bdaName,it_BDA) )
		return false;

	bdaAttrib = it_BDA->second;

	return true;
}

///
/// @brief
///     �ж�ĳAppID��SMV���ƿ��Ƿ����
///
/// @param[in]
///     appID - ���ƿ��AppIDֵ
///
/// @return
///     0 - ���ڣ� -1 - ������
///
int SCLCacheWithMap::IsSMVAppIDExisted(const unsigned short appID)
{
	// ����ָ��SMV
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
///     ��ȡCache��SampledValueControl����Ŀ
///
/// @return
///     SampledValueControl����Ŀ
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
///     ��ȡCache��ȫ����SampledValueControl��ͨ�Ų�����Ϣ
///
/// @param[in]
///     cnntAPAttribArray - ͨ�Ų����������Ŀ��ƿ����ڵķ��ʵ����Ϣ
/// @param[in]
///     smvAttribArray    - SampledValueControl��ͨ�Ų�����Ϣ
/// @param[in]
///    arraySize          - ����ͨ�Ų�����Ϣ�������Size
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ����Cache�е�ĳһ��iedName��ldInst��cbNameΪ������SampledValueControl��������ȫ��ͨ�Ų�����Ϣ
///
/// @param[in]
///     iedName       - SampledValueControl���ڵ�IED������
/// @param[in]
///     ldInst        - SampledValueControl���ڵ�LDevice��ʵ����
/// @param[in]
///     cbName        - SampledValueControl������
/// @param[in]
///     conntAPAttrib - SampledValueControl���ڵķ��ʵ����Ϣ
/// @param[in]
///     smvAddr       - SampledValueControl��ͨ�Ų�����Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
					// AppID���䣬��ֱ���滻
					pInfo->cnntAPAttrib = conntAPAttrib;
					pInfo->smvAttrib = smvAddr;
				}
				else
				{
					// AppID�ı䣬��ɾ���ɵģ����������
					if ( pPrevInfo == NULL )
						it_SMV->second = pInfo->pNextCache;
					else
						pPrevInfo->pNextCache = pInfo->pNextCache;
					// �Ƴ���SmvCommn
					if ( it_SMV->second == NULL )
					{
						m_SCLMap->smvCommnMap->erase(it_SMV);
					}
					delete pInfo;
					// �����SmvCommn
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
///     ��Cache�л�ȡĳһSampledValueControl��������ȫ��ͨ�Ų�����Ϣ
///     �����ڶ����ͬAPPID���򷵻ص�һ��
///
/// @param[in]
///     appID         - ͨ�Ų����е�appID��,ȫվΨһ
/// @param[in]
///     conntAPAttrib - ������Ҫ��ȡ��ͨ�Ų�����������SampledValueControl���ڵķ��ʵ����Ϣ
/// @param[in]
///     smvAddr       - ������Ҫ��ȡ��ͨ�Ų�����Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetSMVCommn(const unsigned short appID, SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, SCL_SMV_ATTRIB &smvAddr)
{
	// ����ָ��SMV
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
///     ��Cache�л�ȡĳһ��iedName��ldInst��cbNameΪ������SampledValueControl��������ȫ��ͨ�Ų�����Ϣ
///
/// @param[in]
///     iedName       - SampledValueControl���ڵ�IED������
/// @param[in]
///     ldInst        - SampledValueControl���ڵ�LDevice��ʵ����
/// @param[in]
///     cbName        - SampledValueControl������
/// @param[in]
///     conntAPAttrib - ������Ҫ��ȡ��ͨ�Ų�����������SampledValueControl���ڵķ��ʵ����Ϣ
/// @param[in]
///     smvAddr       - ������Ҫ��ȡ��ͨ�Ų�����Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     �ж�ĳAppID��GOOSE���ƿ��Ƿ����
///
/// @param[in]
///     appID - ���ƿ��AppIDֵ
///
/// @return
///     0 - ���ڣ� -1 - ������
///
int SCLCacheWithMap::IsGSEAppIDExisted(const unsigned short appID)
{
	// ����ָ��GSE
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
///     ��ȡCache��GSEControl����Ŀ
///
/// @return
///     GSEControl����Ŀ
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
///     ��ȡCache��ȫ����GSEControl��ͨ�Ų�����Ϣ
///
/// @param[in]
///     cnntAPAttribArray - ͨ�Ų����������Ŀ��ƿ����ڵķ��ʵ����Ϣ
/// @param[in]
///     gseAttribArray    - GSEControl��ͨ�Ų�����Ϣ
/// @param[in]
///    arraySize          - ����ͨ�Ų�����Ϣ�������Size
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ����Cache�е�ĳһ��iedName��ldInst��cbNameΪ������GSEControl��������ȫ��ͨ�Ų�����Ϣ
///
/// @param[in]
///     iedName       - GSEControl���ڵ�IED������
/// @param[in]
///     ldInst        - GSEControl���ڵ�LDevice��ʵ����
/// @param[in]
///     cbName        - GSEControl������
/// @param[in]
///     conntAPAttrib - GSEControl���ڵķ��ʵ����Ϣ
/// @param[in]
///     smvAddr       - GSEControl��ͨ�Ų�����Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
					// AppID���䣬��ֱ���滻
					pInfo->cnntAPAttrib = conntAPAttrib;
					pInfo->gseAttrib = gseAddr;
				}
				else
				{
					// AppID�ı䣬��ɾ���ɵģ����������
					if ( pPrevInfo == NULL )
						it_GSE->second = pInfo->pNextCache;
					else
						pPrevInfo->pNextCache = pInfo->pNextCache;
					// �Ƴ���GseCommn
					if ( it_GSE->second == NULL )
					{
						m_SCLMap->gseCommnMap->erase(it_GSE);
					}
					delete pInfo;
					// �����GseCommn
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
///     ��Cache�л�ȡĳһGSEControl��������ȫ��ͨ�Ų�����Ϣ
///     �����ڶ����ͬAPPID���򷵻ص�һ��
///
/// @param[in]
///     appID         - ͨ�Ų����е�appID�ȫվΨһ
/// @param[in]
///     conntAPAttrib - ������Ҫ��ȡ��ͨ�Ų�����������GSEControl���ڵķ��ʵ����Ϣ
/// @param[in]
///     gseAddr       - ������Ҫ��ȡ��ͨ�Ų�����Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::GetGSECommn(const unsigned short appID, SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, SCL_GSE_ATTRIB &gseAddr)
{
	// ����ָ��GSE
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
///     ��Cache�л�ȡĳһ��iedName��ldInst��cbNameΪ������GSEControl��������ȫ��ͨ�Ų�����Ϣ
///
/// @param[in]
///     iedName       - GSEControl���ڵ�IED������
/// @param[in]
///     ldInst        - GSEControl���ڵ�LDevice��ʵ����
/// @param[in]
///     cbName        - GSEControl������
/// @param[in]
///     conntAPAttrib - ������Ҫ��ȡ��ͨ�Ų�����������GSEControl���ڵķ��ʵ����Ϣ
/// @param[in]
///     smvAddr       - ������Ҫ��ȡ��ͨ�Ų�����Ϣ
///
/// @return
///     true���ɹ���  false��ʧ��
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
///     ��Cache���Ƴ�ĳһ��iedName��ldInst��lnName��dataSetNameΪ������ĳһ˳��ŵ�FCDA
///
/// @param[in]
///     iedName       - FCDA���ڵ�IED������
/// @param[in]
///     ldInst        - FCDA���ڵ�LDevice��ʵ����
/// @param[in]
///     lnName        - FCDA���ڵ�LN������
/// @param[in]
///     dataSetName   - FCDA���ڵ�DataSet������
/// @param[in]
///     idxSeq        - FCDA��˳��ţ���0��ʼ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::RemoveFCDAAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
									   const SCL_STR &datSetName, const int idxSeq)
{
	// ����ָ��DataSet
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

	// ��DataSet������FCDA
	FCDA_MAP *fcdaMap = it_DataSet->second->fcdaMap;
	FCDA_MAP::iterator it_FCDA = fcdaMap->find(idxSeq);

	if ( it_FCDA == fcdaMap->end() )
		return false;

	// �ͷ�FCDA
	if ( it_FCDA->second != NULL )
	{
		delete it_FCDA->second;
		it_FCDA->second = NULL;
	}

	// Ŀ��FCDA֮��Ĳ�����ǰ��1
	FCDA_MAP::iterator it_NextFCDA = fcdaMap->find(idxSeq+1);
	for ( ; it_NextFCDA != fcdaMap->end(); it_NextFCDA++ )
	{
		it_NextFCDA->second->fcdaAttrib.idxSeq--;
		it_FCDA->second = it_NextFCDA->second;
		it_FCDA         = it_NextFCDA;
	}

	// �Ƴ�Map�����һ���¼
	it_FCDA->second = NULL;
	fcdaMap->erase(it_FCDA);

	return true;
}

///
/// @brief
///     ��Cache���Ƴ�ָ��DoType�е�ĳһ˳��ŵ�DA
///
/// @param[in]
///     doTypeID      - doType��ID
/// @param[in]
///     idxSeq        - DA��˳��ţ���0��ʼ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::RemoveDAAttrib(const SCL_STR &doTypeID, const int idxSeq)
{
	// ����ָ��DoType
	DOTYPE_MAP::iterator it_DoType;
	if ( !FindDOTypeInMap(doTypeID,it_DoType) )
		return false;
	if ( it_DoType->second == NULL || it_DoType->second->daMap == NULL )
		return false;

	// ��DoType������DA
	DA_MAP *daMap = it_DoType->second->daMap;
	DA_MAP::iterator it_DA;
	it_DA = daMap->find(idxSeq);
	if ( it_DA == daMap->end() )
		return false;

	// Ŀ��DA֮��Ĳ�����ǰ��1
	DA_MAP::iterator it_NextDA = daMap->find(idxSeq+1);
	for ( ; it_NextDA != daMap->end(); it_NextDA++ )
	{
		it_DA->second = it_NextDA->second;
		it_DA         = it_NextDA;
	}

	// �Ƴ�Map�����һ���¼
	daMap->erase(it_DA);
	
	return true;
}

///
/// @brief
///     ��Cache���Ƴ�ָ��DaType�е�ĳһ˳��ŵ�BDA
///
/// @param[in]
///     daTypeID      - daType��ID
/// @param[in]
///     idxSeq        - BDA��˳��ţ���0��ʼ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::RemoveBDAAttrib(const SCL_STR &daTypeID, const int idxSeq)
{
	// ����ָ��DaType
	DATYPE_MAP::iterator it_DaType;
	if ( !FindDATypeInMap(daTypeID,it_DaType) )
		return false;
	if ( it_DaType->second == NULL || it_DaType->second->bdaMap == NULL )
		return false;

	// ��DaType������BDA
	BDA_MAP *bdaMap = it_DaType->second->bdaMap;
	BDA_MAP::iterator it_BDA;
	it_BDA = bdaMap->find(idxSeq);
	if ( it_BDA == bdaMap->end() )
		return false;

	// Ŀ��BDA֮��Ĳ�����ǰ��1
	BDA_MAP::iterator it_NextBDA = bdaMap->find(idxSeq+1);
	for ( ; it_NextBDA != bdaMap->end(); it_NextBDA++ )
	{
		it_BDA->second = it_NextBDA->second;
		it_BDA         = it_NextBDA;
	}

	// �Ƴ�Map�����һ���¼
	bdaMap->erase(it_BDA);

	return true;
}

///
/// @brief
///     ��ȡCache��ĳһIED������
///
/// @param[in]
///     iedName - IED������
/// @param[in]
///     it_IED  - ������Ҫ��ȡ��IED������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindIEDInMap(const SCL_STR &iedName, IED_MAP::iterator &it_IED)
{
	if ( m_SCLMap == NULL || m_SCLMap->iedMap == NULL )
		return false;

	// ��IEDMap������ָ��IED
	it_IED = m_SCLMap->iedMap->find(iedName);
	if ( it_IED == m_SCLMap->iedMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡĳһIED�е�ĳһLDevice������
///
/// @param[in]
///     it_IED  - IED������
/// @param[in]
///     ldInst  - LDevice��ʵ����
/// @param[in]
///     it_LD   - ������Ҫ��ȡ��LDevice������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindLDeviceInIED(const IED_MAP::iterator &it_IED, const SCL_STR &ldInst, LDEVICE_MAP::iterator &it_LD)
{
	if ( it_IED->second == NULL || it_IED->second->lDeviceMap == NULL )
		return false;

	// ��IDeviceMap������ָ��LDevice
	it_LD = it_IED->second->lDeviceMap->find(ldInst);
	if ( it_LD == it_IED->second->lDeviceMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡĳһLDevice�е�LN0������
///
/// @param[in]
///     it_LD   - LDevice������
/// @param[in]
///     ln0     - ָ��Ŀ��LN0�����õ�ָ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindLN0InLDevice(const LDEVICE_MAP::iterator &it_LD, LN0_INFO_CACHE **ln0)
{
	if ( it_LD->second == NULL )
		return false;

	//  ��LDevice������LN0
	*ln0 = it_LD->second->ln0InfoCache;
	if ( *ln0 == NULL )
		return false;

	return true;
}

///
/// @brief
///     ��ȡĳһLDevice��ĳһLN������
///
/// @param[in]
///     it_LD   - LDevice������
/// @param[in]
///     lnName  - �߼��ڵ�����
/// @param[in]
///     it_LN   - ������Ҫ��ȡ��LN������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindLNInLDevice(const LDEVICE_MAP::iterator &it_LD, const SCL_STR &lnName, LN_MAP::iterator &it_LN)
{
	if ( it_LD->second == NULL || it_LD->second->lnMap == NULL )
		return false;

	// ��LNMap������ָ��LN
	it_LN = it_LD->second->lnMap->find(lnName);
	if ( it_LN == it_LD->second->lnMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡĳһLN0��ĳһDataSet������
///
/// @param[in]
///     ln0         - LN0������
/// @param[in]
///     dataSetName - DataSet������
/// @param[in]
///     it_DataSet  - ������Ҫ��ȡ��DataSet������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindDataSetInLN0(const LN0_INFO_CACHE * const ln0, const SCL_STR &dataSetName, DATASET_MAP::iterator &it_DataSet)
{
	if ( ln0->datasetMap == NULL )
		return false;

	// ��LN0������DataSet
	it_DataSet = ln0->datasetMap->find(dataSetName);
	if ( it_DataSet == ln0->datasetMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡĳһDataSet�е�FCDA������
///
/// @param[in]
///     it_DataSet - DataSet������
/// @param[in]
///     idxSeq     - FCDA��DataSet�е�˳��ţ���0��ʼ
/// @param[in]
///     it_FCDA    - ������Ҫ��ȡ��FCDA������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindFCDAInDataSet(const DATASET_MAP::iterator &it_DataSet, const int idxSeq, FCDA_MAP::iterator &it_FCDA)
{
	if ( it_DataSet->second == NULL || it_DataSet->second->fcdaMap == NULL )
		return false;

	// ��DataSet������FCDA
	it_FCDA = it_DataSet->second->fcdaMap->find(idxSeq);
	if ( it_FCDA == it_DataSet->second->fcdaMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡĳһLN0�е�SampledValueControl������
///
/// @param[in]
///     ln0        - LN0������
/// @param[in]
///     cbName     - ���ƿ������
/// @param[in]
///     it_SMVCtrl - ������Ҫ��ȡ��SampledValueControl������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindSMVCtrlInLN0(const LN0_INFO_CACHE * const ln0, const SCL_STR &cbName, SMV_CTRL_MAP::iterator &it_SMVCtrl)
{
	if ( ln0->smvCtrlMap == NULL )
		return false;

	// ��LN0������SMV���ƿ�
	it_SMVCtrl = ln0->smvCtrlMap->find(cbName);
	if ( it_SMVCtrl == ln0->smvCtrlMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡĳһLN0�е�GSEControl������
///
/// @param[in]
///     ln0        - LN0������
/// @param[in]
///     cbName     - ���ƿ������
/// @param[in]
///     it_GSECtrl - ������Ҫ��ȡ��GSEControl������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindGSECtrlInLN0(const LN0_INFO_CACHE * const ln0, const SCL_STR &cbName, GSE_CTRL_MAP::iterator &it_GSECtrl)
{
	if ( ln0->gseCtrlMap == NULL )
		return false;

	// ��LN0������GSE���ƿ�
	it_GSECtrl = ln0->gseCtrlMap->find(cbName);
	if ( it_GSECtrl == ln0->gseCtrlMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡĳһLN0�е�DOI������
///
/// @param[in]
///     ln0        - LN0������
/// @param[in]
///     doiName    - DOI������
/// @param[in]
///     it_DOI     - ������Ҫ��ȡ��DOI������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindDOIInLN0(const LN0_INFO_CACHE * const ln0, const SCL_STR &doiName, DOI_MAP::iterator &it_DOI)
{
	if ( ln0->doiMap == NULL )
		return false;

	// ��LN0������DOI
	it_DOI = ln0->doiMap->find(doiName);
	if ( it_DOI == ln0->doiMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡĳһLN�е�DOI������
///
/// @param[in]
///     it_LN      - LN������
/// @param[in]
///     doiName    - DOI������
/// @param[in]
///     it_DOI     - ������Ҫ��ȡ��DOI������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindDOIInLN(const LN_MAP::iterator &it_LN, const SCL_STR &doiName, DOI_MAP::iterator &it_DOI)
{
	if ( it_LN->second == NULL || it_LN->second->doiMap == NULL )
		return false;

	// ��LN0������DOI
	it_DOI = it_LN->second->doiMap->find(doiName);
	if ( it_DOI == it_LN->second->doiMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡĳһLN0�е�ExtRef������
///
/// @param[in]
///     ln0        - LN0������
/// @param[in]
///     idxSeq     - ExtRef��˳��ţ���0��ʼ
/// @param[in]
///     it_ExtRef  - ������Ҫ��ȡ��ExtRef������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindExtRefInLN0(const LN0_INFO_CACHE * const ln0, const int &idxSeq, EXTREF_MAP::iterator &it_ExtRef)
{
	if ( ln0->extRefMap == NULL )
		return false;

	// ��LN0������ExtRef
	it_ExtRef = ln0->extRefMap->find(idxSeq);
	if ( it_ExtRef == ln0->extRefMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡĳһLN�е�ExtRef������
///
/// @param[in]
///     it_LN      - LN������
/// @param[in]
///     idxSeq     - ExtRef��˳��ţ���0��ʼ
/// @param[in]
///     it_ExtRef  - ������Ҫ��ȡ��ExtRef������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindExtRefInLN(const LN_MAP::iterator &it_LN, const int &idxSeq, EXTREF_MAP::iterator &it_ExtRef)
{
	if ( it_LN->second == NULL || it_LN->second->extRefMap == NULL )
		return false;

	// ��LN0������ExtRef
	it_ExtRef = it_LN->second->extRefMap->find(idxSeq);
	if ( it_ExtRef == it_LN->second->extRefMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡCache��ĳһIED��LDevice������
///
/// @param[in]
///     iedName - IED������
/// @param[in]
///     ldInst  - LDevice��ʵ����
/// @param[in]
///     it_LD   - ������Ҫ��ȡ��LDevice������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindLDeviceInMap(const SCL_STR &iedName, const SCL_STR &ldInst, LDEVICE_MAP::iterator &it_LD)
{
	// ��IEDMap������ָ��IED
	IED_MAP::iterator it_IED;
	if ( !FindIEDInMap(iedName,it_IED) )
		return false;

	return FindLDeviceInIED(it_IED,ldInst,it_LD);
}

///
/// @brief
///     ��ȡCache��ĳһIED��ĳһLDevice��LN0������
///
/// @param[in]
///     iedName - IED������
/// @param[in]
///     ldInst  - LDevice��ʵ����
/// @param[in]
///     ln0     - ָ����Ҫ��ȡ��LN0�����õ�ָ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindLN0InMap(const SCL_STR &iedName, const SCL_STR &ldInst ,LN0_INFO_CACHE **ln0)
{
	// ����ָ��LDevice
	LDEVICE_MAP::iterator it_LD;
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	return FindLN0InLDevice(it_LD,ln0);
}

///
/// @brief
///     ��ȡCache��ĳһIED��ĳһLDevice��LN������
///
/// @param[in]
///     iedName - IED������
/// @param[in]
///     ldInst  - LDevice��ʵ����
/// @param[in]
///     lnName  - LN������
/// @param[in]
///     it_LN   - ������Ҫ��ȡ��LN������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindLNInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, LN_MAP::iterator &it_LN)
{
	// ����ָ��LDevice
	LDEVICE_MAP::iterator it_LD;
	if ( !FindLDeviceInMap(iedName,ldInst,it_LD) )
		return false;

	return FindLNInLDevice(it_LD,lnName,it_LN);
}

///
/// @brief
///     ��ȡCache��ĳһIED��ĳһLDevice��LN0�е�DataSet����
///
/// @param[in]
///     iedName     - IED������
/// @param[in]
///     ldInst      - LDevice��ʵ����
/// @param[in]
///     dataSetName - DataSet������
/// @param[in]
///     it_DataSet  - ������Ҫ��ȡ��DatasET������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindDataSetInLN0InMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, DATASET_MAP::iterator &it_DataSet)
{
	// ����ָ��LN0
	LN0_INFO_CACHE *ln0;
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	return FindDataSetInLN0(ln0,dataSetName,it_DataSet);
}

///
/// @brief
///     ��ȡCache��ĳһIED��ĳһLDevice��ĳһLNode��ĳһDataSet�е�ĳһ˳��ŵ�FCDA������
///
/// @param[in]
///     iedName     - IED������
/// @param[in]
///     ldInst      - LDevice��ʵ����
/// @param[in]
///     lnName      - LNode�����ƣ�LN0���߼��ڵ����ƹ̶�Ϊ��LLN0��
/// @param[in]
///     dataSetname - DataSet������
/// @param[in]
///     idxSeq      - FCDA��DataSet�е�˳���
/// @param[in]
///     it_FCDA     - ������Ҫ��ȡ��FCDA������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindFCDAInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
									const SCL_STR &dataSetName,const int idxSeq, FCDA_MAP::iterator &it_FCDA)
{
	// ����ָ��DataSet
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
///     ��ȡCache��ĳһIED��ĳһLDevice��SampledValueControl������
///
/// @param[in]
///     iedName    - IED������
/// @param[in]
///     ldInst     - LDevice��ʵ����
/// @param[in]
///     cbName     - SampledValueControl������
/// @param[in]
///     it_SMVCtrl - ������Ҫ��ȡ��SampledValueControl������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindSMVCtrlInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, SMV_CTRL_MAP::iterator &it_SMVCtrl)
{
	// ����ָ��LN0
	LN0_INFO_CACHE *ln0;
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	return FindSMVCtrlInLN0(ln0,cbName,it_SMVCtrl);
}

///
/// @brief
///     ��ȡCache��ĳһIED��ĳһLDevice��GSEControl������
///
/// @param[in]
///     iedName    - IED������
/// @param[in]
///     ldInst     - LDevice��ʵ����
/// @param[in]
///     cbName     - GSEControl������
/// @param[in]
///     it_GSECtrl - ������Ҫ��ȡ��GSEControl������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindGSECtrlInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, GSE_CTRL_MAP::iterator &it_GSECtrl)
{
	// ����ָ��LN0
	LN0_INFO_CACHE *ln0;
	if ( !FindLN0InMap(iedName,ldInst,&ln0) )
		return false;

	return FindGSECtrlInLN0(ln0,cbName,it_GSECtrl);
}

///
/// @brief
///     ��ȡCache��ĳһIED��ĳһLDevice��ĳһLNode��ĳһLN�е�DOI������
///
/// @param[in]
///     iedName     - IED������
/// @param[in]
///     ldInst      - LDevice��ʵ����
/// @param[in]
///     lnName      - LNode�����ƣ�LN0���߼��ڵ����ƹ̶�Ϊ��LLN0��
/// @param[in]
///     doiName     - DOI������
/// @param[in]
///     it_DOI      - ������Ҫ��ȡ��DOI������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindDOIInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
								   const SCL_STR &doiName, DOI_MAP::iterator &it_DOI)
{
	if ( lnName == _T("LLN0") )
	{
		// ����ָ��LN0
		LN0_INFO_CACHE *ln0;
		if ( !FindLN0InMap(iedName,ldInst,&ln0) )
			return false;
		return FindDOIInLN0(ln0,doiName,it_DOI);
	}
	else
	{
		// ����ָ��LN
		LN_MAP::iterator it_LN;
		if ( !FindLNInMap(iedName,ldInst,lnName,it_LN) )
			return false;
		return FindDOIInLN(it_LN,doiName,it_DOI);
	}
}

///
/// @brief
///     ��ȡCache��ĳһIED��ĳһLDevice��ĳһLNode��ĳһLN�е�ExtRef������
///
/// @param[in]
///     iedName     - IED������
/// @param[in]
///     ldInst      - LDevice��ʵ����
/// @param[in]
///     lnName      - LNode�����ƣ�LN0���߼��ڵ����ƹ̶�Ϊ��LLN0��
/// @param[in]
///     idxSeq      - ExtRef��˳��ţ���0��ʼ
/// @param[in]
///     it_ExtRef   - ������Ҫ��ȡ��ExtRef������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindExtRefInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const int &idxSeq, EXTREF_MAP::iterator &it_ExtRef)
{
	if ( lnName == _T("LLN0") )
	{
		// ����ָ��LN0
		LN0_INFO_CACHE *ln0;
		if ( !FindLN0InMap(iedName,ldInst,&ln0) )
			return false;
		return FindExtRefInLN0(ln0,idxSeq,it_ExtRef);
	}
	else
	{
		// ����ָ��LN
		LN_MAP::iterator it_LN;
		if ( !FindLNInMap(iedName,ldInst,lnName,it_LN) )
			return false;
		return FindExtRefInLN(it_LN,idxSeq,it_ExtRef);
	}
}

///
/// @brief
///     ��ȡCache��ĳһSampledValueControl��ͨ�Ų���������
///
/// @param[in]
///     appID  - ͨ�Ų����е�appID��
/// @param[in]
///     it_SMV - ������Ҫ��ȡ��ͨ�Ų���Map������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindSMVCommnInMap(const unsigned short appID, SMV_COMMN_MAP::iterator &it_SMV)
{
	if ( m_SCLMap == NULL || m_SCLMap->smvCommnMap == NULL )
		return false;

	// ��SMVMap������ָ��SMV
	it_SMV = m_SCLMap->smvCommnMap->find(appID);
	if ( it_SMV == m_SCLMap->smvCommnMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡCache��ĳһGSEControl��ͨ�Ų���������
///
/// @param[in]
///     appID  - ͨ�Ų����е�appID��
/// @param[in]
///     it_GSE- ������Ҫ��ȡ��ͨ�Ų���Map������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindGSECommnInMap(const unsigned short appID, GSE_COMMN_MAP::iterator &it_GSE)
{
	if ( m_SCLMap == NULL || m_SCLMap->gseCommnMap == NULL )
		return false;

	// ��IEDMap������ָ��IED
	it_GSE = m_SCLMap->gseCommnMap->find(appID);
	if ( it_GSE == m_SCLMap->gseCommnMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡCache��ĳһLNType������
///
/// @param[in]
///     lnTypeID  - LNType��id����ֵ
/// @param[in]
///     it_LNType - ������Ҫ��ȡ��LNType������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindLNTypeInMap(const SCL_STR &lnTypeID, LNTYPE_MAP::iterator &it_LNType)
{
	if ( m_SCLMap == NULL || m_SCLMap->lnTypeMap == NULL )
		return false;

	// ��LNTypeMap������ָ��lnType
	it_LNType = m_SCLMap->lnTypeMap->find(lnTypeID);
	if ( it_LNType == m_SCLMap->lnTypeMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡĳһLNType�е�DO������
///
/// @param[in]
///     it_LNType - LNType������
/// @param[in]
///     doName    - DO������
/// @param[in]
///     it_DO     - ������Ҫ��ȡ��DO������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindDOInLNType(const LNTYPE_MAP::iterator &it_LNType, const SCL_STR &doName, DO_MAP::iterator &it_DO)
{
	if ( it_LNType->second == NULL || it_LNType->second->doMap == NULL )
		return false;

	// ��LNType������DO
	it_DO = it_LNType->second->doMap->find(doName);
	if ( it_DO == it_LNType->second->doMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡCache��ĳһDO������
///
/// @param[in]
///     lnTypeID  - LNType��ID
/// @param[in]
///     doName    - DO������
/// @param[in]
///     it_DO     - ������Ҫ��ȡ��DO������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindDOInMap(const SCL_STR &lnTypeID, const SCL_STR &doName, DO_MAP::iterator &it_DO)
{
	// ����ָ��LNType
	LNTYPE_MAP::iterator it_LNType;
	if ( !FindLNTypeInMap(lnTypeID,it_LNType) )
		return false;

	return FindDOInLNType(it_LNType,doName,it_DO);
}

///
/// @brief
///     ��ȡCache��ĳһDOType������
///
/// @param[in]
///     doTypeID  - DOType��id����ֵ
/// @param[in]
///     it_DOType - ������Ҫ��ȡ��DOType������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindDOTypeInMap(const SCL_STR &doTypeID, DOTYPE_MAP::iterator &it_DOType)
{
	if ( m_SCLMap == NULL || m_SCLMap->doTypeMap == NULL )
		return false;

	// ��DOTypeMap������ָ��doType
	it_DOType = m_SCLMap->doTypeMap->find(doTypeID);
	if ( it_DOType == m_SCLMap->doTypeMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡĳһDOType�е�DA������
///
/// @param[in]
///     it_DOType - DOType������
/// @param[in]
///     daName    - DA������
/// @param[in]
///     it_DA     - ������Ҫ��ȡ��DA������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindDAInDOType(const DOTYPE_MAP::iterator &it_DOType, const SCL_STR &daName, DA_MAP::iterator &it_DA)
{
	if ( it_DOType->second == NULL || it_DOType->second->daMap == NULL )
		return false;

	// ��DOType������DA
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
///     ��ȡCache��ĳһDA������
///
/// @param[in]
///     doTypeID  - DOType��ID
/// @param[in]
///     daName    - DA������
/// @param[in]
///     it_DA     - ������Ҫ��ȡ��DA������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindDAInMap(const SCL_STR &doTypeID, const SCL_STR &daName, DA_MAP::iterator &it_DA)
{
	// ����ָ��DOType
	DOTYPE_MAP::iterator it_DOType;
	if ( !FindDOTypeInMap(doTypeID,it_DOType) )
		return false;
	return FindDAInDOType(it_DOType,daName,it_DA);
}

///
/// @brief
///     ��ȡCache��ĳһDAType������
///
/// @param[in]
///     daTypeID  - DAType��id����ֵ
/// @param[in]
///     it_DAType - ������Ҫ��ȡ��DAType������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindDATypeInMap(const SCL_STR &daTypeID, DATYPE_MAP::iterator &it_DAType)
{
	if ( m_SCLMap == NULL || m_SCLMap->daTypeMap == NULL )
		return false;

	// ��DATypeMap������ָ��daType
	it_DAType = m_SCLMap->daTypeMap->find(daTypeID);
	if ( it_DAType == m_SCLMap->daTypeMap->end() )
		return false;

	return true;
}

///
/// @brief
///     ��ȡĳһDAType�е�BDA������
///
/// @param[in]
///     it_DAType - DAType������
/// @param[in]
///     bdaName   - BDA������
/// @param[in]
///     it_BDA    - ������Ҫ��ȡ��BDA������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindBDAInDAType(const DATYPE_MAP::iterator &it_DAType, const SCL_STR & bdaName, BDA_MAP::iterator &it_BDA)
{
	if ( it_DAType->second == NULL || it_DAType->second->bdaMap == NULL )
		return false;

	// ��DAType������BDA
	for ( it_BDA = it_DAType->second->bdaMap->begin(); it_BDA != it_DAType->second->bdaMap->end(); it_BDA++ )
	{
		if ( it_BDA->second.name.compare(bdaName) == 0 )
			return true;
	}
	return false;
// 
// 	// ��DAType������BDA
// 	it_BDA = it_DAType->second->bdaMap->find(bdaName);
// 	if ( it_BDA == it_DAType->second->bdaMap->end() )
// 		return false;
// 	return true;
}

///
/// @brief
///     ��ȡCache��ĳһBDA������
///
/// @param[in]
///     daTypeID  - DAType��ID
/// @param[in]
///     bdaName   - BDA������
/// @param[in]
///     it_BDA    - ������Ҫ��ȡ��BDA������
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLCacheWithMap::FindBDAInMap(const SCL_STR &daTypeID, const SCL_STR & bdaName, BDA_MAP::iterator &it_BDA)
{
	// ����ָ��DAType
	DATYPE_MAP::iterator it_DAType;
	if ( !FindDATypeInMap(daTypeID,it_DAType) )
		return false;
	return FindBDAInDAType(it_DAType,bdaName,it_BDA);
}

///
/// @brief
///     ���CacheĳһFCDA Map�е�����
///
/// @param[in]
///     fcdaMap - ָ��FCDA Map��ָ��
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
///     ���CacheĳһDataSet Map�е�����
///
/// @param[in]
///     dataSetMap - ָ��DataSet Map��ָ��
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
///     ���CacheĳһLN0�е�����
///
/// @param[in]
///     ln0 - ָ��LN0��ָ��
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
///     ���Cache��ĳһLN Map�е�����
///
/// @param[in]
///     lnMap - ָ��LN Map��ָ��
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
///     ���CacheĳһLDevice Map�е�����
///
/// @param[in]
///     ldMap - ָ��LDevice Map��ָ��
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
///     ���CacheĳһIED Map�е�����
///
/// @param[in]
///     iedMap - ָ��IED Map��ָ��
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
///     ���CacheĳһSMVCommn Map�е�����
///
/// @param[in]
///     smvCommnMap - ָ��SMVCommn Map��ָ��
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
///     ���CacheĳһGSECommn Map�е�����
///
/// @param[in]
///     gseCommnMap - ָ��GSECommn Map��ָ��
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
///     ���CacheĳһLNType Map�е�����
///
/// @param[in]
///     lnTypeMap - ָ��LNType Map��ָ��
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
///     ���CacheĳһDOType Map�е�����
///
/// @param[in]
///     doTypeMap - ָ��DOType Map��ָ��
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
///     ���CacheĳһLNType Map�е�����
///
/// @param[in]
///     lnTypeMap - ָ��LNType Map��ָ��
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
///     ���Cache��SCL Map�е�����
///
/// @param[in]
///     sclMap - ָ��SCL Map��ָ��
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

