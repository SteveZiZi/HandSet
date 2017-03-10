#ifndef GUOWANG_FT3_MSG_IMPL_H_

#define GUOWANG_FT3_MSG_IMPL_H_

#include "../include/guowang_ft3_msg.h"

class GuoWangFT3AsduSeg;

///
/// @brief
///     国网FT3采样值报文的编解码接口
///
class GuoWangFT3MsgImpl:public GuoWangFT3Msg
{
public:
	GuoWangFT3MsgImpl();
	virtual ~GuoWangFT3MsgImpl();

	virtual size_t Decode(const uint16_t *msgDataBuf, const size_t bufSize);
	virtual size_t Code(uint16_t *msgDataArr, const size_t arrSize);

	virtual size_t GetASDULen();
	virtual bool   GetASDUPublicInfo(GUOWANG_FT3_ASDU_PUBLIC &asduPublic);
	virtual size_t GetASDUSmpsNum();
	virtual size_t GetASDUSmps(uint16_t *smpValArr, const size_t arrSize);

	virtual size_t GetASDUOff();
	virtual void   GetASDUPublicInfoOff(GUOWANG_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff);
	virtual size_t GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize);

	virtual void   SetASDU(const GUOWANG_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t bufSize);

private:
	GuoWangFT3AsduSeg *m_AsduSeg;
	uint16_t           m_MsgDataBuf[GUOWANG_FT3_MSG_WORD];
	bool               m_bHaveDecode;
};

#endif
