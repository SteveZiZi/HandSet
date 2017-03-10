/// @file
///     pcap_file_i.h
///
/// @brief
///     PCAP�ļ������ӿڶ��󼰽ӿڶ���ʵ���Ĵ������ͷŽӿ�
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
///    |2013.06.26     |1.0.2.626  |���Pcap�ļ����ݻ�������С��д�ļ���ʱ���Ķ�ʱʱ������ü���ȡ�ӿ�|
///
#ifndef PCAP_FILE_INTERFACE_H_

#define PCAP_FILE_INTERFACE_H_

#include "pcap_types.h"
#include <string>

class IPcapFile
{
public:

	virtual ~IPcapFile(){};

	///
	/// @brief
	///     �����ļ����ݻ������Ĵ�С
	///
	/// @param[in]
	///     bufSize - �������Ĵ�С
	///
	/// @return
	///	    true - �ɹ��� false - ʧ��
	///
	virtual bool SetBufSize(const size_t bufSize) = 0;

	///
	/// @brief
	///     ��ȡ��������С
	///
	/// @return
	///     ��������С
	///
	virtual size_t GetBufSize() = 0;

	///
	/// @brief
	///     ���ý�����������д���ļ��Ķ�ʱ��
	///
	/// @return
	/// 
	///
	virtual void SetWriteTimer(uint32_t time) = 0;

	///
	/// @brief
	///     ��ȡ���ƻ���������д���ļ��Ķ�ʱ���Ķ�ʱʱ��
	///
	/// @return
	/// 
	///
	virtual uint32_t GetWriteTimer() = 0;

	///
	/// @brief
	///     ���ļ�
	///
	/// @param[in]
	///     path - �ļ�·��
	/// @param[in]
	///     mode - �ļ�����ģʽ
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT Open(
		std::wstring &path,
		PCAP_FILE_MODE mode) = 0;

	///
	/// @brief
	///     ����Ҫ��ȡ���ļ���λ��ָ��ָ���һ�����ݰ�����ģʽ��Ч
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT GotoFirstPckToRead() = 0;

	///
	/// @brief
	///     ����Ҫ��ȡ���ļ���λ��ָ��ָ����һ�����ݰ�����ģʽ��Ч
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT GotoNextPckToRead() = 0;

	///
	/// @brief
	///     ����Ҫ��ȡ���ļ���λ��ָ��ָ��ĳһ�ض�˳��ŵ����ݰ�������ģʽ��Ч
	///
	/// @param[in]
	///     idxSeq - ���ݰ����ļ��е�˳��ţ���0��ʼ
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT SeekPckToRead(const size_t idxSeq) = 0;

	///
	/// @brief
	///     ���ļ��ж�ȡ��ǰλ�õ����ݰ���ͷ����Ϣ,�����ı�λ��ָ�룬��ģʽ��Ч
	///
	/// @param[in]
	///     pckHeader - �������ݰ�ͷ����Ϣ�Ľṹ��,����ʱ��������ݰ����ݼ�������Ϣ
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT ReadOnePckHeader(PCAP_PACKET_HEADER &pckHeader) = 0;
	
	///
	/// @brief
	///     ���ļ��ж�ȡ��ǰλ�õ����ݰ��еı�����Ϣ�������ı�λ��ָ�룬��ģʽ��Ч
	///
	/// @param[in]
	///     pckDataArr - ���汨����Ϣ������
	/// @param[in]
	///     arrSize    - ��Ҫ��ȡ�ı���֡��ʵ�ʳ���
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT ReadOnePckData(uint8_t *pckDataArr, const size_t arrSize) = 0;
	
	///
	/// @brief
	///     ��ȡ�ļ������ݰ�����Ŀ��Ϣ
	///
	/// @return
	///     �ļ������ݰ�����Ŀ
	///
	virtual size_t GetPckNum() = 0;

	///
	/// @brief
	///     ��ȡPCAP�ļ�ͷ��Ϣ
	///
	/// @param[in]
	///     fileHeader - ����PCAP�ļ�ͷ��Ϣ�Ľṹ��
	///
	/// @return
	/// 
	///
	virtual void GetFileHeader(PCAP_FILE_HEADER &fileHeader) = 0;

	///
	/// @brief
	///     ��ʼ��PCAP�ļ�ͷ��дģʽ��Ч
	///
	/// @param[in]
	///     fh  - PCAP�ļ�ͷ
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT InitFileHeader(const PCAP_FILE_HEADER &fh) = 0;
	
	///
	/// @brief
	///     ���ļ���д����һ�����ݰ���Ϣ��дģʽ��Ч
	///
	// @param[in]
	///     timeStamp  - �����ݰ�������ʱ��ʱ�����Ϣ
	/// @param[in]
	///     pckDataArr - ���ݰ��������ı�����Ϣ
	// @param[in]
	///     arrSize    - ���ĵĳ���
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT WriteNextPck(
		PCAP_FILE_TIME_VAL &timeStamp,
		const uint8_t *pckDataArr,
		const uint32_t arrSize) = 0;

	///
	/// @brief
	///     ����д����Ϣ��дģʽ��Ч�������ر��ļ�
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT Save() = 0;
	
	///
	/// @brief
	///     �ر��ļ�
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT Close() = 0;
};

///
/// @brief
///     ����PCAP�ļ������ӿڶ���ʵ��
///
/// @return
///     ָ��PCAP�ļ������ӿڶ���ʵ����ָ��
///
IPcapFile *CreatePcapFileImpl();

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
void ReleasePcapFileImpl(IPcapFile *fileImpl);

#endif
