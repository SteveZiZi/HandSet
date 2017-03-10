/// @file
///     GaussianElimination.cpp
///
/// @brief
///     高斯消元法实现，用于方程组求解
///
/// @note
///
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.10.21
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#include "stdafx.h"
#include "math.h"
#include "GaussianElimination.h"

///
/// @brief
///     高斯消元法的消元过程
///
/// @param[in]
///     pMatrixA   - 指向方程组的系数矩阵A的缓存
/// @param[in]
///     pMatrixA_R - 用于保存对应矩阵A的消元结果的缓存
/// @param[in]
///     pVectorB   - 指向方程组的矩阵B的缓存
/// @param[in]
///     pMatrixA_R - 用于保存对应矩阵B的消元结果的缓存
/// @param[in]
///     M          - 矩阵A的行数
/// @param[in]
///     N          - 矩阵A的列数
///
/// @return
///     GAUSS_TRUE - 成功； GAUSS_FALSE - 失败
///
GAUSS_BOOL Gauss_Eliminate(double * pMatrixA,double * pMatrixA_R,double * pVectorB, double * pVectorB_R,int M, int N)
{
	double m,temp;
	int i,j,k,l;
	int flag = 0;

	// 复制原始矩阵和向量
	for(i = 0; i < M; i++)
	{
		pVectorB_R[i] = pVectorB[i];
		for(j = 0; j < N; j++)
		{
			pMatrixA_R[i*N+j] = pMatrixA[i*N+j];
		}
	}

	// 开始消去法
	for(k = 0; k < M-1; k++)
	{
		temp = pMatrixA_R[k];
		flag = k;

		// 找出主元素
		for(l = k; l < M; l++ )
		{
			if (fabs(pMatrixA_R[l*N+k]) > fabs(temp)) 
			{
				temp = pMatrixA_R[l*N+k];
				flag = l;
			}
		}

		// 交换行
		if (flag != k)
		{
			// 交换系数矩阵元素
			for(l = k; l < N; l++)
			{
				temp = pMatrixA_R[flag*N+l];
				pMatrixA_R[flag*N+l] = pMatrixA_R[k*N+l];
				pMatrixA_R[k*N+l] = temp;
			}

			// 交换向量元素
			temp = pVectorB_R[flag];
			pVectorB_R[flag] = pVectorB_R[k];
			pVectorB_R[k] = temp;
		}

		for(i = k+1; i < M; i++)
		{
			m = pMatrixA_R[i*N+k]/pMatrixA_R[k*N+k];

			// 消去的元素为0
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
///     高斯消元法的回代过程
///
/// @param[in]
///     pMatrixA_E - 指向对应于方程组的系数矩阵A的消元过程的结果的缓存
/// @param[in]
///     pVectorB_E - 指向对应于方程组的矩阵B的消元过程的结果的缓存
/// @param[in]
///     pSolution  - 用于保存方程组的解的缓存
/// @param[in]
///     M          - 矩阵A的行数
/// @param[in]
///     N          - 矩阵A的列数
///
/// @return
///     GAUSS_TRUE - 成功； GAUSS_FALSE - 失败
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
///     高斯消元法（求解方程组，矩阵形式：A*X=B）
///
/// @param[in]
///     pd_MatrixA - 方程组的系数矩阵A
/// @param[in]
///     pd_VectorB - 方程组的矩阵B
/// @param[in]
///     pd_Coe     - 保存方程组的解的缓存
/// @param[in]
///     n          - 方程组未知数的数量
///
/// @return
///     GAUSS_TRUE - 成功； GAUSS_FALSE - 失败
///
GAUSS_BOOL GaussianElimination(double * pd_MatrixA,double* pd_VectorB,double * pd_Coe, int n)
{
	double * pd_MatrixA_Gauss = new double[n*n];
	double * pd_VectorB_Gauss = new double[n];

	// 消元过程
	Gauss_Eliminate(pd_MatrixA,pd_MatrixA_Gauss,pd_VectorB,pd_VectorB_Gauss,n,n);
	// 回代过程
	Gauss_BackSolve(pd_MatrixA_Gauss, pd_VectorB_Gauss, pd_Coe, n, n);

	delete[] pd_MatrixA_Gauss;
	delete[] pd_VectorB_Gauss;
	return GAUSS_TRUE;
}
