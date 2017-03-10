/// @file
///     goose_types.cpp
///
/// @brief
///     ����Goose����֡�������Զ����������ݵĴ�����
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.3.18
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#include "stdafx.h"
#include "../include/goose_types.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "../msg_seg/bswap.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/utils/utility.h"

///
/// @brief
///    ����goose������float�ľ���
///
#define GOOSE_FLOAT_PRECISION   0.0005

GOOSE_DATA *GooseDataCopy(GOOSE_DATA *pGseData)
{
	if ( pGseData == NULL )
		return NULL;

	switch (pGseData->type)
	{
	case GSEDT_BOOLEAN:
		{
			GOOSE_BOOLEAN *tmp = (GOOSE_BOOLEAN *)pGseData;
			GOOSE_BOOLEAN *newData = new GOOSE_BOOLEAN;
			newData->id = tmp->id;
			newData->boolVal = tmp->boolVal;
			return (GOOSE_DATA *)newData;
		}
		break;
	case GSEDT_BIT_STR:
		{
			GOOSE_BIT_STRING *tmp = (GOOSE_BIT_STRING *)pGseData;
			GOOSE_BIT_STRING *newData = new GOOSE_BIT_STRING;
			newData->id = tmp->id;
			newData->strSize = tmp->strSize;
			newData->padding = tmp->padding;
			newData->bitStr = new uint8_t[tmp->strSize];
			memcpy(newData->bitStr,tmp->bitStr,tmp->strSize);
			return (GOOSE_DATA *)newData;
		}
		break;
	case GSEDT_INT8:
		{
			GOOSE_INT8 *tmp = (GOOSE_INT8 *)pGseData;
			GOOSE_INT8 *newData = new GOOSE_INT8;
			newData->id = tmp->id;
			newData->datVal = tmp->datVal;
			return (GOOSE_DATA *)newData;
		}
		break;
	case GSEDT_INT16:
		{
			GOOSE_INT16 *tmp = (GOOSE_INT16 *)pGseData;
			GOOSE_INT16 *newData = new GOOSE_INT16;
			newData->id = tmp->id;
			newData->datVal = tmp->datVal;
			return (GOOSE_DATA *)newData;
		}
		break;
	case GSEDT_INT24:
		{
			GOOSE_INT24 *tmp = (GOOSE_INT24 *)pGseData;
			GOOSE_INT24 *newData = new GOOSE_INT24;
			newData->id = tmp->id;
			newData->datVal = tmp->datVal;
			return (GOOSE_DATA *)newData;
		}
		break;
	case GSEDT_INT32:
		{
			GOOSE_INT32 *tmp = (GOOSE_INT32 *)pGseData;
			GOOSE_INT32 *newData = new GOOSE_INT32;
			newData->id = tmp->id;
			newData->datVal = tmp->datVal;
			return (GOOSE_DATA *)newData;
		}
		break;
	case GSEDT_UINT8:
		{
			GOOSE_UINT8 *tmp = (GOOSE_UINT8 *)pGseData;
			GOOSE_UINT8 *newData = new GOOSE_UINT8;
			newData->id = tmp->id;
			newData->datVal = tmp->datVal;
			return (GOOSE_DATA *)newData;
		}
		break;
	case GSEDT_UINT16:
		{
			GOOSE_UINT16 *tmp = (GOOSE_UINT16 *)pGseData;
			GOOSE_UINT16 *newData = new GOOSE_UINT16;
			newData->id = tmp->id;
			newData->datVal = tmp->datVal;
			return (GOOSE_DATA *)newData;
		}
		break;
	case GSEDT_UINT24:
		{
			GOOSE_UINT24 *tmp = (GOOSE_UINT24 *)pGseData;
			GOOSE_UINT24 *newData = new GOOSE_UINT24;
			newData->id = tmp->id;
			newData->datVal = tmp->datVal;
			return (GOOSE_DATA *)newData;
		}
		break;
	case GSEDT_UINT32:
		{
			GOOSE_UINT32 *tmp = (GOOSE_UINT32 *)pGseData;
			GOOSE_UINT32 *newData = new GOOSE_UINT32;
			newData->id = tmp->id;
			newData->datVal = tmp->datVal;
			return (GOOSE_DATA *)newData;
		}
		break;
	case GSEDT_FLOAT:
		{
			GOOSE_FLOAT *tmp = (GOOSE_FLOAT *)pGseData;
			GOOSE_FLOAT *newData = new GOOSE_FLOAT;
			newData->id = tmp->id;
			newData->datVal = tmp->datVal;
			return (GOOSE_DATA *)newData;
		}
		break;
	case GSEDT_OCTET_STR:
		{
			GOOSE_OCTET_STRING *tmp = (GOOSE_OCTET_STRING *)pGseData;
			GOOSE_OCTET_STRING *newData = new GOOSE_OCTET_STRING;
			newData->id = tmp->id;
			newData->strSize = tmp->strSize;
			newData->octetStr = new uint8_t[tmp->strSize];
			memcpy(newData->octetStr,tmp->octetStr,tmp->strSize);
			return (GOOSE_DATA *)newData;
		}
		break;
	case GSEDT_VISIBLE_STR:
		{
			GOOSE_VISIBLE_STRING *tmp = (GOOSE_VISIBLE_STRING *)pGseData;
			GOOSE_VISIBLE_STRING *newData = new GOOSE_VISIBLE_STRING;
			newData->id = tmp->id;
			newData->strSize = tmp->strSize;
			newData->visibleStr = new int8_t[tmp->strSize];
			memcpy(newData->visibleStr,tmp->visibleStr,tmp->strSize);
			return (GOOSE_DATA *)newData;
		}
		break;
	case GSEDT_UTC_TIME:
		{
			GOOSE_UTC_TIME *tmp = (GOOSE_UTC_TIME *)pGseData;
			GOOSE_UTC_TIME *newData = new GOOSE_UTC_TIME;
			newData->id   = tmp->id;
			newData->t_s  = tmp->t_s;
			newData->t_ns = tmp->t_ns;
			newData->t_q  = tmp->t_q;
			return (GOOSE_DATA *)newData;
		}
		break;
	case GSEDT_STRUCT:
		{
			GOOSE_STRUCT *tmp = (GOOSE_STRUCT *)pGseData;
			GOOSE_STRUCT *newData = new GOOSE_STRUCT;
			newData->id = tmp->id;
			if ( tmp->member.size() > 0 )
			{
				newData->member.reserve(tmp->member.size());
				std::vector<GOOSE_DATA *>::iterator it;
				for ( it = tmp->member.begin(); it != tmp->member.end(); it++ )
				{
					newData->member.push_back(GooseDataCopy(*it));
				}
			}
			return (GOOSE_DATA *)newData;
		}
		break;
	case GSEDT_UNKNOW:
		{
			GOOSE_UNKNOW *tmp = (GOOSE_UNKNOW *)pGseData;
			GOOSE_UNKNOW *newData = new GOOSE_UNKNOW;
			newData->id       = tmp->id;
			newData->gooseTag = tmp->gooseTag;
			newData->datLen   = tmp->datLen;
			newData->datVal   = new uint8_t[tmp->datLen];
			memcpy(newData->datVal,tmp->datVal,tmp->datLen);
			return (GOOSE_DATA *)newData;
		}
		break;
	default:
		{
			return NULL;
		}
	}
}

///
/// @brief
///     �ͷŴ�����GooseData
///
/// @param[in]
///     pGseData - ָ��GooseData��ָ��
///
/// @return
///     
///
void GooseDataFree(GOOSE_DATA *pGseData)
{
	if ( pGseData == NULL )
		return;

	switch (pGseData->type)
	{
	case GSEDT_BOOLEAN:
		{
			delete (GOOSE_BOOLEAN *)pGseData;
		}
		break;
	case GSEDT_BIT_STR:
		{
			GOOSE_BIT_STRING *tmp = (GOOSE_BIT_STRING *)pGseData;
			if ( tmp->bitStr != NULL )
			{
				delete[] tmp->bitStr;
				tmp->bitStr = NULL;
			}
			delete tmp;
		}
		break;
	case GSEDT_INT8:
		{
			GOOSE_INT8 *tmp = (GOOSE_INT8 *)pGseData;
			delete tmp;		
		}
		break;
	case GSEDT_INT16:
		{
			GOOSE_INT16 *tmp = (GOOSE_INT16 *)pGseData;
			delete tmp;		
		}
		break;
	case GSEDT_INT24:
		{
			GOOSE_INT24 *tmp = (GOOSE_INT24 *)pGseData;
			delete tmp;		
		}
		break;
	case GSEDT_INT32:
		{
			GOOSE_INT32 *tmp = (GOOSE_INT32 *)pGseData;
			delete tmp;		
		}
		break;
	case GSEDT_UINT8:
		{
			GOOSE_UINT8 *tmp = (GOOSE_UINT8 *)pGseData;
			delete tmp;		
		}
		break;
	case GSEDT_UINT16:
		{
			GOOSE_UINT16 *tmp = (GOOSE_UINT16 *)pGseData;
			delete tmp;		
		}
		break;
	case GSEDT_UINT24:
		{
			GOOSE_UINT24 *tmp = (GOOSE_UINT24 *)pGseData;
			delete tmp;		
		}
		break;
	case GSEDT_UINT32:
		{
			GOOSE_UINT32 *tmp = (GOOSE_UINT32 *)pGseData;
			delete tmp;		
		}
		break;
	case GSEDT_FLOAT:
		{
			GOOSE_FLOAT *tmp = (GOOSE_FLOAT *)pGseData;
			delete tmp;
		}
		break;
	case GSEDT_OCTET_STR:
		{
			GOOSE_OCTET_STRING *tmp = (GOOSE_OCTET_STRING *)pGseData;
			if ( tmp->octetStr != NULL )
			{
				delete[] tmp->octetStr;
				tmp->octetStr = NULL;
			}
			delete tmp;
		}
		break;
	case GSEDT_VISIBLE_STR:
		{
			GOOSE_VISIBLE_STRING *tmp = (GOOSE_VISIBLE_STRING *)pGseData;
			if ( tmp->visibleStr != NULL )
			{
				delete[] tmp->visibleStr;
				tmp->visibleStr = NULL;
			}
			delete tmp;
		}
		break;
	case GSEDT_UTC_TIME:
		{
			delete (GOOSE_UTC_TIME *)pGseData;
		}
		break;
	case GSEDT_STRUCT:
		{
			GOOSE_STRUCT *tmp = (GOOSE_STRUCT *)pGseData;
			if ( tmp->member.size() > 0 )
			{
				std::vector<GOOSE_DATA *>::iterator it;
				for ( it = tmp->member.begin(); it != tmp->member.end(); it++ )
				{
					GooseDataFree(*it);
				}
				tmp->member.clear();
			}
			delete tmp;
		}
		break;
	case GSEDT_UNKNOW:
		{
			GOOSE_UNKNOW *tmp = (GOOSE_UNKNOW *)pGseData;
			if ( tmp->datVal != NULL )
			{
				delete[] tmp->datVal;
				tmp->datVal = NULL;
			}
			delete tmp;
		}
	default:
		{
			delete pGseData;
		}
	}
}

///
/// @brief
///     ��ȡGooseData��TLV��ʽ��ռ�õ��ֽ���(Tag(1�ֽ�)+Len(1~5�ֽ�)+Val)
///
/// @param[in]
///     pGseData - ָ��GooseData��ָ��
///
/// @return
///     GooseData��TLV��ʽ��ռ�õ��ֽ���
///
size_t GooseDataTLVBytes(const GOOSE_DATA *pGseData)
{
	if ( pGseData == NULL )
		return 0;

	// ��ȡValue���ֵĳ���
	size_t valLen = 0;
	switch (pGseData->type)
	{
	case GSEDT_BOOLEAN:
		{
			valLen = 1;
		}
		break;
	case GSEDT_BIT_STR:
		{
			GOOSE_BIT_STRING *tmp = (GOOSE_BIT_STRING *)pGseData;
			if ( tmp->bitStr == NULL )
				valLen = 1;
			else
				valLen = tmp->strSize+1;
		}
		break;
	case GSEDT_INT8:
		{
			valLen = 1;
		}
		break;
	case GSEDT_INT16:
		{
			valLen = 2;
		}
		break;
	case GSEDT_INT24:
		{
			valLen = 3;
		}
		break;
	case GSEDT_INT32:
		{
			valLen = 4;
		}
		break;
	case GSEDT_UINT8:
		{
			valLen = 1;
		}
		break;
	case GSEDT_UINT16:
		{
			valLen = 2;
		}
		break;
	case GSEDT_UINT24:
		{
			valLen = 3;
		}
		break;
	case GSEDT_UINT32:
		{
			valLen = 4;
		}
		break;
	case GSEDT_FLOAT:
		{
			valLen = 5;
		}
		break;
	case GSEDT_OCTET_STR:
		{
			GOOSE_OCTET_STRING *tmp = (GOOSE_OCTET_STRING *)pGseData;
			if ( tmp->octetStr == NULL )
				valLen = 0;
			else
				valLen = tmp->strSize;
		}
		break;
	case GSEDT_VISIBLE_STR:
		{
			GOOSE_VISIBLE_STRING *tmp = (GOOSE_VISIBLE_STRING *)pGseData;
			if ( tmp->visibleStr == NULL )
				valLen = 0;
			else
				valLen = tmp->strSize;
		}
		break;
	case GSEDT_UTC_TIME:
		{
			valLen = 8;
		}
		break;
	case GSEDT_STRUCT:
		{
			GOOSE_STRUCT *tmp = (GOOSE_STRUCT *)pGseData;
			std::vector<GOOSE_DATA *>::iterator it;
			size_t len = 0;
			for ( it = tmp->member.begin(); it != tmp->member.end(); it++ )
			{
				len += GooseDataTLVBytes((*it));
			}
			valLen = len;
		}
		break;
	case GSEDT_UNKNOW:
		{
			GOOSE_UNKNOW *tmp = (GOOSE_UNKNOW *)pGseData;
			if ( tmp->datVal == NULL )
				valLen = 0;
			else
				valLen = tmp->datLen;
		}
		break;
	default:
		{
			valLen =  0;
		}
	}

	// ����Tag���ֺ�Len���ֵĳ���
	if ( valLen > 127 )
	{
		if ( ( valLen & 0xFF000000 ) != 0 )
			valLen += 1 + 1 + 4;
		if ( ( valLen & 0x00FF0000 ) != 0 )
			valLen += 1 + 1 + 3;
		if ( ( valLen & 0x0000FF00 ) != 0 )
			valLen += 1 + 1 + 2;
		if ( ( valLen & 0x000000FF ) != 0 )
			valLen += 1 + 1 + 1;
	}
	else
		valLen += 1 + 1;
	return valLen;
}

///
/// @brief
///    ���UTCʱ���ַ�������ʽΪHH:MM:SS.MMM
///
/// @return
///    std::wstring    UTCʱ���ַ���
///
std::wstring tagEventTime::GetString()
{
	tm          stTm1;
	wchar_t     cBuffer[128]    = {0};


	CTime       stTm2((__time64_t)ts_s);
	stTm2.GetGmtTm(&stTm1);

	// ��ʽ����ʱ���ַ���
	swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]), L"%02d:%02d:%02d.%03d", 
		stTm1.tm_hour,
		stTm1.tm_min,
		stTm1.tm_sec,
		ts_ns/1000000
		);

	return cBuffer;
}

///
/// @brief
///     �������������
///
/// @param[in]  stOldTime ��ʱ�����
///
/// @return
///     tagEventTime    ʱ������
///
tagEventTime tagEventTime::operator-(const tagEventTime& stOldTime)
{
    tagEventTime    stRet;


	// �Ƚ�����ֵ���ж��Ƿ�ᷢ����λ
	if (ts_ns >= stOldTime.ts_ns)
	{
		stRet.ts_s  = ts_s - stOldTime.ts_s;                            ///< ��
		stRet.ts_ns = ts_ns - stOldTime.ts_ns;                          ///< ����
		stRet.ts_q  = stOldTime.ts_q;                                   ///< Ʒ��
	}
	else
	{
		stRet.ts_s  = (ts_s - 1) - stOldTime.ts_s;
		stRet.ts_ns = 1000000000 + ts_ns - stOldTime.ts_ns;
		stRet.ts_q  = stOldTime.ts_q;
	}

	return stRet;
}

///
/// @brief
///     �ӷ����������
///
/// @param[in]  stAddTime ���ӵ�ʱ��
///
/// @return
///     tagEventTime    ʱ������
///
tagEventTime tagEventTime::operator+( const tagEventTime& stAddTime )
{
    tagEventTime    stRet;


	uint32_t uiTotalNs = ts_ns + stAddTime.ts_ns;
	if (uiTotalNs >= 1000000000)
	{
		stRet.ts_s  = ts_s + stAddTime.ts_s + 1;
		stRet.ts_ns = uiTotalNs - 1000000000;
	}
	else
	{
		stRet.ts_s  = ts_s + stAddTime.ts_s;
		stRet.ts_ns = uiTotalNs;
	}

	return stRet;
}

///
/// @brief
///     ����GOOSE�����е�ʹ��ASN.1��TLV��ʽ��GOOSE���ݷ�����λ��ʱ����Ϣ
///
/// @param[in]
///     pCodeDataBuf - ����������Ļ���
/// @param[in]
///     bufSize      - �����С
///
/// @return
///     ��������ʵ�ʴ�С
///
size_t tagEventTime::CodeGooseEventTime(uint8_t *pCodeDataBuf, size_t bufSize)
{
	if ( pCodeDataBuf == NULL || bufSize < 10 )
		return 0;

	pCodeDataBuf[0] = GOOSE_TAG_EVENT_TIME;
	pCodeDataBuf[1] = 8;
	uint32_t uint32Tmp  = BSWAP_32(ts_s);
	memcpy_s(&pCodeDataBuf[2],bufSize-2,(uint8_t *)&uint32Tmp,4);
	uint32Tmp = ((uint32_t)((uint64_t)ts_ns*(1<<28)/(1000000000)))<<4;
	// �ж�ֵ�Ƿ��1
	if ( (uint32Tmp & 0xFF) >= 0x80 )
		uint32Tmp += 0x100;
	uint32Tmp = uint32Tmp & 0xFFFFFF00;
	uint32Tmp = BSWAP_32(uint32Tmp);
	memcpy_s(&pCodeDataBuf[6],bufSize-6,(uint8_t *)&uint32Tmp,3);
	pCodeDataBuf[9] = ts_q;
	return (1+1+8);
}

///
/// @brief
///     ����λ��
///
/// @param[in]  strNew  �µ�λ���ַ���
///
/// @return  
///     void    
///
void tagGooseBitString::SetString(std::wstring strNew)
{
	// �ͷ��ϵ�λ���ռ�
	if (NULL != bitStr)
	{
		delete[] bitStr;

		bitStr  = NULL;
		strSize = 0;
		padding = 0;
	}

	// �����µ�λ���ռ�
	bitStr = new uint8_t[(strNew.size() + 7)/8];
	if (bitStr)
	{
		strSize = (uint32_t)(strNew.size() + 7)/8;

		// ����λ��
		memset(bitStr, 0, strSize);
		for (uint8_t i = 0; i < strNew.size(); i++)
		{
			if (L'1' == strNew[i])
			{
				bitStr[i/8] |= (1 << (7 - (i%8)));
			}
		}

		padding = (uint8_t)(strSize * 8 - strNew.size());
	}
}

///
/// @brief
///     ���λ���ַ���
///
/// @return
///     std::wstring    λ���ַ���
///
std::wstring tagGooseBitString::GetString()
{
	std::wstring strRet = L"";
	for(uint8_t i = 0; i < ((strSize*8 - padding) + 7)/8; i++)
	{
		if ((((strSize*8 - padding) + 7)/8 - 1) == i)
		{
			// ���һ��δ��������ֽ�
			for (int j = 7; j >= padding%8; j--)
			{
				if ((bitStr[i] >> j) & 0x01)
				{
					strRet.push_back(L'1');
				}
				else
				{
					strRet.push_back(L'0');
				}
			}
		}
		else
		{
			// ǰN���������ֽ�
			for (int j = 7; j >=0; j--)
			{
				if ((bitStr[i] >> j) & 0x01)
				{
					strRet.push_back(L'1');
				}
				else
				{
					strRet.push_back(L'0');
				}
			}
		}

	}

	return strRet;
}

///
/// @brief
///    ���UTCʱ��
///
///
/// @return
///     SYSTEMTIME    UTCʱ��
///
SYSTEMTIME tagGooseUtcTime::GetUtcTime()
{
	SYSTEMTIME  sysTime;
	CTime       stTm1((__time64_t)t_s);
	tm          stTm2;


	stTm1.GetGmtTm(&stTm2);
	sysTime.wYear           = (WORD)stTm2.tm_year + 1900;
	sysTime.wMonth          = (WORD)stTm2.tm_mon + 1;
	sysTime.wDay            = (WORD)stTm2.tm_mday;
	sysTime.wHour           = (WORD)stTm2.tm_hour;
	sysTime.wMinute         = (WORD)stTm2.tm_min;
	sysTime.wSecond         = (WORD)stTm2.tm_sec;
	sysTime.wDayOfWeek      = (WORD)stTm2.tm_wday;
	sysTime.wMilliseconds   = (WORD)(t_ns/1000000);

	return sysTime;
}

///
/// @brief
///    ���UTCʱ���ַ���
///
///
/// @return
///     std::wstring    UTCʱ���ַ���
///
std::wstring tagGooseUtcTime::GetUtcString()
{
	tm          stTm1;
	wchar_t     cBuffer[128]    = {0};


	CTime       stTm2((__time64_t)t_s);
	stTm2.GetGmtTm(&stTm1);

	// ��ʽ����ʱ���ַ���
	swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]), L"%04d-%02d-%02d %02d:%02d:%02d.%03d", 
		stTm1.tm_year + 1900,
		stTm1.tm_mon + 1,
		stTm1.tm_mday,
		stTm1.tm_hour,
		stTm1.tm_min,
		stTm1.tm_sec,
		t_ns/1000000
		);

	return cBuffer;
}

///
/// @brief
///    ���ָ��ʱ�����1970-01-01 00:00:00������
///
/// @param[in]  st    ָ����ʱ��ṹ��
///
/// @return
///     time_t    ��������
///
time_t tagGooseUtcTime::SystemTimeToTimet( SYSTEMTIME st )
{
	FILETIME ft;
	SystemTimeToFileTime( &st, &ft );

	ULARGE_INTEGER ui;
	ui.LowPart  = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;

	return (time_t)((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
}

///
/// @brief
///    ����UTCʱ��
///
/// @param[in]  sysTime    UTCʱ��
///
void tagGooseUtcTime::SetUtcTime(SYSTEMTIME sysTime)
{
	t_s     = (uint32_t)SystemTimeToTimet(sysTime);
	t_ns    = sysTime.wMilliseconds * 1000000;
}

///
/// @brief
///    ����UTCʱ��
///
/// @param[in]  strTime    UTCʱ���ַ������꣺�£��� ʱ���֣���.���룩
///
void tagGooseUtcTime::SetUctTime(std::wstring strTime)
{
	SYSTEMTIME          sysTime;


	// ����ַ����е��ꡢ�¡��ա�ʱ���֡��롢����
	int iRet = swscanf_s(strTime.c_str(), L"%d-%d-%d %d:%d:%d.%d", 
		&sysTime.wYear, 
		&sysTime.wMonth, 
		&sysTime.wDay, 
		&sysTime.wHour, 
		&sysTime.wMinute, 
		&sysTime.wSecond, 
		&sysTime.wMilliseconds);
	if (7 == iRet)
	{
		SetUtcTime(sysTime);
	}
}

///
/// @brief
///     Ĭ�Ϲ��캯��
///
GOOSE_DATA_VARIANT::GOOSE_DATA_VARIANT()
{
	// ��λ���б���ֵ
	memset(this, 0, sizeof(GOOSE_DATA_VARIANT));
	m_stUnKnown.id.type = GSEDT_UNKNOW;
}

///
/// @brief
///     �������캯��
///
GOOSE_DATA_VARIANT::GOOSE_DATA_VARIANT(const GOOSE_DATA_VARIANT& stGooseDataVariant)
{
	// ��λ���б���ֵ
	memset(this, 0, sizeof(GOOSE_DATA_VARIANT));
	m_stUnKnown.id.type = GSEDT_UNKNOW;
    m_stUnKnown.id.offset = stGooseDataVariant.m_stUnKnown.id.offset;

	// �������Ϳ����ṹ���ֵ
	switch(stGooseDataVariant.m_stUnKnown.id.type)
	{
	case GSEDT_UNKNOW:
		{
			// ��ȿ���
			m_stUnKnown = stGooseDataVariant.m_stUnKnown;
			m_stUnKnown.datVal = new uint8_t[m_stUnKnown.datLen];
			if (m_stUnKnown.datVal)
			{
				memset(m_stUnKnown.datVal, 0, m_stUnKnown.datLen);
				memcpy_s(
					m_stUnKnown.datVal,
					m_stUnKnown.datLen,
					stGooseDataVariant.m_stUnKnown.datVal,
					stGooseDataVariant.m_stUnKnown.datLen
					);
			}
		}
		break;

	case GSEDT_BOOLEAN:
		{
			m_stBoolean = stGooseDataVariant.m_stBoolean;
		}
		break;

	case GSEDT_BIT_STR:
		{
			// ��ȿ���
			m_stBitString           = stGooseDataVariant.m_stBitString;
			m_stBitString.bitStr    = new uint8_t[m_stBitString.strSize];
			if (m_stBitString.bitStr)
			{
				memset(m_stBitString.bitStr, 0, m_stBitString.strSize);
				memcpy_s(
					m_stBitString.bitStr,
					m_stBitString.strSize,
					stGooseDataVariant.m_stBitString.bitStr,
					stGooseDataVariant.m_stBitString.strSize
					);
			}
		}
		break;

	case GSEDT_INT8:
		{
			m_stInt8    = stGooseDataVariant.m_stInt8;
		}
		break;

	case GSEDT_INT16:
		{
			m_stInt16   = stGooseDataVariant.m_stInt16;
		}
		break;

	case GSEDT_INT24:
		{
			m_stInt24   = stGooseDataVariant.m_stInt24;
		}
		break;

	case GSEDT_INT32:
		{
			m_stInt32   = stGooseDataVariant.m_stInt32;
		}
		break;

	case GSEDT_UINT8:
		{
			m_stUInt8   = stGooseDataVariant.m_stUInt8;
		}

	case GSEDT_UINT16:
		{
			m_stUInt16  = stGooseDataVariant.m_stUInt16;
		}
		break;

	case GSEDT_UINT24:
		{
			m_stUInt24  = stGooseDataVariant.m_stUInt24;
		}
		break;

	case GSEDT_UINT32:
		{
			m_stUInt32  = stGooseDataVariant.m_stUInt32;
		}
		break;

	case GSEDT_FLOAT:
		{
			m_stFloat   = stGooseDataVariant.m_stFloat;
		}
		break;

	case GSEDT_OCTET_STR:
		{
			// ��ȿ���
			m_stOctetString             = stGooseDataVariant.m_stOctetString;
			m_stOctetString.octetStr    = new uint8_t[m_stOctetString.strSize];
			if (m_stOctetString.octetStr)
			{
				memset(m_stOctetString.octetStr, 0, m_stOctetString.strSize);
				memcpy_s(
					m_stOctetString.octetStr, 
					m_stOctetString.strSize,
					stGooseDataVariant.m_stOctetString.octetStr,
					stGooseDataVariant.m_stOctetString.strSize);
			}
		}
		break;

	case GSEDT_VISIBLE_STR:
		{
			// ��ȿ���
			m_stVisibleString               = stGooseDataVariant.m_stVisibleString;
			m_stVisibleString.visibleStr    = new int8_t[m_stVisibleString.strSize];
			if (m_stVisibleString.visibleStr)
			{
				memset(m_stVisibleString.visibleStr, 0, m_stVisibleString.strSize);
				memcpy_s(
					m_stVisibleString.visibleStr,
					m_stVisibleString.strSize,
					stGooseDataVariant.m_stVisibleString.visibleStr,
					stGooseDataVariant.m_stVisibleString.strSize
					);
			}
		}
		break;

	case GSEDT_UTC_TIME:
		{
			m_stUtcTime     = stGooseDataVariant.m_stUtcTime;
		}
		break;
	}
}

///
/// @brief
///    ���캯��
///
/// @param[in]  pstGooseData    ָ��GOOSE_DATA�����ָ��
///
GOOSE_DATA_VARIANT::GOOSE_DATA_VARIANT(GOOSE_DATA* pstGooseData)
{
    // �������Ϊ�գ���ôֱ�ӷ���
    if (NULL == pstGooseData)
    {
        return;
    }

    // ��λ���б���ֵ
    memset(this, 0, sizeof(GOOSE_DATA_VARIANT));
    m_stUnKnown.id.type = GSEDT_UNKNOW;
    m_stUnKnown.id.offset = pstGooseData->offset;

	// �������Ϳ����ṹ���ֵ
	switch(pstGooseData->type)
	{
	case GSEDT_UNKNOW:
		{
			GOOSE_UNKNOW* pstGooseUnknown;


			// ��ȿ���
			pstGooseUnknown     = (GOOSE_UNKNOW*)pstGooseData;
			m_stUnKnown         = *pstGooseUnknown;
			m_stUnKnown.datVal  = new uint8_t[m_stUnKnown.datLen];
			if (m_stUnKnown.datVal)
			{
				memset(m_stUnKnown.datVal, 0, m_stUnKnown.datLen);
				memcpy_s(
					m_stUnKnown.datVal,
					m_stUnKnown.datLen,
					pstGooseUnknown->datVal,
					pstGooseUnknown->datLen
					);
			}
		}
		break;

	case GSEDT_BOOLEAN:
		{
			GOOSE_BOOLEAN* pstGooseBoolean;


			pstGooseBoolean = (GOOSE_BOOLEAN*)pstGooseData;
			m_stBoolean     = *pstGooseBoolean;
		}
		break;

	case GSEDT_BIT_STR:
		{
			GOOSE_BIT_STRING* pstGooseBitString;


			// ��ȿ���
			pstGooseBitString       = (GOOSE_BIT_STRING*)pstGooseData;
			m_stBitString           = *pstGooseBitString;
			m_stBitString.bitStr    = new uint8_t[m_stBitString.strSize];
			if (m_stBitString.bitStr)
			{
				memset(m_stBitString.bitStr, 0, m_stBitString.strSize);
				memcpy_s(
					m_stBitString.bitStr,
					m_stBitString.strSize,
					pstGooseBitString->bitStr,
					pstGooseBitString->strSize
					);
			}
		}
		break;

	case GSEDT_INT8:
		{
			GOOSE_INT8* pstGooseInt8;


			pstGooseInt8    = (GOOSE_INT8*)pstGooseData;
			m_stInt8        = *pstGooseInt8;
		}
		break;

	case GSEDT_INT16:
		{
			GOOSE_INT16* pstGooseInt16;


			pstGooseInt16   = (GOOSE_INT16*)pstGooseData;
			m_stInt16       = *pstGooseInt16;
		}
		break;

	case GSEDT_INT24:
		{
			GOOSE_INT24* pstGooseInt24;


			pstGooseInt24   = (GOOSE_INT24*)pstGooseData;
			m_stInt24       = *pstGooseInt24;
		}
		break;

	case GSEDT_INT32:
		{
			GOOSE_INT32* pstGooseInt32;


			pstGooseInt32   = (GOOSE_INT32*)pstGooseData;
			m_stInt32       = *pstGooseInt32;
		}
		break;

	case GSEDT_UINT8:
		{
			GOOSE_UINT8* pstGooseUint8;


			pstGooseUint8   = (GOOSE_UINT8*)pstGooseData;
			m_stUInt8       = *pstGooseUint8;
		}

	case GSEDT_UINT16:
		{
			GOOSE_UINT16* pstGooseUint16;


			pstGooseUint16  = (GOOSE_UINT16*)pstGooseData;
			m_stUInt16      = *pstGooseUint16;
		}
		break;

	case GSEDT_UINT24:
		{
			GOOSE_UINT24* pstGooseUint24;


			pstGooseUint24  = (GOOSE_UINT24*)pstGooseData;
			m_stUInt24      = *pstGooseUint24;
		}
		break;

	case GSEDT_UINT32:
		{
			GOOSE_UINT32* pstGooseUint32;


			pstGooseUint32  = (GOOSE_UINT32*)pstGooseData;
			m_stUInt32      = *pstGooseUint32;
		}
		break;

	case GSEDT_FLOAT:
		{
			GOOSE_FLOAT* pstGooseFloat;


			pstGooseFloat   = (GOOSE_FLOAT*)pstGooseData;
			m_stFloat       = *pstGooseFloat;
		}
		break;

	case GSEDT_OCTET_STR:
		{
			GOOSE_OCTET_STRING* pGooseOctetString;


			// ��ȿ���
			pGooseOctetString           = (GOOSE_OCTET_STRING*)pstGooseData;
			m_stOctetString             = *pGooseOctetString;
			m_stOctetString.octetStr    = new uint8_t[m_stOctetString.strSize];
			if (m_stOctetString.octetStr)
			{
				memset(m_stOctetString.octetStr, 0, m_stOctetString.strSize);
				memcpy_s(
					m_stOctetString.octetStr, 
					m_stOctetString.strSize,
					pGooseOctetString->octetStr,
					pGooseOctetString->strSize);
			}
		}
		break;

	case GSEDT_VISIBLE_STR:
		{
			GOOSE_VISIBLE_STRING* pGooseVisibleString;


			// ��ȿ���
			pGooseVisibleString             = (GOOSE_VISIBLE_STRING*)pstGooseData;
			m_stVisibleString               = *pGooseVisibleString;
			m_stVisibleString.visibleStr    = new int8_t[m_stVisibleString.strSize];
			if (m_stVisibleString.visibleStr)
			{
				memset(m_stVisibleString.visibleStr, 0, m_stVisibleString.strSize);
				memcpy_s(
					m_stVisibleString.visibleStr,
					m_stVisibleString.strSize,
					pGooseVisibleString->visibleStr,
					pGooseVisibleString->strSize
					);
			}
		}
		break;

	case GSEDT_UTC_TIME:
		{
			GOOSE_UTC_TIME* pGooseUtcTime;


			pGooseUtcTime   = (GOOSE_UTC_TIME*)pstGooseData;
			m_stUtcTime     = *pGooseUtcTime;
		}
		break;
	}
}

///
/// @brief
///    �ͷ�����������Ŀռ�
///
///
/// @return
///     void    
///
void GOOSE_DATA_VARIANT::__ReleaseAllData()
{
	switch (m_stUnKnown.id.type)
	{
	case GSEDT_UNKNOW:
		{
			if ( NULL != m_stUnKnown.datVal )
			{
				delete[] m_stUnKnown.datVal;
				m_stUnKnown.datVal = NULL;
			}
		}
		break;

	case GSEDT_BIT_STR:
		{
			if ( NULL != m_stBitString.bitStr )
			{
				delete[] m_stBitString.bitStr;
				m_stBitString.bitStr = NULL;
			}
		}
		break;

	case GSEDT_OCTET_STR:
		{
			if ( NULL != m_stOctetString.octetStr )
			{
				delete[] m_stOctetString.octetStr;
				m_stOctetString.octetStr = NULL;
			}
		}
		break;

	case GSEDT_VISIBLE_STR:
		{
			if ( NULL != m_stVisibleString.visibleStr )
			{
				delete[] m_stVisibleString.visibleStr;
				m_stVisibleString.visibleStr = NULL;
			}
		}
		break;
	}

	// ��λ���б���ֵ
	memset(this, 0, sizeof(GOOSE_DATA_VARIANT));
}

///
/// @brief
///    ���ظ�ֵ������
///
/// @param[in]  stGooseDataVariant    GOOSEͨ��ֵ���ܱ���������
///
/// @return
///     GOOSE_DATA_VARIANT&    GOOSEͨ��ֵ���ܱ���������
///
GOOSE_DATA_VARIANT& GOOSE_DATA_VARIANT::operator=(const GOOSE_DATA_VARIANT& stGooseDataVariant)
{
	// ��λ���б���ֵ
	memset(this, 0, sizeof(GOOSE_DATA_VARIANT));
	m_stUnKnown.id.type = GSEDT_UNKNOW;
    m_stUnKnown.id.offset = stGooseDataVariant.m_stUnKnown.id.offset;

	// �������Ϳ����ṹ���ֵ
	switch(stGooseDataVariant.m_stUnKnown.id.type)
	{
	case GSEDT_UNKNOW:
		{
			// ��ȿ���
			m_stUnKnown = stGooseDataVariant.m_stUnKnown;
			m_stUnKnown.datVal = new uint8_t[m_stUnKnown.datLen];
			if (m_stUnKnown.datVal)
			{
				memset(m_stUnKnown.datVal, 0, m_stUnKnown.datLen);
				memcpy_s(
					m_stUnKnown.datVal,
					m_stUnKnown.datLen,
					stGooseDataVariant.m_stUnKnown.datVal,
					stGooseDataVariant.m_stUnKnown.datLen
					);
			}
		}
		break;

	case GSEDT_BOOLEAN:
		{
			m_stBoolean = stGooseDataVariant.m_stBoolean;
		}
		break;

	case GSEDT_BIT_STR:
		{
			// ��ȿ���
			m_stBitString           = stGooseDataVariant.m_stBitString;
			m_stBitString.bitStr    = new uint8_t[m_stBitString.strSize];
			if (m_stBitString.bitStr)
			{
				memset(m_stBitString.bitStr, 0, m_stBitString.strSize);
				memcpy_s(
					m_stBitString.bitStr,
					m_stBitString.strSize,
					stGooseDataVariant.m_stBitString.bitStr,
					stGooseDataVariant.m_stBitString.strSize
					);
			}
		}
		break;

	case GSEDT_INT8:
		{
			m_stInt8    = stGooseDataVariant.m_stInt8;
		}
		break;

	case GSEDT_INT16:
		{
			m_stInt16   = stGooseDataVariant.m_stInt16;
		}
		break;

	case GSEDT_INT24:
		{
			m_stInt24   = stGooseDataVariant.m_stInt24;
		}
		break;

	case GSEDT_INT32:
		{
			m_stInt32   = stGooseDataVariant.m_stInt32;
		}
		break;

	case GSEDT_UINT8:
		{
			m_stUInt8   = stGooseDataVariant.m_stUInt8;
		}

	case GSEDT_UINT16:
		{
			m_stUInt16  = stGooseDataVariant.m_stUInt16;
		}
		break;

	case GSEDT_UINT24:
		{
			m_stUInt24  = stGooseDataVariant.m_stUInt24;
		}
		break;

	case GSEDT_UINT32:
		{
			m_stUInt32  = stGooseDataVariant.m_stUInt32;
		}
		break;

	case GSEDT_FLOAT:
		{
			m_stFloat   = stGooseDataVariant.m_stFloat;
		}
		break;

	case GSEDT_OCTET_STR:
		{
			// ��ȿ���
			m_stOctetString             = stGooseDataVariant.m_stOctetString;
			m_stOctetString.octetStr    = new uint8_t[m_stOctetString.strSize];
			if (m_stOctetString.octetStr)
			{
				memset(m_stOctetString.octetStr, 0, m_stOctetString.strSize);
				memcpy_s(
					m_stOctetString.octetStr, 
					m_stOctetString.strSize,
					stGooseDataVariant.m_stOctetString.octetStr,
					stGooseDataVariant.m_stOctetString.strSize);
			}
		}
		break;

	case GSEDT_VISIBLE_STR:
		{
			// ��ȿ���
			m_stVisibleString               = stGooseDataVariant.m_stVisibleString;
			m_stVisibleString.visibleStr    = new int8_t[m_stVisibleString.strSize];
			if (m_stVisibleString.visibleStr)
			{
				memset(m_stVisibleString.visibleStr, 0, m_stVisibleString.strSize);
				memcpy_s(
					m_stVisibleString.visibleStr,
					m_stVisibleString.strSize,
					stGooseDataVariant.m_stVisibleString.visibleStr,
					stGooseDataVariant.m_stVisibleString.strSize
					);
			}
		}
		break;

	case GSEDT_UTC_TIME:
		{
			m_stUtcTime     = stGooseDataVariant.m_stUtcTime;
		}
		break;
	}

	return *this;
}

///
/// @brief
///     ������
///
bool GOOSE_DATA_VARIANT::operator !=(GOOSE_DATA_VARIANT& stGooseDataVariant) const
{
    switch(stGooseDataVariant.m_stUnKnown.id.type)
    {
    case GSEDT_UNKNOW:
        {
            // �Ƚ������ֽ���
            if (m_stUnKnown.datLen != stGooseDataVariant.m_stUnKnown.datLen)
            {
                return true;
            }

            // �Ƚ�����
            if (memcmp(m_stUnKnown.datVal, stGooseDataVariant.m_stUnKnown.datVal, m_stUnKnown.datLen))
            {
                return true;
            }
        }
        break;

    case GSEDT_BOOLEAN:
        {
            // ��ʱֻ�Ƚϲ���ֵ
            return (m_stBoolean.boolVal != stGooseDataVariant.m_stBoolean.boolVal);
        }
        break;

    case GSEDT_BIT_STR:
        {
            // �Ƚ������ֽ���
            if (m_stBitString.strSize != stGooseDataVariant.m_stBitString.strSize)
            {
                return true;
            }

            // �Ƚ�����
            if (memcmp(m_stBitString.bitStr, stGooseDataVariant.m_stBitString.bitStr, m_stBitString.strSize))
            {
                return true;
            }
        }
        break;

    case GSEDT_INT8:
        {
            return m_stInt8.datVal != stGooseDataVariant.m_stInt8.datVal;
        }
        break;

    case GSEDT_INT16:
        {
            return m_stInt16.datVal != stGooseDataVariant.m_stInt16.datVal;
        }
        break;

    case GSEDT_INT24:
        {
            return m_stInt24.datVal != stGooseDataVariant.m_stInt24.datVal;
        }
        break;

    case GSEDT_INT32:
        {
            return m_stInt32.datVal != stGooseDataVariant.m_stInt32.datVal;
        }
        break;

    case GSEDT_UINT8:
        {
            return m_stUInt8.datVal != stGooseDataVariant.m_stUInt8.datVal;
        }

    case GSEDT_UINT16:
        {
            return m_stUInt16.datVal != stGooseDataVariant.m_stUInt16.datVal;
        }
        break;

    case GSEDT_UINT24:
        {
            return m_stUInt24.datVal != stGooseDataVariant.m_stUInt24.datVal;
        }
        break;

    case GSEDT_UINT32:
        {
            return m_stUInt32.datVal != stGooseDataVariant.m_stUInt32.datVal;
        }
        break;

    case GSEDT_FLOAT:
        {
            if (fabs(m_stFloat.datVal - stGooseDataVariant.m_stFloat.datVal) >= GOOSE_FLOAT_PRECISION)
            {
                return true;
            }
        }
        break;

    case GSEDT_OCTET_STR:
        {
            // �Ƚ������ֽ���
            if (m_stOctetString.strSize != stGooseDataVariant.m_stOctetString.strSize)
            {
                return true;
            }

            // �Ƚ�����
            if (memcmp(m_stOctetString.octetStr, stGooseDataVariant.m_stOctetString.octetStr, m_stOctetString.strSize))
            {
                return true;
            }
        }
        break;

    case GSEDT_VISIBLE_STR:
        {
            if (m_stVisibleString.strSize != stGooseDataVariant.m_stVisibleString.strSize)
            {
                return true;
            }

            if (memcmp(m_stVisibleString.visibleStr, stGooseDataVariant.m_stVisibleString.visibleStr, m_stVisibleString.strSize))
            {
                return true;
            }
        }
        break;

    case GSEDT_UTC_TIME:
        {
            return ((m_stUtcTime.t_s != stGooseDataVariant.m_stUtcTime.t_s)
                 || (m_stUtcTime.t_ns != stGooseDataVariant.m_stUtcTime.t_ns));
        }
        break;
    }

    return false;
}

///
/// @brief
///    ������������
///
/// @param[in]  eNewDataType    �µ���������
///
void GOOSE_DATA_VARIANT::__SetType(GOOSE_DATA_TYPE eNewDataType)
{
	if (m_stUnKnown.id.type != eNewDataType)
	{
		// �ͷ���ǰ������Ŀռ�
		__ReleaseAllData();

		// ��������
		m_stUnKnown.id.type = eNewDataType;
		switch(eNewDataType)
		{
		case GSEDT_UTC_TIME:
			{
				CTime           tm1;
				SYSTEMTIME      sysTime;
				SYSTEMTIME      utcTime;


				// Ĭ�ϳ�ʼ��Ϊϵͳ��ǰʱ��
				tm1 = CTime::GetCurrentTime();
				tm1.GetAsSystemTime(sysTime);

				// ������ʱ��תΪUTCʱ��
				DM_TzSpecificLocalTimeToSystemTime(&sysTime, &utcTime);
				m_stUtcTime.SetUtcTime(utcTime);
			}
			break;
		}
	}
}

///
/// @brief
///    ����ַ���
///
/// @return
///     std::wstring    �ַ���
///
std::wstring GOOSE_DATA_VARIANT::GetString()
{
    std::wstring    strRet;


    switch(m_stUnKnown.id.type)
    {
    case GSEDT_BOOLEAN:
        {
            if (m_stBoolean.boolVal)
            {
                strRet = _T("true");
            }
            else
            {
                strRet = _T("false");
            }
        }
        break;

    case GSEDT_BIT_STR:
        {
            strRet = m_stBitString.GetString();
        }
        break;

    case GSEDT_UTC_TIME:
        {
            strRet = m_stUtcTime.GetUtcString();
        }
        break;
    }

    return strRet;
}

///
/// @brief
///     ����ASN.1��TLV��ʽ�����е�Len����
///
/// @param[in]
///     eNewDataType - �µ���������
/// @param[in]
///     pCodeDataBuf - ����������Ļ���
/// @param[in]
///     bufSize      - �����С
///
/// @return
///     ��������ʵ�ʴ�С
///
size_t GOOSE_DATA_VARIANT::CodeLenOfTLV(size_t valLenOfTLV, uint8_t *pCodeDataBuf, size_t bufSize)
{
	// �жϳ���ֵ��L�������õĸ�ʽ����/�̸�ʽ��������䳤��ֵ
	if ( valLenOfTLV < 0x80 )
	{
		pCodeDataBuf[0] = (uint8_t)valLenOfTLV;
		return 1;
	}
	else
	{
		int lByte;
		if ( ( valLenOfTLV & 0xFF000000 ) != 0 )
			lByte = 4;
		else if ( ( valLenOfTLV & 0x00FF0000 ) != 0 )
			lByte = 3;
		else if ( ( valLenOfTLV & 0x0000FF00 ) != 0 )
			lByte = 2;
		else if ( ( valLenOfTLV & 0x000000FF ) != 0 )
			lByte = 1;
		else
			lByte = 0;
		pCodeDataBuf[0] = 0x80 + lByte;
		for ( int i = 0; i < lByte; i++ )
		{
			pCodeDataBuf[1+lByte-i] = (uint8_t) ( ( valLenOfTLV << ( (3-i)*8 ) ) >> 3*8 ); 
		}
		return (1+lByte);
	}
}

///
/// @brief
///     ����GOOSE�����е�ʹ��ASN.1��TLV��ʽ�Ļ�����������
///
/// @param[in]
///     pCodeDataBuf - ����������Ļ���
/// @param[in]
///     bufSize      - �����С
///
/// @return
///     ��������ʵ�ʴ�С
///
size_t GOOSE_DATA_VARIANT::CodeGooseData(uint8_t *pCodeDataBuf, size_t bufSize)
{
	if ( pCodeDataBuf == NULL || bufSize < 3 )
		return 0;

	int     valueLen = 0;
	switch (m_stUnKnown.id.type)
	{
	case GSEDT_BOOLEAN:
		{
			pCodeDataBuf[0] = GOOSE_DATA_TAG_BOOLEAN;
			pCodeDataBuf[1] = 1;
			pCodeDataBuf[2] = (uint8_t)m_stBoolean.boolVal;
			return (1+1+1);
		}
		break;
	case GSEDT_BIT_STR:
		{
			pCodeDataBuf[0] = GOOSE_DATA_TAG_BIT_STR;
			valueLen = m_stBitString.strSize+1 < bufSize ? m_stBitString.strSize+1:bufSize;
			size_t lenOfTLVSz = CodeLenOfTLV(valueLen,&pCodeDataBuf[1],bufSize-1); 
			pCodeDataBuf[1+lenOfTLVSz] = m_stBitString.padding;
			memcpy_s(&pCodeDataBuf[2+lenOfTLVSz],bufSize-2-lenOfTLVSz,m_stBitString.bitStr,valueLen-1);
			return (1+lenOfTLVSz+valueLen);
		}
		break;
	case GSEDT_INT8:
		{
			pCodeDataBuf[0] = GOOSE_DATA_TAG_INTEGER;
			pCodeDataBuf[1] = 1;
			pCodeDataBuf[2] = m_stInt8.datVal;
			return (1+1+1);
		}
		break;
	case GSEDT_INT16:
		{
			pCodeDataBuf[0] = GOOSE_DATA_TAG_INTEGER;
			pCodeDataBuf[1] = 2;
			int16_t intTmp = BSWAP_16(m_stInt16.datVal);
			memcpy_s(&pCodeDataBuf[2],bufSize-2,&intTmp,2);
			return (1+1+2);
		}
		break;
	case GSEDT_INT24:
		{
			pCodeDataBuf[0] = GOOSE_DATA_TAG_INTEGER;
			pCodeDataBuf[1] = 3;
			int32_t intTmp = BSWAP_32(m_stInt24.datVal);
			intTmp = intTmp >> 8;
			memcpy_s(&pCodeDataBuf[2],bufSize-2,&intTmp,3);
			return (1+1+3);
		}
		break;
	case GSEDT_INT32:
		{
			pCodeDataBuf[0] = GOOSE_DATA_TAG_INTEGER;
			pCodeDataBuf[1] = 4;
			int32_t intTmp = BSWAP_32(m_stInt32.datVal);
			memcpy_s(&pCodeDataBuf[2],bufSize-2,&intTmp,4);
			return (1+1+4);
		}
		break;
	case GSEDT_UINT8:
		{
			pCodeDataBuf[0] = GOOSE_DATA_TAG_UNSIGNED;
			pCodeDataBuf[1] = 1;
			pCodeDataBuf[2] = m_stUInt8.datVal;
			return (1+1+1);
		}
		break;
	case GSEDT_UINT16:
		{
			pCodeDataBuf[0] = GOOSE_DATA_TAG_UNSIGNED;
			pCodeDataBuf[1] = 2;
			uint16_t intTmp = BSWAP_16(m_stUInt16.datVal);
			memcpy_s(&pCodeDataBuf[2],bufSize-2,&intTmp,2);
			return (1+1+2);
		}
		break;
	case GSEDT_UINT24:
		{
			pCodeDataBuf[0] = GOOSE_DATA_TAG_UNSIGNED;
			pCodeDataBuf[1] = 3;
			uint32_t intTmp = BSWAP_32(m_stUInt24.datVal);
			intTmp = intTmp >> 8;
			memcpy_s(&pCodeDataBuf[2],bufSize-2,&intTmp,3);
			return (1+1+3);
		}
		break;
	case GSEDT_UINT32:
		{
			pCodeDataBuf[0] = GOOSE_DATA_TAG_UNSIGNED;
			pCodeDataBuf[1] = 4;
			uint32_t intTmp = BSWAP_32(m_stUInt32.datVal);
			memcpy_s(&pCodeDataBuf[2],bufSize-2,&intTmp,4);
			return (1+1+4);
		}
		break;
	case GSEDT_FLOAT:
		{
			pCodeDataBuf[0] = GOOSE_DATA_TAG_FLOAT_POINT;
			pCodeDataBuf[1] = 5;
			uint32_t intTmp = *((uint32_t *)&m_stFloat.datVal);
			intTmp = BSWAP_32(intTmp);
			pCodeDataBuf[2] = 0x08;
			memcpy_s(&pCodeDataBuf[3],bufSize-3,&intTmp,sizeof(uint32_t));
			return (1+1+5);
		}
		break;
	case GSEDT_OCTET_STR:
		{
			pCodeDataBuf[0] = GOOSE_DATA_TAG_OCTET_STR;
			valueLen = m_stOctetString.strSize < bufSize ? m_stOctetString.strSize:bufSize;
			size_t lenOfTLVSz = CodeLenOfTLV(valueLen,&pCodeDataBuf[1],bufSize-1); 
			memcpy_s(&pCodeDataBuf[1+lenOfTLVSz],bufSize-1-lenOfTLVSz,m_stOctetString.octetStr,valueLen);
			return (1+lenOfTLVSz+valueLen);
		}
		break;
	case GSEDT_VISIBLE_STR:
		{
			pCodeDataBuf[0] = GOOSE_DATA_TAG_VISIBLE_STR;
			valueLen = m_stVisibleString.strSize < bufSize ? m_stVisibleString.strSize:bufSize;
			size_t lenOfTLVSz = CodeLenOfTLV(valueLen,&pCodeDataBuf[1],bufSize-1); 
			memcpy_s(&pCodeDataBuf[1+lenOfTLVSz],bufSize-1-lenOfTLVSz,m_stVisibleString.visibleStr,valueLen);
			return (1+lenOfTLVSz+valueLen);
		}
		break;
	case GSEDT_UTC_TIME:
		{
			pCodeDataBuf[0] = GOOSE_DATA_TAG_UTC_TIME;
			pCodeDataBuf[1] = 8;
			uint32_t uint32Tmp  = BSWAP_32(m_stUtcTime.t_s);
			memcpy_s(&pCodeDataBuf[2],bufSize-2,(uint8_t *)&uint32Tmp,4);
			uint32Tmp = ((uint32_t)((uint64_t)m_stUtcTime.t_ns*(1<<28)/(1000000000)))<<4;
			// �ж�ֵ�Ƿ��1
			if ( (uint32Tmp & 0xFF) >= 0x80 )
				uint32Tmp += 0x100;
			uint32Tmp = uint32Tmp & 0xFFFFFF00;
			uint32Tmp = BSWAP_32(uint32Tmp);
			memcpy_s(&pCodeDataBuf[6],bufSize-6,(uint8_t *)&uint32Tmp,3);
			pCodeDataBuf[9] = m_stUtcTime.t_q;
			return (1+1+8);
		}
		break;
	default:
		{
			pCodeDataBuf[0] = m_stUnKnown.gooseTag;
			valueLen = m_stUnKnown.datLen < bufSize ? m_stUnKnown.datLen:bufSize;
			size_t lenOfTLVSz = CodeLenOfTLV(valueLen,&pCodeDataBuf[1],bufSize-1); 
			memcpy_s(&pCodeDataBuf[1+lenOfTLVSz],bufSize-1-lenOfTLVSz,m_stUnKnown.datVal,valueLen);
			return (1+lenOfTLVSz+valueLen);
		}
	}
}

///
/// @brief
///     ����GOOSE�����е�ʹ��ASN.1��TLV��ʽ��StNum�ֶΣ��˴�����ʱstNum���ȹ̶�Ϊ4�ֽ�
///
/// @param[in]
///     stNum        - stNum��ֵ
/// @param[in]
///     pCodeDataBuf - ����������Ļ���
/// @param[in]
///     bufSize      - �����С
///
/// @return
///     ��������ʵ�ʴ�С
///
size_t CodeGooseStNum(uint32_t stNum, uint8_t *pCodeDataBuf, size_t bufSize)
{
	pCodeDataBuf[0] = GOOSE_TAG_ST_NUM;
	pCodeDataBuf[1] = 4;
	uint32_t intTmp = BSWAP_32(stNum);
	memcpy_s(&pCodeDataBuf[2],bufSize-2,&intTmp,4);
	return (1+1+4);
}


