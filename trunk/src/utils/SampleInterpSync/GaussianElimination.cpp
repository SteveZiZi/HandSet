/// @file
///     GaussianElimination.cpp
///
/// @brief
///     ��˹��Ԫ��ʵ�֣����ڷ��������
///
/// @note
///
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.10.21
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#include "stdafx.h"
#include "math.h"
#include "GaussianElimination.h"

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
GAUSS_BOOL Gauss_Eliminate(double * pMatrixA,double * pMatrixA_R,double * pVectorB, double * pVectorB_R,int M, int N)
{
	double m,temp;
	int i,j,k,l;
	int flag = 0;

	// ����ԭʼ���������
	for(i = 0; i < M; i++)
	{
		pVectorB_R[i] = pVectorB[i];
		for(j = 0; j < N; j++)
		{
			pMatrixA_R[i*N+j] = pMatrixA[i*N+j];
		}
	}

	// ��ʼ��ȥ��
	for(k = 0; k < M-1; k++)
	{
		temp = pMatrixA_R[k];
		flag = k;

		// �ҳ���Ԫ��
		for(l = k; l < M; l++ )
		{
			if (fabs(pMatrixA_R[l*N+k]) > fabs(temp)) 
			{
				temp = pMatrixA_R[l*N+k];
				flag = l;
			}
		}

		// ������
		if (flag != k)
		{
			// ����ϵ������Ԫ��
			for(l = k; l < N; l++)
			{
				temp = pMatrixA_R[flag*N+l];
				pMatrixA_R[flag*N+l] = pMatrixA_R[k*N+l];
				pMatrixA_R[k*N+l] = temp;
			}

			// ��������Ԫ��
			temp = pVectorB_R[flag];
			pVectorB_R[flag] = pVectorB_R[k];
			pVectorB_R[k] = temp;
		}

		for(i = k+1; i < M; i++)
		{
			m = pMatrixA_R[i*N+k]/pMatrixA_R[k*N+k];

			// ��ȥ��Ԫ��Ϊ0
			pMatrixA_R[i*N+k] = 0;

			for(j = k+1; j < N; j++)
			{
				pMatrixA_R[i*N+j] = pMatrixA_R[i*N+j]-m*pMatrixA_R[k*N+j];
			}

			pVectorB_R[i]= pVectorB_R[i] - m*pVectorB_R[k];
		}
	}
	return GAUSS_TRUE;
}


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
GAUSS_BOOL Gauss_BackSolve(double * pMatrixA_E,double * pVectorB_E, double * pSolution,int M, int N)
{
	int j,k;
	double temp;

	pSolution[N-1] = pVectorB_E[M-1]/pMatrixA_E[(M-1)*N+N-1];

	for(k = N-2; k >= 0; k--)
	{
		temp = 0;
		for(j = k+1; j < N; j++)
		{
			temp += pMatrixA_E[k*N+j]*pSolution[j];
		}
		pSolution[k] = (pVectorB_E[k] - temp)/pMatrixA_E[k*N+k];
	}

	return GAUSS_TRUE;
}

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
GAUSS_BOOL GaussianElimination(double * pd_MatrixA,double* pd_VectorB,double * pd_Coe, int n)
{
	double * pd_MatrixA_Gauss = new double[n*n];
	double * pd_VectorB_Gauss = new double[n];

	// ��Ԫ����
	Gauss_Eliminate(pd_MatrixA,pd_MatrixA_Gauss,pd_VectorB,pd_VectorB_Gauss,n,n);
	// �ش�����
	Gauss_BackSolve(pd_MatrixA_Gauss, pd_VectorB_Gauss, pd_Coe, n, n);

	delete[] pd_MatrixA_Gauss;
	delete[] pd_VectorB_Gauss;
	return GAUSS_TRUE;
}
