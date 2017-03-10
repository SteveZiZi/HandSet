/// @file
///     file_buf.cpp
///
/// @brief
///     文件数据缓存区，缓存文件读/写数据
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
///    |2013.03.05     |1.0.1.305  |添加对操作缓冲区时的缓冲区溢出判断（一次读写数据超出缓存区容量）|
///    |2013.03.05     |1.0.1.305  |添加对文件尺寸问题的判断，由于位置指针为size_t型，所以不能超过4G|
///    |2013.11.04     |1.0.1.1104 |修正设置文件数据缓存区的大小时没有更新相应参数值的Bug           |
///
#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <WinBase.h>
#include <ctime>
#include "file_buf.h"

///
/// @brief
///     文件数据缓存区的默认大小，1M
///
#define DEFAULT_BUF_SIZE 1024*1024*1

///
/// @brief
///     控制缓存区数据写入文件的定时器的默认定时时间，单位ms
///
#define DEFAULT_WRITE_TIMER 500

FileBuf::FileBuf()
: m_ConnectingFile(NULL)
, m_BufSize(DEFAULT_BUF_SIZE)
, m_BufOff(0)
, m_BufEnd(0)
, m_ReadFileOffAddr(0)
, m_WriteFileOffAddr(0)
, m_FileMode(PFM_READ)
, m_WriteTimer(DEFAULT_WRITE_TIMER)
, m_bReadWriteIng(false)
{
	m_Buffer = new uint8_t[DEFAULT_BUF_SIZE];
}

FileBuf::~FileBuf()
{
	if ( m_Buffer != NULL )
	{
		delete[] m_Buffer;
		m_Buffer = NULL;
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
bool FileBuf::SetBufSize(const size_t bufSize)
{
	if ( !m_bReadWriteIng )
	{
		if ( m_Buffer != NULL )
		{
			delete[] m_Buffer;
			m_Buffer = NULL;
		}
		m_Buffer = new uint8_t[bufSize];
		if ( m_Buffer == NULL )
			return false;
		m_BufSize = bufSize;
		return true;
	}
	return false;
}

///
/// @brief
///     获取缓存区大小
///
/// @return
///     缓存区大小
///
size_t FileBuf::GetBufSize()
{
	return m_BufSize;
}

///
/// @brief
///     设置将缓存区数据写入文件的定时器
///
/// @return
/// 
///
void FileBuf::SetWriteTimer(uint32_t time)
{
	m_WriteTimer = time;
}

///
/// @brief
///     获取控制缓存区数据写入文件的定时器的定时时间
///
/// @return
/// 
///
uint32_t FileBuf::GetWriteTimer()
{
	return m_WriteTimer;
}

///
/// @brief
///     缓存区复位
///
/// @return
/// 
///
void FileBuf::Reset()
{
	m_ConnectingFile   = NULL;
	m_BufOff           = 0;
	m_BufEnd           = 0;
	m_ReadFileOffAddr  = 0;
	m_WriteFileOffAddr = 0;
	m_LastWriteTime    = 0;
	m_bReadWriteIng    = false;
}

///
/// @brief
///     将缓存区与指定文件相连，即设置缓存区的读写目标
///
/// @param[in]
///     file - 所连接的文件
/// @param[in]
///     mode - 所连接的文件的文件模式
///
/// @return
/// 
///
void FileBuf::ConnectToFile(FILE *file, PCAP_FILE_MODE mode)
{
	m_ConnectingFile   = file;
	m_BufOff           = 0;
	m_BufEnd           = 0;
	m_ReadFileOffAddr  = 0;
	m_WriteFileOffAddr = 0;
	m_FileMode         = mode;
	if ( mode == PFM_WRITE )
		m_LastWriteTime = GetTickCount();
	m_bReadWriteIng = true;
}

///
/// @brief
///     将指定位置指针开始的文件数据读取到缓存区
///
/// @param[in]
///     fileOffAddr - 文件位置指针
///
/// @return
///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
///
int FileBuf::ReadFileToBuf(const size_t fileOffAddr)
{
	if ( m_ConnectingFile == NULL || m_FileMode != PFM_READ )
		return PCAP_FILE_ERROR;

	// 设置文件位置指针
	if ( fseek(m_ConnectingFile,(long)fileOffAddr,SEEK_SET) == -1 )
		return PCAP_FILE_ERROR;

	// 读取数据至缓存区
	size_t rSize   = fread(m_Buffer, 1, m_BufSize, m_ConnectingFile);
	int rResult = GetIOResult(rSize,m_BufSize);
	if ( rResult == PCAP_FILE_ERROR )
		return PCAP_FILE_ERROR;

	// 更新缓存区及文件位置信息
	m_BufOff  = 0;
	m_BufEnd  = rSize;
	m_ReadFileOffAddr = fileOffAddr;
	return rResult;
}

///
/// @brief
///     向上设置文件位置指针
///
/// @param[in]
///     offset - 相对当前文件位置指针的偏移量
///
/// @return
///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
///
int FileBuf::UpFileOffAddr(const size_t offset)
{
	if ( m_ConnectingFile == NULL || m_FileMode != PFM_READ )
		return PCAP_FILE_ERROR;

	// 判断文件位置指针是否溢出
	if ( m_ReadFileOffAddr < offset )
		return PCAP_FILE_ERROR;

	if ( m_BufOff < offset )
	{// 文件位置指针超过缓存区，则更新读取当前文件位置开始的数据至缓存区
		size_t tmpOff = m_ReadFileOffAddr-offset;
		int rResult = ReadFileToBuf(tmpOff);
		if ( rResult == PCAP_FILE_ERROR )
			return PCAP_FILE_ERROR;
		return rResult;
	}
	else
	{// 更新缓存区及文件位置信息
		m_BufOff -= offset;
		m_ReadFileOffAddr -= offset;
		return PCAP_FILE_SUCCESS;
	}
}

///
/// @brief
///     向下设置文件位置指针
///
/// @param[in]
///     offset - 相对当前文件位置指针的偏移量
///
/// @return
///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
///
int FileBuf::DownFileOffAddr(const size_t offset)
{
	if ( m_ConnectingFile == NULL || m_FileMode != PFM_READ )
		return PCAP_FILE_ERROR;

	// 判断文件位置指针是否溢出，即文件大小不能超过4G
	size_t tmpOff = m_ReadFileOffAddr+offset;
	if ( tmpOff < m_ReadFileOffAddr )
		return PCAP_FILE_ERROR;

	if ( m_BufOff + offset >= m_BufEnd )
	{// 文件位置指针超过缓存区，则更新读取当前文件位置开始的数据至缓存区
		int rResult = ReadFileToBuf(tmpOff);
		if ( rResult == PCAP_FILE_ERROR )
			return PCAP_FILE_ERROR;
		return rResult;
	}
	else
	{// 更新缓存区及文件位置信息
		m_BufOff += offset;
		m_ReadFileOffAddr += offset;
		return PCAP_FILE_SUCCESS;
	}
}

///
/// @brief
///     从缓存区获取当前位置指针开始的数量为arrSize的数据
///
/// @param[in]
///     datArr  - 保存数据的数组
/// @param[in]
///     arrSize - 数组的元素数目
///
/// @return
///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
///
int FileBuf::GetDataFromBuf(uint8_t *datArr, const size_t arrSize)
{
	if ( m_ConnectingFile == NULL || m_FileMode != PFM_READ )
		return PCAP_FILE_ERROR;

	// 缓存区溢出判断，数据大小不能超过缓存区容量
	if ( arrSize > m_BufSize )
		return PCAP_FILE_ERROR;

	// 判断所读数据是否在缓存区中，“否”则更新缓存区数据
	int rResult = PCAP_FILE_SUCCESS;
	if ( m_BufOff + arrSize > m_BufEnd )
		rResult = ReadFileToBuf(m_ReadFileOffAddr);

	// 读取缓存区数据
	if ( rResult != PCAP_FILE_ERROR && (m_BufOff + arrSize) <= m_BufEnd )
	{
		memcpy(datArr,&m_Buffer[m_BufOff],arrSize);
		return PCAP_FILE_SUCCESS;
	}
	return PCAP_FILE_END;
}

///
/// @brief
///     获取当前读位置指针
///
/// @return
///     当前读位置指针
///
size_t FileBuf::GetReadFileOffAddr()
{
	return m_ReadFileOffAddr;
}

///
/// @brief
///     从当前位置指针开始将数量为arrSize的数据添加到缓存区
///
/// @param[in]
///     datArr  - 指向要添加的数据
/// @param[in]
///     arrSize - 数据的大小
///
/// @return
///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
///
int FileBuf::AddDataToBuf(const uint8_t *datArr, const size_t arrSize)
{
	if ( m_ConnectingFile == NULL || m_FileMode != PFM_WRITE )
		return PCAP_FILE_ERROR;

	// 缓存区溢出判断，数据大小不能超过缓存区容量
	if ( arrSize > m_BufSize )
		return PCAP_FILE_ERROR;

	// 判断文件位置指针是否溢出，即文件大小不能超过4G
	size_t tmpOff = m_WriteFileOffAddr+arrSize;
	if ( tmpOff < m_WriteFileOffAddr )
		return PCAP_FILE_ERROR;

	int wResult = PCAP_FILE_SUCCESS;
	uint32_t curTime = GetTickCount();
	// 添加新数据会超出缓存区或达到定时时间，则先将缓存区数据写入文件
	if ( m_BufEnd + arrSize >= m_BufSize || curTime - m_LastWriteTime >= m_WriteTimer )
		wResult = WriteBufToFile();

	// 添加数据并更新缓存区及文件位置指针
	if ( wResult != PCAP_FILE_ERROR )
	{
		memcpy(&m_Buffer[m_BufEnd],datArr,arrSize);
		m_BufEnd += arrSize;
		m_WriteFileOffAddr += arrSize;
	}
	return wResult;
}

///
/// @brief
///     将缓存区的数据写入文件
///
/// @return
///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
///
int FileBuf::WriteBufToFile()
{
	if ( m_ConnectingFile == NULL || m_FileMode != PFM_WRITE )
		return PCAP_FILE_ERROR;

	// 将缓存区数据写入文件
	if ( m_BufEnd > 0 )
	{
		size_t wSize = fwrite(m_Buffer, 1, m_BufEnd, m_ConnectingFile);
		int wResult = GetIOResult(wSize,m_BufEnd);
//		fflush(m_ConnectingFile);
		if ( wResult == PCAP_FILE_ERROR )
			return PCAP_FILE_ERROR;
		m_BufEnd = 0;
		m_LastWriteTime = GetTickCount();
		return wResult;
	}
	return PCAP_FILE_SUCCESS;
}

///
/// @brief
///     获取当前写位置指针
///
/// @return
///     当前写位置指针
///
size_t FileBuf::GetWriteFileOffAddr()
{
	return m_WriteFileOffAddr;
}

///
/// @brief
///     清空缓存区数据
///
/// @return
/// 
///
void FileBuf::ClearBuf()
{
	// 重置缓存区结尾指针和读位置指针
	m_BufOff  = 0;
	m_BufEnd  = 0;
}

///
/// @brief
///     获取文件读写操作的结果
///
/// @param[in]
///     rwReturn - 文件读写操作(fread、fwrite)的返回值
/// @param[in]
///     rwSize   - 该操作所读写的数据的大小
///
/// @return
///     -1 - 发生错误； 0 - 操作成功； 1 - 到达文件结尾
///
int FileBuf::GetIOResult(size_t rwReturn, size_t rwSize)
{
	if ( m_ConnectingFile == NULL )
		return PCAP_FILE_ERROR;

	if ( rwReturn < rwSize && ferror(m_ConnectingFile) != 0 )
	{
		clearerr(m_ConnectingFile);
		return PCAP_FILE_ERROR;
	}

	if ( rwReturn == 0 && m_FileMode == PFM_READ )
		return PCAP_FILE_END;

	return PCAP_FILE_SUCCESS;
}
