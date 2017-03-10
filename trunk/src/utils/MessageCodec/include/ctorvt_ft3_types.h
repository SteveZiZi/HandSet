/// @file
///     ctorvt_ft3_types.h
///
/// @brief
///     �������/��ѹ������FT3����ֵ����֡����������ʹ�õ���������
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.09.05
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#ifndef CTORVT_FT3_TYPES_H_

#define CTORVT_FT3_TYPES_H_

#include "src\include\stdint.h"

///
/// @brief
///     �������/��ѹ������FT3����ֵ����ÿ֡���ĳ���Ϊ13words����CRC��
///
#define ONEPHASE_CTORVT_FT3_MSG_WORD       13

///
/// @brief
///     �������������FT3����ֵ����ÿ֡���ĳ���Ϊ17words����CRC��
///
#define TRIPHASE_CT_FT3_MSG_WORD           17

///
/// @brief
///     �����ѹ������FT3����ֵ����(2010)ÿ֡���ĳ���Ϊ14words����CRC��
///
#define TRIPHASE_VT_FT3_2010_MSG_WORD      14

///
/// @brief
///     �����ѹ������FT3����ֵ����(2011)ÿ֡���ĳ���Ϊ14words����CRC��
///
#define TRIPHASE_VT_FT3_2011_MSG_WORD      14

///
/// @brief
///     �������/��ѹ������FT3����ֵ����ÿ֡���ĳ���Ϊ24words����CRC��
///
#define TRIPHASE_CTORVT_FT3_MSG_WORD       24

///
/// @brief
///     ����/��ѹ������FT3����ֵ����ÿ֡���ĵ���󳤶ȣ���CRC��
///
#define MAX_CTORVT_FT3_MSG_WORD            TRIPHASE_CTORVT_FT3_MSG_WORD

///
/// @brief
///     ����/��ѹ������FT3����ֵ���ĵ�����
///
typedef enum{
	CFT_ONEPHASE_CTORVT = 0,
	CFT_TRIPHASE_CT,
	CFT_TRIPHASE_VT_2010,
	CFT_TRIPHASE_VT_2011,
	CFT_TRIPHASE_CTORVT
}CTORVT_FT3_TYPE;

///
/// @brief
///     ����/��ѹ������FT3����ֵ���ĵ�Ӧ�÷������ݵ�Ԫ��ASDU���г�����ֵ��ͨ��ֵ����Ϣ��Ĺ�����Ϣ
///
typedef struct
{
	uint8_t  msgType;                      ///< ��������
	int8_t   temperature;                  ///< �¶ȣ����϶ȣ�
	uint16_t delayTime;                    ///< ���ʱʱ�䣨us��
	uint16_t statusWord1;                  ///< ״̬��1
	uint16_t statusWord2;                  ///< ״̬��2
	uint16_t smpCnt;                       ///< ��������������ʾ��ǰ���͵Ĳ�������Ŀ
}CTORVT_FT3_ASDU_PUBLIC;

///
/// @brief
///     ����/��ѹ������FT3����ֵ���ĵ�Ӧ�÷������ݵ�Ԫ��ASDU���г�����ֵ��ͨ��ֵ����Ϣ��Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ
///
typedef struct
{
	size_t msgTypeOff;                     ///< �������͵�ƫ�Ƶ�ַ
	size_t temperatureOff;                 ///< �¶ȵ�ƫ�Ƶ�ַ
	size_t delayTimeOff;                   ///< ���ʱʱ���ƫ�Ƶ�ַ
	size_t statusWord1Off;                 ///< ״̬��1��ƫ�Ƶ�ַ
	size_t statusWord2Off;                 ///< ״̬��2��ƫ�Ƶ�ַ
	size_t smpCntOff;                      ///< ������������ƫ�Ƶ�ַ
}CTORVT_FT3_ASDU_PUBLIC_OFFSET;

#endif
