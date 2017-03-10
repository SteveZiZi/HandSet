#ifndef FILE_BUF_H_

#define FILE_BUF_H_

#include "include/pcap_types.h"

class FileBuf
{
public:
	FileBuf();
	~FileBuf();

	bool SetBufSize(const size_t bufSize);
	size_t GetBufSize();

	void SetWriteTimer(uint32_t time);
	uint32_t GetWriteTimer();

	void Reset();

	void ConnectToFile(FILE *file, PCAP_FILE_MODE mode);

	int ReadFileToBuf(const size_t fileOffAddr);
	int GetDataFromBuf(uint8_t *datArr, const size_t arrSize);
	int UpFileOffAddr(const size_t offset);
	int DownFileOffAddr(const size_t offset);
	size_t GetReadFileOffAddr();

	int AddDataToBuf(const uint8_t *datArr, const size_t arrSize);
	int WriteBufToFile();
	size_t GetWriteFileOffAddr();

	void ClearBuf();

protected:
	int GetIOResult(size_t rwReturn, size_t rwSize);

private:
	uint8_t  *m_Buffer;                ///< ָ�򻺴�����ָ��
	size_t    m_BufSize;               ///< ��������С
	size_t    m_BufEnd;                ///< ���������һ����Ч���ݵ���һ����ַ
	size_t    m_BufOff;                ///< ���һ�λ�����������һ���Ƕ������������ݵ���һ����ַ
	size_t    m_ReadFileOffAddr;       ///< ����ȡ���ļ��ĵ�ǰ���ļ�λ�õ�ַ
	size_t    m_WriteFileOffAddr;      ///< ��д����ļ��ĵ�ǰ���ļ�λ�õ�ַ
	FILE     *m_ConnectingFile;        ///< �뻺������������ļ����ļ�ָ��
	PCAP_FILE_MODE m_FileMode;         ///< �ļ��Ĵ�ģʽ
	uint32_t  m_LastWriteTime;         ///< ���һ��д�ļ���ʱ��
	uint32_t  m_WriteTimer;            ///< ���ƻ���������д���ļ��Ķ�ʱ���Ķ�ʱʱ��
	bool      m_bReadWriteIng;         ///< ��־��ǰ�Ƿ����ڽ��ж�д����
};

#endif
