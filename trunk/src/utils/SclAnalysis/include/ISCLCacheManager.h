#ifndef SCL_REDUCER_H_

#define SCL_REDUCER_H_

#include <string>
#include "sclanalysis.h"

// WinCE下默认使用对可用内存是否满足SCL文件解析的检查
#ifdef WINCE
#ifndef MEMORY_ENOUGH_CHECK
#define MEMORY_ENOUGH_CHECK
#endif // MEMORY_ENOUGH_CHECK
#endif // WINCE

class ISCLCacheManager
{
public:
	ISCLCacheManager();
	virtual ~ISCLCacheManager();

	///
	/// @brief
	///     允许SMV信息解析，包括SMV Outputs和Inputs两部分
	///
	/// @param[in]
	///     bInputsOpt - 是否解析Inputs信息
	///
	virtual void EnableSmvAnalysis(bool bInputsOpt = false) = 0;

	///
	/// @brief
	///     禁止SMV信息解析，包括SMV Outputs和Inputs两部分
	///
	virtual void DisableSmvAnalysis() = 0;

	///
	/// @brief
	///     允许Goose信息解析，包括Goose Outputs和Inputs两部分
	///
	/// @param[in]
	///     bInputsOpt - 是否解析Inputs信息
	///
	virtual void EnableGseAnalysis(bool bInputsOpt = false) = 0;

	///
	/// @brief
	///     禁止Goose信息解析，包括Goose Outputs和Inputs两部分
	///
	virtual void DisableGseAnalysis() = 0;

	///
	/// @brief
	///     加载SCL配置文件，并进行相应的解析
	///
	/// @param[in]
	///     strFile - SCL配置文件名(带路径)
	///
	/// @return
	///     0 - 操作成功； -1 - 操作失败
	///
	virtual int LoadSCL( const std::wstring &strFile ) = 0;

	///
	/// @brief
	///     卸载打开的SCL配置文件，释放解析缓存
	///
	virtual void UnloadSCL(void) = 0;

	///
	/// @brief
	///     以SCL文件格式，保存解析缓存
	///
	/// @param[in]
	///     strFile - 要保存到的SCL文件名(带路径)
	///
	/// @return
	///     0 - 操作成功； -1 - 操作失败
	///
	virtual int SaveSCL( const std::wstring &strFile ) = 0;
};

///
/// @brief
///     SCL文件简化器对象实例
///
/// @return
///     SCL文件简化器对象实例指针
///
SCLANALYSISDLL_API ISCLCacheManager *CreateSCLCacheManagerImpl( void );

///
/// @brief
///     释放SCL文件简化器对象实例
///
/// @param[in]
///      objPtr - 指向SCL文件简化器对象实例的指针
///
/// @return
///     
///
SCLANALYSISDLL_API void  RelaseSCLCacheManagerImpl(ISCLCacheManager *objPtr);

#endif
