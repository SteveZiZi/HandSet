/// @file
///
/// @brief
///     FPGA以太网数据接收类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.3.20
///
/// 修改历史：
///

#pragma once

// LPTHREAD_START_ROUTINE 事件 FileMap 线程等操作
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


//前向声明，Net类中包含一个指向接受类的指针，接受数据的具体处理在接受类中
class CFrameRecv;

class CFrameRecvNet
{
public:
    CFrameRecvNet(CFrameRecv* pFrameRecv, HANDLE hGPMDevice, DsmPortName enPortName);
    ~CFrameRecvNet(void);

public:
    ///
    /// @brief
    ///    初始化网络接收
    ///
    /// @return
    ///     true代表成功
    ///
    bool InitNet(const wchar_t *pNotifyEvtName, const wchar_t *pMapFileName, uint32_t nMapFileLen);

    ///
    /// @brief
    ///    网络接收清理
    ///
    void UnInitNet(void);

    ///
    /// @brief
    ///    开始网络接收
    ///
    bool StartNetRecv();

    ///
    /// @brief
    ///    停止网络接收
    ///
    bool StopNetRecv();

    ///
    /// @brief
    ///    设置网络接口过滤
    ///
    bool SetNetFilter( IOCTRL_IN_PARAM_NET_BIG& stIoCtrlParamNet );

    ///
    /// @brief
    ///    设置网络接口探测
    ///
    /// @param[in]  enNetRecvFrameType  指定探测的数据帧类型
    /// @param[in]  nCount              设置一次接收的数据桢数
    /// @param[in]  bEnableTimeout      是否启用接收超时
    /// @param[in]  nTimeoutMilliSecond 指定超时时间，启用超时的时候有效
    ///
    /// @return
    ///     探测信息设置成功返回true
    ///
    bool Detect(NetFt3RecvFrameType enNetRecvFrameType, uint16_t nCount, bool bEnableTimeout, uint32_t nTimeoutMilliSecond);

    ///
    /// @brief
    ///    设置接收超时,单位毫秒
    ///
    /// @param[in]  nTimeoutMilliSecond 指定超时时间，启用超时的时候有效
    ///
    /// @return
    ///     设置成功返回true
    ///
    bool SetRecvTimeOut(uint32_t nMilliSecond = 10);

    ///
    /// @brief
    ///    设置Fpga进行数据接收
    ///
    /// @param[in]  nCount              设置一次接收的数据桢数
    /// @param[in]  bEnableTimeout      是否启用接收超时
    ///
    /// @return
    ///     设置成功返回true
    ///
    bool StartFpgaRecvLoop(uint16_t nCount, bool bEnableTimeOut);

    ///
    /// @brief
    ///    对共享缓冲区加锁
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
    ///    对贡献缓冲区解锁
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
    ///    设置接收线程优先级
    ///
    /// @param[in]  nPriority 要设置的优先级
    ///
    /// @return
    ///     bool 设置成功返回true
    ///
    bool SetRxThreadPriority(int nPriority);

private:

    friend DWORD NetThreadProc(PVOID pArg);
    friend class CFrameRecv;

private:
    HANDLE              m_hExitEvent;                                   ///< 中断处理线程退出事件
    HANDLE              m_hNotifyEvent;                                 ///< 通知通道x指定的接收操作完成
    bool                m_bRecv;                                        ///< 是否接收数据
    HANDLE              m_hGPMDevice;                                   ///< GPM驱动句柄
    HANDLE              m_hMapping;                                     ///< 通道x用来缓冲数据的内存映射
    uint8_t*            m_pDataBuf;                                     ///< 指向通道x的缓冲区
    HANDLE              m_hThread;                                      ///< 以太网 处理线程句柄
    DsmPortName         m_enPortName;                                   ///< 以太网实例对应的实际端口
    CFrameRecv*         m_pFrameRecv;                                   ///< 具体处理接收到数据的类指针
    bool                m_bIsInited;                                    ///< 网络接收是否已经初始化过
    Lock                m_lockSharedBuffer;                             ///< 和驱动交互使用缓冲区时加锁
    Sav91Msg*           m_pSav91Msg;                                    ///< 负责次端口9-1数据解析
    Sav92Msg*           m_pSav92Msg;                                    ///< 负责次端口9-2数据解析
    GooseMsg*           m_pGooseMsg;                                    ///< 负责次端口Goose数据解析
    MsgRapidDecoder     m_cMsgRapidDecoder;                             ///< 负责端口的快速解析
    Sav91Msg*           m_pSav91Msg2;                                   ///< 负责此端口核相时第二个控制块数据解析
    Sav92Msg*           m_pSav92Msg2;                                   ///< 负责此端口核相时第二个控制块数据解析
};

///
/// @brief
///    网络中断处理程序
///
DWORD NetThreadProc(PVOID pArg);
