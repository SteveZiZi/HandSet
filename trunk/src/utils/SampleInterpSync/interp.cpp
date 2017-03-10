/// @file
///     interp.cpp
///
/// @brief
///     插值算法实现
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
///    |2013.10.25     |1.0.1.1025 |插值算法增加一个设置重采样点缓存的大小的参数，修正因采样点时标错误引起的崩溃|
///
#include "stdafx.h"
#include "interp.h"
#include "GaussianElimination.h"

// =================================================================
// 分段线性插值法
// =================================================================
#if 0
///
/// @brief
///     分段线性插值函数的系数计算
///
/// @param[in]
///     pd_X   - 保存原采样点x值的缓存
/// @param[in]
///     pd_Y   - 保存原采样点y值的缓存
/// @param[in]
///     pd_Coe - 保存某一分段的插值函数的系数的缓存
/// @param[in]
///     i      - x角标（x0,x1,...）
///
void LinearCoe(double * pd_X, double * pd_Y,double * pd_Coe, int i)
{
	pd_Coe[0] = (pd_X[i+1]*pd_Y[i]-pd_X[i]*pd_Y[i+1])/(pd_X[i+1]-pd_X[i]);
	pd_Coe[1] = (pd_Y[i+1]-pd_Y[i])/(pd_X[i+1]-pd_X[i]);
}

///
/// @brief
///     分段线性插值（系数方程形式）
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
int  Interp_Linear(double *pd_X, double *pd_Y, int dataNum, double x0, double dx, float *pResult_Y ,int resultYSize)
{
	// 公式：
	// y = a*x + b
	// 当x0 <= x < x1时，
	// a = pd_Coe[1] = (x1*y0-x0*y1)/(x1-x0)
	// b = pd_Coe[0] = (y1-y0)/(x1-x0)
	// 其他分段类似
	double pd_Coe[2];              ///< 存放线性方程的系数a和b
	int i = 0;                     ///< 记录x角标（x0,x1,...）
	int k = 0;                     ///< 重采样点数，即插值y的位置
	double x = x0;                 ///< 重采样点的x值

	LinearCoe(pd_X, pd_Y,pd_Coe,i);

	while (x <= pd_X[dataNum-1])
	{
		// 超过最大重采样点数的限制则返回
		if ( k >= resultYSize )
			break;

		// 判断所在分段，计算该分段函数的系数
		while (x > pd_X[i+1])
		{
			i++;
			LinearCoe(pd_X, pd_Y,pd_Coe,i);
		}
		// 计算y值
		if (x == pd_X[i+1])
		{
			pResult_Y[k] = (float)pd_Y[i+1];
			x += dx;
			k++;
			continue;
		}
		pResult_Y[k] = (float)(pd_Coe[0]+pd_Coe[1]*x);
		x += dx;
		k++;
	}

	return k;
}
#else
///
/// @brief
///     分段线性插值（基函数形式）
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
int  Interp_Linear(double *pd_X, double *pd_Y, int dataNum, double x0, double dx, float *pResult_Y ,int resultYSize)
{
	// 公式：
	// y = l0(x) + l1(x)
	// 当x0 <= x < x1时，
	// l0(x) = y0*(x-x1)/(x0-x1);
	// l1(x) = y1*(x-x0)/(x1-x0);
	// 其他分段类似
	int i = 0;                     ///< 记录x角标（x0,x1,...）
	int k = 0;                     ///< 重采样点数，即插值y的位置
	double x = x0;                 ///< 重采样点的x值

	while (x <= pd_X[dataNum-1])
	{
		// 超过最大重采样点数的限制则返回
		if ( k >= resultYSize )
			break;

		// 判断所在分段
		while (x > pd_X[i+1])
		{
			i++;
		}
		// 计算y值
		if (x == pd_X[i+1])
		{
			pResult_Y[k] = (float)pd_Y[i+1];
			x += dx;
			k++;
			continue;
		}
		double lx0 = pd_Y[i]*(x-pd_X[i+1])/(pd_X[i]-pd_X[i+1]);
		double lx1 = pd_Y[i+1]*(x-pd_X[i])/(pd_X[i+1]-pd_X[i]);
		pResult_Y[k] = (float)(lx0 + lx1);
		x += dx;
		k++;
	}

	return k;
}
#endif

// =================================================================
// 分段二次多项式插值法
// =================================================================
#if 0
///
/// @brief
///     分段二次多项式插值函数的系数计算
///
/// @param[in]
///     pd_X   - 保存原采样点x值的缓存
/// @param[in]
///     pd_Y   - 保存原采样点y值的缓存
/// @param[in]
///     pd_Coe - 保存某一分段的插值函数的系数的缓存
/// @param[in]
///     i      - x角标（x0,x1,...）
///
void QuadrateCoe(double * pd_X, double * pd_Y,double * pd_Coe, int i)
{
	const int n = 2;
	double pd_MatrixA[(n+1)*(n+1)];
	double pd_VectorB[n+1];

	// 使用高斯消元法求解多项式系数
	pd_VectorB[0] = pd_Y[i-1];
	pd_VectorB[1] = pd_Y[i];
	pd_VectorB[2] = pd_Y[i+1];

	for(int j = 0; j < n+1; j++)
	{
		pd_MatrixA[j*3] = pd_X[i+j-1]*pd_X[i+j-1];
		pd_MatrixA[j*3 + 1] = pd_X[i+j-1];
		pd_MatrixA[j*3 + 2] = 1;
	}
	GaussRemove(pd_MatrixA,pd_VectorB,pd_Coe,n+1);
}

///
/// @brief
///     分段二次多项式插值法（系数方程形式）
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
int  Interp_Quadrate(double * pd_X, double * pd_Y, int N, double x0, double dx, float * pd_resultY ,int resultYSize)
{
	double pd_Coe[3];              ///< 记录二次多项式的系数
	int i = 1;                     ///< 记录x角标（x0,x1,...）
	int k = 0;                     ///< 重采样点数，即插值y的位置
	double x = x0;                 ///< 重采样点的x值
	double h = pd_X[2] - pd_X[1];  ///< 记录节点步长

	QuadrateCoe(pd_X, pd_Y,pd_Coe,i);

	// x <= x1+h/2的情况
	while (x <= pd_X[i] + h/2)
	{
		// 超过最大重采样点数的限制则返回
		if ( k >= resultYSize )
			break;

		pd_resultY[k] = (float)(pd_Coe[0]*x*x + pd_Coe[1]*x + pd_Coe[2]);
		x += dx;
		k++;
	}
	// xj - h/2 < x <= xj + h/2的情况
	while (x <= pd_X[N-3] + (pd_X[N-2] - pd_X[N-3])/2 )
	{
		// 超过最大重采样点数的限制则返回
		if ( k >= resultYSize )
			break;

		while (x > pd_X[i] + h/2)
		{
			i++;
			QuadrateCoe(pd_X, pd_Y,pd_Coe,i);
			h = pd_X[i+1] - pd_X[i];
		}		
		if (x == pd_X[i])
		{
			pd_resultY[k] = (float)pd_Y[i];
			x += dx;
			k++;
			continue;
		}
		pd_resultY[k] = (float)(pd_Coe[0]*x*x + pd_Coe[1]*x + pd_Coe[2]);
		x += dx;
		k++;
	}
	// x > x(n-1) - h/2的情况
	while (x <= pd_X[N-1])
	{
		// 超过最大重采样点数的限制则返回
		if ( k >= resultYSize )
			break;

		QuadrateCoe(pd_X, pd_Y,pd_Coe,i);
		if (x == pd_X[i])
		{
			pd_resultY[k] = (float)pd_Y[i];
			x += dx;
			k++;
			continue;
		}
		pd_resultY[k] = (float)(pd_Coe[0]*x*x + pd_Coe[1]*x + pd_Coe[2]);
		if (x == pd_X[N-1])
		{
			pd_resultY[k] = (float)pd_Y[N -1];
		}
		x += dx;
		k++;
	}
	return k;
}
#else
///
/// @brief
///     分段二次多项式插值法（拉格朗日插值基函数形式）
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
int  Interp_Quadrate(double * pd_X, double * pd_Y, int dataNum, double x0, double dx, float * pd_resultY ,int resultYSize)
{
	// 公式：
	// y = l0(x) + l1(x) + l2(x)
	// 当x <= x1+h/2, h = x2-x1时，
	// l0(x) = y0*((x-x1)*(x-x2))/((x0-x1)*(x0-x2));
	// l1(x) = y1*((x-x0)*(x-x2))/((x1-x0)*(x1-x2));
	// l2(x) = y2*((x-x0)*(x-x1))/((x2-x0)*(x2-x1));
	// 其他分段类似
	int i = 1;                     ///< 记录x角标（x0,x1,...）
	int k = 0;                     ///< 重采样点数，即插值y的位置
	double x = x0;                 ///< 重采样点的x值
	double h = pd_X[2] - pd_X[1];  ///< 记录节点步长
	double lx0 = 0.0;
	double lx1 = 0.0;
	double lx2 = 0.0;

	// x <= x1+h/2的情况
	while (x <= pd_X[i] + h/2)
	{
		// 超过最大重采样点数的限制则返回
		if ( k >= resultYSize )
			break;

		lx0 = pd_Y[i-1]*(x-pd_X[i])*(x-pd_X[i+1])/((pd_X[i-1]-pd_X[i])*(pd_X[i-1]-pd_X[i+1]));
		lx1 = pd_Y[i]*(x-pd_X[i-1])*(x-pd_X[i+1])/((pd_X[i]-pd_X[i-1])*(pd_X[i]-pd_X[i+1]));
		lx2 = pd_Y[i+1]*(x-pd_X[i-1])*(x-pd_X[i])/((pd_X[i+1]-pd_X[i])*(pd_X[i+1]-pd_X[i-1]));
		pd_resultY[k] = (float)(lx0 + lx1 + lx2);
		x += dx;
		k++;
	}
	// xj - h/2 < x <= xj + h/2的情况
	while (x <= pd_X[dataNum-3] + (pd_X[dataNum-2] - pd_X[dataNum-3])/2 )
	{
		// 超过最大重采样点数的限制则返回
		if ( k >= resultYSize )
			break;

		while (x > pd_X[i] + h/2)
		{
			i++;
			h = pd_X[i+1] - pd_X[i];
		}		
		if (x == pd_X[i])
		{
			pd_resultY[k] = (float)pd_Y[i];
			x += dx;
			k++;
			continue;
		}
		lx0 = pd_Y[i-1]*(x-pd_X[i])*(x-pd_X[i+1])/((pd_X[i-1]-pd_X[i])*(pd_X[i-1]-pd_X[i+1]));
		lx1 = pd_Y[i]*(x-pd_X[i-1])*(x-pd_X[i+1])/((pd_X[i]-pd_X[i-1])*(pd_X[i]-pd_X[i+1]));
		lx2 = pd_Y[i+1]*(x-pd_X[i-1])*(x-pd_X[i])/((pd_X[i+1]-pd_X[i])*(pd_X[i+1]-pd_X[i-1]));
		pd_resultY[k] = (float)(lx0 + lx1 + lx2);
		x += dx;
		k++;
	}
	// x > x(n-1) - h/2的情况
	while (x <= pd_X[dataNum-1])
	{
		// 超过最大重采样点数的限制则返回
		if ( k >= resultYSize )
			break;

		if (x == pd_X[i])
		{
			pd_resultY[k] = (float)pd_Y[i];
			x += dx;
			k++;
			continue;
		}
		lx0 = pd_Y[i-1]*(x-pd_X[i])*(x-pd_X[i+1])/((pd_X[i-1]-pd_X[i])*(pd_X[i-1]-pd_X[i+1]));
		lx1 = pd_Y[i]*(x-pd_X[i-1])*(x-pd_X[i+1])/((pd_X[i]-pd_X[i-1])*(pd_X[i]-pd_X[i+1]));
		lx2 = pd_Y[i+1]*(x-pd_X[i-1])*(x-pd_X[i])/((pd_X[i+1]-pd_X[i])*(pd_X[i+1]-pd_X[i-1]));
		pd_resultY[k] = (float)(lx0 + lx1 + lx2);
		if (x == pd_X[dataNum-1])
		{
			pd_resultY[k] = (float)pd_Y[dataNum -1];
		}
		x += dx;
		k++;
	}
	return k;
}
#endif

// =================================================================
// 分段三次多项式插值法
// =================================================================

///
/// @brief
///     分段三次多项式插值函数的系数计算
///
/// @param[in]
///     pd_X   - 保存原采样点x值的缓存
/// @param[in]
///     pd_Y   - 保存原采样点y值的缓存
/// @param[in]
///     pd_Coe - 保存某一分段的插值函数的系数的缓存
/// @param[in]
///     i      - 当前分段的索引号
///
void ThriceCoe(double * pd_X, double * pd_Y,double * pd_Coe, int i)
{
	const int n = 3;
	double pd_MatrixA[(n+1)*(n+1)];
	double pd_VectorB[n+1];

	// 设有方程组的矩阵形式为A*X=B，A,X,B是矩阵
	// 初始化矩阵A和B
	pd_VectorB[0] = pd_Y[i-2];
	pd_VectorB[1] = pd_Y[i-1];
	pd_VectorB[2] = pd_Y[i];
	pd_VectorB[3] = pd_Y[i+1];

	for(int j = 0; j < n+1; j++)
	{
		pd_MatrixA[j*4]     = pd_X[i+j-2]*pd_X[i+j-2]*pd_X[i+j-2];
		pd_MatrixA[j*4 + 1] = pd_X[i+j-2]*pd_X[i+j-2];
		pd_MatrixA[j*4 + 2] = pd_X[i+j-2];
		pd_MatrixA[j*4 + 3] = 1;		
	}
	// 使用高斯消元法求解多项式系数
	GaussianElimination(pd_MatrixA,pd_VectorB,pd_Coe,n+1);
}

///
/// @brief
///     分段三次多项式插值法（系数方程形式）
///
/// @param[in]
///     pd_X      - 保存原采样点x值的缓存
/// @param[in]
///     pd_Y      - 保存原采样点y值的缓存
/// @param[in]
///     dataNum   - 原采样点数目
/// @param[in]
///     x0        - 重采样的波形输出的开始x值
/// @param[in]
///     dx        - 重采样的波形输出的采样间隔
/// @param[in]
///     pResult_Y - 保存重采样的波形输出（重采样点）的缓存
///
/// @return
///     重采样的波形输出（重采样点）的数量
///
int  Interp_Thrice(double * pd_X, double * pd_Y, int dataNum, double x0, double dx, float * pd_resultY ,int resultYSize)
{
	double pd_Coe[4];              ///< 记录三次多项式的系数
	int i = 2;                     ///< 记录x角标（x0,x1,...）
	int k = 0;                     ///< 重采样点数，即插值y的位置
	double x = x0;                 ///< 重采样点的x值
	double h = pd_X[3] - pd_X[2];  ///< 记录节点步长

	ThriceCoe(pd_X, pd_Y,pd_Coe,i);

	// x <= x2+h/2的情况
	while (x <= pd_X[i] + h/2)
	{
		// 超过最大重采样点数的限制则返回
		if ( k >= resultYSize )
			break;

		pd_resultY[k] = (float)(pd_Coe[0]*x*x*x + pd_Coe[1]*x*x + pd_Coe[2]*x + pd_Coe[3]);
		x += dx;
		k++;
	}
	// xj - h/2 < x <= xj + h/2的情况
	while (x <= pd_X[dataNum-4] + (pd_X[dataNum-3] - pd_X[dataNum-4])/2 )
	{
		// 超过最大重采样点数的限制则返回
		if ( k >= resultYSize )
			break;

		while (x > pd_X[i] + h/2)
		{
			i++;
			ThriceCoe(pd_X, pd_Y,pd_Coe,i);
			h = pd_X[i+1] - pd_X[i];
		}		
		if (x == pd_X[i-1])
		{
			pd_resultY[k] = (float)pd_Y[i-1];
			x += dx;
			k++;
			continue;
		}
		if (x == pd_X[i])
		{
			pd_resultY[k] = (float)pd_Y[i];
			x += dx;
			k++;
			continue;
		}
		pd_resultY[k] = (float)(pd_Coe[0]*x*x*x + pd_Coe[1]*x*x + pd_Coe[2]*x + pd_Coe[3]);
		x += dx;
		k++;
	}	
	int ll = 0;
	// x > x(n-1) - h/2的情况
	while (x <= pd_X[dataNum-1])
	{
		// 超过最大重采样点数的限制则返回
		if ( k >= resultYSize )
			break;

		ThriceCoe(pd_X, pd_Y,pd_Coe,i);
		if (x == pd_X[i])
		{
			pd_resultY[k] = (float)pd_Y[i];
			x += dx;
			k++;
			continue;
		}
		pd_resultY[k] = (float)(pd_Coe[0]*x*x*x + pd_Coe[1]*x*x + pd_Coe[2]*x + pd_Coe[3]);
		if (x == pd_X[dataNum-1])
		{
			pd_resultY[k] = (float)pd_Y[dataNum -1];
		}
		x += dx;
		k++;
	}	
	return k;
}

// =================================================================
// 三次样条插值法
// =================================================================

///
/// @brief
///     三次样条插值函数的系数组M的计算
///
/// @param[in]
///     pd_X   - 保存原采样点x值的缓存
/// @param[in]
///     pd_Y   - 保存原采样点y值的缓存
/// @param[in]
///     pd_Coe - 保存某一分段的插值函数的系数的缓存
/// @param[in]
///     i      - 当前分段的索引号
///
void SplineCoe(double * pd_X, double * pd_Y, int N, double * pd_h, double * pd_M)
{
	int n = N -1;
	double * pd_Alpha   = new double[n];    ///< 系数数组Alpha
	double * pd_Beta    = new double[n];    ///< 系数数组Beta
	double * pd_Gama    = new double[n];    ///< 系数数组Gama
	double * pd_MatrixA = new double[n*n];
	int i,j;

	// 计算Alpha,Beta,Gama
	for(i = 0; i < n; i++)
	{
		pd_Alpha[i] = pd_h[0]/(pd_h[0] + pd_h[i]);
		pd_Gama[i] = 1 - pd_Alpha[i];
		pd_Beta[i] = 6 * (((pd_Y[1]-pd_Y[0])/pd_h[0]) - ((pd_Y[i+1]-pd_Y[i])/pd_h[i])) / (pd_h[0] + pd_h[i]);
	}

	for(i = 0; i < n; i++)
	{
		for(j = 0; j < n; j++)
		{
			if (i == j)
			{
				pd_MatrixA[i*n+j] = 2;
			}
			else if (j == (i+1))
			{
				pd_MatrixA[i*n+j] = pd_Alpha[i];
			}
			else if (j == (i-1))
			{
				pd_MatrixA[i*n+j] = pd_Gama[i];
			}
			else
				pd_MatrixA[i*n+j] = 0;
		}
	}

	pd_MatrixA[n-1] = pd_Gama[0];
	pd_MatrixA[(n-1)*n] = pd_Alpha[n-1];

	// 使用高斯消元法求解多项式系数
	GaussianElimination(pd_MatrixA,pd_Beta,&(pd_M[1]),N-1);
	pd_M[0] = pd_M[N-1];

	delete[] pd_Alpha;
	delete[] pd_Beta;
	delete[] pd_Gama;
	delete[] pd_MatrixA;
}

///
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
int  Interp_Spline(double * pd_X, double * pd_Y, int dataNum, double x0, double dx, float * pd_resultY ,int resultYSize)
{
	int n  = dataNum - 1;
	double * pd_h = new double[n];        ///< 节点步长数组h
	double * pd_M = new double[dataNum];  ///< 系数数组M
	int i;                                ///< 记录x角标（x0,x1,...）

	//计算节点步长数组h
	for(i = 0; i < n; i++)
	{
		pd_h[i] = pd_X[i + 1] - pd_X[i];
	}

	SplineCoe(pd_X,pd_Y,dataNum,pd_h,pd_M);	

	i = 1;
	int k = 0;                            ///< 重采样点数，即插值y的位置
	double x = x0;                        ///< 重采样点的x值

	pd_resultY[k] = (float)pd_Y[0];
	x += dx;
	k++;

	while (x <= pd_X[dataNum-1])
	{
		// 超过最大重采样点数的限制则返回
		if ( k >= resultYSize )
			break;

		while (x > pd_X[i])
		{
			i++;
		}		
		if (x == pd_X[i])
		{
			pd_resultY[k] = (float)pd_Y[i];
			x += dx;
			k++;
			continue;
		}

		if (x > 171)
		{
			double temp = pow(2,3)/5;
		}

		double xi = pd_X[i] - x;
		double xi1 = x - pd_X[i-1];
		double mi = pd_M[i] / (6*pd_h[i-1]);
		double mi1 = pd_M[i-1] / (6*pd_h[i-1]);
		double yi = pd_Y[i]/pd_h[i-1];
		double yi1 = pd_Y[i-1]/pd_h[i-1];

		pd_resultY[k] = (float)(pd_Y[i-1] + (pd_Y[i] - pd_Y[i-1])*(x - pd_X[i-1])/pd_h[i-1]
		                - ((pd_M[i] / (6*pd_h[i-1]))/6 + (pd_M[i-1] / (6*pd_h[i-1]))/3) * pd_h[i-1]*(x - pd_X[i-1]) 
		                + pow((x - pd_X[i-1]),2) * (pd_M[i-1] / (6*pd_h[i-1]))/2
		                + ((pd_M[i] / (6*pd_h[i-1])) - (pd_M[i-1] / (6*pd_h[i-1]))) * pow((x - pd_X[i-1]),3) / (6 * pd_h[i-1]));

		x += dx;
		k++;
	}

	delete[] pd_h;
	delete[] pd_M;
	return k;
}
