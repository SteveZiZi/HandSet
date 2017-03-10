/// @file
///     sclgooseanalysis.cpp
///
/// @brief
///     SCL文件中关于GOOSE的配置信息的解析
///
/// @note
///
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.2.4
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.02.18     |1.0.3.218  |添加GOOSE的虚端子连线的信息（GOOSE INPUT）解析            |
///    |2013.08.07     |1.0.5.807  |获取GOOSE通道信息时，添加对通道的DOI信息不存在时的处理    |
///    |2013.08.08     |1.0.5.808  |修改某逻辑节点是否为LLN0的判断条件                        |
///    |2013.08.13     |1.0.5.813  |修正以AppID和Mac为索引获取全部控制块信息且部分AppID对应的控制块没有定义时信息获取失败的Bug|
///    |2013.08.15     |1.0.5.815  |修改Goose通道和子通道信息的操作（获取、设置、移除）实现，使其支持通道为DA模式且为类型为结构体的情况|
///
#include "stdafx.h"
#include "sclgooseanalysis.h"

///
/// @brief
///    GOOSE通道的自定义数据类型模板信息
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
///     获取配置文件中所有已配置通信参数的GOOSE控制块的数量
///
/// @return
///     GOOSE控制块的数量 - 操作成功； -1 - 操作失败
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
///     获取配置文件中所有已配置通信参数的GOOSE控制块信息
///
/// @param[in]
///     iedGOOSEInfoArr - 保存GOOSE控制块信息的数组
/// @param[in]
///     arrSize         - 数据的最大容量
///
/// @return
///     数组中保存信息的数量 - 操作成功； -1 - 操作失败
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

	// 获取控制块的通信参数信息
	if ( !pCache->GetAllGSECommn(conntAPAttrib,gseAttrib,arrSize) )
		goto errorHandle;

	// 获取控制块的与装置本身相关的信息，如所在IED的信息、系统参数等
	// 注：SCL文件存在定义了通信参数，但没有定义IED或控制块相关信息的情况
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
///     获取指定MAC和APPID的GOOSE控制块的信息
///     存在多个相同AppID的控制块时，默认选择第一个
///
/// @param[in]
///     gooseMAC     - GOOSE通信组播MAC地址
/// @param[in]
///     gooseAPPID   - GOOSE通信对应的APPID(标准要求全站惟一)
/// @param[in]
///     iedGOOSEInfo - 保存GOOSE控制块信息的结构体
///
/// @return
///     0 - 操作成功； -1 - 操作失败
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
///     获取指定MAC、APPID相关的GOOSE控制块所控制的DataSet的通道（FCDA）的数量
///     存在多个相同AppID的控制块时，默认选择第一个
///
/// @param[in]
///     gooseMAC     - GOOSE通信组播MAC地址
/// @param[in]
///     gooseAPPID   - GOOSE通信对应的APPID(标准要求全站惟一)
///
/// @return
///     DataSet中FCDA的数量 - 操作成功； -1 - 操作失败
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
///     获取指定MAC、APPID相关的GOOSE控制块所控制的DataSet的所有通道（FCDA）信息
///     存在多个相同AppID的控制块时，默认选择第一个
///
/// @param[in]
///     gooseMAC       - GOOSE通信组播MAC地址
/// @param[in]
///     gooseAPPID     - GOOSE通信对应的APPID(标准要求全站惟一)
// @param[in]
///     dataSetInfoArr - 保存GOOSE的通道信息的数组
// @param[in]
///     arrSize        - 数组的最大容量
///
/// @return
///     数组保存的通道（FCDA）信息的数量 - 操作成功； -1 - 操作失败
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
	// 获取数据集信息
	if ( !pCache->GetDataSetAttribInLN0(conntAPAttrib.iedName,gseAttrib.ldInst,gseCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;

	int i = 0;
	for ( i = 0; i < dataSetAttrib.fcdaNum ; i++ )
	{
		if ( i >= arrSize )
			break;
		// 获取FCDA属性信息
		if ( !pCache->GetFCDAAttrib(conntAPAttrib.iedName,gseAttrib.ldInst,_T("LLN0"),gseCtrlAttrib.dataSet,i,fcdaAttrib) )
			return -1;
		lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
		// 获取FCDA所指向的数据的信息
		if ( !pCache->GetDOIAttribInLNorLN0(conntAPAttrib.iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
		{
			doiAttrib.doName = fcdaAttrib.doName;
			doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
		}
		// 获取FCDA所指向的数据所在逻辑节点的信息
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
		// 获取FCDA所指向的数据属性的类型信息
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
///     获取指定MAC、APPID相关的GOOSE控制块所控制的DataSet的某一顺序号的通道（FCDA）信息
///     存在多个相同AppID的控制块时，默认选择第一个
///
/// @param[in]
///     gooseMAC       - GOOSE通信组播MAC地址
/// @param[in]
///     gooseAPPID     - GOOSE通信对应的APPID(标准要求全站惟一)
// @param[in]
///     idxSeq         - 通道的顺序号(从0开始)
// @param[in]
///     dataSetInfo    - 包含通道信息的结构体
///
/// @return
///     0 - 操作成功； -1 - 操作失败
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
	// 获取数据集信息
	if ( !pCache->GetDataSetAttribInLN0(conntAPAttrib.iedName,gseAttrib.ldInst,gseCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;
	// 获取FCDA属性信息
	if ( !pCache->GetFCDAAttrib(conntAPAttrib.iedName,gseAttrib.ldInst,_T("LLN0"),gseCtrlAttrib.dataSet,idxSeq,fcdaAttrib) )
		return -1;
	lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
	// 获取FCDA所指向的数据的信息
	if ( !pCache->GetDOIAttribInLNorLN0(conntAPAttrib.iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
	{
		doiAttrib.doName = fcdaAttrib.doName;
		doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
	}
	// 获取FCDA所指向的数据所在逻辑节点的信息
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
	// 获取FCDA所指向的数据属性的类型信息
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
///     获取指定MAC、APPID相关的GOOSE控制块所控制的DataSet的某一通道（FCDA）的顺序号
///     存在多个相同AppID的控制块时，默认选择第一个
///
/// @param[in]
///     gooseMAC       - GOOSE通信组播MAC地址
/// @param[in]
///     gooseAPPID     - GOOSE通信对应的APPID(标准要求全站惟一)
// @param[in]
///     dataSetInfo    - 包含通道信息的结构体
///
/// @return
///     通道（FCDA）的顺序号 - 操作成功； -1 - 操作失败
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
///     获取指定MAC、APPID的GOOSE控制块的控制的某一父通道的子通道的数目
///     存在多个相同AppID的控制块时，默认选择第一个
///
/// @param[in]
///     gooseMAC       - GOOSE通信组播MAC地址
/// @param[in]
///     gooseAPPID     - GOOSE通信对应的APPID(标准要求全站惟一)
// @param[in]
///     dataSetInfo    - 父通道信息
///
/// @return
///     子通道数目 - 操作成功； -1 - 操作失败
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

	// 获取通道所在IED信息
	if ( !pCache->GetGSECommn(gooseAppID,conntAPAttrib,gseAttrib) )
		return -1;
	ConvMacWStr2Char(gseAttrib.macAddr,macAddr);
	if ( !IsMacAddrSame(gooseMAC,macAddr) )
		return -1;

	// 获取FCDA所指向的数据所在逻辑节点的信息
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
///     获取指定MAC、APPID的GOOSE控制块的控制的某一父通道的全部子通道的信息
///     存在多个相同AppID的控制块时，默认选择第一个
///
/// @param[in]
///     gooseMAC          - GOOSE通信组播MAC地址
/// @param[in]
///     gooseAPPID        - GOOSE通信对应的APPID(标准要求全站惟一)
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
	// 获取FCDA所指向的数据所在逻辑节点的信息
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
///     获取某一IED的所绑定的外部信号（ExtRef，即GOOSE输入信号）的数量
///     每个装置的LLN0部分含有该装置的GOOSE和采样值输入的连线信息
///
/// @param[in]
///     iedName      - IED的名称
///
/// @return
///     ExtRef的数量：成功；  -1：失败
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
///     获取控制某一外部信号输入的GOOSE控制块的信息
///
/// @param[in]
///     inputsInfo   - 外部信号的连线信息
/// @param[in]
///     iedGOOSEInfo - 外部信号相关的GOOSE控制块的信息
///
/// @return
///     0 - 操作成功； -1 - 操作失败
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
///     获取全部的GOOSE控制块的数目（包含未配置通信参数的控制块）
///
/// @return
///     控制块的数目：操作成功；  -1：失败
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
///     获取全部的GOOSE控制块的信息（包含未配置通信参数的控制块）
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
int SCLGOOSEAnalysis::GetAllIEDGOOSEInfoByCtrlRef( SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfoArr,
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

			int oneLdCtrlNum  = pCache->GetGSECtrlNumInLN0(iedAttrib.iedName,ldAttrib.ldInst);
			for ( int k = 0; k < oneLdCtrlNum; k++ )
			{
				if ( ctrlNum >= arrSize )
					break;

				// 获取控制块信息
				SCL_GSECTRL_ATTRIB ctrlAttrib;
				if ( !pCache->GetGSECtrlAttribInLN0(iedAttrib.iedName,ldAttrib.ldInst,k,ctrlAttrib) )
					continue;

				// 获取与控制块关联的数据集的信息
				SCL_DATASET_ATTRIB dataSetAttrib;
				if ( !pCache->GetDataSetAttribInLN0(iedAttrib.iedName,ldAttrib.ldInst,ctrlAttrib.dataSet,dataSetAttrib) )
					continue;

				// 获取与控制块关联的通信参数信息
				SCL_CONNECTEDAP_ATTRIB conntAPAttrib;
				SCL_GSE_ATTRIB commnAttrib;
				if ( !pCache->GetGSECommnWithIdx(iedAttrib.iedName,ldAttrib.ldInst,ctrlAttrib.cbName,
					conntAPAttrib,commnAttrib) )
				{
					// 未配置通信参数，则设置默认值
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
///     设置一个GOOSE控制块
///
/// @param[in]
///     iedGOOSEInfo - GOOSE控制块信息
///
/// @return
///     0：操作成功；  -1：失败
///
int SCLGOOSEAnalysis::SetOneIEDGOOSEInfoByCtrlRef(const SCDGOOSEANALYSIS_IEDGOOSE_INFO &iedGOOSEInfo)
{
	// 判断缓存是否存在
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// 设置控制块信息
	SCL_GSECTRL_ATTRIB ctrlAttrib;
	ctrlAttrib.cbName = iedGOOSEInfo.cbName;
	ctrlAttrib.cbDesc = iedGOOSEInfo.cbDesc;
	ConvDec2WStr(&iedGOOSEInfo.confRev,ctrlAttrib.confRev);
	ctrlAttrib.gooseID = iedGOOSEInfo.gooseID;
	ctrlAttrib.dataSet = iedGOOSEInfo.dataSet;
	ctrlAttrib.type    = iedGOOSEInfo.type;
	if ( !pCache->SetGSECtrlAttribInLN0(iedGOOSEInfo.iedName,iedGOOSEInfo.iedLdInst,iedGOOSEInfo.cbName,ctrlAttrib) )
		return -1;

	// 设置控制块对应的通信参数信息
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

	// 设置控制块对应的数据集的信息
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
	// 获取数据集信息
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,gseCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;

	int i = 0;
	for ( i = 0; i < dataSetAttrib.fcdaNum ; i++ )
	{
		if ( i >= arrSize )
			break;
		// 获取FCDA属性信息
		if ( !pCache->GetFCDAAttrib(iedName,ldInst,_T("LLN0"),gseCtrlAttrib.dataSet,i,fcdaAttrib) )
			return -1;
		lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
		// 获取FCDA所指向的数据的信息
		if ( !pCache->GetDOIAttribInLNorLN0(iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
		{
			doiAttrib.doName = fcdaAttrib.doName;
			doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
		}
		// 获取FCDA所指向的数据所在逻辑节点的信息
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
		// 获取FCDA所指向的数据属性的类型信息
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
	// 获取数据集信息
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,gseCtrlAttrib.dataSet,dataSetAttrib) )
		return -1;
	// 获取FCDA属性信息
	if ( !pCache->GetFCDAAttrib(iedName,ldInst,_T("LLN0"),gseCtrlAttrib.dataSet,idxSeq,fcdaAttrib) )
		return -1;
	lnName = fcdaAttrib.lnPrefix + fcdaAttrib.lnClass + fcdaAttrib.lnInst;
	// 获取FCDA所指向的数据的信息
	if ( !pCache->GetDOIAttribInLNorLN0(iedName,fcdaAttrib.ldInst,lnName,fcdaAttrib.doName,doiAttrib) )
	{
		doiAttrib.doName = fcdaAttrib.doName;
		doiAttrib.doDesc = doiAttrib.dUSAddr = doiAttrib.dUVal = _T("");
	}
	// 获取FCDA所指向的数据所在逻辑节点的信息
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
	// 获取FCDA所指向的数据属性的类型信息
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
int SCLGOOSEAnalysis::SetOneIEDGOOSEDataSetInfo_Def( const std::wstring &iedName, const std::wstring &ldInst,
												 SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
												 CHANNEL_TYPE_GOOSE eChannelType )
{
	// 判断缓存是否存在
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	SCL_DATASET_ATTRIB dataSetAttrib;
	if ( !pCache->GetDataSetAttribInLN0(iedName,ldInst,dataSetInfo.dataSetName,dataSetAttrib) )
		return -1;

	// 自定义通道lnName、doName、daName
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

	// 设置FCDA信息
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

	// 更新LNode信息，不存在则添加;获取LNode类型信息
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

	// 更新DOI信息，不存在则添加
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

	// 更新通道数据到LNType中，数据类型模板为自定义模板
	if ( AddDOIntoDefLNodeType(pCache,dataSetInfo,lnType,eChannelType) == -1 )
		return -1;

	return 0;
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
int SCLGOOSEAnalysis::AddOneIEDGOOSEDataSetInfo_Def( const std::wstring &iedName, const std::wstring &ldInst,
													SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
													CHANNEL_TYPE_GOOSE eChannelType )
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

	// 自定义通道lnName、doName、daName
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

	// 添加FCDA
	if ( !pCache->AddFCDAIntoDatSet(iedName,ldInst,_T("LLN0"),dataSetInfo.dataSetName,fcdaAttrib) )
		return -1;

	// 更新数据集信息
	dataSetAttrib.fcdaNum++;
	if ( !pCache->SetDataSetAttribInLN0(iedName,ldInst,dataSetAttrib.dataSetName,dataSetAttrib) )
		return -1;

	// 更新LNode信息，不存在则添加;获取LNode类型信息
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

	// 更新DOI信息，不存在则添加
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

	// 更新通道数据到LNType中，数据类型模板为自定义模板
	if ( AddDOIntoDefLNodeType(pCache,dataSetInfo,lnType,eChannelType) == -1 )
		return -1;

	return 0;
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
int SCLGOOSEAnalysis::CopyIEDGOOSEDataSetInfoToSetEnd(const std::wstring &iedName, const std::wstring &ldInst,
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
int SCLGOOSEAnalysis::RemoveOneIEDGOOSEDataSetInfo(const std::wstring &iedName, const std::wstring &ldInst,
												   const std::wstring &datSetName, const int idxSeq)
{
	// 判断缓存是否存在
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	if ( !pCache->RemoveFCDAAttrib(iedName,ldInst,_T("LLN0"),datSetName,idxSeq) )
		return -1;

	// 设置控制块对应的数据集的通道数目信息
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

	// 获取FCDA所指向的数据所在逻辑节点的信息
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

	// 获取FCDA所指向的数据所在逻辑节点的信息
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

	// 获取FCDA所指向的数据所在逻辑节点的信息
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
int SCLGOOSEAnalysis::SetOneIEDGOOSESubChannelInfo_Def(const std::wstring &iedName, const std::wstring &ldInst,
												   SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
												   const int *subChannelIdxList, int listSize,
												   CHANNEL_TYPE_GOOSE eChannelType)
{
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// 创建该子通道的自定义类型模板，与原通道一致
	SCL_STR daTypeID_Next;
	if ( CreateDefSubChannel(pCache,iedName,ldInst,channelInfo,subChannelIdxList,listSize-1,daTypeID_Next) == -1 )
		return -1;

	return SetBDAInDaType(pCache,daTypeID_Next,subChannelIdxList[listSize-1],eChannelType);
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
int SCLGOOSEAnalysis::CopyIEDGooseSubChannelToEnd(const std::wstring &iedName, const std::wstring &ldInst,
												  SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
												  const int *subChannelIdxList, int listSize ,int iCopyCount)
{
	if ( subChannelIdxList == NULL )
		return -1;
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	// 创建该子通道的自定义类型模板，与原通道一致
	SCL_STR daTypeID_Next;
	if ( CreateDefSubChannel(pCache,iedName,ldInst,channelInfo,subChannelIdxList,listSize-1,daTypeID_Next) == -1 )
		return -1;

	// 获取指定的子通道的信息
	SCL_BDAINDATYPE_ATTRIB bdaAttrib;
	if ( !pCache->GetBDAAttrib(daTypeID_Next,subChannelIdxList[listSize-1],bdaAttrib) )
		return -1;
	// 添加子通道
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
///     移除Goose控制块所控制数据集中的某一通道中的一个子通道
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
///
/// @return
///     0：操作成功；  -1：失败
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

	// 创建该子通道的自定义类型模板，与原通道一致
	SCL_STR daTypeID_Next;
	if ( CreateDefSubChannel(pCache,iedName,ldInst,channelInfo,subChannelIdxList,listSize-1,daTypeID_Next) == -1 )
		return -1;

	// 移除子通道的信息
	if ( !pCache->RemoveBDAAttrib(daTypeID_Next,subChannelIdxList[listSize-1]) )
		return -1;

	return 0;
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
///     判断某AppID的控制块是否存在
///
/// @param[in]
///     appID - 控制块的AppID值
///
/// @return
///     0 - 存在； -1 - 不存在
///
int SCLGOOSEAnalysis::IsAppIDExisted(const unsigned short appID)
{
	// 判断缓存是否存在
	SCLCache *pCache = m_pSclCacheMgr->GetCache();
	if ( pCache == NULL )
		return -1;

	return pCache->IsGSEAppIDExisted(appID);
}

///
/// @brief
///     获取自定义数据类型模板中的DaName信息
///
/// @param[in]
///     eChannelType - goose通道类型
/// @param[in]
///     daName       - 保存自定义数据类型模板中的DaName信息
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
///     将解析得到的SCL数据转换成IED某一GOOSE传输的信息
///
/// @param[in]
///     gseAttrib     - GOOSE通信参数信息
/// @param[in]
///     iedAttrib     - IED属性信息
/// @param[in]
///     apAttrib      - AccessPoint属性信息
/// @param[in]
///     ldAttrib      - LDevice属性信息
/// @param[in]
///     gseCtrlAttrib - GSEControl属性信息
/// @param[in]
///     iedGOOSEInfo  - 保存IED中某一GOOSE传输相关信息的结构体
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
///     将解析得到的SCL数据转换成某一通道（FCDA）的信息
///
/// @param[in]
///     dataSetAttrib - 该通道（FCDA）所在DataSet的属性信息
/// @param[in]
///     fcdaAttrib    - 该通道（FCDA）的引用信息
/// @param[in]
///     lnDesc        - 该通道（FCDA）所指向的数据所在LNode的描述信息
/// @param[in]
///     doiAttrib     - 该通道（FCDA）所指向的数据的信息
/// @param[in]
///     daBType       - 该通道（FCDA）所指向的数据属性的基本类型信息
/// @param[in]
///     dataSetInfo   - 保存通道（FCDA）的信息的结构体
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
///     将解析得到的SCL数据转换成GOOSE INPUT信息（外部信号连线信息）
///
/// @param[in]
///     iedName       - Inputs所在的IED的名称
/// @param[in]
///     extRefAttrib  - 解析得到的Inputs ExtRef信息
/// @param[in]
///     inputsInfo    - 保存GOOSE INPUT信息的结构体
///
/// @return
///     true - 操作成功； false - 操作失败
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

	// 转换外部信号信息
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
///     通过通道（FCDA）的引用判断是否目标通道（FCDA）
///
/// @param[in]
///     dataSetInfo - 所要判断的通道（FCDA）的信息
/// @param[in]
///     fcdaAttrib  - 该通道（FCDA）的引用信息
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
void SCLGOOSEAnalysis::ConvHex2WStr(const unsigned int *intData, std::wstring &str)
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
void SCLGOOSEAnalysis::ConvDec2WStr(const unsigned int *intData, std::wstring &str)
{
	wchar_t c[256] = {0};
	swprintf_s(c,256,L"%d",*intData);
	str = c;
}

///
/// @brief
///     将SCL文件中定义的时间信息转换为以ms为单位的整数值
///
/// @param[in]
///     timeVal  - SCL文件中定义的时间的值
/// @param[in]
///     timeUnit - SCL文件中定义的时间的单位
/// @param[in]
///     time     - 指向转换结果的指针
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
///     更新GOOSE通道的数据类型模板（LNodeType信息）
///
/// @param[in]
///     lnTypeID - 通道数据所属逻辑节点的类型，即类型ID
/// @param[in]
///     lnClass  - 通道数据所属逻辑节点的类
/// @param[in]
///     doName   - 通道数据(DO)的名称
/// @param[in]
///     eChannelType - 通道类型
///
/// @return
///     0：操作成功；  -1：失败
///
int  SCLGOOSEAnalysis::AddDOIntoDefLNodeType(SCLCache *pCache ,const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
										     const SCL_STR &lnTypeID, CHANNEL_TYPE_GOOSE eChannelType)
{
	if ( pCache == NULL)
		return -1;

	// 不存在LNodeType，则添加
	SCL_LNTYPE_ATTRIB lnTypeAttrib;
	if ( !pCache->GetLNType(lnTypeID,lnTypeAttrib) )
	{
		lnTypeAttrib.id      = lnTypeID;
		lnTypeAttrib.lnClass = _T("DEFLN");
		if ( !pCache->AddLNType(lnTypeAttrib) )
			return -1;
	}

	// 添加DO信息到LNodeType
	SCL_DOINLNTYPE_ATTRIB doAttrib;
	if ( !pCache->GetDOAttrib(lnTypeID,dataSetInfo.doName,doAttrib) )
	{
		SCL_STR doTypeID;

		// LNodeType中不存在该DO，则根据通道类型选择该通道的自定义DOType模板
		// 模板不存在则新建
		if ( eChannelType == CT_GOOSE_STRUCT )
		{
			// 该通道为结构体类型
			doTypeID = _T("CN_GOOSE_DEFSTRUCT");
			SCL_DOTYPE_ATTRIB doTypeAttrib;
			if ( !pCache->GetDOType(doTypeID,doTypeAttrib) )
			{
				doTypeAttrib.cdc = _T("DEF");
				doTypeAttrib.id  = doTypeID;
				if ( !pCache->AddDOType(doTypeAttrib) )
					return -1;
			}

			// 默认结构体包含一个单点数据
			SCL_DAINDOTYPE_ATTRIB daAttrib;
			if ( !pCache->GetDAAttrib(doTypeID,dataSetInfo.daName,daAttrib) )
			{
				SCL_DAINDOTYPE_ATTRIB daAttrib = GOOSE_CHANNEL_TYPE_TEMPLATE[CT_GOOSE_STRUCT];
				daAttrib.name = dataSetInfo.daName;
				daAttrib.type = doTypeAttrib.id + L"_" + dataSetInfo.daName;
				if ( !pCache->AddDAIntoDOType(doTypeAttrib.id,daAttrib) )
					return -1;

				// 添加默认的结构体数据类型定义
				AddDefaultDaType(pCache,daAttrib.type);
			}
		}
		else
		{
			// 该通道为非结构体类型
			doTypeID = _T("CN_GOOSE_DEF");
			SCL_DOTYPE_ATTRIB doTypeAttrib;
			if ( !pCache->GetDOType(doTypeID,doTypeAttrib) )
			{
				doTypeAttrib.cdc = _T("DEF");
				doTypeAttrib.id  = doTypeID;
				if ( !pCache->AddDOType(doTypeAttrib) )
					return -1;

				// 该模板默认含有全部非结构体类型
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

		// 添加/更新DO信息到LNodeType
		doAttrib.name = dataSetInfo.doName;
		doAttrib.type = doTypeID;
		if ( !pCache->AddDOIntoLNType(lnTypeID,doAttrib) )
			return -1;
	}

	return 0;
}

///
/// @brief
///     创建使用自定义数据类型模板的子通道，其数据类型结构与原通道一致
///
/// @param[in]
///     pCache            - 指向SCL解析数据的缓存的指针
/// @param[in]
///     iedName           - 该子通道的所在的IED的名称
/// @param[in]
///     ldInst            - 该子通道的所在的LDevice的Inst号
/// @param[in]
///     channelInfo       - 该子通道所对应的最顶层的通道信息（即控制块所控制数据集中的某一通道）
/// @param[in]
///     subChannelIdxList - 从顶层通道下的第一层子通道开始至当前子通道，各子通道的顺序号列表
/// @param[in]
///     listSize          - 顺序号列表的大小
/// @param[in]
///     daTypeID_Out      - 该子通道的数据类型模板（DaType）的ID（结构体类型子通道有效）
///
/// @return
///     0：操作成功；  -1：失败
///
int  SCLGOOSEAnalysis::CreateDefSubChannel(SCLCache *pCache, const std::wstring &iedName,
										   const std::wstring &ldInst,
										   SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
										   const int *subChannelIdxList, int listSize,
										   SCL_STR &daTypeID_Out)
{
	if ( pCache == NULL)
		return -1;

	// 获取原通道的LNode的类型ID
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

//	// 该通道已使用自定义数据类型模板，则返回
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

	// 获取原通道的DO信息
	SCL_DOINLNTYPE_ATTRIB doAttrib;
	if ( !pCache->GetDOAttrib(lnTypeID,channelInfo.doName,doAttrib) )
		return -1;
	SCL_STR doTypeID = doAttrib.type;

	// 创建新的通道数据类型模板，并拷贝原通道的类型信息
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

	// 使用自定义逻辑节点及类型模板（DA Struct模式），替换原通道
	CHANNEL_TYPE_GOOSE eCType = GetChannelType(channelInfo);
	if ( SetOneIEDGOOSEDataSetInfo_Def(iedName,ldInst,channelInfo,eCType) == -1 )
		return -1;
	lnTypeID = L"XJ_LNODE_DEF";

	// 将新通道的索引道新通道的数据类型模板
	doAttrib.name = channelInfo.doName;
	doAttrib.type = L"CN_GOOSE_DEFSTRUCT";
	if ( !pCache->SetDOAttrib(lnTypeID,channelInfo.doName,doAttrib) )
		return -1;

	// 创建新通道的各级子通道的数据类型模板，并拷贝原通道的各级子通道的数据类型信息
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
///     将LnType1中的指定的DO复制到LnType2中
///
/// @param[in]
///     pCache            - 指向SCL解析数据的缓存的指针
/// @param[in]
///     lnTypeID1         - LnType1的ID
/// @param[in]
///     lnTypeID2         - LnType2的ID
/// @param[in]
///     doName            - 要复制的DO的名称
///
/// @return
///     0：操作成功；  -1：失败
///
int  SCLGOOSEAnalysis::CopyLnType(SCLCache *pCache, const std::wstring &lnTypeID1, const std::wstring &lnTypeID2, const std::wstring &doName)
{
	if ( pCache == NULL)
		return -1;

	if ( lnTypeID1 != lnTypeID2 )
	{
		// LnType2不存在则创建
		SCL_LNTYPE_ATTRIB lnTypeAttrib;
		if ( !pCache->GetLNType(lnTypeID2,lnTypeAttrib) )
		{
			lnTypeAttrib.id      = lnTypeID2;
			lnTypeAttrib.lnClass = L"GGIO_EX";
			if ( !pCache->AddLNType(lnTypeAttrib) )
				return -1;
		}

		// 拷贝LnType1中的指定的DO
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
///     设置DoType中的某一顺序号的DA的类型
///
/// @param[in]
///     pCache            - 指向SCL解析数据的缓存的指针
/// @param[in]
///     DoTypeID          - DoType的ID
/// @param[in]
///     idxSeq            - DA的顺序号
/// @param[in]
///     eChannelType      - DA的类型
///
/// @return
///     0：操作成功；  -1：失败
///
int  SCLGOOSEAnalysis::SetDAInDoType(SCLCache *pCache ,const std::wstring &doTypeID, int idxSeq, CHANNEL_TYPE_GOOSE eChannelType)
{
	if ( pCache == NULL)
		return -1;

	// 默认DA的fc为ST
	SCL_DAINDOTYPE_ATTRIB daAttrib;
	int daNum = pCache->GetDANumInDOType(doTypeID);
	if ( idxSeq >= daNum )
	{
		// DA顺序号大于DaNum,则补全再设置
		if ( !pCache->GetDAAttrib(doTypeID,daNum-1,daAttrib) )
			return -1;
		while ( idxSeq >= daNum )
		{
			if ( daNum == idxSeq )
			{
				daAttrib    = GOOSE_CHANNEL_TYPE_TEMPLATE[eChannelType];
			}
			// daName添加后缀，使其在DOType中保持唯一
			std::wstring str;
			ConvDec2WStr((unsigned int *)&daNum,str);
			daAttrib.name = daAttrib.name + _T("_") + str;

			if ( eChannelType == CT_GOOSE_STRUCT )
			{
				// DA为结构体类型
				daAttrib.type = doTypeID + _T("_") + str;
				if ( !pCache->AddDAIntoDOType(doTypeID,daAttrib) )
					return -1;

				// 添加默认的结构体数据类型定义
				AddDefaultDaType(pCache,daAttrib.type);
			}
			else
			{
				// DA为非结构体类型
				if ( !pCache->AddDAIntoDOType(doTypeID,daAttrib) )
					return -1;
			}
			daNum++;
		}
	}
	else
	{
		daAttrib    = GOOSE_CHANNEL_TYPE_TEMPLATE[eChannelType];

		// daName添加后缀，使其在DOType中保持唯一
		std::wstring str;
		ConvDec2WStr((unsigned int *)&daNum,str);
		daAttrib.name = daAttrib.name + _T("_") + str;

		if ( eChannelType == CT_GOOSE_STRUCT )
		{
			// DA为结构体类型
			daAttrib.type = doTypeID + _T("_") + str;
			if ( !pCache->SetDAAttrib(doTypeID,idxSeq,daAttrib) )
				return -1;

			// 添加默认的结构体数据类型定义
			AddDefaultDaType(pCache,daAttrib.type);
		}
		else
		{
			// DA为非结构体类型
			if ( !pCache->SetDAAttrib(doTypeID,idxSeq,daAttrib) )
				return -1;
		}
	}
	return 0;
}

///
/// @brief
///     将DoType1中的指定fc的DA复制到DoType2中
///
/// @param[in]
///     pCache            - 指向SCL解析数据的缓存的指针
/// @param[in]
///     DoTypeID1         - DoType1的ID
/// @param[in]
///     DoTypeID2         - DoType2的ID
/// @param[in]
///     fc                - 要复制的DA的fc
///
/// @return
///     0：操作成功；  -1：失败
///
int  SCLGOOSEAnalysis::CopyDoType(SCLCache *pCache, const std::wstring &doTypeID1, const std::wstring &doTypeID2, const std::wstring &fc)
{
	if ( pCache == NULL)
		return -1;

	if ( doTypeID1 != doTypeID2 )
	{
		// DoType2不存在则创建
		SCL_DOTYPE_ATTRIB doTypeAttrib;
		if ( !pCache->GetDOType(doTypeID2,doTypeAttrib) )
		{
			doTypeAttrib.cdc = _T("DEF");
			doTypeAttrib.id  = doTypeID2;
			if ( !pCache->AddDOType(doTypeAttrib) )
				return -1;
		}

		// 拷贝DoType1中指定fc的DA
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
///     将DoType中指定fc的DA复制到结构体类型的DaType中
///
/// @param[in]
///     pCache            - 指向SCL解析数据的缓存的指针
/// @param[in]
///     doTypeID          - DoType的ID
/// @param[in]
///     daTypeID          - DaType的ID
/// @param[in]
///     fc                - 要复制的DA的fc
///
/// @return
///     0：操作成功；  -1：失败
///
int  SCLGOOSEAnalysis::CopyDoTypeIntoStructDaType(SCLCache *pCache, const std::wstring &doTypeID, const std::wstring &daTypeID, const std::wstring &fc)
{
	if ( pCache == NULL)
		return -1;

	// DoType2不存在则创建
	SCL_DATYPE_ATTRIB daTypeAttrib;
	if ( !pCache->GetDAType(daTypeID,daTypeAttrib) )
	{
		daTypeAttrib.id  = daTypeID;
		if ( !pCache->AddDAType(daTypeAttrib) )
			return -1;
	}

	// 拷贝DoType中指定fc的DA
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
///     设置DaType中某一顺序号的BDA的类型
///
/// @param[in]
///     pCache            - 指向SCL解析数据的缓存的指针
/// @param[in]
///     daTypeID          - DaType的ID
/// @param[in]
///     idxSeq            - BDA的顺序号
/// @param[in]
///     eChannelType      - BDA的类型
///
/// @return
///     0：操作成功；  -1：失败
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

			// bdaName添加后缀，使其在DaType中保持唯一
			std::wstring str;
			ConvDec2WStr((unsigned int *)&bdaNum,str);
			bdaAttrib.name = bdaAttrib.name + _T("_") + str;

			if ( eChannelType == CT_GOOSE_STRUCT )
			{
				// BDA为结构体类型
				bdaAttrib.type = daTypeID + _T("_") + str;
				if ( !pCache->AddBDAIntoDAType(daTypeID,bdaAttrib) )
					return -1;

				// 添加默认的结构体数据类型定义
				AddDefaultDaType(pCache,bdaAttrib.type);
			}
			else
			{
				// DA为非结构体类型
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

		// bdaName添加后缀，使其在DaType中保持唯一
		std::wstring str;
		ConvDec2WStr((unsigned int *)&idxSeq,str);
		bdaAttrib.name = bdaAttrib.name + _T("_") + str;

		if ( eChannelType == CT_GOOSE_STRUCT )
		{
			// BDA为结构体类型
			bdaAttrib.type = daTypeID + _T("_") + str;
			if ( !pCache->SetBDAAttrib(daTypeID,idxSeq,bdaAttrib) )
				return -1;

			// 添加默认的结构体数据类型定义
			AddDefaultDaType(pCache,bdaAttrib.type);
		}
		else
		{
			// DA为非结构体类型
			if ( !pCache->SetBDAAttrib(daTypeID,idxSeq,bdaAttrib) )
				return -1;
		}
	}
	return 0;
}

///
/// @brief
///     添加默认配置的DaType
///
/// @param[in]
///     pCache            - 指向SCL解析数据的缓存的指针
/// @param[in]
///     daTypeID          - 要添加的DaType的ID
///
/// @return
///     0：操作成功；  -1：失败
///
int  SCLGOOSEAnalysis::AddDefaultDaType(SCLCache *pCache, const std::wstring &daTypeID)
{
	if ( pCache == NULL)
		return -1;

	// 默认结构体含有一个单点数据
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
///     将DaType1中的配置复制到DaType2中
///
/// @param[in]
///     pCache            - 指向SCL解析数据的缓存的指针
/// @param[in]
///     daTypeID1         - DaType1的ID
/// @param[in]
///     daTypeID2         - DaType2的ID
///
/// @return
///     0：操作成功；  -1：失败
///
int  SCLGOOSEAnalysis::CopyDaType(SCLCache *pCache, const std::wstring &daTypeID1, const std::wstring &daTypeID2)
{
	if ( pCache == NULL)
		return -1;

	if ( daTypeID1 != daTypeID2 )
	{
		// DaType2不存在则创建
		SCL_DATYPE_ATTRIB daTypeAttrib;
		if ( !pCache->GetDAType(daTypeID2,daTypeAttrib) )
		{
			daTypeAttrib.id  = daTypeID2;
			if ( !pCache->AddDAType(daTypeAttrib) )
				return -1;
		}

		// 拷贝DaType1
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
///     移除DaType中指定顺序号的BDA，包括该BDA链接的其他DaType
///
/// @param[in]
///     pCache            - 指向SCL解析数据的缓存的指针
/// @param[in]
///     daTypeID          - DaType的ID
/// @param[in]
///     idxSeq            - 要移除的BDA的顺序号
///
/// @return
///     0：操作成功；  -1：失败
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
