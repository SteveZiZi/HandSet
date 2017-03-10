#ifndef IEEE1588_MESSAGE_IMPL_H_

#define IEEE1588_MESSAGE_IMPL_H_

#include "../include/ieee1588_msg.h"

class EthernetIISeg;
class Ieee1588HeaderSeg;

class Ieee1588MsgImpl:public Ieee1588Msg
{
public:
	Ieee1588MsgImpl();
	virtual ~Ieee1588MsgImpl();

	virtual size_t Decode(const uint8_t *msgDataBuf, const size_t bufSize);

	virtual void GetEthernetHeader(ETHER_II_HEADER &ethHdr);
	virtual void GetIeee1588Header(IEEE1588_HEADER &ieee1588Hdr);
	virtual bool GetOriginTimestamp(IEEE1588_TIMESTAMP &originTimestamp);

	virtual void GetEthernetHeaderOff(ETHER_II_HEADER_OFFSET &ethHdrOff);
	virtual void GetIeee1588HeaderOff(IEEE1588_HEADER_OFFSET &ieee1588HdrOff);
	virtual bool GetOriginTimestampOff(IEEE1588_TIMESTAMP_OFFSET &originTimestampOff);

private:
	EthernetIISeg        *m_EtherSeg;
	Ieee1588HeaderSeg    *m_Ieee1588HeaderSeg;
	uint8_t               m_MsgDataBuf[ETHER_FRAME_MAX_LEN];
	size_t                m_szMsgDataBufSize;
	IEEE1588_MESSAGE_TYPE m_eMsgType;
	size_t                m_szIeee1588BodyOff;             ///< IEEE1588报文本体（紧跟报文头之后）的偏移地址
	bool                  m_bHaveDecode;
};

#endif
