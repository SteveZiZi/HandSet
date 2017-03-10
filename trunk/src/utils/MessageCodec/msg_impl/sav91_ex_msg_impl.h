#ifndef SAV91_EX_MESSAGE_IMPL_H_

#define SAV91_EX_MESSAGE_IMPL_H_

#include "../include/sav91_ex_msg.h"

class EthernetIISeg;
class Ieee8021QSeg;
class Iec88023PduPublicSeg;
class Sav91ExPduSeg;

///
/// @brief
///     IEC61850-9-1扩展报文帧的编解码接口的实现
///
class Sav91ExMsgImpl:public Sav91ExMsg
{
public:
	Sav91ExMsgImpl();
	virtual ~Sav91ExMsgImpl();

	///
	/// @brief
	///     编解码函数
	///
	virtual size_t Decode(const uint8_t *msgDataBuf, const size_t bufSize);
	virtual size_t Code(uint8_t *msgDataArr, const size_t arrSize);

	///
	/// @brief
	///     取报文字段数据函数
	///
	virtual void   GetEthernetHeader(ETHER_II_HEADER &ethHdr);
	virtual void   Get8021QVlanInfo(IEEE802_1Q_VLAN_INFO &info);
	virtual void   GetIEC8802Info(IEC8802_3_PDU_PUBLIC &info);
	virtual size_t GetSavPduLen();
	virtual uint16_t GetNoASDU();
	virtual bool   GetASDUPublicInfo(const size_t idxSeq, SAV91_EX_ASDU_PUBLIC &asduPublic);
	virtual size_t GetASDUSmpsNum();
	virtual size_t GetASDUSmps(const size_t idxSeq, uint16_t *smpValArr, const size_t arrSize);

	///
	/// @brief
	///     取报文字段地址信息函数
	///
	virtual void   GetEthernetHeaderOff(ETHER_II_HEADER_OFFSET &ethHdrOff);
	virtual void   Get8021QVlanInfoOff(IEEE802_1Q_VLAN_INFO_OFFSET &infoOff);
	virtual void   GetIEC8802InfoOff(IEC8802_3_PDU_PUBLIC_OFFSET &infoOff);
	virtual size_t GetSavPduOff();
	virtual size_t GetNoASDUOff();
	virtual bool   GetASDUPublicInfoOff(const size_t idxSeq, SAV91_EX_ASDU_PUBLIC_OFFSET &asduPublicOff);
	virtual size_t GetASDUSmpsOff(const size_t idxSeq, uint32_t *smpOffArr, const size_t arrSize);

	///
	/// @brief
	///     报文信息设置函数
	///
	virtual void SetMacAddr(const uint8_t *desMac, const uint8_t *srcMac, size_t macLen);
	virtual void Set8021QVlanInfo(uint8_t userPriority, uint16_t vlanID);
	virtual void SetAppID(const uint16_t appID);
	virtual void ClearSavPDU();
	virtual bool AddAnASDU(const SAV91_EX_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t bufSize);

private:
	EthernetIISeg        *m_EtherSeg;
	Ieee8021QSeg         *m_Ieee8021QSeg;
	Iec88023PduPublicSeg *m_Iec8802Seg;
	Sav91ExPduSeg        *m_Sav91ExPduSeg;
	uint8_t              *m_MsgDataBuf;
	bool                  m_bHaveDecode;
};

#endif
