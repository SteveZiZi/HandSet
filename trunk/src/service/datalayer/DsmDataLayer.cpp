/// @file
///
/// @brief
///     数据层对外提供服务类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.2
///
/// 修改历史：
///

#include "StdAfx.h"
#include "DsmDataLayer.h"
#include "include\ctrlcode_def.h"
#include "src\service\datalayer\utility\data_utility.h"
#include "src\dspclient\include\Dsp.h"

//定义编译警告todo信息
//#pragma message(__FILE__ "(" PRAGMA_STRING(__LINE__) "): 考虑Uninit时是否要先停止网络接口数据接收")
#define PRAGMA_STRING2(x) #x
#define PRAGMA_STRING(x) PRAGMA_STRING2(x)

CDsmDataLayer::CDsmDataLayer(void) : m_pFrameSend(NULL)
                                   , m_pFrameRecv(NULL)
                                   , m_pSclManager(NULL)
                                   , m_hGPMDevice(INVALID_HANDLE_VALUE)
                                   , m_pParamConfigBasic(NULL)
                                   , m_pParamConfigSMVSend(NULL)
                                   , m_pParamConfigGOOSESend(NULL)
                                   , m_pParamConfigGOOSERecv(NULL)
                                   , m_pOpticalPowerManager(NULL)
                                   , m_pPcapFileAnalysis(NULL)
{
}

CDsmDataLayer::~CDsmDataLayer(void)
{
}

///
/// @brief
///    获取数据层应用实例
///
CDsmDataLayer* CDsmDataLayer::getInstance()
{
    static CDsmDataLayer s_DsmDataLayer;
    return &s_DsmDataLayer;
}

///
/// @brief
///    启动或者停止FPGA
///
/// @param[in] bStart 启动停止FPGA，true启动FPGA，false停止FPGA
///
/// @return
///     true表示启动或者停止成功
///
bool CDsmDataLayer::_StartFPGA( bool bStart )
{
    BOOL bStartFPGA = bStart;
    if (FALSE == DeviceIoControl(m_hGPMDevice, IOCTRL_START_FPGA, &bStartFPGA, sizeof(bStartFPGA), NULL,NULL,NULL,NULL) )
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::_StartFPGA : DeviceIoControl:IOCTRL_START_FPGA failed \n"));
        return false;
    }
    return true;
}

///
/// @brief
///    启动FPGA
///
/// @return
///     true表示启动成功
///
bool CDsmDataLayer::StartFPGA()
{
    return _StartFPGA(true);
}

///
/// @brief
///    关闭FPGA
///
/// @return
///     true表示关闭成功
///
bool CDsmDataLayer::StopFPGA()
{
    return _StartFPGA(false);
}

///
/// @brief
///    初始化数据层需要的数据
///
/// @return
///     true表示关闭成功
///
bool CDsmDataLayer::Init()
{
    std::wstring        strSrcPathName;
    std::wstring        strDstPathName;

    // GPM驱动
    if (INVALID_HANDLE_VALUE == m_hGPMDevice)
    {
        //打开GPM驱动
        m_hGPMDevice = CreateFile(L"GPM1:", GENERIC_READ|GENERIC_WRITE, 0, 0,OPEN_ALWAYS, 0, NULL);
        if (INVALID_HANDLE_VALUE == m_hGPMDevice)
        {
            RETAILMSG(1, (_T(" CDsmDataLayer::Init(): Open GPM Device failed\r\n")));
            return false;
        }
    }
    RETAILMSG(1, (_T("Current Gpm Driver Version : %s\r\n"), GetGPMDriverVersion().c_str()));

    //启动FPGA发送接收功能
    StartFPGA();

    m_pParamConfigBasic = CParamConfigBasic::getInstance();
    if (NULL != m_pParamConfigBasic)
    {
         m_pParamConfigBasic->LoadConfig(PARAM_CONFIG_BASIC);
    }
    else
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T(" CDsmDataLayer::Init(): CParamConfigBasic::getInstance() Failed \n")));
    }

    // 初始化全站配置文件
    m_pSclManager = CSCLManager::getInstance();
    if (NULL != m_pSclManager)
    {
#if CHUN_HUA
        m_pSclManager->Load(GetAppDir() + m_pParamConfigBasic->GetSclFile());
#else
		m_pSclManager->Load(m_pParamConfigBasic->GetSclPath()+m_pParamConfigBasic->GetSclFile());
#endif
    }
    else
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T(" CDsmDataLayer::Init(): CSCLManager::getInstance() Failed \n")));
    }

    // SMV发送配置管理器
    m_pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL != m_pParamConfigSMVSend)
    {
        // 如果目标配置文件不存在，那么拷贝源配置文件到目标路径
        strSrcPathName  = GetAppDir() + DEFAULT_CFG_BACKUP + PARAM_CONFIG_SMV_SEND;
        strDstPathName  = GetAppRunTimeCfgDir() + PARAM_CONFIG_SMV_SEND;
        CopyFile(strSrcPathName.c_str(), strDstPathName.c_str(), TRUE);

        // 加载目标配置文件
        if (!m_pParamConfigSMVSend->LoadConfig(strDstPathName))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T(" CDsmDataLayer::Init(): CParamConfigSMVSend::LoadConfig() Failed \n")));
        }
    }

    // GOOSE发送配置管理器
    m_pParamConfigGOOSESend = CParamConfigGOOSESend::getInstance();
    if (NULL != m_pParamConfigGOOSESend)
    {
        // 如果目标配置文件不存在，那么拷贝源配置文件到目标路径
        strSrcPathName  = GetAppDir() + DEFAULT_CFG_BACKUP + PARAM_CONFIG_GOOSE_SEND;
        strDstPathName  = GetAppRunTimeCfgDir() + PARAM_CONFIG_GOOSE_SEND;
        CopyFile(strSrcPathName.c_str(), strDstPathName.c_str(), TRUE);

        // 加载目标配置文件
        if (!m_pParamConfigGOOSESend->LoadConfig(strDstPathName))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T(" CDsmDataLayer::Init(): CParamConfigGOOSESend::LoadConfig() Failed \n")));
        }
    }

    // GOOSE接收配置管理器
    m_pParamConfigGOOSERecv = CParamConfigGOOSERecv::getInstance();
    if (NULL != m_pParamConfigGOOSERecv)
    {
        // 如果目标配置文件不存在，那么拷贝源配置文件到目标路径
        strSrcPathName  = GetAppDir() + DEFAULT_CFG_BACKUP + PARAM_CONFIG_GOOSE_RECV;
        strDstPathName  = GetAppRunTimeCfgDir() + PARAM_CONFIG_GOOSE_RECV;
        CopyFile(strSrcPathName.c_str(), strDstPathName.c_str(), TRUE);

        // 加载目标配置文件
        if (!m_pParamConfigGOOSERecv->LoadConfig(strDstPathName))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T(" CDsmDataLayer::Init(): CParamConfigGOOSERecv::LoadConfig() Failed \n")));
        }
    }

    // 手动试验配置管理器
    m_pParamConfigManualTest = CParamconfigManualTest::getInstance();
    if (NULL != m_pParamConfigManualTest)
    {
        // 如果目标配置文件不存在，那么拷贝源配置文件到目标路径
        strSrcPathName  = GetAppDir() + DEFAULT_CFG_BACKUP + PARAM_CONFIG_MANUAL_TEST;
        strDstPathName  = GetAppRunTimeCfgDir() + PARAM_CONFIG_MANUAL_TEST;
        CopyFile(strSrcPathName.c_str(), strDstPathName.c_str(), TRUE);

        // 加载目标配置文件
        if (!m_pParamConfigManualTest->LoadConfig(strDstPathName))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T(" CDsmDataLayer::Init(): CParamconfigManualTest::LoadConfig() Failed \n")));
        }
    }

    // 状态序列配置管理器
    m_pParamConfigStateSequence = CParamConfigStateSequence::getInstance();
    if (NULL != m_pParamConfigStateSequence)
    {
        // 如果目标配置文件不存在，那么拷贝源配置文件到目标路径
        strSrcPathName  = GetAppDir() + DEFAULT_CFG_BACKUP + PARAM_CONFIG_STATE_SEQUENCE;
        strDstPathName  = GetAppRunTimeCfgDir() + PARAM_CONFIG_STATE_SEQUENCE;
        CopyFile(strSrcPathName.c_str(), strDstPathName.c_str(), TRUE);

        // 加载目标配置文件
        if (!m_pParamConfigStateSequence->LoadConfig(strDstPathName))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T(" CDsmDataLayer::Init(): CParamConfigStateSequence::LoadConfig() Failed \n")));
        }
    }

    //初始化DSP
    if ( !DspInit())
    {
        RETAILMSG(1, (_T("Dsp Init Failed \r\n")));
        return false;
    }

    if (NULL == m_pFrameRecv)
    {
        m_pFrameRecv = new CFrameRecv(m_hGPMDevice);
        if (NULL == m_pFrameRecv)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CDsmDataLayer::Init(): CDsmDataLayer::Init() new CFrameRecv Failed \n")));
            return false;
        }
        if (! m_pFrameRecv->InitFrameRecv())
        {
            return false;
        }
    }

    // 数据发送管理器
    if (NULL == m_pFrameSend)
    {
        m_pFrameSend = CFrameSend::getInstance();                       ///< 获取数据发送管理器
        if (NULL == m_pFrameSend)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CDsmDataLayer::Init(): CFrameSend get instance failed \n")));
            return false;
        }

        if (!m_pFrameSend->InitFrameSend(m_hGPMDevice))                 ///< 初始化数据发送管理器
        {
            return false;
        }
    }

    // 光功率
    if (NULL == m_pOpticalPowerManager)
    {
        m_pOpticalPowerManager = COpticalPowerManager::GetInstance();   ///< 获得光功率管理器
        if (NULL == m_pOpticalPowerManager)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CDsmDataLayer::Init(): COpticalPowerManager get instance failed \n")));
            return false;
        }

        if (!m_pOpticalPowerManager->Init(m_hGPMDevice))                ///< 初始化
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CDsmDataLayer::Init(): COpticalPowerManager init failed \n")));
            return false;
        }
    }

    //pcap文件统计分析
    if (NULL == m_pPcapFileAnalysis)
    {
        m_pPcapFileAnalysis = CPcapAnalysis::getInstance();
        if (NULL == m_pPcapFileAnalysis)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CDsmDataLayer::Init(): CPcapAnalysis::getInstance() failed")));
        }
    }

    return true;
}

///
/// @brief
///    反初始化，清理数据层的数据
///
/// @return
///     true表示清理成功
///
bool CDsmDataLayer::UnInit()
{
    if (NULL != m_pFrameRecv)
    {
        m_pFrameRecv->UnInitFrameRecv();
        delete m_pFrameRecv;
        m_pFrameRecv = NULL;
    }

    if (NULL != m_pFrameSend)
    {
        m_pFrameSend->UnInitFrameSend();
        m_pFrameSend = NULL;
    }

    //卸载DSP
    DspRelease();

    //关闭FPGA的发送接收功能
    StopFPGA();

    //关闭FPGA文件句柄
    if (INVALID_HANDLE_VALUE != m_hGPMDevice)
    {
        CloseHandle(m_hGPMDevice);
        m_hGPMDevice = INVALID_HANDLE_VALUE;
    }

    return true;
}

///
/// @brief
///    进入退出低功耗模式
///
/// @param[in] bLowPowerMode 进入退出低功耗模式，true进入低功耗，false退出低功耗模式
///
/// @return
///     true表示进入或者退出成功
///
bool CDsmDataLayer::_EnterLowPowerMode( bool bLowPowerMode )
{
    BOOL bEnterLowPowerMode = bLowPowerMode;
    if (FALSE == DeviceIoControl(m_hGPMDevice, IOCTRL_LOWER_POWER, &bEnterLowPowerMode, sizeof(bEnterLowPowerMode), NULL,NULL,NULL,NULL) )
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::_EnterLowPowerMode : DeviceIoControl:IOCTRL_LOWER_POWER failed \n"));
        return false;
    }
    return true;
}

///
/// @brief
///    进入低功耗模式
///
/// @return
///     true表示进入低功耗成功
///
bool CDsmDataLayer::EnterLowPowerMode()
{
     return _EnterLowPowerMode(true);
}

///
/// @brief
///    退出低功耗模式
///
/// @return
///     true表示退出低功耗成功
///
bool CDsmDataLayer::LeaveLowPowerMode()
{
    return _EnterLowPowerMode(false);
}

///
/// @brief
///    告诉数据层，要获取探测结果
///    数据层要去掉底层的过滤，获取网络接口的所有报文
///
void CDsmDataLayer::BeginDetect( NetFt3RecvFrameType enNetRecvFrameType, bool bEnabelDetect /*= true*/ )
{
    if (NULL != m_pFrameRecv)
    {
        m_pFrameRecv->BeginDetect(enNetRecvFrameType, bEnabelDetect);
    }
}

///
/// @brief
///    获取探测结果数目
///
/// @return
///     返回探测结果的数目
///
uint32_t CDsmDataLayer::GetNetDetectItemsCount()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetNetDetectItemsCount();
    }
    return 0;
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
CNetDetectItem* CDsmDataLayer::GetNetDetectItem( uint32_t nIndex )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetNetDetectItem(nIndex);
    }
    return NULL;
}

///
/// @brief
///    获取探测结果数目
///
/// @return
///     返回探测结果的数目
///
uint32_t CDsmDataLayer::GetFt3DetectItemsCount()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetFt3DetectItemsCount();
    }
    return 0;
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
CFt3DetectItem* CDsmDataLayer::GetFt3DetectItem( uint32_t nIndex )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetFt3DetectItem(nIndex);
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
uint32_t CDsmDataLayer::GetNet1588DetectItemsCount()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetNet1588DetectItemsCount();
    }
    return 0;
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
CNet1588DetectItem* CDsmDataLayer::GetNet1588DetectItem( uint32_t nIndex )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetNet1588DetectItem(nIndex);
    }
    return NULL;
}

///
/// @brief
///    SCL文件管理器获取接口
///
/// @return
///     CSCLManager* 返回Scl管理器的指针
///
CSCLManager* CDsmDataLayer::GetSclManager()
{
    return m_pSclManager;
}

///
/// @brief
///    获取已经选中的控制块数量
///
/// size_t      返回已经选中的控制块数量
///
size_t CDsmDataLayer::GetSelectedCtrlBlkCount()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetSelectedCtrlBlkCount();
    }
    return 0;
}


///
/// @brief
///    设置当前的控制块，dsp计算此控制块的信息
///
/// @param[in] pDetectItem  要设置的当前控制块
///
void CDsmDataLayer::SetCurrentCtrlBlk( CBaseDetectItem* pDetectItem  )
{
    if (NULL != m_pFrameRecv)
    {
        m_pFrameRecv->SetCurrentCtrlBlk(pDetectItem);
    }
}

///
/// @brief
///    获取当前的控制块指针
///
/// @return
///     CBaseDetectItem*   返回当前控制块指针，失败返回Null
///
CBaseDetectItem* CDsmDataLayer::GetCurrentCtrlBlk()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetCurrentCtrlBlk();
    }
    return NULL;
}

///
/// @brief
///    获取当前的控制块实时通道数
///
/// @return
///     uint32_t   返回当前控制块实时通道数
///
uint32_t CDsmDataLayer::GetCurrentCtrlBlkRealTimeChannelCounts()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetCurrentCtrlBlkRealTimeChannelCounts();
    }
    return 0;
}


///
/// @brief
///    设置当前模式
///
/// @param[in]  enDsmMode 要设置的模式
///
void CDsmDataLayer::SetCurrentMode( DSM_OPERATE_MODE enDsmMode )
{
    if(NULL != m_pFrameRecv)
    {
        m_pFrameRecv->SetCurrentMode(enDsmMode);
    }
}

///
/// @brief
///    获取当前模式
///
/// @return
///     DSM_OPERATE_MODE    返回当前系统模式
///
DSM_OPERATE_MODE CDsmDataLayer::GetCurrentMode()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetCurrentMode();
    }
    return DSM_MODE_UNKOWN;
}


///
/// @brief
///    获取当前的探测类型
///
/// @return
///     NetFt3RecvFrameType 返回当前的探测数据类型
///
NetFt3RecvFrameType CDsmDataLayer::GetCurrentRecvType()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetCurrentRecvType();
    }
    return RECV_TYPE_UNKOWN;
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
bool CDsmDataLayer::AddOneSelectedCtrlBlk( CBaseDetectItem* pDetectItem )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->AddOneSelectedCtrlBlk(pDetectItem);
    }
    return false;
}

///
/// @brief
///    移除一个已经添加的控制块
///
/// @param[in] pDetectItem  要移除的控制块
///
void CDsmDataLayer::RemoveOneSelectedCtrlBlk( CBaseDetectItem* pDetectItem )
{
    if (NULL != m_pFrameRecv)
    {
        m_pFrameRecv->RemoveOneSelectedCtrlBlk(pDetectItem);
    }
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
CBaseDetectItem* CDsmDataLayer::GetPrevSelectCtrlBlk( CBaseDetectItem* pCurrentCtrlBlk )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetPrevSelectCtrlBlk(pCurrentCtrlBlk);
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
CBaseDetectItem* CDsmDataLayer::GetNextSelectCtrlBlk( CBaseDetectItem* pCurrentCtrlBlk )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetNextSelectCtrlBlk(pCurrentCtrlBlk);
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
CBaseDetectItem* CDsmDataLayer::GetSelectCtrlBlkByID( uint32_t nID )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetSelectCtrlBlkByID(nID);
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
CBaseDetectItem* CDsmDataLayer::GetFirstSelectCtrlBlk()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetFirstSelectCtrlBlk();
    }
    return NULL;
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
bool CDsmDataLayer::GetEffectiveVal( CDataEffectiveVal* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetEffectiveVal(pData);
    }
    return false;
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
bool CDsmDataLayer::GetOscillogramData( CDataOscillogram* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetOscillogramData(pData);
    }
    return false;
}

///
/// @brief
///    获取Smv接收公共配置接口
///
CSmvRecvConfigPublic* CDsmDataLayer::GetSmvConfigPublic()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetSmvConfigPublic();
    }
    return NULL;
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
bool CDsmDataLayer::GetPowerData( CDataPower* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetPowerData(pData);
    }
    return false;
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
bool CDsmDataLayer::GetDoubleAdData( CDataDoubleAD* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetDoubleAdData(pData);
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
bool CDsmDataLayer::GetStatusWatchData( CDataStatusWatch* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetStatusWatchData(pData);
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
bool CDsmDataLayer::GetXuliangData( CDataXuliang* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetXuliangData(pData);
    }
    return false;
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
bool CDsmDataLayer::GetPhasorData( CDataPhasor* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetPhasorData(pData);
    }
    return false;
}

///
/// @brief
///    获取当前控制块对应的通道状态信息
///
/// @return
///     CSmvCtrlBlkQuality* 返回当前控制块通道信息的一个对象指针
///
CSmvCtrlBlkQuality* CDsmDataLayer::GetCurrentCtrlBlkChannelStatus()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetCurrentCtrlBlkChannelStatus();
    }
    return NULL;
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
bool CDsmDataLayer::GetHarmonicData( CDataHarmonic* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetHarmonicData(pData);
    }
    return false;
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
bool CDsmDataLayer::GetFrameStatistics( CDataFrameStatistics* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetFrameStatistics(pData);
    }
    return false;
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
bool CDsmDataLayer::GetPolarityData( CDataPolarity* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetPolarityData(pData);
    }
    return false;
}

///
/// @brief
///    重置所有的通道极性
///
void CDsmDataLayer::ResetAllPorlarity()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->ResetAllPorlarity();
    }
}

///
/// @brief
///    获取参数配置/基本参数配置接口
///
CParamConfigBasic* CDsmDataLayer::GetParamConfigBasic()
{
    return m_pParamConfigBasic;
}

///
/// @brief
///    清空统计数据，重新统计
///
/// @return
///     bool    重置成功返回true
///
bool CDsmDataLayer::ResetSmvFrameStatistics()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->ResetSmvFrameStatistics();
    }
    return false;
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
bool CDsmDataLayer::GetRealTimeMsg( CDataRealTimeMsgData* pData )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetRealTimeMsg(pData);
    }
    return false;
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
bool CDsmDataLayer::GetPhaseCheckData( CDataPhaseCheck* pData )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetPhaseCheckData(pData);
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
CDetectItemFrame* CDsmDataLayer::GetDetectItemFrame( CBaseDetectItem* pDetectItem )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetDetectItemFrame(pDetectItem);
    }
    return NULL;
}

///
/// @brief
///    设置核相使用的第1个控制块
///
/// @param[in] pDetectItem  要设置的控制块
///
void CDsmDataLayer::SetPhaseCheckSmv1CtrBlk( CBaseDetectItem* pDetectItem )
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->SetPhaseCheckSmv1CtrBlk(pDetectItem);
    }
}

///
/// @brief
///    设置核相使用的第二个控制块
///
/// @param[in] pDetectItem  要设置的当前控制块
///
void CDsmDataLayer::SetPhaseCheckSmv2CtrBlk( CBaseDetectItem* pDetectItem )
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->SetPhaseCheckSmv2CtrBlk(pDetectItem);
    }
}

///
/// @brief
///    获取核相使用的第1个控制块
///
/// @return
///     CBaseDetectItem*   返回当前控制块指针，失败返回Null
///
CBaseDetectItem* CDsmDataLayer::GetPhaseCheckSmv1CtrlBlk()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetPhaseCheckSmv1CtrlBlk();
    }
    return NULL;
}

///
/// @brief
///    获取核相使用的第二个控制块
///
/// @return
///     CBaseDetectItem*   返回当前控制块指针，失败返回Null
///
CBaseDetectItem* CDsmDataLayer::GetPhaseCheckSmv2CtrlBlk()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetPhaseCheckSmv2CtrlBlk();
    }
    return NULL;
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
bool CDsmDataLayer::GetGooseRealTimeValue( CGooseInfo* pData )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetGooseRealTimeValue(pData);
    }
    return false;
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
bool CDsmDataLayer::GetGooseRealTimeMsg( CDataRealTimeMsgData* pData )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetGooseRealTimeMsg(pData);
    }
    return false;
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
bool CDsmDataLayer::GetGooseFrameStatistics( CGooseFrameStatistics* pData )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetGooseFrameStatistics(pData);
    }
    return false;
}

///
/// @brief
///    获取驱动版本，返回版本字符串 如 2.34
///
std::wstring CDsmDataLayer::GetGPMDriverVersion()
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice)
    {
        uint16_t uBootVer[2];
        wchar_t  wsVer[10] = {0};
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_GET_DRV_VERSION, NULL, NULL, &uBootVer,sizeof(uBootVer),NULL,NULL))
        {
            _snwprintf_s(wsVer, 10, 10, L"%x.%02x",(uBootVer[0]>>8)&0xF, (uBootVer[0]&0xFF));
            return wsVer;
        }
    }
    return L"0.00";
}

///
/// @brief
///    获取FPGA版本，返回版本字符串 如 2.34
///
std::wstring CDsmDataLayer::GetGPMFPGAVersion()
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice)
    {
        uint16_t uBootVer[2];
        wchar_t  wsVer[10] = {0};
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_GET_DRV_VERSION, NULL, NULL, &uBootVer,sizeof(uBootVer),NULL,NULL))
        {
            _snwprintf_s(wsVer, 10, 10, L"%x.%02x",(uBootVer[1]>>8)&0xF, (uBootVer[1]&0xFF));
            return wsVer;
        }
    }
    return L"0.00";
}


///
/// @brief
///    获取dsp版本信息
///
uint8_t CDsmDataLayer::GetDspVersion()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetDspVersion();
    }
    return 0;
}

///
/// @brief
///    清空Goose报文统计数据，重新统计
///
/// @return
///     bool    重置成功返回true
///
bool CDsmDataLayer::ResetGooseFrameStatistics()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->ResetGooseFrameStatistics();
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
bool CDsmDataLayer::GetGooseChangeList( CDataGooseChangeList* pData )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetGooseChangeList(pData);
    }
    return false;
}

///
/// @brief
///    设置录波文件大小,需要在开始录波前设置
///
/// @return
///     bool    设置成功返回true
///
bool CDsmDataLayer::SetRecordWaveSize( uint64_t nSizeInByte )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->SetRecordWaveSize(nSizeInByte);
    }
    return false;
}

///
/// @brief
///    获取设置的录波文件大小
///
uint64_t CDsmDataLayer::GetRecordWaveSize()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetRecordWaveSize();
    }
    return 0;
}
#if !CHUN_HUA
///
/// @brief
///    设置SD卡大小
///
void CDsmDataLayer::SetSDCardSize(uint64_t SDCardFreeSize)
{
	if (m_pFrameRecv)
	{
		m_pFrameRecv->SetSDCardSize(SDCardFreeSize);
	}	
}

#endif

///
/// @brief
///    设置录波文件路径,需要在开始录波前设置
///
void CDsmDataLayer::SetRecordWavePath( const std::wstring& strRecordWavePath )
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->SetRecordWavePath(strRecordWavePath);
    }
}

///
/// @brief
///    获取设置的录波文件路径
///
std::wstring CDsmDataLayer::GetRecordWavePath()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetRecordWavePath();
    }
    return L"";
}


///
/// @brief
///    设置通知界面时使用的窗口
///
bool CDsmDataLayer::SetNotifyWnd(CWnd* pWnd)
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->SetNotifyWnd(pWnd);
    }
    return false;
}

///
/// @brief
///    获取设置的通知界面使用的Cwnd窗口
///
CWnd* CDsmDataLayer::GetNotifyWnd()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetNotifyWnd();
    }
    return NULL;
}

///
/// @brief
///    添加录波控制块,主要针对网络数据帧（网络采样值和Goose）
///
bool CDsmDataLayer::AddRecordItem( CBaseDetectItem* pRecordItem )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->AddRecordItem(pRecordItem);
    }
    return false;
}

///
/// @brief
///    从录波列表条目中移除一个控制块
///
bool CDsmDataLayer::RemoveRecordItem( CBaseDetectItem* pRecordItem )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->RemoveRecordItem(pRecordItem);
    }
    return false;
}

///
/// @brief
///    开始录网络采样值数据，用pcap格式保存
///
bool CDsmDataLayer::StartRecordNetSmv()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->StartRecordNetSmv();
    }
    return false;
}

///
/// @brief
///    开始录Goose报文，采用pcap格式保存
///
bool CDsmDataLayer::StartRecordGoose()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->StartRecordGoose();
    }
    return false;
}

///
/// @brief
///    开始录FT3数据帧
///
bool CDsmDataLayer::StartRecordFt3()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->StartRecordFt3();
    }
    return false;
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
bool CDsmDataLayer::StopRecordWave(uint32_t& nResult)
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->StopRecordWave(nResult);
    }
    return false;
}

///
/// @brief
///    取消当前录波
///
/// @return
///      取消成功返回true
///
bool CDsmDataLayer::CancelRecordWave()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->CancelRecordWave();
    }
    return false;
}

///
/// @brief
///    获取B码对时时间信息
///
/// @param[out] pData   外部申请的数据对象指针，函数填充指针指向对象的内容
///
/// @return
///     bool    数据获取成功返回true，pData中保存获取到的数据
///
bool CDsmDataLayer::GetTimeSyncIRIGB( CDataTimeSyncIRIGB* pData )
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice && IsPortActive(LIGHT_PORT_IRIG_B))
    {
         /*
         *  B码控制码元
         *  Bit8~bit0对应B码控制区域P6的9位；
         *  Bit15~bit9对应B码控制区域P7的低6位bit5~bit0
         */
        uint16_t nIrigBCtrlCode = 0;
        if (!DeviceIoControl(m_hGPMDevice, IOCTRL_GET_IRIGB_CONTROL, NULL, NULL, &nIrigBCtrlCode, sizeof(nIrigBCtrlCode), NULL, NULL) )
        {
            RETAILMSG(1, (_T("DeviceIoControl IOCTRL_GET_IRIGB_CONTROL Failed !\r\n")));
            return false;
        }

        if (pData)
        {
            pData->ResetData();
            UTC_TIME_t stUtcTime;
            if(GetFpgaUtcTime(&stUtcTime))
            {
                SYSTEMTIME sysTime = UtcTime2SystemTime(stUtcTime);
                pData->SetDateTime(&sysTime);
            }

            pData->SetDST(IRIGB_GET_DST(nIrigBCtrlCode));
            pData->SetDSP(IRIGB_GET_DSP(nIrigBCtrlCode));
            pData->SetLS(IRIGB_GET_LS(nIrigBCtrlCode));
            pData->SetLSP(IRIGB_GET_LSP(nIrigBCtrlCode));
            pData->SetTimeQuality(IRIGB_GET_QUALITY(nIrigBCtrlCode));

            float fOffset = static_cast<float>(IRIGB_GET_OFFSET(nIrigBCtrlCode));
            if (IRIGB_GET_OFFSET_0_5(nIrigBCtrlCode))
            {
                fOffset += 0.5;
            }
            if (IRIGB_GET_OFFSET_SIGN(nIrigBCtrlCode))
            {
                fOffset = -fOffset;
            }
            pData->SetTimeOffset(fOffset);

            return true;
        }
    }
    return false;
}

///
/// @brief
///    告诉数据层，已选择的控制块已经添加完毕
///
void CDsmDataLayer::AddSelectedCtrlBlkFinished()
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->AddSelectedCtrlBlkFinished();
    }
}

///
/// @brief
///    开始手动试验SMV发送的线程
///
UINT StartManualTestSMVThread(LPVOID pParam)
{
    // 检查参数指针
    if (NULL == pParam)
    {
        return FALSE;
    }

    // 拷贝线程参数
    SendThreadParam stParam = *(SendThreadParam*)pParam;
    if (NULL == stParam.m_pFrameSend)
    {
        return FALSE;
    }

    // 发送SMV控制块
    enumErrorCode   eRet = stParam.m_pFrameSend->StartManualTestSMV(stParam.m_hWnd);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        // 向窗口发送通知消息
        PostMessage(stParam.m_hWnd, WM_MANUAL_TEST_RESULT, eRet, 0);
        return FALSE;
    }

    return TRUE;
}

///
/// @brief
///    开始发送手动试验SMV控制块
///
/// @param[in]  hWnd  窗口句柄，当DI值发生变化时，会发送WM_DI_CHANGED消息给此窗口
///
/// @return
///     bool    成功返回true
///
bool CDsmDataLayer::StartManualTestSMV( HWND hWnd )
{
    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::SendSMVCbs(): Data sent manager uninit \n"));
        return false;
    }

    // 判断窗口句柄是否是一个存在的窗口
    if (!IsWindow(hWnd))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::SendSMVCbs(): Parameter error \n"));
        return false;
    }

    // 准备子线程参数
    SendThreadParam stParam;
    stParam.m_pFrameSend    = m_pFrameSend;
    stParam.m_hWnd          = hWnd;

    // 创建子线程，初始状态为暂停
    CWinThread* pThread = AfxBeginThread(StartManualTestSMVThread, &stParam, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    if (NULL == pThread)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::SendSMVCbs(): AfxBeginThread failed \n"));
        return false;
    }

    // 设置线程优先级
    CeSetThreadPriority(pThread->m_hThread, DATA_RECV_PROCESS_THREAD_PRIO);

    // 开始运行子线程
    pThread->ResumeThread();

    // 休眠一段时间，让子线程拷贝参数
    Sleep(100);
    return true;
}

///
/// @brief
///    手动试验SMV数据更新线程
///
UINT UpateManualTestSMVThread(LPVOID pParam)
{
    // 检查参数指针
    if (NULL == pParam)
    {
        return FALSE;
    }

    // 拷贝线程参数
    SendThreadParam stParam = *(SendThreadParam*)pParam;
    if (NULL == stParam.m_pFrameSend)
    {
        return FALSE;
    }

    // 更新手动试验SMV控制块
    if (!stParam.m_pFrameSend->UpateManualTestSMV())
    {
        return FALSE;
    }

    return TRUE;
}

///
/// @brief
///    更新手动试验SMV发送的数据
///
/// @return
///     bool    成功返回true
///
bool CDsmDataLayer::UpdateManualTestSMV()
{
    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::UpdateSMVSend(): Data sent manager uninit \n"));
        return false;
    }

    // 准备子线程参数
    SendThreadParam stParam;
    stParam.m_pFrameSend    = m_pFrameSend;
    stParam.m_hWnd          = NULL;

    // 创建子线程，初始状态为暂停
    CWinThread* pThread = AfxBeginThread(UpateManualTestSMVThread, &stParam, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    if (NULL == pThread)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::UpdateSMVSend(): AfxBeginThread failed \n"));
        return false;
    }

    // 设置线程优先级
    CeSetThreadPriority(pThread->m_hThread, DATA_RECV_PROCESS_THREAD_PRIO);

    // 开始运行子线程
    pThread->ResumeThread();

    // 休眠一段时间，让子线程拷贝参数
    Sleep(100);
    return true;
}

///
/// @brief
///    停止手动试验SMV发送
///
bool CDsmDataLayer::StopManualTestSMV()
{
    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::StopSMVSend(): Data sent manager uninit \n"));
        return false;
    }

    // 停止SMV发送
    return m_pFrameSend->StopManualTestSMV();
}

///
/// @brief
///    手动试验发送Goose数据的线程
///
UINT StartManualTestGooseThread(LPVOID pParam)
{
    // 检查参数指针
    if (NULL == pParam)
    {
        return FALSE;
    }

    // 拷贝线程参数
    SendThreadParam stParam = *(SendThreadParam*)pParam;
    if (NULL == stParam.m_pFrameSend)
    {
        return FALSE;
    }

    // 发送Goose控制块
    enumErrorCode eRet = stParam.m_pFrameSend->StartManualTestGoose(stParam.m_hWnd);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        // 向窗口发送通知消息
        PostMessage(stParam.m_hWnd, WM_MANUAL_TEST_RESULT, eRet, 0);
        return FALSE;
    }

    return TRUE;
}

///
/// @brief
///    开始发送手动试验Goose控制块
///
/// @param[in]  hWnd  窗口句柄，当DI值发生变化时，会发送WM_DI_CHANGED消息给此窗口
///
/// @return
///     bool    创建发送线程成功返回true
///
bool CDsmDataLayer::StartManualTestGoose( HWND hWnd )
{
    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::SendGooseCbs(): Data sent manager uninit \n"));
        return false;
    }

    // 判断窗口句柄是否是一个存在的窗口
    if (!IsWindow(hWnd))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::SendGooseCbs(): Parameter error \n"));
        return false;
    }

    // 准备子线程参数
    SendThreadParam stParam;
    stParam.m_pFrameSend    = m_pFrameSend;
    stParam.m_hWnd          = hWnd;

    // 创建子线程，初始状态为暂停
    CWinThread* pThread = AfxBeginThread(StartManualTestGooseThread, &stParam, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    if (NULL == pThread)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::SendGooseCbs(): AfxBeginThread failed \n"));
        return false;
    }

    // 设置线程优先级
    CeSetThreadPriority(pThread->m_hThread, DATA_RECV_PROCESS_THREAD_PRIO);

    // 开始运行子线程
    pThread->ResumeThread();

    // 休眠一段时间，让子线程拷贝参数
    Sleep(100);
    return true;
}

///
/// @brief
///    更新手动试验GOOSE数据的线程
///
UINT UpateManualTestGooseThread(LPVOID pParam)
{
    // 检查参数指针
    if (NULL == pParam)
    {
        return FALSE;
    }

    // 拷贝线程参数
    SendThreadParam stParam = *(SendThreadParam*)pParam;
    if (NULL == stParam.m_pFrameSend)
    {
        return FALSE;
    }

    // 发送Goose控制块
    if (!stParam.m_pFrameSend->UpateManualTestGoose())
    {
        return FALSE;
    }

    return TRUE;
}

///
/// @brief
///    更新手动试验Goose发送的数据
///
/// @return
///     bool    成功返回true
///
bool CDsmDataLayer::UpdateManualTestGoose()
{
    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::UpdateGooseSend(): Data sent manager uninit \n"));
        return false;
    }

    // 准备子线程参数
    SendThreadParam stParam;
    stParam.m_pFrameSend    = m_pFrameSend;
    stParam.m_hWnd          = NULL;

    // 创建子线程，初始状态为暂停
    CWinThread* pThread = AfxBeginThread(UpateManualTestGooseThread, &stParam, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    if (NULL == pThread)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::UpdateGooseSend(): AfxBeginThread failed \n"));
        return false;
    }

    // 设置线程优先级
    CeSetThreadPriority(pThread->m_hThread, DATA_RECV_PROCESS_THREAD_PRIO);

    // 开始运行子线程
    pThread->ResumeThread();

    // 休眠一段时间，让子线程拷贝参数
    Sleep(100);
    return true;
}

///
/// @brief
///    停止手动试验Goose发送
///
bool CDsmDataLayer::StopManualTestGoose()
{
    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::StopGooseSend(): Data sent manager uninit \n"));
        return false;
    }

    // 停止Goose发送
    return m_pFrameSend->StopManualTestGoose();
}

///
/// @brief
///    获得手动试验结果集数量
///
/// @return
///     unsigned int    手动试验结果集数量
///
unsigned int CDsmDataLayer::GetManualTestResultCount()
{
    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::GetManualTestResultCount(): Data sent manager uninit \n"));
        return 0;
    }

    return m_pFrameSend->GetManualTestResultCount();
}

///
/// @brief
///    获得指定索引号的手动试验结果集对象
///
/// @param[in]  nIndex    结果集索引号(从0开始计数)
///
/// @return
///     CStateTestResult*    存放指定状态测试结果的对象，失败时返回NULL
///
CStateTestResult* CDsmDataLayer::GetManualTestResult( unsigned int nIndex )
{
    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::GetManualTestResult(): Data sent manager uninit \n"));
        return 0;
    }

    return m_pFrameSend->GetManualTestResult(nIndex);
}

///
/// @brief
///    获得指定手动试验结果集对象的索引号
///
/// @param[in]  pCurResult    手动试验结果集对象
///
/// @return
///     int    结果集索引号，从0开始计数，失败时返回-1
///
int CDsmDataLayer::GetManualTestResultIndex( CStateTestResult* pCurResult )
{
    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::GetManualTestResultIndex(): Data sent manager uninit \n"));
        return 0;
    }

    return m_pFrameSend->GetManualTestResultIndex(pCurResult);
}

///
/// @brief
///    获得下一个手动试验结果集对象
///
/// @param[in]  pCurTestResult    指向当前结果集对象的指针
///
/// @return
///     CStateTestResult*    指向下一个结果集对象的指针，失败时返回NULL
///
CStateTestResult* CDsmDataLayer::NextManualTestResult( CStateTestResult* pCurTestResult )
{
    // 检查输入参数
    if (NULL == pCurTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::NextManualTestResult(): Parameter error \n"));
        return NULL;
    }

    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::NextManualTestResult(): Data sent manager uninit \n"));
        return NULL;
    }

    return m_pFrameSend->NextManualTestResult(pCurTestResult);
}

///
/// @brief
///    获得上一个手动试验结果集对象
///
/// @param[in]  pCurTestResult    指向当前结果集对象的指针
///
/// @return
///     CStateTestResult*    指向上一个结果集对象的指针，失败时返回NULL
///
CStateTestResult* CDsmDataLayer::PrevManualTestResult( CStateTestResult* pCurTestResult )
{
    // 检查输入参数
    if (NULL == pCurTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::PrevManualTestResult(): Parameter error \n"));
        return NULL;
    }

    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::PrevManualTestResult(): Data sent manager uninit \n"));
        return NULL;
    }

    return m_pFrameSend->PrevManualTestResult(pCurTestResult);
}

///
/// @brief
///    获得下一个含有SMV虚拟通道信息的手动试验结果集对象
///
/// @param[in]  pCurTestResult    指向当前结果集对象的指针
///
/// @return
///     CStateTestResult*    指向下一个结果集对象的指针，失败时返回NULL
///
CStateTestResult* CDsmDataLayer::NextManualTestSMVResult( CStateTestResult* pCurTestResult )
{
    // 检查输入参数
    if (NULL == pCurTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::NextManualTestSMVResult(): Parameter error \n"));
        return NULL;
    }

    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::NextManualTestSMVResult(): Data sent manager uninit \n"));
        return NULL;
    }

    return m_pFrameSend->NextManualTestSMVResult(pCurTestResult);
}

///
/// @brief
///    获得上一个含有SMV虚拟通道信息的手动试验结果集对象
///
/// @param[in]  pCurTestResult    指向当前结果集对象的指针
///
/// @return
///     CStateTestResult*    指向上一个结果集对象的指针，失败时返回NULL
///
CStateTestResult* CDsmDataLayer::PrevManualTestSMVResult( CStateTestResult* pCurTestResult )
{
    // 检查输入参数
    if (NULL == pCurTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::PrevManualTestSMVResult(): Parameter error \n"));
        return NULL;
    }

    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::PrevManualTestSMVResult(): Data sent manager uninit \n"));
        return NULL;
    }

    return m_pFrameSend->PrevManualTestSMVResult(pCurTestResult);
}

///
/// @brief
///    获得下一个含有GOOSE控制块的手动试验结果集对象
///
/// @param[in]  pCurTestResult    指向当前结果集对象的指针
///
/// @return
///     CStateTestResult*    指向下一个结果集对象的指针，失败时返回NULL
///
CStateTestResult* CDsmDataLayer::NextManualTestGooseResult( CStateTestResult* pCurTestResult )
{
    // 检查输入参数
    if (NULL == pCurTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::NextManualTestGooseResult(): Parameter error \n"));
        return NULL;
    }

    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::NextManualTestGooseResult(): Data sent manager uninit \n"));
        return NULL;
    }

    return m_pFrameSend->NextManualTestGooseResult(pCurTestResult);
}

///
/// @brief
///    获得上一个含有GOOSE控制块的手动试验结果集对象
///
/// @param[in]  pCurTestResult    指向当前结果集对象的指针
///
/// @return
///     CStateTestResult*    指向上一个结果集对象的指针，失败时返回NULL
///
CStateTestResult* CDsmDataLayer::PrevManualTestGooseResult( CStateTestResult* pCurTestResult )
{
    // 检查输入参数
    if (NULL == pCurTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::PrevManualTestGooseResult(): Parameter error \n"));
        return NULL;
    }

    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::PrevManualTestGooseResult(): Data sent manager uninit \n"));
        return NULL;
    }

    return m_pFrameSend->PrevManualTestGooseResult(pCurTestResult);
}

///
/// @brief
///    状态序列试验线程
///
UINT StateSequenceTestThread(LPVOID pParam)
{
    // 检查参数指针
    if (NULL == pParam)
    {
        return FALSE;
    }
    SendThreadParam stParam = *(SendThreadParam*)pParam;                ///< 拷贝线程参数

    // 检查数据发送管理器指针
    if (NULL == stParam.m_pFrameSend)
    {
        return FALSE;
    }

    // 开始状态序列测试
    enumErrorCode eRet = stParam.m_pFrameSend->StartStateSequenceTest(stParam.m_hWnd);

    // 向对话框发送消息
    PostMessage(stParam.m_hWnd, WM_STATUS_SEQUENCE_RESULT, eRet, 0);
    return TRUE;
}

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
bool CDsmDataLayer::StartStateSequenceTest( HWND hWnd )
{
    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::StartStateSequenceTest(): Data sent manager uninit \n"));
        return false;
    }

    // 判断窗口句柄是否是一个存在的窗口
    if (!IsWindow(hWnd))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::StartStateSequenceTest(): Parameter error \n"));
        return false;
    }

    // 准备子线程参数
    SendThreadParam stParam;
    stParam.m_pFrameSend    = m_pFrameSend;
    stParam.m_hWnd          = hWnd;

    // 创建子线程，初始状态为暂停
    CWinThread* pThread = AfxBeginThread(StateSequenceTestThread, &stParam, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    if (NULL == pThread)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::StartStateSequenceTest(): AfxBeginThread failed \n"));
        return false;
    }

    // 设置线程优先级
    CeSetThreadPriority(pThread->m_hThread, DATA_RECV_PROCESS_THREAD_PRIO);

    // 开始运行子线程
    pThread->ResumeThread();

    // 休眠一段时间，让子线程拷贝参数
    Sleep(100);
    return true;
}

///
/// @brief
///    停止状态序列测试
///
bool CDsmDataLayer::StopStateSequenceTest()
{
    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::StopStateSequenceTest(): Data sent manager uninit \n"));
        return false;
    }

    return m_pFrameSend->StopStateSequenceTest();
}

///
/// @brief
///    获得状态序列结果集数量
///
/// @return
///     unsigned int    状态序列结果集数量
///
unsigned int CDsmDataLayer::GetStateSequenceResultCount()
{
    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::GetStateSequenceResultCount(): Data sent manager uninit \n"));
        return 0;
    }

    return m_pFrameSend->GetStateSequenceResultCount();
}

///
/// @brief
///    获得指定状态号的状态序列结果集对象
///
/// @param[in]  iStateIndex    状态号(从0开始计数)
///
/// @return
///     CStateTestResult*    存放指定状态测试结果的对象，失败时返回NULL
///
CStateTestResult* CDsmDataLayer::GetStateSequenceResult( unsigned int iStateIndex )
{
    // 判断发送管理器是否已初始化
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::GetStateSequenceResult(): Data sent manager uninit \n"));
        return 0;
    }

    return m_pFrameSend->GetStateSequenceResult(iStateIndex);
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
void CDsmDataLayer::StartRecvData( NetFt3RecvFrameType enRecvType, DSM_OPERATE_MODE enMode /*= DSM_MODE_UNKOWN*/ )
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->StartRecvData(enRecvType, enMode);
    }
}

///
/// @brief
///    停止数据接收
///
void CDsmDataLayer::StopRecvData()
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->StopRecvData();
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
bool CDsmDataLayer::GetSmv1ABCChannles( uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetSmv1ABCChannles(nPhaseA, nPhaseB, nPhaseC);
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
bool CDsmDataLayer::SetSmv1ABCChannles( uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->SetSmv1ABCChannles(nPhaseA, nPhaseB, nPhaseC);
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
bool CDsmDataLayer::GetSmv2ABCChannles( uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetSmv2ABCChannles(nPhaseA, nPhaseB, nPhaseC);
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
bool CDsmDataLayer::SetSmv2ABCChannles( uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->SetSmv2ABCChannles(nPhaseA, nPhaseB, nPhaseC);
    }
    return false;
}

///
/// @brief
///    获取Fpga时间同步方式
///
FpgaTimeSyncMode CDsmDataLayer::GetFpgaTimeSyncMode()
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice)
    {
        FpgaTimeSyncMode enTimeSyncMode;
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_GET_SYNCTIME_MODE, NULL, NULL, &enTimeSyncMode, sizeof(enTimeSyncMode), NULL ,NULL) == FALSE)
        {
            RETAILMSG(1, (_T("CDsmDataLayer::GetFpgaTimeSyncMode() IOCTRL_GET_SYNCTIME_MODE failed\r\n")));
            return FPGA_TIME_SYNC_NONE;
        }
        return enTimeSyncMode;
    }
    return FPGA_TIME_SYNC_NONE;
}


///
/// @brief
///    设置Fpga时间同步方式
///
/// @param[in]  enMode 要设置的对时模式
///
/// @return
///     bool 设置成功返回true
///
bool CDsmDataLayer::SetFpgaTimeSyncMode( FpgaTimeSyncMode enMode )
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice)
    {
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_SET_SYNCTIME_MODE, &enMode, sizeof(enMode), NULL, NULL, NULL ,NULL) == FALSE)
        {
            RETAILMSG(1, (_T("CDsmDataLayer::SetFpgaTimeSyncMode() IOCTRL_SET_SYNCTIME_MODE failed\r\n")));
            return false;
        }
        return true;
    }
    return false;
}


///
/// @brief
///    获取FPGA时间
///
/// @param[out] pUtcTime    保存获取到的utc时间
///
/// @return
///     bool    获取成功返回true
///
bool CDsmDataLayer::GetFpgaUtcTime( UTC_TIME_t* pUtcTime )
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice && NULL != pUtcTime)
    {
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_GET_UTC_TIME, NULL, NULL, pUtcTime, sizeof(UTC_TIME_t), NULL ,NULL) == FALSE)
        {
            RETAILMSG(1, (_T("CDsmDataLayer::GetFpgaUtcTime IOCTRL_GET_UTC_TIME failed\r\n")));
            return false;
        }
        return true;
    }
    return false;
}

///
/// @brief
///    设置FPGA时间
///
/// @param[in] pUtcTime    要设置的utc时间
///
/// @return
///     bool    设置成功返回true
///
bool CDsmDataLayer::SetFpgaUtcTime( UTC_TIME_t* pUtcTime )
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice && NULL != pUtcTime)
    {
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_SET_UTC_TIME, pUtcTime, sizeof(UTC_TIME_t), NULL, NULL, NULL ,NULL) == FALSE)
        {
            RETAILMSG(1, (_T("CDsmDataLayer::SetFpgaUtcTime IOCTRL_SET_UTC_TIME failed\r\n")));
            return false;
        }
        return true;
    }
    return false;

}

///
/// @brief
///    配置接收数据dsp计算使用的采样率
///
bool CDsmDataLayer::SetDspSampleRate( uint32_t nSampleRate )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->SetDspSampleRate(nSampleRate);
    }
    return false;
}

///
/// @brief
///    开始/停止极性信息检测
///
/// @param[in]  bStart  是否开始极性监测，true开始监测，false停止监测
///
/// @note
///     只有在极性检查模式下生效
void CDsmDataLayer::StartPolarityCheck( bool bStart /*= true*/ )
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->StartPolarityCheck(bStart);
    }
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
bool CDsmDataLayer::SetSmvRecvChannelTemplate( enumSmvRecvConfigTemplateType enConfigType, CSmvRecvConfig* pSmvRecvConfig )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->SetSmvRecvChannelTemplate(enConfigType, pSmvRecvConfig);
    }
    return false;
}

///
/// @brief
///    更新缓存的控制块配置信息一次/二次值
///
/// @param[in]  enChannelType 要修改的通道类型，电压通道/电流通道
/// @param[in]  nPriSectype   要修改的值类型，是一次值（1）还是二次值（2）
/// @param[in]  nVal          最新的数值
///
void CDsmDataLayer::UpdateCachedRatedValues( enumChannelType enChannelType, uint32_t nPriSecType, float fVal )
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->UpdateCachedRatedValues(enChannelType, nPriSecType, fVal);
    }
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
bool CDsmDataLayer::GetPhaseCheckRealTimeChannelCounts(uint32_t& nChannelCountSmv1, uint32_t& nChannelCountSmv2)
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetPhaseCheckRealTimeChannelCounts(nChannelCountSmv1, nChannelCountSmv2);
    }
    return false;
}

///
/// @brief
///    获取端口整个状态字
///
/// @param[out] nState  获取成功保存各个端口的状态信息
///
/// @return
///     bool    获取成功返回true
///
bool CDsmDataLayer::GetPortState( uint16_t& nState )
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice)
    {
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_GET_OPTICAL_STATE, NULL, NULL, &nState, sizeof(uint16_t), NULL, NULL))
        {
            return true;
        }
        else
        {
            RETAILMSG(1, (_T("DeviceIoControl IOCTRL_GET_OPTICAL_STATE Failed \r\n")));
        }
    }
    return false;
}

///
/// @brief
///    判断某个端口的连接状态,针对以太网口有效
///
bool CDsmDataLayer::IsPortLinked( DsmPortName enPortName )
{
    bool bRet = false;
    uint16_t nState = 0;
    if (GetPortState(nState))
    {
        switch (enPortName)
        {
        case NET_PORT_LIGHT_A:
            bRet = ((nState & NET_PORT_LIGHT_A_LINK) == NET_PORT_LIGHT_A_LINK);
            break;
        case NET_PORT_LIGHT_B:
            bRet = ((nState & NET_PORT_LIGHT_B_LINK) == NET_PORT_LIGHT_B_LINK);
            break;
        case NET_PORT_ELECTRONIC:
            bRet = ((nState & NET_PORT_LIGHT_ELEC_LINK) == NET_PORT_LIGHT_ELEC_LINK);
            break;
        default:
            break;
        }
    }
    return bRet;
}

///
/// @brief
///    判断某个端口是否正在收发数据
///
bool CDsmDataLayer::IsPortActive( DsmPortName enPortName )
{
    bool bRet = false;
    uint16_t nState = 0;
    if (GetPortState(nState))
    {
        switch (enPortName)
        {
        case NET_PORT_LIGHT_A:
            bRet = ((nState & NET_PORT_LIGHT_A_ACTIVE) == NET_PORT_LIGHT_A_ACTIVE);
            break;
        case NET_PORT_LIGHT_B:
            bRet = ((nState & NET_PORT_LIGHT_B_ACTIVE) == NET_PORT_LIGHT_B_ACTIVE);
            break;
        case NET_PORT_ELECTRONIC:
            bRet = ((nState & NET_PORT_LIGHT_ELEC_ACTIVE) == NET_PORT_LIGHT_ELEC_ACTIVE);
            break;
        case FT3_PORT_LIGHT_A:
            bRet = ((nState & FT3_PORT_A_ACTIVE) == FT3_PORT_A_ACTIVE);
            break;
        case FT3_PORT_LIGHT_B:
            bRet = ((nState & FT3_PORT_B_ACTIVE) == FT3_PORT_B_ACTIVE);
            break;
        case FT3_PORT_LIGHT_C:
            bRet = ((nState & FT3_PORT_C_ACTIVE) == FT3_PORT_C_ACTIVE);
            break;
        case LIGHT_PORT_IRIG_B:
            bRet = ((nState & LIGHT_PORT_IRIGB_ACTIVE) == LIGHT_PORT_IRIGB_ACTIVE);
            break;
        default:
            break;
        }
    }
    return bRet;
}

///
/// @brief
///    开始发送b码(通过FT3发送端口)
///
/// @return
///     bool    发送成功返回true
///
bool CDsmDataLayer::StartIRIGBTx()
{
    return _SetFt3TxStyle(FT3_TX_STYLE_IRIG_B);
}

///
/// @brief
///    停止发送b码(通过FT3发送端口)
///
/// @return
///     bool    发送成功返回true
///
bool CDsmDataLayer::StopIRIGBTx()
{
    return _SetFt3TxStyle(FT3_TX_STYLE_FT3);
}

///
/// @brief
///    设置FT3发送端口发送数据类型(Ft3/B码)
///
/// @return
///     bool    设置成功返回true
///
bool CDsmDataLayer::_SetFt3TxStyle( enumFt3TxStyle enTxStyle )
{
    if (m_hGPMDevice != INVALID_HANDLE_VALUE)
    {
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_SET_FT3_TX_STYLE, (PBYTE)&enTxStyle, sizeof(enTxStyle), NULL, 0, NULL, NULL))
        {
            return true;
        }
    }
    return false;
}


///
/// @brief
///    设置1588对时时钟信息
///
/// @param[in] 
///
/// @return
///     bool    发送成功返回true
///
bool CDsmDataLayer::SetIeee1588MainClock( DsmPortName enPortName, const IEEE1588_CLOCK_SYNC_INFO& stIEEE1588ClockInfo)
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice)
    {
        if (enPortName >= NET_PORT_LIGHT_A && enPortName <= NET_PORT_ELECTRONIC)
        {
            IOCTRL_IN_PARAM_NET_BIG stIoctrlInParam;
            stIoctrlInParam.nDeviceItem = enPortName;
            memcpy_s(stIoctrlInParam.nInBuf, sizeof(stIoctrlInParam.nInBuf), &stIEEE1588ClockInfo, sizeof(stIEEE1588ClockInfo));
            if (DeviceIoControl(m_hGPMDevice, IOCTRL_IEEE1588_MAIN_CLOCK, &stIoctrlInParam, sizeof(stIoctrlInParam), NULL, NULL, NULL, NULL))
            {
                return true;
            }
        }
    }
    return false;
}
