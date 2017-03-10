/// @file
///
/// @brief
///     数据层对外提供服务类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao   2013.4.2 
///
/// 修改历史：
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
///    SMV、GOOSE发送线程参数
///
struct SendThreadParam
{
    CFrameSend*     m_pFrameSend;                                       ///< 数据发送管理器对象指针
    HWND            m_hWnd;                                             ///< 窗口句柄，当发生重要事件时会发送消息给此窗口
};

class CDsmDataLayer
{
private:
    CDsmDataLayer(void);
    ~CDsmDataLayer(void);

public:
    ///
    /// @brief
    ///    获取数据层应用实例
    ///
    static CDsmDataLayer * getInstance();

    ///
    /// @brief
    ///    初始化数据层需要的数据
    ///
    bool Init();

    ///
    /// @brief
    ///    反初始化，清理数据层的数据
    ///
    bool UnInit();

    ///
    /// @brief
    ///    启动FPGA
    ///
    bool StartFPGA();

    ///
    /// @brief
    ///    关闭FPGA
    ///
    bool StopFPGA();

    ///
    /// @brief
    ///    进入低功耗模式
    ///
    bool EnterLowPowerMode();

    ///
    /// @brief
    ///    退出低功耗模式
    ///
    bool LeaveLowPowerMode();

//
//=============================================================================
// 探测相关接口
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
    ///    获取当前的接收数据类型
    ///
    /// @return
    ///     NetFt3RecvFrameType 返回当前的接收数据类型
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
// 管理器获取接口
//=============================================================================
//
    ///
    /// @brief
    ///    SCL文件管理器获取接口
    ///
    /// @return
    ///     CSCLManager* 返回Scl管理器的指针
    ///
    CSCLManager* GetSclManager();

//
//=============================================================================
// 设置过滤信息
//=============================================================================
//
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
    ///    设置当前的控制块，dsp计算此控制块的信息
    ///
    /// @param[in] pDetectItem  要设置的当前控制块
    ///
    void SetCurrentCtrlBlk(CBaseDetectItem* pDetectItem );

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
    ///    获取第一个控制块信息
    ///
    /// @return
    ///     CBaseDetectItem* 获取第一个控制块指针，没有则返回NULL
    ///
    CBaseDetectItem* GetFirstSelectCtrlBlk();

    ///
    /// @brief
    ///    获取上一个控制块信息
    ///
    /// @param[in]  pCurrentCtrlBlk 指向当前的控制块
    ///
    /// @return
    ///     CBaseDetectItem* 获取上一个控制块指针，没有上一个则返回NULL
    ///
    CBaseDetectItem* GetPrevSelectCtrlBlk(CBaseDetectItem* pCurrentCtrlBlk);

    ///
    /// @brief
    ///    获取下一个控制块信息
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
// 设置核相控制块
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
// 更改模式操作
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
// dsp设置接口
//=============================================================================
//
    ///
    /// @brief
    ///    配置接收数据dsp计算使用的采样率
    ///
    bool SetDspSampleRate(uint32_t nSampleRate);

//
//=============================================================================
// 获取业务数据接口
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
    ///    重置所有的通道极性
    ///
    void ResetAllPorlarity();

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
// 获取Goose接收对应数据
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
// 录波接口，录Goose，NetSmv，Ft3数据
//=============================================================================
//
    ///
    /// @brief
    ///    设置录波文件大小,需要在开始录波前设置，设置
    ///
    /// @return
    ///     bool    设置成功返回true
    ///
    bool SetRecordWaveSize(uint64_t nSizeInByte);

    ///
    /// @brief
    ///    获取设置的录波文件大小
    ///
    uint64_t GetRecordWaveSize();

    ///
    /// @brief
    ///    设置录波文件路径
    ///
    void SetRecordWavePath(const std::wstring& strRecordWavePath);

    ///
    /// @brief
    ///    获取设置的录波文件路径
    ///
    std::wstring GetRecordWavePath();

    ///
    /// @brief
    ///    设置通知界面时使用的窗口CWnd指针
    ///
    bool SetNotifyWnd(CWnd* pWnd);

    ///
    /// @brief
    ///    获取设置的通知界面使用的Cwnd窗口
    ///
    CWnd* GetNotifyWnd();

#if !CHUN_HUA
	///
	/// @brief
	///    设置SD卡大小
	///
	void SetSDCardSize(uint64_t SDCardFreeSize);

#endif

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
    /// @return
    ///      取消成功返回true
    ///
    bool CancelRecordWave();

//
//=============================================================================
// 时间同步接口
//=============================================================================
//
    ///
    /// @brief
    ///    获取B码对时时间信息
    ///
    /// @param[out] pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool    数据获取成功返回true，pData中保存获取到的数据
    ///
    bool GetTimeSyncIRIGB(CDataTimeSyncIRIGB* pData);


//
//=============================================================================
// 配置获取接口
//=============================================================================
//
    ///
    /// @brief
    ///    获取核相时标对齐配置类指针
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
    ///    获取Smv接收公共配置接口
    ///
    CSmvRecvConfigPublic* GetSmvConfigPublic();

    ///
    /// @brief
    ///    获取参数配置/基本参数配置接口
    ///
    CParamConfigBasic* GetParamConfigBasic();

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
    ///    更新缓存的控制块配置信息一次/二次值
    ///
    /// @param[in]  enChannelType 要修改的通道类型，电压通道/电流通道
    /// @param[in]  nPriSectype   要修改的值类型，是一次值（1）还是二次值（2）
    /// @param[in]  nVal          最新的数值
    ///
    void UpdateCachedRatedValues(enumChannelType enChannelType, uint32_t nPriSecType, float fVal);


    ///
    /// @brief
    ///    清空缓存的appid接收配置信息,在重新加载scl文件后调用
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
// 数据发送接口
//=============================================================================
//
    ///
    /// @brief
    ///    开始发送手动试验SMV控制块
    ///
    /// @param[in]  hWnd  窗口句柄，当DI值发生变化时，会发送WM_DI_CHANGED消息给此窗口
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool StartManualTestSMV(HWND hWnd);

    ///
    /// @brief
    ///    更新手动试验SMV发送的数据
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool UpdateManualTestSMV();

    ///
    /// @brief
    ///    停止手动试验SMV发送
    ///
    bool StopManualTestSMV();

    ///
    /// @brief
    ///    开始发送手动试验Goose控制块
    ///
    /// @param[in]  hWnd  窗口句柄，当DI值发生变化时，会发送WM_DI_CHANGED消息给此窗口
    ///
    /// @return
    ///     bool    创建发送线程成功返回true
    ///
    bool StartManualTestGoose( HWND hWnd );

    ///
    /// @brief
    ///    更新手动试验Goose发送的数据
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool UpdateManualTestGoose();

    ///
    /// @brief
    ///    停止手动试验Goose发送
    ///
    bool StopManualTestGoose();

    ///
    /// @brief
    ///    获得手动试验结果集数量
    ///
    /// @return
    ///     unsigned int    手动试验结果集数量
    ///
    unsigned int GetManualTestResultCount();

    ///
    /// @brief
    ///    获得指定索引号的手动试验结果集对象
    ///
    /// @param[in]  nIndex    结果集索引号(从0开始计数)
    ///
    /// @return
    ///     CStateTestResult*    存放指定状态测试结果的对象，失败时返回NULL
    ///
    CStateTestResult* GetManualTestResult(unsigned int nIndex);

    ///
    /// @brief
    ///    获得指定手动试验结果集对象的索引号
    ///
    /// @param[in]  pCurResult    手动试验结果集对象
    ///
    /// @return
    ///     int    结果集索引号，从0开始计数，失败时返回-1
    ///
    int GetManualTestResultIndex(CStateTestResult* pCurResult);

    ///
    /// @brief
    ///    获得下一个手动试验结果集对象
    ///
    /// @param[in]  pCurTestResult    指向当前结果集对象的指针
    ///
    /// @return
    ///     CStateTestResult*    指向下一个结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* NextManualTestResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    获得上一个手动试验结果集对象
    ///
    /// @param[in]  pCurTestResult    指向当前结果集对象的指针
    ///
    /// @return
    ///     CStateTestResult*    指向上一个结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* PrevManualTestResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    获得下一个含有SMV虚拟通道信息的手动试验结果集对象
    ///
    /// @param[in]  pCurTestResult    指向当前结果集对象的指针
    ///
    /// @return
    ///     CStateTestResult*    指向下一个结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* NextManualTestSMVResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    获得上一个含有SMV虚拟通道信息的手动试验结果集对象
    ///
    /// @param[in]  pCurTestResult    指向当前结果集对象的指针
    ///
    /// @return
    ///     CStateTestResult*    指向上一个结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* PrevManualTestSMVResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    获得下一个含有GOOSE控制块的手动试验结果集对象
    ///
    /// @param[in]  pCurTestResult    指向当前结果集对象的指针
    ///
    /// @return
    ///     CStateTestResult*    指向下一个结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* NextManualTestGooseResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    获得上一个含有GOOSE控制块的手动试验结果集对象
    ///
    /// @param[in]  pCurTestResult    指向当前结果集对象的指针
    ///
    /// @return
    ///     CStateTestResult*    指向上一个结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* PrevManualTestGooseResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    开始状态序列测试
    ///
    /// @param[in]  hWnd  窗口句柄，当DI值发生变化时，会发送WM_DI_CHANGED消息给此窗口
    ///                             当需要询问用户是否手动切换时，会发送WM_WHETHER_SWITCH消息给此窗口
    ///                             当状态切换后，会发送WM_STATUS_CHANGED消息给此窗口
    ///                             当测试完成后，会发送WM_TEST_COMPLETED消息给此窗口
    ///
    /// @return
    ///     bool    创建发送线程成功返回true
    ///
    bool StartStateSequenceTest(HWND hWnd);

    ///
    /// @brief
    ///    停止状态序列测试
    ///
    bool StopStateSequenceTest();

    ///
    /// @brief
    ///    获得状态序列结果集数量
    ///
    /// @return
    ///     unsigned int    状态序列结果集数量
    ///
    unsigned int GetStateSequenceResultCount();

    ///
    /// @brief
    ///    获得指定状态号的状态序列结果集对象
    ///
    /// @param[in]  iStateIndex    状态号(从0开始计数)
    ///
    /// @return
    ///     CStateTestResult*    存放指定状态测试结果的对象，失败时返回NULL
    ///
    CStateTestResult* GetStateSequenceResult(unsigned int iStateIndex);

///
///=============================================================================
/// pcap文件统计/分析接口
///=============================================================================
///
    ///
    /// @brief
    ///    设置要分析的pcap文件完整路径
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
    ///    设置通知界面时使用的窗口指针
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
    ///    获取界面设置的通知窗口指针
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
    ///    获取pcap文件中控制块（appid）数目
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
    ///    获取一个控制块Appid
    ///
    /// @return
    ///     获取对应索引的appid，-1表示获取失败
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
    ///    设置当前的控制块
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
    ///    获取当前的控制块appid
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
    ///    获取当前控制块的数据帧类型
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
    ///    获取某个控制块的通道数目
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
    ///    获取一个appid采样值对应采样值/帧在pcap中的映射信息
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
    ///    获取pcap文件中所有帧数
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
    ///    获取pcap中某个appid对应的帧数
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
    ///    获取pcap中某个appid对应的采样点数
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
    ///    获取appid的采样率
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
    ///    获取Appid一帧报文asdu数量
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
    ///    获取pcap文件对应索引数据帧的异常信息
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
    ///     关闭pcap文件
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
    ///    开始pcap文件统计
    ///
    ///
    /// @return
    ///     bool    开始成功返回true，失败false
    ///
    bool StartPcapFileStatisticsAnalysis()
    {
        if (m_pPcapFileAnalysis)
        {
            // 创建子线程，初始状态为暂停
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
    ///    停止pcap文件统计
    ///
    /// @return
    ///     bool    停止成功返回true，失败false
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
    ///    获取pcap文件goose分析结果
    ///
    /// @param[in]  pData 外部申请的空间指针，用于保存pcap统计结果
    ///
    /// @return
    ///     bool    获取成功返回true，失败false
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
    ///    获取pcap文件采样值分析结果，包括离散度信息
    ///
    /// @param[in]  pData 外部申请的空间指针，用于保存pcap统计结果
    ///
    /// @return
    ///     bool    采样值获取成功返回true，失败false
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
    ///    设置需要的波形对应的采样点数据范围
    ///
    /// @param[in]  nIndexBegin 采样点起始索引，从0开始
    /// @param[in]  nIndexEnd   采样点结束索引
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
    ///    开始设置范围内采样点的波形处理
    ///
    /// @return
    ///     bool 返回true表示设置开始分析数据成功
    ///
    bool StartPcapFileWaveAnalysis()
    {
        if (m_pPcapFileAnalysis)
        {
            // 创建子线程，初始状态为暂停
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
    ///    停止pcap文件波形图数据分析
    ///
    /// @return
    ///     bool    停止成功返回true，失败false
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
    ///    获取指定索引范围内波形图/相位/有效值数据
    ///
    /// @param[out]  pData   外部申请的数据对象指针，函数填充指针指向对象的内容
    ///
    /// @return
    ///     bool   数据获取成功返回true，pData中保存获取到的数据
    ///
    /// @note
    ///     pData 指向的内存区域由调用者自己申请
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
// 其他信息获取
//=============================================================================
//
    ///
    /// @brief
    ///    获取驱动版本，返回版本字符串 如 2.34
    ///
    std::wstring GetGPMDriverVersion();

    ///
    /// @brief
    ///    获取FPGA版本，返回版本字符串 如 2.34
    ///
    std::wstring GetGPMFPGAVersion();

    ///
    /// @brief
    ///    获取dsp版本信息
    ///
    uint8_t GetDspVersion();

    ///
    /// @brief
    ///    获取Fpga时间同步方式
    ///
    FpgaTimeSyncMode GetFpgaTimeSyncMode();

    ///
    /// @brief
    ///    设置Fpga时间同步方式
    ///
    /// @param[in]  enMode 要设置的对时模式
    ///
    /// @return
    ///     bool 设置成功返回true
    ///
    bool SetFpgaTimeSyncMode(FpgaTimeSyncMode enMode);

    ///
    /// @brief
    ///    获取FPGA时间
    ///
    /// @param[out] pUtcTime    保存获取到的utc时间
    ///
    /// @return
    ///     bool    获取成功返回true
    ///
    bool GetFpgaUtcTime(UTC_TIME_t* pUtcTime);

    ///
    /// @brief
    ///    设置FPGA时间
    ///
    /// @param[in] pUtcTime    要设置的utc时间
    ///
    /// @return
    ///     bool    设置成功返回true
    ///
    bool SetFpgaUtcTime(UTC_TIME_t* pUtcTime);

    ///
    /// @brief
    ///    判断某个端口的连接状态,针对以太网口有效
    ///
    /// @return
    ///     bool 端口已连接返回true
    ///
    bool IsPortLinked(DsmPortName enPortName);

    ///
    /// @brief
    ///    判断某个端口是否正在收发数据，对所有端口有效
    ///
    /// @return
    ///     bool 端口正在收发数据返回true
    ///
    bool IsPortActive(DsmPortName enPortName);

    ///
    /// @brief
    ///    获取端口整个状态字
    ///
    /// @param[out] nState  获取成功保存各个端口的状态信息
    ///
    /// @return
    ///     bool    获取成功返回true
    ///
    bool GetPortState(uint16_t& nState);

    ///
    /// @brief
    ///    开始发送b码(通过FT3发送端口)
    ///
    /// @return
    ///     bool    发送成功返回true
    ///
    bool StartIRIGBTx();

    ///
    /// @brief
    ///    停止发送b码(通过FT3发送端口)
    ///
    /// @return
    ///     bool    发送成功返回true
    ///
    bool StopIRIGBTx();

    ///
    /// @brief
    ///    设置1588对时时钟信息
    ///
    /// @param[in] 
    ///
    /// @return
    ///     bool    发送成功返回true
    ///
    bool SetIeee1588MainClock(DsmPortName enPortName, const IEEE1588_CLOCK_SYNC_INFO& stIEEE1588ClockInfo);

private:
    ///
    /// @brief
    ///    启动或者停止FPGA
    ///
    bool _StartFPGA(bool bStart);

    ///
    /// @brief
    ///    进入退出低功耗模式
    ///
    bool _EnterLowPowerMode(bool bLowPowerMode);

    ///
    /// @brief
    ///    设置FT3发送端口发送数据类型(Ft3/B码)
    ///
    /// @return
    ///     bool    设置成功返回true
    ///
    bool _SetFt3TxStyle(enumFt3TxStyle enTxStyle);


private:
    CFrameRecv*                         m_pFrameRecv;                   ///< 数据接收管理类
    CFrameSend*                         m_pFrameSend;                   ///< 数据发送管理类

    CSCLManager*                        m_pSclManager;                  ///< SCL文件管理类
    CParamConfigBasic*                  m_pParamConfigBasic;            ///< 参数配置|基本参数配置接口
    CParamConfigSMVSend*                m_pParamConfigSMVSend;          ///< SMV发送配置管理器
    CParamConfigGOOSESend*              m_pParamConfigGOOSESend;        ///< GOOSE发送配置管理器
    CParamConfigGOOSERecv*              m_pParamConfigGOOSERecv;        ///< GOOSE接收配置管理器
    CParamconfigManualTest*             m_pParamConfigManualTest;       ///< 手动试验配置管理器
    CParamConfigStateSequence*          m_pParamConfigStateSequence;    ///< 状态序列配置管理器
    COpticalPowerManager*               m_pOpticalPowerManager;         ///< 光功率管理器

    HANDLE                              m_hGPMDevice;                   ///< FPGA文件句柄，用于操作FPGA接收发送数据

    CPcapAnalysis*                      m_pPcapFileAnalysis;            ///< pcap文件统计分析类
};
