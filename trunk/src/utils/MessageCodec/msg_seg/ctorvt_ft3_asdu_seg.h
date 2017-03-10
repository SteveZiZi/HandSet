/// @file
///     ctorvt_ft3_asdu_seg.h
///
/// @brief
///     ����/��ѹ������FT3��ASDU�εı����ӿڼ��ӿڶ���ʵ���Ĵ������ͷŽӿ�
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
#ifndef CTORVT_FT3_ASDU_SEG_H_

#define CTORVT_FT3_ASDU_SEG_H_

#include "../include/ctorvt_ft3_types.h"

class MsgDataElem;

class CTorVTFT3AsduSeg
{
public:
	virtual ~CTorVTFT3AsduSeg(){};

	virtual size_t Decode(const size_t offset, const uint16_t *asduDataBuf, const size_t bufSize) = 0;
	virtual size_t Code(uint16_t *asduDataArr, const size_t arrSize) = 0;

	virtual bool   GetASDUPublicInfo(const uint16_t *msgDataBuf, CTORVT_FT3_ASDU_PUBLIC &asduPublic) = 0;
	virtual size_t GetASDUSmpsNum() = 0;
	virtual size_t GetASDUSmps(const uint16_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize) = 0;
	virtual size_t GetASDULen() = 0;

	virtual size_t GetASDUOff() = 0;
	virtual void   GetASDUPublicInfoOff(CTORVT_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff) = 0;
	virtual size_t GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize) = 0;

	virtual void   SetASDU(const CTORVT_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t &bufSize) = 0;
};

///
/// @brief
///     ��������/��ѹ������FT3��ASDU�εı����ӿڶ����ʵ��
///
/// @param[in]
///     eCTorVTFt3Type - ����/��ѹ������FT3����ֵ���ĵ�����
///
/// @return
///     ָ��CTorVTFT3AsduSegʵ����ָ��
///
CTorVTFT3AsduSeg *CreateCTorVTFT3AsduSegImpl(CTORVT_FT3_TYPE eCTorVTFt3Type);

///
/// @brief
///     �ͷŵ���/��ѹ������FT3��ASDU�εı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��CTorVTFT3AsduSegʵ����ָ��
///
/// @return
///     
///
void ReleaseCTorVTFT3AsduSegImpl(CTorVTFT3AsduSeg *impl);

#endif
