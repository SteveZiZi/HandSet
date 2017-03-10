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
	uint8_t  *m_Buffer;                ///< 指向缓存区的指针
	size_t    m_BufSize;               ///< 缓存区大小
	size_t    m_BufEnd;                ///< 缓存区最后一个有效数据的下一个地址
	size_t    m_BufOff;                ///< 最近一次缓存区操作（一般是读操作）的数据的下一个地址
	size_t    m_ReadFileOffAddr;       ///< 所读取的文件的当前的文件位置地址
	size_t    m_WriteFileOffAddr;      ///< 所写入的文件的当前的文件位置地址
	FILE     *m_ConnectingFile;        ///< 与缓存区相关联的文件的文件指针
	PCAP_FILE_MODE m_FileMode;         ///< 文件的打开模式
	uint32_t  m_LastWriteTime;         ///< 最近一次写文件的时间
	uint32_t  m_WriteTimer;            ///< 控制缓存区数据写入文件的定时器的定时时间
	bool      m_bReadWriteIng;         ///< 标志当前是否正在进行读写操作
};

#endif
