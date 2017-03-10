/// @file
///     pcap_file_i.h
///
/// @brief
///     PCAP�ļ������ӿڶ����ʵ��
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
///    |2013.03.05     |1.0.1.305  |��Ӷ�PCAP�ļ����ݴ�С�˴洢���ж��Լ���Ӧ���ݵ�ת��     |
///    |2013.03.05     |1.0.1.305  |��Ӷ��ļ��Ƿ����PCAP�ļ���ʽ���ж�                     |
///    |2013.03.05     |1.0.1.305  |���Ի�����ʣ�����ݵ�д���Close()�з��룬������ΪSave�ӿ�|
///    |2013.03.05     |1.0.1.305  |���GetDataFromBuf��memcpy����ʱ���ж�                   |
///    |2013.06.28     |1.0.2.628  |�����ж�PCAP�ļ��������Ĵ�С�˴洢��ʽǰû�и�λ��Bug    |
///    |2013.08.19     |1.0.3.819  |�����������ݰ���ַӳ���ʱû���ж����һ�����ݰ��������Ե�Bug|
///    |2013.11.12     |1.0.3.1112 |�����ر�Pcap�ļ�ʱû���ͷ����ݰ���ַӳ�����Դ��bug      |
///
#include "stdafx.h"

#include <stdio.h>
#include "pcap_file.h"

#ifndef _T
#include <tchar.h>
#endif

///
/// @brief
///     16λ���Ĵ�С��ת��
///
#define BSWAP_16(x) (uint16_t)( \
	                ((*(uint16_t*)(&x) & 0xff00) >> 8) | \
	                ((*(uint16_t*)(&x) & 0x00ff) << 8) \
	                )

///
/// @brief
///     32λ���Ĵ�С��ת��
///
#define BSWAP_32(x) (uint32_t)( \
	                (((uint32_t)(x) & 0xff000000) >> 24) | \
                    (((uint32_t)(x) & 0x00ff0000) >> 8 ) | \
					(((uint32_t)(x) & 0x0000ff00) << 8 ) | \
					(((uint32_t)(x) & 0x000000ff) << 24) \
                    )

///
/// @brief
///     PCAP�ļ�ͷ��Ĭ��ֵ
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
///     �����ļ����ݻ������Ĵ�С
///
/// @param[in]
///     bufSize - �������Ĵ�С
///
/// @return
///	    true - �ɹ��� false - ʧ��
///
bool PcapFile::SetBufSize(const size_t bufSize)
{
	return m_PcapBuf->SetBufSize(bufSize);
}

///
/// @brief
///     ��ȡ��������С
///
/// @return
///     ��������С
///
size_t PcapFile::GetBufSize()
{
	return m_PcapBuf->GetBufSize();
}

///
/// @brief
///     ���ý�����������д���ļ��Ķ�ʱ��
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
///     ��ȡ���ƻ���������д���ļ��Ķ�ʱ���Ķ�ʱʱ��
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
PCAP_FILE_RESULT PcapFile::Open(std::wstring &path, PCAP_FILE_MODE mode)
{
	// �ļ�δ�رգ����ش���
	if ( m_PcapFile != NULL )
		return PCAP_FILE_ERROR;

	switch (mode)
	{
	case PFM_READ:
			// ֻ����ʽ���ļ�
			m_PcapFile = _wfopen(path.c_str(), _T("rb"));
		break;
	case PFM_WRITE:
			// ֻд��ʽ���ļ�
			m_PcapFile = _wfopen(path.c_str(), _T("wb"));
		break;
	default:
		goto errorHandle;
	}

	if (m_PcapFile == NULL)
		goto errorHandle;
	m_FileMode = mode;

	// �����ļ�����������ʼ��
	m_PcapBuf->ConnectToFile(m_PcapFile,mode);
	// �����ļ�ͷ��Ϣ
	if ( UpdateFileHeader() != PCAP_FILE_SUCCESS )
		goto errorHandle;
	// �ж��Ƿ����PCAP�ļ���ʽ
	if ( !IsPcapFile() )
		goto errorHandle;
	// �������ݰ���ַӳ���
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
///     ����Ҫ��ȡ���ļ���λ��ָ��ָ���һ�����ݰ�����ģʽ��Ч
///
/// @return
///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
///
PCAP_FILE_RESULT PcapFile::GotoFirstPckToRead()
{
	size_t curOffAddr = m_PcapBuf->GetReadFileOffAddr();

	// Ŀ��λ�ô��ڵ�ǰ��λ�ã������ƶ���Ŀ��λ��
	// ����ˢ�»�����
	if ( curOffAddr <= sizeof(PCAP_FILE_HEADER) )
		return m_PcapBuf->DownFileOffAddr(sizeof(PCAP_FILE_HEADER)-curOffAddr);
	else
		return m_PcapBuf->ReadFileToBuf(sizeof(PCAP_FILE_HEADER));
}

///
/// @brief
///     ����Ҫ��ȡ���ļ���λ��ָ��ָ����һ�����ݰ�����ģʽ��Ч
///
/// @return
///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
///
PCAP_FILE_RESULT PcapFile::GotoNextPckToRead()
{
	PCAP_PACKET_HEADER pckHeader;
	int bufRS;
	// ��ȡPacketͷ��Ϣ
	bufRS =  m_PcapBuf->GetDataFromBuf((uint8_t *)&pckHeader,sizeof(PCAP_PACKET_HEADER));
	if ( bufRS != PCAP_FILE_SUCCESS )
		return bufRS;
	// ��С������ת��
	if ( m_IsDifEndian )
		Bswap_PckHeader(pckHeader);
	bufRS = m_PcapBuf->DownFileOffAddr(sizeof(PCAP_PACKET_HEADER)+pckHeader.capLen);
	return bufRS;
}

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
PCAP_FILE_RESULT PcapFile::SeekPckToRead(const size_t idxSeq)
{
	if ( idxSeq >= m_PcapPckTbl.size() )
		return PCAP_FILE_ERROR;

	size_t offAddr    = m_PcapPckTbl.at(idxSeq);
	size_t curOffAddr = m_PcapBuf->GetReadFileOffAddr();

	// Ŀ��λ�ô��ڵ�ǰ��λ�ã������ƶ���Ŀ��λ��
	// ����ˢ�»�����
	if ( offAddr > curOffAddr )
		return m_PcapBuf->DownFileOffAddr(offAddr-curOffAddr);
	else
		return m_PcapBuf->UpFileOffAddr(curOffAddr-offAddr);
}

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
PCAP_FILE_RESULT PcapFile::ReadOnePckHeader(PCAP_PACKET_HEADER &pckHeader)
{
	int bufRS;
	// ��ȡPacketͷ��Ϣ
	bufRS =  m_PcapBuf->GetDataFromBuf((uint8_t *)&pckHeader,sizeof(PCAP_PACKET_HEADER));
	if ( bufRS != PCAP_FILE_SUCCESS )
		return bufRS;
	// ��С������ת��
	if ( m_IsDifEndian )
		Bswap_PckHeader(pckHeader);
	return PCAP_FILE_SUCCESS;
}

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
///     ��ȡ�ļ������ݰ�����Ŀ��Ϣ
///
/// @return
///     �ļ������ݰ�����Ŀ
///
size_t PcapFile::GetPckNum()
{
	return m_PcapPckTbl.size();
}

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
void PcapFile::GetFileHeader(PCAP_FILE_HEADER &fileHeader)
{
	fileHeader = m_FileHeader;
}

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
PCAP_FILE_RESULT PcapFile::InitFileHeader(const PCAP_FILE_HEADER &fh)
{
	if ( ftell(m_PcapFile) > 0 || m_FileMode != PFM_WRITE )
		return PCAP_FILE_ERROR;
	m_FileHeader = fh;
	return PCAP_FILE_SUCCESS;
}

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
PCAP_FILE_RESULT PcapFile::WriteNextPck(PCAP_FILE_TIME_VAL &timeStamp, const uint8_t *pckDataArr, const uint32_t arrSize)
{
	if ( m_FileMode != PFM_WRITE )
		return PCAP_FILE_ERROR;

	// ����ļ�ͷ
	int wResult;
	if ( m_PcapBuf->GetWriteFileOffAddr() == 0 )
	{
		wResult = m_PcapBuf->AddDataToBuf((uint8_t *)&m_FileHeader,sizeof(PCAP_FILE_HEADER));
		if ( wResult == PCAP_FILE_ERROR )
			return PCAP_FILE_ERROR;
	}

	size_t idx = m_PcapBuf->GetWriteFileOffAddr();

	// �������ݰ�ͷ
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

	// д�����ݰ���Ϣ
	wResult = m_PcapBuf->AddDataToBuf((uint8_t *)&pckHeader,sizeof(PCAP_PACKET_HEADER));
	if ( wResult == PCAP_FILE_ERROR )
		return PCAP_FILE_ERROR;
	wResult = m_PcapBuf->AddDataToBuf(pckDataArr,pckHeader.capLen);
	if ( wResult == PCAP_FILE_ERROR )
		return PCAP_FILE_ERROR;

	// ������ݰ���ַ
	m_PcapPckTbl.push_back(idx);
	return wResult;
}

///
/// @brief
///     ����д����Ϣ��дģʽ��Ч
///
/// @return
///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
///
PCAP_FILE_RESULT PcapFile::Save()
{
	if ( m_FileMode == PFM_WRITE )
	{
		// дģʽ�£�������������д���ļ�
		return m_PcapBuf->WriteBufToFile();
	}
	return PCAP_FILE_SUCCESS;
}

///
/// @brief
///     �ر��ļ�
///
/// @return
///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
///
PCAP_FILE_RESULT PcapFile::Close()
{
	// �ر��ļ�
	if ( m_PcapFile != NULL )
	{
		if ( fclose(m_PcapFile) != 0 )
			return PCAP_FILE_ERROR;
		m_PcapFile = NULL;
	}

	// �ͷ���Դ
	m_PcapBuf->Reset();
	m_PcapPckTbl.clear();

	return PCAP_FILE_SUCCESS;
}

///
/// @brief
///     �����ļ�ͷ��Ϣ����ģʽ��Ч
///
/// @return
///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
///
PCAP_FILE_RESULT PcapFile::UpdateFileHeader()
{
	if ( m_FileMode != PFM_READ )
		return PCAP_FILE_SUCCESS;

	// ��ȡ�ļ�ͷ��Ϣ
	int fResult = m_PcapBuf->ReadFileToBuf(0);
	if ( fResult != PCAP_FILE_SUCCESS )
		return fResult;
	fResult = m_PcapBuf->GetDataFromBuf((uint8_t *)&m_FileHeader,sizeof(PCAP_FILE_HEADER));
	if ( fResult != PCAP_FILE_SUCCESS )
		return fResult;

	// �ж�PCAP�ļ��Ĵ�С�˴洢��ʽ�������Ƿ���ͬ����ͬ�������ת��
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
///     �������ݰ���ַӳ�����ģʽ��Ч
///
/// @return
///     -1 - �������� 0 - �����ɹ��� 1 - �����ļ���β
///
PCAP_FILE_RESULT PcapFile::UpdatePckTab()
{
	int fResult;

	// ��ʼ�����ݰ���ַӳ���
	std::vector<size_t>().swap(m_PcapPckTbl);
	m_PcapPckTbl.reserve(10000);

	if ( m_FileMode != PFM_READ )
		return PCAP_FILE_SUCCESS;

	// ��ȡ��һ�����ݰ��ĵ�ַ
	fResult = GotoFirstPckToRead();
	if ( fResult != PCAP_FILE_SUCCESS )
		return fResult;

	// ��ȡʣ�����ݰ��ĵ�ַ
	while( true )
	{
		size_t pcapOff = m_PcapBuf->GetReadFileOffAddr();
		fResult = GotoNextPckToRead();

		if ( fResult == PCAP_FILE_ERROR )
			return PCAP_FILE_ERROR;
		else if ( fResult == PCAP_FILE_END )
		{
			// �ж����һ�����ݰ��������Ƿ�����
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
///     �ж��ļ��Ƿ����PCAP�ļ���ʽ
///
/// @return
///     true - ���ϣ� false - ������
///
bool PcapFile::IsPcapFile()
{
	return ( m_FileHeader.magic == 0xA1B2C3D4 && \
		     m_FileHeader.majorVersion == 0x0002 && \
			 m_FileHeader.minorVersion == 0x0004 );
}

///
/// @brief
///     PCAP�ļ�ͷ���ݵĴ�С��ת��
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
///     PCAP�ļ���Packetͷ���ݵĴ�С��ת��
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

