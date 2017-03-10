/// @file
///
/// @brief
///     ���ݲ�����ṩ������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao   2013.4.2 
///
/// �޸���ʷ��
///

#pragma once

#include "recv\FrameRecv.h"
#include "send\FrameSend.h"

#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\include\ctrlcode_def.h"
#include "src\service\datalayer\include\datatypes.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"
#include "src\service\datalayer\config\SmvRecvConfigPublic.h"
#include "src\service\datalayer\config\SmvRecvConfig.h"

#include "src\service\datalayer\recv_data\DataEffectiveVal.h"
#include "src\service\datalayer\recv_data\DataOscillogram.h"
#include "src\service\datalayer\recv_data\DataDoubleAD.h"
#include "src\service\datalayer\recv_data\DataPhasor.h"
#include "src\service\datalayer\recv_data\DataPower.h"
#include "src\service\datalayer\recv_data\DataStatusWatch.h"
#include "src\service\datalayer\recv_data\DataXuliang.h"
#include "src\service\datalayer\recv_data\DataTimeSyncIRIGB.h"

#include "src\service\datalayer\scl\SCLManager.h"
#include "src\service\datalayer\config\ParamConfigBasic.h"
#include "src\service\datalayer\config\ParamConfigSMVSend.h"
#include "src\service\datalayer\config\ParamConfigGOOSESend.h"
#include "src\service\datalayer\config\ParamConfigGOOSERecv.h"
#include "src\service\datalayer\config\ParamconfigManualTest.h"
#include "src\service\datalayer\config\ParamConfigStateSequence.h"
#include "src\service\datalayer\OpticalPower\OpticalPowerManager.h"

#include "src\service\datalayer\pcapanalysis\pcapanalsis.h"

///
/// @brief
///    SMV��GOOSE�����̲߳���
///
struct SendThreadParam
{
    CFrameSend*     m_pFrameSend;                                       ///< ���ݷ��͹���������ָ��
    HWND            m_hWnd;                                             ///< ���ھ������������Ҫ�¼�ʱ�ᷢ����Ϣ���˴���
};

class CDsmDataLayer
{
private:
    CDsmDataLayer(void);
    ~CDsmDataLayer(void);

public:
    ///
    /// @brief
    ///    ��ȡ���ݲ�Ӧ��ʵ��
    ///
    static CDsmDataLayer * getInstance();

    ///
    /// @brief
    ///    ��ʼ�����ݲ���Ҫ������
    ///
    bool Init();

    ///
    /// @brief
    ///    ����ʼ�����������ݲ������
    ///
    bool UnInit();

    ///
    /// @brief
    ///    ����FPGA
    ///
    bool StartFPGA();

    ///
    /// @brief
    ///    �ر�FPGA
    ///
    bool StopFPGA();

    ///
    /// @brief
    ///    ����͹���ģʽ
    ///
    bool EnterLowPowerMode();

    ///
    /// @brief
    ///    �˳��͹���ģʽ
    ///
    bool LeaveLowPowerMode();

//
//=============================================================================
// ̽����ؽӿ�
//=============================================================================
//
    ///
    /// @brief
    ///    �������ݲ㣬Ҫ��ȡ̽����
    ///    ���ݲ�Ҫȥ���ײ�Ĺ��ˣ���ȡ����ӿڵ����б���
    ///
    void BeginDetect(NetFt3RecvFrameType enNetRecvFrameType, bool bEnabelDetect = true);

    ///
    /// @brief
    ///    ��ȡ̽������Ŀ
    ///
    /// @return
    /// ����̽��������Ŀ
    ///
    uint32_t GetNetDetectItemsCount();

    ///
    /// @brief
    ///    ��ȡһ��̽����
    ///
    /// @param[in]  nIndex  Ҫ��ȡ̽��������������0��ʼ
    ///
    /// @return
    ///     CNetDetectItem* ����ָ������̽���������ָ��
    ///
    CNetDetectItem* GetNetDetectItem(uint32_t nIndex);

    ///
    /// @brief
    ///    ��ȡ̽������Ŀ
    ///
    /// @return
    /// ����̽��������Ŀ
    ///
    uint32_t GetFt3DetectItemsCount();

    ///
    /// @brief
    ///    ��ȡһ��̽����
    ///
    /// @param[in]  nIndex  Ҫ��ȡ̽��������������0��ʼ
    ///
    /// @return
    ///     CFt3DetectItem* ����ָ������̽���������ָ��
    ///
    CFt3DetectItem* GetFt3DetectItem(uint32_t nIndex);

    ///
    /// @brief
    ///    ��ȡ1588̽������Ŀ
    ///
    /// @return
    /// ����̽��������Ŀ
    ///
    uint32_t GetNet1588DetectItemsCount();

    ///
    /// @brief
    ///    ��ȡһ��1588̽����
    ///
    /// @param[in]  nIndex  Ҫ��ȡ̽��������������0��ʼ
    ///
    /// @return
    ///     CNetDetectItem* ����ָ������̽���������ָ��
    ///
    CNet1588DetectItem* GetNet1588DetectItem(uint32_t nIndex);

    ///
    /// @brief
    ///    ��ȡ���ƿ��Ӧ��һ������֡��Ϣ
    ///
    /// @param[in]      pDetectItem     Ҫ��ȡ����֡�Ŀ��ƿ�ָ��
    ///
    /// @return
    ///     CDetectItemFrame*   ���ؿ��ƿ��Ӧ��һ������֡��NULL��ʾ��ȡʧ��
    ///
    CDetectItemFrame* GetDetectItemFrame(CBaseDetectItem* pDetectItem);

    ///
    /// @brief
    ///    ��ȡ��ǰ�Ľ�����������
    ///
    /// @return
    ///     NetFt3RecvFrameType ���ص�ǰ�Ľ�����������
    ///
    NetFt3RecvFrameType GetCurrentRecvType();

    ///
    /// @brief
    ///    ��ʼ�����ض���������
    ///
    /// @param[enRecvType]  ���ý����������ͣ�goose��smv��ft3
    /// @param[enMode]      ��������Ĭ�Ͻ����ģʽ
    ///
    /// @note
    ///     ���ý��պ󣬶�����˿ڻ����Detect�ӿڽ���˵���������ݣ�û�й��ˣ�����FT3��ʼ���ݽ���
    ///
    void StartRecvData(NetFt3RecvFrameType enRecvType, DSM_OPERATE_MODE enMode = DSM_MODE_UNKOWN);

    ///
    /// @brief
    ///    ֹͣ���ݽ���
    ///
    void StopRecvData();

//
//=============================================================================
// ��������ȡ�ӿ�
//=============================================================================
//
    ///
    /// @brief
    ///    SCL�ļ���������ȡ�ӿ�
    ///
    /// @return
    ///     CSCLManager* ����Scl��������ָ��
    ///
    CSCLManager* GetSclManager();

//
//=============================================================================
// ���ù�����Ϣ
//=============================================================================
//
    ///
    /// @brief
    ///    ���һ��ѡ�еĿ��ƿ�
    ///
    /// @param[in] pDetectItem  Ҫ���õĵ�ǰ���ƿ�
    ///
    /// @return
    ///     bool    ��ӳɹ�����true
    ///
    bool AddOneSelectedCtrlBlk(CBaseDetectItem* pDetectItem);

    ///
    /// @brief
    ///    �Ƴ�һ���Ѿ���ӵĿ��ƿ�
    ///
    /// @param[in] pDetectItem  Ҫ�Ƴ��Ŀ��ƿ�
    ///
    void RemoveOneSelectedCtrlBlk(CBaseDetectItem* pDetectItem);

    ///
    /// @brief
    ///    ��ȡ�Ѿ�ѡ�еĿ��ƿ�����
    ///
    /// size_t      �����Ѿ�ѡ�еĿ��ƿ�����
    ///
    size_t GetSelectedCtrlBlkCount();

    ///
    /// @brief
    ///    ���õ�ǰ�Ŀ��ƿ飬dsp����˿��ƿ����Ϣ
    ///
    /// @param[in] pDetectItem  Ҫ���õĵ�ǰ���ƿ�
    ///
    void SetCurrentCtrlBlk(CBaseDetectItem* pDetectItem );

    ///
    /// @brief
    ///    ��ȡ��ǰ�Ŀ��ƿ�ָ��
    ///
    /// @return
    ///     CBaseDetectItem*   ���ص�ǰ���ƿ�ָ�룬ʧ�ܷ���Null
    ///
    CBaseDetectItem* GetCurrentCtrlBlk();

    ///
    /// @brief
    ///    ��ȡ��ǰ�Ŀ��ƿ�ʵʱͨ����
    ///
    /// @return
    ///     uint32_t   ���ص�ǰ���ƿ�ʵʱͨ����
    ///
    uint32_t GetCurrentCtrlBlkRealTimeChannelCounts();

    ///
    /// @brief
    ///    ��ȡ��һ�����ƿ���Ϣ
    ///
    /// @return
    ///     CBaseDetectItem* ��ȡ��һ�����ƿ�ָ�룬û���򷵻�NULL
    ///
    CBaseDetectItem* GetFirstSelectCtrlBlk();

    ///
    /// @brief
    ///    ��ȡ��һ�����ƿ���Ϣ
    ///
    /// @param[in]  pCurrentCtrlBlk ָ��ǰ�Ŀ��ƿ�
    ///
    /// @return
    ///     CBaseDetectItem* ��ȡ��һ�����ƿ�ָ�룬û����һ���򷵻�NULL
    ///
    CBaseDetectItem* GetPrevSelectCtrlBlk(CBaseDetectItem* pCurrentCtrlBlk);

    ///
    /// @brief
    ///    ��ȡ��һ�����ƿ���Ϣ
    ///
    /// @param[in]  pCurrentCtrlBlk ָ��ǰ�Ŀ��ƿ�
    ///
    /// @return
    ///     CBaseDetectItem* ��ȡ��һ�����ƿ�ָ�룬û����һ���򷵻�NULL
    ///
    CBaseDetectItem* GetNextSelectCtrlBlk(CBaseDetectItem* pCurrentCtrlBlk);

    ///
    /// @brief
    ///    ����id��ȡ�Ѿ�ѡ�еĿ��ƿ�
    ///
    /// @param[in]  nID     ������ɨ���б�ʱʹ�õ�������ȡ���ƿ�ָ��
    ///
    /// @return
    ///     CBaseDetectItem* ��ȡ����id��Ӧ�Ŀ��ƿ�ָ�룬�������򷵻�NULL
    ///
    CBaseDetectItem* GetSelectCtrlBlkByID(uint32_t nID);

    ///
    /// @brief
    ///    �������ݲ㣬��ѡ��Ŀ��ƿ��Ѿ�������
    ///
    void AddSelectedCtrlBlkFinished();

//
//=============================================================================
// ���ú�����ƿ�
//=============================================================================
//
    ///
    /// @brief
    ///    ���ú���ʹ�õĵ�1�����ƿ�
    ///
    /// @param[in] pDetectItem  Ҫ���õĿ��ƿ�
    ///
    void SetPhaseCheckSmv1CtrBlk(CBaseDetectItem* pDetectItem);

    ///
    /// @brief
    ///    ���ú���ʹ�õĵڶ������ƿ�
    ///
    /// @param[in] pDetectItem  Ҫ���õĵ�ǰ���ƿ�
    ///
    void SetPhaseCheckSmv2CtrBlk(CBaseDetectItem* pDetectItem);

    ///
    /// @brief
    ///    ��ȡ����ʹ�õĵ�1�����ƿ�
    ///
    /// @return
    ///     CBaseDetectItem*   ���ص�ǰ���ƿ�ָ�룬ʧ�ܷ���Null
    ///
    CBaseDetectItem* GetPhaseCheckSmv1CtrlBlk();

    ///
    /// @brief
    ///    ��ȡ����ʹ�õĵڶ������ƿ�
    ///
    /// @return
    ///     CBaseDetectItem*   ���ص�ǰ���ƿ�ָ�룬ʧ�ܷ���Null
    ///
    CBaseDetectItem* GetPhaseCheckSmv2CtrlBlk();

    ///
    /// @brief
    ///    ��ȡSmv1����ͨ��
    ///
    /// @param[out]  nPhaseA ����A��ͨ����
    /// @param[out]  nPhaseB ����B��ͨ����
    /// @param[out]  nPhaseC ����C��ͨ����
    ///
    /// return
    ///     bool    ��ȡ�ɹ�����true
    ///
    bool GetSmv1ABCChannles(uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC);

    ///
    /// @brief
    ///    ����Smv1�����ѹ/����ͨ��
    ///
    /// @param[in]  nPhaseA A��ͨ����
    /// @param[in]  nPhaseB B��ͨ����
    /// @param[in]  nPhaseC C��ͨ����
    ///
    /// return
    ///     bool    ���óɹ�����true
    ///
    bool SetSmv1ABCChannles(uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC);

    ///
    /// @brief
    ///    ��ȡSmv2����ͨ��
    ///
    /// @param[out]  nPhaseA ����A��ͨ����
    /// @param[out]  nPhaseB ����B��ͨ����
    /// @param[out]  nPhaseC ����C��ͨ����
    ///
    /// return
    ///     bool    ��ȡ�ɹ�����true
    ///
    bool GetSmv2ABCChannles(uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC);

    ///
    /// @brief
    ///    ����Smv2�����ѹ/����ͨ��
    ///
    /// @param[in]  nPhaseA A��ͨ����
    /// @param[in]  nPhaseB B��ͨ����
    /// @param[in]  nPhaseC C��ͨ����
    ///
    /// return
    ///     bool    ���óɹ�����true
    ///
    bool SetSmv2ABCChannles(uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC);

    ///
    /// @brief
    ///    ��ȡ�����������ƿ�ʵʱͨ����
    ///
    /// @param[out]  nChannelCountSmv1 ������ƿ�1��ͨ����Ŀ
    /// @param[out]  nChannelCountSmv2 ������ƿ�2��ͨ����Ŀ
    ///
    /// @return
    ///     bool   ��ȡ�ɹ�����true
    ///
    bool GetPhaseCheckRealTimeChannelCounts(uint32_t& nChannelCountSmv1, uint32_t& nChannelCountSmv2);

//
//=============================================================================
// ����ģʽ����
//=============================================================================
//
    ///
    /// @brief
    ///    ���õ�ǰģʽ
    ///
    /// @param[in]  enDsmMode Ҫ���õ�ģʽ
    ///
    void SetCurrentMode(DSM_OPERATE_MODE enDsmMode);

    ///
    /// @brief
    ///    ��ȡ��ǰģʽ
    ///
    /// @return
    ///     DSM_OPERATE_MODE    ���ص�ǰϵͳģʽ
    ///
    DSM_OPERATE_MODE GetCurrentMode();

//
//=============================================================================
// dsp���ýӿ�
//=============================================================================
//
    ///
    /// @brief
    ///    ���ý�������dsp����ʹ�õĲ�����
    ///
    bool SetDspSampleRate(uint32_t nSampleRate);

//
//=============================================================================
// ��ȡҵ�����ݽӿ�
//=============================================================================
//
    ///
    /// @brief
    ///    ��ȡ��Чֵ����
    ///
    /// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    /// @note
    ///     pData ָ����ڴ������ɵ������Լ�����
    ///
    bool GetEffectiveVal(CDataEffectiveVal* pData);

    ///
    /// @brief
    ///    ��ȡ����ͼ����
    ///
    /// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    /// @note
    ///     pData ָ����ڴ������ɵ������Լ�����
    ///
    bool GetOscillogramData(CDataOscillogram* pData);

    ///
    /// @brief
    ///    ��ȡ��������
    ///
    /// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    /// @note
    ///     pData ָ����ڴ������ɵ������Լ�����
    ///
    bool GetPowerData(CDataPower* pData);

    ///
    /// @brief
    ///    ��ȡ˫AD����
    ///
    /// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    /// @note
    ///     pData ָ����ڴ������ɵ������Լ�����
    ///
    bool GetDoubleAdData(CDataDoubleAD* pData);

    ///
    /// @brief
    ///    ��ȡ״̬��������
    ///
    /// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    /// @note
    ///     pData ָ����ڴ������ɵ������Լ�����
    ///
    bool GetStatusWatchData(CDataStatusWatch* pData);

    ///
    /// @brief
    ///    ��ȡ��������
    ///
    /// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    /// @note
    ///     pData ָ����ڴ������ɵ������Լ�����
    ///
    bool GetXuliangData(CDataXuliang* pData);

    ///
    /// @brief
    ///    ��ȡ��������
    ///
    /// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    /// @note
    ///     pData ָ����ڴ������ɵ������Լ�����
    ///
    bool GetPhasorData(CDataPhasor* pData);

    ///
    /// @brief
    ///    ��ȡг����Ϣ����
    ///
    /// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    /// @note
    ///     pData ָ����ڴ������ɵ������Լ�����
    ///
    bool GetHarmonicData(CDataHarmonic* pData);

    ///
    /// @brief
    ///    ��ȡ����ͳ��/��ɢ����Ϣ
    ///
    /// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    /// @note
    ///     pData ָ����ڴ������ɵ������Լ�����
    ///
    bool GetFrameStatistics(CDataFrameStatistics* pData);

    ///
    /// @brief
    ///    ���ͳ�����ݣ�����ͳ��
    ///
    /// @return
    ///     bool    ���óɹ�����true
    ///
    bool ResetSmvFrameStatistics();

    ///
    /// @brief
    ///    ��ȡ������Ϣ
    ///
    /// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    /// @note
    ///     pData ָ����ڴ������ɵ������Լ�����
    ///
    bool GetPolarityData(CDataPolarity* pData);

    ///
    /// @brief
    ///    �������е�ͨ������
    ///
    void ResetAllPorlarity();

    ///
    /// @brief
    ///    ��ȡ��ǰ���ƿ��Ӧ��ͨ��״̬��Ϣ
    ///
    /// @return
    ///     CSmvCtrlBlkQuality* ���ص�ǰ���ƿ�ͨ����Ϣ��һ������ָ��
    ///
    CSmvCtrlBlkQuality* GetCurrentCtrlBlkChannelStatus();

    ///
    /// @brief
    ///    ��ȡ����һ֡��������
    ///
    /// @param[out] pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool    ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    bool GetRealTimeMsg(CDataRealTimeMsgData* pData);

    ///
    /// @brief
    ///    ��ȡ����������Ϣ
    ///
    /// @param[out] pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool    ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    bool GetPhaseCheckData(CDataPhaseCheck* pData);

    ///
    /// @brief
    ///    ��ʼ/ֹͣ������Ϣ���
    ///
    /// @param[in]  bStart  �Ƿ�ʼ���Լ�⣬true��ʼ��⣬falseֹͣ���
    ///
    /// @note
    ///     ֻ���ڼ��Լ��ģʽ����Ч
    void StartPolarityCheck(bool bStart = true);

//
//=============================================================================
// ��ȡGoose���ն�Ӧ����
//=============================================================================
//
    ///
    /// @brief
    ///    ��ȡGOOSEʵʱֵ
    ///
    /// @param[out] pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool    ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    bool GetGooseRealTimeValue(CGooseInfo* pData);

    ///
    /// @brief
    ///    ��ȡGOOSEʵʱ����
    ///
    /// @param[out] pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool    ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    bool GetGooseRealTimeMsg(CDataRealTimeMsgData* pData);

    ///
    /// @brief
    ///    ��ȡGOOSE����ͳ����Ϣ
    ///
    /// @param[out] pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool    ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    bool GetGooseFrameStatistics(CGooseFrameStatistics* pData);

    ///
    /// @brief
    ///    ���Goose����ͳ�����ݣ�����ͳ��
    ///
    /// @return
    ///     bool    ���óɹ�����true
    ///
    bool ResetGooseFrameStatistics();

    ///
    /// @brief
    ///    ��ȡGOOSE��λ��Ϣ
    ///
    /// @param[out] pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool    ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    bool GetGooseChangeList(CDataGooseChangeList* pData);

    ///
    ///=============================================================================
    /// ��ȡ1588����ʱ��ӿ�
    ///=============================================================================
    ///
    ///
    /// @brief
    ///    ��ȡ1588ʱ����Ϣ
    ///
    IEEE1588_TIMESTAMP GetNet1588FrameTime()
    {
        if (m_pFrameRecv)
        {
            return m_pFrameRecv->GetNet1588FrameTime();
        }
        IEEE1588_TIMESTAMP stTs;
        stTs.t_ns = 0;
        stTs.t_s  = 0;
        return stTs;
    }

//
//=============================================================================
// ¼���ӿڣ�¼Goose��NetSmv��Ft3����
//=============================================================================
//
    ///
    /// @brief
    ///    ����¼���ļ���С,��Ҫ�ڿ�ʼ¼��ǰ���ã�����
    ///
    /// @return
    ///     bool    ���óɹ�����true
    ///
    bool SetRecordWaveSize(uint64_t nSizeInByte);

    ///
    /// @brief
    ///    ��ȡ���õ�¼���ļ���С
    ///
    uint64_t GetRecordWaveSize();

    ///
    /// @brief
    ///    ����¼���ļ�·��
    ///
    void SetRecordWavePath(const std::wstring& strRecordWavePath);

    ///
    /// @brief
    ///    ��ȡ���õ�¼���ļ�·��
    ///
    std::wstring GetRecordWavePath();

    ///
    /// @brief
    ///    ����֪ͨ����ʱʹ�õĴ���CWndָ��
    ///
    bool SetNotifyWnd(CWnd* pWnd);

    ///
    /// @brief
    ///    ��ȡ���õ�֪ͨ����ʹ�õ�Cwnd����
    ///
    CWnd* GetNotifyWnd();

#if !CHUN_HUA
	///
	/// @brief
	///    ����SD����С
	///
	void SetSDCardSize(uint64_t SDCardFreeSize);

#endif

    ///
    /// @brief
    ///    ���¼�����ƿ�,��Ҫ�����������֡���������ֵ��Goose��
    ///
    bool AddRecordItem(CBaseDetectItem* pRecordItem);

    ///
    /// @brief
    ///    ��¼���б���Ŀ���Ƴ�һ�����ƿ�
    ///
    bool RemoveRecordItem(CBaseDetectItem* pRecordItem);

    ///
    /// @brief
    ///    ��ʼ¼�������ֵ���ݣ���pcap��ʽ����
    ///
    bool StartRecordNetSmv();

    ///
    /// @brief
    ///    ��ʼ¼Goose���ģ�����pcap��ʽ����
    ///
    bool StartRecordGoose();

    ///
    /// @brief
    ///    ��ʼ¼FT3����֡
    ///
    bool StartRecordFt3();

    ///
    /// @brief
    ///    ֹͣ¼��
    ///
    /// @param[out]     ��¼ʧ��ʱ��ʱʧ��ԭ��
    ///
    /// @return
    ///     bool ֹͣ�ɹ�
    ///
    bool StopRecordWave(uint32_t& nResult);

    ///
    /// @brief
    ///    ȡ����ǰ¼��
    ///
    /// @return
    ///      ȡ���ɹ�����true
    ///
    bool CancelRecordWave();

//
//=============================================================================
// ʱ��ͬ���ӿ�
//=============================================================================
//
    ///
    /// @brief
    ///    ��ȡB���ʱʱ����Ϣ
    ///
    /// @param[out] pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool    ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    bool GetTimeSyncIRIGB(CDataTimeSyncIRIGB* pData);


//
//=============================================================================
// ���û�ȡ�ӿ�
//=============================================================================
//
    ///
    /// @brief
    ///    ��ȡ����ʱ�����������ָ��
    ///
    CSampleInterpSync* GetSampleInterSync()
    {
        if (m_pFrameRecv)
        {
            return m_pFrameRecv->GetSampleInterSync();
        }
        return NULL;
    }

    ///
    /// @brief
    ///    ��ȡSmv���չ������ýӿ�
    ///
    CSmvRecvConfigPublic* GetSmvConfigPublic();

    ///
    /// @brief
    ///    ��ȡ��������/�����������ýӿ�
    ///
    CParamConfigBasic* GetParamConfigBasic();

    ///
    /// @brief
    ///    ���ݲ�ͬ�ĳ���Э��Ĭ�����ã����ض�Ӧ��ͨ������
    ///
    /// @param[in]  enConfigType    Ҫ���ص�����ģ������
    /// @param[out] pSmvRecvConfig  Ҫ����������Ϣ
    ///
    /// @return
    ///     bool    ���سɹ�����true
    ///
    bool SetSmvRecvChannelTemplate(enumSmvRecvConfigTemplateType enConfigType, CSmvRecvConfig* pSmvRecvConfig);

    ///
    /// @brief
    ///    ���»���Ŀ��ƿ�������Ϣһ��/����ֵ
    ///
    /// @param[in]  enChannelType Ҫ�޸ĵ�ͨ�����ͣ���ѹͨ��/����ͨ��
    /// @param[in]  nPriSectype   Ҫ�޸ĵ�ֵ���ͣ���һ��ֵ��1�����Ƕ���ֵ��2��
    /// @param[in]  nVal          ���µ���ֵ
    ///
    void UpdateCachedRatedValues(enumChannelType enChannelType, uint32_t nPriSecType, float fVal);


    ///
    /// @brief
    ///    ��ջ����appid����������Ϣ,�����¼���scl�ļ������
    ///
    void ClearAppidSmvRecvCache()
    {
        if (m_pFrameRecv)
        {
            m_pFrameRecv->ClearAppidSmvRecvCache();
        }
    }

//
//=============================================================================
// ���ݷ��ͽӿ�
//=============================================================================
//
    ///
    /// @brief
    ///    ��ʼ�����ֶ�����SMV���ƿ�
    ///
    /// @param[in]  hWnd  ���ھ������DIֵ�����仯ʱ���ᷢ��WM_DI_CHANGED��Ϣ���˴���
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool StartManualTestSMV(HWND hWnd);

    ///
    /// @brief
    ///    �����ֶ�����SMV���͵�����
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool UpdateManualTestSMV();

    ///
    /// @brief
    ///    ֹͣ�ֶ�����SMV����
    ///
    bool StopManualTestSMV();

    ///
    /// @brief
    ///    ��ʼ�����ֶ�����Goose���ƿ�
    ///
    /// @param[in]  hWnd  ���ھ������DIֵ�����仯ʱ���ᷢ��WM_DI_CHANGED��Ϣ���˴���
    ///
    /// @return
    ///     bool    ���������̳߳ɹ�����true
    ///
    bool StartManualTestGoose( HWND hWnd );

    ///
    /// @brief
    ///    �����ֶ�����Goose���͵�����
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool UpdateManualTestGoose();

    ///
    /// @brief
    ///    ֹͣ�ֶ�����Goose����
    ///
    bool StopManualTestGoose();

    ///
    /// @brief
    ///    ����ֶ�������������
    ///
    /// @return
    ///     unsigned int    �ֶ�������������
    ///
    unsigned int GetManualTestResultCount();

    ///
    /// @brief
    ///    ���ָ�������ŵ��ֶ�������������
    ///
    /// @param[in]  nIndex    �����������(��0��ʼ����)
    ///
    /// @return
    ///     CStateTestResult*    ���ָ��״̬���Խ���Ķ���ʧ��ʱ����NULL
    ///
    CStateTestResult* GetManualTestResult(unsigned int nIndex);

    ///
    /// @brief
    ///    ���ָ���ֶ��������������������
    ///
    /// @param[in]  pCurResult    �ֶ�������������
    ///
    /// @return
    ///     int    ����������ţ���0��ʼ������ʧ��ʱ����-1
    ///
    int GetManualTestResultIndex(CStateTestResult* pCurResult);

    ///
    /// @brief
    ///    �����һ���ֶ�������������
    ///
    /// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
    ///
    /// @return
    ///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* NextManualTestResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    �����һ���ֶ�������������
    ///
    /// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
    ///
    /// @return
    ///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* PrevManualTestResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    �����һ������SMV����ͨ����Ϣ���ֶ�������������
    ///
    /// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
    ///
    /// @return
    ///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* NextManualTestSMVResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    �����һ������SMV����ͨ����Ϣ���ֶ�������������
    ///
    /// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
    ///
    /// @return
    ///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* PrevManualTestSMVResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    �����һ������GOOSE���ƿ���ֶ�������������
    ///
    /// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
    ///
    /// @return
    ///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* NextManualTestGooseResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    �����һ������GOOSE���ƿ���ֶ�������������
    ///
    /// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
    ///
    /// @return
    ///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* PrevManualTestGooseResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    ��ʼ״̬���в���
    ///
    /// @param[in]  hWnd  ���ھ������DIֵ�����仯ʱ���ᷢ��WM_DI_CHANGED��Ϣ���˴���
    ///                             ����Ҫѯ���û��Ƿ��ֶ��л�ʱ���ᷢ��WM_WHETHER_SWITCH��Ϣ���˴���
    ///                             ��״̬�л��󣬻ᷢ��WM_STATUS_CHANGED��Ϣ���˴���
    ///                             ��������ɺ󣬻ᷢ��WM_TEST_COMPLETED��Ϣ���˴���
    ///
    /// @return
    ///     bool    ���������̳߳ɹ�����true
    ///
    bool StartStateSequenceTest(HWND hWnd);

    ///
    /// @brief
    ///    ֹͣ״̬���в���
    ///
    bool StopStateSequenceTest();

    ///
    /// @brief
    ///    ���״̬���н��������
    ///
    /// @return
    ///     unsigned int    ״̬���н��������
    ///
    unsigned int GetStateSequenceResultCount();

    ///
    /// @brief
    ///    ���ָ��״̬�ŵ�״̬���н��������
    ///
    /// @param[in]  iStateIndex    ״̬��(��0��ʼ����)
    ///
    /// @return
    ///     CStateTestResult*    ���ָ��״̬���Խ���Ķ���ʧ��ʱ����NULL
    ///
    CStateTestResult* GetStateSequenceResult(unsigned int iStateIndex);

///
///=============================================================================
/// pcap�ļ�ͳ��/�����ӿ�
///=============================================================================
///
    ///
    /// @brief
    ///    ����Ҫ������pcap�ļ�����·��
    ///
    void SetPcapFilePath(const std::wstring& strPcapFile)
    {
        if(m_pPcapFileAnalysis)
        {
            m_pPcapFileAnalysis->SetPcapFilePath(strPcapFile);
        }
    }

    ///
    /// @brief
    ///    ����֪ͨ����ʱʹ�õĴ���ָ��
    ///
    bool SetPcapFileNotifyWnd(CWnd* pWnd)
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->SetPcapFileNotifyWnd(pWnd);
        }
        return false;
    }

    ///
    /// @brief
    ///    ��ȡ�������õ�֪ͨ����ָ��
    ///
    CWnd* GetPcapFileNotifyWnd()
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetPcapFileNotifyWnd();
        }
        return NULL;
    }

    ///
    /// @brief
    ///    ��ȡpcap�ļ��п��ƿ飨appid����Ŀ
    ///
    uint32_t GetPcapFileAppidCount()
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetPcapFileAppidCount();
        }
        return 0;
    }

    ///
    /// @brief
    ///    ��ȡһ�����ƿ�Appid
    ///
    /// @return
    ///     ��ȡ��Ӧ������appid��-1��ʾ��ȡʧ��
    ///
    int32_t GetPcapFileOneAppid(uint32_t nIndex)
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetPcapFileOneAppid(nIndex);
        }
        return -1;
    }

    ///
    /// @brief
    ///    ���õ�ǰ�Ŀ��ƿ�
    ///
    void SetPcapFileCurrentAppid(uint16_t nAppid)
    {
        if (m_pPcapFileAnalysis)
        {
            m_pPcapFileAnalysis->SetCurrentAppid(nAppid);
        }
    }

    ///
    /// @brief
    ///    ��ȡ��ǰ�Ŀ��ƿ�appid
    ///
    int32_t GetPcapFileCurrentAppid()
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetCurrentAppid();
        }
        return -1;
    }

    ///
    /// @brief
    ///    ��ȡ��ǰ���ƿ������֡����
    ///
    enumFrameType GetPcapFileAppidFrameType(int32_t nAppid)
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetPcapFileAppidFrameType(nAppid);
        }
        return FRAME_TYPE_UNKOWN;
    }

    ///
    /// @brief
    ///    ��ȡĳ�����ƿ��ͨ����Ŀ
    ///
    uint32_t GetPcapFileAppidChannelCount(int32_t nAppid)
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetPcapFileAppidChannelCount(nAppid);
        }
        return 0;
    }

    ///
    /// @brief
    ///    ��ȡһ��appid����ֵ��Ӧ����ֵ/֡��pcap�е�ӳ����Ϣ
    ///
    const CAppidSampleMapping* GetPcapFileAppidSampleMapping(int32_t nAppid)
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetPcapFileAppidSampleMapping(nAppid);
        }
        return NULL;
    }

    ///
    /// @brief
    ///    ��ȡpcap�ļ�������֡��
    ///
    uint32_t GetPcapFileTotalFrameCount()
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetPcapFileTotalFrameCount();
        }
        return 0;
    }

    ///
    /// @brief
    ///    ��ȡpcap��ĳ��appid��Ӧ��֡��
    ///
    uint32_t GetPcapFileAppidFrameCount(int32_t nAppid)
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetPcapFileAppidFrameCount(nAppid);
        }
        return 0;
    }

    ///
    /// @brief
    ///    ��ȡpcap��ĳ��appid��Ӧ�Ĳ�������
    ///
    uint32_t GetPcapFileAppidSampleCount(int32_t nAppid)
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetPcapFileAppidSampleCount(nAppid);
        }
        return 0;
    }

    ///
    /// @brief
    ///    ��ȡappid�Ĳ�����
    ///
    uint32_t GetPcapFileAppidSampleRate(int32_t nAppid)
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetPcapFileAppidSampleRate(nAppid);
        }
        return 0;
    }

    ///
    /// @brief
    ///    ��ȡAppidһ֡����asdu����
    ///
    uint32_t GetPcapFileAppidNoAsdu(int32_t nAppid)
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetPcapFileAppidNoAsdu(nAppid);
        }
        return 0;
    }

    ///
    /// @brief
    ///    ��ȡpcap�ļ���Ӧ��������֡���쳣��Ϣ
    ///
    FrameStatus GetPcapFileFrameIndexStatus(uint32_t nIndex)
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetPcapFileFrameIndexStatus(nIndex);
        }
        return FrameStatus(0);
    }

    ///
    /// @brief
    ///     �ر�pcap�ļ�
    ///
    void QuitPcapFileAnalysis()
    {
        if (m_pPcapFileAnalysis)
        {
            m_pPcapFileAnalysis->ClosePcapFile();
        }
    }


    ///
    /// @brief
    ///    ��ʼpcap�ļ�ͳ��
    ///
    ///
    /// @return
    ///     bool    ��ʼ�ɹ�����true��ʧ��false
    ///
    bool StartPcapFileStatisticsAnalysis()
    {
        if (m_pPcapFileAnalysis)
        {
            // �������̣߳���ʼ״̬Ϊ��ͣ
            CWinThread* pThread = AfxBeginThread(PcapFileAnalysisProc, m_pPcapFileAnalysis, THREAD_PRIORITY_NORMAL, 0, 0);
            if (NULL == pThread)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::StartPcapFileStatisticsAnalysis(): AfxBeginThread failed \n"));
                return false;
            }
            return true;
        }
        return false;
    }

    ///
    /// @brief
    ///    ֹͣpcap�ļ�ͳ��
    ///
    /// @return
    ///     bool    ֹͣ�ɹ�����true��ʧ��false
    ///
    bool StopPcapFileStatisticsAnalysis()
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->StopPcapFileStatisticsAnalysis();
        }
        return false;
    }

    ///
    /// @brief
    ///    ��ȡpcap�ļ�goose�������
    ///
    /// @param[in]  pData �ⲿ����Ŀռ�ָ�룬���ڱ���pcapͳ�ƽ��
    ///
    /// @return
    ///     bool    ��ȡ�ɹ�����true��ʧ��false
    ///
    bool GetPcapFileGooseStatisticsResult(CGooseFrameStatistics* pData)
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetPcapFileGooseStatisticsResult(pData);
        }
        return false;
    }

    ///
    /// @brief
    ///    ��ȡpcap�ļ�����ֵ���������������ɢ����Ϣ
    ///
    /// @param[in]  pData �ⲿ����Ŀռ�ָ�룬���ڱ���pcapͳ�ƽ��
    ///
    /// @return
    ///     bool    ����ֵ��ȡ�ɹ�����true��ʧ��false
    ///
    bool GetPcapFileSmvStatisticsResult(CDataFrameStatistics* pData)
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetPcapFileSmvStatisticsResult(pData);
        }
        return false;
    }


    ///
    /// @brief
    ///    ������Ҫ�Ĳ��ζ�Ӧ�Ĳ��������ݷ�Χ
    ///
    /// @param[in]  nIndexBegin ��������ʼ��������0��ʼ
    /// @param[in]  nIndexEnd   �������������
    ///
    void SetPcapFileWaveAnalysisIndex(uint32_t nIndexBegin, uint32_t nIndexEnd)
    {
        if(m_pPcapFileAnalysis)
        {
            m_pPcapFileAnalysis->SetPcapFileWaveAnalysisIndex(nIndexBegin, nIndexEnd);
        }
    }

    ///
    /// @brief
    ///    ��ʼ���÷�Χ�ڲ�����Ĳ��δ���
    ///
    /// @return
    ///     bool ����true��ʾ���ÿ�ʼ�������ݳɹ�
    ///
    bool StartPcapFileWaveAnalysis()
    {
        if (m_pPcapFileAnalysis)
        {
            // �������̣߳���ʼ״̬Ϊ��ͣ
            CWinThread* pThread = AfxBeginThread(PcapFileWaveAnalysisProc, m_pPcapFileAnalysis, THREAD_PRIORITY_NORMAL, 0, 0);
            if (NULL == pThread)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::StartPcapFileWaveAnalysis(): AfxBeginThread failed \n"));
                return false;
            }
            return true;
        }
        return false;
    }

    ///
    /// @brief
    ///    ֹͣpcap�ļ�����ͼ���ݷ���
    ///
    /// @return
    ///     bool    ֹͣ�ɹ�����true��ʧ��false
    ///
    bool StopPcapFileWaveAnalysis()
    {
        if(m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->StopPcapFileWaveAnalysis();
        }
        return false;
    }

    ///
    /// @brief
    ///    ��ȡָ��������Χ�ڲ���ͼ/��λ/��Чֵ����
    ///
    /// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
    ///
    /// @return
    ///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
    ///
    /// @note
    ///     pData ָ����ڴ������ɵ������Լ�����
    ///
    bool GetPcapFileWaveOscillogramData(CDataOscillogram* pData)
    {
        if (m_pPcapFileAnalysis)
        {
            return m_pPcapFileAnalysis->GetPcapFileWaveOscillogramData(pData);
        }
        return false;
    }


//
//=============================================================================
// ������Ϣ��ȡ
//=============================================================================
//
    ///
    /// @brief
    ///    ��ȡ�����汾�����ذ汾�ַ��� �� 2.34
    ///
    std::wstring GetGPMDriverVersion();

    ///
    /// @brief
    ///    ��ȡFPGA�汾�����ذ汾�ַ��� �� 2.34
    ///
    std::wstring GetGPMFPGAVersion();

    ///
    /// @brief
    ///    ��ȡdsp�汾��Ϣ
    ///
    uint8_t GetDspVersion();

    ///
    /// @brief
    ///    ��ȡFpgaʱ��ͬ����ʽ
    ///
    FpgaTimeSyncMode GetFpgaTimeSyncMode();

    ///
    /// @brief
    ///    ����Fpgaʱ��ͬ����ʽ
    ///
    /// @param[in]  enMode Ҫ���õĶ�ʱģʽ
    ///
    /// @return
    ///     bool ���óɹ�����true
    ///
    bool SetFpgaTimeSyncMode(FpgaTimeSyncMode enMode);

    ///
    /// @brief
    ///    ��ȡFPGAʱ��
    ///
    /// @param[out] pUtcTime    �����ȡ����utcʱ��
    ///
    /// @return
    ///     bool    ��ȡ�ɹ�����true
    ///
    bool GetFpgaUtcTime(UTC_TIME_t* pUtcTime);

    ///
    /// @brief
    ///    ����FPGAʱ��
    ///
    /// @param[in] pUtcTime    Ҫ���õ�utcʱ��
    ///
    /// @return
    ///     bool    ���óɹ�����true
    ///
    bool SetFpgaUtcTime(UTC_TIME_t* pUtcTime);

    ///
    /// @brief
    ///    �ж�ĳ���˿ڵ�����״̬,�����̫������Ч
    ///
    /// @return
    ///     bool �˿������ӷ���true
    ///
    bool IsPortLinked(DsmPortName enPortName);

    ///
    /// @brief
    ///    �ж�ĳ���˿��Ƿ������շ����ݣ������ж˿���Ч
    ///
    /// @return
    ///     bool �˿������շ����ݷ���true
    ///
    bool IsPortActive(DsmPortName enPortName);

    ///
    /// @brief
    ///    ��ȡ�˿�����״̬��
    ///
    /// @param[out] nState  ��ȡ�ɹ���������˿ڵ�״̬��Ϣ
    ///
    /// @return
    ///     bool    ��ȡ�ɹ�����true
    ///
    bool GetPortState(uint16_t& nState);

    ///
    /// @brief
    ///    ��ʼ����b��(ͨ��FT3���Ͷ˿�)
    ///
    /// @return
    ///     bool    ���ͳɹ�����true
    ///
    bool StartIRIGBTx();

    ///
    /// @brief
    ///    ֹͣ����b��(ͨ��FT3���Ͷ˿�)
    ///
    /// @return
    ///     bool    ���ͳɹ�����true
    ///
    bool StopIRIGBTx();

    ///
    /// @brief
    ///    ����1588��ʱʱ����Ϣ
    ///
    /// @param[in] 
    ///
    /// @return
    ///     bool    ���ͳɹ�����true
    ///
    bool SetIeee1588MainClock(DsmPortName enPortName, const IEEE1588_CLOCK_SYNC_INFO& stIEEE1588ClockInfo);

private:
    ///
    /// @brief
    ///    ��������ֹͣFPGA
    ///
    bool _StartFPGA(bool bStart);

    ///
    /// @brief
    ///    �����˳��͹���ģʽ
    ///
    bool _EnterLowPowerMode(bool bLowPowerMode);

    ///
    /// @brief
    ///    ����FT3���Ͷ˿ڷ�����������(Ft3/B��)
    ///
    /// @return
    ///     bool    ���óɹ�����true
    ///
    bool _SetFt3TxStyle(enumFt3TxStyle enTxStyle);


private:
    CFrameRecv*                         m_pFrameRecv;                   ///< ���ݽ��չ�����
    CFrameSend*                         m_pFrameSend;                   ///< ���ݷ��͹�����

    CSCLManager*                        m_pSclManager;                  ///< SCL�ļ�������
    CParamConfigBasic*                  m_pParamConfigBasic;            ///< ��������|�����������ýӿ�
    CParamConfigSMVSend*                m_pParamConfigSMVSend;          ///< SMV�������ù�����
    CParamConfigGOOSESend*              m_pParamConfigGOOSESend;        ///< GOOSE�������ù�����
    CParamConfigGOOSERecv*              m_pParamConfigGOOSERecv;        ///< GOOSE�������ù�����
    CParamconfigManualTest*             m_pParamConfigManualTest;       ///< �ֶ��������ù�����
    CParamConfigStateSequence*          m_pParamConfigStateSequence;    ///< ״̬�������ù�����
    COpticalPowerManager*               m_pOpticalPowerManager;         ///< �⹦�ʹ�����

    HANDLE                              m_hGPMDevice;                   ///< FPGA�ļ���������ڲ���FPGA���շ�������

    CPcapAnalysis*                      m_pPcapFileAnalysis;            ///< pcap�ļ�ͳ�Ʒ�����
};
