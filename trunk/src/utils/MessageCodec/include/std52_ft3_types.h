/// @file
///     std52_ft3_types.h
///
/// @brief
///     �����׼FT3����ֵ���ģ�52�ֽڰ汾,��CRC��֡����������ʹ�õ���������
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
#ifndef STD52_FT3_TYPES_H_

#define STD52_FT3_TYPES_H_

#include "src\include\stdint.h"

///
/// @brief
///     ��׼FT3����ֵ���ģ�52�ֽڰ汾,��CRC��ÿ֡���ĳ���Ϊ26words
///
#define STD52_FT3_MSG_WORD       26

///
/// @brief
///     ��׼FT3����ֵ���ģ�52�ֽڰ汾,��CRC����Ӧ�÷������ݵ�Ԫ��ASDU���г�����ֵ��Ϣ��Ĺ�����Ϣ
///
typedef struct
{
	uint16_t asduLen;                      ///< ASDU�ĳ���
	uint8_t  lnName;                       ///< �߼��ڵ�����
	uint8_t  dataSetName;                  ///< ���ݼ�����
	uint16_t ldName;                       ///< �߼��豸����
	uint16_t phsArtg;                      ///< ������
	uint16_t neutArtg;                     ///< ����Ե�(����)����
	uint16_t phsVrtg;                      ///< ����ѹ
	uint16_t delayTime;                    ///< ���ʱʱ��
	uint16_t statusWord1;                  ///< ״̬��1
	uint16_t statusWord2;                  ///< ״̬��2
	uint16_t smpCnt;                       ///< ��������������ʾ��ǰ���͵Ĳ�������Ŀ
}STD52_FT3_ASDU_PUBLIC;

///
/// @brief
///     ��׼FT3����ֵ���ģ�52�ֽڰ汾,��CRC����Ӧ�÷������ݵ�Ԫ��ASDU���г�����ֵ��Ϣ��Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ
///
typedef struct
{
	size_t asduLenOff;                     ///< ASDU�ĳ��ȵ�ƫ�Ƶ�ַ
	size_t lnNameOff;                      ///< �߼��ڵ����Ƶ�ƫ�Ƶ�ַ
	size_t dataSetNameOff;                 ///< ���ݼ����Ƶ�ƫ�Ƶ�ַ
	size_t ldNameOff;                      ///< �߼��豸���Ƶ�ƫ�Ƶ�ַ
	size_t phsArtgOff;                     ///< ��������ƫ�Ƶ�ַ
	size_t neutArtgOff;                    ///< ����Ե������ƫ�Ƶ�ַ
	size_t phsVrtgOff;                     ///< ����ѹ��ƫ�Ƶ�ַ
	size_t delayTimeOff;                   ///< ���ʱʱ���ƫ�Ƶ�ַ
	size_t statusWord1Off;                 ///< ״̬��1��ƫ�Ƶ�ַ
	size_t statusWord2Off;                 ///< ״̬��2��ƫ�Ƶ�ַ
	size_t smpCntOff;                      ///< ������������ƫ�Ƶ�ַ
}STD52_FT3_ASDU_PUBLIC_OFFSET;

#endif
