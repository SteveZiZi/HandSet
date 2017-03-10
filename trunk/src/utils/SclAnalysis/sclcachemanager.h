#ifndef SCL_CACHE_MANAGER_H_

#define SCL_CACHE_MANAGER_H_

#include "scltools.h"
#include "sclcache.h"
#include "include/ISCLCacheManager.h"

class SCLCacheManager:public ISCLCacheManager
{
public:
	SCLCacheManager();
	virtual ~SCLCacheManager();

	virtual void EnableSmvAnalysis(bool bInputsOpt = false);
	virtual void DisableSmvAnalysis();
	virtual void EnableGseAnalysis(bool bInputsOpt = false);
	virtual void DisableGseAnalysis();

	virtual int  LoadSCL(const std::wstring &fileName);
	virtual void UnloadSCL(void);
	virtual int  SaveSCL(const std::wstring &fileName);
	
	SCLCache    *GetCache();

protected:
	bool IsMemoryEnough(const std::wstring &fileName);

	bool CacheAllDataSetInLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst);
	bool CacheAllSMVCtrlInLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst);
	bool CacheAllGSECtrlInLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst);
	bool CacheAllDOIIntoLNorLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName);
	bool CacheAllDOIFromFCDAIntoIED(SCLTools &scl, const SCL_STR &iedName);
	bool CacheAllDOIFromInputsIntoIED(SCLTools &scl, const SCL_STR &iedName);
	bool CacheInputsIntoLNorIN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName);
	bool CacheLN0InLDevice(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst);
	bool CacheLNInLDevice(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst);
	bool CacheAllLDeviceInAP(SCLTools &scl, const SCL_STR &iedName, const SCL_ACCESS_POINT_ATTRIB &apAttrib);
	bool CacheAllAccessPointInIED(SCLTools &scl, const SCL_STR &iedName);
	bool CacheAllIEDInFile(SCLTools &scl);
	bool CacheSMVCommnInSubNetwork(SCLTools &scl);
	bool CacheGSECommnInSubNetwork(SCLTools &scl);
	bool CacheCommnInfo(SCLTools &scl);
	bool CacheDOInLNType(SCLTools &scl,const SCL_STR &lnType);
	bool CacheLNType(SCLTools &scl);
	bool CacheDAInDOType(SCLTools &scl,const SCL_STR &doType);
	bool CacheDOType(SCLTools &scl);
	bool CacheBDAInDAType(SCLTools &scl,const SCL_STR &daType);
	bool CacheDAType(SCLTools &scl);
	bool CacheDataTypeTemplates(SCLTools &scl);

	bool WriteAllDataSetIntoLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst);
	bool WriteAllSMVCtrlIntoLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst);
	bool WriteAllGSECtrlIntoLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst);
	bool WriteAllDOIIntoLNorLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName);
	bool WriteInputsIntoLNorIN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst, const SCL_STR &lnName);
	bool WriteLN0IntoLDevice(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst);
	bool WriteLNIntoLDevice(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst);
	bool WriteAllLDeviceIntoIED(SCLTools &scl, const SCL_STR &iedName);
	bool WriteAllIEDIntoSCL(SCLTools &scl);
	bool WriteSMVCommnIntoSubNetwork(SCLTools &scl);
	bool WriteGSECommnIntoSubNetwork(SCLTools &scl);
	bool WriteCommnInfoIntoSCL(SCLTools &scl);
	bool WriteDOIntoLNType(SCLTools &scl,const SCL_STR &lnType);
	bool WriteLNTypeIntoDataTypeTemplates(SCLTools &scl);
	bool WriteDAIntoDOType(SCLTools &scl,const SCL_STR &doType);
	bool WriteDOTypeIntoDataTypeTemplates(SCLTools &scl);
	bool WriteBDAIntoDAType(SCLTools &scl,const SCL_STR &daType);
	bool WriteDATypeIntoDataTypeTemplates(SCLTools &scl);
	bool WriteDataTypeTemplatesIntoSCL(SCLTools &scl);

	bool IsHaveGseOrSmvOrInputsInLN0(SCLTools &scl, const SCL_STR &iedName, const SCL_STR &ldInst);
	void SplitExtRefIntAddr(const SCL_STR &intAddr,SCL_STR &ldInst,SCL_STR &lnName,SCL_STR &doName,SCL_STR &daName);

private:
	SCLCache *m_SclCache;
	bool m_bSmvCtrlOpt;
	bool m_bGseCtrlOpt;
	bool m_bSmvInputsOpt;
	bool m_bGseInputsOpt;
};

///
/// @brief
///     允许SMV信息解析，包括SMV Outputs和Inputs两部分
///
/// @param[in]
///     bInputsOpt - 是否解析Inputs信息
///
inline void SCLCacheManager::EnableSmvAnalysis(bool bInputsOpt /* = false */)
{
	m_bSmvCtrlOpt   = true;
	m_bSmvInputsOpt = bInputsOpt;
}

///
/// @brief
///     禁止SMV信息解析，包括SMV Outputs和Inputs两部分
///
inline void SCLCacheManager::DisableSmvAnalysis()
{
	m_bSmvCtrlOpt   = false;
	m_bSmvInputsOpt = false;
}

///
/// @brief
///     允许Goose信息解析，包括Goose Outputs和Inputs两部分
///
/// @param[in]
///     bInputsOpt - 是否解析Inputs信息
///
inline void SCLCacheManager::EnableGseAnalysis(bool bInputsOpt /* = false */)
{
	m_bGseCtrlOpt   = true;
	m_bGseInputsOpt = bInputsOpt;
}

///
/// @brief
///     禁止Goose信息解析，包括Goose Outputs和Inputs两部分
///
inline void SCLCacheManager::DisableGseAnalysis()
{
	m_bGseCtrlOpt   = false;
	m_bGseInputsOpt = false;
}

#endif
