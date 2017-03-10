#ifndef SAV91_ASDU_SUBSEG_H_

#define SAV91_ASDU_SUBSEG_H_

#include <vector>
#include "../include/sav91_types.h"

///
/// @brief
///     Sav91采样值报文的ASDU的默认长度
///
#define SAV91_ASDU_DEFAULT_SIZE 46

///
/// @brief
///     Sav91采样值报文的ASDU的数据通道数
///
#define SAV91_ASDU_CHANNEL_NUM  12

class MsgDataElem;

class Sav91AsduSeg
{
public:
	Sav91AsduSeg();
	~Sav91AsduSeg();

	size_t Decode(const size_t offset, const uint8_t *asduDataBuf, const size_t bufSize);
	size_t Code(uint8_t *asduDataArr, const size_t arrSize);

	bool   GetASDUPublicInfo(const uint8_t *msgDataBuf, SAV91_ASDU_PUBLIC &asduPublic);
	bool   GetSmpCntInASDU(const uint8_t *msgDataBuf, uint16_t &smpCnt);
	bool   GetStatusWordsInASDU(const uint8_t *msgDataBuf, uint16_t &statusWord1, uint16_t &statusWord2);
	size_t GetASDUSmps(const uint8_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize);

	void GetASDUPublicInfoOff(SAV91_ASDU_PUBLIC_OFFSET &asduPublicOff);
	int  GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize);

	bool SetASDU(const SAV91_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t &bufSize);

private:
	std::vector<MsgDataElem *> *m_AsduData;
};

#endif
