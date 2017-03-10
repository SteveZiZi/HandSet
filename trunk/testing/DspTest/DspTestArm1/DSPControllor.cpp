/// @file
///
/// @brief
///     DSP端控制器，用于控制DSP端的初始化、结束和执行何种算法
///
/// @note
///     目前仅实现了计算有效值的算法的控制、DSP端的初始化和结束
///
/// Copyright (c) 2012 IUnknownBase Inc
///
/// 作者：
///    chao  2012.12.19
///
/// 修改历史：
///

#include "stdafx.h"
#include "DSPControllor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///
/// @brief
///     动态链接库
///
#pragma comment(lib,"Loop.lib")

///
/// @brief
///     ARM和DSP进行交互的数据缓存区
///
Uint8 *DSPControllor::m_SendBuf = NULL;   ///< 发送时的指针，指向发送缓存区
Uint8 *DSPControllor::m_RecvBuf = NULL;   ///< 接收时的指针，指向接收缓存区

///
/// @brief
///     初始化DSP端
///
/// @return
///     成功返回true；否则返回false
///
BOOL DSPControllor::DSPInit() const
{
	// 创建用户算法程序，下载用户可执行程序(.out文件到DSP端)并分配ARM和DSP通信共享数据区域
	char *DspName ="\\FlashDisk\\loop.out";    // DSP的代码文件
    CStringA strBufSize;
    strBufSize.Format("%d",ARM_DSP_BUF_SIZE);
	//char *BufSize = "122880";                    // DSP通信时的数据交换的内存长度(发送与接收共用内存) , 这个大小有用户定义，注意与DSP端分配的大小要保持一致
	DSP_STATUS status = DSP_Create(DspName, &m_SendBuf, 
        strBufSize.GetBuffer(strBufSize.GetLength()), 0);

    //DSP_STATUS status = DSP_Create(DspName, &m_SendBuf, BufSize, 0);
	
	if ( DSP_FAILED(status) )
	{
		return FALSE;
	}
	return TRUE;
}

///
/// @brief
///     对从ARM端传输到DSP端的数据进行编码并发送
///
/// @param[in]
///     command     - 所定义的对DSP的控制命令（功能码）
/// @param[in]
///     sampleValue - 指向存储采样值数据的结构体的指针
///
/// @return
///     成功返回true；否则返回false
///
BOOL DSPControllor::FrameSend(const Uint16 commandID)
{
	FrameHead * fHead = (FrameHead *)m_SendBuf;
	float * pTmp = (float *)(m_SendBuf + sizeof(FrameHead));

	// 组合数据帧
	fHead->iCommandID = commandID;
    fHead->iLength = ARM_DSP_BUF_SIZE-sizeof(FrameHead);

	// 向DSP端发送数据，启动DSP算法
	DSP_STATUS status = DSP_Execute_Send(0);
	
	if ( DSP_FAILED(status) )
	{
		return FALSE;
	}
	return TRUE;
}

///
/// @brief
///     挂起接收DSP数据，并将接收到的数据返回到*m_RecvBuf缓存区
///
/// @return
///     成功返回true；否则返回false
///
BOOL DSPControllor::FrameRecv()
{
	// 挂起接收DSP数据，并将接收到的数据返回到*m_RecvBuf缓存区
	DSP_STATUS status = DSP_Execute_Receive(0,&m_RecvBuf);
	
	if ( DSP_FAILED(status) )
	{
		return FALSE;
	}
	return TRUE;
}

///
/// @brief
///     结束DSP端
///
void DSPControllor::DSPDelete()
{
	//卸载DSP端代码，释放资源（内存）
    FrameSend(QUIT);
    DSP_Delete(0);
}
