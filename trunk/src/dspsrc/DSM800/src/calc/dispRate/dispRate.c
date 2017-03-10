/// @file
///     dispRate.cpp
/// @brief
///     ������ɢ�ȼ���
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/21
/// @revise
/// 
/*******************************************************************************
������ɢ�ȼ��㣺
    ������ɢ����ͳ��һ��ʱ�������б��Ĵ����ʱ����Ĭ��ʱ��Ĳ�ֵ�ĸ��ʣ��Է�ӳ��
�ĵ�ƫ����������з�Ϊ11������
///                     dispRates[0]:<=-250*ASDU����
///                     dispRates[1]:-250*ASDU����+1 ~ -50*ASDU����
///                     dispRates[2]:-50*ASDU����+1 ~ -10
///                     dispRates[3]:-9~-2
///                     dispRates[4]:-1
///                     dispRates[5]:0
///                     dispRates[6]:1
///                     dispRates[7]:2~9
///                     dispRates[8]:10 ~ 50*ASDU����-1
///                     dispRates[9]:50*ASDU���� ~ 250*ASDU����-1
///                     dispRates[10]:>=250*ASDU����
    ���㷨����һ����������11������ĸ���ֵ������ͳ�����ڸ�����Χ��ı��ĸ�����
Ȼ�����ÿ������ı�����ɢ�ȡ�
    �ڽ����ĸ���ӳ�䵽ĳ����Χ��ʱ������������hash�㷨����һ�ֲ���
floor(log(time)*0.61+1.62)��ӳ��-250~250�ڵĳ�0�����ֵ���㷨Ч�ʸߣ����뱣֤��
�ȡ��ڶ��ֲ����ж���ӳ�䣬�㷨Ч�ʵͣ����Ӷȸߡ������ֲ���һ��499��С�Ĺ̶�����
������ӳ�䣬Ч�ʸߣ�����ռ��һ���Ŀռ䡣�ھ����ܹ���֤������£�Ĭ�ϲ��õ�һ�֡�
*******************************************************************************/
#include "dispRate.h"
#include <string.h>

///< ��������ʱ�������ɢ������hash����Ĵ�С
#define MAX_HASH_COUNT (500*MAX_ASDU_COUNT)

//static uint8_t dispHash1(int32_t time);
static uint8_t dispHash(int32_t timeDif, uint8_t nASDUCount);

static uint32_t dispArr[11] = {0};			///< �����䱨�ĸ���
static uint32_t dispSize = 0;				///< ��������
///
/// @brief
///    ������ɢ�ȼ���
/// @param[in] 
///    dataTimes        ����ʱ�������
/// @param[in]
///    dataSize         ����ʱ�����ݸ���
///    nSamplingRate    ������
///    nASDUCount       ASDU�ĸ���
/// @param[in]
///    defaultTime      �ʱ��
/// @param[out]
///    dispRates        ������ɢ�����飬���������ɢ�Ȱٷֱȣ������С������ڵ���11
///                     dispRates[0]:<=-250*ASDU����
///                     dispRates[1]:-250*ASDU����+1 ~ -50*ASDU����
///                     dispRates[2]:-50*ASDU����+1 ~ -10
///                     dispRates[3]:-9~-2
///                     dispRates[4]:-1
///                     dispRates[5]:0
///                     dispRates[6]:1
///                     dispRates[7]:2-9
///                     dispRates[8]:10 ~ 50*ASDU����-1
///                     dispRates[9]:50*ASDU���� ~ 250*ASDU����-1
///                     dispRates[10]:>=250*ASDU����
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
        // ����ʱ���
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
		
        // ����ʱ�����hash�±�
        hash=dispHash(difTime, nASDUCount);

        // ��Ӧ�������+1
        dispArr[hash]++;
    }

	dispSize = dispSize+dataSize;
    // ������ʰٷֱ�
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

// ���ñ���ͳ������
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
///    ѡ��ʽ�ı�����ɢ��Hash�㷨����֧�Ƚ϶�
/// @param[in] 
///    timeDif ����ʱ���
/// @return
///    int32_t  ����õ���hashֵ
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
///    �̶�����ʽ�ı�����ɢ��Hash�㷨����Ҫһ���Ŀռ�
/// @param[in] 
///    timeDif ����ʱ���
///    
/// @return
///    int32_t
///

///< hash���飬����ͨ������ʱ���ӳ�䱨��ʱ������䣬��ʼ��Ϊ1��ASDU
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
///     ���ɱ���ʱ����뱨�������ӳ��hash����
/// @param[in] 
///    nASDUCount           ASDU�ĸ�����Ŀǰ���֧��8����
///    
/// @return
///    int                  0���ɹ�
///                         -1��ASDU������������
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
