#ifndef SCL_CACHE_WITH_MAP_H_

#define SCL_CACHE_WITH_MAP_H_

#include "sclcache.h"
#include "sclcachemaptype.h"


class SCLCacheWithMap:public SCLCache
{
public:
	SCLCacheWithMap();
	virtual ~SCLCacheWithMap();

	virtual void ClearCache();

	virtual bool AddIED(const SCL_IED_ATTRIB &iedAttrib);
	virtual bool AddLDeviceIntoIED(const SCL_STR &iedName, const SCL_ACCESS_POINT_ATTRIB &apAttrib, const SCL_LDEVICE_ATTRIB &ldAttrib);
	virtual bool AddLN0IntoLD(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_LN0_ATTRIB &ln0Attrib);
	virtual bool AddLNIntoLD(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_LN_ATTRIB &lnAttrib);
	virtual bool AddDataSetIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_DATASET_ATTRIB &datasetAttrib);
	virtual bool AddSMVCtrlIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_SMVCTRL_ATTRIB &smvAttrib);
	virtual bool AddGSECtrlIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_GSECTRL_ATTRIB &gseAttrib);
	virtual bool AddDOIIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_DOI_ATTRIB &doiAttrib);
	virtual bool AddDOIIntoLN(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_DOI_ATTRIB &doiAttrib);
	virtual bool AddExtRefIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_EXTREF_ATTRIB &extRefAttrib);
	virtual bool AddExtRefIntoLN(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_EXTREF_ATTRIB &extRefAttrib);
	virtual bool AddFCDAIntoDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
		                           const SCL_STR &dataSetName, const SCL_FCDA_ATTRIB &fcdaAttrib);
	virtual bool AddSMVCommn(const SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, const SCL_SMV_ATTRIB &smvAttrib);
	virtual bool AddGSECommn(const SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, const SCL_GSE_ATTRIB &gseAttrib);
	virtual bool AddLNType(const SCL_LNTYPE_ATTRIB &lnTypeAttrib);
	virtual bool AddDOIntoLNType(const SCL_STR &lnTypeID, const SCL_DOINLNTYPE_ATTRIB &doAttrib);
	virtual bool AddDOType(const SCL_DOTYPE_ATTRIB &doTypeAttrib);
	virtual bool AddDAIntoDOType(const SCL_STR &doTypeID, const SCL_DAINDOTYPE_ATTRIB &daAttrib);
	virtual bool AddDAType(const SCL_DATYPE_ATTRIB &daTypeAttrib);
	virtual bool AddBDAIntoDAType(const SCL_STR &daTypeID, const SCL_BDAINDATYPE_ATTRIB &bdaAttrib);

	virtual int  GetIEDNum();
	virtual bool SetIEDAttrib(const SCL_STR &iedName, const SCL_IED_ATTRIB &iedAttrib);
	virtual bool SetIEDAttrib(const int idxSeq, const SCL_IED_ATTRIB &iedAttrib);
	virtual bool GetIEDAttrib(const SCL_STR &iedName, SCL_IED_ATTRIB &iedAttrib);
	virtual bool GetIEDAttrib(const int idxSeq, SCL_IED_ATTRIB &iedAttrib);

	virtual int  GetLDeviceNum(const SCL_STR &iedName);
	virtual bool SetLDeviceAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_ACCESS_POINT_ATTRIB &apAttrib, const SCL_LDEVICE_ATTRIB &ldAttrib);
	virtual bool SetLDeviceAttrib(const SCL_STR &iedName, const int idxSeq, const SCL_ACCESS_POINT_ATTRIB &apAttrib, const SCL_LDEVICE_ATTRIB &ldAttrib);
	virtual bool GetLDeviceAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, SCL_ACCESS_POINT_ATTRIB &apAttrib, SCL_LDEVICE_ATTRIB &ldAttrib);
	virtual bool GetLDeviceAttrib(const SCL_STR &iedName, const int idxSeq, SCL_ACCESS_POINT_ATTRIB &apAttrib, SCL_LDEVICE_ATTRIB &ldAttrib);

	virtual bool SetLN0Attrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_LN0_ATTRIB &ln0Attrib);
	virtual bool GetLN0Attrib(const SCL_STR &iedName, const SCL_STR &ldInst, SCL_LN0_ATTRIB &ln0Attrib);

	virtual int  GetLNNum(const SCL_STR &iedName, const SCL_STR &ldInst);
	virtual bool SetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_LN_ATTRIB &lnAttrib);
	virtual bool SetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_LN_ATTRIB &lnAttrib);
	virtual bool GetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, SCL_LN_ATTRIB &lnAttrib);
	virtual bool GetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_LN_ATTRIB &lnAttrib);

	virtual int  GetDataSetNumInLN0(const SCL_STR &iedName, const SCL_STR &ldInst);
	virtual bool SetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, const SCL_DATASET_ATTRIB &dateSetAttrib);
	virtual bool SetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_DATASET_ATTRIB &dateSetAttrib);
	virtual bool GetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, SCL_DATASET_ATTRIB &dateSetAttrib);
	virtual bool GetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_DATASET_ATTRIB &dateSetAttrib);

	virtual int  GetSMVCtrlNumInLN0(const SCL_STR &iedName, const SCL_STR &ldInst);
	virtual bool SetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_SMVCTRL_ATTRIB &smvCtrlAttrib);
	virtual bool SetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, const SCL_SMVCTRL_ATTRIB &smvCtrlAttrib);
	virtual bool SetSMVCtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, const SCL_SMVCTRL_ATTRIB &smvCtrlAttrib);
	virtual bool GetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_SMVCTRL_ATTRIB &smvCtrlAttrib);
	virtual bool GetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, SCL_SMVCTRL_ATTRIB &smvCtrlAttrib);
	virtual bool GetSMVCtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, SCL_SMVCTRL_ATTRIB &smvCtrlAttrib);

	virtual int  GetGSECtrlNumInLN0(const SCL_STR &iedName, const SCL_STR &ldInst);
	virtual bool SetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_GSECTRL_ATTRIB &gseCtrlAttrib);
	virtual bool SetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, const SCL_GSECTRL_ATTRIB &gseCtrlAttrib);
	virtual bool SetGSECtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, const SCL_GSECTRL_ATTRIB &gseCtrlAttrib);
	virtual bool GetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_GSECTRL_ATTRIB &gseCtrlAttrib);
	virtual bool GetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, SCL_GSECTRL_ATTRIB &gseCtrlAttrib);
	virtual bool GetGSECtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, SCL_GSECTRL_ATTRIB &gseCtrlAttrib);

	virtual int  GetDOINumInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName);
	virtual bool SetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const int idxSeq, const SCL_DOI_ATTRIB &doiAttrib);
	virtual bool SetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &doiName, const SCL_DOI_ATTRIB &doiAttrib);
	virtual bool GetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const int idxSeq, SCL_DOI_ATTRIB &doiAttrib);
	virtual bool GetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &doiName, SCL_DOI_ATTRIB &doiAttrib);

	virtual int  GetInputsExtRefNumInIED(const SCL_STR &iedName);
	virtual bool GetAllInputsExtRefInIED(const SCL_STR &iedName, SCL_EXTREF_ATTRIB *extRefAttribArray, const int arrSize);
	virtual int  GetExtRefNumInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName);
	virtual bool SetExtRefAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
		                         const int &idxSeq, const SCL_EXTREF_ATTRIB &extRefAttrib);
	virtual bool GetExtRefAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
		                         const int &idxSeq, SCL_EXTREF_ATTRIB &extRefAttrib);

	virtual bool SetFCDAAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &datSetName,
		                       const int idxSeq, const SCL_FCDA_ATTRIB &fcdaAttrib);
	virtual bool GetFCDAAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &datSetName,
		                       const int idxSeq, SCL_FCDA_ATTRIB &fcdaAttrib);
	virtual int  GetFCDAIdxSeqInDataSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &datSetName,
		                                const SCL_FCDA_ATTRIB &fcdaAttrib);
	virtual int  GetFCDAIdxSeqInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_FCDA_ATTRIB &fcdaAttrib);

	virtual int  IsSMVAppIDExisted(const unsigned short appID);
	virtual int  GetSMVCommnNumInCache();
	virtual bool GetAllSMVCommn(SCL_CONNECTEDAP_ATTRIB *cnnctAPAttribArray, SCL_SMV_ATTRIB *smvAttribArray, const int arraySize);
	virtual bool SetSMVCommnWithIdx(const SCL_STR iedName, const SCL_STR ldInst, const SCL_STR cbName, const SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, const SCL_SMV_ATTRIB &smvAddr);
	virtual bool GetSMVCommn(const unsigned short appID, SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, SCL_SMV_ATTRIB &smvAddr);
	virtual bool GetSMVCommnWithIdx(const SCL_STR iedName, const SCL_STR ldInst, const SCL_STR cbName, SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, SCL_SMV_ATTRIB &smvAddr);

	virtual int  IsGSEAppIDExisted(const unsigned short appID);
	virtual int  GetGSECommnNumInCache();
	virtual bool GetAllGSECommn(SCL_CONNECTEDAP_ATTRIB *cnnctAPAttribArray, SCL_GSE_ATTRIB *gseAttribArray, const int arraySize);
	virtual bool SetGSECommnWithIdx(const SCL_STR iedName, const SCL_STR ldInst, const SCL_STR cbName, const SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, const SCL_GSE_ATTRIB &gseAddr);
	virtual bool GetGSECommn(const unsigned short appID, SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, SCL_GSE_ATTRIB &gseAddr);
	virtual bool GetGSECommnWithIdx(const SCL_STR iedName, const SCL_STR ldInst, const SCL_STR cbName, SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, SCL_GSE_ATTRIB &gseAddr);

	virtual int  GetLNTypeNum();
	virtual bool SetLNType(const int idxSeq, const SCL_LNTYPE_ATTRIB &lnTypeAttrib);
	virtual bool SetLNType(const SCL_STR &lnTypeID, const SCL_LNTYPE_ATTRIB &lnTypeAttrib);
	virtual bool GetLNType(const int idxSeq, SCL_LNTYPE_ATTRIB &lnTypeAttrib);
	virtual bool GetLNType(const SCL_STR &lnTypeID, SCL_LNTYPE_ATTRIB &lnTypeAttrib);

	virtual int  GetDONumInLNType(const SCL_STR &lnType);
	virtual bool SetDOAttrib(const SCL_STR &lnType, const int idxSeq, const SCL_DOINLNTYPE_ATTRIB &doAttrib);
	virtual bool SetDOAttrib(const SCL_STR &lnType, const SCL_STR &doName, const SCL_DOINLNTYPE_ATTRIB &doAttrib);
	virtual bool GetDOAttrib(const SCL_STR &lnType, const int idxSeq, SCL_DOINLNTYPE_ATTRIB &doAttrib);
	virtual bool GetDOAttrib(const SCL_STR &lnType, const SCL_STR &doName, SCL_DOINLNTYPE_ATTRIB &doAttrib);

	virtual int  GetDOTypeNum();
	virtual bool SetDOType(const int idxSeq, const SCL_DOTYPE_ATTRIB &doTypeAttrib);
	virtual bool SetDOType(const SCL_STR &doTypeID, const SCL_DOTYPE_ATTRIB &doTypeAttrib);
	virtual bool GetDOType(const int idxSeq, SCL_DOTYPE_ATTRIB &doTypeAttrib);
	virtual bool GetDOType(const SCL_STR &doTypeID, SCL_DOTYPE_ATTRIB &doTypeAttrib);

	virtual int  GetDANumInDOType(const SCL_STR &doType);
	virtual bool SetDAAttrib(const SCL_STR &doType, const int idxSeq, const SCL_DAINDOTYPE_ATTRIB &daAttrib);
	virtual bool SetDAAttrib(const SCL_STR &doType, const SCL_STR &daName, const SCL_DAINDOTYPE_ATTRIB &daAttrib);
	virtual bool GetDAAttrib(const SCL_STR &doType, const int idxSeq, SCL_DAINDOTYPE_ATTRIB &daAttrib);
	virtual bool GetDAAttrib(const SCL_STR &doType, const SCL_STR &daName, SCL_DAINDOTYPE_ATTRIB &daAttrib);

	virtual int  GetDATypeNum();
	virtual bool SetDAType(const int idxSeq, const SCL_DATYPE_ATTRIB &daTypeAttrib);
	virtual bool SetDAType(const SCL_STR &daTypeID, const SCL_DATYPE_ATTRIB &daTypeAttrib);
	virtual bool GetDAType(const int idxSeq, SCL_DATYPE_ATTRIB &daTypeAttrib);
	virtual bool GetDAType(const SCL_STR &daTypeID, SCL_DATYPE_ATTRIB &daTypeAttrib);

	virtual int  GetBDANumInDAType(const SCL_STR &daType);
	virtual bool SetBDAAttrib(const SCL_STR &daType, const int idxSeq, const SCL_BDAINDATYPE_ATTRIB &bdaAttrib);
	virtual bool SetBDAAttrib(const SCL_STR &daType, const SCL_STR &bdaName, const SCL_BDAINDATYPE_ATTRIB &bdaAttrib);
	virtual bool GetBDAAttrib(const SCL_STR &daType, const int idxSeq, SCL_BDAINDATYPE_ATTRIB &bdaAttrib);
	virtual bool GetBDAAttrib(const SCL_STR &daType, const SCL_STR &bdaName, SCL_BDAINDATYPE_ATTRIB &bdaAttrib);

	virtual bool RemoveFCDAAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
		                          const SCL_STR &datSetName, const int idxSeq);
	virtual bool RemoveDAAttrib(const SCL_STR &doTypeID, const int idxSeq);
	virtual bool RemoveBDAAttrib(const SCL_STR &daTypeID, const int idxSeq);

protected:
	bool FindIEDInMap(const SCL_STR &iedName, IED_MAP::iterator &it_IED);
	bool FindLDeviceInIED(const IED_MAP::iterator &it_IED, const SCL_STR &ldInst, LDEVICE_MAP::iterator &it_LD);
	bool FindLN0InLDevice(const LDEVICE_MAP::iterator &it_LD, LN0_INFO_CACHE **ln0);
	bool FindLNInLDevice(const LDEVICE_MAP::iterator &it_LD, const SCL_STR &lnName, LN_MAP::iterator &it_LN);
	bool FindDataSetInLN0(const LN0_INFO_CACHE * const ln0, const SCL_STR &dataSetName, DATASET_MAP::iterator &it_DataSet);
	bool FindFCDAInDataSet(const DATASET_MAP::iterator &it_DataSet, const int idxSeq, FCDA_MAP::iterator &it_FCDA);
	bool FindSMVCtrlInLN0(const LN0_INFO_CACHE * const ln0, const SCL_STR &cbName, SMV_CTRL_MAP::iterator &it_SMVCtrl);
	bool FindGSECtrlInLN0(const LN0_INFO_CACHE * const ln0, const SCL_STR &cbName, GSE_CTRL_MAP::iterator &it_GSECtrl);
	bool FindDOIInLN0(const LN0_INFO_CACHE * const ln0, const SCL_STR &doiName, DOI_MAP::iterator &it_DOI);
	bool FindDOIInLN(const LN_MAP::iterator &it_LN, const SCL_STR &doiName, DOI_MAP::iterator &it_DOI);
	bool FindExtRefInLN0(const LN0_INFO_CACHE * const ln0, const int &idxSeq, EXTREF_MAP::iterator &it_ExtRef);
	bool FindExtRefInLN(const LN_MAP::iterator &it_LN, const int &idxSeq, EXTREF_MAP::iterator &it_ExtRef);
	bool FindLDeviceInMap(const SCL_STR &iedName, const SCL_STR &ldInst, LDEVICE_MAP::iterator &it_LD);
	bool FindLN0InMap(const SCL_STR &iedName, const SCL_STR &ldInst, LN0_INFO_CACHE **ln0);
	bool FindLNInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, LN_MAP::iterator &it_LN);
	bool FindDataSetInLN0InMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, DATASET_MAP::iterator &it_DataSet);
	bool FindFCDAInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
	                   const SCL_STR &dataSetName, const int idxSeq, FCDA_MAP::iterator &it_FCDA);
	bool FindSMVCtrlInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, SMV_CTRL_MAP::iterator &it_SMVCtrl);
	bool FindGSECtrlInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, GSE_CTRL_MAP::iterator &it_GSECtrl);
	bool FindDOIInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
	                  const SCL_STR &doiName, DOI_MAP::iterator &it_DOI);
	bool FindExtRefInMap(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
	                     const int &idxSeq, EXTREF_MAP::iterator &it_ExtRef);
	bool FindSMVCommnInMap(const unsigned short appID, SMV_COMMN_MAP::iterator &it_SMV);
	bool FindGSECommnInMap(const unsigned short appID, GSE_COMMN_MAP::iterator &it_GSE);
	bool FindLNTypeInMap(const SCL_STR &lnTypeID, LNTYPE_MAP::iterator &it_LNType);
	bool FindDOInLNType(const LNTYPE_MAP::iterator &it_LNType, const SCL_STR &doName, DO_MAP::iterator &it_DO);
	bool FindDOInMap(const SCL_STR &lnTypeID, const SCL_STR &doName, DO_MAP::iterator &it_DO);
	bool FindDOTypeInMap(const SCL_STR &doTypeID, DOTYPE_MAP::iterator &it_DOType);
	bool FindDAInDOType(const DOTYPE_MAP::iterator &it_DOType, const SCL_STR &daName, DA_MAP::iterator &it_DA);
	bool FindDAInMap(const SCL_STR &doTypeID, const SCL_STR &daName, DA_MAP::iterator &it_DA);
	bool FindDATypeInMap(const SCL_STR &daTypeID, DATYPE_MAP::iterator &it_DAType);
	bool FindBDAInDAType(const DATYPE_MAP::iterator &it_DAType, const SCL_STR & bdaName, BDA_MAP::iterator &it_BDA);
	bool FindBDAInMap(const SCL_STR &daTypeID, const SCL_STR & bdaName, BDA_MAP::iterator &it_BDA);

	void ClearFCDAMap(FCDA_MAP *fcdaMap);
	void ClearDataSetMap(DATASET_MAP *dataSetMap);
//	void ClearSMVCtrlMap(SMV_CTRL_MAP *smvCtrlMap);
//	void ClearGSECtrlMap(GSE_CTRL_MAP *gseCtrlMap);
//	void ClearDOIMap(DOI_MAP *doiMap);
	void ClearLN0Cache(LN0_INFO_CACHE *ln0);
	void ClearLNCache(LN_MAP *lnMap);
	void ClearLDeviceMap(LDEVICE_MAP *ldMap);
	void ClearIEDMap(IED_MAP *iedMap);
	void ClearSMVCommnMap(SMV_COMMN_MAP *smvCommnMap);
	void ClearGSECommnMap(GSE_COMMN_MAP *gseCommnMap);
//	void ClearDOMap(DO_MAP *doMap);
	void ClearLNTypeMap(LNTYPE_MAP *lnTypeMap);
//	void ClearDAMap(DA_MAP *daMap);
	void ClearDOTypeMap(DOTYPE_MAP *doTypeMap);
//	void ClearBDAMap(BDA_MAP *bdaMap);
	void ClearDATypeMap(DATYPE_MAP *daTypeMap);
	void ClearSCLMap(SCL_MAP *sclMap);

private:
	SCL_MAP *m_SCLMap;
};

#endif
