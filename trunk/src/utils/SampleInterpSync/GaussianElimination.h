#ifndef GAUSS_ELIMINATION_H_
#define GAUSS_ELIMINATION_H_

#ifndef GAUSS_BOOL
#define GAUSS_BOOL  int
#endif

#ifndef GAUSS_FALSE
#define GAUSS_FALSE int(0)
#endif

#ifndef GAUSS_TRUE
#define GAUSS_TRUE  int(1)
#endif

///
/// @brief
///     ��˹��Ԫ������Ԫ����
///
/// @param[in]
///     pMatrixA   - ָ�򷽳����ϵ������A�Ļ���
/// @param[in]
///     pMatrixA_R - ���ڱ����Ӧ����A����Ԫ����Ļ���
/// @param[in]
///     pVectorB   - ָ�򷽳���ľ���B�Ļ���
/// @param[in]
///     pMatrixA_R - ���ڱ����Ӧ����B����Ԫ����Ļ���
/// @param[in]
///     M          - ����A������
/// @param[in]
///     N          - ����A������
///
/// @return
///     GAUSS_TRUE - �ɹ��� GAUSS_FALSE - ʧ��
///
GAUSS_BOOL Gauss_Eliminate(double * pMatrixA,double * pMatrixA_R,double * pVectorB, double * pVectorB_R,int M, int N);

///
/// @brief
///     ��˹��Ԫ���Ļش�����
///
/// @param[in]
///     pMatrixA_E - ָ���Ӧ�ڷ������ϵ������A����Ԫ���̵Ľ���Ļ���
/// @param[in]
///     pVectorB_E - ָ���Ӧ�ڷ�����ľ���B����Ԫ���̵Ľ���Ļ���
/// @param[in]
///     pSolution  - ���ڱ��淽����Ľ�Ļ���
/// @param[in]
///     M          - ����A������
/// @param[in]
///     N          - ����A������
///
/// @return
///     GAUSS_TRUE - �ɹ��� GAUSS_FALSE - ʧ��
///
GAUSS_BOOL Gauss_BackSolve(double * pMatrixA_E,double * pVectorB_E, double * pSolution,int M, int N);

///
/// @brief
///     ��˹��Ԫ������ⷽ���飬������ʽ��A*X=B��
///
/// @param[in]
///     pd_MatrixA - �������ϵ������A
/// @param[in]
///     pd_VectorB - ������ľ���B
/// @param[in]
///     pd_Coe     - ���淽����Ľ�Ļ���
/// @param[in]
///     n          - ������δ֪��������
///
/// @return
///     GAUSS_TRUE - �ɹ��� GAUSS_FALSE - ʧ��
///
GAUSS_BOOL GaussianElimination(double * pd_MatrixA,double* pd_VectorB,double * pd_Coe, int n);

#endif
