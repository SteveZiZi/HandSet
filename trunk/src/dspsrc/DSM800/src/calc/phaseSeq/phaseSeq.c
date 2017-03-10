/// @file
///     phaseSeq.cpp
/// @brief
///     ��������
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/22
/// @revise
/// 
/*******************************************************************************
��������˵����
    ��������һ�鲻�ԳƵ�������������ѹ���������԰�һ���ķ��������Ƿֽ������
�������������ԳƵ�������������ѹ�������߳�Ϊǰ�ߵĶԳƷ�����ÿһ��ԳƷ�����
���ϣ���С��ȣ��˴�֮�����λ����ȡ�������������������С��ȣ���λ�˴����
120�ȣ��ﵽ���ֵ���Ⱥ������A��B��C��A������������������Ҳ�Ǵ�С��ȣ���λ��
�����120�ȣ����ﵽ���ֵ���Ⱥ������A��C��B��A������������������С��ȣ���λ
��ͬ���������������������򡢸��������ԳƵ��������ѹ�������������õ�һ�鲻�Գ�
��������������ѹ����
    ���㷨���öԳƷ���������⣬�취�ǽ����಻�ԳƵĵ�����Fa��Fb��Fc�ֱ���3���
�Ʒ���Fa1��Fa2��Fa0��Fb1��Fb2��Fb0��Fc1��Fc2��Fc0����ʾ,��F1(Fa1 �� Fb1 �� Fc1)��
F2(Fa2 �� Fb2 �� Fc2)�� F0( Fa0 �� Fb0 �� Fc0)�ֱ��Ϊ���򡢸�����������������
���������˳ʱ��������ΪFa1��Fb1��Fc1����С��ȣ�����120�ȣ��������������
ʱ��������ΪFa2��Fb2��Fc2����С��ȣ�����120�ȣ��������С�����ͬ�࣬������
�ǰ���ʱ�뷽����ת��
    Ȼ����ݹ�ʽ�������������͸����ֵ��
    F0=1/3(Fa+Fb+Fc)
    F1=1/3(Fa+alpha*Fb+alpha^2*Fc)
    F2=1/3(Fa+alpha^2*Fb+alpha*Fc)
    ���У�alpha=-1/2+sqrt(3)/2j��alpha^2=-1/2-sqrt(3)/2j
*******************************************************************************/
#include "phaseSeq.h"
///
/// @brief
///    ��������
/// @param[in]
///    pHarmContents        ���������Чֵ
///    pHarmAngles          ���������λ
/// @param[out]
///    pPhaseSeqContents    [0]����[1]����[2]������Чֵ
///    pPhaseSeqAngles      [0]����[1]����[2]������λ��
///    pUnbalanceRate       ���಻ƽ���
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

	// zero sequence Voltage ����
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
    // ��λ�ǵ���
    //fAngle += PI0_5;
    while(pPhaseSeqAngles[0]>360)
        pPhaseSeqAngles[0] = subsp_i(pPhaseSeqAngles[0],360);
    while(pPhaseSeqAngles[0]<0.0)
        pPhaseSeqAngles[0] = addsp_i(pPhaseSeqAngles[0],360);

    fTmp1 = mpysp_i(fTmp1,fTmp1);
	fTmp2 = mpysp_i(fTmp2,fTmp2);
	fTmp  = sqrtsp_i(addsp_i(fTmp1,fTmp2));
	fTmp  = mpysp_i(fTmp,3.3333334e-1);     // ƽ���Ϳ��������1/3
	pPhaseSeqContents[0] = fTmp;
    

	// positave sequence ����
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
    // ��λ�ǵ���
    //fAngle += PI0_5;
    while(pPhaseSeqAngles[1]>360)
        pPhaseSeqAngles[1] = subsp_i(pPhaseSeqAngles[1],360);
    while(pPhaseSeqAngles[1]<0.0)
        pPhaseSeqAngles[1] = addsp_i(pPhaseSeqAngles[1],360);

    fTmp1 = mpysp_i(fTmp1,fTmp1);
	fTmp2 = mpysp_i(fTmp2,fTmp2);
	fTmp  = sqrtsp_i(addsp_i(fTmp1,fTmp2));
	fTmp  = mpysp_i(fTmp,3.3333334e-1);     // ƽ���Ϳ��������1/3
	pPhaseSeqContents[1] = fTmp;
    
	// negative sequence ����
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
    // ��λ�ǵ���
    //fAngle += PI0_5;
    while(pPhaseSeqAngles[2]>360)
        pPhaseSeqAngles[2] = subsp_i(pPhaseSeqAngles[2],360);
    while(pPhaseSeqAngles[2]<0.0)
        pPhaseSeqAngles[2] = addsp_i(pPhaseSeqAngles[2],360);

	fTmp1 = mpysp_i(fTmp1,fTmp1);
	fTmp2 = mpysp_i(fTmp2,fTmp2);
	fTmp  = sqrtsp_i(addsp_i(fTmp1,fTmp2));
	fTmp  = mpysp_i(fTmp,3.3333334e-1);     // ƽ���Ϳ��������1/3
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
