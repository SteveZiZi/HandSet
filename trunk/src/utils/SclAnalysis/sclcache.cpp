/// @file
///     sclcache.cpp
///
/// @brief
///     SCL��������Cache��ʵ���ӿ�
///
/// @note
///     Ŀǰ����ʹ��map�Ļ�����
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
///	   �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                         |
///
#include "stdafx.h"
#include "sclcache.h"
#include "sclcachewithmap.h"

///
/// @brief
///     SCLCache�Ľӿڶ���ʵ��
///
/// @return
///     SCLCacheʵ��
///
SCLCache *CreateSCLCache(SCL_CACHE_TYPE type)
{
	switch(type)
	{
	case CACHE_WITH_MAP:
		{
			return new SCLCacheWithMap;
		}
		break;
	default:
		{
			return new SCLCacheWithMap;
		}
	}
}

///
/// @brief
///     �ͷ�SCLCache����ʵ��
///
/// @param[in]
///      sclCache - ָ��ӿڶ���ʵ����ָ��
///
/// @return
///     
///
void ReleaseSCLCache(SCLCache *sclCache)
{
	if ( sclCache != NULL )
	{
		delete sclCache;
	}
}