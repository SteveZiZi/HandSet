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
	///     设置文件数据缓存区的大小
	///
	/// @param[in]
	///     bufSize - 缓存区的大小
	///
	/// @return
	///	    true - 成功； false - 失败
	///
	virtual bool SetBufSize(const size_t bufSize);

	///
	/// @brief
	///     获取缓存区大小
	///
	/// @return
	///     缓存区大小
	///
	virtual size_t GetBufSize();

	///
	/// @brief
	///     设置将缓存区数据写入文件的定时器
	///
	/// @return
	/// 
	///
	virtual void SetWriteTimer(uint32_t time);

	///
	/// @brief
	///     获取控制缓存区数据写入文件的定时器的定时时间
	///
	/// @return
	/// 
	///
	virtual uint32_t GetWriteTimer();

	///
	/// @brief
	///     打开文件
	///
	/// @param[in]
	///     path - 文件路径
	/// @param[in]
	///     mode - 文件模式
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT Open(
		std::wstring &path,
		PCAP_FILE_MODE mode);

	///
	/// @brief
	///     将所要读取的文件的位置指针指向第一个数据包，读模式有效
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT GotoFirstPckToRead();

	///
	/// @brief
	///     将所要读取的文件的位置指针指向下一个数据包，读模式有效
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT GotoNextPckToRead();

	///
	/// @brief
	///     将所要读取的文件的位置指针指向某一特定顺序号的数据包处，读模式有效
	///
	/// @param[in]
	///     idxSeq - 数据包在文件中的顺序号，从0开始
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT SeekPckToRead(const size_t idxSeq);

	///
	/// @brief
	///     从文件中读取当前位置的数据包的头部信息，读模式有效
	///
	/// @param[in]
	///     pckHeader - 保存数据包头的信息的结构体,包括时间戳、数据包数据及长度信息
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT ReadOnePckHeader(PCAP_PACKET_HEADER &pckHeader);

	///
	/// @brief
	///     从文件中读取当前位置的数据包中的报文信息，读模式有效
	///
	/// @param[in]
	///     pckDataArr - 保存报文信息的数组
	/// @param[in]
	///     arrSize    - 数组大小
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT ReadOnePckData(uint8_t *pckDataArr, const size_t arrSize);

	///
	/// @brief
	///     获取文件中数据包的数目信息
	///
	/// @return
	///     文件中数据包的数目
	///
	virtual size_t GetPckNum();

	///
	/// @brief
	///     获取PCAP文件头信息
	///
	/// @param[in]
	///     fileHeader - 保存PCAP文件头信息的结构体
	///
	/// @return
	///
	///
	virtual void GetFileHeader(PCAP_FILE_HEADER &fileHeader);

	///
	/// @brief
	///     初始化PCAP文件头，写模式有效
	///
	/// @param[in]
	///     fh  - PCAP文件头
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT InitFileHeader(const PCAP_FILE_HEADER &fh);

	///
	/// @brief
	///     向下文件中写入一个数据包信息，写模式有效
	///
	// @param[in]
	///     timeStamp  - 该数据包被捕获时的时间戳信息
	/// @param[in]
	///     pckDataArr - 数据包所包含的报文信息
	// @param[in]
	///     arrSize    - 报文的长度
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT WriteNextPck(
		PCAP_FILE_TIME_VAL &timeStamp,
		const uint8_t *pckDataArr,
		const uint32_t arrSize);

	///
	/// @brief
	///     保存写入信息，写模式有效
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT Save();

	///
	/// @brief
	///     关闭文件
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT Close();

protected:
	///
	/// @brief
	///     更新文件头信息，读模式有效
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	PCAP_FILE_RESULT UpdateFileHeader();

	///
	/// @brief
	///     更新数据包地址映射表，读模式有效
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	PCAP_FILE_RESULT UpdatePckTab();

	///
	/// @brief
	///     判断文件是否符合PCAP文件格式
	///
	/// @return
	///     true - 符合； false - 不符合
	///
	bool IsPcapFile();

	///
	/// @brief
	///     PCAP文件头数据的大小端转换
	///
	/// @return
	/// 
	///
	void Bswap_FileHeader(PCAP_FILE_HEADER &fh);

	///
	/// @brief
	///     PCAP文件的Packet头数据的大小端转换
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
