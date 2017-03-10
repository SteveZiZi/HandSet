/**
 * \file
 *      utility_goose.h
 *
 * \brief
 *      gooseҵ���÷���ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/7/5
 */

#ifndef UTILITY_GOOSE_H__
#define UTILITY_GOOSE_H__

#include "src/service/datalayer/dsmdatalayer.h"



/**
 *      ��ȡָ��goose data������������ֵ
 *
 * \param]in]
 *      pData           ����ָ��
 *  \param[out]
 *      csType         ������������
 *  \param[out]
 *      csValue        ����ֵ
 */
extern void DSM_GetGooseDataInfo(GOOSE_DATA* pData, CString& csType, CString& csValue);

/**
 *      ����ͨ�����ͻ�ȡͨ������������ֵ������
 *
 *  \param[in]
 *      pChannel        gooseͨ��ָ��
 *  \param[out]
 *      csType          gooseͨ����������
 *  \param[out]
 *      csValue         gooseͨ������ֵ
 */
extern void DSM_GetGooseDataInfo(CGooseChannel* pChannel, CString& csType, CString& csValue);

/**
 *      ����һ����������ʱ����ʾֵ
 *
 *  \param[in]
 *      nInt1           ��������������
 *  \param[in]
 *      nDec1           ������С������
 *  \param[in]
 *      nInt2           ������������
 *  \param[in]
 *      nDec2           ����С������
 *  \param[in]
 *      nT              С�����־���
 *  \return
 *      CString         ��ʾ�ַ���
 */
extern CString DSM_GetDValue(uint32_t nInt1, uint32_t nDec1, uint32_t nInt2, uint32_t nDec2, int nT = 9);

#endif //UTILITY_GOOSE_H__
