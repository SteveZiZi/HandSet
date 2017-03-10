/// @file
///     phaseSeq.cpp
/// @brief
///     序量计算
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/22
/// @revise
/// 
/*******************************************************************************
序量计算说明：
    对于任意一组不对称的三相电流（或电压），都可以按一定的方法把它们分解成正序、
负序和零序三相对称的三相电流（或电压），后者称为前者的对称分量。每一组对称分量都
符合：大小相等，彼此之间的相位差相等。正序分量的三相电流大小相等，相位彼此相差
120度，达到最大值的先后次序是A－B－C－A；负序分量的三相电流也是大小相等，相位彼
此相差120度，但达到最大值的先后次序是A－C－B－A；零序分量三相电流大小相等，相位
相同。反过来，任意三组正序、负序和零序对称电流（或电压）叠加起来，得到一组不对称
的三相电流（或电压）。
    本算法采用对称分量法来求解，办法是将三相不对称的电气量Fa、Fb和Fc分别用3组对
称分量Fa1、Fa2、Fa0、Fb1、Fb2、Fb0和Fc1、Fc2、Fc0来表示,而F1(Fa1 、 Fb1 、 Fc1)，
F2(Fa2 、 Fb2 、 Fc2)和 F0( Fa0 、 Fb0 、 Fc0)分别称为正序、负序和零序分量，其正
相序的相序（顺时方向）依次为Fa1、Fb1、Fc1，大小相等，互隔120度；负相序的相序（逆
时方向）依次为Fa2、Fb2、Fc2，大小相等，互隔120度；零相序大小相等且同相，各相序都
是按逆时针方向旋转。
    然后根据公式计算出零序，正序和负序的值：
    F0=1/3(Fa+Fb+Fc)
    F1=1/3(Fa+alpha*Fb+alpha^2*Fc)
    F2=1/3(Fa+alpha^2*Fb+alpha*Fc)
    其中，alpha=-1/2+sqrt(3)/2j，alpha^2=-1/2-sqrt(3)/2j
*******************************************************************************/
#include "phaseSeq.h"
///
/// @brief
///    序量计算
/// @param[in]
///    pHarmContents        三相基波有效值
///    pHarmAngles          三相基波相位
/// @param[out]
///    pPhaseSeqContents    [0]零序，[1]正序，[2]负序有效值
///    pPhaseSeqAngles      [0]零序，[1]正序，[2]负序相位角
///    pUnbalanceRate       三相不平衡度
/// @return
///    void
///
void phaseSeqCalc(const float pHarmContents[3], const float pHarmAngles[3], 
                  float pPhaseSeqContents[3], float pPhaseSeqAngles[3],
                  float *pUnbalanceRate)
{
	float fTmp,fTmp1,fTmp2;
    float fScale1 = 8.6602539e-1;       // sqrt(3)/2
	float fRe[3],fIm[3];

	fRe[0] = mpysp_i(pHarmContents[0],cosf(mpysp_i(pHarmAngles[0],ANGLE2ARC_FACTOR)));
	fIm[0] = mpysp_i(pHarmContents[0],sinf(mpysp_i(pHarmAngles[0],ANGLE2ARC_FACTOR)));
	fRe[1] = mpysp_i(pHarmContents[1],cosf(mpysp_i(pHarmAngles[1],ANGLE2ARC_FACTOR)));
	fIm[1] = mpysp_i(pHarmContents[1],sinf(mpysp_i(pHarmAngles[1],ANGLE2ARC_FACTOR)));
	fRe[2] = mpysp_i(pHarmContents[2],cosf(mpysp_i(pHarmAngles[2],ANGLE2ARC_FACTOR)));
	fIm[2] = mpysp_i(pHarmContents[2],sinf(mpysp_i(pHarmAngles[2],ANGLE2ARC_FACTOR)));

	// zero sequence Voltage 零序
    // fRe[0]+fRe[1]+fRe[2]
	fTmp1 = addsp_i(fRe[0],addsp_i(fRe[1],fRe[2]));
    // fIm[0]+fIm[1]+fIm[2]
	fTmp2 = addsp_i(fIm[0],addsp_i(fIm[1],fIm[2]));
    if(fTmp2 < 0.00001 && fTmp2 > -0.00001)
    {
        pPhaseSeqAngles[0] = 0;
    }
    else if(fTmp1)
    {
	    pPhaseSeqAngles[0] = mpysp_i(atan(divsp_i(fTmp2,fTmp1)),ARC2ANGLE_FACTOR);
        if((fTmp1<0)&&(fTmp2>0))
        {
            pPhaseSeqAngles[0]= addsp_i(pPhaseSeqAngles[0],180);
        }
        else if((fTmp1<0)&&(fTmp2<0))
        {
            pPhaseSeqAngles[0]= subsp_i(pPhaseSeqAngles[0],180);
        }
    }
    else
    {
        if(fTmp2>0)pPhaseSeqAngles[0] = 90;
        else pPhaseSeqAngles[0] = -90;
    }
    // 相位角调整
    //fAngle += PI0_5;
    while(pPhaseSeqAngles[0]>360)
        pPhaseSeqAngles[0] = subsp_i(pPhaseSeqAngles[0],360);
    while(pPhaseSeqAngles[0]<0.0)
        pPhaseSeqAngles[0] = addsp_i(pPhaseSeqAngles[0],360);

    fTmp1 = mpysp_i(fTmp1,fTmp1);
	fTmp2 = mpysp_i(fTmp2,fTmp2);
	fTmp  = sqrtsp_i(addsp_i(fTmp1,fTmp2));
	fTmp  = mpysp_i(fTmp,3.3333334e-1);     // 平方和开方后乘以1/3
	pPhaseSeqContents[0] = fTmp;
    

	// positave sequence 正序
    // fRe[0]-(fRe[1]+fRe[2])*(1/2)-(fIm[1]-fIm[2])*(sqrt(3)/2)
    fTmp1 = subsp_i(subsp_i(fRe[0],mpysp_i(addsp_i(fRe[1],fRe[2]),0.5)),
        mpysp_i(subsp_i(fIm[1],fIm[2]),fScale1));
    // fIm[0]-(fIm[1]+fIm[2])*(1/2)+(fRe[1]-fRe[2])*(sqrt(3)/2)
	fTmp2 = addsp_i(subsp_i(fIm[0],mpysp_i(addsp_i(fIm[1],fIm[2]),0.5)),
        mpysp_i(subsp_i(fRe[1],fRe[2]),fScale1));
    if(fTmp2 < 0.00001 && fTmp2 > -0.00001)
    {
        pPhaseSeqAngles[1] = 0;
    }
    else if(fTmp1)
    {
        pPhaseSeqAngles[1] = mpysp_i(atan(divsp_i(fTmp2,fTmp1)),ARC2ANGLE_FACTOR);
        if((fTmp1<0)&&(fTmp2>0))
        {
            pPhaseSeqAngles[1]= addsp_i(pPhaseSeqAngles[1],180);
        }
        else if((fTmp1<0)&&(fTmp2<0))
        {
            pPhaseSeqAngles[1]= subsp_i(pPhaseSeqAngles[1],180);
        }
    }
    else
    {
        if(fTmp2>0)pPhaseSeqAngles[1] = 90;
        else pPhaseSeqAngles[1] = -90;
    }
    // 相位角调整
    //fAngle += PI0_5;
    while(pPhaseSeqAngles[1]>360)
        pPhaseSeqAngles[1] = subsp_i(pPhaseSeqAngles[1],360);
    while(pPhaseSeqAngles[1]<0.0)
        pPhaseSeqAngles[1] = addsp_i(pPhaseSeqAngles[1],360);

    fTmp1 = mpysp_i(fTmp1,fTmp1);
	fTmp2 = mpysp_i(fTmp2,fTmp2);
	fTmp  = sqrtsp_i(addsp_i(fTmp1,fTmp2));
	fTmp  = mpysp_i(fTmp,3.3333334e-1);     // 平方和开方后乘以1/3
	pPhaseSeqContents[1] = fTmp;
    
	// negative sequence 负序
    // fRe[0]-(fRe[1]+fRe[2])*(1/2)+(fIm[1]-fIm[2])*(sqrt(3)/2)
    fTmp1 = addsp_i(subsp_i(fRe[0],mpysp_i(addsp_i(fRe[1],fRe[2]),0.5)),
        mpysp_i(subsp_i(fIm[1],fIm[2]),fScale1));
    // fIm[0]-(fIm[1]+fIm[2])*(1/2)-(fRe[1]-fRe[2])*(sqrt(3)/2)
	fTmp2 = subsp_i(subsp_i(fIm[0],mpysp_i(addsp_i(fIm[1],fIm[2]),0.5)),
        mpysp_i(subsp_i(fRe[1],fRe[2]),fScale1));
    if(fTmp2 < 0.00001 && fTmp2 > -0.00001)
    {
        pPhaseSeqAngles[2] = 0;
    }
    else if(fTmp1)
    {
        pPhaseSeqAngles[2] = mpysp_i(atan(divsp_i(fTmp2,fTmp1)),ARC2ANGLE_FACTOR);
        if((fTmp1<0)&&(fTmp2>0))
        {
            pPhaseSeqAngles[2]= addsp_i(pPhaseSeqAngles[2],180);
        }
        else if((fTmp1<0)&&(fTmp2<0))
        {
            pPhaseSeqAngles[2]= subsp_i(pPhaseSeqAngles[2],180);
        }
    }
    else
    {
        if(fTmp2>0)pPhaseSeqAngles[2] = 90;
        else pPhaseSeqAngles[2] = -90;
    }
    // 相位角调整
    //fAngle += PI0_5;
    while(pPhaseSeqAngles[2]>360)
        pPhaseSeqAngles[2] = subsp_i(pPhaseSeqAngles[2],360);
    while(pPhaseSeqAngles[2]<0.0)
        pPhaseSeqAngles[2] = addsp_i(pPhaseSeqAngles[2],360);

	fTmp1 = mpysp_i(fTmp1,fTmp1);
	fTmp2 = mpysp_i(fTmp2,fTmp2);
	fTmp  = sqrtsp_i(addsp_i(fTmp1,fTmp2));
	fTmp  = mpysp_i(fTmp,3.3333334e-1);     // 平方和开方后乘以1/3
	pPhaseSeqContents[2] = fTmp;

    if(pPhaseSeqContents[1]<0.00001 && pPhaseSeqContents[1]>-0.00001)
    {
        *pUnbalanceRate = 0;
    }
    else
    {
        *pUnbalanceRate = mpysp_i(divsp_i(pPhaseSeqContents[2], pPhaseSeqContents[1]), 
                                       100);
    }     
}
