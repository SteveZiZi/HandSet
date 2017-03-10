/// @file
///     guowang_ft3_types.h
///
/// @brief
///     �������FT3����ֵ����֡����������ʹ�õ���������
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
#ifndef GUOWANG_FT3_TYPES_H_

#define GUOWANG_FT3_TYPES_H_

#include "src\include\stdint.h"

///
/// @brief
///     ����FT3����ֵ���ģ���CRC��ÿ֡���ĳ���Ϊ37words
///
#define GUOWANG_FT3_MSG_WORD      37

///
/// @brief
///     ����FT3����ֵ���ĵ�Ӧ�÷������ݵ�Ԫ��ASDU���г�����ֵ��Ϣ��Ĺ�����Ϣ
///
typedef struct
{
	uint16_t asduLen;                      ///< ASDU�ĳ���
	uint8_t  lnName;                       ///< �߼��ڵ�����
	uint8_t  dataSetName;                  ///< ���ݼ�����
	uint16_t ldName;                       ///< �߼��豸����
	uint16_t phsArtg;                      ///< ������
	uint16_t neutArtg;                     ///< ����Ե����
	uint16_t phsVrtg;                      ///< ����ѹ
	uint16_t delayTime;                    ///< ���ʱʱ��
	uint16_t statusWord1;                  ///< ״̬��1
	uint16_t statusWord2;                  ///< ״̬��2
	uint16_t smpCnt;                       ///< ��������������ʾ��ǰ���͵Ĳ�������Ŀ
	// 	uint8_t  smpRate;                      ///< �����Ƶ����ÿ�ܲ�ʱ��������Ĳ������ݼ���Ŀ
	// 	uint8_t  confRev;                      ///< ���ð汾��
}GUOWANG_FT3_ASDU_PUBLIC;

///
/// @brief
///     ����FT3����ֵ���ĵ�Ӧ�÷������ݵ�Ԫ��ASDU���г�����ֵ��Ϣ��Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ
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
	// 	size_t smpRateOff;                     ///< �����Ƶ����ÿ�ܲ�ʱ��������Ĳ������ݼ���Ŀ��ƫ�Ƶ�ַ
	// 	size_t confRevOff;                     ///< ���ð汾�ŵ�ƫ�Ƶ�ַ
}GUOWANG_FT3_ASDU_PUBLIC_OFFSET;

#endif
