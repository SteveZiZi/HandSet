#ifndef _SCL_ANALYSIS_H_
#define _SCL_ANALYSIS_H_

// �ڵ�ǰ��ʹ��SCL�ļ�������Ķ�̬���Ӱ汾ʱ�����ڸ�ͷ�ļ�֮ǰ���_SCLANALYSIS_STATICLINK��,��
// #define _SCLANALYSIS_STATICLINK
// #include "sclanalysis.h"
// ������Ԥ�����������

#if defined(_LIB) || defined(_SCLANALYSIS_STATICLINK)
#define SCLANALYSISDLL_API

#else
#ifdef SCLANALYSIS_EXPORTS
#define SCLANALYSISDLL_API __declspec(dllexport)
#else
#define SCLANALYSISDLL_API __declspec(dllimport)
#endif // SCLANALYSIS_EXPORTS

#endif // defined(_Lib) || defined(_SCLANALYSIS_STATICLINK)

#endif // _SCL_ANALYSIS_H_
