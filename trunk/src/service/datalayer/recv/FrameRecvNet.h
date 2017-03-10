/// @file
///
/// @brief
///     FPGA��̫�����ݽ�����
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.3.20
///
/// �޸���ʷ��
///

#pragma once

// LPTHREAD_START_ROUTINE �¼� FileMap �̵߳Ȳ���
#include <Windows.h>
// uint32_t
#include "src\include\stdint.h"
// Net_Port
#include "src\service\datalayer\include\datatypes.h"
// Ft3_Port / use  APP_PRIO in .cpp
#include "src\service\datalayer\include\ctrlcode_def.h"
#include "FrameRecvStruct.h"
#include "src\dspclient\dsp\lock.h"
#include "src\service\datalayer\include\config.h"

#include "src\utils\MessageCodec\include\sav91_msg.h"
#include "src\utils\MessageCodec\include\sav92_msg.h"
#include "src\utils\MessageCodec\include\goose_msg.h"
#include "src\utils\MessageCodec\include\msg_rapid_decoder.h"


//ǰ��������Net���а���һ��ָ��������ָ�룬�������ݵľ��崦���ڽ�������
class CFrameRecv;

class CFrameRecvNet
{
public:
    CFrameRecvNet(CFrameRecv* pFrameRecv, HANDLE hGPMDevice, DsmPortName enPortName);
    ~CFrameRecvNet(void);

public:
    ///
    /// @brief
    ///    ��ʼ���������
    ///
    /// @return
    ///     true����ɹ�
    ///
    bool InitNet(const wchar_t *pNotifyEvtName, const wchar_t *pMapFileName, uint32_t nMapFileLen);

    ///
    /// @brief
    ///    �����������
    ///
    void UnInitNet(void);

    ///
    /// @brief
    ///    ��ʼ�������
    ///
    bool StartNetRecv();

    ///
    /// @brief
    ///    ֹͣ�������
    ///
    bool StopNetRecv();

    ///
    /// @brief
    ///    ��������ӿڹ���
    ///
    bool SetNetFilter( IOCTRL_IN_PARAM_NET_BIG& stIoCtrlParamNet );

    ///
    /// @brief
    ///    ��������ӿ�̽��
    ///
    /// @param[in]  enNetRecvFrameType  ָ��̽�������֡����
    /// @param[in]  nCount              ����һ�ν��յ���������
    /// @param[in]  bEnableTimeout      �Ƿ����ý��ճ�ʱ
    /// @param[in]  nTimeoutMilliSecond ָ����ʱʱ�䣬���ó�ʱ��ʱ����Ч
    ///
    /// @return
    ///     ̽����Ϣ���óɹ�����true
    ///
    bool Detect(NetFt3RecvFrameType enNetRecvFrameType, uint16_t nCount, bool bEnableTimeout, uint32_t nTimeoutMilliSecond);

    ///
    /// @brief
    ///    ���ý��ճ�ʱ,��λ����
    ///
    /// @param[in]  nTimeoutMilliSecond ָ����ʱʱ�䣬���ó�ʱ��ʱ����Ч
    ///
    /// @return
    ///     ���óɹ�����true
    ///
    bool SetRecvTimeOut(uint32_t nMilliSecond = 10);

    ///
    /// @brief
    ///    ����Fpga�������ݽ���
    ///
    /// @param[in]  nCount              ����һ�ν��յ���������
    /// @param[in]  bEnableTimeout      �Ƿ����ý��ճ�ʱ
    ///
    /// @return
    ///     ���óɹ�����true
    ///
    bool StartFpgaRecvLoop(uint16_t nCount, bool bEnableTimeOut);

    ///
    /// @brief
    ///    �Թ�����������
    ///
    void LockBuffer()
    {
        if (m_pDataBuf)
        {
            m_lockSharedBuffer.Acquire();
            BufferHeader* pBufferHeader = reinterpret_cast<BufferHeader*>(m_pDataBuf);
            pBufferHeader->NetHeader.bAppDispDone = 0;
        }

    }

    ///
    /// @brief
    ///    �Թ��׻���������
    ///
    void UnLockBuffer()
    {
        if (m_pDataBuf)
        {
            m_lockSharedBuffer.Release();
            BufferHeader* pBufferHeader = reinterpret_cast<BufferHeader*>(m_pDataBuf);
            pBufferHeader->NetHeader.bAppDispDone = 1;
        }
    }

    ///
    /// @brief
    ///    ���ý����߳����ȼ�
    ///
    /// @param[in]  nPriority Ҫ���õ����ȼ�
    ///
    /// @return
    ///     bool ���óɹ�����true
    ///
    bool SetRxThreadPriority(int nPriority);

private:

    friend DWORD NetThreadProc(PVOID pArg);
    friend class CFrameRecv;

private:
    HANDLE              m_hExitEvent;                                   ///< �жϴ����߳��˳��¼�
    HANDLE              m_hNotifyEvent;                                 ///< ֪ͨͨ��xָ���Ľ��ղ������
    bool                m_bRecv;                                        ///< �Ƿ��������
    HANDLE              m_hGPMDevice;                                   ///< GPM�������
    HANDLE              m_hMapping;                                     ///< ͨ��x�����������ݵ��ڴ�ӳ��
    uint8_t*            m_pDataBuf;                                     ///< ָ��ͨ��x�Ļ�����
    HANDLE              m_hThread;                                      ///< ��̫�� �����߳̾��
    DsmPortName         m_enPortName;                                   ///< ��̫��ʵ����Ӧ��ʵ�ʶ˿�
    CFrameRecv*         m_pFrameRecv;                                   ///< ���崦����յ����ݵ���ָ��
    bool                m_bIsInited;                                    ///< ��������Ƿ��Ѿ���ʼ����
    Lock                m_lockSharedBuffer;                             ///< ����������ʹ�û�����ʱ����
    Sav91Msg*           m_pSav91Msg;                                    ///< ����ζ˿�9-1���ݽ���
    Sav92Msg*           m_pSav92Msg;                                    ///< ����ζ˿�9-2���ݽ���
    GooseMsg*           m_pGooseMsg;                                    ///< ����ζ˿�Goose���ݽ���
    MsgRapidDecoder     m_cMsgRapidDecoder;                             ///< ����˿ڵĿ��ٽ���
    Sav91Msg*           m_pSav91Msg2;                                   ///< ����˶˿ں���ʱ�ڶ������ƿ����ݽ���
    Sav92Msg*           m_pSav92Msg2;                                   ///< ����˶˿ں���ʱ�ڶ������ƿ����ݽ���
};

///
/// @brief
///    �����жϴ������
///
DWORD NetThreadProc(PVOID pArg);
