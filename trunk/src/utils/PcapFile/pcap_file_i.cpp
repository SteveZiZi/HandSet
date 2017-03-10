/// @file
///     pcap_file_i.h
///
/// @brief
///     PCAP�ļ������ӿڶ���ʵ���Ĵ������ͷź���ʵ��
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.2.27
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#include "stdafx.h"

#include "include/pcap_file_i.h"
#include "pcap_file.h"

///
/// @brief
///     ����PCAP�ļ������ӿڶ���ʵ��
///
/// @return
///     ָ��PCAP�ļ������ӿڶ���ʵ����ָ��
///
IPcapFile *CreatePcapFileImpl()
{
	return new PcapFile;
}

///
/// @brief
///     �ͷ�PCAP�ļ������ӿڶ���ʵ��
///
// @param[in]
///     fileImpl  - ָ����Ҫ�ͷŵ�ʵ����ָ��
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
