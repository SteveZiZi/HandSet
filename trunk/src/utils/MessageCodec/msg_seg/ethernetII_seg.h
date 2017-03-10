#ifndef ETHERNET_II_SEG_H_

#define ETHERNET_II_SEG_H_

#include "../include/etypes.h"

class MsgDataElem;

class EthernetIISeg
{
public:
	EthernetIISeg();
	~EthernetIISeg();
	
	size_t Decode(const size_t offset, const uint8_t *msgBuf, const size_t bufSize);
	size_t Code(uint8_t *userDataBuf, size_t bufSize, uint8_t *msgArr, const size_t arrSize);

	void GetEtherHdr(const uint8_t *msgDataBuf, ETHER_II_HEADER &ethHdr);

	void GetEtherHdrOff(ETHER_II_HEADER_OFFSET &ethHdrOff);

	void SetEthernetHeader(const ETHER_II_HEADER &ethHdr, bool isHasVlan);

private:
	MsgDataElem *m_DesMac;
	MsgDataElem *m_SrcMac;
	MsgDataElem *m_EtherType;
};

#endif
