/// @file
///     interp.cpp
///
/// @brief
///     ��ֵ�㷨ʵ��
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
///    |2013.10.25     |1.0.1.1025 |��ֵ�㷨����һ�������ز����㻺��Ĵ�С�Ĳ����������������ʱ���������ı���|
///
#include "stdafx.h"
#include "interp.h"
#include "GaussianElimination.h"

// =================================================================
// �ֶ����Բ�ֵ��
// =================================================================
#if 0
///
/// @brief
///     �ֶ����Բ�ֵ������ϵ������
///
/// @param[in]
///     pd_X   - ����ԭ������xֵ�Ļ���
/// @param[in]
///     pd_Y   - ����ԭ������yֵ�Ļ���
/// @param[in]
///     pd_Coe - ����ĳһ�ֶεĲ�ֵ������ϵ���Ļ���
/// @param[in]
///     i      - x�Ǳ꣨x0,x1,...��
///
void LinearCoe(double * pd_X, double * pd_Y,double * pd_Coe, int i)
{
	pd_Coe[0] = (pd_X[i+1]*pd_Y[i]-pd_X[i]*pd_Y[i+1])/(pd_X[i+1]-pd_X[i]);
	pd_Coe[1] = (pd_Y[i+1]-pd_Y[i])/(pd_X[i+1]-pd_X[i]);
}

///
/// @brief
///     �ֶ����Բ�ֵ��ϵ��������ʽ��
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
int  Interp_Linear(double *pd_X, double *pd_Y, int dataNum, double x0, double dx, float *pResult_Y ,int resultYSize)
{
	// ��ʽ��
	// y = a*x + b
	// ��x0 <= x < x1ʱ��
	// a = pd_Coe[1] = (x1*y0-x0*y1)/(x1-x0)
	// b = pd_Coe[0] = (y1-y0)/(x1-x0)
	// �����ֶ�����
	double pd_Coe[2];              ///< ������Է��̵�ϵ��a��b
	int i = 0;                     ///< ��¼x�Ǳ꣨x0,x1,...��
	int k = 0;                     ///< �ز�������������ֵy��λ��
	double x = x0;                 ///< �ز������xֵ

	LinearCoe(pd_X, pd_Y,pd_Coe,i);

	while (x <= pd_X[dataNum-1])
	{
		// ��������ز��������������򷵻�
		if ( k >= resultYSize )
			break;

		// �ж����ڷֶΣ�����÷ֶκ�����ϵ��
		while (x > pd_X[i+1])
		{
			i++;
			LinearCoe(pd_X, pd_Y,pd_Coe,i);
		}
		// ����yֵ
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
///     �ֶ����Բ�ֵ����������ʽ��
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
int  Interp_Linear(double *pd_X, double *pd_Y, int dataNum, double x0, double dx, float *pResult_Y ,int resultYSize)
{
	// ��ʽ��
	// y = l0(x) + l1(x)
	// ��x0 <= x < x1ʱ��
	// l0(x) = y0*(x-x1)/(x0-x1);
	// l1(x) = y1*(x-x0)/(x1-x0);
	// �����ֶ�����
	int i = 0;                     ///< ��¼x�Ǳ꣨x0,x1,...��
	int k = 0;                     ///< �ز�������������ֵy��λ��
	double x = x0;                 ///< �ز������xֵ

	while (x <= pd_X[dataNum-1])
	{
		// ��������ز��������������򷵻�
		if ( k >= resultYSize )
			break;

		// �ж����ڷֶ�
		while (x > pd_X[i+1])
		{
			i++;
		}
		// ����yֵ
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
// �ֶζ��ζ���ʽ��ֵ��
// =================================================================
#if 0
///
/// @brief
///     �ֶζ��ζ���ʽ��ֵ������ϵ������
///
/// @param[in]
///     pd_X   - ����ԭ������xֵ�Ļ���
/// @param[in]
///     pd_Y   - ����ԭ������yֵ�Ļ���
/// @param[in]
///     pd_Coe - ����ĳһ�ֶεĲ�ֵ������ϵ���Ļ���
/// @param[in]
///     i      - x�Ǳ꣨x0,x1,...��
///
void QuadrateCoe(double * pd_X, double * pd_Y,double * pd_Coe, int i)
{
	const int n = 2;
	double pd_MatrixA[(n+1)*(n+1)];
	double pd_VectorB[n+1];

	// ʹ�ø�˹��Ԫ��������ʽϵ��
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
///     �ֶζ��ζ���ʽ��ֵ����ϵ��������ʽ��
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
int  Interp_Quadrate(double * pd_X, double * pd_Y, int N, double x0, double dx, float * pd_resultY ,int resultYSize)
{
	double pd_Coe[3];              ///< ��¼���ζ���ʽ��ϵ��
	int i = 1;                     ///< ��¼x�Ǳ꣨x0,x1,...��
	int k = 0;                     ///< �ز�������������ֵy��λ��
	double x = x0;                 ///< �ز������xֵ
	double h = pd_X[2] - pd_X[1];  ///< ��¼�ڵ㲽��

	QuadrateCoe(pd_X, pd_Y,pd_Coe,i);

	// x <= x1+h/2�����
	while (x <= pd_X[i] + h/2)
	{
		// ��������ز��������������򷵻�
		if ( k >= resultYSize )
			break;

		pd_resultY[k] = (float)(pd_Coe[0]*x*x + pd_Coe[1]*x + pd_Coe[2]);
		x += dx;
		k++;
	}
	// xj - h/2 < x <= xj + h/2�����
	while (x <= pd_X[N-3] + (pd_X[N-2] - pd_X[N-3])/2 )
	{
		// ��������ز��������������򷵻�
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
	// x > x(n-1) - h/2�����
	while (x <= pd_X[N-1])
	{
		// ��������ز��������������򷵻�
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
///     �ֶζ��ζ���ʽ��ֵ�����������ղ�ֵ��������ʽ��
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
int  Interp_Quadrate(double * pd_X, double * pd_Y, int dataNum, double x0, double dx, float * pd_resultY ,int resultYSize)
{
	// ��ʽ��
	// y = l0(x) + l1(x) + l2(x)
	// ��x <= x1+h/2, h = x2-x1ʱ��
	// l0(x) = y0*((x-x1)*(x-x2))/((x0-x1)*(x0-x2));
	// l1(x) = y1*((x-x0)*(x-x2))/((x1-x0)*(x1-x2));
	// l2(x) = y2*((x-x0)*(x-x1))/((x2-x0)*(x2-x1));
	// �����ֶ�����
	int i = 1;                     ///< ��¼x�Ǳ꣨x0,x1,...��
	int k = 0;                     ///< �ز�������������ֵy��λ��
	double x = x0;                 ///< �ز������xֵ
	double h = pd_X[2] - pd_X[1];  ///< ��¼�ڵ㲽��
	double lx0 = 0.0;
	double lx1 = 0.0;
	double lx2 = 0.0;

	// x <= x1+h/2�����
	while (x <= pd_X[i] + h/2)
	{
		// ��������ز��������������򷵻�
		if ( k >= resultYSize )
			break;

		lx0 = pd_Y[i-1]*(x-pd_X[i])*(x-pd_X[i+1])/((pd_X[i-1]-pd_X[i])*(pd_X[i-1]-pd_X[i+1]));
		lx1 = pd_Y[i]*(x-pd_X[i-1])*(x-pd_X[i+1])/((pd_X[i]-pd_X[i-1])*(pd_X[i]-pd_X[i+1]));
		lx2 = pd_Y[i+1]*(x-pd_X[i-1])*(x-pd_X[i])/((pd_X[i+1]-pd_X[i])*(pd_X[i+1]-pd_X[i-1]));
		pd_resultY[k] = (float)(lx0 + lx1 + lx2);
		x += dx;
		k++;
	}
	// xj - h/2 < x <= xj + h/2�����
	while (x <= pd_X[dataNum-3] + (pd_X[dataNum-2] - pd_X[dataNum-3])/2 )
	{
		// ��������ز��������������򷵻�
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
	// x > x(n-1) - h/2�����
	while (x <= pd_X[dataNum-1])
	{
		// ��������ز��������������򷵻�
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
// �ֶ����ζ���ʽ��ֵ��
// =================================================================

///
/// @brief
///     �ֶ����ζ���ʽ��ֵ������ϵ������
///
/// @param[in]
///     pd_X   - ����ԭ������xֵ�Ļ���
/// @param[in]
///     pd_Y   - ����ԭ������yֵ�Ļ���
/// @param[in]
///     pd_Coe - ����ĳһ�ֶεĲ�ֵ������ϵ���Ļ���
/// @param[in]
///     i      - ��ǰ�ֶε�������
///
void ThriceCoe(double * pd_X, double * pd_Y,double * pd_Coe, int i)
{
	const int n = 3;
	double pd_MatrixA[(n+1)*(n+1)];
	double pd_VectorB[n+1];

	// ���з�����ľ�����ʽΪA*X=B��A,X,B�Ǿ���
	// ��ʼ������A��B
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
	// ʹ�ø�˹��Ԫ��������ʽϵ��
	GaussianElimination(pd_MatrixA,pd_VectorB,pd_Coe,n+1);
}

///
/// @brief
///     �ֶ����ζ���ʽ��ֵ����ϵ��������ʽ��
///
/// @param[in]
///     pd_X      - ����ԭ������xֵ�Ļ���
/// @param[in]
///     pd_Y      - ����ԭ������yֵ�Ļ���
/// @param[in]
///     dataNum   - ԭ��������Ŀ
/// @param[in]
///     x0        - �ز����Ĳ�������Ŀ�ʼxֵ
/// @param[in]
///     dx        - �ز����Ĳ�������Ĳ������
/// @param[in]
///     pResult_Y - �����ز����Ĳ���������ز����㣩�Ļ���
///
/// @return
///     �ز����Ĳ���������ز����㣩������
///
int  Interp_Thrice(double * pd_X, double * pd_Y, int dataNum, double x0, double dx, float * pd_resultY ,int resultYSize)
{
	double pd_Coe[4];              ///< ��¼���ζ���ʽ��ϵ��
	int i = 2;                     ///< ��¼x�Ǳ꣨x0,x1,...��
	int k = 0;                     ///< �ز�������������ֵy��λ��
	double x = x0;                 ///< �ز������xֵ
	double h = pd_X[3] - pd_X[2];  ///< ��¼�ڵ㲽��

	ThriceCoe(pd_X, pd_Y,pd_Coe,i);

	// x <= x2+h/2�����
	while (x <= pd_X[i] + h/2)
	{
		// ��������ز��������������򷵻�
		if ( k >= resultYSize )
			break;

		pd_resultY[k] = (float)(pd_Coe[0]*x*x*x + pd_Coe[1]*x*x + pd_Coe[2]*x + pd_Coe[3]);
		x += dx;
		k++;
	}
	// xj - h/2 < x <= xj + h/2�����
	while (x <= pd_X[dataNum-4] + (pd_X[dataNum-3] - pd_X[dataNum-4])/2 )
	{
		// ��������ز��������������򷵻�
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
	// x > x(n-1) - h/2�����
	while (x <= pd_X[dataNum-1])
	{
		// ��������ز��������������򷵻�
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
// ����������ֵ��
// =================================================================

///
/// @brief
///     ����������ֵ������ϵ����M�ļ���
///
/// @param[in]
///     pd_X   - ����ԭ������xֵ�Ļ���
/// @param[in]
///     pd_Y   - ����ԭ������yֵ�Ļ���
/// @param[in]
///     pd_Coe - ����ĳһ�ֶεĲ�ֵ������ϵ���Ļ���
/// @param[in]
///     i      - ��ǰ�ֶε�������
///
void SplineCoe(double * pd_X, double * pd_Y, int N, double * pd_h, double * pd_M)
{
	int n = N -1;
	double * pd_Alpha   = new double[n];    ///< ϵ������Alpha
	double * pd_Beta    = new double[n];    ///< ϵ������Beta
	double * pd_Gama    = new double[n];    ///< ϵ������Gama
	double * pd_MatrixA = new double[n*n];
	int i,j;

	// ����Alpha,Beta,Gama
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

	// ʹ�ø�˹��Ԫ��������ʽϵ��
	GaussianElimination(pd_MatrixA,pd_Beta,&(pd_M[1]),N-1);
	pd_M[0] = pd_M[N-1];

	delete[] pd_Alpha;
	delete[] pd_Beta;
	delete[] pd_Gama;
	delete[] pd_MatrixA;
}

///
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
int  Interp_Spline(double * pd_X, double * pd_Y, int dataNum, double x0, double dx, float * pd_resultY ,int resultYSize)
{
	int n  = dataNum - 1;
	double * pd_h = new double[n];        ///< �ڵ㲽������h
	double * pd_M = new double[dataNum];  ///< ϵ������M
	int i;                                ///< ��¼x�Ǳ꣨x0,x1,...��

	//����ڵ㲽������h
	for(i = 0; i < n; i++)
	{
		pd_h[i] = pd_X[i + 1] - pd_X[i];
	}

	SplineCoe(pd_X,pd_Y,dataNum,pd_h,pd_M);	

	i = 1;
	int k = 0;                            ///< �ز�������������ֵy��λ��
	double x = x0;                        ///< �ز������xֵ

	pd_resultY[k] = (float)pd_Y[0];
	x += dx;
	k++;

	while (x <= pd_X[dataNum-1])
	{
		// ��������ز��������������򷵻�
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
