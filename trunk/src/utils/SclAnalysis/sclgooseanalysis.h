#ifndef SCL_GOOSE_ANALYSIS_H_

#define SCL_GOOSE_ANALYSIS_H_

#include "include/SCDGOOSEAnalysis.h"
#include "sclcachemanager.h"

class SCLGOOSEAnalysis:public ISCDGOOSEAnalysis
{
public:
	SCLGOOSEAnalysis( ISCLCacheManager *pSclCacheMgr );
	virtual ~SCLGOOSEAnalysis();

	virtual int GetIEDGOOSENum( void );

	virtual int GetAllIEDGOOSEInfo( 
		SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfoArr, 
		const int arrSize );

	virtual int GetOneIEDGOOSEInfo( 
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfo );

	virtual int GetIEDGOOSEDataSetNum( 
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID );

	virtual int GetAllIEDGOOSEDataSetInfo(
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr,
		const int arrSize );

	virtual int GetOneIEDGOOSEDataSetInfo(
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		const int            idxSeq,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo );

	virtual int GetOneIEDGOOSEDataSetIdxSeq(
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo );

	virtual int GetIEDGOOSESubChannelNum(
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo);

	virtual int GetAllIEDGOOSESubChannelInfo(
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr,
		const int arrSize );

	virtual int GetOneIEDInputsExtRefNum(
		const std::wstring &iedName);

	virtual int GetOneIEDGOINExtRef(
		const std::wstring &iedName,
		SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO *inputsInfo,
		const int arrSize);

	virtual int GetOutAddrGOOSEInfo(
		const SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO &inputsInfo,
		SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfo);

	virtual int GetAllIEDGOOSENumByCtrlRef( void );

	virtual int GetAllIEDGOOSEInfoByCtrlRef(
		SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfoArr, 
		const int arrSize );

	virtual int SetOneIEDGOOSEInfoByCtrlRef(const SCDGOOSEANALYSIS_IEDGOOSE_INFO &iedGOOSEInfo);

	virtual int GetIEDGOOSEDataSetNum(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName);

	virtual int GetAllIEDGOOSEDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr,
		const int arrSize );

	virtual int GetOneIEDGOOSEDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		const int idxSeq,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo);

	virtual int SetOneIEDGOOSEDataSetInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_GOOSE eChannelType );

	virtual int AddOneIEDGOOSEDataSetInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_GOOSE eChannelType );

	virtual int CopyIEDGOOSEDataSetInfoToSetEnd(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &dataSetName,
		int idxSeq,
		int iCopyCount);

	virtual int RemoveOneIEDGOOSEDataSetInfo(
		const SCL_STR &iedName,
		const SCL_STR &ldInst,
		const SCL_STR &datSetName,
		const int idxSeq);

	virtual int GetIEDGOOSESubChannelNum(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo);

	virtual int GetAllIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfoArr,
		const int arrSize );

	virtual int GetOneIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		int idxSeq,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &subChannelInfo );

	virtual int SetOneIEDGOOSESubChannelInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize,
		CHANNEL_TYPE_GOOSE eChannelType);

	virtual int CopyIEDGooseSubChannelToEnd(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize,
		int iCopyCount);

	virtual int RemoveOneIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize);

	virtual CHANNEL_TYPE_GOOSE GetChannelType(const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO& channelInfo);

	virtual int IsAppIDExisted(const unsigned short appID);

	virtual void GetDefDataTemplate_DaName(CHANNEL_TYPE_GOOSE eChannelType, std::wstring &daName);

protected:
	void SCL2IEDGOOSEInfo(
		const SCL_GSE_ATTRIB           &gseAttrib,
		const SCL_IED_ATTRIB           &iedAttrib,
		const SCL_ACCESS_POINT_ATTRIB  &apAttrib,
		const SCL_LDEVICE_ATTRIB       &ldAttrib,
		const SCL_DATASET_ATTRIB       &dataSetAttrib,
		const SCL_GSECTRL_ATTRIB       &gseCtrlAttrib,
		SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfo);

	void SCL2DataSetInfo(
		const SCL_DATASET_ATTRIB               &dataSetAttrib,
		const SCL_FCDA_ATTRIB                  &fcdaAttrib,
		const SCL_STR                          &lnDesc,
		const SCL_DOI_ATTRIB                   &doiAttrib,
		const SCL_STR                          &daBType,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo);

	bool SCL2InputsInfo(
		const SCL_STR                           &iedName, 
		const SCL_EXTREF_ATTRIB                 &extRefAttrib,
		SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO  *inputsInfo);

	bool IsGoalFCDA(
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo,
		const SCL_FCDA_ATTRIB                        &fcdaAttrib);

	void ConvMacWStr2Char(const std::wstring &mac, unsigned char *macRetn);
	void ConvMacChar2WStr(const unsigned char *macRetn,std::wstring &mac);

	void ConvWStr2Hex(const std::wstring &str, unsigned int *intData);
	void ConvHex2WStr(const unsigned int *intData, std::wstring &str);

	void ConvWStr2Dec(const std::wstring &str, unsigned int *intData);
	void ConvDec2WStr(const unsigned int *intData, std::wstring &str);

	void ConvSCLTime(const std::wstring &timeVal, const std::wstring &timeUnit, unsigned int *time);

	bool IsMacAddrSame(const unsigned char *mac1, const unsigned char *mac2);

	int  AddDOIntoDefLNodeType(
		SCLCache *pCache, const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		const SCL_STR &lnTypeID, CHANNEL_TYPE_GOOSE eChannelType
		);

	int  CreateDefSubChannel(
		SCLCache *pCache,
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize,
		SCL_STR &daTypeID_Next);

	int  CopyLnType(SCLCache *pCache, const std::wstring &lnTypeID1, const std::wstring &lnTypeID2, const std::wstring &doName);
	int  SetDAInDoType(SCLCache *pCache, const std::wstring &doTypeID, int idxSeq, CHANNEL_TYPE_GOOSE eChannelType);
	int  CopyDoType(SCLCache *pCache, const std::wstring &doTypeID1, const std::wstring &doTypeID2, const std::wstring &fc);
	int  CopyDoTypeIntoStructDaType(SCLCache *pCache, const std::wstring &doTypeID, const std::wstring &daTypeID, const std::wstring &fc);
	int  SetBDAInDaType(SCLCache *pCache , const std::wstring &daTypeID, int idxSeq, CHANNEL_TYPE_GOOSE eChannelType);
	int  AddDefaultDaType(SCLCache *pCache, const std::wstring &daTypeID);
	int  CopyDaType(SCLCache *pCache, const std::wstring &daTypeID1, const std::wstring &daTypeID2);
	int  RemoveBDAInDaType(SCLCache *pCache, const SCL_STR &daTypeID, int idxSeq);

private:
	SCLCacheManager *m_pSclCacheMgr;
};

#endif
