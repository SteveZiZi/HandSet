/// @file
///     ad2.cpp
/// @brief
///     ˫AD��ؼ���
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/22
/// @revise
/// 
/*******************************************************************************
˫AD�㷨˵����
    
*******************************************************************************/
#include "ad2.h"
///
/// @brief
///    ˫AD�����㷨1��|AD1-AD2|��
/// @param[in] 
///    adv1     AD1����ֵ
///    adv2     AD2����ֵ
/// @return
///    float    |AD1-AD2|
///
float adCalc1(float adv1, float adv2)
{
    return fabs(adv1-adv2);
}

///
/// @brief
///    ˫AD�����㷨2��||AD1|-|AD2||��
/// @param[in] 
///    adv1     AD1����ֵ
///    adv2     AD2����ֵ
/// @return
///    float    ||AD1|-|AD2||
///
float adCalc2(float adv1, float adv2)
{
    return fabs(fabs(adv1)-fabs(adv2));
}
