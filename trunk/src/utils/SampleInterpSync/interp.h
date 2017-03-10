#ifndef INTERP_H_

#define INTERP_H_

#include "src\include\stdint.h"

///
/// @brief
///     �ֶ����Բ�ֵ
///
/// @param[in]
///     pd_X        - ����ԭ������xֵ�Ļ���
/// @param[in]
///     pd_Y        - ����ԭ������yֵ�Ļ���
/// @param[in]
///     dataNum     - ԭ��������Ŀ
/// @param[in]
///     x0          - �ز����Ĳ�������Ŀ�ʼxֵ
/// @param[in]
///     dx          - �ز����Ĳ�������Ĳ������
/// @param[in]
///     pResult_Y   - �����ز����Ĳ���������ز����㣩�Ļ���
/// @param[in]
///     resultYSize - �����ز����Ĳ���������ز����㣩�Ļ���Ĵ�С
///
/// @return
///     �ز����Ĳ���������ز����㣩������
///
int  Interp_Linear(double *pd_X, double *pd_Y, int dataNum, double x0, double dx, float *pd_resultY ,int resultYSize);

///
/// @brief
///     �ֶζ��ζ���ʽ��ֵ��
///
/// @param[in]
///     pd_X        - ����ԭ������xֵ�Ļ���
/// @param[in]
///     pd_Y        - ����ԭ������yֵ�Ļ���
/// @param[in]
///     dataNum     - ԭ��������Ŀ
/// @param[in]
///     x0          - �ز����Ĳ�������Ŀ�ʼxֵ
/// @param[in]
///     dx          - �ز����Ĳ�������Ĳ������
/// @param[in]
///     pResult_Y   - �����ز����Ĳ���������ز����㣩�Ļ���
/// @param[in]
///     resultYSize - �����ز����Ĳ���������ز����㣩�Ļ���Ĵ�С
///
/// @return
///     �ز����Ĳ���������ز����㣩������
///
int  Interp_Quadrate(double *pd_X, double *pd_Y, int dataNum, double x0, double dx, float *pd_resultY ,int resultYSize);

///
/// @brief
///     �ֶ����ζ���ʽ��ֵ����ϵ��������ʽ��
///
/// @param[in]
///     pd_X        - ����ԭ������xֵ�Ļ���
/// @param[in]
///     pd_Y        - ����ԭ������yֵ�Ļ���
/// @param[in]
///     dataNum     - ԭ��������Ŀ
/// @param[in]
///     x0          - �ز����Ĳ�������Ŀ�ʼxֵ
/// @param[in]
///     dx          - �ز����Ĳ�������Ĳ������
/// @param[in]
///     pResult_Y   - �����ز����Ĳ���������ز����㣩�Ļ���
/// @param[in]
///     resultYSize - �����ز����Ĳ���������ز����㣩�Ļ���Ĵ�С
///
/// @return
///     �ز����Ĳ���������ز����㣩������
///
int  Interp_Thrice(double *pd_X, double *pd_Y, int dataNum, double x0, double dx, float *pd_resultY ,int resultYSize);

/// @brief
///     ����������ֵ��
///
/// @param[in]
///     pd_X        - ����ԭ������xֵ�Ļ���
/// @param[in]
///     pd_Y        - ����ԭ������yֵ�Ļ���
/// @param[in]
///     dataNum     - ԭ��������Ŀ
/// @param[in]
///     x0          - �ز����Ĳ�������Ŀ�ʼxֵ
/// @param[in]
///     dx          - �ز����Ĳ�������Ĳ������
/// @param[in]
///     pResult_Y   - �����ز����Ĳ���������ز����㣩�Ļ���
/// @param[in]
///     resultYSize - �����ز����Ĳ���������ز����㣩�Ļ���Ĵ�С
///
/// @return
///     �ز����Ĳ���������ز����㣩������
///
int  Interp_Spline(double *pd_X, double *pd_Y, int dataNum, double x0, double dx, float *pd_resultY ,int resultYSize);

#endif
