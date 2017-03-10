/// @file
///
/// @brief
///     管理数据帧的接收
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.3.21
///
/// 修改历史：
///

#include "StdAfx.h"
#include "FrameRecv.h"
#include "src\service\datalayer\include\config.h"
#include "src\utils\MessageCodec\include\sav91_msg.h"
#include "src\utils\MessageCodec\include\sav92_msg.h"
#include "src\utils\MessageCodec\include\goose_msg.h"
#include "src\utils\MessageCodec\include\nanrui_ft3_msg.h"
#include "src\utils\MessageCodec\include\guowang_ft3_msg.h"
#include "src\utils\MessageCodec\include\msg_rapid_decoder.h"
#include "src\service\datalayer\config\SmvRecvConfig.h"
#include <algorithm>
#include "src\service\datalayer\DsmDataLayer.h"
#include "src\utils\SmvScaler\SmvScaler.h"
#include "src\service\datalayer\utility\data_utility.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"

//定义编译警告todo信息
//#pragma message(__FILE__ "(" STRING(__LINE__) "): 考虑Uninit时是否要先停止网络接口数据接收")
#define STRING2(x) #x
#define STRING(x) STRING2(x)

//定义这个宏值为1时使能所有端口，为0时只开启第一个光串口和光网口
#define GPM_DRIVER_ENABLE_ALL_PORT 1

//定义核相对齐两个相同采样序号时间差用于判断是否同步临界值(us)
#define PHASE_CHECK_SMP_SYNC_TIME_CRITICAL (700000)

//定义探测切换端口时间
#define DETECT_LOOP_INTERVAL    (1000)

//定义是否打印探测定时器相关日志
#define DETECT_TIMER_LOG        (0)

extern uint16_t smp12800to4000[12800];

SmvChannelTemplate g_SmvChannelTemplate[] = {
    {SMV_RECV_CONFIG_TEMPLATE_TYPE_UNKNOWN,          NULL,                                        0},
    {SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_60044_8,  &PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[0], ARRAY_SIZE(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8)},
    {SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_KAIMO,    &PREDEFINED_CHANNEL_INFO_FT3_STD_KAIMO[0],       ARRAY_SIZE(PREDEFINED_CHANNEL_INFO_FT3_STD_KAIMO)},
    {SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_BODIAN,   &PREDEFINED_CHANNEL_INFO_FT3_STD_BODIAN[0],      ARRAY_SIZE(PREDEFINED_CHANNEL_INFO_FT3_STD_BODIAN)},
    {SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BOOK,     &PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[0],     ARRAY_SIZE(PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG)},
    {SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_KAIMO,    &PREDEFINED_CHANNEL_INFO_FT3_EXT_KAIMO[0],       ARRAY_SIZE(PREDEFINED_CHANNEL_INFO_FT3_EXT_KAIMO)},
    {SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BODIAN,   &PREDEFINED_CHANNEL_INFO_FT3_EXT_BODIAN[0],      ARRAY_SIZE(PREDEFINED_CHANNEL_INFO_FT3_EXT_BODIAN)},
    {SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2,              &PREDEFINED_CHANNEL_INFO_9_2LE[0],               ARRAY_SIZE(PREDEFINED_CHANNEL_INFO_9_2LE)},
    {SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_LESS_CHANNEL, &PREDEFINED_CHANNEL_INFO_9_2LE_LESS[0],          ARRAY_SIZE(PREDEFINED_CHANNEL_INFO_9_2LE_LESS)},
    {SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_KAIMO,        &PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[0],         ARRAY_SIZE(PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO)},
    {SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BODIAN,       &PREDEFINED_CHANNEL_INFO_9_2LE_BODIAN[0],        ARRAY_SIZE(PREDEFINED_CHANNEL_INFO_9_2LE_BODIAN)},
    {SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BOOK,         &PREDEFINED_CHANNEL_INFO_9_2LE_BOOK[0],          ARRAY_SIZE(PREDEFINED_CHANNEL_INFO_9_2LE_BOOK)}
};

CFrameRecv* CFrameRecv::m_pFrameRecv = NULL;
CFrameRecv::CFrameRecv(HANDLE hGPMDevice) : m_hGPMDevice(hGPMDevice)
                                        , m_pFrameFt3A(NULL)
                                        , m_pFrameFt3B(NULL)
                                        , m_pFrameNetA(NULL)
                                        , m_pFrameNetB(NULL)
                                        , m_pFrameNetC(NULL)
                                        , m_enCurrentMode(DSM_MODE_UNKOWN)
                                        , m_enCurrentDetectType(RECV_TYPE_UNKOWN)
                                        , m_pSmvRecvConfigPublic(NULL)
                                        , m_hDspSendProc(NULL)
                                        , m_hEventExitDspSendProc(NULL)
                                        , m_hEventSmv(NULL)
                                        , m_hEventSmvTime(NULL)
                                        , m_pDsp(NULL)
                                        , m_pCachedCtrlBlk(NULL)
                                        , m_nRecordBufferSize(DSM_DEFAULT_RECORD_WAVE_BUFFER_SIZE_IN_BYTE)
                                        , m_strRecordWavePath(L"")
                                        , m_pNotifyWnd(NULL)
                                        , m_pPhaseCheckAIndex(NULL)
                                        , m_pPhaseCheckBIndex(NULL)
                                        , m_nPhaseCheckIndexLen(4000)
                                        , m_nPhaseCheckCurIndex(0)
                                        , m_pResultY(NULL)
                                        , m_nResultYLen(0)
#if DSM_RECORD_WARE_REALTIME
                                        , m_pRecordWaveRealTimeBuf(NULL)

#if !CHUN_HUA
										 , m_pRecordWaveRealTimeBuf2(NULL)
										 , m_nTotalRecordSize(0)
										 , m_nCurrentBufferIndex(0)
										 , m_bBuffer1Flag(false)
										 , m_bBuffer2Flag(false)
										 , m_bIsStopRecord(false)
										 , m_nCurrentRecordSize(0)
										 ,m_bIsFinishRecord(false)

#endif
                                        , m_nRecordWaveIndex(0)
#else
                                        , m_pPcapFile(NULL)
#endif
                                        , m_bPoLarityCheckBegin(false)
                                        , m_nDetectTimerId(0)
                                        , m_nDetectTimerCount(0)
{
    memset(m_pCurrentDetectItems, NULL, sizeof(m_pCurrentDetectItems));
    memset(m_nArrayPolarity, 0, sizeof(m_nArrayPolarity));
    //设置dsp默认参数，与dsp端保持一致，采样率使用4k（dsp端默认使用12.8k）
    m_cDspConfig.nASDUCount = 1;
    m_cDspConfig.nMsgTimeCalcCount = 4680;
    m_cDspConfig.nRatedFreq = 50;
    m_cDspConfig.nSamplingFreq = 4000;

    //极性信息初始化
    memset(m_nArrayPolarity, 0 ,sizeof(m_nArrayPolarity));
    memset(m_bPolaDetected, 0, sizeof(m_bPolaDetected));
    memset(m_bPolaDataValid, 0, sizeof(m_bPolaDataValid));
    memset(m_fPolaDataCache, 0, sizeof(m_fPolaDataCache));
    memset(m_nPolaDataIndex, 0, sizeof(m_nPolaDataIndex));

    //缓存通道配置信息初始化
    memset(m_pFt3SmvRecvCofig, 0, sizeof(m_pFt3SmvRecvCofig));

    m_pFrameRecv = this;
}

CFrameRecv::~CFrameRecv(void)
{
}

///
/// @brief
///    初始化以太网和光串口数据的接收
///
/// @return
///     true 代表初始化成功
///
bool CFrameRecv::InitFrameRecv()
{
    if ( INVALID_HANDLE_VALUE == m_hGPMDevice)
    {
        //驱动句柄为空出错
        RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameRecv::InitFrameRecv(): GPM Device Handle Invalid \n"));
        goto exitCode;
    }

    if (NULL == m_pFrameFt3A)
    {
        m_pFrameFt3A = new CFrameRecvFt3(this, m_hGPMDevice, FT3_PORT_LIGHT_A);
        if (NULL == m_pFrameFt3A)
        {
            // 创建Ft3光口实例失败
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameRecv::InitFrameRecv(): Create Ft3 Recv on Port A Failed \n"));
            goto exitCode;
        }
        m_pFrameFt3A->InitFt3(FT3_RECV_NOTIFY_EVENT_NAME_A.c_str(), FT3_RECV_MAP_FILE_NAME_A.c_str(), FT3_RECV_MAP_FILE_LEN);
    }
#if GPM_DRIVER_ENABLE_ALL_PORT
    if (NULL == m_pFrameFt3B)
    {
        m_pFrameFt3B = new CFrameRecvFt3(this, m_hGPMDevice, FT3_PORT_LIGHT_B);
        if (NULL == m_pFrameFt3B)
        {
            // 创建Ft3光口实例失败
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameRecv::InitFrameRecv(): Create Ft3 Recv on Port B Failed \n"));
            goto exitCode;
        }
        m_pFrameFt3B->InitFt3(FT3_RECV_NOTIFY_EVENT_NAME_B.c_str(), FT3_RECV_MAP_FILE_NAME_B.c_str(), FT3_RECV_MAP_FILE_LEN);
    }
#endif

    if (NULL == m_pFrameNetA)
    {
        m_pFrameNetA = new CFrameRecvNet(this, m_hGPMDevice, NET_PORT_LIGHT_A);
        if (NULL == m_pFrameNetA)
        {
            // 创建网口实例失败
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameRecv::InitFrameRecv(): Create NET Recv on Port A Failed \n"));
            goto exitCode;
        }
        m_pFrameNetA->InitNet(NET_RECV_NOTIFY_EVENT_NAME_A.c_str(), NET_RECV_MAP_FILE_NAME_A.c_str(), NET_RECV_MAP_FILE_LEN);
    }

#if GPM_DRIVER_ENABLE_ALL_PORT
    if (NULL == m_pFrameNetB)
    {
        m_pFrameNetB = new CFrameRecvNet(this, m_hGPMDevice, NET_PORT_LIGHT_B);
        if (NULL == m_pFrameNetB)
        {
            // 创建网口实例失败
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameRecv::InitFrameRecv(): Create NET Recv on Port B Failed \n"));
            goto exitCode;
        }
        m_pFrameNetB->InitNet(NET_RECV_NOTIFY_EVENT_NAME_B.c_str(), NET_RECV_MAP_FILE_NAME_B.c_str(), NET_RECV_MAP_FILE_LEN);
    }
    if (NULL == m_pFrameNetC)
    {
        m_pFrameNetC = new CFrameRecvNet(this, m_hGPMDevice, NET_PORT_ELECTRONIC);
        if (NULL == m_pFrameNetC)
        {
            // 创建网口实例失败
            RETAILMSG(OUTPUT_LOG_MSG, (L"CFrameRecv::InitFrameRecv(): Create NET Recv on Port C Failed \n"));
            goto exitCode;
        }
        m_pFrameNetC->InitNet(NET_RECV_NOTIFY_EVENT_NAME_C.c_str(), NET_RECV_MAP_FILE_NAME_C.c_str(), NET_RECV_MAP_FILE_LEN);
    }
#endif

    if (NULL == m_pSmvRecvConfigPublic)
    {
        m_pSmvRecvConfigPublic = CSmvRecvConfigPublic::getInstance();
        m_pSmvRecvConfigPublic->LoadConfigFile(SMV_RECV_CONFIG_PUBLIC);
    }


    //初始化dsp发送等待事件
    _InitDspSendEvents();

    //开启dsp发送线程
    StartDspSendProc();

    //设置核相时标对齐默认参数信息
    m_cSmpSyncTime.SetInterpType(INTERP_TYPE_LINEAR);
    m_cSmpSyncTime.SetDelayCompensateEnable(false);

    return true;

exitCode:
    UnInitFrameRecv();
    return false;
}

///
/// @brief
///    停止以太网和光串口数据接收
///
void CFrameRecv::UnInitFrameRecv()
{
    StopRecvData();

    //清空容器数据
    _ClearAllContainer();

    //清空保留的控制块对应的通道配置信息
    ClearAppidSmvRecvCache();

    //清空核相缓存数据
    _ClearPhaseCheckData();

    //停止dsp发送线程
    StopDspSendProc();

    //反初始化dsp发送等待事件
    _UninitDspSendEvents();

    if (NULL != m_pFrameFt3A)
    {
        m_pFrameFt3A->UnInitFt3();
        delete m_pFrameFt3A;
        m_pFrameFt3A = NULL;
    }
#if GPM_DRIVER_ENABLE_ALL_PORT
    if (NULL != m_pFrameFt3B)
    {
        m_pFrameFt3B->UnInitFt3();
        delete m_pFrameFt3B;
        m_pFrameFt3B = NULL;
    }
#endif
    if (NULL != m_pFrameNetA)
    {
        m_pFrameNetA->UnInitNet();
        delete m_pFrameNetA;
        m_pFrameNetA = NULL;
    }

#if GPM_DRIVER_ENABLE_ALL_PORT
    if (NULL != m_pFrameNetB)
    {
        m_pFrameNetB->UnInitNet();
        delete m_pFrameNetB;
        m_pFrameNetB = NULL;
    }
    if (NULL != m_pFrameNetC)
    {
        m_pFrameNetC->UnInitNet();
        delete m_pFrameNetC;
        m_pFrameNetC = NULL;
    }
#endif

}

///
/// @brief
///    Ft3接收到数据的处理程序
///
void CFrameRecv::_Ft3DataHandler(PVOID pArg)
{
    CFrameRecvFt3 *pFt3 = reinterpret_cast<CFrameRecvFt3*>(pArg);
#if 0
    RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_Ft3DataHandler() Begain Data Process On Ft3 Port %d\n"), pFt3->m_enPortName));
#endif
    if (!pFt3->m_bRecv)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T(" CFrameRecv::_Ft3DataHandler() : Ft3 Port %d is Not accept data \n"), pFt3->m_enPortName));
        return;
    }

    DSM_OPERATE_MODE enOpMode = GetCurrentMode();
    if (DSM_MODE_QUIT_CURRENT_PAGE == enOpMode || DSM_MODE_MAIN_PAGE == enOpMode)
    {
        return;
    }
    else if (DSM_MODE_DETECT == enOpMode)
    {
        _GenFt3DetectInfo(pFt3);
        return;
    }

    CFt3DetectItem* pCurrentDetct = dynamic_cast<CFt3DetectItem*>(GetCurrentCtrlBlk());
    if (NULL == pCurrentDetct)
    {
        return;
    }
    _SetCachedCtrlBlk(pCurrentDetct);
    switch(enOpMode)
    {
    case DSM_MODE_SMV_RECV_OSCILLOGRAM:
    case DSM_MODE_SMV_RECV_EFFECTIVE_VALUE:
    case DSM_MODE_SMV_RECV_STATUS_WATCH:
    case DSM_MODE_SMV_RECV_PHASOR:
    case DSM_MODE_SMV_RECV_XU_LIANG:
    case DSM_MODE_SMV_RECV_POWER:
    case DSM_MODE_SMV_RECV_HARMONIC:
    case DSM_MODE_SMV_RECV_DOUBLE_AD:
    case DSM_MODE_SMV_RECV_POLARITY:
    case DSM_MODE_SMV_RECV_REAL_TIME_MSG:
    case DSM_MODE_SMV_RECV_DISPERSION:
    case DSM_MODE_SMV_RECV_FRAME_STATIC:
        if (pFt3->m_enPortName == pCurrentDetct->enPortName)
        {
            _Ft3SmvAnaly(pFt3, enOpMode, pCurrentDetct);
        }
        break;
    case DSM_MODE_SMV_RECV_FRAME_MONITOR:
        break;
    case DSM_MODE_FT3_RECV_RECORD_WAVE:
        _Ft3RecordWave(pFt3, enOpMode, pCurrentDetct);
        break;
    case DSM_MODE_SMV_RECV_PHASE_CHECK:
        if ((m_pCurrentDetectItems[0] && pFt3->m_enPortName == m_pCurrentDetectItems[0]->enPortName)
          ||(m_pCurrentDetectItems[1] && pFt3->m_enPortName == m_pCurrentDetectItems[1]->enPortName))
        {
            _Ft3PhaseCheck(pFt3);
        }
        break;
    default:
        break;
    }
}

///
/// @brief
///    网络接口接收到数据的处理程序
///    根据 m_bEnableDetect 变量判断是否要获取探测结果
///
void CFrameRecv::_NetDataHandler(PVOID pArg)
{
    CFrameRecvNet *pNet = static_cast<CFrameRecvNet*>(pArg);
#if 0
    RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_NetDataHandler() Begain Data Process On Net Port %d\n"), pNet->m_enPortName));
#endif
    if (!pNet->m_bRecv)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T(" CFrameRecv::_NetDataHandler() : Net Port %d is Not accept data \n"), pNet->m_enPortName));
        return;
    }

    DSM_OPERATE_MODE enOpMode = GetCurrentMode();
    if (DSM_MODE_QUIT_CURRENT_PAGE == enOpMode || DSM_MODE_MAIN_PAGE == enOpMode)
    {
        return;
    }
    else if (DSM_MODE_DETECT == enOpMode)
    {
        _GenNetDetectInfo(pNet);
        return;
    }
    else if (DSM_MODE_GOOSE_RECV_DI == enOpMode)                        ///< 保存手动试验、状态序列的DI
    {
        // 保存手动试验、状态序列测试时GOOSE的DI值
        _NetSaveGooseDI(pNet);
        return;
    }

    CNetDetectItem* pCurrentDetct = dynamic_cast<CNetDetectItem*>(GetCurrentCtrlBlk());
    if (NULL == pCurrentDetct)
    {
        return;
    }
    _SetCachedCtrlBlk(pCurrentDetct);
    switch(enOpMode)
    {
    case DSM_MODE_SMV_RECV_OSCILLOGRAM:
    case DSM_MODE_SMV_RECV_EFFECTIVE_VALUE:
    case DSM_MODE_SMV_RECV_STATUS_WATCH:
    case DSM_MODE_SMV_RECV_PHASOR:
    case DSM_MODE_SMV_RECV_XU_LIANG:
    case DSM_MODE_SMV_RECV_POWER:
    case DSM_MODE_SMV_RECV_HARMONIC:
    case DSM_MODE_SMV_RECV_DOUBLE_AD:
    case DSM_MODE_SMV_RECV_DISPERSION:
    case DSM_MODE_SMV_RECV_FRAME_STATIC:
    case DSM_MODE_SMV_RECV_REAL_TIME_MSG:
        if (pNet->m_enPortName == pCurrentDetct->enPortName)
        {
            _NetSmvAnaly(pNet, enOpMode, pCurrentDetct);
        }
        break;
    case DSM_MODE_SMV_RECV_POLARITY:
        if (m_bPoLarityCheckBegin && pNet->m_enPortName == pCurrentDetct->enPortName)
        {
            _NetSmvAnaly(pNet, enOpMode, pCurrentDetct);
        }
        break;
        //报文监测页面，什么也不做
    case DSM_MODE_SMV_RECV_FRAME_MONITOR:
        break;
    case DSM_MODE_SMV_RECV_RECORD_WAVE:
        _NetRecordWave(pNet, enOpMode, pCurrentDetct);
        break;
    case DSM_MODE_SMV_RECV_PHASE_CHECK:
        if (_IsPortInCurrentCtrlBlk(pNet->m_enPortName))
        {
            _NetPhaseCheck(pNet);
        }
        break;
    case DSM_MODE_GOOSE_RECV_REALTIME_VAL:
    case DSM_MODE_GOOSE_RECV_CHANGE_LIST:
    case DSM_MODE_GOOSE_RECV_REAL_TIME_MSG:
    case DSM_MODE_GOOSE_RECV_FRAME_STATIC:
         _NetGooseAnaly(pNet, enOpMode, pCurrentDetct);
        break;
        //报文监测页面，什么也不做
    case DSM_MODE_GOOSE_RECV_FRAME_MONITOR:
        break;
    case DSM_MODE_GOOSE_RECV_RECORD_WAVE:
        _NetRecordWave(pNet, enOpMode, pCurrentDetct);
        break;
    case DSM_MODE_NET_RECV_1588:
        if (pNet->m_enPortName == pCurrentDetct->enPortName)
        {
            _Net1588Analy(pNet, enOpMode, pCurrentDetct);
        }
        break;
    default:
        break;
    }
}

///
/// @brief
///    告诉数据层，要获取探测结果
///    数据层要去掉底层的过滤，获取网络接口的所有报文
///
void CFrameRecv::BeginDetect(NetFt3RecvFrameType enNetRecvFrameType, bool bEnabelDetect )
{
    //探测前先停止以前所有数据接收
    StopRecvData();

    if (bEnabelDetect)
    {
        SetRxThreadPriority(DATA_RECV_DETECT_THREAD_PRIO);
        m_enCurrentDetectType = enNetRecvFrameType;
        SetCurrentMode(DSM_MODE_DETECT);

        //清除以前探测结果
        _ClearDetectItems();
        //清除所有缓存信息
        _ClearCachedData();
        //清除goose变位列表信息
        _ClearGooseChangeList();
        //清除异常统计信息
        _ClearSmvFrameStatistics();
        _ClearGooseFrameStatistics();
        //清除核相缓存数据
        _ClearPhaseCheckData();

        //开启定时器定时探测，开启失败则开启所有端口探测
        _CloseDetectTimer();
        m_nDetectTimerId = SetTimer(NULL, 0, DETECT_LOOP_INTERVAL, &CFrameRecv::_DetectTimerProc);
        if (m_nDetectTimerId)
        {
            RETAILMSG(DETECT_TIMER_LOG, (_T("DetectTimer(%u) Created\r\n"), m_nDetectTimerId));
        }
        else
        {
            //开始探测，以获取到网络上所有报文,同时探测多个端口
            _Detect(enNetRecvFrameType, NET_DETECT_RECV_FRAMES, NET_DEFAULT_RECV_ENABLETIMEOUT, NET_DEFAULT_RECV_TIMEOUT);
        }
    }
}

///
/// @brief
///    从m_vecSelectedItems中获取信息，设置到各个端口进行过滤
///
bool CFrameRecv::_SetNetFilter()
{
//开启过滤，需要配合使用fpga0.18版去 ，驱动版本0.87
#if 1
    if (m_enCurrentDetectType == NET_RECV_TYPE_GOOSE || m_enCurrentDetectType == NET_RECV_TYPE_SMV)
    {
        //如果是网络采样值或者goose，要设置过滤信息
        uint32_t nMaxFilterCount = DSM_MAX_SMV_CTRL_BLK;
        if (NET_RECV_TYPE_GOOSE == m_enCurrentDetectType)
        {
            nMaxFilterCount = DSM_MAX_GOOSE_CTRL_BLK;
        }
        else if ( RECV_TYPE_UNKOWN == m_enCurrentDetectType
            || NET_RECV_TYPE_IEEE_1588 == m_enCurrentDetectType )
        {
            return true;
        }

        //每个端口上选择的控制块数量
        uint32_t nNetPortA = 0;
        uint32_t nNetPortB = 0;
        uint32_t nNetPortElec = 0;

        //每个端口上所有控制块中一次接收数量的最大值
        //一次接收数据的数目应该包含10个周波数据，因此采样点个数为
        // （采样率/频率）* 10 ，对应的数据帧个数为上面结果除以一个帧Asdu数量
        //一次接收数据帧的数量最小设置 为 200，最多为5000.
        //一个端口为了兼顾多个控制块数据，因此采用上述公式结果的最大值
        uint32_t nPortACount = DSM_CONFIG_RECV_SAMPLE_COUNT_MIN;
        uint32_t nPortBCount = DSM_CONFIG_RECV_SAMPLE_COUNT_MIN;
        uint32_t nPortCCount = DSM_CONFIG_RECV_SAMPLE_COUNT_MIN;
        uint32_t nMaxFrameCount = NET_MAX_RECV_FRAMES/2;

        //过滤时传递参数结构
        //过滤与探测两个命令的主要区别就是需要获取MAC地址，调用IOCTRL_FILTER命令
        //获取过滤地址,地址传递参数放置在 tInMuParam.uInBuf[4]
        IOCTRL_IN_PARAM_NET_BIG stIoCtrlParamNetA, stIoCtrlParamNetB, stIoCtrlParamNetElec;
        FpgaNetFilterItem* pNetFilter = NULL;
        std::vector<CBaseDetectItem*>::iterator itr = m_vecSelectedItems.begin();
        while (itr != m_vecSelectedItems.end())
        {
            enumFrameType enFrameType = (*itr)->enFrameType;
            if ( FRAME_TYPE_NET_9_1 == enFrameType
                || FRAME_TYPE_NET_9_2 == enFrameType
                || FRAME_TYPE_NET_GOOSE == enFrameType )
            {
                if (nNetPortA + nNetPortB + nNetPortElec >= nMaxFilterCount)
                {
                    break;
                }
                CNetDetectItem* pNetDetectItem = dynamic_cast<CNetDetectItem*>((*itr));
                if (pNetDetectItem)
                {
                    switch(pNetDetectItem->enPortName)
                    {
                    case NET_PORT_LIGHT_A:
                        {
                            pNetFilter = reinterpret_cast<FpgaNetFilterItem*>(&stIoCtrlParamNetA.nInBuf[4]);
                            pNetFilter = pNetFilter + nNetPortA;
                            memcpy_s(pNetFilter->macAddr, ETHERNET_MAC_LEN, pNetDetectItem->macAddr, ETHERNET_MAC_LEN);
                            uint8_t* pAppidAddr = (uint8_t *)&(pNetDetectItem->appId);
                            pNetFilter->appId[0] = *(pAppidAddr + 1);
                            pNetFilter->appId[1] = *pAppidAddr;
                            nNetPortA++;
                            if (enFrameType != FRAME_TYPE_NET_GOOSE)
                            {
                                uint32_t nFrames = (pNetDetectItem->nSampleRate * DSM_CONFIG_DEFAULT_RECV_WAVE_COUNT)/(DSM_CONFIG_DEFAULT_ELEC_FREQUENCY * pNetDetectItem->nAsdu);
                                if (nFrames > nPortACount)
                                {
                                    nPortACount = nFrames;
                                }
                            }
                        }
                        break;
                    case NET_PORT_LIGHT_B:
                        {
                            pNetFilter = reinterpret_cast<FpgaNetFilterItem*>(&stIoCtrlParamNetB.nInBuf[4]);
                            pNetFilter = pNetFilter + nNetPortB;
                            memcpy_s(pNetFilter->macAddr, ETHERNET_MAC_LEN, pNetDetectItem->macAddr, ETHERNET_MAC_LEN);
                            uint8_t* pAppidAddr = (uint8_t *)&(pNetDetectItem->appId);
                            pNetFilter->appId[0] = *(pAppidAddr + 1);
                            pNetFilter->appId[1] = *pAppidAddr;
                            nNetPortB++;
                            if (enFrameType != FRAME_TYPE_NET_GOOSE)
                            {
                                uint32_t nFrames = (pNetDetectItem->nSampleRate * DSM_CONFIG_DEFAULT_RECV_WAVE_COUNT)/(DSM_CONFIG_DEFAULT_ELEC_FREQUENCY * pNetDetectItem->nAsdu);
                                if (nFrames > nPortBCount)
                                {
                                    nPortBCount = nFrames;
                                }
                            }
                        }
                        break;
                    case NET_PORT_ELECTRONIC:
                        {
                            pNetFilter = reinterpret_cast<FpgaNetFilterItem*>(&stIoCtrlParamNetElec.nInBuf[4]);
                            pNetFilter = pNetFilter + nNetPortElec;
                            memcpy_s(pNetFilter->macAddr, ETHERNET_MAC_LEN, pNetDetectItem->macAddr, ETHERNET_MAC_LEN);
                            uint8_t* pAppidAddr = (uint8_t *)&(pNetDetectItem->appId);
                            pNetFilter->appId[0] = *(pAppidAddr + 1);
                            pNetFilter->appId[1] = *pAppidAddr;
                            nNetPortElec++;
                            if (enFrameType != FRAME_TYPE_NET_GOOSE)
                            {
                                uint32_t nFrames = (pNetDetectItem->nSampleRate * DSM_CONFIG_DEFAULT_RECV_WAVE_COUNT)/(DSM_CONFIG_DEFAULT_ELEC_FREQUENCY * pNetDetectItem->nAsdu);
                                if (nFrames > nPortCCount)
                                {
                                    nPortCCount = nFrames;
                                }
                            }
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            ++itr;
        }

        //设置过滤数据类型
        stIoCtrlParamNetA.nInBuf[0] = static_cast<uint32_t>(m_enCurrentDetectType);
        stIoCtrlParamNetB.nInBuf[0] = static_cast<uint32_t>(m_enCurrentDetectType);
        stIoCtrlParamNetElec.nInBuf[0] = static_cast<uint32_t>(m_enCurrentDetectType);

        //设置过滤的mac数量
        stIoCtrlParamNetA.nInBuf[3] = nNetPortA;
        stIoCtrlParamNetB.nInBuf[3] = nNetPortB;
        stIoCtrlParamNetElec.nInBuf[3] = nNetPortElec;

        uint32_t nPortBufferCnt = 0;

        //1.设置过滤
        //2.设置超时
        //3.接收线程可以处理数据
        //3.开启接收
        if (m_pFrameNetA)
        {
            if (nNetPortA > 0)
            {
                m_pFrameNetA->SetNetFilter(stIoCtrlParamNetA);
                m_pFrameNetA->SetRecvTimeOut(NET_DEFAULT_RECV_TIMEOUT);
                m_pFrameNetA->StartNetRecv();
                nPortBufferCnt = nPortACount * nNetPortA;
                nPortBufferCnt = nPortBufferCnt > nMaxFrameCount ? nMaxFrameCount : nPortBufferCnt;
                m_pFrameNetA->StartFpgaRecvLoop(nPortBufferCnt , NET_DEFAULT_RECV_ENABLETIMEOUT);
                RETAILMSG(1, (_T("Port A Buffer : %d \r\n"), nPortBufferCnt));
            }
            else
            {
                StopPortFrameRecv(NET_PORT_LIGHT_A);
            }
        }
#if GPM_DRIVER_ENABLE_ALL_PORT
        if (m_pFrameNetB)
        {
            if (nNetPortB > 0)
            {
                m_pFrameNetB->SetNetFilter(stIoCtrlParamNetB);
                m_pFrameNetB->SetRecvTimeOut(NET_DEFAULT_RECV_TIMEOUT);
                m_pFrameNetB->StartNetRecv();
                nPortBufferCnt = nPortBCount * nNetPortB;
                nPortBufferCnt = nPortBufferCnt > nMaxFrameCount ? nMaxFrameCount : nPortBufferCnt;
                m_pFrameNetB->StartFpgaRecvLoop(nPortBufferCnt, NET_DEFAULT_RECV_ENABLETIMEOUT);
                RETAILMSG(1, (_T("Port B Buffer : %d \r\n"), nPortBufferCnt));
            }
            else
            {
                StopPortFrameRecv(NET_PORT_LIGHT_B);
            }
        }
        if (m_pFrameNetC)
        {
            if (nNetPortElec > 0)
            {
                m_pFrameNetC->SetNetFilter(stIoCtrlParamNetElec);
                m_pFrameNetC->SetRecvTimeOut(NET_DEFAULT_RECV_TIMEOUT);
                m_pFrameNetC->StartNetRecv();
                nPortBufferCnt = nPortCCount * nNetPortElec;
                nPortBufferCnt = nPortBufferCnt > nMaxFrameCount ? nMaxFrameCount : nPortBufferCnt;
                m_pFrameNetC->StartFpgaRecvLoop(nPortBufferCnt, NET_DEFAULT_RECV_ENABLETIMEOUT);
                RETAILMSG(1, (_T("Port C Buffer : %d \r\n"), nPortBufferCnt));
            }
            else
            {
                StopPortFrameRecv(NET_PORT_ELECTRONIC);
            }
        }
#endif //GPM_DRIVER_ENABLE_ALL_PORT
    }
    else if(m_enCurrentDetectType == FT3_RECV_TYPE_SMV)
    {
        //如果是ft3探测，接收ft3数据
        if (m_pFrameFt3A)
        {
            m_pFrameFt3A->StartFt3Recv();
        }
#if GPM_DRIVER_ENABLE_ALL_PORT
        if (m_pFrameFt3B)
        {
            m_pFrameFt3B->StartFt3Recv();
        }
#endif //GPM_DRIVER_ENABLE_ALL_PORT
    }
    else if (m_enCurrentDetectType == NET_RECV_TYPE_IEEE_1588)
    {
        //如果是1588探测，则继续接收数据就可以了
        _Detect(m_enCurrentDetectType, NET_DETECT_RECV_FRAMES);
    }

    //设置当前模式
    SetCurrentMode(DSM_MODE_FILTER);
#else  // #if 1
    _Detect(m_enCurrentDetectType, (m_vecSelectedItems.size() > 1 ? NET_MUL_CTRL_BLKS_RECV_FRAMES : NET_DEFAULT_RECV_FRAMES));
#endif // #if 1

    return true;
}


///
/// @brief
///    开始某个端口数据帧接收
///
bool CFrameRecv::StartPortFrameRecv(DsmPortName enPortName)
{
    switch(enPortName)
    {
    case NET_PORT_LIGHT_A:
        if ( (NULL == m_pFrameNetA) || !(m_pFrameNetA->StartNetRecv()) )
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::StartFrameRecv() : m_pFrameNetA->StartFt3Recv() Failed \n")));
            return false;
        }
        break;

#if GPM_DRIVER_ENABLE_ALL_PORT
    case  NET_PORT_LIGHT_B:
        if ( (NULL == m_pFrameNetB) || !(m_pFrameNetB->StartNetRecv()) )
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::StartFrameRecv() : m_pFrameNetB->StartFt3Recv() Failed \n")));
            return false;
        }
        break;
    case  NET_PORT_ELECTRONIC:
        if ( (NULL == m_pFrameNetC) || !(m_pFrameNetC->StartNetRecv()) ) 
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::StartFrameRecv() : m_pFrameNetC->StartFt3Recv() Failed \n")));
            return false;
        }
        break;
#endif
    case FT3_PORT_LIGHT_A:
        if ( (NULL == m_pFrameFt3A) || !(m_pFrameFt3A->StartFt3Recv()) )
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::StartFrameRecv() : m_pFrameFt3A->StartFt3Recv() Failed \n")));
            return false;
        }
        break;
#if GPM_DRIVER_ENABLE_ALL_PORT
    case  FT3_PORT_LIGHT_B:
        if ( (NULL == m_pFrameFt3B) || !(m_pFrameFt3B->StartFt3Recv()) )
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::StartFrameRecv() : m_pFrameFt3B->StartFt3Recv() Failed \n")));
            return false;
        }
        break;
#endif
    default:
        return false;
        break;
    }
    return true;
}

///
/// @brief
///    停止数据帧接收
///
bool CFrameRecv::StopPortFrameRecv(DsmPortName enPortName)
{
    switch(enPortName)
    {
    case NET_PORT_LIGHT_A:
        if ( (NULL == m_pFrameNetA) || !(m_pFrameNetA->StopNetRecv()) )
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::StopFrameRecv() : m_pFrameNetA->StopNetRecv() Failed \n")));
            return false;
        }
        break;

#if GPM_DRIVER_ENABLE_ALL_PORT
    case  NET_PORT_LIGHT_B:
        if ( (NULL == m_pFrameNetB) || !(m_pFrameNetB->StopNetRecv()) )
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::StopFrameRecv() : m_pFrameNetB->StopNetRecv() Failed \n")));
            return false;
        }
        break;
    case  NET_PORT_ELECTRONIC:
        if ( (NULL == m_pFrameNetC) || !(m_pFrameNetC->StopNetRecv()) )
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::StopFrameRecv() : m_pFrameNetC->StopNetRecv() Failed \n")));
            return false;
        }
        break;
#endif
    case FT3_PORT_LIGHT_A:
        if ( (NULL == m_pFrameFt3A) || !(m_pFrameFt3A->StopFt3Recv()) )
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::StopFrameRecv() : m_pFrameFt3A->StopFt3Recv() Failed \n")));
            return false;
        }
        break;
#if GPM_DRIVER_ENABLE_ALL_PORT
    case  FT3_PORT_LIGHT_B:
        if ( (NULL == m_pFrameFt3B) || !(m_pFrameFt3B->StopFt3Recv()) )
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::StopFrameRecv() : m_pFrameFt3B->StopFt3Recv() Failed \n")));
            return false;
        }
        break;
#endif
    default:
        return false;
        break;
    }
    return true;
}

///
/// @brief
///    获取Ft3接口探测结果,数据填充进入m_setFt3DetectInfo
///
///
void CFrameRecv::_GenFt3DetectInfo( CFrameRecvFt3 * pFt3 )
{
    if (NULL == pFt3)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_GenFt3DetectInfo(): INPUT arg pFt3 Null \n")));
        return;
    }

#if GEN_LOG_MESSAGE
    RETAILMSG(OUTPUT_LOG_MSG, (_T("Ft3 Detect ...\r\n")));
#endif
    BufferHeader* pBufferHeader = reinterpret_cast<BufferHeader*>(pFt3->m_pDataBuf);
    //给缓冲区加锁，在处理缓冲区时防止驱动覆盖缓冲区信息
    pFt3->LockBuffer();
    uint32_t nFrameLen = CFt3RecvBuffer::GetBufferAFrameLen(pBufferHeader);
    uint32_t nFrameCount = pBufferHeader->Ft3Header.nFramesCount;
    uint32_t BufferLen = pBufferHeader->Ft3Header.nActiveLength;
    uint32_t nLoopItem = pBufferHeader->Ft3Header.nLoopItem;
    uint8_t* pBufferData = NULL;
    if (0 == nLoopItem)
    {
        pBufferData = pFt3->m_pDataBuf + sizeof(BufferHeader);
    }
    else if (1 == nLoopItem)
    {
        pBufferData = pFt3->m_pDataBuf + sizeof(BufferHeader) + FT3_RECV_LOOP_BUF1_POS;
    }

    uint8_t* pAllocRecvBuffer = NULL;
#if DRIVER_BUFFER_USE_CPY
    pAllocRecvBuffer = new uint8_t[BufferLen];
    memcpy_s(pAllocRecvBuffer, BufferLen, pBufferData, BufferLen);
    CFt3RecvBuffer recvBuffer(pAllocRecvBuffer, nFrameCount, BufferLen);
#else
    CFt3RecvBuffer recvBuffer(pBufferData, nFrameCount, BufferLen);
#endif
    recvBuffer.SetBufferHeader(*pBufferHeader);

    enumFrameType enFrameType = recvBuffer.GetBufferFrameType();
    uint8_t *pFrame = NULL;
    MsgRapidDecoder& msgRapidDecoder = pFt3->m_cMsgRapidDecoder;

    uint16_t nLdName = 0;
    uint32_t nChannelCount = 0;
    uint16_t nAsdu = 1;
    FT3_BAUD_RATE enFt3BaudRate = recvBuffer.GetBufferFrameBaudRate();
    uint32_t nFt3SampleRate = recvBuffer.GetBufferFrameSampleRate();
    for ( uint32_t i=0; i < nFrameCount; ++i)
    {
        if (recvBuffer.GetBufferFrameAddr(i, &pFrame, nFrameLen))
        {
            msgRapidDecoder.GetChannelNum(pFrame, nFrameLen, enFrameType, nChannelCount);
            msgRapidDecoder.GetLDName(pFrame, nFrameLen, enFrameType, nLdName);
            msgRapidDecoder.GetNoASDU(pFrame, nFrameLen, enFrameType, nAsdu);

            CFt3DetectItem* pFt3DetectInfo = new CFt3DetectItem;
            pFt3DetectInfo->enFrameType = enFrameType;
            pFt3DetectInfo->enPortName = pFt3->m_enPortName;
            pFt3DetectInfo->enBaudRate = enFt3BaudRate;
            pFt3DetectInfo->nSampleRate = nFt3SampleRate;
            pFt3DetectInfo->nSampleRateDetect = nFt3SampleRate;
            pFt3DetectInfo->nLDName = nLdName;
            pFt3DetectInfo->nChannelCount = nChannelCount;
            pFt3DetectInfo->nAsdu = nAsdu;
            pFt3DetectInfo->nFrameLen = nFrameLen;

            if (!_AddDetectItem(pFt3DetectInfo))
            {
                delete pFt3DetectInfo;
                pFt3DetectInfo = NULL;
            }
            else
            {
                CDetectItemFrame* pDetectItemFrame = new CDetectItemFrame(pFt3DetectInfo);
                pDetectItemFrame->copyFrameData(pFrame, nFrameLen);
                m_lockDetectItemsFrame.Acquire();
                m_vecDetectItemsFrame.push_back(pDetectItemFrame);
                m_lockDetectItemsFrame.Release();
            }
        }
        else
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_GenFt3DetectInfo: Get Frame Content Failed \n")));
            goto errorHandle;
        }
    }

errorHandle:
    //给缓冲区解锁
    pFt3->UnLockBuffer();
    if (NULL != pAllocRecvBuffer)
    {
        delete[] pAllocRecvBuffer;
        pAllocRecvBuffer = NULL;
    }
}

///
/// @brief
///    获取网络接口探测结果,数据填充进入m_setNetDetectInfo
///
///
void CFrameRecv::_GenNetDetectInfo(CFrameRecvNet * pNet)
{
    if (NULL == pNet)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_GenNetDetectInfo(): INPUT arg pNet Null \n")));
        return;
    }
#if GEN_LOG_MESSAGE
    RETAILMSG(OUTPUT_LOG_MSG, (_T("Net Detect ...\r\n")));
#endif

    //处理数据时先停止数据接收，处理完毕重新接收新数据
    if (NET_RECV_TYPE_SMV == m_enCurrentDetectType)
    {
        pNet->StopNetRecv();
    }

    BufferHeader* pBufferHeader = reinterpret_cast<BufferHeader*>(pNet->m_pDataBuf);
    //给缓冲区加锁，在处理缓冲区时防止驱动覆盖缓冲区信息
    pNet->LockBuffer();
    uint32_t nFrameCount = pBufferHeader->NetHeader.nFramesCount;
    uint32_t BufferLen = pBufferHeader->NetHeader.nActiveLength;
    uint32_t nLoopItem = pBufferHeader->NetHeader.nLoopItem;
    uint32_t nBufferPos0 = 0;
    uint32_t nBufferPos1 = 0;
    uint8_t* pBufferData = NULL;
    if (pBufferHeader->NetHeader.nNetMode >= _9_1_9_2_DETECT && pBufferHeader->NetHeader.nNetMode <= _GOOSE_FILTER)
    {
        nBufferPos0 = NET_RECV_LOOP_BUF0_POS;
        nBufferPos1 = NET_RECV_LOOP_BUF1_POS;
    }
    else if (pBufferHeader->NetHeader.nNetMode == _IEEE1588_DETECT || pBufferHeader->NetHeader.nNetMode == _IEEE1588_FILTER)
    {
        nBufferPos0 = IEEE1588_RECV_BUF0_POS;
        nBufferPos1 = IEEE1588_RECV_BUF1_POS;
    }

    if (0 == nLoopItem)
    {
        pBufferData = pNet->m_pDataBuf + sizeof(BufferHeader) + nBufferPos0;
    }
    else if (1 == nLoopItem)
    {
        pBufferData = pNet->m_pDataBuf + sizeof(BufferHeader) + nBufferPos1;
    }

    uint8_t* pAllocRecvBuffer = NULL;
#if DRIVER_BUFFER_USE_CPY
    pAllocRecvBuffer = new uint8_t[BufferLen];
    memcpy_s(pAllocRecvBuffer, BufferLen, pBufferData, BufferLen);
    CNetRecvBuffer recvBuffer(pAllocRecvBuffer, nFrameCount, BufferLen);
#else
    CNetRecvBuffer recvBuffer(pBufferData, nFrameCount, BufferLen);
#endif

    enumFrameType enFrameType = FRAME_TYPE_UNKOWN;
    uint32_t nFrameLen = 0;
    uint8_t *pFrame = NULL;

    MsgRapidDecoder& msgRapidDecoder = pNet->m_cMsgRapidDecoder;

    uint32_t nSampleRateCfg = 4000;
    uint16_t nSampleRateDetect = 4000;
    SMV92_SAMPLE_RATE en92SmpRate = SMV92_SAMPLE_RATE_AUTO_DETECT;
    CSmvRecvConfigPublic* pSmvRecvPub = CDsmDataLayer::getInstance()->GetSmvConfigPublic();
    if (pSmvRecvPub)
    {
        en92SmpRate = pSmvRecvPub->GetSmv92SampleRate();
        switch(en92SmpRate)
        {
        case SMV92_SAMPLE_RATE_4K:
            nSampleRateCfg = 4000;
            break;
        case SMV92_SAMPLE_RATE_12P8K:
            nSampleRateCfg = 12800;
            break;
        case SMV92_SAMPLE_RATE_AUTO_DETECT:
            break;
        default:
            break;
        }
    }

    uint32_t nChannelCount = 0;
    uint16_t nAppid = 0;
    uint8_t MacAddr[ETHER_MAC_LEN] = {0};
    uint16_t nAsdu = 1;
    FrameInfo stFrameInfo;

    for ( uint32_t i=0; i < nFrameCount; ++i)
    {
        if (recvBuffer.GetBufferFrameAddr(i, &pFrame, nFrameLen))
        {
            recvBuffer.GetBufferFrameInfo(i, stFrameInfo);
            enFrameType = msgRapidDecoder.GetMessageType(pFrame, nFrameLen);
            msgRapidDecoder.GetDstMac(pFrame, nFrameLen, MacAddr);

            if ( FRAME_TYPE_NET_9_1 == enFrameType
              || FRAME_TYPE_NET_9_2 == enFrameType )
            {
                msgRapidDecoder.GetAppID(pFrame, nFrameLen, nAppid);
                msgRapidDecoder.GetChannelNum(pFrame, nFrameLen, enFrameType, nChannelCount);
                msgRapidDecoder.GetNoASDU(pFrame, nFrameLen, enFrameType, nAsdu);

                if (FRAME_TYPE_NET_9_1 == enFrameType)
                {
                    msgRapidDecoder.GetSmpRate(pFrame, nFrameLen, enFrameType, nSampleRateDetect);
                    if (nSampleRateDetect == 0)
                    {
                        nSampleRateDetect = 12800;
                    }
                    else
                    {
                        nSampleRateDetect = nSampleRateDetect * DSM_CONFIG_DEFAULT_ELEC_FREQUENCY;
                    }
                }
                else
                {
                    CNetDetectItem* pNetItem = _GetNetFrameDetectItem(nAppid, enFrameType, pNet->m_enPortName, MacAddr);
                    if (pNetItem != NULL)
                    {
                        //对每一个 9-2帧记录时间戳，采样值数量信息
                        AutoLock autoLock(m_lock92SmpRateCalcInfo);
                        m_map92SmpRateCalcInfo[pNetItem].ParseSample(stFrameInfo.timeStamp, nAsdu);
                    }
                }

                CNetDetectItem stNetDetectItem;
                stNetDetectItem.enFrameType = enFrameType;
                stNetDetectItem.enPortName = pNet->m_enPortName;
                stNetDetectItem.nChannelCount = nChannelCount;
                stNetDetectItem.appId = nAppid;
                if (SMV92_SAMPLE_RATE_AUTO_DETECT == en92SmpRate)
                {
                    stNetDetectItem.nSampleRate = nSampleRateDetect;
                }
                else
                {
                    stNetDetectItem.nSampleRate = nSampleRateCfg;
                }
                stNetDetectItem.nSampleRateDetect = nSampleRateDetect;
                stNetDetectItem.nAsdu = nAsdu;
                stNetDetectItem.nFrameLen = stFrameInfo.capLen;
                memcpy_s(stNetDetectItem.macAddr, ETHER_MAC_LEN, MacAddr, ETHER_MAC_LEN);

                if (!_IsDetectItemExist(&stNetDetectItem))
                {
                    CNetDetectItem *pNetDetectItem = new CNetDetectItem(stNetDetectItem);
                    if (NET_RECV_TYPE_SMV == m_enCurrentDetectType && _AddDetectItem(pNetDetectItem))
                    {
                        CDetectItemFrame* pDetectItemFrame = new CDetectItemFrame(pNetDetectItem);
                        pDetectItemFrame->copyFrameData(pFrame, nFrameLen);
                        m_lockDetectItemsFrame.Acquire();
                        m_vecDetectItemsFrame.push_back(pDetectItemFrame);
                        m_lockDetectItemsFrame.Release();
                    }
                    else
                    {
                        delete pNetDetectItem;
                        pNetDetectItem = NULL;
                    }
                }
            }
            else if (FRAME_TYPE_NET_GOOSE == enFrameType)
            {
                msgRapidDecoder.GetAppID(pFrame, nFrameLen, nAppid);
                msgRapidDecoder.GetChannelNum(pFrame, nFrameLen, enFrameType, nChannelCount);
                msgRapidDecoder.GetNoASDU(pFrame, nFrameLen, enFrameType, nAsdu);

                CNetDetectItem stNetDetectItem;
                stNetDetectItem.enFrameType = enFrameType;
                stNetDetectItem.enPortName = pNet->m_enPortName;
                stNetDetectItem.nChannelCount = nChannelCount;
                stNetDetectItem.appId = nAppid;
                stNetDetectItem.nAsdu = nAsdu;
                stNetDetectItem.nFrameLen = stFrameInfo.capLen;
                memcpy_s(stNetDetectItem.macAddr, ETHER_MAC_LEN, MacAddr, ETHER_MAC_LEN);

                if (!_IsDetectItemExist(&stNetDetectItem))
                {
                    CNetDetectItem *pNetDetectItem = new CNetDetectItem(stNetDetectItem);
                    if (NET_RECV_TYPE_GOOSE == m_enCurrentDetectType && _AddDetectItem(pNetDetectItem))
                    {
                        CDetectItemFrame* pDetectItemFrame = new CDetectItemFrame(pNetDetectItem);
                        pDetectItemFrame->copyFrameData(pFrame, nFrameLen);
                        m_lockDetectItemsFrame.Acquire();
                        m_vecDetectItemsFrame.push_back(pDetectItemFrame);
                        m_lockDetectItemsFrame.Release();
                    }
                    else
                    {
                        delete pNetDetectItem;
                        pNetDetectItem = NULL;
                    }
                }
            }
            else if (FRAME_TYPE_NET_1588 == enFrameType)
            {
                IEEE1588_SOURCE_PORT_ID stClockId;
                uint8_t nVersion = 0;
                msgRapidDecoder.GetSrcMac(pFrame, nFrameLen, MacAddr);
                msgRapidDecoder.GetIeee1588SrcPortID(pFrame, nFrameLen, stClockId);
                msgRapidDecoder.GetIeee1588Version(pFrame, nFrameLen, nVersion);

                CNet1588DetectItem stNet1588DetectItem;
                stNet1588DetectItem.enFrameType = enFrameType;
                stNet1588DetectItem.enPortName = pNet->m_enPortName;
                stNet1588DetectItem.stClockID = stClockId;
                stNet1588DetectItem.n1588Version = nVersion;
                stNet1588DetectItem.nFrameLen = stFrameInfo.capLen;
                memcpy_s(stNet1588DetectItem.macAddr, ETHER_MAC_LEN, MacAddr, ETHER_MAC_LEN);

                if (!_IsDetectItemExist(&stNet1588DetectItem))
                {
                    CNet1588DetectItem *pNetDetectItem = new CNet1588DetectItem(stNet1588DetectItem);
                    if (NET_RECV_TYPE_IEEE_1588 == m_enCurrentDetectType && _AddDetectItem(pNetDetectItem))
                    {
                    }
                    else
                    {
                        delete pNetDetectItem;
                        pNetDetectItem = NULL;
                    }
                }
            }
        }
        else
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_GenNetDetectInfo: Get Frame Content Failed \n")));
            goto errorHandle;
        }
    }

errorHandle:
    //计算并更新此次缓冲区中9-2数据的采样率
    _SmpCalcAfterBufferAnalysis(SMV92_SAMPLE_RATE_AUTO_DETECT == en92SmpRate);

    //给缓冲区解锁
    pNet->UnLockBuffer();
    if (NULL != pAllocRecvBuffer)
    {
        delete[] pAllocRecvBuffer;
        pAllocRecvBuffer = NULL;
    }
    //数据处理完毕，重新开始探测
    if (NET_RECV_TYPE_SMV == m_enCurrentDetectType)
    {
        pNet->Detect(m_enCurrentDetectType, NET_DEFAULT_RECV_FRAMES, NET_DEFAULT_RECV_ENABLETIMEOUT, NET_DEFAULT_RECV_TIMEOUT);
    }
}

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

bool CFrameRecv::_Detect(NetFt3RecvFrameType enNetFt3RecvFrameType, uint16_t nCount, bool bEnableTimeout, uint32_t nTimeoutMilliSecond)
{
    switch(enNetFt3RecvFrameType)
    {
    case NET_RECV_TYPE_GOOSE:
    case NET_RECV_TYPE_IEEE_1588:
    case NET_RECV_TYPE_SMV:
        //设置Net开始探测
        if (m_pFrameNetA)
        {
            m_pFrameNetA->Detect(enNetFt3RecvFrameType, nCount, bEnableTimeout, nTimeoutMilliSecond);
        }
#if GPM_DRIVER_ENABLE_ALL_PORT
        if (m_pFrameNetB)
        {
            m_pFrameNetB->Detect(enNetFt3RecvFrameType, nCount, bEnableTimeout, nTimeoutMilliSecond);
        }
        if (m_pFrameNetC)
        {
            m_pFrameNetC->Detect(enNetFt3RecvFrameType, nCount, bEnableTimeout, nTimeoutMilliSecond);
        }
#endif
        break;
    case  FT3_RECV_TYPE_SMV:
        //设置光口接受信息
        if (m_pFrameFt3A)
        {
            m_pFrameFt3A->StartFt3Recv();
        }
#if GPM_DRIVER_ENABLE_ALL_PORT
        if (m_pFrameFt3B)
        {
            m_pFrameFt3B->StartFt3Recv();
        }
#endif
        break;
    default:
        break;
    }
    return true;
}

///
/// @brief
///    设置当前的控制块，dsp计算此控制块的信息
///
/// @param[in] pDetectItem  要设置的当前控制块
///
void CFrameRecv::SetCurrentCtrlBlk( CBaseDetectItem* pDetectItem )
{
    if (NULL != pDetectItem)
    {
        //如果设置的控制块信息和当前的控制块不一样，需要清空以前保留的信息
        if (pDetectItem != GetCurrentCtrlBlk())
        {
            //设置pDetectItem为当前控制块
            m_pCurrentDetectItems[2] = pDetectItem;

            //设置实时通道数目为当前控制块通道数目，避免出现刚进入有效值等页面时提示通道数目变化的问题
            {
                AutoLock autoLock(m_lockRealTimeChannelCounts);
                m_nRealTimeChannelCounts[0] = pDetectItem->nChannelCount;
            }

            if (_IsSmvFrame(pDetectItem->enFrameType))
            {
                //修改dsp计算使用的采样率
                uint32_t nSmpRate = pDetectItem->nSampleRate;
#if DSM_DSP_CALC_128K_RESAMPLE_TO_4K
                if (_IsModeReSampleTo4k(GetCurrentMode()))
                {
                    if (nSmpRate == 12800)
                    {
                        nSmpRate = 4000;
                    }
                }
#endif
                SetDspSampleRate(nSmpRate);
                SetDspAsduCount(pDetectItem->nAsdu);


                //清除缓存的数据信息
                _ClearCachedData();

                //重新统计报文离散度
                ResetSmvFrameStatistics();
            }
            else if (pDetectItem->enFrameType == FRAME_TYPE_NET_GOOSE)
            {
                ResetGooseFrameStatistics();
            }
            else if (pDetectItem->enFrameType == FRAME_TYPE_NET_1588)
            {
                _ClearNet1588Time();
            }

            //清理实时报文数据
            _ClearRealTimeMsg();

            //如果要设置的控制块在控制块列表中不存在，则将其添加进控制块列表
            bool bFound = false;
            std::vector<CBaseDetectItem*>::iterator itr = m_vecSelectedItems.begin();
            for (; itr != m_vecSelectedItems.end(); ++itr)
            {
                if (pDetectItem->nID == (*itr)->nID )
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                AddOneSelectedCtrlBlk(pDetectItem);
            }

//             if (pDetectItem->enFrameType == FRAME_TYPE_NET_9_2 
//              || pDetectItem->enFrameType == FRAME_TYPE_NET_9_1)
//             {
//                 _SetOneCtrlBlkFilter(dynamic_cast<CNetDetectItem*>(pDetectItem));
//             }
        }
    }
}

///
/// @brief
///    分析网络接口获取的数据，根据设置进行处理
///
void CFrameRecv::_NetSmvAnaly( CFrameRecvNet * pNet, DSM_OPERATE_MODE enMode, CNetDetectItem* pCurrentCtrlBlk )
{
    if (_GetCachedCtrlBlk() != pCurrentCtrlBlk)
    {
        return;
    }

    BufferHeader* pBufferHeader = reinterpret_cast<BufferHeader*>(pNet->m_pDataBuf);
    //给缓冲区加锁，在处理缓冲区时防止驱动覆盖缓冲区信息
    pNet->LockBuffer();
    uint32_t nFrameCount = pBufferHeader->NetHeader.nFramesCount;
    uint32_t BufferLen =  pBufferHeader->NetHeader.nActiveLength;
    uint32_t nLoopItem = pBufferHeader->NetHeader.nLoopItem;
    uint8_t* pBufferData = NULL;
    if (0 == nLoopItem)
    {
        pBufferData = pNet->m_pDataBuf + sizeof(BufferHeader);
    }
    else if (1 == nLoopItem)
    {
        pBufferData = pNet->m_pDataBuf + sizeof(BufferHeader) + NET_RECV_LOOP_BUF1_POS;
    }

    uint8_t* pAllocRecvBuffer = NULL;
#if DRIVER_BUFFER_USE_CPY
    pAllocRecvBuffer = new uint8_t[BufferLen];
    memcpy_s(pAllocRecvBuffer, BufferLen, pBufferData, BufferLen);
    CNetRecvBuffer recvBuffer(pAllocRecvBuffer, nFrameCount, BufferLen);
#else
    CNetRecvBuffer recvBuffer(pBufferData, nFrameCount, BufferLen);
#endif


    _NetSmvBufferAnaly(recvBuffer, enMode, pCurrentCtrlBlk, pNet);
    //对缓冲区解锁
    pNet->UnLockBuffer();
    if (NULL != pAllocRecvBuffer)
    {
        delete[] pAllocRecvBuffer;
        pAllocRecvBuffer = NULL;
    }
}

///
/// @brief
///     分析Ft3接口获取的数据，根据设置进行处理
///
void CFrameRecv::_Ft3SmvAnaly( CFrameRecvFt3* pFt3 , DSM_OPERATE_MODE enMode, CFt3DetectItem* pCurrentCtrlBlk )
{
    BufferHeader* pBufferHeader = reinterpret_cast<BufferHeader*>(pFt3->m_pDataBuf);
    //给缓冲区加锁，在处理缓冲区时防止驱动覆盖缓冲区信息
    pFt3->LockBuffer();
    uint32_t nFrameCount = pBufferHeader->Ft3Header.nFramesCount;
    uint32_t BufferLen = pBufferHeader->Ft3Header.nActiveLength;
    uint32_t nLoopItem = pBufferHeader->Ft3Header.nLoopItem;
    uint8_t* pBufferData = NULL;
    if (0 == nLoopItem)
    {
        pBufferData = pFt3->m_pDataBuf + sizeof(BufferHeader);
    }
    else if (1 == nLoopItem)
    {
        pBufferData = pFt3->m_pDataBuf + sizeof(BufferHeader) + FT3_RECV_LOOP_BUF1_POS;
    }

    uint8_t* pAllocRecvBuffer = NULL;
#if DRIVER_BUFFER_USE_CPY
    pAllocRecvBuffer = new uint8_t[BufferLen];
    memcpy_s(pAllocRecvBuffer, BufferLen, pBufferData, BufferLen);
    CFt3RecvBuffer recvBuffer(pAllocRecvBuffer, nFrameCount, BufferLen);
#else
    CFt3RecvBuffer recvBuffer(pBufferData, nFrameCount, BufferLen);
#endif
    recvBuffer.SetBufferHeader(*pBufferHeader);

    enumFrameType enFrameType = recvBuffer.GetBufferFrameType();

    if (FRAME_TYPE_FT3_GUOWANG == enFrameType)
    {
        _Ft3SmvAnalyGuowang(recvBuffer, enMode, pCurrentCtrlBlk, pFt3);
    }
    else if (FRAME_TYPE_FT3_NANRUI == enFrameType)
    {
        _Ft3SmvAnalyNanrui(recvBuffer, enMode, pCurrentCtrlBlk, pFt3);
    }
    else
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_Ft3SmvAnaly(): Ft3 Buffer Frame type Unkown \n ")));
    }

    //给缓冲区解锁
    pFt3->UnLockBuffer();
    if (NULL != pAllocRecvBuffer)
    {
        delete[] pAllocRecvBuffer;
        pAllocRecvBuffer = NULL;
    }

}

///
/// @brief
///    获取探测结果数目
///
/// @return
/// 返回探测结果的数目
///
uint32_t CFrameRecv::GetNetDetectItemsCount()
{
    m_vecDetectItemsTemp.clear();
    m_lockDetectItems.Acquire();
    std::vector<CBaseDetectItem*>::iterator itr = m_vecDetectItems.begin();
    if (itr != m_vecDetectItems.end())
    {
        if (_IsNetFrame((*itr)->enFrameType))
        {
            m_vecDetectItemsTemp.assign(m_vecDetectItems.begin(), m_vecDetectItems.end());
        }
    }
    m_lockDetectItems.Release();
    return m_vecDetectItemsTemp.size();
}

///
/// @brief
///    获取一个探测结果
///
/// @param[in]  nIndex  要获取探测结果的索引，从0开始
///
/// @return
///     CNetDetectItem* 返回指定索引探测结果对象的指针
///
CNetDetectItem* CFrameRecv::GetNetDetectItem( uint32_t nIndex )
{
    if (nIndex < m_vecDetectItemsTemp.size())
    {
        std::vector<CBaseDetectItem*>::iterator itr = m_vecDetectItemsTemp.begin();
        advance(itr, nIndex);
        if (itr != m_vecDetectItemsTemp.end())
        {
            if (_IsNetFrame((*itr)->enFrameType))
            {
                return dynamic_cast<CNetDetectItem*>(*itr);
            }
        }
    }
    return NULL;
}

///
/// @brief
///    获取探测结果数目
///
/// @return
/// 返回探测结果的数目
///
uint32_t CFrameRecv::GetFt3DetectItemsCount()
{
    m_vecDetectItemsTemp.clear();
    m_lockDetectItems.Acquire();
    std::vector<CBaseDetectItem*>::iterator itr = m_vecDetectItems.begin();
    if (itr != m_vecDetectItems.end())
    {
        if (_IsFt3Frame((*itr)->enFrameType))
        {
            m_vecDetectItemsTemp.assign(m_vecDetectItems.begin(), m_vecDetectItems.end());
        }
    }
    m_lockDetectItems.Release();
    return m_vecDetectItemsTemp.size();
}

///
/// @brief
///    获取一个探测结果
///
/// @param[in]  nIndex  要获取探测结果的索引，从0开始
///
/// @return
///     CFt3DetectItem* 返回指定索引探测结果对象的指针
///
CFt3DetectItem* CFrameRecv::GetFt3DetectItem( uint32_t nIndex )
{
    if (nIndex < m_vecDetectItemsTemp.size())
    {
        std::vector<CBaseDetectItem*>::iterator itr = m_vecDetectItemsTemp.begin();
        advance(itr, nIndex);
        if (itr != m_vecDetectItemsTemp.end())
        {
            if (_IsFt3Frame((*itr)->enFrameType))
            {
                return dynamic_cast<CFt3DetectItem*>(*itr);
            }
        }
    }
    return NULL;
}


///
/// @brief
///    获取1588探测结果数目
///
/// @return
/// 返回探测结果的数目
///
uint32_t CFrameRecv::GetNet1588DetectItemsCount()
{
    m_vecDetectItemsTemp.clear();
    m_lockDetectItems.Acquire();
    std::vector<CBaseDetectItem*>::iterator itr = m_vecDetectItems.begin();
    if (itr != m_vecDetectItems.end())
    {
        if (FRAME_TYPE_NET_1588 == ((*itr)->enFrameType))
        {
            m_vecDetectItemsTemp.assign(m_vecDetectItems.begin(), m_vecDetectItems.end());
        }
    }
    m_lockDetectItems.Release();
    return m_vecDetectItemsTemp.size();
}

///
/// @brief
///    获取一个1588探测结果
///
/// @param[in]  nIndex  要获取探测结果的索引，从0开始
///
/// @return
///     CNetDetectItem* 返回指定索引探测结果对象的指针
///
CNet1588DetectItem* CFrameRecv::GetNet1588DetectItem( uint32_t nIndex )
{
    if (nIndex < m_vecDetectItemsTemp.size())
    {
        std::vector<CBaseDetectItem*>::iterator itr = m_vecDetectItemsTemp.begin();
        advance(itr, nIndex);
        if (itr != m_vecDetectItemsTemp.end())
        {
            if (FRAME_TYPE_NET_1588 == ((*itr)->enFrameType))
            {
                return dynamic_cast<CNet1588DetectItem*>(*itr);
            }
        }
    }
    return NULL;
}


///
/// @brief
///    获取已经选中的控制块数量
///
/// size_t      返回已经选中的控制块数量
///
size_t CFrameRecv::GetSelectedCtrlBlkCount()
{
    return m_vecSelectedItems.size();
}

///
/// @brief
///    获取当前的控制块指针
///
/// @return
///     CBaseDetectItem*   返回当前控制块指针，失败返回Null
///
CBaseDetectItem*  CFrameRecv::GetCurrentCtrlBlk()
{
    return m_pCurrentDetectItems[2];
}

///
/// @brief
///    清空所有容器数据
///
void CFrameRecv::_ClearAllContainer()
{
    _ClearDetectItems();

    _ClearSmvDeque();

    _ClearSmvTimeDeque();

    _ClearGooseChangeList();

    _ClearSmvFrameStatistics();
    _ClearGooseFrameStatistics();
}

///
/// @brief
///    设置当前模式
///
/// @param[in]  enDsmMode 要设置的模式
///
void CFrameRecv::SetCurrentMode( DSM_OPERATE_MODE enDsmMode )
{
    //获取更改前的模式，可能在某些模式切换时要做一些设置/资源清理等操作
    DSM_OPERATE_MODE enDsmModeOld = GetCurrentMode();
    RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::SetCurrentMode(%d),LastMode(%d)\r\n"), enDsmMode, enDsmModeOld));
    if (enDsmModeOld == DSM_MODE_SMV_RECV_PHASE_CHECK)
    {
        //由于对12.8k采样率核相进行了到4k采样率的处理
        //所以退出核相界面，设置dsp采样率到原来水平
        CBaseDetectItem* pCurrentCtrlBlk = GetCurrentCtrlBlk();
        if (pCurrentCtrlBlk)
        {
            SetDspSampleRate(pCurrentCtrlBlk->nSampleRate);
        }
    }

#if DSM_DSP_CALC_128K_RESAMPLE_TO_4K
    if (_IsModeReSampleTo4k(enDsmModeOld))
    {
        //由于对12.8k采样率核相进行了到4k采样率的处理
        //所以退出核相界面，设置dsp采样率到原来水平
        CBaseDetectItem* pCurrentCtrlBlk = GetCurrentCtrlBlk();
        if (pCurrentCtrlBlk)
        {
            SetDspSampleRate(pCurrentCtrlBlk->nSampleRate);
        }
    }
#endif

    if (DSM_MODE_UNKOWN == enDsmMode || DSM_MODE_QUIT_CURRENT_PAGE == enDsmMode)
    {
        _SetCachedCtrlBlk(NULL);
    }
    else if (DSM_MODE_MAIN_PAGE == enDsmMode)
    {
        StopRecvData();
        _ClearAllContainer();
        _ClearCachedData();
        _ClearPhaseCheckData();
        _CloseDetectTimer();
    }
    else if ( DSM_MODE_SMV_RECV_FRAME_STATIC  == enDsmMode
        || DSM_MODE_SMV_RECV_DISPERSION == enDsmMode)
    {
        if (m_pDsp)
        {
            m_pDsp->ResetCache(1);
        }
    }
    else if (DSM_MODE_SMV_RECV_PHASE_CHECK == enDsmMode)
    {
        //进入核相模式，如果控制块的采样率是12.8k，则使用4k的采样率，因为数据层已经将12.8k重新采样为4k
        CBaseDetectItem* pCurrentCtrlBlk = GetCurrentCtrlBlk();
        if (pCurrentCtrlBlk)
        {
            if (pCurrentCtrlBlk->nSampleRate == 12800)
            {
                SetDspSampleRate(4000);
                m_cSmpSyncTime.SetResampleConfig(DSM_CONFIG_DEFAULT_ELEC_FREQUENCY, 4000, 4000);
            }
            else
            {
                SetDspSampleRate(pCurrentCtrlBlk->nSampleRate);
                m_cSmpSyncTime.SetResampleConfig(DSM_CONFIG_DEFAULT_ELEC_FREQUENCY, pCurrentCtrlBlk->nSampleRate, pCurrentCtrlBlk->nSampleRate);
            }
        }
    }
#if DSM_DSP_CALC_128K_RESAMPLE_TO_4K
    else if (_IsModeReSampleTo4k(enDsmMode))
    {
        //进入核相模式，如果控制块的采样率是12.8k，则使用4k的采样率，因为数据层已经将12.8k重新采样为4k
        CBaseDetectItem* pCurrentCtrlBlk = GetCurrentCtrlBlk();
        if (pCurrentCtrlBlk)
        {
            if (pCurrentCtrlBlk->nSampleRate == 12800)
            {
                SetDspSampleRate(4000);
                m_cSmpSyncTime.SetResampleConfig(DSM_CONFIG_DEFAULT_ELEC_FREQUENCY, 4000, 4000);
            }
            else
            {
                SetDspSampleRate(pCurrentCtrlBlk->nSampleRate);
                m_cSmpSyncTime.SetResampleConfig(DSM_CONFIG_DEFAULT_ELEC_FREQUENCY, pCurrentCtrlBlk->nSampleRate, pCurrentCtrlBlk->nSampleRate);
            }
        }
    }
#endif

    AutoLock autoLock(m_lockCurrentMode);
    m_enCurrentMode = enDsmMode;
}

///
/// @brief
///    获取当前模式
///
/// @return
///     DSM_OPERATE_MODE    返回当前系统模式
///
DSM_OPERATE_MODE CFrameRecv::GetCurrentMode()
{
    AutoLock autoLock(m_lockCurrentMode);
    return m_enCurrentMode;
}

///
/// @brief
///    添加探测结果到m_vecDetectItems
///
/// @param[in]  pBaseDetectItem 要添加的探测结果指针
///
/// @return
///     bool 添加成功返回true
///
bool CFrameRecv::_AddDetectItem( CBaseDetectItem* pBaseDetectItem )
{
    if (NULL == pBaseDetectItem)
    {
        return false;
    }
    std::vector<CBaseDetectItem*>::iterator itr;
    switch(pBaseDetectItem->enFrameType)
    {
    case FRAME_TYPE_NET_GOOSE:
    case FRAME_TYPE_NET_9_1:
    case FRAME_TYPE_NET_9_2:
        {
            CNetDetectItem* pNetDetectItem = dynamic_cast<CNetDetectItem*>(pBaseDetectItem);
            if (NULL == pNetDetectItem)
            {
                return false;
            }
            int nMacSame = 0;
            bool bFound = false;
            m_lockDetectItems.Acquire();
            itr = m_vecDetectItems.begin();
            if (itr != m_vecDetectItems.end())
            {
                CNetDetectItem* pNetDetectItemItr = dynamic_cast<CNetDetectItem*>(*itr);
                if (NULL == pNetDetectItemItr)
                {
                    _ClearDetectItems();
                }
            }
            if (!_IsDetectItemExist(pBaseDetectItem))
            {
                pBaseDetectItem->nID = m_vecDetectItems.size() > 0 ? m_vecDetectItems.back()->nID+1 : 0;
                m_vecDetectItems.push_back(pBaseDetectItem);
                m_lockDetectItems.Release();
                return true;
            }
            m_lockDetectItems.Release();
        }
        break;
    case FRAME_TYPE_FT3_NANRUI:
    case FRAME_TYPE_FT3_GUOWANG:
        {
            CFt3DetectItem* pFt3DetectItem = dynamic_cast<CFt3DetectItem*>(pBaseDetectItem);
            if (NULL == pFt3DetectItem)
            {
                return false;
            }
            bool bFound = false;
            m_lockDetectItems.Acquire();
            itr = m_vecDetectItems.begin();
            if (itr != m_vecDetectItems.end())
            {
                CFt3DetectItem* pFt3DetectItemItr = dynamic_cast<CFt3DetectItem*>(*itr);
                if (NULL == pFt3DetectItemItr)
                {
                    _ClearDetectItems();
                }
            }
            if (!_IsDetectItemExist(pBaseDetectItem))
            {
                pBaseDetectItem->nID = m_vecDetectItems.size() > 0 ? m_vecDetectItems.back()->nID+1 : 0;
                m_vecDetectItems.push_back(pBaseDetectItem);
                m_lockDetectItems.Release();
                return true;
            }
            else
            {
                _UpdateFt3DetectItem(pBaseDetectItem);
            }
            m_lockDetectItems.Release();
        }
        break;
    case FRAME_TYPE_NET_1588:
        {
            CNet1588DetectItem* pNet1588DetectItem = dynamic_cast<CNet1588DetectItem*>(pBaseDetectItem);
            if (NULL == pNet1588DetectItem)
            {
                return false;
            }
            bool bFound = false;
            m_lockDetectItems.Acquire();
            itr = m_vecDetectItems.begin();
            if (itr != m_vecDetectItems.end())
            {
                CNet1588DetectItem* pNet1588DetectItemItr = dynamic_cast<CNet1588DetectItem*>(*itr);
                if (NULL == pNet1588DetectItemItr)
                {
                    _ClearDetectItems();
                }
            }
            if (!_IsDetectItemExist(pBaseDetectItem))
            {
                pBaseDetectItem->nID = m_vecDetectItems.size() > 0 ? m_vecDetectItems.back()->nID+1 : 0;
                m_vecDetectItems.push_back(pBaseDetectItem);
                m_lockDetectItems.Release();
                return true;
            }
            m_lockDetectItems.Release();
        }
        break;
    default:
        break;
    }
    return false;
}

///
/// @brief
///    判断当前控制块信息是否已经在探测列表中
///
/// @param[in]  pBaseDetectItem 要添加的探测结果指针
///
/// @return
///     bool 已经存在返回true
///
bool CFrameRecv::_IsDetectItemExist( CBaseDetectItem* pBaseDetectItem )
{
    std::vector<CBaseDetectItem*>::iterator itr;
    switch(pBaseDetectItem->enFrameType)
    {
    case FRAME_TYPE_NET_GOOSE:
    case FRAME_TYPE_NET_9_1:
    case FRAME_TYPE_NET_9_2:
        {
            CNetDetectItem* pNetDetectItem = dynamic_cast<CNetDetectItem*>(pBaseDetectItem);
            if (NULL == pNetDetectItem)
            {
                return false;
            }
            int nMacSame = 0;
            bool bFound = false;
            m_lockDetectItems.Acquire();
            for (itr = m_vecDetectItems.begin(); itr != m_vecDetectItems.end(); ++itr)
            {
                CNetDetectItem* pNetDetectItemItr = dynamic_cast<CNetDetectItem*>(*itr);
                if (pNetDetectItemItr)
                {
                    nMacSame = memcmp(pNetDetectItem->macAddr, pNetDetectItemItr->macAddr, ETHERNET_MAC_LEN);
                    if (pNetDetectItem->enPortName       == pNetDetectItemItr->enPortName 
                        && pNetDetectItem->enFrameType   == pNetDetectItemItr->enFrameType
                        && pNetDetectItem->appId         == pNetDetectItemItr->appId
                        && nMacSame == 0 )
                    {
                        bFound = true;
                        break;
                    }
                }
            }
            m_lockDetectItems.Release();
            return bFound;
        }
        break;
    case FRAME_TYPE_FT3_NANRUI:
    case FRAME_TYPE_FT3_GUOWANG:
        {
            CFt3DetectItem* pFt3DetectItem = dynamic_cast<CFt3DetectItem*>(pBaseDetectItem);
            if (NULL == pFt3DetectItem)
            {
                return false;
            }
            bool bFound = false;
            m_lockDetectItems.Acquire();
            for (itr = m_vecDetectItems.begin(); itr != m_vecDetectItems.end(); ++itr)
            {
                CFt3DetectItem* pFt3DetectItemItr = dynamic_cast<CFt3DetectItem*>(*itr);
                if (pFt3DetectItemItr)
                {
                    if (pFt3DetectItem->enPortName == pFt3DetectItemItr->enPortName)
                    {
                        bFound = true;
                        break;
                    }
                }
            }
            m_lockDetectItems.Release();
            return bFound;
        }
        break;
    case FRAME_TYPE_NET_1588:
        {
            CNet1588DetectItem* pNet1588DetectItem = dynamic_cast<CNet1588DetectItem*>(pBaseDetectItem);
            if (NULL == pNet1588DetectItem)
            {
                return false;
            }
            bool bFound = false;
            m_lockDetectItems.Acquire();
            for (itr = m_vecDetectItems.begin(); itr != m_vecDetectItems.end(); ++itr)
            {
                CNet1588DetectItem* pNet1588DetectItemItr = dynamic_cast<CNet1588DetectItem*>(*itr);
                if (pNet1588DetectItemItr)
                {
                    if (pNet1588DetectItem->enPortName     == pNet1588DetectItemItr->enPortName 
                        && pNet1588DetectItem->enFrameType == pNet1588DetectItemItr->enFrameType
                        && 0 == memcmp(pNet1588DetectItem->macAddr, pNet1588DetectItemItr->macAddr, ETHERNET_MAC_LEN)
                        && pNet1588DetectItem->stClockID.clockIdentity == pNet1588DetectItemItr->stClockID.clockIdentity
                        && pNet1588DetectItem->stClockID.sourcePortID  == pNet1588DetectItemItr->stClockID.sourcePortID
                        && pNet1588DetectItem->n1588Version == pNet1588DetectItemItr->n1588Version)
                    {
                        bFound = true;
                        break;
                    }
                }
            }
            m_lockDetectItems.Release();
            return bFound;
        }
        break;
    default:
        break;
    }
    return false;
}

///
/// @brief
///    清空探测信息
///
void CFrameRecv::_ClearDetectItems()
{

#if GEN_LOG_MESSAGE
    RETAILMSG(OUTPUT_LOG_MSG, (_T("_ClearDetectItems() \r\n")));
#endif
    //清空当前控制块
    memset(m_pCurrentDetectItems, NULL, sizeof(m_pCurrentDetectItems));

    std::vector<CBaseDetectItem*>::iterator itrSelecedItems = m_vecSelectedItems.begin();
    for (; itrSelecedItems != m_vecSelectedItems.end(); ++itrSelecedItems)
    {
        //保留采样值控制块对应的通道配置信息
        if ( NULL != (*itrSelecedItems)->pRecvConfig)
        {
            CBaseDetectItem* pDetetctItem = (*itrSelecedItems);
            CSmvRecvConfig* pSmvRecvConfigCurrent = pDetetctItem->pRecvConfig;
            uint32_t nChannelsCurrent = pSmvRecvConfigCurrent->GetChannelCount();
            CSmvRecvConfig* pSmvRecvConfigSaved = NULL;
            int32_t nConfigSrc = -1;
            uint16_t nNetAppid = 0;
            if (pDetetctItem->enFrameType == FRAME_TYPE_FT3_NANRUI)
            {
                pSmvRecvConfigSaved = m_pFt3SmvRecvCofig[0];
                nConfigSrc = 0;
            }
            else if (pDetetctItem->enFrameType == FRAME_TYPE_FT3_GUOWANG)
            {
                pSmvRecvConfigSaved = m_pFt3SmvRecvCofig[1];
                nConfigSrc = 1;
            }
            else if (pDetetctItem->enFrameType == FRAME_TYPE_NET_9_1)
            {
                CNetDetectItem* pNetDetectItem = dynamic_cast<CNetDetectItem*>(pDetetctItem);
                if (pNetDetectItem)
                {
                    nNetAppid = pNetDetectItem->appId;
                }

                std::map<uint16_t, CSmvRecvConfig*>::iterator itr = m_mapAppIDSmvRecv91Config.find(nNetAppid);
                if (itr != m_mapAppIDSmvRecv91Config.end())
                {
                    pSmvRecvConfigSaved = itr->second;
                }
                nConfigSrc = 2;
            }
            else if (pDetetctItem->enFrameType == FRAME_TYPE_NET_9_2)
            {
                CNetDetectItem* pNetDetectItem = dynamic_cast<CNetDetectItem*>(pDetetctItem);
                if (pNetDetectItem)
                {
                    nNetAppid = pNetDetectItem->appId;
                }

                std::map<uint16_t, CSmvRecvConfig*>::iterator itr = m_mapAppIDSmvRecv92Config.find(nNetAppid);
                if (itr != m_mapAppIDSmvRecv92Config.end())
                {
                    pSmvRecvConfigSaved = itr->second;
                }
                nConfigSrc = 3;
            }

            if (NULL == pSmvRecvConfigSaved)
            {
                pSmvRecvConfigSaved = new CSmvRecvConfig(NULL);
                for (uint32_t i = 0; i<nChannelsCurrent; ++i)
                {
                    pSmvRecvConfigSaved->AddOneChannelInfo(pSmvRecvConfigCurrent->GetOneChannelInfo(i));
                }

                if (nConfigSrc >= 0 && nConfigSrc < 2)
                {
                    m_pFt3SmvRecvCofig[nConfigSrc] = pSmvRecvConfigSaved;
                }
                else if (nConfigSrc == 2)
                {
                    if (!m_mapAppIDSmvRecv91Config.insert(std::make_pair(nNetAppid, pSmvRecvConfigSaved)).second)
                    {
                        delete pSmvRecvConfigSaved;
                        pSmvRecvConfigSaved = NULL;
                    }
                }
                else if (nConfigSrc == 3)
                {
                    if (!m_mapAppIDSmvRecv92Config.insert(std::make_pair(nNetAppid, pSmvRecvConfigSaved)).second)
                    {
                        delete pSmvRecvConfigSaved;
                        pSmvRecvConfigSaved = NULL;
                    }
                }
            }
            else
            {
                uint32_t nChannelSaved = pSmvRecvConfigSaved->GetChannelCount();
                for (uint32_t i = 0; i<nChannelsCurrent; ++i)
                {
                    if (i < nChannelSaved)
                    {
                        pSmvRecvConfigSaved->SetOneChannelInfo(i, pSmvRecvConfigCurrent->GetOneChannelInfo(i));
                    }
                    else
                    {
                        pSmvRecvConfigSaved->AddOneChannelInfo(pSmvRecvConfigCurrent->GetOneChannelInfo(i));
                    }
                }
            }

            //保存采样值的通道配置信息后，清理资源
            delete (*itrSelecedItems)->pRecvConfig;
            (*itrSelecedItems)->pRecvConfig = NULL;
        }
    }
    m_vecSelectedItems.clear();
    //清空已探测到的控制块
    m_vecDetectItemsTemp.clear();
    m_lockDetectItems.Acquire();
    std::vector<CBaseDetectItem*>::iterator itr = m_vecDetectItems.begin();
    for (; itr != m_vecDetectItems.end(); ++itr)
    {
        if (NULL != *itr)
        {
            delete *itr;
            *itr = NULL;
        }
    }
    m_vecDetectItems.clear();
    m_lockDetectItems.Release();

    {
        AutoLock autoLock(m_lockDetectItemsFrame);
        std::vector<CDetectItemFrame*>::iterator itr = m_vecDetectItemsFrame.begin();
        while(itr != m_vecDetectItemsFrame.end())
        {
            if ((*itr))
            {
                delete (*itr);
                (*itr) = NULL;
            }
            itr++;
        }
        m_vecDetectItemsFrame.clear();
    }

    //清空依附于探测到控制块信息的92采样率计算信息
    _Reset92SmpCalcData();
}

///
/// @brief
///    获取当前的探测类型
///
/// @return
///     NetFt3RecvFrameType 返回当前的探测数据类型
///
NetFt3RecvFrameType CFrameRecv::GetCurrentRecvType()
{
    return m_enCurrentDetectType;
}

///
/// @brief
///    添加一个选中的控制块
///
/// @param[in] pDetectItem  要设置的当前控制块
///
/// @return
///     bool    添加成功返回true
///
bool CFrameRecv::AddOneSelectedCtrlBlk( CBaseDetectItem* pDetectItem )
{
    size_t nMaxSmvCtrlBlk = 0;
    nMaxSmvCtrlBlk = (NET_RECV_TYPE_GOOSE == m_enCurrentDetectType ? DSM_MAX_GOOSE_CTRL_BLK : DSM_MAX_SMV_CTRL_BLK);
    if (m_vecSelectedItems.size() < nMaxSmvCtrlBlk)
    {
        CSmvRecvConfig* pSmvRecvConfigSaved = NULL;
        switch(pDetectItem->enFrameType)
        {
        case FRAME_TYPE_FT3_NANRUI:
            pSmvRecvConfigSaved = m_pFt3SmvRecvCofig[0];
            break;
        case FRAME_TYPE_FT3_GUOWANG:
            pSmvRecvConfigSaved = m_pFt3SmvRecvCofig[1];
            break;
        case FRAME_TYPE_NET_9_1:
            {
                CNetDetectItem* pNetDetectItem = dynamic_cast<CNetDetectItem*>(pDetectItem);
                if (pNetDetectItem)
                {
                    std::map<uint16_t, CSmvRecvConfig*>::iterator itr = m_mapAppIDSmvRecv91Config.find(pNetDetectItem->appId);
                    if (itr != m_mapAppIDSmvRecv91Config.end())
                    {
                        pSmvRecvConfigSaved = itr->second;
                    }
                }
            }
            break;
        case FRAME_TYPE_NET_9_2:
            {
                CNetDetectItem* pNetDetectItem = dynamic_cast<CNetDetectItem*>(pDetectItem);
                if (pNetDetectItem)
                {
                    std::map<uint16_t, CSmvRecvConfig*>::iterator itr = m_mapAppIDSmvRecv92Config.find(pNetDetectItem->appId);
                    if (itr != m_mapAppIDSmvRecv92Config.end())
                    {
                        pSmvRecvConfigSaved = itr->second;
                    }
                }
            }
            break;
        case FRAME_TYPE_NET_GOOSE:
            break;
        default:
            break;
        }

        if (NULL == pDetectItem->pRecvConfig)
        {
            CSmvRecvConfig* pSmvRecvConfig = new CSmvRecvConfig(pDetectItem);
            pDetectItem->pRecvConfig = pSmvRecvConfig;

            if (NULL != pSmvRecvConfigSaved)
            {
                uint32_t nSavedChannelCount = pSmvRecvConfigSaved->GetChannelCount();
                for (uint32_t i = 0; i<pDetectItem->nChannelCount; ++i)
                {
                    if (i < nSavedChannelCount)
                    {
                        pSmvRecvConfig->AddOneChannelInfo(pSmvRecvConfigSaved->GetOneChannelInfo(i));
                    }
                    else
                    {
                        CSmvRecvChannelInfo channleInfo;
                        channleInfo.nIndex               = i;
                        channleInfo.pOwnerCtrlBlk        = NULL;
                        channleInfo.enChannelPhase       = CHANNEL_PHASE_UNKOWN;
                        channleInfo.enChannelType        = CHANNEL_TYPE_UNKOWN;
                        channleInfo.enChannelValueType   = CHANNEL_VALUE_TYPE_UNKNOWN;
                        channleInfo.fPrimaryRatedValue   = 0.0f;
                        channleInfo.fSecondaryRatedValue = 0.0f;
                        channleInfo.strChannelDesc       = L"";

                        pSmvRecvConfig->AddOneChannelInfo(&channleInfo);
                    }
                }
            }
            else
            {
                pSmvRecvConfig->init();
            }
        }

        m_vecSelectedItems.push_back(pDetectItem);
        return true;
    }
    return false;
}

///
/// @brief
///    移除一个已经添加的控制块
///
/// @param[in] pDetectItem  要移除的控制块
///
void CFrameRecv::RemoveOneSelectedCtrlBlk( CBaseDetectItem* pDetectItem )
{
    std::vector<CBaseDetectItem*>::iterator itr = m_vecSelectedItems.begin();
    for (; itr != m_vecSelectedItems.end(); ++itr)
    {
        if (pDetectItem->nID == (*itr)->nID)
        {
            m_vecSelectedItems.erase(itr);
            //如果要移除的控制块是当前的控制块，设置当前控制块到选择控制块的第一个
            if (pDetectItem->nID == GetCurrentCtrlBlk()->nID)
            {
                SetCurrentCtrlBlk((*m_vecSelectedItems.begin())) ;
            }

            if (NULL != pDetectItem->pRecvConfig)
            {
                delete pDetectItem->pRecvConfig;
                pDetectItem->pRecvConfig = NULL;
            }
        }
    }
}

bool CFrameRecv::_IsNetPort( DsmPortName enPortName )
{
    switch(enPortName)
    {
    case NET_PORT_LIGHT_A:
    case NET_PORT_LIGHT_B:
    case NET_PORT_ELECTRONIC:
        return true;
    default:
        return false;
    }
    return false;
}

bool CFrameRecv::_IsFt3Port( DsmPortName enPortName )
{
    switch(enPortName)
    {
    case FT3_PORT_LIGHT_A:
    case FT3_PORT_LIGHT_B:
    case FT3_PORT_LIGHT_C:
        return true;
    default:
        return false;
    }
    return false;
}

bool CFrameRecv::_IsNetFrame( enumFrameType enFrameType)
{
    switch(enFrameType)
    {
    case FRAME_TYPE_NET_9_1:
    case FRAME_TYPE_NET_9_2:
    case FRAME_TYPE_NET_GOOSE:
        return true;
    default:
        return false;
    }
    return false;
}

bool CFrameRecv::_IsFt3Frame( enumFrameType enFrameType)
{
    switch(enFrameType)
    {
    case FRAME_TYPE_FT3_GUOWANG:
    case FRAME_TYPE_FT3_NANRUI:
        return true;
    default:
        return false;
    }
    return false;
}

bool CFrameRecv::_IsSmvFrame( enumFrameType enFrameType )
{
    switch(enFrameType)
    {
    case FRAME_TYPE_FT3_GUOWANG:
    case FRAME_TYPE_FT3_NANRUI:
    case FRAME_TYPE_NET_9_1:
    case FRAME_TYPE_NET_9_2:
        return true;
    default:
        return false;
    }
    return false;
}


bool CFrameRecv::_IsPortInCurrentCtrlBlk( DsmPortName enPortName )
{
    uint32_t nSize = sizeof(m_pCurrentDetectItems)/sizeof(m_pCurrentDetectItems[0]);
    for (uint32_t i = 0; i < nSize; ++i)
    {
        if (m_pCurrentDetectItems[i] && enPortName == m_pCurrentDetectItems[i]->enPortName)
        {
            return true;
        }
    }
    return false;
}


bool CFrameRecv::_IsPortInSelecedCtrlBlk( DsmPortName enPortName )
{
    std::vector<CBaseDetectItem*>::iterator itr = m_vecSelectedItems.begin();
    while(itr != m_vecSelectedItems.end())
    {
        if (enPortName == (*itr)->enPortName)
        {
            return true;
        }
        ++itr;
    }
    return false;
}


///
/// @brief
///    获取上一个控制块信息
///
/// @param[in]  pCurrentCtrlBlk 指向当前的控制块
///
/// @return
///     CBaseDetectItem* 获取上一个控制块指针，没有上一个则返回NULL
///
CBaseDetectItem* CFrameRecv::GetPrevSelectCtrlBlk( CBaseDetectItem* pCurrentCtrlBlk )
{
    std::vector<CBaseDetectItem*>::reverse_iterator ritr;
    ritr = find(m_vecSelectedItems.rbegin(), m_vecSelectedItems.rend(), pCurrentCtrlBlk);
    if (ritr != m_vecSelectedItems.rend())
    {
        ritr++;
        if (ritr != m_vecSelectedItems.rend())
        {
            return (*ritr);
        }
    }
    return NULL;
}

///
/// @brief
///    获取下一个控制块信息
///
/// @param[in]  pCurrentCtrlBlk 指向当前的控制块
///
/// @return
///     CBaseDetectItem* 获取下一个控制块指针，没有下一个则返回NULL
///
CBaseDetectItem* CFrameRecv::GetNextSelectCtrlBlk( CBaseDetectItem* pCurrentCtrlBlk )
{
    std::vector<CBaseDetectItem*>::iterator itr;
    itr = find(m_vecSelectedItems.begin(),m_vecSelectedItems.end(), pCurrentCtrlBlk);
    if (itr != m_vecSelectedItems.end())
    {
        itr++;
        if (itr != m_vecSelectedItems.end())
        {
            return (*itr);
        }
    }
    return NULL;
}

///
/// @brief
///    根据id获取已经选中的控制块
///
/// @param[in]  nID     在生成扫描列表时使用的索引获取控制块指针
///
/// @return
///     CBaseDetectItem* 获取索引id对应的控制块指针，不存在则返回NULL
///
CBaseDetectItem* CFrameRecv::GetSelectCtrlBlkByID( uint32_t nID )
{
    std::vector<CBaseDetectItem*>::iterator itr = m_vecSelectedItems.begin();
    while (itr != m_vecSelectedItems.end())
    {
        if ((*itr)->nID == nID)
        {
            return (*itr);
        }
        itr++;
    }
    return NULL;
}


///
/// @brief
///    获取第一个控制块信息
///
/// @return
///     CBaseDetectItem* 获取第一个控制块指针，没有则返回NULL
///
CBaseDetectItem* CFrameRecv::GetFirstSelectCtrlBlk()
{
    if (m_vecSelectedItems.size() > 0)
    {
        return (*m_vecSelectedItems.begin());
    }
    return NULL;
}

///
/// @brief
///    分析smv数据
///
void CFrameRecv::_NetSmvBufferAnaly(CNetRecvBuffer& rNetRecvBuffer, DSM_OPERATE_MODE enOpMode, CNetDetectItem* pCurrentCtrlBlk, CFrameRecvNet * pNet)
{
    CBaseDetectItem* pCachedItem = _GetCachedCtrlBlk();
    if (pCurrentCtrlBlk != pCachedItem)
    {
        return;
    }
    Sav91Msg* pSav91Msg   = pNet->m_pSav91Msg;
    Sav92Msg* pSav92Msg   = pNet->m_pSav92Msg;
    uint32_t p92SmvQlt[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];
    MsgRapidDecoder& msgRapidDecoder = pNet->m_cMsgRapidDecoder;

    uint32_t nFrameLen    = 0;
    uint32_t nFrameCount  = rNetRecvBuffer.GetBufferFrameCount();
    uint8_t* pFrame       = NULL;
    SMV_VALUE stSmvValue;
    SMV_VALUE_FT3_9_1 stSmvValueFt3_91;
    SMV_VALUE_9_2     stSmvValue92;

    uint16_t nAppid = 0;
    uint16_t nSmpCnt = 0;
    bool     bSmpSync = false;
    uint16_t statusWord1 = 0;
    uint16_t statusWord2 = 0;

    //界面上需要显示的数据是原始值/一次/二次值
    enumSmvValueType  enSmvDispType = SMV_VALUE_TYPE_ORIGINANL;
    CSmvRecvConfigPublic* pSmvRecvCfgPub = CDsmDataLayer::getInstance()->GetSmvConfigPublic();
    if (pSmvRecvCfgPub)
    {
        enSmvDispType = pSmvRecvCfgPub->GetSmvDispType();
    }

    //设置的数据帧报文类型是一次/二次值
    enumSmvValueType enFrameValueType = SMV_VALUE_TYPE_SECONDARY;
    CParamConfigBasic* pParamConfigBasic = CDsmDataLayer::getInstance()->GetParamConfigBasic();
    if (pParamConfigBasic)
    {
        enFrameValueType = pParamConfigBasic->GetFrameValueType();
    }

    for ( uint32_t i=0; i < nFrameCount; ++i)
    {
        if (rNetRecvBuffer.GetBufferFrameAddr(i, &pFrame, nFrameLen))
        {
            msgRapidDecoder.GetAppID(pFrame, nFrameLen, nAppid);

            //如果当前缓存的数据块指针变为空，说明可能进行了模式切换/重新探测等操作导致当前正在处理的控制块指针失效
            if (_GetCachedCtrlBlk() != pCurrentCtrlBlk)
            {
                goto errorHandle;
            }
            else
            {
                if (nAppid == pCurrentCtrlBlk->appId && pNet->m_enPortName == pCurrentCtrlBlk->enPortName)
                {
                    stSmvValueFt3_91.pDetectItem = pCurrentCtrlBlk;
                    stSmvValue92.pDetectItem = pCurrentCtrlBlk;
                }
                else
                {
                    //当前数据帧的appid/mac与选择的控制块的appid/mac不同，处理下一数据帧
                    continue;
                }
            }

            enumFrameType enFrameType = msgRapidDecoder.GetMessageType(pFrame, nFrameLen);
            if ( DSM_MODE_SMV_RECV_FRAME_STATIC  == enOpMode
                || DSM_MODE_SMV_RECV_DISPERSION == enOpMode)
            {
                //获取以太网帧到达时间,进行离散度数据统计
                //只有在离散度页面才统计数据
                FrameInfo stFrameInfo;
                if (rNetRecvBuffer.GetBufferFrameInfo(i, stFrameInfo))
                {
                    //在核相业务需要需要数据帧时间信息，核相不会进入此处理函数
//                     stSmvValueFt3_91.stFrameTime = stFrameInfo.timeStamp;
//                     stSmvValue92.stFrameTime = stFrameInfo.timeStamp;
                    _AddSmvTime2Deque(stFrameInfo.timeStamp, pCurrentCtrlBlk);
                }
                _GenSmvFrameStatistics( pNet, pFrame, nFrameLen, stFrameInfo.timeStamp, pCurrentCtrlBlk);

                continue;
            }

            if (DSM_MODE_SMV_RECV_REAL_TIME_MSG == enOpMode)
            {
                m_lockRealTimeMsg.Acquire();
                m_cRealTimeMsg.m_pCtrlBlk   = pCurrentCtrlBlk;
                m_cRealTimeMsg.m_enFrameType = enFrameType;
                m_cRealTimeMsg.m_nFrameLen = nFrameLen;
                memcpy_s(m_cRealTimeMsg.m_nDataBuffer, NET_MAX_FRAME_LEN, pFrame, nFrameLen);
                m_lockRealTimeMsg.Release();
                continue;
            }
            //判断数据帧类型是9-1or9-2
            if (FRAME_TYPE_NET_9_1 == enFrameType)
            {
                if (0 == pSav91Msg->Decode(pFrame, nFrameLen))
                {
                    RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_NetSmvAnaly91: pSav91Msg->Decode Failed \n ")));
                    continue;
                }
                size_t nSmvChannels = pSav91Msg->GetASDUDefaultSmpsNum();
                if (nSmvChannels != NET_9_1_CHANNELS_PER_FRAME)
                {
                    RETAILMSG(OUTPUT_LOG_MSG, (_T("9-1 Channel Count(%d) \r\n"), nSmvChannels));
                    goto errorHandle;
                }
                stSmvValueFt3_91.nChannelCount = nSmvChannels;

                //记录实时报文通道数目
                {
                    AutoLock autoLock(m_lockRealTimeChannelCounts);
                    m_nRealTimeChannelCounts[0] = nSmvChannels;
                }

                uint16_t noASDU = pSav91Msg->GetNoASDU();
                for ( uint16_t j = 0; j < noASDU; ++j)
                {
                    pSav91Msg->GetSmpCntInASDU(j, nSmpCnt);
                    pSav91Msg->GetStatusWordsInASDU(j, statusWord1, statusWord2);

                    stSmvValueFt3_91.nSmpCnt = nSmpCnt;

                    //保存通道状态信息
                    m_lockCtrlBlkQuality.Acquire();
                    m_cSmvCtrlBlkQuality.SetCtrlBlk(pCurrentCtrlBlk);
                    m_cSmvCtrlBlkQuality.SetCtrlBlkStatusWords(statusWord1, statusWord2);
                    m_lockCtrlBlkQuality.Release();

                    if (DSM_MODE_SMV_RECV_STATUS_WATCH == enOpMode)
                    {
                        continue;
                    }

#if DSM_DSP_CALC_128K_RESAMPLE_TO_4K
                    if (_IsModeReSampleTo4k(enOpMode))
                    {
                        //对12.8k采样率进行重采样到4k
                        if (stSmvValueFt3_91.pDetectItem->nSampleRate == 12800)
                        {
                            uint32_t nSafeSmpCnt = nSmpCnt % 12800;
                            if (smp12800to4000[nSafeSmpCnt])
                            {
                                stSmvValueFt3_91.nSmpCnt = smp12800to4000[nSafeSmpCnt] - 1;
                            }
                            else
                            {
                                continue;
                            }
                        }
                    }
#endif

                    pSav91Msg->GetASDUSmps(j, reinterpret_cast<uint16_t*>(stSmvValueFt3_91.nSmvValue), nSmvChannels);
                    if(_SmvValueConvert(stSmvValueFt3_91, stSmvValue, enSmvDispType, enFrameValueType))
                    {
                        _SmvHandle(enOpMode, stSmvValue, pCurrentCtrlBlk);
                    }

                }
            }
            else if (FRAME_TYPE_NET_9_2 == enFrameType)
            {
                if (0 == pSav92Msg->Decode(pFrame, nFrameLen))
                {
                    RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_SaveNetSmv(): pSav92Msg->Decode Failed \n ")));
                    continue;
                }

                uint16_t noASDU = pSav92Msg->GetNoASDU();
                for ( uint16_t j = 0; j < noASDU; ++j)
                {
                    size_t nChannelCount = pSav92Msg->GetASDUSmpsNum(j);

                    //记录实时报文通道数目
                    {
                        AutoLock autoLock(m_lockRealTimeChannelCounts);
                        m_nRealTimeChannelCounts[0] = nChannelCount;
                    }

                    if (nChannelCount > NET_9_2_LE_MAX_CHANNELS_PER_ASDU)
                    {
                        RETAILMSG(OUTPUT_LOG_MSG, (_T("9-2 Channel Count(%d)\r\n"), nChannelCount));
                        continue;;
                    }
                    stSmvValue92.nChannelCount = nChannelCount;

                    pSav92Msg->GetSmpCntInASDU(j, nSmpCnt);
                    pSav92Msg->GetSmpSynchInASDU(j, bSmpSync);
                    stSmvValue92.nSmpCnt = nSmpCnt;

                    pSav92Msg->GetASDUSmps(j, reinterpret_cast<uint32_t*>(stSmvValue92.nSmvValue), p92SmvQlt, nChannelCount);

                    //保存通道状态信息
                    m_lockCtrlBlkQuality.Acquire();
                    m_cSmvCtrlBlkQuality.SetCtrlBlk(pCurrentCtrlBlk);
                    m_cSmvCtrlBlkQuality.SetMuSync(bSmpSync);
                    for (uint16_t k = 0; k < nChannelCount; ++k)
                    {
                        m_cSmvCtrlBlkQuality.SetChannelQuality(k, p92SmvQlt[k]);
                    }
                    m_lockCtrlBlkQuality.Release();

                    if (DSM_MODE_SMV_RECV_STATUS_WATCH == enOpMode)
                    {
                        continue;
                    }

#if DSM_DSP_CALC_128K_RESAMPLE_TO_4K
                    if (_IsModeReSampleTo4k(enOpMode))
                    {
                        //对12.8k采样率进行重采样到4k
                        if (stSmvValue92.pDetectItem->nSampleRate == 12800)
                        {
                            uint32_t nSafeSmpCnt = nSmpCnt % 12800;
                            if (smp12800to4000[nSafeSmpCnt])
                            {
                                stSmvValue92.nSmpCnt = smp12800to4000[nSafeSmpCnt] - 1;
                            }
                            else
                            {
                                continue;
                            }
                        }
                    }
#endif

                    if(_SmvValueConvert(stSmvValue92, stSmvValue, enSmvDispType, enFrameValueType))
                    {
                        _SmvHandle(enOpMode, stSmvValue, pCurrentCtrlBlk);
                    }
                }
            }
        }
        else
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_SaveNetSmv: Get Frame Content Failed \n")));
        }
    }
errorHandle:
    return;
}

///
/// @brief
///    保存手动试验、状态序列测试时GOOSE的DI值
///
/// @param[in]  pNet    以太网数据接收类
///
void CFrameRecv::_NetSaveGooseDI(CFrameRecvNet* pNet)
{
    BufferHeader*           pBufferHeader   = NULL;
    uint8_t*                pBufferData     = NULL;
    GooseMsg*               pGooseMsg       = NULL;
    uint32_t                nFrameCount     = 0;
    uint32_t                nFrameTotalLen  = 0;


    // 获得GOOSE接收参数配置管理器
    CParamConfigGOOSERecv* pParamConfigGooseRecv = CParamConfigGOOSERecv::getInstance();
    if (NULL == pParamConfigGooseRecv)
    {
        return ;
    }

    // 对共享缓冲区进行加锁处理
    pNet->LockBuffer();

    // 获得帧数和数据帧总长度
    pBufferHeader   = reinterpret_cast<BufferHeader*>(pNet->m_pDataBuf);
    pGooseMsg       = pNet->m_pGooseMsg;                                ///< Goose数据解析
    nFrameCount     = pBufferHeader->NetHeader.nFramesCount;            ///< 帧数
    nFrameTotalLen  = pBufferHeader->NetHeader.nActiveLength;           ///< 接收帧总长度

    // 根据循环接收标志，获得当前数据区头指针
    if (0 == pBufferHeader->NetHeader.nLoopItem)
    {
        pBufferData = pNet->m_pDataBuf + sizeof(BufferHeader);
    }
    else
    {
        pBufferData = pNet->m_pDataBuf + sizeof(BufferHeader) + NET_RECV_LOOP_BUF1_POS;
    }

    // 将所有接收帧保存到临时缓冲区
    CNetRecvBuffer  recvBuffer(pBufferData, nFrameCount, nFrameTotalLen);

    // 开始逐帧分析
    FrameInfo               stFrameInfo;
    IEC8802_3_PDU_PUBLIC    stIec8802Info;
    uint32_t                nFrameLen   = 0;
    uint8_t*                puiFrame    = NULL;
    for ( uint32_t iFrameIndex = 0; iFrameIndex < nFrameCount; iFrameIndex++)
    {
        // 获得一帧数据
        if (recvBuffer.GetBufferFrameAddr(iFrameIndex, &puiFrame, nFrameLen) &&
            recvBuffer.GetBufferFrameInfo(iFrameIndex, stFrameInfo))
        {
            // 解码一帧数据
            if (0 == pGooseMsg->Decode(puiFrame, nFrameLen))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_NetGooseAnaly: pGooseMsg->Decode Failed \n ")));
                continue;
            }

            // 设置通道的实时值
            pGooseMsg->GetIEC8802Info(stIec8802Info);
            pParamConfigGooseRecv->SetChannelRecvValue(stIec8802Info.appID, pGooseMsg, stFrameInfo);
        }
        else
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_NetGooseAnaly: Get Frame Content Failed \n")));
        }
    }

    // 对共享缓冲区进行解锁处理
    pNet->UnLockBuffer();
}

///
/// @brief
///    分析goose数据
///
void CFrameRecv::_NetGooseAnaly( CFrameRecvNet * pNet, DSM_OPERATE_MODE enOpMode, CNetDetectItem* pCurrentDetct)
{
    if (NULL == pNet)
    {
        return;
    }
    if (NULL == GetCurrentCtrlBlk() || !_IsPortInSelecedCtrlBlk(pNet->m_enPortName))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_NetGooseAnaly : Current Control Block Empty or pNet->m_enPortName Not match\n")));
        return;
    }

    uint32_t nFrameLen   = 0;
    GooseMsg* pGooseMsg  = pNet->m_pGooseMsg;
    uint8_t* pFrame      = NULL;
    MsgRapidDecoder& msgRapidDecoder = pNet->m_cMsgRapidDecoder;

    BufferHeader* pBufferHeader = reinterpret_cast<BufferHeader*>(pNet->m_pDataBuf);
    //对共享缓冲区进行加锁处理
    pNet->LockBuffer();
    uint32_t nFrameCount = pBufferHeader->NetHeader.nFramesCount;
    uint32_t BufferLen =  pBufferHeader->NetHeader.nActiveLength;
    uint32_t nLoopItem = pBufferHeader->NetHeader.nLoopItem;
    uint8_t* pBufferData = NULL;
    if (0 == nLoopItem)
    {
        pBufferData = pNet->m_pDataBuf + sizeof(BufferHeader);
    }
    else if (1 == nLoopItem)
    {
        pBufferData = pNet->m_pDataBuf + sizeof(BufferHeader) + NET_RECV_LOOP_BUF1_POS;
    }

    uint8_t* pAllocRecvBuffer = NULL;
#if DRIVER_BUFFER_USE_CPY
    pAllocRecvBuffer = new uint8_t[BufferLen];
    DWORD dwStart = 0, timeCopy = 0;
    dwStart = GetTickCount();
    memcpy_s(pAllocRecvBuffer, BufferLen, pBufferData, BufferLen);
    timeCopy = GetTickCount() - dwStart;
    CNetRecvBuffer recvBuffer(pAllocRecvBuffer, nFrameCount, BufferLen);
#else
    CNetRecvBuffer recvBuffer(pBufferData, nFrameCount, BufferLen);
#endif

    uint16_t nAppid = 0;
    GOOSE_PDU_PUBLIC stGoosePduPub;
    CNetDetectItem* pCurrentFrameMatchCtrlBlk = NULL;

    for ( uint32_t i = 0; i < nFrameCount; ++i)
    {
        if (recvBuffer.GetBufferFrameAddr(i, &pFrame, nFrameLen))
        {
            msgRapidDecoder.GetAppID(pFrame, nFrameLen, nAppid);

            //如果当前缓存的数据块指针变为空，说明可能进行了模式切换/重新探测等操作导致当前正在处理的控制块指针失效
            if (_GetCachedCtrlBlk() != pCurrentDetct)
            {
                goto errorHandle;
            }
            else
            {
                if (!_IsAppidInNetFilter(nAppid, pNet->m_enPortName, &pCurrentFrameMatchCtrlBlk))
                {
                    //当前数据帧的appid/mac与选择的控制块的appid/mac不同，处理下一数据帧
                    continue;
                }
            }

            //到达此位置都是在过滤列表的控制块数据，可能包含当前控制块之外的数据

            //变位列表一直统计所有过滤的控制块数据
            {
                if (0 == pGooseMsg->Decode(pFrame, nFrameLen))
                {
                    RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_NetGooseAnaly: pGooseMsg->Decode Failed \n ")));
                    continue;
                }
                pGooseMsg->GetPduPublicInfo(stGoosePduPub);
                uint32_t nChannelCount = pGooseMsg->GetChannelNum();

                if (pCurrentFrameMatchCtrlBlk->nID == GetCurrentCtrlBlk()->nID)
                {
                    AutoLock autoLock(m_lockRealTimeChannelCounts);
                    m_nRealTimeChannelCounts[0] = nChannelCount;
                }

                //获取报文接收时间戳，作为变位列表时间基准
                FrameInfo stFrameInfo;
                recvBuffer.GetBufferFrameInfo(i, stFrameInfo);

                AutoLock autoLock(m_lockGooseChangeList);
                MapDetectItemChangeList::iterator itrMap = m_mapGooseChangeList.find(pCurrentFrameMatchCtrlBlk);
                if (itrMap != m_mapGooseChangeList.end())
                {
                    uint32_t nSize = itrMap->second->GetGooseInfoCounts();
                    if (nSize > 0)
                    {
                        CGooseInfo* pGooseInfo = itrMap->second->GetOneGooseInfo(nSize - 1);
                        if ( pGooseInfo && pGooseInfo->GetGooseStNum() != stGoosePduPub.stNum )
                        {
                            // 当前控制块对应的变位列表是空的 或者
                            // 当前控制块变位列表已经有其他变位信息，但是已经有新的变位信息时，保存变位信息
                            CGooseInfo* pGooseInfo = new CGooseInfo;;
                            pGooseInfo->SetCtrlBlk(pCurrentFrameMatchCtrlBlk);
                            pGooseInfo->SetGooseTTL(stGoosePduPub.timeAllowedtoLive);
                            pGooseInfo->SetGooseStNum(stGoosePduPub.stNum);
                            pGooseInfo->SetGooseSqNum(stGoosePduPub.sqNum);
                            pGooseInfo->SetEventTime(stFrameInfo.timeStamp.ts_sec, stFrameInfo.timeStamp.ts_usec * 1000, stGoosePduPub.eventTm.ts_q);

                            CGooseChannel  stGooseChannel;
                            for (uint32_t i = 0; i < nChannelCount; ++i)
                            {
                                GOOSE_DATA* pGooseData = pGooseMsg->GetAChannel(i);
                                if (pGooseData)
                                {
                                    //暂且只设置通道数据，通道描述，类型需要从scl文件获取的信息在界面需要获取数据时从scl中获取
                                    stGooseChannel.SetChannelID(i);
                                    stGooseChannel.SetChannelData(pGooseData);
                                    pGooseInfo->AddChannel(&stGooseChannel);
                                }
                            }
                            CGooseInfo::FillGooseChannelDescAndType(pGooseInfo);
                            itrMap->second->AddGooseInfo(pGooseInfo);
                        }
                    }
                }
                else
                {
                    //当前的控制块还没有变位信息
                    CGooseInfo* pGooseInfo = new CGooseInfo;;
                    pGooseInfo->SetCtrlBlk(pCurrentFrameMatchCtrlBlk);
                    pGooseInfo->SetGooseTTL(stGoosePduPub.timeAllowedtoLive);
                    pGooseInfo->SetGooseStNum(stGoosePduPub.stNum);
                    pGooseInfo->SetGooseSqNum(stGoosePduPub.sqNum);
                    pGooseInfo->SetEventTime(stFrameInfo.timeStamp.ts_sec, stFrameInfo.timeStamp.ts_usec * 1000, stGoosePduPub.eventTm.ts_q);

                    CGooseChannel  stGooseChannel;
                    for (uint32_t i = 0; i < nChannelCount; ++i)
                    {
                        GOOSE_DATA* pGooseData = pGooseMsg->GetAChannel(i);
                        if (pGooseData)
                        {
                            //暂且只设置通道数据，通道描述，类型需要从scl文件获取的信息在界面需要获取数据时从scl中获取
                            stGooseChannel.SetChannelID(i);
                            stGooseChannel.SetChannelData(pGooseData);
                            pGooseInfo->AddChannel(&stGooseChannel);
                        }
                    }
                    CGooseChangeListData* pGooseChangeListData = new CGooseChangeListData;
                    CGooseInfo::FillGooseChannelDescAndType(pGooseInfo);
                    pGooseChangeListData->AddGooseInfo(pGooseInfo);
                    m_mapGooseChangeList.insert(std::make_pair(pCurrentFrameMatchCtrlBlk, pGooseChangeListData));
                }
            }


            //goose报文统计,只统计当前控制块
            if (enOpMode == DSM_MODE_GOOSE_RECV_FRAME_STATIC && pCurrentFrameMatchCtrlBlk->nID == pCurrentDetct->nID)
            {
                //获取以太网帧到达时间,进行离散度数据统计
                //只有在离散度页面才统计数据
                FrameInfo stFrameInfo;
                if (recvBuffer.GetBufferFrameInfo(i, stFrameInfo))
                {
                    _GenGooseFrameStatistics( pNet, pFrame, nFrameLen, stFrameInfo.timeStamp, pCurrentDetct);
                }
                continue;
            }

            //获取实时报文&获取实时值(保存报文，界面获取的时候解析报文获取实时值)
            if (enOpMode == DSM_MODE_GOOSE_RECV_REAL_TIME_MSG
                || enOpMode == DSM_MODE_GOOSE_RECV_REALTIME_VAL)
            {
                if (pCurrentFrameMatchCtrlBlk->nID == pCurrentDetct->nID )
                {
                    m_lockRealTimeMsg.Acquire();
                    m_cRealTimeMsg.m_pCtrlBlk   = pCurrentFrameMatchCtrlBlk;
                    m_cRealTimeMsg.m_enFrameType = FRAME_TYPE_NET_GOOSE;
                    m_cRealTimeMsg.m_nFrameLen = nFrameLen;
                    memcpy_s(m_cRealTimeMsg.m_nDataBuffer, NET_MAX_FRAME_LEN, pFrame, nFrameLen);
                    m_lockRealTimeMsg.Release();
                }
                
            }
        }
        else
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_NetGooseAnaly: Get Frame Content Failed \n")));
        }
    }

errorHandle:
    //对共享缓冲区进行解锁处理
    pNet->UnLockBuffer();
    if (NULL != pAllocRecvBuffer)
    {
        delete[] pAllocRecvBuffer;
        pAllocRecvBuffer = NULL;
    }
}

///
/// @brief
///    保存采样值到双端队列里
///
/// @note
///     为了防止队列无限增长，限制大小为波形图所需数据
///     队列resize时，如果本身尺寸超过设置的大小，则会删除接近队列尾端的数据
///     为了让缓冲一直保持最新数据，所以每次将数据插入队列头部，时间比较久的数据
///     会被删除，取数据的时候需要注意，队列尾部的数据是时间早一些的数据。
///
void CFrameRecv::_AddSmv2Deque( SMV_VALUE& stSmvValue )
{
#if OSCILLOGRAM_USE_STL_DEQUE
    uint32_t nSampleCaced = _GetCtrlBlkSampleRate(GetCurrentCtrlBlk());
#if DSM_DSP_CALC_128K_RESAMPLE_TO_4K
    if (nSampleCaced == 12800)
    {
        nSampleCaced = 4000;
    }
#endif
    //队列缓存10+2个周波的数据，多2个周波为了取特定采样点其实的波形数据
    nSampleCaced = nSampleCaced * (DSM_MAX_OSCILLOGRAM_WAVE_COUNT + 2)/DSM_CONFIG_DEFAULT_ELEC_FREQUENCY;
    m_lockSmvValuesDeque.Acquire();
    m_dequeSmvValues.push_front(stSmvValue);
    if (m_dequeSmvValues.size() > nSampleCaced)
    {
        //删除尾端老数据
        m_dequeSmvValues.resize(nSampleCaced);
    }
    m_lockSmvValuesDeque.Release();
#else
    m_cOscillogramBuf.AddValue(stSmvValue);
#endif
}

///
/// @brief
///    保存双AD数据
///
void CFrameRecv::_AddSmv2DoubleAD( SMV_VALUE& stSmvValue )
{
    AutoLock autLock(m_lockDoubleADData);
    m_stDoubleADData = stSmvValue;
}

///
/// @brief
///    根据模式，对采样值进行处理
///
void CFrameRecv::_SmvHandle( DSM_OPERATE_MODE enOpMode, SMV_VALUE& stSmvValue, CBaseDetectItem* pCurrentCtrlBlk)
{
    switch(enOpMode)
    {
    //在波形图，保存采样数据,同时需要dsp计算有效值
    case DSM_MODE_SMV_RECV_OSCILLOGRAM:
        {
#if DSM_OSCILLOGRAM_SHOW_EFFECT_PHASE
            _AddSmv4DspSend(enOpMode, stSmvValue, pCurrentCtrlBlk);
#endif
            _AddSmv2Deque(stSmvValue);
        }
        break;
    //双ad需要采样值
    case DSM_MODE_SMV_RECV_DOUBLE_AD:
        _AddSmv2DoubleAD(stSmvValue);
        break;
    //以下状态需要dsp进行数据运算，获取有效值，谐波等信息
    case DSM_MODE_SMV_RECV_EFFECTIVE_VALUE:
    case DSM_MODE_SMV_RECV_PHASOR:
    case DSM_MODE_SMV_RECV_XU_LIANG:
    case DSM_MODE_SMV_RECV_POWER:
    case DSM_MODE_SMV_RECV_HARMONIC:
         _AddSmv4DspSend(enOpMode, stSmvValue, pCurrentCtrlBlk);
         break;
    //极性检查
    case DSM_MODE_SMV_RECV_POLARITY:
        _PolarityCheck(stSmvValue, pCurrentCtrlBlk);
        break;
    case DSM_MODE_SMV_RECV_STATUS_WATCH:
    case DSM_MODE_SMV_RECV_FRAME_MONITOR:
    case DSM_MODE_SMV_RECV_FRAME_STATIC:
    case DSM_MODE_SMV_RECV_DISPERSION:
    case DSM_MODE_SMV_RECV_PHASE_CHECK:
        break;
    default:
        break;
    }
}

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
bool CFrameRecv::GetEffectiveVal( CDataEffectiveVal* pData )
{
    CBaseDetectItem* pCachedItem = _GetCachedCtrlBlk();
    if (!pData || !pCachedItem)
    {
        return false;
    }

    m_cDspDataSet.GetCalcData(m_stSmvDataCalcRt);
    uint32_t nChannelCount = m_stSmvDataCalcRt.nChannelCount;
    if (nChannelCount == 0)
    {
        return false;
    }

    //清空原有数据信息
    pData->ClearChannelVec();

    //需要先设置控制块信息，再添加通道，否则可能添加通道失败
    pData->SetCtrlBlk(pCachedItem);

    CEffectiveChanelInfo stEffectiveChannelInfo;
    m_lockCtrlBlkQuality.Acquire();
    for (size_t i = 0; i < nChannelCount; ++i)
    {
        stEffectiveChannelInfo.m_nIndex = i;
        stEffectiveChannelInfo.m_bChannelValid = m_cSmvCtrlBlkQuality.IsChannelValid(i);

        //通道有效值采用基波（一次谐波）有效值
        stEffectiveChannelInfo.m_fEffectiveValue = m_stSmvDataCalcRt.fRmsValue[i];
        stEffectiveChannelInfo.m_bIsDcValue      = (fabs(m_stSmvDataCalcRt.fHarmContent[i][0]) < 0.0001) ? true : false;
        if (stEffectiveChannelInfo.m_bIsDcValue)
        {
            stEffectiveChannelInfo.m_fPhase          = 0.0f;
        }
        else
        {
            stEffectiveChannelInfo.m_fPhase          = m_stSmvDataCalcRt.fHarmAngle[i][0];
        }

        pData->AddOneChannelInfo(stEffectiveChannelInfo);
    }
    m_lockCtrlBlkQuality.Release();
    return true;
}

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
bool CFrameRecv::GetOscillogramData( CDataOscillogram* pData )
{
    if (!pData || !_GetCachedCtrlBlk())
    {
        return false;
    }

#if OSCILLOGRAM_USE_STL_DEQUE
    std::deque<SMV_VALUE>::reverse_iterator ritr;
    m_lockSmvValuesDeque.Acquire();
    uint32_t nDequeSize = m_dequeSmvValues.size();
    if (nDequeSize == 0)
    {
        m_lockSmvValuesDeque.Release();
        return false;
    }

    //从队列的尾端（最老数据）开始取数据
    ritr = m_dequeSmvValues.rbegin();
    SMV_VALUE& stSmvValue  = (*ritr);
    size_t nChannelCount   = stSmvValue.nChannelCount;
    uint32_t nPointPerWave = (stSmvValue.pDetectItem != NULL) ? stSmvValue.pDetectItem->nSampleRate/DSM_CONFIG_DEFAULT_ELEC_FREQUENCY : 80;
#if DSM_DSP_CALC_128K_RESAMPLE_TO_4K
    if (nPointPerWave == 256)
    {
        nPointPerWave = 80;
    }
#endif //DSM_DSP_CALC_128K_RESAMPLE_TO_4K

    //计算第一个采样点与需要的采样点的距离
    uint32_t nDistance = _GetDesireSmpDistance(stSmvValue.nSmpCnt, 0, nPointPerWave);
//    RETAILMSG(1, (_T("nPW(%d), nSmpCnt(%d), nDis(%d), nDeSize(%d),"), nPointPerWave, stSmvValue.nSmpCnt, nDistance, nDequeSize));

    advance(ritr, min(nDistance, nDequeSize));

    static COscillogramChannelInfo pChannelInfo[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];
    for (uint32_t i=0; i<NET_9_2_LE_MAX_CHANNELS_PER_ASDU; ++i)
    {
        pChannelInfo[i].m_vecChannelData.reserve(nDequeSize);
    }

    uint32_t nChannelSmpCount = nPointPerWave * DSM_MAX_OSCILLOGRAM_WAVE_COUNT;
    for(uint32_t j=0; ritr != m_dequeSmvValues.rend() && j < nChannelSmpCount ; ++ritr, ++j)
    {
        for (size_t i = 0; i < nChannelCount; ++i)
        {
            pChannelInfo[i].m_vecChannelData.push_back((*ritr).fSmvValue[i]);
        }
    }
    m_lockSmvValuesDeque.Release();

    //清空原有数据信息
    pData->ClearChannelVec();

    //需要先设置控制块信息，再添加通道，否则可能添加通道失败
    pData->SetCtrlBlk(_GetCachedCtrlBlk());
#if DSM_OSCILLOGRAM_SHOW_EFFECT_PHASE
    m_cDspDataSet.GetCalcData(m_stSmvDataCalcRt);
    m_lockCtrlBlkQuality.Acquire();
    for (size_t i = 0; i < nChannelCount; ++i)
    {
        pChannelInfo[i].m_nIndex = i;
        pChannelInfo[i].m_bChannelValid = m_cSmvCtrlBlkQuality.IsChannelValid(i);

        if (i < m_stSmvDataCalcRt.nChannelCount)
        {
            //通道有效值采用基波（一次谐波）有效值
            pChannelInfo[i].m_fEffectiveValue = m_stSmvDataCalcRt.fRmsValue[i];
            pChannelInfo[i].m_bIsDcValue      = (fabs(m_stSmvDataCalcRt.fHarmContent[i][0]) < 0.0001) ? true : false;
            if (pChannelInfo[i].m_bIsDcValue)
            {
                pChannelInfo[i].m_fPhase          = 0.0f;
            }
            else
            {
                pChannelInfo[i].m_fPhase          = m_stSmvDataCalcRt.fHarmAngle[i][0];
            }
        }
        else
        {
            pChannelInfo[i].m_fEffectiveValue = 0.0f;
            pChannelInfo[i].m_fPhase          = 0.0f;
        }
        pData->AddOneChannelInfo(pChannelInfo[i]);
    }
    m_lockCtrlBlkQuality.Release();
#else //DSM_OSCILLOGRAM_SHOW_EFFECT_PHASE
    for (size_t i = 0; i < nChannelCount; ++i)
    {
        pChannelInfo[i].m_nIndex = i;
        pData->AddOneChannelInfo(pChannelInfo[i]);
    }
#endif //DSM_OSCILLOGRAM_SHOW_EFFECT_PHASE
    return true;

#else //OSCILLOGRAM_USE_STL_DEQUE
    uint32_t nSmpCount = 0;
    uint32_t nIndex = 0;
    static COscillogramChannelInfo pChannelInfo[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];
    if (m_cOscillogramBuf.GetSmpValue(nSmpCount, nIndex))
    {
        CBaseDetectItem* pDetectItem = m_cOscillogramBuf.GetCtrlBlk();
        if (!pDetectItem)
        {
            m_cOscillogramBuf.ReleaseBufferLock();
            return false;
        }

        size_t nChannelCount   = pDetectItem->nChannelCount;
        uint32_t nPointPerWave = (pDetectItem != NULL) ? pDetectItem->nSampleRate/DSM_CONFIG_DEFAULT_ELEC_FREQUENCY : 80;
#if DSM_DSP_CALC_128K_RESAMPLE_TO_4K
        if (nPointPerWave == 256)
        {
            nPointPerWave = 80;
        }
#endif //DSM_DSP_CALC_128K_RESAMPLE_TO_4K
        uint32_t nDistance = _GetDesireSmpDistance(m_cOscillogramBuf.m_nBufBeginSmpCnt[nIndex], 0, nPointPerWave);
        if (nDistance < nSmpCount)
        {
            uint32_t nDataCount = min(nPointPerWave*10, nSmpCount - nDistance);
//            RETAILMSG(1, (_T("nDistance(%d)/nDataCount(%d)\r\n"), nDistance, nDataCount));
            for (uint32_t i=0; i<nChannelCount; ++i)
            {
                float* fbegin =  &m_cOscillogramBuf.m_pBuf[nIndex][i][0];
                fbegin = fbegin + nDistance;
                float* fend   =  fbegin + nDataCount;
                pChannelInfo[i].m_vecChannelData.assign(fbegin, fend);
            }
        }
        else
        {
//            RETAILMSG(1, (_T("nDistance(%d)/nDataCount(%d)\r\n"), nDistance, nSmpCount));
            for (uint32_t i=0; i<nChannelCount; ++i)
            {
                float* fbegin =  &m_cOscillogramBuf.m_pBuf[nIndex][i][0];
                float* fend   =  fbegin + nSmpCount;
                pChannelInfo[i].m_vecChannelData.assign(fbegin, fend);
            }
        }
        m_cOscillogramBuf.ReleaseBufferLock();

        //清空原有数据信息
        pData->ClearChannelVec();

        //需要先设置控制块信息，再添加通道，否则可能添加通道失败
        pData->SetCtrlBlk(pDetectItem);

#if  DSM_OSCILLOGRAM_SHOW_EFFECT_PHASE
        m_cDspDataSet.GetCalcData(m_stSmvDataCalcRt);
        m_lockCtrlBlkQuality.Acquire();
        for (size_t i = 0; i < nChannelCount; ++i)
        {
            pChannelInfo[i].m_nIndex = i;
            pChannelInfo[i].m_bChannelValid = m_cSmvCtrlBlkQuality.IsChannelValid(i);
            //通道有效值采用基波（一次谐波）有效值
            if (i < m_stSmvDataCalcRt.nChannelCount)
            {
                //通道有效值采用基波（一次谐波）有效值
                pChannelInfo[i].m_fEffectiveValue = m_stSmvDataCalcRt.fRmsValue[i];
                pChannelInfo[i].m_bIsDcValue      = (fabs(m_stSmvDataCalcRt.fHarmContent[i][0]) < 0.0001) ? true : false;
                if (pChannelInfo[i].m_bIsDcValue)
                {
                    pChannelInfo[i].m_fPhase          = 0.0f;
                }
                else
                {
                    pChannelInfo[i].m_fPhase          = m_stSmvDataCalcRt.fHarmAngle[i][0];
                }
            }
            else
            {
                pChannelInfo[i].m_fEffectiveValue = 0.0f;
                pChannelInfo[i].m_fPhase          = 0.0f;
            }
            pData->AddOneChannelInfo(pChannelInfo[i]);
        }
        m_lockCtrlBlkQuality.Release();
#else // DSM_OSCILLOGRAM_SHOW_EFFECT_PHASE
        for (size_t i = 0; i < nChannelCount; ++i)
        {
            pChannelInfo[i].m_nIndex = i;
            pData->AddOneChannelInfo(pChannelInfo[i]);
        }
#endif // DSM_OSCILLOGRAM_SHOW_EFFECT_PHASE
        return true;
    }
    return false;
#endif //OSCILLOGRAM_USE_STL_DEQUE

}

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
bool CFrameRecv::GetPowerData( CDataPower* pData )
{
    if (!pData || !_GetCachedCtrlBlk())
    {
        return false;
    }

    if (!GetEffectiveVal(pData))
    {
        return false;
    }

    pData->ClearPowerData();
    m_cDspDataSet.GetCalcData(m_stSmvDataCalcRt);
    if (m_stSmvDataCalcRt.nChannelCount != 0)
    {
        for (int i = 0; i < 3; ++i)
        {
            pData->m_fActivePower[i]    = m_stSmvDataCalcRt.fActivePower[i];
            pData->m_fReactivePower[i]  = m_stSmvDataCalcRt.fReactivePower[i];
            pData->m_fApparentPower[i]  = m_stSmvDataCalcRt.fApparentPower[i];
            pData->m_fPowerFactor[i]    = m_stSmvDataCalcRt.fPowerFactor[i];
        }
        pData->m_fTotalActivePower      = m_stSmvDataCalcRt.fTotalActivePower;
        pData->m_fTotalReactivePower    = m_stSmvDataCalcRt.fTotalReactivePower;
        pData->m_fTotalApparentPower    = m_stSmvDataCalcRt.fTotalApparentPower;
        pData->m_fTotalPowerFactor      = m_stSmvDataCalcRt.fTotalPowerFactor;
    }
    return true;
}

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
bool CFrameRecv::GetDoubleAdData( CDataDoubleAD* pData )
{
    if (!pData || !_GetCachedCtrlBlk())
    {
        return false;
    }

    AutoLock autoLock(m_lockDoubleADData);
    if (m_stDoubleADData.pDetectItem)
    {
        pData->SetCtrlBlk(m_stDoubleADData.pDetectItem);
        pData->SetDoubleAdSmvValue(m_stDoubleADData);
        return true;
    }
    return false;
}

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
bool CFrameRecv::GetStatusWatchData( CDataStatusWatch* pData )
{
    if (pData)
    {
        m_lockCtrlBlkQuality.Acquire();
        pData->SetCtrlBlk(m_cSmvCtrlBlkQuality.GetCtrlBlk());
        pData->SetMuSync(m_cSmvCtrlBlkQuality.IsMuSync());
        
        switch(m_cSmvCtrlBlkQuality.GetFrameType())
        {
        case FRAME_TYPE_NET_9_1:
        case FRAME_TYPE_FT3_GUOWANG:
        case FRAME_TYPE_FT3_NANRUI:
            {
                uint16_t nStatusWord1 = 0, nStatusWord2 = 0;
                m_cSmvCtrlBlkQuality.GetCtrlBlkStatusWords(nStatusWord1, nStatusWord2);
                pData->SetCtrlBlkStatusWords(nStatusWord1, nStatusWord2);
            }
            break;
        case FRAME_TYPE_NET_9_2:
            {
                uint32_t nChannelCount = m_cSmvCtrlBlkQuality.GetChannelCount();
                uint32_t nChannelQuality = 0;
                for ( uint32_t i = 0; i < nChannelCount; ++i)
                {
                    m_cSmvCtrlBlkQuality.GetChannelQuality(i, nChannelQuality);
                    pData->SetChannelQuality(i, nChannelQuality);
                }
            }
            break;
        default:
            break;
        }
        m_lockCtrlBlkQuality.Release();
        return true;
    }
    return false;
}

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
bool CFrameRecv::GetXuliangData( CDataXuliang* pData )
{
    if (NULL == pData)
    {
        return false;
    }

    if (!GetEffectiveVal(pData))
    {
        return false;
    }

    pData->ClearXLData();
    m_cDspDataSet.GetCalcData(m_stSmvDataCalcRt);
    if (m_stSmvDataCalcRt.nChannelCount != 0)
    {
        pData->SetPhaseSeqContent(CHANNEL_TYPE_SMV_VOLTAGE, m_stSmvDataCalcRt.fPhaseSeqContent[0][0], m_stSmvDataCalcRt.fPhaseSeqContent[0][1], m_stSmvDataCalcRt.fPhaseSeqContent[0][2]);
        pData->SetPhaseSeqContent(CHANNEL_TYPE_SMV_CURRENT, m_stSmvDataCalcRt.fPhaseSeqContent[1][0], m_stSmvDataCalcRt.fPhaseSeqContent[1][1], m_stSmvDataCalcRt.fPhaseSeqContent[1][2]);
        pData->SetPhaseSeqAngle(CHANNEL_TYPE_SMV_VOLTAGE, m_stSmvDataCalcRt.fPhaseSeqAngle[0][0], m_stSmvDataCalcRt.fPhaseSeqAngle[0][1], m_stSmvDataCalcRt.fPhaseSeqAngle[0][2]);
        pData->SetPhaseSeqAngle(CHANNEL_TYPE_SMV_CURRENT, m_stSmvDataCalcRt.fPhaseSeqAngle[1][0], m_stSmvDataCalcRt.fPhaseSeqAngle[1][1], m_stSmvDataCalcRt.fPhaseSeqAngle[1][2]);
        return true;
    }
    return false;
}

///
/// @brief
///    获取Smv接收公共配置接口
///
CSmvRecvConfigPublic* CFrameRecv::GetSmvConfigPublic()
{
    return m_pSmvRecvConfigPublic;
}

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
bool CFrameRecv::GetPhasorData( CDataPhasor* pData )
{
    return GetEffectiveVal(pData);
}

///
/// @brief
///    清除为波形图保存的采样值数据
///
void CFrameRecv::_ClearSmvDeque()
{
#if OSCILLOGRAM_USE_STL_DEQUE
    AutoLock autoLock(m_lockSmvValuesDeque);
    m_dequeSmvValues.clear();
#endif
}

///
/// @brief
///    获取当前控制块对应的通道状态信息
///
/// @return
///     CSmvCtrlBlkQuality* 返回当前控制块通道信息的一个对象指针
///
CSmvCtrlBlkQuality* CFrameRecv::GetCurrentCtrlBlkChannelStatus()
{
    static CSmvCtrlBlkQuality cSmvCtrlBlkQuality;
    m_lockCtrlBlkQuality.Acquire();
    cSmvCtrlBlkQuality = m_cSmvCtrlBlkQuality;
    m_lockCtrlBlkQuality.Release();
    if (cSmvCtrlBlkQuality.GetCtrlBlk() != GetCurrentCtrlBlk())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T(" CFrameRecv::GetCurrentCtrlBlkChannelStatus(): Current CtrlBlk Don't Match Channel Status CtrlBlk \n")));
    }
    return &cSmvCtrlBlkQuality;
}

///
/// @brief
///    启动dsp发送线程
///
bool CFrameRecv::StartDspSendProc()
{
    //创建操作dsp实例对象
    if (NULL == m_pDsp)
    {
        m_pDsp = new CDsp(&m_cDspDataSet);
        //给dsp设置初始化配置参数
        m_pDsp->SetConfig(m_cDspConfig);

        //开启dsp数据接收线程
        if (!m_pDsp->StartRead())
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::StartDspSendProc(): Dsp StartRead() Failed \n")));
            delete m_pDsp;
            m_pDsp = NULL;
            return false;
        }
    }

    //创建线程
    if (NULL == m_hDspSendProc)
    {
        DWORD dwThreadId = 0;
        m_hDspSendProc = CreateThread(NULL, 0, CFrameRecv::DspSendProc, this, NULL, &dwThreadId);
        if (NULL == m_hDspSendProc)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T(" CFrameRecv::StartDspSendProc(): Create DspSendProc Thread Failed \n")));
            return false;
        }
        else 
        {
#if 1
            if(CeSetThreadPriority(m_hDspSendProc, DSP_SEND_PROC_PRIO))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::StartDspSendProc():  DspSendProc(0x%08x) Current Priority %d\r\n"), dwThreadId, CeGetThreadPriority(m_hDspSendProc)));
            }
#endif
        }
    }
    return true;
}

///
/// @brief
///    停止DSP发送线程
///
bool CFrameRecv::StopDspSendProc()
{
    if (NULL != m_hDspSendProc)
    {
        if (NULL != m_hEventExitDspSendProc)
        {
            SetEvent(m_hEventExitDspSendProc);
        }
        DWORD dwRet =  WaitForSingleObject(m_hDspSendProc, 1000);   //等待(1s)线程退出
        if (dwRet == WAIT_OBJECT_0 )
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::StopDspSendProc() Thread Exit Success \n")));
        }
        else if(dwRet == WAIT_TIMEOUT)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::StopDspSendProc()  Wait Thread Exit 1s time out \n")));
            if (TerminateThread(m_hDspSendProc, 44))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("Thread Terminated Success \n")));
            }
            else
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("Thread Terminated Failed  \n")));
            }
        }
        else
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::StopDspSendProc() : Thread Exit Failed ,Wait Failed \n")));
            if (TerminateThread(m_hDspSendProc,44))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("Thread Terminated Success \n")));
            }
            else
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("Thread Terminated Failed   \n")));
            }
        }
        CloseHandle(m_hDspSendProc);
        m_hDspSendProc = NULL;
    }

    if (NULL != m_pDsp)
    {
        m_pDsp->StopRead();
        delete m_pDsp;
        m_pDsp = NULL;
    }

    return true;
}

///
/// @brief
///    dsp发送数据线程
///
DWORD WINAPI CFrameRecv::DspSendProc( LPVOID lpParameter )
{
    CFrameRecv* pFrameRecv = reinterpret_cast<CFrameRecv*>(lpParameter);
    DWORD dwRet = 0;
    HANDLE hEvent[] = {pFrameRecv->m_hEventExitDspSendProc, pFrameRecv->m_hEventSmv, pFrameRecv->m_hEventSmvTime};

    RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::DspSendProc() Started \n")));
    while (true)
    {
        //等待接收事件、线程退出事件
        dwRet = WaitForMultipleObjects(sizeof(hEvent)/sizeof(hEvent[0]), hEvent, FALSE, INFINITE);
        //退出事件
        if (dwRet == WAIT_OBJECT_0)
        {
            break;
        }
        //smv采样值准备好事件
        else if (dwRet == WAIT_OBJECT_0 + 1)
        {
#if 0
            static uint32_t nCount = 0;
            RETAILMSG(1, (_T("\r\nDsp Send Smv Count: %u , time: %d\r\n"), ++nCount, GetTickCount()));
#endif
            AutoLock autoLock(pFrameRecv->m_lockDspSend);
            pFrameRecv->m_pDsp->SendSmv(pFrameRecv->m_stDspSmvData);
            //发送完数据，置缓存数据为无效
            pFrameRecv->m_stDspSmvData.nSmvPerCircle = 0;
        }
        //smv时间发送事件
        else if (dwRet == WAIT_OBJECT_0 + 2)
        {
            AutoLock autoLock(pFrameRecv->m_lockSmvTime);
            MsgTimeData msgTimeData;
            size_t nmsgDequeSize = pFrameRecv->m_dequeSmvTime.size();
            size_t nTimes = min(nmsgDequeSize, MAX_MSG_TIME_PER_CIRCLE);
            memset(&msgTimeData, 0, sizeof(msgTimeData));
            for (uint32_t i = 0; i < nTimes; ++i)
            {
                msgTimeData.msgTimes[i] = pFrameRecv->m_dequeSmvTime.back();
                msgTimeData.nMsgTimeCount++;
                pFrameRecv->m_dequeSmvTime.pop_back();
            }
#if 0
            static uint32_t nCount = 0;
            RETAILMSG(1, (_T("Dsp Send Svm Time Count: (%u), Deque.size()=(%u) \r\n"), ++nCount, pFrameRecv->m_dequeSmvTime.size()));
#endif
            pFrameRecv->m_pDsp->SendMsgTime(msgTimeData);
        }
    }
    RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::DspSendProc() Stoped \n")));
    return 0;
}

///
/// @brief
///    分析国网Ft3数据
///
void CFrameRecv::_Ft3SmvAnalyGuowang(CFt3RecvBuffer& rRecvBuffer, DSM_OPERATE_MODE enOpMode, CFt3DetectItem* pCurrentCtrlBlk, CFrameRecvFt3* pFt3)
{
    GuoWangFT3Msg* pGuoWangFt3Msg = pFt3->m_pGuoWangFT3Msg;
    uint32_t nFrameCount = rRecvBuffer.GetBufferFrameCount();
    uint32_t nFrameLen = FT3_FRAME_LEN[_FT3_TYPE_GUOWANG];
    uint8_t* pFrame = NULL;
    SMV_VALUE stSmvValue;
    SMV_VALUE_FT3_9_1 stSmvValueFt3_91 = {0};
    stSmvValueFt3_91.pDetectItem = pCurrentCtrlBlk;

    //界面上需要显示的数据是原始值/一次/二次值
    enumSmvValueType  enSmvDispType = SMV_VALUE_TYPE_ORIGINANL;
    CSmvRecvConfigPublic* pSmvRecvCfgPub = CDsmDataLayer::getInstance()->GetSmvConfigPublic();
    if (pSmvRecvCfgPub)
    {
        enSmvDispType = pSmvRecvCfgPub->GetSmvDispType();
    }

    //设置的数据帧报文类型是一次/二次值
    enumSmvValueType enFrameValueType = SMV_VALUE_TYPE_SECONDARY;
    CParamConfigBasic* pParamConfigBasic = CDsmDataLayer::getInstance()->GetParamConfigBasic();
    if (pParamConfigBasic)
    {
        enFrameValueType = pParamConfigBasic->GetFrameValueType();
    }

    for ( uint32_t i=0; i < nFrameCount; ++i)
    {
        if (rRecvBuffer.GetBufferFrameAddr(i, &pFrame, nFrameLen))
        {
            if (DSM_MODE_SMV_RECV_REAL_TIME_MSG == enOpMode)
            {
                m_lockRealTimeMsg.Acquire();
                m_cRealTimeMsg.m_pCtrlBlk   = pCurrentCtrlBlk;
                m_cRealTimeMsg.m_enFrameType = FRAME_TYPE_FT3_GUOWANG;
                m_cRealTimeMsg.m_nFrameLen = nFrameLen;
                memcpy_s(m_cRealTimeMsg.m_nDataBuffer, NET_MAX_FRAME_LEN, pFrame, nFrameLen);
                m_lockRealTimeMsg.Release();
                continue;
            }

            if ( DSM_MODE_SMV_RECV_FRAME_STATIC  == enOpMode
                || DSM_MODE_SMV_RECV_DISPERSION == enOpMode)
            {
                FrameTimeStamp stFrameTimeStamp;
                if (rRecvBuffer.GetBufferFrameTimeStamp(i, stFrameTimeStamp))
                {
                    _AddSmvTime2Deque(stFrameTimeStamp, pCurrentCtrlBlk);
                    _GenSmvFrameStatistics(pFt3, pFrame, nFrameLen, stFrameTimeStamp, pCurrentCtrlBlk);
                }
                continue;
            }

            if (0 == pGuoWangFt3Msg->Decode(reinterpret_cast<uint16_t*>(pFrame), nFrameLen/2))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_Ft3SmvAnalyGuowang(): pGuoWangFt3Msg->Decode Failed \n ")));
                continue;
            }

            GUOWANG_FT3_ASDU_PUBLIC stAsduPbl;
            size_t nChannelCount = pGuoWangFt3Msg->GetASDUSmpsNum();

            //记录实时报文通道数目
            {
                AutoLock autoLock(m_lockRealTimeChannelCounts);
                m_nRealTimeChannelCounts[0] = nChannelCount;
            }

            if (nChannelCount != GUOWANG_CHANNELS_PER_FRAME)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("Guowang Channel Count(%d)\r\n"), nChannelCount));
                continue;
            }
            stSmvValueFt3_91.nChannelCount = nChannelCount;
            pGuoWangFt3Msg->GetASDUPublicInfo(stAsduPbl);
            stSmvValueFt3_91.nSmpCnt = stAsduPbl.smpCnt;

            //保存通道状态信息
            m_lockCtrlBlkQuality.Acquire();
            m_cSmvCtrlBlkQuality.SetCtrlBlk(pCurrentCtrlBlk);
            m_cSmvCtrlBlkQuality.SetCtrlBlkStatusWords(stAsduPbl.statusWord1, stAsduPbl.statusWord2);
            m_lockCtrlBlkQuality.Release();

            if (DSM_MODE_SMV_RECV_STATUS_WATCH == enOpMode)
            {
                continue;
            }

#if DSM_DSP_CALC_128K_RESAMPLE_TO_4K
            if (_IsModeReSampleTo4k(enOpMode))
            {
                //对12.8k采样率进行重采样到4k
                if (stSmvValueFt3_91.pDetectItem->nSampleRate == 12800)
                {
                    uint32_t nSafeSmpCnt = stAsduPbl.smpCnt % 12800;
                    if (smp12800to4000[nSafeSmpCnt])
                    {
                        stSmvValueFt3_91.nSmpCnt = smp12800to4000[nSafeSmpCnt] - 1;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
#endif

            pGuoWangFt3Msg->GetASDUSmps(reinterpret_cast<uint16_t*>(stSmvValueFt3_91.nSmvValue), nChannelCount);

            if ( _SmvValueConvert(stSmvValueFt3_91, stSmvValue, enSmvDispType, enFrameValueType) )
            {
                _SmvHandle(enOpMode, stSmvValue, pCurrentCtrlBlk);
            }
        }
        else
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_Ft3SmvAnalyGuowang(): recvBuffer.GetBufferFrameContent Failed \n")));
        }
    }
}

///
/// @brief
///    分析南瑞FT3数据
///
void CFrameRecv::_Ft3SmvAnalyNanrui( CFt3RecvBuffer& rRecvBuffer, DSM_OPERATE_MODE enOpMode, CFt3DetectItem* pCurrentCtrlBlk, CFrameRecvFt3* pFt3 )
{
    NanRuiFT3Msg*  pNanRuiFt3Msg = pFt3->m_pNanRuiFt3Msg;
    uint32_t nFrameCount = rRecvBuffer.GetBufferFrameCount();
    uint32_t nFrameLen = FT3_FRAME_LEN[_FT3_TYPE_NANRUI];
    uint8_t* pFrame = NULL;
    SMV_VALUE stSmvValue;
    SMV_VALUE_FT3_9_1 stSmvValueFt3_91 = {0};
    stSmvValueFt3_91.pDetectItem = pCurrentCtrlBlk;

    //界面上需要显示的数据是原始值/一次/二次值
    enumSmvValueType  enSmvDispType = SMV_VALUE_TYPE_ORIGINANL;
    CSmvRecvConfigPublic* pSmvRecvCfgPub = CDsmDataLayer::getInstance()->GetSmvConfigPublic();
    if (pSmvRecvCfgPub)
    {
        enSmvDispType = pSmvRecvCfgPub->GetSmvDispType();
    }

    //设置的数据帧报文类型是一次/二次值
    enumSmvValueType enFrameValueType = SMV_VALUE_TYPE_SECONDARY;
    CParamConfigBasic* pParamConfigBasic = CDsmDataLayer::getInstance()->GetParamConfigBasic();
    if (pParamConfigBasic)
    {
        enFrameValueType = pParamConfigBasic->GetFrameValueType();
    }

    for ( uint32_t i=0; i < nFrameCount; ++i)
    {
        if (rRecvBuffer.GetBufferFrameAddr(i, &pFrame, nFrameLen))
        {
            if (DSM_MODE_SMV_RECV_REAL_TIME_MSG == enOpMode)
            {
                m_lockRealTimeMsg.Acquire();
                m_cRealTimeMsg.m_pCtrlBlk   = pCurrentCtrlBlk;
                m_cRealTimeMsg.m_enFrameType = FRAME_TYPE_FT3_NANRUI;
                m_cRealTimeMsg.m_nFrameLen = nFrameLen;
                memcpy_s(m_cRealTimeMsg.m_nDataBuffer, NET_MAX_FRAME_LEN, pFrame, nFrameLen);
                m_lockRealTimeMsg.Release();
                continue;
            }

            if ( DSM_MODE_SMV_RECV_FRAME_STATIC  == enOpMode
                || DSM_MODE_SMV_RECV_DISPERSION == enOpMode)
            {
                FrameTimeStamp stFrameTimeStamp;
                if (rRecvBuffer.GetBufferFrameTimeStamp(i, stFrameTimeStamp))
                {
                    _AddSmvTime2Deque(stFrameTimeStamp, pCurrentCtrlBlk);
                    _GenSmvFrameStatistics(pFt3, pFrame, nFrameLen, stFrameTimeStamp, pCurrentCtrlBlk);
                }
                continue;
            }

            if (0 == pNanRuiFt3Msg->Decode(reinterpret_cast<uint16_t*>(pFrame), nFrameLen/2))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_Ft3SmvAnalyNanrui:pNanRuiFt3Msg->Decode Failed \n ")));
                continue;
            }

            NANRUI_FT3_ASDU_PUBLIC stAsduPbl;
            size_t nChannelCount = pNanRuiFt3Msg->GetASDUSmpsNum();

            //记录实时报文通道数目
            {
                AutoLock autoLock(m_lockRealTimeChannelCounts);
                m_nRealTimeChannelCounts[0] = nChannelCount;
            }

            if (nChannelCount != NANRUI_CHANNELS_PER_FRAME)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("NanRui Frame Channel Count(%d)\r\n"), nChannelCount));
                continue;
            }
            stSmvValueFt3_91.nChannelCount = nChannelCount;
            pNanRuiFt3Msg->GetASDUPublicInfo(stAsduPbl);
            stSmvValueFt3_91.nSmpCnt = stAsduPbl.smpCnt;

            //保存通道状态信息
            m_lockCtrlBlkQuality.Acquire();
            m_cSmvCtrlBlkQuality.SetCtrlBlk(pCurrentCtrlBlk);
            m_cSmvCtrlBlkQuality.SetCtrlBlkStatusWords(stAsduPbl.statusWord1, stAsduPbl.statusWord2);
            m_lockCtrlBlkQuality.Release();

            if (DSM_MODE_SMV_RECV_STATUS_WATCH == enOpMode)
            {
                continue;
            }

#if DSM_DSP_CALC_128K_RESAMPLE_TO_4K
            if (_IsModeReSampleTo4k(enOpMode))
            {
                //对12.8k采样率进行重采样到4k
                if (stSmvValueFt3_91.pDetectItem->nSampleRate == 12800)
                {
                    uint32_t nSafeSmpCnt = stAsduPbl.smpCnt % 12800;
                    if (smp12800to4000[nSafeSmpCnt])
                    {
                        stSmvValueFt3_91.nSmpCnt = smp12800to4000[nSafeSmpCnt] - 1;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
#endif

            pNanRuiFt3Msg->GetASDUSmps(reinterpret_cast<uint16_t*>(stSmvValueFt3_91.nSmvValue), nChannelCount);

            if (_SmvValueConvert(stSmvValueFt3_91, stSmvValue, enSmvDispType, enFrameValueType))
            {
                _SmvHandle(enOpMode, stSmvValue, pCurrentCtrlBlk);
            }
        }
        else
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_Ft3SmvAnalyNanrui: recvBuffer.GetBufferFrameContent Failed \n")));
        }
    }
}

///
/// @brief
///    添加采样值到dsp发送采样值缓冲，采样值到达一个周波数据时发送一次采样值给dsp
///
void CFrameRecv::_AddSmv4DspSend(DSM_OPERATE_MODE enOpMode, SMV_VALUE& stSmvValue, CBaseDetectItem* pCurrentCtrlBlk )
{
    if (_GetCachedCtrlBlk() != pCurrentCtrlBlk)
    {
        return;
    }

    //获取采样率信息
    uint32_t nSampleRate = _GetCtrlBlkSampleRate(pCurrentCtrlBlk);
#if DSM_DSP_CALC_128K_RESAMPLE_TO_4K
    if (_IsModeReSampleTo4k(enOpMode))
    {
        if (nSampleRate == 12800)
        {
            nSampleRate = 4000;
        }
    }
#endif

    //获取锁，使用 m_stDspSmvData使用权限
    m_lockDspSend.Acquire();
    if (m_stDspSmvData.nSmvPerCircle >= (nSampleRate/DSM_CONFIG_DEFAULT_ELEC_FREQUENCY))
    {
        //如果判断到缓存的采样值的采样数据没有被清零，表示dsp还没有发送，先直接返回
        SetEvent(m_hEventSmv);
        m_lockDspSend.Release();
        RETAILMSG(1,(_T("\r\nSmv send 2 dsp Droped !")));
        return;
    }
    uint32_t nChannelCountDsp = min(MAX_SMV_CHANNELS, stSmvValue.nChannelCount);
    for (size_t i = 0; i < nChannelCountDsp; ++i)
    {
        m_stDspSmvData.fChannelData[i][m_stDspSmvData.nSmvPerCircle] = stSmvValue.fSmvValue[i];
        m_stDspSmvData.fScaleFactor[i] = 1.0f;
        m_stDspSmvData.nChannelVCType[i] = _GetCtrlBlkChannelVCType(pCurrentCtrlBlk, i, enOpMode);
    }
    m_stDspSmvData.nChannelCount = nChannelCountDsp;
    //如果采样点数达到一个周波，就发送给dsp进行处理
    if (++m_stDspSmvData.nSmvPerCircle == (nSampleRate/DSM_CONFIG_DEFAULT_ELEC_FREQUENCY))
    {
        SetEvent(m_hEventSmv);
    }
    m_lockDspSend.Release();
}

///
/// @brief
///    根据通道类型和通道相别，产生发送dsp需要的相别识别字节
///
uint8_t CFrameRecv::_GetDspChannelVCType( enumChannelType enChannelType, ChannelPhase enChannelPhase )
{
    uint8_t nChannelType = 0x00;
    uint8_t nChannelPhase = 0x00;
    if (CHANNEL_TYPE_SMV_CURRENT == enChannelPhase)
    {
        nChannelType = SMV_VCTYPE_CURRENT;
    }
    else if (CHANNEL_TYPE_SMV_VOLTAGE == enChannelType)
    {
        nChannelType = SMV_VCTYPE_VOLTAGE;
    }

    switch(enChannelPhase)
    {
    case CHANNEL_PHASE_A:
        nChannelPhase = SMV_VCTYPE_PHASE_A;
        break;
    case CHANNEL_PHASE_B:
        nChannelPhase = SMV_VCTYPE_PHASE_B;
        break;
    case CHANNEL_PHASE_C:
        nChannelPhase = SMV_VCTYPE_PHASE_C;
        break;
    default:
        break;
    }

    return nChannelPhase | nChannelType;

}

///
/// @brief
///    获取一个控制块的采样率
///
uint32_t CFrameRecv::_GetCtrlBlkSampleRate( CBaseDetectItem* pCtrlBlk )
{
    uint32_t nSampleRate = 4000;
    if (pCtrlBlk)
    {
        nSampleRate = pCtrlBlk->nSampleRate;
    }
    return nSampleRate;
}

///
/// @brief
///    获取一个控制块某个通道的相别类型信息
///
uint8_t CFrameRecv::_GetCtrlBlkChannelVCType( CBaseDetectItem* pCurrentCtrlBlk, uint32_t nChannelIndex, DSM_OPERATE_MODE enOpMode )
{
    if (pCurrentCtrlBlk)
    {
        if (DSM_MODE_SMV_RECV_PHASE_CHECK == enOpMode)
        {
            const uint8_t nChannelsPhaseType[6] = {0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0};
            if (nChannelIndex < 6)
            {
                return nChannelsPhaseType[nChannelIndex] | 0x01;
            }
        }
        else
        {
            CSmvRecvConfig* pRecvConfig = pCurrentCtrlBlk->pRecvConfig;
            if (pRecvConfig)
            {
                //使用长度为6的数组保存一次获取的电流，电压通道信息
                uint32_t nChannels[6] = {0};
                //使用长度为6的数组保存一次获取的电流，电压通道对应的电压/电流和相别信息
                const uint8_t nChannelsVcType[6] = {0xA2, 0xB2, 0xC2, 0xA1, 0xB1, 0xC1};
                if (pRecvConfig->GetDspSendCurInfo(nChannels[0], nChannels[1], nChannels[2])
                    && pRecvConfig->GetDspSendVolInfo(nChannels[3], nChannels[4], nChannels[5]))
                {
                    for (int i = 0; i < 6; ++i)
                    {
                        if (nChannelIndex == nChannels[i])
                        {
                            return nChannelsVcType[i];
                        }
                    }
                }

                //未设置到ABC相别的其他通道填充电压/电流通道信息
                CSmvRecvChannelInfo* pChannelInfo = pRecvConfig->GetOneChannelInfo(nChannelIndex);
                if (pChannelInfo)
                {
                    if (pChannelInfo->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
                    {
                        return SMV_VCTYPE_VOLTAGE;
                    }
                    else if (pChannelInfo->enChannelType == CHANNEL_TYPE_SMV_CURRENT)
                    {
                        return SMV_VCTYPE_CURRENT;
                    }
                }
            }
        }
    }
    return 0;
}

///
/// @brief
///    初始化dsp发送需要的事件
///
bool CFrameRecv::_InitDspSendEvents()
{
    //创建dsp发送等待事件
    if (NULL == m_hEventExitDspSendProc)
    {
        m_hEventExitDspSendProc = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (NULL == m_hEventExitDspSendProc)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_InitDspSendEvents(): Create m_hEventExitDspSendProc failed\r\n")));
            return false;
        }
    }
    if (NULL == m_hEventSmv)
    {
        m_hEventSmv = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (NULL == m_hEventSmv)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_InitDspSendEvents(): Create m_hEventSmv failed\r\n")));
            goto errHandle;
        }
    }
    if (NULL == m_hEventSmvTime)
    {
        m_hEventSmvTime = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (NULL == m_hEventSmvTime)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_InitDspSendEvents(): Create m_hEventSmvTime failed\r\n")));
            goto errHandle;
        }
    }

    return true;

errHandle:
    if (NULL != m_hEventExitDspSendProc)
    {
        CloseHandle(m_hEventExitDspSendProc);
        m_hEventExitDspSendProc = NULL;
    }
    if (NULL != m_hEventSmv)
    {
        CloseHandle(m_hEventSmv);
        m_hEventSmv = NULL;
    }
    if (NULL != m_hEventSmvTime)
    {
        CloseHandle(m_hEventSmvTime);
        m_hEventSmvTime = NULL;
    }
    return false;
}

///
/// @brief
///    清理dsp发送等待事件
///
void CFrameRecv::_UninitDspSendEvents()
{
    if (NULL != m_hEventExitDspSendProc)
    {
        CloseHandle(m_hEventExitDspSendProc);
        m_hEventExitDspSendProc = NULL;
    }
    if (NULL != m_hEventSmv)
    {
        CloseHandle(m_hEventSmv);
        m_hEventSmv = NULL;
    }
    if (NULL != m_hEventSmvTime)
    {
        CloseHandle(m_hEventSmvTime);
        m_hEventSmvTime = NULL;
    }
}

///
/// @brief
///    设置缓存数据对应的控制块指针
///
void CFrameRecv::_SetCachedCtrlBlk( CBaseDetectItem* pCtrlBlk )
{
    AutoLock autoLock(m_lockCacheCtrlBlk);
    m_pCachedCtrlBlk = pCtrlBlk;
}

///
/// @brief
///    获取缓存数据对应的控制块指针
///
CBaseDetectItem* CFrameRecv::_GetCachedCtrlBlk()
{
    AutoLock autoLock(m_lockCacheCtrlBlk);
    return m_pCachedCtrlBlk;
}

///
/// @brief
///    清空所有缓存信息
///
void CFrameRecv::_ClearCachedData()
{

#if GEN_LOG_MESSAGE
    RETAILMSG(OUTPUT_LOG_MSG, (_T("_ClearCachedData() \r\n")));
#endif

    //清空smv采样值信息
#if OSCILLOGRAM_USE_STL_DEQUE
    _ClearSmvDeque();
#else
    m_cOscillogramBuf.ClearData();
#endif

    //清空smv采样值时间信息
    _ClearSmvTimeDeque();

    //清空状态信息
    {
        AutoLock autoLock(m_lockCtrlBlkQuality);
        m_cSmvCtrlBlkQuality.SetCtrlBlk(NULL);
    }

    //清空dsp计算信息，通知dsp丢弃已发送的数据
    if (NULL != m_pDsp)
    {
        //清空采样值缓存
        m_pDsp->ResetCache(0);
        //重新计算离散度信息
        m_pDsp->ResetCache(1);
    }

    //清空dsp发送采样值缓冲区
    _ClearDspSendSmvBuffer();

    //清空双AD信息
    _ClearDoubleADData();

    // 设置缓存的控制块为空
    _SetCachedCtrlBlk(NULL);

}

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
bool CFrameRecv::GetHarmonicData( CDataHarmonic* pData )
{
    if (!pData)
    {
        return false;
    }

    m_cDspDataSet.GetCalcData(m_stSmvDataCalcRt);
    if (m_stSmvDataCalcRt.nChannelCount == 0)
    {
        return false;
    }

    pData->ClearHarmonicData();
    pData->SetCtrlBlk(_GetCachedCtrlBlk());
    memcpy_s(pData->m_fDCValue, sizeof(pData->m_fDCValue), m_stSmvDataCalcRt.fDCValue, sizeof(m_stSmvDataCalcRt.fDCValue));
    memcpy_s(pData->m_fSumHarmTHD, sizeof(pData->m_fSumHarmTHD), m_stSmvDataCalcRt.fSumHarmTHD, sizeof(m_stSmvDataCalcRt.fSumHarmTHD));
    memcpy_s(pData->m_fHarmContent, sizeof(pData->m_fHarmContent), m_stSmvDataCalcRt.fHarmContent, sizeof(m_stSmvDataCalcRt.fHarmContent));
    memcpy_s(pData->m_fHarmAngle, sizeof(pData->m_fHarmAngle), m_stSmvDataCalcRt.fHarmAngle, sizeof(m_stSmvDataCalcRt.fHarmAngle));
    memcpy_s(pData->m_fHarmTHD, sizeof(pData->m_fHarmTHD), m_stSmvDataCalcRt.fHarmTHD, sizeof(m_stSmvDataCalcRt.fHarmTHD));
    memcpy_s(pData->m_fDCTHD, sizeof(pData->m_fDCTHD), m_stSmvDataCalcRt.fDCTHD, sizeof(m_stSmvDataCalcRt.fDCTHD));
    return true;
}

///
/// @brief
///    smv时间戳处理
///
void CFrameRecv::_SmvTimeHandle( DSM_OPERATE_MODE enOpMode, FrameTimeStamp& stFrameTime,  CBaseDetectItem* pCurrentCtrlBlk )
{
    switch(enOpMode)
    {
    case DSM_MODE_SMV_RECV_OSCILLOGRAM:
    case DSM_MODE_SMV_RECV_STATUS_WATCH:
    case DSM_MODE_SMV_RECV_EFFECTIVE_VALUE:
    case DSM_MODE_SMV_RECV_PHASOR:
    case DSM_MODE_SMV_RECV_XU_LIANG:
    case DSM_MODE_SMV_RECV_POWER:
    case DSM_MODE_SMV_RECV_HARMONIC:
    case DSM_MODE_SMV_RECV_DOUBLE_AD:
    case DSM_MODE_SMV_RECV_DISPERSION:
    case DSM_MODE_SMV_RECV_FRAME_STATIC:
    case DSM_MODE_SMV_RECV_POLARITY:
    case DSM_MODE_SMV_RECV_REAL_TIME_MSG:
        _AddSmvTime2Deque(stFrameTime, pCurrentCtrlBlk);
        break;
    default:
        break;
    }
}

///
/// @brief
///    将时间戳信息添加进发送给dsp的队列
///
void CFrameRecv::_AddSmvTime2Deque( FrameTimeStamp& stFrameTime, CBaseDetectItem* pCurrentCtrlBlk )
{
    if (_GetCachedCtrlBlk() != pCurrentCtrlBlk)
    {
        return;
    }

    uint32_t nSampleRate = _GetCtrlBlkSampleRate(pCurrentCtrlBlk);
    uint32_t nSamplePerWave = nSampleRate/DSM_CONFIG_DEFAULT_ELEC_FREQUENCY;
    uint32_t nSampleCached = nSamplePerWave * DSM_MAX_OSCILLOGRAM_WAVE_COUNT;

    MSGTIME stMsgTime;
    stMsgTime.nSecond = stFrameTime.ts_sec;
    stMsgTime.nMicrosecond = stFrameTime.ts_usec;

    m_lockSmvTime.Acquire();
    m_dequeSmvTime.push_front(stMsgTime);
    if (m_dequeSmvTime.size() > nSampleCached)
    {
        m_dequeSmvTime.resize(nSampleCached);
    }
    if (m_dequeSmvTime.size() > nSamplePerWave)
    {
        SetEvent(m_hEventSmvTime);
    }
    m_lockSmvTime.Release();
}

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
bool CFrameRecv::GetFrameStatistics( CDataFrameStatistics* pData )
{
    if (!pData)
    {
        return false;
    }

    pData->ClearData();
    if ( !m_cDspDataSet.GetMsgDisp(pData->m_fMsgDispersion, sizeof(pData->m_fMsgDispersion)))
    {
        return false;
    }

    {
        CSmvFrameStatistics* pSmvFrameStatistic = NULL;
        AutoLock autoLock(m_lockSmvFrameStatistics);
        MapDetectItemSmvFrameStatistics::iterator itrMap = m_mapSmvFrameStatistics.find(GetCurrentCtrlBlk());
        if (itrMap != m_mapSmvFrameStatistics.end())
        {
            pSmvFrameStatistic = &(itrMap->second);
        }
        if (pSmvFrameStatistic)
        {
            pData->m_stSmvFrameStatistics = pSmvFrameStatistic->m_stSmvFrameStatistics;
        }
        else
        {
            return false;
        }
    }
    return true;
}

///
/// @brief
///    重置所有的通道极性
///
void CFrameRecv::ResetAllPorlarity()
{
    AutoLock autolock(m_lockArrayPolarity);
    memset(m_nArrayPolarity, 0, sizeof(m_nArrayPolarity));
    memset(m_bPolaDetected, 0, sizeof(m_bPolaDetected));
    memset(m_bPolaDataValid, 0, sizeof(m_bPolaDataValid));
    memset(m_fPolaDataCache, 0, sizeof(m_fPolaDataCache));
    memset(m_nPolaDataIndex, 0, sizeof(m_nPolaDataIndex));
    m_bPoLarityCheckBegin = true;
}

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
bool CFrameRecv::GetPolarityData( CDataPolarity* pData )
{
    if (!pData || !_GetCachedCtrlBlk())
    {
        return false;
    }

    //清空原有数据信息
    pData->ClearChannelInfo();
    CBaseDetectItem* pItem = _GetCachedCtrlBlk();
    pData->SetCtrlBlk(pItem);
    uint32_t nChannelCount = pItem->nChannelCount;
    CDataPolarityChannelInfo cPolarityChannelInfo;

    m_lockCtrlBlkQuality.Acquire();
    CSmvCtrlBlkQuality cSmvCtrlBlkQuality = m_cSmvCtrlBlkQuality;
    m_lockCtrlBlkQuality.Release();

    m_lockArrayPolarity.Acquire();
    for (uint32_t i = 0; i < nChannelCount; ++i)
    {
        cPolarityChannelInfo.m_nIndex = i;
        cPolarityChannelInfo.m_bChannelValid = cSmvCtrlBlkQuality.IsChannelValid(i);
        cPolarityChannelInfo.m_nPolarity = m_bPolaDataValid[i] ? m_nArrayPolarity[i] : 0;
        pData->AddOneChannelInfo(cPolarityChannelInfo);
    }
    m_lockArrayPolarity.Release();
    return true;
}

///
/// @brief
///    处理采样值，获取极性信息
///
void CFrameRecv::_PolarityCheck( SMV_VALUE& stSmvValue, CBaseDetectItem* pCurrentCtrlBlk )
{
    if (pCurrentCtrlBlk != _GetCachedCtrlBlk() || stSmvValue.nChannelCount > NET_9_2_LE_MAX_CHANNELS_PER_ASDU)
    {
        return;
    }

    AutoLock autoLock(m_lockArrayPolarity);
    for(uint32_t i = 0; i < stSmvValue.nChannelCount; ++i)
    {
        if (m_bPolaDataValid[i])
        {
            continue;
        }
        else
        {
            float fSillValue      = 0;                                      //门槛值
            float fNullShiftValue = 0;                                      //零漂值
            uint32_t nSampleValue = 0;                                      //采样率
            CSmvRecvConfigPublic* pSmvRecvConfigPub = CDsmDataLayer::getInstance()->GetSmvConfigPublic();
            if (pSmvRecvConfigPub)
            {
                fSillValue = pSmvRecvConfigPub->GetSillValue();
                fNullShiftValue = pSmvRecvConfigPub->GetNullShiftValue();
            }
            //nValue是采样值大小的判断条件，采样值大于nValue时，开始统计极性信息
            float fValue = max(fSillValue, fNullShiftValue);

            if (m_bPolaDetected[i])
            {
                //将一个周波数据放入缓存，如果发现是交流，则不判断极性信息
                if(m_nPolaDataIndex[i] < SMV_MAX_POINT_PER_WAVE)
                {
                    m_fPolaDataCache[i][m_nPolaDataIndex[i]] = stSmvValue.fSmvValue[i];
                    m_nPolaDataIndex[i]++;
                }
                else
                {
                    bool bPos = false, bNeg = false;
                    for(uint32_t n = 0; n < SMV_MAX_POINT_PER_WAVE; ++n)
                    {
                        if (m_fPolaDataCache[i][n] > fValue)
                        {
                            bPos = true;
                        }
                        else if (m_fPolaDataCache[i][n] < -fValue)
                        {
                            bNeg = true;
                        }
                    }
                    if ( (bPos && m_nArrayPolarity[i] < 0) 
                        || (bNeg && m_nArrayPolarity[i] > 0) )
                    {
                        m_nArrayPolarity[i] = 0;
                    }
                    m_bPolaDataValid[i] = true;
                }
            }
            else
            {
                //如果当前的极性信息为0，才进行比较操作
                if (m_nArrayPolarity[i] == 0)
                {
                    if (stSmvValue.fSmvValue[i] > fValue)
                    {
                        m_bPolaDetected[i] = true;
                        m_nArrayPolarity[i] = SMV_PORLARITY_CHECK_POSITIVE;
                    }
                    else if (stSmvValue.fSmvValue[i] < -fValue)
                    {
                        m_bPolaDetected[i] = true;
                        m_nArrayPolarity[i] = SMV_PORLARITY_CHEKC_NEGATIVE;
                    }
                }
            }
        }
    }
}

///
/// @brief
///    对9-1/FT3采样值进行转换为系统设置显示的一次/二次值
///
/// @param[in]  stSmvValueFt3_91  FT3/9-1采样值
/// @param[out] stSmvValue        保存转换后的数值
/// 
/// @return
///     bool    转换成功返回true
///
bool CFrameRecv::_SmvValueConvert( const SMV_VALUE_FT3_9_1& stSmvValueFt3_91, SMV_VALUE& stSmvValue  ,enumSmvValueType enSmvDispType, enumSmvValueType enFrameValueType)
{
    stSmvValue.nChannelCount = stSmvValueFt3_91.nChannelCount;
    stSmvValue.pDetectItem   = stSmvValueFt3_91.pDetectItem;
    stSmvValue.stFrameTime   = stSmvValueFt3_91.stFrameTime;
    stSmvValue.nSmpCnt       = stSmvValueFt3_91.nSmpCnt;

    if (enSmvDispType == SMV_VALUE_TYPE_ORIGINANL )
    {
        for (uint32_t i = 0; i < stSmvValueFt3_91.nChannelCount; ++i)
        {
            stSmvValue.fSmvValue[i] = static_cast<float>(stSmvValueFt3_91.nSmvValue[i]);
        }
        return true;
    }
    else if (enSmvDispType == SMV_VALUE_TYPE_PRIMARY || enSmvDispType == SMV_VALUE_TYPE_SECONDARY)
    {
        CSmvScaler smvScaler;
        smvScaler.SetChannelMsgValType(enFrameValueType);
        smvScaler.SetChannelNtrValType(enSmvDispType);
        CSmvRecvChannelInfo* pSmvRecvChannelInfo = NULL;
        if (stSmvValueFt3_91.pDetectItem && NULL != _GetCachedCtrlBlk())
        {
            CSmvRecvConfig* pSmvRecvConfig = stSmvValueFt3_91.pDetectItem->pRecvConfig;
            if (!pSmvRecvConfig)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("Get Detect Item Recv Config Failed \n")));
                return false;
            }
            uint32_t nChannelCount = pSmvRecvConfig->GetChannelCount();
            m_lockCtrlBlkQuality.Acquire();
            bool bRangeFlag = m_cSmvCtrlBlkQuality.GetRangeFlag() == 1 ? true : false;
            m_lockCtrlBlkQuality.Release();
            if (nChannelCount == stSmvValue.nChannelCount )
            {
                if (DSM_MODE_SMV_RECV_PHASE_CHECK != GetCurrentMode())
                {
                    for (uint32_t i = 0; i < nChannelCount; ++i)
                    {
                        pSmvRecvChannelInfo = pSmvRecvConfig->GetOneChannelInfo(i);
                        if (pSmvRecvChannelInfo)
                        {
                            if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_CURRENT
                                || pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
                            {
                                stSmvValue.fSmvValue[i] = smvScaler.ChannelMsgVal2NtrVal_FT3(stSmvValueFt3_91.nSmvValue[i],
                                    pSmvRecvChannelInfo->fPrimaryRatedValue,
                                    pSmvRecvChannelInfo->fSecondaryRatedValue,
                                    pSmvRecvChannelInfo->enChannelType,
                                    pSmvRecvChannelInfo->enChannelValueType,
                                    bRangeFlag);
                            }
                            else
                            {
                                stSmvValue.fSmvValue[i] = static_cast<float>(stSmvValueFt3_91.nSmvValue[i]);
                            }
                        }
                    }
                }
                else //核相
                {
                    //获取本采样值在核相中使用的通道
                    uint32_t nChannelPhaseCheck[6] = {0};
                    uint32_t nChannelCount = 0;
                    if (m_pCurrentDetectItems[0]->nID ==  m_pCurrentDetectItems[1]->nID)
                    {
                        //核相的两个控制块相同，也就是只有一个控制块，6个通道全来自一个控制块
                        nChannelCount = 6;
                        stSmvValueFt3_91.pDetectItem->pRecvConfig->GetPhaseCheckSmv1Channels(nChannelPhaseCheck[0], nChannelPhaseCheck[1], nChannelPhaseCheck[2]);
                        stSmvValueFt3_91.pDetectItem->pRecvConfig->GetPhaseCheckSmv2Channels(nChannelPhaseCheck[3], nChannelPhaseCheck[4], nChannelPhaseCheck[5]);
                    }
                    else
                    {
                        //核相的两个控制块不同，需要从每个控制块拿到3个通道数据
                        //先判断当前采样值是属于哪一个控制块的，拿到对应的核相使用的通道
                        nChannelCount = 3;
                        if (stSmvValueFt3_91.pDetectItem->nID == m_pCurrentDetectItems[0]->nID)
                        {
                            stSmvValueFt3_91.pDetectItem->pRecvConfig->GetPhaseCheckSmv1Channels(nChannelPhaseCheck[0], nChannelPhaseCheck[1], nChannelPhaseCheck[2]);
                        }
                        else if (stSmvValueFt3_91.pDetectItem->nID == m_pCurrentDetectItems[1]->nID)
                        {
                            stSmvValueFt3_91.pDetectItem->pRecvConfig->GetPhaseCheckSmv2Channels(nChannelPhaseCheck[0], nChannelPhaseCheck[1], nChannelPhaseCheck[2]);
                        }
                    }

                    for (uint32_t i = 0; i < nChannelCount; ++i)
                    {
                        pSmvRecvChannelInfo = pSmvRecvConfig->GetOneChannelInfo(nChannelPhaseCheck[i]);
                        if (pSmvRecvChannelInfo)
                        {
                            if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_CURRENT
                                || pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
                            {
                                stSmvValue.fSmvValue[nChannelPhaseCheck[i]] = smvScaler.ChannelMsgVal2NtrVal_FT3(stSmvValueFt3_91.nSmvValue[nChannelPhaseCheck[i]],
                                    pSmvRecvChannelInfo->fPrimaryRatedValue,
                                    pSmvRecvChannelInfo->fSecondaryRatedValue,
                                    pSmvRecvChannelInfo->enChannelType,
                                    pSmvRecvChannelInfo->enChannelValueType,
                                    bRangeFlag);
                            }
                            else
                            {
                                stSmvValue.fSmvValue[nChannelPhaseCheck[i]] = static_cast<float>(stSmvValueFt3_91.nSmvValue[nChannelPhaseCheck[i]]);
                            }
                        }
                    }
                }
                return true;
            }
            return false;
        }
        return false;
    }
    return false;
}

///
/// @brief
///    对9-2采样值进行转换为系统设置显示的一次/二次值
///
/// @param[in]  stSmvValue92      9-2采样值
/// @param[out] stSmvValue        保存转换后的数值
/// 
/// @return
///     bool    转换成功返回true
///
bool CFrameRecv::_SmvValueConvert( const SMV_VALUE_9_2& stSmvValue92, SMV_VALUE& stSmvValue ,enumSmvValueType  enSmvDispType, enumSmvValueType enFrameValueType)
{
    stSmvValue.nChannelCount = stSmvValue92.nChannelCount;
    stSmvValue.pDetectItem   = stSmvValue92.pDetectItem;
    stSmvValue.stFrameTime   = stSmvValue92.stFrameTime;
    stSmvValue.nSmpCnt       = stSmvValue92.nSmpCnt;

    if (enSmvDispType == SMV_VALUE_TYPE_ORIGINANL)
    {
        for (uint32_t i = 0; i < stSmvValue92.nChannelCount; ++i)
        {
            stSmvValue.fSmvValue[i] = static_cast<float>(stSmvValue92.nSmvValue[i]);
        }
        return true;
    }
    else if (enSmvDispType == SMV_VALUE_TYPE_PRIMARY || enSmvDispType == SMV_VALUE_TYPE_SECONDARY)
    {
        CSmvScaler smvScaler;
        smvScaler.SetChannelMsgValType(enFrameValueType);
        smvScaler.SetChannelNtrValType(enSmvDispType);
        CSmvRecvChannelInfo* pSmvRecvChannelInfo = NULL;
        if (stSmvValue92.pDetectItem && NULL != _GetCachedCtrlBlk())
        {
            CSmvRecvConfig* pSmvRecvConfig = stSmvValue92.pDetectItem->pRecvConfig;
            if (!pSmvRecvConfig)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("Get Detect Item Recv Config Failed \n")));
                return false;
            }
            uint32_t nChannelCount = pSmvRecvConfig->GetChannelCount();
            if (nChannelCount == stSmvValue.nChannelCount)
            {
                if (DSM_MODE_SMV_RECV_PHASE_CHECK != GetCurrentMode())
                {
                    for (uint32_t i = 0; i < nChannelCount; ++i)
                    {
                        pSmvRecvChannelInfo = pSmvRecvConfig->GetOneChannelInfo(i);
                        if (pSmvRecvChannelInfo)
                        {
                            if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE 
                                || pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_CURRENT)
                            {
                                stSmvValue.fSmvValue[i] = smvScaler.ChannelMsgVal2NtrVal_Sav92(stSmvValue92.nSmvValue[i],
                                    pSmvRecvChannelInfo->fPrimaryRatedValue,
                                    pSmvRecvChannelInfo->fSecondaryRatedValue,
                                    pSmvRecvChannelInfo->enChannelType );
                            }
                            else
                            {
                                stSmvValue.fSmvValue[i] = static_cast<float>(stSmvValue92.nSmvValue[i]);
                            }
                        }
                    }
                }
                else
                {
                    //获取本采样值在核相中使用的通道
                    uint32_t nChannelPhaseCheck[6] = {0};
                    uint32_t nChannelCount = 0;
                    if (m_pCurrentDetectItems[0]->nID ==  m_pCurrentDetectItems[1]->nID)
                    {
                        //核相的两个控制块相同，也就是只有一个控制块，6个通道全来自一个控制块
                        nChannelCount = 6;
                        stSmvValue92.pDetectItem->pRecvConfig->GetPhaseCheckSmv1Channels(nChannelPhaseCheck[0], nChannelPhaseCheck[1], nChannelPhaseCheck[2]);
                        stSmvValue92.pDetectItem->pRecvConfig->GetPhaseCheckSmv2Channels(nChannelPhaseCheck[3], nChannelPhaseCheck[4], nChannelPhaseCheck[5]);
                    }
                    else
                    {
                        //核相的两个控制块不同，需要从每个控制块拿到3个通道数据
                        //先判断当前采样值是属于哪一个控制块的，拿到对应的核相使用的通道
                        nChannelCount = 3;
                        if (stSmvValue92.pDetectItem->nID == m_pCurrentDetectItems[0]->nID)
                        {
                            stSmvValue92.pDetectItem->pRecvConfig->GetPhaseCheckSmv1Channels(nChannelPhaseCheck[0], nChannelPhaseCheck[1], nChannelPhaseCheck[2]);
                        }
                        else if (stSmvValue92.pDetectItem->nID == m_pCurrentDetectItems[1]->nID)
                        {
                            stSmvValue92.pDetectItem->pRecvConfig->GetPhaseCheckSmv2Channels(nChannelPhaseCheck[0], nChannelPhaseCheck[1], nChannelPhaseCheck[2]);
                        }
                    }

                    for (uint32_t i = 0; i < nChannelCount; ++i)
                    {
                        pSmvRecvChannelInfo = pSmvRecvConfig->GetOneChannelInfo(nChannelPhaseCheck[i]);
                        if (pSmvRecvChannelInfo)
                        {
                            if (pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_CURRENT
                                || pSmvRecvChannelInfo->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
                            {
                                stSmvValue.fSmvValue[nChannelPhaseCheck[i]] = smvScaler.ChannelMsgVal2NtrVal_Sav92(stSmvValue92.nSmvValue[nChannelPhaseCheck[i]],
                                    pSmvRecvChannelInfo->fPrimaryRatedValue,
                                    pSmvRecvChannelInfo->fSecondaryRatedValue,
                                    pSmvRecvChannelInfo->enChannelType );
                            }
                            else
                            {
                                stSmvValue.fSmvValue[nChannelPhaseCheck[i]] = static_cast<float>(stSmvValue92.nSmvValue[nChannelPhaseCheck[i]]);
                            }
                        }
                    }
                }
                return true;
            }
            return false;
        }
        return false;
    }
    return false;
}

///
/// @brief
///    清空统计数据，重新统计
///
bool CFrameRecv::ResetSmvFrameStatistics()
{
    //清空采样值时间缓存
    _ClearSmvTimeDeque();

    //告诉dsp重新统计离散度
    if (m_pDsp)
    {
        m_pDsp->ResetCache(1);
    }

    //软件重新统计当前控制块数据
    {
        CSmvFrameStatistics* pSmvFrameStatistic = NULL;
        AutoLock autoLock(m_lockSmvFrameStatistics);
        MapDetectItemSmvFrameStatistics::iterator itrMap = m_mapSmvFrameStatistics.find(GetCurrentCtrlBlk());
        if (itrMap != m_mapSmvFrameStatistics.end())
        {
            pSmvFrameStatistic = &(itrMap->second);
        }
        if (pSmvFrameStatistic)
        {
            pSmvFrameStatistic->ResetData();
        }
    }

    return true;
}

///
/// @brief
///    获取最新一帧数据内容
///
/// @param[out] pData   外部申请的数据对象指针，函数填充指针指向对象的内容
///
/// @return
///     bool    数据获取成功返回true，pData中保存获取到的数据
///
bool CFrameRecv::GetRealTimeMsg( CDataRealTimeMsgData* pData )
{
    AutoLock autoLock(m_lockRealTimeMsg);
    if (GetCurrentCtrlBlk() != m_cRealTimeMsg.m_pCtrlBlk || m_cRealTimeMsg.m_enFrameType == FRAME_TYPE_UNKOWN)
    {
        return false;
    }
    *pData = m_cRealTimeMsg;
    return true;
}

///
/// @brief
///    获取核相数据信息
///
/// @param[out] pData   外部申请的数据对象指针，函数填充指针指向对象的内容
///
/// @return
///     bool    数据获取成功返回true，pData中保存获取到的数据
///
bool CFrameRecv::GetPhaseCheckData( CDataPhaseCheck* pData )
{
    if (pData)
    {
        //从dsp获取核相数据信息，填充到pdata对应的成员
        m_cDspDataSet.GetCalcData(m_stSmvDataCalcRt);
        if (m_stSmvDataCalcRt.nChannelCount == 0)
        {
            return false;
        }

        pData->ResetData();
        pData->SetCtrlBlk(_GetCachedCtrlBlk());

        pData->m_fFreq[0] = m_stSmvDataCalcRt.fFreq[0];
        pData->m_fFreq[1] = m_stSmvDataCalcRt.fFreq[3];

        for (int j = 0; j < 3; ++j) //ABC三相
        {
            for (int i = 0; i < 2; ++i)//smv1，smv2两个控制块
            {
                pData->m_stPhaseABCData[i][j].m_fValue = m_stSmvDataCalcRt.fRmsValue[i * 3 + j];
                pData->m_stPhaseABCData[i][j].m_fPhase = m_stSmvDataCalcRt.fHarmAngle[i * 3 + j][0];

                pData->m_stBetweenPhasePhasorDiff[i][j].m_fValue = m_stSmvDataCalcRt.fIPVectorDifContent[i * 3 + j];
                pData->m_stBetweenPhasePhasorDiff[i][j].m_fPhase = m_stSmvDataCalcRt.fIPVectorDifAngle[i * 3 + j];
            }
            pData->m_stInPhasePhasorDiff[j].m_fPhase = m_stSmvDataCalcRt.fSPVectorDifAngle[j];
            pData->m_stInPhasePhasorDiff[j].m_fValue = m_stSmvDataCalcRt.fSPVectorDifContent[j];

            pData->m_fInPhaseAngleDiff[j] = m_stSmvDataCalcRt.fSPAngleDif[j];
        }
        return true;
    }
    return false;
}

///
/// @brief
///    获取控制块对应的一个数据帧信息
///
/// @param[in]      pDetectItem     要获取数据帧的控制块指针
///
/// @return
///     CDetectItemFrame*   返回控制块对应的一个数据帧，NULL表示获取失败
///
CDetectItemFrame* CFrameRecv::GetDetectItemFrame( CBaseDetectItem* pDetectItem )
{
    AutoLock autoLock(m_lockDetectItemsFrame);
    std::vector<CDetectItemFrame*>::iterator itr = m_vecDetectItemsFrame.begin();
    while(itr != m_vecDetectItemsFrame.end())
    {
        if (pDetectItem == (*itr)->GetDetectItem())
        {
            return (*itr);
        }
        ++itr;
    }
    return NULL;
}

///
/// @brief
///    设置核相使用的第1个控制块
///
/// @param[in] pDetectItem  要设置的控制块
///
void CFrameRecv::SetPhaseCheckSmv1CtrBlk(CBaseDetectItem* pDetectItem)
{
    if (pDetectItem)
    {
        //如果第一个控制块发生变化，清空核相已发送数据信息
        if (pDetectItem != m_pCurrentDetectItems[0])
        {
            {
                AutoLock autoLock(m_lockDspSend);
                m_stDspSmvData.nSmvPerCircle = 0;

                //清空采样值缓存
                if (m_pDsp)
                {
                    m_pDsp->ResetCache(0);
                }
            }

            //程序缓存控制块数据，因此更改控制块时，更新缓存的数据信息
            {
                AutoLock autolock(m_lockPhaseCheckData);
                if (m_pPhaseCheckAIndex != NULL)
                {
                    memset(m_pPhaseCheckAIndex, 0, sizeof(SMV_VALUE) * m_nPhaseCheckIndexLen);
                }
            }

            m_pCurrentDetectItems[0] = pDetectItem;
            {
                AutoLock autoLock(m_lockRealTimeChannelCounts);
                m_nRealTimeChannelCounts[0] = pDetectItem->nChannelCount;
            }

            //设置核相控制块时标对齐参数
            {
                uint32_t nSmvPhaseABC[3] = {0};
                CSmvRecvConfig* pSmvRecvCofig1 = pDetectItem->pRecvConfig;
                if (pSmvRecvCofig1)
                {
                    pSmvRecvCofig1->GetPhaseCheckSmv1Channels(nSmvPhaseABC[0], nSmvPhaseABC[1], nSmvPhaseABC[2]);
                    m_cSmpSyncTime.SetSyncChannelIdx(0,nSmvPhaseABC,3);
                }
            }
        }
    }
}

///
/// @brief
///    设置核相使用的第二个控制块
///
/// @param[in] pDetectItem  要设置的当前控制块
///
void CFrameRecv::SetPhaseCheckSmv2CtrBlk( CBaseDetectItem* pDetectItem )
{
    if (pDetectItem)
    {
        //如果第二个控制块发生变化，清空核相已发送数据信息
        if (pDetectItem != m_pCurrentDetectItems[1])
        {
            {
                AutoLock autoLock(m_lockDspSend);
                m_stDspSmvData.nSmvPerCircle = 0;

                //清空采样值缓存
                if (m_pDsp)
                {
                    m_pDsp->ResetCache(0);
                }
            }

            //程序缓存控制块数据，因此更改控制块时，更新缓存的数据信息
            {
                AutoLock autolock(m_lockPhaseCheckData);
                if (m_pPhaseCheckBIndex != NULL)
                {
                    memset(m_pPhaseCheckBIndex, 0, sizeof(SMV_VALUE) * m_nPhaseCheckIndexLen);
                }
            }

            m_pCurrentDetectItems[1] = pDetectItem;
            {
                AutoLock autoLock(m_lockRealTimeChannelCounts);
                m_nRealTimeChannelCounts[1] = pDetectItem->nChannelCount;
            }

            //设置核相控制块时标对齐参数
            {
                uint32_t nSmvPhaseABC[3] = {0};
                CSmvRecvConfig* pSmvRecvCofig = pDetectItem->pRecvConfig;
                if (pSmvRecvCofig)
                {
                    pSmvRecvCofig->GetPhaseCheckSmv2Channels(nSmvPhaseABC[0], nSmvPhaseABC[1], nSmvPhaseABC[2]);
                    m_cSmpSyncTime.SetSyncChannelIdx(1,nSmvPhaseABC,3);
                }
            }
        }
    }
}

///
/// @brief
///    获取核相使用的第1个控制块
///
/// @return
///     CBaseDetectItem*   返回当前控制块指针，失败返回Null
///
CBaseDetectItem* CFrameRecv::GetPhaseCheckSmv1CtrlBlk()
{
    return m_pCurrentDetectItems[0];
}

///
/// @brief
///    获取核相使用的第二个控制块
///
/// @return
///     CBaseDetectItem*   返回当前控制块指针，失败返回Null
///
CBaseDetectItem* CFrameRecv::GetPhaseCheckSmv2CtrlBlk()
{
    return m_pCurrentDetectItems[1];
}

///
/// @brief
///    获取GOOSE实时值
///
/// @param[out] pData   外部申请的数据对象指针，函数填充指针指向对象的内容
///
/// @return
///     bool    数据获取成功返回true，pData中保存获取到的数据
///
bool CFrameRecv::GetGooseRealTimeValue( CGooseInfo* pData )
{
    if (!pData)
    {
        return false;
    }

    AutoLock autoLock(m_lockRealTimeMsg);
    if (GetCurrentCtrlBlk() != m_cRealTimeMsg.m_pCtrlBlk || m_cRealTimeMsg.m_enFrameType == FRAME_TYPE_UNKOWN)
    {
        return false;
    }
    static GooseMsg* pGooseMsg = CreateGooseMsgImpl();
    pGooseMsg->Decode(m_cRealTimeMsg.m_nDataBuffer, m_cRealTimeMsg.m_nFrameLen);

    GOOSE_PDU_PUBLIC stGoosePduPub;
    pGooseMsg->GetPduPublicInfo(stGoosePduPub);
    pData->ClearChannelData();
    pData->SetCtrlBlk(m_cRealTimeMsg.m_pCtrlBlk);
    pData->SetGooseTTL(stGoosePduPub.timeAllowedtoLive);
    pData->SetGooseStNum(stGoosePduPub.stNum);
    pData->SetGooseSqNum(stGoosePduPub.sqNum);
    pData->SetEventTime(stGoosePduPub.eventTm.ts_s, stGoosePduPub.eventTm.ts_ns, stGoosePduPub.eventTm.ts_q);
    CGooseChannel  stGooseChannel;
    uint32_t nChannelCount = pGooseMsg->GetChannelNum();
    for (uint32_t i = 0; i < nChannelCount; ++i)
    {
        GOOSE_DATA* pGooseData = pGooseMsg->GetAChannel(i);
        if (pGooseData)
        {
            stGooseChannel.SetChannelID(i);
            stGooseChannel.SetChannelData(pGooseData);
            pData->AddChannel(&stGooseChannel);
        }
    }
    CGooseInfo::FillGooseChannelDescAndType(pData);
    return true;
}

///
/// @brief
///    获取GOOSE实时报文
///
/// @param[out] pData   外部申请的数据对象指针，函数填充指针指向对象的内容
///
/// @return
///     bool    数据获取成功返回true，pData中保存获取到的数据
///
bool CFrameRecv::GetGooseRealTimeMsg( CDataRealTimeMsgData* pData )
{
    return GetRealTimeMsg(pData);
}

///
/// @brief
///    获取GOOSE报文统计信息
///
/// @param[out] pData   外部申请的数据对象指针，函数填充指针指向对象的内容
///
/// @return
///     bool    数据获取成功返回true，pData中保存获取到的数据
///
bool CFrameRecv::GetGooseFrameStatistics( CGooseFrameStatistics* pData )
{
    if (!pData)
    {
        return false;
    }

    CBaseDetectItem* pCurrentCtrlBlk = GetCurrentCtrlBlk();
    CGooseRTStatistics* pGooseRtStatistics = NULL;
    AutoLock autoLock(m_lockGooseFrameStatistics);
    MapDetectItemGooseFrameStatistics::iterator itrMap = m_mapGooseFrameStatistics.find(pCurrentCtrlBlk);
    if (itrMap != m_mapGooseFrameStatistics.end())
    {
        pGooseRtStatistics = &(itrMap->second);
    }
    if (pGooseRtStatistics)
    {
        pData->m_stGooseFrameStatistics = pGooseRtStatistics->m_stGooseFrameStatistics;
        return true;
    }
    return false;
}

///
/// @brief
///    清空Goose报文统计数据，重新统计
///
/// @return
///     bool    重置成功返回true
///
bool CFrameRecv::ResetGooseFrameStatistics()
{
    CGooseRTStatistics* pGooseRtStatistics = NULL;
    AutoLock autoLock(m_lockGooseFrameStatistics);
    MapDetectItemGooseFrameStatistics::iterator itrMap = m_mapGooseFrameStatistics.find(GetCurrentCtrlBlk());
    if (itrMap != m_mapGooseFrameStatistics.end())
    {
        pGooseRtStatistics = &(itrMap->second);
    }
    if (pGooseRtStatistics)
    {
        pGooseRtStatistics->ResetData();
        return true;
    }
    return false;
}

///
/// @brief
///    获取GOOSE变位信息
///
/// @param[out] pData   外部申请的数据对象指针，函数填充指针指向对象的内容
///
/// @return
///     bool    数据获取成功返回true，pData中保存获取到的数据
///
bool CFrameRecv::GetGooseChangeList( CDataGooseChangeList* pData )
{
    if (pData)
    {
        CBaseDetectItem* pCurrentDetectItem  = GetCurrentCtrlBlk();
        AutoLock autoLock(m_lockGooseChangeList);
        MapDetectItemChangeList::iterator itr = m_mapGooseChangeList.find(pCurrentDetectItem);
        if (itr != m_mapGooseChangeList.end())
        {
            pData->SetCtrlBlk(pCurrentDetectItem);
            pData->_ClearChangeList();
            CGooseChangeListData* pVecGooseInfo = itr->second;
            uint32_t nTotalChangeTimes = pVecGooseInfo->GetTotalChangeTimes();
            uint32_t nLastChangeTimes = pVecGooseInfo->GetLastGetChangeTimes();
            CGooseOneTimeChange stGooseOneTimeChange;
            size_t nGooseChanges = pVecGooseInfo->GetGooseInfoCounts();
            if (nGooseChanges < 2)
            {
                //如果列表中只有0条或者1条数据，表示没有发生变位信息
                return false;
            }
            // 容器有n个元素表示有n-1次变位
            for(size_t i = 0; i < nGooseChanges - 1; ++i)
            {
                stGooseOneTimeChange.SetDataBeforeChange(pVecGooseInfo->GetOneGooseInfo(i));
                stGooseOneTimeChange.SetDataAfterChange(pVecGooseInfo->GetOneGooseInfo(i + 1));
                stGooseOneTimeChange.m_nChangeIndex = nTotalChangeTimes - (nGooseChanges - 1) + 1 + i;
                stGooseOneTimeChange.GenChangeList();
                pData->m_vecGooseChangeList.push_back(stGooseOneTimeChange);
            }
            uint32_t nNewChangeTimes = nTotalChangeTimes - nLastChangeTimes;
            if (nNewChangeTimes > DSM_MAX_GOOSE_CHANGELIST_SIZE)
            {
                nNewChangeTimes = DSM_MAX_GOOSE_CHANGELIST_SIZE;
            }
            pVecGooseInfo->SetLastGetChangeTines(nTotalChangeTimes);
            pData->SetNewChangeTimes(nNewChangeTimes);
            return true;
        }
    }
    return false;
}

///
/// @brief
///    设置录波文件大小
///
bool CFrameRecv::SetRecordWaveSize( uint64_t nSizeInByte )
{
    DSM_OPERATE_MODE enMode  = GetCurrentMode();
    if (DSM_MODE_SMV_RECV_RECORD_WAVE == enMode
     || DSM_MODE_FT3_RECV_RECORD_WAVE == enMode
     || DSM_MODE_GOOSE_RECV_RECORD_WAVE == enMode)
    {
        return false;
    }
    m_nRecordBufferSize = nSizeInByte;
    return true;
}

///
/// @brief
///    获取设置的录波文件大小
///
uint64_t CFrameRecv::GetRecordWaveSize()
{
    return m_nRecordBufferSize;
}

///
/// @brief
///    设置录波文件路径
///
void CFrameRecv::SetRecordWavePath( const std::wstring& strRecordWavePath )
{
    m_strRecordWavePath = strRecordWavePath;
}

///
/// @brief
///    获取设置的录波文件路径
///
std::wstring CFrameRecv::GetRecordWavePath()
{
    return m_strRecordWavePath;
}

///
/// @brief
///    设置通知界面时使用的窗口Cwnd指针
///
bool CFrameRecv::SetNotifyWnd( CWnd* pWnd )
{
    m_pNotifyWnd = pWnd;
    return true;
}

///
/// @brief
///    获取设置的通知界面使用的Cwnd窗口
///
CWnd* CFrameRecv::GetNotifyWnd()
{
    return m_pNotifyWnd;
}


///
/// @brief
///    添加录波控制块,主要针对网络数据帧（网络采样值和Goose）
///
bool CFrameRecv::AddRecordItem( CBaseDetectItem* pRecordItem )
{
    if (_IsDetectItemInRecordList(pRecordItem))
    {
        return false;
    }
    m_vecRecordWaveList.push_back(pRecordItem);
    return true;
}

///
/// @brief
///    判断pRecordItem是否在录波列表中
///
/// @return
///     bool 如果存在返回true
///
bool CFrameRecv::_IsDetectItemInRecordList( CBaseDetectItem* pRecordItem )
{
    if (m_vecRecordWaveList.size() > 0)
    {
        std::vector<CBaseDetectItem*>::iterator itr = m_vecRecordWaveList.begin();
        while(itr != m_vecRecordWaveList.end())
        {
            if (pRecordItem->nID == (*itr)->nID)
            {
                return true;
            }
            ++itr;
        }
    }
    return false;
}

///
/// @brief
///    从录波列表条目中移除一个控制块
///
bool CFrameRecv::RemoveRecordItem(CBaseDetectItem* pRecordItem)
{
    std::vector<CBaseDetectItem*>::iterator itr = m_vecRecordWaveList.begin();
    while(itr != m_vecRecordWaveList.end())
    {
        if (pRecordItem->nID == (*itr)->nID)
        {
            m_vecRecordWaveList.erase(itr);
            return true;
        }
        ++itr;
    }
    return false;
}

///
/// @brief
///    从录波列表条目中移除所有控制块
///
bool CFrameRecv::RemoveAllRecordItem()
{
    if (m_vecRecordWaveList.size() > 0)
    {
        m_vecRecordWaveList.clear();
    }
    return true;
}

///
/// @brief
///    开始录网络采样值数据，用pcap格式保存
///
bool CFrameRecv::StartRecordNetSmv()
{
    if (m_strRecordWavePath.empty())
    {
        return false;
    }

#if DSM_RECORD_WARE_REALTIME
    if (NULL == m_pRecordWaveRealTimeBuf)
    {
        m_pRecordWaveRealTimeBuf = new uint8_t[m_nRecordBufferSize];
    }
#if !CHUN_HUA
	if (NULL == m_pRecordWaveRealTimeBuf2)
	{
		m_pRecordWaveRealTimeBuf2 = new uint8_t[m_nRecordBufferSize];
	}
#endif
#else
    AutoLock autoLock(m_lockPcapFile);
    if (NULL == m_pPcapFile)
    {
        m_pPcapFile = CreatePcapFileImpl();
    }
    m_pPcapFile->SetBufSize(m_nRecordBufferSize);
    if (PCAP_FILE_SUCCESS != m_pPcapFile->Open(m_strRecordWavePath, PFM_WRITE) )
    {
        ReleasePcapFileImpl(m_pPcapFile);
        m_pPcapFile = NULL;
        return false;
    }
#endif
	StartRecord();
    SetCurrentMode(DSM_MODE_SMV_RECV_RECORD_WAVE);
    return true;
}

///
/// @brief
///    开始录Goose报文，采用pcap格式保存
///
bool CFrameRecv::StartRecordGoose()
{
    if (m_strRecordWavePath.empty())
    {
        return false;
    }

#if DSM_RECORD_WARE_REALTIME
    if (NULL == m_pRecordWaveRealTimeBuf)
    {
        m_pRecordWaveRealTimeBuf = new uint8_t[m_nRecordBufferSize];
    }

#if !CHUN_HUA
	if (NULL == m_pRecordWaveRealTimeBuf2)
	{
		m_pRecordWaveRealTimeBuf2 = new uint8_t[m_nRecordBufferSize];
	}
#endif

#else
    AutoLock autoLock(m_lockPcapFile);
    if (NULL == m_pPcapFile)
    {
        m_pPcapFile = CreatePcapFileImpl();
    }
    m_pPcapFile->SetBufSize(m_nRecordBufferSize);
    if (PCAP_FILE_SUCCESS != m_pPcapFile->Open(m_strRecordWavePath, PFM_WRITE) )
    {
        ReleasePcapFileImpl(m_pPcapFile);
        m_pPcapFile = NULL;
        return false;
    }
#endif
	StartRecord();
    SetCurrentMode(DSM_MODE_GOOSE_RECV_RECORD_WAVE);
    return true;
}

///
/// @brief
///    开始录FT3数据帧
///
bool CFrameRecv::StartRecordFt3()
{
    if (m_strRecordWavePath.empty())
    {
        return false;
    }

#if DSM_RECORD_WARE_REALTIME
    if (NULL == m_pRecordWaveRealTimeBuf)
    {
        m_pRecordWaveRealTimeBuf = new uint8_t[m_nRecordBufferSize];
    }

#if !CHUN_HUA
	if (NULL == m_pRecordWaveRealTimeBuf2)
	{
		m_pRecordWaveRealTimeBuf2 = new uint8_t[m_nRecordBufferSize];
	}
#endif

#else
    AutoLock autoLock(m_lockPcapFile);
    if (NULL == m_pPcapFile)
    {
        m_pPcapFile = CreatePcapFileImpl();
    }
    m_pPcapFile->SetBufSize(m_nRecordBufferSize);
    if (PCAP_FILE_SUCCESS != m_pPcapFile->Open(m_strRecordWavePath, PFM_WRITE) )
    {
        ReleasePcapFileImpl(m_pPcapFile);
        m_pPcapFile = NULL;
        return false;
    }
    PCAP_FILE_HEADER stPcapFileHeaderFt3 = {
        PCAP_DEFAULT_MAGIC,
        PCAP_DEFAULT_MAJOR_VERSION,
        PCAP_DEFAULT_MINOR_VERSION,
        PCAP_DEFAULT_THIS_ZONE,
        PCAP_DEFAULT_SIGFIGS,
        PCAP_DEFAULT_SANPLEN,
        PCAP_LINK_TYPE_FT3 };
    m_pPcapFile->InitFileHeader(stPcapFileHeaderFt3);
#endif

	StartRecord();
    SetCurrentMode(DSM_MODE_FT3_RECV_RECORD_WAVE);
    return true;
}

///
/// @brief
///    停止录波
///
/// @param[out]     记录失败时候时失败原因
///
/// @return
///     bool 停止成功
///
#if !CHUN_HUA
bool CFrameRecv::StopRecordWave(uint32_t& nResult)
{

	SetCurrentMode(DSM_MODE_QUIT_CURRENT_PAGE);
	m_nCurrentRecordSize = m_nRecordWaveIndex;
	if (1 == m_nCurrentBufferIndex )
	{
		m_bBuffer1Flag = true;
	}else if (2 == m_nCurrentBufferIndex )
	{
		m_bBuffer2Flag = true;
	}
	
	m_nCurrentBufferIndex = 0;
    m_bIsStopRecord = true;
	
	while(!m_bIsFinishRecord)
	{
		Sleep(100);
	}

    return true;
}

#else
bool CFrameRecv::StopRecordWave(uint32_t& nResult)
{
	DSM_OPERATE_MODE enMode = GetCurrentMode();
	SetCurrentMode(DSM_MODE_QUIT_CURRENT_PAGE);
	bool bRet = true;

#if DSM_RECORD_WARE_REALTIME
	AutoLock autoLock(m_lockRecordWaveRealTimeBuf);
	if (NULL != m_pRecordWaveRealTimeBuf)
	{
		PCAP_FILE_HEADER Default_FH =
		{
			PCAP_DEFAULT_MAGIC,
			PCAP_DEFAULT_MAJOR_VERSION,
			PCAP_DEFAULT_MINOR_VERSION,
			PCAP_DEFAULT_THIS_ZONE,
			PCAP_DEFAULT_SIGFIGS,
			PCAP_DEFAULT_SANPLEN,
			PCAP_LINK_TYPE_ETHERNET
		};
		if (enMode == DSM_MODE_FT3_RECV_RECORD_WAVE)
		{
			Default_FH.linkType = PCAP_LINK_TYPE_FT3;
		}

		//保存缓冲区数据
		CFile file;
		if (file.Open(m_strRecordWavePath.c_str(), CFile::modeCreate|CFile::modeWrite))
		{
			try
			{
				file.Write(&Default_FH, sizeof(Default_FH));
				file.Write(m_pRecordWaveRealTimeBuf, m_nRecordWaveIndex);
				file.Close();
			}
			catch (CFileException* e)
			{
				nResult = e->m_cause;
				RETAILMSG(1, (_T("CFileException rason : %d\r\n"), e->m_cause));
				e->Delete();
				bRet = false;
			}
			catch (CException* e)
			{
				nResult = DATA_LAYER_UNKOWN_EXCEPTION;
				RETAILMSG(1, (_T("UnkownException \r\n")));
				e->Delete();
				bRet = false;
			}
		}
		else
		{
			nResult = DATA_LAYER_FILE_OPEN_FAILED;
			bRet = false;
		}

		//释放缓冲区
		delete[] m_pRecordWaveRealTimeBuf;
		m_pRecordWaveRealTimeBuf  = NULL;
		m_nRecordWaveIndex = 0;
	}
#else
	AutoLock autoLock(m_lockPcapFile);

	if (m_pPcapFile)
	{
		if (PCAP_FILE_SUCCESS != m_pPcapFile->Save())
		{
			bRet = false;
		}
		if (PCAP_FILE_SUCCESS != m_pPcapFile->Close())
		{
			bRet = false;
		}
	}
	ReleasePcapFileImpl(m_pPcapFile);
	m_pPcapFile = NULL;
#endif

	return bRet;
}
#endif

///
/// @brief
///    取消当前录波
///
bool CFrameRecv::CancelRecordWave()
{
    SetCurrentMode(DSM_MODE_QUIT_CURRENT_PAGE);

#if CHUN_HUA
#if DSM_RECORD_WARE_REALTIME
    AutoLock autoLock(m_lockRecordWaveRealTimeBuf);
    if (NULL != m_pRecordWaveRealTimeBuf)
    {
        //释放缓冲区
        delete[] m_pRecordWaveRealTimeBuf;
        m_pRecordWaveRealTimeBuf  = NULL;
        m_nRecordWaveIndex = 0;
    }
#else
    AutoLock autoLock(m_lockPcapFile);
    ReleasePcapFileImpl(m_pPcapFile);
    m_pPcapFile = NULL;
#endif

#else
	
	m_nCurrentRecordSize = 0;
	m_nCurrentBufferIndex = 0;
	m_bIsStopRecord = true;

#endif
	

    return true;
}

///
/// @brief
///    清除goose变位列表记录数据
///
void CFrameRecv::_ClearGooseChangeList()
{
    AutoLock autoLock(m_lockGooseChangeList);
    MapDetectItemChangeList::iterator itr = m_mapGooseChangeList.begin();
    while(itr != m_mapGooseChangeList.end())
    {
        if (itr->second != NULL)
        {
            delete itr->second;
            itr->second = NULL;
        }

        ++itr;
    }
    m_mapGooseChangeList.clear();
}

///
/// @brief
///    清空发送采样值时间（计算离散度）信息
///
void CFrameRecv::_ClearSmvTimeDeque()
{
    AutoLock autoLock(m_lockSmvTime);
    m_dequeSmvTime.clear();
}

///
/// @brief
///    告诉数据层，已选择的控制块已经添加完毕
///
void CFrameRecv::AddSelectedCtrlBlkFinished()
{
    //停止探测定时器
    _CloseDetectTimer();

    //停止接收数据
    StopRecvData();

    //设置优先级
    SetRxThreadPriority(DATA_RECV_PROCESS_THREAD_PRIO);

//     //更新9-2控制块采样率信息
//     //如果没有探测到，使用系统设置更新控制块采样率
//     _UpdateSmvRecvPub92SmpRate();

    //界面上已经添加完选择的控制块，在这里设置过滤
    _SetNetFilter();
}

///
/// @brief
///    Goose,Smv9-1/9-2录波处理
///
void CFrameRecv::_NetRecordWave( CFrameRecvNet * pNet, DSM_OPERATE_MODE enOpMode, CBaseDetectItem* pCurrentDetct )
{
    BufferHeader* pBufferHeader = reinterpret_cast<BufferHeader*>(pNet->m_pDataBuf);
    //给缓冲区加锁，在处理缓冲区时防止驱动覆盖缓冲区信息
    pNet->LockBuffer();
    uint32_t nFrameCount = pBufferHeader->NetHeader.nFramesCount;
    uint32_t BufferLen = pBufferHeader->NetHeader.nActiveLength;
    uint32_t nLoopItem = pBufferHeader->NetHeader.nLoopItem;
    uint8_t* pBufferData = NULL;
    if (0 == nLoopItem)
    {
        pBufferData = pNet->m_pDataBuf + sizeof(BufferHeader);
    }
    else if (1 == nLoopItem)
    {
        pBufferData = pNet->m_pDataBuf + sizeof(BufferHeader) + NET_RECV_LOOP_BUF1_POS;
    }

    uint8_t* pAllocRecvBuffer = NULL;
#if DRIVER_BUFFER_USE_CPY
    pAllocRecvBuffer = new uint8_t[BufferLen];
    memcpy_s(pAllocRecvBuffer, BufferLen, pBufferData, BufferLen);
    CNetRecvBuffer recvBuffer(pAllocRecvBuffer, nFrameCount, BufferLen);
#else
    CNetRecvBuffer recvBuffer(pBufferData, nFrameCount, BufferLen);
#endif

    enumFrameType enFrameType = FRAME_TYPE_UNKOWN;
    uint32_t nFrameLen = 0;
    uint8_t *pFrame = NULL;

    MsgRapidDecoder& msgRapidDecoder = pNet->m_cMsgRapidDecoder;
    FrameInfo stFrameInfo = {0};
    PCAP_FILE_TIME_VAL stPcapFileTimeVal = {0};
    uint8_t MacAddr[ETHER_MAC_LEN] = {0};

    for ( uint32_t i=0; i < nFrameCount; ++i)
    {
        if (recvBuffer.GetBufferFrameAddr(i, &pFrame, nFrameLen))
        {
            enFrameType = CNetRecvBuffer::GetFrameType(pFrame, nFrameLen);
            uint16_t nAppid = 0;

            msgRapidDecoder.GetAppID(pFrame, nFrameLen, nAppid);
            msgRapidDecoder.GetDstMac(pFrame, nFrameLen, MacAddr);
            recvBuffer.GetBufferFrameInfo(i, stFrameInfo);
            stPcapFileTimeVal.timeStamp_Second = stFrameInfo.timeStamp.ts_sec;
            stPcapFileTimeVal.timeStamp_USec   = stFrameInfo.timeStamp.ts_usec;

            if (_IsAppidMacInRecordWaveList(nAppid, MacAddr, ETHER_MAC_LEN) )
            {
#if DSM_RECORD_WARE_REALTIME
                if (!_WritePckToBuf(stPcapFileTimeVal, pFrame, nFrameLen))
                {
                    break;
                }
#else
                AutoLock autoLock(m_lockPcapFile);
                if (m_pPcapFile)
                {
                    m_pPcapFile->WriteNextPck(stPcapFileTimeVal, pFrame, nFrameLen);
                }
#endif
            }
        }
        else
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_GenNetDetectInfo: Get Frame Content Failed \n")));
            goto errorHandle;
        }
    }

errorHandle:
    //给缓冲区解锁
    pNet->UnLockBuffer();
    if (NULL != pAllocRecvBuffer)
    {
        delete[] pAllocRecvBuffer;
        pAllocRecvBuffer = NULL;
    }
}

///
/// @brief
///    FT3录波处理
///
void CFrameRecv::_Ft3RecordWave( CFrameRecvFt3 * pFt3, DSM_OPERATE_MODE enOpMode, CBaseDetectItem* pCurrentDetct )
{
    BufferHeader* pBufferHeader = reinterpret_cast<BufferHeader*>(pFt3->m_pDataBuf);
    //给缓冲区加锁，在处理缓冲区时防止驱动覆盖缓冲区信息
    pFt3->LockBuffer();
    uint32_t nFrameCount = pBufferHeader->Ft3Header.nFramesCount;
    uint32_t BufferLen =  pBufferHeader->Ft3Header.nActiveLength;
    uint32_t nLoopItem = pBufferHeader->Ft3Header.nLoopItem;
    uint8_t* pBufferData = NULL;
    if (0 == nLoopItem)
    {
        pBufferData = pFt3->m_pDataBuf + sizeof(BufferHeader);
    }
    else if (1 == nLoopItem)
    {
        pBufferData = pFt3->m_pDataBuf + sizeof(BufferHeader) + FT3_RECV_LOOP_BUF1_POS;
    }

    CFt3RecvBuffer recvBuffer(pBufferData, nFrameCount, BufferLen);
    recvBuffer.SetBufferHeader(*pBufferHeader);

    PCAP_FILE_TIME_VAL stPcapFileTimeVal = {0};
    uint8_t* pFrame = NULL;
    uint32_t nFrameLen = 0;

    if (_IsAppidMacInRecordWaveList(pCurrentDetct))
    {
#if DSM_RECORD_WARE_REALTIME
        for ( uint32_t i=0; i < nFrameCount; ++i)
        {
            if (recvBuffer.GetBufferFrameAddr(i, &pFrame, nFrameLen))
            {
                UTC_TIME_t stTimeStamp = {0};
                recvBuffer.GetBufferFrameTimeStamp(i, stTimeStamp);
                stPcapFileTimeVal.timeStamp_Second = stTimeStamp.uSecondSinceEpoch;
                stPcapFileTimeVal.timeStamp_USec   = stTimeStamp.uFractionOfSecond;
                if (!_WritePckToBuf(stPcapFileTimeVal, pFrame, nFrameLen))
                {
                    break;
                }
            }
        }
#else
        AutoLock autoLock(m_lockPcapFile);
        for ( uint32_t i=0; i < nFrameCount; ++i)
        {
            if (recvBuffer.GetBufferFrameAddr(i, &pFrame, nFrameLen))
            {
                UTC_TIME_t stTimeStamp = {0};
                recvBuffer.GetBufferFrameTimeStamp(i, stTimeStamp);
                stPcapFileTimeVal.timeStamp_Second = stTimeStamp.uSecondSinceEpoch;
                stPcapFileTimeVal.timeStamp_USec   = stTimeStamp.uFractionOfSecond;
                if (m_pPcapFile)
                {
                    m_pPcapFile->WriteNextPck(stPcapFileTimeVal, pFrame, nFrameLen);
                }
            }
        }
#endif
    }

    //给缓冲区解锁
    pFt3->UnLockBuffer();
}

///
/// @brief
///    判断要当前appid，mac地址对应的数据帧是否在录波的列表内
///
/// @return
///     如果当前appid/mac地址在录波列表返回true
///
bool CFrameRecv::_IsAppidMacInRecordWaveList( uint16_t nAppid, uint8_t* pMacAddr, uint8_t nLenMac )
{
    if (nLenMac < ETHER_MAC_LEN)
    {
        return false;
    }
    std::vector<CBaseDetectItem*>::iterator itr = m_vecRecordWaveList.begin();
    while(itr != m_vecRecordWaveList.end())
    {
        if (_IsNetFrame((*itr)->enFrameType))
        {
            CNetDetectItem* pNetDetectItem = dynamic_cast<CNetDetectItem*>((*itr));
            if (pNetDetectItem)
            {
                if (pNetDetectItem->appId == nAppid && 0 == memcmp(pNetDetectItem->macAddr, pMacAddr, ETHER_MAC_LEN) )
                {
                    return true;
                }
            }
        }
        ++itr;
    }
    return false;
}

///
/// @brief
///    判断要当前控制块对应的数据帧是否在录波的列表内
///
/// @return
///     如果当前appid/mac地址在录波列表返回true
///
bool CFrameRecv::_IsAppidMacInRecordWaveList( CBaseDetectItem* pDetectItem )
{
    if (!pDetectItem)
    {
        return false;
    }

    std::vector<CBaseDetectItem*>::iterator itr = m_vecRecordWaveList.begin();
    while(itr != m_vecRecordWaveList.end())
    {
        if ((*itr)->nID == pDetectItem->nID)
        {
            return true;
        }
        ++itr;
    }
    return false;
}

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
void CFrameRecv::StartRecvData( NetFt3RecvFrameType enRecvType, DSM_OPERATE_MODE enMode /*= DSM_MODE_UNKOWN*/ )
{
    StopRecvData();
    m_enCurrentDetectType = enRecvType;
    SetCurrentMode(enMode);
    //开始探测，以获取到网络上所有报文
    _Detect(enRecvType, NET_DEFAULT_RECV_FRAMES, NET_DEFAULT_RECV_ENABLETIMEOUT, NET_DEFAULT_RECV_TIMEOUT);
}

///
/// @brief
///    停止数据接收
///
void CFrameRecv::StopRecvData()
{
    StopPortFrameRecv(NET_PORT_LIGHT_A);
    StopPortFrameRecv(NET_PORT_LIGHT_B);
    StopPortFrameRecv(NET_PORT_ELECTRONIC);
    StopPortFrameRecv(FT3_PORT_LIGHT_A);
    StopPortFrameRecv(FT3_PORT_LIGHT_B);
}

///
/// @brief
///    核相处理流程
///
void CFrameRecv::_NetPhaseCheck( CFrameRecvNet * pNet)
{
    CNetDetectItem* pNetDetectItem1 = dynamic_cast<CNetDetectItem*>(m_pCurrentDetectItems[0]);
    CNetDetectItem* pNetDetectItem2 = dynamic_cast<CNetDetectItem*>(m_pCurrentDetectItems[1]);

    //只有选择了两个控制块（允许两个控制块相同），才进行数据处理
    if (pNetDetectItem1 && pNetDetectItem2)
    {
        //界面上需要显示的数据是原始值/一次/二次值
        enumSmvValueType  enSmvDispType = SMV_VALUE_TYPE_ORIGINANL;
        CSmvRecvConfigPublic* pSmvRecvCfgPub = CDsmDataLayer::getInstance()->GetSmvConfigPublic();
        if (pSmvRecvCfgPub)
        {
            enSmvDispType = pSmvRecvCfgPub->GetSmvDispType();
        }

        //设置的数据帧报文类型是一次/二次值
        enumSmvValueType enFrameValueType = SMV_VALUE_TYPE_SECONDARY;
        CParamConfigBasic* pParamConfigBasic = CDsmDataLayer::getInstance()->GetParamConfigBasic();
        if (pParamConfigBasic)
        {
            enFrameValueType = pParamConfigBasic->GetFrameValueType();
        }

        //给缓冲区加锁，在处理缓冲区时防止驱动覆盖缓冲区信息
        pNet->LockBuffer();
        BufferHeader* pBufferHeader = reinterpret_cast<BufferHeader*>(pNet->m_pDataBuf);
        uint32_t nFrameCount = pBufferHeader->NetHeader.nFramesCount;
        uint32_t BufferLen =  pBufferHeader->NetHeader.nActiveLength;;
        uint32_t nLoopItem = pBufferHeader->NetHeader.nLoopItem;
        uint8_t* pBufferData = NULL;
        if (0 == nLoopItem)
        {
            pBufferData = pNet->m_pDataBuf + sizeof(BufferHeader);
        }
        else if (1 == nLoopItem)
        {
            pBufferData = pNet->m_pDataBuf + sizeof(BufferHeader) + NET_RECV_LOOP_BUF1_POS;
        }

        CNetRecvBuffer recvBuffer(pBufferData, nFrameCount, BufferLen);

        MsgRapidDecoder& msgRapidDecoder = pNet->m_cMsgRapidDecoder;
        Sav91Msg* pSav91Msg   = pNet->m_pSav91Msg;
        Sav92Msg* pSav92Msg   = pNet->m_pSav92Msg;

        uint32_t nFrameLen    = 0;
        uint8_t* pFrame       = NULL;
        SMV_VALUE stSmvValue;
        SMV_VALUE_FT3_9_1 stSmvValueFt3_91 = {0};
        SMV_VALUE_9_2     stSmvValue92 = {0};

        uint16_t nAppid  = 0;
        uint16_t nSmpCnt = 0;

        //记录以太网帧到达时间
        FrameInfo stFrameInfo = {0};
        for ( uint32_t i=0; i < nFrameCount; ++i)
        {
            if (recvBuffer.GetBufferFrameAddr(i, &pFrame, nFrameLen))
            {
                recvBuffer.GetBufferFrameInfo(i, stFrameInfo);

                enumFrameType enFrameType = CNetRecvBuffer::GetFrameType(pFrame, nFrameLen);
                //判断数据帧类型是9-1or9-2
                if (FRAME_TYPE_NET_9_1 == enFrameType)
                {
                    msgRapidDecoder.GetAppID(pFrame, nFrameLen, nAppid);

                    //如果当前缓存的数据块指针变为空，说明可能进行了模式切换/重新探测等操作导致当前正在处理的控制块指针失效
                    if (NULL == _GetCachedCtrlBlk())
                    {
                        goto errorHandle;
                    }
                    else
                    {
                        if (pNetDetectItem1 && pNetDetectItem2)
                        {
                            bool bMatch = false;
                            if (nAppid == pNetDetectItem1->appId && pNet->m_enPortName == pNetDetectItem1->enPortName)
                            {
                                stSmvValueFt3_91.pDetectItem = pNetDetectItem1;
                                pSav91Msg  = pNet->m_pSav91Msg;
                                bMatch = true;
                            }
                            else if (nAppid == pNetDetectItem2->appId && pNet->m_enPortName == pNetDetectItem2->enPortName)
                            {
                                stSmvValueFt3_91.pDetectItem = pNetDetectItem2;
                                pSav91Msg  = pNet->m_pSav91Msg2;
                                bMatch = true;
                            }
                            if (!bMatch)
                            {
                                //当前数据帧的appid与选择的控制块的appid不同，处理下一数据帧
                                continue;
                            }
                        }
                        else
                        {
                            goto errorHandle;
                        }
                    }

                    if (0 == pSav91Msg->Decode(pFrame, nFrameLen))
                    {
                        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_NetPhaseCheck(): pSav91Msg->Decode Failed \n ")));
                        continue;
                    }

                    size_t nSmvChannels = pSav91Msg->GetASDUDefaultSmpsNum();
                    //记录实时报文通道数目
                    {
                        AutoLock autoLock(m_lockRealTimeChannelCounts);
                        if (stSmvValueFt3_91.pDetectItem->nID == m_pCurrentDetectItems[0]->nID)
                        {
                            m_nRealTimeChannelCounts[0] = nSmvChannels;
                        }
                        if (stSmvValueFt3_91.pDetectItem->nID == m_pCurrentDetectItems[1]->nID)
                        {
                            m_nRealTimeChannelCounts[1] = nSmvChannels;
                        }
                    }
                    if (nSmvChannels != NET_9_1_CHANNELS_PER_FRAME)
                    {
                        RETAILMSG(OUTPUT_LOG_MSG, (_T("9-1 Channel Count Not 12 \r\n")));
                        continue;
                    }
                    stSmvValueFt3_91.nChannelCount = nSmvChannels;

                    uint16_t noASDU = pSav91Msg->GetNoASDU();
                    for ( uint16_t j = 0; j < noASDU; ++j)
                    {
                        pSav91Msg->GetSmpCntInASDU(j, nSmpCnt);
                        stSmvValueFt3_91.nSmpCnt = nSmpCnt;

                        //对12.8k采样率进行重采样到4k
                        if (stSmvValueFt3_91.pDetectItem->nSampleRate == 12800 )
                        {
                            uint32_t nSafeSmpCnt = nSmpCnt % 12800;
                            if (smp12800to4000[nSafeSmpCnt])
                            {
                                stSmvValueFt3_91.nSmpCnt = smp12800to4000[nSafeSmpCnt] - 1;
                            }
                            else
                            {
                                continue;
                            }
                        }

                        stSmvValueFt3_91.stFrameTime = stFrameInfo.timeStamp + j * (1000000/stSmvValueFt3_91.pDetectItem->nSampleRate); 

                        pSav91Msg->GetASDUSmps(j, reinterpret_cast<uint16_t*>(stSmvValueFt3_91.nSmvValue), nSmvChannels);

                        if(_SmvValueConvert(stSmvValueFt3_91, stSmvValue, enSmvDispType, enFrameValueType))
                        {
                            _PhaseCheckDataHandle(stSmvValue);
                        }

                    }
                }
                else if (FRAME_TYPE_NET_9_2 == enFrameType)
                {
                    msgRapidDecoder.GetAppID(pFrame, nFrameLen, nAppid);

                    //如果当前缓存的数据块指针变为空，说明可能进行了模式切换/重新探测等操作导致当前正在处理的控制块指针失效
                    if (_GetCachedCtrlBlk() == NULL)
                    {
                        goto errorHandle;
                    }
                    else
                    {
                        if (pNetDetectItem1 && pNetDetectItem2)
                        {
                            bool bMatch = false;
                            if (nAppid == pNetDetectItem1->appId && pNet->m_enPortName == pNetDetectItem1->enPortName)
                            {
                                stSmvValue92.pDetectItem = pNetDetectItem1;
                                pSav92Msg  = pNet->m_pSav92Msg;
                                bMatch = true;
                            }
                            else if (nAppid == pNetDetectItem2->appId && pNet->m_enPortName == pNetDetectItem2->enPortName)
                            {
                                stSmvValue92.pDetectItem = pNetDetectItem2;
                                pSav92Msg  = pNet->m_pSav92Msg2;
                                bMatch = true;
                            }
                            if (!bMatch)
                            {
                                //当前数据帧的appid与选择的控制块的appid不同，处理下一数据帧
                                continue;
                            }
                        }
                        else
                        {
                            goto errorHandle;
                        }
                    }

                    if (0 == pSav92Msg->Decode(pFrame, nFrameLen))
                    {
                        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_NetPhaseCheck(): pSav92Msg->Decode Failed \n ")));
                        continue;
                    }

                    uint16_t noASDU = pSav92Msg->GetNoASDU();
                    for ( uint16_t j = 0; j < noASDU; ++j)
                    {
                        size_t nChannelCount = pSav92Msg->GetASDUSmpsNum(j);
                        //记录实时报文通道数目
                        {
                            AutoLock autoLock(m_lockRealTimeChannelCounts);
                            if (stSmvValue92.pDetectItem->nID == m_pCurrentDetectItems[0]->nID)
                            {
                                m_nRealTimeChannelCounts[0] = nChannelCount;
                            }
                            if (stSmvValue92.pDetectItem->nID == m_pCurrentDetectItems[1]->nID)
                            {
                                m_nRealTimeChannelCounts[1] = nChannelCount;
                            }
                        }
                        if (nChannelCount > NET_9_2_LE_MAX_CHANNELS_PER_ASDU)
                        {
                            RETAILMSG(OUTPUT_LOG_MSG, (_T("9-2 Channel Count(%d)\r\n"), nChannelCount));
                            continue;
                        }
                        stSmvValue92.nChannelCount = nChannelCount;
                        pSav92Msg->GetSmpCntInASDU(j, nSmpCnt);
                        stSmvValue92.nSmpCnt = nSmpCnt;

                        //对12.8k采样率进行重采样到4k
                        if (stSmvValue92.pDetectItem->nSampleRate == 12800)
                        {
                            uint32_t nSafeSmpCnt = nSmpCnt % 12800;
                            if (smp12800to4000[nSafeSmpCnt])
                            {
                                stSmvValue92.nSmpCnt = smp12800to4000[nSafeSmpCnt] - 1;
                            }
                            else
                            {
                                continue;
                            }
                        }

                        stSmvValue92.stFrameTime = stFrameInfo.timeStamp + j * (1000000/stSmvValue92.pDetectItem->nSampleRate);
                        pSav92Msg->GetASDUSmps(j, reinterpret_cast<uint32_t*>(stSmvValue92.nSmvValue), NULL, nChannelCount);
                        if(_SmvValueConvert(stSmvValue92, stSmvValue, enSmvDispType, enFrameValueType))
                        {
                            _PhaseCheckDataHandle(stSmvValue);
                        }
                    }
                }
            }
            else
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_NetPhaseCheck: Get Frame Content Failed \n")));
            }
        }

errorHandle:
        //对缓冲区解锁
        pNet->UnLockBuffer();
    }
}

///
/// @brief
///    FT3核相处理流程
///
void CFrameRecv::_Ft3PhaseCheck( CFrameRecvFt3 * pFt3 )
{
    //给缓冲区加锁，在处理缓冲区时防止驱动覆盖缓冲区信息
    pFt3->LockBuffer();
    if (m_pCurrentDetectItems[0] && m_pCurrentDetectItems[1])
    {
        BufferHeader* pBufferHeader = reinterpret_cast<BufferHeader*>(pFt3->m_pDataBuf);
        uint32_t nFrameCount = pBufferHeader->Ft3Header.nFramesCount;
        uint32_t BufferLen =  pBufferHeader->Ft3Header.nActiveLength;
        uint32_t nLoopItem = pBufferHeader->Ft3Header.nLoopItem;
        uint8_t* pBufferData = NULL;
        if (0 == nLoopItem)
        {
            pBufferData = pFt3->m_pDataBuf + sizeof(BufferHeader);
        }
        else if (1 == nLoopItem)
        {
            pBufferData = pFt3->m_pDataBuf + sizeof(BufferHeader) + FT3_RECV_LOOP_BUF1_POS;
        }
        CFt3RecvBuffer recvBuffer(pBufferData, nFrameCount, BufferLen);
        recvBuffer.SetBufferHeader(*pBufferHeader);

        //界面上需要显示的数据是原始值/一次/二次值
        enumSmvValueType  enSmvDispType = SMV_VALUE_TYPE_ORIGINANL;
        CSmvRecvConfigPublic* pSmvRecvCfgPub = CDsmDataLayer::getInstance()->GetSmvConfigPublic();
        if (pSmvRecvCfgPub)
        {
            enSmvDispType = pSmvRecvCfgPub->GetSmvDispType();
        }

        //设置的数据帧报文类型是一次/二次值
        enumSmvValueType enFrameValueType = SMV_VALUE_TYPE_SECONDARY;
        CParamConfigBasic* pParamConfigBasic = CDsmDataLayer::getInstance()->GetParamConfigBasic();
        if (pParamConfigBasic)
        {
            enFrameValueType = pParamConfigBasic->GetFrameValueType();
        }

        enumFrameType enFrameType = recvBuffer.GetBufferFrameType();

        if (FRAME_TYPE_FT3_GUOWANG == enFrameType)
        {
            GuoWangFT3Msg* pGuoWangFt3Msg = pFt3->m_pGuoWangFT3Msg;
            uint32_t nFrameLen = FT3_FRAME_LEN[_FT3_TYPE_GUOWANG];
            uint8_t* pFrame = NULL;
            SMV_VALUE stSmvValue;
            SMV_VALUE_FT3_9_1 stSmvValueFt3_91 = {0};

            if (pFt3->m_enPortName == m_pCurrentDetectItems[0]->enPortName)
            {
                stSmvValueFt3_91.pDetectItem = m_pCurrentDetectItems[0];
            }
            else if (pFt3->m_enPortName == m_pCurrentDetectItems[1]->enPortName)
            {
                stSmvValueFt3_91.pDetectItem = m_pCurrentDetectItems[1];
            }

            for ( uint32_t i=0; i < nFrameCount; ++i)
            {
                if (recvBuffer.GetBufferFrameAddr(i, &pFrame, nFrameLen))
                {
                    if (0 == pGuoWangFt3Msg->Decode(reinterpret_cast<uint16_t*>(pFrame), nFrameLen/2))
                    {
                        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_Ft3PhaseCheck(): pGuoWangFt3Msg->Decode Failed \n ")));
                        continue;
                    }
                    recvBuffer.GetBufferFrameTimeStamp(i, stSmvValueFt3_91.stFrameTime);

                    GUOWANG_FT3_ASDU_PUBLIC stAsduPbl;
                    size_t nChannelCount = pGuoWangFt3Msg->GetASDUSmpsNum();
                    //记录实时报文通道数目
                    {
                        AutoLock autoLock(m_lockRealTimeChannelCounts);
                        if (stSmvValueFt3_91.pDetectItem->nID == m_pCurrentDetectItems[0]->nID)
                        {
                            m_nRealTimeChannelCounts[0] = nChannelCount;
                        }
                        if (stSmvValueFt3_91.pDetectItem->nID == m_pCurrentDetectItems[1]->nID)
                        {
                            m_nRealTimeChannelCounts[1] = nChannelCount;
                        }
                    }
                    if (nChannelCount != GUOWANG_CHANNELS_PER_FRAME)
                    {
                        RETAILMSG(OUTPUT_LOG_MSG, (_T("Guowang Channel Count(%d)\r\n"), nChannelCount));
                        continue;
                    }
                    stSmvValueFt3_91.nChannelCount = nChannelCount;
                    pGuoWangFt3Msg->GetASDUPublicInfo(stAsduPbl);
                    stSmvValueFt3_91.nSmpCnt = stAsduPbl.smpCnt;

                    //对12.8k采样率进行重采样到4k
                    if (stSmvValueFt3_91.pDetectItem->nSampleRate == 12800)
                    {
                        uint32_t nSafeSmpCnt = stAsduPbl.smpCnt % 12800;
                        if (smp12800to4000[nSafeSmpCnt])
                        {
                            stSmvValueFt3_91.nSmpCnt = smp12800to4000[nSafeSmpCnt] - 1;
                        }
                        else
                        {
                            continue;
                        }
                    }

                    pGuoWangFt3Msg->GetASDUSmps(reinterpret_cast<uint16_t*>(stSmvValueFt3_91.nSmvValue), nChannelCount);

                    if ( _SmvValueConvert(stSmvValueFt3_91, stSmvValue, enSmvDispType, enFrameValueType) )
                    {
                        _PhaseCheckDataHandle(stSmvValue);
                    }
                }
                else
                {
                    RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_Ft3PhaseCheck(): recvBuffer.GetBufferFrameContent Failed \n")));
                }
            }
        }
        else if (FRAME_TYPE_FT3_NANRUI == enFrameType)
        {
            NanRuiFT3Msg*  pNanRuiFt3Msg = pFt3->m_pNanRuiFt3Msg;
            uint32_t nFrameLen = FT3_FRAME_LEN[_FT3_TYPE_NANRUI];
            uint8_t* pFrame = NULL;
            SMV_VALUE stSmvValue;
            SMV_VALUE_FT3_9_1 stSmvValueFt3_91 = {0};
            if (pFt3->m_enPortName == m_pCurrentDetectItems[0]->enPortName)
            {
                stSmvValueFt3_91.pDetectItem = m_pCurrentDetectItems[0];
            }
            else if (pFt3->m_enPortName == m_pCurrentDetectItems[1]->enPortName)
            {
                stSmvValueFt3_91.pDetectItem = m_pCurrentDetectItems[1];
            }

            for ( uint32_t i=0; i < nFrameCount; ++i)
            {
                if (recvBuffer.GetBufferFrameAddr(i, &pFrame, nFrameLen))
                {
                    if (0 == pNanRuiFt3Msg->Decode(reinterpret_cast<uint16_t*>(pFrame), nFrameLen/2))
                    {
                        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_Ft3PhaseCheck:pNanRuiFt3Msg->Decode Failed \n ")));
                        continue;
                    }
                    recvBuffer.GetBufferFrameTimeStamp(i, stSmvValueFt3_91.stFrameTime);

                    NANRUI_FT3_ASDU_PUBLIC stAsduPbl;
                    size_t nChannelCount = pNanRuiFt3Msg->GetASDUSmpsNum();
                    //记录实时报文通道数目
                    {
                        AutoLock autoLock(m_lockRealTimeChannelCounts);
                        if (stSmvValueFt3_91.pDetectItem->nID == m_pCurrentDetectItems[0]->nID)
                        {
                            m_nRealTimeChannelCounts[0] = nChannelCount;
                        }
                        if (stSmvValueFt3_91.pDetectItem->nID == m_pCurrentDetectItems[1]->nID)
                        {
                            m_nRealTimeChannelCounts[1] = nChannelCount;
                        }
                    }
                    if (nChannelCount != NANRUI_CHANNELS_PER_FRAME)
                    {
                        RETAILMSG(OUTPUT_LOG_MSG, (_T("NanRui Frame Channel Count(%d)\r\n"), nChannelCount));
                        continue;
                    }
                    stSmvValueFt3_91.nChannelCount = nChannelCount;
                    pNanRuiFt3Msg->GetASDUPublicInfo(stAsduPbl);
                    stSmvValueFt3_91.nSmpCnt = stAsduPbl.smpCnt;

                    //对12.8k采样率进行重采样到4k
                    if (stSmvValueFt3_91.pDetectItem->nSampleRate == 12800)
                    {
                        uint32_t nSafeSmpCnt = stAsduPbl.smpCnt % 12800;
                        if (smp12800to4000[nSafeSmpCnt])
                        {
                            stSmvValueFt3_91.nSmpCnt = smp12800to4000[nSafeSmpCnt] - 1;
                        }
                        else
                        {
                            continue;
                        }
                    }

                    pNanRuiFt3Msg->GetASDUSmps(reinterpret_cast<uint16_t*>(stSmvValueFt3_91.nSmvValue), nChannelCount);

                    if (_SmvValueConvert(stSmvValueFt3_91, stSmvValue, enSmvDispType, enFrameValueType))
                    {
                        _PhaseCheckDataHandle(stSmvValue);
                    }
                }
                else
                {
                    RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_Ft3PhaseCheck: recvBuffer.GetBufferFrameContent Failed \n")));
                }
            }
        }
        else
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_Ft3SmvAnaly(): Ft3 Buffer Frame type Unkown \n ")));
        }
    }

    //给缓冲区解锁
    pFt3->UnLockBuffer();
}

///
/// @brief
///    核相数据处理
///
void CFrameRecv::_PhaseCheckDataHandle( const SMV_VALUE& stSmvValue )
{
    if (m_pCurrentDetectItems[0] && m_pCurrentDetectItems[1])
    {
        //获取采样率信息
        uint32_t nSampleRate = _GetCtrlBlkSampleRate(m_pCurrentDetectItems[0]);
        if (nSampleRate == 12800)
        {
            nSampleRate = 4000;
        }
        uint32_t nPointsPerWave = nSampleRate/DSM_CONFIG_DEFAULT_ELEC_FREQUENCY;
        //获取选中的ABC相别通道
        uint32_t nSmvPhaseABC[PHASE_CHECK_CHANNELS] = {0};
        CSmvRecvConfig* pSmvRecvCofig1 = m_pCurrentDetectItems[0]->pRecvConfig;
        CSmvRecvConfig* pSmvRecvCofig2 = m_pCurrentDetectItems[1]->pRecvConfig;
        if (pSmvRecvCofig1)
        {
            pSmvRecvCofig1->GetPhaseCheckSmv1Channels(nSmvPhaseABC[0], nSmvPhaseABC[1], nSmvPhaseABC[2]);
        }
        if (pSmvRecvCofig2)
        {
            pSmvRecvCofig2->GetPhaseCheckSmv2Channels(nSmvPhaseABC[3], nSmvPhaseABC[4], nSmvPhaseABC[5]);
        }

        //两个控制块相同，即选择的是同一控制块的数据
        if (m_pCurrentDetectItems[0]->nID == m_pCurrentDetectItems[1]->nID)
        {
            //获取锁，使用 m_stDspSmvData使用权限
            m_lockDspSend.Acquire();
            if (m_stDspSmvData.nSmvPerCircle >= nPointsPerWave)
            {
                //如果判断到缓存的采样值的采样数据没有被清零，表示dsp还没有发送，先直接返回
                RETAILMSG(1,(_T("\r\nPhaseCheck Smv send 2 dsp Droped !")));
                SetEvent(m_hEventSmv);
                m_lockDspSend.Release();
                return;
            }
            for (size_t i = 0; i < PHASE_CHECK_CHANNELS; ++i)
            {
                m_stDspSmvData.fChannelData[i][m_stDspSmvData.nSmvPerCircle] = nSmvPhaseABC[i] < stSmvValue.nChannelCount ? stSmvValue.fSmvValue[nSmvPhaseABC[i]] : 0;
                m_stDspSmvData.fScaleFactor[i] = 1.0f;
                m_stDspSmvData.nChannelVCType[i] = _GetCtrlBlkChannelVCType(m_pCurrentDetectItems[0], i, DSM_MODE_SMV_RECV_PHASE_CHECK);
            }
            m_stDspSmvData.nChannelCount = PHASE_CHECK_CHANNELS;
            //如果采样点数达到一个周波，就发送给dsp进行处理
            if (++m_stDspSmvData.nSmvPerCircle == nPointsPerWave)
            {
                SetEvent(m_hEventSmv);
            }
            m_lockDspSend.Release();
        }
        //两个控制块不一样，需要对齐数据交给dsp发送
        else
        {
            //获取对齐方式
            SMV_PHASE_CHECK_ALIGNMENT enPhaseCheckAlign = SMV_PHASE_CHECK_ALIGN_INDEX;
            CSmvRecvConfigPublic* pRecvConfigPub = CDsmDataLayer::getInstance()->GetSmvConfigPublic();
            if (pRecvConfigPub)
            {
                enPhaseCheckAlign = pRecvConfigPub->GetPhaseCheckAlign();
            }

            //使用序号对齐
            if(SMV_PHASE_CHECK_ALIGN_INDEX == enPhaseCheckAlign)
            {
                //每次过来一个采样值，判断是哪一个控制块的，然后到对方控制块缓冲区找对齐数据

                //采样序号对缓冲区长度取模，避免超过缓冲区数据范围导致越界
                uint32_t nSafeSmpCnt = stSmvValue.nSmpCnt % m_nPhaseCheckIndexLen;
                if (stSmvValue.pDetectItem->nID == m_pCurrentDetectItems[0]->nID)
                {
                    //此块处理数据对齐，自身数据的缓存
                    {
                        AutoLock autoLock(m_lockPhaseCheckData);
                        if(m_pPhaseCheckAIndex == NULL)
                        {
                            m_pPhaseCheckAIndex = new SMV_VALUE[m_nPhaseCheckIndexLen];
                        }
                        m_pPhaseCheckAIndex[nSafeSmpCnt] = stSmvValue;
                    }
                }
                else if (stSmvValue.pDetectItem->nID == m_pCurrentDetectItems[1]->nID)
                {
                     //此块处理数据对齐，自身数据的缓存
                    {
                        AutoLock autoLock(m_lockPhaseCheckData);
                        if(m_pPhaseCheckBIndex == NULL)
                        {
                            m_pPhaseCheckBIndex = new SMV_VALUE[m_nPhaseCheckIndexLen];
                        }
                        m_pPhaseCheckBIndex[nSafeSmpCnt] = stSmvValue;
                    }
                }

                //分别从两个缓冲区取数据，交给dsp
                if (stSmvValue.pDetectItem->nID == m_pCurrentDetectItems[0]->nID)
                {
                    SMV_VALUE stSmvValue1, stSmvValue2;
                    uint32_t nSafeCurIndex = m_nPhaseCheckCurIndex % m_nPhaseCheckIndexLen;

                    {
                        AutoLock autolock(m_lockPhaseCheckData);
                        if (m_pPhaseCheckAIndex && m_pPhaseCheckAIndex[nSafeCurIndex].pDetectItem != NULL)
                        {
                            stSmvValue1 = m_pPhaseCheckAIndex[nSafeCurIndex];
                        }
                        else
                        {
                            return;
                        }

                        if (m_pPhaseCheckBIndex && m_pPhaseCheckBIndex[nSafeCurIndex].pDetectItem != NULL)
                        {
                            stSmvValue2 = m_pPhaseCheckBIndex[nSafeCurIndex];
                        }
                        else
                        {
                            return;
                        }
                    }

                    //获取锁，使用 m_stDspSmvData使用权限
                    m_lockDspSend.Acquire();
                    if (m_stDspSmvData.nSmvPerCircle >= nPointsPerWave)
                    {
                        //如果判断到缓存的采样值的采样数据没有被清零，表示dsp还没有发送，先直接返回
                        SetEvent(m_hEventSmv);
                        m_lockDspSend.Release();
                        RETAILMSG(1,(_T("\r\nPhaseCheck1 Smv send 2 dsp Droped !")));
                        return;
                    }

                    //自身是第二控制块，将查找到的数据放入前3个通道，自己数据放入后三个通道
                    for (size_t i = 0; i < 3; ++i)
                    {
                        m_stDspSmvData.fChannelData[i][m_stDspSmvData.nSmvPerCircle] = nSmvPhaseABC[i] < stSmvValue1.nChannelCount ? stSmvValue1.fSmvValue[nSmvPhaseABC[i]] : 0;
                        m_stDspSmvData.fScaleFactor[i] = 1.0f;
                        m_stDspSmvData.nChannelVCType[i] = _GetCtrlBlkChannelVCType(m_pCurrentDetectItems[0], i, DSM_MODE_SMV_RECV_PHASE_CHECK);
                    }
                    for (size_t i = 3; i < PHASE_CHECK_CHANNELS; ++i)
                    {
                        m_stDspSmvData.fChannelData[i][m_stDspSmvData.nSmvPerCircle] = nSmvPhaseABC[i] < stSmvValue2.nChannelCount ? stSmvValue2.fSmvValue[nSmvPhaseABC[i]] : 0;
                        m_stDspSmvData.fScaleFactor[i] = 1.0f;
                        m_stDspSmvData.nChannelVCType[i] = _GetCtrlBlkChannelVCType(m_pCurrentDetectItems[0], i, DSM_MODE_SMV_RECV_PHASE_CHECK);
                    }

                    m_stDspSmvData.nChannelCount = PHASE_CHECK_CHANNELS;
                    //如果采样点数达到一个周波，就发送给dsp进行处理
                    if (++m_stDspSmvData.nSmvPerCircle == nPointsPerWave)
                    {
                        SetEvent(m_hEventSmv);
                    }
                    m_nPhaseCheckCurIndex = (m_nPhaseCheckCurIndex + 1) % m_nPhaseCheckIndexLen;
                    m_lockDspSend.Release();
                }
            }
            //时标对齐
            else if (SMV_PHASE_CHECK_ALIGN_TIME == enPhaseCheckAlign)
            {
                //使用朝泽提供的接口进行时标对齐（包括插值计算）
#if 1
                AutoLock autoLock(m_lockPhaseCheckData);
                if (!(m_pResultY && m_nResultYLen >= PHASE_CHECK_CHANNELS * nPointsPerWave))
                {
                    uint32_t nSize = nPointsPerWave * PHASE_CHECK_CHANNELS;
                    float* pTmp = new float[nSize];
                    delete[] m_pResultY;
                    m_pResultY = pTmp;
                    m_nResultYLen = nSize;
                }

                {
                    uint32_t nCtrlBlkIndex = 0;
                    if (stSmvValue.pDetectItem->nID == m_pCurrentDetectItems[1]->nID)
                    {
                        nCtrlBlkIndex = 1;
                    }
                    m_cSmpSyncTime.AddASample(nCtrlBlkIndex, stSmvValue);
                    if ( m_cSmpSyncTime.IsSampleSync() )
                    {
                        //获取对齐结果
                        m_cSmpSyncTime.GetSyncSamples(m_pResultY, PHASE_CHECK_CHANNELS, nPointsPerWave);
                        //对对齐结果进行处理
                        {
                            m_lockDspSend.Acquire();
                            if (m_stDspSmvData.nSmvPerCircle >= nPointsPerWave)
                            {
                                //如果判断到缓存的采样值的采样数据没有被清零，表示dsp还没有发送，先直接返回
                                SetEvent(m_hEventSmv);
                                m_lockDspSend.Release();
                                RETAILMSG(1,(_T("\r\nPhaseCheck1 Smv send 2 dsp Droped !")));
                                return;
                            }

                            for (size_t i = 0; i < nPointsPerWave; ++i)
                            {
                                for (size_t j = 0; j < PHASE_CHECK_CHANNELS; ++j)
                                {
                                    m_stDspSmvData.fChannelData[j][i] = m_pResultY[j * nPointsPerWave + i];
                                }
                            }
                            for (size_t j = 0; j < PHASE_CHECK_CHANNELS; ++j)
                            {
                                m_stDspSmvData.fScaleFactor[j]    = 1.0f;
                                m_stDspSmvData.nChannelVCType[j]  = _GetCtrlBlkChannelVCType(m_pCurrentDetectItems[nCtrlBlkIndex], j, DSM_MODE_SMV_RECV_PHASE_CHECK);
                            }
                            m_stDspSmvData.nChannelCount = PHASE_CHECK_CHANNELS;
                            m_stDspSmvData.nSmvPerCircle = nPointsPerWave;

                            SetEvent(m_hEventSmv);
                            m_lockDspSend.Release();
                        }
                    }
                }
#endif
            } //END enPhaseCheckAlign
        } //END 两控制块是否相同
    }
}

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
bool CFrameRecv::GetSmv1ABCChannles( uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC )
{
    if (m_pCurrentDetectItems[0] && m_pCurrentDetectItems[0]->pRecvConfig)
    {
        return m_pCurrentDetectItems[0]->pRecvConfig->GetPhaseCheckSmv1Channels(nPhaseA, nPhaseB, nPhaseC);
    }
    return false;
}

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
bool CFrameRecv::SetSmv1ABCChannles( uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC )
{
    if (m_pCurrentDetectItems[0] && m_pCurrentDetectItems[0]->pRecvConfig)
    {
        //设置核相控制块时标对齐通道参数
        uint32_t nChannles[3] = {nPhaseA, nPhaseB, nPhaseC};
        m_cSmpSyncTime.SetSyncChannelIdx(0,nChannles,3);

        return m_pCurrentDetectItems[0]->pRecvConfig->SetPhaseCheckSmv1Channels(nPhaseA, nPhaseB, nPhaseC);
    }
    return false;
}

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
bool CFrameRecv::GetSmv2ABCChannles( uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC )
{
    if (m_pCurrentDetectItems[1] && m_pCurrentDetectItems[1]->pRecvConfig)
    {
        return m_pCurrentDetectItems[1]->pRecvConfig->GetPhaseCheckSmv2Channels(nPhaseA, nPhaseB, nPhaseC);
     }
    return false;
}

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
bool CFrameRecv::SetSmv2ABCChannles( uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC )
{
    if (m_pCurrentDetectItems[1] && m_pCurrentDetectItems[1]->pRecvConfig)
    {
        //设置核相控制块时标对齐通道参数
        uint32_t nChannles[3] = {nPhaseA, nPhaseB, nPhaseC};
        m_cSmpSyncTime.SetSyncChannelIdx(1,nChannles,3);
        return m_pCurrentDetectItems[1]->pRecvConfig->SetPhaseCheckSmv2Channels(nPhaseA, nPhaseB, nPhaseC);
    }
    return false;
}

///
/// @brief
///    设置接收线程优先级
///
void CFrameRecv::SetRxThreadPriority(int nPriority )
{
    if (m_pFrameFt3A)
    {
        m_pFrameFt3A->SetRxThreadPriority(nPriority);
    }
    if (m_pFrameNetA)
    {
        m_pFrameNetA->SetRxThreadPriority(nPriority);
    }
#if GPM_DRIVER_ENABLE_ALL_PORT
    if (m_pFrameFt3B)
    {
        m_pFrameFt3B->SetRxThreadPriority(nPriority);
    }
    if (m_pFrameNetB)
    {
        m_pFrameNetB->SetRxThreadPriority(nPriority);
    }
    if (m_pFrameNetC)
    {
        m_pFrameNetC->SetRxThreadPriority(nPriority);
    }
#endif
}

///
/// @brief
///    配置dsp计算使用的采样率
///
bool CFrameRecv::SetDspSampleRate( uint32_t nSampleRate )
{
    if (nSampleRate != m_cDspConfig.nSamplingFreq)
    {
        m_cDspConfig.nSamplingFreq = nSampleRate;
        if (m_pDsp)
        {
            return m_pDsp->SetConfig(m_cDspConfig);
        }
        return false;
    }
    return true;
}


///
/// @brief
///    配置dsp中计算离散度使用的Asdu数目
///
bool CFrameRecv::SetDspAsduCount( uint32_t nAsdu /*= 1*/ )
{
    if (nAsdu != m_cDspConfig.nASDUCount)
    {
        m_cDspConfig.nASDUCount = nAsdu;
        if (m_pDsp)
        {
            return m_pDsp->SetConfig(m_cDspConfig);
        }
        return false;
    }
    return true;
}

///
/// @brief
///    开始/停止极性信息检测
///
/// @param[in]  bStart  是否开始极性监测，true开始监测，false停止监测
///
/// @note
///     只有在极性检查模式下生效
void CFrameRecv::StartPolarityCheck( bool bStart /*= true*/ )
{
    m_bPoLarityCheckBegin = bStart;
}

///
/// @brief
///    清空缓存的appid接收配置信息
///
void CFrameRecv::ClearAppidSmvRecvCache()
{
    std::map<uint16_t, CSmvRecvConfig*>::iterator itr = m_mapAppIDSmvRecv91Config.begin();
    while (itr != m_mapAppIDSmvRecv91Config.end())
    {
        if (itr->second != NULL)
        {
            delete itr->second;
            itr->second = NULL;
        }
        ++itr;
    }
    m_mapAppIDSmvRecv91Config.clear();

    itr = m_mapAppIDSmvRecv92Config.begin();
    while (itr != m_mapAppIDSmvRecv92Config.end())
    {
        if (itr->second != NULL)
        {
            delete itr->second;
            itr->second = NULL;
        }
        ++itr;
    }
    m_mapAppIDSmvRecv92Config.clear();

    uint32_t nCount = sizeof(m_pFt3SmvRecvCofig)/sizeof(m_pFt3SmvRecvCofig[0]);
    for ( uint32_t i = 0; i < nCount; ++i )
    {
        if (NULL != m_pFt3SmvRecvCofig[i])
        {
            delete m_pFt3SmvRecvCofig[i];
            m_pFt3SmvRecvCofig[i] = NULL;
        }
    }
}

///
/// @brief
///    判断某个appid是否在过滤列表中
///
/// @param[out]     如果传入参数不为空，则保存当前数据匹配的控制块指针
///
///
bool CFrameRecv::_IsAppidInNetFilter( uint16_t nAppid, DsmPortName enPortName/* = DSM_PORT_UNKOWN*/, CNetDetectItem** pNetItem /*= NULL*/)
{
    bool bIgnorePortName = false;
    if (enPortName == DSM_PORT_UNKOWN)
    {
        bIgnorePortName = true;
    }
    std::vector<CBaseDetectItem*>::iterator itr = m_vecSelectedItems.begin();
    while(itr != m_vecSelectedItems.end())
    {
        CNetDetectItem* pNetDetectItem = dynamic_cast<CNetDetectItem*>(*itr);
        if (pNetDetectItem && nAppid == pNetDetectItem->appId)
        {
            if (bIgnorePortName)
            {
                if (pNetItem)
                {
                    *pNetItem = pNetDetectItem;
                }
                return true;
            }
            else
            {
                if (pNetDetectItem->enPortName == enPortName)
                {
                    if (pNetItem)
                    {
                        *pNetItem = pNetDetectItem;
                    }
                    return true;
                }
            }
        }

        ++itr;
    }
    return false;
}

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
bool CFrameRecv::SetSmvRecvChannelTemplate( enumSmvRecvConfigTemplateType enConfigType, CSmvRecvConfig* pSmvRecvConfig )
{
    if (pSmvRecvConfig)
    {
        //获取设置中一次/二次/额定延时数据
        float fPrimaryAmp = 0, fSecondaryAmp = 0;
        float fPrimaryVol = 0, fSecondaryVol = 0;
        uint32_t nRatedMuDelay = 0;
        CParamConfigBasic* pParamConfigBasic = CDsmDataLayer::getInstance()->GetParamConfigBasic();
        if (pParamConfigBasic)
        {
            double dfPrimaryAmp   = pParamConfigBasic->GetRatedPrimaryCurrent();
            double dfSecondaryAmp = pParamConfigBasic->GetRatedSecondaryCurrent();
            double dfPrimaryVol   = pParamConfigBasic->GetRatedPrimaryVoltage();
            double dfSecondaryVol = pParamConfigBasic->GetRatedSecondaryVoltage();

            std::wstring strMuDelay = pParamConfigBasic->GetMuRatedDelay();
            StrUint32Convert(strMuDelay, NULL, &nRatedMuDelay);

            //设置中保存的数据是按照kv/a为单位的
            fPrimaryAmp = static_cast<float>(dfPrimaryAmp);
            fPrimaryVol = static_cast<float>(1000 * dfPrimaryVol);
            fSecondaryAmp = static_cast<float>(dfSecondaryAmp);
            fSecondaryVol = static_cast<float>(dfSecondaryVol);
        }

         if (enConfigType == SMV_RECV_CONFIG_TEMPLATE_FROME_SCL)
         {
             CSCLManager* pSclManager = CDsmDataLayer::getInstance()->GetSclManager();
             if (pSclManager)
             {
                 uint16_t nAppid = 0;
                 CBaseDetectItem* pBaseDetectItem = pSmvRecvConfig->GetCtrlBlk();
                 if (pBaseDetectItem && pBaseDetectItem->enFrameType == FRAME_TYPE_NET_9_2)
                 {
                     CNetDetectItem* pNetDetectItem = dynamic_cast<CNetDetectItem*>(pBaseDetectItem);
                     if (pNetDetectItem)
                     {
                         nAppid = pNetDetectItem->appId;
                     }
                 }
                 CIEC9_2Cb* pSmv92Cb = pSclManager->GetIEC9_2Cb(nAppid);
                 if (pSmv92Cb)
                 {
                     CChannel* pChannel = pSmv92Cb->FirstChannel();
                     uint32_t    nIndex = 0;
                     CSmvRecvChannelInfo stSmvRecvChannelInfo;
                     while(pChannel)
                     {
                         stSmvRecvChannelInfo.nIndex = nIndex;
                         stSmvRecvChannelInfo.pOwnerCtrlBlk = NULL;
                         stSmvRecvChannelInfo.enChannelValueType = CHANNEL_VALUE_TYPE_UNKNOWN;
                         stSmvRecvChannelInfo.enChannelPhase = pChannel->GetChannelPhase();
                         stSmvRecvChannelInfo.enChannelType = pChannel->GetChannelType();
                         stSmvRecvChannelInfo.strChannelDesc = pChannel->GetDescription();

                         if (stSmvRecvChannelInfo.enChannelType == CHANNEL_TYPE_SMV_CURRENT)
                         {
                             stSmvRecvChannelInfo.fPrimaryRatedValue = fPrimaryAmp;
                             stSmvRecvChannelInfo.fSecondaryRatedValue = fSecondaryAmp;
                         }
                         else if (stSmvRecvChannelInfo.enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
                         {
                             stSmvRecvChannelInfo.fPrimaryRatedValue = fPrimaryVol;
                             stSmvRecvChannelInfo.fSecondaryRatedValue = fSecondaryVol;
                         }
                         else if (stSmvRecvChannelInfo.enChannelType == CHANNEL_TYPE_SMV_TIME)
                         {
                             stSmvRecvChannelInfo.fPrimaryRatedValue = static_cast<float>(nRatedMuDelay);
                             stSmvRecvChannelInfo.fSecondaryRatedValue = static_cast<float>(nRatedMuDelay);
                         }
                         pSmvRecvConfig->SetOneChannelInfo(nIndex, &stSmvRecvChannelInfo);
                         nIndex++;
                         pChannel = pSmv92Cb->NextChannel(pChannel);
                     }

                     uint32_t nChannelCountRecvConfig = pSmvRecvConfig->GetChannelCount();

                     //在实际报文通道大于scl文件中appid对应通道时，填充通道属性
                     for (; nIndex < nChannelCountRecvConfig; nIndex++)
                     {
                         stSmvRecvChannelInfo.nIndex = nIndex;
                         stSmvRecvChannelInfo.pOwnerCtrlBlk = NULL;
                         stSmvRecvChannelInfo.enChannelValueType = CHANNEL_VALUE_TYPE_UNKNOWN;
                         stSmvRecvChannelInfo.enChannelPhase = CHANNEL_PHASE_UNKOWN;
                         stSmvRecvChannelInfo.enChannelType = CHANNEL_TYPE_UNKOWN;
                         stSmvRecvChannelInfo.strChannelDesc = L"";

                         pSmvRecvConfig->SetOneChannelInfo(nIndex, &stSmvRecvChannelInfo);
                     }
                     return true;
                 }
             }
            return false;
         }

        uint32_t nTemplateCount = sizeof(g_SmvChannelTemplate)/sizeof(g_SmvChannelTemplate[0]);
        for (uint32_t i = 0; i < nTemplateCount; ++i)
        {
            if (enConfigType == g_SmvChannelTemplate[i].enTemplateType)
            {
                uint32_t nChannelsTemplate = g_SmvChannelTemplate[i].nTemplateSize;
                uint32_t nChannelsSmvConfig = pSmvRecvConfig->GetChannelCount();
                const PREDEFINED_CHANNEL_INFO* pChannelTemplate = g_SmvChannelTemplate[i].pChannelTempalte;
                for (uint32_t i = 0; i < nChannelsSmvConfig; ++i)
                {
                    CSmvRecvChannelInfo stSmvRecvChannelInfo;
                    stSmvRecvChannelInfo.nIndex = i;
                    stSmvRecvChannelInfo.pOwnerCtrlBlk = NULL;

                    if (i < nChannelsTemplate)
                    {
                        stSmvRecvChannelInfo.enChannelValueType = pChannelTemplate[i].enChannelValueType;
                        stSmvRecvChannelInfo.enChannelPhase = pChannelTemplate[i].enChannelPhase;
                        stSmvRecvChannelInfo.enChannelType  = pChannelTemplate[i].enChannelType;
                        stSmvRecvChannelInfo.strChannelDesc = pChannelTemplate[i].strChannelDesc;

                        if (stSmvRecvChannelInfo.enChannelType == CHANNEL_TYPE_SMV_CURRENT)
                        {
                            stSmvRecvChannelInfo.fPrimaryRatedValue = fPrimaryAmp;
                            stSmvRecvChannelInfo.fSecondaryRatedValue = fSecondaryAmp;
                        }
                        else if (stSmvRecvChannelInfo.enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
                        {
                            stSmvRecvChannelInfo.fPrimaryRatedValue = fPrimaryVol;
                            stSmvRecvChannelInfo.fSecondaryRatedValue = fSecondaryVol;
                        }
                        else if (stSmvRecvChannelInfo.enChannelType == CHANNEL_TYPE_SMV_TIME)
                        {
                            stSmvRecvChannelInfo.fPrimaryRatedValue = static_cast<float>(nRatedMuDelay);
                            stSmvRecvChannelInfo.fSecondaryRatedValue = static_cast<float>(nRatedMuDelay);
                        }
                    }
                    else
                    {
                        stSmvRecvChannelInfo.enChannelValueType = CHANNEL_VALUE_TYPE_UNKNOWN;
                        stSmvRecvChannelInfo.enChannelPhase = CHANNEL_PHASE_UNKOWN;
                        stSmvRecvChannelInfo.enChannelType  = CHANNEL_TYPE_UNKOWN;
                        stSmvRecvChannelInfo.strChannelDesc = L"";
                    }
                    pSmvRecvConfig->SetOneChannelInfo(i, &stSmvRecvChannelInfo);
                }
                return true;
            }
        }
    }
    return false;
}

///
/// @brief
///    实时录波时将数据帧内容填入内存缓冲区
///
/// @return
///     添加成功返回true
///
#if CHUN_HUA
bool CFrameRecv::_WritePckToBuf( PCAP_FILE_TIME_VAL stPcapFileTimeVal, uint8_t * pFrame, uint32_t nFrameLen )
{
#if DSM_RECORD_WARE_REALTIME
    AutoLock autolock(m_lockRecordWaveRealTimeBuf);
    if (NULL == m_pRecordWaveRealTimeBuf)
    {
        return false;
    }
    // 配置数据包头
    PCAP_PACKET_HEADER pckHeader;
    pckHeader.len = nFrameLen;
    pckHeader.timeStamp = stPcapFileTimeVal; 

    if (m_nRecordWaveIndex + nFrameLen + sizeof(PCAP_PACKET_HEADER) <= m_nRecordBufferSize)
    {
        pckHeader.capLen = nFrameLen;
    }
    else
    {
        if (m_nRecordBufferSize - m_nRecordWaveIndex <= sizeof(PCAP_PACKET_HEADER))
        {
            //如果剩余空间不足以存放数据帧，则通知界面
            _NotifyMessage(WM_DATA_LAYER_MESSAGE, MSG_RECORD_SIZE_REACHED);
            return false;
        }
        pckHeader.capLen = m_nRecordBufferSize - m_nRecordWaveIndex - sizeof(PCAP_PACKET_HEADER);
    }

    memcpy_s(m_pRecordWaveRealTimeBuf + m_nRecordWaveIndex, m_nRecordBufferSize  - m_nRecordWaveIndex, &pckHeader,sizeof(PCAP_PACKET_HEADER));
    m_nRecordWaveIndex += sizeof(PCAP_PACKET_HEADER);
    memcpy_s(m_pRecordWaveRealTimeBuf + m_nRecordWaveIndex, pckHeader.capLen, pFrame, pckHeader.capLen);
    m_nRecordWaveIndex += pckHeader.capLen;
    return true;
#endif
    return false;
}

#else

bool CFrameRecv::_WritePckToBuf( PCAP_FILE_TIME_VAL stPcapFileTimeVal, uint8_t * pFrame, uint32_t nFrameLen )
{
#if DSM_RECORD_WARE_REALTIME
	AutoLock autolock(m_lockRecordWaveRealTimeBuf);

	if (1 == m_nCurrentBufferIndex  && NULL == m_pRecordWaveRealTimeBuf)
	{
		return false;
	}
	else if (2 == m_nCurrentBufferIndex  &&  NULL == m_pRecordWaveRealTimeBuf2)
	{
		return false;
	}
	else if (0 == m_nCurrentBufferIndex)
	{
		return false;
	}
		
	

	if (m_nSDCardFreeSize - m_nRecordBufferSize< m_nTotalRecordSize)
	{
			//如果剩余空间不足，则通知界面
			_NotifyMessage(WM_DATA_LAYER_MESSAGE, MSG_RECORD_SIZE_REACHED);
			return false;
	}

	// 配置数据包头
	PCAP_PACKET_HEADER pckHeader;
	pckHeader.len = nFrameLen;
	pckHeader.timeStamp = stPcapFileTimeVal; 

	//满20M
	if (m_nRecordWaveIndex + nFrameLen + sizeof(PCAP_PACKET_HEADER) > m_nRecordBufferSize)
	{
		if(!m_bBuffer1Flag && 2==m_nCurrentBufferIndex) //当前正在写入的是buffer2，且buffer1空闲，则转为写入buffer1
		{
			m_nCurrentBufferIndex = 1;	
			m_bBuffer2Flag = true;
			m_nTotalRecordSize += m_nRecordWaveIndex;
		}
		else if(!m_bBuffer2Flag && 1==m_nCurrentBufferIndex) //当前正在写入的是buffer1，且buffer2空闲，则转为写入buffer2
		{
			m_nCurrentBufferIndex = 2;
			m_bBuffer1Flag = true;
			m_nTotalRecordSize += m_nRecordWaveIndex;
		}
		m_nCurrentRecordSize = m_nRecordWaveIndex;
		m_nRecordWaveIndex = 0;
	}
	pckHeader.capLen = nFrameLen;
	
	if ( 1 == m_nCurrentBufferIndex )
	{
		memcpy_s(m_pRecordWaveRealTimeBuf + m_nRecordWaveIndex, m_nRecordBufferSize  - m_nRecordWaveIndex, &pckHeader,sizeof(PCAP_PACKET_HEADER));
		m_nRecordWaveIndex += sizeof(PCAP_PACKET_HEADER);
		memcpy_s(m_pRecordWaveRealTimeBuf + m_nRecordWaveIndex, pckHeader.capLen, pFrame, pckHeader.capLen);
		m_nRecordWaveIndex += pckHeader.capLen;
		return true;
	}
	else if ( 2 == m_nCurrentBufferIndex )
	{
		memcpy_s(m_pRecordWaveRealTimeBuf2 + m_nRecordWaveIndex, m_nRecordBufferSize  - m_nRecordWaveIndex, &pckHeader,sizeof(PCAP_PACKET_HEADER));
		m_nRecordWaveIndex += sizeof(PCAP_PACKET_HEADER);
		memcpy_s(m_pRecordWaveRealTimeBuf2 + m_nRecordWaveIndex, pckHeader.capLen, pFrame, pckHeader.capLen);
		m_nRecordWaveIndex += pckHeader.capLen;
		return true;
	}
	
#endif
	return false;
}
#endif
///
/// @brief
///    获取当前的控制块实时通道数
///
/// @return
///     uint32_t   返回当前控制块实时通道数
///
uint32_t CFrameRecv::GetCurrentCtrlBlkRealTimeChannelCounts()
{
    AutoLock autoLock(m_lockRealTimeChannelCounts);
    return m_nRealTimeChannelCounts[0];
}

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
bool CFrameRecv::GetPhaseCheckRealTimeChannelCounts( uint32_t& nChannelCountSmv1, uint32_t& nChannelCountSmv2 )
{
    if (m_pCurrentDetectItems[0] && m_pCurrentDetectItems[1])
    {
        AutoLock autoLock(m_lockRealTimeChannelCounts);
        nChannelCountSmv1 = m_nRealTimeChannelCounts[0];
        nChannelCountSmv2 = m_nRealTimeChannelCounts[1];
        return true;
    }
    return false;
}

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
LRESULT CFrameRecv::_NotifyMessage( uint32_t nMessageId, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, bool bPostMessage /*= true*/ )
{
    if (m_pNotifyWnd && m_pNotifyWnd->GetSafeHwnd())
    {
        if (bPostMessage)
        {
            return ::PostMessage(m_pNotifyWnd->GetSafeHwnd(), nMessageId, wParam, lParam);
        }
        else
        {
            return ::SendMessage(m_pNotifyWnd->GetSafeHwnd(), nMessageId, wParam, lParam);
        }
    }
    return false;
}

///
/// @brief
///    清理实时报文数据
///
void CFrameRecv::_ClearRealTimeMsg()
{
    AutoLock autoLock(m_lockRealTimeMsg);
    m_cRealTimeMsg.m_enFrameType = FRAME_TYPE_UNKOWN;
    m_cRealTimeMsg.m_pCtrlBlk = NULL;
    m_cRealTimeMsg.m_nFrameLen  = 0;
}

///
/// @brief
///    解析数据帧，产生统计信息
///
void CFrameRecv::_GenSmvFrameStatistics( CFrameRecvNet * pNet, uint8_t* pFrame, uint32_t nFrameLen, FrameTimeStamp& stFrameTime, CNetDetectItem* pCurrentCtrlBlk )
{
    enumFrameType enFrameType = pNet->m_cMsgRapidDecoder.GetMessageType(pFrame, nFrameLen);
    CSmvFrameStatistics* pSmvFrameStatistic = NULL;
    AutoLock autoLock(m_lockSmvFrameStatistics);
    MapDetectItemSmvFrameStatistics::iterator itrMap = m_mapSmvFrameStatistics.find(pCurrentCtrlBlk);
    if (itrMap != m_mapSmvFrameStatistics.end())
    {
        pSmvFrameStatistic = &(itrMap->second);
    }
    else
    {
        CSmvFrameStatistics smvFrameStatistic;
        m_mapSmvFrameStatistics.insert(std::make_pair(pCurrentCtrlBlk, smvFrameStatistic));
        itrMap = m_mapSmvFrameStatistics.find(pCurrentCtrlBlk);
        if (itrMap != m_mapSmvFrameStatistics.end())
        {
            pSmvFrameStatistic = &(itrMap->second);
        }
    }

    if (enFrameType == FRAME_TYPE_NET_9_1)
    {
        uint16_t nSmpCnt = 0;
        uint16_t statusWord1 = 0;
        uint16_t statusWord2 = 0;

        Sav91Msg* pSav91Msg = pNet->m_pSav91Msg;
        if (0 == pSav91Msg->Decode(pFrame, nFrameLen))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_GenSmvFrameStatistics: pSav91Msg->Decode Failed \n ")));
            return;
        }
        size_t nSmvChannels = pSav91Msg->GetASDUDefaultSmpsNum();
        if (nSmvChannels != NET_9_1_CHANNELS_PER_FRAME)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("9-1 Channel Count(%d)\r\n"), nSmvChannels));
            return;
        }

        //总报文帧数, 时间抖动
        ASSERT(pSmvFrameStatistic != NULL);
        pSmvFrameStatistic->ParsingFrameTimeStamp(pCurrentCtrlBlk, stFrameTime);

        uint16_t noASDU = pSav91Msg->GetNoASDU();
        for ( uint16_t j = 0; j < noASDU; ++j)
        {
            pSav91Msg->GetSmpCntInASDU(j, nSmpCnt);
            pSav91Msg->GetStatusWordsInASDU(j, statusWord1, statusWord2);

            //处理采样计数器数值
            pSmvFrameStatistic->ParsingSmpIndex(nSmpCnt);

            //处理检修，失步等信息
            CSmvCtrlBlkQuality smvCtrlBlkQuality;
            smvCtrlBlkQuality.SetCtrlBlk(pCurrentCtrlBlk);
            smvCtrlBlkQuality.SetCtrlBlkStatusWords(statusWord1, statusWord2);
            pSmvFrameStatistic->ParsingStatusQuality(smvCtrlBlkQuality);
        }
    }
    else if (enFrameType == FRAME_TYPE_NET_9_2)
    {
        uint16_t nSmpCnt = 0;
        bool     bSmpSync = false;
        uint32_t p92SmvQlt[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];

        Sav92Msg* pSav92Msg = pNet->m_pSav92Msg;
        if (0 == pSav92Msg->Decode(pFrame, nFrameLen))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_GenSmvFrameStatistics(): pSav92Msg->Decode Failed \n ")));
            return;
        }

        //总报文帧数, 时间抖动
        ASSERT(pSmvFrameStatistic != NULL);
        pSmvFrameStatistic->ParsingFrameTimeStamp(pCurrentCtrlBlk, stFrameTime);

        uint16_t noASDU = pSav92Msg->GetNoASDU();
        for ( uint16_t j = 0; j < noASDU; ++j)
        {
            size_t nChannelCount = pSav92Msg->GetASDUSmpsNum(j);
            if (nChannelCount > NET_9_2_LE_MAX_CHANNELS_PER_ASDU)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (_T("9-2 Channel Count(%d)\r\n"), nChannelCount));
                return;
            }

            pSav92Msg->GetSmpCntInASDU(j, nSmpCnt);
            pSav92Msg->GetSmpSynchInASDU(j, bSmpSync);

            //处理采样计数器数值
            pSmvFrameStatistic->ParsingSmpIndex(nSmpCnt);

            //处理检修，失步等信息
            CSmvCtrlBlkQuality smvCtrlBlkQuality;
            smvCtrlBlkQuality.SetCtrlBlk(pCurrentCtrlBlk);
            smvCtrlBlkQuality.SetMuSync(bSmpSync);
            pSav92Msg->GetASDUSmps(j, NULL, p92SmvQlt, nChannelCount);
            for (uint16_t k = 0; k < nChannelCount; ++k)
            {
                smvCtrlBlkQuality.SetChannelQuality(k, p92SmvQlt[k]);
            }
            pSmvFrameStatistic->ParsingStatusQuality(smvCtrlBlkQuality);
        }
    }
}

///
/// @brief
///    解析Ft3采样值数据帧，产生统计信息
///
void CFrameRecv::_GenSmvFrameStatistics( CFrameRecvFt3 * pFt3, uint8_t* pFrame, uint32_t nFrameLen, FrameTimeStamp& stFrameTime, CFt3DetectItem* pCurrentCtrlBlk )
{
    enumFrameType enFrameType = pFt3->m_cMsgRapidDecoder.GetMessageType(pFrame, nFrameLen);
    CSmvFrameStatistics* pSmvFrameStatistic = NULL;
    AutoLock autoLock(m_lockSmvFrameStatistics);
    MapDetectItemSmvFrameStatistics::iterator itrMap = m_mapSmvFrameStatistics.find(pCurrentCtrlBlk);
    if (itrMap != m_mapSmvFrameStatistics.end())
    {
        pSmvFrameStatistic = &(itrMap->second);
    }
    else
    {
        CSmvFrameStatistics smvFrameStatistic;
        m_mapSmvFrameStatistics.insert(std::make_pair(pCurrentCtrlBlk, smvFrameStatistic));
        itrMap = m_mapSmvFrameStatistics.find(pCurrentCtrlBlk);
        if (itrMap != m_mapSmvFrameStatistics.end())
        {
            pSmvFrameStatistic = &(itrMap->second);
        }
    }

    //总报文帧数, 时间抖动
    ASSERT(pSmvFrameStatistic != NULL);
    pSmvFrameStatistic->ParsingFrameTimeStamp(pCurrentCtrlBlk, stFrameTime);

    if (enFrameType == FRAME_TYPE_FT3_GUOWANG)
    {
        GuoWangFT3Msg* pGuoWangFt3Msg = pFt3->m_pGuoWangFT3Msg;
        if (0 == pGuoWangFt3Msg->Decode(reinterpret_cast<uint16_t*>(pFrame), nFrameLen/2))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_GenSmvFrameStatistics(): pGuoWangFt3Msg->Decode Failed \n ")));
            return;
        }
        GUOWANG_FT3_ASDU_PUBLIC stAsduPbl;
        pGuoWangFt3Msg->GetASDUPublicInfo(stAsduPbl);

        pSmvFrameStatistic->ParsingSmpIndex(stAsduPbl.smpCnt);
        CSmvCtrlBlkQuality smvCtrlBlkQuality;
        smvCtrlBlkQuality.SetCtrlBlk(pCurrentCtrlBlk);
        smvCtrlBlkQuality.SetCtrlBlkStatusWords(stAsduPbl.statusWord1,stAsduPbl.statusWord2);
        pSmvFrameStatistic->ParsingStatusQuality(smvCtrlBlkQuality);
    }
    else if (enFrameType == FRAME_TYPE_FT3_NANRUI)
    {
        NanRuiFT3Msg* pNanRuiFt3Msg = pFt3->m_pNanRuiFt3Msg;
        if (0 == pNanRuiFt3Msg->Decode(reinterpret_cast<uint16_t*>(pFrame), nFrameLen/2))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_GenSmvFrameStatistics(): pNanRuiFt3Msg->Decode Failed \n ")));
            return;
        }
        NANRUI_FT3_ASDU_PUBLIC stAsduPbl;
        pNanRuiFt3Msg->GetASDUPublicInfo(stAsduPbl);

        pSmvFrameStatistic->ParsingSmpIndex(stAsduPbl.smpCnt);
        CSmvCtrlBlkQuality smvCtrlBlkQuality;
        smvCtrlBlkQuality.SetCtrlBlk(pCurrentCtrlBlk);
        smvCtrlBlkQuality.SetCtrlBlkStatusWords(stAsduPbl.statusWord1,stAsduPbl.statusWord2);
        pSmvFrameStatistic->ParsingStatusQuality(smvCtrlBlkQuality);
    }
}

void CFrameRecv::_GenGooseFrameStatistics( CFrameRecvNet * pNet, uint8_t* pFrame, uint32_t nFrameLen, FrameTimeStamp& stFrameTime , CBaseDetectItem* pCurrentCtrlBlk)
{
    CGooseRTStatistics* pGooseRtFrameStatistic = NULL;
    AutoLock autoLock(m_lockGooseFrameStatistics);
    MapDetectItemGooseFrameStatistics::iterator itrMap = m_mapGooseFrameStatistics.find(pCurrentCtrlBlk);
    if (itrMap != m_mapGooseFrameStatistics.end())
    {
        pGooseRtFrameStatistic = &(itrMap->second);
    }
    else
    {
        CGooseRTStatistics gooseRtStatistics;
        m_mapGooseFrameStatistics.insert(std::make_pair(pCurrentCtrlBlk, gooseRtStatistics));
        itrMap = m_mapGooseFrameStatistics.find(pCurrentCtrlBlk);
        if (itrMap != m_mapGooseFrameStatistics.end())
        {
            pGooseRtFrameStatistic = &(itrMap->second);
        }
    }

    GOOSE_PDU_PUBLIC    stGoosePduPublic;
    GooseMsg* pGooseMsg = pNet->m_pGooseMsg;
    if (0 == pGooseMsg->Decode(pFrame, nFrameLen))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_GenGooseFrameStatistics: pSav91Msg->Decode Failed \n ")));
        return;
    }
    pGooseMsg->GetPduPublicInfo(stGoosePduPublic);
    ASSERT(pGooseRtFrameStatistic != NULL);
    pGooseRtFrameStatistic->ParsingFrameTimeStamp(stGoosePduPublic.timeAllowedtoLive, stFrameTime);
    pGooseRtFrameStatistic->ParsingGooseUtcTime(stGoosePduPublic.eventTm.ts_q);
    pGooseRtFrameStatistic->ParsingStSqNum(stGoosePduPublic.stNum, stGoosePduPublic.sqNum);
}

///
/// @brief
///    清空采样值统计信息
///
void CFrameRecv::_ClearSmvFrameStatistics()
{
    AutoLock autoLock(m_lockSmvFrameStatistics);
    m_mapSmvFrameStatistics.clear();
}

///
/// @brief
///    清空goose统计信息
///
void CFrameRecv::_ClearGooseFrameStatistics()
{
    AutoLock autoLock(m_lockGooseFrameStatistics);
    m_mapGooseFrameStatistics.clear();
}

///
/// @brief
///    判断核相时两个时间戳是否可以被认为是同一时刻的采样值
///
bool CFrameRecv::_IsPhaseCheckTimeStampAlign(const FrameTimeStamp& stTimeStamp1, const FrameTimeStamp& stTimeStamp2, CBaseDetectItem* pDetectItem )
{
    //计算控制块对应每帧发送间隔时间
    uint32_t nTimeInterval = pDetectItem->nAsdu * 1000000 / pDetectItem->nSampleRate;
    int64_t ntm1 = stTimeStamp1.ts_sec;
    ntm1 = ntm1 * 1000000 + stTimeStamp1.ts_usec;
    int64_t ntm2 = stTimeStamp2.ts_sec;
    ntm2 = ntm2 * 1000000 + stTimeStamp2.ts_usec;
    int64_t nTimeDiff = _abs64(ntm2 - ntm1);
    if (nTimeDiff <= nTimeInterval)
    {
        return true;
    }
    return false;
}

///
/// @brief
///     清空核相缓存的数据
///
void CFrameRecv::_ClearPhaseCheckData()
{
    AutoLock autoLock(m_lockPhaseCheckData);
    if (NULL != m_pPhaseCheckAIndex)
    {
        delete[] m_pPhaseCheckAIndex;
        m_pPhaseCheckAIndex = NULL;
    }
    if (NULL != m_pPhaseCheckBIndex)
    {
        delete[] m_pPhaseCheckBIndex;
        m_pPhaseCheckBIndex = NULL;
    }

    if (NULL != m_pResultY)
    {
        delete[] m_pResultY;
        m_pResultY = NULL;
    }
    m_nPhaseCheckCurIndex = 0;
    m_nResultYLen = 0;
}

///
/// @brief
///    更新缓存的控制块配置信息一次/二次值
///
/// @param[in]  enChannelType 要修改的通道类型，电压通道/电流通道
/// @param[in]  nPriSectype   要修改的值类型，是一次值（1）还是二次值（2）
/// @param[in]  fVal          最新的数值
///
void CFrameRecv::UpdateCachedRatedValues( enumChannelType enChannelType, uint32_t nPriSecType, float fVal )
{
    CSmvRecvConfig* pRecvCofig = NULL;
    CSmvRecvChannelInfo* pChannelInfo = NULL;

    //缓存的9-1
    if (m_mapAppIDSmvRecv91Config.size() > 0)
    {
        std::map<uint16_t, CSmvRecvConfig*>::iterator itr = m_mapAppIDSmvRecv91Config.begin();
        while(itr != m_mapAppIDSmvRecv91Config.end())
        {
            pRecvCofig = itr->second;
            if (pRecvCofig)
            {
                uint32_t nChannelCount = pRecvCofig->GetChannelCount();
                for (uint32_t i = 0; i < nChannelCount; ++i)
                {
                    pChannelInfo = pRecvCofig->GetOneChannelInfo(i);
                    if (pChannelInfo)
                    {
                        if ((pChannelInfo->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE && enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
                            || (pChannelInfo->enChannelType == CHANNEL_TYPE_SMV_CURRENT && enChannelType == CHANNEL_TYPE_SMV_CURRENT) )
                        {
                            if (nPriSecType == 1)
                            {
                                pChannelInfo->fPrimaryRatedValue = fVal;
                            }
                            else if (nPriSecType == 2)
                            {
                                pChannelInfo->fSecondaryRatedValue = fVal;
                            }
                        }
                    }
                }
            }
            ++itr;
        }
    }

    //缓存的9-2
    if (m_mapAppIDSmvRecv92Config.size() > 0)
    {
        std::map<uint16_t, CSmvRecvConfig*>::iterator itr = m_mapAppIDSmvRecv92Config.begin();
        while(itr != m_mapAppIDSmvRecv92Config.end())
        {
            pRecvCofig = itr->second;
            if (pRecvCofig)
            {
                uint32_t nChannelCount = pRecvCofig->GetChannelCount();
                for (uint32_t i = 0; i < nChannelCount; ++i)
                {
                    pChannelInfo = pRecvCofig->GetOneChannelInfo(i);
                    if (pChannelInfo)
                    {
                        if ((pChannelInfo->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE && enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
                            || (pChannelInfo->enChannelType == CHANNEL_TYPE_SMV_CURRENT && enChannelType == CHANNEL_TYPE_SMV_CURRENT) )
                        {
                            if (nPriSecType == 1)
                            {
                                pChannelInfo->fPrimaryRatedValue = fVal;
                            }
                            else if (nPriSecType == 2)
                            {
                                pChannelInfo->fSecondaryRatedValue = fVal;
                            }
                        }
                    }
                }
            }
            ++itr;
        }
    }

    //缓存的Ft3标准帧（南瑞）
    if (NULL != m_pFt3SmvRecvCofig[0])
    {
        pRecvCofig = m_pFt3SmvRecvCofig[0];
        uint32_t nChannelCount = pRecvCofig->GetChannelCount();
        for (uint32_t i = 0; i < nChannelCount; ++i)
        {
            pChannelInfo = pRecvCofig->GetOneChannelInfo(i);
            if (pChannelInfo)
            {
                if ((pChannelInfo->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE && enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
                    || (pChannelInfo->enChannelType == CHANNEL_TYPE_SMV_CURRENT && enChannelType == CHANNEL_TYPE_SMV_CURRENT) )
                {
                    if (nPriSecType == 1)
                    {
                        pChannelInfo->fPrimaryRatedValue = fVal;
                    }
                    else if (nPriSecType == 2)
                    {
                        pChannelInfo->fSecondaryRatedValue = fVal;
                    }
                }
            }
        }
    }

    //缓存的Ft3扩展帧（国网）
    if (NULL != m_pFt3SmvRecvCofig[1])
    {
        pRecvCofig = m_pFt3SmvRecvCofig[1];
        uint32_t nChannelCount = pRecvCofig->GetChannelCount();
        for (uint32_t i = 0; i < nChannelCount; ++i)
        {
            pChannelInfo = pRecvCofig->GetOneChannelInfo(i);
            if (pChannelInfo)
            {
                if ((pChannelInfo->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE && enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
                    || (pChannelInfo->enChannelType == CHANNEL_TYPE_SMV_CURRENT && enChannelType == CHANNEL_TYPE_SMV_CURRENT) )
                {
                    if (nPriSecType == 1)
                    {
                        pChannelInfo->fPrimaryRatedValue = fVal;
                    }
                    else if (nPriSecType == 2)
                    {
                        pChannelInfo->fSecondaryRatedValue = fVal;
                    }
                }
            }
        }
    }
}

///
/// @brief
///    获取dsp版本信息
///
uint8_t CFrameRecv::GetDspVersion()
{
    if (m_pDsp)
    {
        DspInfo stDspinfo;
        m_cDspDataSet.GetDspInfo(stDspinfo);
        return stDspinfo.nVersion;
    }
    return 0;
}

///
/// @brief
///    一次缓冲区最后一帧分析完毕时条用，标记某些符合条件的appid，不需要进行后续分析
///
void CFrameRecv::_SmpCalcAfterBufferAnalysis(bool bAutoDetect)
{
    AutoLock autoLock(m_lock92SmpRateCalcInfo);
    if (m_map92SmpRateCalcInfo.size() > 0)
    {
        MapSmpRateCalcInfo::iterator itr = m_map92SmpRateCalcInfo.begin();
        while(itr != m_map92SmpRateCalcInfo.end())
        {
            if (!itr->second.bBufFin)
            {
                if (itr->second.nFrameCnt > 1 && itr->second.nAsdu != 0)
                {
                    itr->second.bBufFin = true;
                    uint32_t nSmpRate = itr->second.GetSmpRate();
                    if (itr->first)
                    {
                        itr->first->nSampleRateDetect = nSmpRate;
                        if (bAutoDetect)
                        {
                            itr->first->nSampleRate = nSmpRate;
                        }
                        RETAILMSG(1, (_T("Appid: 0x%4x, SmpRate: %d\r\n"), itr->first->appId, nSmpRate));
                    }
                }
                else
                {
                    itr->second.ResetData();
                }
            }
            ++itr;
        }
    }
}

///
/// @brief
///    重置9-2采样率计算缓存数据
///
void CFrameRecv::_Reset92SmpCalcData()
{
    AutoLock autoLock(m_lock92SmpRateCalcInfo);
    m_map92SmpRateCalcInfo.clear();
}

///
/// @brief
///    获取正常处理数据帧对应的控制块信息
///
CNetDetectItem* CFrameRecv::_GetNetFrameDetectItem( uint16_t nAppid, enumFrameType enFrameType, DsmPortName enPortName, const uint8_t macAddr[] )
{
    AutoLock autoLock(m_lockDetectItems);
    std::vector<CBaseDetectItem*>::iterator itr = m_vecDetectItems.begin();
    int nMacSame = 0;
    for (; itr != m_vecDetectItems.end(); ++itr)
    {
        CNetDetectItem* pNetDetectItemItr = dynamic_cast<CNetDetectItem*>(*itr);
        if (pNetDetectItemItr)
        {
            nMacSame = memcmp(macAddr, pNetDetectItemItr->macAddr, ETHERNET_MAC_LEN);
            if (enPortName       == pNetDetectItemItr->enPortName 
                && enFrameType   == pNetDetectItemItr->enFrameType
                && nAppid        == pNetDetectItemItr->appId
                && nMacSame == 0 )
            {
                return pNetDetectItemItr;
            }
        }
    }
    return NULL;
}

///
/// @brief
///    更新系统9-2采样率设置
///
void CFrameRecv::_UpdateSmvRecvPub92SmpRate()
{
    if (m_vecSelectedItems.size() > 0)
    {
        CSmvRecvConfigPublic* pSmvRecvPub = CDsmDataLayer::getInstance()->GetSmvConfigPublic();
        if (pSmvRecvPub)
        {
            CBaseDetectItem* pDetectItem = m_vecSelectedItems.back();
            if (pDetectItem->enFrameType == FRAME_TYPE_NET_9_2)
            {
                //当探测的采样率为0时，更新选择控制块的采样率到设置的采样率，默认4k
                if (pDetectItem->nSampleRate == 0)
                {
                    uint32_t nSmpRate = 4000;
                    if (pSmvRecvPub->GetSmv92SampleRate() == SMV92_SAMPLE_RATE_12P8K)
                    {
                        nSmpRate = 12800;
                    }

                    std::vector<CBaseDetectItem*>::iterator itr = m_vecSelectedItems.begin();
                    while(itr !=  m_vecSelectedItems.end())
                    {
                        (*itr)->nSampleRate = nSmpRate;
                        ++itr;
                    }
                }
            }
        }
    }
}

///
/// @brief
///    清空待发送给dsp的采样值信息
///
void CFrameRecv::_ClearDspSendSmvBuffer()
{
    AutoLock autoLock(m_lockDspSend);
    m_stDspSmvData.nSmvPerCircle = 0;
}

///
/// @brief
///    设置过滤单个控制块
///
bool CFrameRecv::_SetOneCtrlBlkFilter( CNetDetectItem* pNetDetectItem )
{
    if (pNetDetectItem == NULL)
    {
        return false;
    }
    CFrameRecvNet* pNetABC = NULL;
    switch (pNetDetectItem->enPortName)
    {
    case NET_PORT_LIGHT_A:                                               ///< 以太网光口A
        pNetABC = m_pFrameNetA;
        break;
    case NET_PORT_LIGHT_B:                                               ///< 以太网光口B
        pNetABC = m_pFrameNetB;
        break;
    case NET_PORT_ELECTRONIC:                                            ///< 电以太网口
        pNetABC = m_pFrameNetC;
        break;
    default:
        break;
    }
    if (pNetABC == NULL)
    {
        return false;
    }

    //停止所有数据接收
    StopRecvData();

    //组装过滤参数
    IOCTRL_IN_PARAM_NET_BIG stIoCtrlParamNet;
    FpgaNetFilterItem* pNetFilter = NULL;
    pNetFilter = reinterpret_cast<FpgaNetFilterItem*>(&stIoCtrlParamNet.nInBuf[4]);
    memcpy_s(pNetFilter->macAddr, ETHERNET_MAC_LEN, pNetDetectItem->macAddr, ETHERNET_MAC_LEN);
    uint8_t* pAppidAddr = (uint8_t *)&(pNetDetectItem->appId);
    pNetFilter->appId[0] = *(pAppidAddr + 1);
    pNetFilter->appId[1] = *pAppidAddr;

    //设置过滤数据类型
    stIoCtrlParamNet.nInBuf[0] = static_cast<uint32_t>(m_enCurrentDetectType);

    //设置过滤的mac数量
    stIoCtrlParamNet.nInBuf[3] = 1;

    pNetABC->SetNetFilter(stIoCtrlParamNet);
    pNetABC->SetRecvTimeOut(NET_DEFAULT_RECV_TIMEOUT);
    pNetABC->StartNetRecv();
    uint32_t nPortBufferCnt = (pNetDetectItem->nSampleRate * DSM_CONFIG_DEFAULT_RECV_WAVE_COUNT)/(DSM_CONFIG_DEFAULT_ELEC_FREQUENCY * pNetDetectItem->nAsdu);
    return pNetABC->StartFpgaRecvLoop(nPortBufferCnt , NET_DEFAULT_RECV_ENABLETIMEOUT); 
}

inline uint32_t CFrameRecv::_GetDesireSmpDistance(uint32_t nSmpCntCurrent, uint32_t nSmpCntStd, uint32_t nPointWave )
{
    uint32_t nDistance = 0;
    if (nPointWave != 0)
    {
        nSmpCntCurrent = nSmpCntCurrent % nPointWave;
        nSmpCntStd     = nSmpCntStd % nPointWave;

        if (nSmpCntCurrent > nSmpCntStd)
        {
            nDistance = nSmpCntStd + nPointWave - nSmpCntCurrent;
        }
        else
        {
            nDistance = nSmpCntStd - nSmpCntCurrent;
        }
    }
    return nDistance;
}

///
/// @brief
///    分析1588数据
///
void CFrameRecv::_Net1588Analy( CFrameRecvNet * pNet, DSM_OPERATE_MODE enOpMode, CNetDetectItem* pCurrentDetct )
{
    if (NULL == pNet)
    {
        return;
    }
    CNet1588DetectItem *pNet1588DetectItem = dynamic_cast<CNet1588DetectItem*>(pCurrentDetct);
    if(!pNet1588DetectItem)
    {
        return;
    }


    BufferHeader* pBufferHeader = reinterpret_cast<BufferHeader*>(pNet->m_pDataBuf);
    //给缓冲区加锁，在处理缓冲区时防止驱动覆盖缓冲区信息
    pNet->LockBuffer();
    uint32_t nFrameCount = pBufferHeader->NetHeader.nFramesCount;
    uint32_t BufferLen = pBufferHeader->NetHeader.nActiveLength;
    uint32_t nLoopItem = pBufferHeader->NetHeader.nLoopItem;
    uint32_t nBufferPos0 = 0;
    uint32_t nBufferPos1 = 0;
    uint8_t* pBufferData = NULL;
    if (pBufferHeader->NetHeader.nNetMode >= _9_1_9_2_DETECT && pBufferHeader->NetHeader.nNetMode <= _GOOSE_FILTER)
    {
        nBufferPos0 = NET_RECV_LOOP_BUF0_POS;
        nBufferPos1 = NET_RECV_LOOP_BUF1_POS;
    }
    else if (pBufferHeader->NetHeader.nNetMode == _IEEE1588_DETECT || pBufferHeader->NetHeader.nNetMode == _IEEE1588_FILTER)
    {
        nBufferPos0 = IEEE1588_RECV_BUF0_POS;
        nBufferPos1 = IEEE1588_RECV_BUF1_POS;
    }

    if (0 == nLoopItem)
    {
        pBufferData = pNet->m_pDataBuf + sizeof(BufferHeader) + nBufferPos0;
    }
    else if (1 == nLoopItem)
    {
        pBufferData = pNet->m_pDataBuf + sizeof(BufferHeader) + nBufferPos1;
    }

    CNetRecvBuffer recvBuffer(pBufferData, nFrameCount, BufferLen);

    enumFrameType enFrameType        = FRAME_TYPE_UNKOWN;
    uint32_t nFrameLen               = 0;
    uint8_t *pFrame                  = NULL;
    MsgRapidDecoder& msgRapidDecoder = pNet->m_cMsgRapidDecoder;
    uint8_t MacAddr[ETHER_MAC_LEN]   = {0};

    for ( uint32_t i=0; i < nFrameCount; ++i)
    {
        if (recvBuffer.GetBufferFrameAddr(i, &pFrame, nFrameLen))
        {
            enFrameType = msgRapidDecoder.GetMessageType(pFrame, nFrameLen);
            if (FRAME_TYPE_NET_1588 == enFrameType)
            {
                IEEE1588_SOURCE_PORT_ID stClockId;
                uint8_t nVersion = 0;
                msgRapidDecoder.GetSrcMac(pFrame, nFrameLen, MacAddr);
                msgRapidDecoder.GetIeee1588SrcPortID(pFrame, nFrameLen, stClockId);
                msgRapidDecoder.GetIeee1588Version(pFrame, nFrameLen, nVersion);

                //判断当前数据帧的mac/clockid与选择的是否一致
                if ((memcmp(pNet1588DetectItem->macAddr, MacAddr, ETHER_MAC_LEN) == 0)
                 && (pNet1588DetectItem->stClockID.clockIdentity == stClockId.clockIdentity)
                 && (pNet1588DetectItem->stClockID.sourcePortID  == stClockId.sourcePortID)
                 && (pNet1588DetectItem->n1588Version            == nVersion))
                {
                    IEEE1588_TIMESTAMP st1588TimeStamp;
                    if(msgRapidDecoder.GetIeee1588OriginTimeStamp(pFrame, nFrameLen, st1588TimeStamp))
                    {
                        AutoLock autoLock(m_lock1588TimeStamp);
                        m_st1588TimeStamp = st1588TimeStamp;
                    }
                }
            }
        }
        else
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CFrameRecv::_Net1588Analy: Get Frame Content Failed \n")));
            goto errorHandle;
        }
    }

errorHandle:
    //给缓冲区解锁
    pNet->UnLockBuffer();
}

///
/// @brief
///    清空1588时间信息
///
void CFrameRecv::_ClearNet1588Time()
{
    AutoLock autoLock(m_lock1588TimeStamp);
    m_st1588TimeStamp.t_s  = 0;
    m_st1588TimeStamp.t_ns = 0;
}

///
/// @brief
///    判断某个模式是否处于减点模式（12.8k重新采样到4k）
///
bool CFrameRecv::_IsModeReSampleTo4k(DSM_OPERATE_MODE enMode)
{
    DSM_OPERATE_MODE arrayModeReSmpTo4k[] = 
    {
        DSM_MODE_SMV_RECV_EFFECTIVE_VALUE,                                  ///< 有效值
        DSM_MODE_SMV_RECV_OSCILLOGRAM,                                      ///< 波形图
        DSM_MODE_SMV_RECV_PHASOR,                                           ///< 相量
        DSM_MODE_SMV_RECV_XU_LIANG,                                         ///< 序量
        DSM_MODE_SMV_RECV_POWER,                                            ///< 功率
        DSM_MODE_SMV_RECV_HARMONIC,                                         ///< 谐波
    };

    uint32_t nSize = sizeof(arrayModeReSmpTo4k)/sizeof(arrayModeReSmpTo4k[0]);
    for (uint32_t i = 0; i < nSize; ++i)
    {
        if (enMode == arrayModeReSmpTo4k[i])
        {
            return true;
        }
    }
    return false;
}


///
/// @brief
///    更新Ft3控制块信息
///
void CFrameRecv::_UpdateFt3DetectItem( CBaseDetectItem* pBaseDetectItem )
{
    CFt3DetectItem* pFt3DetectItem = dynamic_cast<CFt3DetectItem*>(pBaseDetectItem);
    if (NULL == pFt3DetectItem)
    {
        return;
    }
    AutoLock autoLock(m_lockDetectItems);
    std::vector<CBaseDetectItem*>::iterator itr = m_vecDetectItems.begin();
    while (itr != m_vecDetectItems.end())
    {
        CFt3DetectItem* pFt3DetectItemItr = dynamic_cast<CFt3DetectItem*>(*itr);
        if (NULL == pFt3DetectItemItr)
        {
            ++itr;
            continue;
        }

        if (pFt3DetectItemItr->enPortName == pFt3DetectItem->enPortName)
        {
            pFt3DetectItemItr->enFrameType = pFt3DetectItem->enFrameType;
            pFt3DetectItemItr->enBaudRate  = pFt3DetectItem->enBaudRate;
            pFt3DetectItemItr->nSampleRate = pFt3DetectItem->nSampleRate;
            pFt3DetectItemItr->nAsdu       = pFt3DetectItem->nAsdu;
            pFt3DetectItemItr->nChannelCount = pFt3DetectItem->nChannelCount;
            pFt3DetectItemItr->nFrameLen   = pFt3DetectItem->nFrameLen;
            pFt3DetectItemItr->nLDName     = pFt3DetectItem->nLDName;
        }

        ++itr;
    }
}

///
/// @brief
///    清空双AD缓存信息
///
void CFrameRecv::_ClearDoubleADData()
{
    AutoLock autLock(m_lockDoubleADData);
    memset(&m_stDoubleADData, 0, sizeof(m_stDoubleADData));
}

///
/// @brief
///    探测时切换探测端口的定时器处理函数
///
void CALLBACK CFrameRecv::_DetectTimerProc( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime )
{
    if (!m_pFrameRecv)
    {
        return;
    }
    RETAILMSG(DETECT_TIMER_LOG, (_T("_DetectTimerProc(%u) TimerCount(%d)\r\n"), m_pFrameRecv->m_nDetectTimerId, m_pFrameRecv->m_nDetectTimerCount));

    //探测后第一次进入处理函数
    //只有针对网络采样值的探测进行不同端口的轮询探测，其他数据探测一直开启探测
    if (0 == m_pFrameRecv->m_nDetectTimerCount)
    {
        switch(m_pFrameRecv->m_enCurrentDetectType)
        {
        case NET_RECV_TYPE_GOOSE:
        case NET_RECV_TYPE_IEEE_1588:
            //设置Net开始探测
            if (m_pFrameRecv->m_pFrameNetA)
            {
                m_pFrameRecv->m_pFrameNetA->Detect(m_pFrameRecv->m_enCurrentDetectType, NET_DETECT_RECV_FRAMES, NET_DEFAULT_RECV_ENABLETIMEOUT, NET_DEFAULT_RECV_TIMEOUT);
            }
            if (m_pFrameRecv->m_pFrameNetB)
            {
                m_pFrameRecv->m_pFrameNetB->Detect(m_pFrameRecv->m_enCurrentDetectType, NET_DETECT_RECV_FRAMES, NET_DEFAULT_RECV_ENABLETIMEOUT, NET_DEFAULT_RECV_TIMEOUT);
            }
            if (m_pFrameRecv->m_pFrameNetC)
            {
                m_pFrameRecv->m_pFrameNetC->Detect(m_pFrameRecv->m_enCurrentDetectType, NET_DETECT_RECV_FRAMES, NET_DEFAULT_RECV_ENABLETIMEOUT, NET_DEFAULT_RECV_TIMEOUT);
            }
            m_pFrameRecv->_CloseDetectTimer();
            break;
        case FT3_RECV_TYPE_SMV:
            //设置光口接受信息
            if (m_pFrameRecv->m_pFrameFt3A)
            {
                m_pFrameRecv->m_pFrameFt3A->StartFt3Recv();
            }
            if (m_pFrameRecv->m_pFrameFt3B)
            {
                m_pFrameRecv->m_pFrameFt3B->StartFt3Recv();
            }
            m_pFrameRecv->_CloseDetectTimer();
            break;
        case NET_RECV_TYPE_SMV:
            //针对采样值，第一次只开启第一个网络端口，关闭其余网络端口
            {
                m_pFrameRecv->StopPortFrameRecv(NET_PORT_LIGHT_A);
                m_pFrameRecv->StopPortFrameRecv(NET_PORT_ELECTRONIC);
                if (m_pFrameRecv->m_pFrameNetB)
                {
                    m_pFrameRecv->m_pFrameNetB->Detect(m_pFrameRecv->m_enCurrentDetectType, NET_DETECT_RECV_FRAMES, NET_DEFAULT_RECV_ENABLETIMEOUT, NET_DEFAULT_RECV_TIMEOUT);
                    RETAILMSG(DETECT_TIMER_LOG, (_T("B \r\n")));
                }
            }
            break;
        default:
            break;
        }

        m_pFrameRecv->m_nDetectTimerCount++;
        return;
    }

    if(NET_RECV_TYPE_SMV == m_pFrameRecv->m_enCurrentDetectType)
    {
        uint32_t nPort = m_pFrameRecv->m_nDetectTimerCount % 3;
        switch (nPort)
        {
        case 0:
            {
                m_pFrameRecv->StopPortFrameRecv(NET_PORT_LIGHT_A);
                m_pFrameRecv->StopPortFrameRecv(NET_PORT_ELECTRONIC);
                if (m_pFrameRecv->m_pFrameNetB)
                {
                    m_pFrameRecv->m_pFrameNetB->Detect(m_pFrameRecv->m_enCurrentDetectType, NET_DETECT_RECV_FRAMES, NET_DEFAULT_RECV_ENABLETIMEOUT, NET_DEFAULT_RECV_TIMEOUT);
                    RETAILMSG(DETECT_TIMER_LOG, (_T("B \r\n")));
                }
            }
            break;
        case 1:
            {
                m_pFrameRecv->StopPortFrameRecv(NET_PORT_LIGHT_B);
                m_pFrameRecv->StopPortFrameRecv(NET_PORT_ELECTRONIC);
                if (m_pFrameRecv->m_pFrameNetA)
                {
                    m_pFrameRecv->m_pFrameNetA->Detect(m_pFrameRecv->m_enCurrentDetectType, NET_DETECT_RECV_FRAMES, NET_DEFAULT_RECV_ENABLETIMEOUT, NET_DEFAULT_RECV_TIMEOUT);
                    RETAILMSG(DETECT_TIMER_LOG, (_T("A \r\n")));
                }
            }
            break;
        case 2:
            {
                m_pFrameRecv->StopPortFrameRecv(NET_PORT_LIGHT_A);
                m_pFrameRecv->StopPortFrameRecv(NET_PORT_LIGHT_B);
                if (m_pFrameRecv->m_pFrameNetC)
                {
                    m_pFrameRecv->m_pFrameNetC->Detect(m_pFrameRecv->m_enCurrentDetectType, NET_DETECT_RECV_FRAMES, NET_DEFAULT_RECV_ENABLETIMEOUT, NET_DEFAULT_RECV_TIMEOUT);
                    RETAILMSG(DETECT_TIMER_LOG, (_T("C \r\n")));
                }
            }
            break;
        default:
            break;
        }
        m_pFrameRecv->m_nDetectTimerCount++;
    }
}

///
/// @brief
///    关闭探测定时器
///
void CFrameRecv::_CloseDetectTimer()
{
    if (m_nDetectTimerId != 0)
    {
        if (KillTimer(NULL, m_nDetectTimerId))
        {
            RETAILMSG(DETECT_TIMER_LOG, (_T("DetectTimer(%u) Killed\r\n"), m_nDetectTimerId));
            m_nDetectTimerId = 0;
        }
        else
        {
            RETAILMSG(DETECT_TIMER_LOG, (_T("DetectTimer(%u) Kill Failed\r\n"), m_nDetectTimerId));
        }
    }
    m_nDetectTimerCount = 0;
}

#if !CHUN_HUA
void CFrameRecv::SetSDCardSize( uint64_t SDCardFreeSize )
{
	m_nSDCardFreeSize = SDCardFreeSize;
}

uint64_t CFrameRecv::GetSDCardSize()
{
	return m_nSDCardFreeSize;
}


void CFrameRecv::StartRecord()
{
	m_nCurrentBufferIndex = 1;
	m_bIsStopRecord = false;
	if (m_hRecordThread == NULL)
	{
		DWORD dwThreadId = 0;
		m_hRecordThread = CreateThread(NULL, 0, RecordThreadProc, this, NULL, &dwThreadId);
		if (m_hRecordThread == NULL)
		{
			RETAILMSG(OUTPUT_LOG_MSG, (_T("FrameRecv: Create Record Thread Failed\r\n")));
			
		}
		else 
		{
			//创建线程成功，设置优先级
			if(CeSetThreadPriority(m_hRecordThread, DATA_RECV_DETECT_THREAD_PRIO))
			{
#if GEN_LOG_MESSAGE
				RETAILMSG(OUTPUT_LOG_MSG, (_T("FrameRecv: InitFt3: Port %d ThreadId(0x%08x) Current Priority %d\r\n"), m_enPortName, dwThreadId, CeGetThreadPriority(m_hThread)));
#endif
			}

		}
	}
}


DWORD CFrameRecv::RecordThreadProc( PVOID pArg )
{
	CFrameRecv* pFrameRecv = reinterpret_cast<CFrameRecv*>(pArg);
	DSM_OPERATE_MODE enMode = pFrameRecv->GetCurrentMode();
	pFrameRecv->m_bIsFinishRecord = false;
	while (TRUE)
	{
		
		if (pFrameRecv->m_bBuffer1Flag && pFrameRecv->m_nCurrentBufferIndex != 1)
		{
			_Write2File(pFrameRecv,pFrameRecv->m_pRecordWaveRealTimeBuf,enMode);			
			pFrameRecv->m_bBuffer1Flag = false;

			if (TRUE ==pFrameRecv->m_bBuffer2Flag	)
			{
				pFrameRecv->m_nCurrentRecordSize = pFrameRecv->m_nRecordWaveIndex;
				continue;
			}
			if (pFrameRecv->m_bIsStopRecord)
			{
				break;
			}
		}
		else if (pFrameRecv->m_bBuffer2Flag && pFrameRecv->m_nCurrentBufferIndex != 2)
		{
			_Write2File(pFrameRecv,pFrameRecv->m_pRecordWaveRealTimeBuf,enMode);
			pFrameRecv->m_bBuffer2Flag = false;
			if (TRUE ==pFrameRecv->m_bBuffer1Flag	)
			{
				pFrameRecv->m_nCurrentRecordSize = pFrameRecv->m_nRecordWaveIndex;
				continue;
			}
			if (pFrameRecv->m_bIsStopRecord)
			{
				break;
			}
		}
	}
	//释放缓冲区
	delete[] pFrameRecv->m_pRecordWaveRealTimeBuf;
	pFrameRecv->m_pRecordWaveRealTimeBuf  = NULL;
	delete[] pFrameRecv->m_pRecordWaveRealTimeBuf2;
	pFrameRecv->m_pRecordWaveRealTimeBuf2  = NULL;

	pFrameRecv->m_bIsFinishRecord = true;

	pFrameRecv->m_nRecordWaveIndex = 0;
	
	pFrameRecv->m_hRecordThread = NULL;


	
	return TRUE;
}

void CFrameRecv::_Write2File( CFrameRecv* pFrameRecv,uint8_t* buffer, DSM_OPERATE_MODE enMode )
{
	AutoLock autoLock(pFrameRecv->m_lockRecordWaveRealTimeBuf);
	if (NULL != buffer)
	{
		PCAP_FILE_HEADER Default_FH =
		{
			PCAP_DEFAULT_MAGIC,
			PCAP_DEFAULT_MAJOR_VERSION,
			PCAP_DEFAULT_MINOR_VERSION,
			PCAP_DEFAULT_THIS_ZONE,
			PCAP_DEFAULT_SIGFIGS,
			PCAP_DEFAULT_SANPLEN,
			PCAP_LINK_TYPE_ETHERNET
		};
		if (enMode == DSM_MODE_FT3_RECV_RECORD_WAVE)
		{
			Default_FH.linkType = PCAP_LINK_TYPE_FT3;
		}

		SYSTEMTIME st;
		GetLocalTime(&st);

		CString FilePath;
		FilePath.Format(L"%s%04d%02d%02d_%02d%02d%02d_%03d.pcap", pFrameRecv->m_strRecordWavePath.c_str(),st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

		//保存缓冲区数据
		CFile file;
		if (file.Open(FilePath, CFile::modeCreate|CFile::modeWrite))
		{
			try
			{
				file.Write(&Default_FH, sizeof(Default_FH));
				file.Write(buffer, pFrameRecv->m_nCurrentRecordSize);
				file.Close();
				pFrameRecv->m_nCurrentRecordSize = 0;
			}
			catch (CFileException* e)
			{
				RETAILMSG(1, (_T("CFileException rason : %d\r\n"), e->m_cause));
				e->Delete();

			}
			catch (CException* e)
			{

				RETAILMSG(1, (_T("UnkownException \r\n")));
				e->Delete();

			}
		}
		else
		{
			RETAILMSG(1, (_T("Open file error\r\n")));

		}

		//释放缓冲区
		//delete[] m_pRecordWaveRealTimeBuf;
		//pFrameRecv->m_pRecordWaveRealTimeBuf  = NULL;
		//pFrameRecv->m_nRecordWaveIndex = 0;
	}
}



#endif