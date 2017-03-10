#ifndef DSP_CONTROLLOR_H_

#define DSP_CONTROLLOR_H_

#include "loop.h"
#include "DspArmProtocol.h"

///
/// @brief
///     DSP端控制器，主要有ARM端对DSP端的数据的发送和接收以及控制DSP端执行各类算法
///
class DSPControllor
{
public:
	BOOL  DSPInit() const;
	void  DSPDelete();

protected:
	BOOL  FrameRecv();
	BOOL  FrameSend(const Uint16 commandID);

private:
	static Uint8 *m_SendBuf;                ///< 发送时的指针，指向发送缓存区
	static Uint8 *m_RecvBuf;                ///< 接收时的指针，指向接收缓存区
};

#endif
