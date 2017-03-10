/// @file
///
/// @brief
///     FPGA FT3数据接收处理类
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
// Ft3_Port
#include "src\service\datalayer\include\datatypes.h"
// Ft3_Port / use  APP_PRIO in .cpp
#include "src\service\datalayer\include\ctrlcode_def.h"
#include "src\dspclient\dsp\lock.h"
#include "src\service\datalayer\include\config.h"

#include "src\utils\MessageCodec\include\guowang_ft3_msg.h"
#include "src\utils\MessageCodec\include\nanrui_ft3_msg.h"
#include "src\utils\MessageCodec\include\msg_rapid_decoder.h"


//前向声明，FT3类中包含一个指向接受类的指针，接受数据的具体处理在接受类中
class CFrameRecv;

class CFrameRecvFt3
{
public:
    CFrameRecvFt3(CFrameRecv *pFrameRecv, HANDLE hGPMDevice, DsmPortName enPortName);
    ~CFrameRecvFt3(void);


public:
    ///
    /// @brief
    ///    初始化FT3接收
    ///
    /// @return
    ///     true代表成功
    ///
    bool InitFt3(const wchar_t *pNotifyEvtName,const wchar_t *pMapFileName, uint32_t nMapFileLen );

    ///
    /// @brief
    ///    Ft3接收卸载
    ///
    void UnInitFt3(void);

    ///
    /// @brief
    ///    开始ft3接收
    ///
    bool StartFt3Recv();

    ///
    /// @brief
    ///    停止Ft3接收
    ///
    bool StopFt3Recv();

    ///
    /// @brief
    ///    设置接收超时,单位毫秒
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
            pBufferHeader->Ft3Header.bAppDispDone = 0;
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
            pBufferHeader->Ft3Header.bAppDispDone = 1;
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
    friend DWORD Ft3ThreadProc(PVOID pArg);
    friend class CFrameRecv;


private:
    HANDLE              m_hExitEvent;                                   ///< 中断处理线程退出事件
    HANDLE              m_hNotifyEvent;                                 ///< 通知通道x指定的接收操作完成
    bool                m_bRecv;                                        ///< 是否处理接收数据
    HANDLE              m_hGPMDevice;                                   ///< GPM驱动句柄
    HANDLE              m_hMapping;                                     ///< 通道x用来缓冲数据的内存映射
    uint8_t *           m_pDataBuf;                                     ///< 指向通道x的缓冲区
    HANDLE              m_hThread;                                      ///< FT3 处理线程句柄
    DsmPortName         m_enPortName;                                   ///< Ft3实例对应的实际端口
    CFrameRecv*         m_pFrameRecv;                                   ///< 指向处理接收数据的类指针
    bool                m_bIsInited;                                    ///< Ft3接收是否已经初始化
    Lock                m_lockSharedBuffer;                             ///< 和驱动交互使用缓冲区时加锁
    GuoWangFT3Msg*      m_pGuoWangFT3Msg;                               ///< 管理本端口上国网数据帧解析
    NanRuiFT3Msg*       m_pNanRuiFt3Msg;                                ///< 管理本端口上南瑞数据帧解析
    MsgRapidDecoder     m_cMsgRapidDecoder;                             ///< 负责端口的快速解析


};

///
/// @brief
///    中断服务线程
///
DWORD Ft3ThreadProc(PVOID pArg);

