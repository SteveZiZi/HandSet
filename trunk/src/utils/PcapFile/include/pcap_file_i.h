/// @file
///     pcap_file_i.h
///
/// @brief
///     PCAP文件操作接口对象及接口对象实例的创建与释放接口
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
///    |2013.06.26     |1.0.2.626  |添加Pcap文件数据缓存区大小和写文件定时器的定时时间的设置及获取接口|
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
	///     设置文件数据缓存区的大小
	///
	/// @param[in]
	///     bufSize - 缓存区的大小
	///
	/// @return
	///	    true - 成功； false - 失败
	///
	virtual bool SetBufSize(const size_t bufSize) = 0;

	///
	/// @brief
	///     获取缓存区大小
	///
	/// @return
	///     缓存区大小
	///
	virtual size_t GetBufSize() = 0;

	///
	/// @brief
	///     设置将缓存区数据写入文件的定时器
	///
	/// @return
	/// 
	///
	virtual void SetWriteTimer(uint32_t time) = 0;

	///
	/// @brief
	///     获取控制缓存区数据写入文件的定时器的定时时间
	///
	/// @return
	/// 
	///
	virtual uint32_t GetWriteTimer() = 0;

	///
	/// @brief
	///     打开文件
	///
	/// @param[in]
	///     path - 文件路径
	/// @param[in]
	///     mode - 文件访问模式
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT Open(
		std::wstring &path,
		PCAP_FILE_MODE mode) = 0;

	///
	/// @brief
	///     将所要读取的文件的位置指针指向第一个数据包，读模式有效
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT GotoFirstPckToRead() = 0;

	///
	/// @brief
	///     将所要读取的文件的位置指针指向下一个数据包，读模式有效
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT GotoNextPckToRead() = 0;

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
	virtual PCAP_FILE_RESULT SeekPckToRead(const size_t idxSeq) = 0;

	///
	/// @brief
	///     从文件中读取当前位置的数据包的头部信息,但不改变位置指针，读模式有效
	///
	/// @param[in]
	///     pckHeader - 保存数据包头的信息的结构体,包括时间戳、数据包数据及长度信息
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT ReadOnePckHeader(PCAP_PACKET_HEADER &pckHeader) = 0;
	
	///
	/// @brief
	///     从文件中读取当前位置的数据包中的报文信息，但不改变位置指针，读模式有效
	///
	/// @param[in]
	///     pckDataArr - 保存报文信息的数组
	/// @param[in]
	///     arrSize    - 所要获取的报文帧的实际长度
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT ReadOnePckData(uint8_t *pckDataArr, const size_t arrSize) = 0;
	
	///
	/// @brief
	///     获取文件中数据包的数目信息
	///
	/// @return
	///     文件中数据包的数目
	///
	virtual size_t GetPckNum() = 0;

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
	virtual void GetFileHeader(PCAP_FILE_HEADER &fileHeader) = 0;

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
	virtual PCAP_FILE_RESULT InitFileHeader(const PCAP_FILE_HEADER &fh) = 0;
	
	///
	/// @brief
	///     向文件中写入下一个数据包信息，写模式有效
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
		const uint32_t arrSize) = 0;

	///
	/// @brief
	///     保存写入信息，写模式有效，并不关闭文件
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT Save() = 0;
	
	///
	/// @brief
	///     关闭文件
	///
	/// @return
	///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
	///
	virtual PCAP_FILE_RESULT Close() = 0;
};

///
/// @brief
///     创建PCAP文件操作接口对象实例
///
/// @return
///     指向PCAP文件操作接口对象实例的指针
///
IPcapFile *CreatePcapFileImpl();

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
void ReleasePcapFileImpl(IPcapFile *fileImpl);

#endif
