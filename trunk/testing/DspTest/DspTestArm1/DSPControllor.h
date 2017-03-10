#ifndef DSP_CONTROLLOR_H_

#define DSP_CONTROLLOR_H_

#include "loop.h"
#include "DspArmProtocol.h"

///
/// @brief
///     DSP�˿���������Ҫ��ARM�˶�DSP�˵����ݵķ��ͺͽ����Լ�����DSP��ִ�и����㷨
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
	static Uint8 *m_SendBuf;                ///< ����ʱ��ָ�룬ָ���ͻ�����
	static Uint8 *m_RecvBuf;                ///< ����ʱ��ָ�룬ָ����ջ�����
};

#endif
