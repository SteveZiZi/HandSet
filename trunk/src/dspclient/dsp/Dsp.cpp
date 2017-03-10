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
///    ���Ͳ���ֵ����
/// ˵����
///    ����Dsp��Arm��ͨ��Э�飨���DspArmProtocal.h������������ݸ�DSP
/// @param[in]
///    smvData           ԭʼ����������
/// @return
///    bool                  true���ɹ�
///                          false��DSP��ʼ��ʧ�ܻ��д����   
///
bool CDsp::SendSmv(const SmvData &smvData)
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return false;
    }

    // ���͸�DSP
    int nRet = m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_SMV_DATA, &smvData, 
            sizeof(SmvData));

    // ����ʧ�ܷ��ش�����
    if(nRet < 0)
    {
        return false;
    }
    return true;
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
///    ���ɲ�������
/// @param[in] 
///    smvGenData           ����DSP����Ҫ���ɵĲ��β���
/// @return
///    bool                  true���ɹ�
///                          false��DSP��ʼ��ʧ�ܻ��д����
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
///    ���ͱ���ʱ������
/// @param[in] 
///    msgTimeData          ����ʱ������
/// @return
///    bool                  true���ɹ�
///                          false��DSP��ʼ��ʧ�ܻ��д����
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
///    ��ȡDSP��Ϣ����汾��
/// @param[out] 
///    dspInfo      DSP��Ϣ
/// @return
///    bool                  true���ɹ�
///                          false��DSP��ʼ��ʧ�ܻ��д����
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
///    ���DSP�Ĳ��������ݻ��� 
/// @param[in]
///    nType                 0����ղ���ֵ���棬1����ձ���ʱ�仺��
/// @return
///    bool                  true���ɹ�
///                          false��DSP��ʼ��ʧ�ܻ��д����
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

// Ĭ�Ϲ��캯��
CDsp::CDsp()
{
    m_pDspCtrl = CDspController::GetInstance();

    // �����ں��¼��������Կ��Ʒ��ͺͽ���
    m_iEvents[T_SMV_GEN] = CreateEvent(NULL, false, true, NULL);
    m_oEvents[T_SMV_GEN] = CreateEvent(NULL, false, false, NULL);
    SetEvent(m_iEvents[T_SMV_GEN]);
    ResetEvent(m_oEvents[T_SMV_GEN]);

    m_nErrno[T_SMV_GEN] = 0;

    // �������߳�
    StartRead();

    GetDspInfo();
}

// ��Ҫ�ⲿ���ݼ������ָ��
CDsp::CDsp( CDspDataSet *pDspDataSet )
{
    m_pDspCtrl = CDspController::GetInstance();
    m_pDspDataSet = pDspDataSet;
    // �������߳�
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
///    ��ȡ�����̺߳���
/// @param[] 
///    pArg
/// @return
///    DWORD                    0���ɹ�
///
DWORD CDsp::ReadDataThread( LPVOID pArg )
{    
    UINT8 nCommandID;
    void *pData = NULL;
    UINT32 nDataLength = 0;
    while(m_fRead)
    {
        // ��������֡
        int nRet = m_pDspCtrl->FrameRecv(nCommandID, &pData, nDataLength);
        if(nRet < 0)
        {
            continue;
        }

        // ���ָʾ�������˳�
        if(nCommandID == DSP2ARM_CONTROLCODE_STOP_READ)
        {
            ExitThread(0);
            return 0;
        }

        // ������õ�CDspDataSet���ݼ����洢������
        if(NULL != m_pDspDataSet && nCommandID != DSP2ARM_CONTROLCODE_SMV_GEN_RT)
        {
            m_pDspDataSet->Write(nCommandID, pData, nDataLength);
        }

#if 1
        // �������ڲ������ɼ���
        if(NULL != m_pSmvGenCalcData && nCommandID == DSP2ARM_CONTROLCODE_SMV_GEN_RT)
        {
            // ��������ֱ�ӷ��أ��û�������յ��ȴ���ʱ�Ĵ���
            if(nDataLength > sizeof(SMV_GEN_DATA_CALC_RT))
            {
                continue;
            } 
            // ��һ����ʱָ�뱣��        
            void *pTmp = m_pSmvGenCalcData;

            // ������¼�
            SetEvent(m_iEvents[T_SMV_GEN]);

            // ��������������
            memcpy(pTmp, pData, nDataLength);

            // ��������¼�
            SetEvent(m_oEvents[T_SMV_GEN]);        
        }
#endif
    }
    return 0;
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
        CeSetThreadPriority(m_hThreadReadData,DSP_RECV_PROC_PRIO);
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
        // ����ָ����DSP��Ӧһ���������̵߳�ָ��
        m_pDspCtrl->FrameSend(ARM2DSP_CONTROLCODE_STOP_READ);

        // �������߳�
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
///    �������ɼ���
/// @param[in] 
///    dwTimeout                �ȴ���ʱʱ��
///    smvGenData               ����Ĳ������ɲ���
/// @param[out]
///    pSmvGenCalcRT            ����Ĳ������ɼ�������ָ��
/// @return
///    int                      0���ɹ�
///                            -1��DSP��ʼ��ʧ��
///                            -2���ȴ�ʧ��
///                            -3���ȴ���ʱ
///                            -4����������ʧ��
///
int CDsp::SmvGenCalc( DWORD dwTimeout, const SmvGenData &smvGenData, 
                     SMV_GEN_DATA_CALC_RT *pSmvGenCalcRT )
{
    if(!m_pDspCtrl->IsInitSuccess())
    {
        return -1;
    }
    
    // �ȴ�д�¼�����
    DWORD dwResult = CheckSend(T_SMV_GEN, dwTimeout);
    if(WAIT_FAILED == dwResult)
    {
        return -2;
    }

    if(WAIT_TIMEOUT == dwResult)
    {
        return -3;
    }

    // �洢��������ַ
    m_pSmvGenCalcData = pSmvGenCalcRT;

    // ��DSP��������
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
///    ��ⷢ������
/// @param[in] 
///    type                         �����������͵�����
///    dwTimeout                    ��ʱʱ�䣨���룩
/// @return
///    DWORD                        WAIT_OBJECT_0���ɹ�
///                                 WAIT_FAILED��ʧ��
///                                 WAIT_TIMEOUT����ʱ
///
DWORD CDsp::CheckSend( Type type, DWORD dwTimeout)
{
    DWORD dwResult = WaitForSingleObject(m_iEvents[type], dwTimeout);
    return dwResult;
}
///
/// @brief
///    �ȴ������ض����͵�����
/// @param[in] 
///    type                     Ҫ�ȴ��������͵�����
///    dwMilliseconds           ��ʱ������
/// @return
///    int                       0���ɹ�
///                             -1��DSP��ʼ��ʧ��
///                             -2����ȡ�߳�δ����
///                             -3���ȴ�ʧ��
///                             -4���ȴ���ʱ
///                           ��ֵ��Dsp�������ʱ�Ĵ�����
///
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
    DWORD dwResult = WaitForSingleObject(m_oEvents[type], dwMilliseconds);
    if(WAIT_FAILED == dwResult)
    {
        return -3;
    }

    if(WAIT_TIMEOUT == dwResult)
    {
        return -4;
    }

    // �յ�DSP������Ϣ���ش�����
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