#ifndef SAV92_MESSAGE_IMPL_H_

#define SAV92_MESSAGE_IMPL_H_

#include "../include/sav92_msg.h" 

class EthernetIISeg;
class Ieee8021QSeg;
class Iec88023PduPublicSeg;
class Sav92PduSeg;

///
/// @brief
///     IEC61850-9-2采样值报文的编解码接口对象的实现
///
class Sav92MsgImpl:public Sav92Msg
{
public:
	Sav92MsgImpl();
	virtual ~Sav92MsgImpl();

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
	virtual size_t GetASDULen(const size_t idxSeq);
	virtual size_t GetSeqOfDatLen(const size_t idxSeq);
	virtual bool   GetASDUPublicInfo(const size_t idxSeq, SAV92_ASDU_PUBLIC &asduPublic);
	virtual bool   GetSmpCntInASDU(const size_t idxSeq, uint16_t &smpCnt);
	virtual bool   GetSmpSynchInASDU(const size_t idxSeq, bool &smpSynch);
	virtual size_t GetASDUSmpsNum(const size_t idxSeq);
	virtual size_t GetASDUSmps(const size_t idxSeq, uint32_t *smpValArr, uint32_t *smpQltArr, const size_t arrSize);	
	virtual void   GetSavPduOpts(SAV92_SAVPDU_OPTS &pduOpts);

	///
	/// @brief
	///     取报文字段地址信息函数
	///
	virtual void   GetEthernetHeaderOff(ETHER_II_HEADER_OFFSET &ethHdrOff);
	virtual void   Get8021QVlanInfoOff(IEEE802_1Q_VLAN_INFO_OFFSET &infoOff);
	virtual void   GetIEC8802InfoOff(IEC8802_3_PDU_PUBLIC_OFFSET &infoOff);
	virtual size_t GetSavPduOff();
	virtual size_t GetNoASDUOff();
	virtual size_t GetASDUOff(const size_t idxSeq);
	virtual bool   GetASDUPublicInfoOff(const size_t idxSeq, SAV92_ASDU_PUBLIC_OFFSET &asduPublicOff);
	virtual size_t GetASDUSmpsOff(const size_t idxSeq,size_t &seqOfDataOff,size_t *smpValOffArr,
	                              size_t *smpQltOffArr,const size_t arrSize);

	///
	/// @brief
	///     报文信息设置函数
	///
	virtual void SetMacAddr(const uint8_t *desMac, const uint8_t *srcMac, size_t macLen, bool isHasVlan);
	virtual void Set8021QVlanInfo(uint8_t userPriority, uint16_t vlanID, bool isHasVlan);
	virtual void SetAppID(const uint16_t appID);
	virtual void ClearSavPDU();
	virtual bool AddAnASDU(const SAV92_ASDU_PUBLIC &asduPublic,const uint32_t *smpValBuf,
	                       const uint32_t *smpQltBuf,const size_t bufSize);
	virtual void SetSavPduOpts(SAV92_SAVPDU_OPTS &pduOpts);

	bool				m_bHasVlanFlag;
private:
	EthernetIISeg        *m_EtherSeg;
	Ieee8021QSeg         *m_Ieee8021QSeg;
	Iec88023PduPublicSeg *m_Iec8802Seg;
	Sav92PduSeg          *m_Sav92PduSeg;
	uint8_t              *m_MsgDataBuf;
	size_t                m_MsgDataBufSize;
	bool                  m_bHaveDecode;
};

#endif
