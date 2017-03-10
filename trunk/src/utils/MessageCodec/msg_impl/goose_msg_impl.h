#ifndef GOOSE_MESSAGE_IMPL_H_

#define GOOSE_MESSAGE_IMPL_H_

#include "../include/goose_msg.h" 

class EthernetIISeg;
class Ieee8021QSeg;
class Iec88023PduPublicSeg;
class GoosePduSeg;

class GooseMsgImpl:public GooseMsg
{
public:
	GooseMsgImpl();
	virtual ~GooseMsgImpl();

	virtual size_t Decode(const uint8_t *msgData, const size_t datLen);
	virtual size_t Code(uint8_t *msgData, const size_t arrSize);

	virtual void   GetEthernetHeader(ETHER_II_HEADER &ethHdr);
	virtual void   Get8021QVlanInfo(IEEE802_1Q_VLAN_INFO &info);
	virtual void   GetIEC8802Info(IEC8802_3_PDU_PUBLIC &info);
	virtual size_t GetGoosePduLen();	
	virtual size_t GetGooseDataLen();
	virtual bool   GetPduPublicInfo(GOOSE_PDU_PUBLIC &pduPublic);
	virtual bool   GetPduPublicInfo(GOOSE_PDU_PUBLIC_EX &pduPublic);
	virtual size_t GetChannelNum();
	virtual GOOSE_DATA *GetAChannel(size_t idxSeq);
	virtual bool   GetSecurityOpt();
	
	virtual void   GetEthernetHeaderOff(ETHER_II_HEADER_OFFSET &ethHdrOff);
	virtual void   Get8021QVlanInfoOff(IEEE802_1Q_VLAN_INFO_OFFSET &infoOff);
	virtual void   GetIEC8802InfoOff(IEC8802_3_PDU_PUBLIC_OFFSET &infoOff);
	virtual size_t GetGoosePduOff();
	virtual size_t GetGooseDataOff();
	virtual void   GetPduPublicInfoOff(GOOSE_PDU_PUBLIC_OFFSET &pduPblcOff);

	virtual void   SetMacAddr(const uint8_t *desMac, const uint8_t *srcMac, size_t macLen, bool isHasVlan);
	virtual void   Set8021QVlanInfo(uint8_t userPriority, uint16_t vlanID, bool isHasVlan);
	virtual void   SetAppID(const uint16_t appID);
	virtual void   SetPduPublicInfo(GOOSE_PDU_PUBLIC &pduPblc);
	virtual bool   AddAChannel(const GOOSE_DATA *channel);
	virtual void   SetSecurityOpt(bool opt);
	virtual void   ClearGoosePDU();

//	virtual void   ChannelDataToStr(const GOOSE_CHANNEL_DATA &datVal, char *outStr, const size_t strSize);
	bool				  m_bHasVlanFlag;
private:
	EthernetIISeg        *m_EtherSeg;
	Ieee8021QSeg         *m_Ieee8021QSeg;
	Iec88023PduPublicSeg *m_Iec8802Seg;
	GoosePduSeg          *m_GoosePduSeg;
	uint8_t              *m_MsgDataBuf;
	size_t                m_szMsgDataBufSize;
	bool                  m_bHaveDecode;
	
};

#endif
