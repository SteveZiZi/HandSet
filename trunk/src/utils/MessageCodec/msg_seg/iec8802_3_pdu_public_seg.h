#ifndef IEC8802_3_PDU_PUBLIC_SEG_H_

#define IEC8802_3_PDU_PUBLIC_SEG_H_

#include "../include/etypes.h"

class MsgDataElem;

class Iec88023PduPublicSeg
{
public:
	Iec88023PduPublicSeg();
	~Iec88023PduPublicSeg();

	size_t Decode(const size_t offset, const uint8_t *pduDataBuf, const size_t bufSize);
	size_t Code(uint8_t *apduDataBuf, size_t bufSize, uint8_t *pduDataArr, const size_t arrSize);

	void GetIEC8802Info(const uint8_t *msgDataBuf, IEC8802_3_PDU_PUBLIC &info);

	void GetIEC8802InfoOff(IEC8802_3_PDU_PUBLIC_OFFSET &infoOff);

	void SetIEC8802Info(const IEC8802_3_PDU_PUBLIC &info);

private:
	MsgDataElem *m_AppID;
	MsgDataElem *m_Len;
	MsgDataElem *m_Reserved1;
	MsgDataElem *m_Reserved2;
};

#endif
