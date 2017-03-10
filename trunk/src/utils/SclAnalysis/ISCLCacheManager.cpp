#include "stdafx.h"
#include "include/ISCLCacheManager.h"
#include "sclcachemanager.h"

ISCLCacheManager::ISCLCacheManager()
{

}


ISCLCacheManager::~ISCLCacheManager()
{

}

///
/// @brief
///     SCL文件简化器对象实例
///
/// @return
///     SCL文件简化器对象实例指针
///
SCLANALYSISDLL_API ISCLCacheManager *CreateSCLCacheManagerImpl( void )
{
	return new SCLCacheManager;
}

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
SCLANALYSISDLL_API void  RelaseSCLCacheManagerImpl(ISCLCacheManager *objPtr)
{
	if ( objPtr != NULL )
	{
		delete objPtr;
		objPtr = NULL;
	}
}
