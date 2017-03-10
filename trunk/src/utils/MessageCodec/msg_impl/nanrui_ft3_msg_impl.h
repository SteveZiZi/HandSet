#ifndef NANRUI_FT3_MSG_IMPL_H_

#define NANRUI_FT3_MSG_IMPL_H_

#include "../include/nanrui_ft3_msg.h"

class NanRuiFT3AsduSeg;

///
/// @brief
///     南瑞FT3采样值报文的编解码接口
///
class NanRuiFT3MsgImpl:public NanRuiFT3Msg
{
public:
	NanRuiFT3MsgImpl();
	virtual ~NanRuiFT3MsgImpl();

	virtual size_t Decode(const uint16_t *msgDataBuf, const size_t bufSize);
	virtual size_t Code(uint16_t *msgDataArr, const size_t arrSize);

	virtual size_t GetASDULen();
	virtual bool   GetASDUPublicInfo(NANRUI_FT3_ASDU_PUBLIC &asduPublic);
	virtual size_t GetASDUSmpsNum();
	virtual size_t GetASDUSmps(uint16_t *smpValArr, const size_t arrSize);
	
	virtual size_t GetASDUOff();
	virtual void   GetASDUPublicInfoOff(NANRUI_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff);
	virtual size_t GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize);
	
	virtual void   SetASDU(const NANRUI_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t bufSize);

private:
	NanRuiFT3AsduSeg *m_AsduSeg;
	uint16_t          m_MsgDataBuf[NANRUI_FT3_MSG_WORD];
	bool              m_bHaveDecode;
};

#endif
