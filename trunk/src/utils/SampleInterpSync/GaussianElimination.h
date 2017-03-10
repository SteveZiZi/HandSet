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
GAUSS_BOOL Gauss_Eliminate(double * pMatrixA,double * pMatrixA_R,double * pVectorB, double * pVectorB_R,int M, int N);

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
GAUSS_BOOL Gauss_BackSolve(double * pMatrixA_E,double * pVectorB_E, double * pSolution,int M, int N);

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
GAUSS_BOOL GaussianElimination(double * pd_MatrixA,double* pd_VectorB,double * pd_Coe, int n);

#endif
