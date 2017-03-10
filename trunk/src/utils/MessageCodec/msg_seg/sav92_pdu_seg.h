#ifndef SAV92_PDU_SEG_H_

#define SAV92_PDU_SEG_H_

#include <vector>
#include "../include/sav92_types.h"

class MsgDataElem;
class Sav92AsduSeg;

class Sav92PduSeg
{
public:
	Sav92PduSeg();
	~Sav92PduSeg();

	typedef enum {SAV92_IEEE802_1Q = 0}SAV92_ULP_TYPE;

	size_t Decode(const size_t offset, const uint8_t *msgData, const size_t datLen);
	size_t Code(uint8_t *msgData, const size_t arrSize);

	size_t GetPduLen();
	uint16_t GetNoASDU();
	size_t GetASDULen(const size_t idxSeq);
	size_t GetSeqOfDatLen(const size_t idxSeq);
	bool   GetASDUPublicInfo(const size_t idxSeq, const uint8_t *msgDataBuf, SAV92_ASDU_PUBLIC &asduPublic);
	bool   GetSmpCntInASDU(const size_t idxSeq, const uint8_t *msgDataBuf, uint16_t &smpCnt);
	bool   GetSmpSynchInASDU(const size_t idxSeq, const uint8_t *msgDataBuf, bool &smpSynch);
	size_t GetASDUSmpsNum(const size_t idxSeq);
	size_t GetASDUSmps(const size_t idxSeq, const uint8_t *msgDataBuf, uint32_t *smpValArr, uint32_t *smpQltArr, const size_t arrSize);
	void   GetSavPduOpts(SAV92_SAVPDU_OPTS &pduOpts);
	
	size_t GetPduOff();
	size_t GetNoASDUOff();
	size_t GetASDUOff(const size_t idxSeq);
	bool   GetASDUPublicInfoOff(const size_t idxSeq, SAV92_ASDU_PUBLIC_OFFSET &asduPublicOff);
	size_t GetASDUSmpsOff(const size_t idxSeq, size_t &seqOfDataOff, size_t *smpValOff, size_t *smpQltOff, const size_t arrSize);
	
	void   Clear();
	void   SetULPType(const SAV92_ULP_TYPE type);
	bool   AddAnASDU(const SAV92_ASDU_PUBLIC &asduPublic, const uint32_t *smpValBuf, const uint32_t *smpQltBuf, const size_t &bufSize);
	void   SetSavPduOpts(const SAV92_SAVPDU_OPTS &pduOpts);

private:
	MsgDataElem *m_pNoASDU;
	MsgDataElem *m_pSecurity;
	size_t       m_PduLen;
	size_t       m_PduOff;
	size_t       m_AsduSize;
	size_t       m_MaxPduSize;
	uint16_t     m_nNoASDU;
	SAV92_SAVPDU_OPTS m_PduOpts;
	std::vector<Sav92AsduSeg *> m_AsduSubSeg;
};

#endif
