/// @file
///     pcap_file_i.h
///
/// @brief
///     PCAP文件操作接口对象实例的创建和释放函数实现
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.2.27
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#include "stdafx.h"

#include "include/pcap_file_i.h"
#include "pcap_file.h"

///
/// @brief
///     创建PCAP文件操作接口对象实例
///
/// @return
///     指向PCAP文件操作接口对象实例的指针
///
IPcapFile *CreatePcapFileImpl()
{
	return new PcapFile;
}

///
/// @brief
///     释放PCAP文件操作接口对象实例
///
// @param[in]
///     fileImpl  - 指向所要释放的实例的指针
///
/// @return
///
///
void ReleasePcapFileImpl(IPcapFile *fileImpl)
{
	if ( fileImpl != NULL )
	{
		delete fileImpl;
		fileImpl = NULL;
	}
}
