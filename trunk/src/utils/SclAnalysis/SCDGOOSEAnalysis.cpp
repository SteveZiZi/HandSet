/// @file
///     SCDGOOSEAnalysis.cpp
///
/// @brief
///     SCD�ļ�GOOSE���ý����ӿڽӿڶ���ʵ��
///
/// @note
///     
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.2.4
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
#include "sclgooseanalysis.h"

ISCDGOOSEAnalysis::ISCDGOOSEAnalysis()
{

}

ISCDGOOSEAnalysis::~ISCDGOOSEAnalysis()
{

}

///
/// @brief
///     SCD�ļ�GOOSE���ý����ӿڽӿڶ���ʵ��
///
///
/// @return
///     SCD�ļ�GOOSE���ý����ӿ�
///
ISCDGOOSEAnalysis *CreateSCDGOOSEAnalysisImpl( ISCLCacheManager *pSclCacheMgr )
{
	return new SCLGOOSEAnalysis(pSclCacheMgr);
}

///
/// @brief
///     �ͷ�SCD�ļ�GOOSE���ý����ӿڶ���ʵ��
///
/// @param[in]
///     objPtr - ָ��ӿڶ���ʵ����ָ��
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