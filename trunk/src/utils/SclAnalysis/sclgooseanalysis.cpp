/// @file
///     sclgooseanalysis.cpp
///
/// @brief
///     SCL�ļ��й���GOOSE��������Ϣ�Ľ���
///
/// @note
///
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.2.4
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.02.18     |1.0.3.218  |���GOOSE����������ߵ���Ϣ��GOOSE INPUT������            |
///    |2013.08.07     |1.0.5.807  |��ȡGOOSEͨ����Ϣʱ����Ӷ�ͨ����DOI��Ϣ������ʱ�Ĵ���    |
///    |2013.08.08     |1.0.5.808  |�޸�ĳ�߼��ڵ��Ƿ�ΪLLN0���ж�����                        |
///    |2013.08.13     |1.0.5.813  |������AppID��MacΪ������ȡȫ�����ƿ���Ϣ�Ҳ���AppID��Ӧ�Ŀ��ƿ�û�ж���ʱ��Ϣ��ȡʧ�ܵ�Bug|
///    |2013.08.15     |1.0.5.815  |�޸�Gooseͨ������ͨ����Ϣ�Ĳ�������ȡ�����á��Ƴ���ʵ�֣�ʹ��֧��ͨ��ΪDAģʽ��Ϊ����Ϊ�ṹ������|
///
#include "stdafx.h"
#include "sclgooseanalysis.h"

///
/// @brief
///    GOOSEͨ�����Զ�����������ģ����Ϣ
///
const SCL_DAINDOTYPE_ATTRIB GOOSE_CHANNEL_TYPE_TEMPLATE[] = {
	{_T("unknown"),_T(""),_T("ST"),_T("UNKNOWN"),_T("")},
	{_T("struct"),_T(""),_T("ST"),_T("Struct"),_T("")},
	{_T("stVal1"),_T(""),_T("ST"),_T("BOOLEAN"),_T("")},
	{_T("stVal2"),_T(""),_T("ST"),_T("Dbpos"),_T("")},
	{_T("q"),_T(""),_T("ST"),_T("Quality"),_T("")},
	{_T("t"),_T(""),_T("ST"),_T("Timestamp"),_T("")},
	{_T("et"),_T(""),_T("ST"),_T("EntryTime"),_T("")},
	{_T("int8"),_T(""),_T("ST"),_T("INT8"),_T("")},
	{_T("int16"),_T(""),_T("ST"),_T("INT16"),_T("")},
	{_T("int24"),_T(""),_T("ST"),_T("INT24"),_T("")},
	{_T("int32"),_T(""),_T("ST"),_T("INT32"),_T("")},
	{_T("int128"),_T(""),_T("ST"),_T("INT128"),_T("")},
	{_T("uint8"),_T(""),_T("ST"),_T("INT8U"),_T("")},
	{_T("uint16"),_T(""),_T("ST"),_T("INT16U"),_T("")},
	{_T("uint24"),_T(""),_T("ST"),_T("INT24U"),_T("")},
	{_T("uint32"),_T(""),_T("ST"),_T("INT32U"),_T("")},
	{_T("f32"),_T(""),_T("ST"),_T("FLOAT32"),_T("")},
	{_T("f64"),_T(""),_T("ST"),_T("FLOAT64"),_T("")},
	{_T("oct"),_T(""),_T("ST"),_T("Octet64"),_T("")},
	{_T("vs"),_T(""),_T("ST"),_T("VisString255"),_T("")},
	{_T("uc"),_T(""),_T("ST"),_T("Unicode255"),_T("")}
};

SCLGOOSEAnalysis::SCLGOOSEAnalysis( ISCLCacheManager *pSclCacheMgr )
{
	m_pSclCacheMgr = (SCLCacheManager *)pSclCacheMgr;
}

SCLGOOSEAnalysis::~SCLGOOSEAnalysis()
{

}

///
/// @brief
///     ��ȡ�����ļ�������������ͨ�Ų�����GOOSE���ƿ������
///
/// @return
///     GOOSE���ƿ������ - �����ɹ��� -1 - ����ʧ��
///
int SCLGOOSEAnalysis::GetIEDGOOSENum()
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;
	return pCache->GetGSECommnNumInCache();
}

///
/// @brief
///     ��ȡ�����ļ�������������ͨ�Ų�����GOOSE���ƿ���Ϣ
///
/// @param[in]
///     iedGOOSEInfoArr - ����GOOSE���ƿ���Ϣ������
/// @param[in]
///     arrSize         - ���ݵ��������
///
/// @return
///     �����б�����Ϣ������ - �����ɹ��� -1 - ����ʧ��
///
int SCLGOOSEAnalysis::GetAllIEDGOOSEInfo( SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfoArr, const int arrSize )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL || arrSize == 0 )
		return -1;

	SCL_IED_ATTRIB          iedAttrib;
	SCL_ACCESS_POINT_ATTRIB apAttrib;
	SCL_LDEVICE_ATTRIB      ldAttrib;
	SCL_GSECTRL_ATTRIB      gseCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	SCL_CONNECTEDAP_ATTRIB *conntAPAttrib = new SCL_CONNECTEDAP_ATTRIB [arrSize];
	SCL_GSE_ATTRIB         *gseAttrib     = new SCL_GSE_ATTRIB         [arrSize];
	if ( conntAPAttrib == NULL || gseAttrib == NULL )
		return -1;

	// ��ȡ���ƿ��ͨ�Ų�����Ϣ
	if ( !pCache->GetAllGSECommn(conntAPAttrib,gseAttrib,arrSize) )
		goto errorHandle;

	// ��ȡ���ƿ����װ�ñ�����ص���Ϣ��������IED����Ϣ��ϵͳ������
	// ע��SCL�ļ����ڶ�����ͨ�Ų�������û�ж���IED����ƿ������Ϣ�����
	int ctrlNum = 0;
	for (int i=0; i<arrSize; i++)
	{
		if ( !pCache->GetIEDAttrib(conntAPAttrib[i].iedName,iedAttrib) )
			continue;
		if ( !pCache->GetLDeviceAttrib(conntAPAttrib[i].iedName,gseAttrib[i].ldInst,apAttrib,ldAttrib) )
			continue;
		if ( !pCache->GetGSECtrlAttribInLN0(conntAPAttrib[i].iedName,gseAttrib[i].ldInst,gseAttrib[i].cbName,gseCtrlAttrib) )
			continue;
		if ( !pCache->GetDataSetAttribInLN0(conntAPAttrib[i].iedName,gseAttrib[i].ldInst,gseCtrlAttrib.dataSet,dataSetAttrib) )
			continue;

		SCL2IEDGOOSEInfo(gseAttrib[i],iedAttrib,apAttrib,ldAttrib,dataSetAttrib,gseCtrlAttrib,&iedGOOSEInfoArr[ctrlNum]);
		ctrlNum++;
	}

	delete[] conntAPAttrib;
	delete[] gseAttrib;
	return ctrlNum;

errorHandle:
	delete[] conntAPAttrib;
	delete[] gseAttrib;
	return -1;
}

///
/// @brief
///     ��ȡָ��MAC��APPID��GOOSE���ƿ����Ϣ
///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
///
/// @param[in]
///     gooseMAC     - GOOSEͨ���鲥MAC��ַ
/// @param[in]
///     gooseAPPID   - GOOSEͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
/// @param[in]
///     iedGOOSEInfo - ����GOOSE���ƿ���Ϣ�Ľṹ��
///
/// @return
///     0 - �����ɹ��� -1 - ����ʧ��
///
int SCLGOOSEAnalysis::GetOneIEDGOOSEInfo( const unsigned char *gooseMAC, const unsigned short gooseAppID, SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfo )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_CONNECTEDAP_ATTRIB  conntAPAttrib;
	SCL_GSE_ATTRIB          gseAttrib;
	SCL_IED_ATTRIB          iedAttrib;
	SCL_ACCESS_POINT_ATTRIB apAttrib;
	SCL_LDEVICE_ATTRIB      ldAttrib;
	SCL_GSECTRL_ATTRIB      gseCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	unsigned char           macAddr[GOOSEANALYSIS_MAC_SIZE];

	if ( !pCache->GetGSECommn(gooseAppID,conntAPAttrib,gseAttrib) )
		return -1;
	ConvMacWStr2Char(gseAttrib.macAddr,macAddr);
	if ( !IsMacAddrSame(gooseMAC,macAddr) )
		return -1;
	ConvMacWStr2Char(gseAttrib.macAddr,macAddr);
	if ( !IsMacAddrSame(gooseMAC,macAddr) )
		return -1;
	if ( !pCache->GetIEDAttrib(conntAPAttrib.iedName,iedAttrib) )
		return -1;
	if ( !pCache->GetLDeviceAttrib(conntAPAttrib.iedName,gseAttrib.ldInst,apAttrib,ldAttrib) )
		return -1;
	if ( !pCache->GetGSECtrlAttribInLN0(conntAPAttrib.iedName,gseAttrib.ldInst,gseAttrib.cbName,gseCtrlAttrib) )
		return -1;
	if ( !pCache->GetDataSetAttribInLN0(conntAPAttrib.iedName,gseAttrib.ldInst,gseCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;

	SCL2IEDGOOSEInfo(gseAttrib,iedAttrib,apAttrib,ldAttrib,dataSetAttrib,gseCtrlAttrib,iedGOOSEInfo);

	return 0;
}

///
/// @brief
///     ��ȡָ��MAC��APPID��ص�GOOSE���ƿ������Ƶ�DataSet��ͨ����FCDA��������
///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
///
/// @param[in]
///     gooseMAC     - GOOSEͨ���鲥MAC��ַ
/// @param[in]
///     gooseAPPID   - GOOSEͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
///
/// @return
///     DataSet��FCDA������ - �����ɹ��� -1 - ����ʧ��
///
int SCLGOOSEAnalysis::GetIEDGOOSEDataSetNum( const unsigned char *gooseMAC, const unsigned short gooseAppID )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_CONNECTEDAP_ATTRIB  conntAPAttrib;
	SCL_GSE_ATTRIB          gseAttrib;
	SCL_GSECTRL_ATTRIB      gseCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	unsigned char           macAddr[6];

	if ( !pCache->GetGSECommn(gooseAppID,conntAPAttrib,gseAttrib) )
		return -1;
	ConvMacWStr2Char(gseAttrib.macAddr,macAddr);
	if ( !IsMacAddrSame(gooseMAC,macAddr) )
		return -1;
	if ( !pCache->GetGSECtrlAttribInLN0(conntAPAttrib.iedName,gseAttrib.ldInst,gseAttrib.cbName,gseCtrlAttrib) )
		return -1;
	if ( !pCache->GetDataSetAttribInLN0(conntAPAttrib.iedName,gseAttrib.ldInst,gseCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;

	return dataSetAttrib.fcdaNum;
}

///
/// @brief
///     ��ȡָ��MAC��APPID��ص�GOOSE���ƿ������Ƶ�DataSet������ͨ����FCDA����Ϣ
///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
///
/// @param[in]
///     gooseMAC       - GOOSEͨ���鲥MAC��ַ
/// @param[in]
///     gooseAPPID     - GOOSEͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
// @param[in]
///     dataSetInfoArr - ����GOOSE��ͨ����Ϣ������
// @param[in]
///     arrSize        - ������������
///
/// @return
///     ���鱣���ͨ����FCDA����Ϣ������ - �����ɹ��� -1 - ����ʧ��
///
int SCLGOOSEAnalysis::GetAllIEDGOOSEDataSetInfo( const unsigned char *gooseMAC, const unsigned short gooseAppID, SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr, const int arrSize )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_CONNECTEDAP_ATTRIB  conntAPAttrib;
	SCL_GSE_ATTRIB          gseAttrib;
	SCL_GSECTRL_ATTRIB      gseCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	SCL_FCDA_ATTRIB         fcdaAttrib;
	SCL_LN0_ATTRIB          ln0Attrib;
	SCL_LN_ATTRIB           lnAttrib;
	SCL_DOI_ATTRIB          doiAttrib;
	SCL_DOINLNTYPE_ATTRIB   doAttrib;
	SCL_DAINDOTYPE_ATTRIB   daAttirb;
	SCL_BDAINDATYPE_ATTRIB  bdaAttrib;
	SCL_STR                 lnName;
	SCL_STR                 lnType;
	SCL_STR                 lnDesc;
	SCL_STR                 daName;
	SCL_STR                 bdaName;
	SCL_STR                 daBType;
	unsigned char           macAddr[GOOSEANALYSIS_MAC_SIZE];

	if ( !pCache->GetGSECommn(gooseAppID,conntAPAttrib,gseAttrib) )
		return -1;
	ConvMacWStr2Char(gseAttrib.macAddr,macAddr);
	if ( !IsMacAddrSame(gooseMAC,macAddr) )
		return -1;
	if ( !pCache->GetGSECtrlAttribInLN0(conntAPAttrib.iedName,gseAttrib.ldInst,gseAttrib.cbName,gseCtrlAttrib) )
		return -1;
	// ��ȡ���ݼ���Ϣ
	if ( !pCache->GetDataSetAttribInLN0(conntAPAttrib.iedName,gseAttrib.ldInst,gseCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;

	int i = 0;
	for ( i = 0; i < dataSetAttrib.fcdaNum ; i++ )
	{
		if ( i >= arrSize )
			break;
		// ��ȡFCDA������Ϣ
		if ( !pCache->GetFCDAAttrib(conntAPAttrib.iedName,gseAttrib.ldInst,_T("LLN0"),gseCtrlAttrib.dataSet,i,fcdaAttrib) )
			return -1;
		lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
		// ��ȡFCDA��ָ������ݵ���Ϣ
		if ( !pCache->GetDOIAttribInLNorLN0(conntAPAttrib.iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
		{
			doiAttrib.doName = fcdaAttrib.doName;
			doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
		}
		// ��ȡFCDA��ָ������������߼��ڵ����Ϣ
		if ( fcdaAttrib.lnClass == _T("LLN0") )
		{
			if ( !pCache->GetLN0Attrib(conntAPAttrib.iedName,fcdaAttrib.ldInst,ln0Attrib) )
				return -1;
			lnType = ln0Attrib.lnType;
			lnDesc = ln0Attrib.lnDesc;
		}
		else
		{
			if ( !pCache->GetLNAttrib(conntAPAttrib.iedName,fcdaAttrib.ldInst,lnName,lnAttrib) )
				return -1;
			lnType = lnAttrib.lnType;
			lnDesc = lnAttrib.lnDesc;
		}
		// ��ȡFCDA��ָ����������Ե�������Ϣ
		if ( !fcdaAttrib.daName.empty() )
		{
			if ( !pCache->GetDOAttrib(lnType,fcdaAttrib.doName,doAttrib) )
				return -1;
			int pos = (int)fcdaAttrib.daName.find(_T("."));
			if ( pos == -1 )
			{
				if ( !pCache->GetDAAttrib(doAttrib.type,fcdaAttrib.daName,daAttirb) )
					return -1;
				daBType = daAttirb.bType;
			}
			else
			{
				daName  = fcdaAttrib.daName.substr(0,pos);
				bdaName = fcdaAttrib.daName.substr(pos+1);
				if ( !pCache->GetDAAttrib(doAttrib.type,daName,daAttirb) )
					return -1;
				if ( !pCache->GetBDAAttrib(daAttirb.type,bdaName,bdaAttrib) )
					return -1;
				daBType = bdaAttrib.bType;
			}
		}
		else
		{
			daBType = _T("");
		}
		SCL2DataSetInfo(dataSetAttrib,fcdaAttrib,lnDesc,doiAttrib,daBType,&dataSetInfoArr[i]);
	}

	return i;
}

///
/// @brief
///     ��ȡָ��MAC��APPID��ص�GOOSE���ƿ������Ƶ�DataSet��ĳһ˳��ŵ�ͨ����FCDA����Ϣ
///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
///
/// @param[in]
///     gooseMAC       - GOOSEͨ���鲥MAC��ַ
/// @param[in]
///     gooseAPPID     - GOOSEͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
// @param[in]
///     idxSeq         - ͨ����˳���(��0��ʼ)
// @param[in]
///     dataSetInfo    - ����ͨ����Ϣ�Ľṹ��
///
/// @return
///     0 - �����ɹ��� -1 - ����ʧ��
///
int SCLGOOSEAnalysis::GetOneIEDGOOSEDataSetInfo( const unsigned char *gooseMAC, const unsigned short gooseAppID, const int idxSeq, SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_CONNECTEDAP_ATTRIB  conntAPAttrib;
	SCL_GSE_ATTRIB          gseAttrib;
	SCL_GSECTRL_ATTRIB      gseCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	SCL_FCDA_ATTRIB         fcdaAttrib;
	SCL_LN0_ATTRIB          ln0Attrib;
	SCL_LN_ATTRIB           lnAttrib;
	SCL_DOI_ATTRIB          doiAttrib;
	SCL_DOINLNTYPE_ATTRIB   doAttrib;
	SCL_DAINDOTYPE_ATTRIB   daAttirb;
	SCL_BDAINDATYPE_ATTRIB  bdaAttrib;
	SCL_STR                 lnName;
	SCL_STR                 lnType;
	SCL_STR                 lnDesc;
	SCL_STR                 daName;
	SCL_STR                 bdaName;
	SCL_STR                 daBType;
	unsigned char           macAddr[GOOSEANALYSIS_MAC_SIZE];

	if ( !pCache->GetGSECommn(gooseAppID,conntAPAttrib,gseAttrib) )
		return -1;
	ConvMacWStr2Char(gseAttrib.macAddr,macAddr);
	if ( !IsMacAddrSame(gooseMAC,macAddr) )
		return -1;
	if ( !pCache->GetGSECtrlAttribInLN0(conntAPAttrib.iedName,gseAttrib.ldInst,gseAttrib.cbName,gseCtrlAttrib) )
		return -1;
	// ��ȡ���ݼ���Ϣ
	if ( !pCache->GetDataSetAttribInLN0(conntAPAttrib.iedName,gseAttrib.ldInst,gseCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;
	// ��ȡFCDA������Ϣ
	if ( !pCache->GetFCDAAttrib(conntAPAttrib.iedName,gseAttrib.ldInst,_T("LLN0"),gseCtrlAttrib.dataSet,idxSeq,fcdaAttrib) )
		return -1;
	lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
	// ��ȡFCDA��ָ������ݵ���Ϣ
	if ( !pCache->GetDOIAttribInLNorLN0(conntAPAttrib.iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
	{
		doiAttrib.doName = fcdaAttrib.doName;
		doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
	}
	// ��ȡFCDA��ָ������������߼��ڵ����Ϣ
	if ( fcdaAttrib.lnClass == _T("LLN0") )
	{
		if ( !pCache->GetLN0Attrib(conntAPAttrib.iedName,fcdaAttrib.ldInst,ln0Attrib) )
			return -1;
		lnType = ln0Attrib.lnType;
		lnDesc = ln0Attrib.lnDesc;
	}
	else
	{
		if ( !pCache->GetLNAttrib(conntAPAttrib.iedName,fcdaAttrib.ldInst,lnName,lnAttrib) )
			return -1;
		lnType = lnAttrib.lnType;
		lnDesc = lnAttrib.lnDesc;
	}
	// ��ȡFCDA��ָ����������Ե�������Ϣ
	if ( !fcdaAttrib.daName.empty() )
	{
		if ( !pCache->GetDOAttrib(lnType,fcdaAttrib.doName,doAttrib) )
			return -1;
		int pos = (int)fcdaAttrib.daName.find(_T("."));
		if ( pos == -1 )
		{
			if ( !pCache->GetDAAttrib(doAttrib.type,fcdaAttrib.daName,daAttirb) )
				return -1;
			daBType = daAttirb.bType;
		}
		else
		{
			daName  = fcdaAttrib.daName.substr(0,pos);
			bdaName = fcdaAttrib.daName.substr(pos+1);
			if ( !pCache->GetDAAttrib(doAttrib.type,daName,daAttirb) )
				return -1;
			if ( !pCache->GetBDAAttrib(daAttirb.type,bdaName,bdaAttrib) )
				return -1;
			daBType = bdaAttrib.bType;
		}
	}
	else
	{
		daBType = _T("");
	}
	SCL2DataSetInfo(dataSetAttrib,fcdaAttrib,lnDesc,doiAttrib,daBType,dataSetInfo);

	return 0;
}

///
/// @brief
///     ��ȡָ��MAC��APPID��ص�GOOSE���ƿ������Ƶ�DataSet��ĳһͨ����FCDA����˳���
///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
///
/// @param[in]
///     gooseMAC       - GOOSEͨ���鲥MAC��ַ
/// @param[in]
///     gooseAPPID     - GOOSEͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
// @param[in]
///     dataSetInfo    - ����ͨ����Ϣ�Ľṹ��
///
/// @return
///     ͨ����FCDA����˳��� - �����ɹ��� -1 - ����ʧ��
///
int SCLGOOSEAnalysis::GetOneIEDGOOSEDataSetIdxSeq( const unsigned char *gooseMAC, const unsigned short gooseAppID, const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_CONNECTEDAP_ATTRIB  conntAPAttrib;
	SCL_GSE_ATTRIB          gseAttrib;
	SCL_GSECTRL_ATTRIB      gseCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	SCL_FCDA_ATTRIB         fcdaAttrib;
	SCL_DOI_ATTRIB          doiAttrib;
	unsigned char           macAddr[GOOSEANALYSIS_MAC_SIZE];

	if ( !pCache->GetGSECommn(gooseAppID,conntAPAttrib,gseAttrib) )
		return -1;
	ConvMacWStr2Char(gseAttrib.macAddr,macAddr);
	if ( !IsMacAddrSame(gooseMAC,macAddr) )
		return -1;
	if ( !pCache->GetGSECtrlAttribInLN0(conntAPAttrib.iedName,gseAttrib.ldInst,gseAttrib.cbName,gseCtrlAttrib) )
		return -1;
	if ( !pCache->GetDataSetAttribInLN0(conntAPAttrib.iedName,gseAttrib.ldInst,gseCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;
	for ( int i = 0; i < dataSetAttrib.fcdaNum ; i++ )
	{
		if ( !pCache->GetFCDAAttrib(conntAPAttrib.iedName,gseAttrib.ldInst,_T("LLN0"),gseCtrlAttrib.dataSet,i,fcdaAttrib) )
			return -1;
		if ( IsGoalFCDA(dataSetInfo,fcdaAttrib) )
		{
			return i;
		}
	}

	return -1;
}

///
/// @brief
///     ��ȡָ��MAC��APPID��GOOSE���ƿ�Ŀ��Ƶ�ĳһ��ͨ������ͨ������Ŀ
///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
///
/// @param[in]
///     gooseMAC       - GOOSEͨ���鲥MAC��ַ
/// @param[in]
///     gooseAPPID     - GOOSEͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
// @param[in]
///     dataSetInfo    - ��ͨ����Ϣ
///
/// @return
///     ��ͨ����Ŀ - �����ɹ��� -1 - ����ʧ��
///
int SCLGOOSEAnalysis::GetIEDGOOSESubChannelNum(
	const unsigned char *gooseMAC, 
	const unsigned short gooseAppID,
	const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo)
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_CONNECTEDAP_ATTRIB  conntAPAttrib;
	SCL_GSE_ATTRIB          gseAttrib;
	SCL_LN0_ATTRIB          ln0Attrib;
	SCL_LN_ATTRIB           lnAttrib;
	SCL_LNTYPE_ATTRIB       lnTypeAttrib;
	SCL_DOINLNTYPE_ATTRIB   doAttrib;
	SCL_DAINDOTYPE_ATTRIB   daAttrib;
	SCL_STR                 lnType;
	unsigned char           macAddr[GOOSEANALYSIS_MAC_SIZE];

	// ��ȡͨ������IED��Ϣ
	if ( !pCache->GetGSECommn(gooseAppID,conntAPAttrib,gseAttrib) )
		return -1;
	ConvMacWStr2Char(gseAttrib.macAddr,macAddr);
	if ( !IsMacAddrSame(gooseMAC,macAddr) )
		return -1;

	// ��ȡFCDA��ָ������������߼��ڵ����Ϣ
	if ( dataSetInfo.lnClass == _T("LLN0") )
	{
		if ( !pCache->GetLN0Attrib(conntAPAttrib.iedName,dataSetInfo.ldInst,ln0Attrib) )
			return -1;
		lnType = ln0Attrib.lnType;
	}
	else
	{
		SCL_STR lnName = dataSetInfo.lnPrefix+dataSetInfo.lnClass+dataSetInfo.lnInst;
		if ( !pCache->GetLNAttrib(conntAPAttrib.iedName,dataSetInfo.ldInst,lnName,lnAttrib) )
			return -1;
		lnType = lnAttrib.lnType;
	}

	if ( !pCache->GetDOAttrib(lnType,dataSetInfo.doName,doAttrib) )
		return -1;

	int subChannelNum = 0;
	if ( dataSetInfo.daName.empty() )
	{
		int i = 0;
		while ( pCache->GetDAAttrib(doAttrib.type,i,daAttrib) )
		{
			if ( daAttrib.fc.compare(dataSetInfo.daFc) == 0 )
				subChannelNum++;
			i++;
		}
	}
	else
	{
		if ( dataSetInfo.daBType == L"Struct" )
		{
			if ( !pCache->GetDAAttrib(doAttrib.type,dataSetInfo.daName,daAttrib) )
				return -1;

			SCL_BDAINDATYPE_ATTRIB bdaAttrib;
			while ( pCache->GetBDAAttrib(daAttrib.type,subChannelNum,bdaAttrib) )
			{
				subChannelNum++;
			}
		}
	}

	return subChannelNum;
}

///
/// @brief
///     ��ȡָ��MAC��APPID��GOOSE���ƿ�Ŀ��Ƶ�ĳһ��ͨ����ȫ����ͨ������Ϣ
///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
///
/// @param[in]
///     gooseMAC          - GOOSEͨ���鲥MAC��ַ
/// @param[in]
///     gooseAPPID        - GOOSEͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
// @param[in]
///     dataSetInfo       - ��ͨ����Ϣ
// @param[in]
///     subChannelInfoArr - ������ͨ����Ϣ������
// @param[in]
///     arrSize           - �����С
///
/// @return
///     ʵ�ʵ���ͨ���� - �����ɹ��� -1 - ����ʧ��
///
int SCLGOOSEAnalysis::GetAllIEDGOOSESubChannelInfo(
	const unsigned char *gooseMAC, 
	const unsigned short gooseAppID,
	const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
	SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfoArr,
	const int arrSize )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_CONNECTEDAP_ATTRIB  conntAPAttrib;
	SCL_GSE_ATTRIB          gseAttrib;
	SCL_LN0_ATTRIB          ln0Attrib;
	SCL_LN_ATTRIB           lnAttrib;
	SCL_LNTYPE_ATTRIB       lnTypeAttrib;
	SCL_DOINLNTYPE_ATTRIB   doAttrib;
	SCL_DAINDOTYPE_ATTRIB   daAttrib;
	SCL_STR                 lnType;
	unsigned char           macAddr[GOOSEANALYSIS_MAC_SIZE];

	if ( !pCache->GetGSECommn(gooseAppID,conntAPAttrib,gseAttrib) )
		return -1;
	ConvMacWStr2Char(gseAttrib.macAddr,macAddr);
	if ( !IsMacAddrSame(gooseMAC,macAddr) )
		return -1;
	// ��ȡFCDA��ָ������������߼��ڵ����Ϣ
	if ( dataSetInfo.lnClass == _T("LLN0") )
	{
		if ( !pCache->GetLN0Attrib(conntAPAttrib.iedName,dataSetInfo.ldInst,ln0Attrib) )
			return -1;
		lnType = ln0Attrib.lnType;
	}
	else
	{
		SCL_STR lnName = dataSetInfo.lnPrefix+dataSetInfo.lnClass+dataSetInfo.lnInst;
		if ( !pCache->GetLNAttrib(conntAPAttrib.iedName,dataSetInfo.ldInst,lnName,lnAttrib) )
			return -1;
		lnType = lnAttrib.lnType;
	}

	if ( !pCache->GetDOAttrib(lnType,dataSetInfo.doName,doAttrib) )
		return -1;
	
	int subChannelNum = 0;
	if ( dataSetInfo.daName.empty() )
	{
		int i = 0;
		while ( pCache->GetDAAttrib(doAttrib.type,i,daAttrib) )
		{
			if ( daAttrib.fc.compare(dataSetInfo.daFc) == 0 )
			{
				if ( subChannelNum >= arrSize )
					break;
				subChannelInfoArr[subChannelNum] = dataSetInfo;
				subChannelInfoArr[subChannelNum].idxSeq  = subChannelNum;
				subChannelInfoArr[subChannelNum].daName  = daAttrib.name;
				subChannelInfoArr[subChannelNum].daBType = daAttrib.bType;
				subChannelNum++;
			}
			i++;
		}
	}
	else
	{
		if ( dataSetInfo.daBType == L"Struct" )
		{
			if ( !pCache->GetDAAttrib(doAttrib.type,dataSetInfo.daName,daAttrib) )
				return -1;

			SCL_BDAINDATYPE_ATTRIB bdaAttrib;
			while ( pCache->GetBDAAttrib(daAttrib.type,subChannelNum,bdaAttrib) )
			{
				if ( subChannelNum >= arrSize )
					break;
				subChannelInfoArr[subChannelNum] = dataSetInfo;
				subChannelInfoArr[subChannelNum].idxSeq  = subChannelNum;
				subChannelInfoArr[subChannelNum].daName  = daAttrib.name+L"."+bdaAttrib.name;
				subChannelInfoArr[subChannelNum].daBType = bdaAttrib.bType;;
				subChannelNum++;
			}
		}
	}
	
	return subChannelNum;
}

///
/// @brief
///     ��ȡĳһIED�����󶨵��ⲿ�źţ�ExtRef����GOOSE�����źţ�������
///     ÿ��װ�õ�LLN0���ֺ��и�װ�õ�GOOSE�Ͳ���ֵ�����������Ϣ
///
/// @param[in]
///     iedName      - IED������
///
/// @return
///     ExtRef���������ɹ���  -1��ʧ��
///
int SCLGOOSEAnalysis::GetOneIEDInputsExtRefNum(const std::wstring &iedName)
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	return pCache->GetInputsExtRefNumInIED(iedName);
}

///
/// @brief
///     ��ȡָ��IED�����󶨵�ȫ���ⲿ�źŵ�������Ϣ
///
/// @param[in]
///     iedName    - IED����
/// @param[in]
///     inputsInfo - �����ⲿ�źŵ�������Ϣ������
/// @param[in]
///     arrSize    - �����ⲿ�źŵ�������Ϣ������Ĵ�С
///
/// @return
///     �����б������Ϣ������ - �����ɹ��� -1 - ����ʧ��
///
int SCLGOOSEAnalysis::GetOneIEDGOINExtRef(const std::wstring &iedName, SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO *inputsInfo, const int arrSize)
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_EXTREF_ATTRIB      *extRefAttrib = new SCL_EXTREF_ATTRIB[arrSize];
	if ( extRefAttrib == NULL )
		return -1;

	if ( !pCache->GetAllInputsExtRefInIED(iedName,extRefAttrib,arrSize) )
		goto errorHandle;

	int goInNum = 0;
	for (int i=0; i<arrSize; i++)
	{
		if ( extRefAttrib[i].intAddr.find(_T("GOIN")) != -1 )
		{
			if ( !SCL2InputsInfo(iedName,extRefAttrib[i],&inputsInfo[goInNum]) )
				goto errorHandle;
			goInNum++;
		}
	}

	delete[] extRefAttrib;
	return goInNum;

errorHandle:
	delete[] extRefAttrib;
	return -1;
}

///
/// @brief
///     ��ȡ����ĳһ�ⲿ�ź������GOOSE���ƿ����Ϣ
///
/// @param[in]
///     inputsInfo   - �ⲿ�źŵ�������Ϣ
/// @param[in]
///     iedGOOSEInfo - �ⲿ�ź���ص�GOOSE���ƿ����Ϣ
///
/// @return
///     0 - �����ɹ��� -1 - ����ʧ��
///
int SCLGOOSEAnalysis::GetOutAddrGOOSEInfo(const SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO &inputsInfo, SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfo)
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_IED_ATTRIB iedAttrib;
	SCL_ACCESS_POINT_ATTRIB apAttrib;
	SCL_LDEVICE_ATTRIB ldAttrib;
	SCL_DATASET_ATTRIB dataSetAttrib;
	SCL_FCDA_ATTRIB fcdaAttrib;
	SCL_CONNECTEDAP_ATTRIB conntAPAttirb;
	SCL_GSECTRL_ATTRIB gseCtrlAttrib;
	SCL_GSE_ATTRIB gseAttrib;

	// ��ȡIED��Ϣ
	if ( !pCache->GetIEDAttrib(inputsInfo.iedName_Out,iedAttrib) )
		return -1;

	// ��ȡLDevice��Ϣ
	if ( !pCache->GetLDeviceAttrib(inputsInfo.iedName_Out,inputsInfo.ldInst_Out,apAttrib,ldAttrib) )
		return -1;

	// ��ȡָ�����ƿ鼰ͨ�Ų�����Ϣ
	int dataSetNum = pCache->GetDataSetNumInLN0(inputsInfo.iedName_Out,inputsInfo.ldInst_Out);
	if ( dataSetNum <= 0 )
		return -1;
	for ( int i = 0; i < dataSetNum; i++)
	{
		if ( !pCache->GetDataSetAttribInLN0(inputsInfo.iedName_Out,inputsInfo.ldInst_Out,i,dataSetAttrib) )
			return -1;
		for ( int j = 0; j < dataSetAttrib.fcdaNum; j++ )
		{
			if ( !pCache->GetFCDAAttrib(inputsInfo.iedName_Out,inputsInfo.ldInst_Out,_T("LLN0"),dataSetAttrib.dataSetName,j,fcdaAttrib) )
				return -1;
			if ( ( inputsInfo.daName_Out == fcdaAttrib.daName ) &&
				 ( inputsInfo.doName_Out == fcdaAttrib.doName ) &&
				 ( inputsInfo.lnName_Out == ( fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst ) ) )
			{
				if ( pCache->GetGSECtrlAttribWithDatSet(inputsInfo.iedName_Out,inputsInfo.ldInst_Out,dataSetAttrib.dataSetName,gseCtrlAttrib))
				{
					if ( pCache->GetGSECommnWithIdx(inputsInfo.iedName_Out,inputsInfo.ldInst_Out,gseCtrlAttrib.cbName,conntAPAttirb,gseAttrib) )
					{
						SCL2IEDGOOSEInfo(gseAttrib,iedAttrib,apAttrib,ldAttrib,dataSetAttrib,gseCtrlAttrib,iedGOOSEInfo);
						return 0;
					}
				}
			}
		}
	}
	return -1;
}

///
/// @brief
///     ��ȡȫ����GOOSE���ƿ����Ŀ������δ����ͨ�Ų����Ŀ��ƿ飩
///
/// @return
///     ���ƿ����Ŀ�������ɹ���  -1��ʧ��
///
int SCLGOOSEAnalysis::GetAllIEDGOOSENumByCtrlRef( void )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	int ctrlNum = 0;
	int	iedNum = pCache->GetIEDNum();
	for ( int i = 0; i < iedNum; i++ )
	{
		SCL_IED_ATTRIB iedAttrib;
		if ( !pCache->GetIEDAttrib(i,iedAttrib) )
			continue;

		int ldNum = pCache->GetLDeviceNum(iedAttrib.iedName);
		for (int j = 0; j < ldNum; j++ )
		{
			SCL_ACCESS_POINT_ATTRIB apAttrib;
			SCL_LDEVICE_ATTRIB ldAttrib;
			if ( !pCache->GetLDeviceAttrib(iedAttrib.iedName,j,apAttrib,ldAttrib) )
				continue;
			ctrlNum += pCache->GetGSECtrlNumInLN0(iedAttrib.iedName,ldAttrib.ldInst);
		}		
	}

	return ctrlNum;
}

///
/// @brief
///     ��ȡȫ����GOOSE���ƿ����Ϣ������δ����ͨ�Ų����Ŀ��ƿ飩
///     ���У�δ���ò��ֽ���Ĭ������
///
/// @param[in]
///     iedGOOSEInfoArr - ����SMV���ƿ���Ϣ������
/// @param[in]
///     arrSize         - �����С
///
/// @return
///     �����б���Ŀ��ƿ�������������ɹ���  -1��ʧ��
///
int SCLGOOSEAnalysis::GetAllIEDGOOSEInfoByCtrlRef( SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfoArr,
												  const int arrSize )
{
	// �жϻ����Ƿ����
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	int ctrlNum = 0;
	int	iedNum = pCache->GetIEDNum();
	for ( int i = 0; i < iedNum; i++ )
	{
		if ( ctrlNum >= arrSize )
			break;

		// ��ȡIED��Ϣ
		SCL_IED_ATTRIB iedAttrib;
		if ( !pCache->GetIEDAttrib(i,iedAttrib) )
			continue;

		int ldNum = pCache->GetLDeviceNum(iedAttrib.iedName);
		for (int j = 0; j < ldNum; j++ )
		{
			if ( ctrlNum >= arrSize )
				break;

			// ��ȡLDevice��Ϣ
			SCL_ACCESS_POINT_ATTRIB apAttrib;
			SCL_LDEVICE_ATTRIB ldAttrib;
			if ( !pCache->GetLDeviceAttrib(iedAttrib.iedName,j,apAttrib,ldAttrib) )
				continue;

			int oneLdCtrlNum  = pCache->GetGSECtrlNumInLN0(iedAttrib.iedName,ldAttrib.ldInst);
			for ( int k = 0; k < oneLdCtrlNum; k++ )
			{
				if ( ctrlNum >= arrSize )
					break;

				// ��ȡ���ƿ���Ϣ
				SCL_GSECTRL_ATTRIB ctrlAttrib;
				if ( !pCache->GetGSECtrlAttribInLN0(iedAttrib.iedName,ldAttrib.ldInst,k,ctrlAttrib) )
					continue;

				// ��ȡ����ƿ���������ݼ�����Ϣ
				SCL_DATASET_ATTRIB dataSetAttrib;
				if ( !pCache->GetDataSetAttribInLN0(iedAttrib.iedName,ldAttrib.ldInst,ctrlAttrib.dataSet,dataSetAttrib) )
					continue;

				// ��ȡ����ƿ������ͨ�Ų�����Ϣ
				SCL_CONNECTEDAP_ATTRIB conntAPAttrib;
				SCL_GSE_ATTRIB commnAttrib;
				if ( !pCache->GetGSECommnWithIdx(iedAttrib.iedName,ldAttrib.ldInst,ctrlAttrib.cbName,
					conntAPAttrib,commnAttrib) )
				{
					// δ����ͨ�Ų�����������Ĭ��ֵ
					conntAPAttrib.iedName = iedAttrib.iedName;
					conntAPAttrib.apName  = apAttrib.apName;
					commnAttrib.ldInst    = ldAttrib.ldInst;
					commnAttrib.cbName    = ctrlAttrib.cbName;
					unsigned int appID    = 0xFFFF;
					ConvHex2WStr(&appID,commnAttrib.appID);
					unsigned char macAddr[]  = {0x01,0x0C,0xCD,0x01,0xFF,0xFF};
					ConvMacChar2WStr(macAddr,commnAttrib.macAddr);
					commnAttrib.vlanID       = L"0";
					commnAttrib.vlanPriority = L"4";
					commnAttrib.minTimeVal   = L"2";
					commnAttrib.minTimeUnit  = L"ms";
					commnAttrib.maxTimeVal   = L"5000";
					commnAttrib.maxTimeUnit  = L"ms";
					if ( !pCache->AddGSECommn(conntAPAttrib,commnAttrib) )
						return -1;
				}
				SCL2IEDGOOSEInfo(commnAttrib,iedAttrib,apAttrib,ldAttrib,dataSetAttrib,ctrlAttrib,&iedGOOSEInfoArr[ctrlNum]);
				ctrlNum++;
			}
		}		
	}
	return ctrlNum;
}

///
/// @brief
///     ����һ��GOOSE���ƿ�
///
/// @param[in]
///     iedGOOSEInfo - GOOSE���ƿ���Ϣ
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int SCLGOOSEAnalysis::SetOneIEDGOOSEInfoByCtrlRef(const SCDGOOSEANALYSIS_IEDGOOSE_INFO &iedGOOSEInfo)
{
	// �жϻ����Ƿ����
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// ���ÿ��ƿ���Ϣ
	SCL_GSECTRL_ATTRIB ctrlAttrib;
	ctrlAttrib.cbName = iedGOOSEInfo.cbName;
	ctrlAttrib.cbDesc = iedGOOSEInfo.cbDesc;
	ConvDec2WStr(&iedGOOSEInfo.confRev,ctrlAttrib.confRev);
	ctrlAttrib.gooseID = iedGOOSEInfo.gooseID;
	ctrlAttrib.dataSet = iedGOOSEInfo.dataSet;
	ctrlAttrib.type    = iedGOOSEInfo.type;
	if ( !pCache->SetGSECtrlAttribInLN0(iedGOOSEInfo.iedName,iedGOOSEInfo.iedLdInst,iedGOOSEInfo.cbName,ctrlAttrib) )
		return -1;

	// ���ÿ��ƿ��Ӧ��ͨ�Ų�����Ϣ
	SCL_CONNECTEDAP_ATTRIB conntAPAttrib;
	SCL_GSE_ATTRIB commnAttrib;
	conntAPAttrib.iedName = iedGOOSEInfo.iedName;
	conntAPAttrib.apName  = iedGOOSEInfo.iedAPName;
	commnAttrib.ldInst    = iedGOOSEInfo.iedLdInst;
	commnAttrib.cbName    = iedGOOSEInfo.cbName;
	ConvMacChar2WStr(iedGOOSEInfo.gooseMAC,commnAttrib.macAddr);

	unsigned int tmp = (unsigned int)iedGOOSEInfo.gooseAppID;
	ConvHex2WStr(&tmp,commnAttrib.appID);	
	tmp = (unsigned int)iedGOOSEInfo.gooseVlanID; 
	ConvHex2WStr(&tmp,commnAttrib.vlanID);
	tmp = (unsigned int)iedGOOSEInfo.gooseVlanPri;
	ConvDec2WStr(&tmp,commnAttrib.vlanPriority);
	ConvDec2WStr(&iedGOOSEInfo.gooseMinTime,commnAttrib.minTimeVal);
	ConvDec2WStr(&iedGOOSEInfo.gooseMaxTime,commnAttrib.maxTimeVal);
	commnAttrib.minTimeUnit = _T("ms");
	commnAttrib.maxTimeUnit = _T("ms");
	if ( !pCache->SetGSECommnWithIdx(iedGOOSEInfo.iedName,iedGOOSEInfo.iedLdInst,iedGOOSEInfo.cbName,
		conntAPAttrib,commnAttrib) )
		return -1;

	// ���ÿ��ƿ��Ӧ�����ݼ�����Ϣ
	SCL_DATASET_ATTRIB dataSetAttrib;
	dataSetAttrib.dataSetName = iedGOOSEInfo.dataSetName;
	dataSetAttrib.dataSetDesc = iedGOOSEInfo.dataSetDesc;
	dataSetAttrib.fcdaNum     = iedGOOSEInfo.channelNum;
	if ( !pCache->SetDataSetAttribInLN0(iedGOOSEInfo.iedName,iedGOOSEInfo.iedLdInst,iedGOOSEInfo.dataSetName,
		dataSetAttrib))
		return -1;

	return 0;
}

///
/// @brief
///     ��ȡָ��������ַ��GOOSE���ƿ������Ƶ�DataSet��ͨ����FCDA��������
///
/// @param[in]
///     iedName        - GOOSE���ƿ����ڵ�IED������
/// @param[in]
///     ldInst         - GOOSE���ƿ����ڵ�LDevice��Inst��
/// @param[in]
///     cbName         - GOOSE���ƿ������
///
/// @return
///     DataSet��FCDA�������������ɹ���  -1��ʧ��
///
int SCLGOOSEAnalysis::GetIEDGOOSEDataSetNum(const std::wstring &iedName, const std::wstring &ldInst, const std::wstring &cbName)
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_GSECTRL_ATTRIB      ctrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;

	if ( !pCache->GetGSECtrlAttribInLN0(iedName,ldInst,cbName,ctrlAttrib) )
		return -1;
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,ctrlAttrib.dataSet,dataSetAttrib) )
		return -1;

	return dataSetAttrib.fcdaNum;
}

///
/// @brief
///     ��ȡָ��������ַ��GOOSE���ƿ������Ƶ�DataSet������ͨ����FCDA����Ϣ
///
/// @param[in]
///     iedName        - GOOSE���ƿ����ڵ�IED������
/// @param[in]
///     ldInst         - GOOSE���ƿ����ڵ�LDevice��Inst��
/// @param[in]
///     cbName         - GOOSE���ƿ������
/// @param[in]
///     dataSetInfoArr - ����GOOSE��ͨ����Ϣ������
/// @param[in]
///     arrSize        - ������������
///
/// @return
///     ���鱣��ͨ����Ϣ�������������ɹ���  -1��ʧ��
///
int SCLGOOSEAnalysis::GetAllIEDGOOSEDataSetInfo( const std::wstring &iedName, const std::wstring &ldInst,
												 const std::wstring &cbName, SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr,
												 const int arrSize )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_GSECTRL_ATTRIB      gseCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	SCL_FCDA_ATTRIB         fcdaAttrib;
	SCL_LN0_ATTRIB          ln0Attrib;
	SCL_LN_ATTRIB           lnAttrib;
	SCL_DOI_ATTRIB          doiAttrib;
	SCL_DOINLNTYPE_ATTRIB   doAttrib;
	SCL_DAINDOTYPE_ATTRIB   daAttrib;
	SCL_BDAINDATYPE_ATTRIB  bdaAttrib;
	SCL_STR                 lnName;
	SCL_STR                 lnType;
	SCL_STR                 lnDesc;
	SCL_STR                 daName;
	SCL_STR                 bdaName;
	SCL_STR                 daBType;

	if ( !pCache->GetGSECtrlAttribInLN0(iedName,ldInst,cbName,gseCtrlAttrib) )
		return -1;
	// ��ȡ���ݼ���Ϣ
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,gseCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;

	int i = 0;
	for ( i = 0; i < dataSetAttrib.fcdaNum ; i++ )
	{
		if ( i >= arrSize )
			break;
		// ��ȡFCDA������Ϣ
		if ( !pCache->GetFCDAAttrib(iedName,ldInst,_T("LLN0"),gseCtrlAttrib.dataSet,i,fcdaAttrib) )
			return -1;
		lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
		// ��ȡFCDA��ָ������ݵ���Ϣ
		if ( !pCache->GetDOIAttribInLNorLN0(iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
		{
			doiAttrib.doName = fcdaAttrib.doName;
			doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
		}
		// ��ȡFCDA��ָ������������߼��ڵ����Ϣ
		if ( fcdaAttrib.lnClass == _T("LLN0") )
		{
			if ( !pCache->GetLN0Attrib(iedName,fcdaAttrib.ldInst,ln0Attrib) )
				return -1;
			lnType = ln0Attrib.lnType;
			lnDesc = ln0Attrib.lnDesc;
		}
		else
		{
			if ( !pCache->GetLNAttrib(iedName,fcdaAttrib.ldInst,lnName,lnAttrib) )
				return -1;
			lnType = lnAttrib.lnType;
			lnDesc = lnAttrib.lnDesc;
		}
		// ��ȡFCDA��ָ����������Ե�������Ϣ
		if ( !fcdaAttrib.daName.empty() )
		{
			if ( !pCache->GetDOAttrib(lnType,fcdaAttrib.doName,doAttrib) )
				return -1;
			int off = 0;
			int off_n = fcdaAttrib.daName.find(L".",off);
			if ( !pCache->GetDAAttrib(doAttrib.type,fcdaAttrib.daName.substr(off,off_n-off),daAttrib) )
				return -1;
			daBType = daAttrib.bType;

			SCL_STR daTypeID = daAttrib.type;
			SCL_BDAINDATYPE_ATTRIB bdaAttrib;
			while ( off_n != std::wstring::npos )
			{
				off = off_n + 1;
				off_n = fcdaAttrib.daName.find(L".",off);
				if ( !pCache->GetBDAAttrib(daTypeID,fcdaAttrib.daName.substr(off,off_n-off),bdaAttrib) )
					return -1;
				daTypeID = bdaAttrib.type;
				daBType  = bdaAttrib.bType;
			}
		}
		else
		{
			daBType = _T("");
		}
		SCL2DataSetInfo(dataSetAttrib,fcdaAttrib,lnDesc,doiAttrib,daBType,&dataSetInfoArr[i]);
	}

	return i;
}

///
/// @brief
///     ��ȡָ��������ַ��GOOSE���ƿ������Ƶ�DataSet��ĳһ˳��ŵ�ͨ����FCDA����Ϣ
///
/// @param[in]
///     iedName        - GOOSE���ƿ����ڵ�IED������
/// @param[in]
///     ldInst         - GOOSE���ƿ����ڵ�LDevice��Inst��
/// @param[in]
///     cbName         - GOOSE���ƿ������
/// @param[in]
///     idxSeq         - GOOSEͨ����˳���
/// @param[in]
///     dataSetInfo    - ָ�򱣴�GOOSE��ͨ����Ϣ�Ľṹ���ָ��
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int SCLGOOSEAnalysis::GetOneIEDGOOSEDataSetInfo(const std::wstring &iedName, const std::wstring &ldInst,
									            const std::wstring &cbName, const int idxSeq,
												SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo)
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_GSECTRL_ATTRIB      gseCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	SCL_FCDA_ATTRIB         fcdaAttrib;
	SCL_LN0_ATTRIB          ln0Attrib;
	SCL_LN_ATTRIB           lnAttrib;
	SCL_DOI_ATTRIB          doiAttrib;
	SCL_DOINLNTYPE_ATTRIB   doAttrib;
	SCL_DAINDOTYPE_ATTRIB   daAttrib;
	SCL_BDAINDATYPE_ATTRIB  bdaAttrib;
	SCL_STR                 lnName;
	SCL_STR                 lnType;
	SCL_STR                 lnDesc;
	SCL_STR                 daName;
	SCL_STR                 bdaName;
	SCL_STR                 daBType;

	if ( !pCache->GetGSECtrlAttribInLN0(iedName,ldInst,cbName,gseCtrlAttrib) )
		return -1;
	// ��ȡ���ݼ���Ϣ
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,gseCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;
	// ��ȡFCDA������Ϣ
	if ( !pCache->GetFCDAAttrib(iedName,ldInst,_T("LLN0"),gseCtrlAttrib.dataSet,idxSeq,fcdaAttrib) )
		return -1;
	lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
	// ��ȡFCDA��ָ������ݵ���Ϣ
	if ( !pCache->GetDOIAttribInLNorLN0(iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
	{
		doiAttrib.doName = fcdaAttrib.doName;
		doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
	}
	// ��ȡFCDA��ָ������������߼��ڵ����Ϣ
	if ( fcdaAttrib.lnClass == _T("LLN0") )
	{
		if ( !pCache->GetLN0Attrib(iedName,fcdaAttrib.ldInst,ln0Attrib) )
			return -1;
		lnType = ln0Attrib.lnType;
		lnDesc = ln0Attrib.lnDesc;
	}
	else
	{
		if ( !pCache->GetLNAttrib(iedName,fcdaAttrib.ldInst,lnName,lnAttrib) )
			return -1;
		lnType = lnAttrib.lnType;
		lnDesc = lnAttrib.lnDesc;
	}
	// ��ȡFCDA��ָ����������Ե�������Ϣ
	if ( !fcdaAttrib.daName.empty() )
	{
		if ( !pCache->GetDOAttrib(lnType,fcdaAttrib.doName,doAttrib) )
			return -1;
		int off = 0;
		int off_n = fcdaAttrib.daName.find(L".",off);
		if ( !pCache->GetDAAttrib(doAttrib.type,fcdaAttrib.daName.substr(off,off_n-off),daAttrib) )
			return -1;
		daBType = daAttrib.bType;

		SCL_STR daTypeID = daAttrib.type;
		SCL_BDAINDATYPE_ATTRIB bdaAttrib;
		while ( off_n != std::wstring::npos )
		{
			off = off_n + 1;
			off_n = fcdaAttrib.daName.find(L".",off);
			if ( !pCache->GetBDAAttrib(daTypeID,fcdaAttrib.daName.substr(off,off_n-off),bdaAttrib) )
				return -1;
			daTypeID = bdaAttrib.type;
			daBType  = bdaAttrib.bType;
		}
	}
	else
	{
		daBType = _T("");
	}
	SCL2DataSetInfo(dataSetAttrib,fcdaAttrib,lnDesc,doiAttrib,daBType,dataSetInfo);

	return 0;
}

///
/// @brief
///     ����Goose���ƿ����������ݼ��е�һ��ͨ����FCDA��
///     ע�⣬��ͨ����Ϣ�е�lnPrefix��lnClass��lnInst��doName��daName��daFc��daBType��dUsAddr���Ե�������Ч
///
/// @param[in]
///     iedName      - ��ͨ�����ڵ�IED������
/// @param[in]
///     ldInst       - ��ͨ�����ڵ�LDevice��Inst��
/// @param[in]
///     dataSetInfo  - ��ͨ����������������Ϣ
/// @param[in]
///     eChannelType - ��ͨ��������
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int SCLGOOSEAnalysis::SetOneIEDGOOSEDataSetInfo_Def( const std::wstring &iedName, const std::wstring &ldInst,
												 SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
												 CHANNEL_TYPE_GOOSE eChannelType )
{
	// �жϻ����Ƿ����
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_DATASET_ATTRIB dataSetAttrib;
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,dataSetInfo.dataSetName,dataSetAttrib) )
		return -1;

	// �Զ���ͨ��lnName��doName��daName
	SCL_STR lnPrefix = L"DEF_" + iedName + ldInst + L"_" + dataSetInfo.dataSetName;
	std::wstring str;
	ConvDec2WStr((unsigned int *)&dataSetInfo.idxSeq,str);
	SCL_STR lnInst = str;
	SCL_STR doName,daName;
	if ( eChannelType == CT_GOOSE_STRUCT )
	{
		doName = lnPrefix + L"_Do_" + str;
		daName = lnPrefix + L"_Da_" + str;
	}
	else
	{
		doName = L"Def";
		daName = GOOSE_CHANNEL_TYPE_TEMPLATE[eChannelType].name;
	}

	// ����FCDA��Ϣ
	SCL_FCDA_ATTRIB fcdaAttrib;
	fcdaAttrib.idxSeq   = dataSetInfo.idxSeq;
	fcdaAttrib.ldInst   = dataSetInfo.ldInst;
	fcdaAttrib.lnPrefix = dataSetInfo.lnPrefix = lnPrefix;
	fcdaAttrib.lnClass  = dataSetInfo.lnClass  = L"GGIO_EX";
	fcdaAttrib.lnInst   = dataSetInfo.lnInst   = lnInst;
	fcdaAttrib.fc       = dataSetInfo.daFc     = L"ST";
	fcdaAttrib.doName   = dataSetInfo.doName   = doName;
	fcdaAttrib.daName   = dataSetInfo.daName   = daName;
	dataSetInfo.daBType = GOOSE_CHANNEL_TYPE_TEMPLATE[eChannelType].bType;

	// ����DataSet�е�ͨ��������Ϣ����FCDA��
	if ( dataSetInfo.idxSeq >= dataSetAttrib.fcdaNum )
	{
		return -1;
	}
	else
	{
		if ( !pCache->SetFCDAAttrib(iedName,ldInst,_T("LLN0"),dataSetInfo.dataSetName,dataSetInfo.idxSeq,fcdaAttrib) )
			return -1;
	}

	// ����LNode��Ϣ�������������;��ȡLNode������Ϣ
	SCL_STR lnType;
	SCL_STR lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
	if ( fcdaAttrib.lnClass == _T("LLN0") )
	{
		SCL_LN0_ATTRIB ln0Attrib;
		if ( !pCache->GetLN0Attrib(iedName,ldInst,ln0Attrib) )
		{
			ln0Attrib.lnClass = dataSetInfo.lnClass;
			ln0Attrib.lnInst  = dataSetInfo.lnInst;
			ln0Attrib.lnType  = _T("XJ_LNODE_DEF");
			ln0Attrib.lnDesc  = dataSetInfo.lnDesc;
			if ( !pCache->AddLN0IntoLD(iedName,ldInst,ln0Attrib) )
				return -1;
		}
		else
		{
			ln0Attrib.lnDesc  = dataSetInfo.lnDesc;
			if ( !pCache->SetLN0Attrib(iedName,ldInst,ln0Attrib) )
				return -1;
		}
		lnType = ln0Attrib.lnType;
	}
	else
	{
		SCL_LN_ATTRIB lnAttrib;
		if ( !pCache->GetLNAttrib(iedName,ldInst,lnName,lnAttrib) )
		{
			lnAttrib.lnPrefix = dataSetInfo.lnPrefix;
			lnAttrib.lnClass  = dataSetInfo.lnClass;
			lnAttrib.lnInst   = dataSetInfo.lnInst;
			lnAttrib.lnType   = _T("XJ_LNODE_DEF");
			lnAttrib.lnDesc   = dataSetInfo.lnDesc;
			if ( !pCache->AddLNIntoLD(iedName,ldInst,lnAttrib) )
				return -1;
		}
		else
		{
			lnAttrib.lnDesc  = dataSetInfo.lnDesc;
			if ( !pCache->SetLNAttrib(iedName,ldInst,lnName,lnAttrib) )
				return -1;
		}
		lnType = lnAttrib.lnType;
	}

	// ����DOI��Ϣ�������������
	SCL_DOI_ATTRIB doiAttrib;
	if ( !pCache->GetDOIAttribInLNorLN0(iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
	{
		doiAttrib.doName  = dataSetInfo.doName;
		doiAttrib.doDesc  = dataSetInfo.doDesc;
		doiAttrib.dUVal   = dataSetInfo.dUVal;
		doiAttrib.dUSAddr = dataSetInfo.dUsAddr = _T("");
		if ( lnName == _T("LLN0") )
		{
			if ( !pCache->AddDOIIntoLN0(iedName,fcdaAttrib.ldInst,doiAttrib) )
				return -1;
		}
		else
		{
			if ( !pCache->AddDOIIntoLN(iedName,fcdaAttrib.ldInst,lnName,doiAttrib) )
				return -1;
		}
	}
	else
	{
		doiAttrib.doDesc = dataSetInfo.doDesc;
		doiAttrib.dUVal  = dataSetInfo.dUVal;
		if ( !pCache->SetDOIAttribInLNorLN0(iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
			return -1;
	}

	// ����ͨ�����ݵ�LNType�У���������ģ��Ϊ�Զ���ģ��
	if ( AddDOIntoDefLNodeType(pCache,dataSetInfo,lnType,eChannelType) == -1 )
		return -1;

	return 0;
}

///
/// @brief
///     ��Goose���ƿ����������ݼ�ĩβ���һ��ͨ����FCDA��
///     ע�⣬��ͨ����Ϣ�е�lnPrefix��lnClass��lnInst��doName��daName��daFc��daBType��dUsAddr���Ե�������Ч
///
/// @param[in]
///     iedName      - ��ͨ�����ڵ�IED������
/// @param[in]
///     ldInst       - ��ͨ�����ڵ�LDevice��Inst��
/// @param[in]
///     dataSetInfo  - ��ͨ����������������Ϣ
/// @param[in]
///     eChannelType - ��ͨ��������
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int SCLGOOSEAnalysis::AddOneIEDGOOSEDataSetInfo_Def( const std::wstring &iedName, const std::wstring &ldInst,
													SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
													CHANNEL_TYPE_GOOSE eChannelType )
{
	// �жϻ����Ƿ����
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// ����DataSet�е�ͨ��������Ϣ����FCDA���������������
	SCL_DATASET_ATTRIB dataSetAttrib;
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,dataSetInfo.dataSetName,dataSetAttrib) )
	{
		dataSetAttrib.dataSetName = dataSetInfo.dataSetName;
		dataSetAttrib.fcdaNum = 0;
		if ( !pCache->AddDataSetIntoLN0(iedName,ldInst,dataSetAttrib) )
			return -1;
	}

	// �Զ���ͨ��lnName��doName��daName
	SCL_STR lnPrefix = L"DEF_" + iedName + ldInst + L"_" + dataSetInfo.dataSetName;
	std::wstring str;
	ConvDec2WStr((unsigned int *)&dataSetInfo.idxSeq,str);
	SCL_STR lnInst = str;
	SCL_STR doName,daName;
	if ( eChannelType == CT_GOOSE_STRUCT )
	{
		doName = lnPrefix + L"_Do_" + str;
		daName = lnPrefix + L"_Da_" + str;
	}
	else
	{
		doName = L"Def";
		daName = GOOSE_CHANNEL_TYPE_TEMPLATE[eChannelType].name;
	}

	SCL_FCDA_ATTRIB fcdaAttrib;
	fcdaAttrib.idxSeq   = dataSetAttrib.fcdaNum;
	fcdaAttrib.ldInst   = dataSetInfo.ldInst;
	fcdaAttrib.lnPrefix = dataSetInfo.lnPrefix = lnPrefix;
	fcdaAttrib.lnClass  = dataSetInfo.lnClass  = L"GGIO_EX";
	fcdaAttrib.lnInst   = dataSetInfo.lnInst   = lnInst;
	fcdaAttrib.fc       = dataSetInfo.daFc     = L"ST";
	fcdaAttrib.doName   = dataSetInfo.doName   = doName;
	fcdaAttrib.daName   = dataSetInfo.daName   = daName;
	dataSetInfo.daBType = GOOSE_CHANNEL_TYPE_TEMPLATE[eChannelType].bType;

	// ���FCDA
	if ( !pCache->AddFCDAIntoDatSet(iedName,ldInst,_T("LLN0"),dataSetInfo.dataSetName,fcdaAttrib) )
		return -1;

	// �������ݼ���Ϣ
	dataSetAttrib.fcdaNum++;
	if ( !pCache->SetDataSetAttribInLN0(iedName,ldInst,dataSetAttrib.dataSetName,dataSetAttrib) )
		return -1;

	// ����LNode��Ϣ�������������;��ȡLNode������Ϣ
	SCL_STR lnType;
	SCL_STR lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
	if ( fcdaAttrib.lnClass == _T("LLN0") )
	{
		SCL_LN0_ATTRIB ln0Attrib;
		if ( !pCache->GetLN0Attrib(iedName,ldInst,ln0Attrib) )
		{
			ln0Attrib.lnClass = dataSetInfo.lnClass;
			ln0Attrib.lnInst  = dataSetInfo.lnInst;
			ln0Attrib.lnType  = _T("XJ_LNODE_DEF");
			ln0Attrib.lnDesc  = dataSetInfo.lnDesc;
			if ( !pCache->AddLN0IntoLD(iedName,ldInst,ln0Attrib) )
				return -1;
		}
		else
		{
			ln0Attrib.lnDesc  = dataSetInfo.lnDesc;
			if ( !pCache->SetLN0Attrib(iedName,ldInst,ln0Attrib) )
				return -1;
		}
		lnType = ln0Attrib.lnType;
	}
	else
	{
		SCL_LN_ATTRIB lnAttrib;
		if ( !pCache->GetLNAttrib(iedName,ldInst,lnName,lnAttrib) )
		{
			lnAttrib.lnPrefix = dataSetInfo.lnPrefix;
			lnAttrib.lnClass  = dataSetInfo.lnClass;
			lnAttrib.lnInst   = dataSetInfo.lnInst;
			lnAttrib.lnType   = _T("XJ_LNODE_DEF");
			lnAttrib.lnDesc   = dataSetInfo.lnDesc;
			if ( !pCache->AddLNIntoLD(iedName,ldInst,lnAttrib) )
				return -1;
		}
		else
		{
			lnAttrib.lnDesc  = dataSetInfo.lnDesc;
			if ( !pCache->SetLNAttrib(iedName,ldInst,lnName,lnAttrib) )
				return -1;
		}
		lnType = lnAttrib.lnType;
	}

	// ����DOI��Ϣ�������������
	SCL_DOI_ATTRIB doiAttrib;
	if ( !pCache->GetDOIAttribInLNorLN0(iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
	{
		doiAttrib.doName  = dataSetInfo.doName;
		doiAttrib.doDesc  = dataSetInfo.doDesc;
		doiAttrib.dUVal   = dataSetInfo.dUVal;
		doiAttrib.dUSAddr = dataSetInfo.dUsAddr = _T("");
		if ( lnName == _T("LLN0") )
		{
			if ( !pCache->AddDOIIntoLN0(iedName,fcdaAttrib.ldInst,doiAttrib) )
				return -1;
		}
		else
		{
			if ( !pCache->AddDOIIntoLN(iedName,fcdaAttrib.ldInst,lnName,doiAttrib) )
				return -1;
		}
	}
	else
	{
		doiAttrib.doDesc = dataSetInfo.doDesc;
		doiAttrib.dUVal  = dataSetInfo.dUVal;
		if ( !pCache->SetDOIAttribInLNorLN0(iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
			return -1;
	}

	// ����ͨ�����ݵ�LNType�У���������ģ��Ϊ�Զ���ģ��
	if ( AddDOIntoDefLNodeType(pCache,dataSetInfo,lnType,eChannelType) == -1 )
		return -1;

	return 0;
}

///
/// @brief
///     ��GOOSEͨ������ĩβ������ͨ������ĳһͨ������Ϣ����n��
///
/// @param[in]
///     iedName      - ͨ�����ڵ�IED������
/// @param[in]
///     ldInst       - ͨ�����ڵ�LDevice��Inst��
/// @param[in]
///     dataSetName  - ͨ�����ڵ����ݼ�������
/// @param[in]
///     idxSeq       - Ҫ������ͨ����˳��ţ���0��ʼ��
/// @param[in]
///     iCopyCount   - ��������Ŀ
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int SCLGOOSEAnalysis::CopyIEDGOOSEDataSetInfoToSetEnd(const std::wstring &iedName, const std::wstring &ldInst,
													  const std::wstring &dataSetName, int idxSeq, int iCopyCount)
{
	// �жϻ����Ƿ����
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// ����DataSet�е�ͨ��������Ϣ����FCDA���������������
	SCL_DATASET_ATTRIB dataSetAttrib;
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,dataSetName,dataSetAttrib) )
		return -1;

	if ( idxSeq >= dataSetAttrib.fcdaNum )
		return -1;
	SCL_FCDA_ATTRIB fcdaAttrib;
	if ( !pCache->GetFCDAAttrib(iedName,ldInst,_T("LLN0"),dataSetName,idxSeq,fcdaAttrib) )
		return -1;
	// ���FCDA
	for ( int i = 0; i < iCopyCount; i++ )
	{
		fcdaAttrib.idxSeq = i + dataSetAttrib.fcdaNum;
		if ( !pCache->AddFCDAIntoDatSet(iedName,ldInst,_T("LLN0"),dataSetName,fcdaAttrib) )
			return -1;
	}
	// ����ͨ����
	dataSetAttrib.fcdaNum += iCopyCount;
	if ( !pCache->SetDataSetAttribInLN0(iedName,ldInst,dataSetName,dataSetAttrib) )
		return -1;

	return 0;
}

///
/// @brief
///     �Ƴ�Goose���ƿ����������ݼ��е�һ��ͨ����FCDA��
///
/// @param[in]
///     iedName    - ��ͨ�����ڵ�IED������
/// @param[in]
///     ldInst     - ��ͨ�����ڵ�LDevice��Inst��
/// @param[in]
///     datSetName - ��ͨ�����ڵ�DataSet������
/// @param[in]
///     idxSeq     - ��ͨ����˳���
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int SCLGOOSEAnalysis::RemoveOneIEDGOOSEDataSetInfo(const std::wstring &iedName, const std::wstring &ldInst,
												   const std::wstring &datSetName, const int idxSeq)
{
	// �жϻ����Ƿ����
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	if ( !pCache->RemoveFCDAAttrib(iedName,ldInst,_T("LLN0"),datSetName,idxSeq) )
		return -1;

	// ���ÿ��ƿ��Ӧ�����ݼ���ͨ����Ŀ��Ϣ
	SCL_DATASET_ATTRIB dataSetAttrib;
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,datSetName,dataSetAttrib))
		return -1;
	dataSetAttrib.fcdaNum--;
	if ( !pCache->SetDataSetAttribInLN0(iedName,ldInst,datSetName,dataSetAttrib))
		return -1;

	return 0;
}

///
/// @brief
///     ��ȡָ��������ַ��GOOSE���ƿ�Ŀ��Ƶ�ĳһ��ͨ������ͨ������Ŀ
///
/// @param[in]
///     iedName        - ����ͨ�����ڵ�IED������
/// @param[in]
///     ldInst         - ����ͨ�����ڵ�LDevice��Inst��
// @param[in]
///     dataSetInfo    - ��ͨ����Ϣ
///
/// @return
///     ��ͨ����Ŀ - �����ɹ��� -1 - ����ʧ��
///
int SCLGOOSEAnalysis::GetIEDGOOSESubChannelNum(const std::wstring &iedName, const std::wstring &ldInst,
									           const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo)
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_LN0_ATTRIB          ln0Attrib;
	SCL_LN_ATTRIB           lnAttrib;
	SCL_LNTYPE_ATTRIB       lnTypeAttrib;
	SCL_DOINLNTYPE_ATTRIB   doAttrib;
	SCL_DAINDOTYPE_ATTRIB   daAttrib;
	SCL_STR                 lnType;

	// ��ȡFCDA��ָ������������߼��ڵ����Ϣ
	if ( dataSetInfo.lnClass == _T("LLN0") )
	{
		if ( !pCache->GetLN0Attrib(iedName,dataSetInfo.ldInst,ln0Attrib) )
			return -1;
		lnType = ln0Attrib.lnType;
	}
	else
	{
		SCL_STR lnName = dataSetInfo.lnPrefix+dataSetInfo.lnClass+dataSetInfo.lnInst;
		if ( !pCache->GetLNAttrib(iedName,dataSetInfo.ldInst,lnName,lnAttrib) )
			return -1;
		lnType = lnAttrib.lnType;
	}

	if ( !pCache->GetDOAttrib(lnType,dataSetInfo.doName,doAttrib) )
		return -1;

	int subChannelNum = 0;
	if ( dataSetInfo.daName.empty() )
	{
		int i = 0;
		while ( pCache->GetDAAttrib(doAttrib.type,i,daAttrib) )
		{
			if ( daAttrib.fc.compare(dataSetInfo.daFc) == 0 )
				subChannelNum++;
			i++;
		}
	}
	else
	{
		if ( dataSetInfo.daBType == L"Struct" )
		{
			int off = 0;
			int off_n = dataSetInfo.daName.find(L".",off);
			if ( !pCache->GetDAAttrib(doAttrib.type,dataSetInfo.daName.substr(off,off_n-off),daAttrib) )
				return -1;
			SCL_STR daTypeID = daAttrib.type;
			
			SCL_BDAINDATYPE_ATTRIB bdaAttrib;
			while ( off_n != std::wstring::npos )
			{
				off = off_n + 1;
				off_n = dataSetInfo.daName.find(L".",off);
				if ( !pCache->GetBDAAttrib(daTypeID,dataSetInfo.daName.substr(off,off_n-off),bdaAttrib) )
					return -1;
				daTypeID = bdaAttrib.type;
			}

			while ( pCache->GetBDAAttrib(daTypeID,subChannelNum,bdaAttrib) )
			{
				subChannelNum++;
			}
		}
	}

	return subChannelNum;
}

///
/// @brief
///     ��ȡָ��������ַ��GOOSE���ƿ�Ŀ��Ƶ�ĳһ��ͨ����ȫ����ͨ������Ϣ
///
/// @param[in]
///     iedName           - ����ͨ�����ڵ�IED������
/// @param[in]
///     ldInst            - ����ͨ�����ڵ�LDevice��Inst��
// @param[in]
///     dataSetInfo       - ��ͨ����Ϣ
// @param[in]
///     subChannelInfoArr - ������ͨ����Ϣ������
// @param[in]
///     arrSize           - �����С
///
/// @return
///     ʵ�ʵ���ͨ���� - �����ɹ��� -1 - ����ʧ��
///
int SCLGOOSEAnalysis::GetAllIEDGOOSESubChannelInfo(const std::wstring &iedName, const std::wstring &ldInst,
	                                               const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
	                                               SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfoArr,
	                                               const int arrSize )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_LN0_ATTRIB          ln0Attrib;
	SCL_LN_ATTRIB           lnAttrib;
	SCL_LNTYPE_ATTRIB       lnTypeAttrib;
	SCL_DOINLNTYPE_ATTRIB   doAttrib;
	SCL_DAINDOTYPE_ATTRIB   daAttrib;
	SCL_STR                 lnType;

	// ��ȡFCDA��ָ������������߼��ڵ����Ϣ
	if ( dataSetInfo.lnClass == _T("LLN0") )
	{
		if ( !pCache->GetLN0Attrib(iedName,dataSetInfo.ldInst,ln0Attrib) )
			return -1;
		lnType = ln0Attrib.lnType;
	}
	else
	{
		SCL_STR lnName = dataSetInfo.lnPrefix+dataSetInfo.lnClass+dataSetInfo.lnInst;
		if ( !pCache->GetLNAttrib(iedName,dataSetInfo.ldInst,lnName,lnAttrib) )
			return -1;
		lnType = lnAttrib.lnType;
	}

	if ( !pCache->GetDOAttrib(lnType,dataSetInfo.doName,doAttrib) )
		return -1;

	int subChannelNum = 0;
	if ( dataSetInfo.daName.empty() )
	{
		int i = 0;
		while ( pCache->GetDAAttrib(doAttrib.type,i,daAttrib) )
		{
			if ( daAttrib.fc.compare(dataSetInfo.daFc) == 0 )
			{
				if ( subChannelNum >= arrSize )
					break;
				subChannelInfoArr[subChannelNum] = dataSetInfo;
				subChannelInfoArr[subChannelNum].idxSeq  = subChannelNum;
				subChannelInfoArr[subChannelNum].daName  = daAttrib.name;
				subChannelInfoArr[subChannelNum].daBType = daAttrib.bType;
				subChannelNum++;
			}
			i++;
		}
	}
	else
	{
		if ( dataSetInfo.daBType == L"Struct" )
		{
			int off = 0;
			int off_n = dataSetInfo.daName.find(L".",off);
			if ( !pCache->GetDAAttrib(doAttrib.type,dataSetInfo.daName.substr(off,off_n-off),daAttrib) )
				return -1;
			SCL_STR daTypeID = daAttrib.type;

			SCL_BDAINDATYPE_ATTRIB bdaAttrib;
			while ( off_n != std::wstring::npos )
			{
				off = off_n + 1;
				off_n = dataSetInfo.daName.find(L".",off);
				if ( !pCache->GetBDAAttrib(daTypeID,dataSetInfo.daName.substr(off,off_n-off),bdaAttrib) )
					return -1;
				daTypeID = bdaAttrib.type;
			}

			while ( pCache->GetBDAAttrib(daTypeID,subChannelNum,bdaAttrib) )
			{
				if ( subChannelNum >= arrSize )
					break;
				subChannelInfoArr[subChannelNum] = dataSetInfo;
				subChannelInfoArr[subChannelNum].idxSeq  = subChannelNum;
				subChannelInfoArr[subChannelNum].daName  = dataSetInfo.daName+L"."+bdaAttrib.name;
				subChannelInfoArr[subChannelNum].daBType = bdaAttrib.bType;
				subChannelNum++;
			}
		}
	}

	return subChannelNum;
}

///
/// @brief
///     ��ȡָ��������ַ��GOOSE���ƿ�Ŀ��Ƶ�ĳһ��ͨ����ĳһ��ͨ������Ϣ
///
/// @param[in]
///     iedName           - ����ͨ�����ڵ�IED������
/// @param[in]
///     ldInst            - ����ͨ�����ڵ�LDevice��Inst��
// @param[in]
///     dataSetInfo       - ��ͨ����Ϣ
// @param[in]
///     idxSeq            - ��ͨ����˳��ţ���0��ʼ��
// @param[in]
///     subChannelInfo    - ������ͨ����Ϣ������
///
/// @return
///     0 - �����ɹ��� -1 - ����ʧ��
///
int SCLGOOSEAnalysis::GetOneIEDGOOSESubChannelInfo(const std::wstring &iedName, const std::wstring &ldInst,
												   const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
												   int idxSeq,
												   SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &subChannelInfo )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_LN0_ATTRIB          ln0Attrib;
	SCL_LN_ATTRIB           lnAttrib;
	SCL_LNTYPE_ATTRIB       lnTypeAttrib;
	SCL_DOINLNTYPE_ATTRIB   doAttrib;
	SCL_DAINDOTYPE_ATTRIB   daAttrib;
	SCL_STR                 lnType;

	// ��ȡFCDA��ָ������������߼��ڵ����Ϣ
	if ( dataSetInfo.lnClass == _T("LLN0") )
	{
		if ( !pCache->GetLN0Attrib(iedName,dataSetInfo.ldInst,ln0Attrib) )
			return -1;
		lnType = ln0Attrib.lnType;
	}
	else
	{
		SCL_STR lnName = dataSetInfo.lnPrefix+dataSetInfo.lnClass+dataSetInfo.lnInst;
		if ( !pCache->GetLNAttrib(iedName,dataSetInfo.ldInst,lnName,lnAttrib) )
			return -1;
		lnType = lnAttrib.lnType;
	}

	if ( !pCache->GetDOAttrib(lnType,dataSetInfo.doName,doAttrib) )
		return -1;

	int subChannelNum = 0;
	if ( dataSetInfo.daName.empty() )
	{
		int i = 0;
		while ( pCache->GetDAAttrib(doAttrib.type,i,daAttrib) )
		{
			if ( daAttrib.fc.compare(dataSetInfo.daFc) == 0 )
			{
				if ( subChannelNum == idxSeq  )
				{
					subChannelInfo = dataSetInfo;
					subChannelInfo.idxSeq  = subChannelNum;
					subChannelInfo.daName  = daAttrib.name;
					subChannelInfo.daBType = daAttrib.bType;
					return 0;
				}
				subChannelNum++;
			}
			i++;
		}
	}
	else
	{
		if ( dataSetInfo.daBType == L"Struct" )
		{
			int off = 0;
			int off_n = dataSetInfo.daName.find(L".",off);
			if ( !pCache->GetDAAttrib(doAttrib.type,dataSetInfo.daName.substr(off,off_n-off),daAttrib) )
				return -1;
			SCL_STR daTypeID = daAttrib.type;

			SCL_BDAINDATYPE_ATTRIB bdaAttrib;
			while ( off_n != std::wstring::npos )
			{
				off = off_n + 1;
				off_n = dataSetInfo.daName.find(L".",off);
				if ( !pCache->GetBDAAttrib(daTypeID,dataSetInfo.daName.substr(off,off_n-off),bdaAttrib) )
					return -1;
				daTypeID = bdaAttrib.type;
			}

			if ( pCache->GetBDAAttrib(daTypeID,idxSeq,bdaAttrib) )
			{
				subChannelInfo = dataSetInfo;
				subChannelInfo.idxSeq  = subChannelNum;
				subChannelInfo.daName  = dataSetInfo.daName+L"."+bdaAttrib.name;
				subChannelInfo.daBType = bdaAttrib.bType;
				return 0;
			}
		}
	}

	return -1;
}

///
/// @brief
///     ����Goose���ƿ����������ݼ��е�ĳһͨ���е�һ����ͨ��
///     ע�⣬��ͨ��Ϊ���Զ���ͨ��ʱ���Զ�ת��Ϊ�Զ���ͨ��
///
/// @param[in]
///     iedName           - ����ͨ�����ڵ�IED������
/// @param[in]
///     ldInst            - ����ͨ�����ڵ�LDevice��Inst��
/// @param[in]
///     channelInfo       - ����ͨ������Ӧ������ͨ����Ϣ�������ƿ����������ݼ��е�ĳһͨ����
/// @param[in]
///     subChannelIdxList - �Ӷ���ͨ���µĵ�һ����ͨ����ʼ����ǰ��ͨ��������ͨ����˳����б�
/// @param[in]
///     listSize          - ˳����б�Ĵ�С
/// @param[in]
///     eChannelType      - ����ͨ����Ҫ���õ�ͨ�����ͣ����㡢˫��...��
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int SCLGOOSEAnalysis::SetOneIEDGOOSESubChannelInfo_Def(const std::wstring &iedName, const std::wstring &ldInst,
												   SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
												   const int *subChannelIdxList, int listSize,
												   CHANNEL_TYPE_GOOSE eChannelType)
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// ��������ͨ�����Զ�������ģ�壬��ԭͨ��һ��
	SCL_STR daTypeID_Next;
	if ( CreateDefSubChannel(pCache,iedName,ldInst,channelInfo,subChannelIdxList,listSize-1,daTypeID_Next) == -1 )
		return -1;

	return SetBDAInDaType(pCache,daTypeID_Next,subChannelIdxList[listSize-1],eChannelType);
}

///
/// @brief
///     ��Gooseͨ����ĳһ��ͨ����ĩβ��������ͨ�����е�һ����ͨ������n��
///
/// @param[in]
///     iedName           - ����ͨ�����ڵ�IED������
/// @param[in]
///     ldInst            - ����ͨ�����ڵ�LDevice��Inst��
/// @param[in]
///     channelInfo       - ����ͨ������Ӧ������ͨ����Ϣ�������ƿ����������ݼ��е�ĳһͨ����
/// @param[in]
///     subChannelIdxList - �Ӷ���ͨ���µĵ�һ����ͨ����ʼ����ǰ��ͨ��������ͨ����˳����б�
/// @param[in]
///     listSize          - ˳����б�Ĵ�С
/// @param[in]
///     iCopyCount        - ����������
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int SCLGOOSEAnalysis::CopyIEDGooseSubChannelToEnd(const std::wstring &iedName, const std::wstring &ldInst,
												  SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
												  const int *subChannelIdxList, int listSize ,int iCopyCount)
{
	if ( subChannelIdxList == NULL )
		return -1;
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// ��������ͨ�����Զ�������ģ�壬��ԭͨ��һ��
	SCL_STR daTypeID_Next;
	if ( CreateDefSubChannel(pCache,iedName,ldInst,channelInfo,subChannelIdxList,listSize-1,daTypeID_Next) == -1 )
		return -1;

	// ��ȡָ������ͨ������Ϣ
	SCL_BDAINDATYPE_ATTRIB bdaAttrib;
	if ( !pCache->GetBDAAttrib(daTypeID_Next,subChannelIdxList[listSize-1],bdaAttrib) )
		return -1;
	// �����ͨ��
	for ( int i = 0; i < iCopyCount; i++ )
	{
		std::wstring str;
		ConvDec2WStr((unsigned int *)&i,str);
		bdaAttrib.name += L"_"+str;
		if ( !pCache->AddBDAIntoDAType(daTypeID_Next,bdaAttrib) )
			return -1;
	}

	return 0;
}

///
/// @brief
///     �Ƴ�Goose���ƿ����������ݼ��е�ĳһͨ���е�һ����ͨ��
///
/// @param[in]
///     iedName           - ����ͨ�����ڵ�IED������
/// @param[in]
///     ldInst            - ����ͨ�����ڵ�LDevice��Inst��
/// @param[in]
///     channelInfo       - ����ͨ������Ӧ������ͨ����Ϣ�������ƿ����������ݼ��е�ĳһͨ����
/// @param[in]
///     subChannelIdxList - �Ӷ���ͨ���µĵ�һ����ͨ����ʼ����ǰ��ͨ��������ͨ����˳����б�
/// @param[in]
///     listSize          - ˳����б�Ĵ�С
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int SCLGOOSEAnalysis::RemoveOneIEDGOOSESubChannelInfo(const std::wstring &iedName, const std::wstring &ldInst,
													  SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
													  const int *subChannelIdxList, int listSize)
{
	if ( subChannelIdxList == NULL )
		return -1;
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// ��������ͨ�����Զ�������ģ�壬��ԭͨ��һ��
	SCL_STR daTypeID_Next;
	if ( CreateDefSubChannel(pCache,iedName,ldInst,channelInfo,subChannelIdxList,listSize-1,daTypeID_Next) == -1 )
		return -1;

	// �Ƴ���ͨ������Ϣ
	if ( !pCache->RemoveBDAAttrib(daTypeID_Next,subChannelIdxList[listSize-1]) )
		return -1;

	return 0;
}

///
/// @brief
///     ��ȡͨ����������Ϣ
///
/// @param[in]
///     stGooseChannelInfo - goose���ݼ��е�����ͨ����Ϣ
///
/// @return
///     ���ض�Ӧ��ͨ��������Ϣ ����/˫��/ʱ��...
///
CHANNEL_TYPE_GOOSE SCLGOOSEAnalysis::GetChannelType(const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO& channelInfo)
{
	CHANNEL_TYPE_GOOSE enChannelType = CT_GOOSE_UNKNOW;
	if ( (channelInfo.daBType.empty() && channelInfo.daName.empty()) || channelInfo.daBType == L"Struct" )
	{
		enChannelType = CT_GOOSE_STRUCT;
	}
	else if (channelInfo.daBType == L"BOOLEAN")
	{
		enChannelType = CT_GOOSE_POINT;
	}
	else if (channelInfo.daBType == L"Dbpos")
	{
		enChannelType = CT_GOOSE_DOUBLE_POINT;
	}
	else if (channelInfo.daBType == L"Quality")
	{
		enChannelType = CT_GOOSE_QUALITY;
	}
	else if (channelInfo.daBType == L"Timestamp")
	{
		enChannelType = CT_GOOSE_TIME_STAMP;
	}
	else if (channelInfo.daBType == L"EntryTime")
	{
		enChannelType = CT_GOOSE_ENTRY_TIME;
	}
	else if (channelInfo.daBType == L"INT8")
	{
		enChannelType = CT_GOOSE_INT8;
	}
	else if (channelInfo.daBType == L"INT16")
	{
		enChannelType = CT_GOOSE_INT16;
	}
	else if (channelInfo.daBType == L"INT24")
	{
		enChannelType = CT_GOOSE_INT24;
	}
	else if (channelInfo.daBType == L"INT32")
	{
		enChannelType = CT_GOOSE_INT32;
	}
	else if (channelInfo.daBType == L"INT128")
	{
		enChannelType = CT_GOOSE_INT128;
	}
	else if (channelInfo.daBType == L"INT8U")
	{
		enChannelType = CT_GOOSE_UINT8;
	}
	else if (channelInfo.daBType == L"INT16U")
	{
		enChannelType = CT_GOOSE_UINT16;
	}
	else if (channelInfo.daBType == L"INT24U")
	{
		enChannelType = CT_GOOSE_UINT24;
	}
	else if (channelInfo.daBType == L"INT32U")
	{
		enChannelType = CT_GOOSE_UINT32;
	}
	else if (channelInfo.daBType == L"FLOAT32")
	{
		enChannelType = CT_GOOSE_FLOAT32;
	}
	else if (channelInfo.daBType == L"FLOAT64")
	{
		enChannelType = CT_GOOSE_FLOAT64;
	}
	else if (channelInfo.daBType.find(L"Octet") != std::wstring::npos)
	{
		enChannelType = CT_GOOSE_OCTET;
	}
	else if (channelInfo.daBType.find(L"VisString") != std::wstring::npos)
	{
		enChannelType = CT_GOOSE_VISSTRING;
	}
	else if (channelInfo.daBType.find(L"Unicode") != std::wstring::npos)
	{
		enChannelType = CT_GOOSE_UNICODE;
	}

	return enChannelType;
}

///
/// @brief
///     �ж�ĳAppID�Ŀ��ƿ��Ƿ����
///
/// @param[in]
///     appID - ���ƿ��AppIDֵ
///
/// @return
///     0 - ���ڣ� -1 - ������
///
int SCLGOOSEAnalysis::IsAppIDExisted(const unsigned short appID)
{
	// �жϻ����Ƿ����
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	return pCache->IsGSEAppIDExisted(appID);
}

///
/// @brief
///     ��ȡ�Զ�����������ģ���е�DaName��Ϣ
///
/// @param[in]
///     eChannelType - gooseͨ������
/// @param[in]
///     daName       - �����Զ�����������ģ���е�DaName��Ϣ
///
/// @return
///     
///
void SCLGOOSEAnalysis::GetDefDataTemplate_DaName(CHANNEL_TYPE_GOOSE eChannelType, std::wstring &daName)
{
	daName = GOOSE_CHANNEL_TYPE_TEMPLATE[eChannelType].name;
}

///
/// @brief
///     �������õ���SCL����ת����IEDĳһGOOSE�������Ϣ
///
/// @param[in]
///     gseAttrib     - GOOSEͨ�Ų�����Ϣ
/// @param[in]
///     iedAttrib     - IED������Ϣ
/// @param[in]
///     apAttrib      - AccessPoint������Ϣ
/// @param[in]
///     ldAttrib      - LDevice������Ϣ
/// @param[in]
///     gseCtrlAttrib - GSEControl������Ϣ
/// @param[in]
///     iedGOOSEInfo  - ����IED��ĳһGOOSE���������Ϣ�Ľṹ��
///
/// @return
///     
///
void SCLGOOSEAnalysis::SCL2IEDGOOSEInfo(const SCL_GSE_ATTRIB &gseAttrib, const SCL_IED_ATTRIB &iedAttrib,
										const SCL_ACCESS_POINT_ATTRIB &apAttrib, const SCL_LDEVICE_ATTRIB &ldAttrib,
										const SCL_DATASET_ATTRIB &dataSetAttrib, const SCL_GSECTRL_ATTRIB &gseCtrlAttrib,
										SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfo)
{
	iedGOOSEInfo->iedName   = iedAttrib.iedName;
	iedGOOSEInfo->iedDesc   = iedAttrib.iedDesc;

	iedGOOSEInfo->iedAPName = apAttrib.apName;
	iedGOOSEInfo->iedAPDesc = apAttrib.apDesc;

	iedGOOSEInfo->iedLdInst = ldAttrib.ldInst;
	iedGOOSEInfo->iedLdDesc = ldAttrib.ldDesc;

	iedGOOSEInfo->dataSetName = dataSetAttrib.dataSetName;
	iedGOOSEInfo->dataSetDesc = dataSetAttrib.dataSetDesc;
	iedGOOSEInfo->channelNum  = dataSetAttrib.fcdaNum;

	ConvMacWStr2Char(gseAttrib.macAddr,iedGOOSEInfo->gooseMAC);

	unsigned int tmp;
	ConvWStr2Hex(gseAttrib.appID,&tmp);
	iedGOOSEInfo->gooseAppID   = (unsigned short)tmp;
	ConvWStr2Hex(gseAttrib.vlanID,&tmp);
	iedGOOSEInfo->gooseVlanID  = (unsigned short)tmp;
	ConvWStr2Dec(gseAttrib.vlanPriority,&tmp);
	iedGOOSEInfo->gooseVlanPri = (unsigned char)tmp;
	ConvSCLTime(gseAttrib.minTimeVal,gseAttrib.minTimeUnit,&iedGOOSEInfo->gooseMinTime);
	ConvSCLTime(gseAttrib.maxTimeVal,gseAttrib.maxTimeUnit,&iedGOOSEInfo->gooseMaxTime);

	iedGOOSEInfo->cbDesc  = gseCtrlAttrib.cbDesc;
	iedGOOSEInfo->cbName  = gseCtrlAttrib.cbName;
	ConvWStr2Dec(gseCtrlAttrib.confRev,&iedGOOSEInfo->confRev);
	iedGOOSEInfo->dataSet = gseCtrlAttrib.dataSet;
	iedGOOSEInfo->gooseID = gseCtrlAttrib.gooseID;
	iedGOOSEInfo->type    = gseCtrlAttrib.type;
}

///
/// @brief
///     �������õ���SCL����ת����ĳһͨ����FCDA������Ϣ
///
/// @param[in]
///     dataSetAttrib - ��ͨ����FCDA������DataSet��������Ϣ
/// @param[in]
///     fcdaAttrib    - ��ͨ����FCDA����������Ϣ
/// @param[in]
///     lnDesc        - ��ͨ����FCDA����ָ�����������LNode��������Ϣ
/// @param[in]
///     doiAttrib     - ��ͨ����FCDA����ָ������ݵ���Ϣ
/// @param[in]
///     daBType       - ��ͨ����FCDA����ָ����������ԵĻ���������Ϣ
/// @param[in]
///     dataSetInfo   - ����ͨ����FCDA������Ϣ�Ľṹ��
///
/// @return
///     
///
void SCLGOOSEAnalysis::SCL2DataSetInfo(const SCL_DATASET_ATTRIB &dataSetAttrib, const SCL_FCDA_ATTRIB &fcdaAttrib, const SCL_STR &lnDesc, const SCL_DOI_ATTRIB &doiAttrib, const SCL_STR &daBType, SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo)
{
	dataSetInfo->idxSeq      = fcdaAttrib.idxSeq;
	dataSetInfo->dataSetName = dataSetAttrib.dataSetName;
	dataSetInfo->dataSetDesc = dataSetAttrib.dataSetDesc;

	dataSetInfo->ldInst      = fcdaAttrib.ldInst;
	dataSetInfo->lnPrefix    = fcdaAttrib.lnPrefix;
	dataSetInfo->lnClass     = fcdaAttrib.lnClass;
	dataSetInfo->lnInst      = fcdaAttrib.lnInst;
	dataSetInfo->lnDesc      = lnDesc;

	dataSetInfo->doName      = fcdaAttrib.doName;
	dataSetInfo->doDesc      = doiAttrib.doDesc;
	dataSetInfo->dUsAddr     = doiAttrib.dUSAddr;
	dataSetInfo->dUVal       = doiAttrib.dUVal;

	dataSetInfo->daName      = fcdaAttrib.daName;
	dataSetInfo->daFc        = fcdaAttrib.fc;
	dataSetInfo->daBType     = daBType;
}

///
/// @brief
///     �������õ���SCL����ת����GOOSE INPUT��Ϣ���ⲿ�ź�������Ϣ��
///
/// @param[in]
///     iedName       - Inputs���ڵ�IED������
/// @param[in]
///     extRefAttrib  - �����õ���Inputs ExtRef��Ϣ
/// @param[in]
///     inputsInfo    - ����GOOSE INPUT��Ϣ�Ľṹ��
///
/// @return
///     true - �����ɹ��� false - ����ʧ��
///
bool SCLGOOSEAnalysis::SCL2InputsInfo(const SCL_STR &iedName, const SCL_EXTREF_ATTRIB &extRefAttrib, SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO *inputsInfo)
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return false;

	SCL_LN0_ATTRIB     ln0Attrib;
	SCL_LN_ATTRIB      lnAttrib;
	SCL_DOI_ATTRIB     doiAttrib;
	SCL_DATASET_ATTRIB dataSetAttrib;
	SCL_FCDA_ATTRIB    fcdaAttrib;
	SCL_STR            lnName = extRefAttrib.lnPrefix + extRefAttrib.lnClass + extRefAttrib.lnInst;
	SCL_STR            lnDesc;
	int                idxSeq = 0;
	size_t             pos1   = 0;
	size_t             pos2   = 0;

	// ת���ⲿ�ź���Ϣ
	if ( extRefAttrib.lnClass == _T("LLN0") )
	{
		if ( !pCache->GetLN0Attrib(extRefAttrib.iedName,extRefAttrib.ldInst,ln0Attrib) )
			return false;
		lnDesc = ln0Attrib.lnDesc;
		if ( !pCache->GetDOIAttribInLNorLN0(extRefAttrib.iedName,extRefAttrib.ldInst,_T("LLN0"),extRefAttrib.doName,doiAttrib) )
		{
			doiAttrib.doName = extRefAttrib.doName;
			doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
		}
	}
	else
	{
		if ( !pCache->GetLNAttrib(extRefAttrib.iedName,extRefAttrib.ldInst,lnName,lnAttrib) )
			return false;
		lnDesc = lnAttrib.lnDesc;
		if ( !pCache->GetDOIAttribInLNorLN0(extRefAttrib.iedName,extRefAttrib.ldInst,lnName,extRefAttrib.doName,doiAttrib) )
		{
			doiAttrib.doName = extRefAttrib.doName;
			doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
		}
	}
	fcdaAttrib.ldInst   = extRefAttrib.ldInst;
	fcdaAttrib.lnPrefix = extRefAttrib.lnPrefix;
	fcdaAttrib.lnClass  = extRefAttrib.lnClass;
	fcdaAttrib.lnInst   = extRefAttrib.lnInst;
	fcdaAttrib.doName   = extRefAttrib.doName;
	fcdaAttrib.daName   = extRefAttrib.daName;
	idxSeq = pCache->GetFCDAIdxSeqInLNorLN0(extRefAttrib.iedName,extRefAttrib.ldInst,_T("LLN0"),fcdaAttrib);
	
	inputsInfo->idxSeq_Out  = idxSeq;
	inputsInfo->iedName_Out = extRefAttrib.iedName;
	inputsInfo->ldInst_Out  = extRefAttrib.ldInst;
	inputsInfo->lnName_Out  = lnName;
	inputsInfo->doName_Out  = extRefAttrib.doName;
	inputsInfo->daName_Out  = extRefAttrib.daName;
	inputsInfo->lnDesc_Out  = lnDesc;
	inputsInfo->doDesc_Out  = doiAttrib.doDesc;
	inputsInfo->dUDesc_Out  = doiAttrib.dUVal;

	// �ֽ��ⲿ�ź����󶨵��ڲ���ַ����Ϣ
	inputsInfo->idxSeq_In  = extRefAttrib.idxSeq;
	inputsInfo->iedName_In = iedName;
	pos1 = extRefAttrib.intAddr.find(_T("/"));
	if ( pos1 == std::wstring::npos )
		inputsInfo->ldInst_In = _T("");
	else
		inputsInfo->ldInst_In = extRefAttrib.intAddr.substr(0,pos1);
	pos2 = extRefAttrib.intAddr.find(_T("."),pos1+1);
	if ( pos2 == std::wstring::npos )
		inputsInfo->lnName_In = _T("");
	else
		inputsInfo->lnName_In = extRefAttrib.intAddr.substr(pos1+1,pos2-pos1-1);
	pos1 = extRefAttrib.intAddr.find(_T("."),pos2+1);
	if ( pos1 == std::wstring::npos )
	{
		inputsInfo->doName_In = extRefAttrib.intAddr.substr(pos2+1);
		inputsInfo->daName_In = _T("");
	}
	else
	{
		inputsInfo->doName_In = extRefAttrib.intAddr.substr(pos2+1,pos1-pos2-1);
		inputsInfo->daName_In = extRefAttrib.intAddr.substr(pos1+1);
	}

	// ��ȡ�ڲ���ַ��������Ϣ
	if ( extRefAttrib.lnClass == _T("LLN0") )
	{
		if ( !pCache->GetLN0Attrib(inputsInfo->iedName_In,inputsInfo->ldInst_In,ln0Attrib) )
			return false;
		lnDesc = ln0Attrib.lnDesc;
		if ( !pCache->GetDOIAttribInLNorLN0(inputsInfo->iedName_In,inputsInfo->ldInst_In,_T("LLN0"),inputsInfo->doName_In,doiAttrib) )
		{
			doiAttrib.doName = inputsInfo->doName_In;
			doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
		}
	}
	else
	{
		if ( !pCache->GetLNAttrib(inputsInfo->iedName_In,inputsInfo->ldInst_In,inputsInfo->lnName_In,lnAttrib) )
			return false;
		lnDesc = lnAttrib.lnDesc;
		if ( !pCache->GetDOIAttribInLNorLN0(inputsInfo->iedName_In,inputsInfo->ldInst_In,inputsInfo->lnName_In,inputsInfo->doName_In,doiAttrib) )
		{
			doiAttrib.doName = inputsInfo->doName_In;
			doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
		}
	}
	inputsInfo->lnDesc_In = lnDesc;
	inputsInfo->doDesc_In = doiAttrib.doDesc;
	inputsInfo->dUDesc_In = doiAttrib.dUVal;

	return true;
}

///
/// @brief
///     ͨ��ͨ����FCDA���������ж��Ƿ�Ŀ��ͨ����FCDA��
///
/// @param[in]
///     dataSetInfo - ��Ҫ�жϵ�ͨ����FCDA������Ϣ
/// @param[in]
///     fcdaAttrib  - ��ͨ����FCDA����������Ϣ
///
/// @return
///     
///
bool SCLGOOSEAnalysis::IsGoalFCDA(const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo, const SCL_FCDA_ATTRIB &fcdaAttrib)
{
	if ( dataSetInfo->ldInst   == fcdaAttrib.ldInst &&
		dataSetInfo->lnPrefix == fcdaAttrib.lnPrefix &&
		dataSetInfo->lnClass  == fcdaAttrib.lnClass &&
		dataSetInfo->lnInst   == fcdaAttrib.lnInst &&
		dataSetInfo->doName   == fcdaAttrib.doName &&
		dataSetInfo->daName   == fcdaAttrib.daName &&
		dataSetInfo->daFc     == fcdaAttrib.fc)
	{
		return true;
	}
	else
	{
		return false;
	}
}

///
/// @brief
///     ��MACAddr��wstringת��Ϊchar����
///
/// @param[in]
///     mac      - Ҫת����macAddr
/// @param[in]
///     macRetn  - ����ת�������char�����ָ��
///
/// @return
///     
///
void SCLGOOSEAnalysis::ConvMacWStr2Char(const std::wstring &mac, unsigned char *macRetn)
{
	unsigned int k;
	int          j = 0;
	wchar_t      c[2];

	for ( int i=0 ; i<17 ;i = i+3)
	{
		c[0] = mac.c_str()[i];
		c[1] = mac.c_str()[i+1];
		swscanf_s(c,_T("%x"),&k);
		macRetn[j] = (unsigned char)k;
		j++;
	}
}

///
/// @brief
///     ��MACAddr��char����ת��Ϊwstring
///
/// @param[in]
///     macRetn  - Ҫת����macAddr��char�����ָ��
/// @param[in]
///     mac      - macAddr
///
/// @return
///     
///
void SCLGOOSEAnalysis::ConvMacChar2WStr(const unsigned char *macRetn,std::wstring &mac)
{
	wchar_t      c[256] = {0};

	mac = L"";
	for ( int i=0 ; i<5 ;i++ )
	{
		swprintf_s(c,256,L"%02x-",macRetn[i]);
		mac += c;
	}
	swprintf_s(c,256,L"%02x",macRetn[5]);
	mac += c;
}

///
/// @brief
///     ��ʮ������wstringת��Ϊ����
///
/// @param[in]
///     str      - Ҫת����wstring
/// @param[in]
///     intData  - ָ��ת�������ָ��
///
/// @return
///     
///
void SCLGOOSEAnalysis::ConvWStr2Hex(const std::wstring &str, unsigned int *intData)
{
	if ( str.length() == 0 )
	{
		*intData = 0;
		return;
	}
	swscanf_s(str.c_str(),_T("%x"),intData);
}

///
/// @brief
///     ������ת��Ϊʮ������wstring
///
/// @param[in]
///     intData  - ָ��������ָ��
/// @param[in]
///     str      - ʮ������wstring
///
/// @return
///     
///
void SCLGOOSEAnalysis::ConvHex2WStr(const unsigned int *intData, std::wstring &str)
{
	wchar_t c[256] = {0};
	swprintf_s(c,256,L"%x",*intData);
	str = c;
}

///
/// @brief
///     ��ʮ����wstringת��Ϊ����
///
/// @param[in]
///     str      - Ҫת����wstring
/// @param[in]
///     intData  - ָ��ת�������ָ��
///
/// @return
///     
///
void SCLGOOSEAnalysis::ConvWStr2Dec(const std::wstring &str, unsigned int *intData)
{
	if ( str.length() == 0 )
	{
		*intData = 0;
		return;
	}
	swscanf_s(str.c_str(),_T("%d"),intData);
}

///
/// @brief
///     ������ת��Ϊʮ����wstring
///
/// @param[in]
///     intData  - ָ��Ҫת����������ָ��
/// @param[in]
///     str      - ʮ����wstring
///
/// @return
///     
///
void SCLGOOSEAnalysis::ConvDec2WStr(const unsigned int *intData, std::wstring &str)
{
	wchar_t c[256] = {0};
	swprintf_s(c,256,L"%d",*intData);
	str = c;
}

///
/// @brief
///     ��SCL�ļ��ж����ʱ����Ϣת��Ϊ��msΪ��λ������ֵ
///
/// @param[in]
///     timeVal  - SCL�ļ��ж����ʱ���ֵ
/// @param[in]
///     timeUnit - SCL�ļ��ж����ʱ��ĵ�λ
/// @param[in]
///     time     - ָ��ת�������ָ��
///
/// @return
///     
///
void SCLGOOSEAnalysis::ConvSCLTime(const std::wstring &timeVal, const std::wstring &timeUnit, unsigned int *time)
{
	swscanf_s(timeVal.c_str(),_T("%d"),time);
	if ( timeUnit == _T("us") )
	{
		*time = *time / 1000;
	}
	if ( timeUnit == _T("s") )
	{
		*time = *time * 1000;
	}
}

///
/// @brief
///     �ж�Mac��ַ�Ƿ���ͬ
///
/// @param[in]
///     mac1 - Mac��ַ1
/// @param[in]
///     mac2 - Mac��ַ2
///
/// @return
///     true����ͬ��false����ͬ
///
bool SCLGOOSEAnalysis::IsMacAddrSame(const unsigned char *mac1, const unsigned char *mac2)
{
	for ( int i=0; i<GOOSEANALYSIS_MAC_SIZE; i++)
	{
		if ( mac1[i] != mac2[i] )
			return false;
	}
	return true;
}

///
/// @brief
///     ����GOOSEͨ������������ģ�壨LNodeType��Ϣ��
///
/// @param[in]
///     lnTypeID - ͨ�����������߼��ڵ�����ͣ�������ID
/// @param[in]
///     lnClass  - ͨ�����������߼��ڵ����
/// @param[in]
///     doName   - ͨ������(DO)������
/// @param[in]
///     eChannelType - ͨ������
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int  SCLGOOSEAnalysis::AddDOIntoDefLNodeType(SCLCache *pCache ,const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
										     const SCL_STR &lnTypeID, CHANNEL_TYPE_GOOSE eChannelType)
{
	if ( pCache == NULL)
		return -1;

	// ������LNodeType�������
	SCL_LNTYPE_ATTRIB lnTypeAttrib;
	if ( !pCache->GetLNType(lnTypeID,lnTypeAttrib) )
	{
		lnTypeAttrib.id      = lnTypeID;
		lnTypeAttrib.lnClass = _T("DEFLN");
		if ( !pCache->AddLNType(lnTypeAttrib) )
			return -1;
	}

	// ���DO��Ϣ��LNodeType
	SCL_DOINLNTYPE_ATTRIB doAttrib;
	if ( !pCache->GetDOAttrib(lnTypeID,dataSetInfo.doName,doAttrib) )
	{
		SCL_STR doTypeID;

		// LNodeType�в����ڸ�DO�������ͨ������ѡ���ͨ�����Զ���DOTypeģ��
		// ģ�岻�������½�
		if ( eChannelType == CT_GOOSE_STRUCT )
		{
			// ��ͨ��Ϊ�ṹ������
			doTypeID = _T("CN_GOOSE_DEFSTRUCT");
			SCL_DOTYPE_ATTRIB doTypeAttrib;
			if ( !pCache->GetDOType(doTypeID,doTypeAttrib) )
			{
				doTypeAttrib.cdc = _T("DEF");
				doTypeAttrib.id  = doTypeID;
				if ( !pCache->AddDOType(doTypeAttrib) )
					return -1;
			}

			// Ĭ�Ͻṹ�����һ����������
			SCL_DAINDOTYPE_ATTRIB daAttrib;
			if ( !pCache->GetDAAttrib(doTypeID,dataSetInfo.daName,daAttrib) )
			{
				SCL_DAINDOTYPE_ATTRIB daAttrib = GOOSE_CHANNEL_TYPE_TEMPLATE[CT_GOOSE_STRUCT];
				daAttrib.name = dataSetInfo.daName;
				daAttrib.type = doTypeAttrib.id + L"_" + dataSetInfo.daName;
				if ( !pCache->AddDAIntoDOType(doTypeAttrib.id,daAttrib) )
					return -1;

				// ���Ĭ�ϵĽṹ���������Ͷ���
				AddDefaultDaType(pCache,daAttrib.type);
			}
		}
		else
		{
			// ��ͨ��Ϊ�ǽṹ������
			doTypeID = _T("CN_GOOSE_DEF");
			SCL_DOTYPE_ATTRIB doTypeAttrib;
			if ( !pCache->GetDOType(doTypeID,doTypeAttrib) )
			{
				doTypeAttrib.cdc = _T("DEF");
				doTypeAttrib.id  = doTypeID;
				if ( !pCache->AddDOType(doTypeAttrib) )
					return -1;

				// ��ģ��Ĭ�Ϻ���ȫ���ǽṹ������
				for ( int i = 0; i < sizeof(GOOSE_CHANNEL_TYPE_TEMPLATE)/sizeof(SCL_DAINDOTYPE_ATTRIB); i++ )
				{
					if ( i == (int)CT_GOOSE_STRUCT)
						continue;
					SCL_DAINDOTYPE_ATTRIB daAttrib = GOOSE_CHANNEL_TYPE_TEMPLATE[i];
					if ( !pCache->AddDAIntoDOType(doTypeAttrib.id,daAttrib) )
						return -1;
				}
			}
		}

		// ���/����DO��Ϣ��LNodeType
		doAttrib.name = dataSetInfo.doName;
		doAttrib.type = doTypeID;
		if ( !pCache->AddDOIntoLNType(lnTypeID,doAttrib) )
			return -1;
	}

	return 0;
}

///
/// @brief
///     ����ʹ���Զ�����������ģ�����ͨ�������������ͽṹ��ԭͨ��һ��
///
/// @param[in]
///     pCache            - ָ��SCL�������ݵĻ����ָ��
/// @param[in]
///     iedName           - ����ͨ�������ڵ�IED������
/// @param[in]
///     ldInst            - ����ͨ�������ڵ�LDevice��Inst��
/// @param[in]
///     channelInfo       - ����ͨ������Ӧ������ͨ����Ϣ�������ƿ����������ݼ��е�ĳһͨ����
/// @param[in]
///     subChannelIdxList - �Ӷ���ͨ���µĵ�һ����ͨ����ʼ����ǰ��ͨ��������ͨ����˳����б�
/// @param[in]
///     listSize          - ˳����б�Ĵ�С
/// @param[in]
///     daTypeID_Out      - ����ͨ������������ģ�壨DaType����ID���ṹ��������ͨ����Ч��
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int  SCLGOOSEAnalysis::CreateDefSubChannel(SCLCache *pCache, const std::wstring &iedName,
										   const std::wstring &ldInst,
										   SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
										   const int *subChannelIdxList, int listSize,
										   SCL_STR &daTypeID_Out)
{
	if ( pCache == NULL)
		return -1;

	// ��ȡԭͨ����LNode������ID
	SCL_STR lnTypeID;
	if ( channelInfo.lnClass == L"LLN0" )
	{
		SCL_LN0_ATTRIB ln0Attrib;
		if ( !pCache->GetLN0Attrib(iedName,ldInst,ln0Attrib) )
			return -1;
		lnTypeID = ln0Attrib.lnType;
	}
	else
	{
		SCL_STR lnName = channelInfo.lnPrefix + channelInfo.lnClass + channelInfo.lnInst;
		SCL_LN_ATTRIB lnAttrib;
		if ( !pCache->GetLNAttrib(iedName,ldInst,lnName,lnAttrib) )
			return -1;
		lnTypeID = lnAttrib.lnType;
	}

//	// ��ͨ����ʹ���Զ�����������ģ�壬�򷵻�
	std::wstring str;
	ConvDec2WStr((unsigned int *)&channelInfo.idxSeq,str);
	SCL_STR daTypeID = L"CN_GOOSE_DEFSTRUCT_DEF_"+iedName+ldInst+L"_"+channelInfo.dataSetName+L"_Da_"+str;
	daTypeID_Out = daTypeID;
// 	for ( int i = 0; i < listSize; i++ )
// 	{
// 		std::wstring str;
// 		ConvDec2WStr((unsigned int *)&subChannelIdxList[i],str);
// 		daTypeID_Out = daTypeID_Out+L"_"+str;
// 	}
// 	SCL_DATYPE_ATTRIB daTypeAttrib;
// 	if ( pCache->GetDAType(daTypeID_Out,daTypeAttrib) && lnTypeID == L"XJ_LNODE_DEF")
// 	{
// 		return 0;
// 	}

	// ��ȡԭͨ����DO��Ϣ
	SCL_DOINLNTYPE_ATTRIB doAttrib;
	if ( !pCache->GetDOAttrib(lnTypeID,channelInfo.doName,doAttrib) )
		return -1;
	SCL_STR doTypeID = doAttrib.type;

	// �����µ�ͨ����������ģ�壬������ԭͨ����������Ϣ
	if ( channelInfo.daName.empty() )
	{
		if ( CopyDoTypeIntoStructDaType(pCache,doTypeID,daTypeID,channelInfo.daFc) == -1 )
			return -1;
	}
	else
	{
		SCL_DAINDOTYPE_ATTRIB daAttrib;
		if ( !pCache->GetDAAttrib(doTypeID,channelInfo.daName,daAttrib) )
			return -1;
		if ( daAttrib.bType != GOOSE_CHANNEL_TYPE_TEMPLATE[CT_GOOSE_STRUCT].bType )
			return -1;
		if ( CopyDaType(pCache,daAttrib.type,daTypeID) == -1 )
			return -1;
	}

	// ʹ���Զ����߼��ڵ㼰����ģ�壨DA Structģʽ�����滻ԭͨ��
	CHANNEL_TYPE_GOOSE eCType = GetChannelType(channelInfo);
	if ( SetOneIEDGOOSEDataSetInfo_Def(iedName,ldInst,channelInfo,eCType) == -1 )
		return -1;
	lnTypeID = L"XJ_LNODE_DEF";

	// ����ͨ������������ͨ������������ģ��
	doAttrib.name = channelInfo.doName;
	doAttrib.type = L"CN_GOOSE_DEFSTRUCT";
	if ( !pCache->SetDOAttrib(lnTypeID,channelInfo.doName,doAttrib) )
		return -1;

	// ������ͨ���ĸ�����ͨ������������ģ�壬������ԭͨ���ĸ�����ͨ��������������Ϣ
	daTypeID_Out = daTypeID;
	for ( int i = 0; i < listSize; i++ )
	{
		SCL_BDAINDATYPE_ATTRIB bdaAttrib;
		if ( !pCache->GetBDAAttrib(daTypeID_Out,subChannelIdxList[i],bdaAttrib) )
			return -1;
		if ( bdaAttrib.bType != GOOSE_CHANNEL_TYPE_TEMPLATE[CT_GOOSE_STRUCT].bType )
			return -1;

		std::wstring str;
		ConvDec2WStr((unsigned int *)&subChannelIdxList[i],str);
		daTypeID = daTypeID_Out+L"_"+str;

		if ( daTypeID != bdaAttrib.type )
		{
			if ( CopyDaType(pCache,bdaAttrib.type,daTypeID) == -1 )
				return -1;

			bdaAttrib.type = daTypeID;
			if ( !pCache->SetBDAAttrib(daTypeID_Out,bdaAttrib.name,bdaAttrib) )
				return -1;
		}
		daTypeID_Out = daTypeID;
	}
	return 0;
}

///
/// @brief
///     ��LnType1�е�ָ����DO���Ƶ�LnType2��
///
/// @param[in]
///     pCache            - ָ��SCL�������ݵĻ����ָ��
/// @param[in]
///     lnTypeID1         - LnType1��ID
/// @param[in]
///     lnTypeID2         - LnType2��ID
/// @param[in]
///     doName            - Ҫ���Ƶ�DO������
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int  SCLGOOSEAnalysis::CopyLnType(SCLCache *pCache, const std::wstring &lnTypeID1, const std::wstring &lnTypeID2, const std::wstring &doName)
{
	if ( pCache == NULL)
		return -1;

	if ( lnTypeID1 != lnTypeID2 )
	{
		// LnType2�������򴴽�
		SCL_LNTYPE_ATTRIB lnTypeAttrib;
		if ( !pCache->GetLNType(lnTypeID2,lnTypeAttrib) )
		{
			lnTypeAttrib.id      = lnTypeID2;
			lnTypeAttrib.lnClass = L"GGIO_EX";
			if ( !pCache->AddLNType(lnTypeAttrib) )
				return -1;
		}

		// ����LnType1�е�ָ����DO
		SCL_DOINLNTYPE_ATTRIB doAttrib;
		if ( !pCache->GetDOAttrib(lnTypeID1,doName,doAttrib) )
			return -1;
		if ( !pCache->SetDOAttrib(lnTypeID2,doName,doAttrib))
			return -1;
	}
	return 0;
}

///
/// @brief
///     ����DoType�е�ĳһ˳��ŵ�DA������
///
/// @param[in]
///     pCache            - ָ��SCL�������ݵĻ����ָ��
/// @param[in]
///     DoTypeID          - DoType��ID
/// @param[in]
///     idxSeq            - DA��˳���
/// @param[in]
///     eChannelType      - DA������
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int  SCLGOOSEAnalysis::SetDAInDoType(SCLCache *pCache ,const std::wstring &doTypeID, int idxSeq, CHANNEL_TYPE_GOOSE eChannelType)
{
	if ( pCache == NULL)
		return -1;

	// Ĭ��DA��fcΪST
	SCL_DAINDOTYPE_ATTRIB daAttrib;
	int daNum = pCache->GetDANumInDOType(doTypeID);
	if ( idxSeq >= daNum )
	{
		// DA˳��Ŵ���DaNum,��ȫ������
		if ( !pCache->GetDAAttrib(doTypeID,daNum-1,daAttrib) )
			return -1;
		while ( idxSeq >= daNum )
		{
			if ( daNum == idxSeq )
			{
				daAttrib    = GOOSE_CHANNEL_TYPE_TEMPLATE[eChannelType];
			}
			// daName��Ӻ�׺��ʹ����DOType�б���Ψһ
			std::wstring str;
			ConvDec2WStr((unsigned int *)&daNum,str);
			daAttrib.name = daAttrib.name + _T("_") + str;

			if ( eChannelType == CT_GOOSE_STRUCT )
			{
				// DAΪ�ṹ������
				daAttrib.type = doTypeID + _T("_") + str;
				if ( !pCache->AddDAIntoDOType(doTypeID,daAttrib) )
					return -1;

				// ���Ĭ�ϵĽṹ���������Ͷ���
				AddDefaultDaType(pCache,daAttrib.type);
			}
			else
			{
				// DAΪ�ǽṹ������
				if ( !pCache->AddDAIntoDOType(doTypeID,daAttrib) )
					return -1;
			}
			daNum++;
		}
	}
	else
	{
		daAttrib    = GOOSE_CHANNEL_TYPE_TEMPLATE[eChannelType];

		// daName��Ӻ�׺��ʹ����DOType�б���Ψһ
		std::wstring str;
		ConvDec2WStr((unsigned int *)&daNum,str);
		daAttrib.name = daAttrib.name + _T("_") + str;

		if ( eChannelType == CT_GOOSE_STRUCT )
		{
			// DAΪ�ṹ������
			daAttrib.type = doTypeID + _T("_") + str;
			if ( !pCache->SetDAAttrib(doTypeID,idxSeq,daAttrib) )
				return -1;

			// ���Ĭ�ϵĽṹ���������Ͷ���
			AddDefaultDaType(pCache,daAttrib.type);
		}
		else
		{
			// DAΪ�ǽṹ������
			if ( !pCache->SetDAAttrib(doTypeID,idxSeq,daAttrib) )
				return -1;
		}
	}
	return 0;
}

///
/// @brief
///     ��DoType1�е�ָ��fc��DA���Ƶ�DoType2��
///
/// @param[in]
///     pCache            - ָ��SCL�������ݵĻ����ָ��
/// @param[in]
///     DoTypeID1         - DoType1��ID
/// @param[in]
///     DoTypeID2         - DoType2��ID
/// @param[in]
///     fc                - Ҫ���Ƶ�DA��fc
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int  SCLGOOSEAnalysis::CopyDoType(SCLCache *pCache, const std::wstring &doTypeID1, const std::wstring &doTypeID2, const std::wstring &fc)
{
	if ( pCache == NULL)
		return -1;

	if ( doTypeID1 != doTypeID2 )
	{
		// DoType2�������򴴽�
		SCL_DOTYPE_ATTRIB doTypeAttrib;
		if ( !pCache->GetDOType(doTypeID2,doTypeAttrib) )
		{
			doTypeAttrib.cdc = _T("DEF");
			doTypeAttrib.id  = doTypeID2;
			if ( !pCache->AddDOType(doTypeAttrib) )
				return -1;
		}

		// ����DoType1��ָ��fc��DA
		int i = 0;
		SCL_DAINDOTYPE_ATTRIB daAttrib;
		while ( pCache->GetDAAttrib(doTypeID1,i,daAttrib) )
		{
			if ( daAttrib.fc == fc )
			{
				daAttrib.fc = _T("ST");
				if ( !pCache->AddDAIntoDOType(doTypeID2,daAttrib) )
					return -1;
			}
			i++;
		}
	}
	return 0;
}

///
/// @brief
///     ��DoType��ָ��fc��DA���Ƶ��ṹ�����͵�DaType��
///
/// @param[in]
///     pCache            - ָ��SCL�������ݵĻ����ָ��
/// @param[in]
///     doTypeID          - DoType��ID
/// @param[in]
///     daTypeID          - DaType��ID
/// @param[in]
///     fc                - Ҫ���Ƶ�DA��fc
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int  SCLGOOSEAnalysis::CopyDoTypeIntoStructDaType(SCLCache *pCache, const std::wstring &doTypeID, const std::wstring &daTypeID, const std::wstring &fc)
{
	if ( pCache == NULL)
		return -1;

	// DoType2�������򴴽�
	SCL_DATYPE_ATTRIB daTypeAttrib;
	if ( !pCache->GetDAType(daTypeID,daTypeAttrib) )
	{
		daTypeAttrib.id  = daTypeID;
		if ( !pCache->AddDAType(daTypeAttrib) )
			return -1;
	}

	// ����DoType��ָ��fc��DA
	int i = 0;
	SCL_DAINDOTYPE_ATTRIB daAttrib;
	while ( pCache->GetDAAttrib(doTypeID,i,daAttrib) )
	{
		if ( daAttrib.fc == fc )
		{
			SCL_BDAINDATYPE_ATTRIB bdaAttrib;
			bdaAttrib.name  = daAttrib.name;
			bdaAttrib.bType = daAttrib.bType;
			bdaAttrib.type  = daAttrib.type;
			if ( !pCache->AddBDAIntoDAType(daTypeID,bdaAttrib) )
				return -1;
		}
		i++;
	}
	return 0;
}

///
/// @brief
///     ����DaType��ĳһ˳��ŵ�BDA������
///
/// @param[in]
///     pCache            - ָ��SCL�������ݵĻ����ָ��
/// @param[in]
///     daTypeID          - DaType��ID
/// @param[in]
///     idxSeq            - BDA��˳���
/// @param[in]
///     eChannelType      - BDA������
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int  SCLGOOSEAnalysis::SetBDAInDaType(SCLCache *pCache, const std::wstring &daTypeID, int idxSeq, CHANNEL_TYPE_GOOSE eChannelType)
{
	if ( pCache == NULL)
		return -1;

	SCL_BDAINDATYPE_ATTRIB bdaAttrib;
	int bdaNum = pCache->GetBDANumInDAType(daTypeID);
	if ( idxSeq >= bdaNum )
	{
		if ( !pCache->GetBDAAttrib(daTypeID,bdaNum-1,bdaAttrib))
			return -1;

		while ( idxSeq>= bdaNum )
		{
			if ( idxSeq == bdaNum )
			{
				bdaAttrib.name  = GOOSE_CHANNEL_TYPE_TEMPLATE[eChannelType].name;
				bdaAttrib.bType = GOOSE_CHANNEL_TYPE_TEMPLATE[eChannelType].bType;
			}

			// bdaName��Ӻ�׺��ʹ����DaType�б���Ψһ
			std::wstring str;
			ConvDec2WStr((unsigned int *)&bdaNum,str);
			bdaAttrib.name = bdaAttrib.name + _T("_") + str;

			if ( eChannelType == CT_GOOSE_STRUCT )
			{
				// BDAΪ�ṹ������
				bdaAttrib.type = daTypeID + _T("_") + str;
				if ( !pCache->AddBDAIntoDAType(daTypeID,bdaAttrib) )
					return -1;

				// ���Ĭ�ϵĽṹ���������Ͷ���
				AddDefaultDaType(pCache,bdaAttrib.type);
			}
			else
			{
				// DAΪ�ǽṹ������
				if ( !pCache->AddBDAIntoDAType(daTypeID,bdaAttrib) )
					return -1;
			}
			bdaNum++;
		}
	}
	else
	{
		bdaAttrib.name  = GOOSE_CHANNEL_TYPE_TEMPLATE[eChannelType].name;
		bdaAttrib.bType = GOOSE_CHANNEL_TYPE_TEMPLATE[eChannelType].bType;

		// bdaName��Ӻ�׺��ʹ����DaType�б���Ψһ
		std::wstring str;
		ConvDec2WStr((unsigned int *)&idxSeq,str);
		bdaAttrib.name = bdaAttrib.name + _T("_") + str;

		if ( eChannelType == CT_GOOSE_STRUCT )
		{
			// BDAΪ�ṹ������
			bdaAttrib.type = daTypeID + _T("_") + str;
			if ( !pCache->SetBDAAttrib(daTypeID,idxSeq,bdaAttrib) )
				return -1;

			// ���Ĭ�ϵĽṹ���������Ͷ���
			AddDefaultDaType(pCache,bdaAttrib.type);
		}
		else
		{
			// DAΪ�ǽṹ������
			if ( !pCache->SetBDAAttrib(daTypeID,idxSeq,bdaAttrib) )
				return -1;
		}
	}
	return 0;
}

///
/// @brief
///     ���Ĭ�����õ�DaType
///
/// @param[in]
///     pCache            - ָ��SCL�������ݵĻ����ָ��
/// @param[in]
///     daTypeID          - Ҫ��ӵ�DaType��ID
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int  SCLGOOSEAnalysis::AddDefaultDaType(SCLCache *pCache, const std::wstring &daTypeID)
{
	if ( pCache == NULL)
		return -1;

	// Ĭ�Ͻṹ�庬��һ����������
	SCL_DATYPE_ATTRIB daTypeAttrib;
	if ( !pCache->GetDAType(daTypeID,daTypeAttrib) )
	{
		daTypeAttrib.id = daTypeID;
		if ( !pCache->AddDAType(daTypeAttrib) )
			return -1;
		SCL_BDAINDATYPE_ATTRIB bdaAttrib;
		bdaAttrib.name  = GOOSE_CHANNEL_TYPE_TEMPLATE[CT_GOOSE_POINT].name;
		bdaAttrib.bType = GOOSE_CHANNEL_TYPE_TEMPLATE[CT_GOOSE_POINT].bType;
		if ( !pCache->AddBDAIntoDAType(daTypeAttrib.id,bdaAttrib) )
			return -1;
	}
	return 0;
}

///
/// @brief
///     ��DaType1�е����ø��Ƶ�DaType2��
///
/// @param[in]
///     pCache            - ָ��SCL�������ݵĻ����ָ��
/// @param[in]
///     daTypeID1         - DaType1��ID
/// @param[in]
///     daTypeID2         - DaType2��ID
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int  SCLGOOSEAnalysis::CopyDaType(SCLCache *pCache, const std::wstring &daTypeID1, const std::wstring &daTypeID2)
{
	if ( pCache == NULL)
		return -1;

	if ( daTypeID1 != daTypeID2 )
	{
		// DaType2�������򴴽�
		SCL_DATYPE_ATTRIB daTypeAttrib;
		if ( !pCache->GetDAType(daTypeID2,daTypeAttrib) )
		{
			daTypeAttrib.id  = daTypeID2;
			if ( !pCache->AddDAType(daTypeAttrib) )
				return -1;
		}

		// ����DaType1
		int i = 0;
		SCL_BDAINDATYPE_ATTRIB bdaAttrib;
		while ( pCache->GetBDAAttrib(daTypeID1,i,bdaAttrib) )
		{
			if ( !pCache->AddBDAIntoDAType(daTypeID2,bdaAttrib) )
				return -1;
			i++;
		}
	}
	return 0;
}

///
/// @brief
///     �Ƴ�DaType��ָ��˳��ŵ�BDA��������BDA���ӵ�����DaType
///
/// @param[in]
///     pCache            - ָ��SCL�������ݵĻ����ָ��
/// @param[in]
///     daTypeID          - DaType��ID
/// @param[in]
///     idxSeq            - Ҫ�Ƴ���BDA��˳���
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int  SCLGOOSEAnalysis::RemoveBDAInDaType(SCLCache *pCache, const SCL_STR &daTypeID, int idxSeq)
{
	if ( pCache == NULL)
		return -1;

	SCL_BDAINDATYPE_ATTRIB bdaAttrib;
	if ( !pCache->GetBDAAttrib(daTypeID,idxSeq,bdaAttrib) )
		return -1;
	if ( bdaAttrib.bType == GOOSE_CHANNEL_TYPE_TEMPLATE[CT_GOOSE_STRUCT].bType )
	{
		SCL_STR daTypeID_t = bdaAttrib.type;
		int bdaNum = pCache->GetBDANumInDAType(daTypeID_t);
		for ( int i = 0; i < bdaNum; i++ )
		{
			if ( RemoveBDAInDaType(pCache,daTypeID_t,i) == -1 )
				return -1;
		}
	}
	if ( !pCache->RemoveBDAAttrib(daTypeID,idxSeq) )
		return -1;
	return 0;
}
