/// @file
///     etypes.h
///
/// @brief
///     ������̫������֡����������ʹ�õ���������
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
#ifndef ETHERNET_TYPES_H_

#define ETHERNET_TYPES_H_

#include "src\include\stdint.h"

///
/// @brief
///     ��̫��V2��Mac��ַ�ĳ���
///
#define ETHER_MAC_LEN 6

///
/// @brief
///     ��̫��֡����󳤶�
///
#define ETHER_FRAME_MAX_LEN 1514

///
/// @brief
///     ��̫������ֵ��������ʶ������֡Ϊ��IEEE 802.1Q��ǩ������֡
///
#ifndef ETHERTYPE_VLAN
#define ETHERTYPE_VLAN		0x8100
#endif

///
/// @brief
///     ��̫������ֵ��������ʶ������֡ΪGOOSE����
///
#ifndef ETHERTYPE_IEC61850_GOOSE
#define ETHERTYPE_IEC61850_GOOSE    0x88b8
#endif

///
/// @brief
///     ��̫������ֵ��������ʶ������֡Ϊ9-1/2 Sav����
///
#ifndef ETHERTYPE_IEC61850_SV
#define ETHERTYPE_IEC61850_SV       0x88ba
#endif

///
/// @brief
///     ��̫������ֵ��������ʶ������֡ΪIEEE1588����
///
//
#ifndef ETHERTYPE_IEEE1588
#define ETHERTYPE_IEEE1588          0x88F7
#endif

///
/// @brief
///     ��̫��V2��֡ͷ��Ϣ
///
typedef struct
{
	uint8_t  desMac[ETHER_MAC_LEN];        ///< Ŀ�ĵ�ַ
	uint8_t  srcMac[ETHER_MAC_LEN];        ///< Դ��ַ
	uint16_t etherType;                    ///< ��̫��֡���ͱ�ʶ����ֵΪ0x8100ʱ��ʶ�ñ��Ĵ���IEEE802.1Q��ǩ
}ETHER_II_HEADER;

///
/// @brief
///     IEEE802.1Q��VLAN��ǩ�е�TCI(��ǩ������Ϣ)
///
typedef struct
{
	uint16_t vlanID : 12;              ///< �����������ʶ�����ڱ�Ǹñ���Ӧ�÷��͵��ĸ�VLAN
	uint16_t cfi : 1;                  ///< �淶��ʽ��ǣ��ܱ�����Ϊ0
	uint16_t userPriority : 3;         ///< �û����ȼ�	
}IEEE802_1Q_TAG_TCI;

///
/// @brief
///     IEEE802.1Q��VLAN��ǩ��Ϣ
///
typedef struct
{
	IEEE802_1Q_TAG_TCI tci;                                  ///< TCI(��ǩ������Ϣ)
	uint16_t etherType;                    ///< ��̫��֡���ͱ�ʶ
}IEEE802_1Q_VLAN_INFO;

///
/// @brief
///     ISO/IEC8802-3��̫��֡��Э�����ݵ�Ԫ(PDU)�Ĺ������ֵ���Ϣ
///
typedef struct
{
	uint16_t appID;                        ///< Ӧ����Ϣ��ʶ�룬��������Ӧ�ù�����������GOOSE��ģ��������ֵ��SAV��Э��
	uint16_t len;                          ///< PDU�ĳ��ȣ�����APPID��ʼ��APDU������ȫ���ֽ���
	uint16_t reserved1;                    ///< ����λ1
	uint16_t reserved2;                    ///< ����λ2
}IEC8802_3_PDU_PUBLIC;

///
/// @brief
///     ��̫��V2��֡ͷ��Ϣ��ƫ�Ƶ�ַ��Ϣ
///
typedef struct
{
	size_t desMacOff;                      ///< Ŀ�ĵ�ַ��ƫ�Ƶ�ַ
	size_t srcMacOff;                      ///< Դ��ַ��ƫ�Ƶ�ַ
	size_t etherTypeOff;                   ///< ��̫��֡���ͱ�ʶ��ƫ�Ƶ�ַ
}ETHER_II_HEADER_OFFSET;

///
/// @brief
///     IEEE802.1Q��VLAN��ǩ��Ϣ��ƫ�Ƶ�ַ��Ϣ
///
typedef struct
{
	size_t tciOff;                         ///< TCI(��ǩ������Ϣ)��ƫ�Ƶ�ַ
	size_t etherTypeOff;                   ///< ��̫��֡���ͱ�ʶ��ƫ�Ƶ�ַ
}IEEE802_1Q_VLAN_INFO_OFFSET;

///
/// @brief
///     ISO/IEC8802-3��̫��֡��Э�����ݵ�Ԫ(PDU)�Ĺ������ֵ���Ϣ��ƫ�Ƶ�ַ��Ϣ
///
typedef struct
{
	size_t appIDOff;                       ///< Ӧ����Ϣ��ʶ���ƫ�Ƶ�ַ
	size_t lenOff;                         ///< PDU�ĳ��ȵ�ƫ�Ƶ�ַ
	size_t reserved1Off;                   ///< ����λ1��ƫ�Ƶ�ַ
	size_t reserved2Off;                   ///< ����λ2��ƫ�Ƶ�ַ
}IEC8802_3_PDU_PUBLIC_OFFSET;

#endif
