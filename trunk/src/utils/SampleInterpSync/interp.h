#ifndef INTERP_H_

#define INTERP_H_

#include "src\include\stdint.h"

///
/// @brief
///     分段线性插值
///
/// @param[in]
///     pd_X        - 保存原采样点x值的缓存
/// @param[in]
///     pd_Y        - 保存原采样点y值的缓存
/// @param[in]
///     dataNum     - 原采样点数目
/// @param[in]
///     x0          - 重采样的波形输出的开始x值
/// @param[in]
///     dx          - 重采样的波形输出的采样间隔
/// @param[in]
///     pResult_Y   - 保存重采样的波形输出（重采样点）的缓存
/// @param[in]
///     resultYSize - 保存重采样的波形输出（重采样点）的缓存的大小
///
/// @return
///     重采样的波形输出（重采样点）的数量
///
int  Interp_Linear(double *pd_X, double *pd_Y, int dataNum, double x0, double dx, float *pd_resultY ,int resultYSize);

///
/// @brief
///     分段二次多项式插值法
///
/// @param[in]
///     pd_X        - 保存原采样点x值的缓存
/// @param[in]
///     pd_Y        - 保存原采样点y值的缓存
/// @param[in]
///     dataNum     - 原采样点数目
/// @param[in]
///     x0          - 重采样的波形输出的开始x值
/// @param[in]
///     dx          - 重采样的波形输出的采样间隔
/// @param[in]
///     pResult_Y   - 保存重采样的波形输出（重采样点）的缓存
/// @param[in]
///     resultYSize - 保存重采样的波形输出（重采样点）的缓存的大小
///
/// @return
///     重采样的波形输出（重采样点）的数量
///
int  Interp_Quadrate(double *pd_X, double *pd_Y, int dataNum, double x0, double dx, float *pd_resultY ,int resultYSize);

///
/// @brief
///     分段三次多项式插值法（系数方程形式）
///
/// @param[in]
///     pd_X        - 保存原采样点x值的缓存
/// @param[in]
///     pd_Y        - 保存原采样点y值的缓存
/// @param[in]
///     dataNum     - 原采样点数目
/// @param[in]
///     x0          - 重采样的波形输出的开始x值
/// @param[in]
///     dx          - 重采样的波形输出的采样间隔
/// @param[in]
///     pResult_Y   - 保存重采样的波形输出（重采样点）的缓存
/// @param[in]
///     resultYSize - 保存重采样的波形输出（重采样点）的缓存的大小
///
/// @return
///     重采样的波形输出（重采样点）的数量
///
int  Interp_Thrice(double *pd_X, double *pd_Y, int dataNum, double x0, double dx, float *pd_resultY ,int resultYSize);

/// @brief
///     三次样条插值法
///
/// @param[in]
///     pd_X        - 保存原采样点x值的缓存
/// @param[in]
///     pd_Y        - 保存原采样点y值的缓存
/// @param[in]
///     dataNum     - 原采样点数目
/// @param[in]
///     x0          - 重采样的波形输出的开始x值
/// @param[in]
///     dx          - 重采样的波形输出的采样间隔
/// @param[in]
///     pResult_Y   - 保存重采样的波形输出（重采样点）的缓存
/// @param[in]
///     resultYSize - 保存重采样的波形输出（重采样点）的缓存的大小
///
/// @return
///     重采样的波形输出（重采样点）的数量
///
int  Interp_Spline(double *pd_X, double *pd_Y, int dataNum, double x0, double dx, float *pd_resultY ,int resultYSize);

#endif
