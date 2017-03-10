#ifndef DSP_ARM_PROTOCOL_H_
#define DSP_ARM_PROTOCOL_H_

///
#define ARM_DSP_BUF_SIZE (32*1024)

///
/// @brief
///     ARM��DSP�Ŀ�����������룩
///
#define TEST		 (0x100)				///< ���ڲ���
#define QUIT         (0xFFF)				///< �˳�DSP
///
/// @brief
///     ����ARM���͵�DSP������֡��֡ͷ,֡ͷ֮���float�͵����ݶ�
///
typedef struct
{
	Uint16 iCommandID;                      ///< ARM��DSP�Ŀ�����������룩
	Uint32 iLength;                         ///< ���ݳ���
}FrameHead, *PFrameHead;

#endif
