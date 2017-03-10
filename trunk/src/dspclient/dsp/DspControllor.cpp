///
/// @file
///     DspControllor.cpp
/// @brief
///     DSP�˿����������ڿ���DSP�˵ĳ�ʼ�������������ݵķ��ͺͽ���
/// @note
///     ��д���̲����˼��������Ա�֤���ݵĿɿ���
///
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/03/19
/// @revise
/// 
#include "stdafx.h"
#include "DSPControllor.h"
#include "../dsplib/loop.h"

#include "../../../inc/DspArmProtocol.h"
#include "DspTransport.h"
Lock CDspController::m_singleLock;
CDspController::CDspController()
:m_fDspRunning(false)
{
    
}
CDspController::~CDspController()
{
    
}
///
/// @brief
///    ����DspController�ĵ���
/// @return
///    CDspController *     ����ָ��
///
CDspController *CDspController::GetInstance()
{
    static CDspController* dspController = NULL;
    
    if(NULL == dspController)
    {
        m_singleLock.Acquire();
        if(NULL == dspController)
        {
            dspController = new CDspController();
        }
        m_singleLock.Release();
    }
    return dspController;
}
///
/// @brief
///     ��ʼ��DSP��
/// @return
///     true����ʼ���ɹ���false����ʼ��ʧ��
///
bool CDspController::DspInit()
{
	//�����û��㷨���������û���ִ�г���(.out�ļ���DSP��)������GPP��DSPͨ�Ź�
    //����������
    if(!m_fDspRunning)
    {
        m_fDspRunning  = CDspTransport::Initial(DSP_LOOP_PATH, ARM_DSP_BUF_SIZE);
    }
    return m_fDspRunning;
}

///
/// @brief
///    ����DSP����֡�����
/// @param[in] 
///    nCommandID   �����룬���DspArmProtocal.h
/// @return
///    int              0���ɹ���-1��DSPδ��ʼ�����ʼ��ʧ��; -2�� DSP�������ݴ���
///
int CDspController::FrameSend(const UINT8 nCommandID)
{
    AutoLock autoLock(m_wLock);
    FrameHead fh;
    if(!m_fDspRunning)
    {
        return -1;
    }
    fh.nCommandID = nCommandID;
    fh.nDataLength = 0;

    // ��ֹ���͹��쵼��DSP����
    //Sleep(2);

    CDspTransport::Write(0, (UINT8 *)&fh, sizeof(FrameHead));
    if(!CDspTransport::Send())
    {
        return -2;
    }
    else
    {
        return 0;
    }
}
///
/// @brief
///    ����DSP����֡������+�������ݿ飩
/// @param[in] 
///    nCommandID       �����룬���DspArmProtocol.h
///    pData            Ҫ���͵�����ָ��
///    nDataLength      Ҫ���͵����ݳ���
/// @return
///    int              0���ɹ���
///                     -1��DSPδ��ʼ�����ʼ��ʧ��; 
///                     -2�����뻺������ָ��ΪNULL�������ݳ���Ϊ0
///                     -3��DSP�������ݴ���
///
int CDspController::FrameSend(const UINT8 nCommandID, 
                               const void* pData,
                               const UINT32 nDataLength)
{
    AutoLock autoLock(m_wLock);
    FrameHead fh;
    
    if(!m_fDspRunning)
    {
        return -1;
    }
    if(NULL == pData  || 0 == nDataLength)
    {
        return -2;
    }
    fh.nCommandID = nCommandID;
    fh.nDataLength = nDataLength;
    
    // ��ֹ���͹��쵼��DSP����
    //Sleep(2);

    CDspTransport::Write(0, (UINT8 *)&fh, sizeof(FrameHead));
    CDspTransport::Write(sizeof(FrameHead), (UINT8 *)pData, nDataLength);
    if(!CDspTransport::Send())
    {
        return -3;
    }
    else
    {
        return 0;
    }
}

///
/// @brief
///    ����DSP����֡������+������ݿ飩
/// @note
///    �˺���������϶�����ݿ�Ȼ����Ϊ���巢�͸�DSP������Ҫ��������ṹ��S1��S2��
///    ���ȹ���һ��2��Ԫ��void*���飺
///         void *pDatas[2] = {&S1, &S2};
///    Ȼ���ٹ���һ��2��Ԫ�ص�������������ָ�������ṹ��ĳ��ȣ�
///         UINT32 nDataLengths[2] = {sizeof(S1),sizeof(S2)};
///    Ȼ�����FrameSend(nCommandID, pDatas, nDataLengths, 2)����һ�����֡���ݣ�
///    �൱�ڷ�����һ����������ݣ�|FrameHead|S1|S2| 
/// @param[] 
///    nCommandID       �����룬���DspArmProtocal.h
///    pDatas           ���ݿ�ָ�����飬���ڴ洢�������Ҫ��װ������ָ��
///    nDataLengths     ���ݿ鳤�����飬���������ָ��ָ������ݴ�С
///    nDataNum         ���ݿ����
/// @return
///    int              0���ɹ���
///                     -1��DSPδ��ʼ�����ʼ��ʧ��; 
///                     -2�����뻺������ָ��ΪNULL�������ݳ���Ϊ0
///                     -3��DSP�������ݴ���
///
int CDspController::FrameSend(const UINT8 nCommandID, 
                              const void* pDatas[],
                              const UINT32 nDataLengths[],
                              const UINT8 nDataNum)
{
	AutoLock autoLock(m_wLock);
    FrameHead fh;
    
    if(!m_fDspRunning)
    {
        return -1;
    }
	// �������֡
    UINT32 nOffset = 0;
    for(int i=0; i<nDataNum; i++)
    {
        if(pDatas==NULL || pDatas[i]==NULL || nDataLengths[i]==0)
        {
            return -2;
        }
        CDspTransport::Write(sizeof(FrameHead)+nOffset, 
                (UINT8 *)pDatas[i], nDataLengths[i]);
        nOffset += nDataLengths[i];
    }

    // д��֡ͷ
    fh.nCommandID = nCommandID;
    fh.nDataLength = nOffset;
    
    // ��ֹ���͹��쵼��DSP����
    //Sleep(2);

    CDspTransport::Write(0, (UINT8 *)&fh, sizeof(FrameHead));

    //��DSP�˷������ݣ�����DSP�㷨
    if(!CDspTransport::Send())
    {
        return -3;
    }
    return 0;
}

///
/// @brief
///    ����DSP����
/// @param[out] 
///    nCommandID           ��DSP���ص�������
///    pData                ����DSP������ָ��
///    nDataLength          ���ݵĳ���
/// @return
///    int                 0���ɹ���
///                        -1��DSPδ��ʼ�����ʼ��ʧ��
///                        -2����ȡ����ʧ�ܣ�nDataLength��Ϊ0��
///
int CDspController::FrameRecv( UINT8 &nCommandID, void **pData, UINT32 &nDataLength)
{
    AutoLock autoLock(m_rLock);

    if(!m_fDspRunning)
    {
        return -1;
    }
    
    //TRACE(L"TIME1:%d\t",GetTickCount());
    bool fIsOK = CDspTransport::Read();
    //TRACE(L"TIME2:%d\t",GetTickCount());
    if(!fIsOK)
    {
        nDataLength = 0;
        return -2;
    }

    
    UINT8 *pBuf = CDspTransport::GetOutBuffer();

    FrameHead* fh = (FrameHead *)pBuf;
    nCommandID = fh->nCommandID;
    nDataLength = fh->nDataLength;
    *pData = pBuf + sizeof(FrameHead);
#if 0
    //TRACE(L"TIME3:%d\t",GetTickCount());
    if(nCommandID == DSP2ARM_CONTROLCODE_SMV_CALC_RT)
    {
        SMV_DATA_CACL_RT *t = (SMV_DATA_CACL_RT *)*pData;
        TRACE(L"%d\t%d\t%f\t",nDataLength, sizeof(SMV_DATA_CACL_RT), 
            t->fApparentPower[0]);
        TRACE(L"%f\n", t->fApparentPower[0]);
    }
#endif
    return 0;
}

///
/// @brief
///     ����DSP��
///
void CDspController::DspDelete()
{
    if(m_fDspRunning)
    {
	    //��DSP�˷�������,����DSP_TSK,DSP�˶�����ʱʹ��
	    FrameSend(ARM2DSP_CONTROLCODE_QUIT);	    
    }
    //ж��DSP�˴��룬�ͷ���Դ���ڴ棩
    CDspTransport::Destroy();
    m_fDspRunning  = false;
}

///
/// @brief
///    Dsp��ʼ���Ƿ�ɹ�
/// @return
///    bool             true:�ɹ�
///                     false:ʧ��
///
bool CDspController::IsInitSuccess()
{
    return m_fDspRunning;
}
