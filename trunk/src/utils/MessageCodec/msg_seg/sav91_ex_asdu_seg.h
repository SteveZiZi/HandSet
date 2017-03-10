#ifndef SAV91_EX_EX_ASDU_SUBSEG_H_

#define SAV91_EX_EX_ASDU_SUBSEG_H_

#include <vector>
#include "../include/etypes.h"
#include "../include/sav91_ex_types.h"

class MsgDataElem;

class Sav91ExAsduSeg
{
public:
	Sav91ExAsduSeg();
	~Sav91ExAsduSeg();

	size_t Decode(const size_t offset, const uint8_t *asduDataBuf, const size_t bufSize);
	size_t Code(uint8_t *asduDataArr, const size_t arrSize);

	bool   GetASDUPublicInfo(const uint8_t *msgDataBuf, SAV91_EX_ASDU_PUBLIC &asduPublic);
	size_t GetASDUSmpsNum();
	size_t GetASDUSmps(const uint8_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize);

	void GetASDUPublicInfoOff(SAV91_EX_ASDU_PUBLIC_OFFSET &asduPublicOff);
	int  GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize);

	bool SetASDU(const SAV91_EX_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t &bufSize);

private:
	std::vector<MsgDataElem *> *m_AsduData;
	size_t m_SmpsNum;
};

#endif
