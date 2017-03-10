/// @file
///     Dsp.cpp
/// @brief
///     DSP算法接口，用于各种算法的计算
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/03/23
/// @revise
/// 
#include "stdafx.h"
#include "../include/Dsp.h"

#include <assert.h>
#include <WinBase.h>
#include <pkfuncs.h>

#include "DspControllor.h"
#include "../include/DspDataSet.h"
#include "../../../inc/thread_priority.h"

bool CDsp::m_fRead = false;
HANDLE CDsp::m_iEvents[TypeNum]={NULL};
HANDLE CDsp::m_oEvents[TypeNum]={NULL};
SMV_GEN_DATA_CALC_RT *CDsp::m_pSmvGenCalcData = NULL;
CDspController *CDsp::m_pDspCtrl = NULL;
CDspDataSet *CDsp::m_pDspDataSet = NULL;
int CDsp::m_nErrno[TypeNum] = {0};
HANDLE CDsp::m_hThreadReadData = NULL;
///
/// @brief
///    发送采样值数据
/// 说明：
///    按照Dsp与Arm端通信协议（详见DspArmProtocal.h）传入采样数据给DSP
/// @param[in]
///    smvData           原始采样点数据
/// @return
///    bool                  true：成功
///                          false：DSP初始化失败或读写错误   
///
bool CDsp::SendSmv(const SmvData &smvData)
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return false;
    }

    // 发送给DSP
    int nRet = m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SMV_DATA, &smvData, 
            sizeof(SmvData));

    // 发送失败返回错误码
    if(nRet < 0)
    {
        return false;
    }
    return true;
}

///
/// @brief
///    设置DSP参数
/// @param[in] 
///    dspConfig    DSP的设置信息
/// @return
///    bool                  true：成功
///                          false：DSP初始化失败或读写错误
///
bool CDsp::SetConfig( const Config &dspConfig)
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return false;
    }
    int nRet =m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_CONFIG, 
                &dspConfig, sizeof(Config));
    if(nRet < 0)
    {
        return false;
    }
    return true;
}

///
/// @brief
///    生成波形数据
/// @param[in] 
///    smvGenData           传给DSP的所要生成的波形参数
/// @return
///    bool                  true：成功
///                          false：DSP初始化失败或读写错误
///
#if 0
bool CDsp::SendSmvGen(const SmvGenData &smvGenData)
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return false;
    }
    int nRet =m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SMV_GEN, &smvGenData, 
                sizeof(SmvGenData));
    if(nRet < 0)
    {
        return false;
    }
    return true;
}
#endif

///
/// @brief
///    发送报文时间数据
/// @param[in] 
///    msgTimeData          报文时间数据
/// @return
///    bool                  true：成功
///                          false：DSP初始化失败或读写错误
///
bool CDsp::SendMsgTime( const MsgTimeData &msgTimeData)
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return false;
    }
    int nRet = m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_TIME_DATA, &msgTimeData, 
        sizeof(MsgTimeData));
    if(nRet < 0)
    {
        return false;
    }
    return true;
}

///
/// @brief
///    获取DSP信息，如版本等
/// @param[out] 
///    dspInfo      DSP信息
/// @return
///    bool                  true：成功
///                          false：DSP初始化失败或读写错误
///
bool CDsp::GetDspInfo()
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return false;
    }
    int nRet =m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_INFO);
    if(nRet < 0)
    {
        return false;
    }
    return true;
}

///
/// @brief
///    清空DSP的采样点数据缓存 
/// @param[in]
///    nType                 0：清空采样值缓存，1：清空报文时间缓存
/// @return
///    bool                  true：成功
///                          false：DSP初始化失败或读写错误
///
bool CDsp::ResetCache(int nType)
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return false;
    }
    int nRet;
    switch(nType)
    {
    case 0:
        nRet = m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_RESET_SMV);
        break;
    case 1:
        nRet = m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_RESET_TIME);
        break;
    }
    if(nRet < 0)
    {
        return false;
    }
    return true;
}

// 默认构造函数
CDsp::CDsp()
{
    m_pDspCtrl = CDspController::GetInstance();

    // 创建内核事件对象用以控制发送和接收
    m_iEvents[T_SMV_GEN] = CreateEvent(NULL, false, true, NULL);
    m_oEvents[T_SMV_GEN] = CreateEvent(NULL, false, false, NULL);
    SetEvent(m_iEvents[T_SMV_GEN]);
    ResetEvent(m_oEvents[T_SMV_GEN]);

    m_nErrno[T_SMV_GEN] = 0;

    // 开启读线程
    StartRead();

    GetDspInfo();
}

// 需要外部数据集对象的指针
CDsp::CDsp( CDspDataSet *pDspDataSet )
{
    m_pDspCtrl = CDspController::GetInstance();
    m_pDspDataSet = pDspDataSet;
    // 开启读线程
    StartRead();

    GetDspInfo();
}

CDsp::~CDsp()
{
    CloseHandle(m_iEvents[T_SMV_GEN]);
    CloseHandle(m_oEvents[T_SMV_GEN]);   
}


///
/// @brief
///    读取数据线程函数
/// @param[] 
///    pArg
/// @return
///    DWORD                    0：成功
///
DWORD CDsp::ReadDataThread( LPVOID pArg )
{    
    UINT8 nCommandID;
    void *pData = NULL;
    UINT32 nDataLength = 0;
    while(m_fRead)
    {
        // 接收数据帧
        int nRet = m_pDspCtrl->FrameRecv(nCommandID, &pData, nDataLength);
        if(nRet < 0)
        {
            continue;
        }

        // 如果指示读进程退出
        if(nCommandID == DSP2ARM_CONTROLCODE_STOP_READ)
        {
            ExitThread(0);
            return 0;
        }

        // 如果采用的CDspDataSet数据集来存储计算结果
        if(NULL != m_pDspDataSet && nCommandID != DSP2ARM_CONTROLCODE_SMV_GEN_RT)
        {
            m_pDspDataSet->Write(nCommandID, pData, nDataLength);
        }

#if 1
        // 单独用于波形生成计算
        if(NULL != m_pSmvGenCalcData && nCommandID == DSP2ARM_CONTROLCODE_SMV_GEN_RT)
        {
            // 长度问题直接返回，用户端则会收到等待超时的错误
            if(nDataLength > sizeof(SMV_GEN_DATA_CALC_RT))
            {
                continue;
            } 
            // 用一个临时指针保存        
            void *pTmp = m_pSmvGenCalcData;

            // 激活发送事件
            SetEvent(m_iEvents[T_SMV_GEN]);

            // 拷贝到缓冲区中
            memcpy(pTmp, pData, nDataLength);

            // 激活接收事件
            SetEvent(m_oEvents[T_SMV_GEN]);        
        }
#endif
    }
    return 0;
}

///
/// @brief
///    启动读取线程
/// @return
///    bool                 true:启动读取线程成功（只有一个读取线程）
///                         false:DSP初始化不成功，不启动线程
///
bool CDsp::StartRead()
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return false;
    }
    if(false == m_fRead)
    {
        m_fRead = true;
        m_hThreadReadData = CreateThread(NULL, 0, ReadDataThread, this, 0, NULL);
        CeSetThreadPriority(m_hThreadReadData,DSP_RECV_PROC_PRIO);
    }
    return true;
}

///
/// @brief
///    停止读取线程
/// @return
///    void
///
void CDsp::StopRead()
{
    if(true == m_fRead)
    {        
        // 发送指令让DSP回应一个结束读线程的指令
        m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_STOP_READ);

        // 结束读线程
        DWORD dwResult = WaitForSingleObject(m_hThreadReadData,1000);
        if(WAIT_OBJECT_0 != dwResult)
        {
            TerminateThread(m_hThreadReadData, -1);       
        }
        CloseHandle(m_hThreadReadData);

        m_fRead = false;
    }
}

///
/// @brief
///    波形生成计算
/// @param[in] 
///    dwTimeout                等待超时时间
///    smvGenData               输入的波形生成参数
/// @param[out]
///    pSmvGenCalcRT            输出的波形生成计算数据指针
/// @return
///    int                      0：成功
///                            -1：DSP初始化失败
///                            -2：等待失败
///                            -3：等待超时
///                            -4：发送数据失败
///
int CDsp::SmvGenCalc( DWORD dwTimeout, const SmvGenData &smvGenData, 
                     SMV_GEN_DATA_CALC_RT *pSmvGenCalcRT )
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return -1;
    }
    
    // 等待写事件激活
    DWORD dwResult = CheckSend(T_SMV_GEN, dwTimeout);
    if(WAIT_FAILED == dwResult)
    {
        return -2;
    }

    if(WAIT_TIMEOUT == dwResult)
    {
        return -3;
    }

    // 存储缓冲区地址
    m_pSmvGenCalcData = pSmvGenCalcRT;

    // 向DSP发送数据
    int nRet =m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SMV_GEN, &smvGenData, 
        sizeof(SmvGenData));
    if(nRet < 0)
    {
        return -4;
    }

    return 0;
}

///
/// @brief
///    检测发送条件
/// @param[in] 
///    type                         发送哪种类型的数据
///    dwTimeout                    超时时间（毫秒）
/// @return
///    DWORD                        WAIT_OBJECT_0：成功
///                                 WAIT_FAILED：失败
///                                 WAIT_TIMEOUT：超时
///
DWORD CDsp::CheckSend( Type type, DWORD dwTimeout)
{
    DWORD dwResult = WaitForSingleObject(m_iEvents[type], dwTimeout);
    return dwResult;
}
///
/// @brief
///    等待接收特定类型的数据
/// @param[in] 
///    type                     要等待哪种类型的数据
///    dwMilliseconds           超时毫秒数
/// @return
///    int                       0：成功
///                             -1：DSP初始化失败
///                             -2：读取线程未启动
///                             -3：等待失败
///                             -4：等待超时
///                           正值：Dsp计算错误时的错误码
///
int CDsp::WaitForObject( Type type, DWORD dwMilliseconds )
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return -1;
    }

    // 读取线程未启动
    if(false == m_fRead)
    {
        return -2;
    }

    // 等待失败
    DWORD dwResult = WaitForSingleObject(m_oEvents[type], dwMilliseconds);
    if(WAIT_FAILED == dwResult)
    {
        return -3;
    }

    if(WAIT_TIMEOUT == dwResult)
    {
        return -4;
    }

    // 收到DSP错误信息返回错误码
    int nErrno = m_nErrno[type];
    if(0 != nErrno)
    {
        m_nErrno[type] = 0;
        return nErrno;
    }

    return 0;
}

bool CDsp::IsReading()
{
    return m_fRead;
}

bool DspInit()
{
	CDspController *pDspCtrl = CDspController::GetInstance();
    return pDspCtrl->DspInit();
}

void DspRelease()
{
	CDspController *pDspCtrl = CDspController::GetInstance();
    pDspCtrl->DspDelete();
	delete pDspCtrl;
}