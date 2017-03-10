/// @file
///
/// @brief 
///     ��������֡�Ľ���
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.3.20
///
/// �޸���ʷ��
///

#pragma once

#include <string>
#include <set>
#include <deque>
#include <vector>
#include "src\service\datalayer\include\ctrlcode_def.h"
#include "FrameRecvFt3.h"
#include "FrameRecvNet.h"
#include "Ft3RecvBuffer.h"
#include "NetRecvBuffer.h"
#include "FrameRecvStruct.h"
#include "src\service\datalayer\config\SmvRecvConfigPublic.h"
//SmvData
#include "recv_data\DataEffectiveVal.h"
#include "recv_data\DataOscillogram.h"
#include "recv_data\DataDoubleAD.h"
#include "recv_data\DataPower.h"
#include "recv_data\DataXuliang.h"
#include "recv_data\DataStatusWatch.h"
#include "recv_data\DataPhasor.h"
#include "recv_data\DataHarmonic.h"
#include "recv_data\DataFrameStatistics.h"
#include "recv_data\DataPolarity.h"
#include "recv_data\DataRealTimeMsg.h"
#include "recv_data\DataPhaseCheck.h"
//GooseData
#include "recv_data\DataGooseInfo.h"
#include "recv_data\DataGooseFrameStatistics.h"
#include "recv_data\DataGooseChangeList.h"
//Dsp
#include "src\dspclient\include\DspDataSet.h"
#include "src\dspclient\include\Dsp.h"
//Lock
#include "src\dspclient\dsp\lock.h"
//Pcap
#include "src\utils\PcapFile\include\pcap_file_i.h"
//ʱ�����
#include "src\utils\SampleInterpSync\include\SampleInterpSync.h"
#include "src\utils\SampleInterpSync\interp.h"

#include "src\service\datalayer\recv\OscillogramBuf.h"

#define WM_DATA_LAYER_MESSAGE           (PRIVATE_WND_USERID + 50)                   ///< ���ݲ㷢�͸��������ϢID,��ͬ��Ϣʹ��wparam����
#define MSG_RECORD_SIZE_REACHED         (1)                                         ///< ¼����С�Ѿ��ﵽ�趨����ֵ

//����ͼ���ݻ���ѡ��1Ϊstl˫�˶��У�0Ϊ˫����
#define OSCILLOGRAM_USE_STL_DEQUE  1

//�����ļ�������룬�쳣������CFileException�д���ԭ����һ�£��ļ���ʧ�ܶ���Ϊ0
#define DATA_LAYER_FILE_OPEN_FAILED             0
#define DATA_LAYER_FILE_GEN_EXCEPTION           CFileException::genericException
#define DATA_LAYER_FILE_NOT_FOUND               CFileException::fileNotFound
#define DATA_LAYER_FILE_BAD_PATH                CFileException::badPath
#define DATA_LAYER_FILE_TOO_MANY_OPEN_FILES     CFileException::tooManyOpenFiles
#define DATA_LAYER_FILE_ACCESS_DENIED           CFileException::accessDenied
#define DATA_LAYER_FILE_INVALID_FILE            CFileException::invalidFile
#define DATA_LAYER_FILE_REMOVE_CURRENT_DIR      CFileException::removeCurrentDir
#define DATA_LAYER_FILE_DIR_FULL                CFileException::directoryFull
#define DATA_LAYER_FILE_BAD_SEEK                CFileException::badSeek
#define DATA_LAYER_FILE_HARD_IO                 CFileException::hardIO
#define DATA_LAYER_FILE_SHARE_VIOLATION         CFileException::sharingViolation
#define DATA_LAYER_FILE_LOCK_VIOLATION          CFileException::lockViolation
#define DATA_LAYER_FILE_DISK_FULL               CFileException::diskFull
#define DATA_LAYER_FILE_END_OF_FILE             CFileException::endOfFile
#define DATA_LAYER_UNKOWN_EXCEPTION             15

///
/// @brief
///    ̽��9-2����֡���Զ�����������������Ϣ
///
struct SmpRateCalcInfo
{
    FrameTimeStamp  stFirst;                                               ///< �������д�appid��һ֡ʱ��
    FrameTimeStamp  stLast;                                                ///< �������д�Appid���һ֡ʱ��
    uint32_t        nFrameCnt;                                             ///< �������д�appid����֡�ĸ�����һ��֡�����ж�������㣩
    uint32_t        nAsdu;                                                 ///< һ������֡�в��������
    bool            bBufFin;                                               ///< һ���������Ƿ����

    SmpRateCalcInfo(): nFrameCnt(0)
                     , nAsdu(0)
                     , bBufFin(0)
    {
    }

    void ParseSample(const FrameTimeStamp& stFrameTime, uint32_t nAsduCnt)
    {
        //ֻ��bBufFinΪfalse���Ž�������ͳ��
        //bBufFin��һ������������ʱ���������֡��������2����Ϊtrue
        if (!bBufFin)
        {
            if (nFrameCnt == 0)
            {
                stFirst = stFrameTime;
                stLast  = stFrameTime;
                nAsdu   = nAsduCnt;
            }
            else
            {
                stLast = stFrameTime;
            }
            nFrameCnt++;
        }
    }

    ///
    /// @brief
    ///    ���³�ʼ������
    ///
    void ResetData()
    {
        nFrameCnt = 0;
        nAsdu     = 0;
        bBufFin   = false;
        memset(&stFirst, 0, sizeof(stFirst));
        memset(&stLast,  0, sizeof(stLast));
    }

    ///
    /// @brief
    ///    ���������
    ///
    /// @return
    ///    Ĭ�Ϸ���4k�����̽�⵽�ȽϿ���12.8k���򷵻�12.8k,
    ///    ̽�������û�л�ȡ������򷵻�0
    ///
    uint32_t GetSmpRate()
    {
        if (bBufFin)
        {
            int32_t nSmpRate = 4000;
            int64_t nTimeUsec = stLast - stFirst;
            int64_t nSmpCnt = nFrameCnt * nAsdu;
            if (nTimeUsec > 0 && nSmpCnt > 1)
            {
                int64_t nSmpRateTmp = (nSmpCnt - 1)*1000000/nTimeUsec;
                RETAILMSG(1, (_T("SmpCnt: %I64d/%I64d CalSmpRate:%I64d\r\n"), nSmpCnt, nTimeUsec, nSmpRateTmp));
                if (nSmpRateTmp > 6500)
                {
                    nSmpRate = 12800;
                }
            }
            return nSmpRate;
        }
        return 0;
    }
};

///
/// @brief
///    ����appid����Ӧ�Ĳ�������Ϣӳ�����ͣ�����ʹ��
///
typedef std::map<CNetDetectItem*, SmpRateCalcInfo> MapSmpRateCalcInfo;


class CFrameRecv
{
public:
    CFrameRecv(HANDLE hGPMDevice);
    ~CFrameRecv(void);

public:
    friend DWORD Ft3ThreadProc(PVOID pArg);
    friend DWORD NetThreadProc(PVOID pArg);

    ///
    /// @brief
    ///    ��ʼ����������֡
    ///
    /// @return
    ///     true��ʶ��ʼ���ɹ���falseʧ��
    bool InitFrameRecv();

    ///
    /// @brief
    ///    �������ݽ���֡�Ľ���
    ///
    /// @return
    ///     true��ʶ����ʼ���ɹ���falseʧ��
    void UnInitFrameRecv();

    ///
    /// @brief
    ///    ��ʼĳ���˿�����֡����
    ///
    bool StartPortFrameRecv(DsmPortName enPortName);

    ///
    /// @brief
    ///    ֹͣĳ���˿�����֡����
    ///
    bool StopPortFrameRecv(DsmPortName enPortName);

    ///
    /// @brief
    ///    ���ý����߳����ȼ�
    ///
    void SetRxThreadPriority(int nPriority);


//
//=============================================================================
// ̽�����
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
    ///    ��ȡ��ǰ��̽������
    ///
    /// @return
    ///     NetFt3RecvFrameType ���ص�ǰ��̽����������
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
// ���ù������
//=============================================================================
//    
    ///
    /// @brief
    ///    ���õ�ǰ�Ŀ��ƿ飬dsp����˿��ƿ����Ϣ
    ///
    /// @param[in] pDetectItem  Ҫ���õĵ�ǰ���ƿ�
    ///
    void SetCurrentCtrlBlk( CBaseDetectItem* pDetectItem);

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
    ///    ��ȡ�Ѿ�ѡ�еĵ�һ�����ƿ���Ϣ
    ///
    /// @return
    ///     CBaseDetectItem* ��ȡ��һ�����ƿ�ָ�룬û���򷵻�NULL
    ///
    CBaseDetectItem* GetFirstSelectCtrlBlk();

    ///
    /// @brief
    ///    ��ȡ�Ѿ�ѡ�е���һ�����ƿ���Ϣ
    ///
    /// @param[in]  pCurrentCtrlBlk ָ��ǰ�Ŀ��ƿ�
    ///
    /// @return
    ///     CBaseDetectItem* ��ȡ��һ�����ƿ�ָ�룬û����һ���򷵻�NULL
    ///
    CBaseDetectItem* GetPrevSelectCtrlBlk(CBaseDetectItem* pCurrentCtrlBlk);
    
    ///
    /// @brief
    ///    ��ȡ�Ѿ�ѡ�е���һ�����ƿ���Ϣ
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
// ������Ϣ������ؽӿ�
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
// ����ģʽ�����ݲ�ͬҳ�����ö�Ӧ��ҳ��ģʽ�����ݲ����ģʽ��������
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
// ��ȡSMV����ҵ�����ݽӿ�
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
    ///    ��ȡ��ǰ���ƿ��Ӧ��ͨ��״̬��Ϣ
    ///
    /// @return
    ///     CSmvCtrlBlkQuality* ���ص�ǰ���ƿ�ͨ����Ϣ��һ������ָ��
    ///
    CSmvCtrlBlkQuality* GetCurrentCtrlBlkChannelStatus();

    ///
    /// @brief
    ///    �������е�ͨ������
    ///
    void ResetAllPorlarity();

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
// ��ȡgoose����ҵ��ӿ�
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
        AutoLock autoLock(m_lock1588TimeStamp);
        return m_st1588TimeStamp;
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
    bool SetRecordWaveSize(uint64_t nSizeInByte);

    ///
    /// @brief
    ///    ��ȡ���õ�¼���ļ���С
    ///
    uint64_t GetRecordWaveSize();


#if !CHUN_HUA
	///
	/// @brief
	///    ����SD����С
	///
	void SetSDCardSize(uint64_t SDCardFreeSize);

	///
	/// @brief
	///    ��ȡSD����С
	///
	uint64_t GetSDCardSize();

	///
	/// @brief
	///    ��ʼ¼��
	///
	void StartRecord();///

	/// @brief
	///    ��ȡSD����С
	///
	static	DWORD WINAPI RecordThreadProc(PVOID pArg);

	static void _Write2File( CFrameRecv* pFrameRecv,uint8_t* buffer, DSM_OPERATE_MODE enMode );

#endif
    ///
    /// @brief
    ///    ����¼���ļ�·������Ҫ��¼����ʼǰ����
    ///
    void SetRecordWavePath(const std::wstring& strRecordWavePath);

    ///
    /// @brief
    ///    ��ȡ���õ�¼���ļ�·��
    ///
    std::wstring GetRecordWavePath();

    ///
    /// @brief
    ///    ����֪ͨ����ʱʹ�õĴ���Cwndָ��
    ///
    bool SetNotifyWnd(CWnd* pWnd);

    ///
    /// @brief
    ///    ��ȡ���õ�֪ͨ����ʹ�õ�Cwnd����
    ///
    CWnd* GetNotifyWnd();

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
    ///    ��¼���б���Ŀ���Ƴ����п��ƿ�
    ///
    bool RemoveAllRecordItem();

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
    bool CancelRecordWave();

//
//=============================================================================
// �������ýӿ�
//=============================================================================
//
    ///
    /// @brief
    ///    ��ȡ����ʱ�����������ָ�룬�����������ط��޸�ʱ���������
    ///
    CSampleInterpSync* GetSampleInterSync()
    {
        return &m_cSmpSyncTime;
    }

    ///
    /// @brief
    ///    ��ȡSmv���չ������ýӿ�
    ///
    CSmvRecvConfigPublic* GetSmvConfigPublic();

    ///
    /// @brief
    ///    ���»���Ŀ��ƿ�������Ϣһ��/����ֵ
    ///
    /// @param[in]  enChannelType Ҫ�޸ĵ�ͨ�����ͣ���ѹͨ��/����ͨ��
    /// @param[in]  nPriSectype   Ҫ�޸ĵ�ֵ���ͣ���һ��ֵ��1�����Ƕ���ֵ��2��
    /// @param[in]  nVal          ���µ���ֵ
    ///
    void UpdateCachedRatedValues(enumChannelType enChannelType, uint32_t nPriSecType, float fVal);

//
//=============================================================================
// dsp�������
//=============================================================================
//
    ///
    /// @brief
    ///    dsp���������߳�
    ///
    static DWORD WINAPI DspSendProc(LPVOID lpParameter);

    ///
    /// @brief
    ///    ����dsp�����߳�
    ///
    bool StartDspSendProc();

    ///
    /// @brief
    ///    ֹͣDSP�����߳�
    ///
    bool StopDspSendProc();

    ///
    /// @brief
    ///    ����dsp����ʹ�õĲ�����
    ///
    bool SetDspSampleRate(uint32_t nSampleRate);

    ///
    /// @brief
    ///    ����dsp�м�����ɢ��ʹ�õ�Asdu��Ŀ
    ///
    bool SetDspAsduCount(uint32_t nAsdu = 1);

    ///
    /// @brief
    ///    ��ȡdsp�汾��Ϣ
    ///
    uint8_t GetDspVersion();

//
//=============================================================================
// ����Ԥ����ͨ������
//=============================================================================
//
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
    ///    ��ջ����appid����������Ϣ
    ///
    void ClearAppidSmvRecvCache();

private:
    ///
    /// @brief
    ///    ������յ���Ft3����
    ///
    void _Ft3DataHandler(PVOID pArg);

    ///
    /// @brief
    ///    ������յ�����̫������
    ///
    void _NetDataHandler(PVOID pArg);

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
    bool _Detect(NetFt3RecvFrameType enNetRecvFrameType, uint16_t nCount = NET_DEFAULT_RECV_FRAMES, bool bEnableTimeout = NET_DEFAULT_RECV_ENABLETIMEOUT, uint32_t nTimeoutMilliSecond = NET_DEFAULT_RECV_TIMEOUT);

    ///
    /// @brief
    ///    ��ȡFt3�ӿ�̽����
    ///
    void _GenFt3DetectInfo( CFrameRecvFt3 * pFt3 );

    ///
    /// @brief
    ///    ��ȡ����ӿ�̽����
    ///
    void _GenNetDetectInfo(CFrameRecvNet * pNet);

    ///
    /// @brief
    ///    ��������ӿڻ�ȡ�����ݣ��������ý��д���
    ///
    void _NetSmvAnaly( CFrameRecvNet * pNet , DSM_OPERATE_MODE enMode, CNetDetectItem* pCurrentCtrlBlk);

    ///
    /// @brief
    ///     ����Ft3�ӿڻ�ȡ�����ݣ��������ý��д���
    ///
    void _Ft3SmvAnaly(CFrameRecvFt3* pFt3 , DSM_OPERATE_MODE enMode, CFt3DetectItem* pCurrentCtrlBlk);

    ///
    /// @brief
    ///    ���̽������m_vecDetectItems
    ///
    /// @param[in]  pBaseDetectItem Ҫ��ӵ�̽����ָ��
    ///
    /// @return
    ///     bool ��ӳɹ�����true
    ///
    bool _AddDetectItem(CBaseDetectItem* pBaseDetectItem);

    ///
    /// @brief
    ///    �жϵ�ǰ���ƿ���Ϣ�Ƿ��Ѿ���̽���б���
    ///
    /// @param[in]  pBaseDetectItem Ҫ��ӵ�̽����ָ��
    ///
    /// @return
    ///     bool �Ѿ����ڷ���true
    ///
    bool _IsDetectItemExist(CBaseDetectItem* pBaseDetectItem);

///
///=============================================================================
/// ̽������в����ʼ��㸨������
///=============================================================================
///
    ///
    /// @brief
    ///    һ�λ��������һ֡�������ʱ���ã����ĳЩ����������appid������Ҫ���к�������
    ///
    void _SmpCalcAfterBufferAnalysis(bool bAutoDetect);

    ///
    /// @brief
    ///    ����9-2�����ʼ��㻺������
    ///
    void _Reset92SmpCalcData();

    ///
    /// @brief
    ///    ����ϵͳ9-2����������,���û���ѡ���ƿ�����̽�⵽9-2�Ĳ����ʣ��޸�ϵͳ9-2���ղ�����
    ///    ̽��Ŀ��ƿ��Ѿ�������̽�⵽��9-2�����ʣ����ѡ�еĿ��ƿ�Ĳ���������ͬ�ģ����湴ѡ��֤��
    ///
    void _UpdateSmvRecvPub92SmpRate();

    ///
    /// @brief
    ///    ��ȡ������������֡��Ӧ�Ŀ��ƿ���Ϣ
    ///
    CNetDetectItem* _GetNetFrameDetectItem(uint16_t nAppid, enumFrameType enFrameType, DsmPortName enPortName, const uint8_t macAddr[]);

    ///
    /// @brief
    ///    ���������������
    ///
    void _ClearAllContainer();

    ///
    /// @brief
    ///    ���̽����Ϣ
    ///
    void _ClearDetectItems();

    ///
    /// @brief
    ///    ��m_vecSelectedItems�л�ȡ��Ϣ�����õ������˿ڽ��й���
    ///
    bool _SetNetFilter();

    ///
    /// @brief
    ///    �ж϶˿��Ƿ�������˿�
    ///
    bool _IsNetPort(DsmPortName enPortName);

    ///
    /// @brief
    ///    �ж϶˿��Ƿ��ǹ⴮��
    ///
    bool _IsFt3Port(DsmPortName enPortName);

    ///
    /// @brief
    ///    �ж��Ƿ���FT3��������֡
    ///
    bool _IsFt3Frame(enumFrameType enFrameType);

    ///
    /// @brief
    ///    �ж��Ƿ���������������֡
    ///
    bool _IsNetFrame(enumFrameType enFrameType);

    ///
    /// @brief
    ///    �ж��Ƿ��ǲ���ֵ����֡,9-1/9-2/ft3
    ///
    bool  _IsSmvFrame(enumFrameType enFrameType);

    ///
    /// @brief
    ///    �ж�ĳ���˿������Ƿ��ڵ�ǰ���ƿ���
    ///
    bool _IsPortInCurrentCtrlBlk(DsmPortName enPortName);

    ///
    /// @brief
    ///    �ж�ĳ���˿������Ƿ����Ѿ�ѡ��Ŀ��ƿ���
    ///
    bool _IsPortInSelecedCtrlBlk(DsmPortName enPortName);

    ///
    /// @brief
    ///    ����smv����
    ///
    void _NetSmvBufferAnaly(CNetRecvBuffer& rNetRecvBuffer, DSM_OPERATE_MODE enOpMode, CNetDetectItem* pCurrentCtrlBlk, CFrameRecvNet * pNet);

    ///
    /// @brief
    ///    �����ֶ����顢״̬���в���ʱGOOSE��DIֵ
    ///
    /// @param[in]  pNet    ��̫�����ݽ�����
    ///
    void _NetSaveGooseDI(CFrameRecvNet* pNet);

    ///
    /// @brief
    ///    ����goose����
    ///
    void _NetGooseAnaly(CFrameRecvNet* pNet, DSM_OPERATE_MODE enOpMode, CNetDetectItem* pCurrentDetct);

    ///
    /// @brief
    ///    ����1588����
    ///
    void _Net1588Analy( CFrameRecvNet * pNet, DSM_OPERATE_MODE enOpMode, CNetDetectItem* pCurrentDetct );

    ///
    /// @brief
    ///    ��������Ft3����
    ///
    void _Ft3SmvAnalyGuowang(CFt3RecvBuffer& rRecvBuffer, DSM_OPERATE_MODE enOpMode, CFt3DetectItem* pCurrentCtrlBlk, CFrameRecvFt3* pFt3);

    ///
    /// @brief
    ///    ��������FT3����
    ///
    void _Ft3SmvAnalyNanrui(CFt3RecvBuffer& rRecvBuffer, DSM_OPERATE_MODE enOpMode, CFt3DetectItem* pCurrentCtrlBlk, CFrameRecvFt3* pFt3);

    ///
    /// @brief
    ///    �������ֵ��˫�˶�����
    ///
    /// @note
    ///     Ϊ�˷�ֹ�����������������ƴ�СΪ����ͼ�������ݵ�2��
    ///     ����resizeʱ���������ߴ糬�����õĴ�С�����ɾ���ӽ�����β�˵�����
    ///     Ϊ���û���һֱ�����������ݣ�����ÿ�ν����ݲ������ͷ����ʱ��ȽϾõ�����
    ///     �ᱻɾ����ȡ���ݵ�ʱ����Ҫע�⣬����β����������ʱ����һЩ�����ݡ�
    ///
    void _AddSmv2Deque(SMV_VALUE& stSmvValue);

    ///
    /// @brief
    ///    ����˫AD����
    ///
    void _AddSmv2DoubleAD(SMV_VALUE& stSmvValue);

    ///
    /// @brief
    ///    ����ģʽ���Բ���ֵ���д���
    ///
    void _SmvHandle(DSM_OPERATE_MODE enOpMode, SMV_VALUE& stSmvValue, CBaseDetectItem* pCurrentCtrlBlk);

    ///
    /// @brief
    ///    ���Ϊ����ͼ����Ĳ���ֵ����
    ///
    void _ClearSmvDeque();

    ///
    /// @brief
    ///    ��Ӳ���ֵ��dsp���Ͳ���ֵ���壬����ֵ����һ���ܲ�����ʱ����һ�β���ֵ��dsp
    ///
    void _AddSmv4DspSend(DSM_OPERATE_MODE enOpMode, SMV_VALUE& stSmvValue, CBaseDetectItem* pCurrentCtrlBlk);

    ///
    /// @brief
    ///    ����ͨ�����ͺ�ͨ����𣬲�������dsp��Ҫ�����ʶ���ֽ�
    ///
    uint8_t _GetDspChannelVCType(enumChannelType enChannelType, ChannelPhase enChannelPhase);

    ///
    /// @brief
    ///    ��ȡһ�����ƿ�Ĳ�����
    ///
    uint32_t _GetCtrlBlkSampleRate(CBaseDetectItem* pCtrlBlk);

    ///
    /// @brief
    ///    ��ȡһ�����ƿ�ĳ��ͨ�������������Ϣ
    ///
    uint8_t _GetCtrlBlkChannelVCType(CBaseDetectItem* pCtrlBlk, uint32_t nChannelIndex, DSM_OPERATE_MODE enOpMode );

    ///
    /// @brief
    ///    ��ʼ��dsp������Ҫ���¼�
    ///
    bool _InitDspSendEvents();

    ///
    /// @brief
    ///    ����dsp���͵ȴ��¼�
    ///
    void _UninitDspSendEvents();

    ///
    /// @brief
    ///    ���û������ݶ�Ӧ�Ŀ��ƿ�ָ��
    ///
    void _SetCachedCtrlBlk(CBaseDetectItem* pCtrlBlk);

    ///
    /// @brief
    ///    ��ȡ�������ݶ�Ӧ�Ŀ��ƿ�ָ��
    ///
    CBaseDetectItem* _GetCachedCtrlBlk();

    ///
    /// @brief
    ///    ������л�����Ϣ
    ///
    void _ClearCachedData();

    ///
    /// @brief
    ///    ��շ��Ͳ���ֵʱ�䣨������ɢ�ȣ���������
    ///
    void _ClearSmvTimeDeque();

    ///
    /// @brief
    ///    ���goose��λ�б��¼����
    ///
    void _ClearGooseChangeList();

    ///
    /// @brief
    ///    smvʱ�������
    ///
    void _SmvTimeHandle( DSM_OPERATE_MODE enOpMode, FrameTimeStamp& stFrameTime, CBaseDetectItem* pCurrentCtrlBlk );

    ///
    /// @brief
    ///    ��ʱ�����Ϣ��ӽ����͸�dsp�Ķ���
    ///
    void _AddSmvTime2Deque( FrameTimeStamp& stFrameTime, CBaseDetectItem* pCurrentCtrlBlk );

    ///
    /// @brief
    ///    �������ֵ����ȡ������Ϣ
    ///
    void _PolarityCheck( SMV_VALUE& stSmvValue, CBaseDetectItem* pCurrentCtrlBlk );

    ///
    /// @brief
    ///    ��9-1/FT3����ֵ����ת��Ϊϵͳ������ʾ��һ��/����ֵ
    ///
    /// @param[in]  stSmvValueFt3_91  FT3/9-1����ֵ
    /// @param[out] stSmvValue        ����ת�������ֵ
    /// @param[in]  enSmvDispType     ������������ʾֵ����
    /// @param[in]  enFrameValueType  ���������б���ֵ����
    /// 
    /// @return
    ///     bool    ת���ɹ�����true
    ///
    bool _SmvValueConvert(const SMV_VALUE_FT3_9_1& stSmvValueFt3_91, SMV_VALUE& stSmvValue, enumSmvValueType enSmvDispType = SMV_VALUE_TYPE_SECONDARY, enumSmvValueType enFrameValueType = SMV_VALUE_TYPE_SECONDARY);

    ///
    /// @brief
    ///    ��9-2����ֵ����ת��Ϊϵͳ������ʾ��һ��/����ֵ
    ///
    /// @param[in]  stSmvValue92      9-2����ֵ
    /// @param[out] stSmvValue        ����ת�������ֵ
    /// @param[in]  enSmvDispType     ������������ʾֵ����
    /// @param[in]  enFrameValueType  ���������б���ֵ����
    /// 
    /// @return
    ///     bool    ת���ɹ�����true
    ///
    bool _SmvValueConvert(const SMV_VALUE_9_2& stSmvValue92, SMV_VALUE& stSmvValue, enumSmvValueType enSmvDispType = SMV_VALUE_TYPE_SECONDARY, enumSmvValueType enFrameValueType = SMV_VALUE_TYPE_SECONDARY);

    ///
    /// @brief
    ///    �ж�pRecordItem�Ƿ���¼���б���
    ///
    /// @return
    ///     bool ������ڷ���true
    ///
    bool _IsDetectItemInRecordList( CBaseDetectItem* pRecordItem );

    ///
    /// @brief
    ///    Goose,Smv9-1/9-2¼������
    ///
    void _NetRecordWave( CFrameRecvNet * pNet, DSM_OPERATE_MODE enOpMode, CBaseDetectItem* pCurrentDetct );

    ///
    /// @brief
    ///    FT3¼������
    ///
    void _Ft3RecordWave( CFrameRecvFt3 * pFt3, DSM_OPERATE_MODE enOpMode, CBaseDetectItem* pCurrentDetct );

    ///
    /// @brief
    ///    �ж�Ҫ��ǰappid��mac��ַ��Ӧ������֡�Ƿ���¼�����б���
    ///
    /// @return
    ///     �����ǰappid/mac��ַ��¼���б���true
    ///
    bool _IsAppidMacInRecordWaveList( uint16_t nAppid, uint8_t*  pMacAddr, uint8_t nLenMac );

    ///
    /// @brief
    ///    �ж�Ҫ��ǰ���ƿ��Ӧ������֡�Ƿ���¼�����б���
    ///
    /// @return
    ///     �����ǰappid/mac��ַ��¼���б���true
    ///
    bool _IsAppidMacInRecordWaveList( CBaseDetectItem* pDetectItem );

    ///
    /// @brief
    ///    ������ദ������
    ///
    void _NetPhaseCheck( CFrameRecvNet * pNet );

    ///
    /// @brief
    ///    FT3���ദ������
    ///
    void _Ft3PhaseCheck( CFrameRecvFt3 * pFt3 );

    ///
    /// @brief
    ///    �������ݴ���
    ///
    void _PhaseCheckDataHandle( const SMV_VALUE& stSmvValue );

    ///
    /// @brief
    ///    �ж�ĳ��appid portname�Ƿ��ڹ����б���
    ///
    bool _IsAppidInNetFilter(uint16_t nAppid, DsmPortName enPortName = DSM_PORT_UNKOWN, CNetDetectItem** pNetItem = NULL);

    ///
    /// @brief
    ///    ʵʱ¼��ʱ������֡���������ڴ滺����
    ///
    /// @return
    ///     ��ӳɹ�����true
    ///
    bool _WritePckToBuf( PCAP_FILE_TIME_VAL stPcapFileTimeVal, uint8_t * pFrame, uint32_t nFrameLen );

    ///
    /// @brief
    ///    ֪ͨ���淢���쳣���
    ///
    /// @param[in] nMessageId   ֪ͨ��Ϣ��Id
    /// @param[in] wParam       ����
    /// @param[in] lParam       ����
    /// @param[in] bPostMessage �Ƿ�ʹ��PostMessage��trueʹ��PostMessage��falseʹ��SendMessage������ ��Ϣ
    ///
    /// @return
    ///     LRESULT    ʹ��sendMessage���صĽ���������postmessage������0��ʾ����ʧ��
    ///
    LRESULT  _NotifyMessage(uint32_t nMessageId, WPARAM wParam = 0, LPARAM lParam = 0, bool bPostMessage = true);

    ///
    /// @brief
    ///    ����ʵʱ��������
    ///
    void _ClearRealTimeMsg();

    ///
    /// @brief
    ///    �����������ֵ����֡������ͳ����Ϣ
    ///
    void _GenSmvFrameStatistics( CFrameRecvNet * pNet, uint8_t* pFrame, uint32_t nFrameLen, FrameTimeStamp& stFrameTime, CNetDetectItem* pCurrentCtrlBlk );

    ///
    /// @brief
    ///    ����Ft3����ֵ����֡������ͳ����Ϣ
    ///
    void _GenSmvFrameStatistics( CFrameRecvFt3 * pFt3, uint8_t* pFrame, uint32_t nFrameLen, FrameTimeStamp& stFrameTime, CFt3DetectItem* pCurrentCtrlBlk );

    ///
    /// @brief
    ///    ��ղ���ֵͳ����Ϣ
    ///
    void _ClearSmvFrameStatistics();

    ///
    /// @brief
    ///    ����goose����֡������ͳ����Ϣ
    ///
    void _GenGooseFrameStatistics( CFrameRecvNet * pNet, uint8_t* pFrame, uint32_t nFrameLen, FrameTimeStamp& stFrameTime, CBaseDetectItem* pCurrentCtrlBlk );

    ///
    /// @brief
    ///    ���gooseͳ����Ϣ
    ///
    void _ClearGooseFrameStatistics();

    ///
    /// @brief
    ///    �жϺ���ʱ����ʱ����Ƿ���Ա���Ϊ��ͬһʱ�̵Ĳ���ֵ
    ///    �����֡��ʱ�����ڷ���֡ʱ������һ�����£���ʱ��Ϊ��ͬһʱ�̵Ĳ���ֵ
    ///
    bool _IsPhaseCheckTimeStampAlign(const FrameTimeStamp& stTimeStamp1, const FrameTimeStamp& stTimeStamp2, CBaseDetectItem* pDetectItem);

    ///
    /// @brief
    ///     ��պ��໺�������
    ///
    void _ClearPhaseCheckData();

    ///
    /// @brief
    ///    ��մ����͸�dsp�Ĳ���ֵ��Ϣ
    ///
    void _ClearDspSendSmvBuffer();

    ///
    /// @brief
    ///    ���ù��˵������ƿ�
    ///
    bool _SetOneCtrlBlkFilter(CNetDetectItem* pNetDetectItem);

    ///
    /// @brief
    ///    ���ݵ�ǰ������ź����ڲ����㣬���㵱ǰ�����nSmpCntStd���Զ����
    ///
    inline uint32_t _GetDesireSmpDistance(uint32_t nSmpCntCurrent, uint32_t nSmpCntStd, uint32_t nPointWave );

    ///
    /// @brief
    ///    ���1588ʱ����Ϣ
    ///
    void _ClearNet1588Time();

    ///
    /// @brief
    ///    �ж�ĳ��ģʽ�Ƿ��ڼ���ģʽ��12.8k���²�����4k��
    ///
    bool _IsModeReSampleTo4k(DSM_OPERATE_MODE enMode);

    ///
    /// @brief
    ///    ����Ft3���ƿ���Ϣ
    ///
    void _UpdateFt3DetectItem( CBaseDetectItem* pBaseDetectItem );

    ///
    /// @brief
    ///    ���˫AD������Ϣ
    ///
    void _ClearDoubleADData();

    ///
    /// @brief
    ///    ̽��ʱ�л�̽��˿ڵĶ�ʱ��������
    ///
    static void CALLBACK _DetectTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);

    ///
    /// @brief
    ///    �ر�̽�ⶨʱ��
    ///
    void _CloseDetectTimer();
#if !CHUN_HUA
public:
    uint8_t*                     m_pRecordWaveRealTimeBuf;              ///< ʵʱ¼����������ַ
	uint8_t*                     m_pRecordWaveRealTimeBuf2;              ///< ʵʱ¼����������ַ2
	
	uint64_t                     m_nSDCardFreeSize;                      ///< SD�����ÿռ�
	uint64_t                     m_nTotalRecordSize;                      ///< �ܹ�ʹ���˵ü�¼�ռ�
	uint8_t						 m_nCurrentBufferIndex;					///��ǰʹ�õ�Buffer��

	bool						 m_bBuffer1Flag;
	bool						 m_bBuffer2Flag;
	bool 						 m_bIsStopRecord;

	bool						 m_bIsFinishRecord;						///�Ƿ������д��¼���ļ�

	uint32_t				     m_nCurrentRecordSize;
    uint32_t                     m_nRecordWaveIndex;                    ///< ��¼¼����������ʹ�õĳ���

	HANDLE						 m_hRecordThread;                       ///< ¼�� �����߳̾��

    Lock                         m_lockRecordWaveRealTimeBuf;           ///< ʹ�ù����ڴ���
#endif

private:
    HANDLE                       m_hGPMDevice;                          ///< GPMC��������ڲ���FTPA
    CFrameRecvFt3*               m_pFrameFt3A;                          ///< �⴮��A�����ձ���
    CFrameRecvFt3*               m_pFrameFt3B;                          ///< �⴮��B�����ձ���
    CFrameRecvNet*               m_pFrameNetA;                          ///< ����̫��A
    CFrameRecvNet*               m_pFrameNetB;                          ///< ����̫��B
    CFrameRecvNet*               m_pFrameNetC;                          ///< ����̫��C

    CSmvRecvConfigPublic*        m_pSmvRecvConfigPublic;                ///< Smv���չ�������

    DSM_OPERATE_MODE             m_enCurrentMode;                       ///< ��ǰģʽ
    Lock                         m_lockCurrentMode;                     ///< ��ǰ����ģʽ������

    //̽���������
    NetFt3RecvFrameType          m_enCurrentDetectType;                 ///< ��ǰ̽������
    std::vector <CBaseDetectItem*> m_vecDetectItems;                    ///< ���浱ǰ��̽��������
    std::vector <CBaseDetectItem*> m_vecDetectItemsTemp;                ///< ���浱ǰ��̽��������,�ṩ��������ȡ���ʹ��
    Lock                           m_lockDetectItems;                   ///< ����̽����Ϣ������

    //��ɨ���б���ӿ��ƿ���Ҫ���ƿ���ϸ������֡��Ϣ
    std::vector <CDetectItemFrame*> m_vecDetectItemsFrame;              ///< ����̽�������̽�⵽���ƿ��һ������֡
    Lock                            m_lockDetectItemsFrame;             ///< ̽�⵽���ƿ��һ������֡������

    //��ǰѡ�п��ƿ�͵�ǰ���ƿ�
    std::vector<CBaseDetectItem*> m_vecSelectedItems;                   ///< ���浱ǰ��ѡ�еĿ��ƿ�
    CBaseDetectItem*              m_pCurrentDetectItems[3];             ///< ���浱ǰ���ƿ飬ʹ��������Ϊ����ʹ�ú���ʱ��ͬʱָ���������ƿ�
                                                                        ///< �ڽ�������ҵ��ʱ��ֻʹ��[2]��ʾ��ǰ���ƿ�

    //��ǰ���ƿ�ʵʱ���ݵ�ͨ���������ܺ�̽��ʱ��ͨ������һ��
    uint32_t                     m_nRealTimeChannelCounts[2];           ///< ��ǰ���ƿ�ʵʱͨ����Ŀ��[0],����ʱ���ڱ����������ƿ�ʵʱͨ����
    Lock                         m_lockRealTimeChannelCounts;           ///< ���ʵ�ǰ���ƿ�ʵʱͨ����Ŀ��

    //���ݲ㻺���������
#if OSCILLOGRAM_USE_STL_DEQUE
    std::deque<SMV_VALUE>        m_dequeSmvValues;                      ///< ����ɼ����Ĳ���ֵ����,����ͼ������Ҫ
    Lock                         m_lockSmvValuesDeque;                  ///< �����������в���ֵ���ݵ��ٽ��� m_dequeSmvValues�Ļ��������
#else
    COscillogramBuf              m_cOscillogramBuf;                     ///< ����ֵ������
#endif

    //˫AD��������
    SMV_VALUE                    m_stDoubleADData;                      ///< ˫adҵ�����ֵ����
    Lock                         m_lockDoubleADData;                    ///< ˫AD��������

    //���ƿ�״̬��Ϣ����
    CSmvCtrlBlkQuality           m_cSmvCtrlBlkQuality;                  ///< �������µĿ��ƿ�ͨ����Чֵ״̬
    Lock                         m_lockCtrlBlkQuality;                  ///< ������ʿ��ƿ�ͨ����Чֵ��Ϣ������ʵ���
    CBaseDetectItem*             m_pCachedCtrlBlk;                      ///< �������ݵĿ��ƿ�ָ��
    Lock                         m_lockCacheCtrlBlk;                    ///< ���ʻ������ݿ��ƿ�ָ����

    //Dsp�������
    HANDLE                       m_hDspSendProc;                        ///< DSP�����߳̾��
    HANDLE                       m_hEventExitDspSendProc;               ///< �˳�dsp�߳��¼�
    HANDLE                       m_hEventSmv;                           ///< ����ֵ׼�����¼�������֪ͨdsp���з���
    HANDLE                       m_hEventSmvTime;                       ///< ����ֵʱ��׼���ã�����֪ͨdsp���з���

    CDsp*                        m_pDsp;                                ///< dspʵ��ָ��
    Config                       m_cDspConfig;                          ///< dsp������Ϣ
    CDspDataSet                  m_cDspDataSet;                         ///< dsp���㷵����Ϣ
    SMV_DATA_CALC_RT             m_stSmvDataCalcRt;                     ///< ����dsp���㷵�ص�ʵ������

    SmvData                      m_stDspSmvData;                        ///< ���͸�dsp�Ĳ���ֵ������(�ṹ��)
    Lock                         m_lockDspSend;                         ///< ����dsp���͵��������ڴ����̺߳��������̣߳����õ�ǰ���ƿ�ʱ��
                                                                         ///< ���ܻ�ͬʱ�޸�dsp���ͻ���������

    std::deque <MSGTIME>         m_dequeSmvTime;                        ///< �������ݵ�ʱ�������Ҫ���͸�dsp���м�����ɢ�ȼ���
    Lock                         m_lockSmvTime;                         ///< �����޸Ĳ���ֵ���е�ʱ�����

    //���Լ���������
    bool                         m_bPoLarityCheckBegin;                 ///< ���Լ���Ƿ�ʼ
    int8_t       m_nArrayPolarity[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];    ///< ����浱ǰ���ƿ鼫����Ϣ���飬һ��ͨ����Ӧһ��Ԫ��
    Lock                         m_lockArrayPolarity;                   ///< ���ʼ�����Ϣ��
    bool         m_bPolaDetected[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];     ///< �Ƿ��⵽�˼�����Ϣ����⵽���ݺ�Ҫ������ȡһ�ܲ��������ж��Ƿ�������ȷ�ϼ�����Ϣ��Ч���
    bool         m_bPolaDataValid[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];    ///< ������Ϣͨ��һ�ܲ��������ݵ��ж���Ϊ����Ч��
    float   m_fPolaDataCache[NET_9_2_LE_MAX_CHANNELS_PER_ASDU][SMV_MAX_POINT_PER_WAVE]; ///< ���漫���ж�ʱ��һ��ͨ��һ���ܲ�������
    uint32_t     m_nPolaDataIndex[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];    ///< ��ǰһ��ͨ������Ĳ���ֵ���

    CDataRealTimeMsgData         m_cRealTimeMsg;                        ///< ʵʱ������Ϣ���������µ�һ֡����
    Lock                         m_lockRealTimeMsg;                     ///< ����ʵʱ������Ϣ��

    MapDetectItemChangeList      m_mapGooseChangeList;                  ///< ����goose��λ��Ϣ
    Lock                         m_lockGooseChangeList;                 ///< Goose��λ��Ϣ������

    //¼���������
    uint64_t                     m_nRecordBufferSize;                   ///< ¼��ʹ�õ��ڴ滺������С
    std::wstring                 m_strRecordWavePath;                   ///< ¼���ļ�·��
    CWnd*                        m_pNotifyWnd;                          ///< ¼�������г�����Ҫ֪ͨ����������Ƿ�����Ϣ�Ĵ���
    std::vector<CBaseDetectItem*> m_vecRecordWaveList;                  ///< ������Ӷ�����ƿ飬�Զ�����ƿ����¼��
#if DSM_RECORD_WARE_REALTIME
#if CHUN_HUA    

	uint8_t*                     m_pRecordWaveRealTimeBuf;              ///< ʵʱ¼����������ַ
    uint32_t                     m_nRecordWaveIndex;                    ///< ��¼¼����������ʹ�õĳ���
    Lock                         m_lockRecordWaveRealTimeBuf;           ///< ʹ�ù����ڴ���
#endif

#else
    IPcapFile*                   m_pPcapFile;                           ///< pcap�ļ��ӿ�
    Lock                         m_lockPcapFile;                        ///< ʹ��pcap�ļ��ӿ���
#endif

    //�����������
    const uint32_t               m_nPhaseCheckIndexLen;                 ///< ������������ʱ��������ƿ����ݸ���,��Ϊ12800�����Դ���4k��12.8k�Ĳ�����
    SMV_VALUE*                   m_pPhaseCheckAIndex;                   ///< ������������ʱ�������һ�����ƿ�����ָ��
    SMV_VALUE*                   m_pPhaseCheckBIndex;                   ///< ������������ʱ������ڶ������ƿ�����ָ��
    Lock                         m_lockPhaseCheckData;                  ///< ���ʺ��໺��������
    uint32_t                     m_nPhaseCheckCurIndex;                 ///< ��ǰ���͵Ĳ������
    CSampleInterpSync            m_cSmpSyncTime;                        ///< ʱ��������ݴ�����
    float*                       m_pResultY;                            ///< ʱ����뱣�����������ָ��
    uint32_t                     m_nResultYLen;                         ///< ʱ����뱣���������Ŀ

    std::map<uint16_t, CSmvRecvConfig*> m_mapAppIDSmvRecv91Config;      ///< ����������������һ��9-1 appid��Ӧ��ͨ������
    std::map<uint16_t, CSmvRecvConfig*> m_mapAppIDSmvRecv92Config;      ///< ����������������һ��9-2 appid��Ӧ��ͨ������
    CSmvRecvConfig*              m_pFt3SmvRecvCofig[2];                 ///< ����FT3ͨ���Ķ�Ӧ���ã�0��ʾ����ͨ�����ã�1��ʾ����ͨ������

    //Smv�����쳣ͳ��
    MapDetectItemSmvFrameStatistics  m_mapSmvFrameStatistics;           ///< ����ͳ�ƹ����У����汨��ͳ�������Ϣ
    Lock                             m_lockSmvFrameStatistics;          ///< ����ͳ����Ϣʹ����

    //Goose�����쳣ͳ��
    MapDetectItemGooseFrameStatistics m_mapGooseFrameStatistics;        ///< ����ͳ�ƹ����У�����gooseͳ���м���
    Lock                              m_lockGooseFrameStatistics;       ///< Goose����ͳ����Ϣʹ����

    //̽�������9-2�����ʹ�����Ϣ
    Lock                              m_lock92SmpRateCalcInfo;          ///< ���ڻ������9-2����������Ϣ
    MapSmpRateCalcInfo                m_map92SmpRateCalcInfo;           ///< ̽�������9-2�����ʹ�����Ϣӳ���

    //1588ʱ�����Ϣ
    IEEE1588_TIMESTAMP                m_st1588TimeStamp;                ///< 1588����/ʱ�����Ϣ
    Lock                              m_lock1588TimeStamp;              ///< 1588ʱ�������Ϣ

    //̽������ж�ʱ����ʱ�л�̽��˿ڣ���ֹͬʱ̽�����˿���ɵ�12.8k������64ͨ������ֵ��ס������
    static CFrameRecv*                m_pFrameRecv;                     ///< Ϊ�˶�ʱ���ص��п���ʹ�ý������е���غ�������
    uint32_t                          m_nDetectTimerId;                 ///< ̽�ⶨʱ��id
    uint32_t                          m_nDetectTimerCount;              ///< ���붨ʱ�����������ݴ����ж϶˿ڵĴ򿪹ر�
};
