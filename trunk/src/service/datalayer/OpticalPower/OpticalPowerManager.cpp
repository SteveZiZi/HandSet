/// @file
///
/// @brief
///     光功率管理器
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.7.19
///
/// 修改历史：
///

#include "StdAfx.h"
#include "OpticalPowerManager.h"
#include "src\service\datalayer\include\config.h"

///
/// @brief
///     构造函数
///
COpticalPowerManager::COpticalPowerManager(void)
: m_bIsGathering(false)
, m_hGPMDevice(NULL)
{
    m_vecLightPorts.clear();
    m_eventStopGather.ResetEvent();
}

///
/// @brief
///     析构函数
///
COpticalPowerManager::~COpticalPowerManager(void)
{
    ClearAllLightPorts();
}

///
/// @brief
///     获取光功率管理器对象
///
COpticalPowerManager* COpticalPowerManager::GetInstance()
{
    static COpticalPowerManager s_opticalPowerManager;
    return &s_opticalPowerManager;
}

///
/// @brief
///     初始化
///
/// @param[in]  hGPMDevice    GPM驱动句柄
///
/// @return
///     bool    初始化成功返回true
///
bool COpticalPowerManager::Init(HANDLE hGPMDevice)
{
    // 保存GPM驱动句柄
    if(NULL == hGPMDevice)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"COpticalPowerManager::Init(): Parameter error \n"));
        return false;
    }
    m_hGPMDevice = hGPMDevice;

    // 如果端口容器为空，那么创建各个端口对象
    if (0 == m_vecLightPorts.size())
    {
        DsmPortName ePortName[] = 
        {
            NET_PORT_LIGHT_A,                                           ///< 以太网光口A
            NET_PORT_LIGHT_B,                                           ///< 以太网光口B
        };


        // 向容器中添加光口对象
        for (int i = 0; i < sizeof(ePortName)/sizeof(ePortName[0]); i++)
        {
            // 创建光口对象
            CLightPort* pLightPort = new CLightPort(ePortName[i], m_hGPMDevice);
            if (NULL == pLightPort)
            {
                return false;
            }

            // 保存对象指针到容器中
            m_vecLightPorts.push_back(pLightPort);
        }
    }

    return true;
}

///
/// @brief
///    清除所有的光口对象
///
void COpticalPowerManager::ClearAllLightPorts()
{
    std::vector<CLightPort*>::iterator iter;


    // 释放容器中的所有对象
    for (iter = m_vecLightPorts.begin(); iter != m_vecLightPorts.end(); iter++)
    {
        CLightPort* pLightPort = *iter;
        delete pLightPort;
    }

    // 清空容器
    m_vecLightPorts.clear();
}

///
/// @brief
///    光功率采集线程
///
UINT COpticalPowerManager::__OpticalPowerGatherThread(LPVOID pParam)
{
    std::vector<CLightPort*>::iterator iter;


    // 获得采集管理器指针
    COpticalPowerManager* pManager = (COpticalPowerManager*)pParam;
    if (NULL == pManager)
    {
        goto ExitHandle;
    }

    // 如果当前正在采集光功率，那么直接返回
    pManager->m_criticalGatherStatus.Lock();
    if (pManager->m_bIsGathering)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"COpticalPowerManager::__OpticalPowerGatherThread(): Gather thread already started \n"));
        pManager->m_criticalGatherStatus.Unlock();
        return true;
    }
    // 标记为正在采集光功率
    pManager->m_bIsGathering = true;
    pManager->m_criticalGatherStatus.Unlock();

    // 开始不断采集功率
    while(1)
    {
        // 休眠一段时间，同时判断是否需要停止采集线程
        if (pManager->m_eventStopGather.Lock(100))
        {
            break;
        }

        // 遍历每一个光口对象，刷新其光功率和波长
        for (iter = pManager->m_vecLightPorts.begin(); iter != pManager->m_vecLightPorts.end(); iter++)
        {
            CLightPort* pLightPort = *iter;
            if (NULL != pLightPort)
            {
                // 刷新光功率
                pLightPort->__GatherCurOpticalPower();
            }
        }
    }

ExitHandle:
    // 标记为已停止采集光功率
    pManager->m_bIsGathering = false;
    return TRUE;
}

///
/// @brief
///    开始采集光功率
///
/// @return
///     bool    成功返回true
///
bool COpticalPowerManager::StartGatherOpticalPower()
{
    // 检查是否已初始化
    if (0 == m_vecLightPorts.size())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"COpticalPowerManager::StartGatherOpticalPower(): Optical power manager uninit \n"));
        return false;
    }

    // 创建光功率采集子线程
    if (NULL == AfxBeginThread(__OpticalPowerGatherThread, this))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"COpticalPowerManager::StartGatherOpticalPower(): AfxBeginThread failed \n"));
        return false;
    }

    return true;
}

///
/// @brief
///    停止采集光功率
///
/// @return
///     bool    成功返回true
///
bool COpticalPowerManager::StopGatherOpticalPower()
{
    // 如果当前正在采集光功率，那么停止采集线程
    if (m_bIsGathering)
    {
        // 置位取消事件
        m_eventStopGather.SetEvent();

        // 循环等待采集线程结束
        while(m_bIsGathering)
        {
            Sleep(10);
        }

        // 复位取消事件
        m_eventStopGather.ResetEvent();
    }

    return true;
}

///
/// @brief
///    获得指定名字的光口对象
///
/// @param[in]  ePortName    光口名
///
/// @return
///     CLightPort*    指向光口对象的指针，失败时返回NULL
///
CLightPort* COpticalPowerManager::GetLightPort( DsmPortName ePortName )
{
    std::vector<CLightPort*>::iterator iter;


    // 遍历容器中的所有对象
    for (iter = m_vecLightPorts.begin(); iter != m_vecLightPorts.end(); iter++)
    {
        CLightPort* pLightPort = *iter;


        // 如果找到了指定光口名的对象，那么返回对象指针
        if ((NULL != pLightPort) && (ePortName == pLightPort->GetPortName()))
        {
            return pLightPort;
        }
    }

    return NULL;
}

///
/// @brief
///    复位统计值
///
void COpticalPowerManager::Reset()
{
    std::vector<CLightPort*>::iterator  iter;
    bool                                bIsGathering    = false;


    // 保存采集状态
    bIsGathering = m_bIsGathering;

    // 停止采集光功率
    StopGatherOpticalPower();

    // 遍历容器中的所有对象，复位每一个端口对象的统计数据
    for (iter = m_vecLightPorts.begin(); iter != m_vecLightPorts.end(); iter++)
    {
        CLightPort* pLightPort = *iter;


        // 复位每一个端口的统计数据
        if (NULL != pLightPort)
        {
            pLightPort->Reset();
        }
    }

    // 如果先前正在采集数据，那么重新创建采集线程
    if (bIsGathering)
    {
        // 开始采集光功率
        StartGatherOpticalPower();
    }
}
