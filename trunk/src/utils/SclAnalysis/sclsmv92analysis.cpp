/// @file
///     sclsmv92analysis.cpp
///
/// @brief
///     SCL�ļ��й���IEC61850-9-2��������Ϣ�Ľ���
///
/// @note
///
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
///    |2013.02.18     |1.0.3.218  |���SMV92����������ߵ���Ϣ��SMV92 INPUT������            |
///    |2013.08.07     |1.0.5.807  |��ȡSMVͨ����Ϣʱ����Ӷ�ͨ����DOI��Ϣ������ʱ�Ĵ���      |
///    |2013.08.08     |1.0.5.808  |�޸�ĳ�߼��ڵ��Ƿ�ΪLLN0���ж�����                        |
///    |2013.08.08     |1.0.5.808  |�޸�SMVͨ���������Ϣ�ж�ģ��                             |
///    |2013.08.09     |1.0.5.809  |�޸�SMVͨ���������Ϣ�жϣ���Ӷ�ȫ����ĸ���жϡ���dUVal��daVal��ֵ���ж�|
///    |2013.08.09     |1.0.5.809  |�޸�SMVͨ����������Ϣ�жϣ���Ӷ��߼��ڵ�����ΪGGIOʱ��������Ϣ���жϡ��޸Ķ�ʱ������ͨ�����ж�|
///
#include "stdafx.h"
#include "sclsmv92analysis.h"

///
/// @brief
///     �����Ϣ�ж�ģ��
///
static std::wstring PHASE_A_TEMPLATE[]={L"A��", L"a��", L"����"};
static std::wstring PHASE_B_TEMPLATE[]={L"B��", L"b��", L"����"};
static std::wstring PHASE_C_TEMPLATE[]={L"C��", L"c��", L"����"};
static std::wstring PHASE_N_TEMPLATE[]={L"N��", L"n��", L"����", L"���Ե�", L"����"};
static std::wstring PHASE_X_TEMPLATE[]={L"X��", L"x��", L"����", L"ͬ��", L"ĸ��", L"��ȡ"};

SCLSmv92Analysis::SCLSmv92Analysis(ISCLCacheManager *pSclCacheMgr)
{
	m_pSclCacheMgr = (SCLCacheManager *)pSclCacheMgr;
}

SCLSmv92Analysis::~SCLSmv92Analysis()
{

}

///
/// @brief
///     ��ȡ�����ļ������е�SMV���ƿ��������������δ����ͨ�Ų����Ŀ��ƿ飩
///
/// @return
///     SMV���ƿ������ - �����ɹ��� -1 - ����ʧ��
///
int SCLSmv92Analysis::GetIEDSMVNum()
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	return pCache->GetSMVCommnNumInCache();
}

///
/// @brief
///     ��ȡ�����ļ������е�SMV���ƿ���Ϣ��������δ����ͨ�Ų����Ŀ��ƿ飩
///
/// @param[in]
///     iedSMVInfoArr - ����SMV���ƿ���Ϣ������
/// @param[in]
///     arrSize       - ���ݵ��������
///
/// @return
///     �����б�����Ϣ�������������ɹ���  -1������ʧ��
///
int SCLSmv92Analysis::GetAllIEDSMVInfo( SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfoArr, 
									    const int arrSize )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL || arrSize == 0 )
		return -1;

	SCL_IED_ATTRIB          iedAttrib;
	SCL_ACCESS_POINT_ATTRIB apAttrib;
	SCL_LDEVICE_ATTRIB      ldAttrib;
	SCL_SMVCTRL_ATTRIB      smvCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	SCL_CONNECTEDAP_ATTRIB *conntAPAttrib = new SCL_CONNECTEDAP_ATTRIB [arrSize];
	SCL_SMV_ATTRIB         *smvAttrib     = new SCL_SMV_ATTRIB         [arrSize];
	if ( conntAPAttrib == NULL || smvAttrib == NULL )
		return -1;

	// ��ȡ���ƿ��ͨ�Ų�����Ϣ
	if ( !pCache->GetAllSMVCommn(conntAPAttrib,smvAttrib,arrSize) )
		goto errorHandle;

	// ��ȡ���ƿ����װ�ñ�����ص���Ϣ��������IED����Ϣ��ϵͳ������
	// ע��SCL�ļ����ڶ�����ͨ�Ų�������û�ж���IED����ƿ������Ϣ�����
	int ctrlNum = 0;
	for (int i=0; i<arrSize; i++)
	{
		if ( !pCache->GetIEDAttrib(conntAPAttrib[i].iedName,iedAttrib) )
			continue;
		if ( !pCache->GetLDeviceAttrib(conntAPAttrib[i].iedName,smvAttrib[i].ldInst,apAttrib,ldAttrib) )
			continue;
		if ( !pCache->GetSMVCtrlAttribInLN0(conntAPAttrib[i].iedName,smvAttrib[i].ldInst,smvAttrib[i].cbName,smvCtrlAttrib) )
			continue;
		if ( !pCache->GetDataSetAttribInLN0(conntAPAttrib[i].iedName,smvAttrib[i].ldInst,smvCtrlAttrib.dataSet,dataSetAttrib) )
			continue;
		SCL2IEDSMVInfo(smvAttrib[i],iedAttrib,apAttrib,ldAttrib,dataSetAttrib,smvCtrlAttrib,&iedSMVInfoArr[ctrlNum]);
		ctrlNum++;
	}

	delete[] conntAPAttrib;
	delete[] smvAttrib;
	return ctrlNum;

errorHandle:
	delete[] conntAPAttrib;
	delete[] smvAttrib;
	return -1;
}

///
/// @brief
///     ��ȡָ��MAC��APPID��SMV���ƿ���Ϣ
///
/// @param[in]
///     smvMAC     - SMVͨ���鲥MAC��ַ
/// @param[in]
///     smvAPPID   - SMVͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
/// @param[in]
///     iedSMVInfo - ����SMV���ƿ���Ϣ�Ľṹ��
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int SCLSmv92Analysis::GetOneIEDSMVInfo( const unsigned char *smvMAC, const unsigned short smvAppID,
									    SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfo )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_CONNECTEDAP_ATTRIB  conntAPAttrib;
	SCL_SMV_ATTRIB          smvAttrib;
	SCL_IED_ATTRIB          iedAttrib;
	SCL_ACCESS_POINT_ATTRIB apAttrib;
	SCL_LDEVICE_ATTRIB      ldAttrib;
	SCL_SMVCTRL_ATTRIB      smvCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	unsigned char           macAddr[SCDSMV92ANALYSIS_MAC_SIZE];

	// ��ȡ���ƿ��ͨ�Ų�����Ϣ
	if ( !pCache->GetSMVCommn(smvAppID,conntAPAttrib,smvAttrib) )
		return -1;

	// Mac��ַ�ж�
	ConvMacWStr2Char(smvAttrib.macAddr,macAddr);
	if ( !IsMacAddrSame(smvMAC,macAddr) )
		return -1;

	// ��ȡ���ƿ����װ�ñ�����ص���Ϣ��������IED����Ϣ��ϵͳ������
	if ( !pCache->GetIEDAttrib(conntAPAttrib.iedName,iedAttrib) )
		return -1;
	if ( !pCache->GetLDeviceAttrib(conntAPAttrib.iedName,smvAttrib.ldInst,apAttrib,ldAttrib) )
		return -1;
	if ( !pCache->GetSMVCtrlAttribInLN0(conntAPAttrib.iedName,smvAttrib.ldInst,smvAttrib.cbName,smvCtrlAttrib) )
		return -1;
	if ( !pCache->GetDataSetAttribInLN0(conntAPAttrib.iedName,smvAttrib.ldInst,smvCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;
	SCL2IEDSMVInfo(smvAttrib,iedAttrib,apAttrib,ldAttrib,dataSetAttrib,smvCtrlAttrib,iedSMVInfo);

	return 0;
}

///
/// @brief
///     ��ȡָ��MAC��APPID��ص�SMV���ƿ������Ƶ�DataSet��ͨ����FCDA��������
///
/// @param[in]
///     smvMAC     - SMVͨ���鲥MAC��ַ
/// @param[in]
///     smvAPPID   - SMVͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
///
/// @return
///     DataSet��FCDA�������������ɹ���  -1��ʧ��
///
int SCLSmv92Analysis::GetIEDSMVDataSetNum( const unsigned char *smvMAC, const unsigned short smvAppID )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_CONNECTEDAP_ATTRIB  conntAPAttrib;
	SCL_SMV_ATTRIB          smvAttrib;
	SCL_SMVCTRL_ATTRIB      smvCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	unsigned char           macAddr[SCDSMV92ANALYSIS_MAC_SIZE];

	if ( !pCache->GetSMVCommn(smvAppID,conntAPAttrib,smvAttrib) )
		return -1;
	ConvMacWStr2Char(smvAttrib.macAddr,macAddr);
	if ( !IsMacAddrSame(smvMAC,macAddr) )
		return -1;
	if ( !pCache->GetSMVCtrlAttribInLN0(conntAPAttrib.iedName,smvAttrib.ldInst,smvAttrib.cbName,smvCtrlAttrib) )
		return -1;
	if ( !pCache->GetDataSetAttribInLN0(conntAPAttrib.iedName,smvAttrib.ldInst,smvCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;

	return dataSetAttrib.fcdaNum;
}

///
/// @brief
///     ��ȡָ��MAC��APPID��ص�SMV���ƿ������Ƶ�DataSet������ͨ����FCDA����Ϣ
///
/// @param[in]
///     smvMAC         - SMVͨ���鲥MAC��ַ
/// @param[in]
///     smvAPPID       - SMVͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
/// @param[in]
///     dataSetInfoArr - ����SMV��ͨ����Ϣ������
/// @param[in]
///     arrSize        - ������������
///
/// @return
///     ���鱣��ͨ����Ϣ�������������ɹ���  -1��ʧ��
///
int SCLSmv92Analysis::GetAllIEDSMVDataSetInfo( const unsigned char *smvMAC, const unsigned short smvAppID,
											   SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfoArr,
											   const int arrSize )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_CONNECTEDAP_ATTRIB  conntAPAttrib;
	SCL_SMV_ATTRIB          smvAttrib;
	SCL_SMVCTRL_ATTRIB      smvCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	SCL_FCDA_ATTRIB         fcdaAttrib;
	SCL_DOI_ATTRIB          doiAttrib;
	SCL_STR                 lnName;
	SCL_STR                 lnDesc;
	unsigned char           macAddr[SCDSMV92ANALYSIS_MAC_SIZE];

	if ( !pCache->GetSMVCommn(smvAppID,conntAPAttrib,smvAttrib) )
		return -1;
	ConvMacWStr2Char(smvAttrib.macAddr,macAddr);
	if ( !IsMacAddrSame(smvMAC,macAddr) )
		return -1;
	if ( !pCache->GetSMVCtrlAttribInLN0(conntAPAttrib.iedName,smvAttrib.ldInst,smvAttrib.cbName,smvCtrlAttrib) )
		return -1;
	if ( !pCache->GetDataSetAttribInLN0(conntAPAttrib.iedName,smvAttrib.ldInst,smvCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;
	int i = 0;
	for ( i = 0; i < arrSize ; i++ )
	{
		if ( !pCache->GetFCDAAttrib(conntAPAttrib.iedName,smvAttrib.ldInst,_T("LLN0"),smvCtrlAttrib.dataSet,i,fcdaAttrib) )
			return -1;
		lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
		if ( fcdaAttrib.lnClass == _T("LLN0") )
		{
			SCL_LN0_ATTRIB ln0Attrib;
			if ( !pCache->GetLN0Attrib(conntAPAttrib.iedName,smvAttrib.ldInst,ln0Attrib) )
				lnDesc = _T("");
			else
				lnDesc = ln0Attrib.lnDesc;
		}
		else
		{
			SCL_LN_ATTRIB lnAttrib;
			if ( !pCache->GetLNAttrib(conntAPAttrib.iedName,smvAttrib.ldInst,lnName,lnAttrib) )
				lnDesc = _T("");
			else
				lnDesc = lnAttrib.lnDesc;
		}
		if ( !pCache->GetDOIAttribInLNorLN0(conntAPAttrib.iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
		{
			doiAttrib.doName = fcdaAttrib.doName;
			doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
		}
		SCL2DataSetInfo(dataSetAttrib,fcdaAttrib,lnDesc,doiAttrib,&dataSetInfoArr[i]);
	}

	return i;
}

///
/// @brief
///     ��ȡָ��MAC��APPID��ص�SMV���ƿ������Ƶ�DataSet��ĳһ˳��ŵ�ͨ����FCDA����Ϣ
///
/// @param[in]
///     smvMAC      - SMVͨ���鲥MAC��ַ
/// @param[in]
///     smvAPPID    - SMVͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
/// @param[in]
///     idxSeq      - ͨ����˳���(��0��ʼ)
/// @param[in]
///     dataSetInfo - ͨ����Ϣ
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int SCLSmv92Analysis::GetOneIEDSMVDataSetInfo( const unsigned char *smvMAC, const unsigned short smvAppID,
											   const int            idxSeq,
											   SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_CONNECTEDAP_ATTRIB  conntAPAttrib;
	SCL_SMV_ATTRIB          smvAttrib;
	SCL_SMVCTRL_ATTRIB      smvCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	SCL_FCDA_ATTRIB         fcdaAttrib;
	SCL_DOI_ATTRIB          doiAttrib;
	SCL_STR                 lnName;
	SCL_STR                 lnDesc;
	unsigned char           macAddr[6];

	if ( !pCache->GetSMVCommn(smvAppID,conntAPAttrib,smvAttrib) )
		return -1;
	ConvMacWStr2Char(smvAttrib.macAddr,macAddr);
	if ( !IsMacAddrSame(smvMAC,macAddr) )
		return -1;
	if ( !pCache->GetSMVCtrlAttribInLN0(conntAPAttrib.iedName,smvAttrib.ldInst,smvAttrib.cbName,smvCtrlAttrib) )
		return -1;
	if ( !pCache->GetDataSetAttribInLN0(conntAPAttrib.iedName,smvAttrib.ldInst,smvCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;
	if ( !pCache->GetFCDAAttrib(conntAPAttrib.iedName,smvAttrib.ldInst,_T("LLN0"),smvCtrlAttrib.dataSet,idxSeq,fcdaAttrib) )
		return -1;
	lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
	if ( fcdaAttrib.lnClass == _T("LLN0") )
	{
		SCL_LN0_ATTRIB ln0Attrib;
		if ( pCache->GetLN0Attrib(conntAPAttrib.iedName,smvAttrib.ldInst,ln0Attrib) )
			lnDesc = _T("");
		else
			lnDesc = ln0Attrib.lnDesc;
	}
	else
	{
		SCL_LN_ATTRIB lnAttrib;
		if ( !pCache->GetLNAttrib(conntAPAttrib.iedName,smvAttrib.ldInst,lnName,lnAttrib) )
			lnDesc = _T("");
		else
			lnDesc = lnAttrib.lnDesc;
	}
	if ( !pCache->GetDOIAttribInLNorLN0(conntAPAttrib.iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
	{
		doiAttrib.doName = fcdaAttrib.doName;
		doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
	}
	SCL2DataSetInfo(dataSetAttrib,fcdaAttrib,lnDesc,doiAttrib,dataSetInfo);

	return 0;
}

///
/// @brief
///     ��ȡָ��MAC��APPID��ص�SMV���ƿ������Ƶ�DataSet��ĳһͨ����FCDA����˳���
///
/// @param[in]
///     smvMAC      - SMVͨ���鲥MAC��ַ
/// @param[in]
///     smvAPPID    - SMVͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
/// @param[in]
///     dataSetInfo - ����ͨ����Ϣ�Ľṹ��
///
/// @return
///     ͨ����˳��ţ������ɹ���  -1��ʧ��
///
int SCLSmv92Analysis::GetOneIEDSMVDataSetIdxSeq( const unsigned char *smvMAC, const unsigned short smvAppID,
												 const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_CONNECTEDAP_ATTRIB  conntAPAttrib;
	SCL_SMV_ATTRIB          smvAttrib;
	SCL_SMVCTRL_ATTRIB      smvCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	SCL_FCDA_ATTRIB         fcdaAttrib;
	SCL_DOI_ATTRIB          doiAttrib;
	unsigned char           macAddr[6];

	if ( !pCache->GetSMVCommn(smvAppID,conntAPAttrib,smvAttrib) )
		return -1;
	ConvMacWStr2Char(smvAttrib.macAddr,macAddr);
	if ( !IsMacAddrSame(smvMAC,macAddr) )
		return -1;
	if ( !pCache->GetSMVCtrlAttribInLN0(conntAPAttrib.iedName,smvAttrib.ldInst,smvAttrib.cbName,smvCtrlAttrib) )
		return -1;
	if ( !pCache->GetDataSetAttribInLN0(conntAPAttrib.iedName,smvAttrib.ldInst,smvCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;
	for ( int i = 0; i < dataSetAttrib.fcdaNum ; i++ )
	{
		if ( !pCache->GetFCDAAttrib(conntAPAttrib.iedName,smvAttrib.ldInst,_T("LLN0"),smvCtrlAttrib.dataSet,i,fcdaAttrib) )
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
///     ��ȡĳһIED�����󶨵��ⲿ�źţ�ExtRef����SMV�����źţ�������
///     ÿ��װ�õ�LLN0���ֺ��и�װ�õ�GOOSE�Ͳ���ֵ�����������Ϣ
///
/// @param[in]
///     iedName      - IED������
///
/// @return
///     ExtRef���������ɹ���  -1��ʧ��
///
int SCLSmv92Analysis::GetOneIEDInputsExtRefNum(const std::wstring &iedName)
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
int SCLSmv92Analysis::GetOneIEDSVINExtRef(const std::wstring &iedName, SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO *inputsInfo, const int arrSize)
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_EXTREF_ATTRIB      *extRefAttrib = new SCL_EXTREF_ATTRIB[arrSize];
	if ( extRefAttrib == NULL )
		return -1;

	if ( !pCache->GetAllInputsExtRefInIED(iedName,extRefAttrib,arrSize) )
		goto errorHandle;

	int svInNum = 0;
	for (int i=0; i<arrSize; i++)
	{
		if ( extRefAttrib[i].intAddr.find(_T("SVIN")) != -1 )
		{
			if ( !SCL2InputsInfo(iedName,extRefAttrib[i],&inputsInfo[svInNum]) )
				goto errorHandle;
			svInNum++;
		}
	}

	delete[] extRefAttrib;
	return svInNum;

errorHandle:
	delete[] extRefAttrib;
	return -1;
}

///
/// @brief
///     ��ȡ����ĳһ�ⲿ�ź������SMV���ƿ���Ϣ
///
/// @param[in]
///     inputsInfo - �ⲿ�źŵ�������Ϣ
/// @param[in]
///     iedSMVInfo - ���Ƹ��ⲿ�ź������SMV���ƿ���Ϣ
///
/// @return
///     0 - �����ɹ��� -1 - ����ʧ��
///
int SCLSmv92Analysis::GetOutAddrSMVInfo(const SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO &inputsInfo, SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfo)
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
	SCL_SMVCTRL_ATTRIB smvCtrlAttrib;
	SCL_SMV_ATTRIB smvAttrib;

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
			if ( ( inputsInfo.daName_Out == fcdaAttrib.daName || fcdaAttrib.daName == _T("")) &&
				 ( inputsInfo.doName_Out == fcdaAttrib.doName ) &&
				 ( inputsInfo.lnName_Out == ( fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst ) ) )
			{
				if ( pCache->GetSMVCtrlAttribWithDatSet(inputsInfo.iedName_Out,inputsInfo.ldInst_Out,dataSetAttrib.dataSetName,smvCtrlAttrib))
				{
					if ( pCache->GetSMVCommnWithIdx(inputsInfo.iedName_Out,inputsInfo.ldInst_Out,smvCtrlAttrib.cbName,conntAPAttirb,smvAttrib) )
					{
						SCL2IEDSMVInfo(smvAttrib,iedAttrib,apAttrib,ldAttrib,dataSetAttrib,smvCtrlAttrib,iedSMVInfo);
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
///     ��ȡȫ����SMV���ƿ����Ŀ������δ����ͨ�Ų����Ŀ��ƿ飩
///
/// @return
///     ���ƿ����Ŀ�������ɹ���  -1��ʧ��
///
int SCLSmv92Analysis::GetAllIEDSMVNumByCtrlRef( void )
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
			ctrlNum += pCache->GetSMVCtrlNumInLN0(iedAttrib.iedName,ldAttrib.ldInst);
		}		
	}

	return ctrlNum;
}

///
/// @brief
///     ��ȡȫ����SMV���ƿ����Ϣ������δ����ͨ�Ų����Ŀ��ƿ飩
///     ���У�δ���ò��ֽ���Ĭ������
///
/// @param[in]
///     iedSMVInfoArr - ����SMV���ƿ���Ϣ������
/// @param[in]
///     arrSize       - �����С
///
/// @return
///     �����б���Ŀ��ƿ�������������ɹ���  -1��ʧ��
///
int SCLSmv92Analysis::GetAllIEDSMVInfoByCtrlRef(SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfoArr, 
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

			int oneLdCtrlNum  = pCache->GetSMVCtrlNumInLN0(iedAttrib.iedName,ldAttrib.ldInst);
			for ( int k = 0; k < oneLdCtrlNum; k++ )
			{
				if ( ctrlNum >= arrSize )
					break;

				// ��ȡ���ƿ���Ϣ
				SCL_SMVCTRL_ATTRIB ctrlAttrib;
				if ( !pCache->GetSMVCtrlAttribInLN0(iedAttrib.iedName,ldAttrib.ldInst,k,ctrlAttrib) )
					continue;

				// ��ȡ����ƿ���������ݼ�����Ϣ
				SCL_DATASET_ATTRIB dataSetAttrib;
				if ( !pCache->GetDataSetAttribInLN0(iedAttrib.iedName,ldAttrib.ldInst,ctrlAttrib.dataSet,dataSetAttrib) )
					continue;

				// ��ȡ����ƿ������ͨ�Ų�����Ϣ
				SCL_CONNECTEDAP_ATTRIB conntAPAttrib;
				SCL_SMV_ATTRIB commnAttrib;
				if ( !pCache->GetSMVCommnWithIdx(iedAttrib.iedName,ldAttrib.ldInst,ctrlAttrib.cbName,
					conntAPAttrib,commnAttrib) )
				{
					// δ����ͨ�Ų�����������Ĭ��ֵ
					conntAPAttrib.iedName = iedAttrib.iedName;
					conntAPAttrib.apName  = apAttrib.apName;
					commnAttrib.ldInst    = ldAttrib.ldInst;
					commnAttrib.cbName    = ctrlAttrib.cbName;
					unsigned int appID = 0xFFFF;
					ConvHex2WStr(&appID,commnAttrib.appID);
					unsigned char macAddr[] = {0x01,0x0C,0xCD,0x04,0xFF,0xFF};
					ConvMacChar2WStr(macAddr,commnAttrib.macAddr);
					commnAttrib.vlanID    = L"0";
					commnAttrib.vlanPriority = L"4";
					if ( !pCache->AddSMVCommn(conntAPAttrib,commnAttrib) )
						return -1;
				}
				SCL2IEDSMVInfo(commnAttrib,iedAttrib,apAttrib,ldAttrib,dataSetAttrib,ctrlAttrib,&iedSMVInfoArr[ctrlNum]);
				ctrlNum++;
			}
		}		
	}
	return ctrlNum;
}

///
/// @brief
///     ����һ��SMV���ƿ�
///
/// @param[in]
///     iedSMVInfo - SMV���ƿ���Ϣ
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int SCLSmv92Analysis::SetOneIEDSMVInfoByCtrlRef(const SCDSMV92ANALYSIS_IEDSMV_INFO &iedSMVInfo)
{
	// �жϻ����Ƿ����
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// ���ÿ��ƿ���Ϣ
	SCL_SMVCTRL_ATTRIB ctrlAttrib;
	ctrlAttrib.cbName = iedSMVInfo.smvCbName;
	ctrlAttrib.cbDesc = iedSMVInfo.smvDesc;
	ConvDec2WStr(&iedSMVInfo.smvCbConfRev,ctrlAttrib.confRev);
	ctrlAttrib.smvID   = iedSMVInfo.smvCbSmvID;
	ctrlAttrib.dataSet = iedSMVInfo.smvCbDataSet;
	ConvDec2WStr(&iedSMVInfo.smvCbNofASDU,ctrlAttrib.nofASDU);
	ConvDec2WStr(&iedSMVInfo.smvCbSmpRate,ctrlAttrib.smpRate);
	ctrlAttrib.multicast      = BOOLEAN_SMV92toSTD(iedSMVInfo.smvCbMulticast);
	ctrlAttrib.smvOptReftTm   = BOOLEAN_SMV92toSTD(iedSMVInfo.smvCbOptRefreshTime);
	ctrlAttrib.smvOptSmpRate  = BOOLEAN_SMV92toSTD(iedSMVInfo.smvCbOptSmpRate);
	ctrlAttrib.smvOptDataRef  = BOOLEAN_SMV92toSTD(iedSMVInfo.smvCbOptDataRef);
	ctrlAttrib.smvOptSmpSync  = BOOLEAN_SMV92toSTD(iedSMVInfo.smvCbOptSmpSync);
	ctrlAttrib.smvOptSecurity = BOOLEAN_SMV92toSTD(iedSMVInfo.smvCbOptSecurity);
	if ( !pCache->SetSMVCtrlAttribInLN0(iedSMVInfo.iedName,iedSMVInfo.iedLdInst,iedSMVInfo.smvCbName,ctrlAttrib) )
		return -1;

	// ���ÿ��ƿ��Ӧ��ͨ�Ų�����Ϣ
	SCL_CONNECTEDAP_ATTRIB conntAPAttrib;
	SCL_SMV_ATTRIB commnAttrib;
	conntAPAttrib.iedName = iedSMVInfo.iedName;
	conntAPAttrib.apName  = iedSMVInfo.iedAPName;
	commnAttrib.ldInst    = iedSMVInfo.iedLdInst;
	commnAttrib.cbName    = iedSMVInfo.smvCbName;
	ConvMacChar2WStr(iedSMVInfo.smvMAC,commnAttrib.macAddr);

	unsigned int tmp = (unsigned int)iedSMVInfo.smvAppID;
	ConvHex2WStr(&tmp,commnAttrib.appID);
	tmp = (unsigned int)iedSMVInfo.smvVlanID;
	ConvHex2WStr(&tmp,commnAttrib.vlanID);
	tmp = (unsigned int)iedSMVInfo.smvVlanPri;
	ConvDec2WStr(&tmp,commnAttrib.vlanPriority);

	if ( !pCache->SetSMVCommnWithIdx(iedSMVInfo.iedName,iedSMVInfo.iedLdInst,iedSMVInfo.smvCbName,
		conntAPAttrib,commnAttrib) )
		return -1;

	// ���ÿ��ƿ��Ӧ�����ݼ���Ϣ
	SCL_DATASET_ATTRIB dataSetAttrib;
	dataSetAttrib.dataSetName = iedSMVInfo.dataSetName;
	dataSetAttrib.dataSetDesc = iedSMVInfo.dataSetDesc;
	dataSetAttrib.fcdaNum     = iedSMVInfo.channelNum;
	if ( !pCache->SetDataSetAttribInLN0(iedSMVInfo.iedName,iedSMVInfo.iedLdInst,iedSMVInfo.dataSetName,
		dataSetAttrib))
		return -1;

	return 0;
}

///
/// @brief
///     ��ȡָ��������ַ��SMV���ƿ������Ƶ�DataSet��ͨ����FCDA��������
///
/// @param[in]
///     iedName        - SMV���ƿ����ڵ�IED������
/// @param[in]
///     ldInst         - SMV���ƿ����ڵ�LDevice��Inst��
/// @param[in]
///     cbName         - SMV���ƿ������
///
/// @return
///     DataSet��FCDA�������������ɹ���  -1��ʧ��
///
int SCLSmv92Analysis::GetIEDSMVDataSetNum( const std::wstring &iedName, const std::wstring &ldInst, const std::wstring &cbName )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_SMVCTRL_ATTRIB      smvCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;

	if ( !pCache->GetSMVCtrlAttribInLN0(iedName,ldInst,cbName,smvCtrlAttrib) )
		return -1;
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,smvCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;

	return dataSetAttrib.fcdaNum;
}

///
/// @brief
///     ��ȡָ��������ַ��SMV���ƿ������Ƶ�DataSet������ͨ����FCDA����Ϣ
///
/// @param[in]
///     iedName        - SMV���ƿ����ڵ�IED������
/// @param[in]
///     ldInst         - SMV���ƿ����ڵ�LDevice��Inst��
/// @param[in]
///     cbName         - SMV���ƿ������
/// @param[in]
///     dataSetInfoArr - ����SMV��ͨ����Ϣ������
/// @param[in]
///     arrSize        - ������������
///
/// @return
///     ���鱣��ͨ����Ϣ�������������ɹ���  -1��ʧ��
///
int SCLSmv92Analysis::GetAllIEDSMVDataSetInfo( const std::wstring &iedName, const std::wstring &ldInst,
											   const std::wstring &cbName, SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfoArr,
											   const int arrSize )
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_SMVCTRL_ATTRIB      smvCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	SCL_FCDA_ATTRIB         fcdaAttrib;
	SCL_DOI_ATTRIB          doiAttrib;
	SCL_STR                 lnName;
	SCL_STR                 lnDesc;

	if ( !pCache->GetSMVCtrlAttribInLN0(iedName,ldInst,cbName,smvCtrlAttrib) )
		return -1;
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,smvCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;
	int i;
	for ( i = 0; i < dataSetAttrib.fcdaNum ; i++ )
	{
		if ( i >= arrSize )
			break;

		if ( !pCache->GetFCDAAttrib(iedName,ldInst,_T("LLN0"),smvCtrlAttrib.dataSet,i,fcdaAttrib) )
			return -1;
		lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
		if ( fcdaAttrib.lnClass == _T("LLN0") )
		{
			SCL_LN0_ATTRIB ln0Attrib;
			if ( !pCache->GetLN0Attrib(iedName,ldInst,ln0Attrib) )
				lnDesc = _T("");
			else
				lnDesc = ln0Attrib.lnDesc;
		}
		else
		{
			SCL_LN_ATTRIB lnAttrib;
			if ( !pCache->GetLNAttrib(iedName,ldInst,lnName,lnAttrib) )
				lnDesc = _T("");
			else
				lnDesc = lnAttrib.lnDesc;
		}

		if ( !pCache->GetDOIAttribInLNorLN0(iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
		{
			doiAttrib.doName = fcdaAttrib.doName;
			doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
		}

		SCL2DataSetInfo(dataSetAttrib,fcdaAttrib,lnDesc,doiAttrib,&dataSetInfoArr[i]);
	}

	return i;
}

///
/// @brief
///     ��ȡָ��������ַ��SMV���ƿ������Ƶ�DataSet��ĳһ˳��ŵ�ͨ����FCDA����Ϣ
///
/// @param[in]
///     iedName        - SMV���ƿ����ڵ�IED������
/// @param[in]
///     ldInst         - SMV���ƿ����ڵ�LDevice��Inst��
/// @param[in]
///     cbName         - SMV���ƿ������
/// @param[in]
///     idxSeq         - SMVͨ����˳���
/// @param[in]
///     dataSetInfo    - ָ�򱣴�SMV��ͨ����Ϣ�Ľṹ���ָ��
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int SCLSmv92Analysis::GetOneIEDSMVDataSetInfo(const std::wstring &iedName, const std::wstring &ldInst,
									          const std::wstring &cbName, const int idxSeq,
											  SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo)
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_SMVCTRL_ATTRIB      smvCtrlAttrib;
	SCL_DATASET_ATTRIB      dataSetAttrib;
	SCL_FCDA_ATTRIB         fcdaAttrib;
	SCL_DOI_ATTRIB          doiAttrib;
	SCL_STR                 lnName;
	SCL_STR                 lnDesc;

	if ( !pCache->GetSMVCtrlAttribInLN0(iedName,ldInst,cbName,smvCtrlAttrib) )
		return -1;
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,smvCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;

	if ( !pCache->GetFCDAAttrib(iedName,ldInst,_T("LLN0"),smvCtrlAttrib.dataSet,idxSeq,fcdaAttrib) )
		return -1;
	lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
	if ( fcdaAttrib.lnClass == _T("LLN0") )
	{
		SCL_LN0_ATTRIB ln0Attrib;
		if ( !pCache->GetLN0Attrib(iedName,ldInst,ln0Attrib) )
			lnDesc = _T("");
		else
			lnDesc = ln0Attrib.lnDesc;
	}
	else
	{
		SCL_LN_ATTRIB lnAttrib;
		if ( !pCache->GetLNAttrib(iedName,ldInst,lnName,lnAttrib) )
			lnDesc = _T("");
		else
			lnDesc = lnAttrib.lnDesc;
	}
	if ( !pCache->GetDOIAttribInLNorLN0(iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
	{
		doiAttrib.doName = fcdaAttrib.doName;
		doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
	}
	SCL2DataSetInfo(dataSetAttrib,fcdaAttrib,lnDesc,doiAttrib,dataSetInfo);

	return 0;
}

///
/// @brief
///     ����SMV���ƿ����������ݼ��е�һ��ͨ����FCDA��
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
int SCLSmv92Analysis::SetOneIEDSMVDataSetInfo_Def( const std::wstring &iedName, const std::wstring &ldInst,
												   SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
												   CHANNEL_TYPE_SMV eChannelType )
{
	// �жϻ����Ƿ����
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// ����DataSet�е�ͨ��������Ϣ����FCDA���������������
	SCL_DATASET_ATTRIB dataSetAttrib;
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,dataSetInfo.dataSetName,dataSetAttrib) )
		return -1;
	
	// �Զ���ͨ��lnName
	SCL_STR lnPrefix = L"DEF_" + iedName + ldInst + L"_" + dataSetInfo.dataSetName;
	std::wstring str;
	ConvDec2WStr((unsigned int *)&dataSetInfo.idxSeq,str);
	SCL_STR lnInst = str;
	SCL_STR lnClass,doName;
	// ����ͨ�����ͣ�����Ĭ��lnClass��doName
	switch ( eChannelType )
	{
	case CT_SMV_TIME:
		{
			lnClass = _T("TVTR");
			doName  = _T("DelayTRtg");
		}
		break;
	case CT_SMV_CURRENT:
		{
			lnClass = _T("TCTR");
			doName  = _T("Amp");
		}
		break;
	case CT_SMV_VOLTAGE:
		{
			lnClass = _T("TVTR");
			doName  = _T("Vol");
		}
	default:
		{
			lnClass = _T("GGIO_EX");
			doName  = _T("Def_Smv");
		}
	}

	SCL_FCDA_ATTRIB fcdaAttrib;
	fcdaAttrib.idxSeq   = dataSetInfo.idxSeq;
	fcdaAttrib.ldInst   = dataSetInfo.ldInst;
	fcdaAttrib.lnPrefix = dataSetInfo.lnPrefix = lnPrefix;
	fcdaAttrib.lnClass  = dataSetInfo.lnClass  = lnClass;
	fcdaAttrib.lnInst   = dataSetInfo.lnInst   = lnInst;
	fcdaAttrib.fc       = dataSetInfo.daFc     = L"MX";
	fcdaAttrib.doName   = dataSetInfo.doName   = doName;
	fcdaAttrib.daName   = dataSetInfo.daName   = L"";

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

	// ���¸�ͨ�����ڵ��߼��ڵ����Ϣ
	SCL_STR lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
	SCL_STR lnType = _T("XJ_") + dataSetInfo.lnClass + _T("_DEF");
	if ( fcdaAttrib.lnClass == _T("LLN0") )
	{
		SCL_LN0_ATTRIB ln0Attrib;
		if ( !pCache->GetLN0Attrib(iedName,ldInst,ln0Attrib) )
		{
			// �߼��ڵ㲻���ڣ����½�����ʹ���Զ��������ģ��
			ln0Attrib.lnClass = dataSetInfo.lnClass;
			ln0Attrib.lnInst  = dataSetInfo.lnInst;
			ln0Attrib.lnType  = lnType;
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
	}
	else
	{
		SCL_LN_ATTRIB lnAttrib;
		if ( !pCache->GetLNAttrib(iedName,ldInst,lnName,lnAttrib) )
		{
			// �߼��ڵ㲻���ڣ����½�����ʹ���Զ��������ģ��
			lnAttrib.lnPrefix = dataSetInfo.lnPrefix;
			lnAttrib.lnClass  = dataSetInfo.lnClass;
			lnAttrib.lnInst   = dataSetInfo.lnInst;
			lnAttrib.lnType   = lnType;
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
	}

	// ���¸�ͨ����Ӧ��DOI����Ϣ�����������½�
	SCL_DOI_ATTRIB doiAttrib;
	if ( !pCache->GetDOIAttribInLNorLN0(iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
	{
		doiAttrib.doName = dataSetInfo.doName;
		doiAttrib.doDesc = dataSetInfo.doDesc;
		doiAttrib.dUVal  = dataSetInfo.daVal;
		doiAttrib.dUSAddr = dataSetInfo.dasAddr;
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
		doiAttrib.dUVal  = dataSetInfo.daVal;
		if ( !pCache->SetDOIAttribInLNorLN0(iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
			return -1;
	}

	// ����Զ�������ģ��
	if ( AddDefChannelDataTemplates(lnType,dataSetInfo.lnClass,dataSetInfo.doName) == -1 )
		return -1;

	return 0;
}

///
/// @brief
///     ��SMV���ƿ����������ݼ�ĩβ���һ��ͨ����FCDA��
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
int SCLSmv92Analysis::AddOneIEDSMVDataSetInfo_Def( const std::wstring &iedName, const std::wstring &ldInst,
												  SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
												  CHANNEL_TYPE_SMV eChannelType )
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

	// �Զ���ͨ��lnName
	SCL_STR lnPrefix = L"DEF_" + iedName + ldInst + L"_" + dataSetInfo.dataSetName;
	std::wstring str;
	ConvDec2WStr((unsigned int *)&dataSetInfo.idxSeq,str);
	SCL_STR lnInst = str;
	SCL_STR lnClass,doName;
	// ����ͨ�����ͣ�����Ĭ��lnClass��doName
	switch ( eChannelType )
	{
	case CT_SMV_TIME:
		{
			lnClass = _T("TVTR");
			doName  = _T("DelayTRtg");
		}
		break;
	case CT_SMV_CURRENT:
		{
			lnClass = _T("TCTR");
			doName  = _T("Amp");
		}
		break;
	case CT_SMV_VOLTAGE:
		{
			lnClass = _T("TVTR");
			doName  = _T("Vol");
		}
	default:
		{
			lnClass = _T("GGIO_EX");
			doName  = _T("Def_Smv");
		}
	}

	SCL_FCDA_ATTRIB fcdaAttrib;
	fcdaAttrib.idxSeq   = dataSetInfo.idxSeq;
	fcdaAttrib.ldInst   = dataSetInfo.ldInst;
	fcdaAttrib.lnPrefix = dataSetInfo.lnPrefix = lnPrefix;
	fcdaAttrib.lnClass  = dataSetInfo.lnClass  = lnClass;
	fcdaAttrib.lnInst   = dataSetInfo.lnInst   = lnInst;
	fcdaAttrib.fc       = dataSetInfo.daFc     = L"MX";
	fcdaAttrib.doName   = dataSetInfo.doName   = doName;
	fcdaAttrib.daName   = dataSetInfo.daName   = L"";

	// ���FCDA
	if ( !pCache->AddFCDAIntoDatSet(iedName,ldInst,_T("LLN0"),dataSetInfo.dataSetName,fcdaAttrib) )
		return -1;

	// �������ݼ���Ϣ
	dataSetAttrib.fcdaNum++;
	if ( !pCache->SetDataSetAttribInLN0(iedName,ldInst,dataSetAttrib.dataSetName,dataSetAttrib) )
		return -1;

	// ���¸�ͨ�����ڵ��߼��ڵ����Ϣ
	SCL_STR lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
	SCL_STR lnType = _T("XJ_") + dataSetInfo.lnClass + _T("_DEF");
	if ( fcdaAttrib.lnClass == _T("LLN0") )
	{
		SCL_LN0_ATTRIB ln0Attrib;
		if ( !pCache->GetLN0Attrib(iedName,ldInst,ln0Attrib) )
		{
			// �߼��ڵ㲻���ڣ����½�����ʹ���Զ��������ģ��
			ln0Attrib.lnClass = dataSetInfo.lnClass;
			ln0Attrib.lnInst  = dataSetInfo.lnInst;
			ln0Attrib.lnType  = lnType;
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
	}
	else
	{
		SCL_LN_ATTRIB lnAttrib;
		if ( !pCache->GetLNAttrib(iedName,ldInst,lnName,lnAttrib) )
		{
			// �߼��ڵ㲻���ڣ����½�����ʹ���Զ��������ģ��
			lnAttrib.lnPrefix = dataSetInfo.lnPrefix;
			lnAttrib.lnClass  = dataSetInfo.lnClass;
			lnAttrib.lnInst   = dataSetInfo.lnInst;
			lnAttrib.lnType   = lnType;
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
	}

	// ���¸�ͨ����Ӧ��DOI����Ϣ�����������½�
	SCL_DOI_ATTRIB doiAttrib;
	if ( !pCache->GetDOIAttribInLNorLN0(iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
	{
		doiAttrib.doName = dataSetInfo.doName;
		doiAttrib.doDesc = dataSetInfo.doDesc;
		doiAttrib.dUVal  = dataSetInfo.daVal;
		doiAttrib.dUSAddr = dataSetInfo.dasAddr;
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
		doiAttrib.dUVal  = dataSetInfo.daVal;
		if ( !pCache->SetDOIAttribInLNorLN0(iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
			return -1;
	}

	// ����Զ�������ģ��
	if ( AddDefChannelDataTemplates(lnType,dataSetInfo.lnClass,dataSetInfo.doName) == -1 )
		return -1;

	return 0;
}

///
/// @brief
///     ��SMVͨ������ĩβ������ͨ������ĳһͨ������Ϣ����n��
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
int SCLSmv92Analysis::CopyIEDSMVDataSetInfoToSetEnd(const std::wstring &iedName, const std::wstring &ldInst,
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
///     �Ƴ�SMV���ƿ����������ݼ��е�һ��ͨ��
///
/// @param[in]
///     iedName    - ��ͨ����FCDA���Ķ�����Ϣ���ڵ�IED������
/// @param[in]
///     ldInst     - ��ͨ����FCDA���Ķ�����Ϣ���ڵ�LDevice��Inst��
/// @param[in]
///     datSetName - ��ͨ����FCDA���Ķ�����Ϣ���ڵ�DataSet������
/// @param[in]
///     idxSeq     - ��ͨ����FCDA����˳���
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int SCLSmv92Analysis::RemoveOneIEDSMVDataSetInfo(const std::wstring &iedName, const std::wstring &ldInst,
												 const std::wstring &datSetName, const int idxSeq)
{
	// �жϻ����Ƿ����
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// ���ÿ��ƿ��Ӧ�����ݼ���ͨ����Ŀ��Ϣ
	SCL_DATASET_ATTRIB dataSetAttrib;
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,datSetName,dataSetAttrib))
		return -1;
	dataSetAttrib.fcdaNum--;
	if ( !pCache->SetDataSetAttribInLN0(iedName,ldInst,datSetName,dataSetAttrib))
		return -1;

	// �Ƴ�FCDA
	if ( !pCache->RemoveFCDAAttrib(iedName,ldInst,_T("LLN0"),datSetName,idxSeq) )
		return -1;

	return 0;
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
int SCLSmv92Analysis::IsAppIDExisted(const unsigned short appID)
{
	// �жϻ����Ƿ����
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	return pCache->IsSMVAppIDExisted(appID);
}

///
/// @brief
///     ��ȡͨ���������Ϣ
///
/// @param[in]
///     stSmvChannelInfo - ����ֵ���ݼ��е�����ͨ����Ϣ
/// @param[in]
///     eChannelType     - ͨ������
///
/// @return
///     ���ض�Ӧ��ͨ����� A/B/C��
///
CHANNEL_PHASE_SMV SCLSmv92Analysis::GetChannelPhase(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo, CHANNEL_TYPE_SMV eChannelType)
{
	// ��ͨ��Ϊʱ���δ֪���ͣ��������
	if (eChannelType == CT_SMV_UNKNOW || eChannelType == CT_SMV_TIME)
	{
		return CP_SMV_PHASE_UNKNOW;
	}

	// �ж��Ƿ�ΪA��
	int nSize = sizeof(PHASE_A_TEMPLATE)/sizeof(PHASE_A_TEMPLATE[0]);
	for (int i = 0; i < nSize; ++i)
	{
		if (stSmvChannelInfo.doDesc.find(PHASE_A_TEMPLATE[i].c_str()) != std::wstring::npos)
		{
			return CP_SMV_PHASE_A;
		}
		if (stSmvChannelInfo.lnDesc.find(PHASE_A_TEMPLATE[i].c_str()) != std::wstring::npos)
		{
			return CP_SMV_PHASE_A;
		}
		if (stSmvChannelInfo.daVal.find(PHASE_A_TEMPLATE[i].c_str()) != std::wstring::npos)
		{
			return CP_SMV_PHASE_A;
		}
	}
// 	if (stSmvChannelInfo.lnPrefix.find(L"A") != std::wstring::npos ||
// 		stSmvChannelInfo.lnPrefix.find(L"a") != std::wstring::npos)
// 	{
// 		return CP_SMV_PHASE_A;
// 	}

	// �ж��Ƿ�ΪB��
	nSize = sizeof(PHASE_B_TEMPLATE)/sizeof(PHASE_B_TEMPLATE[0]);
	for (int i = 0; i < nSize; ++i)
	{
		if (stSmvChannelInfo.doDesc.find(PHASE_B_TEMPLATE[i].c_str()) != std::wstring::npos)
		{
			return CP_SMV_PHASE_B;
		}
		if (stSmvChannelInfo.lnDesc.find(PHASE_B_TEMPLATE[i].c_str()) != std::wstring::npos)
		{
			return CP_SMV_PHASE_B;
		}
		if (stSmvChannelInfo.daVal.find(PHASE_B_TEMPLATE[i].c_str()) != std::wstring::npos)
		{
			return CP_SMV_PHASE_B;
		}
	}
// 	if (stSmvChannelInfo.lnPrefix.find(L"B") != std::wstring::npos ||
// 		stSmvChannelInfo.lnPrefix.find(L"b") != std::wstring::npos)
// 	{
// 		return CP_SMV_PHASE_B;
// 	}

	// �ж��Ƿ�ΪC��
	nSize = sizeof(PHASE_C_TEMPLATE)/sizeof(PHASE_C_TEMPLATE[0]);
	for (int i = 0; i < nSize; ++i)
	{

		if (stSmvChannelInfo.doDesc.find(PHASE_C_TEMPLATE[i].c_str()) != std::wstring::npos)
		{
			return CP_SMV_PHASE_C;
		}
		if (stSmvChannelInfo.lnDesc.find(PHASE_C_TEMPLATE[i].c_str()) != std::wstring::npos)
		{
			return CP_SMV_PHASE_C;
		}
		if (stSmvChannelInfo.daVal.find(PHASE_C_TEMPLATE[i].c_str()) != std::wstring::npos)
		{
			return CP_SMV_PHASE_C;
		}
	}
// 	if (stSmvChannelInfo.lnPrefix.find(L"C") != std::wstring::npos ||
// 		stSmvChannelInfo.lnPrefix.find(L"c") != std::wstring::npos)
// 	{
// 		return CP_SMV_PHASE_C;
// 	}

	// �ж��Ƿ�ΪN��
	nSize = sizeof(PHASE_N_TEMPLATE)/sizeof(PHASE_N_TEMPLATE[0]);
	for (int i = 0; i < nSize; ++i)
	{

		if (stSmvChannelInfo.doDesc.find(PHASE_N_TEMPLATE[i].c_str()) != std::wstring::npos)
		{
			return CP_SMV_PHASE_N;
		}
		if (stSmvChannelInfo.lnDesc.find(PHASE_N_TEMPLATE[i].c_str()) != std::wstring::npos)
		{
			return CP_SMV_PHASE_N;
		}
		if (stSmvChannelInfo.daVal.find(PHASE_N_TEMPLATE[i].c_str()) != std::wstring::npos)
		{
			return CP_SMV_PHASE_N;
		}
	}

	return CP_SMV_PHASE_X;
}

///
/// @brief
///     ��ȡͨ����������Ϣ
///
/// @param[in]
///     stSmvChannelInfo - ����ֵ���ݼ��е�����ͨ����Ϣ
///
/// @return
///     ���ض�Ӧ��ͨ��������Ϣ ʱ��/��ѹ/����...
///
CHANNEL_TYPE_SMV SCLSmv92Analysis::GetChannelType(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo )
{
	CHANNEL_TYPE_SMV enChannelType = CT_SMV_UNKNOW;
	if (stSmvChannelInfo.doName == L"DelayTRtg" ||
		stSmvChannelInfo.lnDesc.find(L"ʱ��") != std::wstring::npos ||
		stSmvChannelInfo.lnDesc.find(L"��ʱ") != std::wstring::npos ||
		stSmvChannelInfo.lnDesc.find(L"�ӳ�") != std::wstring::npos ||
		stSmvChannelInfo.doDesc.find(L"ʱ��") != std::wstring::npos ||
		stSmvChannelInfo.doDesc.find(L"��ʱ") != std::wstring::npos ||
		stSmvChannelInfo.doDesc.find(L"�ӳ�") != std::wstring::npos ||
		stSmvChannelInfo.daVal.find(L"ʱ��") != std::wstring::npos ||
		stSmvChannelInfo.daVal.find(L"��ʱ") != std::wstring::npos ||
		stSmvChannelInfo.daVal.find(L"�ӳ�") != std::wstring::npos)
	{
		enChannelType = CT_SMV_TIME;
	}
	else if (stSmvChannelInfo.lnClass == L"TVTR")
	{
		enChannelType = CT_SMV_VOLTAGE;
	}
	else if (stSmvChannelInfo.lnClass == L"TCTR")
	{
		enChannelType = CT_SMV_CURRENT;
	}
	else if (stSmvChannelInfo.lnClass == L"GGIO")
	{
		if (stSmvChannelInfo.lnDesc.find(L"����") != std::wstring::npos ||
			stSmvChannelInfo.lnDesc.find(L"Current") != std::wstring::npos ||
			stSmvChannelInfo.doDesc.find(L"����") != std::wstring::npos ||
			stSmvChannelInfo.doDesc.find(L"Current") != std::wstring::npos ||
			stSmvChannelInfo.daVal.find(L"����") != std::wstring::npos ||
			stSmvChannelInfo.daVal.find(L"Current") != std::wstring::npos)
		{
			enChannelType = CT_SMV_CURRENT;
		}
		else if (stSmvChannelInfo.lnDesc.find(L"��ѹ") != std::wstring::npos ||
			     stSmvChannelInfo.lnDesc.find(L"Voltage") != std::wstring::npos ||
			     stSmvChannelInfo.doDesc.find(L"��ѹ") != std::wstring::npos ||
				 stSmvChannelInfo.doDesc.find(L"Voltage") != std::wstring::npos ||
				 stSmvChannelInfo.daVal.find(L"��ѹ") != std::wstring::npos ||
				 stSmvChannelInfo.daVal.find(L"Voltage") != std::wstring::npos)
		{
			enChannelType = CT_SMV_VOLTAGE;
		}
	}

	return enChannelType;
}

///
/// @brief
///     �������õ���SCL����ת����IEDĳһSMV�������Ϣ
///
/// @param[in]
///     smvAttrib     - SMVͨ�Ų�����Ϣ
/// @param[in]
///     iedAttrib     - IED������Ϣ
/// @param[in]
///     apAttrib      - AccessPoint������Ϣ
/// @param[in]
///     ldAttrib      - LDevice������Ϣ
/// @param[in]
///     smvCtrlAttrib - SampledValueControl������Ϣ
/// @param[in]
///     iedSMVInfo    - ����IED��ĳһSMV���������Ϣ�Ľṹ��
///
/// @return
///     
///
void SCLSmv92Analysis::SCL2IEDSMVInfo(const SCL_SMV_ATTRIB &smvAttrib, const SCL_IED_ATTRIB &iedAttrib,
									  const SCL_ACCESS_POINT_ATTRIB &apAttrib, const SCL_LDEVICE_ATTRIB &ldAttrib,
									  const SCL_DATASET_ATTRIB &dataSetAttrib, const SCL_SMVCTRL_ATTRIB &smvCtrlAttrib,
									  SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfo)
{
	iedSMVInfo->iedName = iedAttrib.iedName;
	iedSMVInfo->iedDesc = iedAttrib.iedDesc;

	iedSMVInfo->iedAPName = apAttrib.apName;
	iedSMVInfo->iedAPDesc = apAttrib.apDesc;

	iedSMVInfo->iedLdInst = ldAttrib.ldInst;
	iedSMVInfo->iedLdDesc = ldAttrib.ldDesc;

	iedSMVInfo->dataSetName = dataSetAttrib.dataSetName;
	iedSMVInfo->dataSetDesc = dataSetAttrib.dataSetDesc;
	iedSMVInfo->channelNum  = dataSetAttrib.fcdaNum;

	ConvMacWStr2Char(smvAttrib.macAddr,iedSMVInfo->smvMAC);
	unsigned int tmp;
	ConvWStr2Hex(smvAttrib.appID,&tmp);
	iedSMVInfo->smvAppID  = (unsigned short)tmp;
	ConvWStr2Hex(smvAttrib.vlanID,&tmp);
	iedSMVInfo->smvVlanID = (unsigned short)tmp;
	ConvWStr2Dec(smvAttrib.vlanPriority,&tmp);
	iedSMVInfo->smvVlanPri = (unsigned char)tmp;

	iedSMVInfo->smvDesc = smvCtrlAttrib.cbDesc;
	iedSMVInfo->smvCbName = smvCtrlAttrib.cbName;
	ConvWStr2Dec(smvCtrlAttrib.confRev,&iedSMVInfo->smvCbConfRev);
	iedSMVInfo->smvCbMulticast = (SMV92_BOOLEAN)smvCtrlAttrib.multicast;
	iedSMVInfo->smvCbDataSet = smvCtrlAttrib.dataSet;
	ConvWStr2Dec(smvCtrlAttrib.nofASDU,&iedSMVInfo->smvCbNofASDU);
	ConvWStr2Dec(smvCtrlAttrib.smpRate,&iedSMVInfo->smvCbSmpRate);
	iedSMVInfo->smvCbSmvID = smvCtrlAttrib.smvID;
	iedSMVInfo->smvCbOptRefreshTime = (SMV92_BOOLEAN)smvCtrlAttrib.smvOptReftTm;
	iedSMVInfo->smvCbOptSmpRate     = (SMV92_BOOLEAN)smvCtrlAttrib.smvOptSmpRate;
	iedSMVInfo->smvCbOptDataRef     = (SMV92_BOOLEAN)smvCtrlAttrib.smvOptDataRef;
	iedSMVInfo->smvCbOptSmpSync     = (SMV92_BOOLEAN)smvCtrlAttrib.smvOptSmpSync;
	iedSMVInfo->smvCbOptSecurity    = (SMV92_BOOLEAN)smvCtrlAttrib.smvOptSecurity;
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
///     doiAttrib     - ��ͨ����FCDA����������Ϣ
/// @param[in]
///     dataSetInfo   - ����ͨ����FCDA������Ϣ�Ľṹ��
///
/// @return
///     
///
void SCLSmv92Analysis::SCL2DataSetInfo(const SCL_DATASET_ATTRIB &dataSetAttrib,
									   const SCL_FCDA_ATTRIB &fcdaAttrib,
									   const SCL_STR &lnDesc,
									   const SCL_DOI_ATTRIB &doiAttrib,
									   SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo)
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
	dataSetInfo->daFc        = fcdaAttrib.fc;
	dataSetInfo->daName      = fcdaAttrib.daName;
	dataSetInfo->dasAddr     = doiAttrib.dUSAddr;
	dataSetInfo->daVal       = doiAttrib.dUVal;
}

///
/// @brief
///     �������õ���SCL����ת����SMV INPUT��Ϣ���ⲿ�ź�������Ϣ��
///
/// @param[in]
///     iedName       - Inputs���ڵ�IED������
/// @param[in]
///     extRefAttrib  - �����õ���Inputs ExtRef��Ϣ
/// @param[in]
///     inputsInfo    - ����SMV INPUT��Ϣ�Ľṹ��
///
/// @return
///     true - �����ɹ��� false - ����ʧ��
///
bool SCLSmv92Analysis::SCL2InputsInfo(const SCL_STR &iedName, const SCL_EXTREF_ATTRIB &extRefAttrib, SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO *inputsInfo)
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
			lnDesc = _T("");
		else
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
			lnDesc = _T("");
		else
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
			lnDesc = _T("");
		else
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
			lnDesc = _T("");
		else
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
///     ��SMV92_BOOLEAN��������ת��Ϊ��׼bool������
///
bool SCLSmv92Analysis::BOOLEAN_SMV92toSTD(SMV92_BOOLEAN b)
{
	if ( b == SMV92_TRUE )
		return true;
	else
		return false;
}

///
/// @brief
///     ͨ��ͨ����FCDA���������ж��Ƿ�ΪĿ��ͨ��
///
/// @param[in]
///     dataSetInfo - ��Ҫ�жϵ�ͨ����FCDA������Ϣ
/// @param[in]
///     fcdaAttrib  - ��ͨ����FCDA����������Ϣ
///
/// @return
///     
///
bool SCLSmv92Analysis::IsGoalFCDA(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo, const SCL_FCDA_ATTRIB &fcdaAttrib)
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
void SCLSmv92Analysis::ConvMacWStr2Char(const std::wstring &mac,unsigned char *macRetn)
{
	unsigned int k;
	int          j = 0;
	wchar_t      c[3] = {0};

	for ( int i=0 ; i<17 ;i = i+3)
	{
		c[0] = mac.c_str()[i];
		c[1] = mac.c_str()[i+1];
		swscanf_s(c,L"%x",&k);
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
void SCLSmv92Analysis::ConvMacChar2WStr(const unsigned char *macRetn,std::wstring &mac)
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
void SCLSmv92Analysis::ConvWStr2Hex(const std::wstring &str, unsigned int *intData)
{
	if ( str.length() == 0 )
	{
		*intData = 0;
		return;
	}
	swscanf_s(str.c_str(),L"%x",intData);
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
void SCLSmv92Analysis::ConvHex2WStr(const unsigned int *intData, std::wstring &str)
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
void SCLSmv92Analysis::ConvWStr2Dec(const std::wstring &str, unsigned int *intData)
{
	if ( str.length() == 0 )
	{
		*intData = 0;
		return;
	}
	swscanf_s(str.c_str(),L"%d",intData);
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
void SCLSmv92Analysis::ConvDec2WStr(const unsigned int *intData, std::wstring &str)
{
	wchar_t c[256] = {0};
	swprintf_s(c,256,L"%d",*intData);
	str = c;
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
bool SCLSmv92Analysis::IsMacAddrSame(const unsigned char *mac1,const unsigned char *mac2)
{
	for ( int i=0; i<SCDSMV92ANALYSIS_MAC_SIZE; i++)
	{
		if ( mac1[i] != mac2[i] )
			return false;
	}
	return true;
}

///
/// @brief
///     ����Զ���ͨ������������ģ��
///
/// @param[in]
///     lnType  - �Զ���ͨ�����������߼��ڵ�����ͣ�������ID
/// @param[in]
///     lnClass - �Զ���ͨ�����������߼��ڵ����
/// @param[in]
///     doName  - �Զ���ͨ�����ݵ�����
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int  SCLSmv92Analysis::AddDefChannelDataTemplates(const SCL_STR &lnType, const SCL_STR &lnClass, const SCL_STR &doName)
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_LNTYPE_ATTRIB lnTypeAttrib;
	if ( !pCache->GetLNType(lnType,lnTypeAttrib) )
	{
		lnTypeAttrib.id      = lnType;
		lnTypeAttrib.lnClass = lnClass;
		if ( !pCache->AddLNType(lnTypeAttrib) )
			return -1;

		SCL_DOTYPE_ATTRIB doTypeAttrib;
		if ( !pCache->GetDOType(_T("CN_SAV"),doTypeAttrib) )
		{
			doTypeAttrib.cdc = _T("SAV");
			doTypeAttrib.id  = _T("CN_SAV");
			if ( !pCache->AddDOType(doTypeAttrib) )
				return -1;

			SCL_DAINDOTYPE_ATTRIB daAttrib;
			daAttrib.name  = _T("instMag");
			daAttrib.fc    = _T("MX");
			daAttrib.bType = _T("Struct");
			daAttrib.type  = _T("CN_AnalogueValue_I");
			if ( !pCache->AddDAIntoDOType(doTypeAttrib.id,daAttrib) )
				return -1;
			daAttrib.name  = _T("q");
			daAttrib.fc    = _T("MX");
			daAttrib.bType = _T("Quality");
			daAttrib.type  = _T("");
			if ( !pCache->AddDAIntoDOType(doTypeAttrib.id,daAttrib) )
				return -1;
		}

		SCL_DATYPE_ATTRIB daTypeAttrib;
		if ( !pCache->GetDAType(_T("CN_AnalogueValue_I"),daTypeAttrib) )
		{
			daTypeAttrib.id = _T("CN_AnalogueValue_I");
			if ( !pCache->AddDAType(daTypeAttrib) )
				return -1;

			SCL_BDAINDATYPE_ATTRIB bdaAttrib;
			bdaAttrib.name = _T("i");
			bdaAttrib.type = _T("INT32");
			if ( !pCache->AddBDAIntoDAType(daTypeAttrib.id,bdaAttrib) )
				return -1;
		}
	}

	SCL_DOINLNTYPE_ATTRIB doAttrib;
	doAttrib.name = doName;
	doAttrib.type = _T("CN_SAV");
	if ( !pCache->AddDOIntoLNType(lnType,doAttrib) )
		return -1;

	return 0;
}
