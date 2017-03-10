/// @file
///     SCDSMV92Analysis.cpp
///
/// @brief
///     SCD文件SMV92配置解析接口接口对象实例
///
/// @note
///     
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.1.24
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
#include "sclsmv92analysis.h"

ISCDSMV92Analysis::ISCDSMV92Analysis()
{

}

ISCDSMV92Analysis::~ISCDSMV92Analysis()
{

}

///
/// @brief
///     SCD文件SMV92配置解析接口接口对象实例
///
///
/// @return
///     SCD文件SMV92配置解析接口
///
ISCDSMV92Analysis *CreateSCDSMV92AnalysisImpl( ISCLCacheManager *pSclCacheMgr )
{
	return new SCLSmv92Analysis(pSclCacheMgr);
}

///
/// @brief
///     释放SCD文件SMV92配置解析接口对象实例
///
/// @param[in]
///     objPtr - 指向接口对象实例的指针
///
/// @return
///     
///
void  RelaseSCDSMV92AnalysisImpl(ISCDSMV92Analysis *objPtr)
{
	if ( objPtr != NULL )
	{
		delete objPtr;
	}
}
