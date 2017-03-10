#ifndef DSP_ARM_PROTOCOL_H_
#define DSP_ARM_PROTOCOL_H_

///
#define ARM_DSP_BUF_SIZE (32*1024)

///
/// @brief
///     ARM对DSP的控制命令（功能码）
///
#define TEST		 (0x100)				///< 用于测试
#define QUIT         (0xFFF)				///< 退出DSP
///
/// @brief
///     定义ARM发送到DSP的数据帧的帧头,帧头之后接float型的数据段
///
typedef struct
{
	Uint16 iCommandID;                      ///< ARM对DSP的控制命令（功能码）
	Uint32 iLength;                         ///< 数据长度
}FrameHead, *PFrameHead;

#endif
