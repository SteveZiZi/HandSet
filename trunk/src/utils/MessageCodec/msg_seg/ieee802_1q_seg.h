#ifndef IEEE802_1Q_SEG_H_

#define IEEE802_1Q_SEG_H_

#include "../include/etypes.h"

class MsgDataElem;

class Ieee8021QSeg
{
public:
	Ieee8021QSeg();
	~Ieee8021QSeg();

	size_t Decode(const size_t offset, const uint8_t *msgData, const size_t datLen);
	size_t Code(uint8_t *pduData, size_t datLen, uint8_t *msgData, const size_t arrSize);

	void Get8021QVlanInfo(const uint8_t *msgDatabuf, IEEE802_1Q_VLAN_INFO &info);

	void Get8021QVlanInfoOff(IEEE802_1Q_VLAN_INFO_OFFSET &infoOff);

	void Set8021QVlanInfo(const IEEE802_1Q_VLAN_INFO &info, bool isHasVlan);

private:
	MsgDataElem *m_TCI;
	MsgDataElem *m_EtherType;
};

#endif
