/// @file
///     SCDSMV92Analysis.cpp
///
/// @brief
///     SCD�ļ�SMV92���ý����ӿڽӿڶ���ʵ��
///
/// @note
///     
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.1.24
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
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
///     SCD�ļ�SMV92���ý����ӿڽӿڶ���ʵ��
///
///
/// @return
///     SCD�ļ�SMV92���ý����ӿ�
///
ISCDSMV92Analysis *CreateSCDSMV92AnalysisImpl( ISCLCacheManager *pSclCacheMgr )
{
	return new SCLSmv92Analysis(pSclCacheMgr);
}

///
/// @brief
///     �ͷ�SCD�ļ�SMV92���ý����ӿڶ���ʵ��
///
/// @param[in]
///     objPtr - ָ��ӿڶ���ʵ����ָ��
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
