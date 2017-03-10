/// @file
///
/// @brief
///     DSP�˿����������ڿ���DSP�˵ĳ�ʼ����������ִ�к����㷨
///
/// @note
///     Ŀǰ��ʵ���˼�����Чֵ���㷨�Ŀ��ơ�DSP�˵ĳ�ʼ���ͽ���
///
/// Copyright (c) 2012 IUnknownBase Inc
///
/// ���ߣ�
///    chao  2012.12.19
///
/// �޸���ʷ��
///

#include "stdafx.h"
#include "DSPControllor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///
/// @brief
///     ��̬���ӿ�
///
#pragma comment(lib,"Loop.lib")

///
/// @brief
///     ARM��DSP���н��������ݻ�����
///
Uint8 *DSPControllor::m_SendBuf = NULL;   ///< ����ʱ��ָ�룬ָ���ͻ�����
Uint8 *DSPControllor::m_RecvBuf = NULL;   ///< ����ʱ��ָ�룬ָ����ջ�����

///
/// @brief
///     ��ʼ��DSP��
///
/// @return
///     �ɹ�����true�����򷵻�false
///
BOOL DSPControllor::DSPInit() const
{
	// �����û��㷨���������û���ִ�г���(.out�ļ���DSP��)������ARM��DSPͨ�Ź�����������
	char *DspName ="\\FlashDisk\\loop.out";    // DSP�Ĵ����ļ�
    CStringA strBufSize;
    strBufSize.Format("%d",ARM_DSP_BUF_SIZE);
	//char *BufSize = "122880";                    // DSPͨ��ʱ�����ݽ������ڴ泤��(��������չ����ڴ�) , �����С���û����壬ע����DSP�˷���Ĵ�СҪ����һ��
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
///     �Դ�ARM�˴��䵽DSP�˵����ݽ��б��벢����
///
/// @param[in]
///     command     - ������Ķ�DSP�Ŀ�����������룩
/// @param[in]
///     sampleValue - ָ��洢����ֵ���ݵĽṹ���ָ��
///
/// @return
///     �ɹ�����true�����򷵻�false
///
BOOL DSPControllor::FrameSend(const Uint16 commandID)
{
	FrameHead * fHead = (FrameHead *)m_SendBuf;
	float * pTmp = (float *)(m_SendBuf + sizeof(FrameHead));

	// �������֡
	fHead->iCommandID = commandID;
    fHead->iLength = ARM_DSP_BUF_SIZE-sizeof(FrameHead);

	// ��DSP�˷������ݣ�����DSP�㷨
	DSP_STATUS status = DSP_Execute_Send(0);
	
	if ( DSP_FAILED(status) )
	{
		return FALSE;
	}
	return TRUE;
}

///
/// @brief
///     �������DSP���ݣ��������յ������ݷ��ص�*m_RecvBuf������
///
/// @return
///     �ɹ�����true�����򷵻�false
///
BOOL DSPControllor::FrameRecv()
{
	// �������DSP���ݣ��������յ������ݷ��ص�*m_RecvBuf������
	DSP_STATUS status = DSP_Execute_Receive(0,&m_RecvBuf);
	
	if ( DSP_FAILED(status) )
	{
		return FALSE;
	}
	return TRUE;
}

///
/// @brief
///     ����DSP��
///
void DSPControllor::DSPDelete()
{
	//ж��DSP�˴��룬�ͷ���Դ���ڴ棩
    FrameSend(QUIT);
    DSP_Delete(0);
}
