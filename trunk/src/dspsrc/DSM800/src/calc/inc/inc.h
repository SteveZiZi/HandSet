#ifndef INC_H
#define INC_H

#ifdef _WIN32
#include <stdio.h>
//#include <tchar.h>
#include <math.h>

#define mpysp_i(a,b) ((a)*(b))
#define divsp_i(a,b) ((a)/(b))
#define subsp_i(a,b) ((a)-(b))
#define addsp_i(a,b) ((a)+(b))
#define sqrtsp_i(a) sqrtf(a)
#define intsp_i(a)  (int)(a)
#define uintsp_i(a) (unsigned int)(a)     

typedef signed char         Int8;
typedef signed short        Int16;
typedef signed int          Int32;
typedef signed int          Int;
typedef unsigned char       Uint8;
typedef unsigned short      Uint16;
typedef unsigned int        Uint32;
typedef unsigned int        Uint;
#endif

#ifdef _DSP
#include    <std.h>
#include    <math.h>
#include    <mathf.h>
//#include    "fastrts_i.h"
#define mpysp_i(a,b) ((a)*(b))
#define divsp_i(a,b) ((a)/(b))
#define subsp_i(a,b) ((a)-(b))
#define addsp_i(a,b) ((a)+(b))
#define sqrtsp_i(a) sqrtf(a)
#define intsp_i(a)  (int)(a)
#define uintsp_i(a) (unsigned int)(a)
#endif

#define PI                       3.14159265358979323846
#define PI0_5                    1.57079632679489661923
#define PI2                      6.28318530717958647693

#define ANGLE2ARC_FACTOR         1.74532925199432957692e-2         ///< 角度转弧度因子
#define ARC2ANGLE_FACTOR         5.72957795130823208768e1          ///< 弧度转角度因子

#endif
