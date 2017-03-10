/// @file
///     sav92_types.h
///
/// @brief
///     ����IEC61850-9-2LE����֡����������ʹ�õ���������
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
#ifndef SAV92_TYPES_H_

#define SAV92_TYPES_H_

#include <string>
#include "src\include\stdint.h"

///
/// @brief
///     9-2����֡��Visible String�������ݵ����Size������'\0'��
///
#define SAV92_VISIBLE_STR_MAX_SIZE 35+1

///
/// @brief
///     ����9-2��׼��SavPDU��Ӧ�÷������ݵ�Ԫ��ASDU���г�����ֵ��Ϣ��Ĺ�����Ϣ
///
typedef struct
{
	char         svIDStr[SAV92_VISIBLE_STR_MAX_SIZE];///< ��ʾ����ֵ���ƿ��ʶ
	char         datSetStr[SAV92_VISIBLE_STR_MAX_SIZE]; ///< ��ʾ�������ݼ������������ݼ���������ѡ��
	uint16_t     smpCnt;                   ///< ��ʾ����������,����������Ŀ
	uint32_t     confRev;                  ///< ��ʾ���ð汾��
	struct
	{
		uint32_t ts_s;                     ///< ʱ�������������
		uint32_t ts_ns;                    ///< ʱ��������������֣���С������
		uint8_t  ts_q;                     ///< ʱ�����Ʒ��
	}refrTm;                               ///< ������ˢ��ʱ�䣨UTCʱ�䣩����ѡ������й�ռ8 Byte
	bool         smpSynch;                 ///< ��ʾͬ����ʶλ�����ڷ�ӳ�ϲ���Ԫ��ͬ��״̬
	uint16_t     smpRate;                  ///< ������,��ѡ��
}SAV92_ASDU_PUBLIC;

///
/// @brief
///     ����9-2��׼��SavPDU��Ӧ�÷������ݵ�Ԫ��ASDU���г�����ֵ��Ϣ��Ĺ�����Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
typedef struct
{
	size_t svIDOff;                        ///< ����ֵ���ƿ��ʶ��ƫ�Ƶ�ַ
	size_t datSetOff;                      ///< �������ݼ������������ݼ�������ƫ�Ƶ�ַ
	size_t smpCntOff;                      ///< ������������ƫ�Ƶ�ַ
	size_t confRevOff;                     ///< ���ð汾�ŵ�ƫ�Ƶ�ַ
	size_t refrTmOff;                      ///< ������ˢ��ʱ���ƫ�Ƶ�ַ
	size_t smpSynchOff;                    ///< ͬ����ʶλ��ƫ�Ƶ�ַ
	size_t smpRateOff;                     ///< �����ʵ�ƫ�Ƶ�ַ
}SAV92_ASDU_PUBLIC_OFFSET;

///
/// @brief
///     ����9-2����ֵ�����еĿ�ѡ��������Ϣ����ʾ�������Ƿ��������
///
typedef struct 
{
	bool bSecurityOpt;                     ///< ��ȫ��Ϣ
	bool bDataSetOpt;                      ///< ���ݼ���
	bool bRefrTmOpt;                       ///< ˢ��ʱ��
	bool bSmpRateOpt;                      ///< ������
}SAV92_SAVPDU_OPTS;

#endif
