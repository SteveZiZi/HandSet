/// @file
///     phaseCheck.cpp
/// @brief
///     �������
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/21
/// @revise
/// 
/*******************************************************************************
���ࣺ
    ��ָ�ڵ���ϵͳ�������������Ǳ�������ֶκ˶�����Դ��·��λ�������Ƿ���ͬ��
Ҳ������ʵ�ʵ����������У�����λ��Ĳ�����

1) ΪʲôҪ����
    �½����Ľ���������ı�����������·���Լ�����·������ϡ����û��͵�ǰ������
����������·�������飬��ȷ�������·�������û����ฺ�������������һ�¡�
    ��������Զ�·��Դ���Եġ���·��Դ��Ҫ��ͬһ���õ��豸����ʱ����Ͷ��ʱ��Ҫ��
���е���к��ࡣ
    ����·��Դ��Ҫ���е���ʱ���������࣬���ڰ�װ���ߴ��󣬿��ܳ���������λ����
һ�£������·�¹ʣ�Ӱ���������硣
    ����·��Դ��Ҫͣ�絹��ʱ���������࣬������������һ�£����������豸�ķ�����
���У������ķ�ת��
    ��ˣ��ڵڶ�·��ԴͶ��ʱ��һ��Ҫ���һ·��Դ���к��ࡣ

2) ��ʲô����º��ࣺ
һ����˵�����������������ϵĵ�Դ�����������֮һʱ��Ҫ���ࣺ
1��	�в���Ҫ��ʱ�����豸��װ��Ͷ������ǰӦ�ú���
2��	��Ϊ������Դʱ�����豸��װ��Ͷ������ǰӦ�ú��ࡣ
3��	���������豸�������ޣ��п��ܸı�һ������ʱ���ڴ��޺�Ͷ������ǰӦ���º��ࡣ

3) ��������
phaseAngle1ָ��1��������1�Ļ�����λ��
phaseAngle2ָ��2��������2�Ļ�����λ��

*******************************************************************************/
#include "phaseCheck.h"

///
/// @brief
///    �����㷨��λ��
/// @param[in] 
///    phaseAngle1   ��1��λ��
///    phaseAngle2   ��2��λ��
/// @return
///    float    ��λ��
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
///    �����㷨������
/// @param[in] 
///    value1           ����1��С(����Ϊ�Ǹ�ֵ)
///    phaseAngle1      ����1��λ��
///    value2           ����2��С(����Ϊ�Ǹ�ֵ)
///    phaseAngle2      ����2��λ��
/// @param[out]
///    pDifValue        �������С
///    pDifPhaseAngle   ������Ƕ�
/// @note
///     ������С����Ϊ�Ǹ�ֵ��������dValue��dPhaseAngleΪ0;
/// @return
///    void
///
void phaseCheckVectorCalc(float value1, float phaseAngle1,
                             float value2, float phaseAngle2,
                             float *pDifValue, float *pDifPhaseAngle)
{
    float valueX, valueY;
    float phaseArc1, phaseArc2;
    
    // ������С����Ϊ�Ǹ�ֵ
    if(value1<0 || value2<0)
    {
        *pDifValue=0;
        *pDifPhaseAngle=0;
        return;
    }

    // ���Ƕ�תΪ����
    phaseArc1 = mpysp_i(phaseAngle1,ANGLE2ARC_FACTOR);
    phaseArc2 = mpysp_i(phaseAngle2,ANGLE2ARC_FACTOR);
    
    // ��X���Y����������
    valueX= subsp_i(mpysp_i(value1,cosf(phaseArc1)),
        mpysp_i(value2,cosf(phaseArc2)));
    valueY= subsp_i(mpysp_i(value1,sinf(phaseArc1)),
        mpysp_i(value2,sinf(phaseArc2)));

    // ��������
    *pDifValue=sqrt(addsp_i(mpysp_i(valueX,valueX),mpysp_i(valueY,valueY)));

    // ���������СΪ0��֤�����������ڣ�����������Ϊ0
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
