/// @file
///     pcap_file_i.h
///
/// @brief
///     PCAP文件操作接口对象的实现
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
///    |2013.03.05     |1.0.1.305  |添加对PCAP文件数据大小端存储的判断以及相应数据的转换     |
///    |2013.03.05     |1.0.1.305  |添加对文件是否符合PCAP文件格式的判断                     |
///    |2013.03.05     |1.0.1.305  |将对缓存区剩余数据的写入从Close()中分离，单独作为Save接口|
///    |2013.03.05     |1.0.1.305  |添加GetDataFromBuf对memcpy数据时的判断                   |
///    |2013.06.28     |1.0.2.628  |修正判断PCAP文件与主机的大小端存储方式前没有复位的Bug    |
///    |2013.08.19     |1.0.3.819  |修正更新数据包地址映射表时没有判断最后一个数据包的完整性的Bug|
///    |2013.11.12     |1.0.3.1112 |修正关闭Pcap文件时没有释放数据包地址映射表资源的bug      |
///
#include "stdafx.h"

#include <stdio.h>
#include "pcap_file.h"

#ifndef _T
#include <tchar.h>
#endif

///
/// @brief
///     16位数的大小端转换
///
#define BSWAP_16(x) (uint16_t)( \
	                ((*(uint16_t*)(&x) & 0xff00) >> 8) | \
	                ((*(uint16_t*)(&x) & 0x00ff) << 8) \
	                )

///
/// @brief
///     32位数的大小端转换
///
#define BSWAP_32(x) (uint32_t)( \
	                (((uint32_t)(x) & 0xff000000) >> 24) | \
                    (((uint32_t)(x) & 0x00ff0000) >> 8 ) | \
					(((uint32_t)(x) & 0x0000ff00) << 8 ) | \
					(((uint32_t)(x) & 0x000000ff) << 24) \
                    )

///
/// @brief
///     PCAP文件头的默认值
///
const PCAP_FILE_HEADER g_Default_FH = {
	PCAP_DEFAULT_MAGIC,
	PCAP_DEFAULT_MAJOR_VERSION,
	PCAP_DEFAULT_MINOR_VERSION,
	PCAP_DEFAULT_THIS_ZONE,
	PCAP_DEFAULT_SIGFIGS,
	PCAP_DEFAULT_SANPLEN,
	PCAP_LINK_TYPE_ETHERNET};

PcapFile::PcapFile()
: m_PcapFile(NULL)
, m_FileMode(PFM_READ)
, m_FileHeader(g_Default_FH)
, m_IsDifEndian(false)
{
	m_PcapBuf = new FileBuf;
	m_PcapPckTbl.reserve(10000);
}

PcapFile::~PcapFile()
{
	if ( m_PcapBuf != NULL )
	{
		delete m_PcapBuf;
		m_PcapBuf = NULL;
	}
}

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
bool PcapFile::SetBufSize(const size_t bufSize)
{
	return m_PcapBuf->SetBufSize(bufSize);
}

///
/// @brief
///     获取缓存区大小
///
/// @return
///     缓存区大小
///
size_t PcapFile::GetBufSize()
{
	return m_PcapBuf->GetBufSize();
}

///
/// @brief
///     设置将缓存区数据写入文件的定时器
///
/// @return
/// 
///
void PcapFile::SetWriteTimer(uint32_t time)
{
	m_PcapBuf->SetWriteTimer(time);
}

///
/// @brief
///     获取控制缓存区数据写入文件的定时器的定时时间
///
/// @return
/// 
///
uint32_t PcapFile::GetWriteTimer()
{
	return m_PcapBuf->GetWriteTimer();
}

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
PCAP_FILE_RESULT PcapFile::Open(std::wstring &path, PCAP_FILE_MODE mode)
{
	// 文件未关闭，返回错误
	if ( m_PcapFile != NULL )
		return PCAP_FILE_ERROR;

	switch (mode)
	{
	case PFM_READ:
			// 只读方式打开文件
			m_PcapFile = _wfopen(path.c_str(), _T("rb"));
		break;
	case PFM_WRITE:
			// 只写方式打开文件
			m_PcapFile = _wfopen(path.c_str(), _T("wb"));
		break;
	default:
		goto errorHandle;
	}

	if (m_PcapFile == NULL)
		goto errorHandle;
	m_FileMode = mode;

	// 连接文件缓存区并初始化
	m_PcapBuf->ConnectToFile(m_PcapFile,mode);
	// 更新文件头信息
	if ( UpdateFileHeader() != PCAP_FILE_SUCCESS )
		goto errorHandle;
	// 判断是否符合PCAP文件格式
	if ( !IsPcapFile() )
		goto errorHandle;
	// 更新数据包地址映射表
	if ( UpdatePckTab() == PCAP_FILE_ERROR )
		goto errorHandle;

	return PCAP_FILE_SUCCESS;

errorHandle:
	if ( m_PcapFile != NULL )
	{
		Close();
	}
	return PCAP_FILE_ERROR;
}

///
/// @brief
///     将所要读取的文件的位置指针指向第一个数据包，读模式有效
///
/// @return
///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
///
PCAP_FILE_RESULT PcapFile::GotoFirstPckToRead()
{
	size_t curOffAddr = m_PcapBuf->GetReadFileOffAddr();

	// 目标位置大于当前读位置，向下移动至目标位置
	// 否则，刷新缓存区
	if ( curOffAddr <= sizeof(PCAP_FILE_HEADER) )
		return m_PcapBuf->DownFileOffAddr(sizeof(PCAP_FILE_HEADER)-curOffAddr);
	else
		return m_PcapBuf->ReadFileToBuf(sizeof(PCAP_FILE_HEADER));
}

///
/// @brief
///     将所要读取的文件的位置指针指向下一个数据包，读模式有效
///
/// @return
///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
///
PCAP_FILE_RESULT PcapFile::GotoNextPckToRead()
{
	PCAP_PACKET_HEADER pckHeader;
	int bufRS;
	// 获取Packet头信息
	bufRS =  m_PcapBuf->GetDataFromBuf((uint8_t *)&pckHeader,sizeof(PCAP_PACKET_HEADER));
	if ( bufRS != PCAP_FILE_SUCCESS )
		return bufRS;
	// 大小端数据转换
	if ( m_IsDifEndian )
		Bswap_PckHeader(pckHeader);
	bufRS = m_PcapBuf->DownFileOffAddr(sizeof(PCAP_PACKET_HEADER)+pckHeader.capLen);
	return bufRS;
}

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
PCAP_FILE_RESULT PcapFile::SeekPckToRead(const size_t idxSeq)
{
	if ( idxSeq >= m_PcapPckTbl.size() )
		return PCAP_FILE_ERROR;

	size_t offAddr    = m_PcapPckTbl.at(idxSeq);
	size_t curOffAddr = m_PcapBuf->GetReadFileOffAddr();

	// 目标位置大于当前读位置，向下移动至目标位置
	// 否则，刷新缓存区
	if ( offAddr > curOffAddr )
		return m_PcapBuf->DownFileOffAddr(offAddr-curOffAddr);
	else
		return m_PcapBuf->UpFileOffAddr(curOffAddr-offAddr);
}

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
PCAP_FILE_RESULT PcapFile::ReadOnePckHeader(PCAP_PACKET_HEADER &pckHeader)
{
	int bufRS;
	// 获取Packet头信息
	bufRS =  m_PcapBuf->GetDataFromBuf((uint8_t *)&pckHeader,sizeof(PCAP_PACKET_HEADER));
	if ( bufRS != PCAP_FILE_SUCCESS )
		return bufRS;
	// 大小端数据转换
	if ( m_IsDifEndian )
		Bswap_PckHeader(pckHeader);
	return PCAP_FILE_SUCCESS;
}

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
PCAP_FILE_RESULT PcapFile::ReadOnePckData(uint8_t *pckDataArr, const size_t arrSize)
{
	uint8_t *p = new uint8_t[sizeof(PCAP_PACKET_HEADER)+arrSize];
	int bufRS = m_PcapBuf->GetDataFromBuf(p,sizeof(PCAP_PACKET_HEADER)+arrSize);
	if ( bufRS == PCAP_FILE_SUCCESS )
	{
		memcpy(pckDataArr,&p[sizeof(PCAP_PACKET_HEADER)],arrSize);
	}
	delete[] p;
	return bufRS;
}

///
/// @brief
///     获取文件中数据包的数目信息
///
/// @return
///     文件中数据包的数目
///
size_t PcapFile::GetPckNum()
{
	return m_PcapPckTbl.size();
}

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
void PcapFile::GetFileHeader(PCAP_FILE_HEADER &fileHeader)
{
	fileHeader = m_FileHeader;
}

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
PCAP_FILE_RESULT PcapFile::InitFileHeader(const PCAP_FILE_HEADER &fh)
{
	if ( ftell(m_PcapFile) > 0 || m_FileMode != PFM_WRITE )
		return PCAP_FILE_ERROR;
	m_FileHeader = fh;
	return PCAP_FILE_SUCCESS;
}

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
PCAP_FILE_RESULT PcapFile::WriteNextPck(PCAP_FILE_TIME_VAL &timeStamp, const uint8_t *pckDataArr, const uint32_t arrSize)
{
	if ( m_FileMode != PFM_WRITE )
		return PCAP_FILE_ERROR;

	// 填充文件头
	int wResult;
	if ( m_PcapBuf->GetWriteFileOffAddr() == 0 )
	{
		wResult = m_PcapBuf->AddDataToBuf((uint8_t *)&m_FileHeader,sizeof(PCAP_FILE_HEADER));
		if ( wResult == PCAP_FILE_ERROR )
			return PCAP_FILE_ERROR;
	}

	size_t idx = m_PcapBuf->GetWriteFileOffAddr();

	// 配置数据包头
	PCAP_PACKET_HEADER pckHeader;
	if ( m_FileHeader.snapLen == 0xFFFF || arrSize < m_FileHeader.snapLen )
	{
		pckHeader.capLen = pckHeader.len = arrSize;
	}
	else
	{
		pckHeader.capLen = m_FileHeader.snapLen;
		pckHeader.len    = arrSize;
	}
	pckHeader.timeStamp = timeStamp;

	// 写入数据包信息
	wResult = m_PcapBuf->AddDataToBuf((uint8_t *)&pckHeader,sizeof(PCAP_PACKET_HEADER));
	if ( wResult == PCAP_FILE_ERROR )
		return PCAP_FILE_ERROR;
	wResult = m_PcapBuf->AddDataToBuf(pckDataArr,pckHeader.capLen);
	if ( wResult == PCAP_FILE_ERROR )
		return PCAP_FILE_ERROR;

	// 添加数据包地址
	m_PcapPckTbl.push_back(idx);
	return wResult;
}

///
/// @brief
///     保存写入信息，写模式有效
///
/// @return
///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
///
PCAP_FILE_RESULT PcapFile::Save()
{
	if ( m_FileMode == PFM_WRITE )
	{
		// 写模式下，将缓存区数据写入文件
		return m_PcapBuf->WriteBufToFile();
	}
	return PCAP_FILE_SUCCESS;
}

///
/// @brief
///     关闭文件
///
/// @return
///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
///
PCAP_FILE_RESULT PcapFile::Close()
{
	// 关闭文件
	if ( m_PcapFile != NULL )
	{
		if ( fclose(m_PcapFile) != 0 )
			return PCAP_FILE_ERROR;
		m_PcapFile = NULL;
	}

	// 释放资源
	m_PcapBuf->Reset();
	m_PcapPckTbl.clear();

	return PCAP_FILE_SUCCESS;
}

///
/// @brief
///     更新文件头信息，读模式有效
///
/// @return
///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
///
PCAP_FILE_RESULT PcapFile::UpdateFileHeader()
{
	if ( m_FileMode != PFM_READ )
		return PCAP_FILE_SUCCESS;

	// 获取文件头信息
	int fResult = m_PcapBuf->ReadFileToBuf(0);
	if ( fResult != PCAP_FILE_SUCCESS )
		return fResult;
	fResult = m_PcapBuf->GetDataFromBuf((uint8_t *)&m_FileHeader,sizeof(PCAP_FILE_HEADER));
	if ( fResult != PCAP_FILE_SUCCESS )
		return fResult;

	// 判断PCAP文件的大小端存储方式与主机是否相同，不同则需进行转换
	m_IsDifEndian = false;
	if ( m_FileHeader.magic != 0xA1B2C3D4 )
	{
		Bswap_FileHeader(m_FileHeader);
		m_IsDifEndian = true;
	}
	return PCAP_FILE_SUCCESS;
}

///
/// @brief
///     更新数据包地址映射表，读模式有效
///
/// @return
///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
///
PCAP_FILE_RESULT PcapFile::UpdatePckTab()
{
	int fResult;

	// 初始化数据包地址映射表
	std::vector<size_t>().swap(m_PcapPckTbl);
	m_PcapPckTbl.reserve(10000);

	if ( m_FileMode != PFM_READ )
		return PCAP_FILE_SUCCESS;

	// 获取第一个数据包的地址
	fResult = GotoFirstPckToRead();
	if ( fResult != PCAP_FILE_SUCCESS )
		return fResult;

	// 获取剩余数据包的地址
	while( true )
	{
		size_t pcapOff = m_PcapBuf->GetReadFileOffAddr();
		fResult = GotoNextPckToRead();

		if ( fResult == PCAP_FILE_ERROR )
			return PCAP_FILE_ERROR;
		else if ( fResult == PCAP_FILE_END )
		{
			// 判断最后一个数据包的数据是否完整
			PCAP_PACKET_HEADER pckHeader;
			if ( m_PcapBuf->ReadFileToBuf(pcapOff) != PCAP_FILE_SUCCESS )
				return PCAP_FILE_END;
			if ( ReadOnePckHeader(pckHeader) != PCAP_FILE_SUCCESS )
				return PCAP_FILE_END;
			uint8_t *pPckData = new uint8_t[pckHeader.capLen];
			fResult = ReadOnePckData(pPckData,pckHeader.capLen);
			delete[] pPckData;
			if ( fResult == PCAP_FILE_SUCCESS )
			{
				m_PcapPckTbl.push_back(pcapOff);
				return PCAP_FILE_SUCCESS;
			}
			else
			{
				return PCAP_FILE_END;
			}
		}
		else
			m_PcapPckTbl.push_back(pcapOff);
	}

	return fResult;
}

///
/// @brief
///     判断文件是否符合PCAP文件格式
///
/// @return
///     true - 符合； false - 不符合
///
bool PcapFile::IsPcapFile()
{
	return ( m_FileHeader.magic == 0xA1B2C3D4 && \
		     m_FileHeader.majorVersion == 0x0002 && \
			 m_FileHeader.minorVersion == 0x0004 );
}

///
/// @brief
///     PCAP文件头数据的大小端转换
///
/// @return
/// 
///
void PcapFile::Bswap_FileHeader(PCAP_FILE_HEADER &fh)
{
	fh.magic = BSWAP_32(fh.magic);
	fh.majorVersion = BSWAP_16(fh.majorVersion);
	fh.minorVersion = BSWAP_16(fh.minorVersion);
	fh.thisZone = BSWAP_32(fh.thisZone);
	fh.sigfigs = BSWAP_32(fh.sigfigs);
	fh.snapLen = BSWAP_32(fh.snapLen);
	fh.linkType = BSWAP_32(fh.linkType);
}

///
/// @brief
///     PCAP文件的Packet头数据的大小端转换
///
/// @return
/// 
///
void PcapFile::Bswap_PckHeader(PCAP_PACKET_HEADER &ph)
{
	ph.timeStamp.timeStamp_Second = BSWAP_32(ph.timeStamp.timeStamp_Second);
	ph.timeStamp.timeStamp_USec = BSWAP_32(ph.timeStamp.timeStamp_USec);
	ph.capLen = BSWAP_32(ph.capLen);
	ph.len = BSWAP_32(ph.len);
}

