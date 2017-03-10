/// @file
///     SCDGOOSEAnalysis.cpp
///
/// @brief
///     SCD文件GOOSE配置解析接口接口对象实例
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
///    |               |           |                                                         |
///
#include "stdafx.h"
#include "sclgooseanalysis.h"

ISCDGOOSEAnalysis::ISCDGOOSEAnalysis()
{

}

ISCDGOOSEAnalysis::~ISCDGOOSEAnalysis()
{

}

///
/// @brief
///     SCD文件GOOSE配置解析接口接口对象实例
///
///
/// @return
///     SCD文件GOOSE配置解析接口
///
ISCDGOOSEAnalysis *CreateSCDGOOSEAnalysisImpl( ISCLCacheManager *pSclCacheMgr )
{
	return new SCLGOOSEAnalysis(pSclCacheMgr);
}

///
/// @brief
///     释放SCD文件GOOSE配置解析接口对象实例
///
/// @param[in]
///     objPtr - 指向接口对象实例的指针
///
/// @return
///     
///
void  RelaseSCDGOOSEAnalysisImpl(ISCDGOOSEAnalysis *objPtr)
{
	if ( objPtr != NULL )
	{
		delete objPtr;
	}
}