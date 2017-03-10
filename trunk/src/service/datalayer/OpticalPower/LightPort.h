/// @file
///
/// @brief
///     计算光功率的端口
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.7.19
///
/// 修改历史：
///

#ifndef __LIGHT_PORT_H__
#define __LIGHT_PORT_H__
#include "src\service\datalayer\include\datatypes.h"

class CLightPort
{
    friend class COpticalPowerManager;

public:
    ///
    /// @brief
    ///    构造函数
    ///
    /// @param[in]  ePortName    端口设备标识
    /// @param[in]  hGPMDevice    GPM驱动句柄
    ///
    CLightPort(DsmPortName ePortName, HANDLE hGPMDevice);

    ///
    /// @brief
    ///    析构函数
    ///
    virtual ~CLightPort(void);

    ///
    /// @brief
    ///    获得光口对象的名称标记
    ///
    /// @return
    ///     DsmPortName    光口名称标记
    ///
    DsmPortName GetPortName();

    ///
    /// @brief
    ///    获得光口波长（nm）
    ///
    /// @return
    ///     int    波长（nm）
    ///
    int GetWaveLength();

    ///
    /// @brief
    ///    获得光口的当前光功率
    ///
    /// @param[out]  fDBM    当前光功率，以dBm为单位
    /// @param[out]  fUW    当前光功率，以uW为单位
    ///
    void GetCurOpticalPower(float& fDBM, float& fUW);

    ///
    /// @brief
    ///    获得光口的历史最小光功率（dBm）
    ///
    /// @return
    ///     float    光功率（dBm）
    ///
    float GetMinOpticalPower();

    ///
    /// @brief
    ///    获得光口的历史最大光功率（dBm）
    ///
    /// @return
    ///     float    光功率（dBm）
    ///
    float GetMaxOpticalPower();

    ///
    /// @brief
    ///    复位统计值
    ///
    void Reset();

protected:

    ///
    /// @brief
    ///    重新读取光口的当前光功率
    ///
    void __GatherCurOpticalPower();

private:
    DsmPortName                         m_ePortName;                    ///< 端口设备标识
    HANDLE                              m_hGPMDevice;                   ///< GPM设备句柄
    bool                                m_bInited;                      ///< 是否已成功读取一次数据

    int                                 m_iWaveLength;                  ///< 波长（nm）
    float                               m_fCurOpticalPower;             ///< 当前光功率（uW）
    float                               m_fMinOpticalPower;             ///< 最小光功率（uW）
    float                               m_fMaxOpticalPower;             ///< 最大光功率（uW）
};
#endif
