/// @file
///
/// @brief 
///     管理数据帧的接收
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.3.20
///
/// 修改历史：
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
//时标对齐
#include "src\utils\SampleInterpSync\include\SampleInterpSync.h"
#include "src\utils\SampleInterpSync\interp.h"

#include "src\service\datalayer\recv\OscillogramBuf.h"

#define WM_DATA_LAYER_MESSAGE           (PRIVATE_WND_USERID + 50)                   ///< 数据层发送给界面的消息ID,不同消息使用wparam区分
#define MSG_RECORD_SIZE_REACHED         (1)                                         ///< 录波大小已经达到设定的数值

//波形图数据缓冲选择，1为stl双端队列，0为双缓冲
#define OSCILLOGRAM_USE_STL_DEQUE  1

//保存文件出错代码，异常代码与CFileException中错误原因定义一致，文件打开失败定义为0
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
///    探测9-2数据帧，自动分析采样率所需信息
///
struct SmpRateCalcInfo
{
    FrameTimeStamp  stFirst;                                               ///< 缓冲区中此appid第一帧时间
    FrameTimeStamp  stLast;                                                ///< 缓冲区中此Appid最后一帧时间
    uint32_t        nFrameCnt;                                             ///< 缓冲区中此appid数据帧的个数（一个帧可能有多个采样点）
    uint32_t        nAsdu;                                                 ///< 一个数据帧中采样点个数
    bool            bBufFin;                                               ///< 一个缓冲区是否结束

    SmpRateCalcInfo(): nFrameCnt(0)
                     , nAsdu(0)
                     , bBufFin(0)
    {
    }

    void ParseSample(const FrameTimeStamp& stFrameTime, uint32_t nAsduCnt)
    {
        //只有bBufFin为false，才进行数据统计
        //bBufFin在一个缓冲区结束时，如果数据帧个数大于2则标记为true
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
    ///    重新初始化数据
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
    ///    计算采样率
    ///
    /// @return
    ///    默认返回4k，如果探测到比较靠近12.8k，则返回12.8k,
    ///    探测过程中没有获取到结果则返回0
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
///    定义appid到对应的采样点信息映射类型，方便使用
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
    ///    初始化接收数据帧
    ///
    /// @return
    ///     true标识初始化成功，false失败
    bool InitFrameRecv();

    ///
    /// @brief
    ///    清理数据接收帧的接收
    ///
    /// @return
    ///     true标识反初始化成功，false失败
    void UnInitFrameRecv();

    ///
    /// @brief
    ///    开始某个端口数据帧接收
    ///
    bool StartPortFrameRecv(DsmPortName enPortName);

    ///
    /// @brief
    ///    停止某个端口数据帧接收
    ///
    bool StopPortFrameRecv(DsmPortName enPortName);

    ///
    /// @brief
    ///    设置接收线程优先级
    ///
    void SetRxThreadPriority(int nPriority);


//
//=============================================================================
// 探测相关
//=============================================================================
//
    ///
    /// @brief
    ///    告诉数据层，要获取探测结果
    ///    数据层要去掉底层的过滤，获取网络接口的所有报文
    ///
    void BeginDetect(NetFt3RecvFrameType enNetRecvFrameType, bool bEnabelDetect = true);

    ///
    /// @brief
    ///    获取探测结果数目
    ///
    /// @return
    /// 返回探测结果的数目
    ///
    uint32_t GetNetDetectItemsCount();

    ///
    /// @brief
    ///    获取一个探测结果
    ///
    /// @param[in]  nIndex  要获取探测结果的索引，从0开始
    ///
    /// @return
    ///     CNetDetectItem* 返回指定索引探测结果对象的指针
    ///
    CNetDetectItem* GetNetDetectItem(uint32_t nIndex);

    ///
    /// @brief
    ///    获取探测结果数目
    ///
    /// @return
    /// 返回探测结果的数目
    ///
    uint32_t GetFt3DetectItemsCount();

    ///
    /// @brief
    ///    获取一个探测结果
    ///
    /// @param[in]  nIndex  要获取探测结果的索引，从0开始
    ///
    /// @return
    ///     CFt3DetectItem* 返回指定索引探测结果对象的指针
    ///
    CFt3DetectItem* GetFt3DetectItem(uint32_t nIndex);

    ///
    /// @brief
    ///    获取1588探测结果数目
    ///
    /// @return
    /// 返回探测结果的数目
    ///
    uint32_t GetNet1588DetectItemsCount();

    ///
    /// @brief
    ///    获取一个1588探测结果
    ///
    /// @param[in]  nIndex  要获取探测结果的索引，从0开始
    ///
    /// @return
    ///     CNetDetectItem* 返回指定索引探测结果对象的指针
    ///
    CNet1588DetectItem* GetNet1588DetectItem(uint32_t nIndex);

    ///
    /// @brief
    ///    获取控制块对应的一个数据帧信息
    ///
    /// @param[in]      pDetectItem     要获取数据帧的控制块指针
    ///
    /// @return
    ///     CDetectItemFrame*   返回控制块对应的一个数据帧，NULL表示获取失败
    ///
    CDetectItemFrame* GetDetectItemFrame(CBaseDetectItem* pDetectItem);

    ///
    /// @brief
    ///    获取当前的探测类型
    ///
    /// @return
    ///     NetFt3RecvFrameType 返回当前的探测数据类型
    ///
    NetFt3RecvFrameType GetCurrentRecvType();

    ///
    /// @brief
    ///    开始接受特定类型数据
    ///
    /// @param[enRecvType]  设置结束数据类型，goose，smv，ft3
    /// @param[enMode]      接收数据默认进入的模式
    ///
    /// @note
    ///     设置接收后，对网络端口会调用Detect接口接收说有网络数据（没有过滤），对FT3开始数据接收
    ///
    void StartRecvData(NetFt3RecvFrameType enRecvType, DSM_OPERATE_MODE enMode = DSM_MODE_UNKOWN);

    ///
    /// @brief
    ///    停止数据接收
    ///
    void StopRecvData();

//
//=============================================================================
// 设置过滤相关
//=============================================================================
//    
    ///
    /// @brief
    ///    设置当前的控制块，dsp计算此控制块的信息
    ///
    /// @param[in] pDetectItem  要设置的当前控制块
    ///
    void SetCurrentCtrlBlk( CBaseDetectItem* pDetectItem);

    ///
    /// @brief
    ///    获取当前的控制块指针
    ///
    /// @return
    ///     CBaseDetectItem*   返回当前控制块指针，失败返回Null
    ///
    CBaseDetectItem* GetCurrentCtrlBlk();

    ///
    /// @brief
    ///    获取当前的控制块实时通道数
    ///
    /// @return
    ///     uint32_t   返回当前控制块实时通道数
    ///
    uint32_t GetCurrentCtrlBlkRealTimeChannelCounts();

    ///
    /// @brief
    ///    添加一个选中的控制块
    ///
    /// @param[in] pDetectItem  要设置的当前控制块
    ///
    /// @return
    ///     bool    添加成功返回true
    ///
    bool AddOneSelectedCtrlBlk(CBaseDetectItem* pDetectItem);

    ///
    /// @brief
    ///    移除一个已经添加的控制块
    ///
    /// @param[in] pDetectItem  要移除的控制块
    ///
    void RemoveOneSelectedCtrlBlk(CBaseDetectItem* pDetectItem);

    ///
    /// @brief
    ///    获取已经选中的控制块数量
    ///
    /// size_t      返回已经选中的控制块数量
    ///
    size_t GetSelectedCtrlBlkCount();

    ///
    /// @brief
    ///    获取已经选中的第一个控制块信息
    ///
    /// @return
    ///     CBaseDetectItem* 获取第一个控制块指针，没有则返回NULL
    ///
    CBaseDetectItem* GetFirstSelectCtrlBlk();

    ///
    /// @brief
    ///    获取已经选中的上一个控制块信息
    ///
    /// @param[in]  pCurrentCtrlBlk 指向当前的控制块
    ///
    /// @return
    ///     CBaseDetectItem* 获取上一个控制块指针，没有上一个则返回NULL
    ///
    CBaseDetectItem* GetPrevSelectCtrlBlk(CBaseDetectItem* pCurrentCtrlBlk);
    
    ///
    /// @brief
    ///    获取已经选中的下一个控制块信息
    ///
    /// @param[in]  pCurrentCtrlBlk 指向当前的控制块
    ///
    /// @return
    ///     CBaseDetectItem* 获取下一个控制块指针，没有下一个则返回NULL
    ///
    CBaseDetectItem* GetNextSelectCtrlBlk(CBaseDetectItem* pCurrentCtrlBlk);

    ///
    /// @brief
    ///    根据id获取已经选中的控制块
    ///
    /// @param[in]  nID     在生成扫描列表时使用的索引获取控制块指针
    ///
    /// @return
    ///     CBaseDetectItem* 获取索引id对应的控制块指针，不存在则返回NULL
    ///
    CBaseDetectItem* GetSelectCtrlBlkByID(uint32_t nID);

    ///
    /// @brief
    ///    告诉数据层，已选择的控制块已经添加完毕
    ///
    void AddSelectedCtrlBlkFinished();

//
//=============================================================================
// 核相信息设置相关接口
//=============================================================================
//

    ///
    /// @brief
    ///    设置核相使用的第1个控制块
    ///
    /// @param[in] pDetectItem  要设置的控制块
    ///
    void SetPhaseCheckSmv1CtrBlk(CBaseDetectItem* pDetectItem);

    ///
    /// @brief
    ///    设置核相使用的第二个控制块
    ///
    /// @param[in] pDetectItem  要设置的当前控制块
    ///
    void SetPhaseCheckSmv2CtrBlk(CBaseDetectItem* pDetectItem);

    ///
    /// @brief
    ///    获取核相使用的第1个控制块
    ///
    /// @return
    ///     CBaseDetectItem*   返回当前控制块指针，失败返回Null
    ///
    CBaseDetectItem* GetPhaseCheckSmv1CtrlBlk();

    ///
    /// @brief
    ///    获取核相使用的第二个控制块
    ///
    /// @return
    ///     CBaseDetectItem*   返回当前控制块指针，失败返回Null
    ///
    CBaseDetectItem* GetPhaseCheckSmv2CtrlBlk();

    ///
    /// @brief
    ///    获取Smv1三相通道
    ///
    /// @param[out]  nPhaseA 保存A相通道号
    /// @param[out]  nPhaseB 保存B相通道号
    /// @param[out]  nPhaseC 保存C相通道号
    ///
    /// return
    ///     bool    获取成功返回true
    ///
    bool GetSmv1ABCChannles(uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC);

    ///
    /// @brief
    ///    设置Smv1三相电压/电流通道
    ///
    /// @param[in]  nPhaseA A相通道号
    /// @param[in]  nPhaseB B相通道号
    /// @param[in]  nPhaseC C相通道号
    ///
    /// return
    ///     bool    设置成功返回true
    ///
    bool SetSmv1ABCChannles(uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC);

    ///
    /// @brief
    ///    获取Smv2三相通道
    ///
    /// @param[out]  nPhaseA 保存A相通道号
    /// @param[out]  nPhaseB 保存B相通道号
    /// @param[out]  nPhaseC 保存C相通道号
    ///
    /// return
    ///     bool    获取成功返回true
    ///
    bool GetSmv2ABCChannles(uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC);

    ///
    /// @brief
    ///    设置Smv2三相电压/电流通道
    ///
    /// @param[in]  nPhaseA A相通道号
    /// @param[in]  nPhaseB B相通道号
    /// @param[in]  nPhaseC C相通道号
    ///
    /// return
    ///     bool    设置成功返回true
    ///
    bool SetSmv2ABCChannles(uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC);

    ///
    /// @brief
    ///    获取核相两个控制块实时通道数
    ///
    /// @param[out]  nChannelCountSmv1 保存控制块1的通道数目
    /// @param[out]  nChannelCountSmv2 保存控制块2的通道数目
    ///
    /// @return
    ///     bool   获取成功返回true
    ///
    bool GetPhaseCheckRealTimeChannelCounts(uint32_t& nChannelCountSmv1, uint32_t& nChannelCountSmv2);

//
//=============================================================================
// 设置模式，根据不同页面设置对应的页面模式，数据层根据模式处理数据
//=============================================================================
//
    ///
    /// @brief
    ///    设置当前模式
    ///
    /// @param[in]  enDsmMode 要设置的模式
    ///
    void SetCurrentMode(DSM_OPERATE_MODE enDsmMode);

    ///
    /// @brief
    ///    获取当前模式
    ///
    /// @return
    ///     DSM_OPERATE_MODE    返回当前系统模式
    ///
    DSM_OPERATE_MODE GetCurrentMode();

//
//=============================================================================
// 获取SMV接收业务数据接口
//=============================================================================
//
    ///
    /// @brief
    ///    获取有效值数据
    ///
    /// @param[out]  pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool   数据获取成功返回true，pData中保存获取到的数据
    ///
    /// @note
    ///     pData 指向的内存区域由调用者自己申请
    ///
    bool GetEffectiveVal(CDataEffectiveVal* pData);

    ///
    /// @brief
    ///    获取波形图数据
    ///
    /// @param[out]  pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool   数据获取成功返回true，pData中保存获取到的数据
    ///
    /// @note
    ///     pData 指向的内存区域由调用者自己申请
    ///
    bool GetOscillogramData(CDataOscillogram* pData);

    ///
    /// @brief
    ///    获取功率数据
    ///
    /// @param[out]  pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool   数据获取成功返回true，pData中保存获取到的数据
    ///
    /// @note
    ///     pData 指向的内存区域由调用者自己申请
    ///
    bool GetPowerData(CDataPower* pData);

    ///
    /// @brief
    ///    获取双AD数据
    ///
    /// @param[out]  pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool   数据获取成功返回true，pData中保存获取到的数据
    ///
    /// @note
    ///     pData 指向的内存区域由调用者自己申请
    ///
    bool GetDoubleAdData(CDataDoubleAD* pData);

    ///
    /// @brief
    ///    获取状态监视数据
    ///
    /// @param[out]  pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool   数据获取成功返回true，pData中保存获取到的数据
    ///
    /// @note
    ///     pData 指向的内存区域由调用者自己申请
    ///
    bool GetStatusWatchData(CDataStatusWatch* pData);

    ///
    /// @brief
    ///    获取序量数据
    ///
    /// @param[out]  pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool   数据获取成功返回true，pData中保存获取到的数据
    ///
    /// @note
    ///     pData 指向的内存区域由调用者自己申请
    ///
    bool GetXuliangData(CDataXuliang* pData);

    ///
    /// @brief
    ///    获取相量数据
    ///
    /// @param[out]  pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool   数据获取成功返回true，pData中保存获取到的数据
    ///
    /// @note
    ///     pData 指向的内存区域由调用者自己申请
    ///
    bool GetPhasorData(CDataPhasor* pData);

    ///
    /// @brief
    ///    获取谐波信息数据
    ///
    /// @param[out]  pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool   数据获取成功返回true，pData中保存获取到的数据
    ///
    /// @note
    ///     pData 指向的内存区域由调用者自己申请
    ///
    bool GetHarmonicData(CDataHarmonic* pData);

    ///
    /// @brief
    ///    获取报文统计/离散度信息
    ///
    /// @param[out]  pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool   数据获取成功返回true，pData中保存获取到的数据
    ///
    /// @note
    ///     pData 指向的内存区域由调用者自己申请
    ///
    bool GetFrameStatistics(CDataFrameStatistics* pData);

    ///
    /// @brief
    ///    清空统计数据，重新统计
    ///
    /// @return
    ///     bool    重置成功返回true
    ///
    bool ResetSmvFrameStatistics();

    ///
    /// @brief
    ///    获取当前控制块对应的通道状态信息
    ///
    /// @return
    ///     CSmvCtrlBlkQuality* 返回当前控制块通道信息的一个对象指针
    ///
    CSmvCtrlBlkQuality* GetCurrentCtrlBlkChannelStatus();

    ///
    /// @brief
    ///    重置所有的通道极性
    ///
    void ResetAllPorlarity();

    ///
    /// @brief
    ///    获取极性信息
    ///
    /// @param[out]  pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool   数据获取成功返回true，pData中保存获取到的数据
    ///
    /// @note
    ///     pData 指向的内存区域由调用者自己申请
    ///
    bool GetPolarityData(CDataPolarity* pData);

    ///
    /// @brief
    ///    获取最新一帧数据内容
    ///
    /// @param[out] pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool    数据获取成功返回true，pData中保存获取到的数据
    ///
    bool GetRealTimeMsg(CDataRealTimeMsgData* pData);

    ///
    /// @brief
    ///    获取核相数据信息
    ///
    /// @param[out] pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool    数据获取成功返回true，pData中保存获取到的数据
    ///
    bool GetPhaseCheckData(CDataPhaseCheck* pData);

    ///
    /// @brief
    ///    开始/停止极性信息检测
    ///
    /// @param[in]  bStart  是否开始极性监测，true开始监测，false停止监测
    ///
    /// @note
    ///     只有在极性检查模式下生效
    void StartPolarityCheck(bool bStart = true);

//
//=============================================================================
// 获取goose数据业务接口
//=============================================================================
//
    ///
    /// @brief
    ///    获取GOOSE实时值
    ///
    /// @param[out] pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool    数据获取成功返回true，pData中保存获取到的数据
    ///
    bool GetGooseRealTimeValue(CGooseInfo* pData);

    ///
    /// @brief
    ///    获取GOOSE实时报文
    ///
    /// @param[out] pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool    数据获取成功返回true，pData中保存获取到的数据
    ///
    bool GetGooseRealTimeMsg(CDataRealTimeMsgData* pData);

    ///
    /// @brief
    ///    获取GOOSE报文统计信息
    ///
    /// @param[out] pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool    数据获取成功返回true，pData中保存获取到的数据
    ///
    bool GetGooseFrameStatistics(CGooseFrameStatistics* pData);

    ///
    /// @brief
    ///    清空Goose报文统计数据，重新统计
    ///
    /// @return
    ///     bool    重置成功返回true
    ///
    bool ResetGooseFrameStatistics();

    ///
    /// @brief
    ///    获取GOOSE变位信息
    ///
    /// @param[out] pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool    数据获取成功返回true，pData中保存获取到的数据
    ///
    bool GetGooseChangeList(CDataGooseChangeList* pData);

///
///=============================================================================
/// 获取1588报文时间接口
///=============================================================================
///
    ///
    /// @brief
    ///    获取1588时间信息
    ///
    IEEE1588_TIMESTAMP GetNet1588FrameTime()
    {
        AutoLock autoLock(m_lock1588TimeStamp);
        return m_st1588TimeStamp;
    }

//
//=============================================================================
// 录波接口，录Goose，NetSmv，Ft3数据
//=============================================================================
//
    ///
    /// @brief
    ///    设置录波文件大小,需要在开始录波前设置，设置
    ///
    bool SetRecordWaveSize(uint64_t nSizeInByte);

    ///
    /// @brief
    ///    获取设置的录波文件大小
    ///
    uint64_t GetRecordWaveSize();


#if !CHUN_HUA
	///
	/// @brief
	///    设置SD卡大小
	///
	void SetSDCardSize(uint64_t SDCardFreeSize);

	///
	/// @brief
	///    获取SD卡大小
	///
	uint64_t GetSDCardSize();

	///
	/// @brief
	///    开始录波
	///
	void StartRecord();///

	/// @brief
	///    获取SD卡大小
	///
	static	DWORD WINAPI RecordThreadProc(PVOID pArg);

	static void _Write2File( CFrameRecv* pFrameRecv,uint8_t* buffer, DSM_OPERATE_MODE enMode );

#endif
    ///
    /// @brief
    ///    设置录波文件路径，需要在录波开始前设置
    ///
    void SetRecordWavePath(const std::wstring& strRecordWavePath);

    ///
    /// @brief
    ///    获取设置的录波文件路径
    ///
    std::wstring GetRecordWavePath();

    ///
    /// @brief
    ///    设置通知界面时使用的窗口Cwnd指针
    ///
    bool SetNotifyWnd(CWnd* pWnd);

    ///
    /// @brief
    ///    获取设置的通知界面使用的Cwnd窗口
    ///
    CWnd* GetNotifyWnd();

    ///
    /// @brief
    ///    添加录波控制块,主要针对网络数据帧（网络采样值和Goose）
    ///
    bool AddRecordItem(CBaseDetectItem* pRecordItem);

    ///
    /// @brief
    ///    从录波列表条目中移除一个控制块
    ///
    bool RemoveRecordItem(CBaseDetectItem* pRecordItem);

    ///
    /// @brief
    ///    从录波列表条目中移除所有控制块
    ///
    bool RemoveAllRecordItem();

    ///
    /// @brief
    ///    开始录网络采样值数据，用pcap格式保存
    ///
    bool StartRecordNetSmv();

    ///
    /// @brief
    ///    开始录Goose报文，采用pcap格式保存
    ///
    bool StartRecordGoose();

    ///
    /// @brief
    ///    开始录FT3数据帧
    ///
    bool StartRecordFt3();

    ///
    /// @brief
    ///    停止录波
    ///
    /// @param[out]     记录失败时候时失败原因
    ///
    /// @return
    ///     bool 停止成功
    ///
    bool StopRecordWave(uint32_t& nResult);

    ///
    /// @brief
    ///    取消当前录波
    ///
    bool CancelRecordWave();

//
//=============================================================================
// 接收配置接口
//=============================================================================
//
    ///
    /// @brief
    ///    获取核相时标对齐配置类指针，可以在其他地方修改时标对齐配置
    ///
    CSampleInterpSync* GetSampleInterSync()
    {
        return &m_cSmpSyncTime;
    }

    ///
    /// @brief
    ///    获取Smv接收公共配置接口
    ///
    CSmvRecvConfigPublic* GetSmvConfigPublic();

    ///
    /// @brief
    ///    更新缓存的控制块配置信息一次/二次值
    ///
    /// @param[in]  enChannelType 要修改的通道类型，电压通道/电流通道
    /// @param[in]  nPriSectype   要修改的值类型，是一次值（1）还是二次值（2）
    /// @param[in]  nVal          最新的数值
    ///
    void UpdateCachedRatedValues(enumChannelType enChannelType, uint32_t nPriSecType, float fVal);

//
//=============================================================================
// dsp操作相关
//=============================================================================
//
    ///
    /// @brief
    ///    dsp发送数据线程
    ///
    static DWORD WINAPI DspSendProc(LPVOID lpParameter);

    ///
    /// @brief
    ///    启动dsp发送线程
    ///
    bool StartDspSendProc();

    ///
    /// @brief
    ///    停止DSP发送线程
    ///
    bool StopDspSendProc();

    ///
    /// @brief
    ///    配置dsp计算使用的采样率
    ///
    bool SetDspSampleRate(uint32_t nSampleRate);

    ///
    /// @brief
    ///    配置dsp中计算离散度使用的Asdu数目
    ///
    bool SetDspAsduCount(uint32_t nAsdu = 1);

    ///
    /// @brief
    ///    获取dsp版本信息
    ///
    uint8_t GetDspVersion();

//
//=============================================================================
// 加载预定义通道配置
//=============================================================================
//
    ///
    /// @brief
    ///    根据不同的厂商协议默认设置，加载对应的通道配置
    ///
    /// @param[in]  enConfigType    要加载的配置模板类型
    /// @param[out] pSmvRecvConfig  要填充的配置信息
    ///
    /// @return
    ///     bool    加载成功返回true
    ///
    bool SetSmvRecvChannelTemplate(enumSmvRecvConfigTemplateType enConfigType, CSmvRecvConfig* pSmvRecvConfig);


    ///
    /// @brief
    ///    清空缓存的appid接收配置信息
    ///
    void ClearAppidSmvRecvCache();

private:
    ///
    /// @brief
    ///    处理接收到的Ft3数据
    ///
    void _Ft3DataHandler(PVOID pArg);

    ///
    /// @brief
    ///    处理接收到的以太网数据
    ///
    void _NetDataHandler(PVOID pArg);

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
    bool _Detect(NetFt3RecvFrameType enNetRecvFrameType, uint16_t nCount = NET_DEFAULT_RECV_FRAMES, bool bEnableTimeout = NET_DEFAULT_RECV_ENABLETIMEOUT, uint32_t nTimeoutMilliSecond = NET_DEFAULT_RECV_TIMEOUT);

    ///
    /// @brief
    ///    获取Ft3接口探测结果
    ///
    void _GenFt3DetectInfo( CFrameRecvFt3 * pFt3 );

    ///
    /// @brief
    ///    获取网络接口探测结果
    ///
    void _GenNetDetectInfo(CFrameRecvNet * pNet);

    ///
    /// @brief
    ///    分析网络接口获取的数据，根据设置进行处理
    ///
    void _NetSmvAnaly( CFrameRecvNet * pNet , DSM_OPERATE_MODE enMode, CNetDetectItem* pCurrentCtrlBlk);

    ///
    /// @brief
    ///     分析Ft3接口获取的数据，根据设置进行处理
    ///
    void _Ft3SmvAnaly(CFrameRecvFt3* pFt3 , DSM_OPERATE_MODE enMode, CFt3DetectItem* pCurrentCtrlBlk);

    ///
    /// @brief
    ///    添加探测结果到m_vecDetectItems
    ///
    /// @param[in]  pBaseDetectItem 要添加的探测结果指针
    ///
    /// @return
    ///     bool 添加成功返回true
    ///
    bool _AddDetectItem(CBaseDetectItem* pBaseDetectItem);

    ///
    /// @brief
    ///    判断当前控制块信息是否已经在探测列表中
    ///
    /// @param[in]  pBaseDetectItem 要添加的探测结果指针
    ///
    /// @return
    ///     bool 已经存在返回true
    ///
    bool _IsDetectItemExist(CBaseDetectItem* pBaseDetectItem);

///
///=============================================================================
/// 探测过程中采样率计算辅助函数
///=============================================================================
///
    ///
    /// @brief
    ///    一次缓冲区最后一帧分析完毕时条用，标记某些符合条件的appid，不需要进行后续分析
    ///
    void _SmpCalcAfterBufferAnalysis(bool bAutoDetect);

    ///
    /// @brief
    ///    重置9-2采样率计算缓存数据
    ///
    void _Reset92SmpCalcData();

    ///
    /// @brief
    ///    更新系统9-2采样率设置,在用户勾选控制块后根据探测到9-2的采样率，修改系统9-2接收采样率
    ///    探测的控制块已经保留了探测到的9-2采样率，因此选中的控制块的采样率是相同的（界面勾选保证）
    ///
    void _UpdateSmvRecvPub92SmpRate();

    ///
    /// @brief
    ///    获取正常处理数据帧对应的控制块信息
    ///
    CNetDetectItem* _GetNetFrameDetectItem(uint16_t nAppid, enumFrameType enFrameType, DsmPortName enPortName, const uint8_t macAddr[]);

    ///
    /// @brief
    ///    清空所有容器数据
    ///
    void _ClearAllContainer();

    ///
    /// @brief
    ///    清空探测信息
    ///
    void _ClearDetectItems();

    ///
    /// @brief
    ///    从m_vecSelectedItems中获取信息，设置到各个端口进行过滤
    ///
    bool _SetNetFilter();

    ///
    /// @brief
    ///    判断端口是否是网络端口
    ///
    bool _IsNetPort(DsmPortName enPortName);

    ///
    /// @brief
    ///    判断端口是否是光串口
    ///
    bool _IsFt3Port(DsmPortName enPortName);

    ///
    /// @brief
    ///    判断是否是FT3类型数据帧
    ///
    bool _IsFt3Frame(enumFrameType enFrameType);

    ///
    /// @brief
    ///    判断是否是网络类型数据帧
    ///
    bool _IsNetFrame(enumFrameType enFrameType);

    ///
    /// @brief
    ///    判断是否是采样值数据帧,9-1/9-2/ft3
    ///
    bool  _IsSmvFrame(enumFrameType enFrameType);

    ///
    /// @brief
    ///    判断某个端口数据是否在当前控制块中
    ///
    bool _IsPortInCurrentCtrlBlk(DsmPortName enPortName);

    ///
    /// @brief
    ///    判断某个端口数据是否在已经选择的控制块中
    ///
    bool _IsPortInSelecedCtrlBlk(DsmPortName enPortName);

    ///
    /// @brief
    ///    分析smv数据
    ///
    void _NetSmvBufferAnaly(CNetRecvBuffer& rNetRecvBuffer, DSM_OPERATE_MODE enOpMode, CNetDetectItem* pCurrentCtrlBlk, CFrameRecvNet * pNet);

    ///
    /// @brief
    ///    保存手动试验、状态序列测试时GOOSE的DI值
    ///
    /// @param[in]  pNet    以太网数据接收类
    ///
    void _NetSaveGooseDI(CFrameRecvNet* pNet);

    ///
    /// @brief
    ///    分析goose数据
    ///
    void _NetGooseAnaly(CFrameRecvNet* pNet, DSM_OPERATE_MODE enOpMode, CNetDetectItem* pCurrentDetct);

    ///
    /// @brief
    ///    分析1588数据
    ///
    void _Net1588Analy( CFrameRecvNet * pNet, DSM_OPERATE_MODE enOpMode, CNetDetectItem* pCurrentDetct );

    ///
    /// @brief
    ///    分析国网Ft3数据
    ///
    void _Ft3SmvAnalyGuowang(CFt3RecvBuffer& rRecvBuffer, DSM_OPERATE_MODE enOpMode, CFt3DetectItem* pCurrentCtrlBlk, CFrameRecvFt3* pFt3);

    ///
    /// @brief
    ///    分析南瑞FT3数据
    ///
    void _Ft3SmvAnalyNanrui(CFt3RecvBuffer& rRecvBuffer, DSM_OPERATE_MODE enOpMode, CFt3DetectItem* pCurrentCtrlBlk, CFrameRecvFt3* pFt3);

    ///
    /// @brief
    ///    保存采样值到双端队列里
    ///
    /// @note
    ///     为了防止队列无限增长，限制大小为波形图所需数据的2倍
    ///     队列resize时，如果本身尺寸超过设置的大小，则会删除接近队列尾端的数据
    ///     为了让缓冲一直保持最新数据，所以每次将数据插入队列头部，时间比较久的数据
    ///     会被删除，取数据的时候需要注意，队列尾部的数据是时间早一些的数据。
    ///
    void _AddSmv2Deque(SMV_VALUE& stSmvValue);

    ///
    /// @brief
    ///    保存双AD数据
    ///
    void _AddSmv2DoubleAD(SMV_VALUE& stSmvValue);

    ///
    /// @brief
    ///    根据模式，对采样值进行处理
    ///
    void _SmvHandle(DSM_OPERATE_MODE enOpMode, SMV_VALUE& stSmvValue, CBaseDetectItem* pCurrentCtrlBlk);

    ///
    /// @brief
    ///    清除为波形图保存的采样值数据
    ///
    void _ClearSmvDeque();

    ///
    /// @brief
    ///    添加采样值到dsp发送采样值缓冲，采样值到达一个周波数据时发送一次采样值给dsp
    ///
    void _AddSmv4DspSend(DSM_OPERATE_MODE enOpMode, SMV_VALUE& stSmvValue, CBaseDetectItem* pCurrentCtrlBlk);

    ///
    /// @brief
    ///    根据通道类型和通道相别，产生发送dsp需要的相别识别字节
    ///
    uint8_t _GetDspChannelVCType(enumChannelType enChannelType, ChannelPhase enChannelPhase);

    ///
    /// @brief
    ///    获取一个控制块的采样率
    ///
    uint32_t _GetCtrlBlkSampleRate(CBaseDetectItem* pCtrlBlk);

    ///
    /// @brief
    ///    获取一个控制块某个通道的相别类型信息
    ///
    uint8_t _GetCtrlBlkChannelVCType(CBaseDetectItem* pCtrlBlk, uint32_t nChannelIndex, DSM_OPERATE_MODE enOpMode );

    ///
    /// @brief
    ///    初始化dsp发送需要的事件
    ///
    bool _InitDspSendEvents();

    ///
    /// @brief
    ///    清理dsp发送等待事件
    ///
    void _UninitDspSendEvents();

    ///
    /// @brief
    ///    设置缓存数据对应的控制块指针
    ///
    void _SetCachedCtrlBlk(CBaseDetectItem* pCtrlBlk);

    ///
    /// @brief
    ///    获取缓存数据对应的控制块指针
    ///
    CBaseDetectItem* _GetCachedCtrlBlk();

    ///
    /// @brief
    ///    清空所有缓存信息
    ///
    void _ClearCachedData();

    ///
    /// @brief
    ///    清空发送采样值时间（计算离散度）缓存数据
    ///
    void _ClearSmvTimeDeque();

    ///
    /// @brief
    ///    清除goose变位列表记录数据
    ///
    void _ClearGooseChangeList();

    ///
    /// @brief
    ///    smv时间戳处理
    ///
    void _SmvTimeHandle( DSM_OPERATE_MODE enOpMode, FrameTimeStamp& stFrameTime, CBaseDetectItem* pCurrentCtrlBlk );

    ///
    /// @brief
    ///    将时间戳信息添加进发送给dsp的队列
    ///
    void _AddSmvTime2Deque( FrameTimeStamp& stFrameTime, CBaseDetectItem* pCurrentCtrlBlk );

    ///
    /// @brief
    ///    处理采样值，获取极性信息
    ///
    void _PolarityCheck( SMV_VALUE& stSmvValue, CBaseDetectItem* pCurrentCtrlBlk );

    ///
    /// @brief
    ///    对9-1/FT3采样值进行转换为系统设置显示的一次/二次值
    ///
    /// @param[in]  stSmvValueFt3_91  FT3/9-1采样值
    /// @param[out] stSmvValue        保存转换后的数值
    /// @param[in]  enSmvDispType     接收设置中显示值类型
    /// @param[in]  enFrameValueType  基本设置中报文值类型
    /// 
    /// @return
    ///     bool    转换成功返回true
    ///
    bool _SmvValueConvert(const SMV_VALUE_FT3_9_1& stSmvValueFt3_91, SMV_VALUE& stSmvValue, enumSmvValueType enSmvDispType = SMV_VALUE_TYPE_SECONDARY, enumSmvValueType enFrameValueType = SMV_VALUE_TYPE_SECONDARY);

    ///
    /// @brief
    ///    对9-2采样值进行转换为系统设置显示的一次/二次值
    ///
    /// @param[in]  stSmvValue92      9-2采样值
    /// @param[out] stSmvValue        保存转换后的数值
    /// @param[in]  enSmvDispType     接收设置中显示值类型
    /// @param[in]  enFrameValueType  基本设置中报文值类型
    /// 
    /// @return
    ///     bool    转换成功返回true
    ///
    bool _SmvValueConvert(const SMV_VALUE_9_2& stSmvValue92, SMV_VALUE& stSmvValue, enumSmvValueType enSmvDispType = SMV_VALUE_TYPE_SECONDARY, enumSmvValueType enFrameValueType = SMV_VALUE_TYPE_SECONDARY);

    ///
    /// @brief
    ///    判断pRecordItem是否在录波列表中
    ///
    /// @return
    ///     bool 如果存在返回true
    ///
    bool _IsDetectItemInRecordList( CBaseDetectItem* pRecordItem );

    ///
    /// @brief
    ///    Goose,Smv9-1/9-2录波处理
    ///
    void _NetRecordWave( CFrameRecvNet * pNet, DSM_OPERATE_MODE enOpMode, CBaseDetectItem* pCurrentDetct );

    ///
    /// @brief
    ///    FT3录波处理
    ///
    void _Ft3RecordWave( CFrameRecvFt3 * pFt3, DSM_OPERATE_MODE enOpMode, CBaseDetectItem* pCurrentDetct );

    ///
    /// @brief
    ///    判断要当前appid，mac地址对应的数据帧是否在录波的列表内
    ///
    /// @return
    ///     如果当前appid/mac地址在录波列表返回true
    ///
    bool _IsAppidMacInRecordWaveList( uint16_t nAppid, uint8_t*  pMacAddr, uint8_t nLenMac );

    ///
    /// @brief
    ///    判断要当前控制块对应的数据帧是否在录波的列表内
    ///
    /// @return
    ///     如果当前appid/mac地址在录波列表返回true
    ///
    bool _IsAppidMacInRecordWaveList( CBaseDetectItem* pDetectItem );

    ///
    /// @brief
    ///    网络核相处理流程
    ///
    void _NetPhaseCheck( CFrameRecvNet * pNet );

    ///
    /// @brief
    ///    FT3核相处理流程
    ///
    void _Ft3PhaseCheck( CFrameRecvFt3 * pFt3 );

    ///
    /// @brief
    ///    核相数据处理
    ///
    void _PhaseCheckDataHandle( const SMV_VALUE& stSmvValue );

    ///
    /// @brief
    ///    判断某个appid portname是否在过滤列表中
    ///
    bool _IsAppidInNetFilter(uint16_t nAppid, DsmPortName enPortName = DSM_PORT_UNKOWN, CNetDetectItem** pNetItem = NULL);

    ///
    /// @brief
    ///    实时录波时将数据帧内容填入内存缓冲区
    ///
    /// @return
    ///     添加成功返回true
    ///
    bool _WritePckToBuf( PCAP_FILE_TIME_VAL stPcapFileTimeVal, uint8_t * pFrame, uint32_t nFrameLen );

    ///
    /// @brief
    ///    通知界面发生异常情况
    ///
    /// @param[in] nMessageId   通知消息的Id
    /// @param[in] wParam       参数
    /// @param[in] lParam       参数
    /// @param[in] bPostMessage 是否使用PostMessage，true使用PostMessage，false使用SendMessage来发送 消息
    ///
    /// @return
    ///     LRESULT    使用sendMessage返回的结果，如果是postmessage，返回0表示发送失败
    ///
    LRESULT  _NotifyMessage(uint32_t nMessageId, WPARAM wParam = 0, LPARAM lParam = 0, bool bPostMessage = true);

    ///
    /// @brief
    ///    清理实时报文数据
    ///
    void _ClearRealTimeMsg();

    ///
    /// @brief
    ///    解析网络采样值数据帧，产生统计信息
    ///
    void _GenSmvFrameStatistics( CFrameRecvNet * pNet, uint8_t* pFrame, uint32_t nFrameLen, FrameTimeStamp& stFrameTime, CNetDetectItem* pCurrentCtrlBlk );

    ///
    /// @brief
    ///    解析Ft3采样值数据帧，产生统计信息
    ///
    void _GenSmvFrameStatistics( CFrameRecvFt3 * pFt3, uint8_t* pFrame, uint32_t nFrameLen, FrameTimeStamp& stFrameTime, CFt3DetectItem* pCurrentCtrlBlk );

    ///
    /// @brief
    ///    清空采样值统计信息
    ///
    void _ClearSmvFrameStatistics();

    ///
    /// @brief
    ///    解析goose数据帧，产生统计信息
    ///
    void _GenGooseFrameStatistics( CFrameRecvNet * pNet, uint8_t* pFrame, uint32_t nFrameLen, FrameTimeStamp& stFrameTime, CBaseDetectItem* pCurrentCtrlBlk );

    ///
    /// @brief
    ///    清空goose统计信息
    ///
    void _ClearGooseFrameStatistics();

    ///
    /// @brief
    ///    判断核相时两个时间戳是否可以被认为是同一时刻的采样值
    ///    如果两帧的时间间隔在发送帧时间间隔的一半以下，暂时认为是同一时刻的采样值
    ///
    bool _IsPhaseCheckTimeStampAlign(const FrameTimeStamp& stTimeStamp1, const FrameTimeStamp& stTimeStamp2, CBaseDetectItem* pDetectItem);

    ///
    /// @brief
    ///     清空核相缓存的数据
    ///
    void _ClearPhaseCheckData();

    ///
    /// @brief
    ///    清空待发送给dsp的采样值信息
    ///
    void _ClearDspSendSmvBuffer();

    ///
    /// @brief
    ///    设置过滤单个控制块
    ///
    bool _SetOneCtrlBlkFilter(CNetDetectItem* pNetDetectItem);

    ///
    /// @brief
    ///    根据当前采样序号和周期采样点，计算当前点距里nSmpCntStd差多远距离
    ///
    inline uint32_t _GetDesireSmpDistance(uint32_t nSmpCntCurrent, uint32_t nSmpCntStd, uint32_t nPointWave );

    ///
    /// @brief
    ///    清空1588时间信息
    ///
    void _ClearNet1588Time();

    ///
    /// @brief
    ///    判断某个模式是否处于减点模式（12.8k重新采样到4k）
    ///
    bool _IsModeReSampleTo4k(DSM_OPERATE_MODE enMode);

    ///
    /// @brief
    ///    更新Ft3控制块信息
    ///
    void _UpdateFt3DetectItem( CBaseDetectItem* pBaseDetectItem );

    ///
    /// @brief
    ///    清空双AD缓存信息
    ///
    void _ClearDoubleADData();

    ///
    /// @brief
    ///    探测时切换探测端口的定时器处理函数
    ///
    static void CALLBACK _DetectTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);

    ///
    /// @brief
    ///    关闭探测定时器
    ///
    void _CloseDetectTimer();
#if !CHUN_HUA
public:
    uint8_t*                     m_pRecordWaveRealTimeBuf;              ///< 实时录波缓冲区地址
	uint8_t*                     m_pRecordWaveRealTimeBuf2;              ///< 实时录波缓冲区地址2
	
	uint64_t                     m_nSDCardFreeSize;                      ///< SD卡可用空间
	uint64_t                     m_nTotalRecordSize;                      ///< 总共使用了得记录空间
	uint8_t						 m_nCurrentBufferIndex;					///当前使用得Buffer块

	bool						 m_bBuffer1Flag;
	bool						 m_bBuffer2Flag;
	bool 						 m_bIsStopRecord;

	bool						 m_bIsFinishRecord;						///是否已完成写入录波文件

	uint32_t				     m_nCurrentRecordSize;
    uint32_t                     m_nRecordWaveIndex;                    ///< 记录录波缓冲区已使用的长度

	HANDLE						 m_hRecordThread;                       ///< 录波 处理线程句柄

    Lock                         m_lockRecordWaveRealTimeBuf;           ///< 使用共享内存锁
#endif

private:
    HANDLE                       m_hGPMDevice;                          ///< GPMC句柄，用于操作FTPA
    CFrameRecvFt3*               m_pFrameFt3A;                          ///< 光串口A，接收报文
    CFrameRecvFt3*               m_pFrameFt3B;                          ///< 光串口B，接收报文
    CFrameRecvNet*               m_pFrameNetA;                          ///< 光以太网A
    CFrameRecvNet*               m_pFrameNetB;                          ///< 光以太网B
    CFrameRecvNet*               m_pFrameNetC;                          ///< 电以太网C

    CSmvRecvConfigPublic*        m_pSmvRecvConfigPublic;                ///< Smv接收公用配置

    DSM_OPERATE_MODE             m_enCurrentMode;                       ///< 当前模式
    Lock                         m_lockCurrentMode;                     ///< 当前操作模式访问锁

    //探测相关数据
    NetFt3RecvFrameType          m_enCurrentDetectType;                 ///< 当前探测类型
    std::vector <CBaseDetectItem*> m_vecDetectItems;                    ///< 保存当前的探测结果容器
    std::vector <CBaseDetectItem*> m_vecDetectItemsTemp;                ///< 保存当前的探测结果容器,提供给界面层获取结果使用
    Lock                           m_lockDetectItems;                   ///< 访问探测信息互斥锁

    //从扫描列表添加控制块需要控制块详细的数据帧信息
    std::vector <CDetectItemFrame*> m_vecDetectItemsFrame;              ///< 保存探测过程中探测到控制块的一个数据帧
    Lock                            m_lockDetectItemsFrame;             ///< 探测到控制块的一个数据帧访问锁

    //当前选中控制块和当前控制块
    std::vector<CBaseDetectItem*> m_vecSelectedItems;                   ///< 保存当前的选中的控制块
    CBaseDetectItem*              m_pCurrentDetectItems[3];             ///< 保存当前控制块，使用数组是为了在使用核相时，同时指定两个控制块
                                                                        ///< 在进行其他业务时，只使用[2]表示当前控制块

    //当前控制块实时数据的通道数，可能和探测时的通道数不一致
    uint32_t                     m_nRealTimeChannelCounts[2];           ///< 当前控制块实时通道数目在[0],核相时用于保存两个控制块实时通道数
    Lock                         m_lockRealTimeChannelCounts;           ///< 访问当前控制块实时通道数目锁

    //数据层缓存相关数据
#if OSCILLOGRAM_USE_STL_DEQUE
    std::deque<SMV_VALUE>        m_dequeSmvValues;                      ///< 保存采集到的采样值数据,波形图数据需要
    Lock                         m_lockSmvValuesDeque;                  ///< 操作保存所有采样值数据的临界区 m_dequeSmvValues的互斥访问锁
#else
    COscillogramBuf              m_cOscillogramBuf;                     ///< 采样值缓冲区
#endif

    //双AD缓存数据
    SMV_VALUE                    m_stDoubleADData;                      ///< 双ad业务采样值缓存
    Lock                         m_lockDoubleADData;                    ///< 双AD缓存数据

    //控制块状态信息数据
    CSmvCtrlBlkQuality           m_cSmvCtrlBlkQuality;                  ///< 保存最新的控制块通道有效值状态
    Lock                         m_lockCtrlBlkQuality;                  ///< 互斥访问控制块通道有效值信息互斥访问的锁
    CBaseDetectItem*             m_pCachedCtrlBlk;                      ///< 缓存数据的控制块指针
    Lock                         m_lockCacheCtrlBlk;                    ///< 访问缓存数据控制块指针锁

    //Dsp计算相关
    HANDLE                       m_hDspSendProc;                        ///< DSP发送线程句柄
    HANDLE                       m_hEventExitDspSendProc;               ///< 退出dsp线程事件
    HANDLE                       m_hEventSmv;                           ///< 采样值准备好事件，可以通知dsp进行发送
    HANDLE                       m_hEventSmvTime;                       ///< 采样值时间准备好，可以通知dsp进行发送

    CDsp*                        m_pDsp;                                ///< dsp实例指针
    Config                       m_cDspConfig;                          ///< dsp配置信息
    CDspDataSet                  m_cDspDataSet;                         ///< dsp计算返回信息
    SMV_DATA_CALC_RT             m_stSmvDataCalcRt;                     ///< 保留dsp计算返回的实际数据

    SmvData                      m_stDspSmvData;                        ///< 发送给dsp的采样值缓冲区(结构体)
    Lock                         m_lockDspSend;                         ///< 管理dsp发送的锁，由于处理线程和主界面线程（设置当前控制块时）
                                                                         ///< 可能会同时修改dsp发送缓冲区数据

    std::deque <MSGTIME>         m_dequeSmvTime;                        ///< 采样数据的时间戳，需要发送给dsp进行计算离散度计算
    Lock                         m_lockSmvTime;                         ///< 访问修改采样值队列的时间的锁

    //极性监测相关数据
    bool                         m_bPoLarityCheckBegin;                 ///< 极性监测是否开始
    int8_t       m_nArrayPolarity[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];    ///< 缓存存当前控制块极性信息数组，一个通道对应一个元素
    Lock                         m_lockArrayPolarity;                   ///< 访问极性信息锁
    bool         m_bPolaDetected[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];     ///< 是否检测到了极性信息，监测到数据后要继续获取一周波数据来判断是否交流电来确认极性信息有效与否
    bool         m_bPolaDataValid[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];    ///< 极性信息通过一周波后续数据的判断认为是有效的
    float   m_fPolaDataCache[NET_9_2_LE_MAX_CHANNELS_PER_ASDU][SMV_MAX_POINT_PER_WAVE]; ///< 缓存极性判断时，一个通道一个周波的数据
    uint32_t     m_nPolaDataIndex[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];    ///< 当前一个通道保存的采样值序号

    CDataRealTimeMsgData         m_cRealTimeMsg;                        ///< 实时报文信息，保存最新的一帧报文
    Lock                         m_lockRealTimeMsg;                     ///< 访问实时报文信息锁

    MapDetectItemChangeList      m_mapGooseChangeList;                  ///< 保存goose变位信息
    Lock                         m_lockGooseChangeList;                 ///< Goose变位信息访问锁

    //录波相关设置
    uint64_t                     m_nRecordBufferSize;                   ///< 录波使用的内存缓冲区大小
    std::wstring                 m_strRecordWavePath;                   ///< 录波文件路径
    CWnd*                        m_pNotifyWnd;                          ///< 录波过程中出现需要通知界面的事情是发送消息的窗口
    std::vector<CBaseDetectItem*> m_vecRecordWaveList;                  ///< 可以添加多个控制块，对多个控制块进行录波
#if DSM_RECORD_WARE_REALTIME
#if CHUN_HUA    

	uint8_t*                     m_pRecordWaveRealTimeBuf;              ///< 实时录波缓冲区地址
    uint32_t                     m_nRecordWaveIndex;                    ///< 记录录波缓冲区已使用的长度
    Lock                         m_lockRecordWaveRealTimeBuf;           ///< 使用共享内存锁
#endif

#else
    IPcapFile*                   m_pPcapFile;                           ///< pcap文件接口
    Lock                         m_lockPcapFile;                        ///< 使用pcap文件接口锁
#endif

    //核相相关数据
    const uint32_t               m_nPhaseCheckIndexLen;                 ///< 按照索引对齐时，缓存控制块数据个数,定为12800，可以处理4k和12.8k的采样率
    SMV_VALUE*                   m_pPhaseCheckAIndex;                   ///< 按照索引对齐时，缓存第一个控制块数据指针
    SMV_VALUE*                   m_pPhaseCheckBIndex;                   ///< 按照索引对齐时，缓存第二个控制块数据指针
    Lock                         m_lockPhaseCheckData;                  ///< 访问核相缓存数据锁
    uint32_t                     m_nPhaseCheckCurIndex;                 ///< 当前发送的采样序号
    CSampleInterpSync            m_cSmpSyncTime;                        ///< 时标对齐数据处理类
    float*                       m_pResultY;                            ///< 时标对齐保留结果缓冲区指针
    uint32_t                     m_nResultYLen;                         ///< 时标对齐保留结果的数目

    std::map<uint16_t, CSmvRecvConfig*> m_mapAppIDSmvRecv91Config;      ///< 缓存在启动过程中一个9-1 appid对应的通道配置
    std::map<uint16_t, CSmvRecvConfig*> m_mapAppIDSmvRecv92Config;      ///< 缓存在启动过程中一个9-2 appid对应的通道配置
    CSmvRecvConfig*              m_pFt3SmvRecvCofig[2];                 ///< 缓存FT3通道的对应配置，0表示南瑞通道配置，1表示国网通道配置

    //Smv报文异常统计
    MapDetectItemSmvFrameStatistics  m_mapSmvFrameStatistics;           ///< 报文统计过程中，保存报文统计相关信息
    Lock                             m_lockSmvFrameStatistics;          ///< 报文统计信息使用锁

    //Goose报文异常统计
    MapDetectItemGooseFrameStatistics m_mapGooseFrameStatistics;        ///< 报文统计过程中，保存goose统计中间结果
    Lock                              m_lockGooseFrameStatistics;       ///< Goose报文统计信息使用锁

    //探测过程中9-2采样率估算信息
    Lock                              m_lock92SmpRateCalcInfo;          ///< 用于互斥访问9-2采样估算信息
    MapSmpRateCalcInfo                m_map92SmpRateCalcInfo;           ///< 探测过程中9-2采样率估算信息映射表

    //1588时间戳信息
    IEEE1588_TIMESTAMP                m_st1588TimeStamp;                ///< 1588报文/时间戳信息
    Lock                              m_lock1588TimeStamp;              ///< 1588时间访问信息

    //探测过程中定时器定时切换探测端口，防止同时探测多个端口造成的12.8k采样率64通道采样值卡住的问题
    static CFrameRecv*                m_pFrameRecv;                     ///< 为了定时器回调中可以使用接收类中的相关函数变量
    uint32_t                          m_nDetectTimerId;                 ///< 探测定时器id
    uint32_t                          m_nDetectTimerCount;              ///< 进入定时器次数，根据次数判断端口的打开关闭
};
