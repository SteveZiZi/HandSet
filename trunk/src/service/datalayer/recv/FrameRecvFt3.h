/// @file
///
/// @brief
///     FPGA FT3���ݽ��մ�����
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
// Ft3_Port
#include "src\service\datalayer\include\datatypes.h"
// Ft3_Port / use  APP_PRIO in .cpp
#include "src\service\datalayer\include\ctrlcode_def.h"
#include "src\dspclient\dsp\lock.h"
#include "src\service\datalayer\include\config.h"

#include "src\utils\MessageCodec\include\guowang_ft3_msg.h"
#include "src\utils\MessageCodec\include\nanrui_ft3_msg.h"
#include "src\utils\MessageCodec\include\msg_rapid_decoder.h"


//ǰ��������FT3���а���һ��ָ��������ָ�룬�������ݵľ��崦���ڽ�������
class CFrameRecv;

class CFrameRecvFt3
{
public:
    CFrameRecvFt3(CFrameRecv *pFrameRecv, HANDLE hGPMDevice, DsmPortName enPortName);
    ~CFrameRecvFt3(void);


public:
    ///
    /// @brief
    ///    ��ʼ��FT3����
    ///
    /// @return
    ///     true����ɹ�
    ///
    bool InitFt3(const wchar_t *pNotifyEvtName,const wchar_t *pMapFileName, uint32_t nMapFileLen );

    ///
    /// @brief
    ///    Ft3����ж��
    ///
    void UnInitFt3(void);

    ///
    /// @brief
    ///    ��ʼft3����
    ///
    bool StartFt3Recv();

    ///
    /// @brief
    ///    ֹͣFt3����
    ///
    bool StopFt3Recv();

    ///
    /// @brief
    ///    ���ý��ճ�ʱ,��λ����
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
            pBufferHeader->Ft3Header.bAppDispDone = 0;
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
            pBufferHeader->Ft3Header.bAppDispDone = 1;
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
    friend DWORD Ft3ThreadProc(PVOID pArg);
    friend class CFrameRecv;


private:
    HANDLE              m_hExitEvent;                                   ///< �жϴ����߳��˳��¼�
    HANDLE              m_hNotifyEvent;                                 ///< ֪ͨͨ��xָ���Ľ��ղ������
    bool                m_bRecv;                                        ///< �Ƿ����������
    HANDLE              m_hGPMDevice;                                   ///< GPM�������
    HANDLE              m_hMapping;                                     ///< ͨ��x�����������ݵ��ڴ�ӳ��
    uint8_t *           m_pDataBuf;                                     ///< ָ��ͨ��x�Ļ�����
    HANDLE              m_hThread;                                      ///< FT3 �����߳̾��
    DsmPortName         m_enPortName;                                   ///< Ft3ʵ����Ӧ��ʵ�ʶ˿�
    CFrameRecv*         m_pFrameRecv;                                   ///< ָ����������ݵ���ָ��
    bool                m_bIsInited;                                    ///< Ft3�����Ƿ��Ѿ���ʼ��
    Lock                m_lockSharedBuffer;                             ///< ����������ʹ�û�����ʱ����
    GuoWangFT3Msg*      m_pGuoWangFT3Msg;                               ///< �����˿��Ϲ�������֡����
    NanRuiFT3Msg*       m_pNanRuiFt3Msg;                                ///< �����˿�����������֡����
    MsgRapidDecoder     m_cMsgRapidDecoder;                             ///< ����˿ڵĿ��ٽ���


};

///
/// @brief
///    �жϷ����߳�
///
DWORD Ft3ThreadProc(PVOID pArg);

