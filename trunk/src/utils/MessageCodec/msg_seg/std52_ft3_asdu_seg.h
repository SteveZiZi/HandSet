#ifndef STD52_FT3_ASDU_SEG_H_

#define STD52_FT3_ASDU_SEG_H_

#include <vector>
#include "../include/std52_ft3_types.h"

///
/// @brief
///     标准FT3采样值报文（52字节版本,含CRC）的ASDU的数据通道数
///
#define STD52_FT3_ASDU_CHANNEL_NUM  12

class MsgDataElem;

class Std52FT3AsduSeg
{
public:
	Std52FT3AsduSeg();
	~Std52FT3AsduSeg();

	size_t Decode(const size_t offset, const uint16_t *asduDataBuf, const size_t bufSize);
	size_t Code(uint16_t *asduDataArr, const size_t arrSize);

	bool   GetASDUPublicInfo(const uint16_t *msgDataBuf, STD52_FT3_ASDU_PUBLIC &asduPublic);
	size_t GetASDUSmpsNum();
	size_t GetASDUSmps(const uint16_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize);
	size_t GetASDULen();

	size_t GetASDUOff();
	void   GetASDUPublicInfoOff(STD52_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff);
	size_t GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize);

	void   SetASDU(const STD52_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t &bufSize);

private:
	std::vector<MsgDataElem *> *m_AsduData;
	size_t m_AsduLen;
	size_t m_AsduOff;
};

#endif
