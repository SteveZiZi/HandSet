/// @file
///     sclsmv92analysis.cpp
///
/// @brief
///     SCL文件中关于IEC61850-9-2的配置信息的解析
///
/// @note
///
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
///    |2013.02.18     |1.0.3.218  |添加SMV92的虚端子连线的信息（SMV92 INPUT）解析            |
///    |2013.08.07     |1.0.5.807  |获取SMV通道信息时，添加对通道的DOI信息不存在时的处理      |
///    |2013.08.08     |1.0.5.808  |修改某逻辑节点是否为LLN0的判断条件                        |
///    |2013.08.08     |1.0.5.808  |修改SMV通道的相别信息判断模板                             |
///    |2013.08.09     |1.0.5.809  |修改SMV通道的相别信息判断：添加对全角字母的判断、对dUVal（daVal）值的判断|
///    |2013.08.09     |1.0.5.809  |修改SMV通道的类型信息判断：添加对逻辑节点类型为GGIO时各描述信息的判断、修改对时间类型通道的判断|
///
#include "stdafx.h"
#include "sclsmv92analysis.h"

///
/// @brief
///     相别信息判断模板
///
static std::wstring PHASE_A_TEMPLATE[]={L"A相", L"a相", L"Ａ相"};
static std::wstring PHASE_B_TEMPLATE[]={L"B相", L"b相", L"Ｂ相"};
static std::wstring PHASE_C_TEMPLATE[]={L"C相", L"c相", L"Ｃ相"};
static std::wstring PHASE_N_TEMPLATE[]={L"N相", L"n相", L"Ｎ相", L"中性点", L"零序"};
static std::wstring PHASE_X_TEMPLATE[]={L"X相", L"x相", L"Ｘ相", L"同期", L"母线", L"抽取"};

SCLSmv92Analysis::SCLSmv92Analysis(ISCLCacheManager *pSclCacheMgr)
{
	m_pSclCacheMgr = (SCLCacheManager *)pSclCacheMgr;
}

SCLSmv92Analysis::~SCLSmv92Analysis()
{

}

///
/// @brief
///     获取配置文件中所有的SMV控制块的数量（不包含未配置通信参数的控制块）
///
/// @return
///     SMV控制块的数量 - 操作成功； -1 - 操作失败
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
///     获取配置文件中所有的SMV控制块信息（不包含未配置通信参数的控制块）
///
/// @param[in]
///     iedSMVInfoArr - 保存SMV控制块信息的数组
/// @param[in]
///     arrSize       - 数据的最大容量
///
/// @return
///     数组中保存信息的数量：操作成功；  -1：操作失败
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

	// 获取控制块的通信参数信息
	if ( !pCache->GetAllSMVCommn(conntAPAttrib,smvAttrib,arrSize) )
		goto errorHandle;

	// 获取控制块的与装置本身相关的信息，如所在IED的信息、系统参数等
	// 注：SCL文件存在定义了通信参数，但没有定义IED或控制块相关信息的情况
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
///     获取指定MAC和APPID的SMV控制块信息
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

	// 获取控制块的通信参数信息
	if ( !pCache->GetSMVCommn(smvAppID,conntAPAttrib,smvAttrib) )
		return -1;

	// Mac地址判断
	ConvMacWStr2Char(smvAttrib.macAddr,macAddr);
	if ( !IsMacAddrSame(smvMAC,macAddr) )
		return -1;

	// 获取控制块的与装置本身相关的信息，如所在IED的信息、系统参数等
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
///     获取指定MAC、APPID相关的SMV控制块所控制的DataSet的通道（FCDA）的数量
///
/// @param[in]
///     smvMAC     - SMV通信组播MAC地址
/// @param[in]
///     smvAPPID   - SMV通信对应的APPID(标准要求全站惟一)
///
/// @return
///     DataSet中FCDA的数量：操作成功；  -1：失败
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
///     获取指定MAC、APPID相关的SMV控制块所控制的DataSet的所有通道（FCDA）信息
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
///     获取指定MAC、APPID相关的SMV控制块所控制的DataSet的某一顺序号的通道（FCDA）信息
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
///     获取指定MAC、APPID相关的SMV控制块所控制的DataSet的某一通道（FCDA）的顺序号
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
///     获取某一IED的所绑定的外部信号（ExtRef，即SMV输入信号）的数量
///     每个装置的LLN0部分含有该装置的GOOSE和采样值输入的连线信息
///
/// @param[in]
///     iedName      - IED的名称
///
/// @return
///     ExtRef的数量：成功；  -1：失败
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
///     获取控制某一外部信号输入的SMV控制块信息
///
/// @param[in]
///     inputsInfo - 外部信号的连线信息
/// @param[in]
///     iedSMVInfo - 控制该外部信号输入的SMV控制块信息
///
/// @return
///     0 - 操作成功； -1 - 操作失败
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

	// 获取IED信息
	if ( !pCache->GetIEDAttrib(inputsInfo.iedName_Out,iedAttrib) )
		return -1;

	// 获取LDevice信息
	if ( !pCache->GetLDeviceAttrib(inputsInfo.iedName_Out,inputsInfo.ldInst_Out,apAttrib,ldAttrib) )
		return -1;

	// 获取指定控制块及通信参数信息
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
///     获取全部的SMV控制块的数目（包含未配置通信参数的控制块）
///
/// @return
///     控制块的数目：操作成功；  -1：失败
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
///     获取全部的SMV控制块的信息（包含未配置通信参数的控制块）
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
int SCLSmv92Analysis::GetAllIEDSMVInfoByCtrlRef(SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfoArr, 
												const int arrSize )
{
	// 判断缓存是否存在
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	int ctrlNum = 0;
	int	iedNum = pCache->GetIEDNum();
	for ( int i = 0; i < iedNum; i++ )
	{
		if ( ctrlNum >= arrSize )
			break;

		// 获取IED信息
		SCL_IED_ATTRIB iedAttrib;
		if ( !pCache->GetIEDAttrib(i,iedAttrib) )
			continue;

		int ldNum = pCache->GetLDeviceNum(iedAttrib.iedName);
		for (int j = 0; j < ldNum; j++ )
		{
			if ( ctrlNum >= arrSize )
				break;

			// 获取LDevice信息
			SCL_ACCESS_POINT_ATTRIB apAttrib;
			SCL_LDEVICE_ATTRIB ldAttrib;
			if ( !pCache->GetLDeviceAttrib(iedAttrib.iedName,j,apAttrib,ldAttrib) )
				continue;

			int oneLdCtrlNum  = pCache->GetSMVCtrlNumInLN0(iedAttrib.iedName,ldAttrib.ldInst);
			for ( int k = 0; k < oneLdCtrlNum; k++ )
			{
				if ( ctrlNum >= arrSize )
					break;

				// 获取控制块信息
				SCL_SMVCTRL_ATTRIB ctrlAttrib;
				if ( !pCache->GetSMVCtrlAttribInLN0(iedAttrib.iedName,ldAttrib.ldInst,k,ctrlAttrib) )
					continue;

				// 获取与控制块关联的数据集的信息
				SCL_DATASET_ATTRIB dataSetAttrib;
				if ( !pCache->GetDataSetAttribInLN0(iedAttrib.iedName,ldAttrib.ldInst,ctrlAttrib.dataSet,dataSetAttrib) )
					continue;

				// 获取与控制块关联的通信参数信息
				SCL_CONNECTEDAP_ATTRIB conntAPAttrib;
				SCL_SMV_ATTRIB commnAttrib;
				if ( !pCache->GetSMVCommnWithIdx(iedAttrib.iedName,ldAttrib.ldInst,ctrlAttrib.cbName,
					conntAPAttrib,commnAttrib) )
				{
					// 未配置通信参数，则设置默认值
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
///     设置一个SMV控制块
///
/// @param[in]
///     iedSMVInfo - SMV控制块信息
///
/// @return
///     0：操作成功；  -1：失败
///
int SCLSmv92Analysis::SetOneIEDSMVInfoByCtrlRef(const SCDSMV92ANALYSIS_IEDSMV_INFO &iedSMVInfo)
{
	// 判断缓存是否存在
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// 设置控制块信息
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

	// 设置控制块对应的通信参数信息
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

	// 设置控制块对应的数据集信息
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
int SCLSmv92Analysis::SetOneIEDSMVDataSetInfo_Def( const std::wstring &iedName, const std::wstring &ldInst,
												   SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
												   CHANNEL_TYPE_SMV eChannelType )
{
	// 判断缓存是否存在
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// 更新DataSet中的通道索引信息（即FCDA），不存在则添加
	SCL_DATASET_ATTRIB dataSetAttrib;
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,dataSetInfo.dataSetName,dataSetAttrib) )
		return -1;
	
	// 自定义通道lnName
	SCL_STR lnPrefix = L"DEF_" + iedName + ldInst + L"_" + dataSetInfo.dataSetName;
	std::wstring str;
	ConvDec2WStr((unsigned int *)&dataSetInfo.idxSeq,str);
	SCL_STR lnInst = str;
	SCL_STR lnClass,doName;
	// 根据通道类型，设置默认lnClass和doName
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

	// 更新DataSet中的通道索引信息（即FCDA）
	if ( dataSetInfo.idxSeq >= dataSetAttrib.fcdaNum )
	{
		return -1;
	}
	else
	{
		if ( !pCache->SetFCDAAttrib(iedName,ldInst,_T("LLN0"),dataSetInfo.dataSetName,dataSetInfo.idxSeq,fcdaAttrib) )
			return -1;
	}

	// 更新该通道所在的逻辑节点的信息
	SCL_STR lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
	SCL_STR lnType = _T("XJ_") + dataSetInfo.lnClass + _T("_DEF");
	if ( fcdaAttrib.lnClass == _T("LLN0") )
	{
		SCL_LN0_ATTRIB ln0Attrib;
		if ( !pCache->GetLN0Attrib(iedName,ldInst,ln0Attrib) )
		{
			// 逻辑节点不存在，则新建，并使用自定义的类型模板
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
			// 逻辑节点不存在，则新建，并使用自定义的类型模板
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

	// 更新该通道对应的DOI的信息，不存在则新建
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

	// 添加自定义类型模板
	if ( AddDefChannelDataTemplates(lnType,dataSetInfo.lnClass,dataSetInfo.doName) == -1 )
		return -1;

	return 0;
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
int SCLSmv92Analysis::AddOneIEDSMVDataSetInfo_Def( const std::wstring &iedName, const std::wstring &ldInst,
												  SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
												  CHANNEL_TYPE_SMV eChannelType )
{
	// 判断缓存是否存在
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// 更新DataSet中的通道索引信息（即FCDA），不存在则添加
	SCL_DATASET_ATTRIB dataSetAttrib;
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,dataSetInfo.dataSetName,dataSetAttrib) )
	{
		dataSetAttrib.dataSetName = dataSetInfo.dataSetName;
		dataSetAttrib.fcdaNum = 0;
		if ( !pCache->AddDataSetIntoLN0(iedName,ldInst,dataSetAttrib) )
			return -1;
	}

	// 自定义通道lnName
	SCL_STR lnPrefix = L"DEF_" + iedName + ldInst + L"_" + dataSetInfo.dataSetName;
	std::wstring str;
	ConvDec2WStr((unsigned int *)&dataSetInfo.idxSeq,str);
	SCL_STR lnInst = str;
	SCL_STR lnClass,doName;
	// 根据通道类型，设置默认lnClass和doName
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

	// 添加FCDA
	if ( !pCache->AddFCDAIntoDatSet(iedName,ldInst,_T("LLN0"),dataSetInfo.dataSetName,fcdaAttrib) )
		return -1;

	// 更新数据集信息
	dataSetAttrib.fcdaNum++;
	if ( !pCache->SetDataSetAttribInLN0(iedName,ldInst,dataSetAttrib.dataSetName,dataSetAttrib) )
		return -1;

	// 更新该通道所在的逻辑节点的信息
	SCL_STR lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
	SCL_STR lnType = _T("XJ_") + dataSetInfo.lnClass + _T("_DEF");
	if ( fcdaAttrib.lnClass == _T("LLN0") )
	{
		SCL_LN0_ATTRIB ln0Attrib;
		if ( !pCache->GetLN0Attrib(iedName,ldInst,ln0Attrib) )
		{
			// 逻辑节点不存在，则新建，并使用自定义的类型模板
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
			// 逻辑节点不存在，则新建，并使用自定义的类型模板
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

	// 更新该通道对应的DOI的信息，不存在则新建
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

	// 添加自定义类型模板
	if ( AddDefChannelDataTemplates(lnType,dataSetInfo.lnClass,dataSetInfo.doName) == -1 )
		return -1;

	return 0;
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
int SCLSmv92Analysis::CopyIEDSMVDataSetInfoToSetEnd(const std::wstring &iedName, const std::wstring &ldInst,
													const std::wstring &dataSetName, int idxSeq, int iCopyCount)
{
	// 判断缓存是否存在
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// 更新DataSet中的通道索引信息（即FCDA），不存在则添加
	SCL_DATASET_ATTRIB dataSetAttrib;
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,dataSetName,dataSetAttrib) )
		return -1;

	if ( idxSeq >= dataSetAttrib.fcdaNum )
		return -1;
	SCL_FCDA_ATTRIB fcdaAttrib;
	if ( !pCache->GetFCDAAttrib(iedName,ldInst,_T("LLN0"),dataSetName,idxSeq,fcdaAttrib) )
		return -1;
	// 添加FCDA
	for ( int i = 0; i < iCopyCount; i++ )
	{
		fcdaAttrib.idxSeq = i + dataSetAttrib.fcdaNum;
		if ( !pCache->AddFCDAIntoDatSet(iedName,ldInst,_T("LLN0"),dataSetName,fcdaAttrib) )
			return -1;
	}
	// 更新通道数
	dataSetAttrib.fcdaNum += iCopyCount;
	if ( !pCache->SetDataSetAttribInLN0(iedName,ldInst,dataSetName,dataSetAttrib) )
		return -1;

	return 0;
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
int SCLSmv92Analysis::RemoveOneIEDSMVDataSetInfo(const std::wstring &iedName, const std::wstring &ldInst,
												 const std::wstring &datSetName, const int idxSeq)
{
	// 判断缓存是否存在
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// 设置控制块对应的数据集的通道数目信息
	SCL_DATASET_ATTRIB dataSetAttrib;
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,datSetName,dataSetAttrib))
		return -1;
	dataSetAttrib.fcdaNum--;
	if ( !pCache->SetDataSetAttribInLN0(iedName,ldInst,datSetName,dataSetAttrib))
		return -1;

	// 移除FCDA
	if ( !pCache->RemoveFCDAAttrib(iedName,ldInst,_T("LLN0"),datSetName,idxSeq) )
		return -1;

	return 0;
}

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
int SCLSmv92Analysis::IsAppIDExisted(const unsigned short appID)
{
	// 判断缓存是否存在
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	return pCache->IsSMVAppIDExisted(appID);
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
CHANNEL_PHASE_SMV SCLSmv92Analysis::GetChannelPhase(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo, CHANNEL_TYPE_SMV eChannelType)
{
	// 该通道为时间或未知类型，则无相别
	if (eChannelType == CT_SMV_UNKNOW || eChannelType == CT_SMV_TIME)
	{
		return CP_SMV_PHASE_UNKNOW;
	}

	// 判断是否为A相
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

	// 判断是否为B相
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

	// 判断是否为C相
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

	// 判断是否为N相
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
///     获取通道的类型信息
///
/// @param[in]
///     stSmvChannelInfo - 采样值数据集中的数据通道信息
///
/// @return
///     返回对应的通道类型信息 时间/电压/电流...
///
CHANNEL_TYPE_SMV SCLSmv92Analysis::GetChannelType(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo )
{
	CHANNEL_TYPE_SMV enChannelType = CT_SMV_UNKNOW;
	if (stSmvChannelInfo.doName == L"DelayTRtg" ||
		stSmvChannelInfo.lnDesc.find(L"时间") != std::wstring::npos ||
		stSmvChannelInfo.lnDesc.find(L"延时") != std::wstring::npos ||
		stSmvChannelInfo.lnDesc.find(L"延迟") != std::wstring::npos ||
		stSmvChannelInfo.doDesc.find(L"时间") != std::wstring::npos ||
		stSmvChannelInfo.doDesc.find(L"延时") != std::wstring::npos ||
		stSmvChannelInfo.doDesc.find(L"延迟") != std::wstring::npos ||
		stSmvChannelInfo.daVal.find(L"时间") != std::wstring::npos ||
		stSmvChannelInfo.daVal.find(L"延时") != std::wstring::npos ||
		stSmvChannelInfo.daVal.find(L"延迟") != std::wstring::npos)
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
		if (stSmvChannelInfo.lnDesc.find(L"电流") != std::wstring::npos ||
			stSmvChannelInfo.lnDesc.find(L"Current") != std::wstring::npos ||
			stSmvChannelInfo.doDesc.find(L"电流") != std::wstring::npos ||
			stSmvChannelInfo.doDesc.find(L"Current") != std::wstring::npos ||
			stSmvChannelInfo.daVal.find(L"电流") != std::wstring::npos ||
			stSmvChannelInfo.daVal.find(L"Current") != std::wstring::npos)
		{
			enChannelType = CT_SMV_CURRENT;
		}
		else if (stSmvChannelInfo.lnDesc.find(L"电压") != std::wstring::npos ||
			     stSmvChannelInfo.lnDesc.find(L"Voltage") != std::wstring::npos ||
			     stSmvChannelInfo.doDesc.find(L"电压") != std::wstring::npos ||
				 stSmvChannelInfo.doDesc.find(L"Voltage") != std::wstring::npos ||
				 stSmvChannelInfo.daVal.find(L"电压") != std::wstring::npos ||
				 stSmvChannelInfo.daVal.find(L"Voltage") != std::wstring::npos)
		{
			enChannelType = CT_SMV_VOLTAGE;
		}
	}

	return enChannelType;
}

///
/// @brief
///     将解析得到的SCL数据转换成IED某一SMV传输的信息
///
/// @param[in]
///     smvAttrib     - SMV通信参数信息
/// @param[in]
///     iedAttrib     - IED属性信息
/// @param[in]
///     apAttrib      - AccessPoint属性信息
/// @param[in]
///     ldAttrib      - LDevice属性信息
/// @param[in]
///     smvCtrlAttrib - SampledValueControl属性信息
/// @param[in]
///     iedSMVInfo    - 保存IED中某一SMV传输相关信息的结构体
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
///     将解析得到的SCL数据转换成某一通道（FCDA）的信息
///
/// @param[in]
///     dataSetAttrib - 该通道（FCDA）所在DataSet的属性信息
/// @param[in]
///     fcdaAttrib    - 该通道（FCDA）的引用信息
/// @param[in]
///     doiAttrib     - 该通道（FCDA）的数据信息
/// @param[in]
///     dataSetInfo   - 保存通道（FCDA）的信息的结构体
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
///     将解析得到的SCL数据转换成SMV INPUT信息（外部信号连线信息）
///
/// @param[in]
///     iedName       - Inputs所在的IED的名称
/// @param[in]
///     extRefAttrib  - 解析得到的Inputs ExtRef信息
/// @param[in]
///     inputsInfo    - 保存SMV INPUT信息的结构体
///
/// @return
///     true - 操作成功； false - 操作失败
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

	// 转换外部信号信息
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

	// 分解外部信号所绑定的内部地址的信息
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

	// 获取内部地址的描述信息
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
///     将SMV92_BOOLEAN类型数据转换为标准bool型数据
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
///     通过通道（FCDA）的引用判断是否为目标通道
///
/// @param[in]
///     dataSetInfo - 所要判断的通道（FCDA）的信息
/// @param[in]
///     fcdaAttrib  - 该通道（FCDA）的引用信息
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
///     将MACAddr由wstring转换为char数组
///
/// @param[in]
///     mac      - 要转换的macAddr
/// @param[in]
///     macRetn  - 保存转换结果的char数组的指针
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
///     将MACAddr由char数组转换为wstring
///
/// @param[in]
///     macRetn  - 要转换的macAddr的char数组的指针
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
///     将十六进制wstring转换为整数
///
/// @param[in]
///     str      - 要转换的wstring
/// @param[in]
///     intData  - 指向转换结果的指针
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
///     将整数转换为十六进制wstring
///
/// @param[in]
///     intData  - 指向整数的指针
/// @param[in]
///     str      - 十六进制wstring
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
///     将十进制wstring转换为整数
///
/// @param[in]
///     str      - 要转换的wstring
/// @param[in]
///     intData  - 指向转换结果的指针
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
///     将整数转换为十进制wstring
///
/// @param[in]
///     intData  - 指向要转换的整数的指针
/// @param[in]
///     str      - 十进制wstring
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
///     判断Mac地址是否相同
///
/// @param[in]
///     mac1 - Mac地址1
/// @param[in]
///     mac2 - Mac地址2
///
/// @return
///     true：相同；false：不同
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
///     添加自定义通道的数据类型模板
///
/// @param[in]
///     lnType  - 自定义通道数据所属逻辑节点的类型，即类型ID
/// @param[in]
///     lnClass - 自定义通道数据所属逻辑节点的类
/// @param[in]
///     doName  - 自定义通道数据的名称
///
/// @return
///     0：操作成功；  -1：失败
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
