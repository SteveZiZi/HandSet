/// @file
///     Dsp.cpp
/// @brief
///     DSP�㷨�ӿڣ����ڸ����㷨�ļ���
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
HANDLE CDsp::m_iMutexs[6];             ///< д������
HANDLE CDsp::m_oMutexs[6];             ///< ��������
///
/// @brief
///    ���Ͳ���ֵ����
/// ˵����
///    ����Dsp��Arm��ͨ��Э�飨���DspArmProtocal.h������������ݸ�DSP
/// @param[in]
///    smvData           ԭʼ����������
/// @return
///    bool                  true���ɹ�
///                          false��DSP��ʼ��ʧ�ܻ��д����   
///
int CDsp::SmvCalc(DWORD dwTimeout, const SmvData &smvData, SMV_DATA_CALC_RT *pSmvCalcRT);
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return -1;
    }

    // �ȴ��ź�������
    if(WAIT_OBJECT_0 != CheckSend(T_SMV))
    {
        return -2;
    }

    // ���͸�DSP
    int nRet = m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SMV_DATA, &smvData, 
            sizeof(SmvData));

    // ����ʧ��
    if(nRet < 0)
    {
        return -3;
    }
    return 0;
}

///
/// @brief
///    ����DSP����
/// @param[in] 
///    dspConfig    DSP��������Ϣ
/// @return
///    bool                  true���ɹ�
///                          false��DSP��ʼ��ʧ�ܻ��д����
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
///    ���ɲ�������
/// @param[in] 
///    smvGenData           ����DSP����Ҫ���ɵĲ��β���
/// @return
///    bool                  true���ɹ�
///                          false��DSP��ʼ��ʧ�ܻ��д����
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
///    ���ͱ���ʱ������
/// @param[in] 
///    msgTimeData          ����ʱ������
/// @return
///    bool                  true���ɹ�
///                          false��DSP��ʼ��ʧ�ܻ��д����
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
///    ��ȡDSP��Ϣ����汾��
/// @param[out] 
///    dspInfo      DSP��Ϣ
/// @return
///    bool                  true���ɹ�
///                          false��DSP��ʼ��ʧ�ܻ��д����
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
///    ���DSP�Ĳ��������ݻ��� 
/// @param[in]
///    nType                 0����ղ���ֵ���棬1����ձ���ʱ�仺��
/// @return
///    bool                  true���ɹ�
///                          false��DSP��ʼ��ʧ�ܻ��д����
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
///    ��ȡ�����̺߳���
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
///    �����ݶ�ȡ��DspDataSet������
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
///    ������ȡ�߳�
/// @return
///    bool                 true:������ȡ�̳߳ɹ���ֻ��һ����ȡ�̣߳�
///                         false:DSP��ʼ�����ɹ����������߳�
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
///    ֹͣ��ȡ�߳�
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

    // ��ȡ�߳�δ����
    if(false == m_fRead)
    {
        return -2;
    }

    // �ȴ�ʧ��
    DWORD dwResult = WaitForSingleObject(m_oMutexs[type], dwMilliseconds);
    if(WAIT_OBJECT_0 != dwResult)
    {
        return -3;
    }

    // �յ�DSP������Ϣ���ش�����
    int errno = m_errno[type];
    if(0 != errno)
    {
        m_errno[type] = 0;
        return errno;
    }

    return 0;
    
}
