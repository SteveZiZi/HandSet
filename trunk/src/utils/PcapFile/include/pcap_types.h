/// @file
///     pcap_types.h
///
/// @brief
///     PCAP�ļ������ӿ����õ�����������
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
///    |2013.03.05     |1.0.1.305  |�޸�PCAP�ļ�������͵�ע��                               |
///    |2013.03.05     |1.0.1.305  |��PCAP_FILE_TIME_VAL�ĳ�Ա��Ϊ�з�������                 |
///
#ifndef PCAP_TYPES_H_

#define PCAP_TYPES_H_

#include "src\include\stdint.h"

#define MAX_ETHERNET_SIZE      1514      ///< ��̫��֡����󳤶�

///
/// @brief
///     �Զ������ͣ���ʾPCAP�ļ������ķ���ֵ
///
typedef int PCAP_FILE_RESULT;
#define PCAP_FILE_ERROR   PCAP_FILE_RESULT(-1)   ///< ������������
#define PCAP_FILE_SUCCESS PCAP_FILE_RESULT(0)    ///< �����ɹ�
#define PCAP_FILE_END     PCAP_FILE_RESULT(1)    ///< �����ļ���β

///
/// @brief
///     PCAP�ļ�ͷ��Ĭ��ֵ
///
#define PCAP_DEFAULT_MAGIC         0xA1B2C3D4
#define PCAP_DEFAULT_MAJOR_VERSION 0x0002
#define PCAP_DEFAULT_MINOR_VERSION 0x0004
#define PCAP_DEFAULT_THIS_ZONE     0x00000000
#define PCAP_DEFAULT_SIGFIGS       0x00000000
#define PCAP_DEFAULT_SANPLEN       0x0000FFFF

///
/// @brief
///     ����¼���ļ�ʹ�õ���·������
///
#define PCAP_LINK_TYPE_ETHERNET 1      ///< ��̫�����ͣ���Ӧ9-1/2��GOOSE
#define PCAP_LINK_TYPE_FT3      10     ///< FDDI���ͣ���ӦFT3

///
/// @brief
///     �ļ�����ģʽ����Ϊ����дģʽ
///
enum PCAP_FILE_MODE{PFM_READ = 0, PFM_WRITE};

#pragma pack(push, 1)

///
/// @brief
///     PCAP�ļ�ͷ�ṹ,��24���ֽ�
///
typedef struct
{
	uint32_t magic;                    ///< ��ʶλ����ʶ�ļ���ʼ���������ж��ļ��Ĵ�С�˴洢��Ĭ��ֵΪ0xA1B2C3D4��4���ֽ�
	uint16_t majorVersion;             ///< ���汾�ţ�Ĭ��ֵΪ0x02��2���ֽ�
	uint16_t minorVersion;             ///< ���汾�ţ�Ĭ��ֵΪ0x04��2���ֽ�
	uint32_t thisZone;                 ///< ����ʱ�䣨����ڸ�������ʱ��ľֲ�У������ʵ���в���ʹ�ã�Ĭ��Ϊ0��4���ֽ�
	uint32_t sigfigs;                  ///< ʱ����ľ��ȣ�ʵ���в���ʹ�ã�Ĭ��Ϊ0��4���ֽ�
	uint32_t snapLen;                  ///< PCAP�ļ������ݰ������洢���ȣ���ֵΪ0xFFFFʱ��ʾ�����������ݰ���4���ֽ�
	uint32_t linkType;                 ///< ��·�����ͣ������ݰ�����·���ͷ������4���ֽ�
}PCAP_FILE_HEADER;

///
/// @brief
///     ʱ����Ľṹ�壬��8���ֽ�
///
typedef struct
{
	int32_t timeStamp_Second;          ///< ʱ�����������4���ֽ�
	int32_t timeStamp_USec;            ///< ʱ�����΢����,����ʱ�����С�����֣�4���ֽ�
}PCAP_FILE_TIME_VAL;

///
/// @brief
///     ���ݰ�ͷ�ṹ����16���ֽ�
///
typedef struct
{
	PCAP_FILE_TIME_VAL timeStamp;      ///< �Ӹ�������ʱ���1970-1-1 00:00:00��ץ��ʱ������ʱ�䣬8���ֽ�
	uint32_t  capLen;                  ///< ��ʾ������PCAP�ļ��е����ݰ���ʵ�ʳ��ȣ�4���ֽ�
	uint32_t  len;                     ///< ��ʾ���ݰ�����ʵ���ȣ�ԭʼ���ĳ��ȣ���4���ֽ�
	                                   ///< len��capLen�����ʾ��ǰ��������ݰ�������
}PCAP_PACKET_HEADER;

#pragma pack( pop)

#endif
