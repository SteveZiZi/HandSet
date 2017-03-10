#ifndef SCL_CACHE_H_

#define SCL_CACHE_H_

#include "scltype.h"

class SCLCache
{
public:
	virtual ~SCLCache(){};
	virtual void ClearCache() = 0;

	virtual bool AddIED(const SCL_IED_ATTRIB &iedAttrib) = 0;
	virtual bool AddLDeviceIntoIED(const SCL_STR &iedName, const SCL_ACCESS_POINT_ATTRIB &apAttrib, const SCL_LDEVICE_ATTRIB &ldAttrib) = 0;
	virtual bool AddLN0IntoLD(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_LN0_ATTRIB &ln0Attrib) = 0;
	virtual bool AddLNIntoLD(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_LN_ATTRIB &lnAttrib) = 0;
	virtual bool AddDataSetIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_DATASET_ATTRIB &datasetAttrib) = 0;
	virtual bool AddSMVCtrlIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_SMVCTRL_ATTRIB &smvAttrib) = 0;
	virtual bool AddGSECtrlIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_GSECTRL_ATTRIB &gseAttrib) = 0;
	virtual bool AddDOIIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_DOI_ATTRIB &doiAttrib) = 0;
	virtual bool AddDOIIntoLN(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_DOI_ATTRIB &doiAttrib) = 0;
	virtual bool AddExtRefIntoLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_EXTREF_ATTRIB &extRefAttrib) = 0;
	virtual bool AddExtRefIntoLN(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_EXTREF_ATTRIB &extRefAttrib) = 0;
	virtual bool AddFCDAIntoDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
	                               const SCL_STR &dataSetName, const SCL_FCDA_ATTRIB &fcdaAttrib) = 0;
	virtual bool AddSMVCommn(const SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, const SCL_SMV_ATTRIB &smvAttrib) = 0;
	virtual bool AddGSECommn(const SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, const SCL_GSE_ATTRIB &gseAttrib) = 0;
	virtual bool AddLNType(const SCL_LNTYPE_ATTRIB &lnTypeAttrib) = 0;
	virtual bool AddDOIntoLNType(const SCL_STR &lnTypeID, const SCL_DOINLNTYPE_ATTRIB &doAttrib) = 0;
	virtual bool AddDOType(const SCL_DOTYPE_ATTRIB &doTypeAttrib) = 0;
	virtual bool AddDAIntoDOType(const SCL_STR &doTypeID, const SCL_DAINDOTYPE_ATTRIB &daAttrib) = 0;
	virtual bool AddDAType(const SCL_DATYPE_ATTRIB &daTypeAttrib) = 0;
	virtual bool AddBDAIntoDAType(const SCL_STR &daTypeID, const SCL_BDAINDATYPE_ATTRIB &bdaAttrib) = 0;

	virtual int  GetIEDNum() = 0;
	virtual bool SetIEDAttrib(const SCL_STR &iedName, const SCL_IED_ATTRIB &iedAttrib) = 0;
	virtual bool SetIEDAttrib(const int idxSeq, const SCL_IED_ATTRIB &iedAttrib) = 0;
	virtual bool GetIEDAttrib(const SCL_STR &iedName, SCL_IED_ATTRIB &iedAttrib) = 0;
	virtual bool GetIEDAttrib(const int idxSeq, SCL_IED_ATTRIB &iedAttrib) = 0;

	virtual int  GetLDeviceNum(const SCL_STR &iedName) = 0;
	virtual bool SetLDeviceAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_ACCESS_POINT_ATTRIB &apAttrib, const SCL_LDEVICE_ATTRIB &ldAttrib) = 0;
	virtual bool SetLDeviceAttrib(const SCL_STR &iedName, const int idxSeq, const SCL_ACCESS_POINT_ATTRIB &apAttrib, const SCL_LDEVICE_ATTRIB &ldAttrib) = 0;
	virtual bool GetLDeviceAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, SCL_ACCESS_POINT_ATTRIB &apAttrib, SCL_LDEVICE_ATTRIB &ldAttrib) = 0;
	virtual bool GetLDeviceAttrib(const SCL_STR &iedName, const int idxSeq, SCL_ACCESS_POINT_ATTRIB &apAttrib, SCL_LDEVICE_ATTRIB &ldAttrib) = 0;

	virtual bool SetLN0Attrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_LN0_ATTRIB &ln0Attrib) = 0;
	virtual bool GetLN0Attrib(const SCL_STR &iedName, const SCL_STR &ldInst, SCL_LN0_ATTRIB &ln0Attrib) = 0;

	virtual int  GetLNNum(const SCL_STR &iedName, const SCL_STR &ldInst) = 0;
	virtual bool SetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_LN_ATTRIB &lnAttrib) = 0;
	virtual bool SetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_LN_ATTRIB &lnAttrib) = 0;
	virtual bool GetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, SCL_LN_ATTRIB &lnAttrib) = 0;
	virtual bool GetLNAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_LN_ATTRIB &lnAttrib) = 0;

	virtual int  GetDataSetNumInLN0(const SCL_STR &iedName, const SCL_STR &ldInst) = 0;
	virtual bool SetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, const SCL_DATASET_ATTRIB &dateSetAttrib) = 0;
	virtual bool SetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_DATASET_ATTRIB &dateSetAttrib) = 0;
	virtual bool GetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, SCL_DATASET_ATTRIB &dateSetAttrib) = 0;
	virtual bool GetDataSetAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_DATASET_ATTRIB &dateSetAttrib) = 0;
	
	virtual int  GetSMVCtrlNumInLN0(const SCL_STR &iedName, const SCL_STR &ldInst) = 0;
	virtual bool SetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_SMVCTRL_ATTRIB &smvCtrlAttrib) = 0;
	virtual bool SetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, const SCL_SMVCTRL_ATTRIB &smvCtrlAttrib) = 0;
	virtual bool SetSMVCtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, const SCL_SMVCTRL_ATTRIB &smvCtrlAttrib) = 0;
	virtual bool GetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_SMVCTRL_ATTRIB &smvCtrlAttrib) = 0;
	virtual bool GetSMVCtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, SCL_SMVCTRL_ATTRIB &smvCtrlAttrib) = 0;
	virtual bool GetSMVCtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, SCL_SMVCTRL_ATTRIB &smvCtrlAttrib) = 0;
	
	virtual int  GetGSECtrlNumInLN0(const SCL_STR &iedName, const SCL_STR &ldInst) = 0;
	virtual bool SetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, const SCL_GSECTRL_ATTRIB &gseCtrlAttrib) = 0;
	virtual bool SetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, const SCL_GSECTRL_ATTRIB &gseCtrlAttrib) = 0;
	virtual bool SetGSECtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, const SCL_GSECTRL_ATTRIB &gseCtrlAttrib) = 0;
	virtual bool GetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const int idxSeq, SCL_GSECTRL_ATTRIB &gseCtrlAttrib) = 0;
	virtual bool GetGSECtrlAttribInLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &cbName, SCL_GSECTRL_ATTRIB &gseCtrlAttrib) = 0;
	virtual bool GetGSECtrlAttribWithDatSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &dataSetName, SCL_GSECTRL_ATTRIB &gseCtrlAttrib) = 0;

	virtual int  GetDOINumInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName) = 0;
	virtual bool SetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const int idxSeq, const SCL_DOI_ATTRIB &doiAttrib) = 0;
	virtual bool SetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &doiName, const SCL_DOI_ATTRIB &doiAttrib) = 0;
	virtual bool GetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const int idxSeq, SCL_DOI_ATTRIB &doiAttrib) = 0;
	virtual bool GetDOIAttribInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &doiName, SCL_DOI_ATTRIB &doiAttrib) = 0;
	
	virtual int  GetInputsExtRefNumInIED(const SCL_STR &iedName) = 0;
	virtual bool GetAllInputsExtRefInIED(const SCL_STR &iedName, SCL_EXTREF_ATTRIB *extRefAttribArray, const int arrSize) = 0;
	virtual int  GetExtRefNumInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName) = 0;
	virtual bool GetExtRefAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
	                             const int &idxSeq, SCL_EXTREF_ATTRIB &extRefAttrib) = 0;
	virtual bool SetExtRefAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
		                         const int &idxSeq, const SCL_EXTREF_ATTRIB &extRefAttrib) = 0;
	
	virtual int  IsSMVAppIDExisted(const unsigned short appID) = 0;
	virtual int  GetSMVCommnNumInCache() = 0;	
	virtual bool GetAllSMVCommn(SCL_CONNECTEDAP_ATTRIB *cnnctAPAttribArray, SCL_SMV_ATTRIB *smvAttribArray, const int arraySize) = 0;
	virtual bool SetSMVCommnWithIdx(const SCL_STR iedName, const SCL_STR ldInst, const SCL_STR cbName, const SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, const SCL_SMV_ATTRIB &smvAddr) = 0;
	virtual bool GetSMVCommn(const unsigned short appID, SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, SCL_SMV_ATTRIB &smvAddr) = 0;
	virtual bool GetSMVCommnWithIdx(const SCL_STR iedName, const SCL_STR ldInst, const SCL_STR cbName, SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, SCL_SMV_ATTRIB &smvAddr) = 0;
	
	virtual int  IsGSEAppIDExisted(const unsigned short appID) = 0;
	virtual int  GetGSECommnNumInCache() = 0;
	virtual bool GetAllGSECommn(SCL_CONNECTEDAP_ATTRIB *cnnctAPAttribArray, SCL_GSE_ATTRIB *gseAttribArray, const int arraySize) = 0;
	virtual bool SetGSECommnWithIdx(const SCL_STR iedName, const SCL_STR ldInst, const SCL_STR cbName, const SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, const SCL_GSE_ATTRIB &gseAddr) = 0;
	virtual bool GetGSECommn(const unsigned short appID, SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, SCL_GSE_ATTRIB &gseAddr) = 0;
	virtual bool GetGSECommnWithIdx(const SCL_STR iedName, const SCL_STR ldInst, const SCL_STR cbName, SCL_CONNECTEDAP_ATTRIB &conntAPAttrib, SCL_GSE_ATTRIB &gseAddr) = 0;
	
	virtual bool SetFCDAAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &datSetName,
		                       const int idxSeq, const SCL_FCDA_ATTRIB &fcdaAttrib) = 0;
	virtual bool GetFCDAAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &datSetName,
	                           const int idxSeq, SCL_FCDA_ATTRIB &fcdaAttrib) = 0;
	virtual int  GetFCDAIdxSeqInDataSet(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_STR &datSetName,
	                                    const SCL_FCDA_ATTRIB &fcdaAttrib) = 0;
	virtual int  GetFCDAIdxSeqInLNorLN0(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName, const SCL_FCDA_ATTRIB &fcdaAttrib) = 0;
	
	virtual int  GetLNTypeNum() = 0;
	virtual bool SetLNType(const int idxSeq, const SCL_LNTYPE_ATTRIB &lnTypeAttrib) = 0;
	virtual bool SetLNType(const SCL_STR &lnTypeID, const SCL_LNTYPE_ATTRIB &lnTypeAttrib) = 0;
	virtual bool GetLNType(const int idxSeq, SCL_LNTYPE_ATTRIB &lnTypeAttrib) = 0;
	virtual bool GetLNType(const SCL_STR &lnTypeID, SCL_LNTYPE_ATTRIB &lnTypeAttrib) = 0;

	virtual int  GetDONumInLNType(const SCL_STR &lnType) = 0;
	virtual bool SetDOAttrib(const SCL_STR &lnType, const int idxSeq, const SCL_DOINLNTYPE_ATTRIB &doAttrib) = 0;
	virtual bool SetDOAttrib(const SCL_STR &lnType, const SCL_STR &doName, const SCL_DOINLNTYPE_ATTRIB &doAttrib) = 0;
	virtual bool GetDOAttrib(const SCL_STR &lnType, const int idxSeq, SCL_DOINLNTYPE_ATTRIB &doAttrib) = 0;
	virtual bool GetDOAttrib(const SCL_STR &lnType, const SCL_STR &doName, SCL_DOINLNTYPE_ATTRIB &doAttrib) = 0;

	virtual int  GetDOTypeNum() = 0;
	virtual bool SetDOType(const int idxSeq, const SCL_DOTYPE_ATTRIB &doTypeAttrib) = 0;
	virtual bool SetDOType(const SCL_STR &doTypeID, const SCL_DOTYPE_ATTRIB &doTypeAttrib) = 0;
	virtual bool GetDOType(const int idxSeq, SCL_DOTYPE_ATTRIB &doTypeAttrib) = 0;
	virtual bool GetDOType(const SCL_STR &doTypeID, SCL_DOTYPE_ATTRIB &doTypeAttrib) = 0;

	virtual int  GetDANumInDOType(const SCL_STR &doType) = 0;
	virtual bool SetDAAttrib(const SCL_STR &doType, const int idxSeq, const SCL_DAINDOTYPE_ATTRIB &daAttrib) = 0;
	virtual bool SetDAAttrib(const SCL_STR &doType, const SCL_STR &daName, const SCL_DAINDOTYPE_ATTRIB &daAttrib) = 0;
	virtual bool GetDAAttrib(const SCL_STR &doType, const int idxSeq, SCL_DAINDOTYPE_ATTRIB &daAttrib) = 0;
	virtual bool GetDAAttrib(const SCL_STR &doType, const SCL_STR &daName, SCL_DAINDOTYPE_ATTRIB &daAttrib) = 0;

	virtual int  GetDATypeNum() = 0;
	virtual bool SetDAType(const int idxSeq, const SCL_DATYPE_ATTRIB &daTypeAttrib) = 0;
	virtual bool SetDAType(const SCL_STR &daTypeID, const SCL_DATYPE_ATTRIB &daTypeAttrib) = 0;
	virtual bool GetDAType(const int idxSeq, SCL_DATYPE_ATTRIB &daTypeAttrib) = 0;
	virtual bool GetDAType(const SCL_STR &daTypeID, SCL_DATYPE_ATTRIB &daTypeAttrib) = 0;

	virtual int  GetBDANumInDAType(const SCL_STR &daType) = 0;
	virtual bool SetBDAAttrib(const SCL_STR &daType, const int idxSeq, const SCL_BDAINDATYPE_ATTRIB &bdaAttrib) = 0;
	virtual bool SetBDAAttrib(const SCL_STR &daType, const SCL_STR &bdaName, const SCL_BDAINDATYPE_ATTRIB &bdaAttrib) = 0;
	virtual bool GetBDAAttrib(const SCL_STR &daType, const int idxSeq, SCL_BDAINDATYPE_ATTRIB &bdaAttrib) = 0;
	virtual bool GetBDAAttrib(const SCL_STR &daType, const SCL_STR &bdaName, SCL_BDAINDATYPE_ATTRIB &bdaAttrib) = 0;

	virtual bool RemoveFCDAAttrib(const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName,
		                          const SCL_STR &datSetName, const int idxSeq) = 0;
	virtual bool RemoveDAAttrib(const SCL_STR &doTypeID, const int idxSeq) = 0;
	virtual bool RemoveBDAAttrib(const SCL_STR &daTypeID, const int idxSeq) = 0;
};

enum SCL_CACHE_TYPE {CACHE_WITH_MAP = 0,OTHERS};

SCLCache *CreateSCLCache(SCL_CACHE_TYPE type);

void ReleaseSCLCache(SCLCache *sclCache);

#endif

