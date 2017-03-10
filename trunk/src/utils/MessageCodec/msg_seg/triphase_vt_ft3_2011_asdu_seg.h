#ifndef TRIPHASE_VT_FT3_2011_ASDU_SEG_H_

#define TRIPHASE_VT_FT3_2011_ASDU_SEG_H_

#include <vector>
#include "ctorvt_ft3_asdu_seg.h"

///
/// @brief
///     三相电压互感器FT3采样值报文（2011）的ASDU的数据通道数
///
#define TRIPHASE_VT_FT3_2011_ASDU_CHANNEL_NUM  6

class TriphaseVTFT3_2011AsduSeg:public CTorVTFT3AsduSeg
{
public:
	TriphaseVTFT3_2011AsduSeg();
	virtual ~TriphaseVTFT3_2011AsduSeg();

	virtual size_t Decode(const size_t offset, const uint16_t *asduDataBuf, const size_t bufSize);
	virtual size_t Code(uint16_t *asduDataArr, const size_t arrSize);

	virtual bool   GetASDUPublicInfo(const uint16_t *msgDataBuf, CTORVT_FT3_ASDU_PUBLIC &asduPublic);
	virtual size_t GetASDUSmpsNum();
	virtual size_t GetASDUSmps(const uint16_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize);
	virtual size_t GetASDULen();

	virtual size_t GetASDUOff();
	virtual void   GetASDUPublicInfoOff(CTORVT_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff);
	virtual size_t GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize);

	virtual void   SetASDU(const CTORVT_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t &bufSize);

private:
	std::vector<MsgDataElem *> *m_AsduData;
	size_t m_AsduLen;
	size_t m_AsduOff;
};

#endif
