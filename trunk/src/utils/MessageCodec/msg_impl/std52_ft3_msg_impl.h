#ifndef STD52_FT3_MSG_IMPL_H_

#define STD52_FT3_MSG_IMPL_H_

#include "../include/std52_ft3_msg.h"

class Std52FT3AsduSeg;

///
/// @brief
///     南瑞FT3采样值报文的编解码接口
///
class Std52FT3MsgImpl:public Std52FT3Msg
{
public:
	Std52FT3MsgImpl();
	virtual ~Std52FT3MsgImpl();

	virtual size_t Decode(const uint16_t *msgDataBuf, const size_t bufSize);
	virtual size_t Code(uint16_t *msgDataArr, const size_t arrSize);

	virtual size_t GetASDULen();
	virtual bool   GetASDUPublicInfo(STD52_FT3_ASDU_PUBLIC &asduPublic);
	virtual size_t GetASDUSmpsNum();
	virtual size_t GetASDUSmps(uint16_t *smpValArr, const size_t arrSize);

	virtual size_t GetASDUOff();
	virtual void   GetASDUPublicInfoOff(STD52_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff);
	virtual size_t GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize);

	virtual void   SetASDU(const STD52_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t bufSize);

private:
	Std52FT3AsduSeg *m_AsduSeg;
	uint16_t         m_MsgDataBuf[STD52_FT3_MSG_WORD];
	bool             m_bHaveDecode;
};

#endif
