/// @file
///     phaseCheck.cpp
/// @brief
///     核相计算
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/21
/// @revise
/// 
/*******************************************************************************
核相：
    是指在电力系统电气操作中用仪表或其他手段核对两电源或环路相位、相序是否相同。
也就是在实际电力的运行中，对相位差的测量。

1) 为什么要核相
    新建、改建、扩建后的变电所和输电线路，以及在线路检修完毕、向用户送电前，都必
须进行三相电路核相试验，以确保输电线路相序与用户三相负载所需求的相序一致。
    核相是针对二路电源而言的。二路电源需要向同一个用电设备供电时，在投入时，要在
并列点进行核相。
    若二路电源需要并列倒电时，若不核相，由于安装接线错误，可能出现相序（相位）不
一致，引起短路事故，影响正常供电。
    若二路电源需要停电倒电时，若不核相，可能由于相序不一致，引起三相设备的非正常
运行，如电机的反转。
    因此，在第二路电源投入时，一定要与第一路电源进行核相。

2) 在什么情况下核相：
一般来说，当两个或两个以上的电源，有下列情况之一时需要核相：
1、	有并列要求时。在设备安装后，投入运行前应该核相
2、	作为互备电源时。在设备安装后，投入运行前应该核相。
3、	以上两项设备经过大修，有可能改变一次相序时，在大修后，投入运行前应重新核相。

3) 参数解释
phaseAngle1指相1，代表相1的基波相位角
phaseAngle2指相2，代表相2的基波相位角

*******************************************************************************/
#include "phaseCheck.h"

///
/// @brief
///    核相算法相位差
/// @param[in] 
///    phaseAngle1   相1相位角
///    phaseAngle2   相2相位角
/// @return
///    float    相位差
///
float phaseCheckPhaseCalc(float phaseAngle1, float phaseAngle2)
{
	float phaseAngleDif = phaseAngle1-phaseAngle2;
	while(phaseAngleDif >= 360)
	{
        phaseAngleDif-=360;
	}
	while(phaseAngleDif <= -360)
	{
        phaseAngleDif+=360;
	}	
    return phaseAngleDif;
}
///
/// @brief
///    核相算法向量差
/// @param[in] 
///    value1           向量1大小(必须为非负值)
///    phaseAngle1      向量1相位角
///    value2           向量2大小(必须为非负值)
///    phaseAngle2      向量2相位角
/// @param[out]
///    pDifValue        向量差大小
///    pDifPhaseAngle   向量差角度
/// @note
///     向量大小必须为非负值，否则置dValue，dPhaseAngle为0;
/// @return
///    void
///
void phaseCheckVectorCalc(float value1, float phaseAngle1,
                             float value2, float phaseAngle2,
                             float *pDifValue, float *pDifPhaseAngle)
{
    float valueX, valueY;
    float phaseArc1, phaseArc2;
    
    // 向量大小必须为非负值
    if(value1<0 || value2<0)
    {
        *pDifValue=0;
        *pDifPhaseAngle=0;
        return;
    }

    // 将角度转为弧度
    phaseArc1 = mpysp_i(phaseAngle1,ANGLE2ARC_FACTOR);
    phaseArc2 = mpysp_i(phaseAngle2,ANGLE2ARC_FACTOR);
    
    // 求X轴和Y轴向量分量
    valueX= subsp_i(mpysp_i(value1,cosf(phaseArc1)),
        mpysp_i(value2,cosf(phaseArc2)));
    valueY= subsp_i(mpysp_i(value1,sinf(phaseArc1)),
        mpysp_i(value2,sinf(phaseArc2)));

    // 求向量差
    *pDifValue=sqrt(addsp_i(mpysp_i(valueX,valueX),mpysp_i(valueY,valueY)));

    // 如果向量大小为0，证明向量不存在，将向量角置为0
    if(valueX >= 0.00001 || valueX<=-0.00001)
    {
        *pDifPhaseAngle=mpysp_i(atan(divsp_i(valueY,valueX)),ARC2ANGLE_FACTOR);
    }
    else
    {
        if(valueY>0.00001)
        {
            *pDifPhaseAngle=90;
        }
        else if(valueY<-0.00001)
        {
            *pDifPhaseAngle=-90;
        }
        else
        {
            *pDifPhaseAngle=0;
        }
    }
}
