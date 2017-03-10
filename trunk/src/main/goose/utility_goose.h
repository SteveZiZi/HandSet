/**
 * \file
 *      utility_goose.h
 *
 * \brief
 *      goose业务常用方法头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/5
 */

#ifndef UTILITY_GOOSE_H__
#define UTILITY_GOOSE_H__

#include "src/service/datalayer/dsmdatalayer.h"



/**
 *      获取指定goose data的类型描述与值
 *
 * \param]in]
 *      pData           数据指针
 *  \param[out]
 *      csType         数据类型描述
 *  \param[out]
 *      csValue        数据值
 */
extern void DSM_GetGooseDataInfo(GOOSE_DATA* pData, CString& csType, CString& csValue);

/**
 *      根据通道类型获取通道关联的数据值与类型
 *
 *  \param[in]
 *      pChannel        goose通道指针
 *  \param[out]
 *      csType          goose通道数据类型
 *  \param[out]
 *      csValue         goose通道数据值
 */
extern void DSM_GetGooseDataInfo(CGooseChannel* pChannel, CString& csType, CString& csValue);

/**
 *      计算一个合理的相对时间显示值
 *
 *  \param[in]
 *      nInt1           被减数整数部分
 *  \param[in]
 *      nDec1           被减数小数部分
 *  \param[in]
 *      nInt2           减数整数部分
 *  \param[in]
 *      nDec2           减数小数部分
 *  \param[in]
 *      nT              小数部分精度
 *  \return
 *      CString         显示字符串
 */
extern CString DSM_GetDValue(uint32_t nInt1, uint32_t nDec1, uint32_t nInt2, uint32_t nDec2, int nT = 9);

#endif //UTILITY_GOOSE_H__
