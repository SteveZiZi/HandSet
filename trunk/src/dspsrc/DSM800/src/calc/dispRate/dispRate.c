/// @file
///     dispRate.cpp
/// @brief
///     报文离散度计算
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/21
/// @revise
/// 
/*******************************************************************************
报文离散度计算：
    报文离散度是统计一定时间内所有报文传输的时间与默认时间的差值的概率，以反映报
文的偏差情况。其中分为11个级别：
///                     dispRates[0]:<=-250*ASDU个数
///                     dispRates[1]:-250*ASDU个数+1 ~ -50*ASDU个数
///                     dispRates[2]:-50*ASDU个数+1 ~ -10
///                     dispRates[3]:-9~-2
///                     dispRates[4]:-1
///                     dispRates[5]:0
///                     dispRates[6]:1
///                     dispRates[7]:2~9
///                     dispRates[8]:10 ~ 50*ASDU个数-1
///                     dispRates[9]:50*ASDU个数 ~ 250*ASDU个数-1
///                     dispRates[10]:>=250*ASDU个数
    本算法采用一个数组存放这11个级别的概率值，首先统计落在各个范围里的报文个数，
然后计算每个级别的报文离散度。
    在将报文个数映射到某个范围里时，采用了两种hash算法，第一种采用
floor(log(time)*0.61+1.62)来映射-250~250内的除0以外的值，算法效率高，但须保证精
度。第二种采用判断来映射，算法效率低，复杂度高。第三种采用一个499大小的固定数组
来进行映射，效率高，但须占用一定的空间。在精度能够保证的情况下，默认采用第一种。
*******************************************************************************/
#include "dispRate.h"
#include <string.h>

///< 定义最大的时间差与离散度区间hash数组的大小
#define MAX_HASH_COUNT (500*MAX_ASDU_COUNT)

//static uint8_t dispHash1(int32_t time);
static uint8_t dispHash(int32_t timeDif, uint8_t nASDUCount);

static uint32_t dispArr[11] = {0};			///< 各区间报文个数
static uint32_t dispSize = 0;				///< 报文总数
///
/// @brief
///    报文离散度计算
/// @param[in] 
///    dataTimes        报文时间差数据
/// @param[in]
///    dataSize         报文时间数据个数
///    nSamplingRate    采样率
///    nASDUCount       ASDU的个数
/// @param[in]
///    defaultTime      额定时间
/// @param[out]
///    dispRates        报文离散度数组，输出报文离散度百分比，数组大小必须大于等于11
///                     dispRates[0]:<=-250*ASDU个数
///                     dispRates[1]:-250*ASDU个数+1 ~ -50*ASDU个数
///                     dispRates[2]:-50*ASDU个数+1 ~ -10
///                     dispRates[3]:-9~-2
///                     dispRates[4]:-1
///                     dispRates[5]:0
///                     dispRates[6]:1
///                     dispRates[7]:2-9
///                     dispRates[8]:10 ~ 50*ASDU个数-1
///                     dispRates[9]:50*ASDU个数 ~ 250*ASDU个数-1
///                     dispRates[10]:>=250*ASDU个数
/// @return
///    void
///
void dispRateCalc(int32_t dataTimeDifs[], uint16_t dataSize, uint16_t nSamplingRate, 
                  uint8_t nASDUCount, float dispRates[])
{
    uint16_t i=0;
	int32_t difTime;
	int8_t hash;
    int32_t defaultTimeDif = nASDUCount* 1000000 / nSamplingRate;
    if(dataSize == 0 || nASDUCount > 8)
    {
        return;
    }
    for(i=0; i<dataSize; i++)
    {
        // 计算时间差
        if(dataTimeDifs[i] > defaultTimeDif && dataTimeDifs[i]-defaultTimeDif > 2147483647)
		{
			difTime = 2147483647;
		}
		else if(dataTimeDifs[i] < defaultTimeDif && defaultTimeDif - dataTimeDifs[i] > 2147483648)
		{
			difTime = -2147483647;
		}
		else
		{
			difTime = dataTimeDifs[i] - defaultTimeDif;
		}
		
        // 计算时间差级别的hash下标
        hash=dispHash(difTime, nASDUCount);

        // 相应级别次数+1
        dispArr[hash]++;
    }

	dispSize = dispSize+dataSize;
    // 计算概率百分比
    for(i=0; i<11; i++)
    {
        if(dispArr[i] == 0)
        {
            dispRates[i] = 0.0f;
        }
        else
        {
            dispRates[i] = mpysp_i(divsp_i((float)dispArr[i],(float)dispSize),100);
        }        
    }
}

// 重置报文统计数据
void resetDisp()
{
    int i = 0;
	for(i=0; i<11; i++)
    {
        dispArr[i] = 0;
    }
	dispSize = 0;
}
#if 0
///
/// @brief
///    选择式的报文离散度Hash算法，分支比较多
/// @param[in] 
///    timeDif 报文时间差
/// @return
///    int32_t  计算得到的hash值
///
static uint8_t dispHash1(int32_t timeDif)
{
    if(time==0)
    {
        return 5;
    }
    else if(time>0)
    {
        if(time<2)
        {
            return 6;
        }
        else if(time<10)
        {
            return 7;
        }
        else if(time<50)
        {
            return 8;
        }
        else if(time<250)
        {
            return 9;
        }
        else
        {
            return 10;
        }
    }
    else
    {
        if(time>-2)
        {
            return 4;
        }
        else if(time>-10)
        {
            return 3;
        }
        else if(time>-50)
        {
            return 2;
        }
        else if(time>-250)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}
#endif
///
/// @brief
///    固定数组式的报文离散度Hash算法，需要一定的空间
/// @param[in] 
///    timeDif 报文时间差
///    
/// @return
///    int32_t
///

///< hash数组，用于通过报文时间差映射报文时间差区间，初始化为1个ASDU
static uint8_t timeMsgHash[MAX_HASH_COUNT]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,4,5,6,7,7,7,7,7,7,7,7,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9};

///
/// @brief
///     生成报文时间差与报文区间的映射hash数组
/// @param[in] 
///    nASDUCount           ASDU的个数（目前最多支持8个）
///    
/// @return
///    int                  0：成功
///                         -1：ASDU个数超过限制
///
int dispHashCreate(uint8_t nASDUCount)
{
    if(nASDUCount > 8 || nASDUCount == 0)
    {
        return -1;
    }
    memset(&timeMsgHash[0], 1, 200*nASDUCount);
    memset(&timeMsgHash[200*nASDUCount], 2, 50*nASDUCount-10);
    memset(&timeMsgHash[250*nASDUCount-10], 3, 8);
    memset(&timeMsgHash[250*nASDUCount-2], 4, 1);
    memset(&timeMsgHash[250*nASDUCount-1], 5, 1);
    memset(&timeMsgHash[250*nASDUCount], 6, 1);
    memset(&timeMsgHash[250*nASDUCount+1], 7, 8);
    memset(&timeMsgHash[250*nASDUCount+9], 8, 50*nASDUCount-10);
    memset(&timeMsgHash[300*nASDUCount-1], 9, 200*nASDUCount);
    return 0;
}

uint8_t dispHash(int32_t timeDif, uint8_t nASDUCount)
{
    int32_t nMaxTime = 250*nASDUCount;
    if(timeDif >= nMaxTime)
    {
        return 10;
    }
    else if(timeDif <= -nMaxTime)
    {
        return 0;
    }
    else
    {
        return timeMsgHash[timeDif + nMaxTime - 1];
    }
}
