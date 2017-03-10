/// @file
///     sav91_ex_types.h
///
/// @brief
///     ����IEC61850-9-1��չ����֡�����Ͼ���������ƶ���������������������ʹ�õ���������
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.09.06
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#ifndef SAV91_EX_TYPES_H_

#define SAV91_EX_TYPES_H_

#include "src\include\stdint.h"

///
/// @brief
///     IEC61850-9-1��չ����֡��SavPDU��Ӧ�÷������ݵ�Ԫ��ASDU���г�����ֵ��Ϣ��Ĺ�����Ϣ
///     ע�⣬IEC61850-9-1��չ����֡�е�״̬�ֺ�����FT3�ͱ�׼9-1����ͬ
///
typedef struct
{
	uint16_t asduLen;                      ///< ASDU�ĳ���
	uint8_t  lnName;                       ///< �߼��ڵ����ƣ�һ��Ϊ0x02
	uint8_t  dataSetName;                  ///< ���ݼ����ƣ�һ��Ϊ0xFE
	uint16_t ldName;                       ///< �߼��豸���ƣ�һ��Ϊ0
	uint16_t statusWord1;                  ///< ״̬��1�����׼9-1һ��
	uint16_t statusWord2;                  ///< ״̬��2�����׼9-1һ��
	uint16_t smpCnt;                       ///< ��������������ʾ��ǰ���͵Ĳ�������Ŀ
	uint8_t  smpRate;                      ///< �����Ƶ����ÿ�ܲ�ʱ��������Ĳ������ݼ���Ŀ
	uint8_t  confRev;                      ///< ���ð汾�ţ�һ��Ϊ0x57
}SAV91_EX_ASDU_PUBLIC;

///
/// @brief
///     IEC61850-9-1��չ����֡��SavPDU��Ӧ�÷������ݵ�Ԫ��ASDU���г�����ֵ��Ϣ��Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ
///
typedef struct
{
	size_t   asduLenOff;                   ///< ASDU�ĳ��ȵ�ƫ�Ƶ�ַ
	size_t   lnNameOff;                    ///< �߼��ڵ����Ƶ�ƫ�Ƶ�ַ
	size_t   dataSetNameOff;               ///< ���ݼ����Ƶ�ƫ�Ƶ�ַ
	size_t   ldNameOff;                    ///< �߼��豸���Ƶ�ƫ�Ƶ�ַ
	size_t   statusWord1Off;               ///< ״̬��1��ƫ�Ƶ�ַ
	size_t   statusWord2Off;               ///< ״̬��2��ƫ�Ƶ�ַ
	size_t   smpCntOff;                    ///< ��������������ʾ��ǰ���͵Ĳ�������Ŀ��ƫ�Ƶ�ַ
	size_t   smpRateOff;                   ///< �����Ƶ����ÿ�ܲ�ʱ��������Ĳ������ݼ���Ŀ��ƫ�Ƶ�ַ
	size_t   confRevOff;                   ///< ���ð汾�ŵ�ƫ�Ƶ�ַ
}SAV91_EX_ASDU_PUBLIC_OFFSET;

#endif
