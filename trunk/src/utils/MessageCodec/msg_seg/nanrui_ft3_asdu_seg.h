#ifndef NANRUI_FT3_ASDU_SEG_H_

#define NANRUI_FT3_ASDU_SEG_H_

#include <vector>
#include "../include/nanrui_ft3_types.h"

///
/// @brief
///     南瑞FT3采样值报文的ASDU的数据通道数
///
#define NANRUI_FT3_ASDU_CHANNEL_NUM  12

class MsgDataElem;

class NanRuiFT3AsduSeg
{
public:
	NanRuiFT3AsduSeg();
	~NanRuiFT3AsduSeg();

	size_t Decode(const size_t offset, const uint16_t *asduDataBuf, const size_t bufSize);
	size_t Code(uint16_t *asduDataArr, const size_t arrSize);

	bool   GetASDUPublicInfo(const uint16_t *msgDataBuf, NANRUI_FT3_ASDU_PUBLIC &asduPublic);
	size_t GetASDUSmpsNum();
	size_t GetASDUSmps(const uint16_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize);
	size_t GetASDULen();

	size_t GetASDUOff();
	void   GetASDUPublicInfoOff(NANRUI_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff);
	size_t GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize);

	void   SetASDU(const NANRUI_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t &bufSize);

private:
	std::vector<MsgDataElem *> *m_AsduData;
	size_t m_AsduLen;
	size_t m_AsduOff;
};

#endif
