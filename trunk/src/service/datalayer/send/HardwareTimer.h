///
/// @brief
///     硬件定时器
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.12.23
///
/// 修改历史：
///
#pragma once

class CHardwareTimer
{
public:
    CHardwareTimer(void);
    virtual ~CHardwareTimer(void);

    ///
    /// @brief
    ///     初始化硬件定时器
    ///
    /// @param[in]  csName  硬件定时器名字
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool Init(CString csName);

    ///
    /// @brief
    ///     卸载硬件定时器
    ///
    void UnInit();

    ///
    /// @brief
    ///     等待指定时间或用户取消事件置位
    ///
    /// @param[in]  dwDelay     等待超时（ms）
    /// @param[in]  hEvtCancel  用户取消事件
    ///
    /// @return
    ///     bool    等待成功返回true
    ///
    bool Wait(DWORD dwDelay, HANDLE hEvtCancel);

private:
    HANDLE                              m_hTmrDev;                      ///< 硬件定时器句柄
    HANDLE                              m_hEvtExpired;                  ///< 超时事件句柄
};
