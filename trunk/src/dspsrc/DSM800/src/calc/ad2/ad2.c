/// @file
///     ad2.cpp
/// @brief
///     双AD相关计算
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/22
/// @revise
/// 
/*******************************************************************************
双AD算法说明：
    
*******************************************************************************/
#include "ad2.h"
///
/// @brief
///    双AD计算算法1（|AD1-AD2|）
/// @param[in] 
///    adv1     AD1采样值
///    adv2     AD2采样值
/// @return
///    float    |AD1-AD2|
///
float adCalc1(float adv1, float adv2)
{
    return fabs(adv1-adv2);
}

///
/// @brief
///    双AD计算算法2（||AD1|-|AD2||）
/// @param[in] 
///    adv1     AD1采样值
///    adv2     AD2采样值
/// @return
///    float    ||AD1|-|AD2||
///
float adCalc2(float adv1, float adv2)
{
    return fabs(fabs(adv1)-fabs(adv2));
}
