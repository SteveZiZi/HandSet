#ifndef PCAP_FILE_H_

#define PCAP_FILE_H_

#include <vector>
#include "include/pcap_file_i.h"
#include "file_buf.h"

class PcapFile:public IPcapFile
{
public:
	PcapFile();
	virtual ~PcapFile();

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
	virtual bool SetBufSize(const size_t bufSize);

	///
	/// @brief
	///     ��ȡ��������С
	///
	/// @return
	///     ��������С
	///
	virtual size_t GetBufSize();

	///
	/// @brief
	///     ���ý�����������д���ļ��Ķ�ʱ��
	///
	/// @return
	/// 
	///
	virtual void SetWriteTimer(uint32_t time);

	///
	/// @brief
	///     ��ȡ���ƻ���������д���ļ��Ķ�ʱ���Ķ�ʱʱ��
	///
	/// @return
	/// 
	///
	virtual uint32_t GetWriteTimer();

	///
	/// @brief
	///     ���ļ�
	///
	/// @param[in]
	///     path - �ļ�·��
	/// @param[in]
	///     mode - �ļ�ģʽ
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT Open(
		std::wstring &path,
		PCAP_FILE_MODE mode);

	///
	/// @brief
	///     ����Ҫ��ȡ���ļ���λ��ָ��ָ���һ�����ݰ�����ģʽ��Ч
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT GotoFirstPckToRead();

	///
	/// @brief
	///     ����Ҫ��ȡ���ļ���λ��ָ��ָ����һ�����ݰ�����ģʽ��Ч
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT GotoNextPckToRead();

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
	virtual PCAP_FILE_RESULT SeekPckToRead(const size_t idxSeq);

	///
	/// @brief
	///     ���ļ��ж�ȡ��ǰλ�õ����ݰ���ͷ����Ϣ����ģʽ��Ч
	///
	/// @param[in]
	///     pckHeader - �������ݰ�ͷ����Ϣ�Ľṹ��,����ʱ��������ݰ����ݼ�������Ϣ
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT ReadOnePckHeader(PCAP_PACKET_HEADER &pckHeader);

	///
	/// @brief
	///     ���ļ��ж�ȡ��ǰλ�õ����ݰ��еı�����Ϣ����ģʽ��Ч
	///
	/// @param[in]
	///     pckDataArr - ���汨����Ϣ������
	/// @param[in]
	///     arrSize    - �����С
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT ReadOnePckData(uint8_t *pckDataArr, const size_t arrSize);

	///
	/// @brief
	///     ��ȡ�ļ������ݰ�����Ŀ��Ϣ
	///
	/// @return
	///     �ļ������ݰ�����Ŀ
	///
	virtual size_t GetPckNum();

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
	virtual void GetFileHeader(PCAP_FILE_HEADER &fileHeader);

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
	virtual PCAP_FILE_RESULT InitFileHeader(const PCAP_FILE_HEADER &fh);

	///
	/// @brief
	///     �����ļ���д��һ�����ݰ���Ϣ��дģʽ��Ч
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
		const uint32_t arrSize);

	///
	/// @brief
	///     ����д����Ϣ��дģʽ��Ч
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT Save();

	///
	/// @brief
	///     �ر��ļ�
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	virtual PCAP_FILE_RESULT Close();

protected:
	///
	/// @brief
	///     �����ļ�ͷ��Ϣ����ģʽ��Ч
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	PCAP_FILE_RESULT UpdateFileHeader();

	///
	/// @brief
	///     �������ݰ���ַӳ�����ģʽ��Ч
	///
	/// @return
	///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
	///
	PCAP_FILE_RESULT UpdatePckTab();

	///
	/// @brief
	///     �ж��ļ��Ƿ����PCAP�ļ���ʽ
	///
	/// @return
	///     true - ���ϣ� false - ������
	///
	bool IsPcapFile();

	///
	/// @brief
	///     PCAP�ļ�ͷ���ݵĴ�С��ת��
	///
	/// @return
	/// 
	///
	void Bswap_FileHeader(PCAP_FILE_HEADER &fh);

	///
	/// @brief
	///     PCAP�ļ���Packetͷ���ݵĴ�С��ת��
	///
	/// @return
	/// 
	///
	void Bswap_PckHeader(PCAP_PACKET_HEADER &ph);

private:
	FILE               *m_PcapFile;
	FileBuf            *m_PcapBuf;
	PCAP_FILE_MODE      m_FileMode;
	PCAP_FILE_HEADER    m_FileHeader;
	bool                m_IsDifEndian;
	std::vector<size_t> m_PcapPckTbl;
};

#endif
