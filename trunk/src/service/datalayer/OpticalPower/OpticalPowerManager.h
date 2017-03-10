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

#ifndef __OPTICAL_POWER_MANAGER_H__
#define __OPTICAL_POWER_MANAGER_H__
#include <vector>
#include <afxmt.h>
#include "LightPort.h"

class COpticalPowerManager
{
private:
    COpticalPowerManager(void);
    ~COpticalPowerManager(void);

public:
    ///
    /// @brief
    ///     获取光功率管理器对象
    ///
    static COpticalPowerManager* GetInstance();

    ///
    /// @brief
    ///     初始化
    ///
    /// @param[in]  hGPMDevice    GPM驱动句柄
    ///
    /// @return
    ///     bool    初始化成功返回true
    ///
    bool Init(HANDLE hGPMDevice);

    ///
    /// @brief
    ///    清除所有的光口对象
    ///
    void ClearAllLightPorts();

    ///
    /// @brief
    ///    开始采集光功率
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool StartGatherOpticalPower();

    ///
    /// @brief
    ///    停止采集光功率
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool StopGatherOpticalPower();

    ///
    /// @brief
    ///    获得指定名字的光口对象
    ///
    /// @param[in]  ePortName    光口名
    ///
    /// @return
    ///     CLightPort*    指向光口对象的指针，失败时返回NULL
    ///
    CLightPort* GetLightPort(DsmPortName ePortName);

    ///
    /// @brief
    ///    复位统计值
    ///
    void Reset();

protected:
    ///
    /// @brief
    ///    光功率采集线程
    ///
    static UINT __OpticalPowerGatherThread(LPVOID pParam);

private:
    HANDLE                              m_hGPMDevice;                   ///< GPM设备句柄
    std::vector<CLightPort*>            m_vecLightPorts;                ///< 存放端口对象指针的容器

    CCriticalSection                    m_criticalGatherStatus;         ///< 采集状态临界区
    bool                                m_bIsGathering;                 ///< 当前是否正在采集光功率
    CEvent                              m_eventStopGather;              ///< 停止采集事件
};
#endif
