#ifndef GOOSE_PDU_SEG_H_

#define GOOSE_PDU_SEG_H_

#include <vector>
#include "../include/goose_types.h"

class MsgDataElem;
class GooseDataSeg;

class GoosePduSeg
{
public:
	GoosePduSeg();
	~GoosePduSeg();

	typedef enum {GOOSE_IEEE802_1Q = 0}GOOSE_ULP_TYPE;

	size_t Decode(const size_t offset, const uint8_t *pduDataBuf, const size_t bufSize);
	size_t Code(uint8_t *pduDataArr, const size_t arrSize);

	size_t GetPduLen();	
	size_t GetGooseDataLen();
	bool   GetPduPublicInfo(const uint8_t *msgDataBuf, GOOSE_PDU_PUBLIC &pduPublic);
	bool   GetPduPublicInfo(const uint8_t *msgDataBuf, GOOSE_PDU_PUBLIC_EX &pduPublic);
	void   ResetChannelDecodedFlag();
	size_t GetChannelNum();
	GOOSE_DATA * GetAChannel(size_t idxSeq, const uint8_t *msgDataBuf, size_t bufSize);
	bool   GetSecurityOpt();
	
	size_t GetPduOff();
	size_t GetGooseDataOff();
	void   GetPduPublicInfoOff(GOOSE_PDU_PUBLIC_OFFSET &pduPblcOff);

	void   SetPduPublicInfo(GOOSE_PDU_PUBLIC &pduPblc);
	bool   AddAChannel(const GOOSE_DATA *channel);
	void   SetSecurityOpt(bool opt);
	void   Clear();
	void   SetULPType(const GOOSE_ULP_TYPE type);

protected:
	size_t GetUint32MinLen(uint32_t &inData);

private:
	std::vector<MsgDataElem *> *m_PduPblc;
	GooseDataSeg *m_GooseData;
	bool   m_bSecurityOpt;
	size_t m_PduLen;
	size_t m_PduOff;
	size_t m_GooseDataSize;
	size_t m_MaxPduSize;
};

#endif
