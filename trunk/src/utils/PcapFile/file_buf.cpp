/// @file
///     file_buf.cpp
///
/// @brief
///     �ļ����ݻ������������ļ���/д����
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
///    |2013.03.05     |1.0.1.305  |��ӶԲ���������ʱ�Ļ���������жϣ�һ�ζ�д���ݳ���������������|
///    |2013.03.05     |1.0.1.305  |��Ӷ��ļ��ߴ�������жϣ�����λ��ָ��Ϊsize_t�ͣ����Բ��ܳ���4G|
///    |2013.11.04     |1.0.1.1104 |���������ļ����ݻ������Ĵ�Сʱû�и�����Ӧ����ֵ��Bug           |
///
#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <WinBase.h>
#include <ctime>
#include "file_buf.h"

///
/// @brief
///     �ļ����ݻ�������Ĭ�ϴ�С��1M
///
#define DEFAULT_BUF_SIZE 1024*1024*1

///
/// @brief
///     ���ƻ���������д���ļ��Ķ�ʱ����Ĭ�϶�ʱʱ�䣬��λms
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
///     �����ļ����ݻ������Ĵ�С
///
/// @param[in]
///     bufSize - �������Ĵ�С
///
/// @return
///	    true - �ɹ��� false - ʧ��
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
///     ��ȡ��������С
///
/// @return
///     ��������С
///
size_t FileBuf::GetBufSize()
{
	return m_BufSize;
}

///
/// @brief
///     ���ý�����������д���ļ��Ķ�ʱ��
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
///     ��ȡ���ƻ���������д���ļ��Ķ�ʱ���Ķ�ʱʱ��
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
///     ��������λ
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
///     ����������ָ���ļ������������û������Ķ�дĿ��
///
/// @param[in]
///     file - �����ӵ��ļ�
/// @param[in]
///     mode - �����ӵ��ļ����ļ�ģʽ
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
///     ��ָ��λ��ָ�뿪ʼ���ļ����ݶ�ȡ��������
///
/// @param[in]
///     fileOffAddr - �ļ�λ��ָ��
///
/// @return
///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
///
int FileBuf::ReadFileToBuf(const size_t fileOffAddr)
{
	if ( m_ConnectingFile == NULL || m_FileMode != PFM_READ )
		return PCAP_FILE_ERROR;

	// �����ļ�λ��ָ��
	if ( fseek(m_ConnectingFile,(long)fileOffAddr,SEEK_SET) == -1 )
		return PCAP_FILE_ERROR;

	// ��ȡ������������
	size_t rSize   = fread(m_Buffer, 1, m_BufSize, m_ConnectingFile);
	int rResult = GetIOResult(rSize,m_BufSize);
	if ( rResult == PCAP_FILE_ERROR )
		return PCAP_FILE_ERROR;

	// ���»��������ļ�λ����Ϣ
	m_BufOff  = 0;
	m_BufEnd  = rSize;
	m_ReadFileOffAddr = fileOffAddr;
	return rResult;
}

///
/// @brief
///     ���������ļ�λ��ָ��
///
/// @param[in]
///     offset - ��Ե�ǰ�ļ�λ��ָ���ƫ����
///
/// @return
///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
///
int FileBuf::UpFileOffAddr(const size_t offset)
{
	if ( m_ConnectingFile == NULL || m_FileMode != PFM_READ )
		return PCAP_FILE_ERROR;

	// �ж��ļ�λ��ָ���Ƿ����
	if ( m_ReadFileOffAddr < offset )
		return PCAP_FILE_ERROR;

	if ( m_BufOff < offset )
	{// �ļ�λ��ָ�볬��������������¶�ȡ��ǰ�ļ�λ�ÿ�ʼ��������������
		size_t tmpOff = m_ReadFileOffAddr-offset;
		int rResult = ReadFileToBuf(tmpOff);
		if ( rResult == PCAP_FILE_ERROR )
			return PCAP_FILE_ERROR;
		return rResult;
	}
	else
	{// ���»��������ļ�λ����Ϣ
		m_BufOff -= offset;
		m_ReadFileOffAddr -= offset;
		return PCAP_FILE_SUCCESS;
	}
}

///
/// @brief
///     ���������ļ�λ��ָ��
///
/// @param[in]
///     offset - ��Ե�ǰ�ļ�λ��ָ���ƫ����
///
/// @return
///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
///
int FileBuf::DownFileOffAddr(const size_t offset)
{
	if ( m_ConnectingFile == NULL || m_FileMode != PFM_READ )
		return PCAP_FILE_ERROR;

	// �ж��ļ�λ��ָ���Ƿ���������ļ���С���ܳ���4G
	size_t tmpOff = m_ReadFileOffAddr+offset;
	if ( tmpOff < m_ReadFileOffAddr )
		return PCAP_FILE_ERROR;

	if ( m_BufOff + offset >= m_BufEnd )
	{// �ļ�λ��ָ�볬��������������¶�ȡ��ǰ�ļ�λ�ÿ�ʼ��������������
		int rResult = ReadFileToBuf(tmpOff);
		if ( rResult == PCAP_FILE_ERROR )
			return PCAP_FILE_ERROR;
		return rResult;
	}
	else
	{// ���»��������ļ�λ����Ϣ
		m_BufOff += offset;
		m_ReadFileOffAddr += offset;
		return PCAP_FILE_SUCCESS;
	}
}

///
/// @brief
///     �ӻ�������ȡ��ǰλ��ָ�뿪ʼ������ΪarrSize������
///
/// @param[in]
///     datArr  - �������ݵ�����
/// @param[in]
///     arrSize - �����Ԫ����Ŀ
///
/// @return
///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
///
int FileBuf::GetDataFromBuf(uint8_t *datArr, const size_t arrSize)
{
	if ( m_ConnectingFile == NULL || m_FileMode != PFM_READ )
		return PCAP_FILE_ERROR;

	// ����������жϣ����ݴ�С���ܳ�������������
	if ( arrSize > m_BufSize )
		return PCAP_FILE_ERROR;

	// �ж����������Ƿ��ڻ������У���������»���������
	int rResult = PCAP_FILE_SUCCESS;
	if ( m_BufOff + arrSize > m_BufEnd )
		rResult = ReadFileToBuf(m_ReadFileOffAddr);

	// ��ȡ����������
	if ( rResult != PCAP_FILE_ERROR && (m_BufOff + arrSize) <= m_BufEnd )
	{
		memcpy(datArr,&m_Buffer[m_BufOff],arrSize);
		return PCAP_FILE_SUCCESS;
	}
	return PCAP_FILE_END;
}

///
/// @brief
///     ��ȡ��ǰ��λ��ָ��
///
/// @return
///     ��ǰ��λ��ָ��
///
size_t FileBuf::GetReadFileOffAddr()
{
	return m_ReadFileOffAddr;
}

///
/// @brief
///     �ӵ�ǰλ��ָ�뿪ʼ������ΪarrSize��������ӵ�������
///
/// @param[in]
///     datArr  - ָ��Ҫ��ӵ�����
/// @param[in]
///     arrSize - ���ݵĴ�С
///
/// @return
///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
///
int FileBuf::AddDataToBuf(const uint8_t *datArr, const size_t arrSize)
{
	if ( m_ConnectingFile == NULL || m_FileMode != PFM_WRITE )
		return PCAP_FILE_ERROR;

	// ����������жϣ����ݴ�С���ܳ�������������
	if ( arrSize > m_BufSize )
		return PCAP_FILE_ERROR;

	// �ж��ļ�λ��ָ���Ƿ���������ļ���С���ܳ���4G
	size_t tmpOff = m_WriteFileOffAddr+arrSize;
	if ( tmpOff < m_WriteFileOffAddr )
		return PCAP_FILE_ERROR;

	int wResult = PCAP_FILE_SUCCESS;
	uint32_t curTime = GetTickCount();
	// ��������ݻᳬ����������ﵽ��ʱʱ�䣬���Ƚ�����������д���ļ�
	if ( m_BufEnd + arrSize >= m_BufSize || curTime - m_LastWriteTime >= m_WriteTimer )
		wResult = WriteBufToFile();

	// ������ݲ����»��������ļ�λ��ָ��
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
///     ��������������д���ļ�
///
/// @return
///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
///
int FileBuf::WriteBufToFile()
{
	if ( m_ConnectingFile == NULL || m_FileMode != PFM_WRITE )
		return PCAP_FILE_ERROR;

	// ������������д���ļ�
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
///     ��ȡ��ǰдλ��ָ��
///
/// @return
///     ��ǰдλ��ָ��
///
size_t FileBuf::GetWriteFileOffAddr()
{
	return m_WriteFileOffAddr;
}

///
/// @brief
///     ��ջ���������
///
/// @return
/// 
///
void FileBuf::ClearBuf()
{
	// ���û�������βָ��Ͷ�λ��ָ��
	m_BufOff  = 0;
	m_BufEnd  = 0;
}

///
/// @brief
///     ��ȡ�ļ���д�����Ľ��
///
/// @param[in]
///     rwReturn - �ļ���д����(fread��fwrite)�ķ���ֵ
/// @param[in]
///     rwSize   - �ò�������д�����ݵĴ�С
///
/// @return
///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
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
