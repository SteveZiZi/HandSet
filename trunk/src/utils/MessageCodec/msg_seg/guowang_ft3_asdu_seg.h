#ifndef GUOWANG_FT3_ASDU_SEG_H_

#define GUOWANG_FT3_ASDU_SEG_H_

#include <vector>
#include "../include/guowang_ft3_types.h"

///
/// @brief
///     国网FT3采样值报文的ASDU的数据通道数
///
#define GUOWANG_FT3_ASDU_CHANNEL_NUM  22

class MsgDataElem;

class GuoWangFT3AsduSeg
{
public:
	GuoWangFT3AsduSeg();
	~GuoWangFT3AsduSeg();

	size_t Decode(const size_t offset, const uint16_t *asduDataBuf, const size_t bufSize);
	size_t Code(uint16_t *asduDataArr, const size_t arrSize);

	bool   GetASDUPublicInfo(const uint16_t *msgDataBuf, GUOWANG_FT3_ASDU_PUBLIC &asduPublic);
	size_t GetASDUSmpsNum();
	size_t GetASDUSmps(const uint16_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize);
	size_t GetASDULen();
	
	size_t GetASDUOff();
	void   GetASDUPublicInfoOff(GUOWANG_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff);
	size_t GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize);

	void   SetASDU(const GUOWANG_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpVal, const size_t &smpNum);

private:
	std::vector<MsgDataElem *> *m_AsduData;
	size_t m_AsduLen;
	size_t m_AsduOff;
};

#endif
