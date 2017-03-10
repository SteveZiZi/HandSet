#ifndef IEEE1588_HEADER_SEG_H_

#define IEEE1588_HEADER_SEG_H_

#include <vector>
#include "../include/ieee1588_types.h"

class MsgDataElem;

class Ieee1588HeaderSeg
{
public:
	Ieee1588HeaderSeg();
	~Ieee1588HeaderSeg();

	size_t Decode(const size_t offset, const uint8_t *headerBuf, const size_t bufSize);
	size_t Code(uint8_t *bodyAndSuffixBuf, size_t bufSize, uint8_t *msgArr, const size_t arrSize);

	IEEE1588_MESSAGE_TYPE GetIeee1588MsgType(const uint8_t *msgDataBuf);
	void GetIeee1588Header(const uint8_t *msgDataBuf, IEEE1588_HEADER &ieee1588Header);

	void GetIeee1588HeaderOff(IEEE1588_HEADER_OFFSET &ieee1588HeaderOff);

	void SetIeee1588Header(const IEEE1588_HEADER &ieee1588Header);

private:
	std::vector<MsgDataElem *> m_vIeee1588Header;
};

#endif
