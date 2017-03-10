#ifndef SCL_SMV92_ANALYSIS_H_

#define SCL_SMV92_ANALYSIS_H_

#include "include/SCDSMV92Analysis.h"
#include "sclcachemanager.h"

class SCLSmv92Analysis:public ISCDSMV92Analysis
{
public:
	SCLSmv92Analysis(ISCLCacheManager *pSclCacheMgr);
	virtual ~SCLSmv92Analysis();

	virtual int GetIEDSMVNum( void );

	virtual int GetAllIEDSMVInfo( 
		SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfoArr, 
		const int arrSize );

	virtual int GetOneIEDSMVInfo( 
		const unsigned char *smvMAC, 
		const unsigned short smvAppID,
		SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfo );

	virtual int GetIEDSMVDataSetNum( 
		const unsigned char *smvMAC, 
		const unsigned short smvAppID );

	virtual int GetAllIEDSMVDataSetInfo(
		const unsigned char *smvMAC, 
		const unsigned short smvAppID,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfoArr,
		const int arrSize );

	virtual int GetOneIEDSMVDataSetInfo(
		const unsigned char *smvMAC, 
		const unsigned short smvAppID,
		const int            idxSeq,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo );

	virtual int GetOneIEDSMVDataSetIdxSeq(
		const unsigned char *smvMAC, 
		const unsigned short smvAppID,
		const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo );

	virtual int GetOneIEDInputsExtRefNum(
		const std::wstring &iedName);

	virtual int GetOneIEDSVINExtRef(
		const std::wstring &iedName,
		SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO *inputsInfo,
		const int arrSize);

	virtual int GetOutAddrSMVInfo(
		const SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO &inputsInfo,
		SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfo);

	virtual int GetAllIEDSMVNumByCtrlRef( void );

	virtual int GetAllIEDSMVInfoByCtrlRef(
		SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfoArr, 
		const int arrSize );

	virtual int SetOneIEDSMVInfoByCtrlRef(const SCDSMV92ANALYSIS_IEDSMV_INFO &iedSMVInfo);

	virtual int GetIEDSMVDataSetNum(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName);

	virtual int GetAllIEDSMVDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfoArr,
		const int arrSize );

	virtual int GetOneIEDSMVDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		const int idxSeq,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo);

	virtual int SetOneIEDSMVDataSetInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_SMV eChannelType );

	virtual int AddOneIEDSMVDataSetInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_SMV eChannelType );

	virtual int CopyIEDSMVDataSetInfoToSetEnd(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &dataSetName,
		int idxSeq, int iCopyCount );

	virtual int RemoveOneIEDSMVDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &datSetName,
		const int idxSeq);

	virtual int IsAppIDExisted(const unsigned short appID);
	
	virtual CHANNEL_TYPE_SMV GetChannelType(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo);

	virtual CHANNEL_PHASE_SMV GetChannelPhase(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo, CHANNEL_TYPE_SMV eChannelType);

protected:
	void SCL2IEDSMVInfo(
		const SCL_SMV_ATTRIB &smvAttrib,
		const SCL_IED_ATTRIB &iedAttrib,
		const SCL_ACCESS_POINT_ATTRIB &apAttrib,
		const SCL_LDEVICE_ATTRIB &ldAttrib,
		const SCL_DATASET_ATTRIB &dataSetAttrib,
		const SCL_SMVCTRL_ATTRIB &smvCtrlAttrib,
		SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfo);

	void SCL2DataSetInfo(
		const SCL_DATASET_ATTRIB &dataSetAttrib,
		const SCL_FCDA_ATTRIB &fcdaAttrib,
		const SCL_STR &lnDesc,
		const SCL_DOI_ATTRIB &doiAttrib,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo);

	bool SCL2InputsInfo(
		const SCL_STR                           &iedName, 
		const SCL_EXTREF_ATTRIB                 &extRefAttrib,
		SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO     *inputsInfo);

	bool BOOLEAN_SMV92toSTD(SMV92_BOOLEAN b);

	bool IsGoalFCDA(
		const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo,
		const SCL_FCDA_ATTRIB &fcdaAttrib);
	
	void ConvMacWStr2Char(const std::wstring &mac, unsigned char *macRetn);
	void ConvMacChar2WStr(const unsigned char *macRetn,std::wstring &mac);

	void ConvWStr2Hex(const std::wstring &str, unsigned int *intData);
	void ConvHex2WStr(const unsigned int *intData, std::wstring &str);

	void ConvWStr2Dec(const std::wstring &str, unsigned int *intData);
	void ConvDec2WStr(const unsigned int *intData, std::wstring &str);

	bool IsMacAddrSame(const unsigned char *mac1, const unsigned char *mac2);

	int  AddDefChannelDataTemplates(const SCL_STR &lnType, const SCL_STR &lnClass, const SCL_STR &doName);

private:
	SCLCacheManager *m_pSclCacheMgr;
};

#endif
