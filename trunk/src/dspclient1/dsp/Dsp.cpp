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
#include <assert.h>


#include "../include/Dsp.h"
#include "../include/DspControllor.h"
#include "../include/DspDataSet.h"

bool CDsp::m_fRead = false;
HANDLE CDsp::m_iMutexs[6];             ///< 写互斥量
HANDLE CDsp::m_oMutexs[6];             ///< 读互斥量
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
int CDsp::SmvCalc(DWORD dwTimeout, const SmvData &smvData, SMV_DATA_CALC_RT *pSmvCalcRT);
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return -1;
    }

    // 等待信号量错误
    if(WAIT_OBJECT_0 != CheckSend(T_SMV))
    {
        return -2;
    }

    // 发送给DSP
    int nRet = m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SMV_DATA, &smvData, 
            sizeof(SmvData));

    // 发送失败
    if(nRet < 0)
    {
        return -3;
    }
    return 0;
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
int CDsp::SetConfig(DWORD dwTimeout, const Config &dspConfig);
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return -1;
    }

    if(WAIT_OBJECT_0 != CheckSend(T_CONFIG))
    {
        return -2;
    }

    int nRet =m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_CONFIG, 
                &dspConfig, sizeof(Config));
    if(nRet < 0)
    {
        return -3;
    }
    return 0;
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
int CDsp::SmvGenCalc(DWORD dwTimeout, const SmvGenData &smvGenData, 
               SMV_GEN_DATA_CALC_RT *pSmvGenCalcRT);
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return -1;
    }

    if(WAIT_OBJECT_0 != CheckSend(T_SMV_GEN))
    {
        return -2;
    }

    int nRet =m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SMV_GEN, &smvGenData, 
                sizeof(SmvGenData));
    if(nRet < 0)
    {
        return -3;
    }
    return 0;
}

///
/// @brief
///    发送报文时间数据
/// @param[in] 
///    msgTimeData          报文时间数据
/// @return
///    bool                  true：成功
///                          false：DSP初始化失败或读写错误
///
int CDsp::MsgTimeCalc(DWORD dwTimeout, const MsgTimeData &msgTimeData, 
                MSG_DISP_CALC_RT *pMsgDispRT);
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return -1;
    }

    if(WAIT_OBJECT_0 != CheckSend(T_MSG_TIME))
    {
        return -2;
    }

    int nRet = m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_TIME_DATA, &msgTimeData, 
        sizeof(MsgTimeData));

    if(nRet < 0)
    {
        return -3;
    }
    return 0;
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
int CDsp::GetDspInfo()
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return -1;
    }
    
    if(WAIT_OBJECT_0 != CheckSend(T_DSP_INFO))
    {
        return -2;
    }

    int nRet =m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_INFO);
    if(nRet < 0)
    {
        return -3;
    }
    return 0;
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
int CDsp::ResetCache(DWORD dwTimeout, int nType);
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return -1;
    }
    if(WAIT_OBJECT_0 != CheckSend(T_CONFIG))
    {
        return -2;
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
        return -3;
    }
    return 0;
}

CDsp::CDsp()
:m_hThreadReadData(NULL)
{
    m_pDspCtrl = CDspController::GetInstance();
    StartRead();
    for(int i=0; i<TypeNum; i++)
    {
        m_iMutexs[i] = CreateMutex(NULL, true, NULL);
        m_oMutexs[i] = CreateMutex(NULL, false, NULL);
    }
}

CDsp::~CDsp()
{
    StopRead();
    delete m_pDspCtrl;
}


///
/// @brief
///    读取数据线程函数
/// @param[] 
///    pArg
/// @return
///    DWORD
///
DWORD CDsp::ReadDataThread( LPVOID pArg )
{
    CDsp *pDsp= static_cast<CDsp *>(pArg);
    while(m_fRead)
    {      
        pDsp->ReadData();        
    }
    return 0;
}

///
/// @brief
///    将数据读取到DspDataSet集合中
/// @return
///    void
///
void CDsp::ReadData()
{
    AutoLock autoLock(m_ioLock);
    UINT8 nCommandID;
    void *pData = NULL;
    UINT32 nDataLength = 0;
    
    //TRACE(L"TIME_1:%d\t",GetTickCount());
    int nRet = m_pDspCtrl->FrameRecv(nCommandID, &pData, nDataLength);
    if(nRet < 0)
    {
        return;
    }
    m_dspDataSet.Write(nCommandID, pData, nDataLength);

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
        SetThreadPriority(m_hThreadReadData,THREAD_PRIORITY_TIME_CRITICAL);
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
        m_fRead = false;
        WaitForSingleObject(m_hThreadReadData,100);
        TerminateThread(m_hThreadReadData, 0);
        CloseHandle(m_hThreadReadData);
    }
}

CDsp *CDsp::GetInstance()
{
    static CDsp *pDsp = NULL;
    if(NULL == pDsp)
    {
        m_singleLock.Acquire();
        if(NULL == pDsp)
        {
            pDsp = new CDsp();
        }
        m_singleLock.Release();
    }
    return pDsp;
}

DWORD CDsp::CheckSend( Type type )
{
    DWORD dwResult = WaitForSingleObject(m_iMutexs[type], dwTimeout);
    return dwResult;
}

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
    DWORD dwResult = WaitForSingleObject(m_oMutexs[type], dwMilliseconds);
    if(WAIT_OBJECT_0 != dwResult)
    {
        return -3;
    }

    // 收到DSP错误信息返回错误码
    int errno = m_errno[type];
    if(0 != errno)
    {
        m_errno[type] = 0;
        return errno;
    }

    return 0;
    
}
