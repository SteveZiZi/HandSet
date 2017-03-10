#ifndef SAV92_ASDU_SEG_H_

#define SAV92_ASDU_SEG_H_

#include <vector>
#include "../include/sav92_types.h"

///
/// @brief
///     符合9-2采样值报文中的ASDU的可选项的配置信息，表示ASDU是否包含该项
///
typedef struct 
{
	bool bDataSetOpt;                      ///< 数据集名
	bool bRefrTmOpt;                       ///< 刷新时间
	bool bSmpRateOpt;                      ///< 采样率
}SAV92_ASDU_OPTS;

class MsgDataElem;

class Sav92AsduSeg
{
public:
	Sav92AsduSeg();
	~Sav92AsduSeg();

	size_t Decode(const size_t offset, const uint8_t *asduDataBuf, const size_t bufSize);
	size_t Code(uint8_t *asduDataArr, const size_t arrSize);

	size_t GetASDULen();
	bool   GetASDUPublicInfo(const uint8_t *msgDataBuf, SAV92_ASDU_PUBLIC &asduPublic);
	bool   GetSmpCntInASDU(const uint8_t *msgDataBuf, uint16_t &smpCnt);
	bool   GetSmpSynchInASDU(const uint8_t *msgDataBuf, bool &smpSynch);
	size_t GetASDUSmpsNum();
	size_t GetASDUSmps(const uint8_t *msgDataBuf, uint32_t *smpValArr, uint32_t *smpQltArr, const size_t arrSize);
	size_t GetSeqOfDatLen();
	void   GetASDUOpts(SAV92_ASDU_OPTS &asduOpts);

	size_t GetASDUOff();
	void   GetASDUPublicInfoOff(SAV92_ASDU_PUBLIC_OFFSET &asduPublicOff);
	size_t GetASDUSmpsOff(size_t &seqOfDataOff, size_t *smpValOffArr, size_t *smpQltOffArr, const size_t arrSize);

	bool   SetASDU(const SAV92_ASDU_PUBLIC &asduPublic, const uint32_t *smpValBuf, const uint32_t *smpQltBuf,
		           const size_t bufSize);
	void   SetASDUOpts(SAV92_ASDU_OPTS &asduOpts);

protected:
	void   Init();

private:
	std::vector<MsgDataElem *> *m_AsduData;
	SAV92_ASDU_OPTS m_AsduOpts;
	size_t m_AsduLen;
	size_t m_AsduOff;
};

#endif
