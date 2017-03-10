#ifndef CTORVT_FT3_MSG_IMPL_H_

#define CTORVT_FT3_MSG_IMPL_H_

#include "../include/ctorvt_ft3_msg.h"

class CTorVTFT3AsduSeg;

///
/// @brief
///     南瑞FT3采样值报文的编解码接口
///
class CTorVTFT3MsgImpl:public CTorVTFT3Msg
{
public:
	CTorVTFT3MsgImpl(CTORVT_FT3_TYPE eCTorVTFt3Type);
	virtual ~CTorVTFT3MsgImpl();

	virtual size_t Decode(const uint16_t *msgDataBuf, const size_t bufSize);
	virtual size_t Code(uint16_t *msgDataArr, const size_t arrSize);

	virtual size_t GetASDULen();
	virtual bool   GetASDUPublicInfo(CTORVT_FT3_ASDU_PUBLIC &asduPublic);
	virtual size_t GetASDUSmpsNum();
	virtual size_t GetASDUSmps(uint16_t *smpValArr, const size_t arrSize);

	virtual size_t GetASDUOff();
	virtual void   GetASDUPublicInfoOff(CTORVT_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff);
	virtual size_t GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize);

	virtual void   SetASDU(const CTORVT_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t bufSize);

private:
	CTorVTFT3AsduSeg *m_AsduSeg;
	uint16_t          m_MsgDataBuf[MAX_CTORVT_FT3_MSG_WORD];
	bool              m_bHaveDecode;
};

#endif
