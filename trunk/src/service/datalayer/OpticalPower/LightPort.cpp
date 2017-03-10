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

#include "StdAfx.h"
#include "LightPort.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\include\ctrlcode_def.h"

///
/// @brief
///    构造函数
///
/// @param[in]  ePortName    端口设备标识
/// @param[in]  hGPMDevice    GPM驱动句柄
///
CLightPort::CLightPort(DsmPortName ePortName, HANDLE hGPMDevice)
: m_ePortName(ePortName)
, m_hGPMDevice(hGPMDevice)
, m_bInited(false)
, m_fCurOpticalPower(0.0f)
, m_fMinOpticalPower(FLT_MAX)
, m_fMaxOpticalPower(FLT_GUARD)
{
    // 根据设备标识初始化波长
    switch(m_ePortName)
    {
    case NET_PORT_LIGHT_A:
    case NET_PORT_LIGHT_B:
        {
            m_iWaveLength = 1310;
        }
        break;

    default:
        {
            m_iWaveLength = 850;
        }
    }
}

///
/// @brief
///    析构函数
///
CLightPort::~CLightPort(void)
{
}

///
/// @brief
///    获得光口对象的名称标记
///
/// @return
///     DsmPortName    光口名称标记
///
DsmPortName CLightPort::GetPortName()
{
    return m_ePortName;
}

///
/// @brief
///    获得光口波长（nm）
///
/// @return
///     int    波长（nm）
///
int CLightPort::GetWaveLength()
{
    return m_iWaveLength;
}

///
/// @brief
///    获得光口的当前光功率
///
/// @param[out]  fDBM    当前光功率，以dBm为单位
/// @param[out]  fUW    当前光功率，以uW为单位
///
void CLightPort::GetCurOpticalPower(float& fDBM, float& fUW)
{
    fUW     = m_fCurOpticalPower;
    fDBM    = (float)(10 * log10(fUW/1000));
}

///
/// @brief
///    获得光口的历史最小光功率（dBm）
///
/// @return
///     float    光功率（dBm）
///
float CLightPort::GetMinOpticalPower()
{
    float       fUW = 0.0f;


    // 如果已成功读取一次数据
    if (m_bInited)
    {
        fUW = m_fMinOpticalPower;
    }

    return (float)(10 * log10(fUW/1000));
}

///
/// @brief
///    获得光口的历史最大光功率（dBm）
///
/// @return
///     float    光功率（dBm）
///
float CLightPort::GetMaxOpticalPower()
{
    float       fUW = 0.0f;


    // 如果已成功读取一次数据
    if (m_bInited)
    {
        fUW = m_fMaxOpticalPower;
    }

    return (float)(10 * log10(fUW/1000));
}

///
/// @brief
///    重新读取光口的当前光功率
///
void CLightPort::__GatherCurOpticalPower()
{
    unsigned short  usOpticalPower  = 0;


    // 在此读取光口功率(0.1uW)
    if (DeviceIoControl(
        m_hGPMDevice, 
        IOCTRL_GET_OPTICAL_POWER, 
        &m_ePortName, 
        sizeof(m_ePortName), 
        &usOpticalPower, 
        sizeof(usOpticalPower), 
        NULL, 
        NULL
        ) == FALSE)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CLightPort::__GatherCurOpticalPower(): IOCTRL_GET_OPTICAL_POWER failed\n"));
        return ;
    }

    // 标记为已成功读取过数据
    m_bInited = true;

    // 记录当前光功率(uW)
    m_fCurOpticalPower = (float)usOpticalPower/10;

    // 如果当前光功率比历史最大光功率大
    if (m_fCurOpticalPower > m_fMaxOpticalPower)
    {
        m_fMaxOpticalPower = m_fCurOpticalPower;
    }

    // 如果当前光功率比历史最小光功率小
    if (m_fCurOpticalPower < m_fMinOpticalPower)
    {
        m_fMinOpticalPower = m_fCurOpticalPower;
    }
}

///
/// @brief
///    复位统计值
///
void CLightPort::Reset()
{
    // 标记为未读取一次数据
    m_bInited           = false;

    // 复位最小值最大值
    m_fMinOpticalPower  = FLT_MAX;
    m_fMaxOpticalPower  = FLT_GUARD;
}
