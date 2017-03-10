#ifndef SCL_TOOLS_H_

#define SCL_TOOLS_H_

#include "scltype.h"

class SCLTools
{
public:
	SCLTools();
	~SCLTools();

	///
	/// @brief
	///     加载SCL文件
	///
	bool LoadSCL(const SCL_CSTR_FILENAME &szFileName);

	///
	/// @brief
	///     复位函数
	///
	void ResetPos();
	void ResetMainPos();
	bool OutOfElem();
	bool IntoElem();

	///
	/// @brief
	///     定位函数
	///     在某一层级中定位之前确保已进入该层级，可用IntoElem（）
	///     在同一层级内定位时，切换到不同Tag的元素前需用ResetMainPos（）复位
	///
	bool FindSCL();

	bool FindIEDInSCL(SCL_STR iedName = _T(""));
	bool FindAccessPointInIED(SCL_STR apName = _T(""));
	bool FindServerInAccessPoint();
	bool FindLDeviceInServer(SCL_STR ldInst = _T(""));
	bool FindLDeviceInIED(const SCL_STR &ldInst);
	bool FindLN0InLDevice();
	bool FindLNInLDevice(const SCL_STR &lnPrefix, const SCL_STR &lnClass,
	                     const SCL_STR &lnInst);
	bool FindLNInLDevice(SCL_STR lnName = _T(""));
	bool FindDataSetInLN0(SCL_STR dataSetName = _T(""));
	bool FindSMVCtrlInLN0(SCL_STR cbName = _T(""));
	bool FindGSECtrlInLN0(SCL_STR cbName = _T(""));
	bool FindDOIInLNorLN0(SCL_STR doName = _T(""));
	bool FindInputsInLNorLN0();
	bool FindExtRefInInputs();
	bool FindDAIInDOI(SCL_STR daName = _T(""));

	bool FindCommnInSCL();
	bool FindSubNetworkInCommn(SCL_STR subNetworkName = _T(""));
	bool FindConnectedAPInSubNetwork(SCL_STR apName = _T(""), SCL_STR iedName = _T(""));
	bool FindAddressInConnnectedAP();
	bool FindSMVInConnectedAP(SCL_STR cbName = _T(""), SCL_STR ldInst = _T(""));
	bool FindGSEInConnectedAP(SCL_STR cbName = _T(""), SCL_STR ldInst = _T(""));

	bool FindDataTypeTmpltInSCL();
	bool FindLNTypeInDataTypeTmplt(SCL_STR id = _T(""));
	bool FindDOInLNType(SCL_STR doName = _T(""));
	bool FindDOTypeInDataTypeTmplt(SCL_STR id = _T(""));
	bool FindDAInDOType(SCL_STR daName = _T(""));
	bool FindDATypeInDataTypeTmplt(SCL_STR id = _T(""));
	bool FindBDAInDAType(SCL_STR name = _T(""));
	bool FindEnumTypeInDataTypeTmplt(SCL_STR id = _T(""));

	///
	/// @brief
	///     取值函数，取值前需先定位到该元素处
	///
	SCL_STR GetElemTag();
	bool GetIEDAttrib(SCL_IED_ATTRIB &iedAttrib);
	bool GetAccessPointAttrib(SCL_ACCESS_POINT_ATTRIB &apAttrib);
	bool GetServerAttrib(SCL_SERVER_ATTRIB &serverAttrib);
	bool GetLDeviceAttrib(SCL_LDEVICE_ATTRIB &ldAttrib);
	bool GetLN0Attrib(SCL_LN0_ATTRIB &ln0Attrib);
	bool GetLNAttrib(SCL_LN_ATTRIB &lnAttrib);
	bool GetDataSetAttrib(SCL_DATASET_ATTRIB &datSetAttrib);
	bool GetOneFCDAAttrib(SCL_FCDA_ATTRIB &fcdaAttrib, int idxSeq);
	bool GetDOIAttrib(SCL_DOI_ATTRIB &doiAttrib);
	bool GetDAIAttrib(SCL_DAI_ATTRIB &daiAttrib);
	bool GetExtRefAttrib(SCL_EXTREF_ATTRIB &extRefAttrib);
	bool GetSMVCtrlAttrib(SCL_SMVCTRL_ATTRIB &smvCtrlAttrib);
	bool GetGSECtrlAttrib(SCL_GSECTRL_ATTRIB &gseCtrlAttrib);

	bool GetSubNetworkAttrib(SCL_SUBNETWORK_ATTRIB &subNetAttrib);
	bool GetConnectedAPAttrib(SCL_CONNECTEDAP_ATTRIB &connectedAPAttrib);
	bool GetAddrAttrib(SCL_ADDR_ATTRIB &mmsAddr);
	bool GetSMVAttrib(SCL_SMV_ATTRIB &smvAttrib);
	bool GetGSEAttrib(SCL_GSE_ATTRIB &gseAttrib);

	bool GetLNTypeAttrib(SCL_LNTYPE_ATTRIB &lnTypeAttrib);
	bool GetDOAttrib(SCL_DOINLNTYPE_ATTRIB &doAttrib);
	bool GetDOTypeAttrib(SCL_DOTYPE_ATTRIB &doTypeAttrib);
	bool GetDAAttrib(SCL_DAINDOTYPE_ATTRIB &daAttrib);
	bool GetDATypeAttrib(SCL_DATYPE_ATTRIB &daTypeAttrib);
	bool GetBDAAttrib(SCL_BDAINDATYPE_ATTRIB &bdaAttrib);
	bool GetEnumValAttrib(SCL_ENUMVAL_ATTRIB &enumValAttrib, SCL_STR ord);

	///
	/// @brief
	///     在某一元素内添加子元素，添加前需先定位到该元素处
	///
	bool AddSCL();
	bool AddIEDIntoSCL(const SCL_IED_ATTRIB &attrib);
	bool AddAccessPointIntoIED(const SCL_ACCESS_POINT_ATTRIB &attrib);
	bool AddServerIntoAccessPoint();
	bool AddLDeviceIntoServer(const SCL_LDEVICE_ATTRIB &attrib);
	bool AddLN0IntoLDevice(const SCL_LN0_ATTRIB &attrib);
	bool AddLNIntoLDevice(const SCL_LN_ATTRIB &attrib);
	bool AddDataSetIntoLN0(const SCL_DATASET_ATTRIB &attrib);
	bool AddFCDAIntoDataSet(const SCL_FCDA_ATTRIB &attrib);
	bool AddSMVCtrlIntoLN0(const SCL_SMVCTRL_ATTRIB &attrib);
	bool AddGSECtrlIntoLN0(const SCL_GSECTRL_ATTRIB &attrib);
	bool AddDOIIntoLNorLN0(const SCL_DOI_ATTRIB &attrib);
	bool AddInputsIntoLNorLN0();
	bool AddExtRefIntoInputs(const SCL_EXTREF_ATTRIB &attrib);
	bool AddDAIIntoDOI(const SCL_DAI_ATTRIB &attrib);

	bool AddCommnIntoSCL();
	bool AddSubNetworkIntoCommn(const SCL_SUBNETWORK_ATTRIB &attrib);
	bool AddConnectedAPIntoSubNetwork(const SCL_CONNECTEDAP_ATTRIB &attrib);
	bool AddSMVIntoConnectedAP(const SCL_SMV_ATTRIB &attrib);
	bool AddGSEIntoConnectedAP(const SCL_GSE_ATTRIB &attrib);

	bool AddDataTypeTmpltIntoSCL();
	bool AddLNTypeIntoDataTypeTmplt(const SCL_LNTYPE_ATTRIB &attrib);
	bool AddDOIntoLNType(const SCL_DOINLNTYPE_ATTRIB &attrib);
	bool AddDOTypeIntoDataTypeTmplt(const SCL_DOTYPE_ATTRIB &attrib);
	bool AddDAIntoDOType(const SCL_DAINDOTYPE_ATTRIB &attrib);
	bool AddDATypeIntoDataTypeTmplt(const SCL_DATYPE_ATTRIB &attrib);
	bool AddBDAIntoDAType(const SCL_BDAINDATYPE_ATTRIB &attrib);
	bool AddEnumTypeIntoDataTypeTmplt(const SCL_ENUMVAL_ATTRIB &attrib);

	///
	/// @brief
	///     保存SCL文件
	///
	bool SaveSCL(const SCL_CSTR_FILENAME &szFileName);

protected:
	bool FindElemWithAttrib(const SCL_CSTR elemTag, const SCL_CSTR attribName,
		const SCL_STR attribVal);
	bool FindChildElemWithAttrib(const SCL_STR parentElem, const SCL_CSTR childElem,
		const SCL_CSTR attribName, const SCL_STR attribVal);

private:
	CMarkup m_SCLFile;
};

#endif
